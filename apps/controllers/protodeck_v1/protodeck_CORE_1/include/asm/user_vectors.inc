; $Id: user_vectors.inc 49 2008-01-30 21:47:31Z tk $
;
; User vectortable - contains user hooks to MIOS
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
_User_Init
	goto	USER_Init
_USER_Tick
	goto	USER_Tick
_USER_Timer
	goto	USER_Timer
_USER_MPROC_DebugTrigger
	goto	USER_MPROC_DebugTrigger
_USER_DISPLAY_Init
	goto	USER_DISPLAY_Init
_USER_DISPLAY_Tick
	goto	USER_DISPLAY_Tick
_USER_SR_Service_Prepare
	goto	USER_SR_Service_Prepare
_USER_SR_Service_Finish
	goto	USER_SR_Service_Finish
_USER_DIN_NotifyToggle
	goto	USER_DIN_NotifyToggle
_USER_ENC_NotifyChange
	goto	USER_ENC_NotifyChange
_USER_AIN_NotifyChange
	goto	USER_AIN_NotifyChange
_USER_MPROC_NotifyReceivedByte
	goto	USER_MPROC_NotifyReceivedByte
_USER_MPROC_NotifyTimeout
	goto	USER_MPROC_NotifyTimeout
_USER_MPROC_NotifyReceivedEvent
	goto	USER_MPROC_NotifyReceivedEvent
_USER_MPROC_NotifyFoundEvent
	goto	USER_MPROC_NotifyFoundEvent
_USER_LCD_Init
	goto	USER_LCD_Init
_USER_LCD_Clear
	goto	USER_LCD_Clear
_USER_LCD_CursorSet
	goto	USER_LCD_CursorSet
_USER_LCD_PrintChar
	goto	USER_LCD_PrintChar
_USER_MIDI_NotifyRx
	goto	USER_MIDI_NotifyRx
_USER_MIDI_NotifyTx
	goto	USER_MIDI_NotifyTx
_USER_LCD_Data
	goto	USER_LCD_Data
_USER_LCD_Cmd
	goto	USER_LCD_Cmd
_USER_LCD_SpecialCharInit
	goto	USER_LCD_SpecialCharInit
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
