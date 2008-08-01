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
	JUMPTABLE_2BYTES 10 ; entries
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

;; ==========================================================================
