/*
 * Main header file
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MAIN_H
#define _MAIN_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#define NUMBER_OF_SRIO        16    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of analog toolbox application
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned DISPLAY_UPDATE_REQ:1;  // requests a display update
  };
} app_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
extern app_flags_t app_flags;

#endif /* _MAIN_H */
