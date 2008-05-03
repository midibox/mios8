$Id$

Clockbox EKX
===============================================================================

This application sends a MIDI clock and outputs a 24ppqn signal at CORE::J14
from 48 to 255 BPM

It is also capable of sending the MIDI song position to sequencers which are
able to parse this message (F2 pos_L pos_H)

The user interface consists of an encoder for BPM selection,
a Stop/Pause/Play/Rev and Fwd button, and a LCD, which displays the
selected BPM and the current song position

The hardware is the same like for MIDIbox SID CS Step A (without the SID module)

The schematic describes the connections is located here:
http://www.ucapps.de/mios/clockbox.pdf

-------------------------------------------------------------------------------

Update V1.1:
A "multi clock out" option has been added which supports 8 additional
MIDI Outs. The clocks which are sent can be enabled/disabled with
8 seperate play and stop buttons. Changes will be synchronized to
the beat (1/4 note)

This option is based on an idea of Braintu, his full concept can be
found under http://www.midibox.org/dokuwiki/doku.php?id=midimasterclock
Only the stop/play function has been implemented yet, he will continue
on this project.

Technical details about this option: the 8 MIDI outs are available at
J5 of the core module. This port is normaly used as analog input. In
order to ensure, that no short circuit happens if a user uploads this
application on a MIDIbox which has pots or AIN modules at J5, the
MULTI_CLOCK_OPTION flag in main.h is disabled (0) by default.
It has to be set to 1 to get the MIDI outs working

MIDI events are sent over these outputs with the "bit banging" method.
(see mclock.c, MCLOCK_SendMultiPort)

The enabled MIDI clock channels are displayed with 8 LEDs

There is a special schematic which describes the additional
connections: http://www.ucapps.de/mios/clockbox_multi.pdf

Modification by Michaël Heyvaert (micheyvaert@hotmail.com):
This mod makes it possible to select a divisor
the pause button is replaced by a encoder mode button,
the play button now acts as a playpause button
note: right now the divisor only works when compiling with
MTC mode set to 0 (standard mode)

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
   o one DINX1 module (or just a 74HC595 shift register + pull-up resistors)
   o one rotary encoder
   o 5 buttons
   o 2x16 LCD

Optional hardware for Multi Clock option:
   o a DINX3 (or DINX4) module instead of DINX1
   o a DOUTX1 (or DOUTX4) module
   o 16 buttons
   o 8 LEDs
   o 16 * 220 Ohm resistors + 8 MIDI sockets

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
   - send_bit_macro.inc: some assembly code for bit-banging MIDI out

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
