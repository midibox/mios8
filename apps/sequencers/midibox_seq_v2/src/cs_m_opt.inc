; $Id$
;
; MBSEQ Menu "Options"
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
CS_M_OPT_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x4b, 3,	CS_M_OPT_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x4b, 3,	CS_M_OPT_ValueChanged

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_OPT_Select
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor
	call	CS_MENU_IncCursorPos

	;; copy new option
	rgoto	CS_M_OPT_CopyOpt

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_OPT_Init
	;; install parameter table
	TABLE_ADDR CS_M_OPT_PARAMETERS	; pointer to table
	movlw	2			; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy selected option to menu parameter depending on "cursor pos"
	rcall	CS_M_OPT_CopyOpt

	;; print static strings
	TABLE_ADDR CS_M_OPT_STR0
	goto	MIOS_LCD_PrintString

CS_M_OPT_STR0 STRING  13, 0x00, "MBSEQ Options"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_OPT_Tick
	;; print option at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_OPT_NAME_TABLE		; initial pointer to table
	movf	CS_MENU_CURSOR_POS, W
	mullw	CS_M_OPT_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_OPT_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	movlw	':'
	call	MIOS_LCD_PrintChar
	movlw	'o'
	call	MIOS_LCD_PrintChar
	movlw	'f'
	btfsc	CS_MENU_PARAMETER_L, 0
	movlw 'n'
	call	MIOS_LCD_PrintChar
	movlw	'f'
	btfsc	CS_MENU_PARAMETER_L, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_OPT_ValueChanged
	rcall	CS_M_OPT_Hlp_Get_CFG_H
	rgoto	CS_M_OPT_Hlp_Set_CFG_H

;; --------------------------------------------------------------------------
;;  This function copies the selected option into CS_MENU_PARAMETER_L
;; --------------------------------------------------------------------------
CS_M_OPT_CopyOpt
	rcall	CS_M_OPT_Hlp_Get_CFG_H
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	andwf	MIOS_PARAMETER1, W
	skpz
	movlw	0x01
	movwf	CS_MENU_PARAMETER_L
	return

;; --------------------------------------------------------------------------
;;  two help routines to get/set the CFG_H parameter
;; --------------------------------------------------------------------------
CS_M_OPT_Hlp_Get_CFG_H
	movlw	SEQ_CC_OFFSET_CFG_H
	call	SEQ_CCOUT_Get
	movwf	MIOS_PARAMETER1
	return

CS_M_OPT_Hlp_Set_CFG_H
	movf	CS_MENU_PARAMETER_L, W
	bz	CS_M_OPT_Hlp_Set_CFG_H_0
CS_M_OPT_Hlp_Set_CFG_H_1
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	iorwf	MIOS_PARAMETER1, F
	rgoto	CS_M_OPT_Hlp_Set_CFG_H_C

CS_M_OPT_Hlp_Set_CFG_H_0
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitANDMask
	andwf	MIOS_PARAMETER1, F
	;; 	rgoto	CS_M_OPT_Hlp_Set_CFG_H_C
	
CS_M_OPT_Hlp_Set_CFG_H_C
	movlw	SEQ_CC_OFFSET_CFG_H
	goto	SEQ_CCIN_Set

;; ==========================================================================

CS_M_OPT_NAME_LEN		EQU	10	; 10 characters per entry
CS_M_OPT_NAME_TABLE				; all entries are listed here
	db	"ArpReset  "
	db	"SyncPatChg"
