; $Id$
;
; MIDIbox64
; Bank Load/Store Routines
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

        
;; --------------------------------------------------------------------------
;;  Writes data to EEPROM or BankStick
;;  IN: Value in WREG, 16bit address offset in MBADRR[LH]
;;  OUT: increments address
;; --------------------------------------------------------------------------
MB64_BANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_WriteInternal

MB64_BANK_WriteBankStick				; BankStick write:
	rcall	MB64_BANK_Calc_BankStickAddr	; calculate bankstick address
	movf	MIOS_PARAMETER3, W
	call	MIOS_BANKSTICK_Write		; write content
	;; reset core if bankstick not available anymore (just for debugging)
	btfss	 MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	reset
	bz	MB64_BANK_Write_BS_End		; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

MB64_BANK_Write_BS_End
	rgoto	MB64_BANK_Write_End

MB64_BANK_WriteInternal
	;; branch depending on address:
	;; 0x000-0x1ff: SectFL, internal flash memory
	;; 0x200-0x3ff: SectE, internal EEPROM memory
	;; 0x400-0x7ff: SectFU, internal flash memory
	movf	MB_ADDRH, W
	bz	MB64_BANK_WriteInternal_SectFL
	decf	WREG, W
	bz	MB64_BANK_WriteInternal_SectFL
	decf	WREG, W
	bz	MB64_BANK_WriteInternal_SectE
	decf	WREG, W
	bz	MB64_BANK_Write_End
	;; 	rgoto	MB64_BANK_WriteInternal_SectFU

MB64_BANK_WriteInternal_SectFL
MB64_BANK_WriteInternal_SectFU
	;; not implemented - use MB64_BANK_WritePage instead!
	rgoto	MB64_BANK_Write_End
	
MB64_BANK_WriteInternal_SectE
	rcall	MB64_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	;; don't write if address is 0xff (used to save the device ID)
	incf	EEADR, W
	bz	MB64_BANK_WriteInternal_NOk
	
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
MB64_BANK_WriteInternal_NOk

MB64_BANK_Write_End
	;; increment 16bit MB address
	incf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to Flash, EEPROM or BankStick
;;  IN: Values in FSR1 buffer, 16bit address offset in MB_ADDR[LH]
;;  OUT: increments address
;; --------------------------------------------------------------------------
MB64_BANK_WritePage
	;; align MB_ADDRL
	movlw	0xc0
	andwf	MB_ADDRL, F

	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_WritePageInt

MB64_BANK_WritePageBankStick			; BankStick write:
	rcall	MB64_BANK_Calc_BankStickAddr	; calculate bankstick address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	;; reset core if bankstick not available anymore (just for debugging)
	btfss	 MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	reset
	bz	MB64_BANK_WritePage_BS_End	; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

MB64_BANK_WritePage_BS_End
	rgoto	MB64_BANK_WritePage_End

MB64_BANK_WritePageInt
	;; branch depending on address:
	;; 0x000-0x1ff: SectFL, internal flash memory
	;; 0x200-0x3ff: SectE, internal EEPROM memory
	;; 0x400-0x7ff: SectFU, internal flash memory
	movf	MB_ADDRH, W
	bz	MB64_BANK_WritePageInt_SectFL
	decf	WREG, W
	bz	MB64_BANK_WritePageInt_SectFL
	decf	WREG, W
	bz	MB64_BANK_WritePageInt_SectE
	decf	WREG, W
	bz	MB64_BANK_WritePage_End
	;; 	rgoto	MB64_BANK_WritePageInt_SectFU

MB64_BANK_WritePageInt_SectFL
MB64_BANK_WritePageInt_SectFU
	rcall	MB64_BANK_Calc_FlashAddr	; calculate FLASH address
	;; buffer already prepared in FSR1
 	call	MIOS_FLASH_Write		; write page
	;; here we could add an exception handler
	rgoto	MB64_BANK_WritePage_End
	
MB64_BANK_WritePageInt_SectE
	;; if MB_ADDRL == 0xc0, copy device ID to 0xff (READONLY location)
	movlw	0xc0
	cpfseq	MB_ADDRL, ACCESS
	rgoto MB64_BANK_WritePageInt_SctE_NP
