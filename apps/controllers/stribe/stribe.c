// $Id$
/*
 * Utility functions for the Stribe LEDs
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include <max72xx.h>

#include "main.h"
#include "stribe.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status flags (see bitfield declaration in stribe.h)
stribe_flags_t stribe_flags;


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Initializes the Stribe LEDs
/////////////////////////////////////////////////////////////////////////////
void STRIBE_Init(void) __wparam
{
  // initialize LED drivers
  MAX72XX_Init();
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick to update the LEDs if requested
/////////////////////////////////////////////////////////////////////////////
void STRIBE_Tick(void) __wparam
{
  // exit if no request
  if( !stribe_flags.STRIBE_UPDATE_REQ )
    return;

  // update all LEDs
  MAX72XX_UpdateAllDigits();

  // clear request
  stribe_flags.STRIBE_UPDATE_REQ = 0;
}


/////////////////////////////////////////////////////////////////////////////
// Sets a vertical bar
// IN: stribe: stribe number (0-7)
//     lr: left/right flags (1=left, 2=right, 3=both)
//     value: 6bit value (0-63)
/////////////////////////////////////////////////////////////////////////////
void STRIBE_SetVBar(unsigned char stribe, unsigned char lr, unsigned char value) __wparam
{
  unsigned char and_mask, or_mask, offset, i;

  and_mask = ~(0x3 << ((stribe&3)<<1));
  or_mask = lr << ((stribe&3)<<1);
  offset = ((stribe&4)<<4);

  for(i=0; i<64; ++i) {
    if( value >= i ) {
      max72xx_digits[offset+63-i] |= or_mask;
    } else {
      max72xx_digits[offset+63-i] &= and_mask;
    }
  }

  stribe_flags.STRIBE_UPDATE_REQ = 1;
}


/////////////////////////////////////////////////////////////////////////////
// Sets a single dot
// IN: stribe: stribe number (0-7)
//     lr: left/right flags (1=left, 2=right, 3=both)
//     value: 6bit value (0-63)
/////////////////////////////////////////////////////////////////////////////
void STRIBE_SetDot(unsigned char stribe, unsigned char lr, unsigned char value) __wparam
{
  unsigned char and_mask, or_mask, offset, i;

  and_mask = ~(0x3 << ((stribe&3)<<1));
  or_mask = lr << ((stribe&3)<<1);
  offset = ((stribe&4)<<4);

  for(i=0; i<64; ++i) {
    if( value == i ) {
      max72xx_digits[offset+63-i] |= or_mask;
    } else {
      max72xx_digits[offset+63-i] &= and_mask;
    }
  }

  stribe_flags.STRIBE_UPDATE_REQ = 1;
}


/////////////////////////////////////////////////////////////////////////////
// Sets a pattern like commonly used for panorama knobs
// IN: stribe: stribe number (0-7)
//     lr: left/right flags (1=left, 2=right, 3=both)
//     value: 6bit value (0-63)
/////////////////////////////////////////////////////////////////////////////
void STRIBE_SetPan(unsigned char stribe, unsigned char lr, unsigned char value) __wparam
{
  unsigned char and_mask, or_mask, offset, i;

  and_mask = ~(0x3 << ((stribe&3)<<1));
  or_mask = lr << ((stribe&3)<<1);
  offset = ((stribe&4)<<4);

  if( value >= 32 ) {
    for(i=0; i<64; ++i) {
      if( i >= 32 && value >= i ) {
	max72xx_digits[offset+63-i] |= or_mask;
      } else {
	max72xx_digits[offset+63-i] &= and_mask;
      }
    }
  } else {
    for(i=0; i<64; ++i) {
      if( i <= 32 && i >= value ) {
	max72xx_digits[offset+63-i] |= or_mask;
      } else {
	max72xx_digits[offset+63-i] &= and_mask;
      }
    }
  }

  stribe_flags.STRIBE_UPDATE_REQ = 1;
}
