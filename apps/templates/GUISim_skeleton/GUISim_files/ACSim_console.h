/*
 *  ACSim_console.h
 *  v 0.0.7
 *
 *  2006 April 06 Created by Michael Markert, http://www.audiocommander.de
 *  mios simulator source for MIOS (c) by T. Klose, http://www.midibox.org
 *  2006 May 19 stryd_one, Added Windoze stuff
 *  2006 July 15 general rework, adapted name spaces and filenames, correct header implementation,
 *				 added MIOS_HLP, changed license to GNU GPL
 *  2007 January 17 improved MIOS_IIC for SpeakJet debugging
 *  2007 January 21 added MIOS Merger and MIDI-Clock stuff, updated to v 0.0.6
 *  2007 January 26	added MIDI debug messages
 *					improved Timer() calls
 *					added Tick() calls
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
 * YOU ARE NOT ALLOWED NOT USE IT WITHIN PROPRIETARY CLOSED-SOURCE PROJECTS
 */


#ifndef _ACSIM_CONSOLE_H
#define _ACSIM_CONSOLE_H


// ***** DEFINES *****

// this is a very important define
// if _DEBUG_C is defines, the current target is the simulator
// use #ifdef _DEBUG_C to implement code ONLY for the simulator class!
#define _DEBUG_C

// OS selection
// You must choose one of the below options to set the correct Operating System for the application
#define _DEBUG_OS_WIN
//#define _DEBUG_OS_MAC

// adapt general debug settings
#define DEBUG_VERBOSE				1
#define DEBUG_SPEAKJET_VERBOSE		0 // shows SJ-Control Names instead of odd Chars if enabled
#define DEBUG_MIDI_VERBOSE			1 // shows additional MIDI Names for Numbers

// adapt application related settings
#ifdef LCD_1x16
	#define DEBUG_LCD_LINES			1
	#define DEBUG_LCD_SIZE			16
#elifdef LCD_2x8
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			8
#elifdef LCD_2x16
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			16
#elifdef LCD_2x40
	#define DEBUG_LCD_LINES			2
	#define DEBUG_LCD_SIZE			40
#elifdef LCD_4x16
	#define DEBUG_LCD_LINES			4
	#define DEBUG_LCD_SIZE			16
#else
	#define DEBUG_LCD_LINES			4
	#define DEBUG_LCD_SIZE			20
#endif

#define DEBUG_BANKSTICK_SIZE		32768
#define DEBUG_BANKSTICK_NUM			1

// Number of AIN-Lines, either write it here or define AIN_NUM in main.h
#if AIN_NUM == 0  // DEBUG_AIN_NUM mustn't be zero
	#define DEBUG_AIN_NUM				1
#else
	#define DEBUG_AIN_NUM				AIN_NUM
#endif /*AIN_NUM */

// Number of Encoders, either write it here or define ENC_NUM in main.h
#ifndef ENC_NUM
    #define ENC_NUM                 0
#endif
#define DEBUG_ENC_NUM				ENC_NUM

// "default" Encoder #
#define DEBUG_ENCODER				0


// DIN-Pin config
#define DEBUG_BUTTON_OK				0 // BUTTON_OK
#define DEBUG_BUTTON_INCREASE		0 // BUTTON_INCREASE
#define DEBUG_BUTTON_DECREASE		0 // BUTTON_DECREASE

#define DEBUG_BUTTON_AIN_PEDAL		0 // BUTTON_AIN_PEDAL
#define DEBUG_PEDAL_PRESSED			1



// define __wparam (ignore on ACSim)
#define __wparam

int random(void);

#endif /* _ACSIM_CONSOLE_H */

