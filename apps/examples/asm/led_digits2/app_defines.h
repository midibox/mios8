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

LEDDIGITS_CTR		EQU	0x010	; led digits counter, used by leddigits.inc
LEDDIGITS_0		EQU	0x011
LEDDIGITS_1		EQU	0x012
LEDDIGITS_2		EQU	0x013
LEDDIGITS_3		EQU	0x014
LEDDIGITS_4		EQU	0x015
LEDDIGITS_5		EQU	0x016
LEDDIGITS_6		EQU	0x017
LEDDIGITS_7		EQU	0x018
LEDDIGITS_8		EQU	0x019
LEDDIGITS_9		EQU	0x01a
LEDDIGITS_10		EQU	0x01b
LEDDIGITS_11		EQU	0x01c
LEDDIGITS_12		EQU	0x01d
LEDDIGITS_13		EQU	0x01e
LEDDIGITS_14		EQU	0x01f
LEDDIGITS_15		EQU	0x020

;; define the number of available digits here - it can be derived from the address
;; this number is only used by the initialization routine and not by the update handler itself
LEDDIGITS_NUMBER	EQU	(LEDDIGITS_15-LEDDIGITS_0) + 1


COUNTDOWN_L		EQU	0x30	; for the countdown
COUNTDOWN_H		EQU	0x31
COUNTUP_L		EQU	0x32	; for the incremented counter
COUNTUP_H		EQU	0x33
