; $Id$
;
; MIDIbox64
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
;;  sends current pot/button settings
;; --------------------------------------------------------------------------
MB64_PATCH_SendCurrent
	;; copy current pot/button values into arrays

#if DEFAULT_NUMBER_POTS
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_POT, BANKED
MB64_PATCH_SendCurrent_P_Loop
	call	MB64_FSR_PotValue
	movff	MB64_CURRENT_POT, WREG
	call	MIOS_AIN_Pin7bitGet
	movwf	INDF0

	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	movlw	DEFAULT_NUMBER_POTS-1
	cpfsgt	MB64_CURRENT_POT, BANKED
	rgoto	MB64_PATCH_SendCurrent_P_Loop
#endif

#if DEFAULT_NUMBER_BUTTONS
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_BUTTON, BANKED
MB64_PATCH_SendCurrent_B_Loop
	rrf	MB64_CURRENT_BUTTON, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0xf	
	call	MIOS_DIN_SRGet
	movwf	TMP1

	call	MB64_FSR_ButtonValue
	movff	TMP1, INDF0

	SET_BSR	MB64_BASE
	movlw	8
	addwf	MB64_CURRENT_BUTTON, F, BANKED
	movlw	DEFAULT_NUMBER_BUTTONS-1
	cpfsgt	MB64_CURRENT_BUTTON, BANKED
	rgoto	MB64_PATCH_SendCurrent_B_Loop
#endif

	rgoto	MB64_PATCH_SendCurrentCont

;; --------------------------------------------------------------------------
;;  sends a pot/button snapshot
;; --------------------------------------------------------------------------
MB64_PATCH_SENDING_STR_0	STRING 16, 0x00, "Sending Snapshot"
MB64_PATCH_Send

	;; restore settings
	call	MB64_DUMP_RestorePatch

	;; optionally send current pot/button settings without restoring patch
MB64_PATCH_SendCurrentCont
	;; clear screen
	call	MIOS_LCD_Clear

	;; display message
	TABLE_ADDR MB64_PATCH_SENDING_STR_0
	call	MIOS_LCD_PrintMessage

	;; for the progress bar: begin at second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

#if DEFAULT_NUMBER_POTS
	;; send pot events
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_POT, BANKED
MB64_PATCH_Send_Pots_Loop
	;; print a "#" character on every 8th event
	movf	MB64_CURRENT_POT, W, BANKED
	andlw	0x07
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	call	MB64_MIDI_SendPotEvent

	;; add short delay for relaxed timings (required for some sequencers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	movlw	DEFAULT_NUMBER_POTS-1
	cpfsgt	MB64_CURRENT_POT, BANKED
	rgoto MB64_PATCH_Send_Pots_Loop
#endif	

#if DEFAULT_NUMBER_BUTTONS
	;; send button events
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_BUTTON, BANKED
MB64_PATCH_Send_Buttons_Loop
	;; print a "#" character on every 8th event
	movf	MB64_CURRENT_BUTTON, W, BANKED
	andlw	0x07
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	call	MB64_MIDI_SendButtonEvent

	;; add short delay for relaxed timings (required for some sequencers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	movlw	DEFAULT_NUMBER_BUTTONS-1
	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_BUTTON, F, BANKED
	cpfsgt	MB64_CURRENT_BUTTON, BANKED
	rgoto MB64_PATCH_Send_Buttons_Loop
#endif

	;; stop message and exit
	goto	MIOS_LCD_MessageStop


;; --------------------------------------------------------------------------
;;  stores all pot/button values in EEPROM/BankStick
;; --------------------------------------------------------------------------
MB64_PATCH_SAVE_STR_0	STRING 16, 0x00, "*** Snapshot ***"
MB64_PATCH_SAVE_STR_1	STRING 16, 0x40, "* Cheeese :-D **"
MB64_PATCH_Save
	;; display message
	TABLE_ADDR MB64_PATCH_SAVE_STR_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	;; store values in dump structure and exit
	goto	MB64_DUMP_StorePatch


;; --------------------------------------------------------------------------
;;  Use this function when changing to another patch to save the current and
;;  to restore the new values
;;  IN: new patch in WREG
;; --------------------------------------------------------------------------
MB64_PATCH_Change
	;; temporary save bank in MB_NEW_BANK (misused as NEW_PATCH register)
	movwf	MB_NEW_BANK

	;; if auto-snapshot function set, save current enc/button settings
	SET_BSR	MB64_BASE
	RCALL_IFSET MB64_CFG1, MB64_CFG1_AUTO_SNAPSHOT, BANKED, MB64_DUMP_StorePatch

	;; now move new patch number to MB_PATCH
	movff	MB_NEW_BANK, MB_PATCH

	;; restore data
	call	MB64_DUMP_RestorePatch

	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; send a program change of user flag is set
	SET_BSR	MB64_BASE
	BRA_IFCLR MB64_CFG1, MB64_CFG1_SEND_PC_ON_BCHANGE, BANKED, MB64_PATCH_Change_End
MB64_PATCH_Change_SendPC
	movlw	0xc0 | ((DEFAULT_PATCH_PRG_CHNG_CHANNEL-1) & 0xf)
	call	MIOS_MIDI_TxBufferPut
	movf	MB_PATCH, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
MB64_PATCH_Change_End

	;; if auto-snapshot function set, restore new enc/button settings
	SET_BSR	MB64_BASE
	CALL_IFSET MB64_CFG1, MB64_CFG1_AUTO_SNAPSHOT, BANKED, MB64_PATCH_Send

	return


