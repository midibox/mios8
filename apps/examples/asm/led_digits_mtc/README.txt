$Id$

LED Digits MTC Example

This example receives MTC code from the MIDI line and displays it
at a 8-digit LED display

LED Digits driver and configuration: see leddigits.inc
MTC decoding: see mtc.inc

see following functions in THIS file how the routines have to be integrated 
into a project:
   o USER_Init (calls LEDDIGITS_Init)
   o USER_SR_Service_Prepare (calls LEDDIGITS_Update)
   o USER_MPROC_NotifyReceivedByte (calls MTC_Handler)
