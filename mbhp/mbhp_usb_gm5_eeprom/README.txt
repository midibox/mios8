
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

Optional hardware:
   o LCD to output status message

===============================================================================

This application allows to program the optional IIC EEPROM for MBHP_USB_GM5
with customized USB descriptors.

Adaptions have to be done in eeprom_content.c (e.g. changing number of MIDI
interfaces, changing device name, changing PID/VID, etc...)

The EEPROM structure has been provided by Ploytec, the USB descriptors
are taken from the MBHP_USB_PIC project

Note that only IIC EEPROMs with 8bit addressing can be used (e.g. 27C04)

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
