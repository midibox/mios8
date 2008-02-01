// $Id$
/*
 * AOUT program module
 * This module is contains a lot of assembler-optimized code for fastest execution
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

#include "aout.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// AOUT values
unsigned int aout_value[8];

// stores the last AOUT values (to determine, if the shift registers have to be updated)
unsigned int last_aout_value[8];

// the gate pins
aout_gates_t aout_gates;

// last state of the aout gate flags to determine changes
aout_gates_t last_aout_gates;

/////////////////////////////////////////////////////////////////////////////
// This function initializes the AOUT module
/////////////////////////////////////////////////////////////////////////////
void AOUT_Init(void)
{
  unsigned char i;

  // Enable pin drivers
__asm
	;; since the pins are defined in assembler-friendly syntax (-> aout.h)
	;; we have to clear the pins within an ASM block
	bcf	AOUT_TRIS_CS, AOUT_PIN_CS
	bcf	AOUT_TRIS_SCLK, AOUT_PIN_SCLK
	bcf	AOUT_TRIS_DIN, AOUT_PIN_DIN
__endasm;

  // clear gate pins and request update
  aout_gates.ALL = 0x00;
  last_aout_gates.ALL = 0xff;

  // set AOUT voltages to 0V and request update
  for(i=0; i<sizeof(aout_value); ++i) {
    aout_value[i] = 0x0000;
    last_aout_value[i] = 0xffff;
  }
}


/////////////////////////////////////////////////////////////////////////////
// This is an internal assembler function which updates two chained 
// MAX525 devices
/////////////////////////////////////////////////////////////////////////////
// following variables are placed outside the function (instead of making them static)
// to simplify the addressing within an assembler block
unsigned char loop_ctr;    // loop counter
unsigned int aout_sr0; // chain data
unsigned int aout_sr1;
void AOUT_Load2SR(unsigned int sr0_value, unsigned int sr1_value)
{
  aout_sr0 = sr0_value;
  aout_sr1 = sr1_value;

__asm
        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; clear clock
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select

	;; you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	banksel _loop_ctr
	movlw	32				; init loop counter
	movwf	_loop_ctr, B

AOUT_Load2SR_Loop:
	banksel _aout_sr0
	bcf	AOUT_LAT_DIN, AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	_aout_sr1+1, 7, B
	bsf	AOUT_LAT_DIN, AOUT_PIN_DIN
	rlcf	_aout_sr0+0, F, B		; start to shift the 32-bit value
	rlcf	_aout_sr0+1, F, B		; second step for the 32-bit shift
	bsf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; rising clock edge
	rlcf	_aout_sr1+0, F, B		; third step for the 32-bit shift
	rlcf	_aout_sr1+1, F, B		; last step for the 32-bit shift
	bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; falling clock edge

	banksel _loop_ctr
	decfsz	_loop_ctr, F, B		; loop 32 times
	bra	AOUT_Load2SR_Loop

	bsf	AOUT_LAT_CS, AOUT_PIN_CS; deactivate chip select

__endasm;
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick() to update the requested
// AOUT pins
/////////////////////////////////////////////////////////////////////////////
void AOUT_Update(void)
{
  if( (aout_value[0] != last_aout_value[0]) || (aout_value[4] != last_aout_value[4]) ) {
    AOUT_Load2SR(aout_value[0] | (0x3000 | (0 << 14)), aout_value[4] | (0x3000 | (0 << 14)));
    last_aout_value[0] = aout_value[0];
    last_aout_value[4] = aout_value[4];
  }

  if( (aout_value[1] != last_aout_value[1]) || (aout_value[5] != last_aout_value[5]) ) {
    AOUT_Load2SR(aout_value[1] | (0x3000 | (1 << 14)), aout_value[5] | (0x3000 | (1 << 14)));
    last_aout_value[1] = aout_value[1];
    last_aout_value[5] = aout_value[5];
  }

  if( (aout_value[2] != last_aout_value[2]) || (aout_value[6] != last_aout_value[6]) ) {
    AOUT_Load2SR(aout_value[2] | (0x3000 | (2 << 14)), aout_value[6] | (0x3000 | (2 << 14)));
    last_aout_value[2] = aout_value[2];
    last_aout_value[6] = aout_value[6];
  }

  if( (aout_value[3] != last_aout_value[3]) || (aout_value[7] != last_aout_value[7]) ) {
    AOUT_Load2SR(aout_value[3] | (0x3000 | (3 << 14)), aout_value[7] | (0x3000 | (3 << 14)));
    last_aout_value[3] = aout_value[3];
    last_aout_value[7] = aout_value[7];
  }

  if( aout_gates.ALL != last_aout_gates.ALL ) {
    AOUT_Load2SR(aout_gates.G0 ? 0x4000 : 0x2000, aout_gates.G1 ? 0x4000 : 0x2000);
    last_aout_gates.ALL = aout_gates.ALL;
  }
}

