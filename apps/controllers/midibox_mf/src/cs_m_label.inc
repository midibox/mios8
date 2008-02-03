; $Id$
;
; MBMF Menu for editing fader label string and mode
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_LABEL_Exit
	;; store current label
	rcall	CS_M_LABEL_StoreBuffer

	;; since we temporary used the BANKNAME buffer for the labels,
	;; we have to restore the name here
	goto	MBMF_DUMP_RestoreBankName

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_LABEL_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; don't increment if mode == 0
	SET_BSR	MBMF_BASE
	movf	MBMF_BANKNAME_BEGIN, W, BANKED
	skpnz
	return

	;; increment cursor position
	incf	CS_MENU_CURSOR_POS, F
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_ELEMENT_LEN, W, BANKED
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	clrf CS_MENU_CURSOR_POS

	;; update menu parameter
	rgoto	CS_M_LABEL_Update_MenuParam


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_LABEL_Init
	;; begin with "mode" entry
	clrf	CS_MENU_CURSOR_POS

	rcall	CS_M_LABEL_UpdateBuffer
	;; (sets also the CS_MENU_PARAMETER_[LH] and CS_MENU_PARAMETER_MAX_[LH])

	;; install "changed" hook
	TABLE_ADDR CS_M_LABEL_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print header line
	TABLE_ADDR CS_M_LABEL_STR_0
	call	MIOS_LCD_PrintString

	;; init special characters for vertical bars
	goto	MBMF_LCD_InitVBars

CS_M_LABEL_STR_0 STRING 13, 0x00, "Label of Fader#"

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_LABEL_ValueChanged
	;; copy menu value to MBMF_BANKNAME[CS_MENU_CURSOR_POS]
	rcall	CS_M_LABEL_Hlp_GetFSR

	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_LABEL_ValueChanged_Mode
CS_M_LABEL_ValueChanged_String
	movf	CS_MENU_PARAMETER_L, W
	addlw	0x20
	movwf	INDF0
	return

CS_M_LABEL_ValueChanged_Mode
	;; convert CS_MENU_PARAMETER_L to mode parameter
	clrf	INDF0
	movf	CS_MENU_PARAMETER_L, W
	bz	CS_M_LABEL_ValueChanged_Mode0
CS_M_LABEL_ValueChanged_Modex
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	movwf	INDF0
	decf	CS_MENU_PARAMETER_L, W
	btfsc	WREG, 2
	bsf	INDF0, 5
	bsf	INDF0, 6

CS_M_LABEL_ValueChanged_Mode0
	;; update the label length and exit
	rgoto	CS_M_LABEL_Update_MenuParam


;; ==========================================================================
	;; returns pointer to MBMF_BANKNAME[CS_MENU_CURSOR_POS] in FSR0
CS_M_LABEL_Hlp_GetFSR
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	movf	CS_MENU_CURSOR_POS, W
	addwf	FSR0L, F
	return
	

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_LABEL_Tick
	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR CS_STAT, CS_STAT_DISPLAY_GLCD, ACCESS, MIOS_LCD_Cmd

	;; if fader has been changed, update buffer
	SET_BSR	MBMF_BASE
	movf	MBMF_LAST_FADER0, W, BANKED
	call	MBMF_FADER_Map
	xorwf	MBMF_CURRENT_ELEMENT, W, BANKED
	bz	CS_M_LABEL_Tick_NoChange
	rcall	CS_M_LABEL_StoreBuffer	; store current buffer
	rcall	CS_M_LABEL_UpdateBuffer	; copy new label name in buffer
	clrf	CS_MENU_CURSOR_POS	; reset cursor
CS_M_LABEL_Tick_NoChange

	;; print fader number at first line, 14th column
	movlw	0x0d
	call	MIOS_LCD_CursorSet
	movff	MBMF_CURRENT_ELEMENT, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD3	

	;; print label at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	call	CS_M_LABEL_PrintLabel

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movf	CS_MENU_CURSOR_POS, W
	decf	CS_MENU_CURSOR_POS, W
	skpc
	movff	MBMF_CURRENT_ELEMENT_LEN, WREG
	iorlw	0x40
	movwf	TMP1
	;; cursor == 0: print 4 spaces
	;; else: print 1 space
	movlw	0x01
	movf	CS_MENU_CURSOR_POS, F
	skpnz
	movlw	0x04
	movwf	TMP2
	goto	CS_MENU_CursorHandler


