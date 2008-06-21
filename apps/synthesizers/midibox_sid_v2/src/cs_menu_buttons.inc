; $Id$
;
; SID Control Surface Button Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This handler has to be called by USER_DIN_NotifyToggle when a button
;;  has been pressed/depressed
;;  It goes through the CS_MENU_DIN_TABLE in "setup_*.asm",
;;  searches for a matching DIN pin and calls the appr. function
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;         0-127: "normal" DIN button
;;       128-255: matrix button (only first 64 are implemented)
;;
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Handler
	;; stop MIOS based LCD message if button pressed
	CALL_IFCLR MIOS_PARAMETER2, 0, ACCESS, MIOS_LCD_MessageStop

	;; get SID number
	call	CS_MENU_MS_GetSIDNumber

	;; go through the DIN map
	TABLE_ADDR CS_MENU_DIN_TABLE
CS_MENU_BUTTON_Handler_Loop
	tblrd*+				; get low-byte of function pointer
	movf	TABLAT, W		; load it in WREG
	movwf	TMP1			; and store it in TMP1
	tblrd*+				; get high byte
	movff	TABLAT, TMP2		; store it in TMP2 without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	CS_MENU_BUTTON_Handler_End; in this case jump to the end

	tblrd*+				; get pin-number
	movf	TABLAT, W
	tblrd*+				; and read dummy entry
	;; pin number still in WREG, compare it with MIOS_PARAMETER1
	xorwf	MIOS_PARAMETER1, W
	bnz	CS_MENU_BUTTON_Handler_Loop; next loop if not equal

	;; else: matching pin has been found
	;; jump indirectly to the function (pointer in TMP1/TMP2)
	IRQ_DISABLE		; (IRQs have to be disabled when TOSL/TOSH is modified)
	push
	movf	TMP1, W
	movwf	TOSL
	movf	TMP2, W
	movwf	TOSH
	IRQ_ENABLE
	return			; this return leads to a jump to the function stored in TOSL/TOSH

CS_MENU_BUTTON_Handler_End
	;; no matching pin found, exit handler
	return


;; --------------------------------------------------------------------------
;; the button functions are defined here
;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Dec
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; decrement 1
	movlw	0xff
	;; branch to ENC handler
	goto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Inc
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; increment 1
	movlw	0x01
	;; branch to ENC handler
	goto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Exec
	;; special startup condition: if MENU button pressed while CS_MENU_REINIT_CFG_CTR is != 0, clone slaves
	movf	CS_MENU_REINIT_CFG_CTR, W
	bz	CS_MENU_BUTTON_Exec_NoStartup
	btfsc	MIOS_PARAMETER2, 0
	return

	goto	MBNET_Clone_Me


CS_MENU_BUTTON_Exec_NoStartup
	;; activate/deactivate menu button function
	bcf	CS_MENU_MODE, CS_MENU_MODE_MENU_PRESSED
	btfss	MIOS_PARAMETER2, 0; if button is pressed
	bsf	CS_MENU_MODE, CS_MENU_MODE_MENU_PRESSED

	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; EXTRA for Exec/Menu button:
	;; exit SID player mode if CS is disabled
	GOTO_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, SIDPLAY_Disable

	goto	CS_MENU_Exec

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Sel10
	;; select button #10, set cursor to 10th position
	movlw	0x09
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel9
	;; select button #9, set cursor to 9th position
	movlw	0x08
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel8
	;; select button #8, set cursor to 8th position
	movlw	0x07
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel7
	;; select button #7, set cursor to 7th position
	movlw	0x06
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel6
	;; select button #6, set cursor to 6th position
	movlw	0x05
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel5
	;; select button #5, set cursor to 5th position
	movlw	0x04
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel4
	;; select button #4, set cursor to 4th position
	movlw	0x03
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel3
	;; select button #3, set cursor to 3rd position
	movlw	0x02
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel2
	;; select button #2, set cursor to 2nd position
	movlw	0x01
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel1
	;; select button #1, set cursor to 1st position
	movlw	0x00
	;; 	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Select_Cont
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; branch if in SHIFT page
	GOTO_IFSET CS_STAT3, CS_STAT3_SHIFT_PAGE, ACCESS, CS_MENU_SHIFT_Button

	;; set cursor to: CS_MENU_PAGE_OFFSET + number in WREG
	addwf	CS_MENU_PAGE_OFFSET, W

	;; branch to the CS_MENU_Select function
	goto	CS_MENU_Select

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_SID1
	movlw	1 << 0
	rgoto	CS_MENU_BUTTON_SID_Cont
	
