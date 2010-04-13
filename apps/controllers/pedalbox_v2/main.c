/*
* MIOS Pedal Box / Pedal Board - main.c
 * v2.6rc1 - April 2010
 * ==========================================================================
 *
 *  Copyright (C) 2010  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#include "cmios.h"
#include <pic18fregs.h>

#include "main.h"
#include "pbx_config.h"
#include "pbx_midi_config.h"
//#include "pbx_relay_config.h"
#include "pbx.c"
#if DIGITS_CONNECTED
#include "pbx_digits.c"
#endif
#include "pbx_lcd.c"


/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS after startup to initialize the
// application
/////////////////////////////////////////////////////////////////////////////
void Init(void) __wparam
{

	// initialize the AIN driver
	MIOS_AIN_NumberSet(AIN_NUMBER_INPUTS);  // 8 pots are connected
	MIOS_AIN_UnMuxed();    // no AINX4 modules are used
	MIOS_AIN_DeadbandSet(AIN_DEADBAND); // should be 7 when 7bit resolution is used


	//Enable SRIO
	// set shift register update frequency
	MIOS_SRIO_UpdateFrqSet(1); // ms

	// THERE IS NO NEED TO CHANGE THIS UNLESS YOU'VE BEEN MODIFYING CODE
	MIOS_SRIO_NumberSet(9);

	// debouncing value for DINs
	MIOS_SRIO_DebounceSet(DIN_DEBOUNCE_VALUE);


	//Enable MIDI Merger
#if MIDI_MERGER
	MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_ENABLED);
#else
	MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_DISABLED);
#endif




	//Load some data from bankstick + check they hooked up correctly


	for (t=0; t<8; t++)
	{
		MIOS_BANKSTICK_CtrlSet(t);
		bankstick_channel[t] = MIOS_BANKSTICK_Read(0x0004);
		if ( !MIOS_BOX_STAT.BS_AVAILABLE )
		{
			bankstick_channel[t] = 255; // will be used as error code for no bankstick
		}
		else
		{
			if (MIOS_BANKSTICK_Read(0x0000) == BS_MAGIC_NUMBER_1 && MIOS_BANKSTICK_Read(0x0001) == BS_MAGIC_NUMBER_2)
			{
				test = MIOS_BANKSTICK_Read(0x0003);
				test_version = test << 1;
				test_version = test_version >> 1;
				if (t == 0 && test >> 7 != PATCH)
				{
					error = 220 + t;
				}
				else if (t > 0 && test >> 7 == PATCH)
				{
					error = 230 + t;
				}
				else
				{
					if (test_version != BS_VERSION) error = 210 + t;
				}
			}
			else
			{
				error = 240 + t;
			}
		}
	}




	// Set Variables
	for (t=0; t<8; t++)
	{
		//current_programchange_param1[t] = 0x00;
		current_programchange_param1[t] = 0x00;
		event_entry_current_programchange[t] = 128;
		event_entry_previous_programchange[t] = 128;
	}
	bank = 1;


	MIOS_BANKSTICK_CtrlSet(0);
	for (i=0;i<144;i++)
	{

		button_event_map[i] = MIOS_BANKSTICK_Read(0x6900 + (i << 2));
		button_event_typeAndBankstick[i] = MIOS_BANKSTICK_Read(0x6900 + (i << 2) + 3);
		button_bankpin_on_value[i] = MIOS_BANKSTICK_Read(0x6900 + (i << 2) + 2);

	}

	MIOS_BANKSTICK_CtrlSet(0);
	for (t=0; t<8; t++) AIN_fixed[t] = MIOS_BANKSTICK_Read(0x6ff0 + t);

	program_mode = NONE;
	previous_bankpin = 255;
	count = 0;

	current_cue_list = MIOS_BANKSTICK_Read(0x0005);
	current_cue = 0;

	// Set SR map
	relay_dout_start_pin = RELAY_SHIFT_REGISTER << 3;
	relay_led_dout_start_pin = RELAY_LED_SHIFT_REGISTER << 3;
	digits_start_pin = DOUT_DIGITS_SR << 3;
	
		// Set relays to off state
	for (i=0;i<8;i++) {

		if (extended_MIOS_EEPROM_Read(0x78 + i) == 1) {
			MIOS_DOUT_PinSet(relay_dout_start_pin + i, 1); 
		}
	}
	
	current_pedalswap_patch = 130;
	active_pedalswap = 7;
	
	
	

	


#if PEDALBOARD
#if DIGITS_CONNECTED
	digits_handler(bank); // display current bank
#endif
#if USE_LED_INDICATORS
	app_flags.LED_UPDATE_REQ = 1; //update led indicators
#endif
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS in the mainloop when nothing else is to do
/////////////////////////////////////////////////////////////////////////////
void Tick(void) __wparam
{
//suspend LCD timer
	if(app_flags.PCRX == 1) {
		suspend_lcd_counter++;
		if (suspend_lcd_counter == 1000) {
			resume_LCD();
		}
	}
	
	if(app_flags.LED_UPDATE_REQ == 1) {
		app_flags.LED_UPDATE_REQ = 0;
		
		//need to isolate this call to here
			set_led_indicators();
		
		// need to change ALL functions to REQUEST an update - function takes to too long to process on midi input pedal movements
	}
	
	
	


}

/////////////////////////////////////////////////////////////////////////////
// This function is periodically called by MIOS. The frequency has to be
// initialized with MIOS_Timer_Set
/////////////////////////////////////////////////////////////////////////////
void Timer(void) __wparam
{
	/* 
	if (app_flags.PCRX == 1)
	{
		countms++;
		if (countms == 10)
		{
			app_flags.PCRX = 0;
			MIOS_TIMER_Stop();
		}
	}
	else
	{ 
	*/
		bpmcountms++; //add 1 to bpmcountms
		if (bpmcountms > 3000 )   //3 seconds have passed
		{
			tap_tempo_stop();
		}
	//}

}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when the display content should be
// initialized. Thats the case during startup and after a temporary message
// has been printed on the screen
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Init(void) __wparam
{
	if (app_flags.SPLASH == 0)
	{
		MIOS_LCD_Clear();
		MIOS_LCD_CursorSet(0x00);
#if PEDALBOARD
		MIOS_LCD_PrintCString("Pedal Board     ");
#else
		MIOS_LCD_PrintCString("Pedal Box       ");
#endif
		MIOS_LCD_CursorSet(0x40);
		if (error)
		{
			MIOS_LCD_PrintCString("Error        ");
			MIOS_LCD_PrintBCD3(error);
			MIOS_UserSuspend(); // Stop the MIOS from functioning
		}
		else
		{
			MIOS_LCD_PrintCString("Version 2.6b1   ");
		}

		for (t = 0; t<16; t++) MIOS_LCD_PrintChar(' ');
		app_flags.SPLASH = 1;
	}
	else
	{
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

}

/////////////////////////////////////////////////////////////////////////////
//  This function is called in the mainloop when no temporary message is shown
//  on screen. Print the realtime messages here
/////////////////////////////////////////////////////////////////////////////
void DISPLAY_Tick(void) __wparam
{

	
	

	
	
	// do nothing if no update has been requested
	if ( !app_flags.DISPLAY_UPDATE_REQ )
	return;

	display_handler();

#if DIGITS_CONNECTED
	digits_handler(bank); // update bank display
#endif


//OVERWRITE EVERYTHING WHEN IDLE (DEBUG)
//MIOS_LCD_CursorSet(0x40);
//MIOS_LCD_PrintBCD3(current_programchange_param1[0]);
//MIOS_LCD_PrintBCD3(debug);

}


/////////////////////////////////////////////////////////////////////////////
//  This function is called by MIOS when a complete MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
{

#if ENABLE_MIDI_IN

	evnt0_MSB = evnt0 >> 4;
	evnt0_MSB = evnt0_MSB << 4;
	evnt0_LSB = evnt0 - evnt0_MSB;


	for (t=0; t<9; t++)    // set bankstick based on channel
	{
		if (t == 8) return; // if no matching bankstick channel, do not process MPROC_NotifyReceivedEvnt
		if (bankstick_channel[t] == evnt0_LSB)
		{
			MIOS_BANKSTICK_CtrlSet(t);
			channel = bankstick_channel[t];
			bankstick_select = t;
			//found_event.bankstick = t;
			break;
		}
	}

	// Allow LCD if it's the same event
	//if (previous_evnt0 == evnt0 && previous_evnt1 == evnt1) resume_LCD();
	previous_evnt0 = evnt0;
	previous_evnt1 = evnt1;

#endif


	
#if ENABLE_MIDI_TO_AIN
	//Override MIDI In to Covert MIDI pedal to AIN
	
	if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL0_E1)
	{
		AIN_handler(0, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL1_E1)
	{
		AIN_handler(1, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL2_E1)
	{
		AIN_handler(2, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL3_E1)
	{
		AIN_handler(3, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL4_E1)
	{
		AIN_handler(4, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL5_E1)
	{
		AIN_handler(5, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL6_E1)
	{
		AIN_handler(6, evnt2);
	} 
	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == MIDI_PEDAL7_E1)
	{
		AIN_handler(7, evnt2);
	}
	
	
	else
	{
	// End override
#endif
	// Setup and misc functions

	if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == EVENT_SETUP_E1 && evnt2 == EVENT_SETUP_E2)
	{
		if (program_mode == NONE)
		{
			MIOS_BANKSTICK_CtrlSet(0);
			if (bankpin < 143)
			{
				found_event.bankstick = MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2) + 3) & 0x07;
			}
			else
			{
				if (AIN_fixed[bankpin - 144] == 0)
				{
					found_event.bankstick = MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + (current_programchange_param1[0] << 2) + 3) & 0x07;
				}
				else
				{
					found_event.bankstick = MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + 3) & 0x07;
				}
			}

			MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
			current_bankstick_C = found_event.bankstick;
			channel = bankstick_channel[found_event.bankstick];
			program_mode = ENTER_SETUP;
			MIOS_LCD_MessageStop();
			app_flags.DISPLAY_UPDATE_REQ = 1;


		}
		else
		{
			program_mode_handler(); //save
		}
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == SCREEN_CHANGE_E1 && evnt2 == SCREEN_CHANGE_E2 && program_mode != NONE)
	{

		MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
		if (program_mode == SET_EVENT_BANKSTICK)
		{
			program_mode = SET_EVENT_TYPE;
			correct_program_mode_selection();
		}
		else if (program_mode == SET_EVENT_TYPE)
		{
			program_mode = EVENT_SELECT;
			correct_program_mode_selection();
		}
		else if (program_mode == EVENT_SELECT)
		{
			program_mode = SET_MIN;
		}
		else if (program_mode == SET_MIN)
		{
			program_mode = SET_MAX;
		}
		else if (program_mode == SET_MAX)
		{
			program_mode = SET_EVENT_BANKSTICK;
		}
		else if (program_mode == SET_AIN_FIXED)
		{
			program_mode = SET_BANKSTICK1_CH;
		}
		else if (program_mode >= SET_BANKSTICK1_CH && program_mode < SET_BANKSTICK8_CH)
		{
			program_mode++;
		}
		else if (program_mode == SET_BANKSTICK8_CH)
		{
			program_mode = SET_AIN_FIXED;
		}
		else if (program_mode == CUE_SELECT_LIST)
		{
			program_mode = CUE_PATCH_SELECT;
		}
		else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
		{
			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_TYPE;
		}
		else if (program_mode == PATCH_SET_EVENT_TYPE)
		{
			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			correct_program_mode_selection();
			program_mode = PATCH_EVENT_SELECT;
		}
		else if (program_mode == PATCH_EVENT_SELECT)
		{
			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_VALUE;
		}
		else if (program_mode == PATCH_SET_EVENT_VALUE)
		{
			MIOS_BANKSTICK_CtrlSet(0);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3, patch_event_bankstick | (patch_event_enabled << 7) | (patch_event_type << 4)); // button_type is used here to hold enabled or disabled
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 0, patch_entry);
			//MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 1, found_event.low_value);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 2, patch_event_high_value);

			patch_sub++;
			if (patch_sub >= 32) patch_sub = 0;
			//found_event.bankstick = MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3)  & 0x07;
			fill_found_control_info(0x0020, event_entry_current_programchange[0], 0x92, 16, patch_sub, 4);

			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
		}
		else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
		{
			program_mode++;
			if (program_mode > PATCH_SET_RELAY8) program_mode = PATCH_SET_RELAY1;
		}
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == GLOBAL_SETUP_E1 && evnt2 == GLOBAL_SETUP_E2)
	{

		if (program_mode == NONE)
		{
			program_mode = SET_AIN_FIXED;
		}
		else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
		{
			current_relay = program_mode - PATCH_SET_RELAY1;
			if (relay_enabled[current_relay] != 0)
			{
				relay_enabled[current_relay] = 0;
			}
			else
			{
				relay_enabled[current_relay] = 1;
			}
		}
		else if (program_mode == PATCH_SET_EVENT_VALUE) {
			if (patch_event_enabled == 0) {
				patch_event_enabled = 1;
			} else {
				patch_event_enabled = 0;
			}
		}
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == CUE_SELECT_E1 && evnt2 == CUE_SELECT_E2)
	{
		program_mode = CUE_SELECT_LIST;
		current_cue = 0;
		patch_select = 0;
		patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select * 2));
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == CUE_E1 && evnt2 == 0x01)
	{
		if (program_mode == CUE_PATCH_SELECT)
		{
			MIOS_BANKSTICK_Write(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select * 2), patch_event);
			patch_select--;
			patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select * 2));
			MIOS_LCD_MessageStop();
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else
		{
			// go prev cue
			go_prevcue();
		}
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == CUE_E1 && evnt2 == 0x02)
	{
		if (program_mode == CUE_PATCH_SELECT)
		{
			
			MIOS_BANKSTICK_Write(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1), patch_event);
			patch_select++;
			patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1));
			MIOS_LCD_MessageStop();
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else
		{
			// go next cue
			go_nextcue();
		}
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 >= RELAY_E1 && evnt1 <= RELAY_E1 + 0x08)
	{
		found_event.entry = evnt1 - RELAY_E1 + 2;
		found_event.bankstick = 0;
		found_event.event_type = STAT_SPECIAL;
		if (evnt2 >= 0x40)
		{
			if (MIOS_BANKSTICK_Read(0x5ff0 + evnt1 - RELAY_E1) == 1) {
				dout_set = 0; //if inverted
			} else {
				dout_set = 1;
			}
			//debug = 1;
			MIOS_DOUT_PinSet(relay_dout_start_pin + evnt1 - RELAY_E1, dout_set);
			MIOS_DOUT_PinSet(relay_led_dout_start_pin + evnt1 - RELAY_E1, 1);
			set_value_related_buttons(found_event.entry, STAT_SPECIAL, 0, 0x7f);
		}
		else
		{
			//dout_set = 0;
			if (MIOS_BANKSTICK_Read(0x5ff0 + evnt1 - RELAY_E1) == 1) {
				dout_set = 1;  // if inverted
			} else {
				dout_set = 0;	
			}
			debug = 0;
			MIOS_DOUT_PinSet(relay_dout_start_pin + evnt1 - RELAY_E1, dout_set);
			MIOS_DOUT_PinSet(relay_led_dout_start_pin + evnt1 - RELAY_E1, 0);
			set_value_related_buttons(found_event.entry, STAT_SPECIAL, 0, 0x00);
		}
		previous_input_type = MIDI;
		display_temp();
	}

	else if (evnt0_MSB == 0xb0 && bankstick_select == 0 && evnt1 == PATCH_SETUP_E1 && evnt2 == PATCH_SETUP_E2)
	{
		if (program_mode == NONE)
		{
			patch_sub = 0;

			//////REVIEW//////////

			//patch_event_bankstick = MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3)  & 0x07;

			fill_found_control_info(0x0020, event_entry_current_programchange[0], 0x92, 16, patch_sub, 4);

			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
			MIOS_LCD_MessageStop();
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else if (program_mode == PATCH_SET_EVENT_VALUE)
		{
			if (patch_event_enabled != 0)
			{
				patch_event_enabled = 0;
			}
			else
			{
				patch_event_enabled = 1;
			}
			MIOS_LCD_MessageStop();
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
		{
			for (t=0;t<8;t++)
			{
				relay_on_off[t] = (MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x90)) << t;
				relay_on_off[t] = relay_on_off[t] >> 7;
				relay_enabled[t] = (MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x91)) << t;
				relay_enabled[t] = relay_enabled[t] >> 7;
			}
			program_mode = PATCH_SET_RELAY1;
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
		else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
		{
			mask = 7;
			relay_enabled[8] = 0;
			relay_on_off[8] = 0;
			for (t=0;t<8;t++)
			{
				relay_enabled[8] = relay_enabled[8] + (relay_enabled[t] << mask);
				relay_on_off[8] = relay_on_off[8] + (relay_on_off[t] << mask);
				mask--;
			}
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x90, relay_on_off[8]);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x91, relay_enabled[8]);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
			app_flags.DISPLAY_UPDATE_REQ = 1;
		}
	}
	
	// Normal MIDI mode

