// $Id$
/*
 * Header file for AOUT Routines
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _AOUT_H
#define _AOUT_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void AOUT_Init(void) __wparam;
extern void AOUT_ReInit(void) __wparam;

extern void AOUT_InvertedSet(unsigned char inv) __wparam;
extern unsigned char AOUT_InvertedGet(void) __wparam;

extern void AOUT_Pin16bitSet(unsigned char pin, unsigned int value) __wparam;
extern void AOUT_Pin7bitSet(unsigned char pin, unsigned char value) __wparam;

extern void AOUT_DigitalPinsSet(unsigned char value) __wparam;

extern void AOUT_Update(void) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _AOUT_H */
