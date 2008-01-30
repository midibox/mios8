; $Id$
;
; User vectortable - contains user hooks to the application side
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; never change the origin or the order of entries!
	org	0x3000
USER_Init
	return
	nop
USER_Tick
	return
	nop
USER_Timer
	return
	nop
USER_MPROC_DebugTrigger
	return
	nop
USER_DISPLAY_Init
	goto	_USER_DISPLAY_Init
USER_DISPLAY_Tick
	return
	nop
USER_SR_Service_Prepare
	return
	nop
USER_SR_Service_Finish
	return
	nop
USER_DIN_NotifyToggle
	return
	nop
USER_ENC_NotifyChange
	return
	nop
USER_AIN_NotifyChange
	return
	nop
USER_MPROC_NotifyReceivedByte
	return
	nop
USER_MPROC_NotifyTimeout
	return
	nop
USER_MPROC_NotifyReceivedEvent
	return
	nop
USER_MPROC_NotifyFoundEvent
	return
	nop
USER_LCD_Init
	return
	nop
USER_LCD_Clear
	return
	nop
USER_LCD_CursorSet
	return
	nop
USER_LCD_PrintChar
	return
	nop

USER_MIDI_NotifyRx
	return
	nop
USER_MIDI_NotifyTx
	return
	nop

USER_LCD_Data
	return
	nop
USER_LCD_Cmd
	return
	nop
USER_LCD_SpecialCharInit
	return
	nop

USER_RESERVED_25
	return
	nop
USER_RESERVED_26
	return
	nop
USER_RESERVED_27
	return
	nop
USER_RESERVED_28
	return
	nop
USER_RESERVED_29
	return
	nop
USER_RESERVED_30
	return
	nop
USER_RESERVED_31
	return
	nop
USER_RESERVED_32
	return
	nop
