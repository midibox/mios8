; $Id$
;
; MB64E Menu Touch Sensor Mode Handler
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
CS_M_TSMODE_Exit
	;; save touch sensor mode and exit
	goto	MB64E_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_TSMODE_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TSMODE_Init
	;; set max value
	movlw	0x02
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current setting into parameter
	SET_BSR	MB64E_BASE
	movlw	0x00
	btfsc	MB64E_CFG0, MB64E_CFG0_TSMODE0, BANKED
	iorlw 0x01
	btfsc	MB64E_CFG0, MB64E_CFG0_TSMODE1, BANKED
	iorlw 0x02
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install "changed" hook
	TABLE_ADDR CS_M_TSMODE_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print static string
	TABLE_ADDR CS_M_TSMODE_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TSMODE_STR_0 STRING 12, 0x00, "Touch Sensor"
CS_M_TSMODE_STR_1 STRING  5, 0x40, "Mode:"


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TSMODE_ValueChanged
	;; save TS mode
	SET_BSR	MB64E_BASE
	bcf	MB64E_CFG0, MB64E_CFG0_TSMODE0, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	MB64E_CFG0, MB64E_CFG0_TSMODE0, BANKED

	bcf	MB64E_CFG0, MB64E_CFG0_TSMODE1, BANKED
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	MB64E_CFG0, MB64E_CFG0_TSMODE1, BANKED

	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TSMODE_Tick
	;; print touch sensor mode
	movlw	0x46
	call	MIOS_LCD_CursorSet
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_LCD_PrintBCD1

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x46
	movwf	TMP1
	movlw	0x01
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================
