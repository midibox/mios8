$Id$

MBHP_TV by Thorsten Klose
=========================

Inspired from the great PIC video projects of Rickard Gunee
at http://www.rickard.gunee.com/projects/
 
See also his well written HowTo if you want to understand the
driver in video.inc
 
Note: THIS IS A STANDALONE APPLICATION
due to the extreme high CPU load and the required realtime 
capability the Video handler bypasses the whole operating system, 
all interrupts are disabled and no additional MBHP extension (DIN/DOUT/AIN
modules) can be serviced.
Also the upload of new code will not work - you have to use the 
first level bootstrap loader (during startup) in order to exchange
the application

Following SysEx strings are supported:
   o Clear Screen:
        f0 00 00 7e 3f 00 00 f7
   o Set Cursor Pos and Print Characters:
        f0 00 00 7e 3f 00 01 <pos> <char_1> <char_2> ... <char_n> f7
   o Print Characters (don't set cursor):
        f0 00 00 7e 3f 00 02 <char_1> <char_2> ... <char_n> f7
