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

unsigned char stribe_trace_delay;
unsigned char trace_ctr_l[4*64]; // traces of 4 left stribes
unsigned char trace_ctr_r[4*64]; // traces of 4 right stribes

unsigned char previous_value[8]; // previous values (will be cleared with delay in trace mode)

/////////////////////////////////////////////////////////////////////////////
// Initializes the Stribe LEDs
/////////////////////////////////////////////////////////////////////////////
void STRIBE_Init(void) __wparam
{
  // initialize LED drivers
  MAX72XX_Init();

  stribe_flags.TRACE_MODE = 0; // by default disabled
  stribe_flags.TRACE_MASK = 3; // if enabled: trace on left and right bar
  stribe_trace_delay = 50;     // * 5 mS
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick to update the LEDs if requested
/////////////////////////////////////////////////////////////////////////////
void STRIBE_Tick(void) __wparam
{
  // exit if no request
  if( !stribe_flags.LED_UPDATE_REQ )
    return;

  // update all LEDs
  MAX72XX_UpdateAllDigits();

  // clear request
  stribe_flags.LED_UPDATE_REQ = 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called periodically (e.g. each mS) to handle
// the trace mode
/////////////////////////////////////////////////////////////////////////////
void STRIBE_Timer(void) __wparam
{
  unsigned char i;

  // exit if not in trace mode
  if( !stribe_flags.TRACE_MODE )
    return;

  // decrement counters if != 0, turn LED off once counter reached 0
  // could be easily assembly-optimized later to improve performance
  // we toggle between left/right half to ensure, that timer doesn't affect overall performance too much
  // (2 * 256 values need to be processed - the SDCC generated assembly code looks ugly!)
  if( stribe_flags.TIMER_TOGGLE ) {
    stribe_flags.TIMER_TOGGLE = 0;
    i=0;
    do {
      if( trace_ctr_l[i] ) {
	if( !--trace_ctr_l[i] ) {
	  max72xx_digits[i >> 2] &= ~(stribe_flags.TRACE_MASK << ((i&3)<<1));
	  stribe_flags.LED_UPDATE_REQ = 1;
	}
      }
    } while( ++i != 0);
  } else {
    stribe_flags.TIMER_TOGGLE = 1;
    i=0;
    do {
      if( trace_ctr_r[i] ) {
	if( !--trace_ctr_r[i] ) {
	  max72xx_digits[64 + (i >> 2)] &= ~(stribe_flags.TRACE_MASK << ((i&3)<<1));
	  stribe_flags.LED_UPDATE_REQ = 1;
	}
      }
    } while( ++i != 0);
  }
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
    INTCONbits.GIE = 0; // temporary disable IRQs to avoid clash with STRIBE_Timer()
    if( value >= i ) {
      max72xx_digits[offset+63-i] |= or_mask;
    } else {
      max72xx_digits[offset+63-i] &= and_mask;
    }
    INTCONbits.GIE = 1;
  }

  stribe_flags.LED_UPDATE_REQ = 1;
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

  if( stribe_flags.TRACE_MODE ) {
    INTCONbits.GIE = 0; // temporary disable IRQs to avoid clash with STRIBE_Timer()

    // set new LED
    max72xx_digits[offset+63-value] |= or_mask;

    // clear counter of current value (to ensure that LED won't be cleared by accident)
    if( stribe < 4 ) {
      trace_ctr_l[(stribe&3) + ((63-value)<<2)] = 0;
    } else {
      trace_ctr_r[(stribe&3) + ((63-value)<<2)] = 0;
    }

    // clear previous LED with delay
    if( value != previous_value[stribe] ) {
      if( stribe < 4 ) {
	trace_ctr_l[(stribe&3) + ((63-previous_value[stribe])<<2)] = stribe_trace_delay;
      } else {
	trace_ctr_r[(stribe&3) + ((63-previous_value[stribe])<<2)] = stribe_trace_delay;
      }
      previous_value[stribe] = value;
    }

    INTCONbits.GIE = 1;
  } else {
    for(i=0; i<64; ++i) {
      if( value == i ) {
	max72xx_digits[offset+63-i] |= or_mask;
      } else {
	max72xx_digits[offset+63-i] &= and_mask;
      }
    }
  }

  stribe_flags.LED_UPDATE_REQ = 1;
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
      INTCONbits.GIE = 0; // temporary disable IRQs to avoid clash with STRIBE_Timer()
      if( i >= 32 && value >= i ) {
	max72xx_digits[offset+63-i] |= or_mask;
      } else {
	max72xx_digits[offset+63-i] &= and_mask;
      }
      INTCONbits.GIE = 1;
    }
  } else {
    for(i=0; i<64; ++i) {
      INTCONbits.GIE = 0; // temporary disable IRQs to avoid clash with STRIBE_Timer()
      if( i <= 32 && i >= value ) {
	max72xx_digits[offset+63-i] |= or_mask;
      } else {
	max72xx_digits[offset+63-i] &= and_mask;
      }
      INTCONbits.GIE = 1;
    }
  }

  stribe_flags.LED_UPDATE_REQ = 1;
}