CS_MENU_BUTTON_SID2
	movlw	1 << 1
	rgoto	CS_MENU_BUTTON_SID_Cont
	
CS_MENU_BUTTON_SID3
	movlw	1 << 2
	rgoto	CS_MENU_BUTTON_SID_Cont
	
CS_MENU_BUTTON_SID4
	movlw	1 << 3
	;; 	rgoto	CS_MENU_BUTTON_SID_Cont

CS_MENU_BUTTON_SID_Cont
	movwf	TMP1

	;; always request new display-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_SID_Hlp_Off

	;; else branch depending on state of all 4 SID buttons:
	;; if only one SID button pressed: radio-button function (1 of 4)
	;; if more than one SID button pressed: toggle function (4 of 4)
	movf	CS_MENU_SELECTED_SID_FLAGS, W
	andlw	0xf0
	xorlw	0xf0
	bz	CS_MENU_BUTTON_SID_Radio
CS_MENU_BUTTON_SID_Select
	;; toggle SID flag
	movf	TMP1, W
	xorwf	CS_MENU_SELECTED_SID_FLAGS, F
 	rgoto	CS_MENU_BUTTON_SID_End
CS_MENU_BUTTON_SID_Radio
	;; select 1 of 4
	movlw	0xf0
	andwf	CS_MENU_SELECTED_SID_FLAGS, F
	movf	TMP1, W
	iorwf	CS_MENU_SELECTED_SID_FLAGS, F
	;; 	rgoto	CS_MENU_BUTTON_SID_End
	
CS_MENU_BUTTON_SID_End
	;; save button state
	rcall	CS_MENU_BUTTON_SID_Hlp_Sav

	;; save patch/bank number of newly selected SID into CS_MENU_SAVE_PATCH/Bank
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	INDF1, W
	andlw	0x7f
	movwf	CS_MENU_SAVE_PATCH
	call	CS_MENU_MS_GetSIDBank; sets FSR1 to bank register
	movf	INDF1, W
	andlw	0x07
	movwf	CS_MENU_SAVE_BANK

	;; when we are switching from one to another engine, the menu page could be invalid
	;; therefore the pages of the appr. engines are stored in an array, and restored
	;; whenever required. In order to save menu, the cursor position and page offset
	;; is not stored...
	;; this measure is only required if we are neither in ensemble, nor main menu
	movf	CS_MENU, W
	andlw	0xc0
	bnz	CS_MENU_BUTTON_SID_End_NoEngPage
CS_MENU_BUTTON_SID_End_EngPage
	lfsr	FSR1, CS_MENU_LASTPAGE_ENG_0
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	movwf	SAVED_CS_ENGINE		; store in SAVED_CS_ENGINE for later comparison
	movff	CS_MENU, PLUSW1
CS_MENU_BUTTON_SID_End_NoEngPage

	;; checks if a slave is available - if not, a connection will be retried
	call	CS_MENU_MBNET_Retry

	;; update edit buffer
	call	CS_MENU_MS_UpdateEditBuffer

	;; if not in main/ensemble menu, and engine has been changed: use last stored CS_MENU of the appr. engine
	movf	CS_MENU, W
	andlw	0xc0
	bnz	CS_MENU_BUTTON_SID_End_NoEngPag2
