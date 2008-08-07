// $Id$
/*
 * MIOS Custom LCD Driver Example for DOG LCDs
 * 
 * please find the LCD driver in $MIOS_PATH/modules/app_lcd/dog/app_lcd.asm
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#include "cmios.h"
#include "pic18fregs.h"


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // select LCD type #7
  // (custom LCD driver, hooks in $MIOS_PATH/modules/app_lcd/dog/app_lcd.asm module will be used)
  // Parameter #1 and #2 select the enable pins like for common CLCD driver
  // Note that the DOG driver switches to CLCD (type #0) after initialisation!
  // Up to 2 DOG based LCDs are supported
  // E input of first LCD at D.7, E of second LCD @C.4
  MIOS_LCD_TypeSet(0x07, 0x37, 0x24);
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
#if 0
  // 3 line display - upper line with small letters, two lower lines with big letters
  MIOS_LCD_Cmd(0x3c);
  MIOS_LCD_Clear();
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString(" EA DOCM163x-A  ");
  MIOS_LCD_CursorSet(0x40);
  MIOS_LCD_PrintCString("Powered by MIOS!");
#else
  // 3 lines
  MIOS_LCD_Clear();
  MIOS_LCD_CursorSet(0x00);
  MIOS_LCD_PrintCString(" EA DOCM163x-A  ");
  MIOS_LCD_CursorSet(0x40);
  MIOS_LCD_PrintCString("Powered by MIOS!");
  MIOS_LCD_CursorSet(0x50);
  MIOS_LCD_PrintCString(" Third line :-) ");
#endif
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
}
