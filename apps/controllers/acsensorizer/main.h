/*
 *  main.h
 *  ACSensorizer
 *
 *  Created by audiocommander on 30.08.06.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
 *
 *
 * See readme.txt for version history!
 * 
 *
 * Examples on how to use this Class along with a Bankstick or an Interface (Hardware
 * HUI or MIDI/NRPN-Config) can be found in the main.h/.c files of the Sensorizer-
 * Project. You can select witch features should be compiled into the application.
 * Please see readme.txt => this file is more a manual than a readme!
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


#ifndef _MAIN_H
#define _MAIN_H



// ***** DEFINES *****


// ** common defines **
#define FALSE						0
#define TRUE						1


// include custom defines
#include "ACSensorizerDefines.h"


// LCD related (internal)
#define LCD_MESSAGE_DURATION		191		// 1 second: 127, 2 seconds: 255

#define LCD_CENTER_ROW_ONE			0x00
#define LCD_CENTER_ROW_TWO			0x40

#if SENSORIZER_INTERFACE_SYNC
	#define LCD_MENU_ITEMS_NUM		11
	#define LCD_MENU_BNK_PRG		0x00
	#define LCD_MENU_PATCH			0x01
	#define LCD_MENU_ENA_VAL		0x02
	#define LCD_MENU_CH_CC			0x03
	#define LCD_MENU_Q_BPM			0x04
	#define LCD_MENU_BAS_SCL		0x05
	#define LCD_MENU_PED_RLD		0x06
	#define LCD_MENU_INV_AUTO		0x07
	#define LCD_MENU_MIN_MAX		0x08
	#define LCD_MENU_FAC_SLW		0x09
	#define LCD_MENU_FRM_TO			0x0A
#else
	#define LCD_MENU_ITEMS_NUM		10
	#define LCD_MENU_BNK_PRG		0x00
	#define LCD_MENU_PATCH			0x01
	#define LCD_MENU_ENA_VAL		0x02
	#define LCD_MENU_CH_CC			0x03
	#define LCD_MENU_BAS_SCL		0x04
	#define LCD_MENU_PED_RLD		0x05
	#define LCD_MENU_INV_AUTO		0x06
	#define LCD_MENU_MIN_MAX		0x07
	#define LCD_MENU_FAC_SLW		0x08
	#define LCD_MENU_FRM_TO			0x09
#endif /* SENSORIZER_INTERFACE_SYNC */

#define LCD_MENU_STR_BNK_PRG		"BNK|PRG"
#define LCD_MENU_STR_PATCH			" PATCH "
#define LCD_MENU_STR_ENA_VAL		"ENA|VAL"
#define LCD_MENU_STR_CH_CC			"CH |CC "
#define LCD_MENU_STR_Q_BPM			"Q  |BPM"
#define LCD_MENU_STR_BAS_SCL		"BAS|SCL"
#define LCD_MENU_STR_PED_RLD		"PED|RLD"
#define LCD_MENU_STR_INV_AUT		"INV|AUT"
#define LCD_MENU_STR_MIN_MAX		"MIN|MAX"
#define LCD_MENU_STR_FAC_SLW		"F  |SLW"
#define LCD_MENU_STR_FRM_TO			"FRM|TO "

#define LCD_MSG_PANIC				0x01
#define LCD_MSG_PEDAL_SENSE			0x02
#define LCD_MSG_AUTOSENSE			0x03
#define LCD_MSG_PRG_READ			0x05
#define LCD_MSG_PRG_READ_ERROR		0x15
#define LCD_MSG_PRG_WRITE			0x06
#define LCD_MSG_PRG_WRITE_ERROR		0x16

#define LCD_PATCHNAME_CHAR_NUM		8		// do NOT change this!!!
#define LCD_PATCHNAME_CHAR_MIN		0x20	// min character (space)
#define LCD_PATCHNAME_CHAR_START	0x30	// current start character ('0')
#define LCD_PATCHNAME_CHAR_MAX		0x7F	// max character (arrow)

// LCD commands
#define LCD_CMD_DISPLAY_OFF			0x8
#define LCD_CMD_DISPLAY_ON			0xC
#define LCD_CMD_DISPLAY_CURSOR_ON	0xE
#define LCD_CMD_DISPLAY_CURSOR_OFF	0xC



// ***** TYPEDEFS *****

typedef union {
	struct {
		unsigned ALL				:8;	// for storage
	};
	struct {
		unsigned displayNeedsInit	:1;	// request for init LCD
		unsigned displayNeedsUpdate	:1;	// request for complete LCD update
		unsigned displayTickUpdate	:1;	// request for partial LCD update (incl. sensor values)
		unsigned free				:5;	// free
	};
} acapp_t;

typedef union {
	struct {
		unsigned ALL				:8;	// for storage
	};
	struct {
		unsigned selectedSensor		:3;	// current sensor (0-7)
		unsigned selectedMenu		:5;	// current menu (0-31)
		unsigned free				:1;	// free
	};
} acmenu_t;



// ***** GLOBALS *****

extern acapp_t			acapp;
extern acmenu_t			acmenu;

extern unsigned char	lastAIN_pin;



// ***** PROTOTYPES *****

#ifdef _DEBUG_C
	#define __wparam  
	// export functions that are called from within debug_mios.c 
	extern void Init(void) __wparam;
	extern void Tick(void) __wparam;
	extern void Timer(void) __wparam;														// ISR!
	extern void DISPLAY_Init(void) __wparam;
	extern void DISPLAY_Tick(void) __wparam;
	extern void AC_DISPLAY_Message(unsigned char msgCode) __wparam;
	extern void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
	extern void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam;
	extern void MPROC_NotifyTimeout(void) __wparam;
	extern void MPROC_NotifyReceivedByte(unsigned char byte) __wparam;
	extern void SR_Service_Prepare(void) __wparam;											// ISR!
	extern void SR_Service_Finish(void) __wparam;											// ISR!
	extern void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam;
	extern void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam;
	extern void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam;
#else
	extern void AC_DISPLAY_Message(unsigned char msgCode) __wparam;
	extern void ENC_NotifyChange(unsigned char encoder,char incrementer) __wparam;
#endif /* _DEBUG_C */


#endif /* _MAIN_H */
