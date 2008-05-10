$Id$

Scan Matrix Example #2
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
   o one DINX1 module (or DINX4)
   o one DOUTX1 module (or DOUTX4)

Optional hardware:
   o a C64 keyboard, see http://www.midibox.org/forum/index.php?topic=6505.0

===============================================================================

This program demonstrates how to scan a matrix of up to 8x8 buttons/keys
with lowest latency

The Matrix driver can be found in sm_fast.asm, read about further 
informations there. 

By default, the 8 columns and rows of the scan matrix are assigned to
the first DOUT/DIN register. The function "SM_NotifyToggle()" in main.c 
is called when a button has been pressed or released.

===============================================================================

Description about the most important files:

   - mios_wrapper\mios_wrapper.asm and mios_wrapper\mios_tables.inc:
     The MIOS wrapper code and MIOS specific configuration tables

   - pic18f452.c: exports PIC18F452 specific SFRs

   - main.c: the main program with all MIOS hooks

   - sm_fast.asm: the assembly optimized matrix scan code

There are additional .h files for all .c files which contain
general definitions and the declaration of global functions/variables
These .h files must be included into the program parts which
get use of these globals

===============================================================================
