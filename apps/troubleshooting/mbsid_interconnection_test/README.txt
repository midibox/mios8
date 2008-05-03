$Id$

A CORE->SID Module Interconnection Test

This application allows you to check the interconnections to the SID module
with a multimeter. You can control the SID pins by sending a Modulation Wheel
event to your MIDIbox SID - just connect a keyboard or use
MIDI-Ox (View->Control Panel)

By default all unselected pins are 0V, except for the CS# pin which is 5V

The Pins are mapped to following Modulation Wheel Values:

   # 0: Pin A0 = 5V
   # 1: Pin A1 = 5V
   # 2: Pin A2 = 5V
   # 3: Pin A3 = 5V
   # 4: Pin A4 = 5V
   # 5: Pin RES# = 5V
   # 6: Pin D0 = 5V
   # 7: Pin D1 = 5V
   # 8: Pin D2 = 5V
   # 9: Pin D3 = 5V
   #10: Pin D4 = 5V
   #11: Pin D5 = 5V
   #12: Pin D6 = 5V
   #13: Pin D7 = 5V
   #14: Pin CS# = 0V

The pin names are also visible on LCD (if connected)
