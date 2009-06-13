/*
 *  ACHarmonizer.h
 *  ACSensorizer
 *
 *  Created by audiocommander on 01.12.06.
 *  Copyright 2006 Michael Markert, http://www.audiocommander.de

 *  v0.0.1  2002-07-xx  first version in lingo
 *  v0.0.2  2003-01-xx  some improvements for m2
 *  v0.0.3  2004-03-27  complete redesign for m2, modular version still implemented in lingo
 *  v0.0.4  2004-10-23  rewritten as singleton object for cocoa, translated into objective-C
 *  v0.0.5  2005-02-26  corrected harmonies, multiple scale support
 *  v0.0.6  2006-12-01  first version
 *  v0.0.7  2007-08-11  added base note support
 *
 *
 *	description:
 *		notes get harmonized to next possible harmonic notes
 *		a base-note and a scale set are used
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


#ifndef _ACHARMONIZER_H
#define _ACHARMONIZER_H

#include "ACMidiDefines.h"									// for Base-Notes

#include "ACToolbox.h"										// for Random Calculation
#include "ACSensorizerDefines.h"							// for HARMONIZER_ENABLED



// ***** DEFINES *****

#define HARMONIZER_ENABLED			SENSORIZER_INTERFACE_HARM

#define SCALE_MAX					21						// number of available scales
#define SCALE_DEFAULT				20						// 0 for not harmonized, default: 20 (spanish)
#define BASE_DEFAULT				MIDI_NOTE_H				// MIDI_NOTE_x, default: MIDI_NOTE_H


#if HARMONIZER_ENABLED

// ***** GLOBALS *****

extern volatile unsigned char	harmonizer_base;			// 0 = C .. 11 = H
extern volatile unsigned char	harmonizer_scale;			// 0 .. SCALEMAX

extern volatile unsigned char	harmonizer_listen;			// listen for next base note

extern const unsigned char		noteNames[2][12];			// note to name table
extern const unsigned char		scaleNames[SCALE_MAX][4];	// scale name table


// ***** FUNCTIONS *****

extern void ACHarmonizer_Init(void);

extern void ACHarmonizer_SetBase(unsigned char value) __wparam;			// sets new base note, 0..127
extern void ACHarmonizer_SetScale(unsigned char value) __wparam;		// 0..SCALE_MAX:scale# or >SCALE_MAX:next
extern void ACHarmonizer_ToggleScale(unsigned char direction) __wparam;	// 0:down, 1:up
extern void ACHarmonizer_Random(void);									// set new random base & scale

extern unsigned char ACHarmonize(unsigned char value) __wparam;			// 0..127 in, returns harmonized value

#endif /* HARMONIZER_ENABLED */


#endif /* _ACHARMONIZER_H */
