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
;;  IN:
;;    - index of CC parameter in WREG
;;    - value in MIOS_PARAMETER1
;;    - track in MIOS_PARAMETER2
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
	andlw	0x7f		; mask out SEQ_TRKx/SEQ_TRKVARx selection flag
	JUMPTABLE_2BYTES 12 ; entries
	rgoto	SEQ_CCIN_Cmd_NOP
	rgoto	SEQ_CCIN_Cmd_7
	rgoto	SEQ_CCIN_Cmd_6L
	rgoto	SEQ_CCIN_Cmd_3L
	rgoto	SEQ_CCIN_Cmd_4L
	rgoto	SEQ_CCIN_Cmd_4U
	rgoto	SEQ_CCIN_Cmd_B10
	rgoto	SEQ_CCIN_Cmd_B32
	rgoto	SEQ_CCIN_Cmd_B54
	rgoto	SEQ_CCIN_Cmd_B76
	rgoto	SEQ_CCIN_Cmd_TRKMODE_FLAGS
	rgoto	SEQ_CCIN_Cmd_CHANGE_STEP

; ==========================================================================

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_NOP
	;; no jump to end, since no CS update is required
	;; 	rgoto	SEQ_CCIN_Cmd_End
	return

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_7
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	movwf	INDF1
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_6L
	movlw	0xc0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_3L
	movlw	0xf8
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_4L
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_4U
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_B10
	movlw	0xfc
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_B32
	movlw	0xf3
	andwf	INDF1, F
	rlf	MIOS_PARAMETER1, W
	rlf	WREG, W
	andlw	0x0c
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_B54
	movlw	0xcf
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0x30
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_B76
	movlw	0x3f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	iorwf	INDF1, F
	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_TRKMODE_FLAGS
	movlw	0x07
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F

	;; UNSORTED flag mapped to [4]
	btfsc	MIOS_PARAMETER1, 4
	bsf	INDF1, SEQ_TRKMODE_FLAG_UNSORTED

	rgoto	SEQ_CCIN_Cmd_End

;; --------------------------------------------------------------------------
SEQ_CCIN_Cmd_CHANGE_STEP
	;; ignore if value == 0
	movf	MIOS_PARAMETER1, W
	bz	SEQ_CCIN_Cmd_End

	;; special case: first clock (no increment from sequencer)
	movlw	SEQ_TRKVARSTATEx
	BRA_IFCLR PLUSW1, SEQ_TRKVARSTATE_FIRST_CLK, ACCESS, SEQ_CCIN_Cmd_CHANGE_STEP_Not1st
SEQ_CCIN_Cmd_CHANGE_STEP_1st
	decf	MIOS_PARAMETER1, W
	rgoto	SEQ_CCIN_Cmd_CHANGE_STEP_Cont

SEQ_CCIN_Cmd_CHANGE_STEP_Not1st
	;; branch depending on current direction
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW1, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CCIN_Cmd_CHANGE_STEP_Bwd
SEQ_CCIN_Cmd_CHANGE_STEP_Fwd
	movf	MIOS_PARAMETER1, W
	addlw	-2
	rgoto	SEQ_CCIN_Cmd_CHANGE_STEP_Cont
SEQ_CCIN_Cmd_CHANGE_STEP_Bwd
	movf	MIOS_PARAMETER1, W
	;; 	rgoto	SEQ_CCIN_Cmd_CHANGE_STEP_Cont
SEQ_CCIN_Cmd_CHANGE_STEP_Cont
	andlw	0x1f		; supports up to 32 steps
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW1

	;;  	rgoto	SEQ_CCIN_Cmd_End

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
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A
	;; 	rgoto	SEQ_CCIN_SetLayer_Cont

SEQ_CCIN_SetLayer_Cont
	andlw	0x7f
	movff	MIOS_PARAMETER1, PLUSW1
	return

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_SetLayerB
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B
	rgoto	SEQ_CCIN_SetLayer_Cont

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
SEQ_CCIN_SetLayerC
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C
	rgoto	SEQ_CCIN_SetLayer_Cont

