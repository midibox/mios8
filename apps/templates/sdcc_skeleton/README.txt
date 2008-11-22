
SDCC rotary encoder tester
===============================================================================
Copyright (C) 2008  Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
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
   o one DIN module
   o one Character LCD display
   o encoders (4 encoders will be read and values 0-127 displayed on LCD)

Optional hardware:
   o 

===============================================================================

reads encoder changes for four encoders (SR 1, pins 0 - 7),
increments/decrements values 0-127 and shows the values on
character LCD

===============================================================================
