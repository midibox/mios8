; $Id$
;
; MIDIboxSEQ
; handler for general purpose LEDs and general purpose buttons
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  general purpose button function
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
SEQ_GP_Button
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; the menu select function overlays everything
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_GP_Mode4_Button

	;; following code is normaly located in cs_menu_buttons.inc
	;; check for GP buttons if hook has been installed
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	SEQ_GP_Button_NoCS
SEQ_GP_Button_CS
	;; call the GP hook. 
	;; Parameter:
	;;    GP button number in MIOS_PARAMETER1: 0x00-0x0f
	;;    button value in MIOS_PARAMETER2 (forwarded)
	;;    encoder incrementer CS_MENU_ENC_INCREMENTER: always 0
	clrf	CS_MENU_ENC_INCREMENTER
	goto	CS_MENU_ExecMenuGPCallbackHook
SEQ_GP_Button_NoCS

	;; lower status: SEQ_STEP_EDIT mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_GP_Mode0_Button

	;; (song mode flag can be set together with another flag, therefore it has
	;;  the lowest priority)
	rgoto	SEQ_GP_Mode3_Button



;; --------------------------------------------------------------------------
;;  this function is called from SEQ_LED_Update to update the LEDs
;;  IN: a lot of different sequencer settings
;;  OUT: the status of the 16 GP LEDs in SEQ_GP_LED_[LR]
;; --------------------------------------------------------------------------
SEQ_GP_LED_Update
	;; clear the 16 GP LEDs
	SET_BSR	SEQ_BASE
	clrf	SEQ_GP_LED_L, BANKED
	clrf	SEQ_GP_LED_R, BANKED

	;; menu select LED overlays everything
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_GP_Mode4_LED

	;; skip GP_LED hook if edit screen is temporary displayed
	movf	CS_MENU_MESSAGE_CTR, W
	bnz	SEQ_GP_LED_Update_NoHook

	;; SEQ_GP_LED_[LR] hold the status of the GP buttons
	;; it will be transfered to the appr. DOUT registers on every SEQ_LED_UpdateIRQ cycle
	;; if MenuGPCallbackHook installed: show the GP status
	;; if MenuGPCallbackHook not installed: show play/song status depending on SEQ_MODE0
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	SEQ_GP_LED_Update_NoHook

SEQ_GP_LED_Update_Hook
	SET_BSR	SEQ_BASE

	movf	CS_MENU_GP_OVERLAY_DOUT_SR0, W
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, SEQ_GP_LED_Update_NoF1L
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	movlw 0x00
SEQ_GP_LED_Update_NoF1L
	movwf	SEQ_GP_LED_L, BANKED

	movf	CS_MENU_GP_OVERLAY_DOUT_SR1, W
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, SEQ_GP_LED_Update_NoF1R
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	movlw 0x00
SEQ_GP_LED_Update_NoF1R
	movwf	SEQ_GP_LED_R, BANKED

#if DEFAULT_SRM_ENABLED
#if DEFAULT_GP_DOUT_SR_L == DEFAULT_SRM_DOUT_L1
	rcall	SEQ_GP_SRM_OverlayCopy
#else
	rcall	SEQ_GP_SRM_NormalCopy
#endif
#endif
	return

	;; ------------------------------------------------------------------
SEQ_GP_LED_Update_NoHook

	;; lower status: SEQ_STEP_EDIT mode
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_GP_LED_Update_NotMode0
SEQ_GP_LED_Update_Mode0
	rcall	SEQ_GP_Mode0_LED
	rgoto	SEQ_GP_LED_Update_LEDM
SEQ_GP_LED_Update_NotMode0

	;; (song mode flag can be set together with another flag, therefore it has
	;;  the lowest priority)
	rcall	SEQ_GP_Mode3_LED
	;; 	rgoto	SEQ_GP_LED_Update_LEDM

SEQ_GP_LED_Update_LEDM

#if DEFAULT_SRM_ENABLED
	rcall	SEQ_GP_SRM_NormalCopy
#endif

	return


