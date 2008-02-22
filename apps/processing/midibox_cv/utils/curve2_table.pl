#!/usr/bin/perl
# $Id$

printf ";; generated with 'perl utils\curve2_table.pl > cv_curve2_table.inc'\n";
printf "CV_CURVE2_TABLE\n";

int i;
for($i=0; $i<128; ++$i)
{
  printf "\tdw\t0x%04x  ; %3d\n", 0xfe8 - ($i << 5), $i;
}