MB64_BANK_WritePageInt_SctE_P
	setf	EEADR
	call	MIOS_EEPROM_Read
	movwf	MIOS_PARAMETER1
	movlw	0x3f
	movff	MIOS_PARAMETER1, PLUSW1
MB64_BANK_WritePageInt_SctE_NP

	rcall	MB64_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	call	MIOS_EEPROM_WritePage		; write page from EEPROM

MB64_BANK_WritePage_End
	;; increment 16bit MB address
	movlw	0x40
	addwf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: 16bit address offset in MBADRR[LH]
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
MB64_BANK_Read
	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_ReadInternal

MB64_BANK_ReadBankStick				; BankStick read:
	rcall	MB64_BANK_Calc_BankStickAddr	; calculate bankstick address
	call	MIOS_BANKSTICK_Read		; read content
	rgoto	MB64_BANK_Read_End		; branch to the end

MB64_BANK_ReadInternal
	;; branch depending on address:
	;; 0x000-0x1ff: SectFL, internal flash memory
	;; 0x200-0x3ff: SectE, internal EEPROM memory
	;; 0x400-0x7ff: SectFU, internal flash memory
	movf	MB_ADDRH, W
	bz	MB64_BANK_ReadInternal_SectFL
	decf	WREG, W
	bz	MB64_BANK_ReadInternal_SectFL
	decf	WREG, W
	bz	MB64_BANK_ReadInternal_SectE
	decf	WREG, W
	bz	MB64_BANK_Read_End
	;; 	rgoto	MB64_BANK_ReadInternal_SectFU

MB64_BANK_ReadInternal_SectFL
MB64_BANK_ReadInternal_SectFU
	rcall	MB64_BANK_Calc_FlashAddr	; calculate flash address
	tblrd*+					; read from table
	movf	TABLAT, W			; get entry
	rgoto	MB64_BANK_Read_End
	
MB64_BANK_ReadInternal_SectE
	rcall	MB64_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	call	MIOS_EEPROM_Read		; read from EEPROM
	;; here we could add an error exception handler

MB64_BANK_Read_End
	;; increment 16bit MB address
	incf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return

;; --------------------------------------------------------------------------
;;  Reads 64 bytes from EEPROM, FLASH or BankStick
;;  IN: pointer to 64 bytes buffer in FSR1, 16bit address offset in MBADRR[LH]
;;  OUT: result in FSR1 buffer, increments address
;; --------------------------------------------------------------------------
MB64_BANK_ReadPage
	;; align MB_ADDRL
	movlw	0xc0
	andwf	MB_ADDRL, F

	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_ReadPageInt

MB64_BANK_ReadPageBankStick				; BankStick read:
	rcall	MB64_BANK_Calc_BankStickAddr	; calculate bankstick address
MB64_BANK_ReadPageBankStickLoop
	call	MIOS_BANKSTICK_Read		; read content
	movwf	MIOS_PARAMETER3
	decf	MIOS_PARAMETER1, W
	andlw	0x3f
	movff	MIOS_PARAMETER3, PLUSW1
	movf	MIOS_PARAMETER1, W
	andlw	0x3f
	bnz	MB64_BANK_ReadPageBankStickLoop
	rgoto	MB64_BANK_ReadPage_End		; branch to the end

MB64_BANK_ReadPageInt
	;; branch depending on address:
	;; 0x000-0x1ff: SectFL, internal flash memory
	;; 0x200-0x2ff: SectE, internal EEPROM memory
	;; 0x300-0x3ff: reserved, read zero
	;; 0x400-0x7ff: SectFU, internal flash memory
	movf	MB_ADDRH, W
	bz	MB64_BANK_ReadPageInt_SectFL
	decf	WREG, W
	bz	MB64_BANK_ReadPageInt_SectFL
	decf	WREG, W
	bz	MB64_BANK_ReadPageInt_SectE
	decf	WREG, W
	bz	MB64_BANK_ReadPage_Reserved
	;; 	rgoto	MB64_BANK_ReadPageInt_SectFU

