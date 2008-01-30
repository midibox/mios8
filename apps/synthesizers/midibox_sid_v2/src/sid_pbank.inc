; $Id$
;
; MIDIbox SID
; Bank Load/Store Routines for Patches
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function sets a BankStick address
;;  IN: patch offset (0x000-0x1ff) in EEADR[H]
;;      patch number in SID_PATCH
;;      BankStick number in SID_BANK
;;  OUT: address in MIOS_PARAMETER[12]
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;; --------------------------------------------------------------------------
SID_PBANK_SetBankStickAddress
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	rlf	SID_PATCH, W			; derive high byte from patch and EEADRH[0]
	andlw	0xfe
	btfsc	EEADRH, 0
	iorlw 0x01
	movwf	MIOS_PARAMETER2			; copy patch to high-byte
	movf	SID_BANK, W			; select BankStick 0-7
	andlw	0x07
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet


;; --------------------------------------------------------------------------
;;  This function checks if the selected patch is valid
;;  IN: BankStick number in SID_BANK, patch number in SID_PATCH
;;  OUT: ZERO flag set if patch is valid
;;       1 if no BankStick, 2 if no Patch
;; --------------------------------------------------------------------------
SID_PBANK_CheckPatchValid
	;; check for ENS BankStick
#if DEFAULT_ENS_BS_CS >= 0 && DEFAULT_ENS_BS_CS <= 7
	movf	SID_BANK, W
	andlw	0x07
	xorlw	DEFAULT_ENS_BS_CS
 	bz	SID_PBANK_CheckPatchValid_Inv
#endif

	;; check if BankStick available
	movf	SID_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	movlw	0x01
	bz	SID_PBANK_CheckPatchValid_Inv

	;; check if patch valid (for 32k BankSticks, patches >= 64 not valid)
	BRA_IFCLR SID_PATCH, 6, ACCESS, SID_PBANK_CheckPatchValid_Ok
	movf	SID_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_SIZE, W
	movlw	0x02
	bz	SID_PBANK_CheckPatchValid_Inv

SID_PBANK_CheckPatchValid_Ok
	andlw	0x00
	return

SID_PBANK_CheckPatchValid_Inv
	iorlw	0xff		; fix ZERO flag
	return


;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Values in FSR1 buffer, address offset in EEADR[H]
;;      patch number in SID_PATCH
;;      BankStick number in SID_BANK
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SID_PBANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_PBANK_CheckPatchValid
	bnz	SID_PBANK_WritePageInternal
	movf	SID_PATCH, W
	bz	SID_PBANK_WritePageInternal

SID_PBANK_WritePageBankStick			; BankStick write:
	rcall	SID_PBANK_SetBankStickAddress
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	bz	SID_PBANK_WritePage_BS_End	; goto end if write successfull
	;; here we could do something if the bankstick was not available

SID_PBANK_WritePage_BS_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	movlw	0
	addwfc	EEADRH, F
	rgoto	SID_PBANK_WritePage_End

SID_PBANK_WritePageInternal
	;; write 64 bytes
	call	MIOS_EEPROM_WritePage		; write page to EEPROM
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SID_PBANK_WritePage_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR[H]
;;      patch number in SID_PATCH
;;      BankStick number in SID_BANK
;;  OUT: read byte in WREG, increments address
;; --------------------------------------------------------------------------
SID_PBANK_Read
	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_PBANK_CheckPatchValid
	bnz	SID_PBANK_ReadInternal
	movf	SID_PATCH, W
	bz	SID_PBANK_ReadInternal

SID_PBANK_ReadBankStick				; BankStick read:
	rcall	SID_PBANK_SetBankStickAddress
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	skpnz
	incf	EEADRH, F
	rgoto	SID_PBANK_Read_End
	
SID_PBANK_ReadInternal
	call	MIOS_EEPROM_Read		; read content
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SID_PBANK_Read_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Read page from EEPROM or BankStick
;;  IN: Values in FSR1 buffer, address offset in EEADR[H]
;;      patch number in SID_PATCH
;;      BankStick number in SID_BANK
;;  OUT: page in buffer, increments address
;; --------------------------------------------------------------------------
SID_PBANK_ReadPage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_PBANK_CheckPatchValid
	bnz	SID_PBANK_ReadPageInternal
	movf	SID_PATCH, W
	bz	SID_PBANK_ReadPageInternal

SID_PBANK_ReadPageBankStick			; BankStick read:
	rcall	SID_PBANK_SetBankStickAddress

	call	MIOS_BANKSTICK_ReadPage		; read page
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	movlw	0
	addwfc	EEADRH, F
	rgoto	SID_PBANK_ReadPage_End

SID_PBANK_ReadPageInternal
	call	MIOS_EEPROM_ReadPage		; read page
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SID_PBANK_ReadPage_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return


	
