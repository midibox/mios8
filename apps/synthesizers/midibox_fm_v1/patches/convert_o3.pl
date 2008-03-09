#!/usr/bin/perl -w
#########################################################################################
#
# O3 Converter script
# Author: tk@midibox.org (2004-10-15)
#
# Converts a .o3 file to a assembler compatible data table
#
#########################################################################################
#
# SYNTAX:  convert_o3.pl <bin-file> <output-file> [<-debug>]
# EXAMPLE: convert_o3.pl std.o3 bankstick.asm -debug
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): d:\perl\bin\perl convert_o3.pl std.o3 std_o3.inc
# within the DOS command window
#
#########################################################################################
#
# Data format taken from sbiload.c, which is part of the Alsa tools package
# http://www.alsa-project.org
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;

$max_file_len = 0x80000;

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   );

if( scalar(@ARGV) != 2 )
{
   die "SYNTAX:  convert_o3.pl <bin-file> <output-file> [<-debug>]\n" .
       "EXAMPLE: convert_o3.pl std.o3 std_o3.inc\n";
}

$in_file  = $ARGV[0];
$out_file = $ARGV[1];

#########################################################################################
# Read .o3 file
#########################################################################################

open(FILE_IN, "<${in_file}") || die "ERROR: Cannot open ${in_file} for reading!\n";
binmode(FILE_IN);
my $len = 0;
my $o3 = 0;
if( ($len=sysread(FILE_IN, $o3, $max_file_len)) <= 0 )
{
   print "WARNING: ${in_file} is empty - skipping!\n";
}
elsif( $len > $max_file_len )
{
   print "WARNING: ${in_file} bigger than ${max_file_len} (definitely too large for MIOS - skipping!\n";
}
else
{
   print "Loading ${in_file} (${len} bytes)\n";
}
close(FILE_IN);


#########################################################################################
# Write out voice informations
#########################################################################################

