$Id$

This program demonstrates, how PORTA and PORTE can be used to switch
LEDs, Relais, Transmission Gates or similar.
 
These ports can be used when only a small number of output pins are
required so that the DOUTX4 module would be overkill, and when 
the application doesn't allocate the analog input lines.
 
The DOUT pins are switched via MIDI events in this example.
By default they are assigned to the Notes C-4, D-4, E-4, F-4, G-4, A-4, H-4, C-5
The events can be changed in mios_tables.inc

The J5_IO routines are located in $MIOS_PATH/modules/j5_io to simplify the reuse
in other projects
