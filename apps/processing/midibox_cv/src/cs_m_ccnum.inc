; $Id$
;
; MBCV Menu "CC Number"
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
CS_M_CCNUM_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY (CV_C1_CC_NUMBER & 0x7f),		0x7f,	0x43, 3,	CS_M_CCNUM_Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_CCNUM_Init
	;; install parameter table
	TABLE_ADDR CS_M_CCNUM_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_CCNUM_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_CCNUM_STR0  STRING 15, 0x00, "CC/NRPN Number:"
CS_M_CCNUM_STR1  STRING  3, 0x40, "CC#"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_CCNUM_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print CC number
	movlw	0x43
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_CC_NUMBER
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	MIOS_LCD_PrintBCD3

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_CCNUM_Changed
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_CC_NUMBER
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

;; ==========================================================================

