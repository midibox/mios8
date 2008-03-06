/*
 * Clockbox
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
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

#define NUMBER_OF_SRIO         3    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

#define DIN_DEBOUNCE_VALUE    10    // 0..255

// define the encoder speed mode and divider here
#define DATAWHEEL_SPEED_MODE     2  // 0 = slow, 1 = normal, 2 = fast
#define DATAWHEEL_SPEED_DIVIDER  3  // see description about MIOS_ENC_SpeedSet

// if set, MTC will be sent instead of MIDI clock
#define MTC_MODE                 0

// if set, 8 additional MIDI clocks are available at CORE:J5 of the core module
// Attention: don't set this flag to 1 if pots are connected to his junction, otherwise
// short circuits can happen! Therefore this option is 0 by default
#define MULTI_CLOCK_OPTION       0

// define the dout register which should be used to display the status of 
// the 8 outputs (which clocks are running)
// 0: disabled
// 1..16: DOUT shift register 1..16
#define MULTI_CLOCK_STATUS_DOUT_SR   1

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
