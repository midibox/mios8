; $Id$
;
; MB64E Menu Global MIDI Channel Handler
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
CS_M_CHANNEL_Exit
	;; save global channel and exit
	goto	MB64E_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_CHANNEL_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_CHANNEL_Init
	;; set max value
	movlw	16
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy current global channel setting into parameter
	movff	MB64E_GLOBAL_CHANNEL, CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install "changed" hook
	TABLE_ADDR CS_M_CHANNEL_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print static string
	TABLE_ADDR CS_M_CHANNEL_STR_0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_CHANNEL_STR_0	STRING 11, 0x00, "Global MIDI"
CS_M_CHANNEL_STR_1	STRING  8, 0x40, "Channel:"


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_CHANNEL_ValueChanged
	;; change to a new channel
	movff	CS_MENU_PARAMETER_L, MB64E_GLOBAL_CHANNEL
	goto	MB64E_DUMP_RestoreEncButtonEvnts


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_CHANNEL_Tick
	;; set cursor
	movlw	0x49
	call	MIOS_LCD_CursorSet

	;; when global midi channel number is 0x00, print "--"
	SET_BSR	MB64E_GLOBAL_CHANNEL
	movf	MB64E_GLOBAL_CHANNEL, W, BANKED
	bz	CS_M_CHANNEL_Tick_NoGlobal
	;; else print channel number
CS_M_CHANNEL_Tick_Global
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_CHANNEL_Tick_Cont
CS_M_CHANNEL_Tick_NoGlobal
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
CS_M_CHANNEL_Tick_Cont

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movlw	0x49
	movwf	TMP1
	movlw	0x02
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================
