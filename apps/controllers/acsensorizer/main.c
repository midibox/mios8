/*
 * ACSensorizer
 *
 * ==========================================================================
 *
 *  Copyright (C) 2006 Michael Markert, http://www.audiocommander.de
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */


#include <cmios.h>
#include <pic18fregs.h>

#include "ACMidi.h"
#include "ACSensorizer.h"

#if SENSORIZER_INTERFACE_SYNC
	#include "ACSyncronizer.h"
#endif

#if SENSORIZER_INTERFACE_HARM
	#include "ACHarmonizer.h"
#endif


// ***** GLOBALS *****

acapp_t			acapp;
acmenu_t		acmenu;

unsigned char	lastAIN_pin;




// ***** LOCALS *****

#if SENSORIZER_INTERFACE_LCD
static const unsigned char levelchars[8*8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
};
#endif /* SENSORIZER_INTERFACE_LCD */


#if SENSORIZER_INTERFACE_HUI
unsigned char	lastDIN_pin;
unsigned char	lastDIN_value;
unsigned char	lcd_patchname_index;	// highlighted char
#endif /* SENSORIZER_INTERFACE_HUI */


#if SENSORIZER_INTERFACE_NRPN
unsigned char	lastNRPN_MSB = 0xFF;	// initiate undefined, selectedSensor
unsigned char	lastNRPN_LSB = 0xFF;	// initiate undefined, lastControlType
unsigned char	lastData_MSB = 0;		// MSB for 10-bit data
unsigned char	lastData_LSB = 0;		// LSB for 10-bit data, MSB for 7-bit
#endif /* SENSORIZER_INTERFACE_NRPN */



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the 
// application
/////////////////////////////////////////////////////////////////////////////

void Init(void) __wparam 
{
	// acapp config
	acapp.displayNeedsInit = TRUE;
	acmenu.selectedSensor = 0;
	acmenu.selectedMenu = LCD_MENU_PATCH;
	
	// configure MIDI-Merger
	MIOS_MIDI_MergerSet(MIDI_MERGER_MODE);
	
#if SENSORIZER_INTERFACE_LCD
	// LCD config: install special chars
	MIOS_CLCD_SpecialCharsInit(levelchars);
#endif /* SENSORIZER_INTERFACE_LCD */
	
#if SENSORIZER_INTERFACE_HUI
	// DIN/DOUT config
	MIOS_SRIO_NumberSet(SRIO_NUM);
	MIOS_SRIO_DebounceSet(SRIO_DEBOUNCE);
	MIOS_SRIO_UpdateFrqSet(SRIO_UPDATE_FRQ);
	
	// ENC config
#if ENC_TYPES == 1
	MIOS_ENC_SpeedSet(ENC_SENSOR_SELECT, MIOS_ENC_SPEED_SLOW, 0);
	MIOS_ENC_SpeedSet(ENC_MENU_SELECT,   MIOS_ENC_SPEED_SLOW, 0);
	MIOS_ENC_SpeedSet(ENC_PARAM_A,		 MIOS_ENC_SPEED_FAST, 2);
	MIOS_ENC_SpeedSet(ENC_PARAM_B,		 MIOS_ENC_SPEED_FAST, 2);
#elif ENC_TYPES == 2
	MIOS_ENC_SpeedSet(ENC_SENSOR_SELECT, MIOS_ENC_SPEED_SLOW, 1);
	MIOS_ENC_SpeedSet(ENC_MENU_SELECT,   MIOS_ENC_SPEED_SLOW, 1);
	MIOS_ENC_SpeedSet(ENC_PARAM_A,		 MIOS_ENC_SPEED_SLOW, 1);
	MIOS_ENC_SpeedSet(ENC_PARAM_B,		 MIOS_ENC_SPEED_SLOW, 1);
#endif /* ENC_TYPES*/
	
	// patchname
	lcd_patchname_index = 0;
#endif /* SENSORIZER_INTERFACE_HUI */
	
	// Init Objects
#if SENSORIZER_INTERFACE_HARM
	ACHarmonizer_Init();							// Init Harmonizer
#endif
	
	// AIN Config will be done by Sensorizer
	ACSensorizer_Init();							// Init Sensorizer
	
#if SENSORIZER_INTERFACE_SYNC
	ACSYNC_Init();									// Init Syncronizer
	ACSYNC_BPMSet(120);								// set Master
#endif
	
	// load the last used patch
	bank = MIOS_EEPROM_Read(0x00);
	if(bank > 14) { bank = 0; }
	patch = MIOS_EEPROM_Read(0x01);
	if(patch > 127) { patch = 0; }
	if( ACSensorizer_Patch_Read() ) { AC_DISPLAY_Message(LCD_MSG_PRG_READ_ERROR); }
	
}



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////

void Tick(void) __wparam
{
#if SENSORIZER_INTERFACE_SYNC
	ACSYNC_Tick();
#endif /* SENSORIZER_INTERFACE_SYNC */
}



/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////

