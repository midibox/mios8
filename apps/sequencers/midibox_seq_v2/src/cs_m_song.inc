; $Id$
;
; MBSEQ Menu "Song"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_SONG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,	0x01, 2,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x08, 3,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x90,	0x0b, 5,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40, 2,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x43, 2,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x46, 2,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x49, 2,	CS_M_SONG_ValueChanged


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SONG_Select
	;; check if current value has to be saved in BankStick before incrementing cursor
	rcall	CS_M_SONG_WriteValueChk
	;; increment cursor
	goto	CS_MENU_IncCursorPos

	;; increment cursor
	goto	CS_MENU_IncCursorPos


;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_SONG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_GP_Callback_Song
	addlw	-1
	bz	CS_M_SONG_GP_Callback_Pos
	addlw	-1
	bz	CS_M_SONG_GP_Callback_Loop
	;; else call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel
	;; save value if one of the right buttons has been pressed
	RCALL_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SONG_ValueChanged
	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set song number
CS_M_SONG_GP_Callback_Song
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L
	rcall	CS_M_SONG_ValueChanged
	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set song pos (button * 8)
CS_M_SONG_GP_Callback_Pos
	movf	MIOS_PARAMETER1, W
	mullw	8
	movff	PRODL, CS_MENU_PARAMETER_L
	rcall	CS_M_SONG_ValueChanged

	;; if sequencer not running, start it now:
	CALL_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Start

	;; request pattern change
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos

	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set loop counter
CS_M_SONG_GP_Callback_Loop
	movlw	0x10
	cpfsgt	SEQ_EDIT_SONG_PLAY, BANKED
	rgoto CS_M_SONG_GP_Callback_LoopX
CS_M_SONG_GP_Callback_LoopJ
	btfss	MIOS_PARAMETER1, 3; do nothing if button 1-8 is pressed
	return	
	movf	CS_MENU_PARAMETER_L, W
	addlw	-0x11
	andlw	0xf8
	iorwf	MIOS_PARAMETER1, W
	addlw	0x11
	rgoto	CS_M_SONG_GP_Callback_LoopCont

CS_M_SONG_GP_Callback_LoopX
	incf	MIOS_PARAMETER1, W
CS_M_SONG_GP_Callback_LoopCont
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_SONG_ValueChanged
	;; 	rgoto	CS_M_SONG_GP_Callback_Cont

CS_M_SONG_GP_Callback_Cont
	;; update LEDs
	
CS_M_SONG_UpdateGPLEDs
	SET_BSR	SEQ_BASE

	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_UpdateGPLEDs_Song
	addlw	-1
	bz	CS_M_SONG_UpdateGPLEDs_Pos
	addlw	-1
	bz	CS_M_SONG_UpdateGPLEDs_Loop
	;; else branch to LED function of CS_M_PATTERN (re-use)
	goto	CS_M_PATTERN_UpdateGPLEDs

CS_M_SONG_UpdateGPLEDs_Song
	movf	SEQ_SONG, W, BANKED
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Pos
	rrf	SEQ_EDIT_SONG_POS, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Loop
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	skpnz
	return
	movlw	0x10
	cpfsgt	SEQ_EDIT_SONG_PLAY, BANKED
	rgoto CS_M_SONG_UpdateGPLEDs_LoopX
CS_M_SONG_UpdateGPLEDs_LoopJ
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	addlw	-0x11
	andlw	0x07
	iorlw	0x08
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_LoopX
	decf	SEQ_EDIT_SONG_PLAY, W, BANKED
	;; 	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Cont
	movwf	TMP1
	call	MIOS_HLP_GetBitORMask
	btfss	TMP1, 3
	movwf CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	TMP1, 3
	movwf CS_MENU_GP_OVERLAY_DOUT_SR1
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SONG_Init
	;; read record of current selected song/position
	rcall	CS_M_SONG_ReadSongRecord

	;; install parameter table
	TABLE_ADDR CS_M_SONG_PARAMETERS	; pointer to table
	movlw	7			; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy selected register into menu parameter
	lfsr	FSR0, SEQ_SONG
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_Init_Song
	decf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_Init_Pos
	addlw	-1
	bnz	CS_M_SONG_Init_NotP
