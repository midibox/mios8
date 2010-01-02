$Id$

CORE->SID Module Interconnection Test V2

This application allows you to check the interconnections to the SID module
with a multimeter.

You can control the SID pins by sending a MIDI note with the virtual keyboard
of MIOS Studio.

Open the "MIOS Terminal" in MIOS Studio to get some helpful debugging messages
from the application -> see debugging.png as an example setup


The SID pins are mapped to following keys:

   C : Pin A0 = 5V
   C#: Pin A1 = 5V
   D : Pin A2 = 5V
   D#: Pin A3 = 5V
   E : Pin A4 = 5V
   F : Pin RES# = 5V
   F#: Pin D0 = 5V
   G : Pin D1 = 5V
   G#: Pin D2 = 5V
   A : Pin D3 = 5V
   A# : Pin D4 = 5V
   B : Pin D5 = 5V
   C  (next octave): Pin D6 = 5V
   C# (next octave): Pin D7 = 5V
   D  (next octave): Pin CS# = 0V

The octave doesn't matter, keys are mirroed each second octave.


For historical reasons the pins can also be controlled with a Modulation Wheel,
and they are displayed on a LCD. This option hasn't been removed for those people
who follow instructions from expired tutorials without reading this README.txt file.
