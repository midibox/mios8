; $Id$
;
; MBSEQ Menu "Track Events"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_EVNT0,		0x06,	0x40, 6,	CS_M_TRKEVNT_TypeChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_CHANNEL,		0x0f,	0x4b, 2,	CS_M_TRKEVNT_ChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x0f, 1,	CS_M_HLP_TrkChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKEVNT_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; if cursor pos == 0, modify the menu parameter
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_TRKEVNT_Init_NotEvnt0
CS_M_TRKEVNT_Init_Evnt0
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	skpnz
	bsf	WREG, 0		; Note Off -> Note On
	addlw	-1
	movwf	CS_MENU_PARAMETER_L
CS_M_TRKEVNT_Init_NotEvnt0

	;; print static chars
	TABLE_ADDR CS_M_TRKEVNT_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRKEVNT_STR0	STRING 15, 0x00, "Event of Track:"
CS_M_TRKEVNT_STR1	STRING  3, 0x47, "Chn"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print event type depending on selected track and event at second line
	movlw	SEQ_CC_OFFSET_MIDI_EVNT0
	call	CS_M_HLP_TrkCC_Get
	addlw	-1
	andlw	0x07
	movwf	TMP1

	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_TRKEVNT_NAME_TABLE	; initial pointer to table
	movf	TMP1, W
	mullw	CS_M_TRKEVNT_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_TRKEVNT_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print channel
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_TypeChanged
	incf	CS_MENU_PARAMETER_L, W
	iorlw	0x08
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_MIDI_EVNT0
	call	CS_M_HLP_TrkCC_Set
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L
	return

CS_M_TRKEVNT_ChnChanged
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================

CS_M_TRKEVNT_NAME_LEN		EQU	6	; 6 characters per entry
CS_M_TRKEVNT_NAME_ENTRIES	EQU	7	; 7 entries
CS_M_TRKEVNT_NAME_TABLE				; all entries are listed here
	db	"Note  "
	db	"ATch  "
	db	"CC    "
	db	"PChng "
	db	"ChnPrs"
	db	"PitchB"
	db	"AOUT  "
