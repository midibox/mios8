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
#include "mm_vpot.h"
#include "mm_gpc.h"

/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

// the "vpot" display type and values (virtual VPot positions)
unsigned char display_type;
unsigned char vpot_abs_value[8];

// the LEDring patterns
unsigned int ledring_pattern[8]; // 16 bit words for 8 v-pots
unsigned char ledring_update_req; // 8 update request flags

// this table defines the ledring patterns for 32 steps
// note that the 12th LED (the center LED below the encoder) is set by
// MM_VPOT_LEDRing_CheckUpdates separately if the V-Pot pointer hasn't been received from host
const unsigned int preset_patterns[32] = {
  0x0001, //   b'0000000000000001'
  0x0001, //   b'0000000000000001'
  0x0003, //   b'0000000000000011'
  0x0003, //   b'0000000000000011'
  0x0003, //   b'0000000000000011'
  0x0007, //   b'0000000000000111'
  0x0007, //   b'0000000000000111'
  0x0007, //   b'0000000000000111'
  0x000f, //   b'0000000000001111'
  0x000f, //   b'0000000000001111'
  0x000f, //   b'0000000000001111'
  0x001f, //   b'0000000000011111'
  0x001f, //   b'0000000000011111'
  0x001f, //   b'0000000000011111'
  0x003f, //   b'0000000000111111'
  0x003f, //   b'0000000000111111'
  0x003f, //   b'0000000000111111'
  0x003f, //   b'0000000000111111'
  0x007f, //   b'0000000001111111'
  0x007f, //   b'0000000001111111'
  0x007f, //   b'0000000001111111'
  0x00ff, //   b'0000000011111111'
  0x00ff, //   b'0000000011111111'
  0x00ff, //   b'0000000011111111'
  0x01ff, //   b'0000000111111111'
  0x01ff, //   b'0000000111111111'
  0x01ff, //   b'0000000111111111'
  0x03ff, //   b'0000001111111111'
  0x03ff, //   b'0000001111111111'
  0x03ff, //   b'0000001111111111'
  0x07ff, //   b'0000011111111111'
  0x07ff, //   b'0000011111111111'
  };

// this table defines the cathode patterns (a 0 selects a LED)
const unsigned char cathode_patterns[8] = {
  0xfe, // b'11111110'
  0xfd, // b'11111101'
  0xfb, // b'11111011'
  0xf7, // b'11110111'
  0xef, // b'11101111'
  0xdf, // b'11011111'
  0xbf, // b'10111111'
  0x7f, // b'01111111'
};

/////////////////////////////////////////////////////////////////////////////
// This function initializes the LEDring handler and the encoder speed modes
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_Init(void)
{
  unsigned char i;

  // request update of all LEDring patterns
  display_type = 0x00;
  ledring_update_req = 0xff;

  for(i=0; i<8; ++i)
    MIOS_ENC_SpeedSet(i, ENC_VPOT_SPEED_MODE, ENC_VPOT_SPEED_DIVIDER);

  MIOS_ENC_SpeedSet(8, ENC_JOGWHEEL_SPEED_MODE, ENC_JOGWHEEL_SPEED_DIVIDER);

}

/////////////////////////////////////////////////////////////////////////////
// This function returns the vpot display type
/////////////////////////////////////////////////////////////////////////////
unsigned char MM_VPOT_DisplayTypeGet(void)
{
  return display_type;
}

/////////////////////////////////////////////////////////////////////////////
// This function sets the vpot display type
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_DisplayTypeSet(unsigned char type)
{
  display_type = type;
}


/////////////////////////////////////////////////////////////////////////////
// This function returns the absolute vpot value
/////////////////////////////////////////////////////////////////////////////
unsigned char MM_VPOT_ValueGet(unsigned char vpot)
{
  return vpot_abs_value[vpot];
}

/////////////////////////////////////////////////////////////////////////////
// This function sets the absolute vpot value
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_ValueSet(unsigned char vpot, unsigned char value)
{
  vpot_abs_value[vpot] = value;

  ledring_update_req |= (1 << vpot); // request update of ledring
}

