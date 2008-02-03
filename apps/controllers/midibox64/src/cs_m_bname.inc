; $Id$
;
; MB64 Menu for editing the bankname
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
CS_M_BNAME_Exit
	goto	MB64_DUMP_StoreBankName

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_BNAME_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor position, wrap at 16
	incf	CS_MENU_CURSOR_POS, W
	andlw	0x0f
	movwf	CS_MENU_CURSOR_POS

	;; update menu parameter
	rgoto	CS_M_BNAME_Update_MenuParam

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_BNAME_Init
	;; set cursor to first position
	clrf	CS_MENU_CURSOR_POS

	;; 0x60 characters are available
	movlw	0x60-1
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; update menu parameter
	rcall	CS_M_BNAME_Update_MenuParam

	;; install "changed" hook
	TABLE_ADDR CS_M_BNAME_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print header line
	TABLE_ADDR CS_M_BNAME_STR_0
	goto	MIOS_LCD_PrintString

CS_M_BNAME_STR_0 STRING 16, 0x00, "Edit Bankname:  "

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_BNAME_ValueChanged
	;; copy menu value to MB64_BANKNAME[CS_MENU_CURSOR_POS]
	rcall	CS_M_BNAME_Hlp_GetFSR
	movf	CS_MENU_PARAMETER_L, W
	addlw	0x20
	movwf	INDF0

	;; extra: if the name only consists of characters <= 0x20, zero the bankname
	;; if any character > 0x20, ensure that first character is >= 0x20
	movlw	0x10
	movwf	TMP1
	clrf	TMP2
	lfsr	FSR0, MB64_BANKNAME_BEGIN
CS_M_BNAME_ValueChangedChkLoop
	movlw	0x20
	cpfsgt	POSTINC0, ACCESS
	incf	TMP2, F
	decfsz	TMP1, F
	rgoto	CS_M_BNAME_ValueChangedChkLoop

	movlw	0x10
	cpfseq	TMP2, ACCESS
	rgoto CS_M_BNAME_ValueChangedSet0

	lfsr	FSR0, MB64_BANKNAME_BEGIN
CS_M_BNAME_ValueChangedClrLoop
	clrf	POSTINC0
	decfsz	TMP2, F
	rgoto	CS_M_BNAME_ValueChangedClrLoop

	return

CS_M_BNAME_ValueChangedSet0
	lfsr	FSR0, MB64_BANKNAME_BEGIN
	movlw	0x20
	cpfslt	INDF0, ACCESS
	return
	movwf	INDF0
	return


;; ==========================================================================
	;; returns pointer to MB64_BANKNAME[CS_MENU_CURSOR_POS] in FSR0
CS_M_BNAME_Hlp_GetFSR
	lfsr	FSR0, MB64_BANKNAME_BEGIN
	movf	CS_MENU_CURSOR_POS, W
	andlw	0x0f
	addwf	FSR0L, F
	return
	

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_BNAME_Tick
	;; print bank name at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
#if DEFAULT_PATCHMODE == 0
	call	MB64_LCD_PrintBankName
#else
	call	MB64_LCD_PrintPatchName
#endif
	;; expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movf	CS_MENU_CURSOR_POS, W
	iorlw	0x40
	movwf	TMP1
	movlw	0x01
	movwf	TMP2
	goto	CS_MENU_CursorHandler

;; ==========================================================================

	;; updates the menu parameter depending on menu position
CS_M_BNAME_Update_MenuParam
	rcall	CS_M_BNAME_Hlp_GetFSR
	movf	INDF0, W
	addlw	-0x20
	btfsc	WREG, 7
	movlw 0x00
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

