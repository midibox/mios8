#!/usr/bin/perl -w
#########################################################################################
#
# Bin-to-ASM converter
# Author: Thorsten.Klose@gmx.de (2004-04-05)
#
# Converts any binary file to an assembler file which can be assembled with MPASM
#
# NOTE: don't mix this script with syx2asm.pl which decodes the MIOS upload format to
# a binary format. 
#
# This script converts a plain binary file w/o decoding!
#
#########################################################################################
#
# SysEx format: none - the plain binary file will be converted to an assembler file
#
#########################################################################################
#
# SYNTAX:  bin2asm.pl <bin-file> <asm-file> [<-debug>]
# EXAMPLE: bin2syx.pl my_binary_file my_binary_file.inc
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): d:\perl\bin\perl bin2asm.pl my_binary_file my_binary_file.inc
# within the DOS command window
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;

$max_file_len = 0x80000;
$address_origin = 0x400000;

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   );

if( scalar(@ARGV) != 2 )
{
   die "SYNTAX:  bin2asm.pl <binary file> <asm-file> [<-debug>]\n" .
       "EXAMPLE: bin2asm.pl my_binary_file my_binary_file.inc\n";
}

$in_file  = $ARGV[0];
$out_file = $ARGV[1];

#########################################################################################
# Get binary file
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


#########################################################################################
# Write out assembler file
#########################################################################################

open(FILE_OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";

printf "Writing %d byte(s) to %s\n", $len, $out_file;

printf FILE_OUT ";; Converted with: 'bin2asm.pl " . join(" ", @ARGV) . "'\r\n";
printf FILE_OUT "\r\n";
printf FILE_OUT "\t;; Delete following line if you include the file into an existing project:\r\n";
printf FILE_OUT "\tLIST\tP=PIC18F452, R=DEC\r\n";
printf FILE_OUT "\t;; suppress \"Address exceeds maximum range for this processor\" message\r\n";
printf FILE_OUT "\tERRORLEVEL\t-220\r\n";
printf FILE_OUT "\r\n";

print FILE_OUT write_bin($dump, $len);

printf FILE_OUT "\r\n\tEND\r\n";

close(FILE_OUT);

exit;

#########################################################################################
#
# Subroutines
#
#########################################################################################

sub write_bin
{
   my ($dump, $len) = @_;
   my $ret = "";
   my @bytes = ();
   my $i;

   $ret .= sprintf "\torg\t0x%06x\r\n", $address_origin;

   for($i=0; $i<$len; ++$i)
   {
      my $byte = ord(substr($dump, $i, 1));
      push @bytes, sprintf("0x%02x", $byte);
      if( scalar(@bytes) == 8 )
      {
         $ret .= sprintf "%s   ;; (0x%06x)\r\n", get_mem_line(@bytes), $address_origin + $i - scalar(@bytes) + 1;
         @bytes = ();
      }
   }

   if( scalar(@bytes) )
   {
      $ret .= sprintf "%s   ;; (0x%06x)\r\n", get_mem_line(@bytes), $address_origin + $i - scalar(@bytes) + 1;
      @bytes = ();
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
