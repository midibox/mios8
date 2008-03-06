// $Id$
/*
 * Clockbox (MIDI Clock Sender)
 *
 * ==========================================================================
 *
 *  Copyright 2005 Thorsten Klose (tk@midibox.org)
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

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;

/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Application specific encoder table
// the default (dummy) table has been disabled via -DDONT_INCLUDE_MIOS_ENC_TABLE
/////////////////////////////////////////////////////////////////////////////
MIOS_ENC_TABLE {
             // sr pin mode
  MIOS_ENC_ENTRY(1, 0, MIOS_ENC_MODE_DETENTED2),
  MIOS_ENC_EOT
};



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

  // initialize the MIDI clock module (-> mclock.c)
  MCLOCK_Init();
  MCLOCK_BPMSet(140);
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
  // this routine sends the MIDI clock (and Start/Continue/Stop) if requested
  MCLOCK_Tick();
}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
  // forward timer event to MIDI clock module (-> mclock.c)
  MCLOCK_Timer();
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
  MIOS_LCD_PrintCString("BPM  Meter");

  // request display update
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  // do nothing if no update has been requested
  if( !app_flags.DISPLAY_UPDATE_REQ )
    return;

  // clear request
  app_flags.DISPLAY_UPDATE_REQ = 0;

  // print Play/Stop/Pause char at the right upper corner
  MIOS_LCD_CursorSet(0x00 + 15);
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
	MCLOCK_DoStop();
      break;

    case 6: // Pause Button
      if( pin_value == 0 ) 
	MCLOCK_DoPause();
      break;

    case 5: // Play Button
      if( pin_value == 0 ) 
	MCLOCK_DoPlay();
      break;

    case 4: // Rew Button
      if( pin_value == 0 ) 
	MCLOCK_DoRew();
      break;

    case 3: // Fwd Button
      if( pin_value == 0 ) 
	MCLOCK_DoFwd();
      break;
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

  // encoder 0 is used to control the BPM
  if( encoder == 0 ) {
    value = (unsigned int)MCLOCK_BPMGet() - 48;
    if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 255 - 48) ) {
      MCLOCK_BPMSet((unsigned char)value + 48);
      app_flags.DISPLAY_UPDATE_REQ = 1;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}
