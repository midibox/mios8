; $Id$
;
; MIDIbox MF
; FSR pointer return functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the first event byte depending on
;;  MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_FSR_FaderEvnt0
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_FADER_EVNT0_00
	movf	MBMF_CURRENT_FADER, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_FSR_FaderEvnt1
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_FADER_EVNT1_00
	movf	MBMF_CURRENT_FADER, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the fader value depending on
;;  MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_FSR_FaderValue
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_FADER_VALUES_00
	movf	MBMF_CURRENT_FADER, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the fader last value depending on
;;  MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
	;; include this only when morph function enabled!
MBMF_FSR_FaderLastValue
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_FADER_LAST_VALUES_00
	movf	MBMF_CURRENT_FADER, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the first event byte depending on
;;  MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_FSR_ButtonEvnt0
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_BUTTON_EVNT0_00
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_FSR_ButtonEvnt1
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_BUTTON_EVNT1_00
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the button value depending on
;;  MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_FSR_ButtonValue
	;; (status of 8 buttons stored in a SR register)
	SET_BSR	MBMF_BASE
	lfsr	FSR0, MBMF_BUTTON_VALUES_SR0
	rrf	MBMF_CURRENT_BUTTON, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x07
	addwf	FSR0L, F
	return
