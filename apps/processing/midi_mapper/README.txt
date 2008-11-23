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
   o one MBHP_CORE module
	o one MBHP_DIN module
	o one MBHP_DOUT module
	o 20 buttons(NOT switches!) with LED's, or separate LED's
	o 1 32K or 64K Bankstick
	o AC or DC power supply, 7-10V, 800mA. If the device reboots when much
	  LED's are switched on, then you need a more powerfull supply.
	o Switch to turn the device on / off.

===============================================================================
===================== Description of the device ===============================
===============================================================================

The device maps MIDI-input-chanels to MIDI-output-channels.
Each of the 16 input / output channels can be assigned to
any of the 16 buses. So serveral output channels can be fed
by one input channel, or serveral input channels can go to
one or more output channels. Redundancy over the buses will
be solved, a input->output map will be built to forward the
messages. So if a for ex. an input is routed to an output on more than one
bus, each message from this input will be routed to the output just once.

A setup can be stored in one of the 16 presets. 

There are four "screen"-buttons and 16 "value"-buttons and one LED each button:

o preset screen: DIN PIN 0, DOUT PIN 0(LED)
o bus screen: DIN PIN 1, DOUT PIN 1
o input screen: DIN PIN 2, DOUT PIN 2
o output screen: DIN PIN 3, DOUT PIN 3

o value-buttons (1-16), DIN 8-23, DOUT 8-23(LEDS)

all functionality can be configured by these 20 buttons,
the LED's indicate the current state/setup, therefore no
LCD-display is needed.

if you push a screen-button, this screen will be displayed
by the value LED's, the LED for the screen-button is on, (LED's for the other
three screen-buttons off), and you work on this screen.


===============================================================================
==================== Baisc user-interface functionality =======================
===============================================================================

preset screen:
---------------
if selected, LED for preset-screen will be on,
the value-LED's show the current preset. push
on value button loads preset.
save preset: hold preset-screen-button and push
value button to save the current setup in 
preset "value".
to clear all presets (first preset will go in1 -> out1, in2 -> out2 etc,
all other presets will be set empty): hold preset-screen-button for 
min 3 sec. LED's will flash to indicate reset.

bus screen:
-----------
if selected, LED for bus-screen will be on,
the value-LED's show the current bus (this
is the bus wich you can edit now on the
inputo and output-screens. all buses forward
midi-messages, not only the selected one!).
push a value button to select a bus to edit.
reset the current preset: hold bus-screen-button for
min. 3 sec.

input screen:
-------------
if selected, LED for input-screen will be on,
the value LED's show the input-channels assigned
to the currently selectd bus (->bus screen).
push a value button to switch the channel(only one
assigend),
hold the input-screen button and push a value
to add / remove this channel (to assign multiple channels).

output screen:
-------------
if selected, LED for output-screen will be on,
the value LED's show the output-channels assigned
to the currently selectd bus (->bus screen).
push a value button to switch the channel (only one
assigend),
hold the input-screen button and push a value
to add / remove this channel (to assign multiple channels).


===============================================================================
=========== Advanced user-interface functionality (info screens) ==============
===============================================================================

If you want to get information about the configuration of a preset, there
are so-called "info-screens", which you can access by holding two
screen-buttons, and opionally pushing a value button to get more detailed info.

for ex. :
"which <input channels> go to <output channels> [<x>]"

so press input-screen-button + output-screen-button. all the input channels
that go to (any) output channel will be shown by value-LED. If you additionally 
push a value button, x will be this value, all the input channels that are routed
to output-channel x will be indicated be value-LED's.
once a value button is pushed, you don't have to hold it, just push it once.
the screen will be shown then until you release one of the hold screen-buttons.

following screen-button can be used in any combination in this way:
input,output,bus.

examples:
--
question: "which buses have output channels" -> <bus-screen-button>+<output-screen-button>
question: "which buses have output channel <x>" -> <bus-screen-button>+<output-screen-button>+<value button x>
question: "which buses have input channels" -> <bus-screen-button>+<input-screen-button>
question: "which buses have input channel <x>" -> <bus-screen-button>+<input-screen-button>+<value button x>

question: "which inputs go to outputs" -> <input-screen-button>+<output-screen-button>
question: "which inputs go to output <x>" -> <input-screen-button>+<output-screen-button>+<value button x>
question: "which inputs are assigned to buses" -> <input-screen-button>+<bus-screen-button>
question: "which inputs are assigend to bus <x>" -> <input-screen-button>+<bus-screen-button>+<value button x>

question: "which outputs get data from inputs" -> <output-screen-button>+<input-screen-button>
question: "which outputs get data from input <x>" -> <output-screen-button>+<input-screen-button>+<value button x>
question: "which outputs are assigned to buses" -> <output-screen-button>+<bus-screen-button>
question: "which outputs are assigend to bus <x>" -> <output-screen-button>+<bus-screen-button>+<value button x>

you just have to press the buttons in the order they appear in the question sentence.



