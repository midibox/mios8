; $Id$
;
; MIDIbox SID
; Bank Load/Store Routines for Ensembles
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
;;  IN: ensemble offset (0x00-0xff) in EEADR
;;      ensemble number in SID_ENSEMBLE
;;  OUT: address in MIOS_PARAMETER[12]
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;; --------------------------------------------------------------------------
SID_EBANK_SetBankStickAddress
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movf	SID_ENSEMBLE, W			; derive high byte from ensemble number
	andlw	0x7f
	movwf	MIOS_PARAMETER2			; copy ensemble number to high-byte
	movlw	(DEFAULT_ENS_BS_CS & 0x07) | 0x80 ; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet


;; --------------------------------------------------------------------------
;;  This function checks if the selected ensemble is valid
;;  IN:  ensemble number in SID_ENSEMBLE
;;  OUT: ZERO flag set if ensemble is valid
;;       1 if no BankStick, 2 if no Ensemble
;; --------------------------------------------------------------------------
SID_EBANK_CheckValid
	;; check for ENS BankStick
#if DEFAULT_ENS_BS_CS >= 0 && DEFAULT_ENS_BS_CS <= 7
	movlw	DEFAULT_ENS_BS_CS
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bnz	SID_EBANK_CheckValid_Ok
#endif
SID_EBANK_CheckValid_Inv
	iorlw	0xff		; fix ZERO flag
	return

SID_EBANK_CheckValid_Ok
	andlw	0x00
	return


;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Values in FSR1 buffer, address offset in EEADR
;;      ensemble number in SID_ENSEMBLE
;;  OUT: increments address
;; --------------------------------------------------------------------------
SID_EBANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal ensemble if bankstick is not available or E001 selected
	rcall	SID_EBANK_CheckValid
	bnz	SID_EBANK_WritePageInternal
	movf	SID_ENSEMBLE, W
	bz	SID_EBANK_WritePageInternal

SID_EBANK_WritePageBankStick			; BankStick write:
	rcall	SID_EBANK_SetBankStickAddress
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	bz	SID_EBANK_Write_BSP_End		; goto end if write successfull
	;; here we could do something of the bankstick was not available

SID_EBANK_Write_BSP_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	SID_EBANK_WritePage_End

SID_EBANK_WritePageInternal
	;; EEADRH fixed to page 2
	movlw	HIGH(EEPROM_ENSEMBLE)
	movwf	EEADRH
	;; write 64 bytes
	call	MIOS_EEPROM_WritePage		; write to EEPROM

SID_EBANK_WritePage_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR
;;      ensemble number in SID_ENSEMBLE
;;  OUT: read byte in WREG, increments address
;; --------------------------------------------------------------------------
SID_EBANK_Read
	;; branch to internal ensemble if bankstick is not available or E001 selected
	rcall	SID_EBANK_CheckValid
	bnz	SID_EBANK_ReadInternal
	movf	SID_ENSEMBLE, W
	bz	SID_EBANK_ReadInternal

SID_EBANK_ReadBankStick				; BankStick read:
	rcall	SID_EBANK_SetBankStickAddress
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	rgoto	SID_EBANK_Read_End
	
SID_EBANK_ReadInternal
	;; EEADRH fixed to page 2
	movlw	HIGH(EEPROM_ENSEMBLE)
	movwf	EEADRH
	call	MIOS_EEPROM_Read		; read content

SID_EBANK_Read_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Read page from EEPROM or BankStick
;;  IN: Values in FSR1 buffer, address offset in EEADR
;;      ensemble number in SID_ENSEMBLE
;;  OUT: page in buffer, increments address
;; --------------------------------------------------------------------------
SID_EBANK_ReadPage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal ensemble if bankstick is not available or E001 selected
	rcall	SID_EBANK_CheckValid
	bnz	SID_EBANK_ReadPageInternal
	movf	SID_ENSEMBLE, W
	bz	SID_EBANK_ReadPageInternal

SID_EBANK_ReadPageBankStick			; BankStick read:
	rcall	SID_EBANK_SetBankStickAddress

	call	MIOS_BANKSTICK_ReadPage		; read page
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	SID_EBANK_ReadPage_End

SID_EBANK_ReadPageInternal
	;; EEADRH fixed to page 2
	movlw	HIGH(EEPROM_ENSEMBLE)
	movwf	EEADRH
	call	MIOS_EEPROM_ReadPage		; read page

SID_EBANK_ReadPage_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return
