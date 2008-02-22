#!/usr/bin/perl

my @final_ctr = ();
my @final_add = ();
my @final_frq = ();

$tick_rate = 800E-6;

printf ";   x | Frq.   | CTR | ADD | Result Frq\r\n";
printf "; ----+--------+-----+-----+-----------\r\n";

int i;
for($x=0; $x<127; ++$x)
{
   # FRQ = ADD/(CTR*512*TICK_RATE) -> ADD/CTR = FRQ*512*TICK_RATE
   my $frq = exp(0.0305*$x)-1;

   my $add;
   my $ctr;
   my $found_frq;

   my $diff;

   $diff = $frq * 0.025;

   my $found_frq = 0;
   my $found_add;
   my $found_ctr;
   for($add=1; !$found_frq && $add<256; ++$add)
   {
      for($ctr=1; !$found_frq && $ctr<256; ++$ctr)
      {
         my $calc_frq = $add/($ctr*512*$tick_rate);
         if( ($calc_frq > ($frq-$diff)) && ($calc_frq < ($frq+$diff)) )
	 {
            $found_frq = $calc_frq;
            $found_add = $add;
            $found_ctr = $ctr;
	 }
      }
   }
   printf "; %3d | %6.3f | %3d | %3d | %6.3f\r\n", $x+1, $frq, $found_ctr, $found_add, $found_frq;
   $final_ctr[$x] = $found_ctr;
   $final_add[$x] = $found_add;
   $final_frq[$x] = $found_frq;
}
printf "\r\n";
printf "SID_LFO_TABLE\r\n";

for($x=0; $x<127; ++$x)
{
   printf "\tdb\t0x%02x, 0x%02x\t; ADD/CTR for frq=%6.2f\r\n", $final_add[$x], $final_ctr[$x], $final_frq[$x];
}
