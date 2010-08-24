// $Id$
/*
 * MIDI Routing package
 * This file contains hooks which are called from iic_midi.asm and int_midi.asm
 *
 * ==========================================================================
 *
 *  Copyright 2006 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include "main.h"
#include "iic_midi.h"
#include "int_midi.h"
#include "router.h"


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

unsigned char sysex_owner;


// router specific flags (see bitfield declaration in router.h)
router_flags_t router_flags;

// for IIC3 input: if router_flags.IIC3_FWD_FORCE_CHN == 1, channel will be forced to a specific value
unsigned char router_iic3_fwd_force_channel;
// optional transpose of keyboard at IIC3
unsigned char router_iic3_fwd_transpose;


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////
#define MIDI_CHN1	0
#define MIDI_CHN2	1
#define MIDI_CHN3	2
#define MIDI_CHN4	3
#define MIDI_CHN5	4
#define MIDI_CHN6	5
#define MIDI_CHN7	6
#define MIDI_CHN8	7
#define MIDI_CHN9	8
#define MIDI_CHN10	9
#define MIDI_CHN11	10
#define MIDI_CHN12	11
#define MIDI_CHN13	12
#define MIDI_CHN14	13
#define MIDI_CHN15	14
#define MIDI_CHN16	15

/////////////////////////////////////////////////////////////////////////////
// This function initializes the router
/////////////////////////////////////////////////////////////////////////////
void ROUTER_Init(void) __wparam
{
  sysex_owner = PORT_NONE;

  // default setup
  router_flags.IIC2_FWD_CHN8_16 = 0;

  router_flags.IIC3_FWD_FORCE_CHN = 0;
  router_iic3_fwd_force_channel = MIDI_CHN1;
  router_iic3_fwd_transpose = 8; // +0

  router_flags.IIC3_FWD_MBSID = 0;
  router_flags.IIC3_FWD_MBFM = 0;
  router_flags.IIC3_FWD_MBSEQ = 0;
}


/////////////////////////////////////////////////////////////////////////////
// This function locks a receiver path if a SysEx stream is received
/////////////////////////////////////////////////////////////////////////////
unsigned char ROUTER_LockPathOnSysEx(unsigned char port, unsigned char ptype) __wparam
{
  // lock if SysEx starts or continues
  if( ptype == 0x04 ) {
    sysex_owner = port;
    return 1;
  }

  // only any other MIDI event (or if sysex ends): release path
  sysex_owner = PORT_NONE;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// These functions are called from the MIDI parser when a new package has been
// received. They can be modified to customize the routing
//
// evnt0, evnt1, evnt2: the bytes of the MIDI event which has been received
//
// ptype: package type
//   - 0x02  two-byte system common message like MTC, Song Select, etc.
//   - 0x03  three-byte system common message like SSP, etc.
//   - 0x04  SysEx starts or continues
//   - 0x05  SysEx ends with following single byte
//   - 0x06  SysEx ends with following two bytes
//   - 0x07  SysEx ends with following three bytes
//   - 0x08  Note Off
//   - 0x09  Note On
//   - 0x0a  Poly-Key Pressure
//   - 0x0b  Control Change
//   - 0x0c  Program Change
//   - 0x0d  Channel Pressure
//   - 0x0e  Pitch Bend
//   - 0x0f  single byte like MIDI Clock/Start/Stop/Continue
/////////////////////////////////////////////////////////////////////////////
void ROUTER_Rx_INT0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // Core MIDI IN, connected to my PC

  // lock/release this routing path on SysEx streams
  ROUTER_LockPathOnSysEx(PORT_INT0, ptype);

  // forward data to the MIDI OUTs of all IIC slaves
  ROUTER_Tx_IIC0(ptype, evnt0, evnt1, evnt2);
  ROUTER_Tx_IIC1(ptype, evnt0, evnt1, evnt2);
  ROUTER_Tx_IIC2(ptype, evnt0, evnt1, evnt2);
  ROUTER_Tx_IIC3(ptype, evnt0, evnt1, evnt2);
}

void ROUTER_Rx_IIC0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // IIC0: connected to my MIDIbox SID

  // lock/release this routing path on SysEx streams
  ROUTER_LockPathOnSysEx(PORT_IIC0, ptype);

  // forward data to the Core MIDI OUT
  ROUTER_Tx_INT0(ptype, evnt0, evnt1, evnt2);
}

void ROUTER_Rx_IIC1(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // IIC1: connected to my MIDIbox FM

  // lock/release this routing path on SysEx streams
  ROUTER_LockPathOnSysEx(PORT_IIC1, ptype);

  // forward data to the Core MIDI OUT
  ROUTER_Tx_INT0(ptype, evnt0, evnt1, evnt2);
}

void ROUTER_Rx_IIC2(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  // IIC2: connected to my MIDIbox SEQ

  // lock/release this routing path on SysEx streams
  ROUTER_LockPathOnSysEx(PORT_IIC2, ptype);

  // forward MIDI clock/start/stop/continue to all IIC modules
  if( evnt0 >= 0xf8 && evnt0 <= 0xfc ) {
    ROUTER_Tx_IIC0(ptype, evnt0, evnt1, evnt2);
    ROUTER_Tx_IIC1(ptype, evnt0, evnt1, evnt2);
    ROUTER_Tx_IIC2(ptype, evnt0, evnt1, evnt2);
    ROUTER_Tx_IIC3(ptype, evnt0, evnt1, evnt2);
  }

  // if IIC2_FWD_CHN8_16 flag set:
  if( router_flags.IIC2_FWD_CHN8_16 && (ptype >= 0x08 && ptype <= 0x0e) && ((evnt0 & 0x0f) >= MIDI_CHN9) ) {
    // directly forward MIDI channel #9..#16 messages to MIDIbox SID and MIDIbox FM only
    ROUTER_Tx_IIC0(ptype, evnt0, evnt1, evnt2);
    ROUTER_Tx_IIC1(ptype, evnt0, evnt1, evnt2);
  } else {
    // forward all other events to the Core MIDI OUT
    ROUTER_Tx_INT0(ptype, evnt0, evnt1, evnt2);
  }
}

void ROUTER_Rx_IIC3(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char transposed_note;

  // IIC3: connected to my Yamaha AN1x

  // lock/release this routing path on SysEx streams
  ROUTER_LockPathOnSysEx(PORT_IIC3, ptype);

  // filter clock
  if( evnt0 == 0xf8 )
    return;

  // apply exactly the routing as documented under http://www.ucapps.de/midi_router/midi_router_default.gif
  // note: parameters selectable on the UI are not taken into account here
  // if this is desired, replace constants by router_flags.* variables as in the original router.c file
  if( ptype >= 0x08 && ptype <= 0x09 ) {

    if( (evnt0 & 0x0f) == 0x0f ) {
      if( evnt1 < 0x3c ) {
	ROUTER_Tx_IIC0(ptype, (evnt0 & 0xf0) | 9, evnt1, evnt2);

	transposed_note = evnt1 + 2*12;
	// if value >= 0x80, decrement 12 until we have reached the range <= 0x7f again
	while( transposed_note & 0x80 ) transposed_note -= 12;
	ROUTER_Tx_IIC2(ptype, (evnt0 & 0xf0) | 1, transposed_note, evnt2);
      } else {
	ROUTER_Tx_IIC1(ptype, (evnt0 & 0xf0) | 13, evnt1, evnt2);
      }
    }
  }

  // forward data (also) to the Core MIDI OUT
#if 1
  // if no FE
  if( evnt0 != 0xfe )
#endif
    ROUTER_Tx_INT0(ptype, evnt0, evnt1, evnt2);
}



/////////////////////////////////////////////////////////////////////////////
// These functions forward a package to the appr. transmit function in 
// int_midi.asm and iic_midi.asm
//
// ptype: package type (see description of ROUTER_Rx_*)
// evnt0, evnt1, evnt2: the bytes of the MIDI event which should be transmitted
//
// These functions don't need to be modified, the routing is part of the
// ROUTER_Rx_xx() function!
/////////////////////////////////////////////////////////////////////////////
void ROUTER_Tx_INT0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char retry_ctr = 0;
  while( (++retry_ctr != 0xff) && !INT_MIDI_Transmit(0, ptype, evnt0, evnt1, evnt2) );
}

void ROUTER_Tx_IIC0(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char retry_ctr = 0;
  while( (++retry_ctr != 0xff) && !IIC_MIDI_Transmit(0, ptype, evnt0, evnt1, evnt2) );
}

void ROUTER_Tx_IIC1(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char retry_ctr = 0;
  while( (++retry_ctr != 0xff) && !IIC_MIDI_Transmit(1, ptype, evnt0, evnt1, evnt2) );
}

void ROUTER_Tx_IIC2(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char retry_ctr = 0;
  while( (++retry_ctr != 0xff) && !IIC_MIDI_Transmit(2, ptype, evnt0, evnt1, evnt2) );
}

void ROUTER_Tx_IIC3(unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
  unsigned char retry_ctr = 0;
  while( (++retry_ctr != 0xff) && !IIC_MIDI_Transmit(3, ptype, evnt0, evnt1, evnt2) );
}
