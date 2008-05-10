#!/usr/bin/perl
#
# gen_bpm_table.pl by Thorsten Klose (2001-03-22)
# calcs BPM values for the Magic MIDI Delay
#

my $bpm;

printf "\tdw\t0x%02X%02X\t; dummy entry\r\n", 0xff, 0xff;

for($bpm = 1; $bpm < 256; ++$bpm)
{
   my $delay = 60.0 / ($bpm * 24.0);
   my $timerdelay = $delay / 8; # (using prescaler 1:8)
#   printf "%3d: $timerdelay s\n", $bpm;
   my $cycles = int($timerdelay / 100E-9);
#   printf "%3d: $cycles cycles\n", $bpm;
   if( $cycles > 0xffff ) { $cycles = 0xffff; }
   my $tmrh = int($cycles/256);
   my $tmrl = int($cycles-($tmrh*256));
#   printf "%3d: $tmrh $tmrl\n", $bpm;

   printf "\tdw\t0x%02X%02X\t; BPM: %d\r\n", $tmrh, $tmrl, $bpm;
}
