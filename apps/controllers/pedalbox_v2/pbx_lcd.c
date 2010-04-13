/*
 * MIOS Pedal Box / Pedal Board - pbx_lcd.c
 * v2.6rc1 - April 2010
 * ==========================================================================
 *
 *  Copyright (C) 2010  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */
 
const unsigned char pan_map[16][17] =
{
    {"********        "},
    {" *******        "},
    {"  ******        "},
    {"   *****        "},
    {"    ****        "},
    {"     ***        "},
    {"      **        "},
    {"       *        "},
    {"        *       "},
    {"        **      "},
    {"        ***     "},
    {"        ****    "},
    {"        *****   "},
    {"        ******  "},
    {"        ******* "},
    {"        ********"}
};

const unsigned char note_map[12][3] =
{
    {"C "},
    {"C#"},
    {"D "},
    {"D#"},
    {"E "},
    {"F "},
    {"F#"},
    {"G "},
    {"G#"},
    {"A "},
    {"A#"},
    {"B "}
};


const unsigned char status_map[8][17]=
{
    {"Note Off       "},
    {"Note On        "},
    {"After Touch    "},
    {"Control        "},
    {"Program        "},
    {"Pressure       "},
    {"Pitch Bend     "},
    {"Special        "}
};


const unsigned char cue_names[2][17]=
{
    {"Previous Cue   "},
	{"Next Cue       "},
	
};



char octave;

// for value maps
unsigned char num_entries;
unsigned char nodata;

void display_meter(void)   // Displays the gragh in the second line
{
    param2_value_reduced = (found_event.param2 >> 3);
    for (t=0; t<16; t++)
    {
        if (t<=param2_value_reduced)
        {
            MIOS_LCD_PrintChar('*');
        }
        else
        {
            MIOS_LCD_PrintChar(' ');
        }
    }
}

unsigned char marker;

void print_relay_state(unsigned char relay_num, unsigned char relay_state)
{
	if (MIOS_BANKSTICK_Read(0x5ff0 + relay_num) == 1) {
		relay_state = ~relay_state & 0x01;
	}
	
	if (relay_state == 0) {
		marker = 32;
	} else {
		marker = 16;
	}
	
	for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x6000 + (relay_num * 0x30) + marker + t)); 	
	//MIOS_LCD_PrintBCD3(MIOS_BANKSTICK_Read(0x5ff0 + relay_num));
	//MIOS_LCD_PrintBCD3(debug);
	//MIOS_LCD_PrintBCD3(MIOS_DOUT_PinGet(relay_dout_start_pin + found_event.entry - 2));
	//MIOS_LCD_PrintBCD3(MIOS_DOUT_PinGet(relay_led_dout_start_pin + found_event.entry - 2));
	//MIOS_LCD_PrintBCD3(relay_state);

} 

void print_relay_name(unsigned char relay_num)
{
	for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x6000 + (relay_num * 0x30) + t)); 	
}

///////////////////////////////////////////////////////////////////////////
// Permanent display handler - called in main.c
// Messages produced here stay on they screen untill overwritten
///////////////////////////////////////////////////////////////////////////

