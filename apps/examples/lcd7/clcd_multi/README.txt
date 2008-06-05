
MIOS Custom LCD Driver Integration Example for multiple character LCDs

===============================================================================
Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The LCD driver is located in $MIOS_PATH/modules/app_lcd/clcd_multi/

This example demonstrates, how to integrate such a driver into a
C based (-> c/) application.

It can also be used in assembly application, but an example hasn't
been written yet due to lazyness
It has already been tested on a customized MIDIbox LC application

Note especially the MIOS_LCD_TypeSet() initialisation in USER_Init (resp. Init())

And for multi CLCDs: the USER_LCD_Select() function
===============================================================================
