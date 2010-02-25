#!/usr/bin/perl -w
#
# generates the MIOS documentation from the source files
# Thorsten Klose (2003-02-23)
#
# SYNTAX: mios_doc.pl <file(s)>
# EXAMPLE: ./mios_doc.pl ../src/*.inc


%functions = ();
@function_names = ();

$outfile = "mios_fun.html.gen";
$user_outfile = "./mios_fun.html";
$c_outfile = "cmios_fun.html.gen";
$c_user_outfile = "./cmios_fun.html";


#$vectorsfile = "./mios_vectors.inc";
$vectorsfile = 0;
$vectorsfile2 = "./mios__vectors.inc";
$vector_addr = 0x2c00;

@classes = (
   "MIOS_MIDI", "MIOS_MPROC",
   "MIOS_AIN", "MIOS_MF", "MIOS_DIN", "MIOS_DOUT", "MIOS_ENC", "MIOS_SRIO",
   "MIOS_LCD", "MIOS_CLCD", "MIOS_GLCD",
   "MIOS_EEPROM", "MIOS_FLASH", "MIOS_BANKSTICK", "MIOS_IIC", 
   "MIOS_TIMER", "MIOS_HLP", "MIOS", "USER",
   );

#########################################################################################
# Main Program
#########################################################################################


if( !scalar(@ARGV) )
{
   die "SYNTAX: mios_doc.pl <source-file(s)>\n";
}

