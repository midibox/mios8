
Pedal BoX / Pedal Board V2.5
===============================================================================
Copyright (C) 2010  Mick Crozier (mick@durisian.com)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The Pedal Box was designed to be a very basic extension to a floorboard.
Giving it an extra 8 Controller Pedals, and a display to see patch change / CC names, as well as their values. 

It has now expanded with options to build a complete MIOS Powered Floorboard. 

You will need the PC Editor software to create the required bankstick files
see more info here: http://www.midibox.org/dokuwiki/doku.php?id=pedal_box

===============================================================================
A precompiled binary is already part of this package:
   o project.hex (can be loaded into MIOS Studio)

Following tools are required to recompile the code:
   o SDCC
   o gputils

The details are described under
   http://www.midibox.org/dokuwiki/application_development

The details are described under http://www.ucapps.de/mios_c.html


===============================================================================

Required hardware:
   o 1 x MBHP_CORE module
   o Large Character 16x2 LCD Display
   o 1 x 10k pot or CV pedals
   o 2 x 24LC256 Bankstick
   0 A MIDI Floorboard / programable HUI. Or 1 x DINX1 Module and 1 button

Optional hardware:
   o 8 x 10k pot or CV pedals
   o 1 x MBHP_DINX4 module + 1 x MBHP_DINX1 module
   o 34 x buttons
   o 2 x MBHP_DOUTX4 module
   o 32 x LEDs
   o 8 x relays
   o 2 x 7 segment LED digits or a 2x20 LCD Display
   
===============================================================================

Included Files:

main.h                
main.c
pbx_config.h           -- Hardware configuration setting
pbx_midi_config.h      -- Midi Implementation for Pedal Box
pbx_relay_config.h     -- Settings for relay switches
pbx.h
pbx.c                  -- functions, handlers
pbx_lcd.c              -- lcd handlers
pbx_digits.c           -- 7 segment LED digits driver


===============================================================================

MORE INFORMATION - SEE PEDAL BOX IN THE WIKI

http://www.midibox.org/dokuwiki/doku.php?id=pedal_box

===============================================================================
