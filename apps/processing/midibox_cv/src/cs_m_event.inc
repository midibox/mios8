; $Id$
;
; MBCV Menu "Events"
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
CS_M_EVENT_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY CV_C1_MIDI_MODE & 0x7f,		0x05,	0x40, 6,	CS_M_EVENT_ModeChanged
 CS_MENU_PARAMETER_ENTRY CV_C1_MIDI_CHANNEL & 0x7f,		0x0f,	0x4b, 2,	CS_M_EVENT_ChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_EVENT_Init
	;; install parameter table
	TABLE_ADDR CS_M_EVENT_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; dirty: since bit [7:4] of MIDI_MODE can be used for other purposes, we need to mask
	;; out them here...
	;; no other value of this page uses these bits, so we do it generally for all parameters
	movlw	0x0f
	andwf	CS_MENU_PARAMETER_L, F

	;; print static chars
	TABLE_ADDR CS_M_EVENT_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_EVENT_STR0	STRING 12, 0x00, "Event of CV:"
CS_M_EVENT_STR1	STRING  3, 0x47, "Chn"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_EVENT_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print event mode depending on selected track and event at second line
	lfsr	FSR0, CV_C1_MIDI_MODE
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	andlw	0x0f
	movwf	TMP1

	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_EVENT_NAME_TABLE	; initial pointer to table
	movf	TMP1, W
	mullw	CS_M_EVENT_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_EVENT_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print channel
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_MIDI_CHANNEL
	movff	CV_SELECTED_OUT, WREG
	incf	PLUSW0, W
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_EVENT_ModeChanged
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_SELECTED_OUT, W, BANKED
	movf	PLUSW0, W
	andlw	0xf0
	movwf	PRODL

	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	iorwf	PRODL, F

	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	CV_SELECTED_OUT, W, BANKED
	movff	PRODL, PLUSW0

	lfsr	FSR0, CV_C1_STAT
	movf	CV_SELECTED_OUT, W, BANKED
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	return

CS_M_EVENT_ChnChanged
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_MIDI_CHANNEL
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0

	lfsr	FSR0, CV_C1_STAT
	movf	CV_SELECTED_OUT, W, BANKED
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	return


;; ==========================================================================

CS_M_EVENT_NAME_LEN		EQU	6	; characters per entry
CS_M_EVENT_NAME_ENTRIES		EQU	6	; entries
CS_M_EVENT_NAME_TABLE				; all entries are listed here
	db	"Note  "
	db	"Vel.  "
	db	"ATch. "
	db	"CC    "
	db	"NRPN  "
	db	"PitchB"
