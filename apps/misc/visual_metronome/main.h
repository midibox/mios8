
/*
 * VISUAL METRONOME
 *
 * ==========================================================================
 *  Modified by Tim Jefford 2012  (gomiboy99@gmail.com)
 *  Copyright 2005 Thorsten Klose (tk@midibox.org)
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

#define NUMBER_OF_SRIO         2    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

#define DIN_DEBOUNCE_VALUE    127    // 0..255

// define the encoder speed mode and divider here
#define DATAWHEEL_SPEED_MODE     1  // 0 = slow, 1 = normal, 2 = fast
#define DATAWHEEL_SPEED_DIVIDER  3  // see description about MIOS_ENC_SpeedSet


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
    unsigned MIDI_CLOCK_ENABLE:1;      //clock enable
    unsigned METRONOME_ENABLE_SET:1;
    unsigned STORE_ENABLE_SET:1;
 //   unsigned RECALL_ENABLE_SET:1;
    unsigned FORMAT_ENABLE_SET:1;

  };
} app_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
extern app_flags_t app_flags;

#endif /* _MAIN_H */
