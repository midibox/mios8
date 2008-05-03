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

AIN_PIN			EQU	0x011	; number of DOUT pin
INC_VALUE_16BIT_L	EQU	0x012	; low byte of 16-bit absolute value, address must be even!!!
INC_VALUE_16BIT_H	EQU	0x013	; high byte of 16-bit absolute value, address must be odd!!!

AIN_VALUE_L		EQU	0x14	; the current pot position
AIN_VALUE_H		EQU	0x15
JITTER_VALUE_L		EQU	0x16	; the measured jitter which will be displayed
JITTER_VALUE_H		EQU	0x17
INT_JITTER_VALUE_L	EQU	0x18	; the measured jitter
INT_JITTER_VALUE_H	EQU	0x19

TIMER_POSTSCALER	EQU	0x1a	; for the USER_Timer
TIMER_RSTCTR            EQU	0x1b	; for the USER_Timer
