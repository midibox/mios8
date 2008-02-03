; $Id$
;
; MBSEQ Menu "Track Shift"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKSHFT_Init
	;; max value: 0xff
	setf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; start at the middle
	movlw	0x80
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	TABLE_ADDR CS_M_TRKSHFT_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print static strings
	TABLE_ADDR CS_M_TRKSHFT_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRKSHFT_STR0 STRING 15, 0x00, "Shift Track:   "
#if DEFAULT_ENC_DATAWHEEL >= 0
CS_M_TRKSHFT_STR1 STRING 13, 0x40, "Use Datawheel"
#else
CS_M_TRKSHFT_STR1 STRING 14, 0x40, "Use Left/Right"
#endif

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKSHFT_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; cursor handler expects:
	;;  IN: cursor pos in CS_MENU_CURSOR_LCD_POS
	;;      number of spaces (cursor width) in CS_MENU_CURSOR_LCD_WIDTH
	movlw	0x0f
	movwf	CS_MENU_CURSOR_LCD_POS
	movlw	0x01
	movwf	CS_MENU_CURSOR_LCD_WIDTH
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKSHFT_ValueChanged
	;; shift range is just CS_MENU_PARAMETER_L & 0x0f
	SET_BSR	SEQ_BASE
	clrf	SEQ_EVNTT, BANKED
CS_M_TRKSHFT_ValueChangedLoop
	;; don't shift if in drum mode
	call	CS_M_Hlp_CheckDrumMode
	bz	CS_M_TRKSHFT_ValueChangedLNext

	;; check if track selected
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bz	CS_M_TRKSHFT_ValueChangedLNext

	;; shift layers
	lfsr	FSR0, SEQ_POT_VALUES_A_00
	rcall	CS_M_TRKSHFT_ValueChanged_Hlp
	lfsr	FSR0, SEQ_POT_VALUES_B_00
	rcall	CS_M_TRKSHFT_ValueChanged_Hlp
	lfsr	FSR0, SEQ_POT_VALUES_C_00
	rcall	CS_M_TRKSHFT_ValueChanged_Hlp

	;; shift mutes as well
	call	SEQ_FSR_TrkMuted0
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	TMP1
CS_M_TRKSHFT_ValueChanged_M_Loop
	;; rotate left
	rlf	POSTINC1, F
	rlf	POSTDEC1, F
	bcf	INDF1, 0
	skpnc
	bsf	INDF1, 0
	;; loop until TMP1 zero
	decfsz	TMP1, F
	rgoto	CS_M_TRKSHFT_ValueChanged_M_Loop

CS_M_TRKSHFT_ValueChangedLNext
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, CS_M_TRKSHFT_ValueChangedLoop

	;; trigger pot display page
	call	SEQ_MSG_Start

	;; set CS_MENU_PARAMETER_L to 0x80 again
	movlw	0x80
	movwf	CS_MENU_PARAMETER_L
	return


	;; ---
CS_M_TRKSHFT_ValueChanged_Hlp
	swapf	SEQ_EVNTT, W, BANKED
	andlw	0x30
	addwf	FSR0L, F
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	TMP1
CS_M_TRKSHFT_ValueChanged_Hlp_OL
	movlw	0x0f			; save last step in TMP3 and decrement pointer
	addwf	FSR0L, F
	movff	POSTDEC0, TMP3		
	movlw	0x0f			; init loop counter
	movwf	TMP2
CS_M_TRKSHFT_ValueChanged_Hlp_IL
	movff	POSTINC0, POSTDEC0	; save n-1 in n and decrement pointer
	movf	POSTDEC0, W		; decrement pointer again
	decfsz	TMP2, F			; loop 15 times
	rgoto	CS_M_TRKSHFT_ValueChanged_Hlp_IL

	movff	TMP3, PREINC0		; save stored last step in first step

	;; loop until TMP1 zero
	decfsz	TMP1, F
	rgoto	CS_M_TRKSHFT_ValueChanged_Hlp_OL
	return
