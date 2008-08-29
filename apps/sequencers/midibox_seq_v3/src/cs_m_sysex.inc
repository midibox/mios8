; $Id$
;
; MBSEQ Menu "SysEx"
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
CS_M_SYSEX_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x03,	0x40+ 2, 6,	CS_M_SYSEX_SrcChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x06,	0x40+13, 1,	CS_M_SYSEX_TrgBChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+15, 2,	CS_M_SYSEX_TrgPChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+24, 2,	CS_M_SYSEX_IDChanged

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;     o encoder incrementer in CS_MENU_ENC_INCREMENTER if encoder has been used
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;;     should clear CS_MENU_ENC_INCREMENTER if radio-button function desired
;; --------------------------------------------------------------------------
CS_M_SYSEX_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; ignore button 9-16
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SYSEX_GP_Callback_Ignore

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SYSEX_GP_Callback_1
	rgoto	CS_M_SYSEX_GP_Callback_2
	rgoto	CS_M_SYSEX_GP_Callback_3
	rgoto	CS_M_SYSEX_GP_Callback_4
	rgoto	CS_M_SYSEX_GP_Callback_5
	rgoto	CS_M_SYSEX_GP_Callback_6
	rgoto	CS_M_SYSEX_GP_Callback_7
	rgoto	CS_M_SYSEX_GP_Callback_8

