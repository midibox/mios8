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

	SET_BSR	SEQ_BASE
	;; determine step number depending on SEQ_SELECTED_STEPS[0],
	;; add offset to button number and store in SEQ_CURRENT_STEP and SEQ_EVNTS
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
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
	;; check if track selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	CS_M_EDIT_GP_Callback_Loop_Next

	;; do nothing if track is disabled
	call	CS_M_HLP_ChkTrkDisabled
	bz	CS_M_EDIT_GP_Callback_Loop_Next

	;; if in drum mode and no trigger layer assigned: toggle velocity instead of trigger
	call	CS_M_HLP_CheckDrumLayer
	bnz	CS_M_EDIT_GP_Callback_Loop_NoDr
CS_M_EDIT_GP_Callback_Loop_Dr

	;; toggle velocity of layer and step
	call	CS_M_HLP_GetSelectedLayer; save layer in TMP2
	movwf	TMP2
	call	CS_M_HLP_GetLayerVType	; if not velocity layer: switch to layer 0 (always ok)
	xorlw	SEQ_LAYER_V_VEL
	bz	CS_M_EDIT_GP_Callback_Loop_DrSwL
CS_M_EDIT_GP_Callback_Loop_DrSw0
	clrf	TMP2		; switch to layer A
CS_M_EDIT_GP_Callback_Loop_DrSwL
	;; select SEQ_PATTERN and SEQ_PATTERN_BANK depending on track
	call	CS_M_HLP_SetPatternAndBank
	;; SEQ_EVNTT and SEQ_EVNTS already preloaded
	;; get value and toggle it
	movf	TMP2, W		; selected layer
	call	SEQ_LAYER_GetVValue
	bz	CS_M_EDIT_GP_Callback_Loop_Dr_1
CS_M_EDIT_GP_Callback_Loop_Dr_0
	movlw	0x00
	rgoto	CS_M_EDIT_GP_Callback_Loop_Dr_C
CS_M_EDIT_GP_Callback_Loop_Dr_1
	movlw	0x64
	;; 	rgoto	CS_M_EDIT_GP_Callback_Loop_Dr_C
CS_M_EDIT_GP_Callback_Loop_Dr_C
	;; set new value
	movwf	MIOS_PARAMETER1	; value
	movf	TMP2, W		; selected layer
	call	SEQ_LAYER_SetVValue
	;; notify that trigger has been toggled (or here: value has been modified)
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED	
	rgoto	CS_M_EDIT_GP_Callback_Loop_Next

CS_M_EDIT_GP_Callback_Loop_NoDr

	;; get pointer to button trigger register depending on selected trigger layer
	call	SEQ_FSR_TrkTrgA_0
	call	CS_M_HLP_GetSelectedTrgLayer
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x0c
	btfsc	SEQ_EVNTS, 3, BANKED
	addlw	0x01
	btfsc	SEQ_EVNTS, 4, BANKED
	addlw	0x02
	addwf	FSR1L, F

	;; now we've three cases:
	;; a) ALL function active, but ALL button not pressed: invert complete trigger layer
	;; b) ALL function active and ALL button pressed: toggle step, set remaining steps to same new value
	;; c) ALL function not active: toggle step
	;; if DEFAULT_BEH_ALL_WITH_TRIGGERS is 0, we always apply case c)
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_EDIT_GP_Callback_Loop_NoDrC
#if DEFAULT_BEH_ALL_WITH_TRIGGERS
	BRA_IFSET CS_STAT2, CS_STAT2_ALL_SAME_VALUE, ACCESS, CS_M_EDIT_GP_Callback_Loop_NoDrB
CS_M_EDIT_GP_Callback_Loop_NoDrA ; case a)
	movlw	0xfc		; remove register offset again (less branches when we undo the last "add offset" step)
	andwf	FSR1L, F
	comf	POSTINC1, F	; invert step 1-8
	comf	POSTINC1, F	; invert step 9-16
	comf	POSTINC1, F	; invert step 17-24
	comf	POSTINC1, F	; invert step 25-32
	rgoto	CS_M_EDIT_GP_Callback_Loop_NoDrN