CS_MENU_BUTTON_SID_End_EngPag2
	;; check if engine has been changed
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	xorwf	SAVED_CS_ENGINE, W
	andlw	0x03
	bz	CS_MENU_BUTTON_SID_End_NoEngPag2

	;; change page depending on newly selected engine and enter it
	lfsr	FSR1, CS_MENU_LASTPAGE_ENG_0
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	movf	PLUSW1, W
	rcall	CS_MENU_BUTTON_Hlp_MenuChangeOk	; (w/o check)
CS_MENU_BUTTON_SID_End_NoEngPag2

	;; update parameter
	call	CS_MENU_EncParameterSet

	;; request reinitialization of complete CS
	rcall	CS_MENU_BUTTON_DisplayUpdateReq

	;; if SID1 selected, and SID player is active: re-initialize screen
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, CS_MENU_BUTTON_SID_End_NoSP
	BRA_IFCLR SID_STAT, SID_STAT_SIDPLAYER, ACCESS, CS_MENU_BUTTON_SID_End_NoSP
CS_MENU_BUTTON_SID_End_SP
	;; init LCD for SID player
	call	SIDPLAY_InitLCD
	;; disable CS (dirty, but USER_Tick not called before next update)
	bsf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	;; dirty: update LEDs as well (not done anywhere else...)
	call	CS_MENU_LED_Update
CS_MENU_BUTTON_SID_End_NoSP
	return

CS_MENU_BUTTON_SID_Hlp_Off
	;; button has been depressed

CS_MENU_BUTTON_SID_Hlp_Sav
	;; mask out old button status
	swapf	TMP1, W
	xorlw	0xff
	andwf	CS_MENU_SELECTED_SID_FLAGS, F

	;; store new button status (pressed/depressed) in CS_MENU_SELECTED_SID_FLAGS[7..4]
	swapf	TMP1, W
	btfsc	MIOS_PARAMETER2, 0
	iorwf	CS_MENU_SELECTED_SID_FLAGS, F
	return


	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Shift
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; activate/deactivate shift function
	bcf	CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED
	btfss	MIOS_PARAMETER2, 0; if button is pressed
	bsf	CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED

	;; change to SHIFT page
	bcf	CS_STAT3, CS_STAT3_SHIFT_PAGE
	btfss	MIOS_PARAMETER2, 0; if button is pressed
	bsf	CS_STAT3, CS_STAT3_SHIFT_PAGE

	;; always request new display-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_CC
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; toggle CC mode
	btg	CS_MENU_MODE, CS_MENU_MODE_CC
	;; print message
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Edit
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; toggle edit mode
	btg	CS_MENU_MODE, CS_MENU_MODE_EDIT
	;; copy value to flashed LED flag
	bcf	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED
	btfsc	CS_MENU_MODE, CS_MENU_MODE_EDIT
	bsf	CS_MENU_MODE, CS_MENU_MODE_EDIT_FLASHED
	;; set the cursor counter which is also used to flash the LED
	movlw	5*2
	movwf	CS_CURSOR_CTR
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Play
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; CS_MENU_SELECTED_SID_FLAGS[3:0] contain the current selected SIDs
	movlw	0x00
	CALL_IFSET CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x01
	CALL_IFSET CS_MENU_SELECTED_SID_FLAGS, 1, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x02
	CALL_IFSET CS_MENU_SELECTED_SID_FLAGS, 2, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x03
	CALL_IFSET CS_MENU_SELECTED_SID_FLAGS, 3, ACCESS, CS_MENU_MS_PlayNote

	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_SID_LR
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; toggle L/R
	incf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x03
	skpnz
	addlw	1
	movwf	CS_MENU_SELECTED_SID_LR
	
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Sync
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; branch to ENS->CLK menu
	movlw	CS_MENU_E_CLK
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_BUTTON_Osc_Sel_DM
CS_MENU_BUTTON_Osc_Sel_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Osc_Sel_B

