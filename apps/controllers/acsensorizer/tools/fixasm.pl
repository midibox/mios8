#!/usr/bin/perl -w
#########################################################################################
#
# converts a .asm file generated from sdcc to a format which can be assembled with 
# the microchip tool "MPASMWIN"
#
# Author: Thorsten Klose (tk@midibox.org) 2004-07-10
#
#########################################################################################

if( scalar(@ARGV) < 1 )
{
   die "SYNTAX: fixasm.pl <asm-file>\n";
}

$filename = $ARGV[0];

@OUT = ();
push @OUT, ";; BEGIN inserted by fixasm.pl\n";
push @OUT, ";; MPASM workarounds:\n";
push @OUT, "BANKED  EQU 1\n";
push @OUT, ";; END inserted by fixasm.pl\n";


open(IN, "<${filename}") || die "ERROR: cannot open '${filename}' for reading!\n";

while( <IN> )
{
   # to prevent "Error - section '.registers' can not fit the absolute section. Section '.registers' start = 0x00000000, length=0x00000003"
   # for MIOS it is also required to set the start address to 0x010
   s/^\.registers\tudata_ovr\t0x0000/.registers\tudata_ovr\t0x010/;

   # many MIOS functions are using FSR1[LH] as internal working pointer. 
   # FSR1 is used as stack pointer by SDCC --- to avoid problems, I'm
   # swapping FSR1x and FSR0x
   s/^INDF1\s+equ\s+0xfe7/INDF1\tequ\t0xfef ;; normaly 0xfe7, changed by fixasm.pl/;
   s/^POSTINC1\s+equ\s+0xfe6/POSTINC1\tequ\t0xfee ;; normaly 0xfe6, changed by fixasm.pl/;
   s/^POSTDEC1\s+equ\s+0xfe5/POSTDEC1\tequ\t0xfed ;; normaly 0xfe5, changed by fixasm.pl/;
   s/^PREINC1\s+equ\s+0xfe4/PREINC1\tequ\t0xfec ;; normaly 0xfe4, changed by fixasm.pl/;
   s/^PLUSW1\s+equ\s+0xfe3/PLUSW1\tequ\t0xfeb ;; normaly 0xfe3, changed by fixasm.pl/;
   s/^FSR1H\s+equ\s+0xfe2/FSR1H\tequ\t0xfea ;; normaly 0xfe2, changed by fixasm.pl/;
   s/^FSR1L\s+equ\s+0xfe1/FSR1L\tequ\t0xfe9 ;; normaly 0xfe1, changed by fixasm.pl/;

   s/^INDF0\s+equ\s+0xfef/INDF0\tequ\t0xfe7 ;; normaly 0xfef, changed by fixasm.pl/;
   s/^POSTINC0\s+equ\s+0xfee/POSTINC0\tequ\t0xfe6 ;; normaly 0xfee, changed by fixasm.pl/;
   s/^POSTDEC0\s+equ\s+0xfed/POSTDEC0\tequ\t0xfe5 ;; normaly 0xfed, changed by fixasm.pl/;
   s/^PREINC0\s+equ\s+0xfec/PREINC0\tequ\t0xfe4 ;; normaly 0xfec, changed by fixasm.pl/;
   s/^PLUSW0\s+equ\s+0xfeb/PLUSW0\tequ\t0xfe3 ;; normaly 0xfeb, changed by fixasm.pl/;
   s/^FSR0H\s+equ\s+0xfea/FSR0H\tequ\t0xfe2 ;; normaly 0xfea, changed by fixasm.pl/;
   s/^FSR0L\s+equ\s+0xfe9/FSR0L\tequ\t0xfe1 ;; normaly 0xfe9, changed by fixasm.pl/;


   push @OUT, $_;
}
close(IN);

open(OUT, ">${filename}") || die "ERROR: cannot open '${filename}' for writing!\n";
print OUT @OUT;
close(OUT);
