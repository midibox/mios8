#!/usr/bin/perl

my @final_ctr = ();
my @final_add = ();
my @final_frq = ();

$tick_rate = 800E-6;

printf ";   x |  Time/s | ADD_H | ADD_L \r\n";
printf "; ----+---------+-------+-------\r\n";

int i;
for($x=0; $x<127; ++$x)
{
   my $tim = 0.0008+exp(0.00021*$x*$x)-1;

   my $ctr = 65536 / (($tim / $tick_rate) || 1);
   my $add_h = int($ctr / 256);
   my $add_l = $ctr - ($add_h * 256);

   printf "; %3d | %7.4f |   %3d |   %3d\r\n", $x+1, $tim, $add_h, $add_l;
   $final_tim[$x] = $tim;
   $final_addh[$x] = $add_h;
   $final_addl[$x] = $add_l;
}

printf "\r\n";
printf "SID_ENV_TABLE\r\n";
for($x=0; $x<127; ++$x)
{
   printf "\tdb\t0x%02x, 0x%02x\t; delay=%5.4f\r\n", $final_addl[$x], $final_addh[$x], $final_tim[$x];
}
