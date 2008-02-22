#!/usr/bin/perl
#
# bpm.pl by Thorsten Klose (2001-03-22)
# calcs BPM values for MIDImon
#

my $bpm;
my @bytes = ();

# counter = 2*BPM*24/60
# -> BPM = counter * 60 / (2*24)
for($cntr = 0; $cntr < 256; ++$cntr)
{
   my $bpm = $cntr * (60 / 48);

   if( $bpm > 255 ) { $bpm = 255; }

   push @bytes, sprintf("0x%02X", $bpm);

   if( scalar(@bytes) >= 8 )
   {
      print "\tdb\t" . join(", ", @bytes) . "\n";
      @bytes = ();
   }
}
