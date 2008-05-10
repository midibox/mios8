$Id$

MIDIbox MM V2.0                        (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

This application emulates the Motormix protocol like described
in the "Motormix Developers Guide V1.2" from CM Labs
   http://www.cmlabs.net/MotorMix%20V%201.2%20Developer%20pkg3.pdf 

Please read also CHANGELOG.txt for the details

===============================================================================

A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Configuration steps:

   o check the "general application settings" in main.h if changes are
     required for your hardware setup

   o go through the button/led assignment table in mm_dio_table.c
     and map the Motormix emulation specific functions for your needs

   o define the encoder pins in main.c

   o if you plan to use the GPC (General Purpose Controller) mode, define
     the 128 MIDI Events which should be sent in mt_table.c

   o optionally you can also change the lables in mm_gpc_lables.c

===============================================================================

Description about the most important files:

   - main.c: the main program with all MIOS hooks

   - mm_lcd.c: LCD output routines

   - mm_sysex.c: SysEx parser
   - mm_midi.c: MIDI receive handler

   - mm_vpot.c: Encoder/LEDring handling
   - mm_mf.c: Motorfader handling
   - mm_dio.c: Button/LED handling
   - mm_dio_table.c: Button/LED mapping (configuration)
   - mm_leddigits.c: LEDdigits handling

   - mm_gpc.c: General Purpose Controller mode
   - mm_gpc_lables.c: GPC labels (configuration)
   - mt_table.c: MIDI event definitions for GPC mode

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
