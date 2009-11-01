$Id$

MBHP_USB_PIC V1.1
===============================================================================
Copyright 2005-2009 Thorsten Klose (tk@midibox.org)
Licensed for personal non-commercial use only.
All other rights reserved.
===============================================================================

A precompiled binary is already part of this package:
   o project.hex

Following tools are required to recompile the code:
   o SDCC v2.7.0
   o gputils

The details for installing and rebuilding the firmware are described 
under http://www.ucapps.de/mbhp_usb_pic.html

===============================================================================

Required hardware:
   o see http://www.ucapps.de/mbhp_usb_pic.html
     The MBHP_CORE module can be used, stuffed with a PIC18F4550
     Crystal needs to be changed to 20 MHz, Reset and Bootloader Button
     has to be added, and a USB socket + 470 nF cap is required

Optional hardware:
   o Rx LED (Cathode connected via a 220 ohm resistor to PIC Pin RB7, Anode connected to +5V)
   o Tx LED (Cathode connected via a 220 ohm resistor to PIC Pin RB6, Anode connected to +5V)
   o up to 4 MBHP_IIC_MIDI modules as IO extension

===============================================================================

Description about the most important files:

   - main.asm: start vectors, interrupt handler, initialisation
     routine and main loop

   - usbdrv.asm: USB driver based on Microchip Framework, rewritten in assembly

   - usbdsc.c: USB descriptors for USB MIDI

   - usbcls.asm: USB MIDI Class Driver which can handle up to 5 IO ports

   - midi.asm: IO buffer handling for internal MIDI interface

   - int_midi.asm: MIDI parser and interface to usbcls.asm (USB MIDI Package Handlers)

   - iic.asm: IIC transfer routines

   - iic_midi.asm: Interface to usbcls.asm (USB MIDI Package Handlers)

===============================================================================
