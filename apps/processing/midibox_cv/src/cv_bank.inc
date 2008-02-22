; $Id$
;
; MIDIbox CV
; Bank Load/Store Routines
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Value in FSR1 buffer, address offset in EEADR
;;      patch number in CV_PATCH
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
CV_BANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal patch if BankStick not available or patch is 0
	rcall	CV_BANK_CheckPatchValid
	bnz	CV_BANK_WritePageInternal
	movf	CV_PATCH, W
	bz	CV_BANK_WritePageInternal

CV_BANK_WritePageBankStick			; BankStick write:
	rcall	CV_BANK_SetBankStickAddress
	;; buffer already prepared in FSR1
	call	MIOS_BANKSTICK_WritePage	; write page
	bz	CV_BANK_WritePage_BSP_End	; goto end if write successfully
	;; here we could do something if the bankstick is not available

CV_BANK_WritePage_BSP_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	CV_BANK_WritePage_End

CV_BANK_WritePageInternal
	;; write 64 bytes
	call	MIOS_EEPROM_WritePage		; write page to EEPROM

CV_BANK_WritePage_End
	SET_BSR	CV_BASE				; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR
;;      patch number in CV_PATCH
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
CV_BANK_Read
	;; branch to internal patch if bankstick is not available or patch is 0
	rcall	CV_BANK_CheckPatchValid
	bnz	CV_BANK_ReadInternal
	movf	CV_PATCH, W
	bz	CV_BANK_ReadInternal

CV_BANK_ReadBankStick				; BankStick read:
	rcall	CV_BANK_SetBankStickAddress
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	rgoto	CV_BANK_Read_End

CV_BANK_ReadInternal
	call	MIOS_EEPROM_Read		; read from EEPROM

CV_BANK_Read_End
	SET_BSR	CV_BASE				; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Reads 64 bytes from EEPROM or BankStick
;;  IN: pointer to 64 bytes buffer in FSR1, address in EEADR
;;  OUT: result in FSR1 buffer, increments address by 0x40
;; --------------------------------------------------------------------------
CV_BANK_ReadPage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal patch if BankStick not available or patch is 0
	rcall	CV_BANK_CheckPatchValid
	bnz	CV_BANK_ReadPageInternal
	movf	CV_PATCH, W
	bz	CV_BANK_ReadPageInternal

CV_BANK_ReadPageBankStick			; BankStick read:
	rcall	CV_BANK_SetBankStickAddress
	call	MIOS_BANKSTICK_ReadPage		; read page
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	CV_BANK_ReadPage_End

CV_BANK_ReadPageInternal
	call	MIOS_EEPROM_ReadPage		; read page from EEPROM

CV_BANK_ReadPage_End
	SET_BSR	CV_BASE				; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  This function sets a BankStick address
;;  IN: patch offset (0x00-0xff) in EEADR
;;      patch number in CV_PATCH
;;  OUT: address in MIOS_PARAMETER[12]
;;       default BankStick always selected via MIOS_BANKSTICK_CtrlSet
;; --------------------------------------------------------------------------
CV_BANK_SetBankStickAddress
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movf	CV_PATCH, W			; derive high byte from patch
	andlw	0x7f
	movwf	MIOS_PARAMETER2			; copy patch to high-byte
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
#else
	movlw	0x00
#endif
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet


;; --------------------------------------------------------------------------
;;  This function checks if the selected patch is valid
;;  IN: patch number in CV_PATCH
;;  OUT: ZERO flag set if patch is valid
;;       1 if no BankStick, 2 if no Patch
;; --------------------------------------------------------------------------
CV_BANK_CheckPatchValid
	;; patch 0 always valid
	movf	CV_PATCH, W
	bz	CV_BANK_CheckPatchValid_Ok

	;; check if BankStick available
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
	call	MIOS_HLP_GetBitORMask
	andwf	CV_BANKSTICK_STATUS, W
	movlw	0x01
	bz	CV_BANK_CheckPatchValid_Inv
#else
	movlw	0x01
	rgoto	CV_BANK_CheckPatchValid_Inv
#endif

CV_BANK_CheckPatchValid_Ok
	andlw	0x00
	return

CV_BANK_CheckPatchValid_Inv
	iorlw	0xff		; fix ZERO flag
	return


;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear the appr. flag in
;;  CV_BANKSTICK_STATUS if BankStick not available
;; --------------------------------------------------------------------------
CV_BANK_CheckStick
	;; ignore if no BankStick selected
#if DEFAULT_BANKSTICK_NUMBER == 0
	return
