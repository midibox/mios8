/*
 * ACSyncronizer
 * based on MIDI Clock module by TK (mclock.h/mclock.c)
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 * Addional Code Copyright (C) 2007  Michael Markert, http://www.audiocommander.de
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

#include "ACSyncronizer.h"
#include "ACSensorizer.h"


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

mclock_state_t mclock_state;			// the mclock state variable
mclock_sync_state_t mclock_sync_state;	// state of the clock output pin

unsigned char mclock_tick_ctr;			// requests MIDI clocks

unsigned char bpm;						// holds the current BPM setting

unsigned char mclock_ctr_96;			// counts from 0..95
unsigned char mclock_ctr_24;			// counts from 0..23
unsigned char mclock_ctr_beats;			// counts the quarter notes 0..3
unsigned int  mclock_ctr_measures;		// counts the measures (up to 65535)



/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

unsigned int  mclock_master_timeout;	// see MASTER_IS_ALIVE_COUNTER


/////////////////////////////////////////////////////////////////////////////
// This function initializes the MIDI clock module
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_Init(void)
{
	mclock_state.ALL = 0;
	mclock_sync_state.ALL = 0;
	mclock_tick_ctr = 0;
	mclock_master_timeout = 0;
	mclock_state.IS_MASTER = TRUE;	// will switch to FALSE if clock input detected!
	ACSYNC_ResetMeter();
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick to send MIDI clock and
// MIDI Clock Start/Stop/Continue
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_Tick(void)
{
	// start request? Send 0xfa and enter RUN mode
	if( mclock_state.START_REQ ) {
		mclock_state.START_REQ = 0;
#if FORWARD_CLOCK == 0
		if( mclock_state.IS_MASTER ) {
			MIOS_MIDI_TxBufferPut(MIDI_START);		// 0xfa
		}
#else
		MIOS_MIDI_TxBufferPut(MIDI_START);
#endif /* FORWARD_CLOCK == 0 */
		mclock_state.RUN = 1;
	}
	
	// continue request? Send 0xfb and release pause
	if( mclock_state.CONT_REQ ) {
		mclock_state.CONT_REQ = 0;
#if FORWARD_CLOCK == 0
		if( mclock_state.IS_MASTER ) {
			MIOS_MIDI_TxBufferPut(MIDI_CONTINUE);	// 0xfb
		}
#else
		MIOS_MIDI_TxBufferPut(MIDI_CONTINUE);
#endif /* FORWARD_CLOCK == 0 */
		mclock_state.PAUSE = 0;
	}
	
	// stop request? Send 0xfc and leave  RUN mode
	if( mclock_state.STOP_REQ ) {
		mclock_state.STOP_REQ = 0;
#if FORWARD_CLOCK == 0
		if( mclock_state.IS_MASTER ) {
			MIOS_MIDI_TxBufferPut(MIDI_STOP);		// 0xfc
		}
#else
		MIOS_MIDI_TxBufferPut(MIDI_STOP);
#endif /* FORWARD_CLOCK == 0 */
		mclock_state.RUN = 0;
		mclock_state.PAUSE = 0;
		mclock_tick_ctr = 0; // prevent that any requested 0xf8 will be sent
	}
	
#if SENSORIZER_INTERFACE_SYNC
	// update action
	if(mclock_sync_state.UPDATE_REQ) {
		mclock_sync_state.UPDATE_REQ = FALSE;		
		// trigger clock related actions
		ACSensorizer_ReceiveClock();
		if(mclock_ctr_96 == 0) {
			// reset decrementers to keep readings in sync
			sensorizer.resetMeter = TRUE;
#if SEND_CONTINUE_EACH_BAR
			// send continue fix for m4 sync bug
			mclock_state.CONT_REQ = TRUE;
#endif /* SEND_CONTINUE_EACH_BAR */
		}
	}		
