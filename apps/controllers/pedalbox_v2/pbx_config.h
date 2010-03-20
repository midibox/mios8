/*
 * MIOS Pedal Box / Pedal Board - pbx_config.h
 * v2.5beta3 - April 2009
 * ==========================================================================
 *
 *  Copyright (C) 2009  Mick Crozier - mick@durisian.com
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

#define   NUMBER_OF_SRIO         4 // 1-16  number of shift registers connected (count DIN or DOUT, whichever has more)

/* Pedal Settings */
#define   AIN_DEADBAND           7 // 7 for 7-bit midi is best
#define   AIN_NUMBER_INPUTS      8 // 1 - 8 - number of pots connected



/* Pedal Board Buttons */
#define   DIN_DEBOUNCE_VALUE     20 // debounce value

// FIXED BUTTONS MUST CONNECT FIRST
#define   DIN_FIXED_BUTTONS      4   // 0-16 buttons that always stay the same, regardless of bank

// BANKED BUTTONS MUST BE CONNECTED AFTER FIXED BUTTONS
#define   DIN_BANKED_BUTTONS     4   // 0-16 buttons that change with bank. 1 banked button will cause display errors when bank is over 99

 //THESE SPECIAL FUNCTION BUTTONS MUST BE CONNECTED AFTER THE BANKED BUTTONS!!!!!!!
#define   DIN_BANK_UP            6   // 4 - 33, pin number of button used to bank up
#define   DIN_BANK_DOWN          7   // 4 - 33, pin number of button used to bank down

 // THESE SPECIAL BUTTONS CAN BE ANY BUTTON CONNECTED, THEY ARE DUAL USE.
#define   EVENT_SETUP_DIN_PIN    0   // 0 - 33, pin number of button used to enter event setup
#define   GLOBAL_SETUP_DIN_PIN   1   // 0 - 33, pin number of button used to enter AIN setup


 /* Relay setup */
#define   RELAY_SHIFT_REGISTER     6 // 0 - 7 Relay's Shift Register
#define   RELAY_LED_SHIFT_REGISTER 7 // 0 - 7 Relay's LD indicator Shift Register


/* LED setup */
#define   USE_LED_INDICATORS     1  // Enable LED indicators connected 1 = Enabled 0 = Disabled
#define   ENABLE_BANK_LEDS       0  // Enables LED's on bank up and down buttons

/* LED Digit Setup */
#define   DIGITS_CONNECTED       0  // 0-2 How many LED digits are connected
#define   DOUT_DIGITS_SR         0  // What SR do the digits start from.
                                    // 2nd digit will be on the next SR

                                    ////////////////////////////////////
                                    // IF NO LED DIGITS ARE CONNECTED //
                                    // A 2x20 LCD DISPLAY CAN BE USED //
                                    // WITH THE BANK DISPLAYED ON THE //
                                    // LAST 4 CHARACTERS              //
                                    ////////////////////////////////////

