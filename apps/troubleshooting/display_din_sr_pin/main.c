// $Id$
/*
 * Display DIN SR
 *
 * See also the README.txt for additional informations
 *
 * ==========================================================================
 *
 *  Copyright <year> <your name>
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
#include <debug_msg.h>

#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

// last ain/din/dout
unsigned char last_din_pin;
unsigned char last_dout_pin;

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

  // debouncing value for DINs
  MIOS_SRIO_DebounceSet(DIN_DEBOUNCE_VALUE);

  MIOS_SRIO_TS_SensitivitySet(DIN_TS_SENSITIVITY);
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
  MIOS_LCD_PrintCString("DIN SR Pin      ");
  MIOS_LCD_CursorSet(0x40); // second line
  MIOS_LCD_PrintCString("xxx xx Dx       ");

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

  // print status of DIN
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintBCD3(last_din_pin);

  MIOS_LCD_PrintChar(' ');

  MIOS_LCD_PrintBCD2((last_din_pin >> 3) + 1);

  MIOS_LCD_PrintChar(' ');
  MIOS_LCD_PrintChar('D');
  MIOS_LCD_PrintBCD1(last_din_pin & 7);

  MIOS_LCD_PrintChar(' ');
  MIOS_LCD_PrintChar(MIOS_DIN_PinGet(last_din_pin) ? 'o' : '*');
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // check if note on or off event at channel 1 has been received
  if( evnt0 == 0x80 || evnt0 == 0x90 ) {
    // if note off event: force evnt2 to 0 for easier handling of 'LED off'
    if( evnt0 == 0x80 )
      evnt2 = 0;

    // number of DOUT pin in evnt1, value in evnt2
    MIOS_DOUT_PinSet(evnt1, evnt2 ? 0x01 : 0x00);

    // notify display handler in DISPLAY_Tick() that DOUT value has changed
    last_dout_pin = evnt1;
    app_flags.DISPLAY_UPDATE_REQ = 1;
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
  // a button has been pressed, send Note at channel 1
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0x90); // Note at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to note number
  MIOS_MIDI_TxBufferPut(pin_value ? 0x00 : 0x7f); // buttons are high-active
  MIOS_MIDI_EndStream();

  // notify display handler in DISPLAY_Tick() that DIN value has changed
  last_din_pin = pin;
  app_flags.DISPLAY_UPDATE_REQ = 1;

  // send to MIOS Terminal
  DEBUG_MSG_SendHeader();
  DEBUG_MSG_SendCString("DIN:");
  DEBUG_MSG_SendBCD3(pin);
  DEBUG_MSG_SendCString("  SR:");
  DEBUG_MSG_SendBCD2((pin >> 3) + 1);
  DEBUG_MSG_SendCString(" D");
  DEBUG_MSG_SendBCD1(pin & 7);
  DEBUG_MSG_SendCString(" ");
  DEBUG_MSG_SendCString(pin_value ? "depressed" : "pressed");
  DEBUG_MSG_SendFooter();
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
