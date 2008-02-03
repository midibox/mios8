; $Id$
;
; MB64E Menu for editing the MIDI settings
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
CS_M_MIDICFG_Exit
	;; Note: the "to-COM" flag will not be saved, since this would
	;; also affect the first-level bootstrap loader. If a user doesn't
	;; take care for a proper configuration, he would propably never
	;; be able to update MIOS or an application if he don't own a
	;; PIC programmer...
	goto	MB64E_DUMP_StoreGlobal; (save CFG0 and CFG1)

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; switch to-COM option
	call	MIOS_MIDI_InterfaceGet
	xorlw	0x01
	goto	MIOS_MIDI_InterfaceSet

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Init
	movlw	3
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	SET_BSR	MB64E_BASE
	movlw	0x00
	btfss	MB64E_CFG0, MB64E_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	MB64E_CFG1, MB64E_CFG1_MBLINK, BANKED
	iorlw 0x02
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	TABLE_ADDR CS_M_MIDICFG_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	return

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MIDICFG_ValueChanged
	;; copy menu value to MB64E_CFG0 and MB64E_CFG1
	SET_BSR	MB64E_BASE
	bcf	MB64E_CFG0, MB64E_CFG0_MERGEROFF, BANKED
	bcf	MB64E_CFG1, MB64E_CFG1_MBLINK, BANKED
	btfss	CS_MENU_PARAMETER_L, 0
	bsf	MB64E_CFG0, MB64E_CFG0_MERGEROFF, BANKED
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	MB64E_CFG1, MB64E_CFG1_MBLINK, BANKED

	;; set merger function and exit
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MIDI_MergerSet

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Tick
	;; print config at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_MIDICFG_NAME_TABLE	; initial pointer to table
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	mullw	CS_M_MIDICFG_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_MIDICFG_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print to-com option at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_MIDICFG_COM_NAME_TABLE	; initial pointer to table
	call	MIOS_MIDI_InterfaceGet
	andlw	0x01
	mullw	CS_M_MIDICFG_COM_NAME_LEN	; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_MIDICFG_COM_NAME_LEN	; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	goto	CS_MENU_PrintArrows


;; ==========================================================================

CS_M_MIDICFG_NAME_LEN		EQU	16	; 16 characters per entry
CS_M_MIDICFG_NAME_ENTRIES	EQU	4	; 4 entries
CS_M_MIDICFG_NAME_TABLE				; all entries are listed here
	db	"MIDI Merger: off"
	db	"MIDI Merger: on "
	db	"MIDIbox Link EP "
	db	"MIDIbox Link FP "

CS_M_MIDICFG_COM_NAME_LEN	EQU	14	; 14 characters per entry
CS_M_MIDICFG_COM_NAME_ENTRIES	EQU	2	; 2 entries
CS_M_MIDICFG_COM_NAME_TABLE			; all entries are listed here
	db	"To-COM: no    "
	db	"To-COM: yes   "
