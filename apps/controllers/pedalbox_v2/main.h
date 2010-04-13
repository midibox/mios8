/*
 * MIOS Pedal Box / Pedal Board - main.h
 * v2.6rc1 - April 2010
 * ==========================================================================
 *
 *  Copyright (C) 2010  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */


 ///////////////////////////////////////////////////////////////////////////
 // Definitions
 //
 ///////////////////////////////////////////////////////////////////////////

typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned DISPLAY_UPDATE_REQ:1;  // requests a display update
    unsigned TAP_FLAG:1;            // Tells tap tempo timer to start/stop
    unsigned SPLASH:1;              // Used to display splash screen at startup
    unsigned PCRX:1;                // Received program change
	unsigned PEDALSWAP_TRIGGERED:1; // PedalSwap has just been triggered
	unsigned LED_UPDATE_REQ:1;      // requests LED update
  };
} app_flags_t;



typedef struct found_event_format
{
  unsigned char name[17];
  unsigned char status;
  unsigned char param1;
  unsigned char event_handler; // defines the midi function type (read more in pbx_tables.c)
  unsigned char param2;
  unsigned short int entry; // entry number in event map
  unsigned char bankstick;
  unsigned char low_value;
  unsigned char high_value;
  unsigned char button_type;
  unsigned char event_type;
} found_event_t;





const unsigned char status_hex[7]= {
0x80,
0x90,
0xa0,
0xb0,
0xc0,
0xd0,
0xe0
};



///////////////////////////////////////////////////////////////////////////
// Variables
//
///////////////////////////////////////////////////////////////////////////

// status of application
app_flags_t app_flags;

// place holder for found event // used by application for incoming and outgoing midi events
found_event_t found_event;


unsigned char bankpin; // fixed buttons = pin, For banked buttons = (pin + (bank * DIN_BANKED_BUTTONS)) - DIN_BANKED_BUTTONS - DIN_FIXED_BUTTONS + 16.
unsigned char bankpin_map_to_event_entry; // used in programing mode to assign an event_map entry to the current pin
unsigned char previous_bankpin; // as above but for the previous value
unsigned char previous_event_entry;
unsigned char button_bankpin_value[152]; // Stores param2 of bankpin for bankpin_event_entry
unsigned char button_bankpin_on_value[144];
unsigned char button_event_map[152]; // Stores bankpin_event_entry of button
unsigned char button_event_typeAndBankstick[152];
//unsigned char relay_invert[8]; // holds whether to invert the relay disaply of not

unsigned char channel; // current channel to be used, or recieved
unsigned char bank; // current bank

unsigned char bankstick_channel[8]; // holds the channel asigned to each bankstick
unsigned char current_bankstick; // used to hold the current bankstick# when it has been changed for processing
unsigned char current_bankstick_B; // same as above - used to prevent current_bankstick variable being stolen (quick fix hack)
unsigned char current_bankstick_C; // same as above - used to prevent current_bankstick variable being stolen (quick fix hack)
unsigned char bankstick_select; // used in programnming mode

unsigned char previous_input_type; // what was last used - din, ain, or midi

unsigned char event_entry_current_programchange[8]; // event_entry of current program change for each device
unsigned char event_entry_previous_programchange[8]; // event_entry of previous program change for each device
unsigned char current_programchange_param1[8]; // current program change value for each bankstick
unsigned char current_pedalswap_patch; // currently activated pedalswap patch number (130 = off)
unsigned char found_patchnum;
unsigned char active_pedalswap;

unsigned char program_mode; // current program mode for assigning settings

unsigned short int bankstick_offset; // used sometimes when reading and writing banksrick
unsigned short int AIN_map_offset; // used sometimes when reading and writing banksrick *

unsigned char param2_value_reduced; //used by visualization

//used in 'for' octave and as temp variables for calculations
unsigned char t;
unsigned char l;
unsigned short int i;
unsigned char mask; // used to count in opposite direction to the loop

//used for bpm calculations
unsigned short int bpm;
unsigned short int bpmcountms = 0;
unsigned char countms;

//Used by midi out functions
unsigned char evnt0_MSB; // Status TYPE
unsigned char evnt0_LSB; // Status CHANNEL

//Used by midi in functions
unsigned char previous_evnt0; // previous status byte recieved
unsigned char previous_evnt1; // previous param1 byte recieved

unsigned short int count; // a counter used in various places
unsigned char AIN_fixed[8]; // gets values form bankstick. Tells whether or not AIN will change with program changes

//Used to hold starting pin numbers
unsigned char relay_dout_start_pin;
unsigned char relay_led_dout_start_pin;
unsigned char digits_start_pin;

// Used for calculationg indicator led status
unsigned char dout_set_hlp;
unsigned char dout_set;
unsigned char led_set;
unsigned char val_set;

