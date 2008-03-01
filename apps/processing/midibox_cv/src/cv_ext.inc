; $Id$
;
; MIDIbox CV
; Update function for CV outputs and gate pins
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Called from USER_Tick to update the AOUT channels and gates
;; --------------------------------------------------------------------------
CV_EXT_Update
	;; disable interrupts to ensure data consistency
	IRQ_DISABLE

	;; update AOUT channels
	call	AOUT_Update

	;; do nothing if gate bits have not been changed
	SET_BSR	GATES
	movf	GATES, W, BANKED
	xorwf	GATES_LAST, W, BANKED
	bz	CV_EXT_Update_Gates_Skip
CV_EXT_Update_Gates
	;; store new state
	movff	GATES, GATES_LAST

	;; set J5 pins if enabled
#if ENABLE_J5
	movff	CV_GATE_POLARITY, WREG
	xorwf	GATES, W, BANKED
	call	J5_DOUT_Set
#endif	

	;; set digital AOUT pins (if available)
	SET_BSR	GATES
	movff	CV_GATE_POLARITY, WREG
	xorwf	GATES, W, BANKED
	call	AOUT_DigitalPinsSet

CV_EXT_Update_Gates_Skip

	;; enable interrupts again
	IRQ_ENABLE

	return
