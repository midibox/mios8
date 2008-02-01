// $Id$
/*
 * AOUT program module definitions
 *
 * ==========================================================================
 *
 *  Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _AOUT_H
#define _AOUT_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

#if 1
	// common AOUT connections (MIDIbox CV pinning)
#define AOUT_LAT_CS		_LATC	// The chip select pin CS#
#define AOUT_TRIS_CS		_TRISC	// is connected to Port C.5
#define AOUT_PIN_CS		5	// (CANNOT be shared with other outputs!)
//
#define AOUT_LAT_DIN		_LATC	// The data input pin DIN
#define AOUT_TRIS_DIN		_TRISC	// is connected to Port C.4
#define AOUT_PIN_DIN		4	// (can be shared with other outputs)
//
#define AOUT_LAT_SCLK		_LATD	// The shift clock input pin SCLK
#define AOUT_TRIS_SCLK		_TRISD	// is connected to Port D.5
#define AOUT_PIN_SCLK		5	// (can be shared with other outputs)

#else
	// AOUT connections to MB64
#define AOUT_LAT_CS		_LATC	// The chip select pin CS#
#define AOUT_TRIS_CS		_TRISC	// is connected to Port C.5
#define AOUT_PIN_CS		5	// (CANNOT be shared with other outputs!)
//
#define AOUT_LAT_DIN		_LATC	// The data input pin DIN
#define AOUT_TRIS_DIN		_TRISC	// is connected to Port C.4
#define AOUT_PIN_DIN		4	// (can be shared with other outputs)
//
#define AOUT_LAT_SCLK		_LATD	// The shift clock input pin SCLK
#define AOUT_TRIS_SCLK		_TRISD	// is connected to Port D.0
#define AOUT_PIN_SCLK		0	// (can be shared with other outputs)
#endif


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of analog toolbox application
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned G0:1;  // first gate pin
    unsigned G1:1;  // second gate pin
  };
} aout_gates_t;


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void AOUT_Init(void);
void AOUT_Update(void);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern unsigned int aout_value[8];

extern aout_gates_t aout_gates;

#endif /* _AOUT_H */