void display_handler(void) // for permanent display
{

// do nothing if no update has been requested
    if ( !app_flags.DISPLAY_UPDATE_REQ )
        return;

    // clear request
    app_flags.DISPLAY_UPDATE_REQ = 0;

    MIOS_LCD_CursorSet(0x00);  //First Line
////////SETUP MODE

    if (program_mode == ENTER_SETUP)
    {

        MIOS_LCD_Clear();
        MIOS_LCD_PrintCString("Setup           ");
        program_mode = SET_EVENT_BANKSTICK;
        MIOS_LCD_MessageStart(200);
    }

    else if ( program_mode == SET_EVENT_TYPE)
    {
        if (bankpin > 143)
        {
            MIOS_LCD_PrintCString("Map Ped Type  ");
            MIOS_LCD_PrintBCD2(bankpin - 143);

        }
        else
        {
            MIOS_LCD_PrintCString("Map But Type ");
            MIOS_LCD_PrintBCD3(bankpin + 1);
        }

        MIOS_LCD_CursorSet(0x40);  //Second Line
        MIOS_LCD_PrintCString(status_map[found_event.event_type]);
    }

    else if (program_mode == EVENT_SELECT)
    {

        //Prints name
        if (bankpin > 143)
        {
            MIOS_LCD_PrintCString("Map Pedal ");
            MIOS_LCD_PrintBCD2(bankpin - 143);
            MIOS_LCD_PrintCString("    ");
        }
        else
        {
            MIOS_LCD_PrintCString("Map FootSwtch");
            MIOS_LCD_PrintBCD3(bankpin + 1);

        }

        MIOS_LCD_CursorSet(0x40);  //Second Line
        if (MIOS_BANKSTICK_CtrlGet() == 0)
        {
            if (found_event.event_type == STAT_SPECIAL)
            {
				if (bankpin_map_to_event_entry < 2) {
				// is gig control
					MIOS_LCD_PrintCString (cue_names[bankpin_map_to_event_entry]);
				} else {
				// is relay
					print_relay_name(bankpin_map_to_event_entry - 2);
				}
            }
            else
            {
                for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (0x92 * bankpin_map_to_event_entry) + t));
            }
        }
        else
        {
            if (found_event.event_type == STAT_CONTROL)
            {
                for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (bankpin_map_to_event_entry * 20) + t));
            }

            else if (found_event.event_type == STAT_PROGRAM)
            {
                for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + ((128 + bankpin_map_to_event_entry) * 20) + t));
            }

            else if (found_event.event_type == STAT_NOTE_OFF || found_event.event_type == STAT_NOTE_ON || found_event.event_type == STAT_AFTER_TOUCH)
            {
                note = bankpin_map_to_event_entry;
                for (octave= -1;octave<20;octave++)
                {
                    if (note < 12) break;
                    note = note - 12;
                }

                MIOS_LCD_PrintCString(note_map[note]);
                if (octave < 0)
                {
                    octave = octave - octave * 2;
                    MIOS_LCD_PrintChar(' ');
                }
                else
                {
                    MIOS_LCD_PrintChar('-');
                }
                MIOS_LCD_PrintBCD1(octave);
                MIOS_LCD_PrintCString("           ");

            }

            else if (found_event.event_type == STAT_PRESSURE || found_event.event_type == STAT_PITCH_BEND)
            {
                MIOS_LCD_PrintCString("N/A             ");
            }
            else
            {
                MIOS_LCD_PrintCString("N/A             ");
            }
        }
    }

    else if (program_mode == SET_MIN || program_mode == SET_MAX)
    {

        if (bankpin < 144)
        {
            MIOS_LCD_PrintCString("Low   B");
            MIOS_LCD_PrintBCD3(bankpin + 1);
        }
        else
        {
            MIOS_LCD_PrintCString("Low   P");
            MIOS_LCD_PrintBCD3(bankpin - 143);
        }
        MIOS_LCD_PrintCString("  High");
        MIOS_LCD_CursorSet(0x40);  //Second Line
        MIOS_LCD_PrintBCD3(found_event.low_value);
        //MIOS_LCD_PrintCString("     ");
        if (bankpin < 144)
        {
            if ( found_event.button_type == 0 )
            {
                MIOS_LCD_PrintCString("  Toggle  ");
            }
            else
            {
                MIOS_LCD_PrintCString("  Moment  ");
            }
        }
        else
        {
            MIOS_LCD_PrintCString("          ");
        }
        MIOS_LCD_PrintBCD3(found_event.high_value);
        if (program_mode == SET_MIN)
        {
            MIOS_LCD_CursorSet(0x43);
            MIOS_LCD_PrintChar('<');
        }
        else
        {
            MIOS_LCD_CursorSet(0x4c);
            MIOS_LCD_PrintChar('>');
        }
    }


    else if (program_mode == SET_AIN_FIXED)
    {


        MIOS_LCD_PrintCString("LockPed 12345678");
        MIOS_LCD_CursorSet(0x40);  //Second Line
        MIOS_LCD_PrintCString("Y=1 N=0 ");
        for (t=0; t<8; t++) MIOS_LCD_PrintBCD1(AIN_fixed[t]);
    }

    else if (program_mode == EXIT_SETUP)
    {

        MIOS_LCD_Clear();

        MIOS_LCD_PrintCString("Saved");

        program_mode = NONE;
        MIOS_LCD_MessageStart(200);
    }
    else if (program_mode == SET_EVENT_BANKSTICK)
    {
        if (bankpin > 143)
        {
            MIOS_LCD_PrintCString("Pedal ");
            MIOS_LCD_PrintBCD2(bankpin - 143);
        }
        else
        {
            MIOS_LCD_PrintCString("FootSw ");
        }
        MIOS_LCD_PrintCString(" Ch:");
        MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
        channel = bankstick_channel[found_event.bankstick];
        if ( channel != 255 )
        {
            MIOS_LCD_PrintBCD2(channel + 1);
            MIOS_LCD_PrintCString("    ");
            MIOS_LCD_CursorSet(0x40);  //Second Line
            for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0010 + t));
        }
        else
        {
            MIOS_LCD_PrintCString("N/A  ");
            MIOS_LCD_CursorSet(0x40);
            MIOS_LCD_PrintCString("BS Not Available");
        }
    }

    else if (program_mode >= SET_BANKSTICK1_CH && program_mode <= SET_BANKSTICK4_CH)
    {


        MIOS_LCD_PrintCString("B.S   1  2  3  4");
        MIOS_LCD_CursorSet(0x40);  //Second Line
        MIOS_LCD_PrintCString("Ch. ");
        for (t=0; t<4; t++)
        {
            if (bankstick_channel[t] == 255)
            {
                MIOS_LCD_PrintCString(" NA");
            }
            else
            {
                MIOS_LCD_PrintBCD3(bankstick_channel[t] + 1);
            }
        }
        MIOS_LCD_CursorSet(0x44 + ((program_mode - SET_BANKSTICK1_CH) * 3));
        MIOS_LCD_PrintChar('>');
    }
    else if (program_mode >= SET_BANKSTICK5_CH && program_mode <= SET_BANKSTICK8_CH)
    {


        MIOS_LCD_PrintCString("B.S   5  6  7  8");
        MIOS_LCD_CursorSet(0x40);  //Second Line
        MIOS_LCD_PrintCString("Ch. ");
        for (t=4; t<8; t++)
        {
            if (bankstick_channel[t] == 255)
            {
                MIOS_LCD_PrintCString(" NA");
            }
            else
            {
                MIOS_LCD_PrintBCD3(bankstick_channel[t] + 1);
            }
        }
        MIOS_LCD_CursorSet(0x44 + ((program_mode - SET_BANKSTICK5_CH) * 3));
        MIOS_LCD_PrintChar('>');
    }

    else if (program_mode == CUE_SELECT_LIST)
    {
        MIOS_LCD_PrintCString("Select Cue List ");
        MIOS_LCD_CursorSet(0x40);  //Second Line
        for (t=0;t<16;t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x4920 + (current_cue_list * 0x110 ) + t));
    }

    else if (program_mode == CUE_PATCH_SELECT)
    {
        MIOS_LCD_PrintCString("Select Cue:  ");
        MIOS_LCD_PrintBCD3(patch_select);
        MIOS_LCD_CursorSet(0x40);
        for (t=0;t<16;t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (0x92 * patch_event) + t));
    }

    else if (program_mode == PATCH_EVENT_SELECT)
    {
        MIOS_LCD_PrintCString("Msg: ");
        MIOS_LCD_PrintBCD2(patch_sub);
        MIOS_LCD_PrintCString(" Event   ");
        MIOS_LCD_CursorSet(0x40);
        if (patch_event_type == STAT_CONTROL)
        {
            for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (patch_entry * 20) + t));
        }
        else if (patch_event_type == STAT_PROGRAM)
        {
            for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + ((128 + patch_entry) * 20) + t));
        }
        else if (patch_event_type == STAT_NOTE_OFF || patch_event_type == STAT_NOTE_ON || patch_event_type == STAT_AFTER_TOUCH )
        {
            note = patch_entry;
            for (octave= -1;octave<20;octave++)
            {
                if (note < 12) break;
                note = note - 12;
            }

            MIOS_LCD_PrintCString(note_map[note]);
            if (octave < 0)
            {
                octave = octave - octave * 2;
                MIOS_LCD_PrintChar(' ');
            }
            else
            {
                MIOS_LCD_PrintChar('-');
            }
            MIOS_LCD_PrintBCD1(octave);
            MIOS_LCD_PrintCString("           ");

        }


        else if (patch_event_type == STAT_PRESSURE || patch_event_type == STAT_PITCH_BEND)
        {
            MIOS_LCD_PrintCString("N/A             ");
        }
    }

    else if (program_mode == PATCH_SET_EVENT_TYPE)
    {
        MIOS_LCD_PrintCString("Msg: ");
        MIOS_LCD_PrintBCD2(patch_sub);
        MIOS_LCD_PrintCString(" Type    ");
        MIOS_LCD_CursorSet(0x40);
        MIOS_LCD_PrintCString(status_map[patch_event_type]);
    }

    else if (program_mode == PATCH_SET_EVENT_BANKSTICK)
    {
        MIOS_LCD_PrintCString("Msg: ");
        MIOS_LCD_PrintBCD2(patch_sub);
        MIOS_LCD_PrintCString(" BankStk ");
        MIOS_LCD_CursorSet(0x40);
        channel = bankstick_channel[patch_event_bankstick];
        if ( channel != 255 )
        {
            MIOS_BANKSTICK_CtrlSet(patch_event_bankstick);
            for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0010 + t));
        }
        else
        {
            MIOS_LCD_PrintCString("BS Not Available");
        }
    }

    else if (program_mode == PATCH_SET_EVENT_VALUE)
    {

        MIOS_LCD_PrintCString("Msg: ");
        MIOS_LCD_PrintBCD2(patch_sub);
        MIOS_LCD_PrintCString(" Value   ");

        MIOS_LCD_CursorSet(0x40);  //Second Line
        if ( patch_event_enabled == 1 )
        {
            MIOS_LCD_PrintCString("Enabled      ");
        }
        else
        {
            MIOS_LCD_PrintCString("Disabled     ");
        }

        MIOS_LCD_PrintBCD3(patch_event_high_value);
    }

    else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
    {
        MIOS_LCD_PrintCString("Enable  ");
        for (t=0;t<8;t++) MIOS_LCD_PrintBCD1(relay_enabled[t]);
        MIOS_LCD_CursorSet(0x40);
        MIOS_LCD_PrintCString("On/Off  ");
        for (t=0;t<8;t++) MIOS_LCD_PrintBCD1(relay_on_off[t]);

    }

    else   //////////MIDI MODE (program changes only)
    {


        if (found_event.bankstick == 0)
        {
			if(event_entry_current_programchange[0] != 128) {
				MIOS_BANKSTICK_CtrlSet(0);
				for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (event_entry_current_programchange[0] * 0x92) + t));
			} else {
				MIOS_LCD_PrintCString("No Patch Data   ");
			}
		}
        else
        {
			if(event_entry_current_programchange[found_event.bankstick] != 128) {
				MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
				for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + ((128 + event_entry_current_programchange[found_event.bankstick]) * 20) + t));
			} else {
				MIOS_LCD_PrintCString("No Program Data ");
			}        
		}
        MIOS_LCD_CursorSet(0x40);  //Second Line
        for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0010 + t));


        // Prints current bank if no LED digits are connected
