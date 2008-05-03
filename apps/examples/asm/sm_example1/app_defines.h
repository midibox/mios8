; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

DISPLAY_UPDATE_REQ	EQU	0x010	; bit 0 will be set to request a display update

;; used by midi_evnt.inc
MIDI_EVNT0		EQU	0x011
MIDI_EVNT1		EQU	0x012
MIDI_EVNT_VALUE		EQU	0x013

;; used by sm_simple.inc
SM_BUTTON_COLUMN	EQU	0x020
SM_BUTTON_ROW		EQU	0x021
SM_BUTTON_VALUE		EQU	0x022
SM_SELECTED_COLUMN	EQU	0x023
SM_BUTTON_COLUMN_CTR	EQU	0x024

SM_ROW0_VALUE		EQU	0x100
SM_ROW1_VALUE		EQU	0x101
SM_ROW2_VALUE		EQU	0x102
SM_ROW3_VALUE		EQU	0x103
SM_ROW4_VALUE		EQU	0x104
SM_ROW5_VALUE		EQU	0x105
SM_ROW6_VALUE		EQU	0x106
SM_ROW7_VALUE		EQU	0x107

SM_ROW0_CHANGED		EQU	0x108
SM_ROW1_CHANGED		EQU	0x109
SM_ROW2_CHANGED		EQU	0x10a
SM_ROW3_CHANGED		EQU	0x10b
SM_ROW4_CHANGED		EQU	0x10c
SM_ROW5_CHANGED		EQU	0x10d
SM_ROW6_CHANGED		EQU	0x10e
SM_ROW7_CHANGED		EQU	0x10f

