/*
 * VISUAL METRONOME
 * MIDI Clock module
 *
 * ==========================================================================
 *  Modified by Tim Jefford 2012  (gomiboy99@gmail.com)
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
unsigned char meas_ctr_beats;

unsigned char metro_note;

unsigned char encoder;
unsigned char note_on;
unsigned char note_off;
unsigned char note_vel;
unsigned char metro_high;
unsigned char metro_low;
unsigned char midi_ch;
unsigned char note_low;
unsigned char note_high;
unsigned char menu_pos;
unsigned char eeprom_location;

unsigned char clock_enable;
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

  /////////////////////////////////////////////////////////////////////////
  //  DEFAULT SETTINGS                                                   //
  /////////////////////////////////////////////////////////////////////////

  eeprom_location = (MIOS_EEPROM_Read(0x07)); //READ LAST PRESET USED

  MCLOCK_BPMSet(MIOS_EEPROM_Read((eeprom_location * 8) + 0));
  midi_ch = (MIOS_EEPROM_Read((eeprom_location * 8) + 3)); // default channel 1
  note_high = (MIOS_EEPROM_Read((eeprom_location * 8) + 1));  // C4
  note_low = (MIOS_EEPROM_Read((eeprom_location * 8) + 2));   // C5

  note_on = (0x8F + midi_ch);   // DO NOT CHANGE
  note_off = (0x7F + midi_ch);  // DO NOT CHANGE
  note_vel = 0x79;  // MAX 127 VELOCITY

  menu_pos = 8; //PGM

  metro_high = (note_high * 0x0C);

  metro_low = (note_low * 0x0C);

  meas_ctr_beats = (MIOS_EEPROM_Read((eeprom_location * 8) + 4));  //DEFAULT

 if( MIOS_EEPROM_Read((eeprom_location * 8) + 6) == 1 ) {
                    app_flags.METRONOME_ENABLE_SET = 1;
            } else {
                    app_flags.METRONOME_ENABLE_SET = 0;
                    }   //METRONOME MIDI OUT ENABLED BY DEFAULT

  if( MIOS_EEPROM_Read((eeprom_location * 8) + 5) == 1 ) {
                    app_flags.MIDI_CLOCK_ENABLE = 1;
            } else {
                    app_flags.MIDI_CLOCK_ENABLE = 0;
                    }

 // app_flags.MIDI_CLOCK_ENABLE = 1; //ENABLE CLOCK OUT BY DEFAULT
     app_flags.DISPLAY_UPDATE_REQ = 1;

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
    if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
    MIOS_MIDI_TxBufferPut(0xfa);
    }
    mclock_state.RUN = 1;

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // continue request? Send 0xfb and release pause
//  if( mclock_state.CONT_REQ ) {
//    mclock_state.CONT_REQ = 0;
//    if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
//    MIOS_MIDI_TxBufferPut(0xfb);
//    }
//    mclock_state.PAUSE = 0;

    // request display update
//    app_flags.DISPLAY_UPDATE_REQ = 1;
//  }

  // stop request? Send 0xfc and leave  RUN mode
  if( mclock_state.STOP_REQ ) {
    mclock_state.STOP_REQ = 0;
    if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
    MIOS_MIDI_TxBufferPut(0xfc);
    }
    mclock_state.RUN = 0;
    mclock_state.PAUSE = 0;
    mclock_tick_ctr = 0; // prevent that any requested 0xf8 will be sent

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

  // send 0xf8 until counter is 0
  while( mclock_tick_ctr ) {
    if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
    MIOS_MIDI_TxBufferPut(0xf8);
    }
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
      if( ++mclock_ctr_beats == meas_ctr_beats ) {
	mclock_ctr_beats = 0;
	++mclock_ctr_measures;
      }
    }
    if ( mclock_ctr_24 == 0) {
    METRO_Event_On();
    }
    if ( mclock_ctr_24 == 3) { // 3 TICKS NOTE DURATION
    METRO_Event_Off();
    }

    // request display update
    app_flags.DISPLAY_UPDATE_REQ = 1;
  }

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
  unsigned int songpos = (mclock_ctr_beats << 2) | (mclock_ctr_measures << meas_ctr_beats);

  if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {

  MIOS_MIDI_TxBufferPut(0xf2);
  MIOS_MIDI_TxBufferPut((unsigned char)(songpos & 0x7f));
  MIOS_MIDI_TxBufferPut((unsigned char)(songpos >> 7) & 0x7f);
  }
}
/////////////////////////////////////////////////////////////////////////////
// This function sends note on and note off for the metronome
/////////////////////////////////////////////////////////////////////////////

void METRO_Event_On(void)
{
  if( app_flags.METRONOME_ENABLE_SET == 0 )
    return;
  if( mclock_ctr_beats == 0 ) {
        metro_note = metro_high;
    } else {
        metro_note = metro_low;
  }
  MIOS_MIDI_TxBufferPut(note_on + (midi_ch - 1 ));
  MIOS_MIDI_TxBufferPut(metro_note);
  MIOS_MIDI_TxBufferPut(note_vel);

}

void METRO_Event_Off(void)
{
  if( app_flags.METRONOME_ENABLE_SET == 0 )
    return;
  if( mclock_ctr_beats == 0 ) {
    metro_note = metro_high;
  } else {
    metro_note = metro_low;
  }
  MIOS_MIDI_TxBufferPut(note_off + ( midi_ch - 1 ));
  MIOS_MIDI_TxBufferPut(metro_note);
  MIOS_MIDI_TxBufferPut(note_vel);
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