CS_MENU_BUTTON_Osc_Sel_L
	;; select one OSC
	movlw	0x01
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 0
	movlw 0x02
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 1
	movlw 0x04
	movwf	CS_MENU_SELECTED_OSC_FLAGS
	
	movlw	0x00
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 1
	movlw 0x01
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 2
	movlw 0x02
	movwf	CS_MENU_SELECTED_OSC
	;; change to OSC page
	call	CS_MENU_GetMenuID_OSC
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Osc_Sel_B
	;; toggle L/R, oscillator always 1
	incf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x03
	skpnz
	addlw	1
	movwf	CS_MENU_SELECTED_SID_LR

	movlw	0x01
	movwf	CS_MENU_SELECTED_OSC_FLAGS

	;; branch depending on selected OSC Control Layer
	movf	CS_MENU_SELECTED_OSC_CTRL, W
	bz	CS_MENU_BUTTON_Osc_Sel_B_0
	addlw	-1
	bz	CS_MENU_BUTTON_Osc_Sel_B_1
	rgoto	CS_MENU_BUTTON_Osc_Sel_B_2

CS_MENU_BUTTON_Osc_Sel_B_0
CS_MENU_BUTTON_Osc_Sel_B_1
	;; change to OSC page
	call	CS_MENU_GetMenuID_OSC
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Osc_Sel_B_2
	;; change to Knobs page
	call	CS_MENU_GetMenuID_KNB
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Osc_Sel_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Osc_Sel_M

CS_MENU_BUTTON_Osc_Sel_D
	;; always select OSC1 (flag not really used by CS, we are working with instruments)
	movlw	0x01
	movwf	CS_MENU_SELECTED_OSC_FLAGS
	;; change to OSC page
	call	CS_MENU_GetMenuID_OSC
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Osc_Sel_M
	;; always select OSC1 (flag not really used by CS, we are working with instruments)
	movlw	0x01
	movwf	CS_MENU_SELECTED_OSC_FLAGS
	;; change to OSC page
	call	CS_MENU_GetMenuID_OSC
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange


	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_Wav
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; no function in drum mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	skpnz
	return

	;; else increment WAV setting
	movlw	0x01		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_OSC	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_RS
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; no function in drum mode
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	skpnz
	return

	;; else increment R/S setting
	movlw	0x02		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_OSC	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_Ctrl
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; increment OSC Ctrl parameter (0x00-0x02)
	incf	CS_MENU_SELECTED_OSC_CTRL, F
	movlw	0x02+1
	cpfslt	CS_MENU_SELECTED_OSC_CTRL, ACCESS
	clrf CS_MENU_SELECTED_OSC_CTRL
	CALL_IFCLR CS_MENU_SELECTED_OSC_CTRL, 1, ACCESS, CS_MENU_GetMenuID_OSC
	CALL_IFSET CS_MENU_SELECTED_OSC_CTRL, 1, ACCESS, CS_MENU_GetMenuID_KNB	
	rcall	CS_MENU_BUTTON_Hlp_MenuChange

	;; set page offset and cursor pos depending on selected layer
	movf	CS_MENU_SELECTED_OSC_CTRL, W
	bnz	CS_MENU_BUTTON_Osc_Ctrl_Not0
CS_MENU_BUTTON_Osc_Ctrl_0
	movlw	0x03
	rgoto	CS_MENU_BUTTON_Osc_Ctrl_Cont
CS_MENU_BUTTON_Osc_Ctrl_Not0
	BRA_IFSET CS_MENU_SELECTED_OSC_CTRL, 1, ACCESS, CS_MENU_BUTTON_Osc_Ctrl_Not1
CS_MENU_BUTTON_Osc_Ctrl_1
	;; different offset for drum engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bnz	CS_MENU_BUTTON_Osc_Ctrl_1_LBM
CS_MENU_BUTTON_Osc_Ctrl_1_D
	movlw	0x06
	rgoto	CS_MENU_BUTTON_Osc_Ctrl_Cont
