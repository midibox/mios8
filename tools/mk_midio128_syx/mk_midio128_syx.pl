#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# converts a MIDIO128 .ini file into .syx (SysEx file) which can be transfered
# to the MIDIO128 via MIDI-OX or any other SysEx Loader
#
# Author: tk@midibox.org (2001-10-07)
#
# SYNTAX: mk_midio128_syx.pl <ini-file> [<syx-file>] [<-device_id id>] [<-debug>]
#
# See midio128.ini sample for the desired data structure
#
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): "perl mk_midio128_syx.pl midio128.ini"
# within the DOS command window
#
# Use i.e. MIDI-OX to send the .syx file to your MIDIbox. Don't forget to set a
# delay of appr. 750 ms between every SysEx block (after F7) under SysEx->Configure!
#
#########################################################################################

use Getopt::Long;


#########################################################################################
# Global Settings
#########################################################################################

$debug = 0;      # enabled on debug mode
$device_id = 0;  # device ID can be changed from the commandline

$file_type = 0;               # .ini file type (midio128)
%in_settings = ();            # data structure for in streams
%out_settings = ();           # data structure for out streams
%out_types = ();              # data structure for output types
$merger_enabled = 0;          # Merger enabled/disabled
$midibox_link = 0;            # MIDIbox Link enabled/disabled
$enable_alt_progchng = 0;     # Alternative Program Change enabled/disabled
$forward_input_to_output = 0; # Forward Input edges to output pins
$inverse_inputs = 1;          # invert inputs
$inverse_outputs = 0;         # invert outputs
$debounce_time = 20;          # Debounce Time (in ms)
$touchsensor_sensitivity = 3; # Touchsensor Sensitivity
$all_notes_off_channel = 0;   # All Notes Off Channel

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   "device_id=s" => \$device_id,
   );

if( scalar(@ARGV) < 1 || scalar(@ARGV) > 2 )
{
   die "SYNTAX:   mk_midio128_syx.pl <ini-file> [<syx-file>] [<-device_id id>] [<-debug>]\n" .
       "EXAMPLE:  mk_mido128_syx.pl midio128.ini midio128.syx\n" .
       "SHORTCUT: mk_mido128_syx.pl midio128.ini\n" .
       "SHORTCUT: mk_mido128_syx.pl midio128.ini -device_id 2\n";
}

$in_file  = $ARGV[0];

if( scalar(@ARGV) == 2 )
{
   $out_file = $ARGV[1];
}
else
{
   if( lc(substr($in_file, length($in_file)-4)) ne ".ini" )
   {
      die "ERROR: File doesn't end with '.ini'\n";
   }

   $out_file = substr($in_file, 0, -4) . ".syx";
}

$device_id = hex $device_id;
if( $device_id < 0 || $device_id > 7 )
{
   die "ERROR: only device IDs between 0 and 7 are allowed!\n";
}


#########################################################################################
# Output message
#########################################################################################

print "Converting ${in_file} -> ${out_file}\n";


#########################################################################################
# Parse .ini file
#########################################################################################

open(IN, "<${in_file}") || die "ERROR: Cannot open ${in_file} for reading!\n";


$line = 0;
$section = 0;
$section_line = 0;
$section_got_data = 0;

sub parsing_error { die "ERROR in section ${section}, line ${line}: $_[0]\n"; }

