#!/usr/bin/perl
# $Id$

printf "const unsigned char sinewave[256] = { ";

for($x=0; $x<256; ++$x)
{
   my $value = 128 + int(128 * sin((3.1415927*2) * $x/256));

   if( !($x % 16) ) {
     printf "\r\n  ";
   }

   printf "%3d,", $value;
}

printf "\r\n};\r\n";


