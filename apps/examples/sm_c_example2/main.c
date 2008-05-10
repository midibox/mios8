// $Id$
/*
 * Scan Matrix Example #2
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#include "cmios.h"
#include "pic18f452.h"

#include "main.h"
#include "sm_fast.h"

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
  SM_Init();

  // initialize the timer function for the debounce counters
  // we want to setup the timer with a frequency of 1kHz = 1 mS
  // prescaler 1:1 should be used
  // calculate the required number of clocks for this period:
  //   clocks = period / 100 nS = 1 mS / 100 nS = 10000
  MIOS_TIMER_Init(0x00, 10000);

  // initialize the shift registers
  MIOS_SRIO_NumberSet(0);            // DONT USE THE MIOS SRIO DRIVER!!!
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // call the scan matrix button handler
  SM_ButtonHandler();
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
  // call the debounce function
  SM_DebounceTimer();
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

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // update display only when requested to minimize the CPU load
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // print message on screen depending on button status
  MIOS_LCD_CursorSet(0x00 + 0);
  if( sm_button_value )
    MIOS_LCD_PrintCString("Button Depressed");
  else
    MIOS_LCD_PrintCString("Button Pressed  ");

  // print Column and Row
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintCString("Col:");
  MIOS_LCD_PrintBCD3(sm_button_column);
  MIOS_LCD_PrintCString("  Row:");
  MIOS_LCD_PrintBCD3(sm_button_row);
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
  SM_NotifyToggle(pin, pin_value);
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
// in sm_fast.asm, when a pin of the scan matrix has been toggled
// Note: in addition to "pin" and "value", the "sm_button_column" and
// "sm_button_row" are available as global variables (defined in sm_fast.h)
/////////////////////////////////////////////////////////////////////////////
void SM_NotifyToggle(unsigned char pin, unsigned char value) __wparam
{
  // send pin number and value as Note On Event
  MIOS_MIDI_TxBufferPut(0x90);
  MIOS_MIDI_TxBufferPut(pin);
  MIOS_MIDI_TxBufferPut(value ? 0x00 : 0x7f);

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
