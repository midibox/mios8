
MIOS Invaders. Requires a GLCD of a least 64x128 resolution

===============================================================================
Copyright (C) 2009  Phil Taylor (phil@taylor.org.uk)

Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

This is a MIOS version of the classic Space Invaders Game
based on a PROTON+ BASIC program Written by Les Johnson 2003.
For the orignal software visit: (licensed under a non-commerial license)
http://users.picbasic.org/projects/Space%20Invaders/space_invaders.htm

This version has only been tested on a DOG GLCD but any 128x64 GLCD should work.

I cannot get the missile display to work at the moment, I need to work on that 
a bit more but pretty much everything else works.

I also have not got any sound working yet, I think it would be fun to use MIDI
output so you could use your favourite synth!

It also includes two special fonts, one is the SPLASH SCREEN and the other
is a standard 8x6 font with some extra characters.

By default the first 4 white keys on a MIDI keyboard  or the first 4 buttons 
on a DIN module are used as follows:

FIRE_BUTTON = First white key (0x18) and DIN-0
LEFT_BUTTON = Second white key (0x1a) and DIN-1 
RIGHT_BUTTON = Third white key (0x1c) and DIN-2 
LEFT_BUTTON = Fourth white key (0x1d) and DIN-3 
===============================================================================