// Gig Control
unsigned char current_cue;
unsigned char current_cue_list;
unsigned char cue_entry; // hold patch number for the current cue

// Rig Control
unsigned char patch_select; // selected patch
unsigned char patch_event;
unsigned char patch_entry;
unsigned char patch_event_bankstick;
unsigned char patch_event_high_value;
unsigned char patch_event_type;
unsigned char patch_event_enabled;
unsigned char patch_sub; // indicated msg number when programming patchs

unsigned char relay_enabled[9];  // first 8 are used to store 1 or 0, the last is a bitfield for all
unsigned char relay_on_off[9];
unsigned char current_relay;

// Bank calculations
char pin_hlp; // holds the physical pin number of a banked entry (calculated with negative values)
unsigned char count_commands; // is the current Din_map entry number - 16
unsigned char which_bank; // bank number of the current DIN_map entry
unsigned char command_numbers_in_this_bank; // Flag holding the next Din_map entry that would be in the next bank
unsigned char event_help; // helps in calculations

// Error Checking
unsigned char error; // holds current error
unsigned char test; // test variable
unsigned char test_version; // holds version of bankstick file for comparison to current software version

unsigned char note;


unsigned char current_event_type; // hold a restore point for found_event.event_type

unsigned char ain_mf[8];
unsigned char debug;

unsigned char byte_low_nibble; // holds channel number for MPROC_Recieved_byte

unsigned int suspend_lcd_counter;
unsigned int flash_counter;

unsigned char eeprom_data;
//unsigned char debug[8];

void AIN_handler(unsigned char pin, unsigned char pin_value);

///////////////////////////////////////////////////////////////////////////
// Definitions
//
///////////////////////////////////////////////////////////////////////////

// Program Mode
#define NONE          0
#define ENTER_SETUP   1
#define SET_EVENT_BANKSTICK  2
#define SET_EVENT_TYPE 3
#define EVENT_SELECT  4
#define SET_MIN       5
#define SET_MAX       6
#define SET_AIN_FIXED 7
#define EXIT_SETUP    8
#define SET_BANKSTICK1_CH 9
#define SET_BANKSTICK2_CH 10
#define SET_BANKSTICK3_CH 11
#define SET_BANKSTICK4_CH 12
#define SET_BANKSTICK5_CH 13
#define SET_BANKSTICK6_CH 14
#define SET_BANKSTICK7_CH 15
#define SET_BANKSTICK8_CH 16
#define PATCH_SET_EVENT_BANKSTICK 17
#define PATCH_SET_EVENT_TYPE 18
#define PATCH_EVENT_SELECT 19
#define PATCH_SET_EVENT_MIN 20
#define PATCH_SET_EVENT_VALUE 21
#define CUE_SELECT_LIST  22
#define CUE_PATCH_SELECT 23
#define PATCH_SET_RELAY1 24
#define PATCH_SET_RELAY2 25
#define PATCH_SET_RELAY3 26
#define PATCH_SET_RELAY4 27
#define PATCH_SET_RELAY5 28
#define PATCH_SET_RELAY6 29
#define PATCH_SET_RELAY7 30
#define PATCH_SET_RELAY8 31


// Status Definition
#define STAT_NOTE_OFF    0
#define STAT_NOTE_ON     1
#define STAT_AFTER_TOUCH 2
#define STAT_CONTROL     3
#define STAT_PROGRAM     4
#define STAT_PRESSURE    5
#define STAT_PITCH_BEND  6
#define STAT_SPECIAL     7



// Event Handlers
#define PROGRAM_CHANGE   0
#define ON_OFF_ONLY      1
#define TAP_TEMPO        2
#define METER_ONLY       3
#define METER_OFF        4
#define METER_ON_OFF     5
#define METER_PAN        6
#define VALUE            7
#define USE_MAP_1        8
#define USE_MAP_2        9
#define USE_MAP_3        10
#define USE_MAP_4        11
#define USE_MAP_5        12
#define USE_MAP_6        13
#define USE_MAP_7        14
#define USE_MAP_8        15
#define USE_MAP_9        16
#define USE_MAP_10       17
// INTERNAL ONLY
#define NOTE_OFF         18
#define NOTE_ON          19
#define AFTER_TOUCH      20
#define PRESSURE         21
#define PITCH_BEND       22

// Input Types
#define AIN        1
#define DIN        2
#define MIDI       3

// Bankstick Type
#define DEVICE           0
#define PATCH            1

// Switch Type
#define SWITCH           0
#define MOMENT           1

// Misc
#define OFF              0
#define ON               1

#define DISABLED         0
#define ENABLED          1

#define FALSE            0
#define TRUE             1



// Versioning Info
#define   BS_VERSION               22
#define   BS_MAGIC_NUMBER_1        0x50
#define   BS_MAGIC_NUMBER_2        0x09




