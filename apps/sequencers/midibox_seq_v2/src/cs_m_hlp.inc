; $Id$
;
; Help Functions for the MBSEQ Menu pages
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Help routine to change to a menu page
;; --------------------------------------------------------------------------
CS_M_HLP_ChangePage
	;; save page number in TMP1
	movwf	TMP1
	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; branch to selected menu
	movf	TMP1, W
	goto	CS_MENU_ChangePage


;; --------------------------------------------------------------------------
;;  Help routine to return the number of the selected layer
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedLayer
	SET_BSR	SEQ_BASE

	movlw	0x02
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw 0x01
	btfsc	SEQ_SELECTED_LAYERS, 0, BANKED
	movlw 0x00
	return

;; --------------------------------------------------------------------------
;;  Help routine to return the number of the selected track
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE

	movlw	0x03
	btfsc	SEQ_SELECTED_TRKS, 2, BANKED
	movlw 0x02
	btfsc	SEQ_SELECTED_TRKS, 1, BANKED
	movlw 0x01
	btfsc	SEQ_SELECTED_TRKS, 0, BANKED
	movlw 0x00
	return

;; --------------------------------------------------------------------------
;;  Help routine which returns 0x00 if we are in drum mode
;;  expecting track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_Hlp_CheckDrumMode
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x0f
	xorlw	SEQ_TRKMODE_DRUMS
	return

;; --------------------------------------------------------------------------
;;  Help routine which returns the D_TRK_x_y pointer depending on selected track/layer
;; --------------------------------------------------------------------------
CS_M_Hlp_Get_D_TRK_x_y
	rcall	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED
	rcall	CS_M_HLP_GetSelectedLayer
	movwf	SEQ_EVNTD, BANKED
	goto	SEQ_FSR_D_Trkx_y


;; --------------------------------------------------------------------------
;;  Help routine to print the track number at cursor position 0x0f
;; --------------------------------------------------------------------------
CS_M_HLP_PrnTrkAt0F
	movlw	0x0f
	call	MIOS_LCD_CursorSet
CS_M_HLP_PrnTrk
	TABLE_ADDR CS_M_HLP_PrnTrkAt0F_Tab
	movff	SEQ_SELECTED_TRKS, WREG
	andlw	0x0f
	TABLE_ADD_W
	tblrd*+
	movf	TABLAT, W
	goto	MIOS_LCD_PrintChar

CS_M_HLP_PrnTrkAt0F_Tab
	db	'-', '1', '2', 'M', '3', 'M', 'M', 'M'
	db	'4', 'M', 'M', 'M', 'M', 'M', 'M', 'A'


;; --------------------------------------------------------------------------
;;  Help routine to get a CC value depending on selected track
;;  IN: CC number (&0xfc) in WREG
;; --------------------------------------------------------------------------
CS_M_HLP_TrkCC_Get
	;; using TABLAT as temporary register
	movwf	TABLAT

	;; get value from selected track
	rcall	CS_M_HLP_GetSelectedTrk
	addwf	TABLAT, W
	goto	SEQ_CCOUT_Get

;; --------------------------------------------------------------------------
;;  Help routine to transfer the menu parameter to the CCs of all selected tracks
;;  IN: CC number in WREG, Tracks in SEQ_SELECTED_TRKS
;; --------------------------------------------------------------------------
CS_M_HLP_TrkCC_Set
	SET_BSR	SEQ_BASE

	;; (mis-)use SEQ_EVNTS to save CC number
	movwf	SEQ_EVNTS, BANKED

	;; move CS_MENU_PARAMETER_L -> MIOS_PARAMETER1
	movff	CS_MENU_PARAMETER_L, MIOS_PARAMETER1

	;; loop through the tracks, using SEQ_EVNTT as loop counter
	clrf	SEQ_EVNTT, BANKED
CS_M_HLP_TrkCC_Set_Loop
	;; select CC: SEQ_EVNTS + selected track
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	bz	CS_M_HLP_TrkCC_Set_Loop_Next
	movf	SEQ_EVNTT, W, BANKED
	addwf	SEQ_EVNTS, W, BANKED
	call	SEQ_CCIN_Set
	SET_BSR	SEQ_BASE
CS_M_HLP_TrkCC_Set_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, CS_M_HLP_TrkCC_Set_Loop
	return


;; --------------------------------------------------------------------------
;;  Help routine to transfer the menu parameter to SEQ_SELECTED_TRKS
;;  and to select the appr. track
;; --------------------------------------------------------------------------
CS_M_HLP_TrkChanged
	SET_BSR	SEQ_BASE
	movlw	0xf0
	andwf	SEQ_SELECTED_TRKS, F, BANKED

	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	btfsc	CS_MENU_PARAMETER_L, 2
	movlw 0x0f
	andlw	0x0f
	iorwf	SEQ_SELECTED_TRKS, F, BANKED

	return



;; --------------------------------------------------------------------------
;;  Help routine to select a pattern with the GP buttons
;;  can be called from CS_M_xxx_GP_Callback
;; --------------------------------------------------------------------------
CS_M_HLP_GPPatternSel
	;; save current pattern number in TMP2
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x00
	movwf	TMP2

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_HLP_GPPatternSel_R
CS_M_HLP_GPPatternSel_L
	;; left side selects pattern group A-H/a-h
	;; 8 buttons at the right side (which select 1-8) should flash now
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	;; 8 buttons at the left side not
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	;; select pattern group A-H/a-h depending on pressed button --- don't change bank until 1-8 button has been pressed!

	;; mask out first 3 bits (save current pattern number in TMP1)
	movf	TMP2, W
	movwf	TMP1
	andlw	0x07
	movwf	TMP2

	;; add button number [2..0] * 8 to pattern number
	movf	MIOS_PARAMETER1, W
	mullw	8
	movf	PRODL, W
	andlw	0x38
	iorwf	TMP2, F

	;; if new pattern number equal to old button number, toggle 6th bit
	movf	TMP1, W
	xorwf	TMP2, W
	skpnz
	btg	TMP2, 6

	;; save result
	incf	TMP2, W
	movwf	CS_MENU_PARAMETER_L

	;; continue
	rgoto	CS_M_HLP_GPPatternSel_Cont

CS_M_HLP_GPPatternSel_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	TMP2, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	TMP2, F

	;; change value (will also update LEDs)
	incf	TMP2, W
	movwf	CS_MENU_PARAMETER_L

CS_M_HLP_GPPatternSel_Cont
	return

