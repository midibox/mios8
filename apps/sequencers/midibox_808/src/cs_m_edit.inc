; $Id$
;
; MBSEQ Menu "Edit"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_EDIT_GP_Callback
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on Edit Button status
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_EDIT_HOLD, BANKED, CS_M_EDIT_GP_Callback_Sel
CS_M_EDIT_GP_Callback_Normal
	;; determine step number depending on SEQ_SELECTED_STEP_VIEW[1:0],
	;; add offset to button number and store in SEQ_CURRENT_STEP and SEQ_EVNTS
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	andlw	0x30
	addwf	MIOS_PARAMETER1, W
	movwf	SEQ_CURRENT_STEP, BANKED
	movwf	SEQ_EVNTS, BANKED

	;; determine store mask used for XOR operation in TMP1
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	;; using CS_STAT2_TRK_CHANGED flag to determine, if any track is selected (and not disabled)
	bcf	CS_STAT2, CS_STAT2_TRK_CHANGED

	clrf	SEQ_EVNTT, BANKED
CS_M_EDIT_GP_Callback_Loop
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_EDIT_GP_Callback_Loop_All
	;; check if track selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	CS_M_EDIT_GP_Callback_Loop_Next
CS_M_EDIT_GP_Callback_Loop_All

	;; gate layer selected?
	BRA_IFCLR SEQ_SELECTED_LAYERS, 0, BANKED, CS_M_EDIT_GP_Callback_Loop_NotG
	;; NEW: gate and aux layer selected?
	;; -> go to new button handling which allows to set both flags with three button states
	BRA_IFSET SEQ_SELECTED_LAYERS, 1, BANKED, CS_M_EDIT_GP_Callback_Loop_GACrl

CS_M_EDIT_GP_Callback_Loop_Gate
	;; get pointer to gate layer
	call	SEQ_FSR_TrgGate
	rgoto	CS_M_EDIT_GP_Callback_Loop_GateC;ont

CS_M_EDIT_GP_Callback_Loop_NotG

	;; aux layer selected?
	BRA_IFCLR SEQ_SELECTED_LAYERS, 1, BANKED, CS_M_EDIT_GP_Callback_Loop_NotA
CS_M_EDIT_GP_Callback_Loop_Aux
	;; get pointer to aux layer
	call	SEQ_FSR_TrgAux

CS_M_EDIT_GP_Callback_Loop_GateC;ont
;; expecting base pointer to layer in FSR1, step in SEQ_EVNTS, step mask in TMP1
CS_M_EDIT_GP_Callback_Hlp_Stp
	;; add step offset
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F

	;; if this is the first time: xor bit and remember state (so that all other tracks are changed to same state)
	BRA_IFSET CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, CS_M_EDIT_GP_Callback_Hlp_StpNFr
CS_M_EDIT_GP_Callback_Hlp_Stp_Fr
	;; XOR
	movf	TMP1, W
	xorwf	INDF1, F

	;; remember state
	movf	INDF1, W
	andwf	TMP1, W
	bcf	TMP2, 0
	skpz
	bsf	TMP2, 0

	;; notify that trigger has been toggled
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED

CS_M_EDIT_GP_Callback_Hlp_StpNFr
	;; branch depending on 0/1 state of first modified step
	BRA_IFSET TMP2, 0, ACCESS, CS_M_EDIT_GP_Callback_Hlp_Stp_S1
CS_M_EDIT_GP_Callback_Hlp_Stp_S0
	comf	TMP1, W
	andwf	INDF1, F
	rgoto	CS_M_EDIT_GP_Callback_Hlp_StpEnd

CS_M_EDIT_GP_Callback_Hlp_Stp_S1
	movf	TMP1, W
	iorwf	INDF1, F
	;; 	rgoto	CS_M_EDIT_GP_Callback_Hlp_StpEnd
CS_M_EDIT_GP_Callback_Hlp_StpEnd

CS_M_EDIT_GP_Callback_Loop_NotA
	rgoto	CS_M_EDIT_GP_Callback_Loop_Next



	;; new: toggle gate/aux flags independent from layer with three button states
CS_M_EDIT_GP_Callback_Loop_GACrl

	;; if this is the first time: xor bit and remember state (so that all other tracks are changed to same state)
	BRA_IFSET CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, CS_M_EDIT_GP_Callback_Loop_NFr
