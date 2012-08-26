; $Id$
;
; MBHP_MF_NG
; SysEx Dump functions
;
; ==========================================================================
;
;  Copyright 2010 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function restores the whole SysEx dump
;;  Patch number in MF_PATCH
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_DUMP_RestoreAll
	;; restore all values
	clrf	TMP1
MF_DUMP_RestoreAllLoop
	movff	TMP1, EEADR
	call	MF_BANK_Read

	movff	TMP1, MIOS_PARAMETER1
	call	MF_DUMP_WriteData

	incf	TMP1, F		; loop 256 times
	bnz	MF_DUMP_RestoreAllLoop

	return

;; --------------------------------------------------------------------------
;;  This function stores the whole SysEx dump in EEPROM or BankStick
;;  Patch number in MF_PATCH
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_DUMP_StoreAll
	;; store all values
	clrf	TMP1
MF_DUMP_StoreAllLoop
	movff	TMP1, MIOS_PARAMETER1
	call	MF_DUMP_ReadData

	movff	TMP1, EEADR
	call	MF_BANK_Write

	incf	TMP1, F		; loop 256 times
	bnz	MF_DUMP_StoreAllLoop

	return


;; --------------------------------------------------------------------------
;;  This function writes a single data value
;;  IN: data in WREG, address in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
MF_DUMP_WriteData
	movwf	PRODL		; temporary store data in PRODL

	;; branch depending on address
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x10
	bz	MF_DUMP_WriteData_Cfg ; 0x10..0x1f

	movf	MIOS_PARAMETER1, W
	addlw	-0x20
	andlw	0xc0
	bz	MF_DUMP_WriteData_RAM ; 0x20..0x5f (0x40 bytes)

	return			; reserved or ignore area

MF_DUMP_WriteData_RAM
	lfsr	FSR1, MF_MODE_0
	movf	MIOS_PARAMETER1, W
	addlw	-0x20
	addwf	FSR1L, F
	movff	PRODL, INDF1
	return

MF_DUMP_WriteData_Cfg
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MF_DUMP_WriteData_Cfg_10
	rgoto	MF_DUMP_WriteData_Cfg_11
	rgoto	MF_DUMP_WriteData_Cfg_12
	rgoto	MF_DUMP_WriteData_Cfg_13
	rgoto	MF_DUMP_WriteData_Cfg_14
	rgoto	MF_DUMP_WriteData_Cfg_15
	rgoto	MF_DUMP_WriteData_Cfg_16
	rgoto	MF_DUMP_WriteData_Cfg_17
	rgoto	MF_DUMP_WriteData_Cfg_18
	rgoto	MF_DUMP_WriteData_Cfg_19
	rgoto	MF_DUMP_WriteData_Cfg_1A
	rgoto	MF_DUMP_WriteData_Cfg_1B
	rgoto	MF_DUMP_WriteData_Cfg_1C
	rgoto	MF_DUMP_WriteData_Cfg_1D
	rgoto	MF_DUMP_WriteData_Cfg_1E
	rgoto	MF_DUMP_WriteData_Cfg_1F

MF_DUMP_WriteData_Cfg_10
	movf	PRODL, W
	call	AIN_NumberSet
	goto	MF_Enable	; always call this function to re-initialize variables

MF_DUMP_WriteData_Cfg_11
	movff	PRODL, MF_OPERATION_MODE

	;; set LC emulation ID depending on mode
	movf	PRODL, W
	TABLE_ADDR_MUL_W MF_DUMP_WriteData_Cfg_EmuIdTable, 1
	tblrd*
	movff	TABLAT, LC_ID
	return

MF_DUMP_WriteData_Cfg_EmuIdTable
	db	0x00, 0x00	; Pitchbender Chn1..8 and Chn9..16
	db	0x00, 0x00	; CC7 Chn1..8 and Chn9..16
	db	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0	 ; CCs
	db	0x10, 0x11	; Logic Control and Extension
	db	0x14, 0x15	; Mackie Control and Extension
	db	0x00, 0x00	; Mackie HUI and Motormix (no SysEx emulation required)


MF_DUMP_WriteData_Cfg_12
	movf	PRODL, W
	goto	MIOS_MIDI_MergerSet

MF_DUMP_WriteData_Cfg_13
	movff	PRODL, MF_PWM_STEPS
	return