#if PEDALBOARD
#if DIGITS_CONNECTED == 0
        MIOS_LCD_CursorSet(0x10);
        MIOS_LCD_PrintCString("|Bnk");
        MIOS_LCD_CursorSet(0x50);
        MIOS_LCD_PrintChar('|');
        MIOS_LCD_PrintBCD3(bank);
#else
        digits_handler(bank);
#endif
#if USE_LED_INDICATORS
        app_flags.LED_UPDATE_REQ = 1;
#endif
#endif
    //MIOS_LCD_CursorSet(0x00);
    //MIOS_LCD_PrintBCD3(debug);
    //MIOS_LCD_PrintCString("       ");

    } //End Normal Mode

}

///////////////////////////////////////////////////////////////////////////
// Temporary display handler - called in main.c
// Messages produced here stay on they screen for 2 seconds or until overwritten
///////////////////////////////////////////////////////////////////////////

void display_temp(void)
{

    
	if (app_flags.PCRX == 1) {
		#if USE_LED_INDICATORS
		app_flags.LED_UPDATE_REQ = 1;
		#endif
		return;
	}

	if(app_flags.PEDALSWAP_TRIGGERED) {
		MIOS_BANKSTICK_CtrlSet(0);
		MIOS_LCD_CursorSet(0x40);  //Second Line
		MIOS_LCD_PrintCString("PedalSwap ");
		
		if (current_pedalswap_patch == 130) {
			MIOS_LCD_PrintCString("Off   ");
			MIOS_LCD_CursorSet(0x00);  
			for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (0x92 * current_programchange_param1[0]) + t));
		} else {
			MIOS_LCD_PrintCString("Active");
			MIOS_LCD_CursorSet(0x00); 
			for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(0x0020 + (0x92 * current_pedalswap_patch) + t));
		}
		app_flags.PEDALSWAP_TRIGGERED = 0;
	}
	else 
	{
		MIOS_LCD_CursorSet(0x00);  //First Line
		MIOS_BANKSTICK_CtrlSet(found_event.bankstick);

	// patch entry to be looked at for patch number

		if (found_event.bankstick == 0)
		{
			if ( found_event.event_type == STAT_SPECIAL)
			{
				if (found_event.entry < 2) {
				// is gig control
					MIOS_LCD_PrintCString (cue_names[found_event.entry]);
				} else {
				// is relay
					print_relay_name(found_event.entry - 2);
				}
			}
			else
			{
				MIOS_LCD_PrintCString(found_event.name);
			}

		}
		else
		{
			if (found_event.event_type != STAT_CONTROL && found_event.event_type != STAT_PROGRAM)
			{
				MIOS_LCD_PrintCString(status_map[found_event.event_type]);
			}
			else
			{
				MIOS_LCD_PrintCString(found_event.name);
				//MIOS_LCD_PrintCString("TESTER");
			}
		}







		MIOS_LCD_CursorSet(0x40);  //Second Line

		if (found_event.event_type == STAT_SPECIAL)
		{


			if (found_event.bankstick == 0 && found_event.entry >= 2)
			{
				if (MIOS_DOUT_PinGet(relay_dout_start_pin + found_event.entry - 2) == 1)
				{
					print_relay_state(found_event.entry - 2, 1);
					//MIOS_LCD_PrintCString("       On       ");
				}
				else
				{
					print_relay_state(found_event.entry - 2, 0);
					//MIOS_LCD_PrintCString("       Off      ");
				}
			}
			else if (found_event.bankstick == 0 && found_event.entry == 0)
			{
			   // MIOS_LCD_PrintCString ("Next Cue        ");
			   MIOS_LCD_PrintCString(found_event.name);
			  
			}
			else if (found_event.bankstick == 0 && found_event.entry == 1)
			{
				//MIOS_LCD_PrintCString ("Previous Cue    ");
				MIOS_LCD_PrintCString(found_event.name);
			
			}


		}
		else if (found_event.event_type == STAT_CONTROL)
		{


			if ( found_event.event_handler == ON_OFF_ONLY)
			{
				if ( found_event.param2 == found_event.high_value) //cc value is 127
				{
					MIOS_LCD_PrintCString("       On       ");
				}
				else if (found_event.param2 == found_event.low_value)
				{
					MIOS_LCD_PrintCString("       Off      ");
				}
			}

	// Tap Tempo
			else if ( found_event.event_handler == TAP_TEMPO)
			{
				MIOS_LCD_PrintCString("  ");
				MIOS_LCD_PrintBCD5(bpm);
				MIOS_LCD_PrintCString(" BPM      ");
			}

	// Just display_meter
			else if ( found_event.event_handler == METER_ONLY)
			{
				display_meter();
			}

	// display_meter with 'off' when 0x00
			else if ( found_event.event_handler == METER_OFF)
			{
				if (found_event.param2 == found_event.low_value)
				{
					MIOS_LCD_PrintCString("       Off      ");
				}
				else
				{
					display_meter();
				}
			}

	// display_meter with 'on' at 0x7f and 'off' at 0x00
			else if ( found_event.event_handler == METER_ON_OFF )
			{
				if ( found_event.param2 == found_event.high_value) //cc value is 127
				{
					MIOS_LCD_PrintCString("       On       ");
				}
				else if (found_event.param2 == found_event.low_value)
				{
					MIOS_LCD_PrintCString("       Off      ");
				}
				else
				{
					display_meter();
				}
			}

			else if ( found_event.event_handler == METER_PAN )
			{
				param2_value_reduced = (found_event.param2 >> 3);
				if (found_event.param2 == 64)
				{
					MIOS_LCD_PrintCString("       ><       ");
				}
				else
				{
					MIOS_LCD_PrintCString(pan_map[param2_value_reduced]);
					//for (t=0; t<16; t++) MIOS_LCD_PrintChar(pan_map[param2_value_reduced][t]);
				}
			}

			else if ( found_event.event_handler == VALUE )
			{
				MIOS_LCD_PrintCString("      ");
				MIOS_LCD_PrintBCD3(found_event.param2);
				MIOS_LCD_PrintCString("       ");
			}

	// Handler for value_maps
			else if (found_event.event_handler >= USE_MAP_1 && found_event.event_handler <= USE_MAP_10)
			{
				switch (found_event.event_handler)
				{
				case USE_MAP_1:
					bankstick_offset = 0x1420;
					break;
				case USE_MAP_2:
					bankstick_offset = 0x1d22;
					break;
				case USE_MAP_3:
					bankstick_offset = 0x2624;
					break;
				case USE_MAP_4:
					bankstick_offset = 0x2f26;
					break;
				case USE_MAP_5:
					bankstick_offset = 0x3828;
					break;
				case USE_MAP_6:
					bankstick_offset = 0x412a;
					break;
				case USE_MAP_7:
					bankstick_offset = 0x4a2c;
					break;
				case USE_MAP_8:
					bankstick_offset = 0x532e;
					break;
				case USE_MAP_9:
					bankstick_offset = 0x5c30;
					break;
				case USE_MAP_10:
					bankstick_offset = 0x6532;
				}

				num_entries = MIOS_BANKSTICK_Read(bankstick_offset);
				
				if (num_entries < 128 && MIOS_BANKSTICK_Read(bankstick_offset + 2 + (num_entries * 18) + 16) == 129 && found_event.param2 > MIOS_BANKSTICK_Read(bankstick_offset + 2 + ((num_entries - 1) * 18) + 16) ) {
					MIOS_LCD_PrintCString("Not Applicable  ");

				}
				else
				{
				
					for (i=0; i<num_entries; i++)
					{
						if (i == num_entries - 1)   // if it's the last entry!!
						{
						
							
							if (found_event.param2 >= MIOS_BANKSTICK_Read(bankstick_offset + 2 + (i * 18) + 16))
							{
								//if found match copy to found_event_ variables
								for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(bankstick_offset + 2 + (i * 18) + t));
								break; //break for loop if found match
							}
						}
						else   // if not the last entry, get nearest lower value value if no exact match found
						{
						
							if (found_event.param2 >= MIOS_BANKSTICK_Read(bankstick_offset + 2 + (i * 18) + 16) && found_event.param2 < MIOS_BANKSTICK_Read(bankstick_offset + 2 + ((i + 1) * 18) + 16))
							{
								for (t=0; t<16; t++) MIOS_LCD_PrintChar(MIOS_BANKSTICK_Read(bankstick_offset + 2 + (i * 18) + t));
								
								break; //break for loop if found match
							}
						}
					}
				}
			}
		}
		else if (found_event.event_type == STAT_NOTE_OFF || found_event.event_type == STAT_NOTE_ON || found_event.event_type == STAT_AFTER_TOUCH)
		{

			note = found_event.param1;
			for (octave= -1;octave<20;octave++)
			{
				if (note < 12) break;
				note = note - 12;
			}

			MIOS_LCD_PrintCString(note_map[note]);
			MIOS_LCD_PrintBCD2(octave);
			MIOS_LCD_PrintCString(" ");
			MIOS_LCD_PrintBCD3(found_event.param2);
			MIOS_LCD_PrintCString("       ");
		}




		else if (found_event.event_type == STAT_PRESSURE)
		{
			found_event.param1 = found_event.param2;
			display_meter();
		}

		else if (found_event.event_type == STAT_PITCH_BEND)
		{
			param2_value_reduced = (found_event.param2 >> 3);
			if (found_event.param2 == 64)
			{
				MIOS_LCD_PrintCString("       ><       ");
			}
			else
			{
				MIOS_LCD_PrintCString(pan_map[param2_value_reduced]);
				//for (t=0; t<16; t++) MIOS_LCD_PrintChar(pan_map[param2_value_reduced][t]);
			}
		}



	// Prints current bank if no LED digits are connected
	#if PEDALBOARD
	#if DIGITS_CONNECTED == 0
		MIOS_LCD_CursorSet(0x10);
		MIOS_LCD_PrintCString("|Bnk");
		MIOS_LCD_CursorSet(0x50);
		MIOS_LCD_PrintChar('|');
		MIOS_LCD_PrintBCD3(bank);
	#else
		digits_handler(bank);
	#endif

	#endif
		/*
			//debug override
			MIOS_LCD_CursorSet(0x40);
			for (t=0;t<8;t++) MIOS_LCD_PrintHex2(debug[t]);
		*/
	}
#if USE_LED_INDICATORS
    app_flags.LED_UPDATE_REQ = 1;
#else 
	// show pedal swap on screen
	/*
	if (active_pedalswap != 7) {
		MIOS_LCD_CursorSet(0x0f);
		if (MIOS_DOUT_PinGet(active_pedalswap + FIRST_PEDALSWAP_DIN_PIN) == 1) {
			MIOS_LCD_PrintChar('X');
		} else {
			MIOS_LCD_PrintBCD1(active_pedalswap + 1);
		}
	}
	*/
#endif

    MIOS_LCD_MessageStart(255); // Display message for 2 seconds, then run LCD_init
}