; ==========================================================================
; ==========================================================================
; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 0: SEQ_MODE0_STEP_EDIT
;; --------------------------------------------------------------------------
SEQ_GP_Mode0_Button
	SET_BSR	SEQ_BASE

	;; branch if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_GP_Mode0_Button_Depr

	;; store button number in SEQ_CURRENT_STEP and SEQ_EVNTS
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_CURRENT_STEP, BANKED
	movwf	SEQ_EVNTS, BANKED

	;; mute step defined in MIOS_PARAMETER1
	;; store mask used for XOR operation in TMP1
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	;; using CS_STAT2_TRK_CHANGED flag to determine, if any track is selected (and not disabled)
	bcf	CS_STAT2, CS_STAT2_TRK_CHANGED

	clrf	SEQ_EVNTT, BANKED
SEQ_GP_Mode0_Button_Loop
	;; check if track selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_GP_Mode0_Button_Next

	;; do nothing if track is disabled
	call	CS_M_HLP_ChkTrkDisabled
	bz	SEQ_GP_Mode0_Button_Next

	;; if in drum mode and no trigger layer assigned: toggle velocity instead of trigger
	rcall	SEQ_GP_Hlp_CheckDrum
	bnz	SEQ_GP_Mode0_Button_Loop_NoDrum
SEQ_GP_Mode0_Button_Loop_Drum
	;; toggle velocity of layer and step
	call	CS_M_HLP_GetSelectedLayer; save layer in TMP2
	movwf	TMP2
	call	CS_M_HLP_GetLayerVType	; if not velocity layer: switch to layer 0 (always ok)
	xorlw	SEQ_LAYER_V_VEL
	bz	SEQ_GP_Mode0_Button_Loop_DrumSwL
SEQ_GP_Mode0_Button_Loop_DrumSw0
	clrf	TMP2		; switch to layer A
SEQ_GP_Mode0_Button_Loop_DrumSwL
	;; select SEQ_PATTERN and SEQ_PATTERN_BANK depending on track
	call	CS_M_HLP_SetPatternAndBank
	;; SEQ_EVNTT and SEQ_EVNTS already preloaded
	;; get value and toggle it
	movf	TMP2, W		; selected layer
	call	SEQ_LAYER_GetVValue
	bz	SEQ_GP_Mode0_Button_Loop_Drum_1
SEQ_GP_Mode0_Button_Loop_Drum_0
	movlw	0x00
	rgoto	SEQ_GP_Mode0_Button_Loop_Drum_C
SEQ_GP_Mode0_Button_Loop_Drum_1
	movlw	0x64
	;; 	rgoto	SEQ_GP_Mode0_Button_Loop_Drum_C
SEQ_GP_Mode0_Button_Loop_Drum_C
	;; set new value
	movwf	MIOS_PARAMETER1	; value
	movf	TMP2, W		; selected layer
	call	SEQ_LAYER_SetVValue
	;; notify that trigger has been toggled (or here: value has been modified)
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED	
	rgoto	SEQ_GP_Mode0_Button_Next

SEQ_GP_Mode0_Button_Loop_NoDrum
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

	;; XOR
	movf	TMP1, W
	xorwf	INDF1, F

	;; notify that trigger has been toggled
	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED

SEQ_GP_Mode0_Button_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, SEQ_GP_Mode0_Button_Loop


	;; print "track(s) disabled" message when no trigger has been toggled
	GOTO_IFCLR CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, SEQ_LCD_MsgTrkDisabled
	
	;; trigger pot display page and hold it
	goto	SEQ_MSG_Hold

SEQ_GP_Mode0_Button_Depr
	;; trigger pot display page
	goto	SEQ_MSG_Start


	;; ------------------------------------------------------------------
SEQ_GP_Mode0_LED
	;; transfer step trigger status (16 steps) to SEQ_GP_LED_[LR]
	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED

	;; special LED behaviour if a drum layer is displayed
	;; (velocity == 0: LED off, otherwise LED on)
	rcall	SEQ_GP_Hlp_CheckDrum
	bnz	SEQ_GP_Mode0_LED_NoDrum