#endif

	;; select "magic number" and try to read from BankStick
	rcall	CV_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	;; this sets the MIOS_BOX_STAT_BS_AVAILABLE flag

	;; save old BankStick status in TMP1
	movff	CV_BANKSTICK_STATUS, TMP1

	;; modify the available flag of current bankstick
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, CV_BANK_CheckStick_AccPassed
CV_BANK_CheckStick_AccFailed
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
#else
	movlw	0x00
#endif
	call	MIOS_HLP_GetBitANDMask
	andwf	CV_BANKSTICK_STATUS, F
	rgoto	CV_BANK_CheckStick_AccCont
CV_BANK_CheckStick_AccPassed
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
#else
	movlw	0x00
#endif
	call	MIOS_HLP_GetBitORMask
	iorwf	CV_BANKSTICK_STATUS, F
CV_BANK_CheckStick_AccCont

	;; check if flag has been changed
	movf	CV_BANKSTICK_STATUS, W
	xorwf	TMP1, W
	bz	CV_BANK_CheckStick_End		; no change

	;; flag changed - branch depending on available status
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
#else
	movlw	0x00
#endif
	call	MIOS_HLP_GetBitORMask
	andwf	CV_BANKSTICK_STATUS, W
	bnz	CV_BANK_CheckStick_Ext


	;; ------------------------------------------------------------------
	;; --> Internal Patch
CV_BANK_CheckStick_Int
CV_BANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; change to patch 0
	clrf	CV_PATCH
	rcall	CV_DUMP_RestoreAll
	rgoto	CV_BANK_CheckStick_End


	;; ------------------------------------------------------------------
	;; --> External Patch
CV_BANK_CheckStick_Ext
	TABLE_ADDR TEXT_EXTBANK_0		; print message
 	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	clrwdt					; feed watchdog
	movlw	200				; wait some ms to get a stable status
	call	MIOS_Delay			

	rcall	CV_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	CV_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bnz	CV_BANK_CheckStick_Unformatted

	;; stick formatted - fine!
	rgoto	CV_BANK_CheckStick_End

	;; unformatted patches detected
CV_BANK_CheckStick_Unformatted
	rcall	CV_BANK_FormatPatches

	;; write magic word 
	rcall	CV_BANK_CheckStick_SelectMagic
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
	call	MIOS_BANKSTICK_Write

CV_BANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;; selects address of magic number (0x0000)
;; --------------------------------------------------------------------------
CV_BANK_CheckStick_SelectMagic
	;; select BankStick
#if DEFAULT_BANKSTICK_NUMBER
	movlw	(DEFAULT_BANKSTICK_NUMBER-1) & 0x7
#else
	movlw	0x00
#endif
	call	MIOS_BANKSTICK_CtrlSet

	;; select address of magic number
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return


;; --------------------------------------------------------------------------
;;  Format Patch: store EEPROM content into selected patch
;; --------------------------------------------------------------------------
CV_BANK_FormatPatch
	;; print message
	TABLE_ADDR TEXT_FORMATPATCH_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print patch number
	movlw	0x40 + 8
	call	MIOS_LCD_CursorSet
	call	CV_LCD_PrintPatchNumber

	;; copy EEPROM content into BankStick patch
	clrf	EEADR
CV_BANK_FormatPatch_Loop
	lfsr	FSR1, CV_CFG0	; always use CV_CFG0 as buffer - will be loaded with patch after formatting
	call	MIOS_EEPROM_ReadPage
	
	movlw	-0x40
	addwf	EEADR, F

	rcall	CV_BANK_WritePage

	;; EEADR has been incremented by 0x40 -- loop 4 times
	movf	EEADR, W
	bnz	CV_BANK_FormatPatch_Loop

	;; restore the patch and exit
	goto	CV_DUMP_RestoreAll


;; --------------------------------------------------------------------------
;;  Format Patches: zeroes all 128 patches
;; --------------------------------------------------------------------------
CV_BANK_FormatPatches
	;; format patch 1-128 (EEPROM patch not formatted!)
	movlw	0x01
	movwf	CV_PATCH
CV_BANK_FormatPatches_Loop
	clrwdt			; feed the watchdog
	rcall	CV_BANK_FormatPatch
	incf	CV_PATCH, F
	BRA_IFCLR CV_PATCH, 7, ACCESS, CV_BANK_FormatPatches_Loop

	;; restore first patch and exit
	clrf	CV_PATCH
	goto	CV_DUMP_RestoreAll

;; --------------------------------------------------------------------------

TEXT_EXTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_EXTBANK_1   STRING 16, 0x40, "* from BankStick"

TEXT_INTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_INTBANK_1   STRING 16, 0x40, "* from Internal "
	
TEXT_FORMATPATCH_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATPATCH_1 STRING 16, 0x40, "* Patch xxx     "
