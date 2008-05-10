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

#ifndef _MM_MF_H
#define _MM_MF_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_MF_Init(void);
unsigned int MM_MF_FaderPosGet(unsigned char fader);
void MM_MF_FaderEvent(unsigned char fader, unsigned int position);
void MM_MF_FaderMove(unsigned char fader, unsigned char position_l, unsigned char position_h);


#endif /* _MM_MF_H */
