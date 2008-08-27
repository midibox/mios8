; $Id$
;
; MIDIboxSEQ
; Encoder handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when the datawheel has
;;  been moved. Input:
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Datawheel
	SET_BSR	SEQ_BASE
	;; branch in scrub mode
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SCRUB, BANKED, SEQ_ENC_Datawheel_Scrub

	;; if we are in page #0, use the data wheel to modify the "active step"
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_ENC_Datawheel_NotP0
	movf	CS_MENU_POS, W
	bz	SEQ_ENC_Datawheel_P0
SEQ_ENC_Datawheel_NotP0

	;; else branch to the common CS function for the datawheel
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_Enc

	;; ---
SEQ_ENC_Datawheel_Scrub
	;; in scrub mode: enable sequencer if not already running
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_ENC_Datawheel_ScrubRun
	movlw	(1 << SEQ_REQ_START)
	movwf	SEQ_REQ, BANKED	
SEQ_ENC_Datawheel_ScrubRun
	;; do nothing on decrements (handling for reverse scrub would be too difficult)
	btfsc	MIOS_PARAMETER2, 7
	return
	;; release pause if set
	bcf	SEQ_STATE, SEQ_STATE_PAUSE, BANKED
	;; add some ticks to running clock
	movf	MIOS_PARAMETER2, W
	mullw	4*6 / 2		; (1/2 step)
	movf	PRODL, W
	addwf	SEQ_CLK_REQ_CTR, F, BANKED
	return

	;; ---
SEQ_ENC_Datawheel_P0
	;; if we are in page #0, use the data wheel to modify the "active step"
	swapf	SEQ_CURRENT_STEP, W, BANKED	; set the step view correctly
	andlw	0x01
	movwf	SEQ_SELECTED_STEPS, BANKED
	movf	SEQ_CURRENT_STEP, W, BANKED
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	goto	SEQ_ENC_Handler

;; --------------------------------------------------------------------------
;;  This function is called by USER_ENC_NotifyChange when an encoder has
;;  been moved. Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
SEQ_ENC_Handler
	SET_BSR SEQ_BASE

	;; if we are not in page #0 (edit page), use the GP encoders to modify menu values
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_ENC_Handler_MenuPage
	movf	CS_MENU_POS, W
	bz	SEQ_ENC_Handler_EditPage
SEQ_ENC_Handler_MenuPage
#if 0
	;; stop current message
	call	SEQ_MSG_Stop
#endif
	;; set cursor via GP button function (savest solution)
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	SEQ_ENC_Handler_MenuPageNoCS
SEQ_ENC_Handler_MenuPageCS
	movff	MIOS_PARAMETER2, CS_MENU_ENC_INCREMENTER	; store incrementer
	movff	CS_MENU_CURSOR_POS, CS_MENU_CURSOR_POS_OLD	; store current cursor pos
	;; call the GP hook. 
	;; Parameter:
	;;    GP button number in MIOS_PARAMETER1: 0x00-0x0f (-> derived from encoder number)
	;;    button value in MIOS_PARAMETER2 (always 0)
	;; (encoder number already in MIOS_PARAMETER1)
	
	clrf	MIOS_PARAMETER2
	call	CS_MENU_ExecMenuGPCallbackHook
	;; execute to CS_M_*Init if new cursor pos has been selected
	movf	CS_MENU_CURSOR_POS, W
	xorwf	CS_MENU_CURSOR_POS_OLD, W
	bz	SEQ_ENC_Handler_MenuPageCS_NoNew
SEQ_ENC_Handler_MenuPageCS_New
	movlw	CS_MENU_ENTRY_INIT_FUNC_OFFSET
	call	CS_MENU_Hlp_IndirectMenuPageCall
SEQ_ENC_Handler_MenuPageCS_NoNew
	SET_BSR SEQ_BASE
	movf	CS_MENU_ENC_INCREMENTER, W	; restore incrementer (can be modified by GP hook!)
	movwf	MIOS_PARAMETER2
	;; exit if incrementer has been cleared by GP handler
	skpnz
	return
SEQ_ENC_Handler_MenuPageNoCS
	;; branch to the common CS function for the datawheel
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_Enc

SEQ_ENC_Handler_EditPage
	;; save encoder number + offset in SEQ_CURRENT_STEP and SEQ_EVNTS
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_CURRENT_STEP, BANKED
	movwf	SEQ_EVNTS, BANKED

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; save incrementer in SEQ_ENC_INC
	movff	MIOS_PARAMETER2, SEQ_ENC_INC

	;; loop through all selected tracks
	clrf	SEQ_EVNTT, BANKED

	;; using CS_STAT2_TRK_CHANGED flag to determine, if any track is selected (and not disabled)
	bcf	CS_STAT2, CS_STAT2_TRK_CHANGED

