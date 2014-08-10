#!/usr/bin/perl -w
# $Id$
####################################################################################################
# Generates a lookup table for fader calibration based on given points
# Thorsten Klose (2014-07-07)
####################################################################################################

# demo 1
#my @x = (0, 10, 20, 40, 100, 300, 700, 900, 950, 1000, 1023);
#my @y = (0, 20, 40, 70, 110, 250, 800, 850, 920, 990, 1023);

# demo 2 (extreme)
#my @x = (0, 500, 1023);
#my @y = (0, 900, 1023);

# demo 3 (linear)
my @x = (0, 1023);
my @y = (0, 1023);


####################################################################################################

if( scalar(@ARGV) != 1 ) {
  die "ERROR: please specify table name, e.g.:\n" .
      "$0 cal_motor\n";
}

my $tableName = $ARGV[0];

####################################################################################################
open(OUT_C_DAT, ">${tableName}.dat") || die;
open(OUT_C_REF, ">ref.dat") || die;
open(OUT_C_INC, ">${tableName}.inc") || die;

print OUT_C_INC ";; \$Id: \$\n";
print OUT_C_INC ";; Generated with '$0 $tableName'\n";
print OUT_C_INC ";;\n";
print OUT_C_INC ";; Interpolation parameters:\n";
print OUT_C_INC ";; my \@x = (" . join(", ", @x) . ");\n";
print OUT_C_INC ";; my \@y = (" . join(", ", @y) . ");\n";
print OUT_C_INC ";;\n";

for($x=0; $x<1024; ++$x)
{
  my $y = gen_table_item($x);
  printf OUT_C_DAT "%4d\t%4d\n", $x, $y;
  printf OUT_C_REF "%4d\t%4d\n", $x, $x;
  printf OUT_C_INC "  dw %4d ; (%3d -> %3d)\n", $y, $x, $y;
}

close(OUT_C_DAT);
close(OUT_C_REF);
close(OUT_C_INC);

####################################################################################################
# generate gnuplot
system("gnuplot --persist -e \"plot '${tableName}.dat' title 'Interpolated' with lines, 'ref.dat' title 'Ideal' with lines\"") && print "The table has been successfully generated, but can't be displayed because gnuplot isn't installed on your computer.\n";

print "Generated ${tableName}.inc\n";

# the end
exit;

####################################################################################################
####################################################################################################
####################################################################################################
sub gen_table_item
{
  my ($val) = @_;

  my $i;
  for($i=0; $i<scalar(@x)-1; ++$i) {
    if( $x[$i] <= $val && $x[$i+1] >= $val ) {
      my $diffx = $val - $x[$i];
      my $diffn = $x[$i+1] - $x[$i];

      return $y[$i] + (($y[$i+1] - $y[$i]) * $diffx) / $diffn;
    }
  }

  return 0;
}