while( <IN> )
{
   s/^\s+//; # strip line
   s/\s+$//;
   s/\s*#.*$//;
   $_=uc($_); # upper case characters
   ++$line;

   if( length($_) && !/^#/ )
   {
      if( $debug ) { printf "[LINE %3d] %s\n", $line, $_; }

      if( /^\[([A-Z0-9_]*)\]/ )
      {
	 my $new_section = $1;

         if( $section_line && !$section_got_data )
	 {
	    print "WARNING: Section ${section} at line ${section_line} is empty\n";
	 }
	 $section = $new_section;
	 $section_line = $line;
	 $section_got_data = 0;

      }

      elsif( $section eq "TYPE" )
      {
	 if( $file_type )
	 {
	    parsing_error("filetype already defined ('$file_type')");
	 }

	 $file_type = $_;
	 $section_got_data = 1;

	 if( $file_type ne "MIDIO128" )
	 {
	    parsing_error("unknown filetype '${file_type}'");
	 }
      }

      elsif( $section eq "MIDI_IN" )
      {
         if( !/^([0-9]*)[\s]*=[\s]*([0-9A-F\s]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
         my $n = $1;
	 my $events = $2;
	 $section_got_data = 1;
	 
	 if( $n < 1 || $n > 128 )
	 {
	    parsing_error("${file_type} only offers 128 inputs!");
	 }

	 $events =~ s/\s//g; # delete blanks
	 if( length($events) < 2 )
	 {
	    parsing_error("'$events' invalid\nExpecting at least 2 hexadecimal digits in the MIDI stream\nEXAMPLE: 90 30");
	 }
         if( length($events) != 4 )
	 {
	    parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits\nEXAMPLE: 90 30");
	 }

	 my $key = $n;

	 if( exists $in_settings{$key} )
	 {
	    parsing_error("${file_type} IN settings for ${key} already defined!");
	 }

	 $in_settings{$key} = $events;
      }

      elsif( $section eq "MIDI_OUT" )
      {
         if( !/^([0-9]*)[\s]*=[\s]*([0-9A-F\s]*)[\s]*\@([A-Z]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
         my $n = $1;
	 my $events = $2;
	 my $type = $3;
	 $section_got_data = 1;
	 
	 if( $n < 1 || $n > 128 )
	 {
	    parsing_error("${file_type} only offers 128 inputs!");
	 }

	 $events =~ s/\s//g; # delete blanks
         if( length($events) != 12 )
	 {
	    parsing_error("'$events' invalid\nExpecting 12 hexadecimal digits\nEXAMPLE: 90 30 7F 90 30 00");
	 }

	 my $event_1_0 = substr($events, 0, 1);
	 my $event_1_1 = substr($events, 1, 1);
	 my $event_2 = substr($events, 2, 2);

	 my $event_type = 0;
	 if( $type eq "ONOFF" )
	 {
	    $event_type = 0x00;
	 }
	 elsif( $type eq "ONONLY" )
	 {
	    $event_type = 0x01;
	 }
	 elsif( $type eq "TOGGLE" )
	 {
	    $event_type = 0x02;
	 }
	 else
	 {
	    parsing_error("Invalid Input Type.\nAllowed Types. \@OnOff \@OnOnly \@Toggle");
	 }

	 my $key = "${n}";

	 if( exists $out_settings{$key} )
	 {
	    parsing_error("${file_type} OUT settings for ${key} already defined!");
	 }

	 $out_settings{$key} = $events;;
	 $out_types{$key} = $event_type;
      }

      elsif( $section eq "MERGER" || $section eq "MIDI_MERGER" )
      {
         if   ( /^ENABLED/   ) { $merger_enabled = 1; $midibox_link = 0; }
	 elsif( /^DISABLED/  ) { $merger_enabled = 0; $midibox_link = 0; }
         elsif( /^MBLINK_FP/ ) { $merger_enabled = 1; $midibox_link = 1; }
	 elsif( /^MBLINK_EP/ ) { $merger_enabled = 0; $midibox_link = 1; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled', 'disabled', 'mblink_fp' or 'mblink_ep'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "FORWARD_INPUT_TO_OUTPUT" )
      {
         if   ( /^ENABLED/  ) { $forward_input_to_output = 1; }
	 elsif( /^DISABLED/ ) { $forward_input_to_output = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "INVERSE_INPUTS" )
      {
         if   ( /^ENABLED/  ) { $inverse_inputs = 1; }
	 elsif( /^DISABLED/ ) { $inverse_inputs = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "INVERSE_OUTPUTS" )
      {
         if   ( /^ENABLED/  ) { $inverse_outputs = 1; }
	 elsif( /^DISABLED/ ) { $inverse_outputs = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "DEBOUNCE_TIME" )
      {
         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 255 )
	 {
	    parsing_error("invalid syntax.\nAllowed Values: 0 to 255");
	 }
	 $section_got_data = 1;
	 $debounce_time = $1;
      }

      elsif( $section eq "TOUCHSENSOR_SENSITIVITY" )
      {
         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 1 || $1 > 32 )
	 {
	    parsing_error("Value must be between 1 and 32!");
	 }
	 $touchsensor_sensitivity = $1 - 1;
	 $section_got_data = 1;
      }

      elsif( $section eq "ALL_NOTES_OFF_CHANNEL" )
      {
         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 > 16 )
	 {
	    parsing_error("Value must be between 0 and 16!");
	 }
	 $all_notes_off_channel = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "ALT_PROGCHNG_BEHAVIOUR" )
      {
         if   ( /^ENABLED/  ) { $enable_alt_progchng = 1; }
	 elsif( /^DISABLED/ ) { $enable_alt_progchng = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      else
      {
	 parsing_error("don't know what to do with statement '$_'!\n");
      }
   }
}

if( $section_line && !$section_got_data )
{
   print "WARNING: Section ${section} at line ${section_line} is empty\n";
}

close(IN);


#########################################################################################
# Create the SysEx dump
#########################################################################################

my $syx_out = "";

if( $file_type eq "MIDIO128" )
{
   for($n=1; $n<=128; ++$n)
   {
      my $events = exists $in_settings{$n} ? $in_settings{$n} : "FFFF";
      if( $debug && !exists $in_settings{$n} ) { printf("WARNING: IN setting for ${n} does not exists\n"); }
      my $byte0 = hex substr($events, 0, 2);
      my $byte1 = hex substr($events, 2, 2);
      $syx_out .= sprintf("%c%c", $byte0 & 0x7f, $byte1 & 0x7f);
   }

   for($n=1; $n<=128; ++$n)
   {
      my $events = exists $out_settings{$n} ? $out_settings{$n} : "FFFFFFFFFFFF";
      if( $debug && !exists $out_settings{$n} ) { printf("WARNING: OUT setting for ${n} does not exists\n"); }
      my $byte0 = hex substr($events,  0, 2);
      my $byte1 = hex substr($events,  2, 2);
      my $byte2 = hex substr($events,  4, 2);
      my $byte3 = hex substr($events,  6, 2);
      my $byte4 = hex substr($events,  8, 2);
      my $byte5 = hex substr($events, 10, 2);
      $syx_out .= sprintf("%c%c%c%c%c%c", 
         $byte0 & 0x7f, $byte1 & 0x7f, 
         $byte2 & 0x7f, $byte3 & 0x7f, 
         $byte4 & 0x7f, $byte5 & 0x7f);
   }

   # store Merger Enable Flag
   $syx_out .= sprintf("%c%c", ($midibox_link << 1) | $merger_enabled, 0);

   # store Debounce Time
   $syx_out .= sprintf("%c%c", $debounce_time & 0xf, ($debounce_time >> 4) & 0xf);

   # store Alternative Program Change Enable
   $syx_out .= sprintf("%c%c", $enable_alt_progchng & 0x7f, 0);

   # store Forward Input to Output Enable
   $syx_out .= sprintf("%c%c", $forward_input_to_output & 0x7f, 0);

   # store Inverse Input Enable
   $syx_out .= sprintf("%c%c", $inverse_inputs & 0x7f, 0);

   # store Inverse Output Enable
   $syx_out .= sprintf("%c%c", $inverse_outputs & 0x7f, 0);

   # store Touch Sensor Sensitivity
   $syx_out .= sprintf("%c%c", $touchsensor_sensitivity & 0xf, ($touchsensor_sensitivity >> 4) & 0xf);

   # dummy (reserved for Global Channel)
   $syx_out .= sprintf("%c%c", 0x00, 0x00);

   # All Notes Off Channel
   $syx_out .= sprintf("%c%c", $all_notes_off_channel & 0xf, ($all_notes_off_channel >> 4) & 0xf);

   # 7 dummy bytes
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);
   $syx_out .= sprintf("%c%c", 0x00, 0x00);

   # 128 out types (behaviour of inputs: OnOff/OnOnly/Toggle) - packed in 4 nibbles
   for($n=0; $n<32; ++$n)
   {
      my $entry0 = exists $out_types{4*$n+1} ? $out_types{4*$n+1} : 0;
      my $entry1 = exists $out_types{4*$n+2} ? $out_types{4*$n+2} : 0;
      my $entry2 = exists $out_types{4*$n+3} ? $out_types{4*$n+3} : 0;
      my $entry3 = exists $out_types{4*$n+4} ? $out_types{4*$n+4} : 0;
      my $byte0  = ($entry1 << 4) | $entry0;
      my $byte1  = ($entry3 << 4) | $entry2;
      $syx_out .= sprintf("%c%c", $byte0 & 0x7f, $byte1 & 0x7f);
   }

   # fill with dummy bytes
   for($i=0; $i<512-96; ++$i) { $syx_out .= sprintf("%c", 0x7f); }
}
else
{
   die "INTERNAL DESASTER!!!\n";
}


#########################################################################################
# Write out .syx file
#########################################################################################
open(OUT, ">${out_file}") || die "ERROR: Cannot open ${out_file} for writing!\n";
binmode(OUT);

if( length($syx_out) % 256 )
{
   die "FATAL: program error - syx_out not dividable by 256!\n";
}

$number_of_blocks = length($syx_out) / 256;
$application_id = 0x00;

if( $file_type eq "MIDIO128" ) 
{
   $application_id = 0x44;
   if( length($syx_out) != 1536 )
   {
      die sprintf("FATAL: program error - size of syx_out (%d) doesn't match with MIDIO128 dump size (%d)!\n", length($syx_out), 1536);
   }
}
else
{
   die "FATAL: unknown filetype while trying to write out .syx\n";
}

for($block=0; $block<$number_of_blocks; ++$block)
{
   my $checksum;
   my $i;
   my $byte;
   print OUT sprintf("%c%c%c%c%c%c%c", 0xf0, 0x00, 0x00, 0x7e, $application_id, (($device_id&7) << 4) | 0x04, $block);

   $checksum = $block;
   for($i=0; $i<256; ++$i)
   {
      $byte = ord(substr($syx_out, $block*256 + $i, 1));
      $checksum = ($checksum + $byte) & 0xff;
      print OUT sprintf("%c", $byte);
   }
   print OUT sprintf("%c", (-$checksum) & 0x7f);

   print OUT sprintf("%c", 0xf7);
}


close(OUT);

# the end
exit;

