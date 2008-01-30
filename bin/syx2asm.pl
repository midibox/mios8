#!/usr/bin/perl -w
#########################################################################################
#
# SysEx-to-ASM converter
# Author: Thorsten.Klose@gmx.de (2003-09-21)
#
# Converts a MIOS SysEx file to an assembler file which can be assembled with MPASM
#
#########################################################################################
#
# SysEx format: see hex2syx.pl
#
#########################################################################################
#
# SYNTAX:  syx2asm.pl <syx-file> <asm-file> [<-debug>]
# EXAMPLE: hex2syx.pl main.syx memory_dump.inc
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): d:\perl\bin\perl syx2asm.pl main.syx memory_dump.inc
# within the DOS command window
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
   die "SYNTAX:  syx2asm.pl <syx-file> <asm-file> [<-debug>]\n" .
       "EXAMPLE: syx2asm.pl main.syx memory_dump.inc\n";
}

$in_file  = $ARGV[0];
$out_file = $ARGV[1];

#########################################################################################
# Parse .syx file
#########################################################################################

open(FILE_IN, "<${in_file}") || die "ERROR: Cannot open ${in_file} for reading!\n";
binmode(FILE_IN);
my $len = 0;
my $dump = 0;
if( ($len=sysread(FILE_IN, $dump, $max_file_len)) <= 0 )
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

my $state = "WAIT_HEADER";
my $invalid_found = 0;
my $device_id_found = -1;
my @syx_header = (0xf0, 0x00, 0x00, 0x7e, 0x40);
my $syx_header_pos = 0;
my $cmd = -1;
my $cmd_pos = -1;
my $bankstick;
my $checksum;
my $ah;
my $al;
my $ch;
my $cl;
my $address;
my $address_ext;
my $pic_address;
my $counter;
my $b_counter;
my $b;
my $m_counter;
my $m;
my $offset;
my @mem_block;
my %mem = ();
my $blocks_found = 0;

for($i=0;$i<$len;++$i)
{
   my $byte = ord(substr($dump, $i, 1));

   if( $debug )
   {
      printf "[%s] %02X\n", $state, $byte;
   }

   if( $byte >= 0xf8 )
   {
      # ignore realtime events
   }
   elsif( $state ne "WAIT_HEADER" && ($byte & 0x80) )
   {
      # reset state machine on any status byte (also F7...)
      $state = "WAIT_HEADER";
   }
   elsif( $state eq "WAIT_HEADER" )
   {
      my $expected = $syx_header[$syx_header_pos];

      if( $byte != $expected )
      {
	 $syx_header_pos = 0;
	 $invalid_found = 1;
      }

      if( $byte == $expected )
      {
	 ++$syx_header_pos;
	 if( $syx_header_pos == scalar(@syx_header) )
	 {
	    $syx_header_pos = 0;
	    $state = "WAIT_DEVICE_ID";
	 }
      }
   }
   elsif( $state eq "WAIT_DEVICE_ID" )
   {
      if( $device_id_found == -1 )
      {
	 $device_id_found = $byte;
      }

      if( $device_id_found != $byte )
      {
	 printf("ERROR: found SysEx stream for different device id, skipping this block!\n");
	 $state = "WAIT_HEADER";
      }
      else
      {
	 $state = "WAIT_CMD";
      }
   }
   elsif( $state eq "WAIT_CMD" )
   {
      $cmd = $byte & 0x0f;
      $address_ext = $byte >> 4;
      $cmd_pos = 0;
      $checksum = 0;

      if( $cmd == 0x02 )
      {
	 $state = "WRITE_CMD";
      }
      elsif( $cmd == 0x03 )
      {
	 $state = "BS_SELECT_CMD";
      }
      else
      {
	 printf "WARNING: skipping command %02X block!\n", $cmd;
	 $state = "WAIT_HEADER";
      }
   }
   elsif( $state eq "WRITE_CMD" )
   {
      $checksum = ($checksum + $byte) & 0x7f;

      if   ( $cmd_pos == 0 ) { $ah = $byte; }
      elsif( $cmd_pos == 1 ) { $al = $byte; }
      elsif( $cmd_pos == 2 ) { $ch = $byte; }
      elsif( $cmd_pos == 3 ) { $cl = $byte; $b_counter = 0; $b=0; $offset = 0; }
      elsif( $cmd_pos > 5 && $offset == $counter )
      {
	 printf "Found Block %06X-%06X - Checksum: %02X\n", $pic_address, $pic_address+$offset-1, $byte;
	 if( $checksum != 0 ) # the sum of all checksums + the checksum itself must be zero
	 {
	    printf "ERROR: checksum doesn't match - block skipped!\n", $checksum;
	 }
	 else
	 {
	    for($block_offset=0; $block_offset < $counter; ++$block_offset)
	    {
	       $mem{$pic_address + $block_offset} = $mem_block[$block_offset];
	    }
	    ++$blocks_found;
	 }
	 ++$offset;
      }
      elsif( $cmd_pos > 5 && $offset > $counter )
      {
	 printf "WARNING: block at 0x%06x contains more bytes than expected!\n", $pic_address + $offset;
      }
      else
      {
	 if( $cmd_pos == 4 )
	 {
	    $address = ($ah << 10) | ($al << 3);
	    $counter = ($ch << 10) | ($cl << 3);

	    $pic_address = 0;
	    if( $address >= 0x00000 && $address < 0x08000 )
	    {
	       # flash memory
	       $pic_address = $address + ($address_ext << 15);
	    }
	    elsif( $address >= 0x08000 && $address < 0x08100 ) 
	    { 
	       # EEPROM memory
	       $pic_address = 0xf00000 + ($address&0xfff); 
	    }
	    elsif( $address >= 0x08400 && $address < 0x10000 )
	    { 
	       # invalid address range
	       $pic_address = 0xf00000 + ($address&0xfff);
	    }
	    elsif( $address >= 0x10000 && $address < 0x20000 )
	    { 
	       # BankStick memory
	       $pic_address = 0x400000 + ($address&0xffff); 
	    }
	    else { die "Malfunction"; }

	 }

	 $m = $byte;
	 for($m_counter=0; $m_counter<7; ++$m_counter)
	 {
	    $b = ($b << 1) | (($m&0x40) ? 1 : 0);
	    $m = $m << 1;
	    ++$b_counter;
	    if( $b_counter == 8 )
	    {
	       $mem_block[$offset] = $b;
	       $b_counter = 0;
	       $b = 0;
	       ++$offset;
	    }
	 }
      }
      ++$cmd_pos;
   }
   elsif( $state eq "BS_SELECT_CMD" )
   {
      $bankstick = $byte;
      printf "BankStick %d has been selected\n", $bankstick;
      # state machine will be reset with the next 0xf7
   }
   else
   {
      die "FATAL: unknown state: ${state}\n";
   }
}

