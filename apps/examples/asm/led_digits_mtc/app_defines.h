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
LEDDIGITS_0		EQU	0x011	; define registers for all available digits
LEDDIGITS_1		EQU	0x012
LEDDIGITS_2		EQU	0x013
LEDDIGITS_3		EQU	0x014
LEDDIGITS_4		EQU	0x015
LEDDIGITS_5		EQU	0x016
LEDDIGITS_6		EQU	0x017
LEDDIGITS_7		EQU	0x018

;; define the number of available digits here - it can be derived from the address
;; this number is only used by the initialization routine and not by the update handler itself
LEDDIGITS_NUMBER	EQU	(LEDDIGITS_7-LEDDIGITS_0) + 1

MTC_STATE		EQU	0x030	; state of received MTC code
MTC_HR			EQU     0x031	; current "hours"
MTC_MN			EQU     0x032	; current "minutes"
MTC_SC			EQU     0x033	; current "seconds"
MTC_FR			EQU     0x034	; current "frames"
MTC_HR_NEW		EQU	0x035	; received "hours"
MTC_MN_NEW		EQU	0x036	; received "minutes"
MTC_SC_NEW		EQU	0x037	; received "seconds"
MTC_FR_NEW		EQU	0x038	; received "frames"

DISPLAY_UPDATE_REQ	EQU	0x040	; bit 0 will be set to request a display update
