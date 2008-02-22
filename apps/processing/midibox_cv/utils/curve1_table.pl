#!/usr/bin/perl
# $Id$

printf ";; generated with 'perl utils\curve1_table.pl > cv_curve1_table.inc'\n";
printf "CV_CURVE1_TABLE\n";

int i;
my $fact = 1.059463; # 12rt2
my $v    = 0.125;    # voltage at C-0
my @str = ("C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-");

for($i=0; $i<128; ++$i)
{
  my $octave  = int($i / 12);
  my @note = ();

  my $word = $v / 0.0025;
  if( $word > 0xfff ) { $word = 0xfff; }
  printf "\tdw\t0x%04x  ; %s%d: %6.3fV\n", $word, $str[$i % 12], $octave, $v;

  $v *= $fact;
}
