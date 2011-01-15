
MIDIbox FM V1.2
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
   o setup_pic18f452_mbfm_v1.hex         (default setup for PIC18F452)
   o setup_pic18f452_mbfm_tk.hex         (TK's setup for PIC18F452)
   o setup_pic18f4685_mbfm_v1.hex        (setup for PIC18F4685 with enhanced features)
   o setup_pic18f4685_mbfm_tk.hex        (TK's setup for PIC18F4685)
   o setup_pic18f4685_sammich_fm.hex     (setup for sammichFM)

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html

Infos for customization:
   o some general settings can be found in the "Control surface settings" section 
     of the setup_mbfm_v1.asm file
   o define the pins of your rotary encoders (if available) in setup_*.asm as well
   o define the connected buttons and LEDs in (if available) cs_menu_io_tables.inc
   o change the rotary encoder assign map in cs_menu_enc_table.inc if required

For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Some preset patches can be found in the presets directory.

They have to be uploaded into the BankStick via MIDI after MIOS and the 
firmware have been installed.

===============================================================================
