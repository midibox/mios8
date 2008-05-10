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
#include "mm_mf.h"

/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

// used to store the fader position
unsigned int fader_position[8];


/////////////////////////////////////////////////////////////////////////////
// This function initializes the fader positions
/////////////////////////////////////////////////////////////////////////////
void MM_MF_Init(void)
{
  unsigned char i;

  // reset fader values
  for(i=0; i<sizeof(fader_position); ++i)
    fader_position[i] = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function returns the stored fader position
/////////////////////////////////////////////////////////////////////////////
unsigned int MM_MF_FaderPosGet(unsigned char fader)
{
  return fader_position[fader];
}

/////////////////////////////////////////////////////////////////////////////
// This function has to be called from USER_AIN_NotifyChange when a fader
// has been moved manually
/////////////////////////////////////////////////////////////////////////////
void MM_MF_FaderEvent(unsigned char fader, unsigned int position)
{
  unsigned char position_l, position_h;

#if TOUCH_SENSOR_MODE >= 2
  // in this mode, no value will be sent if touch sensor not active
  if( !MIOS_MF_SuspendGet(fader) )
    return;
#endif

  // store position
  fader_position[fader] = position;

  // convert 10-bit conversion result to a left-justified 2*7bit value
  position_l = ((unsigned char)(position << 4)) & 0x7f;
  position_h = ((unsigned char)(position >> (7-4))) & 0x7f;
  
  // ensure that fader number is in the range of 0..7
  fader &= 0x07;

  // send MIDI event
  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xb0 | ((MM_MIDI_CHANNEL-1) & 0x0f));
  MIOS_MIDI_TxBufferPut(0x00 + fader);
  MIOS_MIDI_TxBufferPut(position_h);
  MIOS_MIDI_TxBufferPut(0x20 + fader);
  MIOS_MIDI_TxBufferPut(position_l);
  MIOS_MIDI_EndStream();
}


/////////////////////////////////////////////////////////////////////////////
// This function moves a fader to the given position
// The position is coded in 2*7 bit MIDI format and will be decoded to 10bit
/////////////////////////////////////////////////////////////////////////////
void MM_MF_FaderMove(unsigned char fader, unsigned char position_l, unsigned char position_h)
{
  unsigned int position;

  position = ((unsigned int)position_h << (7-4)) | ((unsigned int)position_l >> 4);
  fader_position[fader] = position;
  MIOS_MF_FaderMove(fader, position);
}

