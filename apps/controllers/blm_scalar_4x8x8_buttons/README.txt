$Id$

BLM_SCALAR Firmware Variant for 4 8x8 button matrices
===============================================================================
Copyright (C) 2009 Thorsten Klose (tk@midibox.org)
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
   o at least 1 DINX4 and 1 DOUTX4 module
   o optional: up to 3 additional DINX4 modules to scan +96 extra buttons

===============================================================================


4 independent 8x8 Matrices are scanned.
Each Matrix has a dedicated DIN and DOUT shift register.

Note that all 4 DOUTs output the same patterns, it's possible to use
a single DOUT register to scan all 4 DIN matrices - decide by yourself
(it depends on your hardware wiring)

The unusued 12 DIN (and 12 DOUT) shift registers can be used
for other purposes.
The default implementation in main.c just sends a MIDI event for each
extra button.


MIDI Protocol
~~~~~~~~~~~~~

  See main.c

  This application is made for easy adaptions

===============================================================================