CS_M_EDIT_GP_Callback_Loop_NoDrB ; case b)
	;; check if current step is set or cleared
	movf	TMP1, W
	andwf	INDF1, W
	bz	CS_M_EDIT_GP_Callback_Loop_NoDB1
CS_M_EDIT_GP_Callback_Loop_NoDB0
	movlw	0xfc		; remove register offset
	andwf	FSR1L, F
	clrf	POSTINC1	; clear step 1-8
	clrf	POSTINC1	; clear step 9-16
	clrf	POSTINC1	; clear step 17-24
	clrf	POSTINC1	; clear step 25-32
	rgoto	CS_M_EDIT_GP_Callback_Loop_NoDrN

CS_M_EDIT_GP_Callback_Loop_NoDB1
	movlw	0xfc		; remove register offset
	andwf	FSR1L, F
	setf	POSTINC1	; set step 1-8
	setf	POSTINC1	; set step 9-16
	setf	POSTINC1	; set step 17-24
	setf	POSTINC1	; set step 25-32
	rgoto	CS_M_EDIT_GP_Callback_Loop_NoDrN
#endif

CS_M_EDIT_GP_Callback_Loop_NoDrC ; case c)
	;; XOR
	movf	TMP1, W
	xorwf	INDF1, F
	;; 	rgoto	CS_M_EDIT_GP_Callback_Loop_NoDrN

CS_M_EDIT_GP_Callback_Loop_NoDrN
	;; notify that trigger has been toggled
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED

CS_M_EDIT_GP_Callback_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_EDIT_GP_Callback_Loop

	;; print "track(s) disabled" message when no trigger has been toggled
	GOTO_IFCLR CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, SEQ_LCD_MsgTrkDisabled

	;; trigger edit screen display
	call	SEQ_MSG_Start

	;; 	rgoto	CS_M_EDIT_GP_Callback_End

CS_M_EDIT_GP_Callback_End
	;;  	rgoto	CS_M_EDIT_UpdateGPLEDs

	
;; --------------------------------------------------------------------------
CS_M_EDIT_UpdateGPLEDs
	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT

	;; transfer step trigger status (16 steps) to CS_MENU_GP_OVERLAY_DOUT_SR[01]
	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED

	;; special LED behaviour if a drum layer is displayed
	;; (velocity == 0: LED off, otherwise LED on)
	call	CS_M_HLP_CheckDrumLayer
	bnz	CS_M_EDIT_UpdateGPLEDs_NoDrum
CS_M_EDIT_UpdateGPLEDs_Drum

	;; if uppper steps are set SEQ_EVNTS offset (step 1-16 or 17-32)
	movlw	0x00
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_EVNTS, BANKED

	;; determine pointer to layer
	;; we assume, that the cache is filled - it has been preloaded by SEQ_PRFTCH
	call	CS_M_HLP_GetSelectedLayer; save layer in TMP2
	call	SEQ_LAYER_Hlp_LayerFSR

	;; get the velocity values of 16 steps and copy the status into CS_MENU_GP_OVERLAY_DOUT_SR[01]
	lfsr	FSR0, CS_MENU_GP_OVERLAY_DOUT_SR0
	call	SEQ_GP_Hlp_CopyDrum
	rgoto	CS_M_EDIT_UpdateGPLEDs_End


CS_M_EDIT_UpdateGPLEDs_NoDrum
	;; get pointer to button trigger register depending on selected trigger layer
	call	SEQ_FSR_TrkTrgA_0
	call	CS_M_HLP_GetSelectedTrgLayer
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x0c
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x02
	addwf	FSR1L, F

	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR0
	movff	POSTINC1, CS_MENU_GP_OVERLAY_DOUT_SR1

CS_M_EDIT_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_EDIT_Select
	;; -> same as SEQ_BUTTON_StepInc
	goto	SEQ_BUTTON_StepInc


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_EDIT_Init
	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_EDIT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; force complete refresh of message
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; begin with the first step
	call	SEQ_MSG_Handler

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_EDIT_Tick
	;; update LEDs
	call	CS_M_EDIT_UpdateGPLEDs

	;; branch to message handler until refresh counter is zero again
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	SEQ_MSG_Handler
	return
