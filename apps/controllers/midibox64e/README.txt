$Id$

MIDIbox64E V2
===============================================================================
Copyright 2003 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The MIDIbox64E project is introduced at this webpage:
http://www.ucapps.de/midibox16e.html

(don't be confused about "16e" - this application supports up to 64
rotary encoders (in such a case, no additional buttons can be connected,
as 64 encoders allocate 128 input pins - the maximum number of DINs)


Please read also CHANGELOG.txt for the last changes.


Some precompiled setup_*.hex files are part of this package

The required application upload procedure is described
under http://www.ucapps.de/mios_bootstrap_newbies.html


Infos for customization:
   o note that a main.asm file doesn't exist, instead, main.inc is
     included from the setup_*.asm files. main.inc doesn't need to be touched!
   o do your setup in setup_midibox16e.asm (or copy this to a new .asm file)
   o define the pins of your rotary encoders (if available) within the setup_*.asm file
   o if the assembler doesn't pass, check for error messages in the setup_*.err file


For details, how to build a new .hex file, see
   http://www.midibox.org/dokuwiki/application_development

===============================================================================