#endif /* SENSORIZER_INTERFACE_SYNC */
	
	while( mclock_tick_ctr ) {
		// send 0xf8 until counter is 0
#if FORWARD_CLOCK == 0
		if( mclock_state.IS_MASTER ) {			// send clock only if master
			MIOS_MIDI_TxBufferPut(MIDI_CLOCK);	// 0xf8
		}
#else
		MIOS_MIDI_TxBufferPut(MIDI_CLOCK);		// 0xf8
#endif
		// decrementing the counter *MUST* be an atomic operation, otherwise it could
		// conflict with ACSYNC_Timer()
		// however, I guess that the compiler will generate a single decf instruction,
		// which is atomic... but better to write it in this way, who knows, how SDCC
		// will behave in future...
		INTCONbits.GIE = 0; // disable interrupts
		--mclock_tick_ctr;
		INTCONbits.GIE = 1; // enable interrupts
		// increment the meter counters
		if( ++mclock_ctr_24 == 24 ) {
			mclock_ctr_24 = 0;
			if( ++mclock_ctr_beats == 4 ) {
				mclock_ctr_beats = 0;
				++mclock_ctr_measures;
			}
		}
		if( ++mclock_ctr_96 == 96 ) { mclock_ctr_96 = 0; }
	}
	
	// if in SLAVE-MODE check if master is still alive
	if( mclock_state.IS_MASTER == 0 ) {
		++mclock_master_timeout;
		// check for timeout
		if( mclock_master_timeout > MASTER_IS_ALIVE_COUNTER ) {
			mclock_master_timeout = 0;
			// fall back to master mode
			mclock_state.IS_MASTER = TRUE;
			ACSYNC_BPMSet(bpm);		// set old bpm value and reInit Timer
			// request display update
			acapp.displayNeedsUpdate = 1;
		}
	}
	
	// refresh view
	switch(mclock_ctr_24) {
		case 1: case 6: case 12: case 18:
			acapp.displayTickUpdate = TRUE;
			break;
	}
	
#if ACSYNC_GARBAGE_SERVICE
	// frequently called cleanup service routine
	if(mclock_sync_state.RESET_REQ) {
		mclock_sync_state.RESET_REQ = FALSE;
		// send panic
		ACMidi_SendPanic(16);		// if value > 15 send panic on all channels
		// reset meters & sync-module
		ACSYNC_DoRun();
	}
