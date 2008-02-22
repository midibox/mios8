; $Id$
;
; SID Control Surface Button Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This handler has to be called by USER_DIN_NotifyToggle when a button
;;  has been pressed/depressed
;;  It goes through the CS_MENU_DIN_TABLE in "cs_menu_io_tables.inc",
;;  searches for a matching DIN pin and calls the appr. function
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Handler
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
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; decrement 1
	movlw	0xff
	;; branch to ENC handler
	rgoto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Inc
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; increment 1
	movlw	0x01
	;; branch to ENC handler
	rgoto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Exec
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; new: play note/start sequencer function if SID buttons are hold
#if 1
	movf	CS_MENU_SELECTED_SID_FLAGS, W
	andlw	0xf0
	xorlw	0xf0
	bz	CS_MENU_BUTTON_Exec_Normal
CS_MENU_BUTTON_Exec_PlayNote
	;; CS_MENU_SELECTED_SID_FLAGS[7:4] contain the current button state
	movlw	0x00
	CALL_IFCLR CS_MENU_SELECTED_SID_FLAGS, 4, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x01
	CALL_IFCLR CS_MENU_SELECTED_SID_FLAGS, 5, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x02
	CALL_IFCLR CS_MENU_SELECTED_SID_FLAGS, 6, ACCESS, CS_MENU_MS_PlayNote
	movlw	0x03
	CALL_IFCLR CS_MENU_SELECTED_SID_FLAGS, 7, ACCESS, CS_MENU_MS_PlayNote

	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_MENU_BUTTON_Exec_Normal
#endif

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

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_SID_Hlp_Sav

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
	;; save patch to CS_MENU_SAVE_PATCH
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	INDF1, W
	andlw	0x7f
	movwf	CS_MENU_SAVE_PATCH
	;; save button state
	rcall	CS_MENU_BUTTON_SID_Hlp_Sav
	;; request new display-initialization in mainpage (required to display the new patchname)
	btfsc	CS_MENU, 7
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; update parameter
	call	CS_MENU_EncParameterSet
	;; request reinitialization of complete CS
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq


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
CS_MENU_BUTTON_Link
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; toggle link flags and update the link mode
	btg	CS_MENU_MODE, CS_MENU_MODE_LINK
	call	CS_MENU_UpdateLinkMode
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_CC
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; toggle CC mode
	btg	CS_MENU_MODE, CS_MENU_MODE_CC
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Edit
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
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
CS_MENU_BUTTON_Osc_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

#if CS_MENU_OSC_BUTTON_BEHAVIOUR == 0	;; old solution: 7 states (1--, -2-, --3, 12-, 1-3, -23, 123)

	;; increment OSC setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_OSC	; menu structure
	goto	CS_MENU_ButtonInc

#else	;; new solution: 3 states for faster access (1--, -2-, --3), combined mode only selectable from the menu
	
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

	;; change to OSC menu
	movlw	CS_MENU_OSC
	rcall	CS_MENU_BUTTON_Hlp_MenuChange

	;; set page offset and cursor pos
	movlw	0x00
	movwf	CS_MENU_PAGE_OFFSET
	movwf	CS_MENU_CURSOR_POS

	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	goto	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter
#endif

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_Wav
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment WAV setting
	movlw	0x01		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_OSC	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_RS
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment R/S setting
	movlw	0x02		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_OSC	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Osc_Ctrl
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; increment OSC Ctrl parameter (0x00-0x02)
	incf	CS_MENU_SELECTED_OSC_CTRL, F
	movlw	0x02+1
	cpfslt	CS_MENU_SELECTED_OSC_CTRL, ACCESS
	clrf CS_MENU_SELECTED_OSC_CTRL
	movlw	CS_MENU_OSC		; change to OSC menu
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
	movlw	0x09
	rgoto	CS_MENU_BUTTON_Osc_Ctrl_Cont
CS_MENU_BUTTON_Osc_Ctrl_Not1
	movlw	0x00
CS_MENU_BUTTON_Osc_Ctrl_Cont
	movwf	CS_MENU_PAGE_OFFSET
	movwf	CS_MENU_CURSOR_POS

	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	goto	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_LFO_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment LFO setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_LFO	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_LFO_Wav
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment LFO setting
	movlw	0x01		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_LFO	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment ENV setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_ENV	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Curve
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment ENV setting
	movlw	0x07		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x03		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_ENV	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Env_Ctrl
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment ENV Ctrl parameter (0x00-0x01)
	incf	CS_MENU_SELECTED_ENV_CTRL, F
	movlw	0x01+1
	cpfslt	CS_MENU_SELECTED_ENV_CTRL, ACCESS
	clrf CS_MENU_SELECTED_ENV_CTRL
	movlw	CS_MENU_ENV		; change to ENV menu
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

	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	goto	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Fil_Sel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment Filter setting
	movlw	0x00		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_FIL	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Fil_Mod
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else increment Filter setting
	movlw	0x04		; cursor pos
	movwf	MIOS_PARAMETER1
	movlw	0x00		; page offset
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_FIL	; menu structure
	goto	CS_MENU_ButtonInc

	;; ------------------------------------------------------------------
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
	;; 	rgoto	CS_MENU_BUTTON_M_SetCol
