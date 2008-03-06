/*
 * Clockbox (MIDI Clock Sender)
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
#include "mtc.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  // set shift register update frequency
  MIOS_SRIO_UpdateFrqSet(1); // ms

  // we need to set at least one IO shift register pair
  MIOS_SRIO_NumberSet(NUMBER_OF_SRIO);

  // debouncing value for DINs
  MIOS_SRIO_DebounceSet(DIN_DEBOUNCE_VALUE);

  // Touch sensor sensitivity *must* be 0, otherwise Port D.4 (CORE::J14) cannot be used as Clock Output
  MIOS_SRIO_TS_SensitivitySet(0);

  // set encoder speed mode of datawheel
  MIOS_ENC_SpeedSet(0, DATAWHEEL_SPEED_MODE, DATAWHEEL_SPEED_DIVIDER);

#if MTC_MODE
  // initialize the MIDI clock module (-> mclock.c)
  MTC_Init();
  MTC_FPSSet(3); // type 3: 30 fps
#else
  // initialize the MIDI clock module (-> mclock.c)
  MCLOCK_Init();
  MCLOCK_BPMSet(140);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
#if MTC_MODE
  // this routine sends the MTC code if requested
  MTC_Tick();
#else
  // this routine sends the MIDI clock (and Start/Continue/Stop) if requested
  MCLOCK_Tick();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
#if MTC_MODE
  // forward timer event to MTC module (-> mtc.c)
  MTC_Timer();
#else
  // forward timer event to MIDI clock module (-> mclock.c)
  MCLOCK_Timer();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////

// default special character set
// created with http://lcd5x8char.midibox.org
//        ;; char #1
//	db b'00001000', b'00001100'; line 1 / 2
//	db b'00001110', b'00001111'; line 3 / 4
//	db b'00001111', b'00001110'; line 5 / 6
//	db b'00001100', b'00001000'; line 7 / 8
//	;; char #2
//	db b'00000000', b'00000000'; line 1 / 2
//	db b'00001110', b'00001110'; line 3 / 4
//	db b'00001110', b'00001110'; line 5 / 6
//	db b'00000000', b'00000000'; line 7 / 8
//	;; char #3
//	db b'00000000', b'00001010'; line 1 / 2
//	db b'00001010', b'00001010'; line 3 / 4
//	db b'00001010', b'00001010'; line 5 / 6
//	db b'00001010', b'00000000'; line 7 / 8

const unsigned char lcd_charset[3*8] = {
  0x08, 0x0c, 0x0e, 0x0f, 0x0f, 0x0e, 0x0c, 0x08, // Play
  0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, // Stop
  0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, // Pause
  };

void DISPLAY_Init(void) __wparam
{
  // init default charset
  MIOS_CLCD_SpecialCharsInit(lcd_charset);

  // clear screen
  MIOS_LCD_Clear();

  // print default screen
  MIOS_LCD_CursorSet(0x00); // first line
#if MTC_MODE
  MIOS_LCD_PrintCString("FPS Time");
#else
  MIOS_LCD_PrintCString("BPM  Meter");
#endif

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  unsigned char fps;

  // do nothing if no update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;

  // clear request
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // print Play/Stop/Pause char at the right upper corner
  MIOS_LCD_CursorSet(0x00 + 15);
#if MTC_MODE
  if( mtc_state.PAUSE ) {
    MIOS_LCD_PrintChar(0x02); // Pause
  } else {
    if( mtc_state.RUN ) {
      MIOS_LCD_PrintChar(0x00); // Play
    } else {
      MIOS_LCD_PrintChar(0x01); // Stop
    }
  }

  // print FPS at lower line, left side
  MIOS_LCD_CursorSet(0x40 + 0);
  switch( MTC_FPSGet() ) {
    case 0: fps = 24; break;
    case 1: fps = 25; break;
    case 2: fps = 30; break;
    case 3: fps = 30; break;
    default: fps = 0; break;
  }
  MIOS_LCD_PrintBCD3(fps);

  // print the meter at lower line, right side
  MIOS_LCD_CursorSet(0x40 + 4);
  MIOS_LCD_PrintBCD1(mtc_ctr_hours);
  MIOS_LCD_PrintChar(':');
  MIOS_LCD_PrintBCD2(mtc_ctr_min);
  MIOS_LCD_PrintChar(':');
  MIOS_LCD_PrintBCD2(mtc_ctr_sec);
  MIOS_LCD_PrintChar('.');
  MIOS_LCD_PrintBCD2(mtc_ctr_frame_x_4 / 4);
#else
  if( mclock_state.PAUSE ) {
    MIOS_LCD_PrintChar(0x02); // Pause
  } else {
    if( mclock_state.RUN ) {
      MIOS_LCD_PrintChar(0x00); // Play
    } else {
      MIOS_LCD_PrintChar(0x01); // Stop
    }
  }

  // print BPM at lower line, left side
  MIOS_LCD_CursorSet(0x40 + 0);
  MIOS_LCD_PrintBCD3(MCLOCK_BPMGet());

  // print the meter at lower line, right side
  MIOS_LCD_CursorSet(0x40 + 4);
  MIOS_LCD_PrintBCD5(mclock_ctr_measures + 1);
  MIOS_LCD_PrintChar(':');
  MIOS_LCD_PrintChar(' ');
  MIOS_LCD_PrintBCD1(mclock_ctr_beats + 1);
  MIOS_LCD_PrintChar(':');
  MIOS_LCD_PrintBCD3(mclock_ctr_24 * 5);
#endif
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has been received
// which has been specified in the MIOS_MPROC_EVENT_TABLE
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI event has not been completly
// received within 2 seconds
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyTimeout(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a MIDI byte has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Prepare(void) __wparam
{
  // this function is called each millisecond
  // we are using it to output the MIDI clock at Pin D.4 for 1 mS
  if( mclock_pin_state.CLK_REQ ) {
    mclock_pin_state.CLK_REQ = 0;
    PORTDbits.RD4 = 1;
  } else {
    PORTDbits.RD4 = 0;
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after the shift register have been loaded
/////////////////////////////////////////////////////////////////////////////
void SR_Service_Finish(void) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{
  // note: pin 0 and 1 are assigned to datawheel (-> see mios_wrapper/mios_tables.inc)

  switch( pin ) {
    case 7: // Stop Button
      if( pin_value == 0 ) 
#if MTC_MODE
	MTC_DoStop();
#else
	MCLOCK_DoStop();
#endif
      break;

    case 6: // Pause Button
      if( pin_value == 0 ) 
#if MTC_MODE
	MTC_DoPause();
#else
	MCLOCK_DoPause();
#endif
      break;

    case 5: // Play Button
      if( pin_value == 0 ) 
#if MTC_MODE
	MTC_DoPlay();
#else
	MCLOCK_DoPlay();
#endif
      break;

    case 4: // Rew Button
      if( pin_value == 0 ) 
#if MTC_MODE
	MTC_DoRew();
#else
	MCLOCK_DoRew();
#endif
      break;

    case 3: // Fwd Button
      if( pin_value == 0 ) 
#if MTC_MODE
	MTC_DoFwd();
#else
	MCLOCK_DoFwd();
#endif
      break;

    case 8:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(0);
      break;

    case 9:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(0);
      break;

    case 10:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(1);
      break;

    case 11:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(1);
      break;

    case 12:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(2);
      break;

    case 13:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(2);
      break;

    case 14:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(3);
      break;

    case 15:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(3);
      break;

    case 16:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(4);
      break;

    case 17:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(4);
      break;

    case 18:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(5);
      break;

    case 19:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(5);
      break;

    case 20:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(6);
      break;

    case 21:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(6);
      break;

    case 22:
      if( pin_value == 0 )
	MCLOCK_DoMultiStop(7);
      break;

    case 23:
      if( pin_value == 0 )
	MCLOCK_DoMultiPlay(7);
      break;

    default:
#if 0
      // enable this for debugging (displays the number of a non-assigned pin)
      MIOS_LCD_CursorSet(0x00);
      MIOS_LCD_PrintCString("DIN: ");
      MIOS_LCD_PrintBCD3(pin);
      MIOS_LCD_PrintChar(pin_value ? 'o' : '*');
#endif
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
  unsigned int value;

  // encoder 0 is used to control the BPM or second (MTC mode)
  if( encoder == 0 ) {
#if MTC_MODE
    if( incrementer < 0 ) {
      while( ++incrementer != 0 )
	MTC_DoRew();
    } else {
      while( --incrementer != 0 )
	MTC_DoFwd();
    }
#else
    value = (unsigned int)MCLOCK_BPMGet() - 48;
    if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 255 - 48) ) {
      MCLOCK_BPMSet((unsigned char)value + 48);
      app_flags.DISPLAY_UPDATE_REQ = 1;
    }
#endif
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}