if( !$blocks_found )
{
   print "No blocks found - writing skipped!\n";
   exit;
}

#########################################################################################
# Write out assembler file
#########################################################################################

open(FILE_OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";

printf "Writing %d block(s) to %s\n", $blocks_found, $out_file;

printf FILE_OUT ";; Converted with: 'syx2asm.pl " . join(" ", @ARGV) . "'\r\n";
printf FILE_OUT "\r\n";
printf FILE_OUT "\t;; Delete following line if you include the file into an existing project:\r\n";
printf FILE_OUT "\tLIST\tP=PIC18F4685, R=DEC\r\n";
printf FILE_OUT "\t;; suppress \"Address exceeds maximum range for this processor\" message\r\n";
printf FILE_OUT "\tERRORLEVEL\t-220\r\n";
printf FILE_OUT "\r\n";

print FILE_OUT get_mem(0x000000, 0x03ffff); # internal flash
print FILE_OUT get_mem(0xf00000, 0xf00fff); # internal EEPROM
print FILE_OUT get_mem(0x400000, 0x40ffff); # external BankStick EEPROM

printf FILE_OUT "\r\n\tEND\r\n";

close(FILE_OUT);

exit;

#########################################################################################
#
# Subroutines
#
#########################################################################################

sub get_mem
{
   my ($begin, $end) = @_;
   my $org_set = 0;
   my @bytes = ();
   my $address = $begin;
   my $ret = "";

   while( $address < $end )
   {
      if( exists $mem{$address} )
      {
	 if( !$org_set )
	 {
	    $org_set = 1;
	    $ret .= sprintf "\torg\t0x%06x\r\n", $address;
	 }

	 push @bytes, sprintf("0x%02x", $mem{$address});
	 if( scalar(@bytes) == 8 )
	 {
	    $ret .= sprintf "%s   ;; (0x%06x)\r\n", get_mem_line(@bytes), $address-scalar(@bytes)+1;
	    @bytes = ();
	 }
      }
      else
      {
	 if( scalar(@bytes) )
	 {
	    $ret .= sprintf "%s   ;; (0x%06x)\r\n", get_mem_line(@bytes), $address-scalar(@bytes)+1;
	    @bytes = ();
	 }
	 $org_set = 0;
      }

      ++$address;
   }

   return $ret;
}


sub get_mem_line
{
   my @bytes = @_;
   my $ret;

   $ret = sprintf "\tdb\t%s", join(", ", @bytes);
   
   return $ret;
}
