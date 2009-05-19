
AOUT Test
===============================================================================
Copyright (C) 2009 
Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
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
   o one Character LCD Module
   o AOUT Module (AOUT / AOUT_LC / AOUT_NG)

Optional hardware:
   o 

===============================================================================

Sets 8 aout-channels to 0 / 32767 / 65534 in 5 seconds step repeatedly. By
default, the aout_ng is selected as target device:

Makefile: AOUT_DEFINES += -DAOUT_INTERFACE_TYPE=3

you can change this value to 1 or 2 (see also readme of the aout module).

===============================================================================
