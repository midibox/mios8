; $Id$
;
; MBCV Menu "Transpose"
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
CS_M_TRANSP_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY (CV_C1_TRANSP_OCT & 0x7f) | 0x80,	0x0f,	0x40, 3,	CS_M_TRANSP_OctChanged
 CS_MENU_PARAMETER_ENTRY (CV_C1_TRANSP_SEM & 0x7f) | 0x80,	0x0f,	0x47, 3,	CS_M_TRANSP_NoteChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_TRANSP_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRANSP_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_TRANSP_STR0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRANSP_STR0  STRING 13, 0x00, "Transpose CV:"
CS_M_TRANSP_STR1  STRING  3, 0x43, "Oct"
CS_M_TRANSP_STR2  STRING  3, 0x4a, "Sem."


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRANSP_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print octave transpose value
	movlw	0x40
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_TRANSP_OCT
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	CV_LCD_Prn_PMDec1

	;; print octave transpose value
	movlw	0x47
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_TRANSP_SEM
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	CV_LCD_Prn_PMDec1

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRANSP_OctChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	0x0f
	andwf	CS_MENU_PARAMETER_L, F

	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_TRANSP_OCT
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0

	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	0x0f
	andwf	CS_MENU_PARAMETER_L, F

	lfsr	FSR0, CV_C1_STAT
	movf	CV_SELECTED_OUT, W, BANKED
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ
	return

CS_M_TRANSP_NoteChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	0x0f
	andwf	CS_MENU_PARAMETER_L, F

	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_TRANSP_SEM
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0

	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	0x0f
	andwf	CS_MENU_PARAMETER_L, F

	lfsr	FSR0, CV_C1_STAT
	movf	CV_SELECTED_OUT, W, BANKED
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	return

;; ==========================================================================

