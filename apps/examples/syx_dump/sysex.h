// $Id$
/*
 * SysEx Parser Demo
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SYSEX_H
#define _SYSEX_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////

// use checksum protection?
// (disadvantage: files cannot be edited without re-calculating the checksum, therefore disabled by default)
#define SYSEX_CHECKSUM_PROTECTION 0


// command states
#define SYSEX_CMD_STATE_BEGIN 0
#define SYSEX_CMD_STATE_CONT  1
#define SYSEX_CMD_STATE_END   2

// ack/disack code
#define SYSEX_DISACK   0x0e
#define SYSEX_ACK      0x0f

// disacknowledge arguments
#define SYSEX_DISACK_LESS_BYTES_THAN_EXP  0x01
#define SYSEX_DISACK_MORE_BYTES_THAN_EXP  0x02
#define SYSEX_DISACK_WRONG_CHECKSUM       0x03
#define SYSEX_DISACK_BS_NOT_AVAILABLE     0x0a
#define SYSEX_DISACK_INVALID_COMMAND      0x0c


/////////////////////////////////////////////////////////////////////////////
// Type definitions
/////////////////////////////////////////////////////////////////////////////

typedef union {
  struct {
    unsigned ALL:8;
  };

  struct {
    unsigned CTR:3;
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned CMD:1;
    unsigned MY_SYSEX:1;
  };

  struct {
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned PATCH_RECEIVED:1;
    unsigned BANK_RECEIVED:1;
    unsigned :1;
    unsigned :1;
  };
} sysex_state_t;

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void SYSEX_Init(void);
void SYSEX_Send(unsigned char bank, unsigned char patch);
void SYSEX_Parser(unsigned char midi_in);

#endif /* _SYSEX_H */
