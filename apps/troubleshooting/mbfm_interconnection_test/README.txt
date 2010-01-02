$Id$

CORE->MBHP_OPL3 Module Interconnection Test V2

This application allows you to check the interconnections to the OPL3 module
with a multimeter. 

You can control the OPL3 pins by sending a MIDI note with the virtual keyboard
of MIOS Studio.

Open the "MIOS Terminal" in MIOS Studio to get some helpful debugging messages
from the application -> see debugging.png as an example setup

By default all unselected pins are 0V

The SID pins are mapped to following keys:
   C : Pin J2_1:RS = 5V
   C#: Pin J2_1:A0 = 5V
   D : Pin J2_1:A1 = 5V
   D#: Pin J2_1:WR = 5V
   E : Pin J2_1:D0 = 5V
   F : Pin J2_1:D1 = 5V
   F#: Pin J2_2:D2 = 5V
   G : Pin J2_2:D3 = 5V
   G#: Pin J2_2:D4 = 5V
   A : Pin J2_2:D5 = 5V
   A#: Pin J2_2:D6 = 5V
   B : Pin J2_2:D7 = 5V

The octave doesn't matter.


For historical reasons the pins can also be controlled with a Modulation Wheel,
and they are displayed on a LCD. This option hasn't been removed for those people
who follow instructions from expired tutorials without reading this README.txt file.
