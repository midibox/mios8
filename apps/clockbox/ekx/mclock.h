// $Id$
/*
 * MIDI clock module definitions
 *
 * ==========================================================================
 *
 *  Copyright 2006-2008:
 *  Thorsten Klose (tk@midibox.org)
 *  Michaël Heyvaert (Mess)
 *  Mike Wanning (Modularkomplex)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _MCLOCK_H
#define _MCLOCK_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of midi clock
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned RUN:1;
    unsigned PAUSE:1;
    unsigned START_REQ:1;
    unsigned STOP_REQ:1;
    unsigned CONT_REQ:1;
  };
} mclock_state_t;

// status of clock output pin
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned CLK_REQ:1;
  };
} mclock_pin_state_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MCLOCK_Init(void);
void MCLOCK_Tick(void);
void MCLOCK_Timer(void);

void MCLOCK_SendMultiPort(unsigned char value, unsigned char mask);

void MCLOCK_BPMSet(unsigned char bpm);
unsigned char MCLOCK_BPMGet(void);

void MCLOCK_ResetMeter(void);
void MCLOCK_SendMeter(void);

void MCLOCK_DoStop(void);
void MCLOCK_DoPause(void);
void MCLOCK_DoPlay(void);
void MCLOCK_DoPlayPause(void);
void MCLOCK_DoRew(void);
void MCLOCK_DoFwd(void);

void MCLOCK_DoMultiPlay(unsigned char out);
void MCLOCK_DoMultiStop(unsigned char out);

unsigned int MCLOCK_GetTimerValue(unsigned char bpm);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern mclock_state_t mclock_state;
extern mclock_pin_state_t mclock_pin_state;
extern app_flags_t app_flags;
extern unsigned char divisor[];
extern unsigned char multi_clk_mask;

extern unsigned char mclock_ctr_96;
extern unsigned char mclock_ctr_beats;
extern unsigned int mclock_ctr_measures;

#endif /* _MCLOCK_H */
