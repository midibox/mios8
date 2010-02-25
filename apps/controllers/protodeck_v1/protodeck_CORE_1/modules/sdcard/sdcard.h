// $Id: sdcard.h 378 2008-07-06 20:38:32Z tk $
/*
 * Header file for SDCARD Routines
 *
 * ==========================================================================
 *
 *  Copyright 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _SDCARD_H
#define _SDCARD_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern unsigned char SDCARD_Init(void) __wparam;
extern unsigned char SDCARD_SectorRead(unsigned long sector) __wparam;
extern unsigned char SDCARD_SectorWrite(unsigned long sector) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char sdcard_address[4];

extern unsigned char sdcard_response;
extern unsigned char sdcard_cmdcode;
extern unsigned char sdcard_crc;

extern unsigned char sdcard_buffer_p0[0x100];
extern unsigned char sdcard_buffer_p1[0x100];

#endif /* _SDCARD_H */
