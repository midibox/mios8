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

MIDICLK_STATE		EQU	0x130	; state of MIDI clock handler
MIDICLK_BEAT_MSB	EQU     0x131	; current "beat" (most significant bits))
MIDICLK_BEAT_LSB	EQU     0x132	; current "beat" (least significant bits))
MIDICLK_QN		EQU     0x133	; current "quarter note"
MIDICLK_CTR		EQU     0x134	; current "clock counter"
MIDICLK_POS_LSB		EQU	0x135	; temporary used to receive the song position
MIDICLK_POS_MSB		EQU	0x136	; temporary used to receive the song position

DISPLAY_UPDATE_REQ	EQU	0x040	; bit 0 will be set to request a display update