#if ENABLE_MIDI_IN
	else if (program_mode == NONE && evnt0_MSB == 0xb0 && evnt1 >= PEDALSWAP_E1 && evnt1 < PEDALSWAP_E1 + NUM_PEDALSWAP_BUTTONS && bankstick_select == 0) {
	
	
		
			
			MIOS_BANKSTICK_CtrlSet(0);
			app_flags.PEDALSWAP_TRIGGERED = 1;
			found_patchnum = MIOS_BANKSTICK_Read(0x09 + (evnt1 - PEDALSWAP_E1));
			if (found_patchnum == current_pedalswap_patch) {
				triggerPedalSwap(7); // disable
			} else {
				triggerPedalSwap(evnt1 - PEDALSWAP_E1);
			}



		
		
	}
	else if (program_mode == NONE)
	{

		found_event.bankstick = bankstick_select;
		if (found_event.bankstick == 0)   //is patch bankstick
		{
			if (evnt0_MSB == 0xc0)
			{
				found_event.entry = evnt1;
				program_change_handler();
				set_value_related_buttons(found_event.entry, STAT_PROGRAM, found_event.bankstick, 0x7f);
				suspend_LCD();
				run_patch(evnt1);
				app_flags.DISPLAY_UPDATE_REQ = 1;
				
			}

		}
		else
		{
			found_event.entry = evnt1;
			found_event.param1 = evnt1;
			found_event.param2 = evnt2;
			found_event.status = evnt0;

			if (evnt0_MSB == 0xb0)
			{

				for (t=0; t<16; t++) found_event.name[t] = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + t);
				//found_event.status = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 16);
				//found_event.param1 = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 17);
				found_event.event_handler = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 18);
				found_event.event_type = STAT_CONTROL;



				if ( found_event.event_handler == TAP_TEMPO )  // is tap tempo button
				{
					tap_tempo_handler();
					display_temp();
				}

				else
				{

					set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
					display_temp();
				}



			}

			else if (evnt0_MSB == 0xc0 )
			{
				found_event.event_type = STAT_PROGRAM;
				found_event.event_handler = PROGRAM_CHANGE;
				program_change_handler();

				MIOS_LCD_MessageStop();
				app_flags.DISPLAY_UPDATE_REQ = 1;

				// This prevents midi messages associated with a program change from being shown on the display
				suspend_LCD();

			}

			else if (evnt0_MSB == 0x80)
			{
				found_event.event_type = STAT_NOTE_OFF;
				found_event.event_handler = NOTE_OFF;
				display_temp();
			}
			else if (evnt0_MSB == 0x90)
			{
				found_event.event_type = STAT_NOTE_ON;
				found_event.event_handler = NOTE_ON;
				display_temp();
			}
			else if (evnt0_MSB == 0xa0)
			{
				found_event.event_type = STAT_AFTER_TOUCH;
				found_event.event_handler = AFTER_TOUCH;
				display_temp();
			}
			else if (evnt0_MSB == 0xd0)
			{
				found_event.event_type = STAT_PRESSURE;
				found_event.event_handler = PRESSURE;
				display_temp();
			}
			else if (evnt0_MSB == 0xe0)
			{
				found_event.event_type = STAT_PITCH_BEND;
				found_event.event_handler = PITCH_BEND;
				display_temp();
			}
		}
	}
