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
;;  Help routine to determine the group offset
;;  Attention: also used by an interrupt service routine (SEQ_LED_UpdateIRQ)
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedGroupOffset
	SET_BSR	SEQ_BASE
	rlf	SEQ_SELECTED_GROUP, W, BANKED
	rlf	WREG, W
	andlw	0x0c
	return

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
;;  Help routine to return the number of the selected button trigger layer
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedTrgLayer
	SET_BSR	SEQ_BASE

	movlw	0x02
	btfsc	SEQ_SELECTED_TRG_LAYERS, 1, BANKED
	movlw 0x01
	btfsc	SEQ_SELECTED_TRG_LAYERS, 0, BANKED
	movlw 0x00
	return

;; --------------------------------------------------------------------------
;;  Help routine to return the layer type of the selected layer
;; --------------------------------------------------------------------------
CS_M_HLP_GetLayerVType
	call	SEQ_FSR_TrkEvnt
	movff	INDF1, MIOS_PARAMETER1
	rcall	CS_M_HLP_GetSelectedLayer
	goto	SEQ_LAYER_GetVType

;; --------------------------------------------------------------------------
;;  Help routine which sets the ZERO flag if the track is in "drum mode" (static note value, different gate trigger behaviour)
;; --------------------------------------------------------------------------
CS_M_HLP_CheckDrumMode
	call	SEQ_FSR_TrkEvnt
	movff	INDF1, MIOS_PARAMETER1
	goto	SEQ_LAYER_CheckDrumMode

;; --------------------------------------------------------------------------
;;  Help routine to return a MIDI event in SEQ_EVNT[012L] depending on selected layer
;; --------------------------------------------------------------------------
CS_M_HLP_GetEvent
	call	SEQ_FSR_TrkEvnt
	movff	INDF1, MIOS_PARAMETER1
	rcall	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_Map_LtoENum
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNT_NUMBER, BANKED
	goto	SEQ_LAYER_GetEvnt

;; --------------------------------------------------------------------------
;;  Help routine to return the number of the selected track
;;  Attention: also used by an interrupt service routine (SEQ_LED_UpdateIRQ)
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE

	rcall	CS_M_HLP_GetSelectedGroupOffset
	movwf	TABLAT

	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addwf	FSR1L, F
	movlw	0x03
	btfsc	INDF1, 2
	movlw 0x02
	btfsc	INDF1, 1
	movlw 0x01
	btfsc	INDF1, 0
	movlw 0x00
	iorwf	TABLAT, W
	return

;; --------------------------------------------------------------------------
;;  Help routine to print the track number at cursor position 0x0f
;; --------------------------------------------------------------------------
CS_M_HLP_PrnTrkAt0F
	movlw	0x0f
	call	MIOS_LCD_CursorSet
CS_M_HLP_PrnTrk
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	andlw	0x0f
	addlw	LOW(CS_M_HLP_PrnTrk_Tab)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_HLP_PrnTrk_Tab)
	addwf	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	goto	MIOS_LCD_PrintChar

CS_M_HLP_PrnTrk_Tab
	db	'-', '1', '2', 'M', '3', 'M', 'M', 'M'
	db	'4', 'M', 'M', 'M', 'M', 'M', 'M', 'A'


;; --------------------------------------------------------------------------
;;  Help routine to print the pattern number
;; --------------------------------------------------------------------------
CS_M_HLP_PrnG
	SET_BSR	SEQ_BASE
	incf	SEQ_SELECTED_GROUP, W, BANKED
	goto	MIOS_LCD_PrintBCD1


;; --------------------------------------------------------------------------
;;  Help routine to print the GxTx style pattern group/track number
;; --------------------------------------------------------------------------
CS_M_HLP_PrnGxTy
	call	CS_MENU_CursorHandler2
	bz	CS_M_HLP_PrnGxTy_End
