// $Id: blm.h 154 2008-03-01 20:46:48Z tk $
/*
 * Button/Duo-LED Matrix Driver
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _BLM_H
#define _BLM_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void BLM_Init(void) __wparam;
extern void BLM_PrepareCol(void) __wparam;
extern void BLM_GetRow(void) __wparam;
extern void BLM_ButtonHandler(void) __wparam;

// moved to main.h
//extern void BLM_NotifyToggle(unsigned char pin, unsigned char value) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char blm_button_column;
extern unsigned char blm_button_row;
extern unsigned char blm_button_value;
extern unsigned char blm_button_debounce_delay;

extern unsigned char blm_row_red[8];
extern unsigned char blm_row_green[8];

#endif /* _BLM_H */
