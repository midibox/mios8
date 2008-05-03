$Id$

Using J5 as DIN
 
This program demonstrates, how PORTA and PORTE can be used to connect
up to 8 button or switches without using a shiftregister (MBHP_DINX module)
 
These ports can be used when only a small number of inputs pins are
required so that the DINX4 module would be overkill, and when 
the application doesn't allocate the analog input lines.
 
The DIN pins are triggering MIDI events in this example.
By default they are assigned to the Notes C-4, D-4, E-4, F-4, G-4, A-4,
to the Sustain Pedal controller and to a Program Change Event
The events can be changed in the MIDI_EVENT_TABLE at the end of this file

The J5_DIN routines are located in "j5_din.inc" and $MIOS_PATH/modules/j5_io
to simplify the reuse in other projects

see following functions in main.asm how the routines have to be integrated 
into a project:
   o USER_Init (calls J5_DIN_Init)
   o USER_SR_Service_Finish (calls J5_DIN_Update)
   o USER_Tick (calls J5_DIN_Handler)
   o USER_J5_DIN_NotifyToggle (see code)
