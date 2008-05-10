/*
 * This application demonstrates, how to realize buttons with velocity
 *
 * See the README.txt for additional informations
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
#include <pic18fregs.h>

#include "main.h"
#include "din_vel.h"

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
  // set shift register update frequency
  MIOS_SRIO_UpdateFrqSet(1); // ms

  // we need to set at least one IO shift register pair
  MIOS_SRIO_NumberSet(NUMBER_OF_SRIO);

  // debouncing value for DINs must be 0 in this application
  MIOS_SRIO_DebounceSet(0);

  // initialize modules
  DIN_VEL_Init();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
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
  MIOS_LCD_PrintCString("Button Velocity");

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

  // print status of button
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintBCD2(DIN_VEL_LastButtonGet() + 1);
  MIOS_LCD_CursorSet(0x40 + 7);
  MIOS_LCD_PrintBCD3(DIN_VEL_LastVelocityGet());
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
  // increment velocity counters
  DIN_VEL_Tick();
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
  // forward event to velocity module
  DIN_VEL_NotifyToggle(pin, pin_value);

  // notify display handler in DISPLAY_Tick() if button has been pressed
  if( (pin & 1) && pin_value == 0 ) {
    app_flags.DISPLAY_UPDATE_REQ = 1;
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
