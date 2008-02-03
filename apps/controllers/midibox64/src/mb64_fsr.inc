; $Id$
;
; MIDIbox64
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
;;  MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_FSR_PotEvnt0
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_POT_EVNT0_00
	movf	MB64_CURRENT_POT, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_FSR_PotEvnt1
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_POT_EVNT1_00
	movf	MB64_CURRENT_POT, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the pot value depending on
;;  MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_FSR_PotValue
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_POT_VALUES_00
	movf	MB64_CURRENT_POT, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the last pot value depending on
;;  MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_FSR_PotLastValue
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_POT_LAST_VALUES_00
	movf	MB64_CURRENT_POT, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the first event byte depending on
;;  MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_FSR_ButtonEvnt0
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_BUTTON_EVNT0_00
	movf	MB64_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_FSR_ButtonEvnt1
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_BUTTON_EVNT1_00
	movf	MB64_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the button value depending on
;;  MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_FSR_ButtonValue
	;; (status of 8 buttons stored in a SR register)
	SET_BSR	MB64_BASE
	lfsr	FSR0, MB64_BUTTON_VALUES_SR0
	rrf	MB64_CURRENT_BUTTON, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x07
	addwf	FSR0L, F
	return
