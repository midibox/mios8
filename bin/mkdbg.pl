#!/usr/bin/perl -w
#########################################################################################
#
# This script creates a debug SysEx command for MIOS
# Author: Thorsten.Klose@gmx.de (2003-02-12)
#
#########################################################################################
#
# SysEx format for debug command:
#    F0 00 00 7E 40 <device-id> 0D <CMD> <AU> <AH> <AL> <WH> <WL> <MP1H> <MP1L> <MP2H> <MP2L> <MP3H> <MP3L> F7
#
#    <CMD>:
#       Debug command. Currently following commands are provided:
#          01: call function. Address of function in <AU>-<AL>; WREG and MIOS_PARAMETERx in the following bytes
#          02: read SRAM. Address in <AU>-<AL>; counter in <WH><WL> (high-byte) and <MP1H><MP1L> (low-byte)
#          03: write SRAM. Address in <AU>-<AL>; write value in <WH><WL> (only one value can be written)
#
#########################################################################################
#
# SYNTAX:   mkdbg.pl exec [-device <id>] <outfile> <address> <WREG> <MIOS_PARAMETER1> <MIOS_PARAMETER2> <MIOS_PARAMETER3> 
#           mkdbg.pl read [-device <id>] <outfile> <address> <counter>
#           mkdbg.pl write [-device <id>] <outfile> <address> <byte>
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): d:\perl\bin\perl hex2syx.pl main.hex
# within the DOS command window
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;
$device_id = "0x00";

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   "device_id=s" => \$device_id,
   );

if( scalar(@ARGV) < 3 )
{
   die "SYNTAX: mkdbg.pl exec  [-device <id>] <outfile> <address> <WREG> <MIOS_PARAMETER1> <MIOS_PARAMETER2> <MIOS_PARAMETER3>\n" .
       "        mkdbg.pl read  [-device <id>] <outfile> <address> <counter>\n" .
       "        mkdbg.pl write [-device <id>] <outfile> <address> <byte>\n";
}

$device_id = hex $device_id;
if( $device_id < 0x00 || $device_id > 0x7f )
{
   die "ERROR: Device ID must be in range 0x00-0x7f!\n";
}

$cmd = $ARGV[0];
$out_file = $ARGV[1];

$cmd_id          = 0x00;
$address         = 0x0000;
$wreg            = 0x00;
$mios_parameter1 = 0x00;
$mios_parameter2 = 0x00;
$mios_parameter3 = 0x00;

if   ( $cmd eq "exec" )
{
   $cmd_id = 0x01;
   if( scalar(@ARGV) != 7 )
   {
      die "ERROR: missing parameters!\n";
   }
   $address = hex $ARGV[2];
   $wreg = hex $ARGV[3];
   $mios_parameter1 = hex $ARGV[4];
   $mios_parameter2 = hex $ARGV[5];
   $mios_parameter3 = hex $ARGV[6];
}
elsif( $cmd eq "read" )
{
   $cmd_id = 0x02;
   if( scalar(@ARGV) != 4 )
   {
      die "ERROR: missing parameters!\n";
   }
   $address = hex $ARGV[2];
   $wreg = ((hex $ARGV[3]) >> 8) & 0xff;
   $mios_parameter1 = ((hex $ARGV[3]) >> 0) & 0xff;
}
elsif( $cmd eq "write" )
{
   $cmd_id = 0x03;
   if( scalar(@ARGV) != 4 )
   {
      die "ERROR: missing parameters!\n";
   }
   $address = hex $ARGV[2];
   $wreg = hex $ARGV[3];
}
else
{
   die "ERROR: unknown command!\n";
}

#########################################################################################
# Write out .syx file
#########################################################################################

open(OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";
binmode(OUT);

sysex_write($device_id, $cmd_id, $address, $wreg, $mios_parameter1, $mios_parameter2, $mios_parameter3);

close(OUT);

exit;


#########################################################################################
#
# Subroutines
#
#########################################################################################

sub sysex_write
{
   my ($device_id, $cmd_id, $address, $wreg, $mios_parameter1, $mios_parameter2, $mios_parameter3) = @_;

   my $au = ($address >> 14) & 0x7f;
   my $ah = ($address >>  7) & 0x7f;
   my $al = ($address >>  0) & 0x7f;
   my $wh = ($wreg >> 4) & 0x0f;
   my $wl = ($wreg     ) & 0x0f;
   my $mp1h = ($mios_parameter1 >> 4) & 0x0f;
   my $mp1l = ($mios_parameter1     ) & 0x0f;
   my $mp2h = ($mios_parameter2 >> 4) & 0x0f;
   my $mp2l = ($mios_parameter2     ) & 0x0f;
   my $mp3h = ($mios_parameter3 >> 4) & 0x0f;
   my $mp3l = ($mios_parameter3     ) & 0x0f;

   print OUT sprintf "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
      0xf0, 0x00, 0x00, 0x7e, 0x40, $device_id, 0x0d, $cmd_id, 
      $au, $ah, $al, $wh, $wl, $mp1h, $mp1l, $mp2h, $mp2l, $mp3h, $mp3l, 
      0xf7;
}