SEQ_GP_Mode0_LED_Drum

	;; if uppper steps are set SEQ_EVNTS offset (step 1-16 or 17-32)
	movlw	0x00
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_EVNTS, BANKED

	;; determine pointer to layer
	;; we assume, that the cache is filled - it has been preloaded by SEQ_PRFTCH
	call	CS_M_HLP_GetSelectedLayer; save layer in TMP2
	call	SEQ_LAYER_Hlp_LayerFSR

	;; get the velocity values of 16 steps and copy the status into SEQ_GP_LED_[LR]
	lfsr	FSR0, SEQ_GP_LED_L
	rcall	SEQ_GP_Hlp_CopyDrum
	rgoto	SEQ_GP_Mode0_LED_End


SEQ_GP_Mode0_LED_NoDrum
	;; get pointer to button trigger register depending on selected trigger layer
	call	SEQ_FSR_TrkTrgA_0
	call	CS_M_HLP_GetSelectedTrgLayer
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x0c
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	movlw	0x02
	addwf	FSR1L, F

	movff	POSTINC1, SEQ_GP_LED_L
	movff	POSTINC1, SEQ_GP_LED_R

SEQ_GP_Mode0_LED_End
	return
	

;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 2: SEQ_MODE0_PATTERN
;; --------------------------------------------------------------------------
	;; no special functions, handler jumps to PATTERN menu

;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 3: SEQ_MODE0_SONG
;; --------------------------------------------------------------------------
SEQ_GP_Mode3_Button
	return

	;; ------------------------------------------------------------------
SEQ_GP_Mode3_LED
	;; transfer song number to SEQ_GP_LED_[LR]
	movlw	0x01
	movwf	SEQ_GP_LED_L, BANKED
	clrf	SEQ_GP_LED_R, BANKED

#if DEFAULT_SRM_ENABLED
#if DEFAULT_GP_DOUT_SR_L == DEFAULT_SRM_DOUT_L1
	rcall	SEQ_GP_SRM_OverlayCopy
#else
	rcall	SEQ_GP_SRM_NormalCopy
#endif
#endif

	return

;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 4: SEQ_MODE0_MENU
;; --------------------------------------------------------------------------
SEQ_GP_Mode4_Button
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; branch to the change menu function of the control surface
	movf	MIOS_PARAMETER1, W
	call	CS_MENU_TABLE_GPMapGetPos
	goto	CS_MENU_ChangePage

	;; ------------------------------------------------------------------
SEQ_GP_Mode4_LED
	;; transfer menu pos to SEQ_GP_LED_[LR]
	call	CS_MENU_TABLE_GPMapGetLED
	movff	MIOS_PARAMETER1, SEQ_GP_LED_L
	movff	MIOS_PARAMETER2, SEQ_GP_LED_R

#if DEFAULT_SRM_ENABLED
#if DEFAULT_GP_DOUT_SR_L == DEFAULT_SRM_DOUT_L1
	rcall	SEQ_GP_SRM_OverlayCopy
#else
	rcall	SEQ_GP_SRM_NormalCopy
#endif
#endif

	return


;; ---------------------------------------------------------------------------
;;  Help Function which sets the ZERO flag when buttons should control a drum layer
;;  and LEDs should show a drum layer
SEQ_GP_Hlp_CheckDrum
	call	CS_M_HLP_CheckDrumMode
	bnz	SEQ_GP_Hlp_CheckDrum_End
	call	CS_M_HLP_GetSelectedTrgLayer
	call	SEQ_TRG_GetTAssigned
SEQ_GP_Hlp_CheckDrum_End
	return


;; ---------------------------------------------------------------------------
;;  This help function copies the selected drum pattern (16 steps) into INDF1 (16bit)
;;  IN: pointer to layer in FSR1, pointer to target location in FSR0
;;  increments FSR1 by 16 and FSR0 by 2
SEQ_GP_Hlp_CopyDrum

	;; clear target location (16 bit)
	clrf	POSTINC0
	clrf	POSTDEC0

	;; run loop which sets 1 on each step/bit which has velocity > 0
	clrf	PRODL		; loop counter (2 loops)
SEQ_GP_Hlp_CopyDrumLoopO
	movlw	1		; (OR mask)
	movwf	PRODH
SEQ_GP_Hlp_CopyDrumLoop
	movf	POSTINC1, W
	andlw	0x7f		; mask out "not cached" flag
	bz	SEQ_GP_Hlp_CopyDrumLoop_Next
	movf	PRODH, W
	iorwf	INDF0, F
