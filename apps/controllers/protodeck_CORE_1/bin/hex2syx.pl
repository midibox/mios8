#!/usr/bin/perl -w
#########################################################################################
#
# Hex-to-SysEx converter
# Author: tk@midibox.org (2003-01-30)
#
# Converts a INHX32 file to a SysEx file which can be uploaded to the MIDIbox using
# the MIOS Bootstrap loader
#
#########################################################################################
#
# SysEx format:
#    F0 00 00 7E 40 <device-id> <AU>2 <AH> <AL> <CH> <CL> <dump> <checksum> F7
#
#    <AU>/<AH>/<AL>: address calculated with following formula:
#       AU = (address >> 15) & 0x07
#       AH = (address >> 10) & 0x7f
#       AL = (address >> 3)  & 0x7f
#       address = (AH << 10) | (AL << 3)
#       if address <  0x08000                       : program memory (PIC: 0x0000-0x7fff)
#       if address >= 0x08000 and address < 0x080ff : data EEPROM memory (PIC: 0xf00000-0xf000ff)
#       if address >= 0x10000 and address < 0x1ffff : BankStick memory (MIOS: 0x400000-0x40ffff)
#
#       only supported for PIC18F4620 (64k PIC):
#       if address >= 0x08000 and address < 0x083ff : data EEPROM memory (PIC: 0xf00000-0xf003ff)
#       if address >  0x28000 and address < 0x28fff : (AU=1) program memory (PIC: 0x8000-0xffff)
#
#    <CH>/<CL>: byte counter * 8
#       CH = (counter >> 10) & 0x7f
#       CL = (counter >> 3) & 0x7f
#       counter = (CH << 10) | (CL << 3)
#
#    <dump>: the data which has to be written, scrambled to a sequence of 7-bit values
#
#    <checksum>: the checksum, from <AH> to last byte of the dump
#                -(sum of bytes) & 0x7f
#
#########################################################################################
#
# SYNTAX:   hex2syx.pl <hex-file>.hex [<-device_id 0x..>] [<-bankstick <0-7>] [-os_upload] [-force] [<-debug>]
# EXAMPLE1: hex2syx.pl main.hex
# EXAMPLE2: hex2syx.pl main.hex -device_id 0x12
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
$os_upload = 0;
$mem_64k   = 0;
$force     = 0;
$bankstick = 0;

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   "device_id=s" => \$device_id,
   "bankstick=s" => \$bankstick,
   "os_upload"   => \$os_upload,
   "force"       => \$force,
   "mem_64k"     => \$mem_64k,
   );

if( scalar(@ARGV) != 1 )
{
   die "SYNTAX:   hex2syx.pl <hex-file>.hex [<-device_id 0x..>] [<-bankstick <0-7>] [-os_upload] [-force] [-mem_64k] [<-debug>]\n" .
       "EXAMPLE1: hex2syx.pl main.hex\n" .
       "EXAMPLE2: hex2syx.pl main.hex -device_id 0x12\n" .
       "EXAMPLE3: hex2syx.pl main.hex -device_id 0x00 -bankstick 2\n";
}

$device_id = hex $device_id;
if( $device_id < 0x00 || $device_id > 0x7f )
{
   die "ERROR: Device ID must be in range 0x00-0x7f!\n";
}

if( $bankstick < 0 || $bankstick > 7 )
{
   die "ERROR: BankStick ID must be between 0 and 7!\n";
}

$in_file  = $ARGV[0];

if( lc(substr($in_file, length($in_file)-4)) ne ".hex" )
{
   die "ERROR: File doesn't end with '.hex'\n";
}

$out_file = substr($in_file, 0, -4) . ".syx";

#########################################################################################
# Parse .hex file
#########################################################################################

open(IN, "<${in_file}") || die "ERROR: Cannot open ${in_file} for reading!\n";

my $line = 0;
my %dump = ();

