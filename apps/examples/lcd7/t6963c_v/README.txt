
MIOS Custom LCD Driver Integration Example for T6963C
with vertical screen (-> up to 64x240)

===============================================================================
Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

The LCD driver is located in $MIOS_PATH/modules/app_lcd/t6963c_v/

This example demonstrates, how to integrate such a driver into an
assembler (-> asm/) and C based (-> c/) application.

Note especially the MIOS_LCD_TypeSet() initialisation in USER_Init (resp. Init())

===============================================================================