SEQ_GP_Hlp_CopyDrumLoop_Next
	clrc			; shift OR mask
	rlf	PRODH, F
	bnz	SEQ_GP_Hlp_CopyDrumLoop
	movf	POSTINC0, W	; increment target pointer
	incf	PRODL, F	; 2 loops
	BRA_IFCLR PRODL, 1, ACCESS, SEQ_GP_Hlp_CopyDrumLoopO

	return


;; ---------------------------------------------------------------------------
;;  Help Function for SRM: copy SEQ_GP_LED_[LR] into TRK0_A_[LR], clear the rest
SEQ_GP_SRM_OverlayCopy
	;; clear LED arrays
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	lfsr	FSR2, SEQ_LEDM_TRK0_B_L
	movlw	8
	movwf	PRODL
SEQ_GP_SRM_OverlayCopy_Loop
	clrf	POSTINC0
	clrf	POSTINC2
	decfsz	PRODL, F
	rgoto	SEQ_GP_SRM_OverlayCopy_Loop

	;; copy SEQ_GP_LED_[LR] to the first row
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	movff	SEQ_GP_LED_L, POSTINC0
	movff	SEQ_GP_LED_R, POSTINC0
	return

;; ---------------------------------------------------------------------------
;;  Help Function for SRM: copy the trigger flags to the TRK0_A_[LR] array
;;  If a layer is assigned to drum mode: copy velocity flags instead
SEQ_GP_SRM_NormalCopy
	;; Matrix LEDs (prepared for 4x16 LEDs)
	SET_BSR	SEQ_BASE

	;; get track offset
	call	CS_M_HLP_GetSelectedGroupOffset
	movwf	SEQ_EVNTT, BANKED

	;; selected track -> TMP1
	call	CS_M_HLP_GetSelectedTrk
	movwf	TMP1

	;; determine first SEQ_EVNTS
	movlw	0x00
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_EVNTS, BANKED

	;; get pointer to trigger layer and store it in TMP[34]
	call	SEQ_FSR_TrkTrgA_0
	call	CS_M_HLP_GetSelectedTrgLayer
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x0c
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x02
	addwf	FSR1L, F
	movff	FSR1L, TMP3
	movff	FSR1H, TMP4

	;; using TMP2 as loop counter
	movlw	4
	movwf	TMP2
	;; prepare pointer to A/B colour
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	lfsr	FSR2, SEQ_LEDM_TRK0_B_L

SEQ_GP_SRM_NormalCopy_Loop
	;; special behaviour in drum mode: use velocity flags
	rcall	SEQ_GP_Hlp_CheckDrum	; note: this function uses FSR1!
	bnz	SEQ_GP_SRM_NormalCopy_Loop_NoDrm
SEQ_GP_SRM_NormalCopy_Loop_Drm
	;; determine pointer to layer
	;; we assume, that the cache is filled - it has been preloaded by SEQ_PRFTCH
	call	CS_M_HLP_GetSelectedLayer; save layer in TMP2
	call	SEQ_LAYER_Hlp_LayerFSR
	
	;; get the velocity values of 16 steps and copy the status into INDF0+0/INDF0+1
	rcall	SEQ_GP_Hlp_CopyDrum
	rgoto	SEQ_GP_SRM_NormalCopy_Loop_Next

SEQ_GP_SRM_NormalCopy_Loop_NoDrm
	movff	TMP3, FSR1L
	movff	TMP4, FSR1H

	;; first colour
	movff	POSTINC1, POSTINC0
	movff	POSTDEC1, POSTINC0

#if 0
	;; disabled - doesn't look so cool
	;; second colour: inverted _A
	movf	POSTDEC0, W	; (decrements)
	movf	POSTDEC0, W

	comf	POSTINC0, W
	movwf	POSTINC2
	comf	POSTINC0, W
	movwf	POSTINC2
#endif

SEQ_GP_SRM_NormalCopy_Loop_Next
	;; switch to trigger layer of next track
	movlw	SEQ_TRKRECORD_LENGTH
	addwf	TMP3, F
	incf	SEQ_EVNTT, F, BANKED

	decfsz	TMP2, F
	rgoto	SEQ_GP_SRM_NormalCopy_Loop

	return
