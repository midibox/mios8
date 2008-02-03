; $Id$
;
; MIDIbox MF
; MBMF Dump functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole dump data (fader/buttons and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MBMF_DUMP_RestoreAll
	rcall	MBMF_DUMP_RestoreGlobal
	rcall	MBMF_DUMP_RestoreFadButtonEvnts
	return


;; --------------------------------------------------------------------------
;;  This function stores the whole dump data (fader/button values and global settings)
;;  IN: current bank in MB_BANK
;; --------------------------------------------------------------------------
MBMF_DUMP_StoreAll
	rcall	MBMF_DUMP_StorePatch
	rgoto	MBMF_DUMP_StoreGlobal



;; --------------------------------------------------------------------------
;;  This function restores the dump data from EEPROM/BankStick
;;  IN: current bank in MB_BANK, global MIDI channel in MBMF_GLOBAL_CHANNEL
;; --------------------------------------------------------------------------
MBMF_DUMP_RestoreFadButtonEvnts
	;; see "mbmf_presets.inc" for the structure

	;; restore fader events
	SET_BSR MBMF_BASE
	clrf	MBMF_CURRENT_FADER, BANKED ; (128 events)
MBMF_DUMP_Restore_Loop1
	call	MBMF_ADDR_FaderEntry	; get address of fader entry

	call	MBMF_FSR_FaderEvnt0	; load and store EVNT0 value
	call	MBMF_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0

	SET_BSR	MBMF_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MBMF_GLOBAL_CHANNEL, W, BANKED
	bz	MBMF_DUMP_Restore_Loop1_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MBMF_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MBMF_DUMP_Restore_Loop1_NoGlob
	
	call	MBMF_FSR_FaderEvnt1	; load and store EVNT1 value
	call	MBMF_BANK_Read
	movwf	INDF0

	call	MBMF_BANK_Read		; Min and Max value not stored in RAM,
	call	MBMF_BANK_Read		; skip it

	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	BRA_IFCLR MBMF_CURRENT_FADER, 7, BANKED, MBMF_DUMP_Restore_Loop1

	;; restore button events
	SET_BSR MBMF_BASE
	clrf	MBMF_CURRENT_BUTTON, BANKED ; (64 events)
MBMF_DUMP_Restore_Loop2
	call	MBMF_ADDR_ButtonEntry	; get address of button entry

	call	MBMF_FSR_ButtonEvnt0	; load and store EVNT0 value
	call	MBMF_BANK_Read
	iorlw	0x80			; set status flag
	movwf	INDF0
	
	SET_BSR	MBMF_BASE		; if global MIDI channel set, overwrite INDF0[3..0]
	movf	MBMF_GLOBAL_CHANNEL, W, BANKED
	bz	MBMF_DUMP_Restore_Loop2_NoGlob
	movlw	0xf0
	andwf	INDF0, F
	decf	MBMF_GLOBAL_CHANNEL, W, BANKED
	andlw	0x0f
	iorwf	INDF0, F
MBMF_DUMP_Restore_Loop2_NoGlob
	
	call	MBMF_FSR_ButtonEvnt1	; load and store EVNT1 value
	call	MBMF_BANK_Read
	movwf	INDF0

	call	MBMF_BANK_Read		; third value and mode not stored in RAM,
	call	MBMF_BANK_Read		; skip it

	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MBMF_CURRENT_BUTTON, 6, BANKED, MBMF_DUMP_Restore_Loop2

	rgoto	MBMF_DUMP_RestorePatch

;; --------------------------------------------------------------------------
;;  This function restores only the fader and button values
;;  IN: current bank in MB_BANK, current patch in MB_PATCH
;; --------------------------------------------------------------------------
MBMF_DUMP_RestorePatch
	SET_BSR	MBMF_BASE

	;; restore fader values
	clrf	MBMF_CURRENT_FADER, BANKED ; (128 entries)
MBMF_DUMP_RestorePatch_Loop1
	call	MBMF_FSR_FaderValue
	call	MBMF_DUMP_GetFaderValue
	movwf	INDF0
	bsf	INDF0, 7		; set soft-overtake flag
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	BRA_IFCLR MBMF_CURRENT_FADER, 7, BANKED, MBMF_DUMP_RestorePatch_Loop1

	;; restore button values
	clrf	MBMF_CURRENT_BUTTON, BANKED ; (64 entries)
MBMF_DUMP_RestorePatch_Loop2
	call	MBMF_FSR_ButtonValue
#if DEFAULT_PATCHMODE == 0
	call	MBMF_ADDR_ButtonValue
	call	MBMF_BANK_Read	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MBMF_BANK_Read	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#else
	call	MBMF_ADDR_PATCH_Button
	call	MBMF_BANK_PatchRead	; high-byte
	andlw	0x0f
	movwf	INDF0
	call	MBMF_BANK_PatchRead	; low-byte
	swapf	WREG, W
	andlw	0xf0
	iorwf	INDF0, F
#endif

	SET_BSR	MBMF_BASE
	movlw	0x08
	addwf	MBMF_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MBMF_CURRENT_BUTTON, 6, BANKED, MBMF_DUMP_RestorePatch_Loop2

	;; init MIDI status (which isn't saved)
	clrf	MBMF_CURRENT_BUTTON, BANKED ; (64 entries)
	lfsr	FSR0, MBMF_MBUTTON_VALUES_SR0
MBMF_DUMP_RestorePatch_Loop3
	setf	POSTINC0
	movlw	0x08
	addwf	MBMF_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MBMF_CURRENT_BUTTON, 6, BANKED, MBMF_DUMP_RestorePatch_Loop3

#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to restore the patchname
	rcall	MBMF_DUMP_RestoreBankName
#endif

	;; request update of fader positions
	bsf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function returns the current fader value,
;;  selected by MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_DUMP_GetFaderValue
#if DEFAULT_PATCHMODE == 0
	;; get address within BANK structure
	call	MBMF_ADDR_FaderValue
	;; read and return value
	goto	MBMF_BANK_Read
#else
	;; get address within BANK structure
	call	MBMF_ADDR_PATCH_Fader
	;; read and return value
	goto	MBMF_BANK_PatchRead
#endif


;; --------------------------------------------------------------------------
;;  This function saves only the fader and button values
;;  IN: current bank in MB_BANK, current patch in MB_PATCH
;; --------------------------------------------------------------------------
MBMF_DUMP_StorePatch
	;; ---[ store fader values (2 * 64byte transfers) ]---
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_FADER, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MBMF_ADDR_FaderValue

	lfsr	FSR1, MBMF_FADER_VALUES_00 + 0x00
	call	MBMF_BANK_WritePage
	lfsr	FSR1, MBMF_FADER_VALUES_00 + 0x40
	call	MBMF_BANK_WritePage
#else
	call	MBMF_ADDR_PATCH_Fader

	lfsr	FSR1, MBMF_FADER_VALUES_00 + 0x00
	call	MBMF_BANK_PatchWritePage
	lfsr	FSR1, MBMF_FADER_VALUES_00 + 0x40
	call	MBMF_BANK_PatchWritePage
#endif

	;; ---[ store button values ]---
	SET_BSR	MBMF_BASE
	clrf	MBMF_CURRENT_BUTTON, BANKED
#if DEFAULT_PATCHMODE == 0
	call	MBMF_ADDR_ButtonValue
#else
	call	MBMF_ADDR_PATCH_Button
#endif
	lfsr	FSR0, MBMF_BUTTON_VALUES_SR0
MBMF_DUMP_StorePatchLoop1
#if DEFAULT_PATCHMODE == 0
	movf	INDF0, W
	andlw	0x0f
	call	MBMF_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MBMF_BANK_Write
#else
	movf	INDF0, W
	andlw	0x0f
	call	MBMF_BANK_PatchWrite
	swapf	POSTINC0, W
	andlw	0x0f
	call	MBMF_BANK_PatchWrite
#endif

	SET_BSR	MBMF_BASE
	movlw	0x08
	addwf	MBMF_CURRENT_BUTTON, F, BANKED
	BRA_IFCLR MBMF_CURRENT_BUTTON, 6, BANKED, MBMF_DUMP_StorePatchLoop1

#if DEFAULT_PATCHMODE == 1
	;; in patch mode we also have to save the patchname
	rcall	MBMF_DUMP_StoreBankName
#endif

	return

;; --------------------------------------------------------------------------
;;  This function restores and inits the global data from EEPROM
;; --------------------------------------------------------------------------
MBMF_DUMP_RestoreGlobal
	;; see "mbmf_presets.inc" for the structure

	;; parts of the configuration are always stored in internal EEPROM, so we
	;; can bypass the MBMF_BANK function
	
	movlw	MBMF_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MBMF_CFG0
	rcall	MBMF_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MBMF_CFG1
	rcall	MBMF_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	movlw	MBMF_Presets_DISPLAY_PAGE & 0xff
	movwf	EEADR
	lfsr	FSR0, MBMF_DISPLAY_PAGE
	rcall	MBMF_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MBMF_MORPH_BANK
	rcall	MBMF_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	lfsr	FSR0, MBMF_GLOBAL_CHANNEL
	rcall	MBMF_DUMP_RestoreGlobal_Hlp	; reads low/high nibble

	;; init MIDI merger depending on selected CFG
	SET_BSR	MBMF_BASE
	movlw	0x00
	btfss	MBMF_CFG0, MBMF_CFG0_MERGEROFF, BANKED
	iorlw 0x01
	btfsc	MBMF_CFG1, MBMF_CFG1_MBLINK, BANKED
	iorlw 0x02
	call	MIOS_MIDI_MergerSet

	;; forward TS sensitivity directly to MIOS
	movlw	MBMF_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	call	MIOS_SRIO_TS_SensitivitySet

	;; forward calibration values directly to MIOS
	movlw	MBMF_Presets_CALI_UP & 0xff
	movwf	EEADR
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	skpnz
	movlw	DEFAULT_MF_PWM_DUTY_UP
	call	MIOS_MF_PWM_DutyCycleUpSet

	rcall	MBMF_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	skpnz
	movlw	DEFAULT_MF_PWM_DUTY_DOWN
	call	MIOS_MF_PWM_DutyCycleDownSet

	rcall	MBMF_DUMP_RestoreGlobal_Hlp
	movf	TMP1, W
	skpnz
	movlw	DEFAULT_MF_PWM_PERIOD
	call	MIOS_MF_PWM_PeriodSet
	
	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MBMF_DUMP_RestoreBankName
#endif

	call	MBMF_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to read
	lfsr	FSR0, MBMF_LED_MAP_BEGIN
MBMF_DUMP_RestoreGlobal_Loop1
	call	MBMF_BANK_Read
	andlw	0x0f
	movwf	INDF0
	call	MBMF_BANK_Read
	swapf	WREG, W
	andlw	0xf0
	iorwf	POSTINC0, F
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MBMF_DUMP_RestoreGlobal_Loop1

	return

	;; ---
MBMF_DUMP_RestoreGlobal_Hlp
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
MBMF_DUMP_StoreGlobal
	;; see "MBMF_presets.inc" for the structure

	movlw	MBMF_Presets_CFG0 & 0xff
	movwf	EEADR
	lfsr	FSR0, MBMF_CFG0
	rcall	MBMF_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MBMF_CFG1
	rcall	MBMF_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	movlw	MBMF_Presets_DISPLAY_PAGE & 0xff
	movwf	EEADR
	lfsr	FSR0, MBMF_DISPLAY_PAGE
	rcall	MBMF_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MBMF_MORPH_BANK
	rcall	MBMF_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	lfsr	FSR0, MBMF_GLOBAL_CHANNEL
	rcall	MBMF_DUMP_StoreGlobal_Hlp	; writes low/high nibble

	;; get TS sensitivity from MIOS
	movlw	MBMF_Presets_TS_SENSITIVITY & 0xff
	movwf	EEADR
	call	MIOS_SRIO_TS_SensitivityGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_StoreGlobal_Hlp

	;; get calibration values from MIOS
	movlw	MBMF_Presets_CALI_UP & 0xff
	movwf	EEADR
	call	MIOS_MF_PWM_DutyCycleUpGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_StoreGlobal_Hlp

	call	MIOS_MF_PWM_DutyCycleDownGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_StoreGlobal_Hlp

	call	MIOS_MF_PWM_PeriodGet
	movwf	TMP1
	lfsr	FSR0, TMP1
	rcall	MBMF_DUMP_StoreGlobal_Hlp

	;; values stored in BankStick
#if DEFAULT_PATCHMODE == 0
	rcall	MBMF_DUMP_StoreBankName
#endif

	call	MBMF_ADDR_LED_Map
	clrf	TMP1		; 8 bytes to write
	lfsr	FSR0, MBMF_LED_MAP_BEGIN
MBMF_DUMP_StoreGlobal_Loop2
	movf	INDF0, W
	andlw	0x0f
	call	MBMF_BANK_Write
	swapf	POSTINC0, W
	andlw	0x0f
	call	MBMF_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MBMF_DUMP_StoreGlobal_Loop2

	return

	;; ---
MBMF_DUMP_StoreGlobal_Hlp
	movf	INDF0, W
	andlw	0x0f
	call	MIOS_EEPROM_Write
	swapf	INDF0, W
	andlw	0x0f
	goto	MIOS_EEPROM_Write


;; --------------------------------------------------------------------------
;;  This function is used by MBMF_DUMP_RestoreGlobal to restore the bankname
;;  it can also be used to fetch the name without changing the other
;;  parameters (-> see cs_m_copy.inc)
;; --------------------------------------------------------------------------
MBMF_DUMP_RestoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MBMF_ADDR_Bankname
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
MBMF_DUMP_RestoreBankNameLoop
	call	MBMF_BANK_Read
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_DUMP_RestoreBankNameLoop
#else
	call	MBMF_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to read
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
MBMF_DUMP_RestoreBankNameLoop
	call	MBMF_BANK_PatchRead
	movwf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_DUMP_RestoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used by MBMF_DUMP_StoreGlobal to store the bankname
;; --------------------------------------------------------------------------
MBMF_DUMP_StoreBankName
#if DEFAULT_PATCHMODE == 0
	call	MBMF_ADDR_Bankname
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
MBMF_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MBMF_BANK_Write
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_DUMP_StoreBankNameLoop
#else
	call	MBMF_ADDR_PATCH_Name
	clrf	TMP1		; 16 bytes to write
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
MBMF_DUMP_StoreBankNameLoop
	movf	POSTINC0, W
	call	MBMF_BANK_PatchWrite
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_DUMP_StoreBankNameLoop
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is used in MBMF_sysex.inc to transfer the internal settings
;;  from BankStick memory to internal EEPROM after a dump upload
;; --------------------------------------------------------------------------
MBMF_DUMP_TransferInternals
	;; don't process copy routine if no bankstick connected
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK
	return

	;; transfer the CFG0 setting
	movlw	MBMF_Presets_CFG0 & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the CFG1 setting
	movlw	MBMF_Presets_CFG1 & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the TS_SENSITIVITY setting
	movlw	MBMF_Presets_TS_SENSITIVITY & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the DISPLAY_PAGE setting
	movlw	MBMF_Presets_DISPLAY_PAGE & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the MORPH_BANK setting
	movlw	MBMF_Presets_MORPH_BANK & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the GLOBAL_CHANNEL setting
	movlw	MBMF_Presets_GLOBAL_CHANNEL & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the CALI_UP setting
	movlw	MBMF_Presets_CALI_UP & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the CALI_DOWN setting
	movlw	MBMF_Presets_CALI_DOWN & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	;; transfer the PERIOD setting
	movlw	MBMF_Presets_PERIOD & 0xff
	rcall	MBMF_DUMP_TransferInternals_Hlp

	return

	;; ---
MBMF_DUMP_TransferInternals_Hlp
	movwf	EEADR
	call	MBMF_ADDR_Global
	movff	EEADR, MB_ADDRL

	;; read first value from BankStick
	call	MBMF_BANK_Read
	;; write to EEPROM
	call	MIOS_EEPROM_Write
	;; read second value from BankStick
	call	MBMF_BANK_Read
	;; write to EEPROM
	goto	MIOS_EEPROM_Write
