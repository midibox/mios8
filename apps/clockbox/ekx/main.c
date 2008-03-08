// $Id$
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

#include "cmios.h"
#include "pic18f452.h"

#include "main.h"
#include "mclock.h"

/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

// status of application (see bitfield declaration in main.h)
app_flags_t app_flags;
//determines the bpm division, set to
// 1:  1:1
// 2:  1:2
// 3:  1:3
// 4:  1:4
unsigned char divisor[] = {0,0,0,0,0,0,0,0};
unsigned char t_divisor[] = {0,0,0,0,0,0,0,0};
unsigned char shuffle[] = {0,0,0,0,0,0,0,0};
unsigned char t_shuffle[] = {0,0,0,0,0,0,0,0};

//this track will be edited
unsigned char current_trk = 0;
unsigned char enc_mode = 0;
unsigned char shift = 0;
unsigned char m_page = 0;
unsigned char m_button = 0;
unsigned char save_patch = 0;

unsigned char out_config_sc_mask = 0;		// mask for output config (start/continue)
unsigned char out_config_bb_mask = 0;		// mask for output config (start on next bar/beat)
unsigned char out_config_sp_mask = 0;		// mask for output config (send song-position)


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{
  unsigned int i = 0;
  unsigned int k = 0;
  unsigned char error = 0;

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
  MCLOCK_BPMSet(132);

  // check if bankstick is available
  if ( MIOS_BOX_STAT.BS_AVAILABLE ) {
	if ( MIOS_BANKSTICK_Read(0x00) != 0xcb && MIOS_BANKSTICK_Read(0x01) != 0xcb) {
	  // format bankstick
	  MIOS_LCD_CursorSet(0x00);
	  MIOS_LCD_PrintCString("Format Bankstick");
	  MIOS_LCD_CursorSet(0x40);
	  error |= MIOS_BANKSTICK_Write(0x00, 0xcb);
	  error |= MIOS_BANKSTICK_Write(0x01, 0xcb);

	  for ( i = 2; i < 32767; i++ ) {
  		error |= MIOS_BANKSTICK_Write(i, 0x17);
		if ( ++k >= 2048 ) {
			k = 0;
			MIOS_LCD_PrintChar('>');
		}
	  }
	}
   if ( error ) {
	  MIOS_LCD_CursorSet(0x40);
	  MIOS_LCD_PrintCString("BS-Format failed");
      MIOS_LCD_MessageStart(255);
   }
  }
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
//  ;; char #1
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
  app_flags.DISPLAY_UPDATE_REQ = 1;
}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{
  unsigned char div;

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

  if ( shift == 1 ) {
  	// print menu
  	MIOS_LCD_CursorSet(0x40); // lower line
  	switch (m_page) {
		case 0:
  			MIOS_LCD_PrintCString("BPM SFL  DIV SCS");
  			break;
		case 1:
  			MIOS_LCD_PrintCString("BBC SPC  SAV LOA");
  			break;
		case 2:
  			MIOS_LCD_PrintCString("TRK             ");
  			break;
  	}
  }
  else {
  	// print meter
	MIOS_LCD_CursorSet(0x40 + 0);
	MIOS_LCD_PrintCString("Pos ");
  	MIOS_LCD_PrintBCD5(mclock_ctr_measures + 1);
  	MIOS_LCD_PrintChar(':');
  	MIOS_LCD_PrintChar(' ');
  	MIOS_LCD_PrintBCD1(mclock_ctr_beats + 1);
  	MIOS_LCD_PrintChar(':');
  	MIOS_LCD_PrintBCD3(mclock_ctr_96 * 5);
  }

// enc-modes: 0 = bpm; 1 = trk; 2 = div; 3 = sfl; 4 = ssc; 5 = bbc; 6 = spc; 7 = sav; 8 = loa

#if MENU_BUTTONS
  // check m_button
  switch (m_button) {
	case 1:

  		switch (m_page) {
		  case 0:
			enc_mode = 0;
		  break;
		  case 1:
			enc_mode = 5;
		  break;
		  case 2:
			enc_mode = 1;
		  break;
		}
	break;
 	case 2:
  		switch (m_page) {
		  case 0:
			enc_mode = 3;
		  break;
		  case 1:
			enc_mode = 6;
		  break;
		}
	break;
 	case 3:
  		switch (m_page) {
		  case 0:
			enc_mode = 2;
		  break;
		  case 1:
			enc_mode = 7;
			app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		  case 9:
			PatchWrite(save_patch);
			enc_mode = 0;
		    m_page = 1;
		    app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		  case 10:
			PatchRead(save_patch);
			enc_mode = 0;
		    m_page = 1;
		    app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		}
	break;
 	case 4:
  		switch (m_page) {
		  case 0:
			enc_mode = 4;
		  break;
		  case 1:
			enc_mode = 8;
			app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		  case 9:
			enc_mode = 0;
		    m_page = 1;
		    app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		  case 10:
			enc_mode = 0;
		    m_page = 1;
		    app_flags.DISPLAY_UPDATE_REQ = 1;
		  break;
		}
	break;
  }

  m_button = 0;
#endif

  //print actual editmode and value
  switch (enc_mode) {
	case 0: //BPM
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("BPM");
		MIOS_LCD_CursorSet(0x00 + 4);
		MIOS_LCD_PrintBCD3(MCLOCK_BPMGet());
		MIOS_LCD_CursorSet(0x00 + 7);
		MIOS_LCD_PrintCString("        ");
		break;
	case 1: //Track
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("TRK");
		MIOS_LCD_CursorSet(0x00 + 4);
		MIOS_LCD_PrintCString("0");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_CursorSet(0x00 + 6);
		MIOS_LCD_PrintCString("         ");
		break;
	case 2: //Divider
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("DIV");
		MIOS_LCD_CursorSet(0x00 + 4);
		MIOS_LCD_PrintChar('/');
		MIOS_LCD_PrintBCD1(t_divisor[current_trk]+1);
		MIOS_LCD_CursorSet(0x00 + 6);
		MIOS_LCD_PrintCString("    (T");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_PrintChar(')');
		break;
	case 3: //Shuffle
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("SFL ");
		MIOS_LCD_CursorSet(0x00 + 4);
		MIOS_LCD_PrintChar('P');
		MIOS_LCD_PrintBCD1(t_shuffle[current_trk]);
		MIOS_LCD_CursorSet(0x00 + 6);
		MIOS_LCD_PrintCString("    (T");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_PrintChar(')');
		break;
	case 4: //start/continue setting
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("SCS");
		MIOS_LCD_CursorSet(0x00 + 4);

		if ( BitTest(out_config_sc_mask,current_trk) == 1 ) {
		  MIOS_LCD_PrintCString("Start");
		}
		else {
		  MIOS_LCD_PrintCString("Cont.");
		}

		MIOS_LCD_CursorSet(0x00 + 9);
		MIOS_LCD_PrintCString(" (T");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_PrintChar(')');
		break;
	case 5: //start on bar/beat setting
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("BBC");
		MIOS_LCD_CursorSet(0x00 + 4);

		if ( BitTest(out_config_bb_mask,current_trk) == 1 ) {
		  MIOS_LCD_PrintCString("Bar  ");
		}
		else {
		  MIOS_LCD_PrintCString("Beat ");
		}

		MIOS_LCD_CursorSet(0x00 + 9);
		MIOS_LCD_PrintCString(" (T");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_PrintChar(')');
		break;
	case 6: //song-position setting
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("SPC");
		MIOS_LCD_CursorSet(0x00 + 4);

		if ( BitTest(out_config_sp_mask,current_trk) == 1 ) {
		  MIOS_LCD_PrintCString("Send ");
		}
		else {
		  MIOS_LCD_PrintCString("No   ");
		}

		MIOS_LCD_CursorSet(0x00 + 9);
		MIOS_LCD_PrintCString(" (T");
		MIOS_LCD_PrintBCD1(current_trk+1);
		MIOS_LCD_PrintChar(')');
		break;
	case 7: //Save
		m_page = 9;
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("SAV ");
		MIOS_LCD_CursorSet(0x00 + 4);
		if ( save_patch < 4 ) {
		  MIOS_LCD_PrintCString("INT ");
		  MIOS_LCD_CursorSet(0x00 + 8);
		  MIOS_LCD_PrintBCD3(save_patch+1);
		  MIOS_LCD_PrintCString("     ");
		}
		else {
		  MIOS_LCD_PrintCString("BS ");
		  MIOS_LCD_CursorSet(0x00 + 7);
		  MIOS_LCD_PrintBCD3(save_patch-3);
		  MIOS_LCD_PrintCString("      ");
		}
   		MIOS_LCD_CursorSet(0x40);
#if MENU_BUTTONS
   		MIOS_LCD_PrintCString("Save?    Yes  No");
#else
   		MIOS_LCD_PrintCString("Fwd=Yes Rew=Exit");
#endif
		break;
	case 8: //Load
		m_page = 10;
		MIOS_LCD_CursorSet(0x00 + 0);
		MIOS_LCD_PrintCString("LOA ");
		MIOS_LCD_CursorSet(0x00 + 4);
		if ( save_patch < 4 ) {
		  MIOS_LCD_PrintCString("INT ");
		  MIOS_LCD_CursorSet(0x00 + 8);
		  MIOS_LCD_PrintBCD3(save_patch+1);
		  MIOS_LCD_PrintCString("     ");
		}
		else {
		  MIOS_LCD_PrintCString("BS ");
		  MIOS_LCD_CursorSet(0x00 + 7);
		  MIOS_LCD_PrintBCD3(save_patch-3);
		  MIOS_LCD_PrintCString("      ");
		}
   		MIOS_LCD_CursorSet(0x40);
#if MENU_BUTTONS
   		MIOS_LCD_PrintCString("Load?    Yes  No");
#else
   		MIOS_LCD_PrintCString("Fwd=Yes Rew=Exit");
#endif
		break;
	}
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
#if MENU_BUTTONS
    case 7: // Shift Button
      if( pin_value == 0 ) {
	  	shift = 1;
  	  }
  	  else {
		shift = 0;
	  }
	  app_flags.DISPLAY_UPDATE_REQ = 1;
      break;

    case 6: // Pause/Stop Button
      if( pin_value == 0 ) {
      	if( shift == 1 ) {
      		MCLOCK_DoStop();
		}
		else {
      		MCLOCK_DoPause();
		}
  	  }
      break;

    case 5: // Play Button
      if( pin_value == 0 )
	  	MCLOCK_DoPlay();
      break;

    case 4: // Rew Button
      if( pin_value == 0 ) {
	    MCLOCK_DoRew();
	  }
      break;

    case 3: // Fwd Button
      if( pin_value == 0 ) {
	    MCLOCK_DoFwd();
      }
      break;

#else // No menu-buttons
   case 7: // Stop Button
      if( pin_value == 0 ) {
		MCLOCK_DoStop();
  	  }
      break;

    case 6: // Pause Button now used to cycle through the encoder modes
      if( pin_value == 0 ) {
		//cycle encoder mode
		if (++enc_mode >= 9) {
		enc_mode = 0;
	    }
	    app_flags.DISPLAY_UPDATE_REQ = 1;
  	  }
      break;

    case 5: // Play Button now used as play/pause button
      if( pin_value == 0 ) {
	    MCLOCK_DoPlayPause();
  	  }
      break;

    case 4: // Rew Button
      if( pin_value == 0 ) {
		if ( enc_mode == 7 ) {
		  enc_mode = 0;
		  m_page = 1;
		  app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else if ( enc_mode == 8 ) {
		  enc_mode = 0;
		  m_page = 1;
		  app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else
		{
		  MCLOCK_DoRew();
  		}
	  }
      break;

    case 3: // Fwd Button
      if( pin_value == 0 ) {
		if ( enc_mode == 7 ) {
		  PatchWrite(save_patch);
		  enc_mode = 0;
		  m_page = 1;
		  app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else if ( enc_mode == 8 ) {
		  PatchRead(save_patch);
		  enc_mode = 0;
		  m_page = 1;
		  app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else
		{
	      MCLOCK_DoFwd();
		}
      }
      break;
#endif
    case 8:
      if( pin_value == 0 ) {
	    MCLOCK_DoMultiStop(0);
      }
      break;

    case 9:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 0;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
		  MCLOCK_DoMultiPlay(0);
		}
  	  }
      break;

    case 10:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(1);
      }
      break;

    case 11:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 1;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(1);
  		}
      }
      break;

    case 12:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(2);
      }
      break;

    case 13:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 2;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(2);
      	}
      }
      break;

    case 14:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(3);
      }
      break;

    case 15:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 3;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(3);
      	}
      }
      break;

    case 16:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(4);
      }
      break;

    case 17:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 4;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(4);
  		}
      }
      break;

    case 18:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(5);
      }
      break;

    case 19:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 5;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(5);
      	}
      }
      break;

    case 20:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(6);
      }
      break;

    case 21:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 6;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(6);
      	}
      }
      break;

    case 22:
      if( pin_value == 0 ) {
	  	MCLOCK_DoMultiStop(7);
      }
      break;

    case 23:
      if( pin_value == 0 ) {
		if( shift == 1 ) {
		  current_trk = 7;
	      app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else {
	  	  MCLOCK_DoMultiPlay(7);
      	}
      }
      break;
#if MENU_BUTTONS
    case 27:		//menu-button 1
      if( pin_value == 0 ) {
		m_button = 1;
		//enc_mode = 0;
		app_flags.DISPLAY_UPDATE_REQ = 1;
  	  }
	  break;
    case 26:		//menu-button 2
      if( pin_value == 0 ) {
		m_button = 2;
		//enc_mode = 1;
		app_flags.DISPLAY_UPDATE_REQ = 1;
  	  }
	  break;
	case 25:		//menu-button 3
      if( pin_value == 0 ) {
		m_button = 3;
		//enc_mode = 3;
		app_flags.DISPLAY_UPDATE_REQ = 1;
  	  }
	  break;
	case 24:		//menu-button 4
      if( pin_value == 0 ) {
		m_button = 4;
		//enc_mode = 2;
		app_flags.DISPLAY_UPDATE_REQ = 1;
  	  }
	  break;
#endif

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
  if( encoder == 0 ) {
	if( shift == 0 ) {
		switch (enc_mode) {
		  case 0:	//edit bpm
			  value = (unsigned int)MCLOCK_BPMGet() - 48;
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 255 - 48) ) {
				MCLOCK_BPMSet((unsigned char)value + 48);
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
			  break;
		  case 1:	//select track
			  value = (unsigned int)current_trk;
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 7) ) {
				current_trk = (unsigned char)value;
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
			  break;
		  case 2:	//edit divisor
			  value = (unsigned int)t_divisor[current_trk];
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 5) ) {
				t_divisor[current_trk] = (unsigned char)value;
				if( !mclock_state.RUN ) {
					divisor[current_trk] = (unsigned char)value;
				}
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
			  break;
		  case 3:	//edit shuffle
			  value = (unsigned int)t_shuffle[current_trk];
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 7) ) {
				t_shuffle[current_trk] = (unsigned char)value;
				if( !mclock_state.RUN ) {
				  shuffle[current_trk] = (unsigned char)value;
				}
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
		  break;
		  case 4:	//edit start/continue-settings
			  value = BitTest(out_config_sc_mask,current_trk);
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 1) ) {
				if ( value == 1 ) {
				  out_config_sc_mask |= (1 << current_trk);
				}
				else {
				  out_config_sc_mask &= (~1 << current_trk);
				}
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
		  break;
		  case 5:	//edit start on beat/bar settings
			  value = BitTest(out_config_bb_mask,current_trk);
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 1) ) {
				if ( value == 1 ) {
				  out_config_bb_mask |= (1 << current_trk);
				}
				else {
				  out_config_bb_mask &= (~1 << current_trk);
				}
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
		  break;
		  case 6:	//edit song-position settings
			  value = BitTest(out_config_sp_mask,current_trk);
			  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 1) ) {
				if ( value == 1 ) {
				  out_config_sp_mask |= (1 << current_trk);
				}
				else {
				  out_config_sp_mask &= (~1 << current_trk);
				}
				app_flags.DISPLAY_UPDATE_REQ = 1;
			  }
		  break;
		  case 7:	//edit save_patch
		  case 8:
			  value = (unsigned int)save_patch;
			  if ( MIOS_BOX_STAT.BS_AVAILABLE ) {
			    if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 131) ) {
				  save_patch = (unsigned char)value;
				  app_flags.DISPLAY_UPDATE_REQ = 1;
			    }
		  	  }
			  else {
			    if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 3) ) {
				  save_patch = (unsigned char)value;
				  app_flags.DISPLAY_UPDATE_REQ = 1;
			    }
		  	  }
			  break;
		  }
	}
	else { // swap menu-pages
	  value = (unsigned int)m_page;
	  if( MIOS_HLP_16bitAddSaturate(incrementer, &value, 2) ) {
		m_page = (unsigned char)value;
		app_flags.DISPLAY_UPDATE_REQ = 1;
	  }
	}
  }
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
}

