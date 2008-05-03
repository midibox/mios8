$Id$

MIDI Router V1.0
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
   o up to 4 MBHP_IIC_MIDI modules
   o Rx and Tx LED, connected to Port RC0 and RC1 of the core module

Very Optional hardware:
   o 2x40 LCD
   o DINX1, stuffed with 1 rotary encoder and 6 buttons

===============================================================================

Project page:
   http://www.ucapps.de/midi_router.html

This application allows to forward and modify MIDI streams between the
MIDI port of the core module and up to four MBHP_IIC_MIDI modules.

The routing is defined in router.c and can be easily adapted for
your own needs.
The button/encoder handling is defined in main.c

===============================================================================

IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT

the J2:RI outputs of the MBHP_IIC_MIDI slaves have to be connected to
J5:A0..J5:A3 of the core module (analog inputs, used as digital inputs
here) - the core permanently polls for these "receive interrupts", without 
this connection, the core could hang up, since the inputs are floating 
(-> random values)

IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT --- IMPORTANT

===============================================================================

Description about the most important files:

   - main.c: the main program with all MIOS hooks

   - int_midi.asm: contains the code to access the internal MIDI port

   - iic_midi_tick.asm: polls the IIC MIDI ports for new data

   - router.c: contains the routing code - make your adaptions here!

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals


Used external code modules:
   - $MIOS_PATH/modules/iic_midi: the MBHP_IIC_MIDI driver
   - $MIOS_PATH/modules/midi_rxtx_leds: MIDI Rx/Tx LED handler

===============================================================================
