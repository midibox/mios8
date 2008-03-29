// $Id$
/*
 * Stribe header file
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _STRIBE_H
#define _STRIBE_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of application
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned LED_UPDATE_REQ:1;  // requests a LED update
    unsigned TRACE_MODE:1;      // nice looking trace mode
    unsigned TRACE_MASK:2;      // mask for left/right bar (bit 0: left side, bit 1: right side)
    unsigned TIMER_TOGGLE:1;    // toggle flag for timer routine
  };
} stribe_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern void STRIBE_Init(void) __wparam;
extern void STRIBE_Tick(void) __wparam;
extern void STRIBE_Timer(void) __wparam;

extern void STRIBE_SetVBar(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;
extern void STRIBE_SetDot(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;
extern void STRIBE_SetPan(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern stribe_flags_t stribe_flags;
extern unsigned char stribe_trace_delay;

#endif /* _STRIBE_H */
