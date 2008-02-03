; $Id$
;
; MIDIboxSEQ
; CC Output Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  reacts on a CC dump request
;; --------------------------------------------------------------------------
SEQ_CCOUT_Handler
	SET_BSR	SEQ_BASE
	;; do nothing if dump has not been requested
	btfss	SEQ_REQ, SEQ_REQ_CC_DUMP, BANKED
	return

	;; clear request
	bcf	SEQ_REQ, SEQ_REQ_CC_DUMP, BANKED

	clrf	TMP1			; TMP1 used as loop counter
SEQ_CCOUT_Handler_Loop
	movf	TMP1, W
	call	SEQ_CCOUT_Get		; get CC parameter value
	movwf	TMP2			; store value in TMP2
	BRA_IFSET TMP2, 7, ACCESS, SEQ_CCOUT_Handler_Next ; (returns WREG[7] if non-CC value)

	movlw	0xb0 | DEFAULT_MIDI_CHANNEL_CFG
	call	MIOS_MIDI_TxBufferPut
	movf	TMP1, W
	call	MIOS_MIDI_TxBufferPut
	movf	TMP2, W
	call	MIOS_MIDI_TxBufferPut

	movlw	8			; add a short delay of 8 mS for slow sequencers
	call	MIOS_Delay

SEQ_CCOUT_Handler_Next
	incf	TMP1, F			; loop until 128 (bit 8 set)
	BRA_IFCLR TMP1, 7, ACCESS, SEQ_CCOUT_Handler_Loop
	
	return

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_Get
	rcall	SEQ_CC_TABLE_Get	; prepare a CC action
	;; now:
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

SEQ_CCOUT_Set_Cmd
	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	bz	SEQ_CCOUT_Cmd_NOP
	addlw	-1
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_CCOUT_Cmd_4L
	rgoto	SEQ_CCOUT_Cmd_4U
	rgoto	SEQ_CCOUT_Cmd_CFG_4L
	rgoto	SEQ_CCOUT_Cmd_CFG_4U
	rgoto	SEQ_CCOUT_Cmd_BPM_4L
	rgoto	SEQ_CCOUT_Cmd_BPM_4U
	rgoto	SEQ_CCOUT_Cmd_Global_4L
	rgoto	SEQ_CCOUT_Cmd_Global_4U

; ==========================================================================


;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_NOP
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_CFG_4L
SEQ_CCOUT_Cmd_Global_4L
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCOUT_Cmd_4L_NoGrp
	
SEQ_CCOUT_Cmd_4L
	;; add group offset
	movf	SEQ_GROUP, W, BANKED
	mullw	0x40
	movf	PRODL, W
	addwf	FSR1L, F

SEQ_CCOUT_Cmd_4L_NoGrp
	;; return value
	movf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_CFG_4U
SEQ_CCOUT_Cmd_Global_4U
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCOUT_Cmd_4U_NoGrp
	
SEQ_CCOUT_Cmd_4U
	;; add group offset
	movf	SEQ_GROUP, W, BANKED
	mullw	0x40
	movf	PRODL, W
	addwf	FSR1L, F

SEQ_CCOUT_Cmd_4U_NoGrp
	;; return value
	swapf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_BPM_4L
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	movf	INDF1, W
	andlw	0x7f
	return

SEQ_CCOUT_Cmd_BPM_4U
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	movlw	0x00
	btfsc	INDF1, 7
	movlw 0x01
	return

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_GetLayerA
	lfsr	FSR1, SEQ_POT_VALUES_A_00
	;; 	rgoto	SEQ_CCOUT_GetLayer_Cont

SEQ_CCOUT_GetLayer_Cont
	andlw	0x3f
	movf	PLUSW1, W
	return

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_GetLayerB
	lfsr	FSR1, SEQ_POT_VALUES_B_00
	rgoto	SEQ_CCOUT_GetLayer_Cont

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_GetLayerC
	lfsr	FSR1, SEQ_POT_VALUES_C_00
	rgoto	SEQ_CCOUT_GetLayer_Cont

