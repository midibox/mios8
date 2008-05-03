$Id$

LED Digits Example 1

Driving 8 digits with 3 shift registers (DOUTX3)
See also LED Digits Example 2 which drives 16 digits with 4 shift registers (DOUTX4)

The digits at the right side are counting from 9999 to 0000
The digits at the left side are counting from 0x270f to 0x0000 (hexadecimal)

The driver can be found in "leddigits.inc", it has to be customized 
depending on the number of used digits.

see following functions in THIS file how the routines have to be integrated 
into a project:
   o USER_Init (calls LEDDIGITS_Init)
   o USER_SR_Service_Prepare (calls LEDDIGITS_Update)
 