#endif /* ACSYNC_GARBAGE_SERVICE */	
	
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Timer to update the MIDI clock
//   MASTER:	Timer set by bmp value, called 96 times per bar
//	 SLAVE:		Timer polled by ReceiveClock, called 96 times per bar
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_Timer(void)
{	
	// just increment a clock counter 
	// it will be decremented in ACSYNC_Tick - each step sends one 0xf8 event
	// the advantage of this counter is, that a clock event can never get
	// lost, regardless how much the CPU is loaded
	
	//  RUN and PAUSE mode should not be queried here because we're dealing with
	// automatic Master/Slave detection. ACSensorizer should be hearable in all cases
	// so the timer must run. START/STOP messages are used to re-sync only
	
//	if( mclock_state.RUN && !mclock_state.PAUSE) {
		++mclock_tick_ctr;
		mclock_sync_state.CLOCK_REQ = TRUE;
		mclock_sync_state.UPDATE_REQ = TRUE;	// call sensorizer_timer each clock tick
//	}
	
	// debug output
#ifdef _DEBUG_C
#if DEBUG_SYSREALTIME_VERBOSE
	printf("\n**ACSYNC_MIDICLOCK: \t %i/4 \t%i/96 \t%i/24 \t%i/x", mclock_ctr_beats, mclock_ctr_96, mclock_ctr_24, mclock_ctr_measures );
#endif /* SYSREALTIME_VERBOSE */
#endif /* _DEBUG_C */
	// END debug output
	
	// re-sync sensorizer subclock
	if(mclock_ctr_96 == 0) { mclock_sync_state.UPDATE_REQ = TRUE; }
	
#if ACSYNC_GARBAGE_SERVICE
	// reset all counters
	if( (mclock_ctr_measures >= ACSYNC_RESET_AFTER_X_MEASURES) && (mclock_ctr_24 >= ACSYNC_GARBAGE_SERVICE_CTR) ) {
		mclock_sync_state.RESET_REQ = TRUE;
	}
#endif /* ACSYNC_GARBAGE_SERVICE */
	
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from MPROC_NotifyReceivedByte
// to update the MIDI clock
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_ReceiveClock(unsigned char byte) __wparam {
	switch(byte) {
		case MIDI_CLOCK:
			if( mclock_state.IS_MASTER ) {	
				// fall back to slave mode
				mclock_state.IS_MASTER = FALSE;
				ACSYNC_BPMSet(0);	// stops timer (passive call in SLAVE mode)
				// request display update
				acapp.displayNeedsUpdate = 1;
			} else {
				// reset lifesign counter (master is alive)
				mclock_master_timeout = 0;
				// call timer
				ACSYNC_Timer();		// passive timer call in SLAVE mode
			}
			break;
		case MIDI_START:
			ACSYNC_DoRun();
			break;
		case MIDI_STOP:
			ACSYNC_DoStop();
			break;
		case MIDI_CONTINUE:
			ACSYNC_DoPause();
	}
}


/////////////////////////////////////////////////////////////////////////////
// This internal function divides 3125000 / BPM
// The formula:
//   -> delay = 60 / BPM * 24
//   timer is clocked at 10 MHz, and we are using a 1:8 prescaler
//   -> timer cycles = ((60/BPM*24)/8) / 100E-9
//   -> 3125000 / BPM
//
// the 24 Bit / 16 Bit division routine has been created by Nikolai Golovchenko, 
// and is published at:
// http://www.piclist.org/techref/microchip/math/div/24by16.htm
/////////////////////////////////////////////////////////////////////////////
unsigned char AARGB0;
unsigned char AARGB1;
unsigned char AARGB2;
unsigned char BARGB0;
unsigned char BARGB1;
unsigned char LOOPCOUNT;
unsigned char REMB0;
unsigned char REMB1;
unsigned int ACSYNC_GetTimerValue(unsigned char _bpm) 
{
	// don't divide by zero
	if(_bpm == 0) { return 0; }
	
#ifndef _DEBUG_C
	//;Inputs:
	//;   Dividend - AARGB0:AARGB1:AARGB2 (0 - most significant!)
	//;   Divisor  - BARGB0:BARGB1
	//;Temporary:
	//;   Counter  - LOOPCOUNT
	//;   Remainder- REMB0:REMB1
	//;Output:
	//;   Quotient - AARGB0:AARGB1:AARGB2
	
	LOOPCOUNT = 24;
	AARGB0 = (unsigned char)(3125000L >> 16L);
	AARGB1 = (unsigned char)(3125000L >>  8L);
	AARGB2 = (unsigned char)(3125000L >>  0L);
	BARGB0 = 0;
	BARGB1 = _bpm;
	REMB0 = 0;
	REMB0 = 1;
	
	__asm
		banksel _LOOPCOUNT;
	
		CLRF _REMB0
        CLRF _REMB1
        MOVLW 24
        MOVWF _LOOPCOUNT
LOOPU2416:
        RLCF _AARGB2, W         ;shift dividend left to move next bit to remainder
		RLCF _AARGB1, F         ;
        RLCF _AARGB0, F         ;
		
        RLCF _REMB1, F          ;shift carry (next dividend bit) into remainder
		RLCF _REMB0, F
			
		RLCF _AARGB2, F         ;finish shifting the dividend and save  carry in AARGB2.0,
								;since remainder can be 17 bit long in some cases
								;(e.g. 0x800000/0xFFFF). This bit will also serve
								;as the next result bit.
		
		MOVF _BARGB1, W         ;substract divisor from 16-bit remainder
		SUBWF _REMB1, F         ;
		MOVF _BARGB0, W         ;
		SKPC                    ;
		INCFSZ _BARGB0, W       ;
		SUBWF _REMB0, F         ;
							
								;here we also need to take into account the 17th bit of remainder, which
								;is in AARGB2.0. If we dont have a borrow after subtracting from lower
								;16 bits of remainder, then there is no borrow regardless of 17th bit 
								;value. But, if we have the borrow, then that will depend on 17th bit 
								;value. If it is 1, then no final borrow will occur. If it is 0, borrow
								;will occur. These values match the borrow flag polarity.
											
		SKPNC                   ;if no borrow after 16 bit subtraction
		BSF _AARGB2, 0          ;then there is no borrow in result. Overwrite
								;AARGB2.0 with 1 to indicate no borrow.
								;if borrow did occur, AARGB2.0 already
								;holds the final borrow value (0-borrow, 1-no borrow)
											
		BTFSC _AARGB2, 0        ;if no borrow after 17-bit subtraction
		BRA UOK46LL             ;skip remainder restoration.
												
		ADDWF _REMB0, F         ;restore higher byte of remainder; contains the value subtracted from it previously)
		MOVF _BARGB1, W         ;restore lower byte of remainder
		ADDWF _REMB1, F         ;

UOK46LL:
		DECFSZ _LOOPCOUNT, f    ;decrement counter
		BRA LOOPU2416			;and repeat the loop if not zero.

		movff _AARGB1, _PRODL
		movf  _AARGB2, W
		return
__endasm;

return 0; // dummy return 

#else 

return ((unsigned int)(3125000.0 / bpm));

#endif /* _DEBUG_C */

}