CS_M_SONG_Init_Song
CS_M_SONG_Init_Play
CS_M_SONG_Init_Pos
	movf	CS_MENU_CURSOR_POS, W
	movf	PLUSW0, W
	andlw	0x7f
	rgoto	CS_M_SONG_Init_P_Cont

CS_M_SONG_Init_NotP
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	movf	CS_MENU_CURSOR_POS, W
	BRA_IFSET PLUSW0, 7, ACCESS, CS_M_SONG_Init_PMuted
CS_M_SONG_Init_PNotMuted
	incf	PLUSW0, W
	rgoto	CS_M_SONG_Init_P_Cont
CS_M_SONG_Init_PMuted
	movlw	0x00
CS_M_SONG_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_SONG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_SONG_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; clear item counter
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; print static info string
	call	SEQ_LCD_PrintInfoStatic

	;; print static string
	TABLE_ADDR CS_M_SONG_STR_BA_0
	BRA_IFSET MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, CS_M_SONG_Init_BA
CS_M_SONG_Init_BNA
	TABLE_ADDR CS_M_SONG_STR_BNA_0
	call	MIOS_LCD_PrintString
CS_M_SONG_Init_BA
	goto	MIOS_LCD_PrintString

CS_M_SONG_STR_BA_0	STRING  8, 0x00, "S    Pos"

CS_M_SONG_STR_BNA_0	STRING 16, 0x00, "* No BankStick *"
CS_M_SONG_STR_BNA_1	STRING 16, 0x40, "* for song :-/ *"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SONG_Tick
	;; ignore first step (for relaxed timings)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	CS_M_SONG_Tick_End

	decf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_M_SONG_Tick_2
CS_M_SONG_Tick_1
	;; print song info
	call	SEQ_LCD_PrintInfo
	rgoto	CS_M_SONG_Tick_End

CS_M_SONG_Tick_2
	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, CS_M_SONG_Tick_End

	;; if cursor line is disabled: check if value selected has to be saved
	RCALL_IFCLR CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_SONG_WriteValueChk


	;; print song number
	movlw	0x01
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG, W, BANKED
	andlw	0x7f
	call	MIOS_LCD_PrintBCD2
	SET_BSR	SEQ_BASE
	movlw	' '
	btfsc	SEQ_SONG, 7, BANKED
	movlw '*'
	rcall	CS_M_SONG_Hlp_PrintChar

	;; print song position
#if 0
	movlw	0x08
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_EDIT_SONG_POS, W, BANKED
	call	MIOS_LCD_PrintBCD3
#else
	;; new naming style
	movlw	0x09
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_LCD_PrnSongPos
#endif

	;; print play parameter
	movlw	0x0b
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, CS_M_SONG_Tick_NoPlay
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	bnz	CS_M_SONG_Tick_Play
CS_M_SONG_Tick_NoPlay
	movlw	' '
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'S'
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	't'
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'o'
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'p'
	rcall	CS_M_SONG_Hlp_PrintChar
	rgoto	CS_M_SONG_Tick_Play_End
CS_M_SONG_Tick_Play
	movlw	0x10
	cpfsgt	SEQ_EDIT_SONG_PLAY, BANKED
	rgoto CS_M_SONG_Tick_Play_NoR
CS_M_SONG_Tick_Play_R
	movlw	'-'
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'>'
	rcall	CS_M_SONG_Hlp_PrintChar
#if 0
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	addlw	-0x10
	call	MIOS_LCD_PrintBCD3
#else
	;; new style pos naming
	movlw	' '
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	addlw	-0x10-1
	call	SEQ_LCD_PrnSongPos
#endif
	rgoto	CS_M_SONG_Tick_Play_End

CS_M_SONG_Tick_Play_NoR
	movlw	' '
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'x'
	rcall	CS_M_SONG_Hlp_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	rcall	CS_M_SONG_Hlp_PrintChar
