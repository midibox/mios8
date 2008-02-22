; $Id$
;
; MBCV Menu "Clock Output"
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
CS_M_CLK_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY CV_CLK_DIVIDER_PRELOAD & 0x7f,		0x12,	0x40, 12,	CS_M_CLK_ModeChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_CLK_Init
	;; install parameter table
	TABLE_ADDR CS_M_CLK_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_CLK_STR0
	goto	MIOS_LCD_PrintString

CS_M_CLK_STR0	STRING 14, 0x00, "Clock Divider:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_CLK_Tick
	;; print clock divider
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_CLK_NAME_TABLE	; initial pointer to table
	movff	CV_CLK_DIVIDER_PRELOAD, WREG
	mullw	CS_M_CLK_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_CLK_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_CLK_ModeChanged
	movff	CS_MENU_PARAMETER_L, CV_CLK_DIVIDER_PRELOAD
	return

;; ==========================================================================

CS_M_CLK_NAME_LEN		EQU	12	; characters per entry
CS_M_CLK_NAME_ENTRIES		EQU	19	; entries
CS_M_CLK_NAME_TABLE				; all entries are listed here
	db	"96 ppqn     "
	db	"48 ppqn     "
	db	"32 ppqn     "
	db	"24 ppqn     "
	db	"24 ppqn /  2"
	db	"24 ppqn /  3"
	db	"24 ppqn /  4"
	db	"24 ppqn /  5"
	db	"24 ppqn /  6"
	db	"24 ppqn /  7"
	db	"24 ppqn /  8"
	db	"24 ppqn /  9"
	db	"24 ppqn / 10"
	db	"24 ppqn / 11"
	db	"24 ppqn / 12"
	db	"24 ppqn / 13"
	db	"24 ppqn / 14"
	db	"24 ppqn / 15"
	db	"24 ppqn / 16"
