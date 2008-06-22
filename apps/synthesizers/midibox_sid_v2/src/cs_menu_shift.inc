; $Id$
;
; MBSID Control Surface SHIFT menu
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Prints the upper line of the CFG page
;; --------------------------------------------------------------------------
CS_MENU_SHIFT_STR_0 STRING 20, 0x00,   "SID1 E. Ply Ini Dmp "
CS_MENU_SHIFT_PrintUL
	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd	

	TABLE_ADDR CS_MENU_SHIFT_STR_0
	call	MIOS_LCD_PrintString

	movlw	0x00 + 3
	call	MIOS_LCD_CursorSet
	call	CS_MENU_MS_GetSIDNumber
	addlw	1
	goto	MIOS_LCD_PrintBCD1
	


;; --------------------------------------------------------------------------
;;  Prints the lower line of the CFG page
;; --------------------------------------------------------------------------
CS_MENU_SHIFT_STR_SYXD_0	db "Pt. "
CS_MENU_SHIFT_STR_SYXD_1	db "Ens "

CS_MENU_SHIFT_PrintLL
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; selected SID channel (Left/Right)
	rcall	CS_MENU_SHIFT_PrintLL_Space
	movlw	'L'
	btfss	CS_MENU_SELECTED_SID_LR, 0
	movlw '-'
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	movlw	'R'
	btfss	CS_MENU_SELECTED_SID_LR, 1
	movlw '-'
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	rcall	CS_MENU_SHIFT_PrintLL_Space

	;; selected Engine
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	call	SID_LCD_PrintEngine
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print play state of selected SID
	rcall	CS_MENU_SHIFT_PrintLL_Space
	call	CS_MENU_MS_GetSIDNumber
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andwf	CS_STAT2, W
	movlw	'o'
	skpz
	movlw	'*'
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	rcall	CS_MENU_SHIFT_PrintLL_Space
	rcall	CS_MENU_SHIFT_PrintLL_Space

	;; Patch/Ensemble
	rcall	CS_MENU_SHIFT_GetSyxType
	movwf	TMP1		; temporary store in TMP1
	btfsc	WREG, 6; 0x70 -> 0x01
	movlw 0x01	
	TABLE_ADDR_MUL_W CS_MENU_SHIFT_STR_SYXD_0, 4
	movlw	4
	call	MIOS_LCD_PrintPreconfString

	BRA_IFSET TMP1, 6, ACCESS, CS_MENU_SHIFT_PrintLL_E
CS_MENU_SHIFT_PrintLL_P
	;; print current bank and patch number
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
	call	SID_LCD_PrintPatchNumber
	rgoto	CS_MENU_SHIFT_PrintLL_P_Cont

CS_MENU_SHIFT_PrintLL_E
	;; print current ensemble number
	call	SID_LCD_PrintEnsembleNumber

CS_MENU_SHIFT_PrintLL_P_Cont

#if DEFAULT_LCD_LINES <= 2
	;; fill
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif
	return

	
	;; help functions
CS_MENU_SHIFT_PrintLL_Space
	movlw	' '
CS_MENU_SHIFT_PrintLL_Chr
	goto	MIOS_LCD_PrintChar

CS_MENU_SHIFT_PrintLL_OnOff
	movwf	TMP1

	rcall	CS_MENU_SHIFT_PrintLL_Space
	movlw	'o'
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	movlw	'f'
	btfsc	TMP1, 0
	movlw 'n'
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	movlw	'f'
	btfsc	TMP1, 0
	movlw ' '
	rcall	CS_MENU_SHIFT_PrintLL_Chr
	rgoto	CS_MENU_SHIFT_PrintLL_Space

CS_MENU_SHIFT_Message
	call	MIOS_LCD_PrintString
	movlw	0x40		; short message
	goto	MIOS_LCD_MessageStart

;; --------------------------------------------------------------------------
;;  A soft button has been pressed
;;  IN: button number in WREG
;; --------------------------------------------------------------------------
CS_MENU_SHIFT_Button
	;; branch depending on button
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_SHIFT_Button_SIDLR
	rgoto	CS_MENU_SHIFT_Button_ENG
	rgoto	CS_MENU_SHIFT_Button_Play
	rgoto	CS_MENU_SHIFT_Button_Init
	rgoto	CS_MENU_SHIFT_Button_SyxD
	return
	return
	return

CS_MENU_SHIFT_Button_SIDLR	; toggle between 1-3 (L-, -R, LR)
	incf	CS_MENU_SELECTED_SID_LR, W
	andlw	0x03
	skpnz
	addlw	1
	movwf	CS_MENU_SELECTED_SID_LR
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_MENU_SHIFT_Button_ENG		; toggle between the 4 engines
	;; increment bitfield [1:0]
	lfsr	FSR0, SID_EDIT_BUFFER + SID_Ix_ENGINE
	movf	INDF0, W
	addlw	1
	andlw	0x03
	movwf	PRODL
	movf	INDF0, W
	andlw	0xfc
	iorwf	PRODL, W
	movwf	INDF0

#if 0
	;; send to SID
	call	CS_MENU_MS_SendParameter
#else
	;; always re-init engine
	;; copy init patch into edit buffer
	call	SID_PATCH_LoadPresetIntoEdit

	;; transfer to all selected SIDs
	call	CS_MENU_MS_Send_EditBufferAll
#endif
	;; ensure that OSC/LFO/ENV/... is within range
	clrf	CS_MENU_SELECTED_INS
	clrf	CS_MENU_SELECTED_OSC
	call	CS_MENU_Hlp_UpdateOSCFlags
	clrf	CS_MENU_SELECTED_LFO
	clrf	CS_MENU_SELECTED_ENV
	clrf	CS_MENU_SELECTED_MOD
	clrf	CS_MENU_SELECTED_TRG
	clrf	CS_MENU_SELECTED_KNOB
	clrf	CS_MENU_SELECTED_WT

	;; switch to root menu
	goto	CS_MENU_EXEC_GoToRoot


