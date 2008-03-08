// $Id$
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

#define NUMBER_OF_SRIO         4    // how many shift registers chained at the DIN/DOUT port (min 1, max 16)

#define DIN_DEBOUNCE_VALUE    10    // 0..255

// define the encoder speed mode and divider here
#define DATAWHEEL_SPEED_MODE     2  // 0 = slow, 1 = normal, 2 = fast
#define DATAWHEEL_SPEED_DIVIDER  3  // see description about MIOS_ENC_SpeedSet

// if set, 4 additional menu-buttons on DIN-Pins 24-27 are used
#define MENU_BUTTONS			 1

// if set, 8 additional MIDI clocks are available at CORE:J5 of the core module
// Attention: don't set this flag to 1 if pots are connected to his junction, otherwise
// short circuits can happen! Therefore this option is 0 by default
#define MULTI_CLOCK_OPTION       0

// define the dout register which should be used to display the status of
// the 8 outputs (which clocks are running)
// 0: disabled
// 1..16: DOUT shift register 1..16
#define MULTI_CLOCK_STATUS_DOUT_SR   1

#define NR_TRK					8		//(number of slave tracks)

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
// Prototypes
/////////////////////////////////////////////////////////////////////////////

int BitTest(unsigned char val, unsigned char bit);
void PatchWrite(unsigned char patch);
void PatchRead(unsigned char patch);
void DataWrite(unsigned char target, unsigned int adr, unsigned char data);
unsigned char DataRead(unsigned char target, unsigned int adr);
unsigned int Mul64(unsigned int c);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
extern app_flags_t app_flags;
extern unsigned char divisor[];
extern unsigned char t_divisor[];
extern unsigned char shuffle[];
extern unsigned char t_shuffle[];
extern unsigned char current_trk;
extern unsigned char out_config_sc_mask;
extern unsigned char out_config_bb_mask;
extern unsigned char out_config_sp_mask;
#endif /* _MAIN_H */
