// $Id$
/*
 * SysEx Parser
 *
 * ==========================================================================
 *
 *  Copyright (C) 2010 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>

#include "sysex.h"


/////////////////////////////////////////////////////////////////////////////
// Internal Prototypes
/////////////////////////////////////////////////////////////////////////////

static void SYSEX_CmdFinished(void);
static void SYSEX_SendFooter(unsigned char force);
static void SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in);

static void SYSEX_Cmd_InfoRequest(unsigned char cmd_state, unsigned char midi_in);
static void SYSEX_Cmd_Ping(unsigned char cmd_state, unsigned char midi_in);


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

sysex_state_t sysex_state;
unsigned char sysex_cmd;


/////////////////////////////////////////////////////////////////////////////
// Static definitions
/////////////////////////////////////////////////////////////////////////////

static const unsigned char sysex_header[] = { 0xf0, 0x00, 0x00, 0x7e, 0x4e };


/////////////////////////////////////////////////////////////////////////////
// This function initializes the SysEx handler
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Init(void)
{
  sysex_state.ALL = 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function sends a SysEx dump with layout informations
/////////////////////////////////////////////////////////////////////////////
void SYSEX_SendLayoutInfo(void)
{
  int i;
  unsigned char checksum;
  unsigned char c;

  // send header
  for(i=0; i<sizeof(sysex_header); ++i)
    MIOS_MIDI_TxBufferPut(sysex_header[i]);

  // send device ID
  MIOS_MIDI_TxBufferPut(MIOS_MIDI_DeviceIDGet());

  // Command #1 (Layout Info)
  MIOS_MIDI_TxBufferPut(0x01);

  // Number of columns
  MIOS_MIDI_TxBufferPut(16); // TODO: should be configurable

  // Number of rows
  MIOS_MIDI_TxBufferPut(16); // TODO: should be configurable

  // number of LED colours
  MIOS_MIDI_TxBufferPut(2); // TODO: should be configurable

  // number of extra rows
  MIOS_MIDI_TxBufferPut(1);

  // number of extra columns
  MIOS_MIDI_TxBufferPut(1);

  // number of extra buttons (e.g. shift)
  MIOS_MIDI_TxBufferPut(1);

  // send footer
  MIOS_MIDI_TxBufferPut(0xf7);
}


/////////////////////////////////////////////////////////////////////////////
// This function sends a SysEx acknowledge to notify the user about the received command
// expects acknowledge code (e.g. 0x0f for good, 0x0e for error) and additional argument
/////////////////////////////////////////////////////////////////////////////
void SYSEX_SendAck(unsigned char ack_code, unsigned char ack_arg)
{
  int i;
  unsigned char checksum;
  unsigned char c;

  // send header
  for(i=0; i<sizeof(sysex_header); ++i)
    MIOS_MIDI_TxBufferPut(sysex_header[i]);

  // send device ID
  MIOS_MIDI_TxBufferPut(MIOS_MIDI_DeviceIDGet());

  // send ack code and argument
  MIOS_MIDI_TxBufferPut(ack_code);
  MIOS_MIDI_TxBufferPut(ack_arg);

  // send footer
  MIOS_MIDI_TxBufferPut(0xf7);
}


/////////////////////////////////////////////////////////////////////////////
// This function parses an incoming sysex stream for SysEx messages
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Parser(unsigned char midi_in)
{
  // ignore realtime messages (see MIDI spec - realtime messages can 
  // always be injected into events/streams, and don't change the running status)
  if( midi_in >= 0xf8 )
    return;

  // branch depending on state
  if( !sysex_state.MY_SYSEX ) {
    if( (sysex_state.CTR < sizeof(sysex_header) && midi_in != sysex_header[sysex_state.CTR]) ||
	(sysex_state.CTR == sizeof(sysex_header) && midi_in != MIOS_MIDI_DeviceIDGet()) ) {
	// incoming byte doesn't match
	SYSEX_CmdFinished();	
    } else {
      if( ++sysex_state.CTR > sizeof(sysex_header) ) {
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
      	SYSEX_Cmd(SYSEX_CMD_STATE_END, midi_in);
      }
      SYSEX_CmdFinished();
    } else {
      // check if command byte has been received
      if( !sysex_state.CMD ) {
	sysex_state.CMD = 1;
	sysex_cmd = midi_in;
	SYSEX_Cmd(SYSEX_CMD_STATE_BEGIN, midi_in);
      }
      else
	SYSEX_Cmd(SYSEX_CMD_STATE_CONT, midi_in);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called at the end of a sysex command or on 
// an invalid message
/////////////////////////////////////////////////////////////////////////////
static void SYSEX_CmdFinished(void)
{
  // clear all status variables
  sysex_state.ALL = 0;
  sysex_cmd = 0;

  // enable MIDI forwarding again
  MIOS_MPROC_MergerEnable();
}

/////////////////////////////////////////////////////////////////////////////
// This function sends the SysEx footer if merger enabled
// if force == 1, send the footer regardless of merger state
/////////////////////////////////////////////////////////////////////////////
static void SYSEX_SendFooter(unsigned char force)
{
  if( force || (MIOS_MIDI_MergerGet() & 0x01) )
    MIOS_MIDI_TxBufferPut(0xf7);
}

/////////////////////////////////////////////////////////////////////////////
// This function handles the sysex commands
/////////////////////////////////////////////////////////////////////////////
static void SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in)
{
  // enter the commands here
  switch( sysex_cmd ) {
    case 0x00:
      SYSEX_Cmd_InfoRequest(cmd_state, midi_in);
      break;
    case 0x01: // Layout Info
    case 0x0e: // error
      // ignore to avoid feedback loops
      break;
    case 0x0f:
      SYSEX_Cmd_Ping(cmd_state, midi_in);
      break;
    default:
      // unknown command
      SYSEX_SendFooter(0);
      SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_INVALID_COMMAND);
      SYSEX_CmdFinished();      
  }
}


/////////////////////////////////////////////////////////////////////////////
// Command 00: Layout Info Request
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Cmd_InfoRequest(unsigned char cmd_state, unsigned char midi_in)
{
  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      // nothing to do
      break;

    case SYSEX_CMD_STATE_CONT:
      // nothing to do
      break;

    default: // SYSEX_CMD_STATE_END
      SYSEX_SendFooter(0);

      // send layout info
      SYSEX_SendLayoutInfo();
      break;
  }
}


/////////////////////////////////////////////////////////////////////////////
// Command 0F: Ping (just send back acknowledge)
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Cmd_Ping(unsigned char cmd_state, unsigned char midi_in)
{
  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      sysex_state.DATA_RECEIVED = 0;
      break;

    case SYSEX_CMD_STATE_CONT:
      sysex_state.DATA_RECEIVED = 1;
      break;

    default: // SYSEX_CMD_STATE_END
      SYSEX_SendFooter(0);

      // send acknowledge, but only if no data received (to avoid feedback loops)
      if( !sysex_state.DATA_RECEIVED )
	SYSEX_SendAck(SYSEX_ACK, 0x00);
      break;
  }
}