// Checks if a given bit is set in a value, returns 1 if bit is set
int BitTest(unsigned char val, unsigned char bit) {
   unsigned char test_val = 0x01;
   test_val = (test_val << bit);
   if ((val & test_val) == 0)
      return 0;
   else
      return 1;
}

void PatchWrite(unsigned char patch) {
	unsigned int base_adr = 0;

	unsigned char i;

	unsigned char target;



	//eeprom

	if ( patch < 4 ) {
	  base_adr = Mul64(patch);
	  target = 0;
	}
	//bankstick

	else {
	  base_adr = Mul64(patch-4);

	  base_adr += 2;
	  target = 1;
	}

	//write marker
	DataWrite(target, base_adr, 0xcb);


	//write divisor
	for (i = 0; i < 8; i++) {
		divisor[i] = t_divisor[i];
		DataWrite(target, (base_adr+1+i), divisor[i]);
	}


	//write shuffle-settings
	for (i = 0; i < 8; i++) {
		shuffle[i] = t_shuffle[i];
		DataWrite(target, (base_adr+9+i), shuffle[i]);
	}


	DataWrite(target, (base_adr+17), multi_clk_mask);     //Channel-Mask
	DataWrite(target, (base_adr+18), MCLOCK_BPMGet());    //BPM
	DataWrite(target, (base_adr+19), out_config_sc_mask); //Start/Continue-Settings
	DataWrite(target, (base_adr+20), out_config_bb_mask); //Start at next Beat/Bar Settings
	DataWrite(target, (base_adr+21), out_config_sp_mask); //Song-Position-Settings

  	MIOS_LCD_CursorSet(0x40 + 0);
  	MIOS_LCD_PrintCString("Settings saved! ");
    MIOS_LCD_MessageStart(255);
}

