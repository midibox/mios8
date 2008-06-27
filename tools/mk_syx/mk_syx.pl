#!/usr/bin/perl -w
# $Id$
#########################################################################################
#
# converts the MIDIbox64/64E/MF .ini file into .syx (SysEx file) which can be 
# transfered to the MIDIbox via MIDI-OX or any other SysEx Loader
#
# Author: tk@midibox.org (2001-2004)
#
# SYNTAX: mk_syx.pl <ini-file> [<syx-file>] [<-device_id id>] [<-debug>]
#
# Example configurations can be found in the same directory like mk_syx.pl
#
#
# DOS/Windows Users: install ActivePerl from the Perl site: http://www.perl.com
# Start the script with (example): "perl mk_syx.pl midibox64.ini"
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

$file_type = 0;             # .ini file type (midibox64e/midibox64)
%pot_settings = ();         # data structure for pots
%enc_modes = ();            # data structure for encoder modes (midibox64e only)
%enc_led_modes = ();        # data structure for encoder modes (midibox64e only)
%led_patterns = ();         # data structure for led patterns (midibox64e only)
%display_strings = ();      # data structure for display strings (MIDIbox64 only)
%button_settings = ();      # data structure for buttons

# MIDIbox64/64E specific configuration flags
$merger_enabled = 1;            # merger enabled?
$pot_behaviour_snapper = 0;     # Snap Mode Active?
$pot_behaviour_relative = 0;    # Relative Mode (Snap must be set)
$pot_behaviour_parallax = 0;    # Parallax Mode (Snap and Relative must be set)
$send_pc_on_bankchange = 0;     # Send Program Change on Bank Changes
$receive_pc_for_bankchange = 0; # Receive Program Change for Bank Changes
$snapshot_at_poweron = 0;       # if snapshot should be sent at poweron or not
$auto_snapshot = 0;             # automatic snapshot on bankchange
$connected_pots = 0;            # MIDIbox64 users are allowed to set the number of connected pots
$bankname = "<unnamed bank>  "; # Bank Name
$touchsensor_sensitivity = 2;   # Touch Sensor Sensitivity
$touchsensor_mode = 1;          # touch sensor mode
$midibox_link = 0;              # MIDIbox Link Flag
%led_map = ();                  # mapped LED assigns
$display_page = 0;              # initial display page
$morph_bank = 0;                # morph bank
$morph_pot_global = 0;          # morph pots
$morph_pot_g1     = 0;          # 
$morph_pot_g2     = 0;          # 
$morph_pot_g3     = 0;          # 
$morph_pot_g4     = 0;          # 
$global_midi_channel   = 0;     # global MIDI channel

#########################################################################################
# Get arguments
#########################################################################################

GetOptions (
   "debug" => \$debug,
   "device_id=s" => \$device_id,
   );