CS_M_SONG_Tick_Play_End

	;; pattern number of group 0-3
	movlw	0x40
	call	MIOS_LCD_CursorSet
	clrf	TMP5
CS_M_SONG_Tick_Loop
	;; saved pattern number -> TMP4
	lfsr	FSR0, SEQ_EDIT_SONG_P0
	movf	TMP5, W
	movff	PLUSW0, TMP4
	;; if TMP5 == CURSOR_POS, write converted CS_MENU_PARAMETER_L to TMP4
	movf	TMP5, W
	addlw	3
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_Tick_Loop_NMP
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP4
CS_M_SONG_Tick_Loop_NMP
	
	BRA_IFCLR TMP4, 7, ACCESS, CS_M_SONG_Tick_Loop_NM
CS_M_SONG_Tick_Loop_M
	movlw	'-'
	rcall	CS_M_SONG_Hlp_PrintChar
	movlw	'-'
	rcall	CS_M_SONG_Hlp_PrintChar
	rgoto	CS_M_SONG_Tick_Loop_C
CS_M_SONG_Tick_Loop_NM
	movf	TMP4, W
	call	SEQ_LCD_PrnPattern
CS_M_SONG_Tick_Loop_C
	movlw	' '
	call	MIOS_LCD_PrintChar
	incf	TMP5, F
	BRA_IFCLR TMP5, 2, ACCESS, CS_M_SONG_Tick_Loop

	;; cursor and GP buttons
	movlw	0x03
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_Tick_Loop_PSel
CS_M_SONG_Tick_Loop_NPSel
	call	CS_MENU_CursorHandler

CS_M_SONG_Tick_End
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	3
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	clrf CS_MENU_REFRESH_ITEM_CTR
	return




CS_M_SONG_Tick_Loop_PSel	
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_SONG_Tick_FlashAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_SONG_Tick_FlashR
CS_M_SONG_Tick_FlashAll
	;; flash both digits
	goto	CS_MENU_CursorHandler

CS_M_SONG_Tick_FlashR
	;; flash only right digit
	incf	CS_MENU_CURSOR_LCD_POS, F
	decf	CS_MENU_CURSOR_LCD_WIDTH, F
	call	CS_MENU_CursorHandler
	decf	CS_MENU_CURSOR_LCD_POS, F
	incf	CS_MENU_CURSOR_LCD_WIDTH, F
	return

;; ==========================================================================

	;; this function is used to save some bytes
	;; (a call consumes 2 words, a rcall only 1 word)
CS_M_SONG_Hlp_PrintChar
	goto	MIOS_LCD_PrintChar

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SONG_ValueChanged
	;; copy menu parameter to SEQ_EDIT_SONG*
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_ValueChanged_Song
	decf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_ValueChanged_Pos
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NotP
CS_M_SONG_ValueChanged_Pos
	movff	CS_MENU_PARAMETER_L, TMP1
	rgoto	CS_M_SONG_ValueChanged_Cont
CS_M_SONG_ValueChanged_Song
	movff	CS_MENU_PARAMETER_L, TMP1
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SONG, 7, BANKED	; copy "song disabled" bit
	bsf	TMP1, 7
	rgoto	CS_M_SONG_ValueChanged_Cont

CS_M_SONG_ValueChanged_NotP
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

CS_M_SONG_ValueChanged_Cont
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_SONG
	movf	CS_MENU_CURSOR_POS, W
	movff	TMP1, PLUSW0

	;; if CURSOR_POS == 0 or 1 and not song mode, change pattern immediately
	bz	CS_M_SONG_ValueChanged_ChkSng1
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NoChkSng1
CS_M_SONG_ValueChanged_ChkSng1
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_ValueChanged_ChkSngEnd
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos
	SET_BSR	SEQ_BASE
	rgoto	CS_M_SONG_ValueChanged_ChkSngEnd

