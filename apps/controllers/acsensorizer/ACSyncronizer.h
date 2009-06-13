/*
 * ACSyncronizer
 * MIDI clock module definitions
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
 * Addional Code Copyright (C) 2007  Michael Markert, http://www.audiocommander.de
 * 
 * ==========================================================================
 * 
 * This file is part of a MIOS application
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ==========================================================================
 */



#ifndef _ACSYNC_H
#define _ACSYNC_H


#include "ACMidiDefines.h"
#include "ACSensorizerDefines.h"


// Defines
#define ACSYNC_GARBAGE_SERVICE			0		// if a reset should be done (send PANIC) each ACSYNC_GARBAGE_SERVICE_CTR
#define ACSYNC_RESET_AFTER_X_MEASURES	32		// reset after x measures, default: 32

// do not change these
#define ACSYNC_GARBAGE_SERVICE_CTR		23		// helper define for ACSYNC_RESET_AFTER_X_MEASURES, default: 23 (1..23)
#define MASTER_IS_ALIVE_COUNTER			4095	// detect timeout at MIDI_CLOCK receive



/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of midi clock
typedef union {
	struct {
		unsigned ALL			:8;
	};
	struct {
		unsigned RUN			:1;		// in run mode
		unsigned PAUSE			:1;		// in pause mode
		unsigned START_REQ		:1;		// start request
		unsigned STOP_REQ		:1;		// stop request
		unsigned CONT_REQ		:1;		// continue request
		unsigned free			:2;		// unassigned
		unsigned IS_MASTER		:1;		// in master mode
	};
} mclock_state_t;

// status of syncronizer
typedef union {
	struct {
		unsigned ALL			:8;
	};
	struct {
		unsigned CLOCK_REQ		:1;		// clock request
		unsigned UPDATE_REQ		:1;		// update polling (request action)
		unsigned RESET_REQ		:1;		// reset request (called frequently as repair service)
		unsigned free			:5;		// unassigned
	};
} mclock_sync_state_t;


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern mclock_state_t			mclock_state;
extern mclock_sync_state_t		mclock_sync_state;

extern unsigned char			bpm;					// holds the current BPM setting

extern unsigned char			mclock_ctr_96;			// counts from 0..95
extern unsigned char			mclock_ctr_24;			// counts from 0..23
extern unsigned char			mclock_ctr_beats;		// counts quarter notes 0..3
extern unsigned int				mclock_ctr_measures;	// counts measures (up to 65535)


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void ACSYNC_Init(void);
void ACSYNC_Tick(void);
void ACSYNC_Timer(void);

void ACSYNC_ReceiveClock(unsigned char byte) __wparam;

void ACSYNC_BPMSet(unsigned char _bpm);
unsigned char ACSYNC_BPMGet(void);

void ACSYNC_ResetMeter(void);
void ACSYNC_SendMeter(void);

void ACSYNC_DoStop(void);
void ACSYNC_DoPause(void);
void ACSYNC_DoRun(void);
void ACSYNC_DoRew(void);
void ACSYNC_DoFwd(void);

unsigned int ACSYNC_GetTimerValue(unsigned char _bpm);


#endif /* _ACSYNC_H */
