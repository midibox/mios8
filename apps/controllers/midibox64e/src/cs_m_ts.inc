; $Id$
;
; MB64E Menu Touch Sensor Sensitivity Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_TS_Exit
	;; save touch sensor sensitivity and exit
	goto	MB64E_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_TS_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TS_Init
	;; set max value
	movlw	0x1f
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current setting into parameter
	call	MIOS_SRIO_TS_SensitivityGet
	andlw	0x1f
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install "changed" hook
	TABLE_ADDR CS_M_TS_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print static string
	TABLE_ADDR CS_M_TS_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TS_STR_0	STRING  7, 0x00, "Status:"
CS_M_TS_STR_1	STRING 10, 0x40, "Sensitiv.:"

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TS_ValueChanged
	;; save TS value
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_SRIO_TS_SensitivitySet


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TS_Tick
	;; print SR status
	movlw	0x08
	call	MIOS_LCD_CursorSet
	movff	MB64E_TS_STATUS, TMP1
	clrf	TMP2
CS_M_TS_TickStatusLoop
	movlw	'o'
	btfsc	TMP1, 0
	movlw '*'
	call	MIOS_LCD_PrintChar
	rrf	TMP1, F
	incf	TMP2, F
	BRA_IFCLR TMP2, 3, ACCESS, CS_M_TS_TickStatusLoop

	;; print touch sensor sensitivity value
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	call	MIOS_SRIO_TS_SensitivityGet
	call	MIOS_LCD_PrintBCD2

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x4b
	movwf	TMP1
	movlw	0x02
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================