void PatchRead(unsigned char patch) {
	unsigned int base_adr = 0;
	unsigned char i;
	unsigned char target;


	//eeprom
	if ( patch < 4 ) {
	  base_adr = Mul64(patch);

	  target = 0;
	}
	//bankstick
	else {
	  base_adr = Mul64(patch);
	  base_adr += 2;

	  target = 1;
	}


	MIOS_LCD_CursorSet(0x40 + 0);

	//check marker
	if ( DataRead(target, base_adr) == 0xcb ) {
	  //read divisor
	  for (i = 0; i < 8; i++) {
		divisor[i] = DataRead(target, base_adr+1+i);
	    t_divisor[i] = divisor[i];
	  }
	  //read shuffle-settings
	  for (i = 0; i < 8; i++) {
		shuffle[i] = DataRead(target, base_adr+9+i);
	    t_shuffle[i] = shuffle[i];
	  }


	  multi_clk_mask = DataRead(target, base_adr+17);     //Channel-Mask
	  MCLOCK_BPMSet(DataRead(target, base_adr+18));       //BPM
	  out_config_sc_mask = DataRead(target, base_adr+19); //Start/Continue-Settings
	  out_config_bb_mask = DataRead(target, base_adr+20); //Start at next Beat/Bar Settings
	  out_config_sp_mask = DataRead(target, base_adr+21); //Song-Position-Settings

	  MIOS_LCD_PrintCString("Settings loaded!");
  }
  else {
	  MIOS_LCD_PrintCString("No valid Data!  ");
  }

  MIOS_LCD_MessageStart(255);
}

void DataWrite(unsigned char target, unsigned int adr, unsigned char mydata) {
	//bankstick

	if ( target == 1 ) {
	  MIOS_BANKSTICK_Write(adr, mydata);
	}
	//eeprom

	else {
	  MIOS_EEPROM_Write(adr, mydata);
	}
}

unsigned char DataRead(unsigned char target, unsigned int adr) {
	//bankstick
	if ( target == 1 ) {
	  return MIOS_BANKSTICK_Read(adr);
	}
	//eeprom
	else {
	  return MIOS_EEPROM_Read(adr);
	}
}

unsigned int Mul64(unsigned int c) {
  	unsigned int res = 0;
  	unsigned char i;

  	for ( i = 0; i < c; i++ ) {
	  res += 64;
  	}

  	return res;
}
