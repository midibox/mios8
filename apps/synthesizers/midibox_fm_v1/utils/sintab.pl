#!/usr/bin/perl

printf "SID_SIN_TABLE\r\n";

int i;
for($x=0; $x<128; $x+=2)
{
   my $value0 = int(128 * sin((3.1415927/2) * ($x+0)/128));
   my $value1 = int(128 * sin((3.1415927/2) * ($x+1)/128));

#   printf "; %3d | %3d\r\n", $x, $value0;
#   printf "; %3d | %3d\r\n", $x+1, $value1;
   printf "\tdb\t0x%02x, 0x%02x\r\n", $value0, $value1;
}


