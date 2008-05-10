/*
 * DIN Velocity module
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
#include <pic18fregs.h>

#include "main.h"
#include "din_vel.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

unsigned char din_velocity_ctr[NUMBER_OF_DIN_VEL - 1];
unsigned char din_velocity[NUMBER_OF_DIN_VEL - 1];

unsigned char last_button;
unsigned char last_velocity;

// this table scales the velocity counter (0..127) to the velocity value which will be sent via MIDI
// it has been generated with "perl utils/velocity_table.pl"
const unsigned char velocity_table[128] = {
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
  0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04,
  0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06,
  0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x08,
  0x08, 0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0b,
  0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0f,
  0x0f, 0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14,
  0x15, 0x16, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
  0x1c, 0x1d, 0x1f, 0x20, 0x21, 0x22, 0x24, 0x25,
  0x27, 0x28, 0x2a, 0x2b, 0x2d, 0x2f, 0x31, 0x33,
  0x35, 0x37, 0x39, 0x3b, 0x3d, 0x40, 0x42, 0x45,
  0x48, 0x4a, 0x4d, 0x50, 0x54, 0x57, 0x5a, 0x5e,
  0x61, 0x65, 0x69, 0x6d, 0x72, 0x76, 0x7b, 0x7f,
};

/////////////////////////////////////////////////////////////////////////////
// This function should be called from Init() in main.c to initialize
// this module
/////////////////////////////////////////////////////////////////////////////
void DIN_VEL_Init(void)
{
  unsigned char i;

  // init velocity counters
  for(i=0; i<NUMBER_OF_DIN_VEL-1; ++i)
    din_velocity_ctr[i] = 127;
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called periodically from SR_Service_Finish()
// it checks for the button status and increments the velocity counters
// in a 1->0 transition
/////////////////////////////////////////////////////////////////////////////
void DIN_VEL_Tick(void)
{
  unsigned char i;
  unsigned char pin;

  for(i=0; i<NUMBER_OF_DIN_VEL-1; ++i) {
    pin = i << 1;
    if( MIOS_DIN_PinGet(pin + 0) == 0 ) { // first contact closed -> button released
      din_velocity_ctr[i] = 127;
    } else if( MIOS_DIN_PinGet(pin + 1) == 1 ) { // second contact open -> decrement velocity counter
      if( din_velocity_ctr[i] )  // decrement until 0 is reached
        --din_velocity_ctr[i];
    } else if( MIOS_DIN_PinGet(pin + 1) == 0 ) { // second contact closed -> capture velocity
      din_velocity[i] = din_velocity_ctr[i];
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from DIN_NotifyToggle() in main.c
/////////////////////////////////////////////////////////////////////////////
void DIN_VEL_NotifyToggle(unsigned char pin, unsigned char pin_value)
{
  // all even pin numbers are active when a button has been released
  // all odd pin numbers are active when a button has been pressed

  // delays are already measured in DIN_VEL_Tick(), so we only need
  // to react on the "pressed contact" (odd pin number)
  if( pin & 1 ) {
    if( pin_value == 0 ) {

      last_button = pin >> 1;

      // scale velocity counter
      last_velocity = velocity_table[din_velocity[last_button]];

      // send Note On at channel 1
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0x90);
      MIOS_MIDI_TxBufferPut(last_button + 0x24); // C-2 is the first button
      MIOS_MIDI_TxBufferPut(last_velocity);
      MIOS_MIDI_EndStream();

    } else {

      // send Note On at channel 1
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0x90);
      MIOS_MIDI_TxBufferPut((pin >> 1) + 0x24); // C-2 is the first button
      MIOS_MIDI_TxBufferPut(0x00);
      MIOS_MIDI_EndStream();

    }
  }
#if 0
  if( (pin & 1) == 0 ) {

    // reload velocity counter if first contact has been released
    if( pin_value )
      din_velocity_ctr[pin >> 1] = 127;

  } else {

    // capture counter if second contact has been closed
    if( !pin_value ) {

      last_button = pin >> 1;

      // scale velocity counter
      last_velocity = velocity_table[din_velocity_ctr[last_button]];

      // send Note On at channel 1
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0x90);
      MIOS_MIDI_TxBufferPut(last_button + 0x24); // C-2 is the first button
      MIOS_MIDI_TxBufferPut(last_velocity);
      MIOS_MIDI_EndStream();

    } else {

      // send Note On at channel 1
      MIOS_MIDI_BeginStream();
      MIOS_MIDI_TxBufferPut(0x90);
      MIOS_MIDI_TxBufferPut((pin >> 1) + 0x24); // C-2 is the first button
      MIOS_MIDI_TxBufferPut(0x00);
      MIOS_MIDI_EndStream();

    }

  }
#endif

}

/////////////////////////////////////////////////////////////////////////////
// This function returns the last button which has been pressed
/////////////////////////////////////////////////////////////////////////////
unsigned char DIN_VEL_LastButtonGet(void)
{
  return last_button;
}

/////////////////////////////////////////////////////////////////////////////
// This function returns the velocity of the last button which has been pressed
/////////////////////////////////////////////////////////////////////////////
unsigned char DIN_VEL_LastVelocityGet(void)
{
  return last_velocity;
}
