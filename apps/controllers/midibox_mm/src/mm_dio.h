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

#ifndef _MM_DIO_H
#define _MM_DIO_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_DIO_ButtonHandler(unsigned char button, unsigned char value);
void MM_DIO_SFBHandler(unsigned char button_id, unsigned char pin_value);
void MM_DIO_SFBLEDUpdate(void);

void MM_DIO_LEDHandler(unsigned char led_id_l, unsigned char led_id_h);
void MM_DIO_LEDSet(unsigned char mm_id, unsigned char led_on);
void MM_DIO_LED_CheckUpdate(void);

#endif /* _MM_DIO_H */
