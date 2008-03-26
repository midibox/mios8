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
	;; don't change if we are already in the menu
	xorwf	CS_MENU_POS, W
	bnz	CS_M_HLP_ChangePage_New
	;; just only update screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_M_HLP_ChangePage_New
	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; branch to selected menu
	movf	TMP1, W
	goto	CS_MENU_ChangePage

;; --------------------------------------------------------------------------
;;  Help routine to return the pointer to selected trigger layer in FSR1
;;  If multiple layers are selected, show gate layer
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedTrgLayerPtr
	SET_BSR	SEQ_BASE

	;; simple if only two layers provided...
	BRA_IFCLR SEQ_SELECTED_LAYERS, 0, BANKED, CS_M_HLP_GetSelectedTrgLayerPtrA
CS_M_HLP_GetSelectedTrgLayerPtrG
	goto	SEQ_FSR_TrgGate
CS_M_HLP_GetSelectedTrgLayerPtrA
	goto	SEQ_FSR_TrgAux


;; --------------------------------------------------------------------------
;;  Help routine to return the number of the selected track
;;  Attention: also used by an interrupt service routine (SEQ_LED_UpdateIRQ)
;; --------------------------------------------------------------------------
CS_M_HLP_GetSelectedTrk
	movff	SEQ_SELECTED_TRK, WREG
	return

;; --------------------------------------------------------------------------
;;  Help routine to set a single track, and to update the SELECTED_TRKS_[01] flags
;; --------------------------------------------------------------------------
CS_M_HLP_SetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_SELECTED_TRK, BANKED
	call	MIOS_HLP_GetBitORMask
	BRA_IFSET SEQ_SELECTED_TRK, 3, BANKED, CS_M_HLP_SetSelectedTrk_1
CS_M_HLP_SetSelectedTrk_0
	movwf	SEQ_SELECTED_TRKS_0, BANKED
	clrf	SEQ_SELECTED_TRKS_1, BANKED
	return
CS_M_HLP_SetSelectedTrk_1
	movwf	SEQ_SELECTED_TRKS_1, BANKED
	clrf	SEQ_SELECTED_TRKS_0, BANKED
	return

;; --------------------------------------------------------------------------
;;  Help routine to print the track number
;; --------------------------------------------------------------------------
CS_M_HLP_PrnTrk
	movff	SEQ_SELECTED_TRK, WREG
	goto	MIOS_LCD_PrintHex1


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
;;  IN: CC number in WREG, Tracks in SEQ_SELECTED_TRKS_x
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
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_HLP_TrkCC_Set_Loop_All
	;; skip if track not selected
	rcall	CS_M_HLP_ChkTrkSelected
	bnz	CS_M_HLP_TrkCC_Set_Loop_Next
CS_M_HLP_TrkCC_Set_Loop_All

	movff	SEQ_EVNTT, MIOS_PARAMETER2
	movf	SEQ_EVNTS, W, BANKED
	call	SEQ_CCIN_Set
	SET_BSR	SEQ_BASE
CS_M_HLP_TrkCC_Set_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_HLP_TrkCC_Set_Loop
	return


;; --------------------------------------------------------------------------
;;  Help routine to check if a track SEQ_EVNTT is selected
;;  ZERO flag set if true
;;  USES: FSR1
;; --------------------------------------------------------------------------
CS_M_HLP_ChkTrkSelected
	lfsr	FSR1, SEQ_SELECTED_TRKS_0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x01
	addwf	FSR1L, F

	movf	SEQ_EVNTT, W, BANKED
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W

	btg	STATUS, Z	; toggle zero flag
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
	;; left side selects pattern group A-H
	;; 8 buttons at the right side (which select 1-8) should flash now
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	;; 8 buttons at the left side not
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	;; select pattern group A-H depending on pressed button --- don't change bank until 1-8 button has been pressed!

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

	;; add requested (or current) lower/upper bank flag
	movf	CS_PATTERN_NEW_BANK, W
	bz	CS_M_HLP_GPPatternSel_L_NoNewBnk
