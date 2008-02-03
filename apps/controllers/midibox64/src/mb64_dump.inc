; $Id$
;
; MIDIbox64
; MB64 Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole dump data (pot/buttons and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64_DUMP_RestoreAll
	rcall	MB64_DUMP_RestoreGlobal
	goto	MB64_DUMP_RestorePotButtonEvents


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (pot/button values and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64_DUMP_StoreAll
	rcall	MB64_DUMP_StorePatch
	rgoto	MB64_DUMP_StoreGlobal



;; --------------------------------------------------------------------------
;;  This function restores the dump data from EEPROM/BankStick
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MB64_DUMP_RestorePotButtonEvents
	;; see "mb64_presets.inc" for the structure

	;; restore pot events
	SET_BSR MB64_BASE
	clrf	MB64_CURRENT_POT, BANKED ; (64 events)
MB64_DUMP_Restore_Loop1
	call	MB64_ADDR_PotEntry	; get address of pot entry

	call	MB64_FSR_PotEvnt0	; load and store EVNT0 value
	call	MB64_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0

	SET_BSR	MB64_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MB64_GLOBAL_CHANNEL, W, BANKED
	bz	MB64_DUMP_Restore_Loop1_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MB64_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MB64_DUMP_Restore_Loop1_NoGlob
	
	call	MB64_FSR_PotEvnt1	; load and store EVNT1 value
	call	MB64_BANK_Read
	movwf	INDF0

	call	MB64_BANK_Read		; Min and Max value not stored in RAM,
	call	MB64_BANK_Read		; skip it

	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	BRA_IFCLR MB64_CURRENT_POT, 6, BANKED, MB64_DUMP_Restore_Loop1

	;; restore button events
	SET_BSR MB64_BASE
	clrf	MB64_CURRENT_BUTTON, BANKED ; (64 events)
MB64_DUMP_Restore_Loop2
	call	MB64_ADDR_ButtonEntry	; get address of button entry

	call	MB64_FSR_ButtonEvnt0	; load and store EVNT0 value
	call	MB64_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0
	
	SET_BSR	MB64_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MB64_GLOBAL_CHANNEL, W, BANKED
	bz	MB64_DUMP_Restore_Loop2_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MB64_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MB64_DUMP_Restore_Loop2_NoGlob
	
	call	MB64_FSR_ButtonEvnt1	; load and store EVNT1 value
	call	MB64_BANK_Read
	movwf	INDF0

	call	MB64_BANK_Read		; third value and mode not stored in RAM,
	call	MB64_BANK_Read		; skip it

	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64_CURRENT_BUTTON, 6, BANKED, MB64_DUMP_Restore_Loop2
	
	rgoto	MB64_DUMP_RestorePatch
	
;; --------------------------------------------------------------------------
;;  This function saves only the pot and button values
;;  IN: current bank in MB64_BANK
;; --------------------------------------------------------------------------
MB64_DUMP_RestorePatch
	SET_BSR	MB64_BASE

	;; restore pot values
	clrf	MB64_CURRENT_POT, BANKED ; (64 entries)
MB64_DUMP_RestorePatch_Loop1
	call	MB64_FSR_PotValue
	call	MB64_DUMP_GetPotValue
	movwf	INDF0
	bsf	INDF0, 7		; set soft-overtake flag
	;; if snap mode active: clear flag if not equal to current pot value
	SET_BSR	MB64_BASE
	BRA_IFCLR MB64_CFG0, MB64_CFG0_SNAP, BANKED, MB64_DUMP_RestorePatch_Loop1Nxt
	movf	MB64_CURRENT_POT, W, BANKED
	call	MIOS_AIN_Pin7bitGet
	iorlw	0x80		; (set snap bit)
	xorwf	INDF0, W	; compare with new value
	skpz			; clear snap bit if not equal
	bcf	INDF0, 7
MB64_DUMP_RestorePatch_Loop1Nxt
	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	BRA_IFCLR MB64_CURRENT_POT, 6, BANKED, MB64_DUMP_RestorePatch_Loop1

	;; restore button values
	clrf	MB64_CURRENT_BUTTON, BANKED ; (64 entries)
MB64_DUMP_RestorePatch_Loop2
	call	MB64_FSR_ButtonValue
#if DEFAULT_PATCHMODE == 0
	call	MB64_ADDR_ButtonValue
	call	MB64_BANK_Read	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MB64_BANK_Read	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#else
	call	MB64_ADDR_PATCH_Button
	call	MB64_BANK_PatchRead	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MB64_BANK_PatchRead	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#endif

	SET_BSR	MB64_BASE
	movlw	0x08
	addwf	MB64_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64_CURRENT_BUTTON, 6, BANKED, MB64_DUMP_RestorePatch_Loop2

	;; init MIDI status (which isn't saved)
	clrf	MB64_CURRENT_BUTTON, BANKED ; (64 entries)
	lfsr	FSR0, MB64_MBUTTON_VALUES_SR0
MB64_DUMP_RestorePatch_Loop3
	setf	POSTINC0
	movlw	0x08
	addwf	MB64_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64_CURRENT_BUTTON, 6, BANKED, MB64_DUMP_RestorePatch_Loop3
	
#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to restore the patchname
	rcall	MB64_DUMP_RestoreBankName
#endif

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function returns the current encoder value,
;;  selected by MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_DUMP_GetPotValue
#if DEFAULT_PATCHMODE == 0
	;; get address within BANK structure
	call	MB64_ADDR_PotValue
	;; read and return value
	goto	MB64_BANK_Read
#else
	;; get address within BANK structure
	call	MB64_ADDR_PATCH_Pot
	;; read and return value
	goto	MB64_BANK_PatchRead
#endif

	
;; --------------------------------------------------------------------------
;;  This function saves only the pot and button values
;;  IN: current bank in MB64_BANK
;; --------------------------------------------------------------------------
MB64_DUMP_StorePatch
	;; ---[ store pot values (64 bytes at once) ]---
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_POT, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MB64_ADDR_PotValue

	lfsr	FSR1, MB64_POT_VALUES_00 + 0x00
	call	MB64_BANK_WritePage
#else
	call	MB64_ADDR_PATCH_Pot

	lfsr	FSR1, MB64_POT_VALUES_00 + 0x00
	call	MB64_BANK_PatchWritePage
#endif


	;; ---[ store button values ]---
	SET_BSR	MB64_BASE
	clrf	MB64_CURRENT_BUTTON, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MB64_ADDR_ButtonValue
#else
	call	MB64_ADDR_PATCH_Button
#endif
	lfsr	FSR0, MB64_BUTTON_VALUES_SR0
MB64_DUMP_StorePatchLoop1
#if DEFAULT_PATCHMODE == 0
	movf	INDF0, W
	andlw	0x0f
	call	MB64_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64_BANK_Write
#else
	movf	INDF0, W
	andlw	0x0f
	call	MB64_BANK_PatchWrite
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64_BANK_PatchWrite
#endif

	SET_BSR	MB64_BASE
	movlw	0x08
	addwf	MB64_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MB64_CURRENT_BUTTON, 6, BANKED, MB64_DUMP_StorePatchLoop1

#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to save the patchname
	rcall	MB64_DUMP_StoreBankName
#endif

	return

;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
MB64_DUMP_RestoreGlobal
	;; see "mb64_presets.inc" for the structure

	;; parts of the configuration are always stored in internal EEPROM, so we
	;; can bypass the MB64_BANK function
	
	movlw	MB64_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64_CFG0
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_CFG1
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_DISPLAY_PAGE
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_BANK
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_GLOBAL
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G0
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G1
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G2
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G3
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	;; init MIDI merger depending on selected CFG
	SET_BSR	MB64_BASE
	movlw	0x00
	btfss	MB64_CFG0, MB64_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	MB64_CFG1, MB64_CFG1_MBLINK, BANKED
	iorlw 0x02
	call	MIOS_MIDI_MergerSet

	;; forward TS sensitivity directly to MIOS
	movlw	MB64_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	lfsr	FSR0, TMP1
	rcall	MB64_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
#if DEFAULT_ENABLE_AMUX
	movlw	0x00
#endif
	call	MIOS_SRIO_TS_SensitivitySet

	
	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MB64_DUMP_RestoreBankName
#endif

	call	MB64_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to read
	lfsr	FSR0, MB64_LED_MAP_BEGIN
MB64_DUMP_RestoreGlobal_Loop1
	call	MB64_BANK_Read
	andlw	0x0f
	movwf	INDF0
	call	MB64_BANK_Read
	swapf	WREG, W
	andlw	0xf0
	iorwf	POSTINC0, F
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64_DUMP_RestoreGlobal_Loop1

	lfsr	FSR0, MB64_GLOBAL_CHANNEL
	rcall	MB64_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	return

	;; ---
MB64_DUMP_RestoreGlobal_Hlp
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
MB64_DUMP_StoreGlobal
	;; see "mb64_presets.inc" for the structure

	movlw	MB64_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MB64_CFG0
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_CFG1
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_DISPLAY_PAGE
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_BANK
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_GLOBAL
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G0
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G1
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G2
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MB64_MORPH_POT_G3
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; get TS sensitivity from MIOS
	movlw	MB64_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	call	MIOS_SRIO_TS_SensitivityGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MB64_DUMP_StoreGlobal_Hlp

	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MB64_DUMP_StoreBankName
#endif

	call	MB64_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to write
	lfsr	FSR0, MB64_LED_MAP_BEGIN
MB64_DUMP_StoreGlobal_Loop2
	movf	INDF0, W
	andlw	0x0f
	call	MB64_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MB64_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64_DUMP_StoreGlobal_Loop2

	lfsr	FSR0, MB64_GLOBAL_CHANNEL
	rcall	MB64_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	return

	;; ---
MB64_DUMP_StoreGlobal_Hlp
	movf	INDF0, W
	andlw	0x0f
	call	MIOS_EEPROM_Write
	swapf	INDF0, W
	andlw	0x0f
	goto	MIOS_EEPROM_Write


;; --------------------------------------------------------------------------
;;  This function is used by MB64_DUMP_RestoreGlobal to restore the bankname
;;  it can also be used to fetch the name without changing the other
;;  parameters (-> see cs_m_copy.inc)
;; --------------------------------------------------------------------------
MB64_DUMP_RestoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MB64_ADDR_Bankname
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MB64_BANKNAME_BEGIN
MB64_DUMP_RestoreBankNameLoop
	call	MB64_BANK_Read
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_DUMP_RestoreBankNameLoop
#else
	call	MB64_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MB64_BANKNAME_BEGIN
MB64_DUMP_RestoreBankNameLoop
	call	MB64_BANK_PatchRead
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_DUMP_RestoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used by MB64_DUMP_StoreGlobal to store the bankname
;; --------------------------------------------------------------------------
MB64_DUMP_StoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MB64_ADDR_Bankname
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MB64_BANKNAME_BEGIN
MB64_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MB64_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_DUMP_StoreBankNameLoop
#else
	call	MB64_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MB64_BANKNAME_BEGIN
MB64_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MB64_BANK_PatchWrite
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_DUMP_StoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used in mb64_sysex.inc to transfer the internal settings
;;  from BankStick memory to internal EEPROM after a dump upload
;; --------------------------------------------------------------------------
MB64_DUMP_TransferInternals
	;; don't process copy routine if no bankstick connected
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK
	return

	;; transfer the CFG0 setting
	movlw	MB64_Presets_CFG0 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the CFG1 setting
	movlw	MB64_Presets_CFG1 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the DISPLAY_PAGE setting
	movlw	MB64_Presets_DISPLAY_PAGE & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the MORPH_BANK setting
	movlw	MB64_Presets_MORPH_BANK & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the MORPH_POT_xx settings
	movlw	MB64_Presets_MORPH_POT_GLOBAL & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp
	movlw	MB64_Presets_MORPH_POT_G0 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp
	movlw	MB64_Presets_MORPH_POT_G1 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp
	movlw	MB64_Presets_MORPH_POT_G2 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp
	movlw	MB64_Presets_MORPH_POT_G3 & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the TS_SENSITIVITY setting
	movlw	MB64_Presets_TS_SENSITIVITY & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	;; transfer the GLOBAL_CHANNEL setting
	movlw	MB64_Presets_GLOBAL_CHANNEL & 0xff
	rcall	MB64_DUMP_TransferInternals_Hlp

	return

	;; ---
MB64_DUMP_TransferInternals_Hlp
	movwf	EEADR
	call	MB64_ADDR_Global
	movff	EEADR, MB_ADDRL

	;; read first value from BankStick
	call	MB64_BANK_Read
	;; write to EEPROM
	call	MIOS_EEPROM_Write
	;; read second value from BankStick
	call	MB64_BANK_Read
	;; write to EEPROM
	goto	MIOS_EEPROM_Write
