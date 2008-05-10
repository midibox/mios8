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

#ifndef _MM_LCD_H
#define _MM_LCD_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_LCD_Init(void);
void MM_LCD_Msg_CursorSet(unsigned char cursor_pos);
unsigned char MM_LCD_Msg_CursorGet(void);
void MM_LCD_Msg_PrintChar(unsigned char c, unsigned char force);
void MM_LCD_Msg_Refresh(void);
void MM_LCD_Msg_PrintHost(unsigned char cursor, unsigned char c);

void MM_LCD_PointerInit(unsigned char type);
void MM_LCD_PointerPrint(unsigned char pointer);
void MM_LCD_PointerSet(unsigned char pointer, unsigned char value);

void MM_LCD_SpecialCharsInit(unsigned char charset);

#endif /* _MM_LCD_H */
