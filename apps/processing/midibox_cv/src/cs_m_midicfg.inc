; $Id$
;
; MBCV Menu for editing the MIDI settings
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_MIDICFG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x03,	0x00, 16,	CS_M_MIDICFG_MergerChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x48, 3,	CS_M_MIDICFG_COMChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Init
	;; install parameter table
	TABLE_ADDR CS_M_MIDICFG_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; set menu parameter depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_MIDICFG_Init_COM
CS_M_MIDICFG_Init_Merger
	SET_BSR	CV_BASE
	movlw	0x00
	btfss	CV_CFG0, CV_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	CV_CFG0, CV_CFG0_MBLINK, BANKED
	iorlw 0x02
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_MIDICFG_Init_Cont
CS_M_MIDICFG_Init_COM
	call	MIOS_MIDI_InterfaceGet
	movwf	CS_MENU_PARAMETER_L
CS_M_MIDICFG_Init_Cont

	;; print static chars
	TABLE_ADDR CS_M_MIDICFG_STR0
	goto	MIOS_LCD_PrintString

CS_M_MIDICFG_STR0 STRING  7, 0x40, "To-COM:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Tick
	;; print config at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_MIDICFG_NAME_TABLE	; initial pointer to table
	SET_BSR	CV_BASE
	movlw	0x00
	btfss	CV_CFG0, CV_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	CV_CFG0, CV_CFG0_MBLINK, BANKED
	iorlw 0x02
	andlw	0x03
	mullw	CS_M_MIDICFG_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_MIDICFG_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print to-com option at second line
	movlw	0x48
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

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MIDICFG_MergerChanged
	;; copy menu value to CV_CFG0[1..0]
	SET_BSR	CV_BASE
	bcf	CV_CFG0, CV_CFG0_MERGEROFF, BANKED
	bcf	CV_CFG0, CV_CFG0_MBLINK, BANKED
	btfss	CS_MENU_PARAMETER_L, 0
	bsf	CV_CFG0, CV_CFG0_MERGEROFF, BANKED
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	CV_CFG0, CV_CFG0_MBLINK, BANKED

	;; set merger function and exit
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MIDI_MergerSet

CS_M_MIDICFG_COMChanged
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MIDI_InterfaceSet

;; ==========================================================================

CS_M_MIDICFG_NAME_LEN		EQU	16	; 16 characters per entry
CS_M_MIDICFG_NAME_ENTRIES	EQU	4	; 4 entries
CS_M_MIDICFG_NAME_TABLE				; all entries are listed here
	db	"MIDI Merger: off"
	db	"MIDI Merger: on "
	db	"MIDIbox Link EP "
	db	"MIDIbox Link FP "

CS_M_MIDICFG_COM_NAME_LEN	EQU	4	; 4 characters per entry
CS_M_MIDICFG_COM_NAME_ENTRIES	EQU	2	; 2 entries
CS_M_MIDICFG_COM_NAME_TABLE			; all entries are listed here
	db	"no  "
	db	"yes "