MB64_BANK_ReadPageInt_SectFL
MB64_BANK_ReadPageInt_SectFU
	rcall	MB64_BANK_Calc_FlashAddr	; calculate flash address
	;; pointer already in FSR1
	call	MIOS_FLASH_Read			; reads 64 bytes
	rgoto	MB64_BANK_ReadPage_End

MB64_BANK_ReadPage_Reserved
	clrf	EEADR
MB64_BANK_ReadPage_ReservedLoop
	movf	EEADR, W
	clrf	PLUSW1
	incf	EEADR, F
	BRA_IFCLR EEADR, 6, ACCESS, MB64_BANK_ReadPage_ReservedLoop
	rgoto	MB64_BANK_ReadPage_End

MB64_BANK_ReadPageInt_SectE
	rcall	MB64_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	call	MIOS_EEPROM_ReadPage		; read page from EEPROM

MB64_BANK_ReadPage_End
	;; increment 16bit MB address
	movlw	0x40
	addwf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return



;; --------------------------------------------------------------------------
;;  Reads a patch from EEPROM or BankStick
;;  IN: patch offset MB_ADRRL, patch number in MB_PATCH
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
MB64_BANK_PatchRead
	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_PatchReadInt
#if DEFAULT_PATCHMODE == 0
	;; branch also if not in patchmode
	rgoto	MB64_BANK_PatchReadInt
#else

MB64_BANK_PatchReadBS
	rcall	MB64_BANK_Calc_PatchBSAddr	; calculate bankstick address
	call	MIOS_BANKSTICK_Read		; read content
	rgoto	MB64_BANK_PatchRead_End	; branch to the end

#endif

MB64_BANK_PatchReadInt
	movff	MB_ADDRL, EEADR			; MB_ADDRL -> EEPROM address
	movlw	0x50-1
	cpfsgt	EEADR, ACCESS
	rgoto MB64_BANK_PatchReadIntOk
MB64_BANK_PatchReadIntName
	movf	EEADR, W
	andlw	0x0f
	addlw	MB64_Presets_BANKNAME & 0xff
	movwf	EEADR

MB64_BANK_PatchReadIntOk
	call	MIOS_EEPROM_Read		; read from EEPROM

MB64_BANK_PatchRead_End
	;; ensure that return value is always 7-bit
	andlw	0x7f

	;; increment 8-bit address
	incf	MB_ADDRL, F
	return

;; --------------------------------------------------------------------------
;;  Writes a patch to EEPROM or BankStick
;;  IN: Value in WREG, patch offset MB_ADRRL, patch number in MB_PATCH
;;  OUT: increments address
;; --------------------------------------------------------------------------
MB64_BANK_PatchWrite
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_PatchWriteInt

#if DEFAULT_PATCHMODE == 0
	;; branch also if not in patchmode
	rgoto	MB64_BANK_PatchWriteInt
#else

MB64_BANK_PatchWriteBS
	rcall	MB64_BANK_Calc_PatchBSAddr	; calculate bankstick address
	movf	MIOS_PARAMETER3, W
	call	MIOS_BANKSTICK_Write		; write content
	;; reset core if bankstick not available anymore (just for debugging)
	btfss	 MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	reset
	bz	MB64_BANK_PatchWriteBS_End		; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

MB64_BANK_PatchWriteBS_End
	rgoto	MB64_BANK_PatchWrite_End

#endif

MB64_BANK_PatchWriteInt
	movff	MB_ADDRL, EEADR			; MB_ADDRL -> EEPROM address
	movlw	0x50		; (no patchname)
	cpfslt	EEADR, ACCESS
	rgoto MB64_BANK_PatchWrite_End
	movf	MIOS_PARAMETER3, W
	call	MIOS_EEPROM_Write		; write to EEPROM

MB64_BANK_PatchWrite_End
	;; increment 8-bit address
	incf	MB_ADDRL, F
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to Flash, EEPROM or BankStick
;;  IN: Values in FSR1 buffer, 16bit address offset in MB_ADDR[LH]
;;  OUT: increments address
;; --------------------------------------------------------------------------
MB64_BANK_PatchWritePage
	;; align MB_ADDRL
	movlw	0xc0
	andwf	MB_ADDRL, F

	;; branch to internal bank if bankstick is not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_PatchWritePageInt