void Timer(void) __wparam
{
#if SENSORIZER_INTERFACE_SYNC
	ACSYNC_Timer();
#endif /* SENSORIZER_INTERFACE_SYNC */
}



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be 
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////

void DISPLAY_Init(void) __wparam
{
#if SENSORIZER_INTERFACE_LCD
	unsigned char c;
	
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(0x00);
	
#if SENSORIZER_LCD_TYPE == 216
	// x16 display		  "0123456789abcdef"
	MIOS_LCD_PrintCString("[1]    <   |   >");
	MIOS_LCD_CursorSet(0x40);
	MIOS_LCD_PrintCString("iP!    <   |   >");
#endif /* SENSORIZER_LCD_TYPE == 216 */
	
	// clear initFlag
	acapp.displayNeedsInit = FALSE;
	// update infos
	acapp.displayNeedsUpdate = TRUE;
	acapp.displayTickUpdate = TRUE;
#endif /* SENSORIZER_INTERFACE_LCD */
}



/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////

void DISPLAY_Tick(void) __wparam
{
#if SENSORIZER_INTERFACE_LCD
	unsigned char c;
	unsigned char selectedSensor = (unsigned char)acmenu.selectedSensor;
	
	if(acapp.displayNeedsInit) {
		DISPLAY_Init();
	}
	if(acapp.displayTickUpdate) {
		MIOS_LCD_CursorSet(0x00);
#if SENSORIZER_LCD_TYPE == 216
#if SENSORIZER_INTERFACE_SYNC
		// indicate the time (mclock_ctr_beats counts measures 0 to 3)
		switch(mclock_ctr_beats) {
			case 1: case 3:
				MIOS_LCD_PrintCString("   ");
				break;
			default:
				MIOS_LCD_PrintCString("[ ]");
				break;
		}
#endif /* SENSORIZER_INTERFACE_SYNC */
		// show selected sensor
		MIOS_LCD_CursorSet(0x01);
		MIOS_LCD_PrintBCD1(selectedSensor + 1);
		// show sensor ON/OFF state
		for(c=0;c<SENSOR_NUM;c++) {
			if((AIN_NUM < 4) || (c > 3)) {
				MIOS_LCD_CursorSet(0x43 + (c - 4));
			} else {
				MIOS_LCD_CursorSet(0x03 + c);
			}
			if(! sensor[c].enabled) { MIOS_LCD_PrintChar('x'); }
		}
		// show last level
		if((AIN_NUM < 4) || (lastAIN_pin > 3)) {
			MIOS_LCD_CursorSet(0x43 + (lastAIN_pin - 4));
		} else {
			MIOS_LCD_CursorSet(0x03 + lastAIN_pin);
		}
		if(sensor[lastAIN_pin].enabled) {
			MIOS_LCD_PrintChar((lastAIN_value[lastAIN_pin] >> 4) & 0x7F);
		} else {
			MIOS_LCD_PrintChar('x');
		}
	}
	if(acapp.displayNeedsUpdate) {
		// show sensor props
		MIOS_LCD_CursorSet(0x40);
		// invert
		if(sensor[selectedSensor].invert) { 
			MIOS_LCD_PrintChar('i');
		} else {
			MIOS_LCD_PrintChar(' ');
		}
		// pedal
		switch(sensor[selectedSensor].pedalMode) {
			case PEDAL_FILTER:
				MIOS_LCD_PrintChar('P');
				MIOS_LCD_PrintChar(' ');
				break;
			case PEDAL_PANIC:
				MIOS_LCD_PrintChar(' ');
				MIOS_LCD_PrintChar('!');
				break;
			case PEDAL_FILTER_PANIC:
				MIOS_LCD_PrintChar('P');
				MIOS_LCD_PrintChar('!');
				break;
			default:
				MIOS_LCD_PrintChar(' ');
				MIOS_LCD_PrintChar(' ');
				break;
		}
		// show selected menu
		MIOS_LCD_CursorSet(0x08);
		switch(acmenu.selectedMenu) {
			case LCD_MENU_BNK_PRG:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_BNK_PRG);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintChar(' ');
				MIOS_LCD_PrintBCD2(bank);
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintBCD3(patch);
				break;
			case LCD_MENU_PATCH:
				MIOS_LCD_PrintCString(LCD_MENU_STR_PATCH);
				MIOS_LCD_CursorSet(0x08 + lcd_patchname_index);
				MIOS_LCD_PrintChar(0x5f);	// indicate current cursor pos
				MIOS_LCD_CursorSet(0x48);
				for(c=0;c<(LCD_PATCHNAME_CHAR_NUM-1);c++) {
					MIOS_LCD_PrintChar(patchName[c]);
				}
				break;
			case LCD_MENU_ENA_VAL:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_ENA_VAL);
				MIOS_LCD_CursorSet(0x48);
				if(sensor[selectedSensor].enabled) {
					MIOS_LCD_PrintCString("ON ");
				} else {
					MIOS_LCD_PrintCString("OFF");
				}
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintBCD3(lastAIN_value[selectedSensor]);
				break;
			case LCD_MENU_CH_CC:
				MIOS_LCD_PrintCString(LCD_MENU_STR_CH_CC);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintChar(' ');
				MIOS_LCD_PrintBCD2(sCH[selectedSensor] + 1);
				MIOS_LCD_CursorSet(0x4c);
#if SENSORIZER_INTERFACE_HARM
				if(sCC[selectedSensor]) {
					MIOS_LCD_PrintBCD3(sCC[selectedSensor]);
				} else {
					MIOS_LCD_PrintCString("NTE");
				}
#else
				MIOS_LCD_PrintBCD3(sCC[selectedSensor]);
#endif /* SENSORIZER_INTERFACE_HARM */
				break;
#if SENSORIZER_INTERFACE_SYNC
			case LCD_MENU_Q_BPM:
				MIOS_LCD_PrintCString(LCD_MENU_STR_Q_BPM);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintBCD2(sync_q_values[(sync_q[selectedSensor])]);
				MIOS_LCD_PrintChar('.');
				MIOS_LCD_CursorSet(0x4c);
				if(mclock_state.IS_MASTER && ACSYNC_BPMGet()) {
					MIOS_LCD_PrintBCD3(ACSYNC_BPMGet());
				} else {
					MIOS_LCD_PrintCString("SLV");
				}
				break;
#endif /* SENSORIZER_INTERFACE_SYNC */
			case LCD_MENU_BAS_SCL:
				MIOS_LCD_PrintCString(LCD_MENU_STR_BAS_SCL);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintChar(noteNames[0][harmonizer_base]);
				MIOS_LCD_PrintChar(noteNames[1][harmonizer_base]);
				MIOS_LCD_PrintChar(' ');
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintCString(scaleNames[harmonizer_scale]);
				break;
			case LCD_MENU_PED_RLD:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_PED_RLD);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintCString("0x");
				MIOS_LCD_PrintBCD1(sensor[selectedSensor].pedalMode);
				MIOS_LCD_CursorSet(0x4c);
				if(sensor[selectedSensor].releaseDetect) {
					MIOS_LCD_PrintCString("YES");
				} else {
					MIOS_LCD_PrintCString("NO ");
				}
				break;
			case LCD_MENU_INV_AUTO:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_INV_AUT);
				MIOS_LCD_CursorSet(0x48);
				if(sensor[selectedSensor].invert) {
					MIOS_LCD_PrintCString("YES");
				} else {
					MIOS_LCD_PrintCString("NO ");
				}
				MIOS_LCD_CursorSet(0x4c);
				switch(sensor[selectedSensor].autoSense) {
					case AUTOSENSE_OFF:	MIOS_LCD_PrintCString("OFF");	break;
					case AUTOSENSE_MIN:	MIOS_LCD_PrintCString("MIN");	break;
					case AUTOSENSE_MAX:	MIOS_LCD_PrintCString("MAX");	break;
				}
				break;
			case LCD_MENU_MIN_MAX:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_MIN_MAX);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintBCD3((sMin[selectedSensor] >> 2));
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintBCD3((sMax[selectedSensor] >> 2));
				break;
			case LCD_MENU_FAC_SLW:
				MIOS_LCD_PrintCString(LCD_MENU_STR_FAC_SLW);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintBCD3(sFactor[selectedSensor]);
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintBCD3(slowdown[selectedSensor]);
				break;
			case LCD_MENU_FRM_TO:	
				MIOS_LCD_PrintCString(LCD_MENU_STR_FRM_TO);
				MIOS_LCD_CursorSet(0x48);
				MIOS_LCD_PrintBCD3(sFrom[selectedSensor]);
				MIOS_LCD_CursorSet(0x4c);
				MIOS_LCD_PrintBCD3(sTo[selectedSensor]);
				break;
		}
		
