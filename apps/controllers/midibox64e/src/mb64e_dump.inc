; $Id$
;
; MIDIbox64E
; MB64E Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole dump data (enc/buttons and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreAll
	rcall	MB64E_DUMP_RestoreGlobal
	call	MB64E_ENC_UpdateMap
	movlw	0xff
	call	MB64E_ENC_UpdateSpeed
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	rcall	MB64E_DUMP_RestoreEncButtonEvnts
	rgoto	MB64E_DUMP_RestoreLEDRP


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (enc/button values and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreAll
	rcall	MB64E_DUMP_StorePatch
	rgoto	MB64E_DUMP_StoreGlobal



;; --------------------------------------------------------------------------
;;  This function restores the dump data from EEPROM/BankStick
;;  IN: current bank in MB_BANK, global MIDI channel in MB64E_GLOBAL_CHANNEL
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreEncButtonEvnts
	;; see "MB64E_presets.inc" for the structure

	;; restore enc events
	SET_BSR MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED ; (128 events)
MB64E_DUMP_Restore_Loop1
	call	MB64E_ADDR_EncEntry	; get address of enc entry

	call	MB64E_FSR_EncEvnt0	; load and store EVNT0 value
	call	MB64E_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0

	SET_BSR	MB64E_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MB64E_GLOBAL_CHANNEL, W, BANKED
	bz	MB64E_DUMP_Restore_Loop1_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MB64E_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MB64E_DUMP_Restore_Loop1_NoGlob
	
	call	MB64E_FSR_EncEvnt1	; load and store EVNT1 value
	call	MB64E_BANK_Read
	movwf	INDF0

	call	MB64E_BANK_Read		; Min and Max value not stored in RAM,
	call	MB64E_BANK_Read		; skip it

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	BRA_IFCLR MB64E_CURRENT_ENTRY, 7, BANKED, MB64E_DUMP_Restore_Loop1

	;; restore button events
	SET_BSR MB64E_BASE
	clrf	MB64E_CURRENT_BUTTON, BANKED ; (64 events)
MB64E_DUMP_Restore_Loop2
	call	MB64E_ADDR_ButtonEntry	; get address of button entry

	call	MB64E_FSR_ButtonEvnt0	; load and store EVNT0 value
	call	MB64E_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0
	
	SET_BSR	MB64E_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MB64E_GLOBAL_CHANNEL, W, BANKED
	bz	MB64E_DUMP_Restore_Loop2_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MB64E_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MB64E_DUMP_Restore_Loop2_NoGlob
	
	call	MB64E_FSR_ButtonEvnt1	; load and store EVNT1 value
	call	MB64E_BANK_Read
	movwf	INDF0

	call	MB64E_BANK_Read		; third value and mode not stored in RAM,
	call	MB64E_BANK_Read		; skip it

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64E_CURRENT_BUTTON, 6, BANKED, MB64E_DUMP_Restore_Loop2

	rgoto	MB64E_DUMP_RestorePatch


;; --------------------------------------------------------------------------
;;  This function restores the enc and button values
;;  IN: current bank in MB64E_BANK, current patch in MB64E_PATCH
;; --------------------------------------------------------------------------
MB64E_DUMP_RestorePatch
	SET_BSR	MB64E_BASE

	;; restore enc values
	clrf	MB64E_CURRENT_ENTRY, BANKED ; (128 entries)
MB64E_DUMP_RestorePatch_Loop1
	call	MB64E_FSR_EncValue
	call	MB64E_DUMP_GetEncValue
	movwf	INDF0
	bsf	INDF0, 7		; set soft-overtake flag
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	BRA_IFCLR MB64E_CURRENT_ENTRY, 7, BANKED, MB64E_DUMP_RestorePatch_Loop1

	;; restore button values
	clrf	MB64E_CURRENT_BUTTON, BANKED ; (64 entries)
MB64E_DUMP_RestorePatch_Loop2
	call	MB64E_FSR_ButtonValue
#if DEFAULT_PATCHMODE == 0
	call	MB64E_ADDR_ButtonValue
	call	MB64E_BANK_Read	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MB64E_BANK_Read	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#else
	call	MB64E_ADDR_PATCH_Button
	call	MB64E_BANK_PatchRead	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MB64E_BANK_PatchRead	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#endif

	SET_BSR	MB64E_BASE
	movlw	0x08
	addwf	MB64E_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64E_CURRENT_BUTTON, 6, BANKED, MB64E_DUMP_RestorePatch_Loop2

	;; init MIDI status (which isn't saved)
	clrf	MB64E_CURRENT_BUTTON, BANKED ; (64 entries)
	lfsr	FSR0, MB64E_MBUTTON_VALUES_SR0
MB64E_DUMP_RestorePatch_Loop3
	setf	POSTINC0
	movlw	0x08
	addwf	MB64E_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64E_CURRENT_BUTTON, 6, BANKED, MB64E_DUMP_RestorePatch_Loop3

#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to restore the patchname
	rcall	MB64E_DUMP_RestoreBankName
#endif

	;; request update of fader positions
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return

	
;; --------------------------------------------------------------------------
;;  This function returns the current encoder value,
;;  selected by MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_DUMP_GetEncValue
#if DEFAULT_PATCHMODE == 0
	;; get address within BANK structure
	call	MB64E_ADDR_EncValue
	;; read and return value
	goto	MB64E_BANK_Read
#else
	;; get address within BANK structure
	call	MB64E_ADDR_PATCH_Enc
	;; read and return value
	goto	MB64E_BANK_PatchRead
#endif
	
;; --------------------------------------------------------------------------
;;  This function saves only the enc and button values
;;  IN: current bank in MB64E_BANK, current patch in MB64E_PATCH
;; --------------------------------------------------------------------------
MB64E_DUMP_StorePatch
	;; ---[ store enc values (2 * 64byte transfers) ]---
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MB64E_ADDR_EncValue

	lfsr	FSR1, MB64E_ENC_VALUES_00 + 0x00
	call	MB64E_BANK_WritePage
	lfsr	FSR1, MB64E_ENC_VALUES_00 + 0x40
	call	MB64E_BANK_WritePage
#else
	call	MB64E_ADDR_PATCH_Enc

	lfsr	FSR1, MB64E_ENC_VALUES_00 + 0x00
	call	MB64E_BANK_PatchWritePage
	lfsr	FSR1, MB64E_ENC_VALUES_00 + 0x40
	call	MB64E_BANK_PatchWritePage
#endif

	;; ---[ store button values ]---
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_BUTTON, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MB64E_ADDR_ButtonValue
#else
	call	MB64E_ADDR_PATCH_Button
#endif
	lfsr	FSR0, MB64E_BUTTON_VALUES_SR0
MB64E_DUMP_StorePatchLoop1
#if DEFAULT_PATCHMODE == 0
	movf	INDF0, W
	andlw	0x0f
	call	MB64E_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64E_BANK_Write
#else
	movf	INDF0, W
	andlw	0x0f
	call	MB64E_BANK_PatchWrite
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64E_BANK_PatchWrite
#endif

	SET_BSR	MB64E_BASE
	movlw	0x08
	addwf	MB64E_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64E_CURRENT_BUTTON, 6, BANKED, MB64E_DUMP_StorePatchLoop1

#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to save the patchname
	rcall	MB64E_DUMP_StoreBankName
#endif

	return

;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreGlobal
	;; see "MB64E_presets.inc" for the structure

	;; parts of the configuration are always stored in internal EEPROM, so we
	;; can bypass the MB64E_BANK function

	movlw	MB64E_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64E_CFG0
	rcall	MB64E_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64E_CFG1
	rcall	MB64E_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	movlw	MB64E_Presets_DISPLAY_PAGE & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64E_DISPLAY_PAGE
	rcall	MB64E_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64E_MORPH_BANK
	rcall	MB64E_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64E_GLOBAL_CHANNEL
	rcall	MB64E_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	;; init MIDI merger depending on selected CFG
	SET_BSR	MB64E_BASE
	movlw	0x00
	btfss	MB64E_CFG0, MB64E_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	MB64E_CFG1, MB64E_CFG1_MBLINK, BANKED
	iorlw 0x02
	call	MIOS_MIDI_MergerSet

	;; forward TS sensitivity directly to MIOS
	movlw	MB64E_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	lfsr	FSR0, TMP1
	rcall	MB64E_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	call	MIOS_SRIO_TS_SensitivitySet

	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MB64E_DUMP_RestoreBankName
#endif

	call	MB64E_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to read
	lfsr	FSR0, MB64E_LED_MAP_BEGIN
MB64E_DUMP_RestoreGlobal_Loop1
	call	MB64E_BANK_Read
	andlw	0x0f
	movwf	INDF0
	call	MB64E_BANK_Read
	swapf	WREG, W
	andlw	0xf0
	iorwf	POSTINC0, F
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64E_DUMP_RestoreGlobal_Loop1

	return

	;; ---
MB64E_DUMP_RestoreGlobal_Hlp
	call	MIOS_EEPROM_Read
	andlw	0x0f
	movwf	INDF0
	call	MIOS_EEPROM_Read
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
	return


;; --------------------------------------------------------------------------
;;  This function saves the global settings in EEPROM
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreGlobal
	;; see "MB64E_presets.inc" for the structure

	movlw	MB64E_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64E_CFG0
	rcall	MB64E_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64E_CFG1
	rcall	MB64E_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	movlw	MB64E_Presets_DISPLAY_PAGE & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64E_DISPLAY_PAGE
	rcall	MB64E_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64E_MORPH_BANK
	rcall	MB64E_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64E_GLOBAL_CHANNEL
	rcall	MB64E_DUMP_StoreGlobal_Hlp	; writes low/high nibble


	;; get TS sensitivity from MIOS
	movlw	MB64E_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	call	MIOS_SRIO_TS_SensitivityGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MB64E_DUMP_StoreGlobal_Hlp

	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MB64E_DUMP_StoreBankName
#endif

	call	MB64E_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to write
	lfsr	FSR0, MB64E_LED_MAP_BEGIN
MB64E_DUMP_StoreGlobal_Loop2
	movf	INDF0, W
	andlw	0x0f
	call	MB64E_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64E_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64E_DUMP_StoreGlobal_Loop2

	return

	;; ---
MB64E_DUMP_StoreGlobal_Hlp
	movf	INDF0, W
	andlw	0x0f
	call	MIOS_EEPROM_Write
	swapf	INDF0, W
	andlw	0x0f
	goto	MIOS_EEPROM_Write


;; --------------------------------------------------------------------------
;;  This function is used by MB64E_DUMP_RestoreGlobal to restore the bankname
;;  it can also be used to fetch the name without changing the other
;;  parameters (-> see cs_m_copy.inc)
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MB64E_ADDR_Bankname
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
MB64E_DUMP_RestoreBankNameLoop
	call	MB64E_BANK_Read
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_DUMP_RestoreBankNameLoop
#else
	call	MB64E_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
MB64E_DUMP_RestoreBankNameLoop
	call	MB64E_BANK_PatchRead
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_DUMP_RestoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used by MB64E_DUMP_StoreGlobal to store the bankname
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MB64E_ADDR_Bankname
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
MB64E_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MB64E_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_DUMP_StoreBankNameLoop
#else
	call	MB64E_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
MB64E_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MB64E_BANK_PatchWrite
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_DUMP_StoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used in MB64E_sysex.inc to transfer the internal settings
;;  from BankStick memory to internal EEPROM after a dump upload
;; --------------------------------------------------------------------------
MB64E_DUMP_TransferInternals
	;; don't process copy routine if no bankstick connected
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK
	return

	;; transfer the CFG0 setting
	movlw	MB64E_Presets_CFG0 & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	;; transfer the CFG1 setting
	movlw	MB64E_Presets_CFG1 & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	;; transfer the TS_SENSITIVITY setting
	movlw	MB64E_Presets_TS_SENSITIVITY & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	;; transfer the DISPLAY_PAGE setting
	movlw	MB64E_Presets_DISPLAY_PAGE & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	;; transfer the MORPH_BANK setting
	movlw	MB64E_Presets_MORPH_BANK & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	;; transfer the GLOBAL_CHANNEL setting
	movlw	MB64E_Presets_GLOBAL_CHANNEL & 0xff
	rcall	MB64E_DUMP_TransferInternals_Hlp

	return

	;; ---
MB64E_DUMP_TransferInternals_Hlp
	movwf	EEADR
	call	MB64E_ADDR_Global
	movff	EEADR, MB_ADDRL

	;; read first value from BankStick
	call	MB64E_BANK_Read
	;; write to EEPROM
	call	MIOS_EEPROM_Write
	;; read second value from BankStick
	call	MB64E_BANK_Read
	;; write to EEPROM
	goto	MIOS_EEPROM_Write



;; --------------------------------------------------------------------------
;;  This function restores the encoder mode ID from EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreEncMode
	;; select first nibble of the 16-bit ENC_MODE entry
	clrf	TMP5
	;; this function will load the enc mode entries into MB64E_ENC_MODE_BUFFER
	;; in packed format
	;; this buffer is located to the same address like MB64E_LEDRING_PATTERNS
	rgoto	MB64E_DUMP_RestoreEncM

;; --------------------------------------------------------------------------
;;  This function stores the encoder mode ID into EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreEncMode
	;; select first nibble of the 16-bit ENC_MODE entry
	clrf	TMP5
	;; this function will store the selected enc mode entries
	rgoto	MB64E_DUMP_StoreEncM

;; --------------------------------------------------------------------------
;;  This function restores the encoder speed ID from EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreEncSpeed
	;; select second nibble of the 16-bit ENC_MODE entry
	movlw	0x01
	movwf	TMP5
	;; this function will load the enc mode entries into MB64E_ENC_MODE_BUFFER
	;; in packed format
	;; this buffer is located to the same address like MB64E_LEDRING_PATTERNS
	rgoto	MB64E_DUMP_RestoreEncM

;; --------------------------------------------------------------------------
;;  This function stores the encoder speed ID into EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreEncSpeed
	;; select second nibble of the 16-bit ENC_MODE entry
	movlw	0x01
	movwf	TMP5
	;; this function will store the selected enc mode entries
	rgoto	MB64E_DUMP_StoreEncM

;; --------------------------------------------------------------------------
;;  This function restores the ledring pattern assign map from EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreLEDRP
	;; select third nibble of the 16-bit ENC_MODE entry
	movlw	0x02
	movwf	TMP5
	;; this function will load the enc mode entries into MB64E_ENC_MODE_BUFFER
	;; in packed format
	;; this buffer is located to the same address like MB64E_LEDRING_PATTERNS
	rgoto	MB64E_DUMP_RestoreEncM

;; --------------------------------------------------------------------------
;;  This function stores the ledring pattern assign map into EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreLEDRP
	;; select third nibble of the 16-bit ENC_MODE entry
	movlw	0x02
	movwf	TMP5
	;; this function will store the selected enc mode entries
	rgoto	MB64E_DUMP_StoreEncM


;; --------------------------------------------------------------------------
;;  This help function selects a nibble from the 16-bit word ENC_MODE entries
;;  and stores it in MB64E_ENC_MODE_BUFFER in packed format
;;  IN: MB_BANK
;;      TMP5[1..0] selects the nibble (0-3)
;; --------------------------------------------------------------------------
MB64E_DUMP_RestoreEncM
	;; go through all 128 entries
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
	;; store values in packed format into the MB64E_ENC_MODE_BUFFER
	lfsr	FSR0, MB64E_ENC_MODE_BUFFER
MB64E_DUMP_RestoreEncMLoop
	;; get address to ENC mode entry
	call	MB64E_ADDR_EncModeEntry
	;; read value
	call	MB64E_BANK_Read
	CALL_IFSET TMP5, 1, ACCESS, MB64E_BANK_Read
	btfsc	TMP5, 0
	swapf	WREG, W
	;; store first 4 bit in TMP1[3..0]
	andlw	0x0f
	movwf	TMP1
	;; next entry
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	;; get address to ENC mode entry
	call	MB64E_ADDR_EncModeEntry
	;; read value
	call	MB64E_BANK_Read
	CALL_IFSET TMP5, 1, ACCESS, MB64E_BANK_Read
	btfsc	TMP5, 0
	swapf	WREG, W
	;; move first 4 bit to WREG[7..4]
	swapf	WREG, W
	andlw	0xf0
	;; OR it with the 4 bit of the last value
	iorwf	TMP1, W
	;; store result in ENC_MODE_BUFFER and increment pointer
	movwf	POSTINC0
	;; loop 64 times
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	BRA_IFCLR MB64E_CURRENT_ENTRY, 7, BANKED, MB64E_DUMP_RestoreEncMLoop
	return


;; --------------------------------------------------------------------------
;;  This help function stores 128 nibbles from MB64E_ENC_MODE_BUFFER to
;;  EEPROM/BankStick
;;  IN: MB_BANK
;;      TMP5[1..0] selects the nibble (0-3)
;; --------------------------------------------------------------------------
MB64E_DUMP_StoreEncM
	;; go through all 128 entries
	SET_BSR	MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
	;; values are stored in packed format in MB64E_ENC_MODE_BUFFER
	lfsr	FSR0, MB64E_ENC_MODE_BUFFER

	;; no additional comments here, just lay back and enjoy the code
MB64E_DUMP_StoreEncMLoop
	rcall	MB64E_DUMP_StoreEncM_Get
	movf	INDF0, W
	rcall	MB64E_DUMP_StoreEncM_Merge

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED

	rcall	MB64E_DUMP_StoreEncM_Get
	swapf	POSTINC0, W
	rcall	MB64E_DUMP_StoreEncM_Merge

	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	
	movf	MB64E_CURRENT_ENTRY, W, BANKED
	andlw	0x1f
	bnz	MB64E_DUMP_StoreEncMLoopNext
	movlw	-0x20
	addwf	MB64E_CURRENT_ENTRY, F, BANKED
	call	MB64E_ADDR_EncModeEntry	
	lfsr	FSR1, MB64E_DATA_BUFFER
	call	MB64E_BANK_WritePage
	SET_BSR	MB64E_BASE
	movlw	0x20
	addwf	MB64E_CURRENT_ENTRY, F, BANKED
MB64E_DUMP_StoreEncMLoopNext
	SET_BSR	MB64E_BASE
	BRA_IFCLR MB64E_CURRENT_ENTRY, 7, BANKED, MB64E_DUMP_StoreEncMLoop
	return

;; help functions
MB64E_DUMP_StoreEncM_Get
	BRA_IFSET TMP5, 1, ACCESS, MB64E_DUMP_StoreEncM_GetH
MB64E_DUMP_StoreEncM_GetL
	call	MB64E_ADDR_EncModeEntry
	call	MB64E_BANK_Read
	btfss	TMP5, 0
	andlw	0xf0
	btfsc	TMP5, 0
	andlw	0x0f
	movwf	TMP1
	return

MB64E_DUMP_StoreEncM_GetH
	call	MB64E_ADDR_EncModeEntry
	call	MB64E_BANK_Read
	movwf	TMP1
	call	MB64E_BANK_Read
	btfss	TMP5, 0
	andlw	0xf0
	btfsc	TMP5, 0
	andlw	0x0f
	movwf	TMP2
	return

MB64E_DUMP_StoreEncM_Merge
	andlw	0x0f
	btfsc	TMP5, 0
	swapf	WREG, W
	BRA_IFSET TMP5, 1, ACCESS, MB64E_DUMP_StoreEncM_MergeH
MB64E_DUMP_StoreEncM_MergeL
	iorwf	TMP1, F	
	call	MB64E_BANK_Read
	CALL_IFSET TMP5, 1, ACCESS, MB64E_BANK_Read
	movwf	TMP2
	rgoto	MB64E_DUMP_StoreEncM_MergeCont

MB64E_DUMP_StoreEncM_MergeH
	iorwf	TMP2, F	

MB64E_DUMP_StoreEncM_MergeCont
	lfsr	FSR1, MB64E_DATA_BUFFER
	SET_BSR	MB64E_BASE
	rlf	MB64E_CURRENT_ENTRY, W, BANKED
	andlw	0x3e
	movff	TMP1, PLUSW1
	addlw	1
	movff	TMP2, PLUSW1
	return
