
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

reads encoder changes for four encoders,
increments/decrements values 0-enc_value_max and shows the values on
character LCD.
modify defines for your needs in main.c. By default, the four encoders are
assumed to be connected to one SR (enc_sr_number).

===============================================================================