SEQ_ENC_Handler_Loop
	;; check if track selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_ENC_Handler_Loop_Next

	;; do nothing if track is disabled
	call	CS_M_HLP_ChkTrkDisabled
	bz	SEQ_ENC_Handler_Loop_Next

	;; select SEQ_PATTERN and SEQ_PATTERN_BANK depending on track
	call	CS_M_HLP_SetPatternAndBank

	;; get layer value and save it in TMP[12]
	movlw	0x00
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	movlw	0x01
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	movlw	0x02
	call	SEQ_LAYER_GetVValue
	movwf	TMP1		; low-byte
	clrf	TMP2		; high-byte

	;; temporary save incrementer in TMP3
	movff	SEQ_ENC_INC, TMP3

	;; multiply incrementer by 4 if arpeggio
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x07
	xorlw	SEQ_TRKMODE_ARPEGGIATOR
	bnz	SEQ_ENC_Handler_NoArpModif
	call	CS_M_HLP_GetLayerVType
	xorlw	SEQ_LAYER_V_LEN
	bz	SEQ_ENC_Handler_NoArpModif
	xorlw	SEQ_LAYER_V_VEL ^ SEQ_LAYER_V_LEN
	bz	SEQ_ENC_Handler_NoArpModif
SEQ_ENC_Handler_ArpModif
	movf	TMP3, W
	mullw	4
	movff	PRODL, TMP3	
SEQ_ENC_Handler_NoArpModif

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; set max value
	movlw	0x7f
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; move incrementer (temporary stored in TMP3) to WREG
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	SET_BSR	SEQ_BASE	; fix BSR
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	btfss	MIOS_PARAMETER1, 0
	return

	;; send value
	call	SEQ_ENC_Send

	;; notify that at least one track has been changed
 	bsf	CS_STAT2, CS_STAT2_TRK_CHANGED

SEQ_ENC_Handler_Loop_Next
	SET_BSR	SEQ_BASE
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, SEQ_ENC_Handler_Loop

	;; print "track(s) disabled" message when no trigger has been toggled
	GOTO_IFCLR CS_STAT2, CS_STAT2_TRK_CHANGED, ACCESS, SEQ_LCD_MsgTrkDisabled

	;; trigger pot display page
	call	SEQ_MSG_Start

	return



;; --------------------------------------------------------------------------
;;  This function sets the encoder speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
;; --------------------------------------------------------------------------
SEQ_ENC_ConfigSpeed
	SET_BSR	SEQ_BASE

	;; data wheel:
#if DEFAULT_ENC_DATAWHEEL >= 0
	;; speed divider
	movlw	DEFAULT_DATAWHEEL_SPEED_VALUE
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw 0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movlw	DEFAULT_ENC_DATAWHEEL
	call	MIOS_ENC_SpeedSet
#endif

	;; the remaining rotary encoders
	movlw	0x01
	movwf	TMP1
SEQ_ENC_ConfigSpeed_Loop
	;; speed divider
	movlw	DEFAULT_ENC_SPEED_VALUE		; button pressed
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw 0x00
	movwf	MIOS_PARAMETER2

	;; speed mode
	movlw	MIOS_ENC_SPEED_FAST
	btfss	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	movlw MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1

	;; encoder number - set speed
	movf	TMP1, W
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	17
	cpfseq	TMP1, ACCESS
	rgoto SEQ_ENC_ConfigSpeed_Loop
	return


;; --------------------------------------------------------------------------
;;  Auto Fast function - should be called whenever
;;     - the edit screen is entered
;;     - a track is changed
;;     - a layer is changed
;;  ignored when DEFAULT_AUTO_FAST_BUTTON is set to 0
;; --------------------------------------------------------------------------
SEQ_ENC_AutoFast
#if DEFAULT_AUTO_FAST_BUTTON == 0
	return
#else
	;; set/clear FAST button depending on layer type
	call	CS_M_HLP_GetSelectedTrk
	movff	WREG, SEQ_EVNTT
	call	CS_M_HLP_GetLayerVType
	xorlw	SEQ_LAYER_V_VEL
	bz	SEQ_ENC_AutoFast_1
	xorlw	SEQ_LAYER_V_CHORD1_VEL ^ SEQ_LAYER_V_VEL
	bz	SEQ_ENC_AutoFast_1
	xorlw	SEQ_LAYER_V_CHORD2_VEL ^ SEQ_LAYER_V_CHORD1_VEL
	bz	SEQ_ENC_AutoFast_1
	xorlw	SEQ_LAYER_V_CC ^ SEQ_LAYER_V_CHORD2_VEL
	bz	SEQ_ENC_AutoFast_1
SEQ_ENC_AutoFast_0
	bcf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	rgoto	SEQ_ENC_ConfigSpeed

