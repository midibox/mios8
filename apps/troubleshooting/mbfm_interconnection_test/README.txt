$Id$

A CORE->MBHP_OPL3 Module Interconnection Test

This application allows you to check the interconnections to the OPL3 module
with a multimeter. You can control the OPL3 pins by sending a Modulation Wheel
event or a Note event to your MIDIbox FM - just connect a keyboard or use
MIDI-Ox (View->Control Panel) or the virtual keyboard of MIOS Studio


By default all unselected pins are 0V

The Pins are mapped to following Modulation Wheel Values:

   # 0: Pin J2_1:RS = 5V
   # 1: Pin J2_1:A0 = 5V
   # 2: Pin J2_1:A1 = 5V
   # 3: Pin J2_1:WR = 5V
   # 4: Pin J2_1:D0 = 5V
   # 5: Pin J2_1:D1 = 5V
   # 6: Pin J2_2:D2 = 5V
   # 7: Pin J2_2:D3 = 5V
   # 8: Pin J2_2:D4 = 5V
   # 9: Pin J2_2:D5 = 5V
   #10: Pin J2_2:D6 = 5V
   #11: Pin J2_2:D7 = 5V


And to following Note On Events (Keys):

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

(the octave doesn't matter)

The pin names are also visible on LCD (if connected)
