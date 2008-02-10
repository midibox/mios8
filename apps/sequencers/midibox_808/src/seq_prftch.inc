; $Id$
;
; MIDIboxSEQ
; Cache Prefetching
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called from USER_Tick to prefetch the layers into cache
;; 
;;  The prefetch mechanism is especially useful for drum mode, where we
;;  need to know the velocity values in order to display the triggers correctly
;; 
;;  The system load is very low, since with each call only one step is loaded
;; --------------------------------------------------------------------------
SEQ_PRFTCH_Tick
	;; copy cache pointers
	movff	SEQ_PRFTCH_EVNTT, SEQ_EVNTT
	movff	SEQ_PRFTCH_EVNTS, SEQ_EVNTS

	;; determine appr. bank and pattern
	call	CS_M_HLP_SetPatternAndBank

	;; load value (this will fill the cache if it hasn't been loaded yet)
	SET_BSR	SEQ_BASE
	movf	SEQ_PRFTCH_LAYER, W, BANKED
	call	SEQ_LAYER_GetVValue
	;; (the value itself is not for interest)

	;; prepare next call of this function
	SET_BSR	SEQ_BASE
	;; increment layer, wrap at 3
	incf	SEQ_PRFTCH_LAYER, F, BANKED
	movlw	3-1
	cpfsgt	SEQ_PRFTCH_LAYER, BANKED
	rgoto SEQ_PRFTCH_Tick_End
	clrf	SEQ_PRFTCH_LAYER, BANKED

	;; increment step, wrap at 32
	incf	SEQ_PRFTCH_EVNTS, W, BANKED
	andlw	0x1f
	movwf	SEQ_PRFTCH_EVNTS, BANKED
	bnz	SEQ_PRFTCH_Tick_End

	;; increment track, wrap at 16
	incf	SEQ_PRFTCH_EVNTT, W, BANKED
	andlw	0x0f
	movwf	SEQ_PRFTCH_EVNTT, BANKED

SEQ_PRFTCH_Tick_End	
	return
