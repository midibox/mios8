; $Id$
;
; MB64E Menu EncMode Handler
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
CS_M_ENCSPEED_Exit
	;; exit "show selected" and "mute encoders" mode
	bcf	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	bcf	MB_STAT2, MB_STAT2_SHOW_SELECTED

	;; store ENC speed modes
	call	MB64E_DUMP_StoreEncSpeed

	;; restore it in order to update the speed settings on-the-fly
	movlw	0xff
	call	MB64E_ENC_UpdateSpeed

	;; restore ledpattern assignments and exit
	goto	MB64E_DUMP_RestoreLEDRP


;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_ENCSPEED_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_ENCSPEED_Init
	;; enter "show selected" and "mute encoders" mode
	bsf	MB_STAT3, MB_STAT3_MUTE_ENCODERS
	bsf	MB_STAT2, MB_STAT2_SHOW_SELECTED

	;; retrieve encoder speed modes and store it in MB64E_ENC_MODE_BUFFER (in packed format)
	call	MB64E_DUMP_RestoreEncSpeed

	;; init menu parameter
	movlw	CS_M_ENCSPEED_NAME_ENTRIES-1
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; update the menu value
	rcall	CS_M_ENCSPEED_UpdateValue

	;; and install the hook
	TABLE_ADDR CS_M_ENCSPEED_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print "Enc #xx Speed:  " at first line
	TABLE_ADDR CS_M_ENCSPEED_STR_0
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_ENCSPEED_ValueChanged
	;; note: mode values stored in packed format:
	;; every mode entry allocates only 4 bit
	lfsr	FSR0, MB64E_ENC_MODE_BUFFER
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
CS_M_ENCSPEED_UpdateValue
	;; store bank and encoder entry number
	movff	MB64E_LAST_ENTRY_MAPPED, CS_MENU_LAST_ENC_ENTRY
	movff	MB_BANK, CS_MENU_LAST_BANK

	;; note: mode values stored in packed format:
	;; every mode entry allocates only 4 bit
	lfsr	FSR0, MB64E_ENC_MODE_BUFFER
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
CS_M_ENCSPEED_Tick
	;; check if encoder entry number has been changed
	;; in this case update new mode setting
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	cpfseq	CS_MENU_LAST_ENC_ENTRY, ACCESS
	rcall CS_M_ENCSPEED_UpdateValue

	;; check if bank has been changed
	;; in this case store current encoder settings in old bank and switch to new bank
	movf	MB_BANK, W
	xorwf	CS_MENU_LAST_BANK, W
	bz	CS_M_ENCSPEED_Tick_NoBankChange
CS_M_ENCSPEED_Tick_BankChange
	;; store ENC speed modes on bank changes
	call	MB64E_DUMP_StoreEncSpeed
CS_M_ENCSPEED_Tick_NoBankChange

	;; print encoder number at first line
	movlw	0x07
	call	MIOS_LCD_CursorSet
	SET_BSR	MB64E_BASE
	movf	MB64E_LAST_ENTRY_MAPPED, W, BANKED
	addlw	1
	call	MIOS_LCD_PrintBCD3

	;; print enc speed mode at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_ENCSPEED_NAME_TABLE	; initial pointer to table
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	mullw	CS_M_ENCSPEED_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_ENCSPEED_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	goto	CS_MENU_PrintArrows


;; ==========================================================================

CS_M_ENCSPEED_STR_0 STRING 16, 0x00, "Entry #xxx Speed"

CS_M_ENCSPEED_NAME_LEN		EQU	14	; 14 characters per entry
CS_M_ENCSPEED_NAME_ENTRIES	EQU	3	; 3 entries
CS_M_ENCSPEED_NAME_TABLE			; all entries are listed here
	db	"Normal        "
	db	"Slow          "
	db	"Fast          "
