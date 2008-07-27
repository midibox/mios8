// $Id$
/*
 * Main header file
 *
 * ==========================================================================
 *
 *  Copyright <year> <your name>
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

#define AIN_NUMBER_INPUTS     64    // number of used analog inputs (0..64)
#define AIN_MUXED_MODE         1    // set this to 1 if AIN modules are used
#define AIN_DEADBAND           7    // define deadband here (higher values reduce the effective resolution but reduce jitter)
                                    // 7 is ideal for value range 0..127 (CC events)

#define NUMBER_OF_SRIO        16    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

#define DIN_DEBOUNCE_VALUE    10    // 0..255
#define DIN_TS_SENSITIVITY     0    // optional touch sensor sensitivity: 0..255

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of application
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
