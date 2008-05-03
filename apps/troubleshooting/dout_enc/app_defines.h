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

ENC_NUMBER		EQU	0x011	; number of last moved encoder
ENC_INC			EQU	0x012	; incrementer value
DOUT_PIN		EQU	0x013	; number of DOUT pin
ENC_VALUE_16BIT_L	EQU	0x014	; low byte of 16-bit absolute value, address must be even!!!
ENC_VALUE_16BIT_H	EQU	0x015	; high byte of 16-bit absolute value, address must be odd!!!
