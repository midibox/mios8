; $Id$
;
; MBCV Menu "Note Handling"
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
CS_M_NOTEH_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY CV_C1_MIDI_MODE & 0x7f,		0x02,	0x40, 6,	CS_M_NOTEH_ModeChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_NOTEH_Init
	;; install parameter table
	TABLE_ADDR CS_M_NOTEH_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; if cursor position == 0: copy bit [7:4] to CS_MENU_PARAMETER_L
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_NOTEH_Init_NotMode
CS_M_NOTEH_Init_Mode
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_SELECTED_OUT, W, BANKED
	swapf	PLUSW0, W
	andlw	0x03
	movwf	CS_MENU_PARAMETER_L
CS_M_NOTEH_Init_NotMode

	;; print static chars
	TABLE_ADDR CS_M_NOTEH_STR0
	goto	MIOS_LCD_PrintString

CS_M_NOTEH_STR0	STRING 14, 0x00, "Note Handling:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_NOTEH_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print note mode
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_NOTEH_NAME_TABLE	; initial pointer to table
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_SELECTED_OUT, W, BANKED
	swapf	PLUSW0, W
	andlw	0x03
	mullw	CS_M_NOTEH_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_NOTEH_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_NOTEH_ModeChanged
	;; copy parameter bit [1:0] to mode bit [5:4]
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_SELECTED_OUT, W, BANKED
	bcf	PLUSW0, 4
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	PLUSW0, 4

	bcf	PLUSW0, 5
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	PLUSW0, 5
	return

;; ==========================================================================

CS_M_NOTEH_NAME_LEN		EQU	6	; characters per entry
CS_M_NOTEH_NAME_ENTRIES		EQU	3	; entries
CS_M_NOTEH_NAME_TABLE				; all entries are listed here
	db	"Mono  "
	db	"Legato"
	db	"Poly  "
