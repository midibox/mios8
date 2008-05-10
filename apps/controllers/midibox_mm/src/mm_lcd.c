// $Id$
/*
 * MIDIbox MM V2 (C version)
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

#include "main.h"
#include "mm_lcd.h"
#include "mm_vpot.h"
#include "mm_gpc.h"

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

unsigned char lcd_charset;
unsigned char msg_cursor;

unsigned char msg_host[128];

// the welcome message              <---------------------------------------->
const unsigned char welcome_msg[] = "---===< Motormix Emulation ready >===---";

// default special character set
const unsigned char charset_vert_bars[8*8] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  };

/////////////////////////////////////////////////////////////////////////////
// This function initializes the LCD
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_Init(void)
{
  unsigned char i;

  lcd_charset = 0xff; // force refresh
  MM_LCD_SpecialCharsInit(0x00);

  // initialize msg array
  for(i=0; i<0x80; ++i) {
    msg_host[i] = ' ';
  }

  // copy welcome message to host buffer
  for(i=0; welcome_msg[i] != 0; ++i ) {
    msg_host[i] = welcome_msg[i];
  }

  // request display update
  mm_flags.MSG_UPDATE_REQ = 1;
}


/////////////////////////////////////////////////////////////////////////////
// This function refreshes the whole LCD screen
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_Msg_Refresh(void)
{
  unsigned int i;
  unsigned char saved_cursor;

  // TK: would be most elegant, but produces incompatible code (usage of FSR1 pointer)
  //     requires library function and increases the code size dramatically!
  //  unsigned char *msg;
  //  msg = mm_flags.GPC_SEL ? msg_gpc : msg_host

  // store current cursor in temporary variable
  saved_cursor = msg_cursor;
    
  // set msg_cursor to first line and update cursor position
  MM_LCD_Msg_CursorSet(0x00);

  // print first line
  if( mm_flags.GPC_SEL ) {
    MM_GPC_Msg_Refresh(0); // 1st line of GPC message
  } else {
    for(i=0x00; i<0x00+40; ++i) // 1st line of host message
      MM_LCD_Msg_PrintChar(msg_host[i], 1);
  }

  // set msg_cursor to second line and update cursor position
  MM_LCD_Msg_CursorSet(0x40);

  // print second line
  if( mm_flags.GPC_SEL ) {
    MM_GPC_Msg_Refresh(1); // 2nd line of GPC message
  } else {
    if( mm_flags.PRINT_POINTERS ) {
      for(i=0; i<8; ++i)
	MM_LCD_PointerPrint(i);
    } else {
      for(i=0x40; i<0x40+40; ++i) // 2nd line of host message
	MM_LCD_Msg_PrintChar(msg_host[i], 1);
    }
  }

  // restore msg_cursor
  msg_cursor = saved_cursor;
}

/////////////////////////////////////////////////////////////////////////////
// this function sets the LCD cursor to the msg_cursor position
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_Msg_CursorSet(unsigned char cursor_pos)
{
  msg_cursor = cursor_pos;
  // cursor_pos could be mapped to another value here
  MIOS_LCD_CursorSet(cursor_pos);
}

/////////////////////////////////////////////////////////////////////////////
// this function returns the current cursor value
/////////////////////////////////////////////////////////////////////////////
unsigned char MM_LCD_Msg_CursorGet(void)
{
  return msg_cursor;
}

/////////////////////////////////////////////////////////////////////////////
// this function is called when a character should be print to the host message section
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_Msg_PrintChar(unsigned char c, unsigned char force)
{
  // print character if not in GPC mode
  // note: with force this check can be bypassed
  if( force || !mm_flags.GPC_SEL ) {
    // print character
    MIOS_LCD_PrintChar(c);

    // increment cursor
    ++msg_cursor;

    // change the cursor if second line has been reached
    if( msg_cursor >= 40 && msg_cursor < 0x40 ) {
      MM_LCD_Msg_CursorSet(0x40);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// stores a character in msg_host at a given cursor position and prints it
// if in HOST mode (!gpc)
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_Msg_PrintHost(unsigned char cursor, unsigned char c)
{
  msg_host[cursor] = c;       // save character
  MM_LCD_Msg_PrintChar(c, 0); // print without force (only print if HOST MSG screen active)
}

/////////////////////////////////////////////////////////////////////////////
// sets a pointer value
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_PointerInit(unsigned char type)
{
  MM_VPOT_DisplayTypeSet(type);
  MM_LCD_SpecialCharsInit(type);
}

/////////////////////////////////////////////////////////////////////////////
// sets a pointer value
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_PointerSet(unsigned char pointer, unsigned char value)
{
  // set VPOT value
  MM_VPOT_ValueSet(pointer, value);

  // update screen if pointers are not printed yet
  if( !mm_flags.PRINT_POINTERS ) {
    mm_flags.PRINT_POINTERS = 1;
    MM_LCD_Msg_Refresh();
  } else {
    // print pointer if not in GPC mode
    MM_LCD_PointerPrint(pointer);
  }
}

/////////////////////////////////////////////////////////////////////////////
// prints a pointer when not in GPC mode
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_PointerPrint(unsigned char pointer)
{
  unsigned char value;

  if( mm_flags.GPC_SEL )
    return;

  // get value
  value = MM_VPOT_ValueGet(pointer);

  // set cursor
  MIOS_LCD_CursorSet(0x40 + pointer*5);

  // print value depending on display and pointer type
  if( MIOS_BOX_CFG0.USE_GLCD ) {
    // graphical LCD - here we could add some nice graphic icons
    // currently we just only print a number and a vertical bar...
    MIOS_LCD_PrintBCD3(value);
    MIOS_LCD_PrintChar((value >> 4) & 0x7f);
    MIOS_LCD_PrintChar(' ');
  } else {
    // character LCD
    switch( MM_VPOT_DisplayTypeGet() ) {
      case 0: // "Left justified horizontal bar graph (Aux pots)"
      case 1: // "Centered horizontal Bar graph"
      case 2: // "Right justified horizontal Bar graph"
      case 3: // "Single verticle line (PAN or PAN R)"
      case 4: // "Left justified double verticle line"
      case 5: // "Centered Q spreading bar (Filter Q)"
      case 6: // "Ascending bar graph (Channel Meters)"
      case 7: // "Descending bar graph (Gaon reduction)"
	// different pointer types not implemented yet
	MIOS_LCD_PrintBCD3(value);
	MIOS_LCD_PrintChar((value >> 4) & 0x7f);
	MIOS_LCD_PrintChar(' ');
	break;

      default:
	MIOS_LCD_PrintBCD3(value);
	MIOS_LCD_PrintChar((value >> 4) & 0x7f);
	MIOS_LCD_PrintChar(' ');
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// initializes special characters
/////////////////////////////////////////////////////////////////////////////
void MM_LCD_SpecialCharsInit(unsigned char charset)
{
  // exit if graphical LCD connected
  if( MIOS_BOX_CFG0.USE_GLCD )
    return;

  // exit if charset already initialized
  if( charset == lcd_charset )
    return;

  // else initialize special characters
  lcd_charset = charset;
  switch( charset ) {
     case 0: // "Left justified horizontal bar graph (Aux pots)"
     case 1: // "Centered horizontal Bar graph"
     case 2: // "Right justified horizontal Bar graph"
     case 3: // "Single verticle line (PAN or PAN R)"
     case 4: // "Left justified double verticle line"
     case 5: // "Centered Q spreading bar (Filter Q)"
     case 6: // "Ascending bar graph (Channel Meters)"
     case 7: // "Descending bar graph (Gaon reduction)"
       MIOS_CLCD_SpecialCharsInit(charset_vert_bars);
       break;
     default:
       MIOS_CLCD_SpecialCharsInit(charset_vert_bars);
       break;
  }
}
