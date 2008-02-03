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
;;  OUT: the status of the 16 GP LEDs in SEQ_GP_LED[01]
;; --------------------------------------------------------------------------
SEQ_GP_LED_Update
	;; clear the 16 GP LEDs
	SET_BSR	SEQ_BASE
	clrf	SEQ_GP_LED0, BANKED
	clrf	SEQ_GP_LED1, BANKED

	;; menu select LED overlays everything
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_MENU, BANKED, SEQ_GP_Mode4_LED

	;; SEQ_GP_LED[01] hold the status of the GP buttons
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
	movwf	SEQ_GP_LED0, BANKED

	movf	CS_MENU_GP_OVERLAY_DOUT_SR1, W
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, SEQ_GP_LED_Update_NoF1R
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	movlw 0x00
SEQ_GP_LED_Update_NoF1R
	movwf	SEQ_GP_LED1, BANKED
	return

	;; ------------------------------------------------------------------
SEQ_GP_LED_Update_NoHook

	;; lower status: SEQ_STEP_EDIT mode
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED, SEQ_GP_Mode0_LED

	;; (song mode flag can be set together with another flag, therefore it has
	;;  the lowest priority)
	rgoto	SEQ_GP_Mode3_LED


; ==========================================================================
; ==========================================================================
; ==========================================================================
; ==========================================================================

;; --------------------------------------------------------------------------
;;  LED/Button functions for Mode 0: SEQ_MODE0_STEP_EDIT
;; --------------------------------------------------------------------------
SEQ_GP_Mode0_Button
	;; branch if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_GP_Mode0_Button_Depr

	;; store button number in SEQ_CURRENT_STEP
	movff	MIOS_PARAMETER1, SEQ_CURRENT_STEP

	;; mute step defined in MIOS_PARAMETER1
	;; store mask used for XOR operation in TMP1
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	clrf	SEQ_EVNTT
SEQ_GP_Mode0_Button_Loop
	;; check if track selected
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bz	SEQ_GP_Mode0_Button_Next

	;; special behaviour in drum mode
	call	CS_M_Hlp_CheckDrumMode
	bz	SEQ_GP_Mode0_Button_Loop_Drums
SEQ_GP_Mode0_Button_Loop_NoDrums
	;; get pointer to SEQ_TRKMUTED0x
	call	SEQ_FSR_TrkMuted0
	;; XOR with MUTED0 or MUTED1
	movf	TMP1, W
	btfss	MIOS_PARAMETER1, 3
	xorwf	INDF1, F
	btfsc	MIOS_PARAMETER1, 3
	xorwf	PREINC1, F
	rgoto	SEQ_GP_Mode0_Button_Next

SEQ_GP_Mode0_Button_Loop_Drums
	call	CS_M_Hlp_Get_D_TRK_x_y
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x03
	addwf	FSR1L, F

	movlw	SEQ_D_TRKMUTED_0x
	btfsc	SEQ_MODE0, SEQ_MODE0_MODIFY_ACCENT, BANKED
	movlw SEQ_D_TRKACC_0x
	addwf	FSR1L, F

	movf	TMP1, W
	btfsc	MIOS_PARAMETER1, 2
	swapf	TMP1, W
	andlw	0x0f
	xorwf	INDF1, F
	;; 	rgoto	SEQ_GP_Mode0_Button_Next

SEQ_GP_Mode0_Button_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 2, BANKED, SEQ_GP_Mode0_Button_Loop
	;; trigger pot display page and hold it
	goto	SEQ_MSG_Hold

SEQ_GP_Mode0_Button_Depr
	;; trigger pot display page
	goto	SEQ_MSG_Start


	;; ------------------------------------------------------------------
SEQ_GP_Mode0_LED
	;; transfer step muted status (16 steps) to SEQ_GP_LED[01]
	call	CS_M_HLP_GetSelectedTrk
	movff	WREG, SEQ_EVNTT
	call	CS_M_Hlp_CheckDrumMode
	bnz	SEQ_GP_Mode0_LED_Cont

	;; in drum mode: copy the layer based mutes to the track based mute variables
	;; (this is a little bit dirty, but it's the most efficient way to ensure that the 4*16 LED option is working correct)
	call	CS_M_Hlp_Get_D_TRK_x_y
	movlw	SEQ_D_TRKMUTED_0x
	btfsc	SEQ_MODE0, SEQ_MODE0_MODIFY_ACCENT, BANKED
	movlw SEQ_D_TRKACC_0x
	addwf	FSR1L, F

	movf	POSTINC1, W
	andlw	0x0f
	movwf	TMP1
	swapf	POSTINC1, W
	andlw	0xf0
	iorwf	TMP1, F
	movf	POSTINC1, W
	andlw	0x0f
	movwf	TMP2
	swapf	POSTINC1, W
	andlw	0xf0
	iorwf	TMP2, F

	call	SEQ_FSR_TrkMuted0
	movff	TMP1, POSTINC1
	movff	TMP2, POSTINC1

SEQ_GP_Mode0_LED_Cont
#if DEFAULT_TRACK_LEDS_ENABLED
	movlw	0x00		; in this mode the first LED row always show the status of the first track
#else
	call	CS_M_HLP_GetSelectedTrk
#endif
	movwf	SEQ_EVNTT, BANKED
	call	SEQ_FSR_TrkMuted0
	comf	POSTINC1, W
	movwf	SEQ_GP_LED0, BANKED
	comf	POSTINC1, W
	movwf	SEQ_GP_LED1, BANKED
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
	;; transfer song number to SEQ_GP_LED[01]
	movlw	0x01
	movwf	SEQ_GP_LED0, BANKED
	clrf	SEQ_GP_LED1, BANKED
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
	;; transfer menu pos to SEQ_GP_LED[01]
	call	CS_MENU_TABLE_GPMapGetLED
	movff	MIOS_PARAMETER1, SEQ_GP_LED0
	movff	MIOS_PARAMETER2, SEQ_GP_LED1
	return
