#!/usr/bin/perl
# $Id$

$tick_rate = 1E-3;

printf "const unsigned int eg_add_table[256] = {\r\n";

printf "  %5d, // %3d: %6.3f s\r\n", 0, 0, 0;

for($x=1; $x<256; ++$x)
{
  # TIME = (32768 / ADD) * TICK_RATE
  # TIME = (32768 * TICK_RATE) / ADD
  # -> ADD = (32768 * TICK_RATE) / TIME

  my $tim = x * 0.0030 + exp(0.00002*$x*$x)-1;
  my $add = int((32768 * $tick_rate) / $tim);
  if( $add >= 32768 ) { $add = 32768; $tim = 1E-3; }
  $effective_tim = (32768 / $add) * $tick_rate;
  printf "  %5d, // %3d: %6.3f s\r\n", $add, $x, $effective_tim;
}

printf "};\r\n";