CS_MENU_SHIFT_Button_Init_MSG_P	STRING 20, 0x40, " Patch Initialised  "
CS_MENU_SHIFT_Button_Init_MSG_E	STRING 20, 0x40, "Ensemble Initialised"
CS_MENU_SHIFT_Button_Init
	rcall	CS_MENU_SHIFT_GetSyxType
	BRA_IFSET WREG, 6, ACCESS, CS_MENU_SHIFT_Button_Init_E
CS_MENU_SHIFT_Button_Init_P
	;; copy init patch into edit buffer
	call	SID_PATCH_LoadPresetIntoEdit

	;; transfer to all selected SIDs
	call	CS_MENU_MS_Send_EditBufferAll

	;; print message and exit
	TABLE_ADDR CS_MENU_SHIFT_Button_Init_MSG_P
	rcall	CS_MENU_SHIFT_Message
#if DEFAULT_LCD_LINES <= 2
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif
	return

CS_MENU_SHIFT_Button_Init_E
	;; initialize ensemble
	call	SID_ENS_LoadPreset

	;; print message and exit
	TABLE_ADDR CS_MENU_SHIFT_Button_Init_MSG_E
	rcall	CS_MENU_SHIFT_Message
#if DEFAULT_LCD_LINES <= 2
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif
	return

#if 0
CS_MENU_SHIFT_Button_Rnd_MSG	STRING 20, 0x40, " --- Randomized --- "
CS_MENU_SHIFT_Button_Rnd
	;; randomize patch depending on engine
	call	SID_RND_GenerateAll

	;; transfer to all selected SIDs
	call	CS_MENU_MS_Send_EditBufferAll

	;; print message and exit
	TABLE_ADDR CS_MENU_SHIFT_Button_Rnd_MSG
	rcall	CS_MENU_SHIFT_Message
#if DEFAULT_LCD_LINES <= 2
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif
	return
#else
CS_MENU_SHIFT_Button_Off_MSG	STRING 20, 0x40, "-- All Notes Off -- "
CS_MENU_SHIFT_Button_On_MSG	STRING 20, 0x40, "-- Playing Patch -- "

CS_MENU_SHIFT_Button_Play
	;; send "all notes on" or "off" command to all selected SIDs
	;; (we can re-use the play button function)
	call	CS_MENU_BUTTON_Play

	;; print message depending on the current SID selection
	call	CS_MENU_MS_GetSIDNumber
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andwf	CS_STAT2, W
	bz	CS_MENU_SHIFT_Button_Play_Off
CS_MENU_SHIFT_Button_Play_On
	TABLE_ADDR CS_MENU_SHIFT_Button_On_MSG
	rgoto	CS_MENU_SHIFT_Button_Play_Cont
CS_MENU_SHIFT_Button_Play_Off
	TABLE_ADDR CS_MENU_SHIFT_Button_Off_MSG
	;; 	rgoto	CS_MENU_SHIFT_Button_Play_Cont

CS_MENU_SHIFT_Button_Play_Cont
	rcall	CS_MENU_SHIFT_Message
#if DEFAULT_LCD_LINES <= 2
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif
	return
#endif


CS_MENU_SHIFT_Button_SyxD_P_MSG	STRING 20, 0x40, "-- Patch Dump sent -"
CS_MENU_SHIFT_Button_SyxD_E_MSG	STRING 20, 0x40, "-- Ens. Dump sent - "
CS_MENU_SHIFT_Button_SyxD
	;; determine sysex type
	rcall	CS_MENU_SHIFT_GetSyxType
	;; store in SID_SYSEX_TYPE
	iorlw	0x08		; RAM dump (so that bankstick slot won't be overwritten on write-back)
	movff	WREG, SID_SYSEX_TYPE

	;; due to RAM dump, patch/bank should be 0
	clrf	SID_BANK
	clrf	SID_PATCH

	;; temporary: SID_MIDI_DEVICE == selected SID
	movff	CS_MENU_SID, SID_MIDI_DEVICE

	BRA_IFSET WREG, 6, ACCESS, CS_MENU_SHIFT_Button_SyxD_E
CS_MENU_SHIFT_Button_SyxD_P
	;; print message and exit
	TABLE_ADDR CS_MENU_SHIFT_Button_SyxD_P_MSG
	
CS_MENU_SHIFT_Button_SyxD_E_Cont
	rcall	CS_MENU_SHIFT_Message
#if DEFAULT_LCD_LINES <= 2
	movlw	20
	call	CS_MENU_Hlp_PrintSpaces
#endif

	call	SID_SYSEX_SendDump_Edit

	;; restore SID_MIDI_DEVICE (derived from MIOS device ID)
	call	MIOS_MIDI_DeviceIDGet
	movwf	SID_MIDI_DEVICE

	return


CS_MENU_SHIFT_Button_SyxD_E
	;; print message and exit
	TABLE_ADDR CS_MENU_SHIFT_Button_SyxD_E_MSG
	rgoto	CS_MENU_SHIFT_Button_SyxD_E_Cont


;; --------------------------------------------------------------------------
;;  Determines which type of sysex dump should be sent depending on current context
;;  OUT: WREG == 0x00: patch
;;       WREG == 0x70: ensemble
;; --------------------------------------------------------------------------
CS_MENU_SHIFT_GetSyxType
	movlw	0x00
	btfsc	CS_MENU, 6
	movlw 0x70
	return
