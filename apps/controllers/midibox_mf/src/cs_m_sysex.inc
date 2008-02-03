; $Id$
;
; MBMF Menu SysEx Handler
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
CS_M_SYSEX_Exit
	;; save device ID in EEPROM
	movlw	(EEPROM_READONLY & 0xff) + 1
	movwf	EEADR
	movff	MBMF_DEVICE_ID, WREG
	goto	MIOS_EEPROM_Write

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_SYSEX_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; send all blocks
	goto	MBMF_SYSEX_SendAll

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SYSEX_Init
	movlw	7
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	rcall	CS_M_SYSEX_Hlp_GetFSR
	movf	INDF0, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	TABLE_ADDR CS_M_SYSEX_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	TABLE_ADDR CS_M_SYSEX_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_SYSEX_STR_0 STRING 16, 0x00, "Press Snapshot  "
CS_M_SYSEX_STR_1 STRING 14, 0x40, "Device ID:    "

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SYSEX_ValueChanged
	;; copy menu value to MBMF_DEVICE_ID
	rcall	CS_M_SYSEX_Hlp_GetFSR
	movff	CS_MENU_PARAMETER_L, INDF0
	return

;; ==========================================================================
	;; returns pointer to MBMF_DEVICE_ID in FSR0
CS_M_SYSEX_Hlp_GetFSR
	lfsr	FSR0, MBMF_DEVICE_ID
	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SYSEX_Tick
	;; print device ID
	movlw	0x4b
	call	MIOS_LCD_CursorSet

	rcall	CS_M_SYSEX_Hlp_GetFSR
	movf	INDF0, W
	call	MIOS_LCD_PrintHex2

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
