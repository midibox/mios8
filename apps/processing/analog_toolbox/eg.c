// $Id$
/*
 * EG program module
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 * 
 * ==========================================================================
 * 
 * This file is part of a MIOS application
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <cmios.h>
#include <pic18fregs.h>

#include "main.h"
#include "eg.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// in order to achive best performance, we are NOT using arrays here!

// external accessible variables
eg_state_t    eg0;
unsigned int  eg0_value;
unsigned char eg0_attack, eg0_decay, eg0_sustain, eg0_release;

// internal state (not externally available)
eg_int_state_t eg0_int;

// this includes the eg_add_table
// it was generated with: perl utils\eg_table.pl > eg_table.inc
#include "eg_table.inc"


/////////////////////////////////////////////////////////////////////////////
// This function initializes the EG module
// it should be called from USER_Init() in main.c
/////////////////////////////////////////////////////////////////////////////
void EG_Init(void)
{
  eg0.ALL     = 0x00;
  eg0_int.ALL = 0x00;

  eg0_value       = 0x00;

  eg0_attack  = 0x40;
  eg0_decay   = 0x40;
  eg0_sustain = 0x80;
  eg0_release = 0x40;
}


/////////////////////////////////////////////////////////////////////////////
// This function is periodically invoked from USER_Timer()
/////////////////////////////////////////////////////////////////////////////
void EG_Tick(void)
{
  // in order to simplify the handling, we are working with an unsigned value here
  // (C doesn't natively provide a carry flag :-/ )
  unsigned int eg_value;
  unsigned int lvl;

  if( eg0.GATE == 1 && eg0_int.LAST_GATE == 0 ) { // rising edge
    eg0_int.LAST_GATE = 1;
    eg0_int.STATE = ATTACK;
  } else if( eg0.GATE == 0 && eg0_int.LAST_GATE == 1 ) { // falling edge
    eg0_int.LAST_GATE = 0;
    eg0_int.STATE = RELEASE;
  }
  // eg0_int.LAST_GATE = eg0.GATE; // doesn't work with SDCC v2.5.0

  eg_value = eg0_value >> 1;

  switch( eg0_int.STATE ) {
    case IDLE:
      eg_value = 0x0000;
      break;

    case ATTACK:
      if( (eg_value += eg_add_table[eg0_attack]) & 0x8000 ) {
        eg_value = 0x7fff;
	eg0_int.STATE = DECAY;
      }
      break;

    case DECAY:
      lvl = (eg0_sustain << 7);

      if( ((eg_value -= eg_add_table[eg0_decay]) <= lvl) || (eg_value & 0x8000) ) {
        eg_value = lvl;
	//        eg0_int.STATE = SUSTAIN;
      }
      break;

    case SUSTAIN:
      eg_value = (eg0_sustain << 7);
      break;

    case RELEASE:
      if( (eg_value -= eg_add_table[eg0_release]) & 0x8000 ) {
        eg_value = 0;
	eg0_int.STATE = IDLE;
      }
      break;
  }

  eg0_value = eg_value << 1;
}