;; ==========================================================================
	;; initializes the text buffer (we are using MBMF_BANKNAME as temporary buffer)
CS_M_LABEL_UpdateBuffer
	SET_BSR	MBMF_BASE
	movf	MBMF_LAST_FADER0, W, BANKED
	call	MBMF_FADER_Map
	movwf	MBMF_CURRENT_ELEMENT, BANKED
	movwf	MBMF_CURRENT_FADER, BANKED

	;; calc address to label
	call	MBMF_ADDR_LabelEntry

	;; copy label to buffer
	clrf	TMP1		; used as loop counter
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
CS_M_LABEL_UpdateBuffer_CopyLoop
	call	MBMF_BANK_Read
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_M_LABEL_UpdateBuffer_CopyLoop

	;; update menu parameter
	rgoto	CS_M_LABEL_Update_MenuParam

;; ==========================================================================
	;; stores the current buffer in Bank
CS_M_LABEL_StoreBuffer
	;; calc address to label
	movff	MBMF_CURRENT_ELEMENT, WREG
	goto	MBMF_MEM_StoreFaderLabel
	

;; ==========================================================================
	;; updates the menu parameter, sets MAX value and the label length
CS_M_LABEL_Update_MenuParam
	rcall	CS_M_LABEL_Hlp_GetFSR
	clrf	CS_MENU_PARAMETER_H
	clrf	CS_MENU_PARAMETER_MAX_H
	SET_BSR	MBMF_BASE

	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_LABEL_Update_MenuParam_Str
CS_M_LABEL_Update_MenuParam_Mode
	;; set max value to 8 (9 different modes)
	movlw	0x08
	movwf	CS_MENU_PARAMETER_MAX_L
	;; convert mode value to CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_L
	BRA_IFCLR INDF0, 6, ACCESS, CS_M_LABEL_Update_MenuParam_Cont
	movf	INDF0, W
	andlw	0x03
	btfsc	INDF0, 5
	iorlw 0x04
	addlw	1
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_LABEL_Update_MenuParam_Cont

CS_M_LABEL_Update_MenuParam_Str
	movf	INDF0, W
	addlw	-0x20
	btfsc	WREG, 7
	movlw 0x00
	movwf	CS_MENU_PARAMETER_L
	;; set max value to 0x5f (0x60 different characters)
	movlw	0x5f
	movwf	CS_MENU_PARAMETER_MAX_L
	;; 	rgoto	CS_M_LABEL_Update_MenuParam_Cont

CS_M_LABEL_Update_MenuParam_Cont
	movf	MBMF_BANKNAME_BEGIN+0, W, BANKED
	bz	CS_M_LABEL_Update_MenuParam_Len0
	movlw	12
	btfss	MBMF_BANKNAME_BEGIN+0, 5, BANKED; (bar flag)
	movlw 13
	rgoto	CS_M_LABEL_Update_MenuParam_CL
CS_M_LABEL_Update_MenuParam_Len0
	movlw	0x0f
	;; 	rgoto	CS_M_LABEL_UpdateLabelLen_Cont
CS_M_LABEL_Update_MenuParam_CL
	movwf	MBMF_CURRENT_ELEMENT_LEN, BANKED
	return

;; ==========================================================================


	;; print the label from temporary MBMF_BANKNAME buffer
CS_M_LABEL_PrintLabel
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	movf	POSTINC0, W
	bz	CS_M_LABEL_PrintLabelEmpty
	movwf	TMP2		; save mode in TMP2, we need it later

	clrf	TMP1		; used as loop counter
CS_M_LABEL_PrintLabelLoop
	movf	POSTINC0, W
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	movff	MBMF_CURRENT_ELEMENT_LEN, WREG
	cpfseq	TMP1, ACCESS
	rgoto CS_M_LABEL_PrintLabelLoop

	;; get value
	movff	MBMF_CURRENT_ELEMENT, MBMF_CURRENT_FADER
	call	MBMF_FSR_FaderValue
	;; continue depending on value mode
	movf	TMP2, W
	andlw	0x03
	call	MBMF_LCD_PrintValue
	CALL_IFSET WREG, 5, ACCESS, MBMF_LCD_PrnVBar
	return

CS_M_LABEL_PrintLabelEmpty
	;; just print standard event string
	movff	MBMF_CURRENT_FADER, WREG
	goto	MBMF_LCD_PrintEvntStd_Fader