/////////////////////////////////////////////////////////////////////////////
// This function requests a LEDring update for the given VPots
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_LEDRingUpdateSet(unsigned char rings)
{
  ledring_update_req |= rings;
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from ENC_NotifyChange() in main.c
// when a V-Pot has been moved
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_SendENCEvent(unsigned char encoder, char incrementer)
{
  if( mm_flags.GPC_SEL ) {
    // in GPC mode: change GPC value
    MM_GPC_SendENCEvent(encoder, incrementer);
  } else {
    // otherwise send relative event to host application
    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(0xb0 | ((MM_MIDI_CHANNEL-1) & 0x0f));
    MIOS_MIDI_TxBufferPut(0x40 | (encoder & 0x07));
    if( incrementer < 0 ) {
      MIOS_MIDI_TxBufferPut((-incrementer & 0x3f) | 0x00); // anti-clockwise
    } else {
      MIOS_MIDI_TxBufferPut((incrementer & 0x3f) | 0x40); // clockwise
    }
    MIOS_MIDI_EndStream();
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from ENC_NotifyChange() in main.c
// when the jogwheel has been moved
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_SendJogWheelEvent(char incrementer)
{
  if( mm_flags.GPC_SEL ) {
    // in GPC mode: select GPC offset
    MM_GPC_SendJogWheelEvent(incrementer);
  } else {
    // otherwise send relative event to host application
    MIOS_MIDI_BeginStream();
    MIOS_MIDI_TxBufferPut(0xb0 | ((MM_MIDI_CHANNEL-1) & 0x0f));
    MIOS_MIDI_TxBufferPut(0x48);
    if( incrementer < 0 ) {
      MIOS_MIDI_TxBufferPut((-incrementer & 0x3f) | 0x00); // anti-clockwise
    } else {
      MIOS_MIDI_TxBufferPut((incrementer & 0x3f) | 0x40); // clockwise
    }
    MIOS_MIDI_EndStream();
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from Tick() in main.c
// it updates the LEDring patterns for which an update has been requested
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_LEDRing_CheckUpdates(void)
{
  unsigned char ledring;
  unsigned char ledring_select;
  unsigned char value;
  unsigned char center_led;

  for(ledring=0, ledring_select=0x01; ledring<8; ++ledring) {

    // check if update has been requested
    if( ledring_update_req & ledring_select ) {
      ledring_update_req &= ~ledring_select; // clear request flag
      
      // get value
      if( mm_flags.GPC_SEL ) {
	center_led = 0;
	value = MM_GPC_VPotValueGet(ledring);
      } else {
	center_led = !mm_flags.PRINT_POINTERS;
	value = vpot_abs_value[ledring];
      }

      // set 12th LED (center LED) of not in GPC mode and no pointer has been received from host yet
      // (can be customized for your own needs)

      // select pattern depending on value
      ledring_pattern[ledring] = preset_patterns[value >> 2] | (center_led ? (1 << (12-1)) : 0);
    }

    ledring_select <<= 1; // shift left the select bit
  }
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from SR_Service_Prepare in main.c
// it copies the current LEDring pattern to the DOUT registers
/////////////////////////////////////////////////////////////////////////////
void MM_VPOT_LEDRing_SRHandler(void)
{
#if LEDRINGS_ENABLED
  static unsigned char sr_ctr;
  unsigned int anode_pattern;

  // increment the counter which selects the ledring that will be visible during
  // the next SRIO update cycle --- wrap at 8 (0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, ...)
  sr_ctr = ++sr_ctr & 0x07;

  // select the cathode of the LEDring (set the appr. pin to 0, and all others to 1)
  MIOS_DOUT_SRSet(LEDRINGS_SR_CATHODES, cathode_patterns[sr_ctr]);

  // set the LEDring pattern on the anodes
  anode_pattern = ledring_pattern[sr_ctr];
  MIOS_DOUT_SRSet(LEDRINGS_SR_ANODES_1, anode_pattern & 0xff);
  MIOS_DOUT_SRSet(LEDRINGS_SR_ANODES_2, (anode_pattern >> 8) & 0xff);
#endif
}
