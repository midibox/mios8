// $Id$
/*
 * MIOS based firmware for Stribe Project
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
#include <pic18fregs.h>

#include <max72xx.h>

#include "main.h"
#include "stribe.h"


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

// for filter function
unsigned char ain_filter_delay;
unsigned char last_conv_value[8];
unsigned char last_conv_ctr[8];

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  unsigned char stribe;

  // initialize LED drivers
  STRIBE_Init();
  stribe_flags.TRAIL_MODE = 1;
  stribe_flags.TRAIL_MASK = 3; // (trail on both bars))

  // initialize timer
  MIOS_TIMER_Init(0x00, 50000); // 5 mS period

  // initialize AIN driver
  MIOS_AIN_NumberSet(8);
  MIOS_AIN_UnMuxed();
  MIOS_AIN_DeadbandSet(7);

  // enable single channel mode by default (can be changed via CC)
  app_flags.SINGLE_CHN_MODE = 1;

  // initial filter delay
  ain_filter_delay = 20; // *5 mS

  // set dots at lowest position
  for(stribe=0; stribe<8; ++stribe) {
    STRIBE_SetDot(stribe, stribe_flags.TRAIL_MODE ? stribe_flags.TRAIL_MASK : 3, 0);
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // update Stribe LEDs if requested
  STRIBE_Tick();
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
  unsigned char i;

  // handle trail counters
  STRIBE_Timer();

  // handle AIN filter counters
  for(i=0; i<8; ++i) {
    if( last_conv_ctr[i] )
      --last_conv_ctr[i];
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
  MIOS_LCD_Clear();

  // print static screen
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString("Stribe");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  unsigned char i;

  // do nothing if no update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;

  // clear request
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // display AIN values
  MIOS_LCD_CursorSet(0x00 + 0);
  for(i=0; i<4; ++i) {
    MIOS_LCD_PrintBCD3(MIOS_AIN_Pin7bitGet(i));
    MIOS_LCD_PrintChar(' ');
  }

  MIOS_LCD_CursorSet(0x40 + 0);
  for(i=4; i<8; ++i) {
    MIOS_LCD_PrintBCD3(MIOS_AIN_Pin7bitGet(i));
    MIOS_LCD_PrintChar(' ');
  }
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char stribe;
  unsigned char mode;

  if( (evnt0 & 0xf0) == 0xb0 ) {

    // special CCs:
    if( (evnt0 & 0x0f) == (STRIBE_MIDI_CHANNEL-1) ) { // defined in main.h
      switch( evnt1 ) {
        case 0x78: // CC#120 turns on/off single channel mode
	  app_flags.SINGLE_CHN_MODE = evnt2 ? 1 : 0;
	  break;
        case 0x79: // CC#121 sets filter delay
	  ain_filter_delay = evnt2; // *5 mS
	  break;
        case 0x7a: // CC#122 sets trail delay
	  if( evnt2 > 0 ) {
	    stribe_flags.TRAIL_MODE = 1;
	    stribe_trail_delay = evnt2;
	  } else {
	    stribe_flags.TRAIL_MODE = 0;
	  }
	  break;
        case 0x7b: // CC#123 sets trail mask
	  stribe_flags.TRAIL_MASK = evnt2 & 3;
	  break;
      }
    }


    // branch depending on channel mode
    if( app_flags.SINGLE_CHN_MODE ) {
      if( (evnt0 & 0x0f) == (STRIBE_MIDI_CHANNEL-1) ) { // defined in main.h

	stribe = evnt1 & 0x07;
	mode = evnt1 >> 3;

	switch( evnt1 >> 3 ) {
	  // CC#16..#23, #24..#31, #32..#39 are used to set single dot
  	  case 0x10>>3: STRIBE_SetDot(stribe, 3, evnt2 >> 1);  break;
  	  case 0x18>>3: STRIBE_SetDot(stribe, 1, evnt2 >> 1);  break;
  	  case 0x20>>3: STRIBE_SetDot(stribe, 2, evnt2 >> 1);  break;

	  // CC#48..#55, #56..#63, #64..71 are used to set vertical bar
  	  case 0x30>>3: STRIBE_SetVBar(stribe, 3, evnt2 >> 1);  break;
  	  case 0x38>>3: STRIBE_SetVBar(stribe, 1, evnt2 >> 1);  break;
  	  case 0x40>>3: STRIBE_SetVBar(stribe, 2, evnt2 >> 1);  break;

	  // CC#80..#87, #88..#95, #96..103 are used to set vertical bar
  	  case 0x50>>3: STRIBE_SetPan(stribe, 3, evnt2 >> 1);  break;
  	  case 0x58>>3: STRIBE_SetPan(stribe, 1, evnt2 >> 1);  break;
  	  case 0x60>>3: STRIBE_SetPan(stribe, 2, evnt2 >> 1);  break;
	}
      }
    }
  } else {

    if( (evnt0 & 0x0f) < 8 ) {
      // CC over MIDI Channel #1..8

      stribe = evnt0 & 0x07;
      switch( evnt1 ) {
	// CC16..18 are used to set single dot
        case 0x10: STRIBE_SetDot(stribe, 3, evnt2 >> 1);  break;
        case 0x11: STRIBE_SetDot(stribe, 1, evnt2 >> 1);  break;
        case 0x12: STRIBE_SetDot(stribe, 2, evnt2 >> 1);  break;

	// CC20..22 are used to set Vertical bar
        case 0x14: STRIBE_SetVBar(stribe, 3, evnt2 >> 1); break;
        case 0x15: STRIBE_SetVBar(stribe, 1, evnt2 >> 1); break;
        case 0x16: STRIBE_SetVBar(stribe, 2, evnt2 >> 1); break;

        // CC24..C26 are used to set panorama pattern
        case 0x18: STRIBE_SetPan(stribe, 3, evnt2 >> 1);  break;
        case 0x19: STRIBE_SetPan(stribe, 1, evnt2 >> 1);  break;
        case 0x1a: STRIBE_SetPan(stribe, 2, evnt2 >> 1);  break;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
  unsigned char val7;
  unsigned char diff;

  val7 = MIOS_AIN_Pin7bitGet(pin);
  diff = val7 > last_conv_value[pin] ? (val7-last_conv_value[pin]) : (last_conv_value[pin] - val7);

  // filter function: event is ommited if:
  //    - value == 0 (softpot released)
  //    - difference between new and previous value > 16, and last change ca. 20 mS ago
  if( val7 > 0 && !(diff > 16 && last_conv_ctr[pin]) ) {
    // send MIDI event
    MIOS_MIDI_BeginStream();
    if( app_flags.SINGLE_CHN_MODE ) {
      MIOS_MIDI_TxBufferPut(0xb0 | ((STRIBE_MIDI_CHANNEL-1) & 0x0f));
      MIOS_MIDI_TxBufferPut(0x10 + pin);
      MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin));
    } else {
      MIOS_MIDI_TxBufferPut(0xb0 | pin);
      MIOS_MIDI_TxBufferPut(0x10);
      MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin));
    }
    MIOS_MIDI_EndStream();

    // display cursor at stribe LEDs
    STRIBE_SetDot(pin, stribe_flags.TRAIL_MODE ? stribe_flags.TRAIL_MASK : 3, MIOS_AIN_Pin7bitGet(pin) >> 1);

    // preload conversion counter with filter delay value (e.g. 20 results into 100 mS delay)
    last_conv_ctr[pin] = ain_filter_delay;
    last_conv_value[pin] = val7;
  }

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
