
MIDIbox FM V1.1
===============================================================================
Copyright 2006 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

The 16x16 multiplication routine is taken from the PIC18Fxx2 datasheet
distributed by Microchip.

Code/Idea for ENV Curve Parameter copyright by Jess D. Skov-Nielsen
===============================================================================

The MIDIbox FM project is introduced at this webpage:
http://www.ucapps.de/midibox_fm.html


Please read also CHANGELOG.txt for the last changes.


Some precompiled setups are part of this package:
   o setup_mbfm_v1.hex         (default setup)
   o setup_mbfm_tk.hex         (TK's setup)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o some general settings can be found in the "Control surface settings" section 
     of the setup_mbfm_v1.asm file
   o define the pins of your rotary encoders (if available) in mios_tables.inc
   o define the connected buttons and LEDs in (if available) cs_menu_io_tables.inc
   o change the rotary encoder assign map in cs_menu_enc_table.inc if required

The http://www.ucapps.de/howto_tools_gpasm.html page describes, how
to build a new .hex file

===============================================================================
