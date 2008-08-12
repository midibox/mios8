// $Id$
/*
 * SysEx Parser Demo
 * see README.txt for details
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>

#include "patch.h"
#include "sysex.h"


/////////////////////////////////////////////////////////////////////////////
// Internal Prototypes
/////////////////////////////////////////////////////////////////////////////

void SYSEX_CmdFinished(void);
void SYSEX_SendFooter(unsigned char force);
void SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in);

void SYSEX_Cmd_ReadPatch(unsigned char cmd_state, unsigned char midi_in);
void SYSEX_Cmd_WritePatch(unsigned char cmd_state, unsigned char midi_in);
void SYSEX_Cmd_Ping(unsigned char cmd_state, unsigned char midi_in);


/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

sysex_state_t sysex_state;
unsigned char sysex_cmd;

unsigned char sysex_bank;
unsigned char sysex_patch;
unsigned char sysex_checksum;
unsigned char sysex_received_checksum;
unsigned int  sysex_receive_ctr;


/////////////////////////////////////////////////////////////////////////////
// Static definitions
/////////////////////////////////////////////////////////////////////////////

// should be changed for your own application
// Headers used by MIDIbox applications are documented here:
// http://svnmios.midibox.org/filedetails.php?repname=svn.mios&path=%2Ftrunk%2Fdoc%2FSYSEX_HEADERS
// if you decide to use "F0 00 00 7E" prefix, please ensure that your
// own ID (here: 0x7f) will be entered into this document.
// Otherwise please use a different header
const unsigned char sysex_header[] = { 0xf0, 0x00, 0x00, 0x7e, 0x7f };


/////////////////////////////////////////////////////////////////////////////
// This function initializes the SysEx handler
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Init(void)
{
  sysex_state.ALL = 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function sends a SysEx dump of the patch structure
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Send(unsigned char bank, unsigned char patch)
{
  int i;
  unsigned char checksum;
  unsigned char c;

  // send header
  for(i=0; i<sizeof(sysex_header); ++i)
    MIOS_MIDI_TxBufferPut(sysex_header[i]);

  // "write patch" command (so that dump could be sent back to overwrite EEPROM w/o modifications)
  MIOS_MIDI_TxBufferPut(0x02);

  // send bank and patch number
  MIOS_MIDI_TxBufferPut(bank);
  MIOS_MIDI_TxBufferPut(patch);

  // send patch content
  for(checksum=0, i=0; i<256; ++i) {
    c = PATCH_ReadByte((unsigned char)i);

    MIOS_MIDI_TxBufferPut(c & 0x7f);
    checksum += c;
  }

#if SYSEX_CHECKSUM_PROTECTION
  // send checksum
  MIOS_MIDI_TxBufferPut((checksum ^ 0xff) & 0x7f);
#endif

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
    if( midi_in != sysex_header[sysex_state.CTR] ) {
      // incoming byte doesn't match
      SYSEX_CmdFinished();
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
void SYSEX_CmdFinished(void)
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
void SYSEX_SendFooter(unsigned char force)
{
  if( force || (MIOS_MIDI_MergerGet() & 0x01) )
    MIOS_MIDI_TxBufferPut(0xf7);
}

/////////////////////////////////////////////////////////////////////////////
// This function handles the sysex commands
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Cmd(unsigned char cmd_state, unsigned char midi_in)
{
  // enter the commands here
  switch( sysex_cmd ) {
    case 0x01:
      SYSEX_Cmd_ReadPatch(cmd_state, midi_in);
      break;
    case 0x02:
      SYSEX_Cmd_WritePatch(cmd_state, midi_in);
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
// Command 01: Read Patch handler
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Cmd_ReadPatch(unsigned char cmd_state, unsigned char midi_in)
{
  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      // nothing to do
      break;

    case SYSEX_CMD_STATE_CONT:
      if( !sysex_state.BANK_RECEIVED ) {
	sysex_bank = midi_in; // store bank number
	sysex_state.BANK_RECEIVED = 1;
      } else if( !sysex_state.PATCH_RECEIVED ) {
	sysex_patch = midi_in; // store patch number
	sysex_state.PATCH_RECEIVED = 1;
      } else {
	// wait for 0xf7
      }
      break;

    default: // SYSEX_CMD_STATE_END
      SYSEX_SendFooter(0);

      // bank and patch received? (PATCH_RECEIVED implies that BANK_RECEIVED already set)
      if( !sysex_state.PATCH_RECEIVED ) {
	// not enough bytes received
	SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_LESS_BYTES_THAN_EXP);
      } else {
	// load patch
	if( PATCH_Load(sysex_bank, sysex_patch) ) {
	  // read failed (bankstick not available)
	  SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_BS_NOT_AVAILABLE);
	} else {
	  // send dump
	  SYSEX_Send(sysex_bank, sysex_patch);
	}
      }

      break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Command 02: Write Patch handler
/////////////////////////////////////////////////////////////////////////////
void SYSEX_Cmd_WritePatch(unsigned char cmd_state, unsigned char midi_in)
{
  switch( cmd_state ) {

    case SYSEX_CMD_STATE_BEGIN:
      sysex_checksum = 0; // clear checksum
      sysex_receive_ctr = 0; // clear byte counter
      sysex_received_checksum = 0;
      break;

    case SYSEX_CMD_STATE_CONT:
      if( !sysex_state.BANK_RECEIVED ) {
	sysex_bank = midi_in; // store bank number
	sysex_state.BANK_RECEIVED = 1;
      } else if( !sysex_state.PATCH_RECEIVED ) {
	sysex_patch = midi_in; // store patch number
	sysex_state.PATCH_RECEIVED = 1;
      } else {
	if( sysex_receive_ctr < 0x100 ) {
	  // new byte has been received - put it into patch structure
	  PATCH_WriteByte((unsigned char)sysex_receive_ctr, midi_in);
	  // add to checksum
	  sysex_checksum += midi_in;
#if SYSEX_CHECKSUM_PROTECTION
	} else if( sysex_receive_ctr == 0x100 ) {
	  // store received checksum
	  sysex_received_checksum = midi_in;
#endif
	} else {
	  // wait for F7
	}

	// increment counter
	++sysex_receive_ctr;
      }
      break;

    default: // SYSEX_CMD_STATE_END
      SYSEX_SendFooter(0);

#if SYSEX_CHECKSUM_PROTECTION
      if( sysex_receive_ctr < 0x101 ) {
#else
      if( sysex_receive_ctr < 0x100 ) {
#endif
	// not enough bytes received
	SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_LESS_BYTES_THAN_EXP);
#if SYSEX_CHECKSUM_PROTECTION
      } else if( sysex_receive_ctr > 0x101 ) {
#else
      } else if( sysex_receive_ctr > 0x100 ) {
#endif
	// too many bytes received
	SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_MORE_BYTES_THAN_EXP);
#if SYSEX_CHECKSUM_PROTECTION
      } else if( sysex_received_checksum != ((sysex_checksum ^ 0xff) & 0x7f) ) {
	// notify that wrong checksum has been received
	SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_WRONG_CHECKSUM);
#endif
      } else {
	// write patch
	if( PATCH_Store(sysex_bank, sysex_patch) ) {
	  // write failed (bankstick not available)
	  SYSEX_SendAck(SYSEX_DISACK, SYSEX_DISACK_BS_NOT_AVAILABLE);
	} else {
	  // notify that bytes have been received
	  SYSEX_SendAck(SYSEX_ACK, 0x00);

	  // print message
	  MIOS_LCD_Clear();
	  MIOS_LCD_CursorSet(0x00);
	  MIOS_LCD_PrintCString("Patch: ");
	  MIOS_LCD_PrintBCD3(sysex_patch);
	  MIOS_LCD_PrintCString(" B: ");
	  MIOS_LCD_PrintBCD1(sysex_bank);

	  MIOS_LCD_CursorSet(0x40);
	  MIOS_LCD_PrintCString("Dump received!");
	  MIOS_LCD_MessageStart(0xff); // temporary message
	}
      }
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
      // nothing to do
      break;

    case SYSEX_CMD_STATE_CONT:
      // nothing to do
      break;

    default: // SYSEX_CMD_STATE_END
      SYSEX_SendFooter(0);

      // send acknowledge
      SYSEX_SendAck(SYSEX_ACK, 0x00);

      break;
  }
}


