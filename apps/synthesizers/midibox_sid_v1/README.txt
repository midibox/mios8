$Id$

MIDIbox SID V1.7303
===============================================================================
Copyright 2006 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.

The 16x16 multiplication routine is taken from the PIC18Fxx2 datasheet
distributed by Microchip.

Code/Idea for ENV Curve Parameter, recalculated SID_FRQ_TABLE and OSC 
synchronization copyright by Jess D. Skov-Nielsen
===============================================================================

The MIDIbox SID project is introduced at this webpage:
http://www.ucapps.de/midibox_sid.html


Please read also CHANGELOG.txt for the last changes.


Some precompiled setups are part of this package:
   o setup_tk.hex              (TK's setup)
   o setup_6581.hex            (SID Master, stuffed with 6581)
   o setup_6581_without_cs.hex (SID Slave, stuffed with 6581)
   o setup_8580.hex            (SID Master, stuffed with 8580)
   o setup_8580_without_cs.hex (SID Slave, stuffed with 8580)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o some general settings can be found in the "Control surface settings" section 
     of main.asm or the appr. setup_*.asm file
     check especially the DEFAULT_DEVICE_ID, CS_ENABLED and CBM8580_FILTER_FIX flag
   o define the pins of your rotary encoders (if available) in mios_tables.inc
   o define the connected buttons and LEDs in (if available) cs_menu_io_tables.inc
   o change the rotary encoder assign map in cs_menu_enc_table.inc if required

For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================