my $end_read = 0;
my $address_extension;

while( <IN> )
{
   ++$line;
   s/[\n\r]//g;

   if( !$end_read && substr($_, 0, 1) eq ":" && length($_) >= 11 && length($_)%2 && !/[^:0-9a-f]/i )
   {
      my @record = ();
      my $i;

      for($i=1; $i<length($_); $i+=2)
      {
	 push @record, hex substr($_, $i, 2);
      }

      my $number_bytes = $record[0];
      my $address      = ($record[1] << 8) | $record[2];
      my $record_type  = $record[3];
      my $checksum     = $record[-1];

      if( scalar(@record) != ($number_bytes+5) )
      {
	 die "ERROR: wrong number of bytes in line ${line}\n";
      }

      my $calculated_checksum = 0x00;
      for($i=0; $i<scalar(@record)-1; ++$i)
      {
	 $calculated_checksum = ($calculated_checksum + $record[$i]) & 0xff;
      }
      $calculated_checksum = (-$calculated_checksum) & 0xff;

      if( $debug )
      {
	 printf("%02X %04X %02X ... %02X (%02X)\n", $number_bytes, $address, $record_type, $checksum, $calculated_checksum);
      }

      if( $calculated_checksum != $checksum )
      {
	 die "ERROR: wrong checksum in line ${line}\n";
      }


      if   ( $record_type == 0x00 )
      {
 	 for($i=0; $i<$number_bytes; ++$i)
         {
	    my $a = $address_extension + $address + $i;

	    if( exists $dump{$a} )
	    {
	       die sprintf "ERROR: in line ${line} - address %X already allocated\n", $a;
	    }

            $dump{$a} = $record[4+$i];
	    if( $debug )
	    {
	       printf "[%08X] %02X\n", $a, $dump{$a};
            }
      }
      }
      elsif( $record_type == 0x01 )
      {
 	 $end_read = 1;
      }
      elsif( $record_type == 0x04 )
      {
         if( scalar(@record) != 5+2 )
	 {
	    die "ERROR: for an INHX32 record (0x04) expecting 2 address bytes!\n";
	 }
	 $address_extension = (($record[4] << 8) | $record[5]) << 16;
      }
      else
      {
	 die "ERROR: unknown record type in line ${line}\n";
      }
   }
   elsif( $end_read )
   {
      print "WARNING: line {$line} ignored, since END record has been notified\n";
   }
   elsif( length($_) )
   {
      print "WARNING: invalid line ${line}: '$_'\n";
   }
   else
   {
      print "WARNING: line ${line} is empty.\n";
   }

}

close(IN);

#########################################################################################
# Write out .syx file
#########################################################################################

