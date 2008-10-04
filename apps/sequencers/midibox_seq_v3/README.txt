$Id$
MIDIbox SEQ V3
===============================================================================
Copyright 2006 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

The 24 Bit / 16 Bit division routine has been created by Nikolai Golovchenko, 
and is published at: http://www.piclist.org/techref/microchip/math/div/24by16.htm
===============================================================================

The MIDIbox SEQ project is introduced at this webpage:
http://www.ucapps.de/midibox_seq.html


Please read also CHANGELOG.txt for the last changes.


Some precompiled setups are part of this package:
   o setup_mbseq_v3.hex        (default setup for rotary encoders)
   o setup_mbseq_tk.hex        (TK's setup)
   o setup_mbseq_cd_cd.hex     (CD CD's setup)
   o setup_mbseq_wilba.hex     (Wilba's setup)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o note that a main.asm file doesn't exist, instead, main.inc is
     included from the setup_mbseq_*.asm files. main.inc doesn't need to be touched!
   o do your setup in setup_mbseq_v3.asm (or copy this to a new .asm file)
     The setup_mbseq_tk.asm file is intented for TK's own setup and partly requires special
     hardware (special rotary encoders, 64k BankSticks, 64 Button/Duo-LED Matrix
   o some general settings can be found in the "Control surface settings" section 
     of the appr. setup_*.asm file
   o define the pins of your rotary encoders (if available) within the setup_*.asm file
   o define the connected buttons and LEDs in (if available) setup_*.asm
   o if the assembler doesn't pass, check for error messages in the setup_*.err file

For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================
