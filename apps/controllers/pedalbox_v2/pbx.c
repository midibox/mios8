/*
 * MIOS Pedal Box / Pedal Board - pbx.c
 * v2.5beta3 - April 2009
 * ==========================================================================
 *
 *  Copyright (C) 2009  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

#include "pbx.h"


////////////////////////////////////////////////////////////////////
//These functions are called by notify midi and DIN in main.c
//They process the tap tempo feature
////////////////////////////////////////////////////////////////////
void tap_tempo_handler(void)
{
    if (app_flags.TAP_FLAG)   // tap has already been pressed
    {
        tap_tempo_restart();
    }
    else
    {
        tap_tempo_start();
    }
}


void tap_tempo_start(void)
{
    MIOS_TIMER_Init(0x00, 10000); // first tap press
    app_flags.TAP_FLAG = 1;
}

void tap_tempo_restart(void)
{
    MIOS_TIMER_Stop();
    bpm = 60000 / bpmcountms; //  1 minute / (milliseconds between presses)
    bpmcountms = 0;
    MIOS_TIMER_Init(0x00, 10000);  // restart timer
}

void tap_tempo_stop(void) // times up for another press, stop the timer
{
    MIOS_TIMER_Stop();
    bpmcountms = 0;
    app_flags.TAP_FLAG = 0;
}

////////////////////////////////////////////////////////////////////
// This functions is called by in main.c
// It runs most of the program mode
//
// AIN_maps are located in the bankstick at 0x7000 - 0x7fff. Each map is 0x200 in size
// DIN_map is located in the bankstick at 0x6900 - 0x66cf
//
////////////////////////////////////////////////////////////////////
void program_mode_handler(void)
{
    if (program_mode == SET_AIN_FIXED || program_mode >= SET_BANKSTICK1_CH && program_mode <= SET_BANKSTICK8_CH)   // AIN_fixed programing mode
    {
        current_bankstick = MIOS_BANKSTICK_CtrlGet();
        //count = 0;
        MIOS_BANKSTICK_CtrlSet(0);
        for (i=0; i<8; i++)
        {
            MIOS_BANKSTICK_Write(i + 0x6ff0, AIN_fixed[i]);
        }
        for (t=0; t<8; t++)
        {
            MIOS_BANKSTICK_CtrlSet(t);
            MIOS_BANKSTICK_Write(0x0004, bankstick_channel[t]);
        }
        MIOS_BANKSTICK_CtrlSet(current_bankstick); // revert to programmed bankstick
        program_mode = EXIT_SETUP;

    }

    else if (program_mode == SET_EVENT_TYPE || program_mode == EVENT_SELECT || program_mode == SET_MIN || program_mode == SET_MAX || program_mode == SET_EVENT_BANKSTICK)   // 2 = saving pedal or button
    {
        current_bankstick = MIOS_BANKSTICK_CtrlGet();
        if (bankpin > 143)   // Programming a pedal
        {
            if (MIOS_BANKSTICK_Read(0x0020 + (bankpin_map_to_event_entry * 20) + 18) < METER_ONLY)   // can't map a pedal to program change, on/off, or tap
            {
                MIOS_LCD_Clear();
                MIOS_LCD_CursorSet(0x00);  //First Line
                MIOS_LCD_PrintCString("Can not Map");
                program_mode = SET_EVENT_BANKSTICK; // Stay in programing mode
            }
            else
            {


                bankpin = bankpin - 144;
                if (AIN_fixed[bankpin] == 0)
                {
                    MIOS_BANKSTICK_CtrlSet(0);    // bankpin * 0x200 = bankpin << 9
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + (current_programchange_param1[0] * 4), bankpin_map_to_event_entry); // save to eeprom
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + (current_programchange_param1[0] * 4) + 1, found_event.low_value);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + (current_programchange_param1[0] * 4) + 2, found_event.high_value);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + (current_programchange_param1[0] * 4) + 3, (found_event.event_type << 4) | found_event.bankstick);
                    button_event_map[bankpin + 144] = bankpin_map_to_event_entry;
                }
                else
                {
                    MIOS_BANKSTICK_CtrlSet(0);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + 0, bankpin_map_to_event_entry);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + 1, found_event.low_value);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + 2, found_event.high_value);
                    MIOS_BANKSTICK_Write(0x7000 + (bankpin << 9) + 3, (found_event.event_type << 4) | found_event.bankstick);
                    button_event_map[bankpin + 144] = bankpin_map_to_event_entry;
                }
                MIOS_BANKSTICK_CtrlSet(current_bankstick); // revert to programmed bankstick
                MIOS_AIN_DeadbandSet(AIN_DEADBAND);
                program_mode = EXIT_SETUP;  // Go to Exit program mode
            }
        }
        else if (program_mode >= PATCH_SET_RELAY1 && program_mode <= PATCH_SET_RELAY8)
        {
            // do nothing

        }
        else if (program_mode >= PATCH_SET_EVENT_BANKSTICK && program_mode <= PATCH_SET_EVENT_VALUE)
        {
            program_mode == EVENT_SELECT;
        }
        else
        {
            current_bankstick = MIOS_BANKSTICK_CtrlGet();
            if (MIOS_BANKSTICK_Read(0x0020 + (bankpin_map_to_event_entry * 20) + 0x12) == PROGRAM_CHANGE || MIOS_BANKSTICK_Read(0x0020 + (bankpin_map_to_event_entry * 20) + 0x12) == ON_OFF_ONLY)
            {
                MIOS_BANKSTICK_CtrlSet(0);
                MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 1, 0x00);
                MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 2, 0x7f);
            }
            else
            {
                MIOS_BANKSTICK_CtrlSet(0);
                MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 1, found_event.low_value);
                MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 2, found_event.high_value);
            }
            MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 0, bankpin_map_to_event_entry);
            MIOS_BANKSTICK_Write(0x6900 + (bankpin << 2) + 3, (found_event.button_type << 7) | (found_event.event_type << 4) | found_event.bankstick );

            button_event_map[bankpin] = bankpin_map_to_event_entry;
            button_event_typeAndBankstick[bankpin] = (found_event.button_type << 7) | (found_event.event_type << 4) | found_event.bankstick;
            MIOS_BANKSTICK_CtrlSet(current_bankstick); // revert to programmed bankstick
            MIOS_AIN_DeadbandSet(AIN_DEADBAND);
            program_mode = EXIT_SETUP;  // Go to Exit program mode
        }
        app_flags.DISPLAY_UPDATE_REQ = 1;

    }


    else if (program_mode == NONE)    // Enter programing mode
    {
        MIOS_BANKSTICK_CtrlSet(0);
        if (bankpin < 143)
        {
            found_event.bankstick = (MIOS_BANKSTICK_Read(0x6900 + (bankpin * 4) + 3) << 1) >> 1;
        }
        else
        {
            if (AIN_fixed[bankpin - 144] == 0)
            {
                AIN_map_offset = bankpin << 9;
                found_event.bankstick = (MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + (current_programchange_param1[0] << 2) + 3)) & 0x07;
            }
            else
            {
                found_event.bankstick = MIOS_BANKSTICK_Read(0x7000 + (AIN_map_offset) + 3) & 0x07;
            }
        }

        //if (bankstick_channel[bankstick_select] == 255) {
        //  bankstick_select = 0;
        //}

        MIOS_BANKSTICK_CtrlSet(found_event.bankstick);
        channel = bankstick_channel[found_event.bankstick];
        MIOS_AIN_DeadbandSet(5);
        program_mode = ENTER_SETUP;  // in the display handler, program mode is set to 2
    }
    else   // return to normal operation
    {
        MIOS_AIN_DeadbandSet(7);  // Back to normal mode
        program_mode = NONE;
    }
    MIOS_LCD_MessageStop();
    app_flags.DISPLAY_UPDATE_REQ = 1;
}

////////////////////////////////////////////////////////////////////
// This function is called by notify AIN and DIN in main.c
// It transmits a midi message
////////////////////////////////////////////////////////////////////

void MIDI_message_TX(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2, unsigned char channel)
{ // Sends a midi event based on the found_event variables
    evnt0_MSB = evnt0 >> 4; // These 2 lines force the channel to 0
    evnt0_MSB = evnt0_MSB << 4;
    evnt0_LSB = channel; // channel is controlled by bankstick

    MIOS_MIDI_TxBufferPut(evnt0_MSB + evnt0_LSB);
    MIOS_MIDI_TxBufferPut(evnt1);
    if (evnt0 != 0xc0)
    {
        MIOS_MIDI_TxBufferPut(evnt2); // Don't send param2 if program change event
    }
}


////////////////////////////////////////////////////////////////////
// These function is called in main.c
// They control the LED indicators
//
////////////////////////////////////////////////////////////////////
void set_led_indicators(unsigned char bank)
{
#if PEDALBOARD
    command_numbers_in_this_bank = DIN_BANKED_BUTTONS;
    which_bank = 1;
    count_commands = 0;
    current_bankstick = MIOS_BANKSTICK_CtrlGet(); // This will later be used to reset the bankstick to it's current #

    // set the dout's based on button_bankpin_value
    for (i=0; i<16 + 128; i++)
    {

        MIOS_BANKSTICK_CtrlSet(0);

        if (i<DIN_FIXED_BUTTONS)
        {
            if (button_bankpin_value[i] == MIOS_BANKSTICK_Read((0x6900 + (i << 2)) + 2))
            {
                MIOS_DOUT_PinSet(i, 1);
            }
            else
            {
                MIOS_DOUT_PinSet(i, 0);
            }
        }
        else if (i > 15)   // is a banked button
        {
            count_commands++;
            if (count_commands == command_numbers_in_this_bank + 1)
            {
                command_numbers_in_this_bank = command_numbers_in_this_bank + DIN_BANKED_BUTTONS;
                which_bank++;
            }
            if (which_bank == bank)
            {
                pin_hlp = ((bank * DIN_BANKED_BUTTONS) - DIN_BANKED_BUTTONS - (i - 16)); // could also use count_commands instead of i - 16
                pin_hlp = 0 - pin_hlp;
                if (button_bankpin_value[i] == MIOS_BANKSTICK_Read((0x6900 + (i << 2)) + 2))
                {
                    MIOS_DOUT_PinSet(pin_hlp + DIN_FIXED_BUTTONS, 1);
                }
                else
                {
                    MIOS_DOUT_PinSet(pin_hlp + DIN_FIXED_BUTTONS, 0);
                }
            }
        }
    } // for loop
    MIOS_BANKSTICK_CtrlSet(current_bankstick); // Reset to current bankstick
#endif // PEDALBOARD
}


/////////////////////////////////////////////////////////////////////////////
// This is an assembly optimized function which scales a 7bit value between
// a minimum and maximum value... Thanks TK
/////////////////////////////////////////////////////////////////////////////
unsigned char Scale_7bit(unsigned char value, unsigned char min, unsigned char max)
{
    // scaled value is (<8-bit random> * ) >> 8
    PRODL = value << 1; // 8bit value

    if (min > max)
    {
        PRODH = min-max+1;  // range
    }
    else
    {
        PRODH = max-min+1;  // range
    }

    __asm
    movf _PRODL, W
    mulwf _PRODH, 0
    __endasm;


    if (min > max)
    {
        return max + PRODH;
    }
    else
    {
        return min + PRODH;
    }
}


/////////////////////////////////////////////////////////////////////////////
// This is an assembly optimized function which scales a 7bit value between
// a minimum and maximum value... Thanks TK
/////////////////////////////////////////////////////////////////////////////
void set_value_related_buttons(unsigned char event, unsigned char type, unsigned char bankstick, unsigned char value)
{
    current_bankstick_B = MIOS_BANKSTICK_CtrlGet();
    MIOS_BANKSTICK_CtrlSet(0);
    for (l=0;l<144;l++)
    {
        if (event == button_event_map[l] && current_bankstick_B == (button_event_typeAndBankstick[l] & 0x07) && type == (button_event_typeAndBankstick[l] & 0x70) >> 4)
        {
            button_bankpin_value[l] = value;
        }
    }
    MIOS_BANKSTICK_CtrlSet(current_bankstick_B);
}

/////////////////////////////////////////////////////////////////////////////
// This is used by run_patch to send the midi events and update the leds
/////////////////////////////////////////////////////////////////////////////
void run_event(void)
{
    MIOS_BANKSTICK_CtrlSet(found_event.bankstick);

    channel = bankstick_channel[found_event.bankstick];
    found_event.status = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 16);
    found_event.param1 = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 17);
    found_event.event_handler = MIOS_BANKSTICK_Read(0x0020 + (found_event.entry * 20) + 18);


    if (found_event.event_type == STAT_CONTROL)
    {
        found_event.status = 0xb0;
        if ( found_event.event_handler == TAP_TEMPO )  // is tap tempo button
        {
            tap_tempo_handler();
            MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        }

        else if ( found_event.event_handler == ON_OFF_ONLY )   // toggle button
        {
            MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
            set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
        }



        else if ( found_event.event_handler >= METER_ONLY )   // Usually pedal functions
        {
            MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
            set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);

        }
    }
    else if (found_event.event_type == STAT_PROGRAM)
    {
        found_event.status = 0xc0;
        // set the program change indicator
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        program_change_handler();
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);

    }
    else if (found_event.event_type == STAT_NOTE_OFF)
    {
        found_event.status = 0x80;
        found_event.param2 = 0x00;
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    }
    else if (found_event.event_type == STAT_NOTE_ON)
    {
        found_event.status = 0x90;
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    }
    else if (found_event.event_type == STAT_AFTER_TOUCH)
    {
        found_event.status = 0xa0;
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    }
    else if (found_event.event_type == STAT_PRESSURE)
    {
        found_event.status = 0xd0;
        found_event.param1 = found_event.param2;
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    }
    else if (found_event.event_type == STAT_PITCH_BEND)
    {
        found_event.status = 0xe0;
        MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
        set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    }
    MIOS_BANKSTICK_CtrlSet(0);
}

/////////////////////////////////////////////////////////////////////////////
// run_patch sends the 32 midi messages and sets the relays in a patch,
/////////////////////////////////////////////////////////////////////////////
void run_patch(unsigned char patch)
{
    current_event_type = found_event.event_type;

    patch_entry == found_event.entry; // found_event.entry gets over written, patch_entry is later used to set found_event.entry back for the display handlers
    for (i=0;i<32;i++)
    {
        MIOS_BANKSTICK_CtrlSet(0);
        if (MIOS_BANKSTICK_Read(0x0020 + (0x92 * patch) + 16 + (i * 4) + 3) >> 7 == ENABLED)
        {
            found_event.bankstick = MIOS_BANKSTICK_Read(0x0020 + (0x92 * patch) + 16 + (i * 4) + 3) & 0x07;
            fill_found_control_info(0x0020, patch, 0x92, 16, i, 4);
            found_event.param2 = found_event.high_value;
            run_event();
        }
    }

    MIOS_BANKSTICK_CtrlSet(0);
    for (i=0;i<8;i++)     // set relays (untested)
    {
        dout_set_hlp = MIOS_BANKSTICK_Read(0x0020 + (0x92 * patch) + 0x91) << i;
        dout_set_hlp = dout_set_hlp >> 7;
        if (dout_set_hlp == ENABLED)
        {
		
					
					
            dout_set = MIOS_BANKSTICK_Read(0x0020 + (0x92 * patch) + 0x90) << i;
            dout_set = dout_set >> 7;
			MIOS_DOUT_PinSet(relay_led_dout_start_pin + i, dout_set);
			if (relay_polarity[i] == 0) {
				dout_set = ~dout_set; 
			}
            MIOS_DOUT_PinSet(relay_dout_start_pin + i, dout_set);
            
			

        }
    }

    found_event.entry = patch_entry;
    found_event.bankstick = 0;
    found_event.event_type = current_event_type;
}


/////////////////////////////////////////////////////////////////////////////
// The following are commonly used sequences of code.
// Placed here to help bring code size down.
/////////////////////////////////////////////////////////////////////////////
void program_change_handler(void)
{
    event_entry_previous_programchange[found_event.bankstick] = event_entry_current_programchange[found_event.bankstick];
    set_value_related_buttons(event_entry_previous_programchange[found_event.bankstick], STAT_PROGRAM, found_event.bankstick, 0x00); //turn led off
    event_entry_current_programchange[found_event.bankstick] = found_event.entry;
    set_value_related_buttons(event_entry_current_programchange[found_event.bankstick], STAT_PROGRAM, found_event.bankstick, 0x7f); //turn new led on
    current_programchange_param1[found_event.bankstick] = found_event.entry; // program change number in hex
    found_event.param2 = 0x7f;
    found_event.status = 0xc0;
}


void fill_found_control_info(unsigned short int offset, unsigned char master_entry, unsigned short int master_entry_length, unsigned char skip_name_bytes, unsigned char sub_entry, unsigned char sub_entry_length)
{
    if (program_mode == PATCH_EVENT_SELECT || program_mode == PATCH_SET_EVENT_BANKSTICK || program_mode == PATCH_SET_EVENT_TYPE || program_mode == PATCH_SET_EVENT_VALUE)
    {
        patch_entry = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 0);
        patch_event_high_value = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 2);
        patch_event_enabled = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) >> 7;
        patch_event_type = (MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) & 0x70) >> 4;
        patch_event_bankstick = (MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) & 0x07);
    }
    else
    {
        found_event.entry = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 0);
        found_event.low_value = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 1);
        found_event.high_value = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 2);
        found_event.button_type = MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) >> 7;
        found_event.event_type = (MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) & 0x70) >> 4;
        found_event.bankstick = (MIOS_BANKSTICK_Read(offset + (master_entry * master_entry_length) + skip_name_bytes + (sub_entry * sub_entry_length) + 3) & 0x07);
    }
}

void send_midi_and_update(void)
{
    MIDI_message_TX(found_event.status, found_event.param1, found_event.param2, channel);
	set_value_related_buttons(found_event.entry, found_event.event_type, found_event.bankstick, found_event.param2);
    set_led_indicators(bank);
    display_temp();
}

unsigned char getHexFromStat(unsigned char stat)
{
    return status_hex[stat];
}

void suspend_LCD(void)
{
    app_flags.PCRX = 1;
    suspend_lcd_counter = 0;
}

void resume_LCD(void)
{
    app_flags.PCRX = 0;
}

void correct_program_mode_selection(void)
{
    if (program_mode == PATCH_SET_EVENT_TYPE)
    {
        if (patch_event_type == STAT_PROGRAM || patch_event_type == STAT_PROGRAM)
        {
            // do nothing
        }
        else
        {
            patch_event_type = STAT_PROGRAM;
        }
    }
    else if (program_mode == SET_EVENT_TYPE)
    {
        if (found_event.bankstick == 0) // master bankstick
        {
            if (found_event.event_type == STAT_SPECIAL || found_event.event_type == STAT_PROGRAM)
            {
                //do nothing
            }
            else
            {
                found_event.event_type = STAT_PROGRAM;
            }
        }
        else // devive bankstick
        {

            if (previous_input_type == AIN)   // is a pedal
            {
                if (found_event.event_type == STAT_CONTROL || found_event.event_type == STAT_AFTER_TOUCH || found_event.event_type == STAT_PRESSURE || found_event.event_type == STAT_PITCH_BEND)
                {
                    // do nothing
                }
                else
                {
                    found_event.event_type = STAT_CONTROL;
                }

            }
            else
            {
                if (found_event.event_type == STAT_SPECIAL) found_event.event_type = STAT_CONTROL;
            }
        }
    }
    else if (program_mode == EVENT_SELECT)
    {
        if (found_event.bankstick == 0)
        {
            if (found_event.event_type == STAT_SPECIAL && bankpin_map_to_event_entry > 9)
            {
                bankpin_map_to_event_entry = 0;
            }
        }
    }
}



