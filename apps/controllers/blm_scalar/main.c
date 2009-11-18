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
#include "main.h"



/////////////////////////////////////////////////////////////////////////////
// Number of LED rows (1..16)
/////////////////////////////////////////////////////////////////////////////

#define NUM_LED_ROWS 4


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize the scan matrix driver
  BLM_Init();

  // set initial debounce delay (should be done after BLM_Init(), as this function overwrites the value)
  blm_button_debounce_delay = 20;    // (-> 20 * 4 mS = 80 mS)

  // initialize the shift registers
  MIOS_SRIO_NumberSet(16);           // use up to 16 shift registers
  MIOS_SRIO_UpdateFrqSet(1);         // set update frequency
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // call the scan matrix button handler
  BLM_ButtonHandler();
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

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
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

  // only MIDI note numbers from 0x3c..0x4b are valid, each channel controls another row (-> 16*16 LEDs)

  if( (event_type == 0x8 || event_type == 0x9) && evnt1 >= 0x3c && evnt1 <= 0x4b ) {
    unsigned char led_column;
    unsigned char led_row;

    // derive LED column and row from note number
    led_column = 2*chn + (evnt1 >= (0x3c+8));
    led_row = (evnt1-0x3c) & 0x7;

    // 90 xx 00 is the same like a note off event!
    // (-> http://www.borg.com/~jglatt/tech/midispec.htm)
    if( event_type == 0x8 || evnt2 == 0x00 ) {

      // Note Off or velocity == 0x00: clear both LEDs
      blm_row_green[led_column] &= MIOS_HLP_GetBitANDMask(led_row);
      blm_row_red[led_column]   &= MIOS_HLP_GetBitANDMask(led_row);

    } else if( evnt2 < 0x40 ) {

      // Velocity < 0x40: set green LED, clear red LED
      blm_row_green[led_column] |= MIOS_HLP_GetBitORMask(led_row);
      blm_row_red[led_column]   &= MIOS_HLP_GetBitANDMask(led_row);

    } else if( evnt2 < 0x60 ) {

      // Velocity < 0x60: clear green LED, set red LED
      blm_row_green[led_column] &= MIOS_HLP_GetBitANDMask(led_row);
      blm_row_red[led_column]   |= MIOS_HLP_GetBitORMask(led_row);

    } else {

      // Velocity >= 0x60: set both LEDs
      blm_row_green[led_column] |= MIOS_HLP_GetBitORMask(led_row);
      blm_row_red[led_column]   |= MIOS_HLP_GetBitORMask(led_row);

    }
  }

  // "check for packed format" which is transfered via CCs
  else if( event_type == 0xb ) {
    unsigned char pattern = evnt2;
    if( evnt1 & 0x01 )
      pattern |= (1 << 7);

    switch( evnt1 & 0xfe ) {
      case 0x10: blm_row_green[2*chn + 0] = pattern; break;
      case 0x12: blm_row_green[2*chn + 1] = pattern; break;
      case 0x20: blm_row_red[2*chn + 0] = pattern; break;
      case 0x22: blm_row_red[2*chn + 1] = pattern; break;
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
  // call the Scan Matrix Driver
  BLM_PrepareCol();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
  // call the Scan Matrix Driver
  BLM_GetRow();
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
  MIOS_MIDI_TxBufferPut(0x3c + (pin & 0xf));
  MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);

  // enable this code (turn #if 0 into #if 1) if buttons should change the LED colour directly
  // disable it when LEDs should only be controlled via MIDI
#if 0
  // cycle colour whenever button has been pressed (value == 0)
  if( !value ) {
      mask = MIOS_HLP_GetBitORMask(blm_button_column);
      if ( blm_row_green[blm_button_row] & mask )
         blm_row_red[blm_button_row] ^= mask;
      blm_row_green[blm_button_row] ^= mask;
  }
#endif
  
  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