MF_DUMP_WriteData_Cfg_14
	movff	PRODL, AIN_DEADBAND
	return

MF_DUMP_WriteData_Cfg_15
	movff	PRODL, MF_DEADBAND
	return

MF_DUMP_WriteData_Cfg_16
	movff	PRODL, MF_TOUCHSENSOR_MODE
	return

MF_DUMP_WriteData_Cfg_17
	movf	PRODL, W
	goto	MIOS_SRIO_TS_SensitivitySet

MF_DUMP_WriteData_Cfg_18
	movf	PRODL, W
	andlw	0x0f
	movwf	MIDI_CHANNEL
	return

MF_DUMP_WriteData_Cfg_19
MF_DUMP_WriteData_Cfg_1A
MF_DUMP_WriteData_Cfg_1B
MF_DUMP_WriteData_Cfg_1C
MF_DUMP_WriteData_Cfg_1D
MF_DUMP_WriteData_Cfg_1E
MF_DUMP_WriteData_Cfg_1F
	return



;; --------------------------------------------------------------------------
;;  This function reads a single data value
;;  IN: address in MIOS_PARAMETER1
;;  OUT: data in WREG
;; --------------------------------------------------------------------------
MF_DUMP_ReadData
	;; branch depending on address
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x10
	bz	MF_DUMP_ReadData_Cfg ; 0x10..0x1f

	movf	MIOS_PARAMETER1, W
	addlw	-0x20
	andlw	0xc0
	bz	MF_DUMP_ReadData_RAM ; 0x20..0x5f (0x40 bytes)

	andlw	0x00		; will also set ZERO flag
	return

MF_DUMP_ReadData_RAM
	lfsr	FSR1, MF_MODE_0
	movf	MIOS_PARAMETER1, W
	addlw	-0x20
	addwf	FSR1L, F
	movf	INDF1, W
	return

MF_DUMP_ReadData_Cfg
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MF_DUMP_ReadData_Cfg_10
	rgoto	MF_DUMP_ReadData_Cfg_11
	rgoto	MF_DUMP_ReadData_Cfg_12
	rgoto	MF_DUMP_ReadData_Cfg_13
	rgoto	MF_DUMP_ReadData_Cfg_14
	rgoto	MF_DUMP_ReadData_Cfg_15
	rgoto	MF_DUMP_ReadData_Cfg_16
	rgoto	MF_DUMP_ReadData_Cfg_17
	rgoto	MF_DUMP_ReadData_Cfg_18
	rgoto	MF_DUMP_ReadData_Cfg_19
	rgoto	MF_DUMP_ReadData_Cfg_1A
	rgoto	MF_DUMP_ReadData_Cfg_1B
	rgoto	MF_DUMP_ReadData_Cfg_1C
	rgoto	MF_DUMP_ReadData_Cfg_1D
	rgoto	MF_DUMP_ReadData_Cfg_1E
	rgoto	MF_DUMP_ReadData_Cfg_1F

MF_DUMP_ReadData_Cfg_10
	call	AIN_NumberGet
	return

MF_DUMP_ReadData_Cfg_11
	movff	MF_OPERATION_MODE, WREG
	return

MF_DUMP_ReadData_Cfg_12
	goto	MIOS_MIDI_MergerGet

MF_DUMP_ReadData_Cfg_13
	movff	MF_PWM_STEPS, WREG
	return

MF_DUMP_ReadData_Cfg_14
	movff	AIN_DEADBAND, WREG
	return

MF_DUMP_ReadData_Cfg_15
	movff	MF_DEADBAND, WREG
	return

MF_DUMP_ReadData_Cfg_16
	movff	MF_TOUCHSENSOR_MODE, WREG
	return

MF_DUMP_ReadData_Cfg_17
	goto	MIOS_SRIO_TS_SensitivityGet

MF_DUMP_ReadData_Cfg_18
	movff	MIDI_CHANNEL, WREG
	andlw	0x0f
	return

MF_DUMP_ReadData_Cfg_19
MF_DUMP_ReadData_Cfg_1A
MF_DUMP_ReadData_Cfg_1B
MF_DUMP_ReadData_Cfg_1C
MF_DUMP_ReadData_Cfg_1D
MF_DUMP_ReadData_Cfg_1E
MF_DUMP_ReadData_Cfg_1F
	andlw	0x00		; will also set ZERO flag
	return
