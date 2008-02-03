; $Id$
;
; MB64E Menu LED ring pattern Handler
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
CS_M_LEDRP_Exit
	;; exit "show selected" and "mute encoders" mode
	bcf	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	bcf	MB_STAT2, MB_STAT2_SHOW_SELECTED
	bcf	MB_STAT2, MB_STAT2_SHOW_SEL_PATTERN

	;; store ledring patterns and exit
	goto	MB64E_DUMP_StoreLEDRP

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_LEDRP_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_LEDRP_Init
	;; enter "show selected" and "mute encoders" mode
	bsf	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	bsf	MB_STAT2, MB_STAT2_SHOW_SELECTED
	bsf	MB_STAT2, MB_STAT2_SHOW_SEL_PATTERN

	;; init menu parameter
	movlw	3
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; update the menu value
	rcall	CS_M_LEDRP_UpdateValue

	;; and install the hook
	TABLE_ADDR CS_M_LEDRP_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; init two display lines
	TABLE_ADDR CS_M_LEDRP_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_LEDRP_ValueChanged
	;; note: mode values stored in packed format:
	;; every mode entry allocates only 4 bit
	lfsr	FSR0, MB64E_LEDRING_PATTERNS
	rrf	CS_MENU_LAST_ENC_ENTRY, W
	andlw	0x3f
	addwf	FSR0L, F

	;; read
	movf	INDF0, W
	btfsc	CS_MENU_LAST_ENC_ENTRY, 0
	swapf	WREG, W

	;; modify
	andlw	0xf0
	iorwf	CS_MENU_PARAMETER_L, W

	;; write
	btfsc	CS_MENU_LAST_ENC_ENTRY, 0
	swapf	WREG, W
	movwf	INDF0

	return

;; --------------------------------------------------------------------------
;;  Stores the current menu value depending on MB64E_LAST_ENTRY_MAPPED
;; --------------------------------------------------------------------------
CS_M_LEDRP_UpdateValue
	;; store bank and encoder entry number
	movff	MB64E_LAST_ENTRY_MAPPED, CS_MENU_LAST_ENC_ENTRY
	movff	MB_BANK, CS_MENU_LAST_BANK

	;; note: mode values stored in packed format:
	;; every mode entry allocates only 4 bit
	lfsr	FSR0, MB64E_LEDRING_PATTERNS
	rrf	CS_MENU_LAST_ENC_ENTRY, W
	andlw	0x3f
	movf	PLUSW0, W
	btfsc	CS_MENU_LAST_ENC_ENTRY, 0
	swapf	WREG, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_LEDRP_Tick
	;; check if encoder entry number has been changed
	;; in this case update new mode setting
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	cpfseq	CS_MENU_LAST_ENC_ENTRY, ACCESS
	rcall CS_M_LEDRP_UpdateValue

	;; check if bank has been changed
	;; in this case store current pattern settings in old bank and switch to new bank
	movf	MB_BANK, W
	xorwf	CS_MENU_LAST_BANK, W
	bz	CS_M_LEDRP_Tick_NoBankChange
CS_M_LEDRP_Tick_BankChange
	;; store ledring patterns on bank changes
	call	MB64E_DUMP_StoreLEDRP
CS_M_LEDRP_Tick_NoBankChange

	;; print encoder number at first line
	movlw	0x07
	call	MIOS_LCD_CursorSet
	SET_BSR	MB64E_BASE
	movf	MB64E_LAST_ENTRY_MAPPED, W, BANKED
	addlw	1
	call	MIOS_LCD_PrintBCD3

	;; print pattern number at second line
	movlw	0x49
	call	MIOS_LCD_CursorSet
	incf	CS_MENU_PARAMETER_L, W
	call	MIOS_LCD_PrintBCD1

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x49
	movwf	TMP1
	movlw	0x01
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows


;; ==========================================================================

CS_M_LEDRP_STR_0 STRING 10, 0x00, "Entry #xxx"
CS_M_LEDRP_STR_1 STRING 10, 0x40, "Pattern #x"