CS_M_HLP_PrnGxTy_NoCursor
	movlw	'G'
	call	MIOS_LCD_PrintChar
	rcall	CS_M_HLP_PrnG
	movlw	'T'
	call	MIOS_LCD_PrintChar
	rcall	CS_M_HLP_PrnTrk
CS_M_HLP_PrnGxTy_End
	return

;; --------------------------------------------------------------------------
;;  Help routine to get a CC value depending on selected track
;;  IN: CC number (&0xfc) in WREG
;; --------------------------------------------------------------------------
CS_M_HLP_TrkCC_Get
	;; using PRODL as temporary register
	movwf	PRODL

	;; get value from selected track
	rcall	CS_M_HLP_GetSelectedTrk
	movwf	MIOS_PARAMETER2
	movf	PRODL, W
	goto	SEQ_CCOUT_Get

;; --------------------------------------------------------------------------
;;  Help routine to transfer the menu parameter to the CCs of all selected tracks
;;  IN: CC number in WREG, Tracks in SEQ_SELECTED_TRKS_Gx
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
	;; skip if track not selected
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	movf	PLUSW1, W
	andlw	0x0f
	movwf	PRODL
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	PRODL, W
	bz	CS_M_HLP_TrkCC_Set_Loop_Next
	movff	SEQ_EVNTT, MIOS_PARAMETER2
	movf	SEQ_EVNTS, W, BANKED
	call	SEQ_CCIN_Set
	SET_BSR	SEQ_BASE
CS_M_HLP_TrkCC_Set_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_HLP_TrkCC_Set_Loop
	return


;; --------------------------------------------------------------------------
;;  Help routine to transfer the menu parameter to SEQ_SELECTED_TRKS_Gx
;;  and SEQ_SELECTED_GROUP, and to select the appr. track
;; --------------------------------------------------------------------------
CS_M_HLP_GxTyChanged
	SET_BSR	SEQ_BASE
	rrf	CS_MENU_PARAMETER_L, W
	rrf	WREG, W
	andlw	0x03
	movwf	SEQ_SELECTED_GROUP, BANKED

	;; clear all current selections
	clrf	SEQ_SELECTED_TRKS_G0, BANKED
	clrf	SEQ_SELECTED_TRKS_G1, BANKED
	clrf	SEQ_SELECTED_TRKS_G2, BANKED
	clrf	SEQ_SELECTED_TRKS_G3, BANKED

	;; set new selection
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addwf	FSR1L, F

	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	iorlw   0xf0            ; (for correct function of the track buttons - no multi selections!)
	movwf	INDF1

	return


;; --------------------------------------------------------------------------
;;  Help routine to check if a track SEQ_EVNTT is selected
;;  ZERO flag set if true
;;  USES: FSR1
;; --------------------------------------------------------------------------
CS_M_HLP_ChkTrkSelected
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addwf	FSR1L, F

	movf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	andlw	0x0f

	btg	STATUS, Z	; toggle zero flag
	return

;; --------------------------------------------------------------------------
;;  Help routine to check if a track SEQ_EVNTT is disabled
;;  ZERO flag set if true
;;  USES: FSR1
;; --------------------------------------------------------------------------
CS_M_HLP_ChkTrkDisabled
	call	SEQ_FSR_TrkVarState
	movf	INDF1, W
	andlw	(1 << SEQ_TRKVARSTATE_DISABLED)

	btg	STATUS, Z	; toggle zero flag
	return

;; --------------------------------------------------------------------------
;;  Help routine to seq SEQ_PATTERN and SEQ_PATTERN_BANK depending on SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_HLP_SetPatternAndBank
	SET_BSR	SEQ_BASE

	lfsr	FSR1, SEQ_PATTERN0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x7f			; (bit #7 must be cleared)
	movwf	SEQ_PATTERN, BANKED

	movlw	(SEQ_PATTERN_BANK0-SEQ_PATTERN0)
	addwf	FSR1L, F
	movf	INDF1, W
	movwf	SEQ_PATTERN_BANK, BANKED
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

