; $Id$
;
; MIDIbox FM
; Ensemble Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function initializes an ensemble
;;  MBFM_ENS_Init: init from EEPROM
;;  MBFM_ENS_Init_RAM: init from RAM
;;  Input:
;;     o ensemble number in MBFM_ENSEMBLE
;;     o bank number in MBFM_EBANK
;; --------------------------------------------------------------------------
MBFM_ENS_Init
	;; load ensemble
	call	MBFM_BANK_LoadE

MBFM_ENS_Init_RAM
	;; init all instruments
	call	MBFM_INSTR_InitAll

	;; init drumset
	goto	MBFM_DRUM_Init
