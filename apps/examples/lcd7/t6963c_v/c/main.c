// $Id$
/*
 * MIOS Custom LCD Driver Example for T6963C --- with vertical screen (-> up to 64x240)
 * 
 * please find the LCD driver in $MIOS_PATH/modules/app_lcd/t6963c_v/app_lcd.asm
 *
 * ==========================================================================
 *
 *  Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#include "cmios.h"
#include "pic18fregs.h"

#include <glcd_font.h>  // located in $MIOS_PATH/modules/glcd_font


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // select LCD type #7
  // (custom LCD driver, hooks in $MIOS_PATH/modules/app_lcd/t6963c_v/app_lcd.asm module will be used)
  MIOS_LCD_TypeSet(0x07, 0x00, 0x00);
  // The second and third value is not used by the driver
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

  // use default font
  MIOS_GLCD_FontInit(MIOS_GLCD_FONT);    

  MIOS_GLCD_GCursorSet(  2,  2);
  MIOS_LCD_PrintCString("  T6963C  ");
  MIOS_GLCD_GCursorSet(  2,  4);
  MIOS_LCD_PrintCString("!vertical!");

  MIOS_GLCD_GCursorSet(  2, 10);
  MIOS_LCD_PrintCString("  Custom  ");
  MIOS_GLCD_GCursorSet(  2, 11);
  MIOS_LCD_PrintCString("   LCD    ");
  MIOS_GLCD_GCursorSet(  2, 12);
  MIOS_LCD_PrintCString("  Driver  ");

  MIOS_GLCD_GCursorSet(  5, 14);
  MIOS_LCD_PrintCString(" powered  ");
  MIOS_GLCD_GCursorSet(  2, 15);
  MIOS_LCD_PrintCString("    by    ");


  // switch to big font (part of glcd_font package)
  MIOS_GLCD_FontInit(GLCD_FONT_BIG);

  MIOS_GLCD_GCursorSet(  0, 16);
  MIOS_LCD_PrintCString("MIOS");


  // switch to knob icons (part of glcd_font package)
  MIOS_GLCD_FontInit(GLCD_FONT_KNOB_ICONS);

  MIOS_GLCD_GCursorSet(  5, 21);
  MIOS_LCD_PrintChar(0x01); // icon #01
  MIOS_LCD_PrintChar(0x05); // icon #05
  MIOS_GLCD_GCursorSet(  5, 24);
  MIOS_LCD_PrintChar(0x07); // icon #07
  MIOS_LCD_PrintChar(0x0a); // icon #10
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
