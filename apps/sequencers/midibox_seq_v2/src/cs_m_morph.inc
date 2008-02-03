; $Id$
;
; MBSEQ Menu "Morphing"
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
CS_M_MORPH_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY 0x00,					0x7f,	0x40, 2,	CS_M_MORPH_PatternChanged
 CS_MENU_PARAMETER_ENTRY 0x00,					0x7f,	0x43, 3,	CS_M_MORPH_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x47, 1,	CS_M_HLP_TrkChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MORPH_MODE,		0x02,	0x49, 4,	CS_M_MORPH_ModeChanged

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
CS_M_MORPH_GP_Callback
	;; do nothing if cursor position != 0
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_MORPH_GP_Callback_Cont

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_MORPH_GP_Callback_R
CS_M_MORPH_GP_Callback_L
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
	rgoto	CS_M_MORPH_GP_Callback_Cont

CS_M_MORPH_GP_Callback_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

	;; change value (will also update LEDs)
	rgoto	CS_M_MORPH_PatternChanged

	;; 	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_Cont

	;; update the LEDs and exit
	;; 	rgoto	CS_M_PATTERN_UpdateGPLEDs


CS_M_MORPH_UpdateGPLEDs
	;; clear LEDs if cursor pos != 0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_MORPH_UpdateGPLEDs_End

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

CS_M_MORPH_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MORPH_Init
	;; install parameter table
	TABLE_ADDR CS_M_MORPH_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; if cursor pos == 0, change morph pattern
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_MORPH_Init_NotPos0
CS_M_MORPH_Init_Pos0
	movff	SEQ_MORPH_PATTERN0, CS_MENU_PARAMETER_L
CS_M_MORPH_Init_NotPos0

	;; if cursor pos == 1, change morph value
	decf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_MORPH_Init_NotPos1
CS_M_MORPH_Init_Pos1
	movff	SEQ_MORPH_VALUE, CS_MENU_PARAMETER_L
CS_M_MORPH_Init_NotPos1

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_MORPH_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_MORPH_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static chars
	TABLE_ADDR CS_M_MORPH_STR0
	goto	MIOS_LCD_PrintString

CS_M_MORPH_STR0 STRING 12, 0x00, "Pt Val T Mod"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MORPH_Tick
	;; print morph pattern
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	SEQ_MORPH_PATTERN0, WREG
	movf	CS_MENU_CURSOR_POS, F
	skpnz
	movf	CS_MENU_PARAMETER_L, W	
	call	SEQ_LCD_PrnPattern

	;; print morph value
	movlw	0x43
	call	MIOS_LCD_CursorSet
	movff	SEQ_MORPH_VALUE, WREG
	call	MIOS_LCD_PrintBCD3

	;; print track number
	movlw	0x47
	call	MIOS_LCD_CursorSet
	call	CS_M_HLP_PrnTrk

	;; print morphing mode depending on selected track
	movlw	SEQ_CC_OFFSET_MORPH_MODE
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1

	movlw	0x49
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_MORPH_NAME_TABLE	; initial pointer to table
	movf	TMP1, W
	mullw	CS_M_MORPH_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_MORPH_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MORPH_PatternChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MORPH_PATTERN0
	;; update LEDs
	rgoto	CS_M_MORPH_UpdateGPLEDs

CS_M_MORPH_ValueChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MORPH_VALUE
	return

CS_M_MORPH_ModeChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_MORPH_MODE
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================

CS_M_MORPH_NAME_LEN		EQU	4	; 4 characters per entry
CS_M_MORPH_NAME_ENTRIES		EQU	3	; 3 entries
CS_M_MORPH_NAME_TABLE				; all entries are listed here
	db	"off "
	db	"Smth"
	db	"Thrs"