while( scalar(@ARGV) )
{
   my $infile = pop @ARGV;

   if( substr($infile, -4) ne ".inc" )
   {
      die "ERROR: ${infile} is no .inc file!\n";
   }

   open(IN, "<${infile}") || die "ERROR: Cannot open ${infile} for input!\n";

   print "scanning ${infile}\n";

   my $line = 0;
   my $state = "SCAN_FUNCTION";
   my %found = ();
   my $substate = 0;
   while( <IN> )
   {
      ++$line;
      s/^\s*//g;
      s/\s*$//g;

      if( $state eq "SCAN_FUNCTION" )
      {
         if( /^;;\s*FUNCTION:\s*(.*)/ )
	 {
	    %found = ();
	    push @function_names, $1;
	    $substate = 0;
	    $found{FUNCTION} = $1;
	    print "Found function: $found{FUNCTION}\n";
	    $state = "SCAN_KEYS";
	 }
      }
      elsif( $state eq "SCAN_KEYS" )
      {
	 if( /^;;\s*DESCRIPTION\s*:\s*(.*)/ )
	 {
	    $substate = "DESCRIPTION";
	    $found{$substate} = $1;
	 } 
	 elsif( /^;;\s*C_DECLARATION\s*:\s*(.*)/ )
	 {
	    $substate = "C_DECLARATION";
	    $found{$substate} = $1;
	 } 
	 elsif( /^;;\s*IN\s*:\s*(.*)/ )
	 {
	    $substate = "IN";
	    $found{$substate} = conv_ltgt($1);
	 }
	 elsif( /^;;\s*C_IN\s*:\s*(.*)/ )
	 {
	    $substate = "C_IN";
	    $found{$substate} = conv_ltgt($1);
	 }
	 elsif( /^;;\s*OUT\s*:\s*(.*)/ )
	 {
	    $substate = "OUT";
	    $found{$substate} = conv_ltgt($1);
	 }
	 elsif( /^;;\s*C_OUT\s*:\s*(.*)/ )
	 {
	    $substate = "C_OUT";
	    $found{$substate} = conv_ltgt($1);
	 }
	 elsif( /^;;\s*NOTE\s*:\s*(.*)/ )
	 {
	    $substate = "NOTE";
	    $found{$substate} = $1;
	 }
	 elsif( /^;;\s*USES\s*:\s*(.*)/ )
	 {
	    $substate = "USES";
	    $found{$substate} = $1;
	 }
	 elsif( /^;;\s*EXAMPLE[S]*\s*:\s*(.*)/ )
	 {
	    $substate = "EXAMPLE";
	    $found{$substate} = "$1\n";
	 }
	 elsif( /^;;\s*C_EXAMPLE[S]*\s*:\s*(.*)/ )
	 {
	    $substate = "C_EXAMPLE";
	    $found{$substate} = "$1\n";
	 }
	 elsif( /^;;\s*ISR\s*:\s*(.*)/ )
	 {
	    $substate = "ISR";
	    $found{$substate} = $1;
	 }
	 elsif( /^;;\s*-------------------------------------------------/ )
	 {
	    my $key = $found{FUNCTION};

	    $functions{"${key}_FUNCTION"} = $key;
	    $functions{"${key}_DESCRIPTION"} = exists $found{DESCRIPTION} ? $found{DESCRIPTION} : 0;
	    $functions{"${key}_C_DECLARATION"} = exists $found{C_DECLARATION} ? $found{C_DECLARATION} : 0;
	    $functions{"${key}_IN"} = exists $found{IN} ? $found{IN} : 0;
	    $functions{"${key}_C_IN"} = exists $found{C_IN} ? $found{C_IN} : 0;
	    $functions{"${key}_OUT"} = exists $found{OUT} ? $found{OUT} : 0;
	    $functions{"${key}_C_OUT"} = exists $found{C_OUT} ? $found{C_OUT} : 0;
	    $functions{"${key}_NOTE"} = exists $found{NOTE} ? $found{NOTE} : 0;
	    $functions{"${key}_USES"} = exists $found{USES} ? $found{USES} : 0;
	    $functions{"${key}_EXAMPLE"} = exists $found{EXAMPLE} ? $found{EXAMPLE} : 0;
	    $functions{"${key}_C_EXAMPLE"} = exists $found{C_EXAMPLE} ? $found{C_EXAMPLE} : 0;
	    $functions{"${key}_ISR"} = exists $found{ISR} ? $found{ISR} : 0;
	    $substate = 0;
	    $state = "SCAN_FUNCTION";
	 }
	 elsif( /^;;(.*)/ )
	 {
	    if( $substate eq "EXAMPLE" )
	    {
 	       my $str = $1;
 	       if( substr($str, 0, 6) eq "      " ) { $str = "  $str"; }
	       $str =~ s/\t/        /g;
	       $found{$substate} .= conv_ltgt($str) . "\n";
	    } elsif( $substate eq "C_EXAMPLE" ) {
 	       my $str = $1;
 	       if( substr($str, 0, 6) eq "      " ) { $str = "  $str"; }
	       $str =~ s/\t/  /g;
	       $found{$substate} .= conv_ltgt($str) . "\n";
	    }
	    elsif(  $substate eq "IN" || $substate eq "OUT" || $substate eq "C_IN" || $substate eq "C_OUT" )
	    {
	       $found{$substate} .= "<BR>" . conv_ltgt($1);
	    }
	    else
	    {
	       $found{$substate} .= " $1";
	    }
	 }
	 else
	 {
	    die "Invalid line:\n$_";
	 }
      }
      else
      {
         die "unknown state: ${state}\n";
      }
   }

   if( $substate )
   {
      die "ERROR: substate already active - Function: $found{FUNCTION}\n";
   }

   close(IN);
}

printf "%d functions found\n", scalar(@function_names);

%class_functions = ();

foreach $funname (@function_names)
{
   my $found = 0;
   foreach $class (@classes)
   {
      if( !$found && substr($funname, 0, length($class)) eq $class )
      {
	 push @{$class_functions{$class}}, $funname;
	 $found = 1;
      }
   }
   if( !$found )
   {
      die "Unknown class: ${funname}\n";
   }
}

