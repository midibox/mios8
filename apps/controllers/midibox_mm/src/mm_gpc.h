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

#ifndef _MM_GPC_H
#define _MM_GPC_H

/////////////////////////////////////////////////////////////////////////////
// Exported variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MM_GPC_Init(void);
unsigned char MM_GPC_VPotValueGet(unsigned char vpot);
void MM_GPC_AbsValue_Received(unsigned char entry, unsigned char value);
void MM_GPC_Msg_Refresh(unsigned char line);
void MM_GPC_Msg_Update(void);
void MM_GPC_Msg_UpdateValues(void);

void MM_GPC_SendENCEvent(unsigned char encoder, char incrementer);
void MM_GPC_SendJogWheelEvent(char incrementer);

#endif /* _MM_GPC_H */
