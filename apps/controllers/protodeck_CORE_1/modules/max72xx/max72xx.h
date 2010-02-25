// $Id: max72xx.h 240 2008-03-30 22:06:06Z tk $
/*
 * Header file for MAX72XX Routines
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _MAX72XX_H
#define _MAX72XX_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// Number of connected MAX72xx chips (can be overruled from Makefile, should not be done from .c file!)
#ifndef MAX72XX_CHAINLENGTH
#define MAX72XX_CHAINLENGTH 1
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void MAX72XX_Init(void) __wparam;

extern void MAX72XX_UpdateDigits(unsigned char digit_num) __wparam;
extern void MAX72XX_UpdateAllDigits(void) __wparam;

extern void MAX72XX_WriteReg(unsigned char max_num, unsigned char addr, unsigned char data) __wparam;
extern void MAX72XX_WriteRegs(unsigned char addr, unsigned char data) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char max72xx_digits[MAX72XX_CHAINLENGTH*8];


#endif /* _MAX72XX_H */
