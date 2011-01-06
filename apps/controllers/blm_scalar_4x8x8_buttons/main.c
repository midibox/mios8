// $Id$
/*
 * BLM_SCALAR Firmware Variant for 4 8x8 button matrices
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

#include <blm_scalar.h>
#include "main.h"


/////////////////////////////////////////////////////////////////////////////
// At which note should the first button start
/////////////////////////////////////////////////////////////////////////////
#define FIRST_MIDI_NOTE 24


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize the scan matrix driver
  BLM_SCALAR_Init();

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
  BLM_SCALAR_ButtonHandler();
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
  BLM_SCALAR_PrepareCol();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
  // call the Scan Matrix Driver
  BLM_SCALAR_GetRow();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
  // send Note On/Off event at Channel #16
  MIOS_MIDI_TxBufferPut(0x9f);
  MIOS_MIDI_TxBufferPut(pin);
  MIOS_MIDI_TxBufferPut(pin_value ? 0x00 : 0x7f);
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
// in $MIOS_PATH/modules/blm_scalar, when a pin of the scan matrix has been toggled
// Note: in addition to "pin" and "value", the "blm_scalar_button_column" and
// "blm_scalar_button_row" are available as global variables (defined in blm.h)
/////////////////////////////////////////////////////////////////////////////
void BLM_SCALAR_NotifyToggle(unsigned char pin, unsigned char value) __wparam
{
  // blm_scalar_button_row ranges from 0x00..0x1f (if 4 DINs are used)
  // blm_scalar_button_column ranges from 0x00..0x07

  // send Note On/Off event at Channel #1..4
  unsigned char channel = blm_scalar_button_row >> 3;
  unsigned char note = FIRST_MIDI_NOTE + 8 * (blm_scalar_button_row & 0x7) + blm_scalar_button_column;
  unsigned char velocity = value ? 0x00 : 0x7f;

  MIOS_MIDI_TxBufferPut(0x90 + channel);
  MIOS_MIDI_TxBufferPut(note);
  MIOS_MIDI_TxBufferPut(velocity);
}
