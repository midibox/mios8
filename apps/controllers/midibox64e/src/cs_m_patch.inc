; $Id$
;
; MB64E Menu "Patterns"
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#if DEFAULT_PATCHMODE == 0
CS_M_PATCH_Exit
CS_M_PATCH_Snapshot
CS_M_PATCH_Tick
	return

CS_M_PATCH_Init
	;; print patch name at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintPatchName

	;; print static string
	TABLE_ADDR CS_M_PATCH_STR_10
	goto	MIOS_LCD_PrintString
CS_M_PATCH_STR_10 STRING 16, 0x40, "Not in Patchmode"


#else

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_PATCH_Exit
	return


;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_PATCH_Snapshot
	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_PATCH_Snapshot_Depressed

CS_M_PATCH_Snapshot_Pressed
	;; install delayed callback hook, used to trigger the format function after 2 seconds
	TABLE_ADDR MB64E_BANK_FormatPatch
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; print message
	TABLE_ADDR CS_M_PATCH_FORMAT_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_PATCH_FORMAT_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_PATCH_FORMAT_STR_1	STRING 16, 0x40, " to format Patch"

CS_M_PATCH_Snapshot_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop

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
CS_M_PATCH_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; ignore if no bankstick connected
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK
	return	

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_PATCH_GP_R
CS_M_PATCH_GP_L
	;; left side selects pattern group A-H/a-h
	;; 8 buttons at the right side (which select 1-8) should flash now
	bcf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT
	;; 8 buttons at the left side not
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT
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
	rgoto	CS_M_PATCH_GP_Cont

CS_M_PATCH_GP_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

CS_M_PATCH_GP_Cont

	;; save value if one of the right buttons has been pressed
	RCALL_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_PATCH_ValueChanged

	;; update the LEDs and exit
	;; 	rgoto	CS_M_PATCH_UpdateGPLEDs

CS_M_PATCH_UpdateGPLEDs
	;; first 8 leds: (CS_MENU_PARAMETER_L)[5..3]
	movf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: (CS_MENU_PARAMETER_L)[2..0]
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_PATCH_Init
	movlw	128-1
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current patch into parameter
	movf	MB_PATCH, W
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install "changed" hook
	TABLE_ADDR CS_M_PATCH_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_PATCH_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_PATCH_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT

	;; update LEDs
	rcall	CS_M_PATCH_UpdateGPLEDs

	;; print patch name at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintPatchName

	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, CS_M_PATCH_Init_WithBS
CS_M_PATCH_Init_WithoutBS
	;; print static string
	TABLE_ADDR CS_M_PATCH_STR_11
	goto	MIOS_LCD_PrintString

CS_M_PATCH_Init_WithBS
	;; print static string
	TABLE_ADDR CS_M_PATCH_STR_10
	goto	MIOS_LCD_PrintString

CS_M_PATCH_STR_10 STRING 11, 0x40, "Act. Patch:"
CS_M_PATCH_STR_11 STRING 16, 0x40, "* No BankStick *"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATCH_Tick
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK; ignore if no bankstick connected
	return	

	;; print patch number
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_LCD_PrnPatch

	;; print '*' if bank not available
	movlw	'*'
	CALL_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MIOS_LCD_PrintChar

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x4b
	movwf	TMP1
	movlw	0x03
	movwf	TMP2
	BRA_IFCLR MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_PATCH_Tick_FlashAll
	BRA_IFSET MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_PATCH_Tick_FlashAll
CS_M_PATCH_Tick_FlashR
	incf	TMP1, F
	decf	TMP2, F
CS_M_PATCH_Tick_FlashAll
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_PATCH_ValueChanged
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK; ignore if no bankstick connected
	return	

	;; change to a new patch
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_PATCH_Change

	;; and update LEDs
	rgoto	CS_M_PATCH_UpdateGPLEDs

;; ==========================================================================

#endif
