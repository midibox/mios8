/*
 * MIOS Pedal Box / Pedal Board - pbx_digits.c
 * v2.5beta3 - April 2009
 * ==========================================================================
 *
 *  Copyright (C) 2009  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */


 ///////////////////////////////////////////////////////////////////////////
 // Driver for 2 x 7 segment LED Digits ( 2 digit decimal input )
 ///////////////////////////////////////////////////////////////////////////

//        a
//        _
//     f|   |b
//       g-
//     e| _ |c
//    o   d
//   dot

const unsigned char LED_digit[10][7] =
//{a,b,c,d,e,f,g}   1 = ON   0 = OFF      // dot not required
{
  {1,1,1,1,1,1,0},   // Zero
  {0,1,1,0,0,0,0},   // One
  {1,1,0,1,1,0,1},   // Two
  {1,1,1,1,0,0,1},   // Three
  {0,1,1,0,0,1,1},   // Four
  {1,0,1,1,0,1,1},   // Five
  {0,0,1,1,1,1,1},   // Six
  {1,1,1,0,0,0,0},   // Seven
  {1,1,1,1,1,1,1},   // Eight
  {1,1,1,0,0,1,1}    // Nine
};

unsigned char decimal_digit1_hlp; // for 1's digit
unsigned char decimal_digit2_hlp; // for 10's digit
unsigned char on_or_off;

void digits_handler (unsigned char decimal_digits)
{
    decimal_digit2_hlp = (decimal_digits / 10);
    decimal_digit1_hlp = decimal_digits - (decimal_digit2_hlp * 10);
    for (i=0; i<7; i++) {
      #if DIGITS_CONNECTED == 2
      //10's
      on_or_off = LED_digit[decimal_digit2_hlp][i];
      MIOS_DOUT_PinSet(i + digits_start_pin + 8, on_or_off);
      #endif
      //1's
      on_or_off = LED_digit[decimal_digit1_hlp][i];
      MIOS_DOUT_PinSet(i + digits_start_pin, on_or_off);
    }

}