CS_MENU_BUTTON_M_SetCol
	;; store index in TMP1
	movwf	TMP1
	
	;; set/clear flag depending on button state
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_M_SetCol_1
CS_MENU_BUTTON_M_SetCol_0	; button depressed
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_MATRIX_SELECTED_COL, F
	return

CS_MENU_BUTTON_M_SetCol_1	; button pressed
	movwf	TABLAT
	;; if one mod target button already pressed: toggle led matrix mode
	movf	CS_MENU_MATRIX_SELECTED_COL, W
	bz	CS_MENU_BUTTON_M_SetCol_1_NoTog
CS_MENU_BUTTON_M_SetCol_1_Tog
	btg	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
CS_MENU_BUTTON_M_SetCol_1_NoTog
	movf	TABLAT, W
	;; init flash counter
	clrf	CS_MENU_MATRIX_FLASH_CTR
	;; store button number in CS_MENU_SELECTED_MOD and set flag
	movwf	CS_MENU_SELECTED_MOD
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_MATRIX_SELECTED_COL, F
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
	;; store index in TMP1
	movwf	TMP1

	;; set/clear flag depending on button state
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_M_SetRow_1
CS_MENU_BUTTON_M_SetRow_0	; button depressed
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_MATRIX_SELECTED_ROW, F
	return
	
CS_MENU_BUTTON_M_SetRow_1	; button pressed
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_MATRIX_SELECTED_ROW, F

	;; select new menu depending on mod button
	movf	TMP1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_BUTTON_Chg_E1
	rgoto	CS_MENU_BUTTON_Chg_E2
	rgoto	CS_MENU_BUTTON_Chg_L1
	rgoto	CS_MENU_BUTTON_Chg_L2
	rgoto	CS_MENU_BUTTON_Chg_L3
	rgoto	CS_MENU_BUTTON_Chg_L4
	rgoto	CS_MENU_BUTTON_Chg_L5
	rgoto	CS_MENU_BUTTON_Chg_L6

CS_MENU_BUTTON_Chg_E1
	movlw	0x00		; selected envelope
	rgoto	CS_MENU_BUTTON_Chg_Ex_Cont

CS_MENU_BUTTON_Chg_E2
	movlw	0x01		; selected envelope
	;; 	rgoto	CS_MENU_BUTTON_Chg_Ex_Cont

CS_MENU_BUTTON_Chg_Ex_Cont
	movwf	CS_MENU_SELECTED_ENV

	;; toggle menu entry
	incf	TMP1, W		; cursor pos & page offset
	movwf	MIOS_PARAMETER1
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_MOD	; menu structure
	call	CS_MENU_ButtonToggle

	movlw	CS_MENU_ENV	; menu structure
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_BUTTON_Chg_L1
	movlw	0x00		; select LFO
	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont
CS_MENU_BUTTON_Chg_L2
	movlw	0x01		; select LFO
	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont
CS_MENU_BUTTON_Chg_L3
	movlw	0x02		; select LFO
	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont
CS_MENU_BUTTON_Chg_L4
	movlw	0x03		; select LFO
	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont
CS_MENU_BUTTON_Chg_L5
	movlw	0x04		; select LFO
	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont
CS_MENU_BUTTON_Chg_L6
	movlw	0x05		; select LFO
	;; 	rgoto	CS_MENU_BUTTON_Chg_Lx_Cont

CS_MENU_BUTTON_Chg_Lx_Cont
	movwf	CS_MENU_SELECTED_LFO

	;; toggle menu entry
	incf	TMP1, W		; cursor pos & page offset
	movwf	MIOS_PARAMETER1
	movwf	MIOS_PARAMETER2
	movlw	CS_MENU_MOD	; menu structure
	call	CS_MENU_ButtonToggle

	movlw	CS_MENU_LFO	; menu structure
	rgoto	CS_MENU_BUTTON_Hlp_MenuChange


	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Hlp_MenuChange
	;; don't change if already in menu
	cpfseq	CS_MENU, ACCESS
	rgoto CS_MENU_BUTTON_Hlp_MenuChangeOk
	return
CS_MENU_BUTTON_Hlp_MenuChangeOk
	;; init menu page
	movwf	CS_MENU
	call	CS_MENU_Page_Init
	;; return

CS_MENU_BUTTON_DisplayUpdateReq
	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return