if( $vectorsfile )
{
   open(OUT_VECTORS, ">${vectorsfile}") || die "ERROR: Cannot open ${vectorsfile} for output!\n";
}
if( $vectorsfile2 )
{
   open(OUT_VECTORS2, ">${vectorsfile2}") || die "ERROR: Cannot open ${vectorsfile2} for output!\n";
   print OUT_VECTORS2 sprintf "\torg    0x%04x\r\n", $vector_addr;
}
open(OUT, ">${outfile}") || die "ERROR: Cannot open ${outfile} for output!\n";
open(C_OUT, ">${c_outfile}") || die "ERROR: Cannot open ${c_outfile} for output!\n";
open(OUT_USER, ">${user_outfile}") || die "ERROR: Cannot open ${user_outfile} for output!\n";
open(C_OUT_USER, ">${c_user_outfile}") || die "ERROR: Cannot open ${c_user_outfile} for output!\n";

print OUT "HEADER 3 MIOS Functions Reference (ASM Version)\n";
print OUT "<A NAME=\"top\">\n";
print OUT "<CENTER><H1>MIOS Functions Reference (ASM Version)</H1></CENTER>\n";
print OUT "<IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";
print OUT "<P CLASS=DESC><A HREF=\"" . substr($c_outfile, 0, -4) . "\">Click here for C version</A>\n";
print OUT "<IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";
print OUT "<UL CLASS=CL>\n";

print C_OUT "HEADER 3 MIOS Functions Reference (C Version)\n";
print C_OUT "<A NAME=\"top\">\n";
print C_OUT "<CENTER><H1>MIOS Functions Reference (C Version)</H1></CENTER>\n";
print C_OUT "<IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";
print C_OUT "<P CLASS=DESC><A HREF=\"" . substr($outfile, 0, -4) . "\">Click here for ASM version</A>\n";
print C_OUT "<IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";
print C_OUT "<UL CLASS=CL>\n";


print OUT_USER get_header("MIOS Functions Reference (ASM Version)");
print OUT_USER "<A NAME=\"top\">\n";
print OUT_USER "<CENTER><H1>MIOS Functions Reference (ASM Version)</H1></CENTER>\n";
print OUT_USER "<UL>\n";

print C_OUT_USER get_header("MIOS Functions Reference (C Version)");
print C_OUT_USER "<A NAME=\"top\">\n";
print C_OUT_USER "<CENTER><H1>MIOS Functions Reference (C Version)</H1></CENTER>\n";
print C_OUT_USER "<UL>\n";

my $fun_description = "";
my $c_fun_description = "";
my $fun_description_user = "";
my $c_fun_description_user = "";

