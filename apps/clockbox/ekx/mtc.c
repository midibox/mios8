// $Id$
/*
 * MTC module
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include "cmios.h"
#include "pic18f452.h"

#include "main.h"
#include "mtc.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

mtc_state_t mtc_state; // the mtc state variable

unsigned char mtc_tick_ctr; // requests MTC events

unsigned char frame_type; // holds the current FPS setting

unsigned char mtc_ctr_send;
unsigned char mtc_ctr_frame_x_4;
unsigned char mtc_ctr_sec;
unsigned char mtc_ctr_min;
unsigned char mtc_ctr_hours;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This function initializes the MIDI clock module
/////////////////////////////////////////////////////////////////////////////
void MTC_Init(void)
{
  mtc_state.ALL = 0;
  mtc_tick_ctr = 0;

  MTC_ResetMeter();
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick to send MIDI clock and
// MIDI Clock Start/Stop/Continue
/////////////////////////////////////////////////////////////////////////////
void MTC_Tick(void)
{
  // start request? Send 0xfa and enter RUN mode
  if( mtc_state.START_REQ ) {
    mtc_state.START_REQ = 0;
    mtc_state.RUN = 1;

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // continue request? Send 0xfb and release pause
  if( mtc_state.CONT_REQ ) {
    mtc_state.CONT_REQ = 0;
    mtc_state.PAUSE = 0;

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // stop request? Send 0xfc and leave  RUN mode
  if( mtc_state.STOP_REQ ) {
    mtc_state.STOP_REQ = 0;
    mtc_state.RUN = 0;
    mtc_state.PAUSE = 0;
    mtc_tick_ctr = 0; // prevent that any requested 0xf8 will be sent

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // send 0xf1 xx events until counter is 0
  while( mtc_tick_ctr ) {
    MIOS_MIDI_TxBufferPut(0xf1);
    switch( mtc_ctr_send ) {
      case 0: MIOS_MIDI_TxBufferPut(0x00 | ((mtc_ctr_frame_x_4 >> 2) & 0x0f)); break;
      case 1: MIOS_MIDI_TxBufferPut(0x10 | ((mtc_ctr_frame_x_4 >> 6) & 0x0f)); break;
      case 2: MIOS_MIDI_TxBufferPut(0x20 | ((mtc_ctr_sec >> 0) & 0x0f)); break;
      case 3: MIOS_MIDI_TxBufferPut(0x30 | ((mtc_ctr_sec >> 4) & 0x0f)); break;
      case 4: MIOS_MIDI_TxBufferPut(0x40 | ((mtc_ctr_min >> 0) & 0x0f)); break;
      case 5: MIOS_MIDI_TxBufferPut(0x50 | ((mtc_ctr_min >> 4) & 0x0f)); break;
      case 6: MIOS_MIDI_TxBufferPut(0x60 | ((mtc_ctr_hours >> 0) & 0x0f)); break;
      case 7:
	MIOS_MIDI_TxBufferPut(0x70 | ((frame_type << 1) & 0x06) | ((mtc_ctr_hours >> 4) & 0x01));
	break;
    }

    if( ++mtc_ctr_send >= 8 )
      mtc_ctr_send = 0;


    // decrementing the counter *MUST* be an atomic operation, otherwise it could
    // conflict with MTC_Timer()
    // however, I guess that the compiler will generate a single decf instruction,
    // which is atomic... but better to write it in this way, who knows, how SDCC
    // will behave in future...
    INTCONbits.GIE = 0; // disable interrupts
    --mtc_tick_ctr;
    INTCONbits.GIE = 1; // enable interrupts

    // increment the meter counters
    if( ++mtc_ctr_frame_x_4 >= 4*30 ) {
      mtc_ctr_frame_x_4 = 0;
      if( ++mtc_ctr_sec >= 60 ) {
	mtc_ctr_sec = 0;
	if( ++mtc_ctr_min >= 60 ) {
	  mtc_ctr_min = 0;
	  ++mtc_ctr_hours;
	}
      }
    }

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Timer to update the MIDI clock
/////////////////////////////////////////////////////////////////////////////
void MTC_Timer(void)
{
  // just increment a clock counter if in RUN and not in PAUSE mode
  // it will be decremented in MTC_Tick - each step sends one 0xf8 event
  // the advantage of this counter is, that a clock event can never get
  // lost, regardless how much the CPU is loaded
  // (however, in this application the CPU is very unbusy, but I prepared
  // this for even more complex programs...)

  if( mtc_state.RUN && !mtc_state.PAUSE) {
    ++mtc_tick_ctr;
  }
}

/////////////////////////////////////////////////////////////////////////////
// These functions are used to set/query the FPS
/////////////////////////////////////////////////////////////////////////////
void MTC_FPSSet(unsigned char rate)
{
  // re-init timer depending on new FPS value
  frame_type = rate;
  // 1:8 divider selected -> clocks = period / 8*100 nS = 1 / (fps * 800 nS)
  switch( frame_type ) {
    case 0: MIOS_TIMER_ReInit(3, 52083 / 4); break; // 24 fps
    case 1: MIOS_TIMER_ReInit(3, 50000 / 4); break; // 25 fps
    case 2: MIOS_TIMER_ReInit(3, 41667 / 4); break; // 30 fps (Drop-Frame)
    case 3: MIOS_TIMER_ReInit(3, 41667 / 4); break; // 30 fps
  }
}

unsigned char MTC_FPSGet(void)
{
  return frame_type;
}


/////////////////////////////////////////////////////////////////////////////
// This function resets the mtc_ctr variables
/////////////////////////////////////////////////////////////////////////////
void MTC_ResetMeter(void)
{
  mtc_ctr_send = 0;
  mtc_ctr_frame_x_4 = 0;
  mtc_ctr_sec = 0;
  mtc_ctr_min = 0;
  mtc_ctr_hours = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function sends the current song position
/////////////////////////////////////////////////////////////////////////////
void MTC_SendMeter(void)
{
  // from http://www.borg.com/~jglatt/tech/midispec.htm:
  // F0 7F cc 01 01 hr mn sc fr F7
  MIOS_MIDI_TxBufferPut(0xf0);
  MIOS_MIDI_TxBufferPut(0x7f);
  MIOS_MIDI_TxBufferPut(0x7f); // "MIDI Channel"
  MIOS_MIDI_TxBufferPut(0x01);
  MIOS_MIDI_TxBufferPut(0x01);
  MIOS_MIDI_TxBufferPut((mtc_ctr_hours & 0x1f) | ((frame_type << 5) & 0x60));
  MIOS_MIDI_TxBufferPut(mtc_ctr_min);
  MIOS_MIDI_TxBufferPut(mtc_ctr_sec);
  MIOS_MIDI_TxBufferPut(mtc_ctr_frame_x_4 / 4);
  MIOS_MIDI_TxBufferPut(0xf7);
}


/////////////////////////////////////////////////////////////////////////////
// These functions are used to control the MTC handler from external
/////////////////////////////////////////////////////////////////////////////
void MTC_DoStop(void)
{
  if( mtc_state.RUN == 0 ) {
    // reset song position of already in stop mode
    MTC_ResetMeter();
    // send Song Position
    MTC_SendMeter();
  }
  // request stop
  mtc_state.STOP_REQ = 1;

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MTC_DoPause(void)
{
  // if in RUN mode:
  if( mtc_state.RUN ) {
    // toggle pause mode
    if( mtc_state.PAUSE ) {
      mtc_state.CONT_REQ = 1;
    } else {
      mtc_state.PAUSE = 1;
    }
  } else {
    // Stop mode: just toggle PAUSE
    mtc_state.PAUSE = mtc_state.PAUSE ? 0 : 1;
  }

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MTC_DoPlay(void)
{
  // reset meter counters
  MTC_ResetMeter();
  // send Song Position
  MTC_SendMeter();
  // request start
  mtc_state.START_REQ = 1;

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MTC_DoPlayPause(void)
{
  // if in RUN mode:
  if( mtc_state.RUN ) {
    // toggle pause mode
    if( mtc_state.PAUSE ) {
      mtc_state.CONT_REQ = 1;
    } else {
      mtc_state.PAUSE = 1;
    }
  } else {
	  // reset meter counters
	  MTC_ResetMeter();
	  // send Song Position
	  MTC_SendMeter();
	  // request start
	  mtc_state.START_REQ = 1;
  }

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}



void MTC_DoRew(void)
{
  // decrement second and reset subcounters
  mtc_ctr_send = 0;
  mtc_ctr_frame_x_4 = 0;
  if( mtc_ctr_sec || mtc_ctr_min || mtc_ctr_hours ) {
    if( mtc_ctr_sec )
      --mtc_ctr_sec;
    else {
      mtc_ctr_sec = 59;
      if( mtc_ctr_min )
        --mtc_ctr_min;
      else {
        mtc_ctr_min = 59;
        if( mtc_ctr_hours )
	  --mtc_ctr_hours;
        else {
	  mtc_ctr_hours = 0;
        }
      }
    }
  }

  // send Song Position
  MTC_SendMeter();

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MTC_DoFwd(void)
{
  // increment measure and reset subcounters
  mtc_ctr_send = 0;
  mtc_ctr_frame_x_4 = 0;
  if( ++mtc_ctr_sec >= 60 ) {
    mtc_ctr_sec = 0;
    if( ++mtc_ctr_min >= 60 ) {
      mtc_ctr_min = 0;
      ++mtc_ctr_hours;
    }
  }

  // send Song Position
  MTC_SendMeter();

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

