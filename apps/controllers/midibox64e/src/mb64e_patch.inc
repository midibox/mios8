; $Id$
;
; MIDIbox64E
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
MB64E_PATCH_SendCurrent
	;; copy current pot/button values into arrays

#if DEFAULT_NUMBER_AIN
	SET_BSR	MB64E_BASE
	movlw	0x40		; pots/faders are starting at offset 0x40
	movwf	MB64E_CURRENT_ENTRY, BANKED
MB64E_PATCH_SendCurrent_P_Loop
	call	MB64E_FSR_FaderValue
	movff	MB64E_CURRENT_ENTRY, WREG
	addlw	-0x40
	call	MIOS_AIN_Pin7bitGet
	movwf	INDF0

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movlw	0x40+DEFAULT_NUMBER_AIN-1
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto	MB64E_PATCH_SendCurrent_P_Loop
#endif

#if DEFAULT_NUMBER_BUTTONS
	clrf	TMP5
MB64E_PATCH_SendCurrent_B_Loop
	movff	TMP5, MIOS_PARAMETER1
	call	MB64E_BUTTON_Map
	bnz	MB64E_PATCH_SendCurrent_B_Next

	SET_BSR	MB64E_BASE
	movf	MIOS_PARAMETER1, W
	movwf	MB64E_CURRENT_BUTTON, BANKED

	rrf	TMP5, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0xf	
	call	MIOS_DIN_SRGet
	movwf	TMP1

	call	MB64E_FSR_ButtonValue
	movff	TMP1, INDF0

MB64E_PATCH_SendCurrent_B_Next
	movlw	8
	addwf	TMP5, F
	movlw	DEFAULT_NUMBER_BUTTONS-1
	cpfsgt	TMP5, ACCESS
	rgoto	MB64E_PATCH_SendCurrent_B_Loop
#endif

	rgoto	MB64E_PATCH_SendCurrentCont


;; --------------------------------------------------------------------------
;;  sends a enc/button snapshot
;; --------------------------------------------------------------------------
MB64E_PATCH_SENDING_STR_0	STRING 16, 0x00, "Sending Snapshot"
MB64E_PATCH_Send

	;; restore settings
	call	MB64E_DUMP_RestorePatch

        ;; optionally send current pot/button settings without restoring patch
MB64E_PATCH_SendCurrentCont
	;; clear screen
	call	MIOS_LCD_Clear

	;; display message
	TABLE_ADDR MB64E_PATCH_SENDING_STR_0
	call	MIOS_LCD_PrintMessage

	;; for the progress bar: begin at second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

#if DEFAULT_NUMBER_ENCS
	;; send enc events
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
MB64E_PATCH_Send_Encs_Loop
	;; print a "#" character on every 16th event
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	andlw	0x0f
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	;; get encoder mode and store it in MB64E_CURRENT_ENC_MODE
	call	MB64E_ADDR_EncModeEntry
	call	MB64E_BANK_Read
	SET_BSR	MB64E_BASE
	andlw	0x0f
	movwf	MB64E_CURRENT_ENC_MODE, BANKED

	;; send event only on ENC_MODE_ABSOLUTE --- don't send relative events here!
	skpnz			; (ENC_MODE_ABSOLUTE == 0)
	call	MB64E_MIDI_SendEncEvent

	;; add short delay for relaxed timings (required for some sequencers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movlw	128-1
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto MB64E_PATCH_Send_Encs_Loop
#endif

#if DEFAULT_NUMBER_BUTTONS
	;; send button events
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_BUTTON, BANKED
MB64E_PATCH_Send_Buttons_Loop
	;; print a "#" character on every 8th event
	movf	MB64E_CURRENT_BUTTON, W, BANKED
	andlw	0x07
	movlw	'#'
	skpnz
	call	MIOS_LCD_PrintChar

	call	MB64E_MIDI_SendButtonEvent

	;; add short delay for relaxed timings (required for some sequencers and old synths)
	movlw	1 ; ms
	call	MIOS_Delay

	movlw	DEFAULT_NUMBER_BUTTONS-1
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_BUTTON, F, BANKED
	cpfsgt	MB64E_CURRENT_BUTTON, BANKED
	rgoto MB64E_PATCH_Send_Buttons_Loop
#endif

	;; stop message and exit
	goto	MIOS_LCD_MessageStop


;; --------------------------------------------------------------------------
;;  stores all enc/button values in EEPROM/BankStick
;; --------------------------------------------------------------------------
MB64E_PATCH_SAVE_STR_0	STRING 16, 0x00, "*** Snapshot ***"
MB64E_PATCH_SAVE_STR_1	STRING 16, 0x40, "* Cheeese :-D **"
MB64E_PATCH_Save
	;; display message
	TABLE_ADDR MB64E_PATCH_SAVE_STR_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	;; store values in dump structure and exit
	goto	MB64E_DUMP_StorePatch



;; --------------------------------------------------------------------------
;;  Use this function when changing to another patch to save the current and
;;  to restore the new values
;;  IN: new patch in WREG
;; --------------------------------------------------------------------------
MB64E_PATCH_Change
	;; temporary save bank in MB_NEW_BANK (misused as NEW_PATCH register)
	movwf	MB_NEW_BANK

	;; if auto-snapshot function set, save current enc/button settings
	SET_BSR	MB64E_BASE
	RCALL_IFSET MB64E_CFG1, MB64E_CFG1_AUTO_SNAPSHOT, BANKED, MB64E_DUMP_StorePatch

	;; now move new patch number to MB_PATCH
	movff	MB_NEW_BANK, MB_PATCH

	;; restore data
	call	MB64E_DUMP_RestorePatch

	;; request re-init of page so that the new patchname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; send a program change of user flag is set
	SET_BSR	MB64E_BASE
	BRA_IFCLR MB64E_CFG1, MB64E_CFG1_SEND_PC_ON_BCHANGE, BANKED, MB64E_PATCH_Change_End
MB64E_PATCH_Change_SendPC
	movlw	0xc0 | ((DEFAULT_PATCH_PRG_CHNG_CHANNEL-1) & 0xf)
	call	MIOS_MIDI_TxBufferPut
	movf	MB_PATCH, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
MB64E_PATCH_Change_End

	;; if auto-snapshot function set, restore new enc/button settings
	SET_BSR	MB64E_BASE
	CALL_IFSET MB64E_CFG1, MB64E_CFG1_AUTO_SNAPSHOT, BANKED, MB64E_PATCH_Send

	return