open(OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";
binmode(OUT);

my $unmapped_address;
my $range = 0;
for($unmapped_address=0; $unmapped_address<0x28000; $unmapped_address+=($range ? $range : 0x400))
{
   my $address;
   my $pic_address = 0;
   $range = 0;
   if( $unmapped_address >= 0x00000 && $unmapped_address <= 0x0ffff )
   { # Flash
      $pic_address = $unmapped_address & 0xffff;
      $address = $pic_address >= 0x8000 ? (($pic_address&0x7fff) | 0x28000) : ($pic_address&0x7fff);
      $range = 0x400;
   }
   elsif( $unmapped_address >= 0x10000 && $unmapped_address <= 0x1ffff )
   { # BankStick
      $pic_address = ($unmapped_address & 0xffff) + 0x400000;
      $address = ($pic_address&0xffff) + 0x10000;
      $range = 0x400;
   }
   elsif( $unmapped_address >= 0x20000 && $unmapped_address < 0x28000 )
   {
      $pic_address = ($unmapped_address & 0x7fff) + 0xf00000;
      $address = ($pic_address&0x7fff) + 0x08000;
      $range = 0x100;
   }
   else { die "Malfunction"; }

   my $block_allocated = 0;
   my @out_dump = ();

   my $offset;
   for($offset=0; $offset<$range; ++$offset)
   {
      my $a = $pic_address + $offset;
      my $b = 0x00;

      if( exists $dump{$a} )
      {
         $block_allocated = 1;
	 $b = $dump{$a};
      }
      else
      {
	 $b = 0xff;
      }
      $out_dump[$offset] = $b;
   }

   if( $block_allocated && ($pic_address < 0x3000) && !$os_upload )
   {
      if( $force )
      {
	 printf("Block %06X-%06X allocated, but skipped by -force option\n", $pic_address, $pic_address+$range-1);
	 $block_allocated = 0;
      }
      else
      {
	 die "ERROR: you are trying to overwrite the MIOS address range\nCheck your main.asm file -- code should be included *after* mios_tables.inc!\nIf it was your intention to convert a MIOS .hex file, add -os_upload!\n";
      }
   }

   if( $block_allocated && ($pic_address >= 0x8000 && $pic_address <= 0xffff) && !$mem_64k )
   {
       die "ERROR: you are trying to allocate code to >= 0x8000!\nIf you are using a 64k PIC device, add -mem_64k!\n";
   }

   if( $block_allocated )
   {
      if( !$range )
      {
	 die sprintf "ERROR: an invalid address range has been selected: %06X-%06X\n", $pic_address, $pic_address+$range-1;
      }

      my $checksum = 0;
      sysex_write_header($device_id, $address, $range, \$checksum);

      my $m = 0x00;
      my $m_counter = 0;
      my $b;
      for($offset=0; $offset<$range; ++$offset)
      {
	 $b = $out_dump[$offset];

	 my $b_counter;
	 for($b_counter=0; $b_counter<8; ++$b_counter)
	 {
            $m = ($m << 1) | ($b & 0x80 ? 0x01 : 0x00);
	    $b = $b << 1;

	    ++$m_counter;
	    if( $m_counter == 7 )
	    {
	       print OUT sprintf("%c", $m);
	       $checksum = ($checksum + $m) & 0xff;
	       $m = 0;
	       $m_counter = 0;
	    }
	 }
      }

      if( $m_counter )
      {
         while( $m_counter < 7 )
   	 {
   	    $m = $m << 1;
	    ++$m_counter;
	 }
	 print OUT sprintf("%c", $m);
	 $checksum = ($checksum + $m) & 0xff;
      }

      $checksum = (-$checksum) & 0x7f;
      sysex_write_footer($checksum);

      printf("Block %06X-%06X allocated - Checksum: %02X\n", $pic_address, $pic_address+$range-1, $checksum);
   }
}

close(OUT);

exit;


#########################################################################################
#
# Subroutines
#
#########################################################################################

sub sysex_write_header
{
   my ($device_id, $address, $counter, $checksum) = @_;

   my $cmd = 0x02;

   # for 64k PICs (use address extension)
   if( $address >= 0x28000 && $address <= 0x2ffff ) 
   {
       $address &= 0x7fff; 
       $cmd |= 0x10;
   }
   elsif( $address >= 0x10000 && $address <= 0x1ffff ) # bankstick selection
   {
       $cmd |= ($bankstick & 0x7) << 4;
   }

   my $ah = ($address >> 10) & 0x7f;
   my $al = ($address >> 3) & 0x7f;
   my $ch = ($counter >> 10) & 0x7f;
   my $cl = ($counter >> 3) & 0x7f;

   print OUT sprintf "%c%c%c%c%c%c%c%c%c%c%c",
      0xf0, 0x00, 0x00, 0x7e, 0x40, $device_id, $cmd, $ah, $al, $ch, $cl;

   $$checksum = ($$checksum + $ah + $al + $ch + $cl) & 0xff;
}

sub sysex_write_footer
{
   my ($checksum) = @_;

   print OUT sprintf "%c%c", $checksum, 0xf7;
}