CS_MENU_BUTTON_Osc_Ctrl_1_LBM	
	movlw	0x09
	rgoto	CS_MENU_BUTTON_Osc_Ctrl_Cont
CS_MENU_BUTTON_Osc_Ctrl_Not1
	movlw	0x00
CS_MENU_BUTTON_Osc_Ctrl_Cont
	movwf	CS_MENU_PAGE_OFFSET
	movwf	CS_MENU_CURSOR_POS

	;; deselect parameter (and update CS_MENU_PARAMETER_L for new cursor pos)
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	goto	CS_MENU_EXEC_Hlp_ChangeMenu

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_LFO_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment LFO setting
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	movlw	0x00		; cursor pos Lead is 0
	skpz
	movlw	0x01		; cursor pos Bassline/Drum/Multi is 1
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_LFO	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_LFO_Wav
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else increment LFO setting
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	movlw	0x01		; cursor pos Lead is 1
	skpz			
	movlw	0x02		; cursor pos Bassline/Drum/Multi is 2
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_LFO	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_BUTTON_Env_Sel_DM
CS_MENU_BUTTON_Env_Sel_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Env_Sel_B

CS_MENU_BUTTON_Env_Sel_L
	;; increment ENV setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_ENV	; menu page
	goto	CS_MENU_ButtonInc

CS_MENU_BUTTON_Env_Sel_B
	;; toggle L/R
	incf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x03
	skpnz
	addlw	1
	movwf	CS_MENU_SELECTED_SID_LR

	;; change to ENV page
	call	CS_MENU_GetMenuID_ENV
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Env_Sel_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Env_Sel_M

CS_MENU_BUTTON_Env_Sel_D
	;; only change to ENV page (one ENV only)
	call	CS_MENU_GetMenuID_ENV
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Env_Sel_M
	;; only change to ENV page (one ENV only)
	call	CS_MENU_GetMenuID_ENV
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange


	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Curve
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else increment ENV setting
	movlw	0x07		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x03		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_ENV	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Ctrl
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
#if DEFAULT_MB6582_CS
	;; else decrement ENV Ctrl parameter (0x00-0x02)
	;; this takes care of "Misc" layer being 0x02 but in middle of toggle/LED order,
	;; i.e. order is 0x00, 0x02, 0x01
	decf	CS_MENU_SELECTED_ENV_CTRL, F
	movlw	0x02
	btfsc	CS_MENU_SELECTED_ENV_CTRL, 1
	movwf CS_MENU_SELECTED_ENV_CTRL
#else
	;; else increment ENV Ctrl parameter (0x00-0x01)
	incf	CS_MENU_SELECTED_ENV_CTRL, F
	movlw	0x01+1
	cpfslt	CS_MENU_SELECTED_ENV_CTRL, ACCESS
	clrf CS_MENU_SELECTED_ENV_CTRL
#endif
#if 0
	call	CS_MENU_GetMenuID_ENV	; menu page
	rcall	CS_MENU_BUTTON_Hlp_MenuChange

	;; set page offset and cursor pos depending on selected layer
	movf	CS_MENU_SELECTED_ENV_CTRL, W
	bnz	CS_MENU_BUTTON_Env_Ctrl_Not0
CS_MENU_BUTTON_Env_Ctrl_0
	movlw	0x00
	rgoto	CS_MENU_BUTTON_Env_Ctrl_Cont
CS_MENU_BUTTON_Env_Ctrl_Not0
	movlw	0x01
