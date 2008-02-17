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
	JUMPTABLE_2BYTES 10 ; entries
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
SEQ_CCIN_Cmd_End
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; and exit
	return


;; ==========================================================================
