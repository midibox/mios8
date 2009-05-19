
DIO Test
===============================================================================
Copyright (C) 2008 Matthias Mächler (maechler@mm-computing.ch/thismaechler@gmx.ch)
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
   o any number of DIN's / DOUT's

Optional hardware:
   o Character LCD

===============================================================================

Test Application for DIN's and DOUT's. After bootup,
alls DOUTS will be walked, change "#define num_douts 32" in main.c
to the numbers of DOUT's you want to test. If a display is connected,
the current DOUT number will be displayed. Change "#define interval 30"
to change the interval between the single steps (in 10ms)
When "#define testrun_srset 1" is set to 1, a secod testrun
will be done with DOUT_SRSet (always one DOUT).

After the testruns, you can push buttons connected to DIN's, and
the PIN-number will be displayed on the LCD. If "#define din_to_dout 1" is
set to 1, on DIN PIN = 0 (button down), the DOUT with PIN=DIN-PIN will
be switched. 

===============================================================================