CS_M_SYSEX_GP_Callback_1
CS_M_SYSEX_GP_Callback_2
	movlw	0	; select Source (Item #0)
	rgoto	CS_M_SYSEX_GP_Callback_Cont

CS_M_SYSEX_GP_Callback_3
	movlw	1	; select Target Bank (Item #1)
	rgoto	CS_M_SYSEX_GP_Callback_Cont

CS_M_SYSEX_GP_Callback_4
	movlw	2	; select Target Pattern (Item #2)
	rgoto	CS_M_SYSEX_GP_Callback_Cont

CS_M_SYSEX_GP_Callback_5
CS_M_SYSEX_GP_Callback_6
	movlw	3	; select Device ID (Item #3)
	rgoto	CS_M_SYSEX_GP_Callback_Cont

CS_M_SYSEX_GP_Callback_7
CS_M_SYSEX_GP_Callback_8
	;; ignore if encoder has been moved
	movf	CS_MENU_ENC_INCREMENTER, W
	bnz	CS_M_SYSEX_GP_Callback_Cont

	;; send SysEx Dump of internal RAM data (SEQ_PATTERN = 0x80..0x83)
	SET_BSR	SEQ_BASE
	movf	SEQ_SYSEX_SOURCE, W, BANKED
	iorlw	0x80
	movwf	SEQ_PATTERN, BANKED
	goto	SEQ_SYSEX_SendDump

CS_M_SYSEX_GP_Callback_Ignore
	;; (no increments)
	clrf	CS_MENU_ENC_INCREMENTER, W
	return

CS_M_SYSEX_GP_Callback_Cont

	;; update cursor if it has been changed
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SYSEX_GP_Callback_NewCur
	return
CS_M_SYSEX_GP_Callback_NewCur
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_ENC_INCREMENTER	; ignore first turn (CS_MENU_PARAMETER_L and max value not up-to-date yet)
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_SYSEX_UpdateGPLEDs
	SET_BSR	SEQ_BASE

	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SYSEX_UpdateGPLEDs_0
	addlw	-1
	bz	CS_M_SYSEX_UpdateGPLEDs_1
	addlw	-1
	bz	CS_M_SYSEX_UpdateGPLEDs_2
	rgoto	CS_M_SYSEX_UpdateGPLEDs_3

CS_M_SYSEX_UpdateGPLEDs_0
	movlw	b'00000011'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
 	rgoto	CS_M_SYSEX_UpdateGPLEDs_Cont

CS_M_SYSEX_UpdateGPLEDs_1
	movlw	b'00000100'
 	rgoto	CS_M_SYSEX_UpdateGPLEDs_Cont

CS_M_SYSEX_UpdateGPLEDs_2
	movlw	b'00001000'
 	rgoto	CS_M_SYSEX_UpdateGPLEDs_Cont

CS_M_SYSEX_UpdateGPLEDs_3
	movlw	b'00110000'
	;;  	rgoto	CS_M_SYSEX_UpdateGPLEDs_Cont

CS_M_SYSEX_UpdateGPLEDs_Cont
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	return

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SYSEX_Select
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SYSEX_Init
	;; install parameter table
	TABLE_ADDR CS_M_SYSEX_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; transfer value to menu parameter depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SYSEX_Init_0
	addlw	-1
	bz	CS_M_SYSEX_Init_1
	addlw	-1
	bz	CS_M_SYSEX_Init_2
	rgoto	CS_M_SYSEX_Init_3

CS_M_SYSEX_Init_0		; source pattern
	movff	SEQ_SYSEX_SOURCE, CS_MENU_PARAMETER_L
	rcall	CS_M_SYSEX_SrcChanged	; (to update target bank and pattern)
	rgoto	CS_M_SYSEX_Init_Cont

CS_M_SYSEX_Init_1		; target bank
	movff	SEQ_SYSEX_TARGET_BANK, CS_MENU_PARAMETER_L
	rgoto	CS_M_SYSEX_Init_Cont

CS_M_SYSEX_Init_2		; target pattern
	movff	SEQ_SYSEX_TARGET_PATTERN, CS_MENU_PARAMETER_L
	rgoto	CS_M_SYSEX_Init_Cont

CS_M_SYSEX_Init_3		; device ID
	movff	MIDI_DEVICE_ID, CS_MENU_PARAMETER_L
	;; 	rgoto	CS_M_SYSEX_Init_Cont
CS_M_SYSEX_Init_Cont

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_SYSEX_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_SYSEX_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SYSEX_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;;  Pattern    Target   Device ID
	;; -0000000----111111---222222222--------------------------------------------------
	;;  G1 RAM      1:A1       00     >>SEND<<
	;; -444444------5555-------66-----77777777-----------------------------------------

	;; print segments 0-7
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SYSEX_Tick_0
	rgoto	CS_M_SYSEX_Tick_1
	rgoto	CS_M_SYSEX_Tick_2
	rgoto	CS_M_SYSEX_Tick_3
	rgoto	CS_M_SYSEX_Tick_4
	rgoto	CS_M_SYSEX_Tick_5
	rgoto	CS_M_SYSEX_Tick_6
	rgoto	CS_M_SYSEX_Tick_7

CS_M_SYSEX_Tick_0
	call	SEQ_LCD_PrintStringAfterCall
	STRING	7, 0x00 + 1, "Pattern"
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	6, 0x00 + 12, "Target"
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_2
	call	SEQ_LCD_PrintStringAfterCall
	STRING	9, 0x00 + 21, "Device ID"
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_3
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_4
	movlw	0x40 + 1
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SYSEX_Tick_Cont

	movlw	'G'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	incf	SEQ_SYSEX_SOURCE, W, BANKED
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'R'
	call	MIOS_LCD_PrintChar
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	'M'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_5
	movlw	0x40 + 13
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SYSEX_Tick_5_Cont

	SET_BSR	SEQ_BASE
	incf	SEQ_SYSEX_TARGET_BANK, W, BANKED
	call	MIOS_LCD_PrintBCD1

CS_M_SYSEX_Tick_5_Cont
	movlw	0x40 + 14
	call	MIOS_LCD_CursorSet
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SYSEX_Tick_5_Cont
	
	SET_BSR	SEQ_BASE
	movf	SEQ_SYSEX_TARGET_PATTERN, W, BANKED
	call	SEQ_LCD_PrnPattern
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_6
	movlw	0x40 + 24
	call	MIOS_LCD_CursorSet
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SYSEX_Tick_5_Cont
	
	movf	MIDI_DEVICE_ID, W
	call	MIOS_LCD_PrintHex2
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_7
	call	SEQ_LCD_PrintStringAfterCall
	STRING	8, 0x40 + 31, ">>SEND<<"
	;; 	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_Cont
	;; Tick function will be called 8 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_SYSEX_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_SYSEX_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SYSEX_SrcChanged
	SET_BSR	SEQ_BASE
	movf	CS_MENU_PARAMETER_L, W
	movwf	SEQ_SYSEX_SOURCE, BANKED

	;; automatically select current pattern of selected RAM source
	lfsr	FSR1, SEQ_PATTERN0
	movf	PLUSW1, W
	andlw	0x7f
	movwf	SEQ_SYSEX_TARGET_PATTERN, BANKED

	clrf	SEQ_SYSEX_TARGET_BANK, BANKED	
	return

CS_M_SYSEX_TrgBChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SYSEX_TARGET_BANK
	return

CS_M_SYSEX_TrgPChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SYSEX_TARGET_PATTERN
	return

CS_M_SYSEX_IDChanged
	movff	CS_MENU_PARAMETER_L, MIDI_DEVICE_ID
	return

;; ==========================================================================
