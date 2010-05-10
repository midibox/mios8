// $Id$
/*
 * MBHP_SCALAR firmware
 *
 * ==========================================================================
 *
 *  Copyright (C) 2009 Thorsten Klose (tk@midibox.org)
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

#include <blm.h>
#include <blm_scalar.h>
#include "main.h"
#include "sysex.h"


/////////////////////////////////////////////////////////////////////////////
// Use BLM or BLM_SCALAR driver?
/////////////////////////////////////////////////////////////////////////////
#define USE_BLM_SCALAR 1


/////////////////////////////////////////////////////////////////////////////
// Number of LED rows (1..16)
/////////////////////////////////////////////////////////////////////////////

#define NUM_LED_ROWS 16


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

// send layout informations each 5 seconds if BLM hasn't been updated
static unsigned int timeoutCounter;

static unsigned char testmode;


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  timeoutCounter = 0;
  testmode = 1;

  // initialize the scan matrix driver
#if USE_BLM_SCALAR
  BLM_SCALAR_Init();

  // set initial debounce delay (should be done after BLM_Init(), as this function overwrites the value)
  //blm_scalar_button_debounce_delay = 20;    // (-> 20 * 4 mS = 80 mS)
#else
  BLM_Init();

  // set initial debounce delay (should be done after BLM_Init(), as this function overwrites the value)
  blm_button_debounce_delay = 20;    // (-> 20 * 4 mS = 80 mS)
#endif

  // initialize SysEx parser
  SYSEX_Init();

  // initialize the shift registers
  MIOS_SRIO_NumberSet(16);           // use up to 16 shift registers
  MIOS_SRIO_UpdateFrqSet(1);         // set update frequency

  // configure AIN
  MIOS_AIN_NumberSet(8);   // up to 8 pots connected
  MIOS_AIN_UnMuxed();      // no AINX4 modules are used
  MIOS_AIN_DeadbandSet(7); // should be 7 when 7bit resolution is used
}



/////////////////////////////////////////////////////////////////////////////
// This function resets the timeout counter and exits testmode
/////////////////////////////////////////////////////////////////////////////
static void ResetTimeoutCounter(void)
{
  // reset counter (atomic access due to multiple bytes!)
  INTCONbits.GIE = 0;
  timeoutCounter = 0;
  INTCONbits.GIE = 1;

  // disable testmode
  testmode = 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // send layout informations each 5 seconds if BLM hasn't been updated
  if( timeoutCounter > 5000 ) {
    ResetTimeoutCounter();
    SYSEX_SendLayoutInfo();
  }

  // call the scan matrix button handler
#if USE_BLM_SCALAR
  BLM_SCALAR_ButtonHandler();
#else
  BLM_ButtonHandler();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
  // clear LCD
  MIOS_LCD_Clear();

  // print static screen (no real LCD support)
  MIOS_LCD_CursorSet(0x00 + 0);
  MIOS_LCD_PrintCString("MBHP_BLM_SCALAR");
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintCString("No LCD supported");
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char chn = evnt0 & 0x0f;
  unsigned char event_type = evnt0 >> 4;

  if( chn >= NUM_LED_ROWS )
    return;

  // control the Duo-LEDs via Note On/Off Events
  // The colour is controlled with velocity value:
  // 0x00:       both LEDs off
  // 0x01..0x3f: green LED on
  // 0x40..0x5f: red LED on
  // 0x60..0x7f: both LEDs on

  // MIDI event assignments: see README.txt

  if( (event_type == 0x8 || event_type == 0x9) ) {
    unsigned char led_row_ix = 0;
    unsigned char led_column_ix = 0;
    unsigned char modify_led = 0;

    if( evnt1 <= 0x0f ) {
      // BLM16x16 LEDs
      led_row_ix = 2*chn + (evnt1 >> 3);
      led_column_ix = evnt1 & 0x7;
      modify_led = 1;
    } else if( evnt1 == 0x40 ) {
      // extra column LEDs
      led_row_ix = 0x20 + ((chn & 3) << 1);
      led_column_ix = chn >> 2;
      modify_led = 1;
    } else if( chn == 0 && evnt1 >= 0x60 && evnt1 <= 0x6f ) {
      // extra row LEDs
      led_row_ix = 0x21 + ((evnt1 >> 1) & 6);
      led_column_ix = evnt1 & 3;
      modify_led = 1;
    } else if( chn == 0xf && evnt1 >= 0x60 && evnt1 <= 0x6f ) {
      // additional extra LEDs
      led_row_ix = 0x21 + ((evnt1 >> 1) & 6);
      led_column_ix = 4 + (evnt1 & 3);
      modify_led = 1;
    }


    if( modify_led ) {
      // 90 xx 00 is the same like a note off event!
      // (-> http://www.borg.com/~jglatt/tech/midispec.htm)
      if( event_type == 0x8 || evnt2 == 0x00 ) {
	// Note Off or velocity == 0x00: clear both LEDs
#if USE_BLM_SCALAR
	blm_scalar_row_green[led_row_ix] &= MIOS_HLP_GetBitANDMask(led_column_ix);
	blm_scalar_row_red[led_row_ix]   &= MIOS_HLP_GetBitANDMask(led_column_ix);
#else
	if( led_row_ix < 8 ) {
	  blm_row_green[led_row_ix] &= MIOS_HLP_GetBitANDMask(led_column_ix);
	  blm_row_red[led_row_ix]   &= MIOS_HLP_GetBitANDMask(led_column_ix);
	}
#endif

      } else if( evnt2 < 0x40 ) {
	// Velocity < 0x40: set green LED, clear red LED
#if USE_BLM_SCALAR
	blm_scalar_row_green[led_row_ix] |= MIOS_HLP_GetBitORMask(led_column_ix);
	blm_scalar_row_red[led_row_ix]   &= MIOS_HLP_GetBitANDMask(led_column_ix);
#else
	if( led_row_ix < 8 ) {
	  blm_row_green[led_row_ix] |= MIOS_HLP_GetBitORMask(led_column_ix);
	  blm_row_red[led_row_ix]   &= MIOS_HLP_GetBitANDMask(led_column_ix);
	}
#endif
      } else if( evnt2 < 0x60 ) {
	// Velocity < 0x60: clear green LED, set red LED
#if USE_BLM_SCALAR
	blm_scalar_row_green[led_row_ix] &= MIOS_HLP_GetBitANDMask(led_column_ix);
	blm_scalar_row_red[led_row_ix]   |= MIOS_HLP_GetBitORMask(led_column_ix);
#else
	if( led_row_ix < 8 ) {
	  blm_row_green[led_row_ix] &= MIOS_HLP_GetBitANDMask(led_column_ix);
	  blm_row_red[led_row_ix]   |= MIOS_HLP_GetBitORMask(led_column_ix);
	}
#endif
      } else {
	// Velocity >= 0x60: set both LEDs
#if USE_BLM_SCALAR
	blm_scalar_row_green[led_row_ix] |= MIOS_HLP_GetBitORMask(led_column_ix);
	blm_scalar_row_red[led_row_ix]   |= MIOS_HLP_GetBitORMask(led_column_ix);
#else
	if( led_row_ix < 8 ) {
	  blm_row_green[led_row_ix] |= MIOS_HLP_GetBitORMask(led_column_ix);
	  blm_row_red[led_row_ix]   |= MIOS_HLP_GetBitORMask(led_column_ix);
	}
#endif
      }

      ResetTimeoutCounter();
    }
  }

  // "check for packed format" which is transfered via CCs
  else if( event_type == 0xb ) {
    unsigned char pattern = evnt2;
    if( evnt1 & 0x01 )
      pattern |= (1 << 7);

    switch( evnt1 & 0xfe ) {
#if USE_BLM_SCALAR
      case 0x10: blm_scalar_row_green[2*chn + 0] = pattern; break;
      case 0x12: blm_scalar_row_green[2*chn + 1] = pattern; break;

      case 0x18:
      case 0x1a: {
	unsigned char offset = ((evnt1 & 0x02) ? 0 : 16) + (chn >> 3);
	unsigned char mask = MIOS_HLP_GetBitORMask(chn);
	if( pattern & 0x80 ) { blm_scalar_row_green[offset +  0] |= mask; } else { blm_scalar_row_green[offset +  0] &= ~mask; }
	if( pattern & 0x40 ) { blm_scalar_row_green[offset +  2] |= mask; } else { blm_scalar_row_green[offset +  2] &= ~mask; }
	if( pattern & 0x20 ) { blm_scalar_row_green[offset +  4] |= mask; } else { blm_scalar_row_green[offset +  4] &= ~mask; }
	if( pattern & 0x10 ) { blm_scalar_row_green[offset +  6] |= mask; } else { blm_scalar_row_green[offset +  6] &= ~mask; }
	if( pattern & 0x08 ) { blm_scalar_row_green[offset +  8] |= mask; } else { blm_scalar_row_green[offset +  8] &= ~mask; }
	if( pattern & 0x04 ) { blm_scalar_row_green[offset + 10] |= mask; } else { blm_scalar_row_green[offset + 10] &= ~mask; }
	if( pattern & 0x02 ) { blm_scalar_row_green[offset + 12] |= mask; } else { blm_scalar_row_green[offset + 12] &= ~mask; }
	if( pattern & 0x01 ) { blm_scalar_row_green[offset + 14] |= mask; } else { blm_scalar_row_green[offset + 14] &= ~mask; }
      } break;

      case 0x20: blm_scalar_row_red[2*chn + 0] = pattern; break;
      case 0x22: blm_scalar_row_red[2*chn + 1] = pattern; break;

      case 0x28:
      case 0x2a: {
	unsigned char offset = ((evnt1 & 0x02) ? 0 : 16) + (chn >> 3);
	unsigned char mask = MIOS_HLP_GetBitORMask(chn);
	if( pattern & 0x80 ) { blm_scalar_row_red[offset +  0] |= mask; } else { blm_scalar_row_red[offset +  0] &= ~mask; }
	if( pattern & 0x40 ) { blm_scalar_row_red[offset +  2] |= mask; } else { blm_scalar_row_red[offset +  2] &= ~mask; }
	if( pattern & 0x20 ) { blm_scalar_row_red[offset +  4] |= mask; } else { blm_scalar_row_red[offset +  4] &= ~mask; }
	if( pattern & 0x10 ) { blm_scalar_row_red[offset +  6] |= mask; } else { blm_scalar_row_red[offset +  6] &= ~mask; }
	if( pattern & 0x08 ) { blm_scalar_row_red[offset +  8] |= mask; } else { blm_scalar_row_red[offset +  8] &= ~mask; }
	if( pattern & 0x04 ) { blm_scalar_row_red[offset + 10] |= mask; } else { blm_scalar_row_red[offset + 10] &= ~mask; }
	if( pattern & 0x02 ) { blm_scalar_row_red[offset + 12] |= mask; } else { blm_scalar_row_red[offset + 12] &= ~mask; }
	if( pattern & 0x01 ) { blm_scalar_row_red[offset + 14] |= mask; } else { blm_scalar_row_red[offset + 14] &= ~mask; }
      } break;

      case 0x40: 
      case 0x42: {
	if( chn == 0 ) {
	  // Bit  0 -> led_row_ix 0x20, Bit 0
	  // Bit  1 -> led_row_ix 0x22, Bit 0
	  // Bit  2 -> led_row_ix 0x24, Bit 0
	  // Bit  3 -> led_row_ix 0x26, Bit 0
	  // Bit  4 -> led_row_ix 0x20, Bit 1
	  // Bit  5 -> led_row_ix 0x22, Bit 1
	  // Bit  6 -> led_row_ix 0x24, Bit 1
	  // Bit  7 -> led_row_ix 0x26, Bit 1
	  // Bit  8 -> led_row_ix 0x20, Bit 2
	  // Bit  9 -> led_row_ix 0x22, Bit 2
	  // Bit 10 -> led_row_ix 0x24, Bit 2
	  // Bit 11 -> led_row_ix 0x26, Bit 2
	  // Bit 12 -> led_row_ix 0x20, Bit 3
	  // Bit 13 -> led_row_ix 0x22, Bit 3
	  // Bit 14 -> led_row_ix 0x24, Bit 3
	  // Bit 15 -> led_row_ix 0x26, Bit 3
	  if( evnt1 >= 0x42 ) {
	    blm_scalar_row_green[0x20] = (blm_scalar_row_green[0x20] & 0xf3) | ((pattern << 2) & 0x04) | ((pattern >> 1) & 0x08);
	    blm_scalar_row_green[0x22] = (blm_scalar_row_green[0x22] & 0xf3) | ((pattern << 1) & 0x04) | ((pattern >> 2) & 0x08);
	    blm_scalar_row_green[0x24] = (blm_scalar_row_green[0x24] & 0xf3) | ((pattern << 0) & 0x04) | ((pattern >> 3) & 0x08);
	    blm_scalar_row_green[0x26] = (blm_scalar_row_green[0x26] & 0xf3) | ((pattern >> 1) & 0x04) | ((pattern >> 4) & 0x08);
	  } else {
	    blm_scalar_row_green[0x20] = (blm_scalar_row_green[0x20] & 0xfc) | ((pattern >> 0) & 0x01) | ((pattern >> 3) & 0x02);
	    blm_scalar_row_green[0x22] = (blm_scalar_row_green[0x22] & 0xfc) | ((pattern >> 1) & 0x01) | ((pattern >> 4) & 0x02);
	    blm_scalar_row_green[0x24] = (blm_scalar_row_green[0x24] & 0xfc) | ((pattern >> 2) & 0x01) | ((pattern >> 5) & 0x02);
	    blm_scalar_row_green[0x26] = (blm_scalar_row_green[0x26] & 0xfc) | ((pattern >> 3) & 0x01) | ((pattern >> 6) & 0x02);
	  }
	}
      } break;

      case 0x48: 
      case 0x4a: {
	if( chn == 0 ) {
	  if( evnt1 >= 0x4a ) {
	    blm_scalar_row_red[0x20] = (blm_scalar_row_red[0x20] & 0xf3) | ((pattern << 2) & 0x04) | ((pattern >> 1) & 0x08);
	    blm_scalar_row_red[0x22] = (blm_scalar_row_red[0x22] & 0xf3) | ((pattern << 1) & 0x04) | ((pattern >> 2) & 0x08);
	    blm_scalar_row_red[0x24] = (blm_scalar_row_red[0x24] & 0xf3) | ((pattern << 0) & 0x04) | ((pattern >> 3) & 0x08);
	    blm_scalar_row_red[0x26] = (blm_scalar_row_red[0x26] & 0xf3) | ((pattern >> 1) & 0x04) | ((pattern >> 4) & 0x08);
	  } else {
	    blm_scalar_row_red[0x20] = (blm_scalar_row_red[0x20] & 0xfc) | ((pattern >> 0) & 0x01) | ((pattern >> 3) & 0x02);
	    blm_scalar_row_red[0x22] = (blm_scalar_row_red[0x22] & 0xfc) | ((pattern >> 1) & 0x01) | ((pattern >> 4) & 0x02);
	    blm_scalar_row_red[0x24] = (blm_scalar_row_red[0x24] & 0xfc) | ((pattern >> 2) & 0x01) | ((pattern >> 5) & 0x02);
	    blm_scalar_row_red[0x26] = (blm_scalar_row_red[0x26] & 0xfc) | ((pattern >> 3) & 0x01) | ((pattern >> 6) & 0x02);
	  }
	}
      } break;


      case 0x60: 
      case 0x62: {
	unsigned char led_row_ix = (evnt1 >= 0x62) ? 0x25 : 0x21;
	if( chn == 0 ) {
	  blm_scalar_row_green[led_row_ix] = (blm_scalar_row_green[led_row_ix] & 0xf0) | (pattern & 0x0f);
	  led_row_ix += 2;
	  blm_scalar_row_green[led_row_ix] = (blm_scalar_row_green[led_row_ix] & 0xf0) | (pattern >> 4);
	} else if( chn == 15 ) {
	  blm_scalar_row_green[led_row_ix] = (blm_scalar_row_green[led_row_ix] & 0x0f) | (pattern << 4);
	  led_row_ix += 2;
	  blm_scalar_row_green[led_row_ix] = (blm_scalar_row_green[led_row_ix] & 0x0f) | (pattern & 0xf0);
	}
      } break;

      case 0x68: 
      case 0x6a: {
	unsigned char led_row_ix = (evnt1 >= 0x6a) ? 0x25 : 0x21;
	if( chn == 0 ) {
	  blm_scalar_row_red[led_row_ix] = (blm_scalar_row_red[led_row_ix] & 0xf0) | (pattern & 0x0f);
	  led_row_ix += 2;
	  blm_scalar_row_red[led_row_ix] = (blm_scalar_row_red[led_row_ix] & 0xf0) | (pattern >> 4);
	} else if( chn == 15 ) {
	  blm_scalar_row_red[led_row_ix] = (blm_scalar_row_red[led_row_ix] & 0x0f) | (pattern << 4);
	  led_row_ix += 2;
	  blm_scalar_row_red[led_row_ix] = (blm_scalar_row_red[led_row_ix] & 0x0f) | (pattern & 0xf0);
	}
      } break;
#else
      case 0x10: blm_row_green[2*chn + 0] = pattern; break;
      case 0x12: blm_row_green[2*chn + 1] = pattern; break;
      case 0x20: blm_row_red[2*chn + 0] = pattern; break;
      case 0x22: blm_row_red[2*chn + 1] = pattern; break;
#endif
    }

    ResetTimeoutCounter();
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
  // forward byte to SysEx Parser
  SYSEX_Parser(byte);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
  // call the Scan Matrix Driver
#if USE_BLM_SCALAR
  BLM_SCALAR_PrepareCol();
#else
  BLM_PrepareCol();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
  // call the Scan Matrix Driver
#if USE_BLM_SCALAR
  BLM_SCALAR_GetRow();
#else
  BLM_GetRow();
#endif

  // increment timeout counter
  ++timeoutCounter;
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
  // a pot has been moved, send modulation CC#1
  MIOS_MIDI_TxBufferPut(0xb0 + (pin & 0x0f));
  MIOS_MIDI_TxBufferPut(0x01);
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin));
}



/////////////////////////////////////////////////////////////////////////////
// This function is NOT called by MIOS, but by the scan matrix handler
// in $MIOS_PATH/modules/blm, when a pin of the scan matrix has been toggled
// Note: in addition to "pin" and "value", the "blm_button_column" and
// "blm_button_row" are available as global variables (defined in blm.h)
/////////////////////////////////////////////////////////////////////////////
void BLM_NotifyToggle(unsigned char pin, unsigned char value) __wparam
{
  unsigned char mask;

  // send pin number and value as Note On Event
  MIOS_MIDI_TxBufferPut(0x90 + (pin >> 4));
  MIOS_MIDI_TxBufferPut(pin & 0x0f);
  MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);

  // enable this code (turn #if 0 into #if 1) if buttons should change the LED colour directly
  // disable it when LEDs should only be controlled via MIDI
  if( testmode ) {
    // cycle colour whenever button has been pressed (value == 0)
    if( !value ) {
      mask = MIOS_HLP_GetBitORMask(blm_button_column);
      if ( blm_row_green[blm_button_row] & mask )
	blm_row_red[blm_button_row] ^= mask;
      blm_row_green[blm_button_row] ^= mask;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// This function is NOT called by MIOS, but by the scan matrix handler
// in $MIOS_PATH/modules/blm_scalar, when a pin of the scan matrix has been toggled
// Note: in addition to "pin" and "value", the "blm_button_column" and
// "blm_button_row" are available as global variables (defined in blm.h)
/////////////////////////////////////////////////////////////////////////////
void BLM_SCALAR_NotifyToggle(unsigned char pin, unsigned char value) __wparam
{
  unsigned char mask;

  // send pin number and value as Note On Event
  if( blm_scalar_button_row < 0x20 ) {
    // BLM 16x16
    MIOS_MIDI_TxBufferPut(0x90 + (pin >> 4));
    MIOS_MIDI_TxBufferPut(pin & 0x0f);
    MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);
  } else if( (blm_scalar_button_row & 0xf9) == 0x20 && blm_scalar_button_column < 4 ) {
    // Extra column
    unsigned char row = ((blm_scalar_button_row >> 1) & 3) | (blm_scalar_button_column << 2);
    MIOS_MIDI_TxBufferPut(0x90 + row);
    MIOS_MIDI_TxBufferPut(0x40);
    MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);
  } else if( (blm_scalar_button_row & 0xf9) == 0x21 ) {
    unsigned char column = ((blm_scalar_button_row << 1) & 0x0c) | (blm_scalar_button_column & 0x3);
    if( blm_scalar_button_column < 4 ) {
      // Extra row
      MIOS_MIDI_TxBufferPut(0x90);
      MIOS_MIDI_TxBufferPut(0x60 + column);
      MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);
    } else {
      // Additional extra buttons
      MIOS_MIDI_TxBufferPut(0x9f);
      MIOS_MIDI_TxBufferPut(0x60 + column);
      MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);
    }
  }

  // enable this to determine/debug the column/row scrambling
#if 0
  MIOS_MIDI_TxBufferPut(0xb0);
  MIOS_MIDI_TxBufferPut(blm_scalar_button_column);
  MIOS_MIDI_TxBufferPut(blm_scalar_button_row);
#endif

  // enable this code (turn #if 0 into #if 1) if buttons should change the LED colour directly
  // disable it when LEDs should only be controlled via MIDI
  if( testmode ) {
    // cycle colour whenever button has been pressed (value == 0)
    if( !value ) {
      mask = MIOS_HLP_GetBitORMask(blm_scalar_button_column);
      if ( blm_scalar_row_green[blm_scalar_button_row] & mask )
	blm_scalar_row_red[blm_scalar_button_row] ^= mask;
      blm_scalar_row_green[blm_scalar_button_row] ^= mask;
    }
  }
}
