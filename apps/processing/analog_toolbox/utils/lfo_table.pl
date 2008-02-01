#!/usr/bin/perl
# $Id$

$tick_rate = 1E-3;

printf "const unsigned int lfo_add_table[256] = {\r\n";

for($x=0; $x<256; ++$x)
{
  # FRQ = 1 / ((65536 / ADD) * TICK_RATE)
  # -> FRQ = ADD / (65536 * TICK_RATE)
  # -> ADD = FRQ * 65536 * TICK_RATE

  my $frq = exp(0.018*$x)-1;
  my $add = int($frq * 65536 * $tick_rate);
  my $effective_frq = $add / (65536 * $tick_rate);
  printf "  %5d, // %3d: %6.3f Hz\r\n", $add, $x, $effective_frq;
}

printf "};\r\n";

