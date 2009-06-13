/*
 *  ACSim_mios.h
 *  v 0.0.9
 *
 *  2006 April 06 Created by Michael Markert, http://www.audiocommander.de
 *  mios simulator header for MIOS (c) by T. Klose, http://www.midibox.org
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


#ifndef _ACSIM_MIOS_H
#define _ACSIM_MIOS_H



#import <Stdio.h>
#import <stdlib.h>
#import <string.h>

#import "ACSim_console.h"

#if DEBUG_SPEAKJET_VERBOSE
	#import "IIC_SpeakJet.h"
#endif

#define __wparam  
#define code const



// MIOS definitions
#define MIOS_MIDI_INTERFACE_COMMON	0x00
#define MIOS_MIDI_INTERFACE_TO_HOST	0x01

#define MIOS_MIDI_MERGER_DISABLED	0x00
#define MIOS_MIDI_MERGER_ENABLED	0x01
#define MIOS_MIDI_MERGER_MBLINK_EP	0x02
#define MIOS_MIDI_MERGER_MBLINK_FP	0x03

#define MIOS_ENC_SPEED_SLOW			0x00
#define MIOS_ENC_SPEED_NORMAL		0x01
#define MIOS_ENC_SPEED_FAST			0x02

#define MIOS_LCD_TYPE_CLCD			0x00
#define MIOS_LCD_TYPE_GLCD0			0x01
#define MIOS_LCD_TYPE_GLCD1			0x02
#define MIOS_LCD_TYPE_GLCD2			0x03
#define MIOS_LCD_TYPE_GLCD3			0x04
#define MIOS_LCD_TYPE_GLCD4			0x05
#define MIOS_LCD_TYPE_MLCD			0x06
#define MIOS_LCD_TYPE_GLCD_CUSTOM	0x07

#define MIOS_GLCD_FONT				0x28fc

// ACSim definitions
#define LCD_1x16					116
#define LCD_2x8						28
#define LCD_2x16					216
#define LCD_2x40					240
#define LCD_4x16					416
#define LCD_4x20					420


// structures of the CFG and STAT bytes
typedef union {
	struct {
		unsigned ALL:8;
	};
	struct {
		unsigned LCD_TYPE:3;  // selects LCD type
		unsigned USE_GLCD:1;  // if 1, graphical LCD is connected
		unsigned MERGER:1;    // if 1, MIDI merger is enabled
		unsigned MBLINK:1;    // if 1, MIDIbox Link is enabled
		unsigned TO_HOST:1;   // if 1, MIDI interface will run with 38400 baud instead of 31250
		unsigned FRQ_20MHz:1; // if 1, it is assumed that the bux is running with 20 MHz, else with 40 MHz
	};
} mios_box_cfg0_t;

typedef union {
	struct {
		unsigned ALL:8;
	};
	struct {
		unsigned BS_A:3;            // A2..A0 of BankStick
		unsigned BS_DIS_VERIFY:1;   // if 1, writes to the BankStick will not be verified
		unsigned IIC_STRETCH_CLK:1; // if 1, IIC clock stretching enabled
unsigned :1;
unsigned :1;
unsigned :1;
	};
} mios_box_cfg1_t;

typedef union {
	struct {
		unsigned ALL:8;
	};
	struct {
		unsigned BS_AVAILABLE:1;        // if 1, BankStick is available
		unsigned MBLINK_TUNNEL_PASS:1;  // if 1, a MBLinked event will be tunnled
		unsigned SUSPEND_ALL:1;         // if 1, all system and user tasks are suspended
		unsigned SUSPEND_USER:1;        // if 1, all user tasks are suspended
		unsigned AUTOREPEAT:1;          // if 1, start the auto-repeat handler
		unsigned MLCD_TRANSFER:1;       // if 1, a transfer to the MLCD has been started
unsigned :1;
unsigned :1;
	};
} mios_box_stat_t;

// debug related structures
typedef union {
	struct {
		unsigned ALL:8;
	};
	struct {
		unsigned TIMER_ENABLED:1;
		unsigned free:7;
	};
} debug_user_timer_t;

// mios tables
extern const unsigned char MIOS_MPROC_EVENT_TABLE[2*128];
extern const unsigned char MIOS_ENC_PIN_TABLE[2*64];




#pragma mark pic18f typedefs
// "pic18f452.h" typedefs
typedef union {
	struct {
		unsigned RC0:1;
		unsigned RC1:1;
		unsigned RC2:1;
		unsigned RC3:1;
		unsigned RC4:1;
		unsigned RC5:1;
		unsigned RC6:1;
		unsigned RC7:1;
	};
	struct {
		unsigned T1OSO:1;
		unsigned T1OSI:1;
unsigned :1;
		unsigned SCK:1;
		unsigned SDI:1;
		unsigned SDO:1;
		unsigned TX:1;
		unsigned RX:1;
	};
	struct {
		unsigned T1CKI:1;
		unsigned CCP2:1;
		unsigned CCP1:1;
		unsigned SCL:1;
		unsigned SDA:1;
unsigned :1;
		unsigned CK:1;
		unsigned DT:1;
	};
} __PORTCbits_t;

typedef union {
	struct {
		unsigned RD0:1;
		unsigned RD1:1;
		unsigned RD2:1;
		unsigned RD3:1;
		unsigned RD4:1;
		unsigned RD5:1;
		unsigned RD6:1;
		unsigned RD7:1;
	};
	struct {
		unsigned AD0:1;
		unsigned AD1:1;
		unsigned AD2:1;
		unsigned AD3:1;
		unsigned AD4:1;
		unsigned AD5:1;
		unsigned AD6:1;
		unsigned AD7:1;
	};
} __PORTDbits_t;

typedef union {
	struct {
		unsigned RBIF:1;
		unsigned INT0F:1;
		unsigned T0IF:1;
		unsigned RBIE:1;
		unsigned INT0E:1;
		unsigned T0IE:1;
		unsigned PEIE:1;
		unsigned GIE:1;
	};
} __INTCONbits_t;



#pragma mark debug typedefs
// bankstick
typedef struct {
	unsigned char buffer[DEBUG_BANKSTICK_SIZE];
} debug_bankstick_t;


#pragma mark mios globals
// mios globals
extern mios_box_stat_t		MIOS_BOX_STAT;
// "pic18f452.h"
extern __PORTCbits_t		PORTCbits;
extern __PORTDbits_t		PORTDbits;
extern __INTCONbits_t		INTCONbits;
// HLP
extern unsigned char		MIOS_PARAMETER1;
extern unsigned char		MIOS_PARAMETER2;
extern unsigned char		MIOS_PARAMETER3;


#pragma mark debug globals
// export globals
extern debug_user_timer_t	debug_user_timer;
extern unsigned char		mios_counter_incr;

extern unsigned char		debug_ain_lastPin;
extern unsigned int			debug_ain_value[DEBUG_AIN_NUM];
extern unsigned char		debug_din_lastPin;
extern unsigned char		debug_din_value[DEBUG_DIN_NUM];
extern unsigned char		debug_dout_value[DEBUG_DOUT_NUM];
extern unsigned int			debug_enc_value[DEBUG_ENC_NUM];

extern unsigned char		debug_cursor_pos;

extern unsigned int			debug_MIDI_byteNum;
extern unsigned int			debug_IIC_byteNum;

extern unsigned char		debug_bankstick_ctrl;
extern debug_bankstick_t	debug_bankstick[DEBUG_BANKSTICK_NUM];

extern unsigned char		debug_eeprom[DEBUG_EEPROM_END_ADDRESS];



#endif /* _ACSIM_MIOS_H */
