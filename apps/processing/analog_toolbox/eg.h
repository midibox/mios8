// $Id$
/*
 * EG program module definitions
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _EG_H
#define _EG_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// EG state definition
typedef enum {
  IDLE, ATTACK, DECAY, SUSTAIN, RELEASE
} eg_state_e;

// EG internal state type
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    eg_state_e STATE:4;
    unsigned LAST_GATE:1;
  };
} eg_int_state_t;

// EG external state type
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned GATE:1;
  };
} eg_state_t;

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void EG_Init(void);
void EG_Tick(void);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

// in order to achive best performance, we are NOT using arrays here!
extern eg_state_t    eg0;
extern unsigned int  eg0_value;
extern unsigned char eg0_attack, eg0_decay, eg0_sustain, eg0_release;

#endif /* _EG_H */
