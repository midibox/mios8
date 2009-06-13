/*
 *  ACSensorizer.h
 *  ACSensorizer
 *
 *  Created by audiocommander on 15.07.06.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
 *
 *
 *			>>>  SEE README.TXT FOR VERSION HISTORY  <<<
 * 
 *
 * Examples on how to use this Class along with a Bankstick or an Interface (Hardware
 * HUI or MIDI/NRPN-Config) can be found in the main.h/.c files of the Sensorizer-
 * Project. You can select witch features should be compiled into the application.
 *
 * readme.txt contains further important in-depth informations, 
 * detailed schematics and listings for bankstick contents and configuration details.
 */

/*
 * Released under GNU General Public License
 * http://www.gnu.org/licenses/gpl.html
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation
 *
 * YOU ARE ALLOWED TO COPY AND CHANGE 
 * BUT YOU MUST RELEASE THE SOURCE TOO (UNDER GNU GPL) IF YOU RELEASE YOUR PRODUCT 
 * YOU ARE NOT ALLOWED TO USE IT WITHIN PROPRIETARY CLOSED-SOURCE PROJECTS
 */

#ifndef _ACSENSORIZER_H
#define _ACSENSORIZER_H


#include "main.h"

#include "ACToolbox.h"				// used for Division, Scale and Random Functions
#include "ACSensorizerDefines.h"	// for SENSORIZER_INTERFACE defines



// ***** DEFINES *****

// patch version control for patch/bankstick compatibility
#define SENSORIZER_BS_VERSION		0x04		// eg 0x02 for v0.2 => 0x20 for v2.0 => 0xF1 for v15.1

// general
#define SENSORIZER_SENSEMODE		1			// 0: efficient (bitshifting), 1: exact (division), default: 1
#define SENSORIZER_AUTOMODE_PEDAL	1			// 0: autosense when set, 1: autosense ONLY if pedal down, default: 1

// additional features
#define SENSORIZER_BANKSTICK_ENA	SENSORIZER_INTERFACE_BS	  // defined in ACSensorizerDefines.h, if bankstick enabled
#define SENSORIZER_HARM_ENA			SENSORIZER_INTERFACE_HARM // defined in ACSensorizerDefines.h, if harmonizer enabled
#define SENSORIZER_SYNC_ENA			SENSORIZER_INTERFACE_SYNC // defined in ACSensorizerDefines.h, if syncronizer enabled
// hidden properties
#define SENSORIZER_CC_ENA			1			// support for CH/CC properties, default: 1
#define SENSORIZER_AUTOSENSE_ENA	1			// autosense mode support, default: 1
// bankstick
#define BS_PATCH_NAME_ADDR			0x10
// EEPROM
#define EEPROM_START_ADDRESS		0x00
#define EEPROM_END_ADDRESS			0xFF		// 256 bytes available in PIC18F452 EEPROM (used to reload last patch)
// AIN
#define SENSOR_NUM					AIN_NUM		// number of sensor AINs, pin#0..pin#(AIN_NUM+1)
// syncronizer
#define SYNC_Q_MAX					12			// number of supported notelengths (eg. 1/4, 1/8, 1/16, 1/32)

// return values
#define SRETURN_ABORT				0xFF
#define SRETURN_AUTOSENSE			0xFE
#define SRETURN_RELEASEDETECT		0xFD

// settings - AUTOSENSE
#define AUTOSENSE_OFF				0x0
#define AUTOSENSE_MIN				0x1
#define AUTOSENSE_MAX				0x2
// settings - PEDALMODE
#define PEDAL_OFF					0x0
#define PEDAL_HOLD					0x1
#define PEDAL_PANIC					0x2
#define PEDAL_HOLD_PANIC			0x3
#define PEDAL_FILTER				0x4
#define PEDAL_FILTER_HOLD			0x5
#define PEDAL_FILTER_PANIC			0x6
#define PEDAL_FILTER_HOLD_PANIC		0x7


// HUI control via MIDI
#define HUIREMOTE_SENSORWHEEL		0x60
#define HUIREMOTE_MENUWHEEL			0x61
#define HUIREMOTE_DATAWHEEL_LEFT	0x62
#define HUIREMOTE_DATAWHEEL_RIGHT	0x63


// control types for NRPN access
// basic sensor settings			0x0_
#define NRPN_CTRL_ENABLED			0x00
#define NRPN_CTRL_PEDAL				0x01
#define NRPN_CTRL_AUTOSENSE			0x02
#define NRPN_CTRL_INVERT			0x03
#define NRPN_CTRL_RELEASE_DETECT	0x04
// sense settings					0x1_
#define NRPN_CTRL_SLOWDOWN			0x10
#define NRPN_CTRL_SMIN				0x11
#define NRPN_CTRL_SMAX				0x12
#define NRPN_CTRL_SFACTOR			0x13
// process settings					0x2_
#define NRPN_CTRL_SFROM				0x21
#define NRPN_CTRL_STO				0x22
// midi settings					0x7_
#define NRPN_CTRL_SCH				0x70
#define NRPN_CTRL_SCC				0x71




