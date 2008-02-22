; $Id$
;
; MBCV Menu "Pitchrange"
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
CS_M_PITCHR_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY (CV_C1_PITCHRANGE & 0x7f),		0x18,	0x40, 2,	CS_M_PITCHR_Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_PITCHR_Init
	;; install parameter table
	TABLE_ADDR CS_M_PITCHR_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_PITCHR_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_PITCHR_STR0  STRING 11, 0x00, "Pitchrange:"
CS_M_PITCHR_STR1  STRING  9, 0x43, "Semitones"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PITCHR_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print pitchrange
	movlw	0x40
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_PITCHRANGE
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_PITCHR_Changed
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_PITCHRANGE
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

;; ==========================================================================

