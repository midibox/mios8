// $Id$
/*
 * This example demonstrates how to drive 64 pots, 128 buttons, 128 LEDs
 *
 * Note: hardware settings (e.g. number of pots, muxed/unmuxed mode)
 *       are specified in main.h
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

#include <cmios.h>
#include <pic18fregs.h>

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

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
unsigned char last_ain_pin;
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

  // initialize the AIN driver
  MIOS_AIN_NumberSet(AIN_NUMBER_INPUTS);
#if AIN_MUXED_MODE
  MIOS_AIN_Muxed();
#else
  MIOS_AIN_UnMuxed();
#endif
  MIOS_AIN_DeadbandSet(AIN_DEADBAND);
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
  MIOS_LCD_PrintCString(" AIN   DIN  DOUT");
  MIOS_LCD_CursorSet(0x40); // second line
  MIOS_LCD_PrintCString("xx:xxx xxxx xxxx");

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

  // print status of AIN
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintBCD2(last_ain_pin + 1);
  MIOS_LCD_PrintChar(':');
  MIOS_LCD_PrintBCD3(MIOS_AIN_Pin7bitGet(last_ain_pin));

  // print status of DIN
  MIOS_LCD_CursorSet(0x40 + 7);
  MIOS_LCD_PrintBCD3(last_din_pin + 1);
  MIOS_LCD_PrintChar(MIOS_DIN_PinGet(last_din_pin) ? 'o' : '*');

  // print status of DOUT
  MIOS_LCD_CursorSet(0x40 + 12);
  MIOS_LCD_PrintBCD3(last_dout_pin + 1);
  MIOS_LCD_PrintChar(MIOS_DOUT_PinGet(last_dout_pin) ? '*' : 'o');
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
  // a pot has been moved, send CC#<pin-number> at channel 1
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xb0); // CC at channel 1
  MIOS_MIDI_TxBufferPut(pin);  // pin number corresponds to CC number
  MIOS_MIDI_TxBufferPut(MIOS_AIN_Pin7bitGet(pin)); // don't send 10bit pin_value, but 7bit value
  MIOS_MIDI_EndStream();

  // notify display handler in DISPLAY_Tick() that AIN value has changed
  last_ain_pin = pin;
  app_flags.DISPLAY_UPDATE_REQ = 1;
}
