// $Id$
/*
 * MIDI program module definitions
 *
 * ==========================================================================
 *
 * Copyright (C) 2005  Thorsten Klose (tk@midibox.org)
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

#ifndef _MIDI_H
#define _MIDI_H

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

// status of analog toolbox application
typedef union {
  struct {
    unsigned ALL:8;
  };
  struct {
    unsigned NOTE:7;
    unsigned GATE:1;
  };
} midi_note_t;



/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

void MIDI_Init(void);
void MIDI_Evnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern midi_note_t midi_note[16];

extern unsigned char midi_cc_chn0[128];

extern unsigned int midi_pitch_bender[16];

#endif /* _MIDI_H */
