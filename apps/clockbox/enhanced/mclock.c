// $Id$
/*
 * MIDI Clock module
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
#include "mclock.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

mclock_state_t mclock_state; // the mclock state variable
mclock_pin_state_t mclock_pin_state; // state of the clock output pin

unsigned char mclock_tick_ctr; // requests MIDI clocks

unsigned char bpm; // holds the current BPM setting

unsigned char mclock_ctr_24;       // counts from 0..23
unsigned char mclock_ctr_beats;    // counts the quarter notes 0..3
unsigned char mclock_ctr_measures; // counts the measures (up to 65535)

unsigned char multi_clk_mask;      // mask for multi clock option
unsigned char multi_synched_start; // each output has a flag, synched start means: with the next 1/4 note
unsigned char multi_synched_stop;  // each output has a flag, synched stop means: with the next 1/4 note

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This function initializes the MIDI clock module
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_Init(void)
{
  mclock_state.ALL = 0;
  mclock_pin_state.ALL = 0;
  mclock_tick_ctr = 0;

  MCLOCK_ResetMeter();

#if MULTI_CLOCK_OPTION
  // init J5 as output
  ADCON1 = 0x07; // disable ADC which allocated the analog pins

  // set pin values to of PORTA.0/1/2/3/5 and PORTE.0/1/2 to 1 (idle)
  LATA |= 0x2f;
  TRISE |= 0x07;

  // turn on the output drivers of PORTA.0/1/2/3/5 and PORTE.0/1/2
  TRISA &= ~0x2f; // (~ means: inverted value)
  TRISE &= ~0x07;
#endif
  multi_clk_mask = 0xff; // by default, all 8 clock outputs enabled
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Tick to send MIDI clock and
// MIDI Clock Start/Stop/Continue
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_Tick(void)
{
  // start request? Send 0xfa and enter RUN mode
  if( mclock_state.START_REQ ) {
    mclock_state.START_REQ = 0;
    MIOS_MIDI_TxBufferPut(0xfa);
#if MULTI_CLOCK_OPTION
    MCLOCK_SendMultiPort(0xfa, 0xff);
#endif
    mclock_state.RUN = 1;

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // continue request? Send 0xfb and release pause
  if( mclock_state.CONT_REQ ) {
    mclock_state.CONT_REQ = 0;
    MIOS_MIDI_TxBufferPut(0xfb);
#if MULTI_CLOCK_OPTION
    MCLOCK_SendMultiPort(0xfb, 0xff);
#endif
    mclock_state.PAUSE = 0;

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // stop request? Send 0xfc and leave  RUN mode
  if( mclock_state.STOP_REQ ) {
    mclock_state.STOP_REQ = 0;
    MIOS_MIDI_TxBufferPut(0xfc);
#if MULTI_CLOCK_OPTION
    MCLOCK_SendMultiPort(0xfc, 0xff);
#endif
    mclock_state.RUN = 0;
    mclock_state.PAUSE = 0;
    mclock_tick_ctr = 0; // prevent that any requested 0xf8 will be sent

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // send 0xf8 until counter is 0
  while( mclock_tick_ctr ) {
    // decrementing the counter *MUST* be an atomic operation, otherwise it could
    // conflict with MCLOCK_Timer()
    // however, I guess that the compiler will generate a single decf instruction,
    // which is atomic... but better to write it in this way, who knows, how SDCC
    // will behave in future...
    INTCONbits.GIE = 0; // disable interrupts
    --mclock_tick_ctr;
    INTCONbits.GIE = 1; // enable interrupts

    // increment the meter counters
    if( ++mclock_ctr_24 == 24 ) {
      mclock_ctr_24 = 0;

#if MULTI_CLOCK_OPTION
      // next beat:

      // check for synched stop
      if( multi_synched_stop ) {
	// send stop event to slaves which should break
	MCLOCK_SendMultiPort(0xfc, multi_synched_stop);
	
	// takeover of synched stop flags
	multi_clk_mask &= ~multi_synched_stop;
	
	// clear requests
	multi_synched_stop = 0;
      }

      // check for synched start
      if( multi_synched_start ) {
	// send continue event to slaves which should continue
	MCLOCK_SendMultiPort(0xfb, multi_synched_start);

	// takeover of synched start flags
	multi_clk_mask |= multi_synched_start;

	// clear requests
	multi_synched_start = 0;
      }
#endif

      if( ++mclock_ctr_beats == 4 ) {
	mclock_ctr_beats = 0;
	++mclock_ctr_measures;
      }

    }

    // send clocks
    MIOS_MIDI_TxBufferPut(0xf8);

#if MULTI_CLOCK_OPTION
    MCLOCK_SendMultiPort(0xf8, multi_clk_mask);
#endif

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

#if MULTI_CLOCK_OPTION && MULTI_CLOCK_STATUS_DOUT_SR
  // forward current clock masks to DOUT, so that LEDs can display if clocks are enabled/disabled
  MIOS_DOUT_SRSet((MULTI_CLOCK_STATUS_DOUT_SR-1) & 0xff, multi_clk_mask);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function should be called from USER_Timer to update the MIDI clock
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_Timer(void)
{
  // just increment a clock counter if in RUN and not in PAUSE mode
  // it will be decremented in MCLOCK_Tick - each step sends one 0xf8 event
  // the advantage of this counter is, that a clock event can never get
  // lost, regardless how much the CPU is loaded
  // (however, in this application the CPU is very unbusy, but I prepared
  // this for even more complex programs...)

  if( mclock_state.RUN && !mclock_state.PAUSE) {
    ++mclock_tick_ctr;
    mclock_pin_state.CLK_REQ = 1;
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
unsigned int MCLOCK_GetTimerValue(unsigned char bpm)
{

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
  BARGB1 = bpm;

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
         BSF _AARGB2, 0         ;then there is no borrow in result. Overwrite
                                ;AARGB2.0 with 1 to indicate no
                                ;borrow.
                                ;if borrow did occur, AARGB2.0 already
                                ;holds the final borrow value (0-borrow,
                                ;1-no borrow)

        BTFSC _AARGB2, 0        ;if no borrow after 17-bit subtraction
         BRA UOK46LL            ;skip remainder restoration.

        ADDWF _REMB0, F         ;restore higher byte of remainder. (w 
                                ;contains the value subtracted from it
                                ;previously)
        MOVF _BARGB1, W         ;restore lower byte of remainder
        ADDWF _REMB1, F         ;

UOK46LL:

        DECFSZ _LOOPCOUNT, f    ;decrement counter
         BRA LOOPU2416         ;and repeat the loop if not zero.

      movff _AARGB1, _PRODL
      movf  _AARGB2, W
      return
__endasm;
 return 0; // dummy return 
}


/////////////////////////////////////////////////////////////////////////////
// This function sends a MIDI byte over the pins of CORE:J5 ("Bit Banging" Method)
// Works only if the "MULTI_CLOCK_OPTION" flag in main.h is enabled
// Timer0 and the output drivers of pin RA.0/1/2/3/5 and RE.0/1/2 are prepared
// in MCLOCK_Init
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_SendMultiPort(unsigned char value, unsigned char mask)
{
  INTCONbits.GIE = 0; // disable all interrupts to ensure stable timings

  // using bit banging method, this requires some inline assembly code
  // the assembly routine expects:
  TBLPTRL = value;				  // value in TBLPTRL
  PRODL = ((~mask & 0x10) << 1) | (~mask & 0x0f); // LATA mask in PRODL
  PRODH = (~mask >> 5) & 0x07;                    // LATE mask in PRODH

__asm
	;; include the "SEND_BIT" macro
	;; this macro is stored in a separate file, since with SDCC it
	;; is not possible to declare macros within inline assembly blocks
	include "send_bit_macro.inc"


	;; sending routine starts here
	;; each bit takes 32 uS

	;; send start bit
	movlw	~0x2f			; start bit (0)
	iorwf	_PRODL, W		; apply OR mask (pins which should not sent are set to 1)
	andwf	_LATA, F
	movlw	~0x07
	iorwf	_PRODH, W		; apply OR mask (pins which should not sent are set to 1)
	andwf	_LATE, F

	movlw	103			; delay loop (required cycles have been measured with simulator)
	movwf	_TABLAT
MCLOCK_SendMultiPort_StartLoop:
	decfsz	_TABLAT, F
	bra	MCLOCK_SendMultiPort_StartLoop
	nop

	;; send MIDI value (LSB first)
	SEND_BIT 0
	SEND_BIT 1
	SEND_BIT 2
	SEND_BIT 3
	SEND_BIT 4
	SEND_BIT 5
	SEND_BIT 6
	SEND_BIT 7

	nop				; (balance cylces)
	nop				; (balance cylces)
	nop				; (balance cylces)

	;; send stop bit
	movlw	0x2f			; stop bit (1)
	nop				; (balance cycles)
	iorwf	_LATA, F
	movlw	0x07
	nop				; (balance cycles)
	iorwf	_LATE, F

	movlw	103			; delay loop (required cycles have been measured with simulator)
	movwf	_TABLAT
MCLOCK_SendMultiPort_StopLoop:
	decfsz	_TABLAT, F
	bra	MCLOCK_SendMultiPort_StopLoop

__endasm;

  INTCONbits.GIE = 1; // enable all interrupts again
}

/////////////////////////////////////////////////////////////////////////////
// These functions are used to set/query the BPM
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_BPMSet(unsigned char _bpm)
{
  // re-init timer depending on new BPM value
  bpm = _bpm;
  MIOS_TIMER_ReInit(3, MCLOCK_GetTimerValue(bpm));
}

unsigned char MCLOCK_BPMGet(void)
{
  return bpm;
}


/////////////////////////////////////////////////////////////////////////////
// This function resets the mclock_ctr variables
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_ResetMeter(void)
{
  mclock_ctr_24 = 0;
  mclock_ctr_beats = 0;
  mclock_ctr_measures = 0;
}

/////////////////////////////////////////////////////////////////////////////
// This function sends the current song position
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_SendMeter(void)
{
  unsigned int songpos = (mclock_ctr_beats << 2) | (mclock_ctr_measures << 4);

  MIOS_MIDI_TxBufferPut(0xf2);
  MIOS_MIDI_TxBufferPut((unsigned char)(songpos & 0x7f));
  MIOS_MIDI_TxBufferPut((unsigned char)(songpos >> 7) & 0x7f);
}


/////////////////////////////////////////////////////////////////////////////
// These functions are used to control the MCLOCK handler from external
/////////////////////////////////////////////////////////////////////////////
void MCLOCK_DoStop(void)
{
  if( mclock_state.RUN == 0 ) {
    // reset song position of already in stop mode
    MCLOCK_ResetMeter();
    // send Song Position
    MCLOCK_SendMeter();
  }
  // request stop
  mclock_state.STOP_REQ = 1;

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MCLOCK_DoPause(void)
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

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MCLOCK_DoPlay(void)
{
  // reset meter counters
  MCLOCK_ResetMeter();
  // send Song Position
  MCLOCK_SendMeter();
  // request start
  mclock_state.START_REQ = 1;

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MCLOCK_DoRew(void)
{
  // decrement measure and reset subcounters
  if( mclock_ctr_measures ) --mclock_ctr_measures;
  mclock_ctr_24 = 0;
  mclock_ctr_beats = 0;

  // send Song Position
  MCLOCK_SendMeter();

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MCLOCK_DoFwd(void)
{
  // increment measure and reset subcounters
  ++mclock_ctr_measures;
  mclock_ctr_24 = 0;
  mclock_ctr_beats = 0;

  // send Song Position
  MCLOCK_SendMeter();

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

void MCLOCK_DoMultiPlay(unsigned char out)
{
  // request synched start for given output
  multi_synched_start |= (1 << out);
  // ensure that stop request is cleared
  multi_synched_stop &= ~(1 << out);
}

void MCLOCK_DoMultiStop(unsigned char out)
{
  // request synched stop for given output
  multi_synched_stop |= (1 << out);
  // ensure that start request is cleared
  multi_synched_start &= ~(1 << out);
}
