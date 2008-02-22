#!/usr/bin/perl
# $Id$

printf ";; generated with 'perl utils\curve3_table.pl > cv_curve3_table.inc\n";
printf "CV_CURVE3_TABLE\n";

int i;
for($i=0; $i<128; ++$i)
{
  my $word = int(0x1000 * (1-exp(-$i/32)));
  printf "\tdw\t0x%04x  ; %3d\n", $word, $i;
}