$processed = 0;
foreach $class (@classes)
{
   print OUT "  <LI>Class: <B>${class}</B>\n";
   print OUT "    <UL>\n";

   print C_OUT "  <LI>Class: <B>${class}</B>\n";
   print C_OUT "    <UL>\n";

   print OUT_USER "  <LI>Class: <B>${class}</B>\n";
   print OUT_USER "    <UL>\n";

   print C_OUT_USER "  <LI>Class: <B>${class}</B>\n";
   print C_OUT_USER "    <UL>\n";

   foreach $funname (sort @{$class_functions{$class}})
   {
      ++$processed;

      print OUT "      <LI><A NAME=\"back_${funname}\"><A HREF=\"#${funname}\">${funname}</A></LI>\n";
      print C_OUT "      <LI><A NAME=\"back_${funname}\"><A HREF=\"#${funname}\">${funname}</A></LI>\n";
      print OUT_USER "      <LI><A NAME=\"back_${funname}\"><A HREF=\"#${funname}\">${funname}</A></LI>\n";
      print C_OUT_USER "      <LI><A NAME=\"back_${funname}\"><A HREF=\"#${funname}\">${funname}</A></LI>\n";

      if( exists $descriptions{$funname} )
      {
	 die "ERROR: description for ${funname} already exists!\n";
      }
      $descriptions{$funname} = 1;

      ###################################################################################################################
      $fun_description .= "<HR><A NAME=\"${funname}\"></A>\n";
      $fun_description .= "<TABLE WIDTH=\"100%\" BORDER=0 CELLSPACING=5 CELLPADDING=0>\n";
      $fun_description .= "  <TR>\n";
      $fun_description .= "    <TD COLSPAN=2 CLASS=TABCOLOR1><FONT SIZE=3><STRONG>${funname}</STRONG></FONT></TD>\n";
      $fun_description .= "  </TR>\n";

      $fun_description .= function_entry($funname, "FUNCTION");
      $fun_description .= function_entry($funname, "DESCRIPTION");
      $fun_description .= function_entry($funname, "IN");
      $fun_description .= function_entry($funname, "OUT");
      $fun_description .= function_entry($funname, "USES");
      $fun_description .= function_entry($funname, "ISR");
      $fun_description .= function_entry($funname, "NOTE");
      $fun_description .= function_entry($funname, "EXAMPLE");

      $fun_description .= function_back_reference($funname);
      $fun_description .= "</TABLE>\n";


      ###################################################################################################################
      $c_fun_description .= "<HR><A NAME=\"${funname}\"></A>\n";
      $c_fun_description .= "<TABLE WIDTH=\"100%\" BORDER=0 CELLSPACING=5 CELLPADDING=0>\n";
      $c_fun_description .= "  <TR>\n";
      $c_fun_description .= "    <TD COLSPAN=2 CLASS=TABCOLOR1><FONT SIZE=3><STRONG>${funname}</STRONG></FONT></TD>\n";
      $c_fun_description .= "  </TR>\n";

      $c_fun_description .= function_entry($funname, "C_DECLARATION");
      $c_fun_description .= function_entry($funname, "DESCRIPTION");
      $c_fun_description .= function_entry($funname, "C_IN");
      $c_fun_description .= function_entry($funname, "C_OUT");
      $c_fun_description .= function_entry($funname, "ISR");
      $c_fun_description .= function_entry($funname, "NOTE");
      $c_fun_description .= function_entry($funname, "C_EXAMPLE");

      $c_fun_description .= function_back_reference($funname);
      $c_fun_description .= "</TABLE>\n";


      ###################################################################################################################
      $fun_description_user .= "<HR><A NAME=\"${funname}\"></A>\n";

      $fun_description_user .= "<TABLE WIDTH=\"100%\" BORDER=0 CELLSPACING=5 CELLPADDING=0>\n";

      $fun_description_user .= function_entry($funname, "FUNCTION");
      $fun_description_user .= function_entry($funname, "DESCRIPTION");
      $fun_description_user .= function_entry($funname, "IN");
      $fun_description_user .= function_entry($funname, "OUT");
      $fun_description_user .= function_entry($funname, "USES");
      $fun_description_user .= function_entry($funname, "ISR");
      $fun_description_user .= function_entry($funname, "NOTE");
      $fun_description_user .= function_entry($funname, "EXAMPLE");

      $fun_description_user .= function_back_reference($funname);
      $fun_description_user .= "</TABLE>\n";


      ###################################################################################################################
      $c_fun_description_user .= "<HR><A NAME=\"${funname}\"></A>\n";

      $c_fun_description_user .= "<TABLE WIDTH=\"100%\" BORDER=0 CELLSPACING=5 CELLPADDING=0>\n";

      $c_fun_description_user .= function_entry($funname, "C_DECLARATION");
      $c_fun_description_user .= function_entry($funname, "DESCRIPTION");
      $c_fun_description_user .= function_entry($funname, "C_IN");
      $c_fun_description_user .= function_entry($funname, "C_OUT");
      $c_fun_description_user .= function_entry($funname, "ISR");
      $c_fun_description_user .= function_entry($funname, "NOTE");
      $c_fun_description_user .= function_entry($funname, "C_EXAMPLE");

      $c_fun_description_user .= function_back_reference($funname);
      $c_fun_description_user .= "</TABLE>\n";


      ###################################################################################################################

      if( $vectorsfile )
      {
         print OUT_VECTORS sprintf "%-40sEQU\t0x%04x\r\n", $funname, $vector_addr;
         print OUT_VECTORS2 sprintf "%-40sgoto\t%s\r\n", "_${funname}", $funname;
	 $vector_addr += 4;
      }
   }

   print OUT "    </UL>\n";
   print OUT "  </LI>\n";
   print OUT "  <IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";

   print C_OUT "    </UL>\n";
   print C_OUT "  </LI>\n";
   print C_OUT "  <IMG SRC=\"images/1x1dot.gif\" height=20 ALT=\"\"><BR>\n";

   print OUT_USER "    </UL>\n";
   print OUT_USER "  </LI>\n";
   print OUT_USER "  &nbsp;<BR>\n";

   print C_OUT_USER "    </UL>\n";
   print C_OUT_USER "  </LI>\n";
   print C_OUT_USER "  &nbsp;<BR>\n";
}

