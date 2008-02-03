; $Id$
;
; MB64E Message Handler
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
MB64E_MSG_Start
	SET_BSR	MB64E_BASE
	movwf	MB64E_MSG_ID, BANKED
	goto	CS_MENU_StartMessage

;; --------------------------------------------------------------------------
;;  Use this function to hold a message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
MB64E_MSG_Hold
	SET_BSR	MB64E_BASE
	movwf	MB64E_MSG_ID, BANKED
	goto	CS_MENU_HoldMessage

;; --------------------------------------------------------------------------
;;  Use this function to stop a message
;;  IN: -
;; --------------------------------------------------------------------------
MB64E_MSG_Stop
	goto	CS_MENU_StopMessage

;; --------------------------------------------------------------------------
;;  This function is called by the CS when a message has been requested
;;  (see cs_menu_tables.inc)
;; --------------------------------------------------------------------------
MB64E_MSG_Handler
	;; branch depending on message ID
	SET_BSR	MB64E_BASE
	movf	MB64E_MSG_ID, W, BANKED
	JUMPTABLE_2BYTES 4	; entries
	rgoto	MB64E_MSG_Bank	; MB64E_MSG_ID_BANK
	rgoto	MB64E_MSG_Map	; MB64E_MSG_ID_MAP

;; --------------------------------------------------------------------------
;;  Group Message
;; --------------------------------------------------------------------------
MB64E_MSG_Bank
#if DEFAULT_PATCHMODE == 0
	;; save current bank
	movff	MB_BANK, MB64E_SELECTED_BP_TMP
	;; switch to selected bank
	movff	MB64E_SELECTED_BANK, MB_BANK
	;; update the bankname only
	call	MB64E_DUMP_RestoreBankName
#else
	;; save current patch
	movff	MB_PATCH, MB64E_SELECTED_BP_TMP
	;; switch to selected patch
	movff	MB64E_SELECTED_PATCH, MB_PATCH
	;; update the patchname only
	call	MB64E_DUMP_RestoreBankName
#endif


	;; upper line: print bank and group number
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintBank

	;; print first - last encoder
	SET_BSR	MB64E_BASE
	incf	MB64E_GROUP_OFFSET, W, BANKED
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	SET_BSR	MB64E_BASE
	movf	MB64E_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_NUMBER_ENCS
	call	MIOS_LCD_PrintBCD3

	;; lower line: print bank name and exit
	movlw	0x40
	call	MIOS_LCD_CursorSet
#if DEFAULT_PATCHMODE == 0
	call	MB64E_LCD_PrintBankName
#else
	movf	MB_PATCH, W
	call	MB64E_LCD_PrnPatch
	movlw	':'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	call	MB64E_LCD_PrintPatchName
#endif

#if DEFAULT_PATCHMODE == 0
	;; restore bank
	movff	MB64E_SELECTED_BP_TMP, MB_BANK
	;; update the bankname only
	call	MB64E_DUMP_RestoreBankName
#else
	;; restore patch
	movff	MB64E_SELECTED_BP_TMP, MB_PATCH
	;; update the bankname only
	call	MB64E_DUMP_RestoreBankName
#endif

	;; and exit
	return


;; --------------------------------------------------------------------------
;;  Map Message
;; --------------------------------------------------------------------------
MB64E_MSG_MAP_STR_0 STRING 16, 0x00, "Map Exx -> Pxxx "
MB64E_MSG_Map
	;; print upper line
	TABLE_ADDR MB64E_MSG_MAP_STR_0
	call	MIOS_LCD_PrintString

	;; print origin encoder
	movlw	0x05
	call	MIOS_LCD_CursorSet
	SET_BSR	MB64E_BASE
	movf	MB64E_LAST_ENTRY, W, BANKED
	andlw	0x7f
	addlw	1
	call	MIOS_LCD_PrintBCD2

	;; print target entry
	movlw	0x0c
	call	MIOS_LCD_CursorSet
	SET_BSR	MB64E_BASE
	movf	MB64E_LAST_ENTRY_MAPPED, W, BANKED
	addlw	1
	call	MIOS_LCD_PrintBCD3

	;; print encoder label at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	goto	MB64E_LCD_PrintLabel
