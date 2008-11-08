; $Id: app_defines.h 326 2008-05-03 21:20:32Z tk $
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
SM_DOUT_VALUE		EQU	0x023

SM_ROW0_VALUE		EQU	0x028
SM_ROW1_VALUE		EQU	0x029
SM_ROW2_VALUE		EQU	0x02a
SM_ROW3_VALUE		EQU	0x02b
SM_ROW4_VALUE		EQU	0x02c
SM_ROW5_VALUE		EQU	0x02d
SM_ROW6_VALUE		EQU	0x02e
SM_ROW7_VALUE		EQU	0x02f

;; the debounce counter: each button uses a 4bit field
SM_DEBOUNCE_CTR0_01	EQU	0x100
SM_DEBOUNCE_CTR0_23	EQU	0x101
SM_DEBOUNCE_CTR0_45	EQU	0x102
SM_DEBOUNCE_CTR0_67	EQU	0x103
SM_DEBOUNCE_CTR1_01	EQU	0x104
	;; ...
SM_DEBOUNCE_CTR7_67	EQU	0x11f
