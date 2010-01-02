$Id$

This application allows you to check the interconnections to the LCD with
a multimeter.

You can control the LCD pins by sending a MIDI note with the virtual keyboard
of MIOS Studio.

Open the "MIOS Terminal" in MIOS Studio to get some helpful debugging messages
from the application -> see debugging.png as an example setup


The Pins are mapped to following keys:

   C : Pin CORE::J15:D0 (PIC Pin RB0, D0 pin of LCD) = ca. 5V
   C#: Pin CORE::J15:D1 (PIC Pin RB1, D1 pin of LCD) = ca. 5V
   D : Pin CORE::J15:D2 (PIC Pin RB2, D2 pin of LCD) = ca. 5V
   D#: Pin CORE::J15:D3 (PIC Pin RB3, D3 pin of LCD) = ca. 5V
   E : Pin CORE::J15:D4 (PIC Pin RB4, D4 pin of LCD) = ca. 5V
   F : Pin CORE::J15:D5 (PIC Pin RB5, D5 pin of LCD) = ca. 5V
   F#: Pin CORE::J15:D6 (PIC Pin RB6, D6 pin of LCD) = ca. 5V
   G : Pin CORE::J15:D7 (PIC Pin RB7, D7 pin of LCD) = ca. 5V
   G#: Pin CORE::J15:E  (PIC Pin RD7, E pin of LCD) = ca. 5V
   A : Pin CORE::J15:RW (PIC Pin RD6, RW pin of LCD) = ca. 5V
   A#: Pin CORE::J15:RS (PIC Pin RD5, RS pin of LCD) = ca. 5V

The octave doesn't matter.

If you measure a voltage much less than 4.8V (e.g. < 3V or even 0V), then
there is either a short circuit or a missing connection between 
the signal lines

Please see the schematic for pinnings:	
   http://www.ucapps.de/mbhp/mbhp_core_v3.pdf


For historical reasons the pins can also be controlled with a Modulation Wheel,
and they are displayed on a LCD (if connected and if it works). This option 
hasn't been removed for those people who follow instructions from expired tutorials
without reading this README.txt file.
