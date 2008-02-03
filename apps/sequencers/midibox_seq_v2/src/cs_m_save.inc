; $Id$
;
; MBSEQ Menu "Save Pattern"
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
CS_M_SAVE_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x44, 2,	CS_M_SAVE_SavePatternChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x04,	0x47, 6,	CS_M_SAVE_SaveModeChanged

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
CS_M_SAVE_GP_Callback
	;; do nothing if cursor position != 0
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_SAVE_GP_Callback_Cont

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SAVE_GP_Callback_R
CS_M_SAVE_GP_Callback_L
	;; left side selects pattern group A-H/a-h
	;; 8 buttons at the right side (which select 1-8) should flash now
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	;; 8 buttons at the left side not
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	;; select pattern group A-H/a-h depending on pressed button --- don't change bank until 1-8 button has been pressed!

	;; mask out first 3 bits (save current pattern number in TMP1)
	movf	CS_MENU_PARAMETER_L, W
	movwf	TMP1
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L

	;; add button number [2..0] * 8 to pattern number
	movf	MIOS_PARAMETER1, W
	mullw	8
	movf	PRODL, W
	andlw	0x38
	iorwf	CS_MENU_PARAMETER_L, F

	;; if new pattern number equal to old button number, toggle 6th bit
	movf	TMP1, W
	xorwf	CS_MENU_PARAMETER_L, W
	skpnz
	btg	CS_MENU_PARAMETER_L, 6

	;; continue
	rgoto	CS_M_SAVE_GP_Callback_Cont

CS_M_SAVE_GP_Callback_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

	;; change value (will also update LEDs)
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_PATTERN
	rgoto	CS_M_SAVE_SavePatternChanged

	;; 	rgoto	CS_M_SAVE_GP_Callback_Cont
CS_M_SAVE_GP_Callback_Cont

	;; update the LEDs and exit
	;; 	rgoto	CS_M_PATTERN_UpdateGPLEDs


CS_M_SAVE_UpdateGPLEDs
	;; clear LEDs if cursor pos != 0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_SAVE_UpdateGPLEDs_End

	;; else:

	;; first 8 leds: CS_MENU_PARAMETER_L[5..3]
	rlf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: CS_MENU_PARAMETER_L[2..0]
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

CS_M_SAVE_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SAVE_Select
	;; increment cursor
	call	CS_MENU_IncCursorPos

	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_SAVE_Select_Depressed

CS_M_SAVE_Select_Pressed
	;; install delayed callback hook, used to trigger the save function after 2 seconds
	TABLE_ADDR CS_M_SAVE_Select_Copy
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; print message
	TABLE_ADDR CS_M_SAVE_HOLD_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_SAVE_HOLD_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_SAVE_HOLD_STR_1	STRING 16, 0x40, "to save pattern "

CS_M_SAVE_Select_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop


	;; callback function
CS_M_SAVE_Select_Copy
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN
	call	SEQ_DUMP_StorePartial

	;; change to new pattern
	SET_BSR	SEQ_BASE
	bcf	SEQ_NEXT_PATTERN0, 7, BANKED	; ensure that new pattern will be loaded by sequencer
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN0
	call	SEQ_CORE_ChangePattern

	;; clear cursor
	clrf	CS_MENU_CURSOR_POS

	;; print message	
	TABLE_ADDR CS_M_SAVE_FINISHED_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_SAVE_FINISHED_STR_0 STRING 16, 0x00, "*** Pattern  ***"
CS_M_SAVE_FINISHED_STR_1 STRING 16, 0x40, "***  saved   ***"

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SAVE_Init
	;; install parameter table
	TABLE_ADDR CS_M_SAVE_PARAMETERS		; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy save pattern number into parameter
	movff	SEQ_SAVE_PATTERN, CS_MENU_PARAMETER_L
	;; if cursor pos != 0: the mode instead
	movf	CS_MENU_CURSOR_POS, W
	skpz
	movff	SEQ_SAVE_MODE, CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_SAVE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_SAVE_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static string
	TABLE_ADDR CS_M_SAVE_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_SAVE_STR_0	STRING 12, 0x00, "Save Pattern"
CS_M_SAVE_STR_1	STRING  2, 0x42, "->"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SAVE_Tick
	;; print current pattern number at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	SEQ_PATTERN0, WREG
	call	SEQ_LCD_PrnPattern

	;; print copy pattern number at second line
	movlw	0x44
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	movf	SEQ_SAVE_PATTERN, W, BANKED
	movf	CS_MENU_CURSOR_POS, F
	skpnz
	movf	CS_MENU_PARAMETER_L, W	
	call	SEQ_LCD_PrnPattern

	;; print mode
	movlw	0x47
	call	MIOS_LCD_CursorSet
	movlw	'('
	rcall	CS_M_SAVE_Hlp_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_SAVE_MODE, W, BANKED
	bnz	CS_M_SAVE_Tick_ModeNotAll
CS_M_SAVE_Tick_ModeAll
	movlw	'A'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movlw	'l'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movlw	'l'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movlw	')'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movlw	' '
	rcall	CS_M_SAVE_Hlp_PrintChar
	rgoto	CS_M_SAVE_Tick_Cont
CS_M_SAVE_Tick_ModeNotAll
	call	CS_M_HLP_GetSelectedTrk
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	'-'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movlw	'>'
	rcall	CS_M_SAVE_Hlp_PrintChar
	movff	SEQ_SAVE_MODE, WREG
	call	MIOS_LCD_PrintBCD1
	movlw	')'
	rcall	CS_M_SAVE_Hlp_PrintChar
CS_M_SAVE_Tick_Cont


	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_SAVE_Tick_FlashAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_SAVE_Tick_FlashAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_SAVE_Tick_FlashR
CS_M_SAVE_Tick_FlashAll
	;; flash both digits
	goto	CS_MENU_CursorHandler
	
CS_M_SAVE_Tick_FlashR
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
CS_M_SAVE_Hlp_PrintChar
	goto	MIOS_LCD_PrintChar

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SAVE_SavePatternChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_PATTERN
	rgoto	CS_M_SAVE_UpdateGPLEDs

CS_M_SAVE_SaveModeChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_MODE
	rgoto	CS_M_SAVE_UpdateGPLEDs