CS_M_EDIT_GP_Callback_Loop_Fr
	;; aux layer selected? -> clear aux and gate flag
	call	SEQ_FSR_TrgAux
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F
	movf	INDF1, W
	andwf	TMP1, W
	bnz	CS_M_EDIT_GP_Callback_Loop_ClrGA

	;; gate layer selected? -> set aux flag
	call	SEQ_FSR_TrgGate
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F
	movf	INDF1, W
	andwf	TMP1, W
	bnz	CS_M_EDIT_GP_Callback_Loop_SetA

	;; set gate flag, clear aux flag
CS_M_EDIT_GP_Callback_Loop_SetG
	bsf	TMP2, 0
	bcf	TMP2, 1
	rgoto	CS_M_EDIT_GP_Callback_Loop_NFr

	;; set aux + gate flag
CS_M_EDIT_GP_Callback_Loop_SetA
	bsf	TMP2, 0
	bsf	TMP2, 1
	rgoto	CS_M_EDIT_GP_Callback_Loop_NFr

	;; clear aux + gate flag
CS_M_EDIT_GP_Callback_Loop_ClrGA
	bcf	TMP2, 0
	bcf	TMP2, 1
	;; 	rgoto	CS_M_EDIT_GP_Callback_Loop_NFr

CS_M_EDIT_GP_Callback_Loop_NFr
	;; notify that trigger has been toggled
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED

	;; 
	;; set/clear gate flag
	;; 
	call	SEQ_FSR_TrgGate
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F

	BRA_IFSET TMP2, 0, ACCESS, CS_M_EDIT_GP_Callback_Loop_G1
CS_M_EDIT_GP_Callback_Loop_G0
	comf	TMP1, W
	andwf	INDF1, F
	rgoto	CS_M_EDIT_GP_Callback_Loop_GC

CS_M_EDIT_GP_Callback_Loop_G1
	movf	TMP1, W
	iorwf	INDF1, F
	;; 	rgoto	CS_M_EDIT_GP_Callback_Loop_GC
CS_M_EDIT_GP_Callback_Loop_GC


	;; 
	;; set/clear aux flag
	;; 
	call	SEQ_FSR_TrgAux
	rlf	SEQ_EVNTS, W, BANKED
	swapf	WREG, W
	andlw	0x07
	addwf	FSR1L, F

	BRA_IFSET TMP2, 1, ACCESS, CS_M_EDIT_GP_Callback_Loop_A1
CS_M_EDIT_GP_Callback_Loop_A0
	comf	TMP1, W
	andwf	INDF1, F
	rgoto	CS_M_EDIT_GP_Callback_Loop_AC

CS_M_EDIT_GP_Callback_Loop_A1
	movf	TMP1, W
	iorwf	INDF1, F
	;; 	rgoto	CS_M_EDIT_GP_Callback_Loop_AC
CS_M_EDIT_GP_Callback_Loop_AC

CS_M_EDIT_GP_Callback_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_EDIT_GP_Callback_Loop

	;; print "track(s) disabled" message when no trigger has been toggled
	GOTO_IFCLR CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, SEQ_LCD_MsgTrkDisabled

	rgoto	CS_M_EDIT_GP_Callback_End


CS_M_EDIT_GP_Callback_Sel
	;; clear "all steps" flag
	bcf	SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED

	;; if multi selection not set (will be set after the first selection), clear all track selections
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_EDIT_HOLD_MULTISEL, BANKED, CS_M_EDIT_GP_Callback_SelNot1st
CS_M_EDIT_GP_Callback_Sel1st
	bsf	SEQ_MODE1, SEQ_MODE1_EDIT_HOLD_MULTISEL, BANKED	; arm multiselection flag
	clrf	SEQ_SELECTED_TRKS_0, BANKED
	clrf	SEQ_SELECTED_TRKS_1, BANKED
CS_M_EDIT_GP_Callback_SelNot1st

	;; toggle track selection depending on button number
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	btfss	MIOS_PARAMETER1, 3
	xorwf	SEQ_SELECTED_TRKS_0, BANKED
	btfsc	MIOS_PARAMETER1, 3
	xorwf	SEQ_SELECTED_TRKS_1, BANKED

	;; if no track selected anymore: set track which corresponds to button
	movf	SEQ_SELECTED_TRKS_0, W, BANKED
	bnz	CS_M_EDIT_GP_Callback_SelOk
	movf	SEQ_SELECTED_TRKS_1, W, BANKED
	bnz	CS_M_EDIT_GP_Callback_SelOk
