/*
 *  ACToolbox.c
 *  kII
 *
 *  Created by audiocommander on 25.02.07.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
 *
 */

#include "ACToolbox.h"



// **** globals ****
unsigned char random_seed_l;
unsigned char random_seed_h;




//////////////////////////////////////////////////////////////////////////////////////////
//	This function divides two integers; There may be better ASM optimized 
//	solutions for this, but the generated ASM code doesn't look any more
//	complicated than the ASM example I've found  ;)
//	Thanks to goule: http://www.midibox.org/forum/index.php?topic=5944.0
//////////////////////////////////////////////////////////////////////////////////////////

unsigned int ACMath_Divide(unsigned int a, unsigned int b) __wparam 
{
	// divdes a / b
#ifdef _DEBUG_C
	if(b > 0) {	
		return ((unsigned int)(a / b));
	} else {
		return 0;
	}
#else
	unsigned int rest = 0;
	unsigned char count = 16;
	char c;
	
	do {
		c = ((a >> (8*sizeof(a)-1)) & 1);
		a <<= 1;
		rest <<= 1;
		if (c) rest |= 1;
		if (rest >= b) {
			rest -= b;
			a |= 1;
		}
	} while (--count);
	return a;
#endif
}




/////////////////////////////////////////////////////////////////////////////
//	This is an assembly optimized function which scales a 7bit value (0-127)
//	between a minimum and maximum value (eg 0 and 64)
//  Code by TK http://www.ucapps.de
/////////////////////////////////////////////////////////////////////////////

unsigned char ACMath_Scale(unsigned char v, unsigned char min, unsigned char max) __wparam
{
	// scales an 7-bit (0-255) value between min & max
#ifdef _DEBUG_C
	unsigned char PRODL;
	unsigned char PRODH;
	unsigned int  PRODLH;
	
	if(v == 0) { return 0; }
	PRODL = v << 1;			// 8bit value
	PRODH = max - min + 1;	// range
	PRODLH = PRODL * PRODH;
	return min + (unsigned char)(PRODLH >> 8);
#else
	if(v == 0) { return 0; }
	// scaled value is (<8-bit> * ) >> 8
	PRODL = v << 1;			// 8bit value
	PRODH = max - min + 1;	// range
	__asm
		movf _PRODL, W		// PRODL in W
		mulwf _PRODH, 0		// W * PRODH => result is 16bit { PRODH:PRODL }
	__endasm;
	
	return min + PRODH;		// return (min + (PRODH:PRODL / 16) );
#endif
}




//////////////////////////////////////////////////////////////////////////////////////////
// This function generates a 16 bit random number
// The code is taken from TK @ http://www.midibox.org/forum/index.php?topic=6211.0
// The 8bit uchar number is returned immediately
// An 16bit int can be returned by combining (random_seed_l & random_seed_h)
//////////////////////////////////////////////////////////////////////////////////////////


unsigned char ACMath_Random(void) {
#ifdef _DEBUG_C
//	random_seed_l = ACRandomChar();
	unsigned int tmp = random_seed_l * random_seed_h;
	random_seed_l = MIOS_GPCounterGet() + (unsigned char)(tmp & 0xff);
	random_seed_h = 0x69 + (unsigned char)(tmp >> 8);
#else
	unsigned int tmp = random_seed_l * random_seed_h;
	random_seed_l = TMR0L + (unsigned char)(tmp & 0xff);
	random_seed_h = 0x69 + (unsigned char)(tmp >> 8);
#endif
	return random_seed_l;
}


unsigned char ACMath_RandomInRange(unsigned char rmax) __wparam {
	unsigned char c = ACMath_Random();
	while(c > rmax) {
		c = c >> 1;
	}
	return c;
}




