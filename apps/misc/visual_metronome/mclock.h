
/* VISUAL METRONOME
 * MIDI clock module definitions
 *
 * ==========================================================================
 *  Modified by Tim Jefford 2012  (gomiboy99@gmail.com)
 *  Copyright 2005 Thorsten Klose (tk@midibox.org)
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
void SPLASH_Screen(void);
void MCLOCK_Init(void);
void MCLOCK_Tick(void);
void MCLOCK_Timer(void);

void MCLOCK_BPMSet(unsigned char bpm);
unsigned char MCLOCK_BPMGet(void);

void MCLOCK_ResetMeter(void);
void MCLOCK_SendMeter(void);

void METRO_Event_On(void);

void METRO_Event_Off(void);

void MCLOCK_DoStop(void);
void MCLOCK_DoMenu(void);
void MCLOCK_DoPlay(void);
void MCLOCK_DoRew(void);
void MCLOCK_DoFwd(void);

unsigned int MCLOCK_GetTimerValue(unsigned char bpm);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern mclock_state_t mclock_state;
extern mclock_pin_state_t mclock_pin_state;

extern unsigned char mclock_ctr_24;
extern unsigned char mclock_ctr_beats;
extern unsigned char mclock_ctr_measures;
extern unsigned char meas_ctr_beats;

extern unsigned char menu_pos;

extern unsigned char midi_ch;

extern unsigned char clock_enable;

extern unsigned char note_high;

extern unsigned char note_low;

extern unsigned char metro_high;

extern unsigned char metro_low;

extern unsigned char eeprom_location;

#endif /* _MCLOCK_H */
