#!/usr/bin/perl

# using tempered scale
# it's 3'o'clock in the morning, currently I don't know an easier way to calculate the table like shown here...

my $force_one_block = 5;

my $fact = 1.059463; # 12rt2
printf $force_one_block ? "MBFM_FRQ_TABLE_ALT" : "MBFM_FRQ_TABLE\r\n";

my $i;
my $a_freq = 13.75;
for($x=0; $x<128; $x+=12)
{
    my $octave  = int($x / 12);
    my @note = ();

    $note[9]  = $a_freq;
    for($i=10; $i<=11; ++$i) { $note[$i] = $note[$i-1] * $fact; }
    for($i=8; $i>=0; --$i)   { $note[$i] = $note[$i+1] / $fact; }

    my @word = ();
    my @error = ();
    for($i=0; $i<12; ++$i)
    {
	my @fnum = ();
	my $block;
	my $less_error = 1000000;
	for($block=0; $block<8; ++$block)
	{
	    if( !$force_one_block || $block == $force_one_block )
	    {
		$fnum = int(($note[$i] * (1 << (20-$block))) / 49715.0);
		$freq = ($fnum / (1 << (20-$block))) * 49715.0;

#   	        printf "%2d %2d %10.4f %10.4f %04x %f\n", $note, $octave, $note[$i], $freq, $fnum, $error;
		if( $freq > 0 )
		{
		    my $error = ($fnum-$freq) / $freq;
		
		    if( $fnum < 0x400 && abs($error) < abs($less_error) )
		    { 
			$less_error = $error; 
			$word[$i] = ($block << 10) | ($fnum);
		    }
		}
	    }
	}
	if( abs($less_error) > 1000 )
	{
	    $word[$i] = 0x1fff;
	}
    }

    my @str = ("C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-");
    for($i=0; $i<12; ++$i)
    {
	if( $force_one_block )
	{
	    my $word = ($word[$i] << 6) & 0xffff;
	    printf "\tdb\t0x%02x, 0x%02x\t; %10.3f Hz (0x%04x  %s%d)\r\n", 
	        $word & 0xff, 
	        $word >> 8, 
	        $note[$i], 
	        $word,
	        $str[$i], 
	        $octave-2;
	}
	else
	{
	    printf "\tdb\t0x%02x, 0x%02x\t; %10.3f Hz (%d:0x%03x  %s%d)\r\n", 
	        $word[$i] & 0xff, 
	        $word[$i] >> 8, 
	        $note[$i], 
	        $word[$i] >> 10,
	        $word[$i] & 0x3ff,
	        $str[$i], 
	        $octave-2;
	}
    }

#   my $value0 = int(128 * sin((3.1415927/2) * ($x+0)/128));
#   my $value1 = int(128 * sin((3.1415927/2) * ($x+1)/128));

#   printf "; %3d | %3d\r\n", $x, $value0;
#   printf "; %3d | %3d\r\n", $x+1, $value1;

    $a_freq *= 2;
}



sub abs
{
    my ($value) = @_;
    return $value >= 0 ? $value : -$value;
}
