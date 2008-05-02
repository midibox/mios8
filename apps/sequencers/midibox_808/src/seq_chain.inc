; $Id$
;
; MIDIboxSEQ
; Pattern Chain Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function clears the pattern chain
;; --------------------------------------------------------------------------
SEQ_CHAIN_Clr
	SET_BSR	SEQ_BASE

	clrf	PRODL
	lfsr	FSR1, SEQ_PATTERN_CHAIN
SEQ_CHAIN_ClrLoop
	clrf	POSTINC1
	incf	PRODL, F
	bnz	SEQ_CHAIN_ClrLoop

	clrf	SEQ_CHAIN_PLAY_PTR, BANKED
	clrf	SEQ_CHAIN_TOP, BANKED

	bsf	SEQ_STATE, SEQ_STATE_CHAIN_SYNCHED, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function adds a pattern/bank to the chain
;;  IN: pattern in SEQ_PATTERN, bank in SEQ_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_CHAIN_Add
	SET_BSR	SEQ_BASE

	;; exit if top of chain reached
	movf	SEQ_CHAIN_TOP, W
	xorlw	0x7f
	bz	SEQ_CHAIN_Add_End

	;; determine pointer to chain entry
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrc
	rlf	SEQ_CHAIN_TOP, W, BANKED
	addwf	FSR1L, F

	;; add bank and pattern to chain
	movff	SEQ_PATTERN_BANK, INDF1
	bsf	POSTINC1, 7	; (for song mode: set marker on bit #7, so that we are able to determine the TOP)
	movff	SEQ_PATTERN, POSTINC1

	;; increment top pointer
	incf	SEQ_CHAIN_TOP, F, BANKED

SEQ_CHAIN_Add_End
	return


;; --------------------------------------------------------------------------
;;  This function changes SEQ_PATTERN0/SEQ_PATTERN_BANK0 to the next chain
;;  entry.
;; --------------------------------------------------------------------------
SEQ_CHAIN_NextPos
	SET_BSR	SEQ_BASE

	;; notify that change has been taken
	bsf	SEQ_STATE, SEQ_STATE_CHAIN_SYNCHED, BANKED

	;; skip if chain is empty
	movf	SEQ_CHAIN_TOP, W, BANKED
	bz	SEQ_CHAIN_NextPos_End

	;; determine pointer to chain entry
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrc
	rlf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	addwf	FSR1L, F

	;; get bank and pattern from chain
	movf	POSTINC1, W
	andlw	0x07
	movwf	SEQ_PATTERN_BANK, BANKED
	movff	POSTINC1, SEQ_PATTERN

	;; increment play pointer and wrap on top entry
	incf	SEQ_CHAIN_PLAY_PTR, F, BANKED
	movf	SEQ_CHAIN_TOP, W, BANKED
	cpfslt	SEQ_CHAIN_PLAY_PTR, BANKED
	clrf	SEQ_CHAIN_PLAY_PTR


SEQ_CHAIN_PrevPos_Cont		; re-used from PrevPos function
	;; compare with currently played pattern
	;; don't change if identical
	movf	SEQ_PATTERN0, W, BANKED
	xorwf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bnz	SEQ_CHAIN_NextPos_New
	movf	SEQ_PATTERN_BANK0, W, BANKED
	xorwf	SEQ_PATTERN_BANK, W, BANKED
	bnz	SEQ_CHAIN_NextPos_New
	rgoto	SEQ_CHAIN_NextPos_Cont

SEQ_CHAIN_NextPos_New
	;; change to new pattern immediately
	movff	SEQ_PATTERN, SEQ_NEXT_PATTERN0
	movff	SEQ_PATTERN_BANK, SEQ_NEXT_PATTERN_BANK0
	rcall	SEQ_CORE_ChangePattern
	;; 	rgoto	SEQ_CHAIN_NextPos_Cont

SEQ_CHAIN_NextPos_Cont
	;; if oneshot mode: clear chain if last position reached
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_CHAIN_ONESHOT, BANKED, SEQ_CHAIN_NextPos_NoClear
	movf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	bnz	SEQ_CHAIN_NextPos_NoClear
SEQ_CHAIN_NextPos_Clear
	rcall	SEQ_CHAIN_Clr
SEQ_CHAIN_NextPos_NoClear

SEQ_CHAIN_NextPos_End
	return


;; --------------------------------------------------------------------------
;;  This function changes SEQ_PATTERN0/SEQ_PATTERN_BANK0 to the previous chain
;;  entry.
;; --------------------------------------------------------------------------
SEQ_CHAIN_PrevPos
	SET_BSR	SEQ_BASE

	;; notify that change has been taken
	bsf	SEQ_STATE, SEQ_STATE_CHAIN_SYNCHED, BANKED

	;; skip if chain is empty
	movf	SEQ_CHAIN_TOP, W, BANKED
	bz	SEQ_CHAIN_PrevPos_End

	;; decrement play pointer and wrap on first entry
	decf	SEQ_CHAIN_PLAY_PTR, F, BANKED
	bc	SEQ_CHAIN_PrevPos_NoWrap
SEQ_CHAIN_PrevPos_Wrap
	decf	SEQ_CHAIN_TOP, W, BANKED
	movwf	SEQ_CHAIN_PLAY_PTR, BANKED
SEQ_CHAIN_PrevPos_NoWrap

	;; determine pointer to chain entry
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrc
	rlf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	addwf	FSR1L, F

	;; get bank and pattern from chain
	movf	POSTINC1, W
	andlw	0x07
	movwf	SEQ_PATTERN_BANK, BANKED
	movff	POSTINC1, SEQ_PATTERN

	;; continue at NextPos function
	rgoto	SEQ_CHAIN_PrevPos_Cont


SEQ_CHAIN_PrevPos_End
	return
