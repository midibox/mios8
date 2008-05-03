$Id$

This application allows you to check the interconnections to the LCD with
a multimeter. You can control the data and control pins by sending a 
Modulation Wheel event to your MIDIbox Core - just connect a keyboard or use
MIDI-Ox (View->Control Panel)

By default all unselected pins are 0V

The Pins are mapped to following Modulation Wheel Values:

   # 0: Pin CORE::J15:D0 (PIC Pin RB0, D0 pin of LCD) = ca. 5V
   # 1: Pin CORE::J15:D1 (PIC Pin RB1, D1 pin of LCD) = ca. 5V
   # 2: Pin CORE::J15:D2 (PIC Pin RB2, D2 pin of LCD) = ca. 5V
   # 3: Pin CORE::J15:D3 (PIC Pin RB3, D3 pin of LCD) = ca. 5V
   # 4: Pin CORE::J15:D4 (PIC Pin RB4, D4 pin of LCD) = ca. 5V
   # 5: Pin CORE::J15:D5 (PIC Pin RB5, D5 pin of LCD) = ca. 5V
   # 6: Pin CORE::J15:D6 (PIC Pin RB6, D6 pin of LCD) = ca. 5V
   # 7: Pin CORE::J15:D7 (PIC Pin RB7, D7 pin of LCD) = ca. 5V
   # 8: Pin CORE::J15:E  (PIC Pin RD7, E pin of LCD) = ca. 5V
   # 9: Pin CORE::J15:RW (PIC Pin RD6, RW pin of LCD) = ca. 5V
   #10: Pin CORE::J15:RS (PIC Pin RD5, RS pin of LCD) = ca. 5V


The pin names are also visible on LCD (if connected and if it works)

If you measure a voltage much less than 4.8V (e.g. < 3V or even 0V), then
there is either a short circuit or a missing connection between 
the signal lines

Please see the schematic for pinnings:	
   http://www.ucapps.de/mbhp/mbhp_core_v3.pdf
