Visual Metronome V0.2   (C) Tim Jefford 2013  (gomiboy99 at gmail.com)
based on clockbox version 1.0 (C) 2005 Thorsten Klose (tk@midibox.org)
===============================================================================

A precompiled binary is already part of this package:
   o project18f452.hex & project18f4685.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

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