print OUT "</UL>\n";
print OUT $fun_description;
print OUT "FOOTER\n";

print C_OUT "</UL>\n";
print C_OUT $c_fun_description;
print C_OUT "FOOTER\n";

print OUT_USER "</UL>\n";
print OUT_USER $fun_description_user;
print OUT_USER get_footer();

print C_OUT_USER "</UL>\n";
print C_OUT_USER $c_fun_description_user;
print C_OUT_USER get_footer();

close (OUT);
close (C_OUT);
close (OUT_USER);
close (C_OUT_USER);

if( $vectorsfile )
{
   while( $vector_addr < 0x3000 )
   {
      print OUT_VECTORS sprintf "%-40sEQU\t0x%04x\r\n", sprintf("MIOS_RESERVED_%d", ($vector_addr & 0x3ff) >> 2), $vector_addr;      
      print OUT_VECTORS2 sprintf "%-40sreturn\r\n", sprintf("MIOS_RESERVED_%d", ($vector_addr & 0x3ff) >> 2);      
      $vector_addr += 4; 
   }

   close(OUT_VECTORS);
}

printf "%d functions processed\n", $processed;

exit;

sub function_entry
{
   my ($funname, $key) = @_;
   my $con;
   my $ret = "";

   if( $con = $functions{"${funname}_${key}"} )
   {
      if( $key eq "EXAMPLE" || $key eq "C_EXAMPLE" )
      {
	 $con = "<PRE>${con}</PRE>";
      }

      $ret .= "  <TR>\n";
      $ret .= "    <TD CLASS=TABCOLOR2 WIDTH=150><B>${key}</B></TD>\n";
      $ret .= "    <TD CLASS=TABCOLOR2>${con}</TD>\n";
      $ret .= "  </TR>\n";
   }

   return $ret;
}

sub function_back_reference
{
   my ($funname) = @_;
   my $ret = "";

   $ret .= "  <TR>\n";
   $ret .= "    <TD COLSPAN=2 CLASS=TABCOLOR2 WIDTH=150><A HREF=\"#back_${funname}\">Back to the top</A></TD>\n";
   $ret .= "  </TR>\n";

   return $ret;
}

sub get_header
{
   my ($title) = @_;
   my $ret = "";

   $ret .= "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
   $ret .= "<HTML>\n";
   $ret .= "<HEAD>\n";
   $ret .= "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=iso-8859-1\">\n";
   $ret .= "<META NAME=\"Author\" CONTENT=\"Thorsten Klose\">\n";
   $ret .= "<TITLE> ${title} </TITLE>\n";
   $ret .= "</HEAD>\n";
   $ret .= "<BODY>\n";
   $ret .= "<HR>\n";
   return $ret;
}

sub get_footer
{
   my $ret = "";

   my $copyright = "<P CLASS=PLEASE>Copyright © 1998-2006, Thorsten Klose. All rights reserved.</P>";
   my @TIME = localtime(time());
   my $date = sprintf("20%02d-%02d-%02d",substr($TIME[5],-2),$TIME[4]+1,$TIME[3]);
   #my $date = `date "+%d-%m-%Y"`;

   $ret .= "<HR>\n";
   $ret .= "${copyright}\n";
   $ret .= "<P align=right>Last update: ${date}</P>\n";
   $ret .= "</BODY>\n";
   $ret .= "</HTML>\n";
}

sub conv_ltgt
{
  my ($ret) = @_;

  $ret =~ s/>/&gt;/g;
  $ret =~ s/</&lt;/g;

  return $ret;
}
