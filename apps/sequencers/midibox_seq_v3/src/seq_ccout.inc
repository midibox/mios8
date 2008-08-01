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
;;  IN: index of CC parameter in WREG, track in MIOS_PARAMETER2
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
	andlw	0x7f		; mask out SEQ_TRKx/SEQ_TRKVARx selection flag
	JUMPTABLE_2BYTES 12 ; entries
	rgoto	SEQ_CCOUT_Cmd_NOP
	rgoto	SEQ_CCOUT_Cmd_7
	rgoto	SEQ_CCOUT_Cmd_6L
	rgoto	SEQ_CCOUT_Cmd_3L
	rgoto	SEQ_CCOUT_Cmd_4L
	rgoto	SEQ_CCOUT_Cmd_4U
	rgoto	SEQ_CCOUT_Cmd_B10
	rgoto	SEQ_CCOUT_Cmd_B32
	rgoto	SEQ_CCOUT_Cmd_B54
	rgoto	SEQ_CCOUT_Cmd_B76
	rgoto	SEQ_CCOUT_Cmd_TRKMODE
	rgoto	SEQ_CCOUT_Cmd_CHANGE_STEP

; ==========================================================================


;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_NOP
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_7
	movf	INDF1, W
	andlw	0x7f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_6L
	movf	INDF1, W
	andlw	0x3f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_3L
	movf	INDF1, W
	andlw	0x07
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_4L
	movf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_4U
	swapf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_B10
	movf	INDF1, W
	andlw	0x03
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_B32
	rrf	INDF1, W
	rrf	WREG, W
	andlw	0x03
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_B54
	swapf	INDF1, W
	andlw	0x03
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_B76
	swapf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_TRKMODE
	swapf	INDF1, W
	andlw	0x0f

	;; UNSORTED flag mapped to [4]
	btfsc	INDF1, SEQ_TRKMODE_FLAG_UNSORTED
	iorlw	(1 << 4)
	return

;; --------------------------------------------------------------------------
SEQ_CCOUT_Cmd_CHANGE_STEP
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW1, W	; return current step+1
	andlw	0x1f
	addlw	1
	return

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_GetLayerA
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_A
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
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_B
	rgoto	SEQ_CCOUT_GetLayer_Cont

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: CC value in WREG
;; --------------------------------------------------------------------------
SEQ_CCOUT_GetLayerC
	lfsr	FSR1, SEQ_LAYER_VALUES_TRK0_C
	rgoto	SEQ_CCOUT_GetLayer_Cont

