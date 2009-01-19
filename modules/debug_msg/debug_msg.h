// $Id$
/*
 * Header file for Debug Message Routines
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _DEBUG_MSG_H
#define _DEBUG_MSG_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern void DEBUG_MSG_SendHeader(void) __wparam;
extern void DEBUG_MSG_SendFooter(void) __wparam;
extern void DEBUG_MSG_SendChar(char) __wparam;
extern void DEBUG_MSG_SendCString(char *str) __wparam;
extern void DEBUG_MSG_SendBCD1(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD2(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD3(unsigned char) __wparam;
extern void DEBUG_MSG_SendBCD4(unsigned int) __wparam;
extern void DEBUG_MSG_SendBCD5(unsigned int) __wparam;
extern void DEBUG_MSG_SendHex1(unsigned char) __wparam;
extern void DEBUG_MSG_SendHex2(unsigned char) __wparam;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

#endif /* _DEBUG_MSG_H */
