/*
 *  ACSensorizerDefines.h
 *  ACSensorizer
 *
 *  Created by audiocommander on 14.12.06.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de
 *
 *  ** Description **
 *	This file contains changeable parameters for recompilation
 *  with different setup modes and adapted settings
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


#ifndef _ACSENSORIZER_DEFINES_H
#define _ACSENSORIZER_DEFINES_H


#include "ACMidiDefines.h"


// ***** BASIC SETUP ***** 
#define SENSORIZER_MODEL_ID_DEFAULT 0x0	// default configuration (DOUT:1, DIN:0, ENC_TYPE2)
#define SENSORIZER_MODEL_ID_ONE		0x1 // first, grey model (DOUT:0, DIN:Xtended with hardware bug, ENC_TYPE1)
#define SENSORIZER_MODEL_ID_TWO		0x2 // second, black model (DOUT:1, DIN:default, ENC_TYPE2) 
// select model ID (dependent settings for DIN, DOUT, ENC_TYPES)
#define SENSORIZER_MODEL_ID			SENSORIZER_MODEL_ID_ONE

// enable modules
// the project compiles to a much smaller size if you set unneeded interface options to 0
#define SENSORIZER_INTERFACE_LCD	1 	// if LCD conncected
#define SENSORIZER_INTERFACE_HUI	1 	// if HUI connected (DIN/ENC)
#define SENSORIZER_INTERFACE_NRPN	1	// if NRPN-control (via MIDI) enabled
#define SENSORIZER_INTERFACE_BS		1	// if Bankstick(s) attached & enabled
#define SENSORIZER_INTERFACE_SYNC	1	// if Syncronizer (Clock forwarding & bar counter) enabled
#define SENSORIZER_INTERFACE_HARM	1	// if Harmonizer enabled
#if SENSORIZER_MODEL_ID == SENSORIZER_MODEL_ID_ONE
	#define SENSORIZER_INTERFACE_DIN	1	// if EXTRA(!) DIN pins enabled (see DIN, usually pins 16-31, just down signal)
	#define SENSORIZER_INTERFACE_DOUT	0	// if DOUT module connected (sensors 1..8 => DOUT SR1 1..8)
#elif SENSORIZER_MODEL_ID == SENSORIZER_MODEL_ID_TWO
	#define SENSORIZER_INTERFACE_DIN	0	// if EXTRA(!) DIN pins enabled (see DIN, usually pins 16-31, just down signal)
	#define SENSORIZER_INTERFACE_DOUT	1	// if DOUT module connected (sensors 1..8 => DOUT SR1 1..8)
#else
	#define SENSORIZER_INTERFACE_DIN	0	// if EXTRA(!) DIN pins enabled (see DIN, usually pins 16-31, just down signal)
	#define SENSORIZER_INTERFACE_DOUT	1	// if DOUT module connected (sensors 1..8 => DOUT SR1 1..8)
#endif

// ***** MIDI SETUP ***** 
#define MIDI_GLOBAL_CH				15	// application wide Midi-Channel: 0-15
#define MIDI_MERGER_MODE			MIOS_MIDI_MERGER_DISABLED	// if MIDI-IN should be forwarded
// harmonizer
#define MIDI_HARMONIZE_NOTES		1	// if incoming notes should be forwarded (harmonized)
#define SENSORIZED_NOTE_VELOCITY	100 // note velocity for sensorized NOTE_ONs (= CC#0)
// syncronizer
#ifdef SENSORIZER_INTERFACE_SYNC
	#define FORWARD_CLOCK			0	// if clock should be forwarded (regardless of MIDI_MERGER_MODE)
	#define SEND_CONTINUE_EACH_BAR	0	// CONTINUE is sent each first bar
#endif

// ***** MIDI ASSIGNMENTS ***** 
#define MIDI_CC_HARMONIZER_LISTEN	37	// 64-127: listen for next note-on, 0-64: exit listen-mode
#define SENSORIZER_PEDAL_CC			MIDI_CC_PEDAL	// select which CC should be sent on pedal down/release

#define BROADCAST_PRG_CH			1	// sends PRG_CH on GLOBAL CHANNEL with current bank/patch on load/store (default: 1)


// ***** LCD ***** 
#define SENSORIZER_LCD_TYPE			216	// currently only supported: 216 (=> 2x16)

// ***** AIN ***** 
#define AIN_MUXED					1	// 32 AINs via AIN-module
#define AIN_UNMUXED					0	// 8 AINs via J5, default setting
#define AIN_MUX						AIN_UNMUXED

#define AIN_NUM						8	// number of Analog Inputs, default: 8
#define AIN_DEADBAND				7	// 7 for 7-bit values, default: 7

// ***** SRIO ***** 
#define	SRIO_NUM					4	// number of ShiftRegisters: 1..16 (128 DI/O max), default: 4 for 1 DIN-module
#define SRIO_DEBOUNCE				0	// 0 = hiQuality buttons, 1 = low quality buttons, default: 0
#define SRIO_UPDATE_FRQ				1	// 1 ms for Rotary Encoders, default: 1

// ***** ENCODERS *****
#define ENC_NUM						4	// number of Encoders
#if SENSORIZER_MODEL_ID == SENSORIZER_MODEL_ID_ONE
	#define ENC_TYPES				1	// mixed speed:1 (for model_one <grey>)
#else
	#define ENC_TYPES				2	// slow:2 <default:2 for Voti.nl Encoders & SMASH's DIN>
#endif
	
// Encoder numbers as defined in mios_wrapper/mios_tables.inc
#define ENC_PARAM_A					0	// enc number for Param A
#define ENC_PARAM_B					1	// enc number for Param B
#define ENC_SENSOR_SELECT			2	// enc number for sensor selection wheel
#define ENC_MENU_SELECT				3	// enc number for menu switching wheel

#define DIN_ENC_PARAM_A				2	// ENC0 PushButton	// not needed
#define DIN_ENC_PARAM_B				6	// ENC1 PushButton	// not needed
#define DIN_ENC_SENSOR_SELECT		10	// not needed
#define DIN_ENC_MENU_SELECT			14	// not needed

// ***** DIN ***** 
#define DIN_PEDAL					3	// DIN-pin of connected Foot-Pedal
#define DIN_PEDAL_SECOND			2	// alternative pin-out (e.g. front _and_ rear)
#define DIN_PANIC					7	// sends PANIC! (all notes off)
#define DIN_LOAD					11	// loads current patch
#define DIN_STORE					15	// stores current settings to current patch
#define DIN_EXT_MIN					16	// Extra DIN pins, send out PIN - DIN_EXT_MIN on pressed (no signal on release)
#define DIN_EXT_MAX					31	// - " -
#define DIN_EXT_CC					14	// CC for extra DIN pins, see above



#endif /* _ACSENSORIZER_DEFINES_H */
