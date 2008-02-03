; $Id$
;
; MB64 Message Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Use this function to start a message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
MB64_MSG_Start
	SET_BSR	MB64_BASE
	movwf	MB64_MSG_ID, BANKED
	goto	CS_MENU_StartMessage

;; --------------------------------------------------------------------------
;;  Use this function to hold a message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
MB64_MSG_Hold
	SET_BSR	MB64_BASE
	movwf	MB64_MSG_ID, BANKED
	goto	CS_MENU_HoldMessage

;; --------------------------------------------------------------------------
;;  Use this function to stop a message
;;  IN: -
;; --------------------------------------------------------------------------
MB64_MSG_Stop
	goto	CS_MENU_StopMessage

;; --------------------------------------------------------------------------
;;  This function is called by the CS when a message has been requested
;;  (see cs_menu_tables.inc)
;; --------------------------------------------------------------------------
MB64_MSG_Handler
	;; branch depending on message ID
	SET_BSR	MB64_BASE
	movf	MB64_MSG_ID, W, BANKED
	JUMPTABLE_2BYTES 2	; entries
	rgoto	MB64_MSG_Morph	; MB64_MSG_ID_MORPH
	rgoto	MB64_MSG_Bank	; MB64_MSG_ID_BANK

;; --------------------------------------------------------------------------
;;  Morphing Message
;; --------------------------------------------------------------------------
MB64_MSG_MORPH_STR_0 STRING 12, 0x00, "Morph Group "
MB64_MSG_Morph
	;; print selected morph groups at first line
	TABLE_ADDR MB64_MSG_MORPH_STR_0
	call	MIOS_LCD_PrintString
	call	MB64_LCD_PrintMorphGroups

	;; print a horizontal bar at the second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	MB64_MORPH_VALUE, WREG
	goto	MB64_LCD_PrintHBar


;; --------------------------------------------------------------------------
;;  Bank Message
;; --------------------------------------------------------------------------
MB64_MSG_BANK_STR_BSNA_1 STRING 16, 0x40, "* No BankStick *"
MB64_MSG_BANK_STR_BSA_1  STRING 16, 0x40, "Act. Bank:      "
MB64_MSG_Bank

	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_MSG_Bank_Ext
MB64_MSG_Bank_Int
	;; no bankstick available
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64_LCD_PrintBankName

	TABLE_ADDR MB64_MSG_BANK_STR_BSNA_1
	goto	MIOS_LCD_PrintString

MB64_MSG_Bank_Ext
	;; external bank selected
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64_LCD_PrintBankName

	TABLE_ADDR MB64_MSG_BANK_STR_BSA_1
	call	MIOS_LCD_PrintString

	;; print bank number
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD3
	;; print '*' if bank not available
	movlw	'*'
	CALL_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MIOS_LCD_PrintChar

	return

