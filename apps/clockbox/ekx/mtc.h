// $Id$
/*
 * MTC module definitions
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _MTC_H
#define _MTC_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of MTC
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
} mtc_state_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MTC_Init(void);
void MTC_Tick(void);
void MTC_Timer(void);

void MTC_FPSSet(unsigned char rate);
unsigned char MTC_FPSGet(void);

void MTC_ResetMeter(void);
void MTC_SendMeter(void);

void MTC_DoStop(void);
void MTC_DoPause(void);
void MTC_DoPlay(void);
void MTC_DoPlayPause(void);
void MTC_DoRew(void);
void MTC_DoFwd(void);

unsigned int MTC_GetTimerValue(unsigned char rate);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern mtc_state_t mtc_state;

extern unsigned char mtc_ctr_frame_x_4;
extern unsigned char mtc_ctr_sec;
extern unsigned char mtc_ctr_min;
extern unsigned char mtc_ctr_hours;

#endif /* _MTC_H */
