; $Id$
;
; MBCV Menu "Keyboard Zone"
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
CS_M_SPLIT_PARAMETERS
 ;;			 address / signed flag			max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY (CV_C1_SPLIT_LOWER & 0x7f),		0x7f,	0x42, 3,	CS_M_SPLIT_LChanged
 CS_MENU_PARAMETER_ENTRY (CV_C1_SPLIT_UPPER & 0x7f),		0x7f,	0x49, 3,	CS_M_SPLIT_UChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x07,   0x0f, 1,	CS_M_HLP_OutChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_SPLIT_Init
	;; install parameter table
	TABLE_ADDR CS_M_SPLIT_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_SPLIT_STR0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_SPLIT_STR0  STRING 14, 0x00, "Keyboard Zone:"
CS_M_SPLIT_STR1  STRING  2, 0x40, "L:"
CS_M_SPLIT_STR2  STRING  2, 0x47, "U:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SPLIT_Tick
	;; print selected out
	movlw	0x0f
	call	MIOS_LCD_CursorSet
	SET_BSR	CV_BASE
	incf	CV_SELECTED_OUT, W, BANKED
	call	MIOS_LCD_PrintBCD1

	;; print lower split note
	movlw	0x42
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_SPLIT_LOWER
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	MIDI_EVNT_PrintNote

	;; print upper split note
	movlw	0x49
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, CV_C1_SPLIT_UPPER
	movff	CV_SELECTED_OUT, WREG
	movf	PLUSW0, W
	call	MIDI_EVNT_PrintNote

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SPLIT_LChanged
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_SPLIT_LOWER
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

CS_M_SPLIT_UChanged
	SET_BSR	CV_BASE
	lfsr	FSR0, CV_C1_SPLIT_UPPER
	movf	CV_SELECTED_OUT, W, BANKED
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

;; ==========================================================================

