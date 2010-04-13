/*
 * MIOS Pedal Box / Pedal Board - pbx_midi_config.h
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
// INTERNAL MIDI IMPLEMENTATION
//
//  status for all events is 0xb# ..   # = patch bankstick channel
//
//  E1 = parameter 1   E2 = parameter 2
///////////////////////////////////////////////////////////////////////////

// Enter setup with MIDI event
#define   EVENT_SETUP_E1        127 // 0 - 127 - Param1
#define   EVENT_SETUP_E2        127 // 0 - 127 - Param2

// AIN setup with MIDI event
#define   GLOBAL_SETUP_E1       127 // 0 - 127 - Param1
#define   GLOBAL_SETUP_E2       126 // 0 - 127 - Param2

// Scrren change with MIDI event
#define   SCREEN_CHANGE_E1      127 // 0 - 127 - Param1 Byte
#define   SCREEN_CHANGE_E2      125 // 0 - 127 - Param2 Byte

// MIDI to AIN Function - MIDI Events can act as AIN
#define   ENABLE_MIDI_TO_AIN    1   // 0 = Disabled  1 = Enabled

#define   MIDI_PEDAL0_E1		20	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL1_E1		21	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL2_E1		22	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL3_E1		23	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL4_E1		24	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL5_E1		25	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL6_E1		26	// 0 - 127 - Param1 Byte | set to 255 to disable individually
#define   MIDI_PEDAL7_E1		27	// 0 - 127 - Param1 Byte | set to 255 to disable individually


// Patch Setup
#define   PATCH_SETUP_E1        127 // 0 - 127 - Param1 Byte
#define   PATCH_SETUP_E2        124 // 0 - 127 - Param2 Byte

//Cue system
#define   CUE_E1                122 // Param1 byte of next/prev cue (param2 byte : 1 = previous, 2 = next)
               

#define   CUE_SELECT_E1         122 // Param1 byte to enter cue select mode
#define   CUE_SELECT_E2         127 // Param2 byte to enter cue select mode

// Relay midi events
#define   RELAY_E1              112  // The first relays trigger - midi param 1. (param2 byte : 0 = off, 127 = on)
                                     // Relays 2 - 8 param1 byte will increase by 1 respectivly
									  
#define	  PEDALSWAP_E1			92   // The first PedalSwap trigger - midi param 1. (param2 byte ignored)
									 // PedalSwap 2-6 param1 byte will increase by 1 respectivly
									 // Also set NUM_PEDALSWAP_BUTTONS in pbx_config.h