#if DEFAULT_PATCHMODE == 0
	;; branch also if not in patchmode
	rgoto	MB64_BANK_PatchWritePageInt
#else

MB64_BANK_PatchWritePageBS			; BankStick write:
	rcall	MB64_BANK_Calc_PatchBSAddr	; calculate bankstick address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	;; reset core if bankstick not available anymore (just for debugging)
	btfss	 MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	reset
	bz	MB64_BANK_PatchWritePage_End	; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE
	rgoto	MB64_BANK_PatchWritePage_End

#endif

MB64_BANK_PatchWritePageInt
	movff	MB_ADDRL, EEADR			; MB_ADDRL -> EEPROM address
	movlw	0x40		; (no buttons, no patchname via page write)
	cpfslt	EEADR, ACCESS
	rgoto MB64_BANK_PatchWritePage_End
	call	MIOS_EEPROM_WritePage		; write to EEPROM

MB64_BANK_PatchWritePage_End
	;; increment 16bit MB address
	movlw	0x40
	addwf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return


;; --------------------------------------------------------------------------
;;  This function calculates the bankstick address depending on MB_ADDR[LH]
;;  and MB_BANK
;;  IN:  16bit address in MB_ADDR[LH]
;;  OUT: 16bit address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MB64_BANK_Calc_BankStickAddr
#if DEFAULT_BANKSTICK_CS == -1
#if DEFAULT_PATCHMODE == 0
	;; not in patchmode: bankstick select: MB_BANK >> 4
	swapf	MB_BANK, W
#else
	;; in patcnmode: bankstick select: MB_BANK
	movf	MB_BANK, W
#endif
	andlw	0x07
	call	MIOS_BANKSTICK_CtrlSet
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	movlw	DEFAULT_BANKSTICK_CS
	call	MIOS_BANKSTICK_CtrlSet
#endif
#endif

	;; the low byte is the same like MB_ADDRL
	movff	MB_ADDRL, MIOS_PARAMETER1
#if DEFAULT_PATCHMODE == 0
	;; not in patchmode: high byte: (MB_BANK&f << 3) | MB_ADDRH
	swapf	MB_BANK, W
	rrf	WREG, W
	andlw	0x78
	iorwf	MB_ADDRH, W
#else
	;; in patchmode: high byte: MB_ADDRH
	movf	MB_ADDRH, W
#endif
	movwf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------
;;  This function calculates the bankstick address to a patch depending on
;;  MB_ADDRL and MB_PATCH
;;  IN:  8bit address in MB_ADDRL
;;  OUT: 16bit address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MB64_BANK_Calc_PatchBSAddr
	;; address ranges:
	;; 0x0000-0x0fff: Bank Configuration
	;; 0x1000-0x2fff: 128 * 64 Knob Values
	;; 0x3000-0x37ff: 128 * 16*4 button values
	;; 0x3800-0x3fff: 128 * 16 characters
	;; 0x4000-0x7ffd: reserved
	;; 0x7ffe-0x7fff: magic bytes for patches
#if DEFAULT_BANKSTICK_CS == -1
	;; bankstick select: MB_BANK & 0x07
	movf	MB_BANK, W
	andlw	0x07
	call	MIOS_BANKSTICK_CtrlSet
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	movlw	DEFAULT_BANKSTICK_CS
	call	MIOS_BANKSTICK_CtrlSet
#endif
#endif
	;; branch depending on knob/button/name range
	BRA_IFSET MB_ADDRL, 6, ACCESS, MB64_BANK_Calc_PatchBSAddr_BN
MB64_BANK_Calc_PatchBSAddr_K
	;; knob: address = 0x1000 + MB_PATCH * 0x40
	movf	MB_PATCH, W
	mullw	0x40
	movf	MB_ADDRL, W
	andlw	0x3f
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movf	PRODH, W
	addlw	0x10
	movwf	MIOS_PARAMETER2
	return

