// $Id$
/*
 * Syx Dump Demonstration
 * See README.txt for details
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

#include "patch.h"
#include "sysex.h"


/////////////////////////////////////////////////////////////////////////////
// DIN assignments
/////////////////////////////////////////////////////////////////////////////
#define DIN_NUMBER_EXEC     7
#define DIN_NUMBER_INC      6
#define DIN_NUMBER_DEC      5
#define DIN_NUMBER_SNAPSHOT 4


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////
unsigned char patch;
unsigned char bank;


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // initialize SRIO driver
  MIOS_SRIO_UpdateFrqSet(1); // ms
  MIOS_SRIO_NumberSet(1);

  // enable Auto Repeat for Inc/Dec button
  MIOS_DIN_PinAutoRepeatEnable(DIN_NUMBER_INC);
  MIOS_DIN_PinAutoRepeatEnable(DIN_NUMBER_DEC);

  // initialize SysEx parser
  SYSEX_Init();

  // initialize patch structure
  PATCH_Init();

  // init local patch/bank
  patch = bank = 0;
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
  MIOS_LCD_Clear();
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString("see README.txt");
  MIOS_LCD_CursorSet(0x40);
  MIOS_LCD_PrintCString("for details");
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
  // forward byte to SysEx Parser
  SYSEX_Parser(byte);
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
  unsigned char print_patch = 0;

  // ignore if button has been depressed
  if( pin_value )
    return;

  // call function depending on button number
  switch( pin ) {
    case DIN_NUMBER_EXEC: // Exec button
      // load patch (again)
      PATCH_Load(bank, patch);

      // send dump
      SYSEX_Send(bank, patch);

      // print patch as temporary message + print "Dump sent"
      print_patch = 2;
      break;

    case DIN_NUMBER_INC: // Inc button
      // increment patch, wrap on overflow
      if( ++patch >= 0x80 )
	patch = 0x00;

      // load patch
      PATCH_Load(bank, patch);

      // print patch as temporary message
      print_patch = 1;
      break;

    case DIN_NUMBER_DEC: // Dec button
      // decrement patch, wrap on underflow
      if( --patch >= 0x80 ) // patch is an unsigned number...
	patch = 0x7f;

      // load patch
      PATCH_Load(bank, patch);

      // print patch as temporary message
      print_patch = 1;
      break;

    case DIN_NUMBER_SNAPSHOT: // Snapshot button
      // no function defined
      break;
  }

  // should patch be print?
  if( print_patch ) {
    MIOS_LCD_Clear();
    MIOS_LCD_CursorSet(0x00);
    MIOS_LCD_PrintCString("Patch: ");
    MIOS_LCD_PrintBCD3(patch);

    if( print_patch == 2 ) {
      MIOS_LCD_CursorSet(0x40);
      MIOS_LCD_PrintCString("Dump sent!");
    }

    MIOS_LCD_MessageStart(0xff); // temporary message
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
