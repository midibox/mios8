// $Id: main.c 172 2008-03-06 21:45:07Z tk $
/*
 * VISUAL METRONOME (MIDI Clock Sender)
 *
 * ==========================================================================
 *  Modified by Tim Jefford 2012  (gomiboy99@gmail.com)
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

  MIOS_DOUT_PinSet(9, 1); //STOP
  MIOS_DOUT_PinSet(11, 1); //MIDI CLICK ON

  menu_pos = 1;

  clock_enable = 0;

  metro_high = (note_high * 0x0C);
  metro_low = (note_low * 0x0C);

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

  // request display update
  MIOS_LCD_CursorSet(0x00 + 15);
  MIOS_LCD_PrintCString("BPM");

  if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
            MIOS_LCD_CursorSet(0x40 + 7);
            MIOS_LCD_PrintCString("Ck");
            MIOS_DOUT_PinSet(8, 1);
  }


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

  MIOS_LCD_CursorSet(0x00 + 0);
  MIOS_LCD_PrintCString("C");
  MIOS_LCD_CursorSet(0x00 + 1);
  MIOS_LCD_PrintBCD2(midi_ch);

  MIOS_LCD_CursorSet(0x00 + 4);
  MIOS_LCD_PrintCString("B");
  MIOS_LCD_CursorSet(0x00 + 5);
  MIOS_LCD_PrintBCD1(meas_ctr_beats);

  MIOS_LCD_CursorSet(0x00 + 7);
  MIOS_LCD_PrintCString("MC");
  MIOS_LCD_CursorSet(0x00 + 9);
  MIOS_LCD_PrintBCD1(note_high - 2);

  MIOS_LCD_CursorSet(0x00 + 11);
  MIOS_LCD_PrintCString("BC");
  MIOS_LCD_CursorSet(0x00 + 13);
  MIOS_LCD_PrintBCD1(note_low - 2);

  // print Play/Stop/Pause char at the right upper corner
  MIOS_LCD_CursorSet(0x00 + 19);
  if( mclock_state.PAUSE ) {
    MIOS_LCD_PrintChar(0x02); // Pause
  } else {
    if( mclock_state.RUN ) {
      MIOS_LCD_PrintChar(0x00); // Play
    } else {
      MIOS_LCD_PrintChar(0x01); // Stop
    }
  }

  MIOS_LCD_CursorSet(0x40); // first line
  MIOS_LCD_PrintCString("BPM");
  // print BPM at lower line, left side
  MIOS_LCD_CursorSet(0x40 + 3);
  MIOS_LCD_PrintBCD3(MCLOCK_BPMGet());

  // print the meter at lower line, right side
  MIOS_LCD_CursorSet(0x40 + 9);
  MIOS_LCD_PrintBCD4(mclock_ctr_measures + 1);
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
  // toggle beat LEDs
    MIOS_DOUT_PinGet(mclock_ctr_beats - 1) ? MIOS_DOUT_PinSet0(mclock_ctr_beats - 1) : MIOS_DOUT_PinSet1(mclock_ctr_beats);

  if(( mclock_ctr_beats + 1) != meas_ctr_beats )  {
    MIOS_DOUT_PinSet0(meas_ctr_beats - 1);
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

    case 5: // Menu Scroll
      if( pin_value == 0 )
        menu_pos = ( menu_pos + 1 );
        if( menu_pos == 7 ) {
            menu_pos = 1;
        }
        if( menu_pos == 1) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("BPM");
        } else if( menu_pos == 5 ) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("Ch ");
        } else if( menu_pos == 6 ) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("Clk");
        } else if( menu_pos == 3 ) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("MC ");
        } else if( menu_pos == 4 ) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("BC ");
        } else if( menu_pos == 2 ) {
      MIOS_LCD_CursorSet(0x00 + 15);
      MIOS_LCD_PrintCString("Bar");
        }

  app_flags.DISPLAY_UPDATE_REQ = 1;

      break;

    case 3: // PLAY
      if( pin_value == 0 )
        if( ! mclock_state.START_REQ )
        return;
    MIOS_DOUT_SRSet (0, 0);
    MIOS_DOUT_SRSet (1, 0);
	MCLOCK_DoPlay();
 	MIOS_DOUT_PinSet(10, 1);
    METRO_Event_On();

        if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
	    MIOS_DOUT_PinSet(8, 1);
        } else {
        MIOS_DOUT_PinSet(8, 0);
        }
        if( app_flags.METRONOME_ENABLE_SET == 1) {
        MIOS_DOUT_PinSet(11, 1);
        } else {
        MIOS_DOUT_PinSet(11, 0);
        }

      break;

    case 4: // STOP
      if( pin_value == 0 )
    METRO_Event_Off();
    MIOS_DOUT_SRSet (0, 0);
	MIOS_DOUT_SRSet (1, 0);
	MIOS_DOUT_PinSet(9, 1);

	MCLOCK_DoStop();

        if( app_flags.MIDI_CLOCK_ENABLE == 1 ) {
	    MIOS_DOUT_PinSet(8, 1);
        } else {
        MIOS_DOUT_PinSet(8, 0);
        }
        if( app_flags.METRONOME_ENABLE_SET == 1) {
        MIOS_DOUT_PinSet(11, 1);
        } else {
        MIOS_DOUT_PinSet(11, 0);
        }

      break;

     case 2: // CLOCK SEND ENABLE

      break;

     case 6: // Fwd Button

      break;

     case 7: // Rev Button


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

  if( menu_pos == 1 ) {
  // encoder 0 is used to control the BPM
    if( encoder == 0 ) {
      value = (unsigned int)MCLOCK_BPMGet() - 48;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 255 - 48) ) {
        MCLOCK_BPMSet((unsigned char)value + 48);
        app_flags.DISPLAY_UPDATE_REQ = 1;
      }
    }
  }

  if( menu_pos == 5 ) {
    if( encoder == 0 ) {
      value = (unsigned char)midi_ch;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 17 - 1) ) {
         midi_ch = value;
        if( midi_ch >= 16 ) {
            midi_ch = 16;
        }
      }
        if( midi_ch <= 1 ) {
            midi_ch = 1;
        }
        app_flags.DISPLAY_UPDATE_REQ = 1;
   }
  }

  if( menu_pos == 6 ) {
  // encoder 0 is used to control the BPM
    if( encoder == 0 ) {
      value = (unsigned char)clock_enable;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 2 - 1) ) {
         clock_enable = value;
        if( clock_enable - 1 == 0 ) {
            app_flags.MIDI_CLOCK_ENABLE = 0;
            MIOS_LCD_CursorSet(0x40 + 7);
            MIOS_LCD_PrintCString("  ");
            MIOS_DOUT_PinSet(8, 0);
        } else {
            app_flags.MIDI_CLOCK_ENABLE = 1;
            MIOS_LCD_CursorSet(0x40 + 7);
            MIOS_LCD_PrintCString("Ck");
            MIOS_DOUT_PinSet(8, 1);
        }
        app_flags.DISPLAY_UPDATE_REQ = 1;
      }
    }
  }

  if( menu_pos == 3 ) {
  // encoder 0 is used to control the BPM
    if( encoder == 0 ) {
      value = (unsigned char)note_high;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 10 - 1) ) {
         note_high = value;

       if(note_high > 9 ) {
            note_high = 9;
        }
        if(note_high < 1 ) {
            note_high = 1;
        }
        if(note_high - 1 != 0 ) {  // enable or disable metronome midi notes
            app_flags.METRONOME_ENABLE_SET = 1;
            MIOS_DOUT_PinSet(11, 1);
        } else {
            app_flags.METRONOME_ENABLE_SET = 0;
            MIOS_DOUT_PinSet(11, 0);
        }
        metro_high = (note_high * 0x0C); // note_high * 12

            app_flags.DISPLAY_UPDATE_REQ = 1;
      }
    }
  }

  if( menu_pos == 4 ) {
  // encoder 0 is used to control the BPM
    if( encoder == 0 ) {
      value = (unsigned char)note_low;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 10 - 1) ) {
         note_low = value;

       if(note_low > 9 ) {
            note_low = 9;
        }
        if(note_low < 1 ) {
            note_low = 1;
        }
        metro_low = (note_low * 0x0C); // note_low * 12

            app_flags.DISPLAY_UPDATE_REQ = 1;
      }
    }
  }

  if( menu_pos == 2 ) {
  // encoder 0 is used to control the BPM
    if( encoder == 0 ) {
      value = (unsigned char)meas_ctr_beats;
      if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 9 - 1) ) {
         meas_ctr_beats = value;
        MCLOCK_DoStop();
        METRO_Event_Off();
        if( meas_ctr_beats >= 8 ) {
            meas_ctr_beats = 8;
        }
        if( meas_ctr_beats <= 2 ) {
            meas_ctr_beats = 2;
        }
      }
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
