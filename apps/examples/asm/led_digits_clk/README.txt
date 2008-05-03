$Id$

LED Digits MIDI Clock Example

This example receives the MIDI clock from the MIDI line and displays it
at a 8-digit LED display

LED Digits driver and configuration: see leddigits.inc
MIDI Clock decoding: see midiclk.inc

see following functions in THIS file how the routines have to be integrated 
into a project:
   o USER_Init (calls LEDDIGITS_Init)
   o USER_SR_Service_Prepare (calls LEDDIGITS_Update)

   o USER_MPROC_NotifyReceivedByte (calls MIDICLK_Handler)

