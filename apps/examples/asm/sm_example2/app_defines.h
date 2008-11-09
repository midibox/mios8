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

#define MB_STAT_DISPLAY_UPDATE_REQ	0 ; if set, display will be updated
#define MB_STAT_DISPLAY_UPDATE_TIME 1 ; if set, display can be updated

;; ==========================================================================

	;; Data memory layout	(Data RAM)
	;; 0x000-0x00f	reserved memory for MIOS
	;; 0x010-0x37f	free memory for user applications
	;; 0x380-0x5ff	reserved memory for MIOS
	;; 0x600-0xf7f	free memory for user applications on 18F4620
	;; 0xf80-0xfff	special function registers (SFR)

;; ==========================================================================
;;  General MIDIO Control Variables in ACCESS page
;; ==========================================================================
MB_STAT				EQU	0x010

;; used by midi_evnt.inc
MIDI_EVNT0			EQU	0x011
MIDI_EVNT1			EQU	0x012
MIDI_EVNT_VALUE		EQU	0x013

;; ==========================================================================
;; used by sm_simple.inc -- banked access so can be anywhere in register file
;; ==========================================================================
SM_BUTTON_COLUMN	EQU	0x020	; the column selected by a DOUT bit
SM_BUTTON_ROW		EQU	0x021
SM_BUTTON_VALUE		EQU	0x022
SM_DOUT_VALUE		EQU	0x023	; DOUT value being shifted as DIN bits are read
								;   to put DOUT bit in position for next latching
SM_LAST_COLUMN		EQU	0x025
SM_DISPLAY_TIMER	EQU 0x026

;; ==========================================================================
;; last captured button inputs -- FSR access so can be anywhere in register file
;;		each row is 8 DIN inputs as selected by 1 DOUT bit
;; ==========================================================================
SM_ROW0_VALUE		EQU	0x028
SM_ROW1_VALUE		EQU	0x029
SM_ROW2_VALUE		EQU	0x02a
SM_ROW3_VALUE		EQU	0x02b
SM_ROW4_VALUE		EQU	0x02c
SM_ROW5_VALUE		EQU	0x02d
SM_ROW6_VALUE		EQU	0x02e
SM_ROW7_VALUE		EQU	0x02f

;; ========================================================================== 
;; the debounce counter: each button uses a 4bit field
;;		-- FSR access so can be anywhere in register file
;; ==========================================================================
SM_DEBOUNCE_CTR0_01	EQU	0x100
SM_DEBOUNCE_CTR0_23	EQU	0x101
SM_DEBOUNCE_CTR0_45	EQU	0x102
SM_DEBOUNCE_CTR0_67	EQU	0x103
SM_DEBOUNCE_CTR1_01	EQU	0x104
	;; ...
SM_DEBOUNCE_CTR7_67	EQU	0x11f
