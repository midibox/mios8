
MBHP_USB_GM EEPROM programmer
===============================================================================
Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o 8 pin socket connected to J4 (same pinning as BankStick with CS#0 -
     pin 1/2/3 of IIC EEPROM connected to ground)

Optional hardware:
   o LCD to output status message

===============================================================================

This application allows to program the optional IIC EEPROM for MBHP_USB_GM5
with customized USB descriptors.

Adaptions have to be done in eeprom_content.c (e.g. changing number of MIDI
interfaces, changing device name, changing PID/VID, etc...)

The EEPROM structure has been provided by Ploytec, the USB descriptors
are taken from the MBHP_USB_PIC project

Note that only IIC EEPROMs with 8bit addressing can be used (e.g. 24C04)

Programming will be performed ca. 5 seconds after startup.

A status code will be displayed if a LCD is connected:
   0x00: no error
   0x01: verify error
   0x02: no EEPROM connected
   0x40: interface descriptor at wrong position
   0x41: endpoint descriptor not found

The status code will also be sent as Program Change message via MIDI 
(useful if no LCD connected)

===============================================================================

Important note: due to an imperfection in the current GM5 firmware, only the
lower 256 byte bank of the 2kbyte EEPROM can be accessed. If more than 
3 MIDI interfaces are defined, the descriptors will exceed the 256 byte
boundary and the device won't enumerate correctly.

This means in other words: currently, all 5 IOs can only be used with the
original descriptors which are stored in GM5 (-> remove EEPROM, set J1..J3
jumpers accordingly)

===============================================================================
