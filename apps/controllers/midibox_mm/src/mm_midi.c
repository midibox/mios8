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

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>

#include "main.h"
#include "mm_midi.h"
#include "mm_mf.h"
#include "mm_dio.h"

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// This function is called from MPROC_NotifyReceivedEvnt when a complete
// MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MM_MIDI_Received(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2)
{
  static unsigned char fader;
  static unsigned char fader_pos_l;
  static unsigned char fader_pos_h;
  static unsigned char led_id_l;
  static unsigned char led_id_h;

  // exit if wrong channel
  if( (evnt0 & 0x0f) != ((MM_MIDI_CHANNEL-1) & 0x0f) )
    return;

  // extract MIDI status (without channel)
  evnt0 &= 0xf0;

  // for easier parsing: convert Note Off to Note On with velocity 0
  if( (evnt0 & 0xf0) == 0x90 ) {
    evnt0 |= 0x10;
    evnt2 = 0x00;
  }

  if( evnt0 == 0x90 ) { // Note On/Off

    // ping: if 90 00 00 has been received, return 90 00 7F
    if( evnt1 == 0x00 && evnt2 == 0x00 ) {
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0x90 | ((MM_MIDI_CHANNEL-1) & 0x0f));
      MIOS_MIDI_TxBufferPut(0x00);
      MIOS_MIDI_TxBufferPut(0x7f);
      MIOS_MIDI_EndStream();
    }

  } else if( evnt0 == 0xb0) { // CC

    if( (evnt1 & 0x78) == 0x00 ) {        // Bn 0[0-7]: Fader MSB
      fader_pos_h = evnt2;
    } else if( (evnt1 & 0x78) == 0x20 ) { // Bn 2[0-7]: Fader LSB
      fader_pos_l = evnt2;
      MM_MF_FaderMove(evnt1 & 0x07, fader_pos_l, fader_pos_h);
    } else if( evnt1 == 0x0c ) {          // Bn 0C: LED ID_H
      led_id_h = evnt2;
    } else if( evnt1 == 0x2c ) {          // Bn 2C: LED ID_L
      led_id_l = evnt2;
      MM_DIO_LEDHandler(led_id_l, led_id_h);
    }

  }

}
