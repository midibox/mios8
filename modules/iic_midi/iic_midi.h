// $Id$
/*
 * Header file for IIC MIDI Routines
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#ifndef _IIC_MIDI_H
#define _IIC_MIDI_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////
extern void IIC_MIDI_Init(void) __wparam;
extern unsigned char IIC_MIDI_Receive(unsigned char slave) __wparam;
extern unsigned char IIC_MIDI_Transmit(unsigned char port, unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;

// (low level routines, normaly not used)
extern unsigned char IIC_MIDI_TransmitStart(unsigned char slave) __wparam;
extern void IIC_MIDI_TransmitByte(unsigned char value) __wparam;
extern void IIC_MIDI_TransmitStop(void) __wparam;

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned char iic_midi_slave;
extern unsigned char iic_midi_available_slaves;

extern unsigned char iic_midi_rx_package[4];
extern unsigned char iic_midi_tx_package[4];

#endif /* _IIC_MIDI_H */
