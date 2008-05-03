$Id$

Jitter monitor

Displays the jitter value of an analog input pin

The AIN pin has to be selected with two buttons (Dec/Inc) at DIN #0 and #1
(note: every "even" pin acts as a DEC button, every "odd" pin acts
 as an INC button, so you can also use buttons which are not connected
 to DIN#0 and DIN#1)

In main.asm header, set "#define AIN_MUXED" to 1 if multiplexers are connected
