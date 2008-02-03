; $Id$
;
; MBSEQ Menu "Metronome"
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
CS_M_METRON_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,	0x49, 2,	CS_M_METRON_ValueChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_METRON_Init
	;; install parameter table
	TABLE_ADDR CS_M_METRON_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy channel number to menu parameter
	movff	SEQ_METRONOME_CHANNEL, CS_MENU_PARAMETER_L

	;; print static strings
	TABLE_ADDR CS_M_METRON_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_METRON_STR0 STRING  9, 0x00, "Metronome"
CS_M_METRON_STR1 STRING  8, 0x40, "Channel:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_METRON_Tick
	;; print channel number at second line
	movlw	0x49
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_METRONOME_CHANNEL, W, BANKED
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_METRON_ValueChanged
	movff	CS_MENU_PARAMETER_L, SEQ_METRONOME_CHANNEL
	return

;; ==========================================================================