MB64_BANK_Calc_PatchBSAddr_BN
	;; button: address = 0x3000 + MB_PATCH * 0x10
	;; name:   address = 0x3800 + MB_PATCH * 0x10
	movf	MB_PATCH, W
	mullw	0x10
	movf	MB_ADDRL, W
	andlw	0x0f
	addwf	PRODL, W
	movwf	MIOS_PARAMETER1
	movf	PRODH, W
	addlw	0x30
	btfsc	MB_ADDRL, 4
	addlw 0x08
	movwf	MIOS_PARAMETER2
	return


;; --------------------------------------------------------------------------
;;  This function calculates the flash address depending on MB_ADDR[LH]
;;  IN:  16bit address in MB_ADDR[LH]
;;  OUT: 16bit address in TBLPTR[LH]
;; --------------------------------------------------------------------------
MB64_BANK_Calc_FlashAddr
	;; the low byte is the same like MB_ADDRL
	movff	MB_ADDRL, TBLPTRL
	;; high byte: 
	;; lower sector (0x000-0x1ff): MB64_Presets_FLASH_Lower + MB_ADDRH
	;; upper sector (0x400-0x7ff): MB64_Presets_FLASH_Upper + MB_ADDRH
	movf	MB_ADDRH, W
	andlw	0xfe
	bz	MB64_BANK_Calc_FlashAddr_Lower
MB64_BANK_Calc_FlashAddr_Upper
	movf	MB_ADDRH, W
	addlw	(MB64_Presets_FLASH_Upper >> 8) - 0x04
	movwf	TBLPTRH
	return
MB64_BANK_Calc_FlashAddr_Lower
	movf	MB_ADDRH, W
	addlw	(MB64_Presets_FLASH_Lower >> 8)
	movwf	TBLPTRH
	return

;; --------------------------------------------------------------------------
;;  This function calculates the EEPROM address depending on MB_ADDR[LH]
;;  IN:  16bit address in MB_ADDR[LH]
;;  OUT: 16bit address in EEADR
;; --------------------------------------------------------------------------
MB64_BANK_Calc_EEPROMAddr
	;; EEPROM address = MB_ADDRL
	movff	MB_ADDRL, EEADR
	return

;; --------------------------------------------------------------------------
;;  Use this function when changing to another bank to save the current and
;;  to restore the new banksettings
;;  IN: new bank in WREG
;; --------------------------------------------------------------------------
MB64_BANK_Change
#if DEFAULT_PATCHMODE == 0
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 16 banks maximum
	andlw	0x0f
#else
	;; 128 banks maximum
	andlw	0x7f
#endif
#else
#if DEFAULT_BANKSTICK_CS >= 0 && DEFAULT_BANKSTICK_CS <= 7
	;; 1 bank maximum
	andlw	0x00
#else
	;; 8 banks maximum
	andlw	0x07
#endif
#endif

	;; temporary save bank in MB_NEW_BANK
	movwf	MB_NEW_BANK

	;; if auto-snapshot function set, save current pot/button settings
	SET_BSR	MB64_BASE
	CALL_IFSET MB64_CFG1, MB64_CFG1_AUTO_SNAPSHOT, BANKED, MB64_DUMP_StorePatch

	;; now move new bank number to MB_BANK
	movff	MB_NEW_BANK, MB_BANK
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK	; check if bankstick for this bank is available
	rcall	MB64_BANK_CheckStick
	call	MB64_DUMP_RestoreAll

	;; request re-init of page so that the new bankname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; send a program change of user flag is set
	SET_BSR	MB64_BASE
	BRA_IFCLR MB64_CFG1, MB64_CFG1_SEND_PC_ON_BCHANGE, BANKED, MB64_BANK_Change_End
MB64_BANK_Change_SendPC
	movlw	0xc0 | ((DEFAULT_BANK_PRG_CHNG_CHANNEL-1) & 0xf)
	call	MIOS_MIDI_TxBufferPut
	movf	MB_BANK, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
