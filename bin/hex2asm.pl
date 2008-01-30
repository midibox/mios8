#!/usr/bin/perl -w
#########################################################################################
#
# Hex-to-ASM converter
# Author: Thorsten.Klose@gmx.de (2006-04-03)
#
# Converts a INHX32 file to a Microchip assembly file which can be included into
# a MIOS application
#
#########################################################################################
#
# SYNTAX:  hex2asm.pl <hex-file> <asm-file> [-start <start-addr>] [-end <end-addr>]
# EXAMPLE: hex2asm.pl main.hex code.inc
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;
$start_addr = "0x000000";
$end_addr   = "0x010000";

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   "start_addr=s" => \$start_addr,
   "end_addr=s" => \$end_addr,
   );

if( scalar(@ARGV) != 2 ) {
  die "SYNTAX:  hex2asm.pl <hex-file> <asm-file> [-start <start-addr>] [-end <end-addr>]\n" .
      "EXAMPLE: hex2asm.pl main.hex code.inc\n";
}

$in_file  = $ARGV[0];
$out_file = $ARGV[1];
$start_addr = hex $start_addr;
$end_addr = hex $end_addr;

if( lc(substr($in_file, length($in_file)-4)) ne ".hex" )
{
   die "ERROR: File doesn't end with '.hex'\n";
}

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
# determine start and end location
#########################################################################################

my $addr;
my $found_start = -1;
my $found_end = -1;

for($addr=$start_addr; $addr<=$end_addr; $addr+=1) {
  if( exists $dump{$addr} ) {
    if( $found_start == -1 ) {
      $found_start = $addr;
    }
    $found_end = $addr;
  }
}

if( $found_start == -1 ) {
  die sprintf("ERROR: no code found between 0x%06x and 0x%06x\n", $start_addr, $end_addr);
}

#########################################################################################
# Write out ASM file
#########################################################################################

printf "Found %d bytes (range: 0x%06x-0x%06x), writing dump to '${out_file}'\n", $found_end-$found_start+1, $found_start, $found_end;

if( $found_start & 0x3f ) {
  $found_start &= ~0x3f;
  printf("Start address aligned to 64 byte, now: 0x%06x\n", $found_start);
}

if( $found_end & 0x3f ) {
  $found_end = ($found_end & ~0x3f) + 0x3f;
  printf("End address aligned to 64 byte, now: 0x%06x\n", $found_end);
}

open(OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";

printf OUT ";; Converted with: 'hex2asm.pl " . join(" ", @ARGV) . "'\r\n";
printf OUT "\r\n";
printf OUT "\torg\t0x%06x\r\n", $found_start;

for($addr=$found_start; $addr<=$found_end; $addr += 8) {
  my @bytes = ();

  for($offset=0; $offset<8; ++$offset) {
    my $a = $addr + $offset;
    push @bytes, sprintf("0x%02x", exists $dump{$a} ? $dump{$a} : 0xff);
  }

  printf OUT "\tdb\t%s   ;; (0x%06x)\r\n", join(", ", @bytes), $addr;  
}

close(OUT);

print "done.\n";
