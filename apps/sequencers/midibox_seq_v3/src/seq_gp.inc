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

	;; the MENU select function overlays everything
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_GP_MenuSel_Button

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

#if DEFAULT_SRM_BUTTONS_ENABLED == 1 && DEFAULT_SRM_BUTTONS_NO_UI == 0
	;; branch back if no overlay function
	rgoto	SEQ_GP_SRM_Button_NoOverlay
#endif

	;; nothing to do...
	return


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

	;; MENU select LED overlays everything
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_GP_MenuSel_LED

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
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_GP_SRM_NormalCopy
	rgoto	SEQ_GP_SRM_OverlayCopy
#else
	rgoto	SEQ_GP_SRM_NormalCopy
#endif
#endif
	return

	;; ------------------------------------------------------------------
SEQ_GP_LED_Update_NoHook

#if DEFAULT_SRM_ENABLED
	;; update the 4x16 LED matrix
	rcall	SEQ_GP_SRM_NormalCopy
#endif

	return


; ==========================================================================
; ==========================================================================
; ==========================================================================
; ==========================================================================


;; --------------------------------------------------------------------------
;;  LED/Button functions for SEQ_MODE0_MENU
;; --------------------------------------------------------------------------
SEQ_GP_MenuSel_Button
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
SEQ_GP_MenuSel_LED
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



;; --------------------------------------------------------------------------
;;  LED/Button functions for Button/DuoLED matrix
;;    o GP Button number (0-15) in MIOS_PARAMETER1
;;    o Button value (0/1) in MIOS_PARAMETER2
;;    o SR row (0-7) in SEQ_BLM_COL_CTR
;; --------------------------------------------------------------------------
SEQ_GP_SRM_Button
	SET_BSR	SEQ_BASE

	;; select track depending on BLM row
#if DEFAULT_SRM_BUTTONS_NO_UI == 0
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_GP_SRM_Button_NoTrkSel	; if BLM also used for UI: only track selection in edit page
#endif
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED, SEQ_GP_SRM_Button_NoTrkSel	; no track selection in pattern page 
SEQ_GP_SRM_Button_TrkSel	
	lfsr	FSR1, SEQ_SELECTED_TRKS_G0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addwf	FSR1L, F
	rrf	SEQ_BLM_COL_CTR, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	iorlw	0xf0
	movwf	INDF1
SEQ_GP_SRM_Button_NoTrkSel


#if DEFAULT_SRM_BUTTONS_NO_UI == 0
	;; branch to "normal" GP button handler
	goto	SEQ_GP_Button
	;; back-branch if no overlay function
SEQ_GP_SRM_Button_NoOverlay
#endif

	SET_BSR	SEQ_BASE

	;; in pattern page, select new pattern
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED, SEQ_GP_SRM_Button_NoPtn
SEQ_GP_SRM_Button_Ptn
	;; group number -> SEQ_GROUP and SEQ_SELECTED_GROUP
	rrf	SEQ_BLM_COL_CTR, W, BANKED
	andlw	0x03
	movwf	SEQ_GROUP, BANKED
	movwf	SEQ_SELECTED_GROUP, BANKED

	;; current pattern -> SEQ_PATTERN
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movff	PLUSW1, SEQ_PATTERN

	;; current bank -> SEQ_PATTERN_BANK
	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movff	PLUSW1, SEQ_PATTERN_BANK

	;; modify bitfield depending on button number
	BRA_IFCLR MIOS_PARAMETER1, 3, ACCESS, SEQ_GP_SRM_Button_Ptn_U
SEQ_GP_SRM_Button_Ptn_L
	movlw	0x78
	andwf	SEQ_PATTERN, F, BANKED
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	SEQ_PATTERN, F, BANKED
	rgoto	SEQ_GP_SRM_Button_Ptn_Cont

SEQ_GP_SRM_Button_Ptn_U
	movlw	0x47
	andwf	SEQ_PATTERN, F, BANKED
	swapf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x38
	iorwf	SEQ_PATTERN, F, BANKED
	;; 	rgoto	SEQ_GP_SRM_Button_Ptn_Cont

SEQ_GP_SRM_Button_Ptn_Cont
	;; change pattern and exit
	goto	SEQ_CORE_ChangePatternSynched


