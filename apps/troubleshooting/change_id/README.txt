$Id$
Change ID V1.9
===============================================================================
Copyright 2006 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

This program allows you to change the ID header of your PIC w/o
using a PIC burner like the JDM or MBHP_BURNER

Use it with care and don't start it if the ID header is already ok!

Required steps for a proper ID initialization:

   o ensure that the MIOS bootloader has already been programmed into
     the PIC. You should see an upload request after power-on
     (see also: http://www.ucapps.de/howto_debug_midi.html)

   o upload the most recent MIOS version like described under:
     http://www.ucapps.de/mios_bootstrap.html

   o open the Makefile (or make.bat file) and change the ID settings for your needs

   o build a new .hex file, see also
     http://www.ucapps.de/howto_tools_gpasm.html

   o upload this project like described under:
     http://www.ucapps.de/mios_bootstrap.html

     -> the MIOS bootloader will get active for 2 seconds
     -> thereafter this program will be started
     -> the MIOS bootloader will be started again, but now with
        the new ID settings
     -> the LCD will show the new ID header (if connected)


Difference between change_id V1.9 and V1.9b:
For PIC18F4620 I had to insert a NOP after the erase sequence - it
seems, that the "tblrd*-" instruction after "bsf EECON1, WR" won't be executed

===============================================================================

Prepared .hex files:

for your convenience, a couble of .hex files have been prepared which
allow to change to the most requested configuration settings without
rebuilding the .hex file

-> device_id_01.hex: change to device ID 00 (-> 0000000000000000)
-> device_id_02.hex: change to device ID 01 (-> 0000000000000001)
-> device_id_03.hex: change to device ID 02 (-> 0000000000000002)
-> device_id_04.hex: change to device ID 03 (-> 0000000000000003)

-> iic_midi_10.hex:  change to IIC MIDI 10  (-> 0000000000100000)
-> iic_midi_12.hex:  change to IIC MIDI 12  (-> 0000000000120000)
-> iic_midi_14.hex:  change to IIC MIDI 14  (-> 0000000000140000)
-> iic_midi_16.hex:  change to IIC MIDI 16  (-> 0000000000160000)

If combinations of these settings are required, or if other config
values should be set (to-COM interface and Display Mode), 
I kindly request to install GPUTILS and to follow the instructions
given above.

Especially the to-COM interface switching is very critical, because
once you did this, you won't be able to switch back via a standard
MIDI interface, unless you own a MBHP_LTC module, or a PIC Burner. 
Since it happened very often in the last time, that newbies just 
click on .hex files without reading about the purpose.

===============================================================================
