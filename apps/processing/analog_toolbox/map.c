// $Id$
/*
 * MAP program module
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
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
#include "aout.h"
#include "midi.h"
#include "lfo.h"
#include "eg.h"
#include "map.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// generated with "perl utils\sinwave.pl"
const unsigned char sinewave[256] = {
  128,131,134,137,140,143,146,149,152,156,159,162,165,168,171,174,
  176,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
  218,220,222,224,226,228,230,232,234,236,237,239,240,242,243,245,
  246,247,248,249,250,251,252,252,253,254,254,255,255,255,255,255,
  255,255,255,255,255,255,254,254,253,252,252,251,250,249,248,247,
  246,245,243,242,240,239,237,236,234,232,230,228,226,224,222,220,
  218,216,213,211,209,206,204,201,199,196,193,191,188,185,182,179,
  176,174,171,168,165,162,159,156,152,149,146,143,140,137,134,131,
  128,125,122,119,116,113,110,107,104,100, 97, 94, 91, 88, 85, 82,
   80, 77, 74, 71, 68, 65, 63, 60, 57, 55, 52, 50, 47, 45, 43, 40,
   38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 19, 17, 16, 14, 13, 11,
   10,  9,  8,  7,  6,  5,  4,  4,  3,  2,  2,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  2,  2,  3,  4,  4,  5,  6,  7,  8,  9,
   10, 11, 13, 14, 16, 17, 19, 20, 22, 24, 26, 28, 30, 32, 34, 36,
   38, 40, 43, 45, 47, 50, 52, 55, 57, 60, 63, 65, 68, 71, 74, 77,
   80, 82, 85, 88, 91, 94, 97,100,104,107,110,113,116,119,122,125,
};

extern const unsigned int lfo_add_table[256]; 

/////////////////////////////////////////////////////////////////////////////
// This function initializes the MAP module
// it should be called from USER_Init() in main.c
/////////////////////////////////////////////////////////////////////////////
void MAP_Init(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This function is invoked each time nothing else is to do
// it should be called from USER_Tick() in main.c
/////////////////////////////////////////////////////////////////////////////
void MAP_Tick(void)
{
  int result;
  unsigned char value_8bit;

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #1: forward Note value of MIDI channel #1
  aout_value[0] = CONV_7BIT_TO_12BIT(midi_note[0].NOTE);
  // first gate: forward gate (Note On/Off) of MIDI channel #1
  aout_gates.G0 = midi_note[0].GATE;

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #2: forward CC#1 (Modwheel) of MIDI channel #1
  aout_value[1] = CONV_7BIT_TO_12BIT(midi_cc_chn0[1]);
  // second gate: 1 if CC#1 value >= 64, else 0
  aout_gates.G1 = midi_cc_chn0[1] >= 64 ? 1 : 0;

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #3: forward AIN0 + AIN1 - AIN2
  // saturation if result < 0
  result = MIOS_AIN_PinGet(0) + MIOS_AIN_PinGet(1) - MIOS_AIN_PinGet(2);
  aout_value[2] = result >= 0 ? result : 0;

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #4: forward f(AIN3) -- the function describes a sinewave
  // note that sinwave[] contains 256 values and returns an 8bit value
  // this means that we have to do following steps:

  value_8bit = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(3)); // convert 10bit value to 8bit
  value_8bit = sinewave[value_8bit];                   // map to sinwave table
  aout_value[3] = CONV_8BIT_TO_12BIT(value_8bit);      // convert to 12bit

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #5: forward f(t) -- the function describes a saw wave
  // t is driven by LFO0, the rate is modified with AIN4
  lfo0_rate = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(4));
  aout_value[4] = CONV_16BIT_TO_12BIT(lfo0_value);

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #6: forward f(t) -- the function describes a sine wave
  // t is driven by LFO1, the rate is modified with AIN5
  lfo1_rate = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(5));
  aout_value[5] = CONV_8BIT_TO_12BIT(sinewave[CONV_16BIT_TO_8BIT(lfo1_value)]);

  ///////////////////////////////////////////////////////////////////////////
  // AOUT #7: forward EG0 output
  aout_value[6] = CONV_16BIT_TO_12BIT(eg0_value);
  eg0.GATE = midi_note[0].GATE; // EG triggered with MIDI note at Channel #1

#if 1
  // for direct pot Control
  eg0_attack  = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(0));
  eg0_decay   = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(1));
  eg0_sustain = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(2));
  eg0_release = CONV_10BIT_TO_8BIT(MIOS_AIN_PinGet(3));
#else
  // for MIDI control
  eg0_attack  = midi_cc_chn0[16]; // CC#16
  eg0_decay   = midi_cc_chn0[17]; // CC#17
  eg0_sustain = midi_cc_chn0[18]; // CC#18
  eg0_release = midi_cc_chn0[19]; // CC#19
#endif
}