MB64_BANK_Change_End

	;; if auto-snapshot function set, restore new pot/button settings
	SET_BSR	MB64_BASE
	CALL_IFSET MB64_CFG1, MB64_CFG1_AUTO_SNAPSHOT, BANKED, MB64_PATCH_Send
	return

	
;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear MB_STAT.MB_STAT_USE_BANKSTICK
;;  if BankStick not available
;; --------------------------------------------------------------------------
MB64_BANK_CheckStick
	;; save BankStick status for later in TMP1
	movff	MB_STAT, TMP1

	;; try to read from BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	rcall	MB64_BANK_Calc_BankStickAddr
	call	MIOS_BANKSTICK_Read

	;; save current status of USE_BANKSTICK again - we use it to determine changes
	movff	MB_STAT, TMP2

	;; set the USE_BANKSTICK and BANKSTICK_AVAILABLE flag if EEPROM responded
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK

	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

	;; check if flag has been changed
	movf	MB_STAT, W			; get new status
	xorwf	TMP2, W				; compare with status last saved in TMP2
	bz	MB64_BANK_CheckStickEnd		; no change

	;; flag changed!
	GOTO_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_CheckStick_Ext
	
	;; --> Internal Bank
MB64_BANK_CheckStick_Int
MB64_BANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	rgoto	MB64_BANK_CheckStick_Reload		; branch to reload routine

	;; --> External Bank
MB64_BANK_CheckStick_Ext
	;; don't print a message if bankstick already was active before checking (status saved in TMP1)
	BRA_IFSET TMP1, MB_STAT_BANKSTICK_AVAILABLE, ACCESS, MB64_BANK_CheckStrick_Ext_NoMsg
	TABLE_ADDR TEXT_EXTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	movlw	255				; wait some ms to get a stable status
	call	MIOS_Delay			
MB64_BANK_CheckStrick_Ext_NoMsg
#if DEFAULT_PATCHMODE == 0
	;; first check if the magic byte location for patches contains values with 7th bit set
	;; if so, delete these entries, so that the patch space will be formated again if
	;; PATCHMODE is set to 1 later
	rcall	MB64_BANK_SetPatchMagicAddr
	call	MIOS_BANKSTICK_Read
	BRA_IFSET WREG, 7, ACCESS, MB64_BANK_CheckStick_Ext_PClr
	call	MIOS_BANKSTICK_Read
	BRA_IFSET WREG, 7, ACCESS, MB64_BANK_CheckStick_Ext_PClr
	rgoto	MB64_BANK_CheckStick_Ext_PEnd

MB64_BANK_CheckStick_Ext_PClr
	;; clear two patch magic bytes
	rcall	MB64_BANK_SetPatchMagicAddr
	movlw	0x00
	call	MIOS_BANKSTICK_Write
	movlw	0x00
	call	MIOS_BANKSTICK_Write
MB64_BANK_CheckStick_Ext_PEnd

#else
	;; first check for patch memory --- directly address the bankstick!
	rcall	MB64_BANK_SetPatchMagicAddr
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_PATCH_MAGIC0
	bnz	MB64_BANK_CheckStick_Ext_PUnF
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_PATCH_MAGIC1
	bz	MB64_BANK_CheckStick_Ext_Bank
	
	;; unformatted patches detected
MB64_BANK_CheckStick_Ext_PUnF
	rcall	MB64_BANK_FormatPatches

	;; write magic word 
	rcall	MB64_BANK_SetPatchMagicAddr
	movlw	BANKSTICK_PATCH_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_PATCH_MAGIC1
	call	MIOS_BANKSTICK_Write
#endif

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
MB64_BANK_CheckStick_Ext_Bank
	;; try to read Magic Numbers from BankStick
	call	MB64_ADDR_MagicNumbers
	rcall	MB64_BANK_Calc_BankStickAddr
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	MB64_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bz	MB64_BANK_CheckStick_Reload
MB64_BANK_CheckStick_Unformatted

	;; format BankStick immediately
	rgoto	MB64_BANK_FormatBankNow

MB64_BANK_CheckStick_Reload
MB64_BANK_FormatBank_Reload
	;; restore dump
	call	MB64_DUMP_RestoreAll
	;; request re-init of page so that the new bankname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	