CS_M_EDIT_GP_Callback_SelFix
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	btfss	MIOS_PARAMETER1, 3
	iorwf	SEQ_SELECTED_TRKS_0, BANKED
	btfsc	MIOS_PARAMETER1, 3
	iorwf	SEQ_SELECTED_TRKS_1, BANKED
CS_M_EDIT_GP_Callback_SelOk

	;; select display view of track
	movff	MIOS_PARAMETER1, SEQ_SELECTED_TRK

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; 	rgoto	CS_M_EDIT_GP_Callback_End
	
CS_M_EDIT_GP_Callback_End
 	rgoto	CS_M_EDIT_UpdateGPLEDs


;; --------------------------------------------------------------------------
CS_M_EDIT_UpdateGPLEDs
	;; branch depending on Edit Button status
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_EDIT_HOLD, BANKED, CS_M_EDIT_UpdateGPLEDs_Sel
CS_M_EDIT_UpdateGPLEDs_Normal
	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; transfer step trigger status (16 steps) to SEQ_GP_LED_[LR]
	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED

	;; get pointer to gate layer
	call	SEQ_FSR_TrgGate
	rlf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	andlw	0x06
	addwf	FSR1L, F

	;; copy pattern to SEQ_GP_LED_[LR]_A
	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR0_A
	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR1_A


	;; get pointer to aux layer
	call	SEQ_FSR_TrgAux
	rlf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	andlw	0x06
	addwf	FSR1L, F

	;; copy pattern to SEQ_GP_LED_[LR]_B
	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR0_B
	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR1_B

	return


CS_M_EDIT_UpdateGPLEDs_Sel
	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; if all flag set: light up all tracks
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_EDIT_UpdateGPLEDs_Sel_NotAll
CS_M_EDIT_UpdateGPLEDs_Sel_All
	;; light up all tracks
	setf	CS_MENU_GP_OVERLAY_DOUT_SR0_A
	setf	CS_MENU_GP_OVERLAY_DOUT_SR1_A
	rgoto	CS_M_EDIT_UpdateGPLEDs_Sel_End

CS_M_EDIT_UpdateGPLEDs_Sel_NotAll
	;; light up selected tracks
	movff	SEQ_SELECTED_TRKS_0, CS_MENU_GP_OVERLAY_DOUT_SR0_A
	movff	SEQ_SELECTED_TRKS_1, CS_MENU_GP_OVERLAY_DOUT_SR1_A

CS_M_EDIT_UpdateGPLEDs_Sel_End
	;; copy A into B LED layer (no dimming used)
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0_A, CS_MENU_GP_OVERLAY_DOUT_SR0_B
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1_A, CS_MENU_GP_OVERLAY_DOUT_SR1_B

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_EDIT_Init
	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_EDIT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; (LEDs updated in CS_M_EDIT_Tick)
	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_EDIT_Tick
	;; update LEDs
	call	CS_M_EDIT_UpdateGPLEDs

	;; layout:
	;; 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    EDIT          Step  1-16          Trigger View: Gate
	;; 00000001111111111111222222222233333333334444444444444455555566666666667777777777
	;;   *    o    o    o    *    o    o    o    *    o    o    o    *    o    o    o  

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD     1-16 Gate
	;; 0000001111111111
	;; *ooo*ooo*ooo*ooo
	;; 2222222222333333


	;; print segments 0-7
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0xf8
	skpz
	rgoto	CS_M_EDIT_Tick_Lower

;; ---
CS_M_EDIT_Tick_Upper
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_EDIT_Tick_0
	rgoto	CS_M_EDIT_Tick_1
	rgoto	CS_M_EDIT_Tick_2
	rgoto	CS_M_EDIT_Tick_3
	rgoto	CS_M_EDIT_Tick_4
	rgoto	CS_M_EDIT_Tick_5
	rgoto	CS_M_EDIT_Tick_6
	rgoto	CS_M_EDIT_Tick_7

