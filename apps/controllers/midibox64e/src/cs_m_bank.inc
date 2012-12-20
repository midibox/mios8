; $Id$
;
; MB64E Menu Bank Handler
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
CS_M_BANK_Exit
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
CS_M_BANK_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; else set new bank and call the ValueChanged function (which restores the new bank)
	movlw	0xf0
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	CS_MENU_PARAMETER_L, F

	call	CS_M_BANK_ValueChanged
	;; update the LEDs and exit
	;; 	rgoto	CS_M_BANK_UpdateGPLEDs

CS_M_BANK_UpdateGPLEDs
	;; set "1-of-16" code
	movf	MB_BANK, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	btfsc	MB_BANK, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	movf	TMP1, W
	btfss	MB_BANK, 3
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_BANK_Snapshot
	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_BANK_Snapshot_Depressed

CS_M_BANK_Snapshot_Pressed
	;; install delayed callback hook, used to trigger the format function after 2 seconds
	TABLE_ADDR MB64E_BANK_FormatBank
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; print message
	TABLE_ADDR CS_M_BANK_FORMAT_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_BANK_FORMAT_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_BANK_FORMAT_STR_1	STRING 16, 0x40, " to format Bank "

CS_M_BANK_Snapshot_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_BANK_Init
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 8 banks maximum
	movlw	8-1
#else
	;; 64 banks maximum
	movlw	64-1
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 1 bank maximum
	movlw	1-1
#else
	;; 8 banks maximum
	movlw	8-1
#endif
#endif
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current bank into parameter
	movf	MB_BANK, W
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install "changed" hook
	TABLE_ADDR CS_M_BANK_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_BANK_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_BANK_UpdateGPLEDs

	;; print bank name at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintBankName

	;; print static string
	TABLE_ADDR CS_M_BANK_STR_1
	goto	MIOS_LCD_PrintString

CS_M_BANK_STR_1	STRING 10, 0x40, "Act. Bank:"

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_BANK_ValueChanged
#define MEASURE_LOAD_PERFORMANCE 0
;; if 1, program change statements will be sent. MIOS Studio shows the timestamps, this allows to measure the performance
#if MEASURE_LOAD_PERFORMANCE
	movlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
#endif
	;; change to a new bank
	movf	CS_MENU_PARAMETER_L, W
	call	MB64E_BANK_Change
	;; and update LEDs
	rcall	CS_M_BANK_UpdateGPLEDs

#if MEASURE_LOAD_PERFORMANCE
	movlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
#endif

	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_BANK_Tick
	;; print bank number
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD3

	;; print '*' if bank not available
	movlw	'*'
	CALL_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MIOS_LCD_PrintChar

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x4a
	movwf	TMP1
	movlw	0x04
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================
