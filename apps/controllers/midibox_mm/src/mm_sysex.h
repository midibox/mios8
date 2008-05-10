// $Id$
/*
 * MIDIbox MM V2 (C version)
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MM_SYSEX_H
#define _MM_SYSEX_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// global definitions
/////////////////////////////////////////////////////////////////////////////

#define SYSEX_CMD_STATE_BEGIN 0
#define SYSEX_CMD_STATE_CONT  1
#define SYSEX_CMD_STATE_END   2

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
    unsigned :1;
    unsigned CURSOR_POS_RECEIVED:1;
    unsigned :1;
    unsigned :1;
  };

  struct {
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned :1;
    unsigned POINTER_TYPE_RECEIVED:1;
    unsigned POINTER_POS_RECEIVED:1;
    unsigned :1;
    unsigned :1;
  };
} sysex_state_t;

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_SYSEX_Init(void);
void MM_SYSEX_Parser(unsigned char midi_in);
void MM_SYSEX_CmdFinished(void);
void MM_SYSEX_SendFooter(unsigned char force);
void MM_SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in);
void MM_SYSEX_Cmd_WriteLCD(unsigned char cmd_state, unsigned char midi_in);
void MM_SYSEX_Cmd_WriteValue(unsigned char cmd_state, unsigned char midi_in);
void MM_SYSEX_Cmd_Digits(unsigned char cmd_state, unsigned char midi_in);

#endif /* _MM_SYSEX_H */