CS_M_SONG_ValueChanged_NoChkSng1

	;; if CURSOR_POS == 2 and not song mode, activate it now
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NoChkSng2
CS_M_SONG_ValueChanged_ChkSng2
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_ValueChanged_ChkSngEnd
	bcf	SEQ_SONG, 7, BANKED
	call	SEQ_SONG_EnterSongMode
	SET_BSR	SEQ_BASE
	rgoto	CS_M_SONG_ValueChanged_ChkSngEnd
CS_M_SONG_ValueChanged_NoChkSng2
CS_M_SONG_ValueChanged_ChkSngEnd

	;; if CURSOR_POS >=3 and not in play mode, activate it
	movlw	0x03-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_ValueChanged_NoChkPly
CS_M_SONG_ValueChanged_ChkPly
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	bnz	CS_M_SONG_ValueChanged_NoChkPly
	movlw	0x01
	movwf	SEQ_EDIT_SONG_PLAY, BANKED
	;; save value in EEPROM
	movlw	SEQ_SBANK_SECTOR_PLAY
	movwf	SEQ_SBANK_SECTOR, BANKED
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	movf	SEQ_EDIT_SONG_PLAY, W, BANKED
	call	SEQ_SBANK_Write
CS_M_SONG_ValueChanged_NoChkPly

	;; if CURSOR_POS <= 1: read record of current selected song/position
	movlw	0x01
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rcall CS_M_SONG_ReadSongRecord

	;; update LEDs
	rgoto	CS_M_SONG_UpdateGPLEDs

;; ==========================================================================

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  function to read a song record
;; --------------------------------------------------------------------------
CS_M_SONG_ReadSongRecord
	SET_BSR	SEQ_BASE
	movlw	SEQ_SBANK_SECTOR_PLAY
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_PLAY, BANKED

	movlw	SEQ_SBANK_SECTOR_P0
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_P0, BANKED

	movlw	SEQ_SBANK_SECTOR_P1
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_P1, BANKED

	movlw	SEQ_SBANK_SECTOR_P2
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_P2, BANKED

	movlw	SEQ_SBANK_SECTOR_P3
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_P3, BANKED
	return


CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_SBANK_SECTOR, BANKED
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	goto	SEQ_SBANK_Read


;; --------------------------------------------------------------------------
;;  this function checks if the currently selected value has to be saved in BankStick
;;  ATTENTION: this function is also called by SEQ_BUTTON* when a new menu position is selected directly by 1/2/3/4/A/B/C button
;; --------------------------------------------------------------------------
CS_M_SONG_WriteValueChk
	;; we have to save MIOS_PARAMETER[12] here since it is used by the button routines
	movff	MIOS_PARAMETER1, TMP1
	movff	MIOS_PARAMETER2, TMP2

	;; don't save if cursor pos <= 1
	movlw	0x01
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_WriteValueChk_End

	;; select sector depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	addlw	-1
	movwf	SEQ_SBANK_SECTOR, BANKED

	;; read value from BankStick depending on song position
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	call	SEQ_SBANK_Read
	decf	EEADR, F	; (fix address for upcoming write)
	movwf	TMP5		; save value in TMP5

	;; check with current edit value
	lfsr	FSR0, SEQ_SONG
	movf	CS_MENU_CURSOR_POS, W
	movf	PLUSW0, W
	xorwf	TMP5, W
	bz	CS_M_SONG_WriteValueChk_End

	;; value not equal: save it in BankStick
#if 0
	;; (debugging: output a MIDI start event whenever the value will be saved in BankStick
	movlw	0xfa
	call	MIOS_MIDI_TxBufferPut
#endif
	movf	CS_MENU_CURSOR_POS, W
	movf	PLUSW0, W
	call	SEQ_SBANK_Write

	;; if not in song mode, change pattern
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_WriteValueChk_NoRfrsh
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos
	SET_BSR	SEQ_BASE
CS_M_SONG_WriteValueChk_NoRfrsh

CS_M_SONG_WriteValueChk_End
	;; restore MIOS_PARAMETER[12]
	movff	TMP1, MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2
	return
