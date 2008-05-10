// $Id$
/*
 * MIDIbox MM V2 (C version)
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _MAIN_H
#define _MAIN_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// define the MIDI channel to which this MM listens and transmits here:
// (Allowed values: 1-16)
#define MM_MIDI_CHANNEL		1

// debounce counter (see the function description of MIOS_SRIO_DebounceSet)
// Use 0 for high-quality buttons, use higher values for low-quality buttons
#define SRIO_DEBOUNCE_CTR	0

// motordriver settings
#define ENABLE_MOTORDRIVER	1	// if 0, MF module will not be enabled
#define NUMBER_MOTORFADERS	8	// number of motorfaders
#define MOTORFADERS_DEADBAND	3	// so called "deadband" of motorfaders
#define AIN_DEADBAND		3	// so called "deadband" of ADC
#define MOTORFADERS_PWM_DUTY_UP	  0x01	// PWM duty cycle for upward moves   (see http://www.ucapps.de/mbhp_mf.html)
#define MOTORFADERS_PWM_DUTY_DOWN 0x01  // PWM duty cycle for downward moves (see http://www.ucapps.de/mbhp_mf.html)
#define MOTORFADERS_PWM_PERIOD    0x03	// PWM period                        (see http://www.ucapps.de/mbhp_mf.html)

// following setting configures the touch sensor behaviour. The touch sensors of the motorfaders
// have to be assigned to the DIN pins in mm_dio_tables.c - the appr. IDs are:
// ID_FADER_TOUCH_CHN1, ID_FADER_TOUCH_CHN2, ... ID_FADER_TOUCH_CHN8 (8 sensors)
// If the master fader option is used, the ID is ID_FADER_TOUCH_MASTER
// 
// TOUCH_SENSOR_MODE 0: touch sensor events (pressed/depressed) are only forwarded to the host program
//                      like specified in the motormix specification (MIDI events)
// TOUCH_SENSOR_MODE 1: like mode 0, but additionaly the motors are suspended by MIOS, so that they
//                      are not moved so long as the touch sensor is pressed
// TOUCH_SENSOR_MODE 2: like mode 0+1, additionally no fader move will be reported to the host
//                      application (no MIDI event) so long the touch sensor is *not* pressed
// 
// Mode "1" selected by default to avoid circular troubleshooting requests in the MIDIbox forum from people
// who don't read this information before starting the application.
// Mode "2" should be the prefered setting if your touch sensors are working properly
#define TOUCH_SENSOR_MODE	1

#define NUMBER_SHIFTREGISTERS	16	// length of shift register chain (max value: 16 == 128 DIN/DOUT pins)
#define SRIO_UPDATE_FREQUENCY	1	// we are using rotary encoders: use 1 ms!
#define TOUCHSENSOR_SENSITIVITY 3	// for the motorfader touchsensors

// define the encoder speed modes and dividers here
#define ENC_VPOT_SPEED_MODE          2  // 0 = slow, 1 = normal, 2 = fast
#define ENC_VPOT_SPEED_DIVIDER       2  // see description about MIOS_ENC_SpeedSet

#define ENC_JOGWHEEL_SPEED_MODE      2  // 0 = slow, 1 = normal, 2 = fast
#define ENC_JOGWHEEL_SPEED_DIVIDER   1  // see description about MIOS_ENC_SpeedSet

// used by mm_dio.c
// the schematic can be found under http://www.ucapps.de/midibox_lc/midibox_lc_ledrings_meters.pdf
// NOTE: LEDrings are normaly not supported by the Motormix protocol --- please read the ChangeLog!!!
// NOTE: Meters are not supported by the Motormix protocol!!!
#define LEDRINGS_ENABLED	1	// if 1, ledrings are enabled
// NOTE2: the shift registers are counted from zero here, means: 0 is the first shift register, 1 the second...
#define LEDRINGS_SR_CATHODES	8	// shift register with cathodes of the 8 LED rings
#define LEDRINGS_SR_ANODES_1	10	// first shift register with anodes of the 8 LED rings
#define LEDRINGS_SR_ANODES_2	11	// second shift register with anodes of the 8 LED rings


// used by mm_leddigits.c
// define the two shift registers to which the status digits are connected here.
// the wiring is similar to http://www.ucapps.de/midibox_lc/midibox_lc_leddigits.pdf
// BUT: there are no select lines! The anode has to be connected directly to +5V
#define LEDDIGITS_SR_SEGMENT_A	12	// the shift register of the first digit (13th in the chain)
#define LEDDIGITS_SR_SEGMENT_B	13	// the shift register of the second digit (14th in the chain)


// the GPC (General Purpose Controller) feature can be activated by using ID_MBMM_*GPC* buttons
// up to 128 midi events are defined in mios_wrapper/mios_tables.inc
// the lables are defined in mm_gpc_lables.c
// optionally a MIDI event will be sent on any SFB action which notifies if GPC is enabled or not
#define GPC_BUTTON_SENDS_MIDI_EVENT	0	// enables the "send GPC status" feature

#define GPC_BUTTON_ON_EVNT0		0x9f	// on event, byte #0
#define GPC_BUTTON_ON_EVNT1		0x3c	// on event, byte #1
#define GPC_BUTTON_ON_EVNT2		0x7f	// on event, byte #2

#define GPC_BUTTON_OFF_EVNT0		0x9f	// off event, byte #0
#define GPC_BUTTON_OFF_EVNT1		0x3c	// off event, byte #1
#define GPC_BUTTON_OFF_EVNT2		0x00	// off event, byte #2


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of MM emulation
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned PRINT_POINTERS:1; // if set: pointers are displayed at second LCD line
                               // if cleared: lower part of message is displayed at second LCD line
    unsigned LAYER_SEL:1;      // if cleared: button/led layer 0 active
                               // if set: button/led layer 1 active
    unsigned GPC_SEL:1;        // if cleared: motormix emulation mode
                               // if set: general purpose controller mode
    unsigned MSG_UPDATE_REQ:1; // if set, host/GPC message will be updated
    unsigned DISPLAY_INIT_REQ:1; // if set, the display will be completely initialized
    unsigned LED_UPDATE_REQ:1; // if set, the LEDs will be updated
    unsigned :1;
    unsigned :1;
  };
} mm_flags_t;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
extern mm_flags_t mm_flags;



#endif /* _MAIN_H */
