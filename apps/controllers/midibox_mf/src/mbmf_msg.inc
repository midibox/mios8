; $Id$
;
; MBMF Message Handler
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
MBMF_MSG_Start
	SET_BSR	MBMF_BASE
	movwf	MBMF_MSG_ID, BANKED
	goto	CS_MENU_StartMessage

;; --------------------------------------------------------------------------
;;  Use this function to hold a message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
MBMF_MSG_Hold
	SET_BSR	MBMF_BASE
	movwf	MBMF_MSG_ID, BANKED
	goto	CS_MENU_HoldMessage

;; --------------------------------------------------------------------------
;;  Use this function to stop a message
;;  IN: -
;; --------------------------------------------------------------------------
MBMF_MSG_Stop
	goto	CS_MENU_StopMessage

;; --------------------------------------------------------------------------
;;  This function is called by the CS when a message has been requested
;;  (see cs_menu_tables.inc)
;; --------------------------------------------------------------------------
MBMF_MSG_Handler
	;; branch depending on message ID
	SET_BSR	MBMF_BASE
	movf	MBMF_MSG_ID, W, BANKED
	JUMPTABLE_2BYTES 1	; entry
	rgoto	MBMF_MSG_Bank	; MBMF_MSG_ID_BANK

;; --------------------------------------------------------------------------
;;  Group Message
;; --------------------------------------------------------------------------
MBMF_MSG_Bank
#if DEFAULT_PATCHMODE == 0
	;; save current bank
	movff	MB_BANK, MBMF_SELECTED_BP_TMP
	;; switch to selected bank
	movff	MBMF_SELECTED_BANK, MB_BANK
	;; update the bankname only
	call	MBMF_DUMP_RestoreBankName
#else
	;; save current patch
	movff	MB_PATCH, MBMF_SELECTED_BP_TMP
	;; switch to selected bank
	movff	MBMF_SELECTED_PATCH, MB_PATCH
	;; update the patchname only
	call	MBMF_DUMP_RestoreBankName
#endif

	;; upper line: print bank and group number
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MBMF_LCD_PrintBank

	;; print first - last fader
	SET_BSR	MBMF_BASE
	incf	MBMF_GROUP_OFFSET, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	SET_BSR	MBMF_BASE
	movf	MBMF_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_NUMBER_FADERS
	call	MIOS_LCD_PrintBCD3

	;; lower line: print bank name and exit
	movlw	0x40
	call	MIOS_LCD_CursorSet
#if DEFAULT_PATCHMODE == 0
	call	MBMF_LCD_PrintBankName
#else
	movf	MB_PATCH, W
	call	MBMF_LCD_PrnPatch
	movlw	':'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	call	MBMF_LCD_PrintPatchName
#endif

#if DEFAULT_PATCHMODE == 0
	;; restore bank
	movff	MBMF_SELECTED_BP_TMP, MB_BANK
	;; update the bankname only
	call	MBMF_DUMP_RestoreBankName
#else
	;; restore patch
	movff	MBMF_SELECTED_BP_TMP, MB_PATCH
	;; update the bankname only
	call	MBMF_DUMP_RestoreBankName
#endif

	;; and exit
	return

