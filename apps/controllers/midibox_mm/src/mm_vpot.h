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

#ifndef _MM_VPOT_H
#define _MM_VPOT_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_VPOT_Init(void);
void MM_VPOT_LEDRing_CheckUpdates(void);
void MM_VPOT_LEDRing_SRHandler(void);

unsigned char MM_VPOT_DisplayTypeGet(void);
void MM_VPOT_DisplayTypeSet(unsigned char type);

unsigned char MM_VPOT_ValueGet(unsigned char pointer);
void MM_VPOT_ValueSet(unsigned char pointer, unsigned char value);

void MM_VPOT_LEDRingUpdateSet(unsigned char rings);

void MM_VPOT_SendENCEvent(unsigned char encoder, char incrementer);
void MM_VPOT_SendJogWheelEvent(char incrementer);      

#endif /* _MM_VPOT_H */
