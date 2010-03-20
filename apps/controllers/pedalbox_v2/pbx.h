/*
 * MIOS Pedal Box / Pedal Board - pbx.h
 * v2.5beta3 - April 2009
 * ==========================================================================
 *
 *  Copyright (C) 2009  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////


void tap_tempo_handler(void);
void tap_tempo_start(void);
void tap_tempo_restart(void);
void tap_tempo_stop(void);
void program_mode_handler(void);
void MIDI_message_TX(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2, unsigned char channel);
void set_led_indicators(unsigned char bank);
unsigned char Scale_7bit(unsigned char value, unsigned char min, unsigned char max);
void set_value_related_buttons(unsigned char event, unsigned char type, unsigned char bankstick, unsigned char value);
void run_event(void);
void run_patch(unsigned char entry);
void program_change_handler(void);
void fill_found_control_info(unsigned short int offset, unsigned char master_entry, unsigned short int master_entry_length, unsigned char skip_name_bytes, unsigned char sub_entry, unsigned char sub_entry_length);
void send_midi_and_update(void);
void correct_program_mode_selection(void);
unsigned char getHexFromStat(unsigned char stat);

void suspend_LCD(void);
void resume_LCD(void);
void print_relay_state(unsigned char relay_num, unsigned char relay_state);


// LCD
extern void display_handler(void);
extern void display_temp(void);
void display_meter(void);


