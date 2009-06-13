/*
 *  ACSim_console.h
 *  v 0.0.9
 *
 *  2006 April 06 Created by Michael Markert, http://www.audiocommander.de
 *  mios simulator source for MIOS (c) by T. Klose, http://www.midibox.org
 *  2006 May 19 stryd_one, Added Windoze stuff
 *  2006 Jul 15 general rework, adapted name spaces and filenames, correct header implementation, 
 *				 added MIOS_HLP, changed license to GNU GPL
 *  2007 Jan 17 improved MIOS_IIC for SpeakJet debugging
 *  2007 Jan 26	added MIDI debug messages
 *				improved Timer() calls
 *				added Tick() calls
 *  2007 Jan 28 added support for grahical program connection (handTracker_demo)
 *  2007 May 04	added MIOS_EEPROM
 *  2007 Dec 12	added MIOS_DOUT
 *				improved help (command overview)
 *
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


#ifndef _ACSIM_CONSOLE_H
#define _ACSIM_CONSOLE_H



// ***** DEFINES *****

// this is a very important define
// if _DEBUG_C is defined, the current target is the simulator
// use #ifdef _DEBUG_C to implement code ONLY for the simulator class!
#define _DEBUG_C


// set your environment
#define MAC							0x1
#define WIN							0x2
#define DEBUG_ENVIRONMENT			MAC

// set debug console output level
#define DEBUG_VERBOSE				0
#define DEBUG_SYSTEM_VERBOSE		1 // shows hardware system setup at startup
#define DEBUG_DOUT_VERBOSE			1 // hex prints state of connected DOUTs
#define DEBUG_SPEAKJET_VERBOSE		0 // shows additional SJ-Control Names for Chars
#define DEBUG_MIDI_VERBOSE			1 // shows additional MIDI Names for Numbers
#define DEBUG_SYSREALTIME_VERBOSE	1 // shows clock output (time, measures)

// link against frameworks
#if DEBUG_ENVIRONMENT == MAC
	#import <CoreFoundation/CoreFoundation.h>
#else
	#import <Stdio.h>
#endif

// include main (from .syx-project)
#include "../main.h"

// include SJ classes for IIC-Speakjet Verbose output
#if DEBUG_SPEAKJET_VERBOSE
	#include "../IIC_SpeakJetDefines.h"
#endif

// setup the hardware config that works for your application
#define DEBUG_LCD_TYPE				KII_LCD_TYPE	// see below, set in main.h
#if DEBUG_LCD_TYPE == 116
	#define DEBUG_LCD_LINES			1
	#define DEBUG_LCD_SIZE			16
#elif DEBUG_LCD_TYPE == 28
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			8
#elif DEBUG_LCD_TYPE == 216
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			16
#elif DEBUG_LCD_TYPE == 240
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			40
#elif DEBUG_LCD_TYPE == 416
	#define DEBUG_LCD_LINES			4
	#define DEBUG_LCD_SIZE			16
#else
	#define DEBUG_LCD_LINES			4
	#define DEBUG_LCD_SIZE			20
#endif

#define DEBUG_BANKSTICK_SIZE		32768			// bytes
#define DEBUG_BANKSTICK_NUM			1				// Number of Banksticks attached

#define DEBUG_EEPROM_SIZE			256				// bytes
#define DEBUG_EEPROM_START_ADDRESS	0x00
#define DEBUG_EEPROM_END_ADDRESS	0xFF

#if AIN_NUM == 0
	#define DEBUG_AIN_NUM			4				// DO NOT SET THIS TO ZERO!
#else
	#define DEBUG_AIN_NUM			AIN_NUM
#endif /*AIN_NUM */

#define DEBUG_DIN_NUM				32				// DO NOT SET THIS TO ZERO!
#define DEBUG_DOUT_NUM				DEBUG_AIN_NUM	// DO NOT SET THIS TO ZERO!

#ifndef ENC_NUM
	#define ENC_NUM					1				// DO NET SET THIS TO ZERO!
#endif
#define DEBUG_ENC_NUM				ENC_NUM
#define DEBUG_ENCODER				0

#define DEBUG_BUTTON_OK				0				// BUTTON_OK
#define DEBUG_BUTTON_INCREASE		0				// BUTTON_INCREASE
#define DEBUG_BUTTON_DECREASE		0				// BUTTON_DECREASE

#define DEBUG_BUTTON_AIN_PEDAL		0				// BUTTON_AIN_PEDAL
#define DEBUG_PEDAL_PRESSED			1				// Default return value on "is-pedal-pressed" query



// define __wparam (ignore on ACSim)
#define __wparam  


// globals

extern unsigned char debug_LCD_line[DEBUG_LCD_LINES][DEBUG_LCD_SIZE];

// prototypes

extern void debug_init();
extern void debug_timer();

extern void debug_lcd_print_char(unsigned char c);
extern void debug_lcd_print_string(unsigned char len, char *str[len]);

extern void debug_notify_ain(unsigned char pin, unsigned int pin_value);

void runloop(int loopcount);

#ifndef _DEBUG_GUI
int main(int argc, char **argv);
#endif



#endif /* _ACSIM_CONSOLE_H */
