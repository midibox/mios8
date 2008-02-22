; $Id$
;
; MBCV Menu "SysEx"
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
CS_M_SYSEX_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x4b, 2,	CS_M_SYSEX_ValueChanged

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SYSEX_Select
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; send SysEx Dump
	goto	CV_SYSEX_SendDump_RAM

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SYSEX_Init
	;; install parameter table
	TABLE_ADDR CS_M_SYSEX_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; transfer device ID to menu parameter
	movff	MIDI_DEVICE_ID, CS_MENU_PARAMETER_L

	;; print static chars
	TABLE_ADDR CS_M_SYSEX_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_SYSEX_STR0 STRING 12, 0x00, "Press Select"
CS_M_SYSEX_STR1 STRING 10, 0x40, "Device ID:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SYSEX_Tick
	;; print device ID
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	movff	MIDI_DEVICE_ID, WREG
	call	MIOS_LCD_PrintHex2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SYSEX_ValueChanged
	movff	CS_MENU_PARAMETER_L, MIDI_DEVICE_ID
	return

;; ==========================================================================