MB64_BANK_CheckStickEnd
	return


;; --------------------------------------------------------------------------
;;  Format Stick: copy the internal bank into the currently selected BankStick Bank
;; --------------------------------------------------------------------------
MB64_BANK_FormatBank
	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_FormatBankNow

	TABLE_ADDR TEXT_FORMATNOT_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage


MB64_BANK_FormatBankNow
	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; format current bank by copying the internal bank to the external bank
	;; print bank number
	movlw	0x47
	call	MIOS_LCD_CursorSet
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD3

	movf	MB_BANK, W
	rcall	MB64_BANK_CopyFromInternal

	rgoto	MB64_BANK_FormatBank_Reload


;; --------------------------------------------------------------------------
;;  Copy function
;;  IN: source bank in MB_BANK
;;      target bank in WREG
;;      TMP3[0] == 0: copy from internal (-> use MB64_BANK_CopyFromInternal)
;;      TMP3[0] == 1: copy from external (-> use MB64_BANK_CopyFromExternal)
;;  USES: TMP1
;; --------------------------------------------------------------------------
MB64_BANK_CopyFromInternal
	bcf	TMP3, 0		; copy from internal: TMP3[0] == cleared
	rgoto	MB64_BANK_Copy

MB64_BANK_CopyFromExternal
	bsf	TMP3, 0		; copy from external: TMP3[0] == set
	;; 	rgoto	MB64_BANK_Copy

MB64_BANK_Copy
	;; store current bank in TMP1
	movff	MB_BANK, TMP1
	;; store target bank in TMP2
	movwf	TMP2

	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_CopyNow

MB64_BANK_CopyNot
MB64_BANK_CopyPatchNot
	TABLE_ADDR TEXT_COPYNOT_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage

MB64_BANK_CopyNow
	;; copy 0x800 bytes
	clrf	MB_ADDRL
	clrf	MB_ADDRH
MB64_BANK_CopyLoop
	clrwdt			; feed watchdog

	;; read page from source bank
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK
	btfss	TMP3, 0
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK
	movff	TMP1, MB_BANK
	lfsr	FSR1, MB64_DATA_BUFFER
	call	MB64_BANK_ReadPage

	;; fixing address
	movlw	-0x40
	addwf	MB_ADDRL, F
	movlw	0xff
	addwfc	MB_ADDRH, F

	;; EXTRA: if MB_ADDRH == 0x02 and MB_ADDRL == MB64_Presets_BANKNAME & 0xc0, exchange bankname
	movf	MB_ADDRH, W
	xorlw	0x02
	bnz	MB64_BANK_Copy_NoBName
	movf	MB_ADDRL, W
	xorlw	MB64_Presets_BANKNAME & 0xc0
	bnz	MB64_BANK_Copy_NoBName
	lfsr	FSR0, MB64_DATA_BUFFER
	movlw	MB64_Presets_BANKNAME & 0x3f
	addwf	FSR0L, F
	TABLE_ADDR MB64_BANK_FORMAT_BNAME
	clrf	TMP4
MB64_BANK_Copy_BNameLoop
	tblrd*+
	movff	TABLAT, POSTINC0
	incf	TMP4, F
	BRA_IFCLR TMP4, 4, ACCESS, MB64_BANK_Copy_BNameLoop

	;; copy external bank number to buffer
	movf	POSTDEC0, W
	movf	POSTDEC0, W
	clrf	MIOS_PARAMETER1
	incf	TMP2, W		; (target bank)
	call	MIOS_HLP_Dec2BCD; convert to BCD
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	addlw	'0'
	movwf	POSTINC0
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	addlw	'0'
	movwf	POSTINC0	
	
MB64_BANK_Copy_NoBName

	;; writing page to target bank
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK
	movff	TMP2, MB_BANK
	lfsr	FSR1, MB64_DATA_BUFFER
	call	MB64_BANK_WritePage

	;; loop until end address reached
	movlw	0x08-1
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MB64_BANK_CopyLoop

	;; change to old bank
	movf	TMP1, W
	rgoto	MB64_BANK_Change
	