CS_MENU_BUTTON_Env_Ctrl_Cont
	movwf	CS_MENU_PAGE_OFFSET
	movwf	CS_MENU_CURSOR_POS

	;; deselect parameter (and update CS_MENU_PARAMETER_L for new cursor pos)
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	goto	CS_MENU_EXEC_Hlp_ChangeMenu
#else
	;; do nothing else...
	;; this is to allow the visible menu items on a more comfortable way (no unwanted menu switching)
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq
#endif

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Fil_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else increment Filter setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_FIL	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Fil_Mod
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else increment Filter setting
	movlw	0x03		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_FIL	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Fil_ExtIn
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else increment Filter setting
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x02		; different cursor pos in drum and multi mode (bit #1 of engine mode set)
	xorlw	0x02

	movlw	0x04		; cursor pos drum and multi
	skpz
	movlw	0x05		; cursor pos lead/bassline mode
	movwf	MIOS_PARAMETER1

	movlw	0x00		; page offset drum and multi
	skpz
	movlw	0x01		; page offset lead/bassline mode
	movwf	MIOS_PARAMETER2
	call	CS_MENU_GetMenuID_FIL	; menu page
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_PageUp
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_PageUp_On
CS_MENU_BUTTON_PageUp_Off
CS_MENU_BUTTON_PageDown_Off
	;; in main menu: request display update and exit
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_BUTTON_PageDown_NotMain
CS_MENU_BUTTON_PageDown_Main
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ	; will also clear PAGE_MSG flag
	return
CS_MENU_BUTTON_PageDown_NotMain
	;; disable page message and DISABLE request display update
	bcf	CS_STAT, CS_STAT_PAGE_MSG
	bcf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the message is still visible for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	return

CS_MENU_BUTTON_PageUp_On
	;; incrementer in WREG
	movlw	1
	goto	CS_MENU_PageUpDown

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_PageDown
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_PageDown_Off
CS_MENU_BUTTON_PageDown_On
	;; incrementer in WREG
	movlw	-1
	goto	CS_MENU_PageUpDown

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_CC_PageUp
	;; Control CC flag if Shift Button *set* (no error)
	;; otherwise Page Up
	GOTO_IFSET CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED, ACCESS, CS_MENU_BUTTON_CC
	rgoto	CS_MENU_BUTTON_PageUp

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Edit_PageDown
	;; Control Edit flag if Shift Button *set* (no error)
	;; otherwise Page Down
	GOTO_IFSET CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED, ACCESS, CS_MENU_BUTTON_Edit
	rgoto	CS_MENU_BUTTON_PageDown

	;; ------------------------------------------------------------------

CS_MENU_BUTTON_M_Mode
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg
	;; else toggle matrix/meter flag
	btg	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

CS_MENU_BUTTON_M_O1Ptch
	movlw	0x00
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_O2Ptch
	movlw	0x01
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_O3Ptch
	movlw	0x02
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_O1PW
	movlw	0x03
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_O2PW
	movlw	0x04
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_O3PW
	movlw	0x05
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_Filter
	movlw	0x06
	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_Vol
	movlw	0x07
	;; 	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_SetCol
	;; store index in CS_MENU_SELECTED_MOD_TARG
	movwf	CS_MENU_SELECTED_MOD_TARG

	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg


	;; set/clear flag depending on button state
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_M_SetCol_1
CS_MENU_BUTTON_M_SetCol_0	; button depressed
	movf	CS_MENU_SELECTED_MOD_TARG, W
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_MATRIX_SELECTED_COL, F
	rgoto	CS_MENU_BUTTON_M_SetCol_End

CS_MENU_BUTTON_M_SetCol_1	; button pressed
	;; if one mod target button already pressed: toggle led matrix mode
	movf	CS_MENU_MATRIX_SELECTED_COL, W
	bz	CS_MENU_BUTTON_M_SetCol_1_NoTog
CS_MENU_BUTTON_M_SetCol_1_Tog
	btg	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	;; matrix/meter LEDs needs updating
	call	CS_MENU_BUTTON_DisplayUpdateReq
CS_MENU_BUTTON_M_SetCol_1_NoTog
	;; set flag
	movf	CS_MENU_SELECTED_MOD_TARG, W
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_MATRIX_SELECTED_COL, F

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_BUTTON_M_SetCol_1_DM
CS_MENU_BUTTON_M_SetCol_1_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_M_SetCol_1_B

CS_MENU_BUTTON_M_SetCol_1_L
	;; init flash counter
	clrf	CS_MENU_MATRIX_FLASH_CTR

	;; if row is selected:
	movf	CS_MENU_SELECTED_MOD_SRC, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_MATRIX_SELECTED_ROW, W
	bz	CS_MENU_BUTTON_M_SetCol_NoTog
	;; toggle modmatrix entry
	;; expecting bit number in CS_MENU_SELECTED_MOD_SRC, modulation target in CS_MENU_SELECTED_MOD_TARG
CS_MENU_BUTTON_M_SetCol_Tog
	movff	CS_MENU_SELECTED_MOD_SRC, CS_MENU_SELECTED_MOD
	movf	CS_MENU_SELECTED_MOD_TARG, W
	rcall	CS_MENU_BUTTON_ToggleMODFlag
CS_MENU_BUTTON_M_SetCol_NoTog

CS_MENU_BUTTON_M_SetCol_End
	return


CS_MENU_BUTTON_M_SetCol_1_B
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM
	movff	CS_MENU_SELECTED_MOD_TARG, INDF0
	call	CS_MENU_MS_SendParameter
	;; change sequence (don't switch to SEQ menu, this disturbs the useflow)
	movff	CS_MENU_SELECTED_MOD_TARG, CS_MENU_WT_SEQ
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	
CS_MENU_BUTTON_M_SetCol_1_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_M_SetCol_1_M
CS_MENU_BUTTON_M_SetCol_1_D
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_D_SEQ_NUM
	movff	CS_MENU_SELECTED_MOD_TARG, INDF0
	call	CS_MENU_MS_SendParameter
	;; change sequence
	movff	CS_MENU_SELECTED_MOD_TARG, CS_MENU_WT_SEQ
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq
	return

CS_MENU_BUTTON_M_SetCol_1_M
	;; TODO: not implemented yet
	return


	;; ------------------------------------------------------------------
CS_MENU_BUTTON_M_E1
	movlw	0x00
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_E2
	movlw	0x01
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L1
	movlw	0x02
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L2
	movlw	0x03
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L3
	movlw	0x04
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L4
	movlw	0x05
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L5
	movlw	0x06
	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_L6
	movlw	0x07
	;; 	rgoto	CS_MENU_BUTTON_M_SetRow
CS_MENU_BUTTON_M_SetRow
	;; set/clear flag depending on button state
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_M_SetRow_1
CS_MENU_BUTTON_M_SetRow_0	; button depressed
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_MATRIX_SELECTED_ROW, F
	return
	
CS_MENU_BUTTON_M_SetRow_1	; button pressed
	;; store index in CS_MENU_SELECTED_MOD_SRC and CS_MENU_SELECTED_MOD
	movwf	CS_MENU_SELECTED_MOD_SRC
	movwf	CS_MENU_SELECTED_MOD

	;; exit with message if SID player mode enabled and SID1 selected
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_BUTTON_SP_IgnoreMsg

	;; branch depending on engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, CS_MENU_BUTTON_M_SetRow_DM
CS_MENU_BUTTON_M_SetRow_LB
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_M_SetRow_B

CS_MENU_BUTTON_M_SetRow_L
	;; set flag
	movf	CS_MENU_SELECTED_MOD, W
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_MATRIX_SELECTED_ROW, F

	;; init flash counter
	clrf	CS_MENU_MATRIX_FLASH_CTR

	;; if column is selected:
	movf	CS_MENU_SELECTED_MOD_TARG, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_MATRIX_SELECTED_COL, W
	bz	CS_MENU_BUTTON_Chg_NoTog
	;; toggle modmatrix entry
	;; expecting bit number in CS_MENU_SELECTED_TARG, modulation target in CS_MENU_SELECTED_MOD
CS_MENU_BUTTON_Chg_Tog
	movf	CS_MENU_SELECTED_MOD_TARG, W
	rcall	CS_MENU_BUTTON_ToggleMODFlag
CS_MENU_BUTTON_Chg_NoTog

	;; if shift function is active, use the buttons to switch into ENV/LFO page
	;; otherwise switch into MOD page
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED, ACCESS, CS_MENU_BUTTON_M_EorLPage

CS_MENU_BUTTON_M_ModPage
	;; change to MOD page
	call	CS_MENU_GetMenuID_MOD
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange


CS_MENU_BUTTON_M_EorLPage
	;; disable SHIFT menu (for combined shift/CFG button)
	bcf	CS_STAT3, CS_STAT3_SHIFT_PAGE

	;; select new menu depending on mod button
	movf	CS_MENU_SELECTED_MOD_SRC, W
	andlw	0xfe
	bz	CS_MENU_BUTTON_Chg_Ex
	rgoto	CS_MENU_BUTTON_Chg_Lx

CS_MENU_BUTTON_Chg_Ex
	;; change to ENV menu
	movf	CS_MENU_SELECTED_MOD_SRC, W
	movwf	CS_MENU_SELECTED_ENV
	call	CS_MENU_GetMenuID_ENV	; menu page
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Chg_Lx
	;; change to LFO menu
	movf	CS_MENU_SELECTED_MOD_SRC, W
	addlw	-2
	movwf	CS_MENU_SELECTED_LFO
	call	CS_MENU_GetMenuID_LFO	; menu page
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange


CS_MENU_BUTTON_M_SetRow_B
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM
	movff	CS_MENU_SELECTED_MOD_SRC, INDF0
	call	CS_MENU_MS_SendParameter
	;; change sequence (don't switch to SEQ menu, this disturbs the useflow)
	movff	CS_MENU_SELECTED_MOD_SRC, CS_MENU_WT_SEQ
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq


CS_MENU_BUTTON_M_SetRow_DM
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_M_SetRow_M
CS_MENU_BUTTON_M_SetRow_D
	;; select track depending on button
	movff	CS_MENU_SELECTED_MOD, CS_MENU_SELECTED_TRK
	;; first change to SEQ page
	movlw	CS_MENU_D_SEQ
	rcall	CS_MENU_BUTTON_Hlp_MenuChange
	;; now change to edit sequence page
	;; DIRTY: ensure that cursor pos and page offset are selected the EDT menu entry
	movlw	2
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_PAGE_OFFSET
	goto	CS_MENU_EXEC_EditSeq

CS_MENU_BUTTON_M_SetRow_M
	;; TODO: not implemented yet
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Hlp_MenuChange
	;; don't change if already in menu (only update screen)
	cpfseq	CS_MENU, ACCESS
	rgoto CS_MENU_BUTTON_Hlp_MenuChangeOk
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

CS_MENU_BUTTON_Hlp_MenuChangeOk
	;; init menu page
	movwf	CS_MENU
	clrf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_PAGE_OFFSET
	call	CS_MENU_Page_Init

	;; deselect parameter
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	call	CS_MENU_EXEC_Hlp_ChangeMenu

	;; rgoto	CS_MENU_BUTTON_DisplayUpdateReq

CS_MENU_BUTTON_DisplayUpdateReq
	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_ToggleMODFlag
	;; IN: bit number in WREG, modulation target in CS_MENU_SELECTED_MOD

	;; force a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; store target number in CS_MENU_PARAMETER_IX
	movwf	CS_MENU_PARAMETER_IX

	;; get current value -> CS_MENU_PARAMETER_L
	lfsr	FSR0, SID_EDIT_BUFFER
	call	CS_MENU_R2P_MODx_Nx

	;; toggle depending on selected SID
	movf	CS_MENU_SELECTED_SID_LR, W
	xorwf	CS_MENU_PARAMETER_L, F

	;; write back and exit
	lfsr	FSR0, SID_EDIT_BUFFER
	goto	CS_MENU_P2R_MODx_Nx


;; ------------------------------------------------------------------
CS_MENU_BUTTON_SP_IgnoreMsg
	;; message print when SID1 selected and SIDplayer active
	goto	SIDPLAY_NoCSMessage