#endif /* SENSORIZER_LCD_TYPE == 216 */
	}	
	// clear update flags
	acapp.displayTickUpdate = FALSE;
	acapp.displayNeedsUpdate = FALSE;
#endif /* SENSORIZER_INTERFACE_LCD */
}



/////////////////////////////////////////////////////////////////////////////
//	This function shows a temporary message depending on the given inputCode
/////////////////////////////////////////////////////////////////////////////

void AC_DISPLAY_Message(unsigned char msgCode) __wparam {
	unsigned char c;
	// stop eventually pending messages
	//MIOS_LCD_MessageStop();
#if SENSORIZER_INTERFACE_LCD
	unsigned char selectedSensor = (unsigned char)acmenu.selectedSensor;
	MIOS_LCD_Clear();
	MIOS_LCD_CursorSet(LCD_CENTER_ROW_ONE);
#if SENSORIZER_LCD_TYPE == 216
	switch(msgCode) {
		//						  "0123456789abcdef"
		case LCD_MSG_PANIC:	
			MIOS_LCD_PrintCString("    PANIC!!!    ");		
			break;
		case LCD_MSG_PEDAL_SENSE:
			MIOS_LCD_PrintCString("  Auto-Sensing  ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			MIOS_LCD_PrintCString(" Press PEDAL... ");
			break;
		case LCD_MSG_AUTOSENSE:
			MIOS_LCD_PrintCString("  Auto-Sensing  ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			MIOS_LCD_PrintCString("Min:    Max:    ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0x4);
			MIOS_LCD_PrintBCD3((sMin[selectedSensor] >> 2));
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0xc);
			MIOS_LCD_PrintBCD3((sMax[selectedSensor] >> 2));
			break;
		case LCD_MSG_PRG_READ:
			//                    "0123456789abcdef"
			//                    " Loading #A.127 "
			MIOS_LCD_PrintCString(" Loading #      ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_ONE + 0xa);
			MIOS_LCD_PrintChar(0x41 + bank);	// eg "A" for "0"
			MIOS_LCD_PrintChar('.');
			MIOS_LCD_PrintBCD3(patch);
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			//                    "0123456789abcdef"
			//                    " Patch: 1234567 "
			MIOS_LCD_PrintCString(" Patch:         ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0x8);
			for(c=0;c<(LCD_PATCHNAME_CHAR_NUM-1);c++) {
				MIOS_LCD_PrintChar(patchName[c]);
			}
			break;
		case LCD_MSG_PRG_WRITE:
			//                    "0123456789abcdef"
			//                    " Writing #A.127 "
			MIOS_LCD_PrintCString(" Writing #      ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_ONE + 0xa);
			MIOS_LCD_PrintChar(0x41 + bank);	// eg "A" for "0"
			MIOS_LCD_PrintChar('.');
			MIOS_LCD_PrintBCD3(patch);
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			//                    "0123456789abcdef"
			//                    " Patch: 1234567 "
			MIOS_LCD_PrintCString(" Patch:         ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0x8);
			for(c=0;c<(LCD_PATCHNAME_CHAR_NUM-1);c++) {
				MIOS_LCD_PrintChar(patchName[c]);
			}
				break;
		case LCD_MSG_PRG_READ_ERROR:
			MIOS_LCD_PrintCString(" PRG Read Error ");
			break;
		case LCD_MSG_PRG_WRITE_ERROR:
			MIOS_LCD_PrintCString(" PRG Write Error");
			break;
	}
#elif SENSORIZER_LCD_TYPE == 420
	switch(msgCode) {
		//						  "0123456789abcdef0123"
		case LCD_MSG_PANIC:
			MIOS_LCD_PrintCString("      PANIC!!!      ");
			break;
		case LCD_MSG_PEDAL_SENSE:
			MIOS_LCD_PrintCString("    Auto-Sensing    ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			MIOS_LCD_PrintCString("   Press PEDAL...   ");
			break;
		case LCD_MSG_PRG_READ:
			//                    "0123456789abcdef0123"
			//                    "   Loading #A.127   "
			MIOS_LCD_PrintCString("   Loading #        ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_ONE + 0xc);
			MIOS_LCD_PrintChar(0x41 + bank);	// eg "A" for "0"
			MIOS_LCD_PrintChar('.');
			MIOS_LCD_PrintBCD3(patch);
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			//                    "0123456789abcdef0123"
			//                    "   Patch: 1234567   "
			MIOS_LCD_PrintCString("   Patch:           ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0xa);
			for(c=0;c<(LCD_PATCHNAME_CHAR_NUM-1);c++) {
				MIOS_LCD_PrintChar(patchName[c]);
			}
			break;
		case LCD_MSG_PRG_WRITE:
			//                    "0123456789abcdef0123"
			//                    "   Writing #A.127   "
			MIOS_LCD_PrintCString("   Writing #        ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_ONE + 0xc);
			MIOS_LCD_PrintChar(0x41 + bank);	// eg "A" for "0"
			MIOS_LCD_PrintChar('.');
			MIOS_LCD_PrintBCD3(patch);
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO);
			//                    "0123456789abcdef0123"
			//                    "   Patch: 1234567   "
			MIOS_LCD_PrintCString("   Patch:           ");
			MIOS_LCD_CursorSet(LCD_CENTER_ROW_TWO + 0xa);
			for(c=0;c<(LCD_PATCHNAME_CHAR_NUM-1);c++) {
				MIOS_LCD_PrintChar(patchName[c]);
			}
			break;
		case LCD_MSG_PRG_READ_ERROR:
			MIOS_LCD_PrintCString("   PRG Read Error   ");
			break;
		case LCD_MSG_PRG_WRITE_ERROR:
			MIOS_LCD_PrintCString("   PRG Write Error  ");
			break;
	}	
#endif /* SENSORIZER_LCD_TYPE == xxx */
	MIOS_LCD_MessageStart(LCD_MESSAGE_DURATION);
#endif /* SENSORIZER_INTERFACE_LCD */
}



/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////

void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{
	unsigned char c;
	
#if SENSORIZER_INTERFACE_NRPN
	unsigned int lcdErrorCode = 0;
	unsigned int lastData = 0;	// for 14bit conversion
#endif /* SENSORIZER_INTERFACE_NRPN */
	
	// Process Events
	switch(evnt0 & 0xF0) {
		
		case MIDI_NOTE_ON:					// ***** NOTE-ON *****
		case MIDI_NOTE_OFF:					// ***** NOTE-OFF *****
#if SENSORIZER_INTERFACE_HARM
			if(harmonizer_listen) {
				// set base
				if(evnt2) { ACHarmonizer_SetBase(evnt1); }
			} else {
				// forward harmonized NOTES if MIDI_HARMONIZE_NOTES is true
				ACMidi_SendEvent(evnt0, (ACHarmonize(evnt1)), evnt2);
			}
#endif /* SENSORIZER_INTERFACE_HARM */
			break;
			
		case MIDI_CC:						// ***** CONTROL CHANGE *****
			switch(evnt1) {
				
				case MIDI_CC_PEDAL: 		// Damper Pedal (CC#64: Sustain)
				case MIDI_CC_SOFT_PEDAL:
				case MIDI_CC_SUSTENUTO:
					sensorizer.pedalUp = (evnt2 >> 6) ^ 0x1;
					break;
					
#if SENSORIZER_INTERFACE_BS
				case MIDI_CC_BANK_SELECT:
					if( (evnt0 & 0x0F) == MIDI_GLOBAL_CH ) { 
						// bank select (coarse only for banks 0..14)
						bank = evnt2;
					}
					break;
#endif /* SENSORIZER_INTERFACE_BS */
					
#if SENSORIZER_INTERFACE_HARM
				case MIDI_CC_HARMONY_BASE:
					ACHarmonizer_SetBase(evnt2);
					break;
				case MIDI_CC_HARMONY_BASE_LISTEN:
					if(evnt2 > 63) {
						harmonizer_listen = TRUE;
					} else {
						harmonizer_listen = FALSE;
					}
					break;
				case MIDI_CC_HARMONY_SCALE:
					ACHarmonizer_SetScale(evnt2);
					break;
				case MIDI_CC_HARMONY_SCALE_RELATIVE_PREV:
					if(evnt2 > 63) { ACHarmonizer_ToggleScale(0); }
					break;
				case MIDI_CC_HARMONY_SCALE_RELATIVE_NEXT:
					if(evnt2 > 63) { ACHarmonizer_ToggleScale(1); }
					break;
				case MIDI_CC_HARMONY_RANDOM:
					if(evnt2 > 63) { ACHarmonizer_Random(); }
					break;
#endif /* SENSORIZER_INTERFACE_HARM */
					
#if SENSORIZER_INTERFACE_SYNC
				case QUANTIZE_BPM:		
					if(mclock_state.IS_MASTER) { ACSYNC_BPMSet((evnt2 + 60)); }
					break;
//				case QUANTIZE_TOGGLE_NEXT:
//							MIOS_HLP_16bitAddSaturate(incrementer, &evnt1, SYNC_Q_MAX);
				case QUANTIZE_SET:
					if(evnt2 > SYNC_Q_MAX) { break; }
					for(c=0;c<SENSOR_NUM;c++) {
						if( sCH[c] == (evnt0 & 0x0F) ) {
							sync_q[c] = evnt2;
						}
					}
					break;
#endif /* SENSORIZER_INTERFACE_SYNC */
					
#if SENSORIZER_INTERFACE_NRPN
				case MIDI_CC_NRPN_MSB:	// set last sensor
					lastNRPN_MSB = evnt2;
					break;
				case MIDI_CC_NRPN_LSB:	// set last control type
					lastNRPN_LSB = evnt2;
					switch(lastNRPN_MSB) {
						case HUIREMOTE_SENSORWHEEL: 	ENC_NotifyChange(ENC_SENSOR_SELECT,((evnt2>64)?1:-1));	break;
						case HUIREMOTE_MENUWHEEL:		ENC_NotifyChange(ENC_MENU_SELECT,((evnt2>64)?1:-1));	break;
						case HUIREMOTE_DATAWHEEL_LEFT:	ENC_NotifyChange(ENC_PARAM_A,((evnt2>64)?1:-1));		break;
						case HUIREMOTE_DATAWHEEL_RIGHT:	ENC_NotifyChange(ENC_PARAM_B,((evnt2>64)?1:-1));		break;
					}
					break;
				case MIDI_CC_DATA_ENTRY_MSB:
					lastData_MSB = evnt2;
					break;
				case MIDI_CC_DATA_ENTRY_LSB:
					lastData_LSB = evnt2;
					if((lastNRPN_MSB == 0xFF) || (lastNRPN_MSB >= AIN_NUM)) { 
						return;			// last sensor has never been set OR illegal sensorNum
					}
					switch(lastNRPN_LSB) {
						case NRPN_CTRL_ENABLED:
							sensor[lastNRPN_MSB].enabled = (lastData_LSB > 0);
#if SENSORIZER_INTERFACE_DOUT
							MIOS_DOUT_PinSet(lastNRPN_MSB, (lastData_LSB > 0));
#endif /* SENSORIZER_INTERFACE_DOUT */
							break;
						case NRPN_CTRL_PEDAL:		
							if(lastData_LSB < 0x8) {	
								sensor[lastNRPN_MSB].pedalMode = lastData_LSB; 
							}
							break;
						case NRPN_CTRL_AUTOSENSE:
							if(lastData_LSB < 0x3) { 
								sensor[lastNRPN_MSB].autoSense = lastData_LSB; 
							}
							break;
						case NRPN_CTRL_INVERT:
							sensor[lastNRPN_MSB].invert = (lastData_LSB > 0);
							break;
						case NRPN_CTRL_RELEASE_DETECT:	
							sensor[lastNRPN_MSB].releaseDetect = (lastData_LSB > 0);
							break;
						case NRPN_CTRL_SLOWDOWN:
							slowdown[lastNRPN_MSB] = lastData_LSB;
							break;
						case NRPN_CTRL_SMIN:
							lastData = (unsigned int)(lastData_MSB << 8) + lastData_LSB;
							sMin[lastNRPN_MSB] = lastData;
							break;
						case NRPN_CTRL_SMAX:
							lastData = (unsigned int)(lastData_MSB << 8) + lastData_LSB;
							sMax[lastNRPN_MSB] = lastData;
							break;
						case NRPN_CTRL_SFACTOR:
							sFactor[lastNRPN_MSB] = lastData_LSB;
							break;
						case NRPN_CTRL_SFROM:
							sFrom[lastNRPN_MSB] = lastData_LSB;
							break;
						case NRPN_CTRL_STO:
							sTo[lastNRPN_MSB] = lastData_LSB;
							break;
						case NRPN_CTRL_SCH:
							if(lastData_LSB < 16) {
								sCH[lastNRPN_MSB] = lastData_LSB; 
							}
							break;
						case NRPN_CTRL_SCC:
							sCC[lastNRPN_MSB] = lastData_LSB;
							break;
					}
					break;
#endif /* SENSORIZER_INTERFACE_NRPN */
			
			}
			break;
			
		case MIDI_PRG:						// ***** PROGRAM CHANGE *****
#if SENSORIZER_INTERFACE_BS
			if( (evnt0 & 0x0F) == MIDI_GLOBAL_CH ) { 
				// select new patch
				patch = evnt1;
				c = (ACSensorizer_Patch_Read() << 4) + 0x5;		// 0x05 = LCD_MSG_READ
																// 0x15 = LCD_MSG_READ_ERROR
				AC_DISPLAY_Message(c);
				// do not broadcast PRG-CH here (broadcast on DIN Load/Store press only!)
			}
#endif /* SENSORIZER_INTERFACE_BS */
			break;
			
	}

#if SENSORIZER_INTERFACE_HUI
	acapp.displayNeedsUpdate = TRUE;
#endif /* SENSORIZER_INTERFACE_HUI */
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
#if SENSORIZER_INTERFACE_SYNC
	// forward byte to Syncronizer
	ACSYNC_ReceiveClock(byte);
#endif
}



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS before the shift register are loaded
/////////////////////////////////////////////////////////////////////////////

void SR_Service_Prepare(void) __wparam
{
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
#if SENSORIZER_INTERFACE_HUI
	unsigned char c;
	unsigned char selectedSensor = acmenu.selectedSensor;
	// set sensorizer values
	switch(pin) {
		case DIN_PANIC:
			if(pin_value == 0) {
				ACMidi_SendPanic(16);	// 16 sends Panic on all Channels
				ACSYNC_DoStop();		// send STOP signal
				AC_DISPLAY_Message(LCD_MSG_PANIC);
			}
			break;
		case DIN_PEDAL:
		case DIN_PEDAL_SECOND:
			sensorizer.pedalUp = pin_value;
			if(pin_value) {		// on release
				for(c=0;c<SENSOR_NUM;c++) {
					if((sensor[c].pedalMode == PEDAL_PANIC) || (sensor[c].pedalMode == PEDAL_FILTER_PANIC)) {
						if(sensor[c].enabled) {	ACMidi_SendPanic(sCH[c]); }	// send PANIC
					}
				}
			}
			// send MIDI_Pedal
			ACMidi_SendCC(MIDI_GLOBAL_CH, SENSORIZER_PEDAL_CC, ((pin_value ^ 0x1) << 6) );
			break;
#if SENSORIZER_INTERFACE_BS
		case DIN_LOAD:
			if(pin_value == 0) {
				c = (ACSensorizer_Patch_Read() << 4) + 0x5;		// 0x05 = LCD_MSG_READ
																// 0x15 = LCD_MSG_READ_ERROR
				AC_DISPLAY_Message(c);
#if BROADCAST_PRG_CH
				// broadcast PRG-CH
				ACMidi_SendPRG_CH(bank,patch);
#endif /* BROADCAST_PRG_CH */
			}
			break;
		case DIN_STORE:
			if(pin_value == 0) {
				c = (ACSensorizer_Patch_Write() << 4) + 0x6;	// 0x06 = LCD_MSG_WRITE
																// 0x16 = LCD_MSG_WRITE_ERROR
				AC_DISPLAY_Message(c);
#if BROADCAST_PRG_CH
				// broadcast PRG-CH
				ACMidi_SendPRG_CH(bank,patch);
#endif /* BROADCAST_PRG_CH */
			}
			break;
#endif /* SENSORIZER_INTERFACE_BS */
#if SENSORIZER_INTERFACE_DIN
		default:
			if((pin >= DIN_EXT_MIN) && (pin <= DIN_EXT_MAX) && (pin_value == 0)) {
#if SENSORIZER_MODEL_ID == SENSORIZER_MODEL_ID_ONE
				// fix sensorizer grey model DIN hardware bug
				if((pin == 24) || (pin == 25)) { return; }
				if(pin == 29) { pin = 24; }
				if(pin == 30) { pin = 25; }
#endif /* SENSORIZER_MODEL_ID == SENSORIZER_MODEL_ID_ONE */
				ACMidi_SendCC(MIDI_GLOBAL_CH, DIN_EXT_CC, (pin - DIN_EXT_MIN));
			}
			break;
#endif /* SENSORIZER_INTERFACE_DIN */
	}
	
	// store last values
	lastDIN_pin = pin;
	lastDIN_value = pin_value;
	// update LCD
	acapp.displayNeedsUpdate = TRUE;
	
#endif /* SENSORIZER_INTERFACE_HUI */
}



/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////

void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
#if SENSORIZER_INTERFACE_HUI
	unsigned int value;
	unsigned char selectedSensor = (unsigned char)acmenu.selectedSensor;
	// stop current message (if any)
	MIOS_LCD_MessageStop();
	switch(encoder) {
		case ENC_SENSOR_SELECT:
			value = (unsigned int)selectedSensor;
			MIOS_HLP_16bitAddSaturate(incrementer, &value, (SENSOR_NUM - 1));
			selectedSensor = (unsigned char)value;
			acmenu.selectedSensor = selectedSensor;
			break;
		case ENC_MENU_SELECT:
			value = (unsigned int)acmenu.selectedMenu;
			MIOS_HLP_16bitAddSaturate(incrementer, &value, (LCD_MENU_ITEMS_NUM - 1));
			if(acmenu.selectedMenu == LCD_MENU_PATCH) {
				// we got to cleanup the screen after this menu
				acapp.displayNeedsInit = TRUE;
			}
			acmenu.selectedMenu = (unsigned char)value;
			break;
		case ENC_PARAM_A:
			switch(acmenu.selectedMenu) {
				case LCD_MENU_BNK_PRG:	
					value = (unsigned int)bank;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 15);
					bank = (unsigned char)value;
					break;
				case LCD_MENU_PATCH:
					value = (unsigned int)lcd_patchname_index;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, (LCD_PATCHNAME_CHAR_NUM - 2));
					lcd_patchname_index = (unsigned char)value;
					break;
				case LCD_MENU_ENA_VAL:
					value = (unsigned int)sensor[selectedSensor].enabled;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 1);
					sensor[selectedSensor].enabled = (unsigned char)value;
#if SENSORIZER_INTERFACE_DOUT
					MIOS_DOUT_PinSet(selectedSensor, (value ? 1:0));
#endif /* SENSORIZER_INTERFACE_DOUT */
					acapp.displayNeedsInit = TRUE;	// refresh whole display; all sensor states
					break;
				case LCD_MENU_CH_CC:
					value = (unsigned int)sCH[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 15);
					sCH[selectedSensor] = (unsigned char)value;
					break;
#if SENSORIZER_INTERFACE_SYNC
				case LCD_MENU_Q_BPM:
					value = (unsigned int)sync_q[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, SYNC_Q_MAX);
					sync_q[selectedSensor] = (unsigned char)value;
					sensorizer.resetMeter = TRUE;	// reset decrementers
					break;
#endif /* SENSORIZER_INTERFACE_SYNC */
				case LCD_MENU_BAS_SCL:
					value = (unsigned int)harmonizer_base;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 11);
					harmonizer_base = (unsigned char)value;
					// broadcast harmony change (ACMidiProtocol)
					ACMidi_SendCC(MIDI_GLOBAL_CH,MIDI_CC_HARMONY_BASE,harmonizer_base);
					break;
				case LCD_MENU_PED_RLD:
					value = (unsigned int)sensor[selectedSensor].pedalMode;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 8);
					sensor[selectedSensor].pedalMode = (unsigned char)value;
					break;
				case LCD_MENU_INV_AUTO:
					value = (unsigned int)sensor[selectedSensor].invert;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 1);
					sensor[selectedSensor].invert = (unsigned char)value;
					break;
				case LCD_MENU_MIN_MAX:
					MIOS_HLP_16bitAddSaturate(incrementer, &sMin[selectedSensor], 1023);
					break;
				case LCD_MENU_FAC_SLW:
					value = (unsigned int)sFactor[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 16);
					sFactor[selectedSensor] = (unsigned char)value;
					if(sFactor[selectedSensor] == 0) { sFactor[selectedSensor] = 1; } // check for valid denominator!
					break;
				case LCD_MENU_FRM_TO:
					value = (unsigned int)sFrom[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					sFrom[selectedSensor] = (unsigned char)value;
					break;
			}
			break;
		case ENC_PARAM_B:
			switch(acmenu.selectedMenu) {
				case LCD_MENU_BNK_PRG:
					value = (unsigned int)patch;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					patch = (unsigned char)value;
					break;
				case LCD_MENU_PATCH:
					value = (unsigned int)patchName[lcd_patchname_index] - LCD_PATCHNAME_CHAR_MIN;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, (LCD_PATCHNAME_CHAR_MAX - LCD_PATCHNAME_CHAR_MIN));
					patchName[lcd_patchname_index] = (unsigned char)value + LCD_PATCHNAME_CHAR_MIN;
					break;
				case LCD_MENU_ENA_VAL:
					value = (unsigned int)lastAIN_value[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					lastAIN_value[selectedSensor] = (unsigned char)value;
					ACMidi_SendCC(sCH[selectedSensor],sCC[selectedSensor],((unsigned char)value));
					break;
				case LCD_MENU_CH_CC:
					value = (unsigned int)sCC[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					sCC[selectedSensor] = (unsigned char)value;
					break;
#if SENSORIZER_INTERFACE_SYNC
				case LCD_MENU_Q_BPM:
					if(mclock_state.IS_MASTER) {
						value = (unsigned int)ACSYNC_BPMGet() - 48;
						MIOS_HLP_16bitAddSaturate(incrementer, &value, (255 - 48));
						value = value + 48;
						ACSYNC_BPMSet((unsigned char)value);
						if((value > 59) || (value < 188)) {
							// broadcast bpm change (ACMidiProtocol)
							ACMidi_SendCC(MIDI_GLOBAL_CH,QUANTIZE_BPM,((unsigned char)(value - 60)));
						}
					}
					break;
#endif /* SENSORIZER_INTERFACE_SYNC */
				case LCD_MENU_BAS_SCL:
					value = (unsigned int)harmonizer_scale;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, (SCALE_MAX - 1));
					harmonizer_scale = (unsigned char)value;
					// broadcast harmony change (ACMidiProtocol)
					ACMidi_SendCC(MIDI_GLOBAL_CH,MIDI_CC_HARMONY_SCALE,harmonizer_scale);
					break;
				case LCD_MENU_PED_RLD:
					value = (unsigned int)sensor[selectedSensor].releaseDetect;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 1);
					sensor[selectedSensor].releaseDetect = (unsigned char)value;
					break;
				case LCD_MENU_INV_AUTO:
					value = (unsigned int)sensor[selectedSensor].autoSense;
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 2);
					sensor[selectedSensor].autoSense = (unsigned char)value;
					if(value) { 
						AC_DISPLAY_Message(LCD_MSG_PEDAL_SENSE); 
					} else {
						MIOS_LCD_MessageStop();
					}
					break;
				case LCD_MENU_MIN_MAX:
					MIOS_HLP_16bitAddSaturate(incrementer, &sMax[selectedSensor], 1023);
					break;
				case LCD_MENU_FAC_SLW:
					value = (unsigned int)slowdown[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					slowdown[selectedSensor] = (unsigned char)value;
					break;
				case LCD_MENU_FRM_TO:
					value = (unsigned int)sTo[selectedSensor];
					MIOS_HLP_16bitAddSaturate(incrementer, &value, 127);
					sTo[selectedSensor] = (unsigned char)value;
					break;
			}
			break;
	}
	acapp.displayNeedsUpdate = TRUE;
#endif /* SENSORIZER_INTERFACE_HUI */
}