SEQ_ENC_AutoFast_1
	bsf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED

	;; set speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
	rgoto	SEQ_ENC_ConfigSpeed
#endif



;; --------------------------------------------------------------------------
;;  This function is called by SEQ_ENC_Handler when a value has to be modified
;;     o track number in SEQ_EVNTT
;;     o step number in SEQ_EVNTS
;;     o absolute value in TMP1
;;     o incrementer in TMP3 (0 if value is changed via pot)
;; --------------------------------------------------------------------------
SEQ_ENC_Send

	;; branch depending on "change all" flag
	SET_BSR	SEQ_BASE
	;; check "all same" option - behaves like "all steps", advantage when it's checked here: active whenever button is pressed independent from the state of the LED (no doubleclick required)
	BRA_IFSET CS_STAT2, CS_STAT2_ALL_SAME_VALUE, ACCESS, SEQ_ENC_Send_AllSame
	;; check "all steps" option
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, SEQ_ENC_Send_NotAll


	;; always set to same value if pot
	movf	TMP3, W
	bz	SEQ_ENC_Send_AllSame

	;; incremental change
SEQ_ENC_Send_AllInc
	;; copy value into all registers of the selected layer
	clrf	SEQ_EVNTS, BANKED
SEQ_ENC_Send_AllIncLoop
	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; determine layer and get value
	call	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_GetVValue
	;; store it in TMP[12]
	movwf	TMP1
	clrf	TMP2
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; set max value
	movlw	0x7f
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; move incrementer (temporary stored in TMP3) to WREG
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	SET_BSR	SEQ_BASE	; fix BSR
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, SEQ_ENC_Send_AllIncLoopNext

	;; otherwise store new value
	movff	TMP1, MIOS_PARAMETER1
	call	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_SetVValue

	;; mute/unmute step depending on value in TMP1
#if 1
	;; (only if current step matches with encoder)
	movf	SEQ_CURRENT_STEP, W, BANKED
	cpfseq	SEQ_EVNTS, BANKED
	rgoto SEQ_ENC_Send_AllIncLoopNext
#endif
	rcall	SEQ_ENC_Hlp_ChkGate

SEQ_ENC_Send_AllIncLoopNext
	incf	SEQ_EVNTS, F, BANKED
	BRA_IFCLR SEQ_EVNTS, 5, BANKED, SEQ_ENC_Send_AllIncLoop
	rgoto	SEQ_ENC_Send_Cont


	;; set all to same value
SEQ_ENC_Send_AllSame
	;; copy value into all registers of the selected layer
	clrf	SEQ_EVNTS, BANKED
SEQ_ENC_Send_AllSameLoop
	movff	TMP1, MIOS_PARAMETER1
	call	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_SetVValue

	;; mute/unmute step depending on value in TMP1
#if 1
	;; (only if current step matches with encoder)
	movf	SEQ_CURRENT_STEP, W, BANKED
	cpfseq	SEQ_EVNTS, BANKED
	rgoto SEQ_ENC_Send_AllSameLoopNext
#endif
 	rcall	SEQ_ENC_Hlp_ChkGate

SEQ_ENC_Send_AllSameLoopNext
	incf	SEQ_EVNTS, F, BANKED
	BRA_IFCLR SEQ_EVNTS, 5, BANKED, SEQ_ENC_Send_AllSameLoop
	rgoto	SEQ_ENC_Send_Cont


	;; no "all"
SEQ_ENC_Send_NotAll
	;; copy value into single register
	movff	TMP1, MIOS_PARAMETER1
	call	CS_M_HLP_GetSelectedLayer
	call	SEQ_LAYER_SetVValue
	;; mute/unmute step depending on value in TMP1
	rcall	SEQ_ENC_Hlp_ChkGate
	
SEQ_ENC_Send_Cont
	return



	;; help function which sets/clears the gate depending on the value in TMP1
SEQ_ENC_Hlp_ChkGate
	;; mute/unmute step if not length
	call	CS_M_HLP_GetLayerVType
	xorlw	SEQ_LAYER_V_LEN
	bz	SEQ_ENC_Send_Cont
	xorlw	SEQ_LAYER_V_LEN
	;; force unmute if CC event
	xorlw	SEQ_LAYER_V_CC
	bz	SEQ_ENC_Hlp_ChkGate_UnMute
	;; else mute if value is zero, else unmute
	movf	TMP1, W
	bnz	SEQ_ENC_Hlp_ChkGate_UnMute
SEQ_ENC_Hlp_ChkGate_Mute
	movlw	0
	goto	SEQ_TRG_GateSet

SEQ_ENC_Hlp_ChkGate_UnMute
	movlw	1
	goto	SEQ_TRG_GateSet