;; --------------------------------------------------------------------------
;;  Format Patch: zero the whole patch and store in BankStick
;; --------------------------------------------------------------------------
MB64_BANK_FormatPatch
	clrf	MB_ADDRL
MB64_BANK_FormatPatchLoop
	;; pot section: 0x7f
	;; button section: 0x7f
	;; bankname: 0x00
	movlw	0x7f
	BRA_IFCLR MB_ADDRL, 6, ACCESS, MB64_BANK_FormatPatchLoop7F
	BRA_IFCLR MB_ADDRL, 4, ACCESS, MB64_BANK_FormatPatchLoop7F
	movlw	0x00
MB64_BANK_FormatPatchLoop7F
	call	MB64_BANK_PatchWrite
	movlw	64 + 16 + 16 - 1	; patch size
	cpfsgt	MB_ADDRL, ACCESS
	rgoto MB64_BANK_FormatPatchLoop

	TABLE_ADDR TEXT_FORMATPATCH_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	movlw	0x49
	call	MIOS_LCD_CursorSet
	movf	MB_PATCH, W
	call	MB64_LCD_PrnPatch

	;; restore the patch and exit
	goto	MB64_DUMP_RestorePatch


;; --------------------------------------------------------------------------
;;  Format Patches: zeroes all 128 patches
;; --------------------------------------------------------------------------
MB64_BANK_FormatPatches
	;; format all 128 patches
	clrf	MB_PATCH
MB64_BANK_FormatPatches_Loop
	clrwdt			; feed the watchdog
	rcall	MB64_BANK_FormatPatch
	incf	MB_PATCH, F
	BRA_IFCLR MB_PATCH, 7, ACCESS, MB64_BANK_FormatPatches_Loop

	;; restore first patch and exit
	clrf	MB_PATCH
	goto	MB64_DUMP_RestorePatch

;; --------------------------------------------------------------------------
;;  Copy function for patch
;;  IN: source bank in MB_PATCH
;;      target bank in WREG
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64_BANK_CopyPatch
	;; store current patch in TMP1
	movff	MB_PATCH, TMP1
	;; store target bank in TMP2
	movwf	TMP2

	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_BANK_CopyPatchNot

	;; copy 144 bytes
	clrf	MB_ADDRL
MB64_BANK_CopyPatchLoop
	;; read byte from source patch
	movff	TMP1, MB_PATCH
	call	MB64_BANK_PatchRead

	;; fixing address
	decf	MB_ADDRL, F

	movff	TMP2, MB_PATCH
	call	MB64_BANK_PatchWrite

	movlw	64 + 16 + 16 - 1
	cpfsgt	MB_ADDRL, ACCESS
	rgoto MB64_BANK_CopyPatchLoop

	;; change to old patch and exit
	movf	TMP1, W
	goto	MB64_PATCH_Change


;; --------------------------------------------------------------------------
;;  This function sets the magic address for bankstick:	0x7ffe
;; --------------------------------------------------------------------------
MB64_BANK_SetPatchMagicAddr
	movf	MB_BANK, W		; select bankstick
	andlw	0x07
	call	MIOS_BANKSTICK_CtrlSet

	movlw	0x7e			; select address
	movwf	MIOS_PARAMETER1
	movlw	0x7f
	movwf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------

TEXT_EXTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_EXTBANK_1   STRING 16, 0x40, "* from BankStick"

TEXT_INTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_INTBANK_1   STRING 16, 0x40, "* from Internal "
	
TEXT_FORMATBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATBANK_1 STRING 16, 0x40, "* Bank   0      "

TEXT_FORMATPATCH_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATPATCH_1 STRING 16, 0x40, "* Patch  xx     "

TEXT_FORMATNOT_0  STRING 16, 0x00, "* Internal Bank "
TEXT_FORMATNOT_1  STRING 16, 0x40, "* not formatable"

TEXT_COPYNOT_0  STRING 16, 0x00, "* Only internal "
TEXT_COPYNOT_1  STRING 16, 0x40, "* bank availabl."

MB64_BANK_FORMAT_BNAME
	db	"External Bank   "
	