$Id$
MIDIbox SEQ Dr
===============================================================================
Copyright 2008 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

The 24 Bit / 16 Bit division routine has been created by Nikolai Golovchenko, 
and is published at: http://www.piclist.org/techref/microchip/math/div/24by16.htm
===============================================================================

The MIDIbox SEQ Dr project is introduced at this webpage:
http://www.ucapps.de/midibox_seq_dr.html

The MIDIbox 808 project is introduced at this webpage:
http://www.ucapps.de/midibox_808.html


Please read also CHANGELOG.txt for the last changes.


Some precompiled setups are part of this package:
   o setup_seq_dr_default.hex          (setup for MIDIbox SEQ Dr users)
   o setup_808_default.hex             (setup for MB808 users)
   o setup_808_with_swingpot.hex       (setup for MB808 users, PCB stuffed with Swing Pot)
   o setup_808_mbseqv3_hardware.hex    (setup for MIDIbox SEQ V3 users)
   o setup_808_mbseqv3_tk.hex          (setup for TK's MIDIbox SEQ V3)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o note that a main.asm file doesn't exist, instead, main.inc is
     included from the setup_*.asm files. main.inc doesn't need to be touched!

MBSEQ Dr Users:
   o do your setup in setup_seq_dr_default.asm (or copy this to a new .asm file)

     setup_seq_dr_default and setup_seq_dr_default_with_swingpot are matching with
     the MBSEQDR PCB.
     Use setup_seq_dr_with_swingpot if a potentiometer is connected to J5.A0 of
     the core, use setup_seq_dr_default if this pin is open (to prevent unstable
     clocks due to floating input)


MB808 Users:
   o do your setup in setup_808_default.asm (or copy this to a new .asm file)
     The setup_808_mbseq_v3_hardware.asm file is intended for owners
     of the MIDIbox SEQ V3 hardware (e.g. two 2x40 LCDs, different
     button and LED assignments
     The setup_808_mbseqv3_tk.asm file is intented for TK's MIDIbox SEQ v3
     and partly requires special hardware (special rotary encoders, 64k BankSticks, 
     64 Button/Duo-LED Matrix

   o some general settings can be found in the "Control surface settings" section 
     of the appr. setup_*.asm file
   o define the pins of your rotary encoders (if available) within the setup_*.asm file
   o define the connected buttons and LEDs in (if available) setup_*.asm
   o if the assembler doesn't pass, check for error messages in the setup_*.err file

For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================
