; $Id$
;
; MIDIO128
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
;;  This function returns a FSR0 pointer to the button value depending on
;;  MIDIO_CURRENT_DIN
;; --------------------------------------------------------------------------
MIDIO_FSR_DINValue
	;; (status of 8 buttons stored in a SR register)
	SET_BSR	MIDIO_BASE
	lfsr	FSR0, MIDIO_DIN_VALUES_SR0
	rrf	MIDIO_CURRENT_DIN, W, BANKED
	rrf	WREG, F
	rrf	WREG, W
	andlw	0x07
	addwf	FSR0L, F
	return
