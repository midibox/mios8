; $Id$
;
; MIDIboxSEQ
; CC Input Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_Set
	rcall	SEQ_CC_TABLE_Get	; prepare a CC action
	;; now:
	;; value     is in MIOS_PARAMETER1
	;; command   is in MIOS_PARAMETER2
	;; parameter is in FSR1 (pointer)

SEQ_CCIN_Set_Cmd
	;; branch depending on command
	movf	MIOS_PARAMETER2, W
	bz	SEQ_CCIN_Cmd_NOP
	addlw	-1
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_CCIN_Cmd_4L
	rgoto	SEQ_CCIN_Cmd_4U
	rgoto	SEQ_CCIN_Cmd_CFG_4L
	rgoto	SEQ_CCIN_Cmd_CFG_4U
	rgoto	SEQ_CCIN_Cmd_BPM_4L
	rgoto	SEQ_CCIN_Cmd_BPM_4U
	rgoto	SEQ_CCIN_Cmd_Global_4L
	rgoto	SEQ_CCIN_Cmd_Global_4U

; ==========================================================================

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_NOP
	;; no jump to end, since no CS update is required
	;; 	rgoto	SEQ_CCIN_Cmd_End
	return

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_4L
	;; add group offset
	movf	SEQ_GROUP, W, BANKED
	mullw	0x40
	movf	PRODL, W
	addwf	FSR1L, F

SEQ_CCIN_Cmd_4L_NoGrp
	;; store value
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_4U
	;; add group offset
	movf	SEQ_GROUP, W, BANKED
	mullw	0x40
	movf	PRODL, W
	addwf	FSR1L, F

SEQ_CCIN_Cmd_4U_NoGrp
	;; store value
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_CFG_4L
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCIN_Cmd_4L_NoGrp

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_CFG_4U
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCIN_Cmd_4U_NoGrp

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_BPM_4L
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	movlw	0x80
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_BPM_Cont

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_BPM_4U
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	bcf	INDF1, 7
	btfsc	MIOS_PARAMETER1, 0
	bsf	INDF1, 7

SEQ_CCIN_Cmd_BPM_Cont
	movf	INDF1, W
	call	SEQ_BPM_Set
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_Global_4L
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCIN_Cmd_4L_NoGrp

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_Global_4U
	;; FSR1H differes from other CCs
	movlw	SEQ_BASE >> 8
	movwf	FSR1H
	rgoto	SEQ_CCIN_Cmd_4U_NoGrp

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_End
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; ==========================================================================


;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_SetLayerA
	lfsr	FSR1, SEQ_POT_VALUES_A_00
	;; 	rgoto	SEQ_CCIN_SetLayer_Cont

SEQ_CCIN_SetLayer_Cont
	andlw	0x3f
	movff	MIOS_PARAMETER1, PLUSW1
	return

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_SetLayerB
	lfsr	FSR1, SEQ_POT_VALUES_B_00
	rgoto	SEQ_CCIN_SetLayer_Cont

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_SetLayerC
	lfsr	FSR1, SEQ_POT_VALUES_C_00
	rgoto	SEQ_CCIN_SetLayer_Cont

