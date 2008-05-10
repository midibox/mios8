// $Id$
/*
 * Random Patch generator for MIDIbox SID
 * 
 * See also the README.txt for additional informations
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
#include "sid_random.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

unsigned char checksum;

unsigned char last_rand0;
unsigned char last_rand1;

/////////////////////////////////////////////////////////////////////////////
// This function is called to initialize the SID Random module
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_Init(void)
{
  // disable ADC, all analog pins are digital pins now
  ADCON1 = 0x07;
}

/////////////////////////////////////////////////////////////////////////////
// This function returns a 8bit random number by polling RA0, where a 
// white noise signal generator is connected
// The result is scaled by the given value
/////////////////////////////////////////////////////////////////////////////
unsigned char SID_RANDOM_Get(unsigned char range)
{
  unsigned char i;
  unsigned char rand0;
  unsigned char rand1;

  for(i=0; i<8; ++i) {
    rand0 = (rand0 << 1) | (PORTAbits.RA0 ? 1 : 0);
  }
  for(i=0; i<8; ++i) {
    rand1 = (rand1 << 1) | (PORTAbits.RA0 ? 1 : 0);
  }
  rand0 = rand0 + last_rand1;
  rand1 = rand1 + last_rand0;

    // scaled value is (<8-bit random> * <range>) >> 8
    PRODL = rand0; // 8bit random number
    PRODH = range; // range
__asm
    movf _PRODL, W
    mulwf _PRODH, 0
__endasm;

  return PRODH;
}

/////////////////////////////////////////////////////////////////////////////
// This function sends a MIDI byte and adds it to the checksum
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_SendB(unsigned char b)
{
  MIOS_MIDI_TxBufferPut(b);
  checksum = (checksum + b) & 0x7f;
}

/////////////////////////////////////////////////////////////////////////////
// This function sends a random MIDI byte within the given range
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_SendR(unsigned char min, unsigned char max)
{
  unsigned char tmp;

  if( max < min ) { // swap range if end < begin
    tmp = min; min = max; max = tmp;
  }

  if( min == max ) {
    SID_RANDOM_SendB(min);
  } else {
    SID_RANDOM_SendB(SID_RANDOM_Get(max-min+1) & 0x7f);
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is a variation of SendR and generates the waveform parameter
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_SendRWav(void)
{
  unsigned char wave = SID_RANDOM_Get(9);
  unsigned char sync_ring = SID_RANDOM_Get(4);
  
  SID_RANDOM_SendB(wave | (sync_ring << 5));
}

/////////////////////////////////////////////////////////////////////////////
// This function is a variation of SendR and generates the LFO mode parameter
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_SendRLFO(void)
{
  unsigned char on_off = SID_RANDOM_Get(2);
  unsigned char sync = SID_RANDOM_Get(2);
  unsigned char wave = SID_RANDOM_Get(4);
  SID_RANDOM_SendB(on_off | (sync << 2) | (wave << 4));
}

/////////////////////////////////////////////////////////////////////////////
// This function generates a new random patch and sends it out
/////////////////////////////////////////////////////////////////////////////
void SID_RANDOM_Generate(unsigned char patch)
{
  unsigned char i;

  MIOS_MIDI_BeginStream();
  MIOS_MIDI_TxBufferPut(0xf0);
  MIOS_MIDI_TxBufferPut(0x00);
  MIOS_MIDI_TxBufferPut(0x00);
  MIOS_MIDI_TxBufferPut(0x7e);
  MIOS_MIDI_TxBufferPut(0x46);
  MIOS_MIDI_TxBufferPut(SID_DEVICE_ID);
  MIOS_MIDI_TxBufferPut(0x02);
  MIOS_MIDI_TxBufferPut(patch);

  checksum = 0;
  SID_RANDOM_SendB('R');  // 0x00-0x0f: Patch Name
  SID_RANDOM_SendB('a');
  SID_RANDOM_SendB('n');
  SID_RANDOM_SendB('d');
  SID_RANDOM_SendB('o');
  SID_RANDOM_SendB('m');
  SID_RANDOM_SendB(' ');
  SID_RANDOM_SendB('P');
  SID_RANDOM_SendB('a');
  SID_RANDOM_SendB('t');
  SID_RANDOM_SendB('c');
  SID_RANDOM_SendB('h');
  SID_RANDOM_SendB(' ');
  SID_RANDOM_SendB(' ');
  SID_RANDOM_SendB(' ');
  SID_RANDOM_SendB(' ');

  SID_RANDOM_SendR(127, 127); // 0x10: Volume
  SID_RANDOM_SendR(  0,   0); // 0x11: Poly
  SID_RANDOM_SendR(  0,   0); // 0x12: SusKey
  SID_RANDOM_SendR(  0,   0); // 0x13: MIDI_Sync
  SID_RANDOM_SendR(  0,   7); // 0x14: OSC Sync
  SID_RANDOM_SendR(  0,   7); // 0x15: ENV Curves
  SID_RANDOM_SendR(  0,   0); // 0x16: dummy
  SID_RANDOM_SendR(  0,   0); // 0x17: dummy
  SID_RANDOM_SendR(  0,   7); // 0x18: Filter Channels
  SID_RANDOM_SendR(  0,   7); // 0x19: Filter Mode
  SID_RANDOM_SendR(  0, 127); // 0x1a: Filter CutOff
  SID_RANDOM_SendR(  0, 127); // 0x1b: Filter Resonance
  SID_RANDOM_SendR(  0, 127); // 0x1c: Filter LFOs
  SID_RANDOM_SendR(  0, 127); // 0x1d: Filter ENVs
  SID_RANDOM_SendR(  0, 127); // 0x1e: ENV1 Curve
  SID_RANDOM_SendR(  0, 127); // 0x1f: ENV2 Curve

  SID_RANDOM_SendR( 64,  64); // 0x20: Voice1 Transpose
  SID_RANDOM_SendR( 64,  64); // 0x21: Voice1 Finetune
  SID_RANDOM_SendR(  2,   2); // 0x22: Voice1 Pitchrange
  SID_RANDOM_SendR(  0,  15); // 0x23: Voice1 Portamento (normaly 127, but doesn't sound so good when patch has portamento)
  SID_RANDOM_SendRWav();      // 0x24: Voice1 Waveform
  SID_RANDOM_SendR(  0, 127); // 0x25: Voice1 Pulsewidth
  SID_RANDOM_SendR(  0,   0); // 0x26: Voice1 Arpeggiator
  SID_RANDOM_SendR(  0, 127); // 0x27: Voice1 Delay
  SID_RANDOM_SendR(  0, 127); // 0x28: Voice1 Attack
  SID_RANDOM_SendR(  0, 127); // 0x29: Voice1 Decay
  SID_RANDOM_SendR(  0, 127); // 0x2a: Voice1 Sustain
  SID_RANDOM_SendR(  0, 127); // 0x2b: Voice1 Release
  SID_RANDOM_SendR(  0, 127); // 0x2c: Voice1 LFOs Pitch
  SID_RANDOM_SendR(  0, 127); // 0x2d: Voice1 LFOs PW
  SID_RANDOM_SendR(  0, 127); // 0x2e: Voice1 ENVs
  SID_RANDOM_SendR(  0,   0); // 0x2f: Voice1 Split Lower

  SID_RANDOM_SendR( 64,  64); // 0x30: Voice2 Transpose
  SID_RANDOM_SendR( 64,  64); // 0x31: Voice2 Finetune
  SID_RANDOM_SendR(  2,   2); // 0x32: Voice2 Pitchrange
  SID_RANDOM_SendR(  0,  15); // 0x33: Voice2 Portamento (normaly 127, but doesn't sound so good when patch has portamento)
  SID_RANDOM_SendRWav();      // 0x34: Voice2 Waveform
  SID_RANDOM_SendR(  0, 127); // 0x35: Voice2 Pulsewidth
  SID_RANDOM_SendR(  0,   0); // 0x36: Voice2 Arpeggiator
  SID_RANDOM_SendR(  0, 127); // 0x37: Voice2 Delay
  SID_RANDOM_SendR(  0, 127); // 0x38: Voice2 Attack
  SID_RANDOM_SendR(  0, 127); // 0x39: Voice2 Decay
  SID_RANDOM_SendR(  0, 127); // 0x3a: Voice2 Sustain
  SID_RANDOM_SendR(  0, 127); // 0x3b: Voice2 Release
  SID_RANDOM_SendR(  0, 127); // 0x3c: Voice2 LFOs Pitch
  SID_RANDOM_SendR(  0, 127); // 0x3d: Voice2 LFOs PW
  SID_RANDOM_SendR(  0, 127); // 0x3e: Voice2 ENVs
  SID_RANDOM_SendR(  0,   0); // 0x3f: Voice2 Split Lower

  SID_RANDOM_SendR( 64,  64); // 0x40: Voice3 Transpose
  SID_RANDOM_SendR( 64,  64); // 0x41: Voice3 Finetune
  SID_RANDOM_SendR(  2,   2); // 0x42: Voice3 Pitchrange
  SID_RANDOM_SendR(  0,  15); // 0x43: Voice3 Portamento (normaly 127, but doesn't sound so good when patch has portamento)
  SID_RANDOM_SendRWav();      // 0x44: Voice3 Waveform
  SID_RANDOM_SendR(  0, 127); // 0x45: Voice3 Pulsewidth
  SID_RANDOM_SendR(  0,   0); // 0x46: Voice3 Arpeggiator
  SID_RANDOM_SendR(  0, 127); // 0x47: Voice3 Delay
  SID_RANDOM_SendR(  0, 127); // 0x48: Voice3 Attack
  SID_RANDOM_SendR(  0, 127); // 0x49: Voice3 Decay
  SID_RANDOM_SendR(  0, 127); // 0x4a: Voice3 Sustain
  SID_RANDOM_SendR(  0, 127); // 0x4b: Voice3 Release
  SID_RANDOM_SendR(  0, 127); // 0x4c: Voice3 LFOs Pitch
  SID_RANDOM_SendR(  0, 127); // 0x4d: Voice3 LFOs PW
  SID_RANDOM_SendR(  0, 127); // 0x4e: Voice3 ENVs
  SID_RANDOM_SendR(  0,   0); // 0x4f: Voice3 Split Lower

  SID_RANDOM_SendR(  0,   0); // 0x50: Velocity Assign
  SID_RANDOM_SendR(  0,   0); // 0x51: Velocity Init
  SID_RANDOM_SendR( 64,  64); // 0x52: Velocity Depth
  SID_RANDOM_SendR(  0,   0); // 0x53: Modulation Assign
  SID_RANDOM_SendR(  0,   0); // 0x54: Modulation Init
  SID_RANDOM_SendR( 64,  64); // 0x55: Modulation Depth
  SID_RANDOM_SendR(  0,   0); // 0x56: Aftertouch Assign
  SID_RANDOM_SendR(  0,   0); // 0x57: Aftertouch Init
  SID_RANDOM_SendR( 64,  64); // 0x58: Aftertouch Depth
  SID_RANDOM_SendR(  0,   0); // 0x59: Wavetable Rate
  SID_RANDOM_SendR(  0,   0); // 0x5a: Wavetable P1 Assign
  SID_RANDOM_SendR(  0,   0); // 0x5b: Wavetable P2 Assign
  SID_RANDOM_SendR(  0,   0); // 0x5c: Wavetable P3 Assign
  SID_RANDOM_SendR(127, 127); // 0x5d: Voice1 Split Upper
  SID_RANDOM_SendR(127, 127); // 0x5e: Voice2 Split Upper
  SID_RANDOM_SendR(127, 127); // 0x5f: Voice3 Split Upper

  SID_RANDOM_SendRLFO();      // 0x60: LFO1 Mode
  SID_RANDOM_SendR(  0, 127); // 0x61: LFO1 Rate
  SID_RANDOM_SendR(  0, 127); // 0x62: LFO1 Depth

  SID_RANDOM_SendRLFO();      // 0x63: LFO2 Mode
  SID_RANDOM_SendR(  0, 127); // 0x64: LFO2 Rate
  SID_RANDOM_SendR(  0, 127); // 0x65: LFO2 Depth

  SID_RANDOM_SendRLFO();      // 0x66: LFO3 Mode
  SID_RANDOM_SendR(  0, 127); // 0x67: LFO3 Rate
  SID_RANDOM_SendR(  0, 127); // 0x68: LFO3 Depth

  SID_RANDOM_SendRLFO();      // 0x69: LFO4 Mode
  SID_RANDOM_SendR(  0, 127); // 0x6a: LFO4 Rate
  SID_RANDOM_SendR(  0, 127); // 0x6b: LFO4 Depth

  SID_RANDOM_SendRLFO();      // 0x6c: LFO5 Mode
  SID_RANDOM_SendR(  0, 127); // 0x6d: LFO5 Rate
  SID_RANDOM_SendR(  0, 127); // 0x6e: LFO5 Depth

  SID_RANDOM_SendRLFO();      // 0x6f: LFO6 Mode
  SID_RANDOM_SendR(  0, 127); // 0x70: LFO6 Rate
  SID_RANDOM_SendR(  0, 127); // 0x71: LFO6 Depth

  SID_RANDOM_SendR(  0, 127); // 0x72: ENV1 Depth
  SID_RANDOM_SendR(  0, 127); // 0x73: ENV1 Attack
  SID_RANDOM_SendR(  0, 127); // 0x74: ENV1 Decay
  SID_RANDOM_SendR(  0, 127); // 0x75: ENV1 Sustain
  SID_RANDOM_SendR(  0, 127); // 0x76: ENV1 Release

  SID_RANDOM_SendR(  0, 127); // 0x77: ENV2 Depth
  SID_RANDOM_SendR(  0, 127); // 0x78: ENV2 Attack
  SID_RANDOM_SendR(  0, 127); // 0x79: ENV2 Decay
  SID_RANDOM_SendR(  0, 127); // 0x7a: ENV2 Sustain
  SID_RANDOM_SendR(  0, 127); // 0x7b: ENV2 Release

  SID_RANDOM_SendR(  0,   0); // 0x7c: dummy
  SID_RANDOM_SendR(  0,   0); // 0x7d: dummy
  SID_RANDOM_SendR(  0,   0); // 0x7e: dummy
  SID_RANDOM_SendR(  0,   0); // 0x7f: dummy (channel and device)

  for(i=0; i<128; ++i)
    SID_RANDOM_SendB(0x00);

  MIOS_MIDI_TxBufferPut(-checksum & 0x7f);
  MIOS_MIDI_TxBufferPut(0xf7);
  MIOS_MIDI_EndStream();
}
