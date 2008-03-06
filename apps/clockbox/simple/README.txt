$Id$

Clockbox V1.0                          (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

This application sends a MIDI clock and outputs a 24ppqn signal at CORE::J14
from 48 to 255 BPM

It is also capable of sending the MIDI song position to sequencers which are
able to parse this message (F2 pos_L pos_H)

The user interface consists of an encoder for BPM selection, 
a Stop/Pause/Play/Rev and Fwd button, and a LCD, which displays the
selected BPM and the current song position

The hardware is the same like for MIDIbox SID CS Step A (without the SID module)

===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC v2.7.0
   o gputils

The details are described under http://www.ucapps.de/mios_c.html

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one DINX1 module (or just a 74HC595 shift register + pull-up resistors)
   o one rotary encoder
   o 5 buttons
   o 2x16 LCD

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup (normaly not)

   o check the encoder type in main.c
     By default, the DETENTED2 type is selected

   o the application can be rebuilt by typing "make" in the command shell

===============================================================================

Description about the most important files:

   - main.c: the main program with all MIOS hooks
   - mclock.c: the MIDI clock handler

===============================================================================
