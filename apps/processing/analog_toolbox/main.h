// $Id$
/*
 * Analog Toolbox
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
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

#define AIN_NUMBER_INPUTS      8    // number of used analog inputs (0..64)
#define AIN_MUXED_MODE         0    // set this to 1 if AIN modules are used
#define AIN_DEADBAND           3    // define deadband here (higher values reduce the effective resolution but reduce jitter)

// Note: AOUT specific settings are defined in aout.h


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

extern unsigned char last_ain_pin;

#endif /* _MAIN_H */
