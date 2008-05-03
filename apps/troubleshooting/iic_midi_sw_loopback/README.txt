$Id$
IIC MIDI Software Loopback Test V1.0
===============================================================================
Copyright 2006 Thorsten Klose (tk@midibox.org)
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
   o LCD (shows only which slaves are connected)
   o up to 4 MBHP_IIC_MIDI modules

===============================================================================

Project page:
   http://www.ucapps.de/mbhp_iic_midi.html

This application scans for available MBHP_IIC_MIDI devices during
startup and displays them on the LCD

Thereafter it creates a software loopback for each MIDI slave.
Incoming messages on the MIDI IN port will be directly forwarded
to the MIDI OUT port of the appr. slave

This allows you to test, if each connected module is working - just
use a (virtual) keyboard, send some Notes and check the received
data, it must be identical.

Module behaviour:
   o Power LED (green) on: the PIC is running, initialization phase finished
   o Rx LED (yellow) flickers: a MIDI message has been received
   o Tx LED (red) flickers: a MIDI message has been sent

Notes:
   o the yellow LED will only flicker until the MIDI buffer 
     of the MBHP_IIC_MIDI slave is full. This scenario can happen 
     if the core cannot poll for the incoming data. 
     This means in other words: this effect can be noticed, if the 
     SC/SD lines are not connected correctly

   o Don't forget the 1k pull-up resistor at J4:SC of the core
     module - this resistor is not available on older PCB revisions of
     MBHP_CORE

   o the core has to be rebooted if the address jumpers of the 
     MBHP_IIC_MIDI slaves have been changed

   o in distance to the MIDI Router application, this test program
     does *not* require a connection between MBHP_IIC_MIDI::J2:RI and the core

   o the iic_midi.asm and iic_midi.h file can be used as a driver
     for own applications which access the MBHP_IIC_MIDI module

===============================================================================

Description about the most important files:

   - mios_wrapper\mios_wrapper.asm and mios_wrapper\mios_tables.inc:
     The MIOS wrapper code and MIOS specific configuration tables

   - pic18f452.c: exports PIC18F452 specific SFRs

   - main.c: the main program with all MIOS hooks

   - iic_midi.asm: contains the code to access external MIDI ports via IIC

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
