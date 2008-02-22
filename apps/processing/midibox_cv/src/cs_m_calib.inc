; $Id$
;
; MBCV Menu "Calibration"
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
CS_M_CALIB_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY CV_CALIB_VALUE & 0x7f,			0x06,	0x40, 6,	CS_M_CALIB_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_CALIB_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exit button is pressed
;; --------------------------------------------------------------------------
CS_M_CALIB_Exit
	;; exit calibration mode and request update of all outputs
	bcf	MB_STAT, MB_STAT_CALIBRATION_MODE
	call	CV_MAP_RequestUpdateAll
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_CALIB_Init
	;; enter calibration mode and request update of all outputs
	bsf	MB_STAT, MB_STAT_CALIBRATION_MODE
	call	CV_MAP_RequestUpdateAll

	;; install parameter table
	TABLE_ADDR CS_M_CALIB_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; transfer calibration value or CV channel to menu parameter depending on selected parameter
	movf	CS_MENU_CURSOR_POS, W
	bnz	CS_M_CALIB_Init_Pos1
CS_M_CALIB_Init_Pos0
	movff	CV_CALIB_VALUE, CS_MENU_PARAMETER_L
	rgoto	CS_M_CALIB_Init_Cont
CS_M_CALIB_Init_Pos1
	;; selected out already copied in CS_MENU_ReadCursorParameter
	;; 	rgoto	CS_M_CALIB_Init_Cont
CS_M_CALIB_Init_Cont

	;; print static chars
	TABLE_ADDR CS_M_CALIB_STR0
	goto	MIOS_LCD_PrintString

CS_M_CALIB_STR0	STRING 12, 0x00, "Calibration:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_CALIB_Tick
	;; print selected output
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print selected value
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_CALIB_VALUE_TABLE	; initial pointer to table
	movff	CV_CALIB_VALUE, WREG
	mullw	CS_M_CALIB_VALUE_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_CALIB_VALUE_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print selected scale
	movlw	0x46
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_CALIB_SCALE_TABLE	; initial pointer to table
	call	CS_M_CALIB_GetScale
	mullw	CS_M_CALIB_SCALE_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_CALIB_SCALE_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_CALIB_ValueChanged
	movff	CS_MENU_PARAMETER_L, CV_CALIB_VALUE

	;; request CV update
	goto	CV_MAP_RequestUpdateAll

CS_M_CALIB_OutChanged
	;; request CV update
	call	CV_MAP_RequestUpdateAll
	goto	CS_M_HLP_OutChanged

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns the selected scale
;;    1 if curve1 is selected (Hz/V)
;;    otherwise 0 for any other curve
;; --------------------------------------------------------------------------
CS_M_CALIB_GetScale
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_CURVE
	movf	CV_SELECTED_OUT, W, BANKED
	decf	PLUSW0, W
	movlw	0x00
	skpnz
	movlw	0x01
	return

;; ==========================================================================

CS_M_CALIB_VALUE_LEN		EQU	6	; characters per entry
CS_M_CALIB_VALUE_ENTRIES	EQU	7	; entries
CS_M_CALIB_VALUE_TABLE		; all entries are listed here
	db	"Min.  "
	db	"Middle"
	db	"Max.  "
	db	"1.00V "
	db	"2.00V "
	db	"4.00V "
	db	"8.00V "

CS_M_CALIB_SCALE_LEN		EQU	8	; characters per entry
CS_M_CALIB_SCALE_ENTRIES	EQU	2	; entries
CS_M_CALIB_SCALE_TABLE		; all entries are listed here
	db	" (V/Oct)"
	db	" (Hz/V )"

