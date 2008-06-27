$Id$

MIDIO128 V2
===============================================================================
Copyright 2003 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The MIDIO128 project is introduced at this webpage:
http://www.ucapps.de/midio128.html


Please read also CHANGELOG.txt for the last changes.


A precompiled setup_*.hex file is part of this package

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o note that a main.asm file doesn't exist, instead, main.inc is
     included from the setup_*.asm files. main.inc doesn't need to be touched!
   o do your setup in setup_midio128.asm (or copy this to a new .asm file)
   o define the pins of your rotary encoders (if available) within the setup_*.asm file
   o if the assembler doesn't pass, check for error messages in the setup_*.err file


For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================

Changing Configuration w/o recompiling source code:

MIDIO128 allows to change most of the parameters (e.g. In/Out Events,
pin polarities, Forwarding mode, Merger mode, etc...) via SysEx, so that
you don't need to create & upload a new .hex file - just only a .syx
file with your favourite SysEx loader

The perl script which creates a .syx file from a .ini file (ASCII text)
is located under $(MIOS_PATH)/tools/mk_midio128_syx 

===============================================================================
