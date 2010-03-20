/*
 * MIOS Pedal Box / Pedal Board - pbx_relay.h
 * v2.5beta3 - April 2009
 * ==========================================================================
 *
 *  Copyright (C) 2009  Mick Crozier - mick@durisian.com
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 *
 * ==========================================================================
 */

const unsigned char special_functions[10][17]=
{
    {"Next Cue       "},
    {"Previous Cue   "},
	
	
	// SET RELAY NAMES HERE
    {"Relay 1        "},
    {"Relay 2        "},
    {"Relay 3        "},
    {"Relay 4        "},
    {"Relay 5        "},
    {"Relay 6        "},
    {"Relay 7        "},
    {"Relay 8        "}
};

const unsigned char relay_polarity[8] = 

// VIRTUAL RELAY POLARITY - When power is appled to the relay, does it make the connection ON or OFF?
//0    1    2    3    4    5    6    7
{ON , ON , ON , ON , ON , ON , ON , ON }; //



const unsigned char relay_state_name[16][17]=
{

// RELAY STATE NAME FOR LCD - 2 enties each provide first the physical 'Off' state, 
// and second the physical 'On' state

	// RELAY 1
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 2
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 3
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 4
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 5
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 6
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 7
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
	
	// RELAY 8
    {"      Off      "},   // physical OFF state (DOUT pin_value = 0)
    {"      On       "},   // physical ON state (DOUT pin_value = 1)
};