CS_M_EDIT_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_1
#if DEFAULT_LCD_WIDTH == 80
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x00 + 6, "EDIT          "
#endif
	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_2
#if DEFAULT_LCD_WIDTH == 80
	call	SEQ_LCD_PrintStringAfterCall
	STRING	5, 0x00 + 20, "Step "
#else
	movlw	0x00 + 6
	call	MIOS_LCD_CursorSet
#endif
	;; print step view
	SET_BSR	SEQ_BASE
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED	; (switches between A/B/C/D view)
	andlw	0x30
	movwf	PRODL
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	'-'
	call	MIOS_LCD_PrintChar
	incf	PRODL, W
	addlw	15
	call	MIOS_LCD_PrintBCD2

	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_3
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 30
	call	MIOS_LCD_CursorSet
	movlw	10
	call	SEQ_LCD_PrintSpaces
#endif
	rgoto	CS_M_EDIT_Tick_Cont


CS_M_EDIT_Tick_4
#if DEFAULT_LCD_WIDTH == 80
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x80 + 0, "Trigger View: "
#endif
	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_5
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x80 + 14
#else
	movlw	0x00 + 12
#endif
	call	MIOS_LCD_CursorSet

	;; print name of selected trigger layer
	call	CS_M_HLP_GetSelectedTrk
	movff	WREG, SEQ_EVNTT

	;; branch depending on selected layer(s)
	SET_BSR	SEQ_BASE
	decf	SEQ_SELECTED_LAYERS, W, BANKED
	andlw	0x03
	bz	CS_M_EDIT_Tick_5_Gate
	addlw	-1
	bz	CS_M_EDIT_Tick_5_Aux
	rgoto	CS_M_EDIT_Tick_5_GateAux

CS_M_EDIT_Tick_5_Gate
	movlw	16			; will print "Gate "
	call	SEQ_TRG_PrnType		; (5 characters)
	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_5_Aux
	call	SEQ_FSR_TrkAssgn	; get aux assignment
	movf	INDF1, W		; print type
	call	SEQ_TRG_PrnType		; (5 characters)
	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_5_GateAux
	movlw	17			; will print "G/Aux"
	call	SEQ_TRG_PrnType		; (5 characters)
	rgoto	CS_M_EDIT_Tick_Cont


CS_M_EDIT_Tick_6
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x80 + 59
#else
	movlw	0x00 + 17
#endif
	call	MIOS_LCD_CursorSet
	movlw	11
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_EDIT_Tick_Cont


;; ---
CS_M_EDIT_Tick_Lower

	;; time to init trigger special chars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_ARROWS
	call	CS_SPECIAL_CHARS_Init

	;; print step trigger/auxiliary

	;; determine cursor position
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
#if DEFAULT_LCD_WIDTH == 80
	andlw	0x07
	mullw	5
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	btfsc	WREG, 3
	bsf	PRODL, 7	; switch to right display if item number >= 8
	movf	PRODL, W
#endif
	addlw	0x40		; offset to lower line
	call	MIOS_LCD_CursorSet

#if DEFAULT_LCD_WIDTH == 80
	;; print two spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces
#endif

	;; print triggers of current step
	SET_BSR	SEQ_BASE
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED	; (switches between A/B/C/D view)
	andlw	0x30
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	movwf	SEQ_EVNTS, BANKED
	movff	SEQ_SELECTED_TRK, SEQ_EVNTT

	;; determine special character
	;; (we have 4 for no selection, gate, aux, gate/aux
	call	SEQ_FSR_TrgGate
	call	SEQ_TRG_GetStepBit
	andlw	0x01
	movwf	TMP1
	call	SEQ_FSR_TrgAux
	call	SEQ_TRG_GetStepBit
	andlw	0x01
	skpz
	bsf	TMP1, 1

	;; print special character (offset 2 in CS_MENU_CHARSET_ARROWS)
	movf	TMP1, W
	addlw	2
	call	MIOS_LCD_PrintChar

#if DEFAULT_LCD_WIDTH == 80
	;; print two spaces
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
	;; 	rgoto	CS_M_EDIT_Tick_Cont

CS_M_EDIT_Tick_Cont
	;; Tick function will be called 24 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	24-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_EDIT_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_EDIT_Tick_End

CS_M_EDIT_Tick_End
	return
