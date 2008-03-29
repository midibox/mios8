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
    unsigned STRIBE_UPDATE_REQ:1;  // requests a display update
  };
} stribe_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern void STRIBE_Init(void) __wparam;
extern void STRIBE_Tick(void) __wparam;

extern void STRIBE_SetVBar(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;
extern void STRIBE_SetDot(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;
extern void STRIBE_SetPan(unsigned char stribe, unsigned char lr, unsigned char value) __wparam;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern stribe_flags_t stribe_flags;

#endif /* _STRIBE_H */
