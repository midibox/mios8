// $Id$
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

#ifndef _BLM_SCALAR_H
#define _BLM_SCALAR_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void BLM_SCALAR_Init(void) __wparam;
extern void BLM_SCALAR_PrepareCol(void) __wparam;
extern void BLM_SCALAR_GetRow(void) __wparam;
extern void BLM_SCALAR_ButtonHandler(void) __wparam;

// moved to main.h
//extern void BLM_SCALAR_NotifyToggle(unsigned char pin, unsigned char value) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char blm_scalar_button_column;
extern unsigned char blm_scalar_button_row;
extern unsigned char blm_scalar_button_value;
extern unsigned char blm_scalar_button_debounce_delay;

extern unsigned char blm_scalar_row_red[5*8];
extern unsigned char blm_scalar_row_green[5*8];

#endif /* _BLM_SCALAR_H */