// ***** TYPEDEFS *****

typedef union {
	struct {
		unsigned	ALL				: 8;		// complete access
	};
	struct {
		unsigned	mode			: 1;		// 0x0: play, 0x1: setup (deprecated since v0.4 and higher)
		unsigned	pedalUp			: 1;		// 0x0: down, 0x1: up (necessary to update in main->DIN!)
		unsigned	resetMeter		: 1;		// resets sync_q_decr values
		unsigned	free			: 6;		//  <unused>
	};
} sensorizer_t;


typedef union {
	struct {
		unsigned	ALL				: 8;		// complete access
	};
	struct {
		unsigned	enabled			: 1;		// process AIN pin only if 1
		unsigned 	pedalMode		: 3;		// read only if pedal is pressed
		unsigned	autoSense		: 2;		// 0x0: OFF,	0x1: MIN,	0x2: MAX
		unsigned 	invert			: 1;		// if signal should be inverted (0 = 127 & vv.)
		unsigned	releaseDetect	: 1;		// sends 0 as value if((value< sMin) &! (lastValue==0))
	};
} sensor_t;






// ***** GLOBALS *****

// sensor config
extern volatile sensorizer_t		sensorizer;				// sensorizer configuration
extern volatile sensor_t			sensor[SENSOR_NUM];		// sensor configurations

// temp storages
extern volatile unsigned int		currentAIN_value[SENSOR_NUM];		// holds current AIN_value (for synced mode)
extern unsigned char				lastAIN_value[SENSOR_NUM];			// last sensed  7-bit value
extern unsigned int					lastAIN_10bitValue[SENSOR_NUM];		// last sensed 10-bit value

// midi prefs
#if SENSORIZER_CC_ENA
extern volatile unsigned char		sCH[SENSOR_NUM];		// send on Channel (0 to 15)
extern volatile unsigned char		sCC[SENSOR_NUM];		// send CC#
#endif
// sense slowdown
extern volatile unsigned char		slowdown[SENSOR_NUM];	// sense only every nth event (0 to 255)
// sense range
extern volatile unsigned int		sMin[SENSOR_NUM];		// 10-bit minimum sense (ignore below; => 7bit:   0)
extern volatile unsigned int		sMax[SENSOR_NUM];		// 10-bit maximum sense (ignore above; => 7bit: 127)
// expand signal
extern volatile unsigned char		sFactor[SENSOR_NUM];	// calibrated sensor factor: (max - min) / 127
															//   if somehow possible use 1, 2, 4, 8 or 16
															//   to use fastest bit-shifting algorithm
															//   otherwise a complex multi-cycle division occurs
extern volatile unsigned char		sFrom[SENSOR_NUM];		// scale to target min value (e.g. 0)
extern volatile unsigned char		sTo[SENSOR_NUM];		// scale to target max value (e.g. 127)


#if SENSORIZER_BANKSTICK_ENA
// bankstick patch
extern volatile unsigned char		bank;					// current bank (0..14)
extern volatile unsigned char		patch;					// current patch (0..31)
extern volatile unsigned char		patchName[LCD_PATCHNAME_CHAR_NUM];
#endif /* SENSORIZER_BANKSTICK_ENA */

#if SENSORIZER_SYNC_ENA
// syncronizer module
extern volatile unsigned char		sync_q[SENSOR_NUM];		// quantize starting point
extern const unsigned char			sync_q_values[SYNC_Q_MAX];	// eg acnl[5] returns 6 for 1/6
#endif


// ***** PROTOTYPES *****

// init & setup
extern void				ACSensorizer_Init(void) __wparam;

#if SENSORIZER_BANKSTICK_ENA
extern unsigned char	ACSensorizer_Patch_Write(void);
extern unsigned char	ACSensorizer_Patch_Read(void);
#endif /* SENSORIZER_BANKSTICK_ENA */

#if SENSORIZER_AUTOSENSE_ENA
// do Autosense (smin[sensor] & smax[sensor] => sensorizer.autosense = 0x?)
extern void				ACSensorizer_AutoSense(unsigned char pin, unsigned int pin_value) __wparam;
#endif /* SENSORIZER_AUTOSENSE_ENA */

// == ACSENSORIZER: Main Signal Processor ==
// process AIN, returns 7bit on success, 255 on error
extern unsigned char	ACSensorizer_Sensorize(unsigned char pin, unsigned int pin_value) __wparam;

#if SENSORIZER_SYNC_ENA
// receive clock signal from ACSyncronizer (polls ACSensorizer_Sensorize)
extern void				ACSensorizer_ReceiveClock(void);
#endif /* SENSORIZER_SYNC_ENA */

#endif /* _ACSENSORIZER_H */