/////////////////////////////////////////////////////////////////////////////
// These functions are used to set/query the BPM
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_BPMSet(unsigned char _bpm)
{
	// (re-)init timer depending on new BPM value
	if( _bpm > 47 ) {
		MIOS_TIMER_ReInit(3, ACSYNC_GetTimerValue(_bpm));
		bpm = _bpm;
	} else {	// _bpm is 0 => timer off (slave mode)
		MIOS_TIMER_Stop();
		mclock_state.IS_MASTER = FALSE;
	}
}

unsigned char ACSYNC_BPMGet(void)
{
	if( mclock_state.IS_MASTER ) {
		return bpm;
	} else {
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// This function resets the mclock_ctr variables
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_ResetMeter(void)
{
	mclock_ctr_96 = 0;
	mclock_ctr_24 = 0;
	mclock_ctr_beats = 0;
	mclock_ctr_measures = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function sends the current song position
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_SendMeter(void)
{
	unsigned int songpos = (mclock_ctr_beats << 2) | (mclock_ctr_measures << 4);
	
	MIOS_MIDI_TxBufferPut(0xf2);
	MIOS_MIDI_TxBufferPut((unsigned char)(songpos & 0x7f));
	MIOS_MIDI_TxBufferPut((unsigned char)(songpos >> 7) & 0x7f);
}


/////////////////////////////////////////////////////////////////////////////
// These functions are used to control the ACSYNC handler from external
/////////////////////////////////////////////////////////////////////////////
void ACSYNC_DoStop(void)
{
	if( mclock_state.RUN == 0 ) {
		// reset song position of already in stop mode
		ACSYNC_ResetMeter();
		// send Song Position
		ACSYNC_SendMeter();
	}
	// request stop
	mclock_state.STOP_REQ = 1;
}

void ACSYNC_DoPause(void)
{
	// if in RUN mode:
	if( mclock_state.RUN ) {
		// toggle pause mode
		if( mclock_state.PAUSE ) {
			mclock_state.CONT_REQ = 1;
		} else {
			mclock_state.PAUSE = 1;
		}
	} else {
		// Stop mode: just toggle PAUSE
		mclock_state.PAUSE = mclock_state.PAUSE ? 0 : 1;
	}
}

void ACSYNC_DoRun(void)
{
	// reset meter counters
	ACSYNC_ResetMeter();
	// send Song Position
	ACSYNC_SendMeter();
	// request start
	mclock_state.START_REQ = 1;
}

void ACSYNC_DoRew(void)
{
	// decrement measure and reset subcounters
	if( mclock_ctr_measures ) --mclock_ctr_measures;
	mclock_ctr_24 = 0;
	mclock_ctr_beats = 0;
	
	// send Song Position
	ACSYNC_SendMeter();
}

void ACSYNC_DoFwd(void)
{
	// increment measure and reset subcounters
	++mclock_ctr_measures;
	if(mclock_ctr_measures > 65534) { mclock_ctr_measures = 0; }
	mclock_ctr_24 = 0;
	mclock_ctr_beats = 0;
	
	// send Song Position
	ACSYNC_SendMeter();
}