CS_M_HLP_GPPatternSel_L_NewBnk
	addlw	-1
	btfsc	WREG, 0
	bsf	TMP2, 6
	rgoto	CS_M_HLP_GPPatternSel_L_NewBnk_C
CS_M_HLP_GPPatternSel_L_NoNewBnk
	btfsc	TMP1, 6
	bsf	TMP2, 6
CS_M_HLP_GPPatternSel_L_NewBnk_C

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

	;; add requested (or current) lower/upper bank flag
	movf	CS_PATTERN_NEW_BANK, W
	bz	CS_M_HLP_GPPatternSel_R_NoNewBnk
CS_M_HLP_GPPatternSel_R_NewBnk
	addlw	-1
	bcf	TMP2, 6
	btfsc	WREG, 0
	bsf	TMP2, 6
CS_M_HLP_GPPatternSel_R_NoNewBnk

	;; change value (will also update LEDs)
	incf	TMP2, W
	movwf	CS_MENU_PARAMETER_L

CS_M_HLP_GPPatternSel_Cont
	return


;; --------------------------------------------------------------------------
;;  Help routine to set the cursor depending on the current segment
;;  in CS_MENU_REFRESH_ITEM_CTR
;;  It is assumed, that the two 2x40 displays are divided into 16 segments
;; --------------------------------------------------------------------------
CS_M_HLP_SetSegmentCursor
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw	0x80
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 3
	iorlw	0x40
	call	MIOS_LCD_CursorSet
	return

;; --------------------------------------------------------------------------
;;  Help routine to print 10 spaces
;;  Position depends on CS_MENU_REFRESH_ITEM_CTR
;;  It is assumed, that the two 2x40 displays are divided into 16 segments
;; --------------------------------------------------------------------------
CS_M_HLP_Print10Spaces
	;; set cursor
	rcall	CS_M_HLP_SetSegmentCursor

	;; print 10 spaces
	movlw	10
	goto	SEQ_LCD_PrintSpaces

;; --------------------------------------------------------------------------
;;  Help routine to print 10 characters
;;  Position depends on CS_MENU_REFRESH_ITEM_CTR
;;  Pointer to string in TBLPTR[LH]
;;  It is assumed, that the two 2x40 displays are divided into 16 segments
;; --------------------------------------------------------------------------
CS_M_HLP_Print10Chars
	;; set cursor
	rcall	CS_M_HLP_SetSegmentCursor

	;; print 10 characters
	movlw	10
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  Help routine to print the track name in normal mode, and to print "!REC! "
;;  in record mode
;;  always outputs 6 characters
;; --------------------------------------------------------------------------
CS_M_HLP_PrnTrkNameOrRec
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, CS_M_HLP_PrnTrkName
	movlw	'!'
	call	MIOS_LCD_PrintChar
	movlw	'R'
	call	MIOS_LCD_PrintChar
	movlw	'E'
	call	MIOS_LCD_PrintChar
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'!'
	call	MIOS_LCD_PrintChar
	movlw	' '
	goto	MIOS_LCD_PrintChar

CS_M_HLP_PrnTrkName
	movwf	PRODL
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_HLP_PrnTrkName_All
	;; if all tracks selected: print "ALL   "
	incf	SEQ_SELECTED_TRKS_0, W, BANKED
	bnz	CS_M_HLP_PrnTrkName_NotAll
	incf	SEQ_SELECTED_TRKS_1, W, BANKED
	bnz	CS_M_HLP_PrnTrkName_NotAll
CS_M_HLP_PrnTrkName_All
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	'L'
	call	MIOS_LCD_PrintChar
	movlw	'L'
	call	MIOS_LCD_PrintChar
	movlw	3
	goto	SEQ_LCD_PrintSpaces
	
CS_M_HLP_PrnTrkName_NotAll
	movf	PRODL, W
	goto	SEQ_TRK_PrnName
	