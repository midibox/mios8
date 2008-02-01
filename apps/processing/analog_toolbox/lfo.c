// $Id$
/*
 * LFO program module
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
#include "lfo.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// in order to achive best performance, we are NOT using arrays here!

// external accessible variables
unsigned int  lfo0_value, lfo1_value;
unsigned char lfo0_rate,  lfo1_rate;

// this includes the lfo_add_table
// it was generated with: perl utils\lfo_table.pl > lfo_table.inc
#include "lfo_table.inc"


/////////////////////////////////////////////////////////////////////////////
// This function initializes the LFO module
// it should be called from USER_Init() in main.c
/////////////////////////////////////////////////////////////////////////////
void LFO_Init(void)
{
  lfo0_value = lfo0_rate = 0x00;
  lfo1_value = lfo1_rate = 0x00;
}


/////////////////////////////////////////////////////////////////////////////
// This function is periodically invoked from USER_Timer()
/////////////////////////////////////////////////////////////////////////////
void LFO_Tick(void)
{
  // this generates a saw waveform with the given frequency rate
  // it can be easily converted to other waveforms by using transforming tables
  // (-> see example in map.c, the sinwave[] array)

  if( lfo0_rate > 0 ) {
    lfo0_value += lfo_add_table[lfo0_rate];
  }

  // note: here you could also add some wired modulations, e.g. lfo0_value controls lfo1_rate

  if( lfo1_rate > 0 ) {
    lfo1_value += lfo_add_table[lfo1_rate];
  }

}

