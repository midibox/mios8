// $Id$
/*
 * This is a random patch generator for the MIDIbox SID
 * 
 * See also the README.txt for additional informations
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

#include <cmios.h>
#include <pic18fregs.h>

#include "main.h"
#include "sid_random.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

unsigned char button_debounce_ctr;
unsigned char button_last_value;  

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // enable MIDI merger
  MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_ENABLED);

  // set shift register update frequency
  MIOS_SRIO_UpdateFrqSet(1); // ms

  // we need to set at least one IO shift register pair
  MIOS_SRIO_NumberSet(1); // one should be enough

  // debouncing value for DINs
  MIOS_SRIO_DebounceSet(10);

  // switch pin RC3 as input (pull-up already installed on core module)
  TRISCbits.TRISC3 = 1;

  // initialize the random module
  SID_RANDOM_Init();

  // init last button value
  button_last_value = PORTCbits.RC3 ? 1 : 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  unsigned char button_new_value;

  // check if button at pin RC3 has been pressed
  // this routine also contains a very simple debouncing mechanism
  // the debounce counter is decremented in SR_Service_Finish (to save one timer)
  if( button_debounce_ctr == 0 ) {
    button_new_value = PORTCbits.RC3 ? 1 : 0;
    if( button_new_value != button_last_value ) {
      button_last_value = button_new_value;
      if( button_new_value == 0 ) { // button pressed
	SID_RANDOM_Generate(0);
	MIOS_LCD_Clear();
	MIOS_LCD_PrintCString("Patch generated");
	MIOS_LCD_MessageStart(255);
      }
      button_debounce_ctr = 50; // ensure that this routine won't be handled in next 50 mS
    }
  }
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
  // clear screen
  MIOS_LCD_Clear();

  // print static messages
  MIOS_LCD_CursorSet(0x00); // first line
  MIOS_LCD_PrintCString("SID Random Patch");
  MIOS_LCD_CursorSet(0x40); // second line
  MIOS_LCD_PrintCString("Generator");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // do nothing if no update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;

  // clear request
  app_flags.DISPLAY_UPDATE_REQ = 0;

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
  // decrement debounce counter until it reaches 0
  if( button_debounce_ctr )
    button_debounce_ctr--;
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
  if( pin_value == 0 ) { // any button has been pressed
    SID_RANDOM_Generate(0);
    MIOS_LCD_Clear();
    MIOS_LCD_PrintCString("Patch generated");
    MIOS_LCD_MessageStart(255);
  }
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