if( scalar(@ARGV) < 1 || scalar(@ARGV) > 2 )
{
   die "SYNTAX:   mk_syx.pl <ini-file> [<syx-file>] [<device-id id>] [<-debug>]\n" .
       "EXAMPLE:  mk_syx.pl midibox64.ini midibox64.syx\n" .
       "SHORTCUT: mk_syx.pl midibox64.ini\n" .
       "SHORTCUT: mk_syx.pl midibox64.ini -device_id 2\n";
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

$number_pots         = 0;
$number_buttons      = 0;
$number_led_sr       = 0;


sub parsing_error { die "ERROR in section ${section}, line ${line}: $_[0]\n"; }

while( <IN> )
{
   s/^\s+//; # strip line
   s/\s+$//;
   $original_line = $_;
   $_=uc($_); # upper case characters
   s/\s*#.*$//;
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

	 if( $file_type eq "MIDIBOX64E" )
	 {
	    $number_pots         = 128;
	    $number_buttons      = 64;
	    $number_led_sr       =  8;
	 }
	 elsif( $file_type eq "MIDIBOX64" )
	 {
	    $number_pots         = 64;
	    $number_buttons      = 64;
	    $number_led_sr       =  8;
	 }
	 else
	 {
	    parsing_error("unknown filetype '${file_type}'");
	 }
      }

      elsif( $section eq "POTS" || $section eq "ENCS" || $section eq "FADERS" )
      {
	 my $pot = 0;
	 my $events = 0;
	 my $min = "00";
	 my $max = "7F";
         my $str = 0;
	 my $str_type = 0;
	 my $str_type_id = 0;
	 my $str_len = 0;

	 if( /^([0-9]*)[\s]*=[\s]*([0-9A-F\s]*)[\s]*\[([0-9A-F]*)-([0-9A-F]*):*([0-9A-F]*)\]/ )
	 {
	    $pot = $1;
	    $events = $2;
	    $min = $3;
	    $max = $4;
	    $value = $5;
	 }
	 else
	 {
	    parsing_error("invalid syntax.");
	 }

	 my $key = "${pot}";

         # check for additional parameters
         if( /[\s]ENC_MODE_([^\s]*)/ )
	 {
	    my $mode_str = $1;
	    my $mode = 0x00;

	    my $speed_str = "NORMAL";
	    if( $mode_str =~ /([^\s]*)&([^\s]*)/ )
	    {
	       $mode_str = $1;
	       $speed_str = $2;
	    }

	    if   ( $mode_str eq "ABSOLUTE"    ) { $mode = 0x00; }
	    elsif( $mode_str eq "40SPEED"     ) { $mode = 0x01; }
	    elsif( $mode_str eq "00SPEED"     ) { $mode = 0x02; }
	    elsif( $mode_str eq "40_1"        ) { $mode = 0x03; }
	    elsif( $mode_str eq "00_1"        ) { $mode = 0x04; }
	    elsif( $mode_str eq "INCDEC"      ) { $mode = 0x05; }
	    elsif( $mode_str eq "LCEMU"       ) { $mode = 0x06; }
	    else
	    {
	       parsing_error("Unknown Encoder Mode: ENC_MODE_${mode_str}");
	    }

	    if   ( $speed_str eq "NORMAL" ) { $mode |= 0x00; }
	    elsif( $speed_str eq "FAST"   ) { $mode |= 0x10; }
	    elsif( $speed_str eq "SLOW"   ) { $mode |= 0x20; }
	    else
	    {
	       parsing_error("Unknown Encoder Speed Mode: END_MODE_${mode_str}\&${speed_str}");
	    }

	    if( $file_type ne "MIDIBOX64E" )
	    {
	       parsing_error("Encoder Modes are only supported by MIDIbox64E!");
	    }
            $enc_modes{$key} = sprintf("%02X", $mode);
	 }

         if( /[\s]LED_PATTERN_([0-9A-F]*)/ )
	 {
	    my $pattern = hex $1;
            if( $pattern < 0 || $pattern > 0x03 )
	    {
	       parsing_error("Invalid LED_PATTERN\nAvailable: LED_PATTERN_0 to LED_PATTERN_3");
	    }
	    if( $file_type ne "MIDIBOX64E" )
	    {
	       parsing_error("LED Patterns are only supported by MIDIbox64E!");
	    }
            $enc_led_modes{$key} = sprintf("%02X", $pattern);
	 }

	 # check for display string
	 if( $original_line =~ /\](.*)/ )
	 {
	    if( $1 =~ /\"(.*)\"[\s]+([\|\w+-]+)$/ )
	    {
	       if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	       {
		  parsing_error("Only MIDIbox64 and MIDIbox64E support display strings.");
	       }
	       $str = $1;
	       $str_type = uc($2);
	       if   ( $str_type eq "HEX"    ) { $str_type_id = 0x40; $str_len = 16-2; }
	       elsif( $str_type eq "+-HEX"  ) { $str_type_id = 0x41; $str_len = 16-3; }
	       elsif( $str_type eq "DEC"    ) { $str_type_id = 0x42; $str_len = 16-3; }
	       elsif( $str_type eq "+-DEC"  ) { $str_type_id = 0x43; $str_len = 16-3; }
	       elsif( $str_type eq "HEX|"   ) { $str_type_id = 0x60; $str_len = 16-2-1; }
	       elsif( $str_type eq "+-HEX|" ) { $str_type_id = 0x61; $str_len = 16-3-1; }
	       elsif( $str_type eq "DEC|"   ) { $str_type_id = 0x62; $str_len = 16-3-1; }
	       elsif( $str_type eq "+-DEC|" ) { $str_type_id = 0x63; $str_len = 16-3-1; }
	       else
	       {
		  parsing_error("Display String type not correct.\nSupported types:\n" .
                  "HEX, +-HEX, DEC, +-DEC, HEX|, +-HEX|, DEC|, +-DEC|");
	       }
	       if( length($str) > $str_len )
	       {
		  parsing_error("Display String too long for ${str_type}\nShould be <= ${str_len}");
	       }
	    }
	    elsif( length($1) )
	    {
	       if( $file_type eq "MIDIBOX64" || $file_type eq "MIDIBOX64E" )
	       {
		  parsing_error("invalid syntax.\nsomething different from a string and type idendifier has been defined after [$min-$max]");
	       }
	       else
	       {
		  parsing_error("invalid syntax.\nNote that only MIDIbox64 and MIDIbox64E support display strings!");
	       }
	    }
	 }

	 $section_got_data = 1;
	 
	 if( $pot < 1 || $pot > $number_pots )
	 {
	    parsing_error("${file_type} only offers ${number_pots} pot/encoder entries!");
	 }

	 $events =~ s/\s//g; # delete blanks
	 if( length($events) < 2 )
	 {
	    parsing_error("'$events' invalid\nExpecting at least 2 hexadecimal digits in the MIDI stream\nEXAMPLE: B0 07");
	 }

	 my $event_1_0 = substr($events, 0, 1);
	 my $event_1_1 = substr($events, 1, 1);
	 my $event_2   = 0;

	 if( $event_1_0 eq "8" )
	 {
	    parsing_error("Note Off not supported. Take Note On (9.) instead!");
	 }
	 elsif( $event_1_0 eq "9" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a Note On event\nEXAMPLE: 90 36");
	    }
	    $event_2 = substr($events, 2, 2);
	 }
	 elsif( $event_1_0 eq "A" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a Poly Aftertouch event\nEXAMPLE: A0 36");
	    }
	    $event_2 = substr($events, 2, 2);
	 }
	 elsif( $event_1_0 eq "B" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a Controller event\nEXAMPLE: B0 07");
	    }
	    $event_2 = substr($events, 2, 2);
	 }
	 elsif( $event_1_0 eq "C" )
	 {
	    if( length($events) != 2 )
	    {
	       parsing_error("'$events' invalid\nExpecting 2 hexadecimal digits for a Program Change event\nEXAMPLE: C0");
	    }
	    $event_2 = 0;
	 }
	 elsif( $event_1_0 eq "D" )
	 {
	    if( length($events) != 2 )
	    {
	       parsing_error("'$events' invalid\nExpecting 2 hexadecimal digits for a Channel Aftertouch event\nEXAMPLE: D0");
	    }
	    $event_2 = 0;
	 }
	 elsif( $event_1_0 eq "E" )
	 {
	    if( length($events) != 2 )
	    {
	       parsing_error("'$events' invalid\nExpecting 2 hexadecimal digits for a Pitch Bender event\nEXAMPLE: E0");
	    }
	    $event_2 = 0;
	 }
	 elsif( $event_1_0 eq "F" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a SysEx event\nEXAMPLE: F0 01");
	    }
	    $event_2 = substr($events, 2, 2);
	 }
	 else
	 {
	    parsing_error("Unsupported MIDI Event\nexpecting 9, A, B, C, D or E at the beginning");
	 }

	 if( hex $min < 0 || hex $min > 0x7f )
	 {
	    parsing_error("${file_type} invalid min value (${min})\nAllowed Range min = 00 to 7F");
	 }
	 if( hex $max < 0 || hex $max > 0x7f )
	 {
	    parsing_error("${file_type} invalid max value (${max})\nAllowed Range min = 00 to 7F");
	 }
	 if( hex $min > hex $max )
	 {
	    parsing_error("${file_type} Min value (${min}) must be less than Max value (${max})");
	 }

	 if( exists $pot_settings{$key} )
	 {
	    parsing_error("${file_type} settings for ${key} already defined!");
	 }

	 my $out;
         if( $file_type eq "MIDIBOX64" || $file_type eq "MIDIBOX64E" )
	 {
            $out = sprintf("#%2d   %X%X %02X [%02X-%02X:%02X]", 
	       $pot, hex $event_1_0, hex $event_1_1, hex $event_2, hex $min, hex $max, hex $value);
	    if( $str_type_id )
	    {
	       $out .= sprintf("   %-15s  %s\n", $str, $str_type);
	    }
	    else
	    {
	       $out .= "   <no display string>\n";
	    }
	 }
	 else
	 {
#            $out = sprintf("POT %2d   %X%X %02X [%02X-%02X:%02X]\n", 
#	       $pot, hex $event_1_0, hex $event_1_1, hex $event_2, hex $min, hex $max, hex $value);
	     $out = "";
	 }
	 if( $debug ) { print $out; }
         $pot_settings{$key} = sprintf("%01X%01X%02X%02X%02X", hex $event_1_0, hex $event_1_1, hex $event_2, hex $min, hex $max);
         $default_values{$key} = sprintf("%02X", hex $value);
         $display_strings{$key} = sprintf("%c%-15s", $str_type_id, $str);
      }

      elsif( $section eq "BUTTONS" )
      {
	 my $button = 0;
	 my $events = 0;
	 my $type = 0;

         if( /^([0-9]*)[\s]*=[\s]*([0-9A-F\s]*)[\s]*\@([A-Z]*)/ )
	 {
	    $button = $1;
	    $events = $2;
	    $type = $3;
	 }
	 else
	 {
	    parsing_error("invalid syntax.");
	 }

	 $section_got_data = 1;
	 
	 if( $button < 1 || $button > $number_buttons )
	 {
	    parsing_error("${file_type} only offers ${number_buttons} buttons!");
	 }

	 $events =~ s/\s//g; # delete blanks
	 if( length($events) < 2 )
	 {
	    parsing_error("'$events' invalid\nExpecting at least 2 hexadecimal digits in the MIDI stream\nEXAMPLE: B0 07");
	 }

	 my $event_1_0 = substr($events, 0, 1);	 
	 my $event_1_1 = substr($events, 1, 1);
	 my $event_2   = substr($events, 2, 2);
	 my $event_3   = 0;

	 if( $event_1_0 eq "8" )
	 {
	    parsing_error("Note Off not supported. Take Note On (9.) instead!");
	 }
	 elsif( $event_1_0 eq "9" )
	 {
	    if( length($events) != 6 )
	    {
	       parsing_error("'$events' invalid\nExpecting 6 hexadecimal digits for a Note On event\nEXAMPLE: 90 36 7F");
	    }
	    $event_3 = substr($events, 4, 2);
	 }
	 elsif( $event_1_0 eq "A" )
	 {
	    if( length($events) != 6 )
	    {
	       parsing_error("'$events' invalid\nExpecting 6 hexadecimal digits for a Poly Aftertouch event\nEXAMPLE: A0 36 7F");
	    }
	    $event_3 = substr($events, 4, 2);
	 }
	 elsif( $event_1_0 eq "B" )
	 {
	    if( length($events) != 6 )
	    {
	       parsing_error("'$events' invalid\nExpecting 6 hexadecimal digits for a Controller event\nEXAMPLE: B0 07 7F");
	    }
	    $event_3 = substr($events, 4, 2);
	 }
	 elsif( $event_1_0 eq "C" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a Program Change event\nEXAMPLE: C0 05");
	    }
	    $event_3 = 0;
	 }
	 elsif( $event_1_0 eq "D" )
	 {
	    if( length($events) != 4 )
	    {
	       parsing_error("'$events' invalid\nExpecting 4 hexadecimal digits for a Channel Aftertouch event\nEXAMPLE: D0 7F");
	    }
	    $event_3 = 0;
	 }
	 elsif( $event_1_0 eq "E" )
	 {
	    if( length($events) != 6 )
	    {
	       parsing_error("'$events' invalid\nExpecting 6 hexadecimal digits for a Pitch Bender event\nEXAMPLE: E0 7F 7F");
	    }
	    $event_3 = substr($events, 4, 2);
	 }
	 elsif( $event_1_0 eq "F" )
	 {
	    if( length($events) != 6 )
	    {
	       parsing_error("'$events' invalid\nExpecting 6 hexadecimal digits for a SysEx event\nEXAMPLE: F0 01 02");
	    }
	    $event_3 = substr($events, 4, 2);
	 }
	 else
	 {
	    parsing_error("Unsupported MIDI Event\nexpecting 9, A, B, C, D or E at the beginning");
	 }

	 my $event_type = 0;
	 if( $type eq "ONOFF" )
	 {
	    $event_type = "00";
	 }
	 elsif( $type eq "ONONLY" )
	 {
	    $event_type = "01";
	 }
	 elsif( $type eq "TOGGLE" )
	 {
	    $event_type = "02";
	 }
	 else
	 {
	    parsing_error("Invalid Button Type.\nAllowed Types. \@OnOff \@OnOnly \@Toggle");
	 }

	 my $key = "${button}";

	 if( exists $button_settings{$key} )
	 {
	    parsing_error("${file_type} settings for ${key} already defined!");
	 }

	 $button_settings{$key} = "${event_1_0}${event_1_1}${event_2}${event_3}${event_type}";
      }

      elsif( $section eq "LED_PATTERNS" )
      {
	 my $pattern = 0;
	 my $step = 0;
	 my $bits = 0;

	 if( $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("LED Patterns are only supported by MIDIbox64E!");
	 }
         if( /^([0-9A-F]*)\.([0-9A-F]*)[\s]*=[\s]*([0-1]*)/ )
	 {
	    $pattern = $1;
	    $step = $2;
	    $bits = $3;
	 }
	 else
	 {
	    parsing_error("invalid syntax.");
	 }

	 if( length($bits) != 16 )
	 {
	    parsing_error("Expecting 16 bits!");
	 }

	 $key = "${pattern}.${step}";
	 if( exists $led_patterns{$key} )
	 {
	    parsing_error("LED pattern for step ${key} already defined!");
	 }
	 my $val = unpack("N", pack("B32", substr("0" x 32 . $bits, -32))); # bin2dec
	 $led_patterns{$key} = sprintf("%04X", $val);
	 $section_got_data = 1;
      }

      elsif( $section eq "MIDI_MERGER" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("With MIDIbox8 and 16, you have to enable the merger in the menu!");
	 }
         if   ( /^ENABLED/   ) { $merger_enabled = 1; $midibox_link = 0; }
	 elsif( /^DISABLED/  ) { $merger_enabled = 0; $midibox_link = 0; }
         elsif( /^MBLINK_FP/ ) { $merger_enabled = 1; $midibox_link = 1; }
	 elsif( /^MBLINK_EP/ ) { $merger_enabled = 0; $midibox_link = 1; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled', 'disabled', 'mblink_fp' or 'mblink_ep'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "POT_BEHAVIOUR" )
      {
	 if( $file_type ne "MIDIBOX64" )
	 {
	    parsing_error("With MIDIbox8 and 16, you have to set the pot behaviour in the menu!");
	 }
         if   ( /^NORMAL/   ) { $pot_behaviour_snapper = 0; $pot_behaviour_relative = 0; $pot_behaviour_parallax = 0; }
         elsif( /^SNAP/     ) { $pot_behaviour_snapper = 1; $pot_behaviour_relative = 0; $pot_behaviour_parallax = 0; }
         elsif( /^RELATIVE/ ) { $pot_behaviour_snapper = 1; $pot_behaviour_relative = 1; $pot_behaviour_parallax = 0; }
         elsif( /^PARALLAX/ ) { $pot_behaviour_snapper = 1; $pot_behaviour_relative = 1; $pot_behaviour_parallax = 1; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'NORMAL', 'SNAP', 'RELATIVE', 'PARALLAX'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "SEND_PC_ON_BANKCHANGE" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("Send PC on Bankchange only supported by MIDIbox64 and MIDIbox64E!");
	 }
         if   ( /^ENABLED/  ) { $send_pc_on_bankchange = 1; }
	 elsif( /^DISABLED/ ) { $send_pc_on_bankchange = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "RECEIVE_PC_FOR_BANKCHANGE" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("Receive PC for Bankchange only supported by MIDIbox64 and MIDIbox64E!");
	 }
         if   ( /^ENABLED/  ) { $receive_pc_for_bankchange = 1; }
	 elsif( /^DISABLED/ ) { $receive_pc_for_bankchange = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "SNAPSHOT_AT_POWERON" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("Snapshot at PowerOn flag only supported by MIDIbox64 and MIDIbox64E!");
	 }
         if   ( /^ENABLED/  ) { $snapshot_at_poweron = 1; }
	 elsif( /^DISABLED/ ) { $snapshot_at_poweron = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "TOUCHSENSOR_SENSITIVITY" )
      {
         if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
         {
	    parsing_error("Touch Sensor Sensitivity only supported by MIDIbox64 and MIDIbox64E!");
	 }

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

      elsif( $section eq "TOUCHSENSOR_MODE" )
      {
         if( $file_type ne "MIDIBOX64E" )
         {
	    parsing_error("Touch Sensor Mode only supported by MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 2 )
	 {
	    parsing_error("Value must be between 0 and 2!");
	 }
	 $touchsensor_mode = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "CONNECTED_POTS" )
      {
	 if( $file_type ne "MIDIBOX64" )
	 {
	    parsing_error("Number of connected pots is only a setting for MIDIbox64!");
	 }
         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 1 || $1 > $number_pots )
	 {
	    parsing_error("Enter a number between 1 and ${number_pots}!");
	 }
	 $section_got_data = 1;
	 $connected_pots = $1;
      }

      elsif( $section eq "AUTO_SNAPSHOT" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("Autosnapshot only supported by MIDIbox64 and MIDIbox64E!");
	 }
         if   ( /^ENABLED/  ) { $auto_snapshot = 1; }
	 elsif( /^DISABLED/ ) { $auto_snapshot = 0; }
	 else                 { parsing_error("invalid syntax.\nexpecting keyword 'enabled' or 'disabled'"); }
	 $section_got_data = 1;
      }

      elsif( $section eq "BANKNAME" )
      {
	 if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	 {
	    parsing_error("Banknames are only supported by MIDIbox64 and MIDIbox64E!");
	 }
         if ( $original_line =~ /^\"(.*)\"$/  ) 
         {
	    my $name = $1;
	    if( length($name) > 16 )
	    {
	       parsing_error("invalid bankname: \"${name}\"\nonly up to 16 characters allowed.");
	    }
	    $bankname = $name;
	 }
	 else
	 { 
	    parsing_error("invalid syntax.\nexpecting \"16 characters\"");
         }
	 $section_got_data = 1;
      }

      elsif( $section eq "DISPLAY_PAGE" )
      {
         if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
         {
	    parsing_error("Display page only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 1 || $1 > 5 )
	 {
	    parsing_error("Value must be between 1 and 5!");
	 }
	 $display_page = $1 - 1;
	 $section_got_data = 1;
      }

      elsif( $section eq "GLOBAL_MIDI_CHANNEL" )
      {
         if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
         {
	    parsing_error("Global MIDI Channel only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 16 )
	 {
	    parsing_error("Value must be between 0 and 16!");
	 }
	 $global_midi_channel = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_BANK" )
      {
         if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
         {
	    parsing_error("Morph Bank only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 1 || $1 > 128 )
	 {
	    parsing_error("Value must be between 1 and 128!");
	 }
	 $morph_bank = $1 - 1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_POT_GLOBAL" )
      {
         if( $file_type ne "MIDIBOX64" )
         {
	    parsing_error("Morphing only supported by MIDIbox64!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 64 )
	 {
	    parsing_error("Value must be between 0 and 64!");
	 }
	 $morph_pot_global = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_POT_G1" )
      {
         if( $file_type ne "MIDIBOX64" )
         {
	    parsing_error("Morphing only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 64 )
	 {
	    parsing_error("Value must be between 0 and 64!");
	 }
	 $morph_pot_g1 = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_POT_G2" )
      {
         if( $file_type ne "MIDIBOX64" )
         {
	    parsing_error("Morphing only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 64 )
	 {
	    parsing_error("Value must be between 0 and 64!");
	 }
	 $morph_pot_g2 = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_POT_G3" )
      {
         if( $file_type ne "MIDIBOX64" )
         {
	    parsing_error("Morphing only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 64 )
	 {
	    parsing_error("Value must be between 0 and 64!");
	 }
	 $morph_pot_g3 = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "MORPH_POT_G4" )
      {
         if( $file_type ne "MIDIBOX64" )
         {
	    parsing_error("Morphing only supported by MIDIbox64 and MIDIbox64E!");
	 }

         if( !/^([0-9]*)$/ )
	 {
	    parsing_error("invalid syntax.");
	 }
	 if( $1 < 0 || $1 > 64 )
	 {
	    parsing_error("Value must be between 0 and 64!");
	 }
	 $morph_pot_g4 = $1;
	 $section_got_data = 1;
      }

      elsif( $section eq "LED_MAP" )
      {
         if( /^\s*LED_SR([0-9]*)\s*=\s*([0-9]*)/ )
	 {
	    my $led_sr = $1;
	    my $mapped_function = $2;

            if( $file_type ne "MIDIBOX64" && $file_type ne "MIDIBOX64E" )
	    {
	       parsing_error("LED Map only supported by MIDIbox64 and MIDIbox64E!");
	    }

	    if( $led_sr < 1 || $led_sr > $number_led_sr )
	    {
	       parsing_error("${file_type} only offers ${number_led_sr} LED shift registers!");
	    }
	    if( $mapped_function < 0 || $mapped_function > 31 )
	    {
	       parsing_error("Mapped function ID #${mapped_function} doesn't exists!");
	    }

	    my $key = sprintf("LED_SR%d", $led_sr);
	    $led_map{$key} = $mapped_function;
	 }
	 else
	 { 
	    parsing_error("invalid syntax.\nexpecting LED_SRx = y");
         }

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
# Write out .syx file
#########################################################################################

$syx_out = "";

if( $file_type eq "MIDIBOX64E" )
{
   # Encoder Events
   $syx_out .= get_pot_settings(1, $number_pots); # from 1 to (max)

   # Button Events
   $syx_out .= get_button_settings(1, $number_buttons); # from 1 to (max)

   # Encoder Modes
   $syx_out .= get_encmode_settings(1, $number_pots); # from 1 to (max)

   # LED patterns
   for($pattern=0; $pattern<4; ++$pattern)
   {
      $syx_out .= get_ledpattern_settings($pattern);
   }

   # snapshot area: fill with default values
   $syx_out .= get_default_values(1, $number_pots); # from 1 to (max)

   $syx_out .= get_2nibbles(0xff);  # buttons
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);

   # store Magic bytes
   $syx_out .= sprintf("%c%c", 0x58, 0x23);

   # store Box Configuration
   $syx_out .= sprintf("%c%c", 
      (($touchsensor_mode&3) << 2) | (($merger_enabled ? 0 : 1) << 0) | (0 << 1),
      0);
   $syx_out .= sprintf("%c%c", 
      ($send_pc_on_bankchange << 0) | ($receive_pc_for_bankchange << 1) | ($midibox_link << 2), 
      ($snapshot_at_poweron << 0) | ($auto_snapshot << 2) );

   # store bank name
   $syx_out .= get_string($bankname, 16);

   # store 2*8 dummy bytes
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);
   $syx_out .= get_2nibbles(0);

   # store touch sensor sensitivity
   $syx_out .= get_2nibbles($touchsensor_sensitivity);

   # store 2*1 dummy bytes
   $syx_out .= get_2nibbles(0);

   # store LED map
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR1"} ? $led_map{"LED_SR1"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR2"} ? $led_map{"LED_SR2"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR3"} ? $led_map{"LED_SR3"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR4"} ? $led_map{"LED_SR4"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR5"} ? $led_map{"LED_SR5"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR6"} ? $led_map{"LED_SR6"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR7"} ? $led_map{"LED_SR7"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR8"} ? $led_map{"LED_SR8"} : 0x00);

   # store display page
   $syx_out .= get_2nibbles($display_page);

   # store morph bank
   $syx_out .= get_2nibbles($morph_bank);

   # store global channel
   $syx_out .= get_2nibbles($global_midi_channel);

   # fill with dummy bytes
   for($i=0; $i<48; ++$i) { $syx_out .= sprintf("%c", 0x00); }
   for($i=0; $i<256; ++$i) { $syx_out .= sprintf("%c", 0x00); }

   # store display strings
   $syx_out .= get_display_settings(1, $number_pots); # from 1 to (max)
}
elsif( $file_type eq "MIDIBOX64" )
{
   $syx_out .= get_pot_settings(1, $number_pots); # from 1 to (max)
   $syx_out .= get_button_settings(1, $number_buttons); # from 1 to 64

   # snapshot area: fill with default values
   $syx_out .= get_default_values(1, $number_pots); # from 1 to (max)

   $syx_out .= get_2nibbles(0xff);  # buttons
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);
   $syx_out .= get_2nibbles(0xff);

   # store Magic bytes
   $syx_out .= sprintf("%c%c", 0x47, 0x11);

   # store Box Configuration
   $syx_out .= sprintf("%c%c", 
      (($merger_enabled ? 0 : 1) << 0) | (0 << 1) | ($pot_behaviour_snapper << 2) | ($pot_behaviour_relative << 3),
      ($pot_behaviour_parallax << 0));
   $syx_out .= sprintf("%c%c", 
      ($send_pc_on_bankchange << 0) | ($receive_pc_for_bankchange << 1) | ($midibox_link << 2), 
      ($snapshot_at_poweron << 0) | ($auto_snapshot << 2) );

   # initial display page
   $syx_out .= get_2nibbles($display_page);

   # morph bank
   $syx_out .= get_2nibbles($morph_bank);

   # morph pots
   $syx_out .= get_2nibbles($morph_pot_global);
   $syx_out .= get_2nibbles($morph_pot_g1);
   $syx_out .= get_2nibbles($morph_pot_g2);
   $syx_out .= get_2nibbles($morph_pot_g3);
   $syx_out .= get_2nibbles($morph_pot_g4);

   # reserved entries
   $syx_out .= get_2nibbles(0x00);
   $syx_out .= get_2nibbles(0x00);

   # store number of connected pots
   $syx_out .= get_2nibbles($connected_pots);

   # store bank name
   $syx_out .= get_string($bankname, 16);

   # touch sensor
   $syx_out .= get_2nibbles($touchsensor_sensitivity);

   # dummy entry
   $syx_out .= get_2nibbles(0x00);

   # LED map
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR1"} ? $led_map{"LED_SR1"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR2"} ? $led_map{"LED_SR2"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR3"} ? $led_map{"LED_SR3"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR4"} ? $led_map{"LED_SR4"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR5"} ? $led_map{"LED_SR5"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR6"} ? $led_map{"LED_SR6"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR7"} ? $led_map{"LED_SR7"} : 0x00);
   $syx_out .= get_2nibbles(exists $led_map{"LED_SR8"} ? $led_map{"LED_SR8"} : 0x00);

   # global MIDI channel
   $syx_out .= get_2nibbles($global_midi_channel);

   # fill with dummy bytes
   for($i=0; $i<112; ++$i) { $syx_out .= sprintf("%c", 0x00); }
   for($i=0; $i<256; ++$i) { $syx_out .= sprintf("%c", 0x00); }

   # store display strings
   $syx_out .= get_display_settings(1, $number_pots); # from 1 to (max)
}
else
{
   die "FATAL: unknown filetype while trying to write out .syx\n";
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

if( $file_type eq "MIDIBOX64E" ) 
{
   $application_id = 0x45;
   if( length($syx_out) != 4096 )
   {
      die sprintf("FATAL: program error - size of syx_out (%d) doesn't match with MB64E dump size (%d)!\n", length($syx_out), 4096);
   }
}
elsif( $file_type eq "MIDIBOX64" ) 
{
   $application_id = 0x43;
   if( length($syx_out) != 2048 )
   {
      die sprintf("FATAL: program error - size of syx_out (%d) doesn't match with MB64E dump size (%d)!\n", length($syx_out), 2048);
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



#########################################################################################
#
# Subroutines
#
#########################################################################################

sub get_pot_settings
{
   my ($pot_begin, $pot_end) = @_;
   my $pot;
   my $ret = "";

   for($pot=$pot_begin; $pot<=$pot_end; ++$pot)
   {
      my $key = "${pot}";
      my $events = exists $pot_settings{$key} ? $pot_settings{$key} : "00000000";
      if( $debug && !exists $pot_settings{$key} ) { printf("WARNING: pot setting for ${key} does not exists\n"); }
      my $byte0 = hex substr($events, 0, 2);
      my $byte1 = hex substr($events, 2, 2);
      my $min   = hex substr($events, 4, 2);
      my $max   = hex substr($events, 6, 2);

      $ret .= sprintf("%c%c", $byte0 & 0x7f, $byte1 & 0x7f);
      $ret .= sprintf("%c%c", $min & 0x7f, $max & 0x7f);
   }

   return $ret;
}

sub get_default_values
{
   my ($pot_begin, $pot_end) = @_;
   my $pot;
   my $ret = "";

   for($pot=$pot_begin; $pot<=$pot_end; ++$pot)
   {
      my $key = "${pot}";
      my $value = exists $default_values{$key} ? $default_values{$key} : "00";
      if( $debug && !exists $default_values{$key} ) { printf("WARNING: pot setting for ${key} does not exists\n"); }

      $ret .= sprintf("%c", hex $value);
   }

   return $ret;
}

sub get_encmode_settings
{
   my ($enc_begin, $enc_end) = @_;
   my $enc;
   my $ret = "";

   for($enc=$enc_begin; $enc<=$enc_end; ++$enc)
   {
      my $key = "${enc}";
      my $enc_mode = exists $enc_modes{$key} ? $enc_modes{$key} : "00";
      my $led_mode = exists $enc_led_modes{$key} ? $enc_led_modes{$key} : "00";
      if( $debug && !exists $enc_modes{$key} ) { printf("WARNING: enc mode setting for ${key} does not exists\n"); }
      if( $debug && !exists $enc_led_modes{$key} ) { printf("WARNING: enc led mode setting for ${key} does not exists\n"); }
      $ret .= sprintf("%c%c", hex substr($enc_mode, 0, 2), hex substr($led_mode, 0, 2));
   }

   return $ret;
}

sub get_ledpattern_settings
{
   my ($pattern) = @_;
   my $step;
   my $ret = "";

   for($step=0; $step<0x20; ++$step)
   {
      my $key = sprintf("%X.%02X", $pattern, $step);
      my $pn = exists $led_patterns{$key} ? $led_patterns{$key} : ($step & 1 ? "AAAA" : "5555");
      my $pn_lsb = hex substr($pn, 0, 2);
      my $pn_msb = hex substr($pn, 2, 2);
      if( $debug && !exists $led_patterns{$key} ) { printf("WARNING: led pattern for ${key} does not exists\n"); }
      $ret .= sprintf("%c%c", $pn_lsb & 0x7f, $pn_lsb & 0x80 ? 1 : 0);
      $ret .= sprintf("%c%c", $pn_msb & 0x7f, $pn_msb & 0x80 ? 1 : 0);
   }

   return $ret;
}

sub get_button_settings
{
   my ($button_begin, $button_end) = @_;
   my $button;
   my $ret = "";

   for($button=$button_begin; $button<=$button_end; ++$button)
   {
      my $key = "${button}";
      my $events = exists $button_settings{$key} ? $button_settings{$key} : "00000000";
      my $byte0 = hex substr($events, 0, 2);
      my $byte1 = hex substr($events, 2, 2);
      my $byte2 = hex substr($events, 4, 2);
      my $mode  = hex substr($events, 6, 2);

      $ret .= sprintf("%c%c%c%c", $byte0 & 0x7f, $byte1 & 0x7f, $byte2 & 0x7f, $mode & 0x7f);
   }

   return $ret;
}

sub get_display_settings
{
   my ($pot_begin, $pot_end) = @_;
   my $pot;
   my $ret = "";

   for($pot=$pot_begin; $pot<=$pot_end; ++$pot)
   {
      my $key = "${pot}";
      my $str = exists $display_strings{$key} ? $display_strings{$key} : 0;
      my $i;

      if( $str ) { for($i=0; $i<16; ++$i) { $ret .= sprintf("%c", (ord substr($str, $i, 1)) & 0x7f); } }
      else       { for($i=0; $i<16; ++$i) { $ret .= sprintf("%c", 0); } }
   }

   return $ret;
}

sub get_string
{
   my ($str, $len) = @_;
   my $ret = "";

   for($i=0; $i<$len; ++$i)
   {
      $ret .= sprintf("%c", ord(substr($str, $i, 1)) & 0x7f);
   }

   return $ret;
}

sub get_2nibbles
{
   my ($value) = @_;

   return sprintf("%c%c", $value & 0xf, ($value >> 4) & 0xf);
}