SEQ_GP_SRM_Button_NoPtn
	;; branch to button handler of the EDIT page, which activates/deactivates the triggers
	goto	CS_M_EDIT_GP_Callback


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

	;; in pattern page, display pattern selection of all groups instead
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED, SEQ_GP_SRM_NormalCopy_NoPtn
SEQ_GP_SRM_NormalCopy_Ptn
	;; prepare pointer to A/B colour and pattern array
	lfsr	FSR0, SEQ_LEDM_TRK0_A_L
	lfsr	FSR2, SEQ_LEDM_TRK0_B_L
	lfsr	FSR1, SEQ_NEXT_PATTERN0

	;; using TMP1 as loop counter
	clrf	TMP1
SEQ_GP_SRM_NormalCopy_PtnLoop
	clrf	POSTINC2	; red LEDs off
	clrf	POSTINC2

#if 0
	;; copy track selection - if it matches with selected group, print overlay registers instead
	movf	TMP1, W
	xorwf	SEQ_SELECTED_GROUP, W, BANKED
	bz	SEQ_GP_SRM_NormalCopy_PtnLoop_Ov
#endif
	;; (disabled - maybe better to show the pattern which is currently played)

	movf	TMP1, W
	BRA_IFSET PLUSW1, 7, ACCESS, SEQ_GP_SRM_NormalCopy_PtnLoop_Mu
	rlf	PLUSW1, W	; A/B/...H selection
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	POSTINC0
	movf	TMP1, W		; 1..8 selection
	movf	PLUSW1, W
	call	MIOS_HLP_GetBitORMask
	movwf	POSTINC0
	rgoto	SEQ_GP_SRM_NormalCopy_PtnLoopNxt

SEQ_GP_SRM_NormalCopy_PtnLoop_Mu
	clrf	POSTINC0	; pattern muted - green LEDs off
	clrf	POSTINC0
	rgoto	SEQ_GP_SRM_NormalCopy_PtnLoopNxt

SEQ_GP_SRM_NormalCopy_PtnLoop_Ov
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, POSTINC0
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, POSTINC0
	;; 	rgoto	SEQ_GP_SRM_NormalCopy_PtnLoopNxt
	
SEQ_GP_SRM_NormalCopy_PtnLoopNxt
	incf	TMP1, F
	BRA_IFCLR TMP1, 2, ACCESS, SEQ_GP_SRM_NormalCopy_PtnLoop
	return
	

SEQ_GP_SRM_NormalCopy_NoPtn
	;; get track offset
	call	CS_M_HLP_GetSelectedGroupOffset
	movwf	SEQ_EVNTT, BANKED

#if 0	; not used anymore
	;; selected track -> TMP1
	call	CS_M_HLP_GetSelectedTrk
	movwf	TMP1
#endif

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
	call	CS_M_HLP_CheckDrumLayer ; note: this function uses FSR1!
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

;; ---------------------------------------------------------------------------
;;  This help function copies the selected drum pattern (16 steps) into INDF1 (16bit)
;;  IN: pointer to layer in FSR1, pointer to target location in FSR0
;;  increments FSR1 by 16 and FSR0 by 2
;; --------------------------------------------------------------------------
SEQ_GP_Hlp_CopyDrum
	;; clear target location (16 bit)
	clrf	POSTINC0
	clrf	POSTDEC0

	;; run loop which sets 1 on each step/bit which has velocity > 0
	clrf	PRODL		; loop counter (2 loops)
SEQ_GP_Hlp_CopyDrum_Loop0
	movlw	1		; (OR mask)
	movwf	PRODH
SEQ_GP_Hlp_CopyDrum_Loop
	movf	POSTINC1, W
	andlw	0x7f		; mask out "not cached" flag
	bz	SEQ_GP_Hlp_CopyDrum_Loop_Next
	movf	PRODH, W
	iorwf	INDF0, F
SEQ_GP_Hlp_CopyDrum_Loop_Next
	clrc			; shift OR mask
	rlf	PRODH, F
	bnz	SEQ_GP_Hlp_CopyDrum_Loop
	movf	POSTINC0, W	; increment target pointer
	incf	PRODL, F	; 2 loops
	BRA_IFCLR PRODL, 1, ACCESS, SEQ_GP_Hlp_CopyDrum_Loop0

	return

