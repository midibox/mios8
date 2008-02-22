; $Id$
;
; MBCV Menu "Interpolation"
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
CS_M_IP_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY (CV_C1_SLEW_RATE & 0x7f),		0x07,	0x40, 12,	CS_M_IP_Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f,  1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_IP_Init
	;; install parameter table
	TABLE_ADDR CS_M_IP_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_IP_STR0
	goto	MIOS_LCD_PrintString

CS_M_IP_STR0  STRING 10, 0x00, "Slew Rate:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_IP_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print slew rate
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_IP_NAME_TABLE	; initial pointer to table
	lfsr	FSR0, CV_C1_SLEW_RATE
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	mullw	CS_M_IP_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_IP_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_IP_Changed
	SET_BSR	CV_BASE

	;; save slew rate
	lfsr	FSR0, CV_C1_SLEW_RATE
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0

	;; request CV update
	goto	CV_MAP_RequestUpdateAll

;; ==========================================================================

CS_M_IP_NAME_LEN		EQU	6	; characters per entry
CS_M_IP_NAME_ENTRIES		EQU	8	; entries
CS_M_IP_NAME_TABLE				; all entries are listed here
	db	"off   "
	db	"  1 mS"
	db	"  2 mS"
	db	"  4 mS"
	db	"  8 mS"
	db	" 16 mS"
	db	" 32 mS"
	db	" 64 mS"