#if ENABLE_MIDI_TO_AIN
	}
#endif
	
	MIOS_MPROC_MergerEnable();

#endif // ENABLE MIDI IN


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
	// Disable merger if recieved event on master bankstick channel to prevent forwarding of internal control messages
	// Merger automatically resumes when a new complete event is recieved
	#if MIDI_MERGER
	#if MIDI_MERGER_DISABLE_MASTER
	if(byte >= 0x80 && byte < 0xf0) {
		byte_low_nibble = byte & 0x0f;
		if (byte_low_nibble == bankstick_channel[0]) {
			MIOS_MPROC_MergerDisable();
		} else {
			MIOS_MPROC_MergerEnable();
		}
	}
	#endif
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
		//Pedalswap flash LED timer

	if (active_pedalswap != 7) {
		flash_counter++;
		if (flash_counter > 1000) {
			MIOS_DOUT_PinSet(active_pedalswap + FIRST_PEDALSWAP_DIN_PIN, ~MIOS_DOUT_PinGet(active_pedalswap + FIRST_PEDALSWAP_DIN_PIN));

			flash_counter = 0;
		}
		/*
		MIOS_LCD_CursorSet(0x0f);
		if (MIOS_DOUT_PinGet(active_pedalswap + FIRST_PEDALSWAP_DIN_PIN) == 1) {
			MIOS_LCD_PrintChar('X');
		} else {
			MIOS_LCD_PrintBCD1(active_pedalswap + 1);
		}
		*/
	}
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
// If floorboard mode not active, this is irrelevant.
/////////////////////////////////////////////////////////////////////////////
void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
{

#if PEDALBOARD

#if ENABLE_BANK_LEDS
	if (pin == DIN_BANK_UP || pin == DIN_BANK_DOWN)
	{
		MIOS_DOUT_PinSet(pin, ~pin_value & 0x01);
	}
# endif

	// Start with handling of special function buttons

	if (pin == EVENT_SETUP_DIN_PIN && pin_value == 0 && MIOS_DIN_PinGet(DIN_BANK_DOWN) == 0 && program_mode == NONE)
	{
		//program_mode_handler; -- THIS SHOULD WORK, BUT DOESN'T, CODE TEMPORARILY ADDED HERE
		MIOS_BANKSTICK_CtrlSet(0);
		if (bankpin < 144)
		{
			//found_event.bankstick = (MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2) + 3) & 0x07);
			fill_found_control_info(0x6900, bankpin, 4, 0, 0, 0);
		}
		else
		{
			if (AIN_fixed[bankpin - 144] == 0)
			{
				
				//found_event.bankstick = MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + (current_programchange_param1[0] << 2) + 3) & 0x07;
				fill_found_control_info(0x7000, bankpin, 0x200, 0, current_programchange_param1[0], 4);
			}
			else
			{
				
				//found_event.bankstick = MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + 3) & 0x07;
				fill_found_control_info(0x7000, bankpin, 0x200, 0, 0, 0);
			}
		}

		MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
		channel = bankstick_channel[found_event.bankstick];
		program_mode = ENTER_SETUP;
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (pin == GLOBAL_SETUP_DIN_PIN && pin_value == 0 && MIOS_DIN_PinGet(DIN_BANK_DOWN) == 0 && program_mode == NONE)
	{
		MIOS_BANKSTICK_CtrlSet(0);
		program_mode = SET_AIN_FIXED;
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (pin == GLOBAL_SETUP_DIN_PIN && pin_value == 0 && program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
	{
		current_relay = program_mode - PATCH_SET_RELAY1;
		if (relay_enabled[current_relay] != 0)
		{
			relay_enabled[current_relay] = 0;
		}
		else
		{
			relay_enabled[current_relay] = 1;
		}

		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (pin == DIN_BANK_UP && pin_value == 0)
	{

		if (program_mode == SET_EVENT_BANKSTICK)
		{
			program_mode = SET_EVENT_TYPE;
			correct_program_mode_selection();
		}

		else if (program_mode == SET_EVENT_TYPE)
		{
			program_mode = EVENT_SELECT;
			correct_program_mode_selection();
		}

		else if (program_mode == EVENT_SELECT)
		{
			program_mode = SET_MIN;
		}

		else if (program_mode == SET_MIN)
		{
			program_mode = SET_MAX;
		}

		else if (program_mode == SET_MAX)
		{
			program_mode = SET_EVENT_BANKSTICK;
		}

		else if (program_mode == SET_AIN_FIXED)
		{
			program_mode = SET_BANKSTICK1_CH;
		}

		else if (program_mode >= SET_BANKSTICK1_CH && program_mode < SET_BANKSTICK8_CH)
		{
			program_mode++;
		}

		else if (program_mode == SET_BANKSTICK8_CH)
		{
			program_mode = SET_AIN_FIXED;
		}

		else if (program_mode == NONE)
		{
			bank++;
			if (bank > (128 / DIN_BANKED_BUTTONS) + 1 ) bank = (128 / DIN_BANKED_BUTTONS) + 1 ; // Max bank = (128 / the amount of banked buttons) + 1
		}

		else if (program_mode == CUE_SELECT_LIST)
		{
			found_event.bankstick = 0;
			MIOS_BANKSTICK_CtrlSet(0);
			program_mode = CUE_PATCH_SELECT;
		}

		else if (program_mode == CUE_PATCH_SELECT)
		{
			found_event.bankstick = 0;
			MIOS_BANKSTICK_CtrlSet(0);
			program_mode = CUE_SELECT_LIST;
		}

		else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
		{
			program_mode = PATCH_SET_EVENT_TYPE;
		}
		else if (program_mode == PATCH_SET_EVENT_TYPE)
		{
			program_mode = PATCH_EVENT_SELECT;
		}

		else if (program_mode == PATCH_EVENT_SELECT)
		{
			program_mode = PATCH_SET_EVENT_VALUE;
		}

		else if (program_mode == PATCH_SET_EVENT_VALUE)
		{
			MIOS_BANKSTICK_CtrlSet(0);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3, (patch_event_enabled << 7) | (patch_event_type << 4) | patch_event_bankstick); // button_type is used here to hold enabled or disabled
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 0, patch_entry);
			//MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 1, found_event.low_value);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 2, patch_event_high_value);

			patch_sub++;
			if (patch_sub >= 32) patch_sub = 0;
			//found_event.bankstick = MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3)  & 0x07;

			fill_found_control_info(0x0020, event_entry_current_programchange[0], 0x92, 16, patch_sub, 4);

			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
		}

		else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
		{
			program_mode++;
			if (program_mode > PATCH_SET_RELAY8) program_mode = PATCH_SET_RELAY1;
		}
		MIOS_LCD_MessageStop();
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (pin == DIN_BANK_DOWN && pin_value == 0)
	{
		if (program_mode > NONE)
		{
			program_mode_handler();
		}
		else
		{
			bank --;
			if (bank < 1) bank = 1;
		}
		MIOS_LCD_MessageStop();

		app_flags.DISPLAY_UPDATE_REQ = 1;
	}

	else if (pin == EVENT_SETUP_DIN_PIN && pin_value == 0 && program_mode != NONE)
	{
		if (program_mode == SET_MIN || program_mode == SET_MAX)
		{
			if (found_event.button_type != 0)
			{
				found_event.button_type = 0;
			}
			else
			{
				found_event.button_type = 1;
			}
		} else if (program_mode == PATCH_SET_EVENT_VALUE) {
			if (patch_event_enabled != 0)
			{
				patch_event_enabled = 0;
			}
			else
			{
				patch_event_enabled = 1;
			}
		}
		if (program_mode == EVENT_SELECT && MIOS_BANKSTICK_CtrlGet() == 0 && bankpin_map_to_event_entry < 130)   // enter patch setup mode
		{
			patch_sub = 0;
			//found_event.bankstick = MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 16 + (patch_sub * 4) + 3) & 0x07;
			fill_found_control_info(0x0020, event_entry_current_programchange[0], 0x92, 16, patch_sub, 4);

			MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
		}

		else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
		{

			for (t=0;t<8;t++)
			{
				relay_on_off[t] = (MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x90)) << t;
				relay_on_off[t] = relay_on_off[t] >> 7;
				relay_enabled[t] = (MIOS_BANKSTICK_Read(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x91)) << t;
				relay_enabled[t] = relay_enabled[t] >> 7;
			}
			program_mode = PATCH_SET_RELAY1;
		}

		else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
		{
			mask = 7;
			relay_enabled[8] = 0;
			relay_on_off[8] = 0;
			for (t=0;t<8;t++)
			{
				relay_enabled[8] = relay_enabled[8] + (relay_enabled[t] << mask);
				relay_on_off[8] = relay_on_off[8] + (relay_on_off[t] << mask);
				mask--;
			}
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x90, relay_on_off[8]);
			MIOS_BANKSTICK_Write(0x0020 + (0x92 * event_entry_current_programchange[0]) + 0x91, relay_enabled[8]);
			program_mode = PATCH_SET_EVENT_BANKSTICK;
		}
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}


	else if (program_mode == NONE || program_mode == CUE_PATCH_SELECT)   // Handling for floorboard mode
	{

		if (pin >= FIRST_PEDALSWAP_DIN_PIN && pin < FIRST_PEDALSWAP_DIN_PIN + NUM_PEDALSWAP_BUTTONS && program_mode == NONE) {
			app_flags.PEDALSWAP_TRIGGERED = 1;
			MIOS_BANKSTICK_CtrlSet(0);
			found_patchnum = MIOS_BANKSTICK_Read(0x09 + (pin - FIRST_PEDALSWAP_DIN_PIN));
			if (found_patchnum == current_pedalswap_patch) {
				triggerPedalSwap(7); // disable
			} else {
				triggerPedalSwap(pin - FIRST_PEDALSWAP_DIN_PIN);
			}
			
		} else {

			MIOS_BANKSTICK_CtrlSet(0); // set to patch bankstick]
			bankpin = 4;
			if (pin > DIN_FIXED_BUTTONS - 1)
			{
				bankpin = pin + (bank * DIN_BANKED_BUTTONS) - DIN_BANKED_BUTTONS - DIN_FIXED_BUTTONS + 16;
				if (bankpin > 143) bankpin = 143; // catch for the 'make up' bank to stop buttons with values higher than 128
			}
			else
			{
				bankpin = pin;
			}

			previous_bankpin = bankpin;
			previous_input_type = DIN;
			found_event.bankstick = MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2) + 3) & 0x07;

			if (found_event.bankstick == 0)
			{
				found_event.entry = MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2));
				found_event.button_type = (MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2) + 3) & 0x80) >> 7;
				found_event.event_type = (MIOS_BANKSTICK_Read(0x6900 + (bankpin << 2) + 3) & 0x70) >> 4;
				if (found_event.event_type == STAT_SPECIAL)
				{
					if (found_event.entry == 0 && pin_value == 0)      // next cue sfb
					{
						if (MIOS_DIN_PinGet(DIN_BANK_DOWN) == 0)
						{
							program_mode = CUE_SELECT_LIST;
							patch_select = 0;
							current_cue = 0;
							patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1));
							MIOS_LCD_MessageStop();
							app_flags.DISPLAY_UPDATE_REQ = 1;
						}
						else if (program_mode == CUE_PATCH_SELECT)
						{
							MIOS_BANKSTICK_Write(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1), patch_event);
							patch_select++;
							patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1));
							
							MIOS_LCD_MessageStop();
							app_flags.DISPLAY_UPDATE_REQ = 1;
						}
						else
						{
							go_nextcue();
						}
					}

					else if (found_event.entry == 1 && pin_value == 0)   // previous cue sfb
					{
						if (program_mode == CUE_PATCH_SELECT)
						{
							MIOS_BANKSTICK_Write(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1), patch_event);
							patch_select--;
							patch_event = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (patch_select << 1));
							MIOS_LCD_MessageStop();
							app_flags.DISPLAY_UPDATE_REQ = 1;
						}
						else
						{
							go_prevcue();
						}
					}

					else if (found_event.entry >= 2)   // relay switces
					{
						bankpin_map_to_event_entry = found_event.entry;

						if (found_event.button_type == SWITCH  && pin_value == 0 )
						{

							if (MIOS_BANKSTICK_Read(0x5ff0 + found_event.entry - 2) == 1) {
								dout_set = 1; // if inverted
							} else {
								dout_set = 0;
							}
								led_set =  0;
							if (MIOS_DOUT_PinGet(relay_dout_start_pin + found_event.entry - 2) == dout_set)
							{
								dout_set = ~dout_set & 0x01;
								led_set = ~led_set & 0x01;
							}
							if (led_set == 0) {
								val_set = 0x00;
							} else {
								val_set = 0x7f;
							}
							MIOS_DOUT_PinSet(relay_dout_start_pin + found_event.entry - 2, dout_set);
							MIOS_DOUT_PinSet(relay_led_dout_start_pin + found_event.entry - 2, led_set);
							set_value_related_buttons(found_event.entry, STAT_SPECIAL, 0, val_set);
						}
						
						else
						{

							
							if (pin_value == 0)
							{
								
								if (MIOS_BANKSTICK_Read(0x5ff0 + found_event.entry - 2) == 1) {
									dout_set = 0; // if inverted
								} else {
									dout_set = 1;
								}
							
								MIOS_DOUT_PinSet(relay_dout_start_pin + found_event.entry - 2, dout_set);
								MIOS_DOUT_PinSet(relay_led_dout_start_pin + found_event.entry - 2, 1);
								set_value_related_buttons(found_event.entry, STAT_SPECIAL, 0, 0x7f);
							}
							else
							{
								if (MIOS_BANKSTICK_Read(0x5ff0 + found_event.entry - 2) == 1) {
									dout_set = 1; // if inverted
								} else {
									dout_set = 0;
								}
								MIOS_DOUT_PinSet(relay_dout_start_pin + found_event.entry - 2, dout_set);
								MIOS_DOUT_PinSet(relay_led_dout_start_pin + found_event.entry - 2, 0);
								set_value_related_buttons(found_event.entry, STAT_SPECIAL, 0, 0x00);
							}
						}

						display_temp();
					}
				}
				if (program_mode == NONE  && pin_value == 0 && found_event.event_type == STAT_SPECIAL) {
					if (found_event.entry == 0 || found_event.entry == 1)      // RUN THE CUE
					{
						cue_entry = found_event.entry;
						found_event.entry = MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + 16 + (current_cue << 1));
						for (t=0; t<16; t++) found_event.name[t] = MIOS_BANKSTICK_Read(0x0020 + (0x92 * found_event.entry) + t);
						program_change_handler();
						run_patch(found_event.entry);
						found_event.entry = cue_entry;
						display_temp();
						//suspend_LCD();
					}
				}

				else if (program_mode == NONE && found_event.event_type != STAT_SPECIAL && pin_value == 0)
				{
					bankpin_map_to_event_entry = found_event.entry;
					program_change_handler();
					suspend_LCD();
					run_patch(found_event.entry);
					MIOS_LCD_MessageStop();
					app_flags.DISPLAY_UPDATE_REQ = 1;

				}
			

			}

			else
			{
				fill_found_control_info(0x6900, bankpin, 4, 0, 0, 0);
				bankpin_map_to_event_entry = found_event.entry; // used by programing mode

				//run_event();
				MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
				channel = bankstick_channel[found_event.bankstick];

				found_event.status = getHexFromStat(found_event.event_type);
				found_event.param1 = found_event.entry;

				if (found_event.event_type == STAT_CONTROL)
				{


					// Set found_event variables
					for (t=0; t<16; t++) found_event.name[t] = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + t);
					//found_event.status = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 16);
					//found_event.param1 = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 17);
					found_event.event_handler = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 18);



					// Button handling dependant on type (event_handler)
					if (found_event.event_handler == ON_OFF_ONLY)    // is an on/off funtion
					{
						if (pin_value == 0 && found_event.button_type == 0)   // is toggle type
						{
							if (button_bankpin_value[bankpin] != 0x7f)
							{
								found_event.param2 = 0x7f;
							}
							else
							{
								found_event.param2 = 0x00;
							}
							send_midi_and_update();
						}
						else if (pin_value == 1 && found_event.button_type == 1)   // is momentary off
						{
							found_event.param2 = 0x00;
							send_midi_and_update();
						}
						else if (pin_value == 0 && found_event.button_type == 1)   // is momentary on
						{
							found_event.param2 = 0x7f;
							send_midi_and_update();
						}
					}


					else if (found_event.event_handler == TAP_TEMPO)  // is the tap tempo function
					{
						if (pin_value == 0)
						{
							tap_tempo_handler();
							button_bankpin_value[bankpin] = 0x7f; // force led on
							send_midi_and_update();
						}
						else
						{
							button_bankpin_value[bankpin] = 0x00; // force led off
						}
	#if USE_LED_INDICATORS
						app_flags.LED_UPDATE_REQ = 1;
	#endif
					}

					else          // > 2  - More handling could go here if needed
					{
						if (pin_value == 0 && found_event.button_type == 0)   // is toggle type
						{
							if (button_bankpin_value[bankpin] != found_event.high_value)
							{
								found_event.param2 = found_event.high_value;
							}
							else
							{
								found_event.param2 = found_event.low_value;
							}
							send_midi_and_update();
						}

						else if (pin_value == 1 && found_event.button_type == 1)   // is momentary off
						{
							found_event.param2 = found_event.low_value;
							send_midi_and_update();
						}

						else if (pin_value == 0 && found_event.button_type == 1)   // is momentary on
						{
							found_event.param2 = found_event.high_value;
							send_midi_and_update();
						}
					}

				}
				else if (found_event.event_type == STAT_PROGRAM)
				{

					if (pin_value == 0)
					{
						program_change_handler();
						MIDI_message_TX(found_event.status, found_event.entry, found_event.param1, channel);
						MIOS_LCD_MessageStop();
						app_flags.DISPLAY_UPDATE_REQ = 1;
						suspend_LCD();
					}
				}
				else if (found_event.event_type == STAT_NOTE_OFF)
				{
					if (pin_value == 0)
					{
						//found_event.status = 0x80;
						found_event.param1 = found_event.entry;
						found_event.param2 = 0x00;
						send_midi_and_update();
					}
				}
				else if (found_event.event_type == STAT_NOTE_ON)
				{
					//found_event.status = 0x80;
					found_event.param1 = found_event.entry;
					if (found_event.button_type == SWITCH)
					{
						if (pin_value == 0)
						{
							if (button_bankpin_value[bankpin] != found_event.high_value)
							{
								found_event.param2 = found_event.high_value;
							}
							else
							{
								found_event.param2 = found_event.low_value;
							}
						}

					}
					else
					{
						if (pin_value == 0)
						{
							found_event.param2 = found_event.high_value;
						}
						else
						{
							found_event.param2 = found_event.low_value;
						}
					}
					send_midi_and_update();
				}

				else if (found_event.event_type == STAT_AFTER_TOUCH)
				{
					//found_event.status = 0xa0;
					found_event.param1 = found_event.entry;
					if (found_event.button_type == SWITCH)
					{
						if (pin_value == 0)
						{
							if (button_bankpin_value[bankpin] != found_event.high_value)
							{
								found_event.param2 = found_event.high_value;
							}
							else
							{
								found_event.param2 = found_event.low_value;
							}
						}

					}
					else
					{
						if (pin_value == 0)
						{
							found_event.param2 = found_event.high_value;
						}
						else
						{
							found_event.param2 = found_event.low_value;
						}
					}
					send_midi_and_update();
				}


				else if (found_event.event_type == STAT_PRESSURE)
				{

					//found_event.status = 0xd0;
					if (found_event.button_type == SWITCH)
					{
						if (pin_value == 0)
						{
							if (button_bankpin_value[bankpin] != found_event.high_value)
							{
								found_event.param1 = found_event.high_value;
							}
							else
							{
								found_event.param1 = found_event.low_value;
							}
						}

					}
					else
					{
						if (pin_value == 0)
						{
							found_event.param1 = found_event.high_value;
						}
						else
						{
							found_event.param1 = found_event.low_value;
						}
					}
					found_event.param1 = found_event.param2;
						
					send_midi_and_update();
				}


				else if (found_event.event_type == STAT_PITCH_BEND)
				{
					//found_event.status = 0xe0;
					found_event.param1 = found_event.entry;
					if (found_event.button_type == SWITCH)
					{
						if (pin_value == 0)
						{
							if (button_bankpin_value[bankpin] != found_event.high_value)
							{
								found_event.param2 = found_event.high_value;
							}
							else
							{
								found_event.param2 = found_event.low_value;
							}
												
						

						}

					}
					else
					{
						if (pin_value == 0)
						{
							found_event.param2 = found_event.high_value;
						}
						else
						{
							found_event.param2 = found_event.low_value;
						}
					}
					found_event.param1 = found_event.param2;
					if (found_event.param2 == 64) found_event.param2 = 0; // allows for pitchbend center
					send_midi_and_update();
				}
			}
		} // PEDALWAP OVERRIDE
	}
	//suspend_LCD();
