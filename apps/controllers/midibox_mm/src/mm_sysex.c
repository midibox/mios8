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

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>

#include "main.h"
#include "mm_lcd.h"
#include "mm_sysex.h"
#include "mm_leddigits.h"

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

sysex_state_t sysex_state;

unsigned char sysex_cmd;

unsigned char status_digit_received;

/////////////////////////////////////////////////////////////////////////////
// Static definitions
/////////////////////////////////////////////////////////////////////////////
const unsigned char sysex_header[] = { 0xf0, 0x00, 0x01, 0x0f, 0x00, 0x11, 0x00 };


/////////////////////////////////////////////////////////////////////////////
// This function initializes the SysEx handler
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_Init(void)
{
   sysex_state.ALL = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function parses an incoming sysex stream for MM messages
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_Parser(unsigned char midi_in)
{
  // ignore realtime messages
  if( midi_in >= 0xf8 )
    return;

  // branch depending on state
  if( !sysex_state.MY_SYSEX ) {
    if( midi_in != sysex_header[sysex_state.CTR] ) {
      // incoming byte doesn't match
      MM_SYSEX_CmdFinished();
    } else {
      if( ++sysex_state.CTR == sizeof(sysex_header) ) {
	// complete header received, waiting for data
	sysex_state.MY_SYSEX = 1;
	// disable merger forwarding until end of sysex message
	MIOS_MPROC_MergerDisable();
      }
    }
  } else {
    // check for end of SysEx message or invalid status byte
    if( midi_in >= 0x80 ) {
      if( midi_in == 0xf7 && sysex_state.CMD ) {
      	MM_SYSEX_Cmd(SYSEX_CMD_STATE_END, midi_in);
      }
      MM_SYSEX_CmdFinished();
    } else {
      // check if command byte has been received
      if( !sysex_state.CMD ) {
	sysex_state.CMD = 1;
	sysex_cmd = midi_in;
	MM_SYSEX_Cmd(SYSEX_CMD_STATE_BEGIN, midi_in);
      }
      else
	MM_SYSEX_Cmd(SYSEX_CMD_STATE_CONT, midi_in);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called at the end of a sysex command or on 
// an invalid message
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_CmdFinished(void)
{
  // clear all status variables
  sysex_state.ALL = 0;
  sysex_cmd = 0;

  status_digit_received = 0;

  // enable MIDI forwarding again
  MIOS_MPROC_MergerEnable();
}

/////////////////////////////////////////////////////////////////////////////
// This function sends the SysEx footer if merger enabled
// if force == 1, send the footer regardless of merger state
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_SendFooter(unsigned char force)
{
  if( force || (MIOS_MIDI_MergerGet() & 0x01) )
    MIOS_MIDI_TxBufferPut(0xf7);
}

/////////////////////////////////////////////////////////////////////////////
// This function handles the sysex commands
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in)
{
  switch( sysex_cmd ) {
    case 0x10:
      MM_SYSEX_Cmd_WriteLCD(cmd_state, midi_in);
      break;
    case 0x11:
      MM_SYSEX_Cmd_WriteValue(cmd_state, midi_in);
      break;
    case 0x12:
      MM_SYSEX_Cmd_Digits(cmd_state, midi_in);
      break;
    default:
      MM_SYSEX_CmdFinished();      
  }
}

/////////////////////////////////////////////////////////////////////////////
// Host application sends characters
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_Cmd_WriteLCD(unsigned char cmd_state, unsigned char midi_in)
{
  unsigned char cursor;

  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      // nothing to do
      break;

    case SYSEX_CMD_STATE_CONT:
      if( !sysex_state.CURSOR_POS_RECEIVED ) {
	sysex_state.CURSOR_POS_RECEIVED = 1;
	MM_LCD_Msg_CursorSet(midi_in < 40 ? midi_in : (midi_in - 40 + 0x40));
      } else {
	cursor = MM_LCD_Msg_CursorGet();

	// if second line reached and pointers are print - 
	// remove them and print text instead
	if( cursor >= 0x40 && mm_flags.PRINT_POINTERS ) {
	  mm_flags.PRINT_POINTERS = 0;
	  MM_LCD_Msg_Refresh();
	}

	// print character
	MM_LCD_Msg_PrintHost(cursor, midi_in);
      }
      break;

    default: // SYSEX_CMD_STATE_END
      MM_SYSEX_SendFooter(0);
      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Host application sends "rotary pointer" values
/////////////////////////////////////////////////////////////////////////////
void MM_SYSEX_Cmd_WriteValue(unsigned char cmd_state, unsigned char midi_in)
{
  static unsigned char pointer_pos;

  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      // nothing to do
      break;

    case SYSEX_CMD_STATE_CONT:
      if( !sysex_state.POINTER_TYPE_RECEIVED ) {
	sysex_state.POINTER_TYPE_RECEIVED = 1;
	MM_LCD_PointerInit(midi_in); // init pointer type
      } else if( !sysex_state.POINTER_POS_RECEIVED ) {
	sysex_state.POINTER_POS_RECEIVED = 1;
        pointer_pos = midi_in & 0x07; // store pointer position
      } else {
	MM_LCD_PointerSet(pointer_pos, midi_in); // set pointer value
      }
      break;

    default: // SYSEX_CMD_STATE_END
      MM_SYSEX_SendFooter(0);
      break;
  }
}

void MM_SYSEX_Cmd_Digits(unsigned char cmd_state, unsigned char midi_in)
{
  static unsigned char digit_ctr;
  static unsigned char pattern;

  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      digit_ctr = 0;
      break;

    case SYSEX_CMD_STATE_CONT:

      switch( ++digit_ctr ) {
        case 1:
	  pattern = (midi_in << 4);
	  if( midi_in & (1 << 6) )
	    pattern |= 0x80;
	  break;

        case 2:
	  MM_LEDDIGITS_Set(0, pattern | (midi_in & 0x0f));
	  break;

        case 3:
	  pattern = (midi_in << 4);
	  if( midi_in & (1 << 6) )
	    pattern |= 0x80;
	  break;

        case 4:
	  MM_LEDDIGITS_Set(1, pattern | (midi_in & 0x0f));
	  break;
      }

      break;

    default: // SYSEX_CMD_STATE_END
      // nothing to do
      MM_SYSEX_SendFooter(0);
      break;
  }
}
