/*
 * CORE 1 PART FOR THE PROTODECK CONTROLLER
 * main.h associated with main.c in the same directory
 *
 *
 * See also http://www.julienbayle.net/diy/protodeck/ for additional informations about protodeck
 * See also http://www.ucapps.de/ for midibox framework informations
 *
 *
 * ==========================================================================
 *
 *  Copyright 2009 protofuse (aka julien.bayle)
 *	Inspired by noofny/mike code
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 *  BIG credits to the midibox framework creator guru: Thorsten Klose
 *
 * ==========================================================================
 */

#ifndef _MAIN_H
#define _MAIN_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#define AIN_NUMBER_INPUTS    32   // number of used analog inputs (0..64) 32
#define AIN_MUXED_MODE         1    // set this to 1 if AIN modules are used
#define AIN_DEADBAND           7    // define deadband here (higher values reduce the effective resolution but reduce jitter)
                                    // 7 is ideal for value range 0..127 (CC events)

#define NUMBER_OF_SRIO        8    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

#define DIN_DEBOUNCE_VALUE    10    // 0..255
#define DIN_TS_SENSITIVITY     0    // optional touch sensor sensitivity: 0..255

#define BLINKING_FEATURE         0    // for testing purpose

// _COLOR_OFF 			0x00	00000000
// _COLOR_RED 			0x10	00010000
// _COLOR_GREEN 		0x20	00100000
// _COLOR_BLUE 			0x40	01000000
// _COLOR_CYAN 			0x60	01100000
// _COLOR_MAGENTA 		0x50	01010000
// _COLOR_YELLOW 		0x30	00110000
// _COLOR_WHITE 		0x70	01110000


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
