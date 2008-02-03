; $Id$
;
; MB64E Menu Copy Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_COPY_Exit
	return

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
CS_M_COPY_GP_Callback
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

#if DEFAULT_PATCHMODE == 0
	;; else set new bank and call the ValueChanged function (which restores the new bank)
	movlw	0xf0
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	CS_MENU_PARAMETER_L, F
	call	CS_M_COPY_ValueChanged
	;; update the LEDs and exit
	;; 	rcall	CS_M_COPY_UpdateGPLEDs
#else

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_COPY_GP_R
CS_M_COPY_GP_L
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
	rgoto	CS_M_COPY_GP_Cont

CS_M_COPY_GP_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

CS_M_COPY_GP_Cont

#endif

CS_M_COPY_UpdateGPLEDs
#if DEFAULT_PATCHMODE == 0
	;; set "1-of-16" code
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	btfsc	CS_MENU_PARAMETER_L, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	movf	TMP1, W
	btfss	CS_MENU_PARAMETER_L, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
#else
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
#endif
	return


;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_COPY_Snapshot
	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, CS_M_COPY_Snapshot_BS
CS_M_COPY_Snapshot_NoBS
	TABLE_ADDR CS_M_COPY_NO_BS
	goto	MIOS_LCD_PrintMessage
CS_M_COPY_NO_BS		STRING 16, 0x00, "* No BankStick *"


CS_M_COPY_Snapshot_BS
	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_COPY_Snapshot_Depressed

CS_M_COPY_Snapshot_Pressed
	;; install delayed callback hook, used to trigger the save function after 2 seconds
	TABLE_ADDR CS_M_COPY_Snapshot_Copy
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; print message
	call	MIOS_LCD_Clear
	TABLE_ADDR CS_M_COPY_HOLD_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
#if DEFAULT_PATCHMODE == 0
CS_M_COPY_HOLD_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_COPY_HOLD_STR_1	STRING 16, 0x40, "  to copy bank  "
#else
CS_M_COPY_HOLD_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_COPY_HOLD_STR_1	STRING 16, 0x40, " to copy patch  "
#endif

CS_M_COPY_Snapshot_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop


	;; callback function
CS_M_COPY_Snapshot_Copy
	;; copy only if source != target bank
	movf	CS_MENU_PARAMETER_L, W
#if DEFAULT_PATCHMODE == 0
	xorwf	MB_BANK, W
#else
	xorwf	MB_PATCH, W
#endif
	skpnz
	return

#if DEFAULT_PATCHMODE == 0
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_BANK_CopyFromExternal
	;; change to new bank
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_BANK_Change
#else
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_BANK_CopyPatch
	;; change to new patch
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_PATCH_Change
#endif

	;; print message	
	TABLE_ADDR CS_M_COPY_FINISHED_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_COPY_FINISHED_STR_0 STRING 16, 0x00, "***   Copy   ***"
CS_M_COPY_FINISHED_STR_1 STRING 16, 0x40, "*** finished ***"


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_COPY_Init

#if DEFAULT_PATCHMODE == 0	
	movlw	64-1	;; 64 banks maximum
#else
	movlw	128-1	;; 128 patches maximum
#endif
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current bank into parameter
#if DEFAULT_PATCHMODE == 0
	movf	MB_BANK, W
#else
	movf	MB_PATCH, W
#endif
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install callback function for general purpose buttons, used as select buttons here
	TABLE_ADDR CS_M_COPY_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_COPY_UpdateGPLEDs

#if DEFAULT_PATCHMODE != 0
	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT
#endif

	;; install "changed" hook
	TABLE_ADDR CS_M_COPY_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	TABLE_ADDR CS_M_COPY_AVAIL_STR_1
	goto	MIOS_LCD_PrintString

CS_M_COPY_AVAIL_STR_1	STRING 11, 0x40, "Copy     =>"

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_COPY_ValueChanged
	;; only update LEDs - value used by CS_M_COPY_Snapshot_Copy
	rgoto	CS_M_COPY_UpdateGPLEDs


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_COPY_Tick
	;; print name of selected bank
	movlw	0x00
	call	MIOS_LCD_CursorSet

#if DEFAULT_PATCHMODE == 0
	movff	MB_BANK, TMP5	; save current bank in TMP5
	movff	CS_MENU_PARAMETER_L, MB_BANK
	call	MB64E_DUMP_RestoreBankName
	call	MB64E_LCD_PrintBankName
	movff	TMP5, MB_BANK	; restore current bank
	call	MB64E_DUMP_RestoreBankName
#else
	movff	MB_PATCH, TMP5	; save current patch in TMP5
	movff	CS_MENU_PARAMETER_L, MB_PATCH
	call	MB64E_DUMP_RestoreBankName
	call	MB64E_LCD_PrintPatchName
	movff	TMP5, MB_PATCH	; restore current patch
	call	MB64E_DUMP_RestoreBankName
#endif
	
	;; else print bank numbers
	movlw	0x45
	call	MIOS_LCD_CursorSet
#if DEFAULT_PATCHMODE == 0
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD3
#else
	movf	MB_PATCH, W
	call	MB64E_LCD_PrnPatch
#endif

	movlw	0x4b
	call	MIOS_LCD_CursorSet
#if DEFAULT_PATCHMODE == 0
	incf	CS_MENU_PARAMETER_L, W
	call	MIOS_LCD_PrintBCD3
#else
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_LCD_PrnPatch
#endif

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x4b
	movwf	TMP1
	movlw	0x03
	movwf	TMP2
#if DEFAULT_PATCHMODE != 0
	BRA_IFCLR MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_COPY_Tick_FlashAll
	BRA_IFSET MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_COPY_Tick_FlashAll
CS_M_COPY_Tick_FlashR
	incf	TMP1, F
	decf	TMP2, F
CS_M_COPY_Tick_FlashAll
#endif
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================
