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

WAIT_TIMER0		EQU	0x010
WAIT_TIMER1		EQU	0x011
RUNNING_CTR		EQU	0x012
BENCHMARK_UPDATE_REQ	EQU	0x013

DELAY_MIN_L		EQU	0x014
DELAY_MIN_H		EQU	0x015
DELAY_MAX_L		EQU	0x016
DELAY_MAX_H		EQU	0x017
