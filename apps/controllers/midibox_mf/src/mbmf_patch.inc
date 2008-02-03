; $Id$
;
; MIDIbox MF
; Patch functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  sends a fader/button snapshot
;; --------------------------------------------------------------------------
MBMF_PATCH_SENDING_STR_0	STRING 16, 0x00, "Sending Snapshot"
MBMF_PATCH_Send

	;; clear screen
	call	MIOS_LCD_Clear

	;; display message
	TABLE_ADDR MBMF_PATCH_SENDING_STR_0
	call	MIOS_LCD_PrintMessage

	;; for the progress bar: begin at second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; restore settings
	call	MBMF_DUMP_RestorePatch

	;; update fader positions
	movlw	0xff
	call	MBMF_FADER_UpdatePos

	;; send fader events
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_FADER, BANKED
MBMF_PATCH_Send_Faders_Loop
	;; print a "#" character on every 16th event
	movf	MBMF_CURRENT_FADER, W, BANKED
	andlw	0x0f
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	;; send fader event
	call	MBMF_MIDI_SendFaderEvent

	;; add short delay for relaxed timings (required for some sequfaderers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	movlw	128-1
	cpfsgt	MBMF_CURRENT_FADER, BANKED
	rgoto MBMF_PATCH_Send_Faders_Loop

#if DEFAULT_NUMBER_BUTTONS	
	;; send button events
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_BUTTON, BANKED
MBMF_PATCH_Send_Buttons_Loop
	;; print a "#" character on every 8th event
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	andlw	0x07
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	call	MBMF_MIDI_SendButtonEvent

	;; add short delay for relaxed timings (required for some sequfaderers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	movlw	DEFAULT_NUMBER_BUTTONS-1
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_BUTTON, F, BANKED
	cpfsgt	MBMF_CURRENT_BUTTON, BANKED
	rgoto MBMF_PATCH_Send_Buttons_Loop
#endif

	;; stop message and exit
	goto	MIOS_LCD_MessageStop


;; --------------------------------------------------------------------------
;;  stores all fader/button values in EEPROM/BankStick
;; --------------------------------------------------------------------------
MBMF_PATCH_SAVE_STR_0	STRING 16, 0x00, "*** Snapshot ***"
MBMF_PATCH_SAVE_STR_1	STRING 16, 0x40, "* Cheeese :-D **"
MBMF_PATCH_Save
	;; display message
	TABLE_ADDR MBMF_PATCH_SAVE_STR_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	;; store values in dump structure and exit
	goto	MBMF_DUMP_StorePatch


;; --------------------------------------------------------------------------
;;  Use this function when changing to another patch to save the current and
;;  to restore the new values
;;  IN: new patch in WREG
;; --------------------------------------------------------------------------
MBMF_PATCH_Change
	;; temporary save bank in MB_NEW_BANK (misused as NEW_PATCH register)
	movwf	MB_NEW_BANK

	;; if auto-snapshot function set, save current enc/button settings
	SET_BSR	MBMF_BASE
	RCALL_IFSET MBMF_CFG1, MBMF_CFG1_AUTO_SNAPSHOT, BANKED, MBMF_DUMP_StorePatch

	;; now move new patch number to MB_PATCH
	movff	MB_NEW_BANK, MB_PATCH

	;; restore data
	call	MBMF_DUMP_RestorePatch

	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; send a program change of user flag is set
	SET_BSR	MBMF_BASE
	BRA_IFCLR MBMF_CFG1, MBMF_CFG1_SEND_PC_ON_BCHANGE, BANKED, MBMF_PATCH_Change_End
MBMF_PATCH_Change_SendPC
	movlw	0xc0 | ((DEFAULT_PATCH_PRG_CHNG_CHANNEL-1) & 0xf)
	call	MIOS_MIDI_TxBufferPut
	movf	MB_PATCH, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
MBMF_PATCH_Change_End

	;; if auto-snapshot function set, restore new enc/button settings
	SET_BSR	MBMF_BASE
	CALL_IFSET MBMF_CFG1, MBMF_CFG1_AUTO_SNAPSHOT, BANKED, MBMF_PATCH_Send

	return


