MIDIbox MIDI-Mapper

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
   - one MBHP_CORE module
	- one MBHP_DIN module
	- one MBHP_DOUT module
	- 20 buttons(NOT switches!) with LED's, or separate LED's
	- 1 32K or 64K Bankstick
	- AC or DC power supply, 7-10V, 800mA. If the device reboots when much
	  LED's are switched on, then you need a more powerfull supply.
	- Switch to turn the device on / off.

Optional hardware:
   o 

===============================================================================

The device maps MIDI-input-chanels to MIDI-output-channels.
Each of the 16 input / output channels can be assigned to
any of the 16 buses. So serveral output channels can be fed
by one input channel, or serveral input channels can go to
one or more output channels. Redundancy over the buses will
be solved, a input->output map will be built to forward the
messages.
A setup can be stored in one of the 16 presets. There are 
four "screen"-buttons and 16 "value"-buttons:

- preset screen: DIN PIN 0, DOUT PIN 0(LED)
- bus screen: DIN PIN 1, DOUT PIN 1
- input screen: DIN PIN 2, DOUT PIN 2
- output screen: DIN PIN 3, DOUT PIN 3

- value-buttons (1-16), DIN 8-23, DOUT 8-23(LEDS)

all functionality can be configured by these 20 buttons,
the LED's indicate the current state/setup, therefore no
LCD-display is needed.




===============================================================================
