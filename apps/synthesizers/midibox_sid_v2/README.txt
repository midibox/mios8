$Id$

MIDIbox SID V2.0
===============================================================================
Copyright 2008 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

The 16x16 multiplication routine is taken from the PIC18Fxx2 datasheet
distributed by Microchip.

Code/Idea for ENV Curve Parameter, recalculated SID_FRQ_TABLE and OSC 
synchronization copyright by Jess D. Skov-Nielsen
Idea for Oscillator Phase Offset approach by Wilba
===============================================================================

The MIDIbox SID project is introduced at this webpage:
http://www.ucapps.de/midibox_sid.html


Please read also CHANGELOG.txt for the last changes.


Some precompiled setups are part of this package:
   o setup_6581.hex            (MBSID stuffed with 6581)
   o setup_8580.hex            (MBSID stuffed with 8580)
   o setup_tk.hex              (TK's Setup)
   o setup_mb6582.hex          (MB-6582 Setup)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o note that a main.asm file doesn't exist, instead, src/main.inc is
     included from the setup_*.asm files. src/main.inc doesn't need to be touched!
   o do your setup in setup_8580.asm or setup_6581.asm
     MIDIbox SID slaves will be detected automatically depending on the
     MIOS device ID
     The setup_tk.asm file is intented for TK's own setup and partly requires special
     hardware


The http://www.ucapps.de/howto_tools_gpasm.html page describes, how
to build a new .hex file

===============================================================================