#endif //PEDALBOARD
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
{
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
{
	AIN_handler(pin, MIOS_AIN_Pin7bitGet(pin));
}

void AIN_handler(unsigned char pin, unsigned char pin_7bit_value)
{
	// Start with program mode handler
	if (program_mode == EVENT_SELECT)    // In program mode
	{
		if (found_event.bankstick == 0 && found_event.event_type == STAT_SPECIAL)
		{
			bankpin_map_to_event_entry = (pin_7bit_value) >> 3;
			if (bankpin_map_to_event_entry > 9) bankpin_map_to_event_entry = 9;
		}
		else if (found_event.bankstick == 0 && found_event.event_type != STAT_SPECIAL)
		{
			bankpin_map_to_event_entry = (pin_7bit_value);

		}
		else
		{
			bankpin_map_to_event_entry = pin_7bit_value;
		}
	}

	else if (program_mode == SET_MIN)
	{
		found_event.low_value = pin_7bit_value;
	}

	else if (program_mode == SET_MAX)
	{
		found_event.high_value = pin_7bit_value;
	}

	else if (program_mode == SET_AIN_FIXED)
	{
		AIN_fixed[pin] = pin_7bit_value >> 6; // gives 0 or 1
	}

	else if (program_mode == SET_EVENT_BANKSTICK)
	{
		found_event.bankstick = pin_7bit_value >> 4;
		if (previous_input_type == AIN && found_event.bankstick == 0) found_event.bankstick = 1;
	}

	else if (program_mode >= SET_BANKSTICK1_CH && program_mode <= SET_BANKSTICK8_CH )
	{
		if (bankstick_channel[program_mode - SET_BANKSTICK1_CH] != 255)
		{
			bankstick_channel[program_mode - SET_BANKSTICK1_CH] = pin_7bit_value >> 3;
		}
	}

	else if (program_mode == CUE_SELECT_LIST)
	{
		MIOS_BANKSTICK_CtrlSet(0);
		current_cue_list = pin_7bit_value >> 2;
		if (current_cue_list > 19) current_cue_list = 19;
	}

	else if (program_mode == CUE_PATCH_SELECT)
	{
		MIOS_BANKSTICK_CtrlSet(0);
		patch_event = pin_7bit_value;
	}

	else if (program_mode == PATCH_EVENT_SELECT)
	{
		MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
		patch_entry = pin_7bit_value;
	}

	else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
	{
		patch_event_bankstick = pin_7bit_value >> 4;
		if (patch_event_bankstick == 0) patch_event_bankstick = 1;
		MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);

	}

	else if (program_mode == PATCH_SET_EVENT_VALUE)
	{
		patch_event_high_value = pin_7bit_value;
	}

	else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
	{
		current_relay = program_mode - PATCH_SET_RELAY1;
		relay_on_off[current_relay] = pin_7bit_value >> 6;
	}
	else if (program_mode == SET_EVENT_TYPE)
	{
		found_event.event_type = MIOS_AIN_Pin7bitGet(pin) >> 4;
		if (found_event.bankstick == 0) {
			if(found_event.event_type != STAT_SPECIAL) found_event.event_type = STAT_PROGRAM;
		}
		else if (previous_input_type == AIN) {
			if(found_event.event_type == STAT_AFTER_TOUCH || found_event.event_type == STAT_PRESSURE || found_event.event_type == STAT_CONTROL || found_event.event_type == STAT_PITCH_BEND || found_event.event_type == STAT_AFTER_TOUCH) {
				//do nothing
			} else {
				found_event.event_type = STAT_CONTROL;
			}
		}
		else if (previous_input_type == DIN) {
			if(found_event.event_type == STAT_SPECIAL) found_event.event_type = STAT_PITCH_BEND;
		}
	}
	else if (program_mode == PATCH_SET_EVENT_TYPE)
	{

		patch_event_type = pin_7bit_value >> 4;
		if(patch_event_type == STAT_SPECIAL) patch_event_type = STAT_PITCH_BEND;

	}

	// Handling for Normal MIDI mode
	else
	{
		bankpin = pin + 144; // used by programing mode to identify AIN pins
		MIOS_BANKSTICK_CtrlSet(0);


		if (AIN_fixed[pin] == 0)   // pin st to change midi event with program change
		{
			if (active_pedalswap == 7) { //no swap active
				fill_found_control_info(0x7000, pin, 0x200, 0, current_programchange_param1[0], 4);
				debug = 221;
			} else {
				fill_found_control_info(0x7000, pin, 0x200, 0, current_pedalswap_patch, 4);
				debug = 112;
			}
			//found_event.bankstick = (MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + (current_programchange_param1[0] << 2) + 3)) & 0x07;
			button_event_map[bankpin] = found_event.entry;
		}
		else
		{
			fill_found_control_info(0x7000, pin, 0x200, 0, 0, 0);
			//found_event.bankstick = (MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + 3)) & 0x07;
			button_event_map[bankpin] = found_event.entry;
		}
		bankpin_map_to_event_entry = found_event.entry; // used by programing mode, displays current setting

		MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
		channel = bankstick_channel[MIOS_BANKSTICK_CtrlGet()];
		// Set found_event variables
		for (t=0; t<16; t++) found_event.name[t] = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + t);
		found_event.param1 = found_event.entry; //found_event.param1 is obsolete - this line saves me codeing time
		found_event.event_handler = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 18);
		previous_bankpin = bankpin;
		previous_input_type = AIN;

		found_event.param2 = Scale_7bit(pin_7bit_value, found_event.low_value, found_event.high_value);
		set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);

	}
	if (program_mode == NONE)
	{
		if (found_event.event_type == STAT_PRESSURE || found_event.event_type == STAT_PITCH_BEND)
		{
			found_event.param1 = found_event.param2;
			if (found_event.param2 == 64) found_event.param2 = 0; // allows for pitchbend center
		}
		found_event.status = getHexFromStat(found_event.event_type);
		send_midi_and_update();
	}
	else
	{
		app_flags.DISPLAY_UPDATE_REQ = 1;
	}
}
