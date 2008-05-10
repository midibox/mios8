
sid_random V1.0                        (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

This application generates random patches for the MIDIbox SID

It just requires one core module, a button to "fire" the random patch,
and a "true random generator", e.g. based on http://willware.net:8080/hw-rng.html

The random generator has to be connected to pin RA0 of the core module (J5:A0)

The button to pin RC3 (J6:SI) - a pull-up resistor is already installed 
there.

The MIDI merger is enabled by default, this allows you to connect
the random generator between a keyboard/PC and your MIDIbox SID.
The additional latency is about 300..400 uS

Random constraints can be edited in sid_random.c

===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)
   o project.syx (can be loaded into any SysEx upload tool)

Following tools are required to recompile the code:
   o SDCC v2.5.0
   o gputils
   o perl

The details are described under http://www.ucapps.de/mios_c.html

===============================================================================

Required hardware:
   o one MBHP_CORE module
   o one button connected to pin RC3 of the PIC

Optional hardware:
   o random patch will also be generated when any button of a DINX4
     module has been pressed

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup (e.g., if no AINX4 multiplexers
     are connected to the core)

   o the application can be rebuilt with the "make.bat" file
     (type "make" in a DOS command shell)

   o Hint: unused analog inputs must be tied to ground, otherwise
     the core will transmit a lot of random CC events

===============================================================================

Description about the most important files:

   - mios_wrapper\mios_wrapper.asm and mios_wrapper\mios_tables.inc:
     The MIOS wrapper code and MIOS specific configuration tables

   - pic18f452.c: exports PIC18F452 specific SFRs

   - main.c: the main program with all MIOS hooks

   - sid_random.c: generates the random patch

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
