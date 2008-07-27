$Id$

LCD Performance Measuring Application

This is some kind of benchmark which helps to evaluate the performance
of various character and graphical displays, as well as the execution
speed of a custom LCD driver

16 characters are print 4 times (-> 64 characters), the execution time 
is measured with timer3, prescaler 1:8 - this means, that the counter 
result has to be multiplied by 8 and 100 nS (@40 MHz) to get the
absolute delay

The result will be print on LCD, but also sent via MIDI
(F0 <hex-digit 1> <hex-digit 2> <hex-digit3> <hex-digit4> F7, MSB first)

And here the results:

Displaytech LCDs from Reichelt:
   o 2x16 KS0070B (HD44780 compatible):        3252 * 8 * 100 nS = 2.60 mS
   o 2x20 KS0076B/KS0063 (HD44780 compatible): 3177 * 8 * 100 nS = 2.54 mS
   o 2x40 KS0076B/KS0063 (HD44780 compatible): 3056 * 8 * 100 nS = 2.44 mS
   o KS0107/0108 based 240x64 GLCD (sold out): 6124 * 8 * 100 nS = 4.90 mS

For reference: 2x40 with 4-bit interface, nearly no difference!
   o 2x40 KS0076B/KS0063 (HD44780 compatible): 3108 * 8 * 100 nS = 2.48 mS

Various displays:
   o Matrix Orbital via IIC:           4612 * 8 * 100 nS =  3.69 mS
   o T6963C in vertical mode:	        10647 * 8 * 100 nS =  8.51 mS
   o T6963C in horizontal mode:       37420 * 8 * 100 nS = 29.93 mS (!!!)
