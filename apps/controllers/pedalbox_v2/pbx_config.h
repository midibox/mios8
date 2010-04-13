/*
 * MIOS Pedal Box / Pedal Board - pbx_config.h
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
// Configuration Parameters
//
///////////////////////////////////////////////////////////////////////////

/* General Settings */
#define   PEDALBOARD             1  // 1 = Pedal Board mode    0 = Pedal Box mode
#define   ENABLE_MIDI_IN         1  // 1 = yes      0 = no

#define   MIDI_MERGER            1  // 0 = off   1 = on
#define   MIDI_MERGER_DISABLE_MASTER 1  // 0 = disabled  1 = enabled - If set will disable forwarding of events on the master bankstick channel

								   
/* Pedal Settings */
#define   AIN_DEADBAND           7 // 7 for 7-bit midi is best
#define   AIN_NUMBER_INPUTS      8 // 1 - 8 - number of pots connected



/* Pedal Board Buttons */
#define   DIN_DEBOUNCE_VALUE     20 // debounce value

// FIXED BUTTONS MUST CONNECT FIRST
#define   DIN_FIXED_BUTTONS      16   // 0-16 buttons that always stay the same, regardless of bank

// BANKED BUTTONS MUST BE CONNECTED AFTER FIXED BUTTONS
#define   DIN_BANKED_BUTTONS     16   // 1-16 buttons that change with bank. 1 banked button will cause display errors when bank is over 99

 //THESE SPECIAL FUNCTION BUTTONS MUST BE CONNECTED AFTER THE BANKED BUTTONS!!!!!!!
#define   DIN_BANK_UP            32   // 4 - 33, pin number of button used to bank up
#define   DIN_BANK_DOWN          33   // 4 - 33, pin number of button used to bank down

#define  FIRST_PEDALSWAP_DIN_PIN 34  // 6 - 33, First PedalSwap DIN pin (others will be next higher sequentially)
									 // Must be higher than Bank Up and Down
									 // Be careful not to set the same in the space of LED Digits or Relays
									 // Best place is next to the bank up/down pins
#define  NUM_PEDALSWAP_BUTTONS   6	 // 0-6, Number of PedalSwap Buttons connected
									 // if you wish to have access to all 6 via MIDI (with less than 6 buttons connected
									 // - please set to 6 and leave the DIN pins free


 // THESE SPECIAL BUTTONS CAN BE ANY BUTTON CONNECTED, THEY ARE DUAL USE. with the exception of Bank Up / Down
#define   EVENT_SETUP_DIN_PIN    0   // 0 - 33, pin number of button used to enter event setup
#define   GLOBAL_SETUP_DIN_PIN   1   // 0 - 33, pin number of button used to enter AIN setup


 /* Relay setup */
#define   RELAY_SHIFT_REGISTER     6 // 0 - 7 Relay's Shift Register
#define   RELAY_LED_SHIFT_REGISTER 7 // 0 - 7 Relay's LD indicator Shift Register


/* LED setup */
#define   USE_LED_INDICATORS       1  // Enable LED indicators connected 1 = Enabled 0 = Disabled
#define   ENABLE_BANK_LEDS         1  // Enables LED's on bank up and down buttons

/* LED Digit Setup */
#define   DIGITS_CONNECTED         1  // 0-2 How many LED digits are connected (Default = 1, relay SR will nedd to be moved you want 2 digits)
#define   DOUT_DIGITS_SR           5  // What SR do the digits start from.
                                      // 2nd digit will be on the next SR

                                    ////////////////////////////////////
                                    // IF NO LED DIGITS ARE CONNECTED //
                                    // A 2x20 LCD DISPLAY CAN BE USED //
                                    // WITH THE BANK DISPLAYED ON THE //
                                    // LAST 4 CHARACTERS              //
                                    ////////////////////////////////////

