; $Id$
;
; MIDIbox64E
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
;;  This function returns a FSR0 pointer to the first entry in the MB64E_ENC_MAP
;;  depending on MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_FSR_EncMap
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_ENC_MAP_00
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the first event byte depending on
;;  MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_FSR_EncEvnt0
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_ENC_EVNT0_00
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_FSR_EncEvnt1
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_ENC_EVNT1_00
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the enc value depending on
;;  MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_FSR_EncValue
MB64E_FSR_FaderValue
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_ENC_VALUES_00
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the enc last value depending on
;;  MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
#if DEFAULT_NUMBER_AIN == 0 && DEFAULT_MORPH_FUNCTION_ENABLED == 1
	;; include this only when morph function enabled!
MB64E_FSR_EncLastValue
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_ENC_LAST_VALUES_00
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addwf	FSR0L, F
	return
#endif

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the first event byte depending on
;;  MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_FSR_ButtonEvnt0
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_BUTTON_EVNT0_00
	movf	MB64E_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the second event byte depending on
;;  MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_FSR_ButtonEvnt1
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_BUTTON_EVNT1_00
	movf	MB64E_CURRENT_BUTTON, W, BANKED
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR0 pointer to the button value depending on
;;  MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_FSR_ButtonValue
	;; (status of 8 buttons stored in a SR register)
	SET_BSR	MB64E_BASE
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
	rrf	MB64E_CURRENT_BUTTON, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x07
	addwf	FSR0L, F
	return