open(FILE_OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";

printf "Writing %d byte(s) to %s\n", $len, $out_file;

printf FILE_OUT ";; Converted with: 'convert_o3.pl " . join(" ", @ARGV) . "'\r\n";
printf FILE_OUT "\r\n";
printf FILE_OUT "\t;; Delete following line if you include the file into an existing project:\r\n";
printf FILE_OUT "\tLIST\tP=PIC18F452, R=DEC\r\n";
printf FILE_OUT "\t;; suppress \"Address exceeds maximum range for this processor\" message\r\n";
printf FILE_OUT "\tERRORLEVEL\t-220\r\n";
printf FILE_OUT "\r\n";
printf FILE_OUT "\torg\t0x%06x\r\n", 0x400000;
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "CS_CFG\r\n";
printf FILE_OUT "\tdb\t0x73,0x12,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
printf FILE_OUT "\tdb\t0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
printf FILE_OUT "\tdb\t0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
printf FILE_OUT "\tdb\t0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "RESERVED\r\n";
for($i=0x20; $i<0x400; $i+=0x8)
{
   printf FILE_OUT "\tdb\t0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
}
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "ENSEMBLES\r\n";
for($i=0x400; $i<0xc00; $i+=0x40)
{
   printf FILE_OUT "ENSEMBLE_%02d\r\n", ($i-0x400)/0x40 + 1;
   printf FILE_OUT "\tdb\t0x00,0x04,0x00,0x00,0x7f,0x00,0x7f,0x40\r\n";
   printf FILE_OUT "\tdb\t0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00\r\n";

   printf FILE_OUT "\tdb\t0x00,0x24,0x01,0x00,0x7f,0x00,0x7f,0x40\r\n";
   printf FILE_OUT "\tdb\t0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00\r\n";

   printf FILE_OUT "\tdb\t0x00,0x53,0x02,0x00,0x7f,0x00,0x7f,0x40\r\n";
   printf FILE_OUT "\tdb\t0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00\r\n";

   printf FILE_OUT "\tdb\t0x00,0x62,0x03,0x00,0x7f,0x00,0x7f,0x40\r\n";
   printf FILE_OUT "\tdb\t0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00\r\n";

   printf FILE_OUT "\r\n";
}
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "DRUMSETS\r\n";
for($i=0xc00; $i<0x1000; $i+=0x40)
{
   printf FILE_OUT "DRUMSET_%02d\r\n", ($i-0xc00)/0x40 + 1;
   printf FILE_OUT "\tdb\t0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0f, 0x17\r\n";
   printf FILE_OUT "\tdb\t0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0xf5\r\n";

   printf FILE_OUT "\tdb\t0xf0, 0xf0, 0xf0, 0xf4, 0xf0, 0xb8, 0x77, 0x05\r\n";
   printf FILE_OUT "\tdb\t0x08, 0x07, 0x75, 0x05, 0x00, 0x00, 0x00, 0x00\r\n";

   printf FILE_OUT "\tdb\t0x02, 0x00, 0x00, 0x10, 0x20, 0x20, 0x60, 0x48\r\n";
   printf FILE_OUT "\tdb\t0x6d, 0x00, 0x70, 0x00, 0x24, 0x24, 0x2e, 0x2e\r\n";

   printf FILE_OUT "\tdb\t0x2c, 0x2c, 0x26, 0x26, 0x28, 0x28, 0x30, 0x30\r\n";
   printf FILE_OUT "\tdb\t0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\r\n";

   printf FILE_OUT "\r\n";
}
printf FILE_OUT "\r\n";

#########################################################################################

my $label_name = uc($in_file);
$label_name =~ s/\./_/gi;
printf FILE_OUT "PATCHES\r\n";
printf FILE_OUT "${label_name}\r\n";

my $pos = 0;
my $instrument = 0;
while( $pos < $len )
{
   # read header
   my @key =  get_bin($o3, \$pos, 4);
   my @name = get_bin($o3, \$pos, 16);
   my @extra = get_bin($o3, \$pos, 16);
   my @voice = get_bin($o3, \$pos, 24);

   my $c;
   my $key = "";
   my $name = "";
   foreach $c (@key)  { $key .= chr($c);  }
   foreach $c (@name) { $name .= $c ? chr($c) : " "; }

   my $instrument_type = 0;
   if( $key eq "SBI\032" || $key eq "2OP\032" )
   {
      $instrument_type = "OPL2";
   }
   elsif( $key eq "4OP\032" )
   {
      $instrument_type = "OPL3";
   }

   printf "%3d: \"%s\" (%s)\n", $instrument, $name, $instrument_type;
   printf FILE_OUT "PATCH_%03d\r\n", $instrument + 1;
   printf FILE_OUT "\tdb\t\"%s\"\r\n", $name, $instrument;

   my $op;
   my @par = ();
   for($op=0; $op < 4; ++$op)
   {
      my $op_offset = $op < 2 ? $op : (11 + ($op-2));
      my $flags_mult      = $voice[0 + $op_offset];
      my $ksl_tl          = $voice[2 + $op_offset];
      my $ar_dr           = $voice[4 + $op_offset];
      my $sl_rr           = $voice[6 + $op_offset];
      my $ws              = $voice[8 + $op_offset];

      $par[0x10 + $op] = $flags_mult;
      $par[0x14 + $op] = $ksl_tl;
      $par[0x18 + $op] = $ar_dr;
      $par[0x1c + $op] = $sl_rr;
      $par[0x20 + $op] = $ws;
   }

   my $op12_ch_fb_cnt = $voice[10];
   my $op34_ch_fb_cnt = $voice[11+10];
   my $echo_delay = $extra[9];
   my $echo_atten = $extra[10];
   my $chorus_spread = $extra[11];
   my $trnsps = $extra[12];
   my $fix_dur = $extra[13];
   my $modes = $extra[14];
   my $fix_key = $extra[15];

   $par[0x24] = $op12_ch_fb_cnt;
   $par[0x25] = $op34_ch_fb_cnt;


   $par[0x26] = 0x02; # CTRL1
   $par[0x27] = 0x00; # CTRL2
   $par[0x28] = 0x00; # AOUT_MOD_ASSGN
   $par[0x29] = 0x40; # FINETUNE
   $par[0x2a] = 0x02; # PITCHRANGE
   $par[0x2b] = 0x00; # PORTA_RATE
   $par[0x2c] = 0x00; # VELOCITY_ASSIGN
   $par[0x2d] = 0x00; # VELOCITY_INIT
   $par[0x2e] = 0x40; # VELOCITY_DEPTH
   $par[0x2f] = 0x00; # AFTERTOUCH_ASSIGN
   $par[0x30] = 0x00; # AFTERTOUCH_INIT
   $par[0x31] = 0x40; # AFTERTOUCH_DEPTH
   $par[0x32] = 0x00; # MODWHEEL_ASSIGN
   $par[0x33] = 0x00; # MODWHEEL_INIT
   $par[0x34] = 0x40; # MODWHEEL_DEPTH
   $par[0x35] = 0x00; # WT_RATE
   $par[0x36] = 0x15; # WT_PAR1
   $par[0x37] = 0x17; # WT_PAR2
   $par[0x38] = 0x52; # WT_PAR3
   $par[0x39] = 0x05; # LFO1_MODE
   $par[0x3a] = 0x00; # LFO1_PHASE
   $par[0x3b] = 0x50; # LFO1_RATE
   $par[0x3c] = 0x40; # LFO1_DEPTH_P
   $par[0x3d] = 0x40; # LFO1_DEPTH_V1
   $par[0x3e] = 0x40; # LFO1_DEPTH_V2
   $par[0x3f] = 0x40; # LFO1_DEPTH_V3
   $par[0x40] = 0x40; # LFO1_DEPTH_V4
   $par[0x41] = 0x40; # LFO1_DEPTH_L
   $par[0x42] = 0x40; # LFO1_DEPTH_A
   $par[0x43] = 0x05; # LFO2_MODE
   $par[0x44] = 0x00; # LFO2_PHASE
   $par[0x45] = 0x70; # LFO2_RATE
   $par[0x46] = 0x40; # LFO2_DEPTH_P
   $par[0x47] = 0x40; # LFO2_DEPTH_V1
   $par[0x48] = 0x40; # LFO2_DEPTH_V2
   $par[0x49] = 0x40; # LFO2_DEPTH_V3
   $par[0x4a] = 0x40; # LFO2_DEPTH_V4
   $par[0x4b] = 0x40; # LFO2_DEPTH_L
   $par[0x4c] = 0x40; # LFO2_DEPTH_A
   $par[0x4d] = 0x00; # EG1_MODE
   $par[0x4e] = 0x20; # EG1_ATTACK
   $par[0x4f] = 0x60; # EG1_ATTLVL
   $par[0x50] = 0x20; # EG1_DECAY1
   $par[0x51] = 0x20; # EG1_DECLVL
   $par[0x52] = 0x20; # EG1_DECAY2
   $par[0x53] = 0x40; # EG1_SUSTAIN
   $par[0x54] = 0x20; # EG1_RELEASE
   $par[0x55] = 0x40; # EG1_CURVE
   $par[0x56] = 0x40; # EG1_DEPTH_P
   $par[0x57] = 0x40; # EG1_DEPTH_V1
   $par[0x58] = 0x40; # EG1_DEPTH_V2
   $par[0x59] = 0x40; # EG1_DEPTH_V3
   $par[0x5a] = 0x40; # EG1_DEPTH_V4
   $par[0x5b] = 0x40; # EG1_DEPTH_A
   $par[0x5c] = 0x40; # EG1_DEPTH_L

   $par[0x5d] = 0x00; # reserved
   $par[0x5e] = 0x00; # reserved
   $par[0x5f] = 0x00; # reserved

   for($i=0x10; $i<0x60; ++$i)
   {
      $par[$i] = sprintf("%02x", $par[$i]);
   }

   print FILE_OUT "\tdb\t0x" . join(",0x", @par[0x10..0x1f]) . "\r\n";
   print FILE_OUT "\tdb\t0x" . join(",0x", @par[0x20..0x2f]) . "\r\n";
   print FILE_OUT "\tdb\t0x" . join(",0x", @par[0x30..0x3f]) . "\r\n";
   print FILE_OUT "\tdb\t0x" . join(",0x", @par[0x40..0x4f]) . "\r\n";
   print FILE_OUT "\tdb\t0x" . join(",0x", @par[0x50..0x5f]) . "\r\n";

   printf FILE_OUT "\r\n";

   ++$instrument;
}
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "WAVETABLES\r\n";
for($i=0x4000; $i<0x8000; $i+=0x80)
{
   printf FILE_OUT "WAVETABLE_%02d\r\n", ($i-0x4000)/0x80 + 1;
   for($j=0x00; $j<0x80; $j+=8)
   {
      printf FILE_OUT "\tdb\t0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\r\n";
   }
   printf FILE_OUT "\r\n";
}
printf FILE_OUT "\r\n";

#########################################################################################

printf FILE_OUT "\tEND\r\n";

close(FILE_OUT);

exit;

#########################################################################################
#
# Subroutines
#
#########################################################################################

sub get_bin
{
    my ($o3, $offset_ptr, $len) = @_;
    my @bytes = ();
    my $i;

    for($i=$$offset_ptr; $i<$$offset_ptr+$len; ++$i)
    {
	push @bytes, ord(substr($o3, $i, 1));
    }
    $$offset_ptr += $len;

    return @bytes;
}

