; $Id$
;
; MIDIboxSEQ
; Pots handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_AIN_NotifyChange in mb_ain.inc when a 
;;  pot has been moved. Input:
;;     o Pot number in MIOS_PARAMETER1
;;     o 7-bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_POT_Handler
	;; save pot number in SEQ_EVNTS and SEQ_CURRENT_STEP
	SET_BSR SEQ_BASE
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	SEQ_CURRENT_STEP, BANKED
	movwf	SEQ_EVNTS, BANKED

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; store current track number in SEQ_EVNTT and SEQ_SELECTED_TRKS
	movlw	0xf0
	andwf	SEQ_SELECTED_TRKS, F, BANKED
	swapf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_EVNTT, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_SELECTED_TRKS, F, BANKED

	;; save absolute value in TMP2
	movff	MIOS_PARAMETER2, TMP2


	;; loop through all selected tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_POT_Handler_Loop
	;; check if track selected
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bz	SEQ_POT_Handler_Loop_Next

	;; get parameter type
	call	SEQ_PTYPE_Get

	;; copy absolute value to TMP1
	movff	TMP2, TMP1

	;; reduced range on note(-like) events
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNT1, BANKED, SEQ_POT_Handler_NoNote
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_ARP, BANKED, SEQ_POT_Handler_NoNote
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_POT_Handler_NoNote
SEQ_POT_Handler_Note
	;; check transpose value, don't reduce range if transpose is -5 (== off)
	call	SEQ_FSR_TrkTransp
	movf	INDF1, W
	andlw	0x0f
	xorlw	-5 & 0x0f
	bz	SEQ_POT_Handler_NoNote

	rrf	TMP2, W
	rrf	WREG, W
	andlw	0x1f
	skpz
	addlw	0x3c - 16
	movwf	TMP1
	
SEQ_POT_Handler_NoNote	

	;; send value
	call	SEQ_POTENC_Send

SEQ_POT_Handler_Loop_Next
	SET_BSR	SEQ_BASE
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, SEQ_POT_Handler_Loop

	;; trigger pot display page
	call	SEQ_MSG_Start

	return
