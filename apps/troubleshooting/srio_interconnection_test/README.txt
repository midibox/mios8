$Id$

CORE->DIN/DOUT Module Interconnection Test V2

This application allows you to check the interconnections to the DIN and DOUT module
with a multimeter. 

You can control the SR/RC/DO pins by sending a MIDI note with the virtual keyboard
of MIOS Studio.

Open the "MIOS Terminal" in MIOS Studio to get some helpful debugging messages
from the application -> see debugging.png as an example setup


The Pins are mapped to following keys:

   C : Pin CORE::J8:SO and DOUT::J1:SO = ca. 5V
   C#: Pin CORE::J8:SC, CORE::J9:SC, DOUT::J1:SC, DIN::J1:SC = ca. 5V
   D : Pin CORE::J8:RC, CORE::J9:RC, DOUT::J1:RC, DIN::J1:RC = ca. 5V

The octave doesn't matter.

If you measure a voltage much less than 4.8V (e.g. < 3V or even 0V), then
there is either a short circuit or a missing connection between 
the signal lines

If a certain shift register is not working, you could measure the
SC/RC voltages directly at the pins of the 74HC595/74HC165, please have
a look into the schematics:
   http://www.ucapps.de/mbhp/mbhp_doutx4.pdf
   http://www.ucapps.de/mbhp/mbhp_dinx4.pdf

The current status of the J9:SI pin is displayed as well.
If you disconnect the DIN module from J9, you can directly set the voltage
level with a cable between J9:SI and Vs (0V), resp. J9:SI and Vd (5V).
If no cable is connected, the pin should read 5V (caused by the Pull-Up 
resistor R9)

The shift registers can be tested with the MIDIO128 application, once you've
ensured that SR/RC are working
Each digital input of the DINX4 module has to trigger a single MIDI event


For historical reasons the pins can also be controlled with a Modulation Wheel,
and they are displayed on a LCD. This option hasn't been removed for those people
who follow instructions from expired tutorials without reading this README.txt file.
