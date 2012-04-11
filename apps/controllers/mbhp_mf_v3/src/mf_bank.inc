; $Id$
;
; MBHP_MF_NG
; Bank Load/Store Routines
;
; ==========================================================================
;
;  Copyright 2010 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Value in FSR1 buffer, address offset in EEADR
;;      patch number in MF_PATCH
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_BANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

MF_BANK_WritePageInternal
	;; write 64 bytes
	call	MIOS_EEPROM_WritePage		; write page to EEPROM

MF_BANK_WritePage_End
	SET_BSR	MF_BASE				; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Writes data into EEPROM or BankStick
;;  IN: address offset in EEADR
;;      patch number in MF_PATCH
;;      data value in WREG
;;  OUT: increments address
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_BANK_Write
MF_BANK_WriteInternal
	call	MIOS_EEPROM_Write
MF_BANK_Write_End
	SET_BSR	MF_BASE				; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR
;;      patch number in MF_PATCH
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_BANK_Read
MF_BANK_ReadInternal
	call	MIOS_EEPROM_Read		; read from EEPROM

MF_BANK_Read_End
	SET_BSR	MF_BASE				; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Reads 64 bytes from EEPROM or BankStick
;;  IN: pointer to 64 bytes buffer in FSR1, address in EEADR
;;  OUT: result in FSR1 buffer, increments address by 0x40
;; --------------------------------------------------------------------------
;; NOTE: MBHB_MF doesn't support BankStick, the appr. sections have been removed
MF_BANK_ReadPage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

MF_BANK_ReadPageInternal
	call	MIOS_EEPROM_ReadPage		; read page from EEPROM

MF_BANK_ReadPage_End
	SET_BSR	MF_BASE				; fix BSR after a MIOS routine has been called
	return

