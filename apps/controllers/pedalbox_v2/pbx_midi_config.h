/*
 * MIOS Pedal Box / Pedal Board - pbx_midi_config.h
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

// Patch Setup
#define   PATCH_SETUP_E1        127 // 0 - 127 - Param1 Byte
#define   PATCH_SETUP_E2        124 // 0 - 127 - Param2 Byte

//Cue system
#define   CUE_E1                122 // Param1 byte of next/prev cue (param2 byte : 1 = previous, 2 = next)

#define   CUE_SELECT_E1         122 // Param1 byte to enter cue select mode
#define   CUE_SELECT_E2         127 // Param2 byte to enter cue select mode

// Relay midi events
#define   RELAY_E1              112  // The first relays trigger - midi param 1. (param2 byte : 0 = off, 127 = on)
                                      // Relays 2 - 8 will increase by 1 respectivly
