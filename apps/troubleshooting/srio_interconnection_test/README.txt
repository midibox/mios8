$Id$

A CORE->DIN/DOUT Module Interconnection Test

This application allows you to check the interconnections to the DIN and DOUT module
with a multimeter. You can control the SR/RC/DO pins by sending a Modulation Wheel
event to your MIDIbox Core - just connect a keyboard or use
MIDI-Ox (View->Control Panel)

By default all unselected pins are 0V, except for the CS# pin which is 5V

The Pins are mapped to following Modulation Wheel Values:

   # 0: Pin CORE::J8:DO and DOUT::J1:DO = ca. 5V
   # 1: Pin CORE::J8:SC, CORE::J9:SC, DOUT::J1:SC, DIN::J1:SC = ca. 5V
   # 2: Pin CORE::J8:RC, CORE::J9:RC, DOUT::J1:RC, DIN::J1:RC = ca. 5V

The pin names are also visible on LCD (if connected)

If you measure a voltage much less than 4.8V (e.g. < 3V or even 0V), then
there is either a short circuit or a missing connection between 
the signal lines

If a certain shift register is not working, you could measure the
SC/RC voltages directly at the pins of the 74HC595/74HC165, please have
a look into the schematics:
   http://www.ucapps.de/mbhp/mbhp_doutx4.pdf
   http://www.ucapps.de/mbhp/mbhp_dinx4.pdf

The J9:DI pin can be tested with the MIDIO128 application, 
once you've ensured that SR/RC are working
Each digital input of the DINX4 module has to trigger a single MIDI event
