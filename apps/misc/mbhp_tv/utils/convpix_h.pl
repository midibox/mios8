#!/usr/bin/perl -w
#
# convpix_8x8 by Thorsten Klose (8/95)
# converts graphic to asm-data for graphic display
#

use Getopt::Long;

$debug = 0;
$icons_per_line = 0;
$height = 8;

$cmd_args = join(" ", @ARGV);

GetOptions (
   "debug" => \$debug,
   "icons_per_line=s" => \$icons_per_line,
   "height=s" => \$height,
   );

if( scalar(@ARGV) != 1 )
{
   die "SYNTAX:   convpix_8x8.pl <xpm-file> [<-debug>]\n" .
       "EXAMPLE:  convpix_8x8.pl font.xpm\n";
}

$input_file=$ARGV[0];

if( $height % 8 )
{
   die "ERROR: height must be multiple of 8!\n";
}

$output_file=sprintf("%s.inc",substr($input_file, 0, length($input_file)-4));

open(IN,"<$input_file");

my @graphic = ();
my $state = "READ_HEADER";
my $y = 0;
my $x_max;
my $y_max;

while( <IN> )
{
   $line=$_;
   chomp($line);

#   print "[${state}] ${line}\n";

   if( $state eq "READ_HEADER" )
   {   
      if( $line ne "/* XPM */" )
      {
         die "ERROR: Expecting .xpm Format!\n";
      }
      $state = "WAIT_RESOLUTION";
   }
   elsif( $state eq "WAIT_RESOLUTION" )
   {
      if( /^[\s]*\"(\d+) *(\d+) *(\d+) *(\d+)\",/ )
      {
         $x_max = $1;
         $y_max = $2;

         if( $y_max % 8 )
         {
            die "ERROR: Y-Size must be dividable by 8!\n";
         }

         if( $x_max % 8 )
         {
            die "ERROR: X-Size must be dividable by 8!\n";
         }

         print "Image Size: $x_max * $y_max\n";

         $state = "WAIT_COLOR_0";
      }
   }
   elsif( $state eq "WAIT_COLOR_0" )
   {
      if( /^[\s]*\".*\",/ )
      {
         $state = "WAIT_COLOR_1";
      }
   }
   elsif( $state eq "WAIT_COLOR_1" )
   {
      if( /^[\s]*\".*\",/ )
      {
         $state = "READ_PIXELS";
      }
   }   
   elsif( $state eq "READ_PIXELS" )
   {
      if( /^[\s]*\/\*/ )
      {
         # do nothing
      }
      else
      {   
         if( substr($line, 0, 1) ne "\"" || substr($line, $x_max+1, 1) ne "\"" )
         {
            die "ERROR: Wrong Pixel Size - expecting x = $x_max!\n";
         } 

         for($i=1;$i<=$x_max;++$i)
         {
            push(@graphic,substr($line, $i, 1));
         }

         if( ++$y == $y_max )
         {
            $state = "READ_BOTTOM";
         }
      }   
   }      
   elsif( $state eq "READ_BOTTOM" )
   {
      if( $line ne "};" )
      {
         die "ERROR: Wrong Pixel Size - expecting y = $x_max!\n";
      }
   }   
   else
   {   
      die "FATAL: Unknown State: ${state}\n";
   }
}   
close(IN);

my $expected_size = $x_max * $y_max;
if( scalar(@graphic) != $expected_size )
{
    die sprintf("FATAL: Wrong Graphic Size: %d (has to be $expected_size)!\n",scalar(@graphic));
}

my $character_offset = 0;
if( $icons_per_line )
{
   $character_offset = $x_max / $icons_per_line;
   printf "Calculated Character Offset: %d\n", $character_offset;
   printf "Character Height:            %d\n", $height;

   if( $y_max % $height )
   {
      die "ERROR: pic height doesn't comply with character height\n";
   }

   my @new_graphic = ();

   my $y=0;
   my $new_y_max = ($x_max * $y_max) / 8;
   my $i;
   my $j;
   my $k;
   my $l;
   my $m;
   for($i=0; $i<$y_max/$height; ++$i)
   {
      for($j=0; $j<$icons_per_line; ++$j)
      {
	 for($l=0; $l<$height; ++$l)
	 {
            for($k=0; $k<$character_offset/8; ++$k)
            {
	       for($m=0; $m<8; ++$m)
	       {
		  my $ix = ($j*$character_offset + $k + $m) + (($i*$height + $l)*$x_max);
		  my $new_ix = $y*8 + $m;
		  $new_graphic[$new_ix] = $graphic[$ix];
	       }
	       ++$y;
	    }
	 }
      }
   }
   @graphic = @new_graphic;
   $x_max = 8;
   $y_max = $new_y_max;

   # workaround for bug in MPASM: combine two lines together
   $x_max = 16;
   $y_max = $y_max / 2;
}

open(OUT,">$output_file");

printf OUT ";; converted with 'convpix_8x8 %s'%c\n%c\n", $cmd_args, 13, 13;

if( 0 ) # for debugging
{
   for($h=0; $h<$y_max; ++$h)
   {
      for($i=0; $i<$x_max; ++$i)
      {
         print OUT $graphic[$h*$x_max + $i];
      }   
      print OUT "\n";
   }
   close(OUT);
   exit;
}

for($h=0; $h<$y_max; ++$h)
{
   for($i=0; $i<$x_max/(2*8); ++$i)
   {
      my $bits = "";
      for($j=0; $j<2; ++$j)
      {
         for($k=0; $k<8; ++$k)
         {
	    if( $graphic[$h*$x_max + $i*2*8 + $j*8 + (7-$k)] eq "." )
	    {
	       $bits = "1${bits}";
	    }
	    else
	    {
	       $bits = "0${bits}";
	    } 
         }
      }
      printf OUT "\tda b'${bits}'%c\n", 13;
   }
   printf OUT "%c\n%c\n", 13, 13;
}

close(OUT);
print "Ok.\n";

