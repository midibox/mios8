; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

DISPLAY_UPDATE_REQ	EQU	0x010	; bit 0 will be set to request a display update

SPEED_SETTING		EQU	0x011	; the speed setting, selected by Inc/Dec button
ENC_VALUE_16BIT_L	EQU	0x012	; low byte of 16-bit absolute value, address must be even!!!
ENC_VALUE_16BIT_H	EQU	0x013	; high byte of 16-bit absolute value, address must be odd!!!
LAST_INC		EQU	0x014	; last incrementer