/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////

void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
#if SENSORIZER_INTERFACE_SYNC
	// store value for later processing
	currentAIN_value[pin] = pin_value;
	// store last pin (value is calculated quantized by Sensorizer)
	lastAIN_pin = pin;
#else
	
	// AIN Notification only if not in sync mode!
	unsigned char value;
	// sensorize
	value = ACSensorizer_Sensorize(pin, pin_value);
	if(value < 0x80) {	// value contains 7bit_value
#if SENSORIZER_INTERFACE_HARM
		if(sCC[pin]) {
			// send CC
			ACMidi_SendCC(sCH[pin], sCC[pin], value);
		} else {
			// send harmonized Note
			value = ACHarmonize(value);
			ACMidi_SendNote(sCH[pin],value,SENSORIZED_NOTE_VELOCITY);
		}
#else
		// send CC
		ACMidi_SendCC(sCH[pin], sCC[pin], value);
#endif /* SENSORIZER_INTERFACE_HARM */
		// store last pin
		lastAIN_pin = pin;
		// update LCD
		acapp.displayNeedsUpdate = TRUE;
	} else if(value > 0x80) {
		// check return code
		switch(value) {
			case SRETURN_AUTOSENSE:
				// show autosense state
#if SENSORIZER_AUTOMODE_PEDAL
				if(sensorizer.pedalUp) { return; }
#endif /* SENSORIZER_AUTOMODE_PEDAL */
				AC_DISPLAY_Message(LCD_MSG_AUTOSENSE);
				break;
			default:
				return;
				break;
		}
	}
	
#endif /* SENSORIZER_INTERFACE_SYNC */
}

