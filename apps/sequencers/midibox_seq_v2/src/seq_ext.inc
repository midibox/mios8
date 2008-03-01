; $Id$
;
; MIDIbox SEQ
; AOUT and Gate update
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function should be called from the USER_Tick loop to update
;;  AOUT and Gates
;; --------------------------------------------------------------------------
SEQ_EXT_Update
	;; disable interrupts to ensure data consistency
	IRQ_DISABLE

	;; update AOUT channels
	call	AOUT_Update

	;; check if gates have to be updated
	SET_BSR	GATES
	movf	GATES, W
	xorwf	GATES_LAST, W, BANKED
	bz	SEQ_EXT_Update_Gates_Skip
SEQ_EXT_Update_Gates
	movff	GATES, GATES_LAST

	SET_BSR	GATES
	movf	GATES, W, BANKED
	call	AOUT_DigitalPinsSet

SEQ_EXT_Update_Gates_Skip
	;; enable interrupts again
	IRQ_ENABLE

	return
