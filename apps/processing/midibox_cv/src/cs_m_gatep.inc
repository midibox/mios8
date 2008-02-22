; $Id$
;
; MBCV Menu "Gate Polarity"
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
CS_M_GATEP_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY CV_GATE_POLARITY & 0x7f,		0x01,	0x40, 8,	CS_M_GATEP_ModeChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_GATEP_Init
	;; install parameter table
	TABLE_ADDR CS_M_GATEP_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_GATEP_STR0
	goto	MIOS_LCD_PrintString

CS_M_GATEP_STR0	STRING 14, 0x00, "Gate Polarity:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_GATEP_Tick
	;; print selected gate
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print polarity of selected gate
	SET_BSR	CV_BASE
	movf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	CV_GATE_POLARITY, W
	movlw	0x00
	skpz
	movlw	0x01
	movwf	TMP1

	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_GATEP_NAME_TABLE	; initial pointer to table
	movf	TMP1, W
	mullw	CS_M_GATEP_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_GATEP_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_GATEP_ModeChanged
	;; ensure that gates will be updated
	call	CV_MAP_RequestUpdateAll

	;; set/clear polarity bit
	SET_BSR	CV_BASE
	BRA_IFCLR CS_MENU_PARAMETER_L, 0, ACCESS, CS_M_GATEP_ModeChanged_Clr
CS_M_GATEP_ModeChanged_Set
	movf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	CV_GATE_POLARITY, F
	return

CS_M_GATEP_ModeChanged_Clr
	movf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	CV_GATE_POLARITY, F
	return

;; ==========================================================================

CS_M_GATEP_NAME_LEN		EQU	8	; characters per entry
CS_M_GATEP_NAME_ENTRIES		EQU	2	; entries
CS_M_GATEP_NAME_TABLE				; all entries are listed here
	db	"Positive"
	db	"Negative"
