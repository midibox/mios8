; $Id$
;
; MIDIO128
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
MIDIO_BANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

MIDIO_BANK_WriteInternal
	;; branch depending on address:
	;; 0x000-0x3ff: SectFL, internal flash memory
	;; 0x400-0x4ff: SectE, internal EEPROM memory
	;; 0x500-0x5ff: reserved, skip
	movlw	0x03
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_WriteInternal_SectFL
	movlw	0x04
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_WriteInternal_SectE
	rgoto	MIDIO_BANK_Write_End

MIDIO_BANK_WriteInternal_SectFL
MIDIO_BANK_WriteInternal_SectFU
	;; not implemented - use MIDIO_BANK_WritePage instead!
	rgoto	MIDIO_BANK_Write_End
	
MIDIO_BANK_WriteInternal_SectE
	rcall	MIDIO_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	;; don't write if address is 0xff (used to save the device ID)
	incf	EEADR, W
	bz	MIDIO_BANK_WriteInternal_NOk
	
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
MIDIO_BANK_WriteInternal_NOk

MIDIO_BANK_Write_End
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
MIDIO_BANK_WritePage
	;; align MB_ADDRL
	movlw	0xc0
	andwf	MB_ADDRL, F

MIDIO_BANK_WritePageInt
	;; branch depending on address:
	;; 0x000-0x3ff: SectFL, internal flash memory
	;; 0x400-0x4ff: SectE, internal EEPROM memory
	;; 0x500-0x5ff: reserved, skip
	movlw	0x03
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_WritePageInt_SectFL
	movlw	0x04
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_WritePageInt_SectE
	rgoto	MIDIO_BANK_WritePage_End

MIDIO_BANK_WritePageInt_SectFL
MIDIO_BANK_WritePageInt_SectFU
	rcall	MIDIO_BANK_Calc_FlashAddr	; calculate FLASH address
	;; buffer already prepared in FSR1
 	call	MIOS_FLASH_Write		; write page
	;; here we could add an exception handler
	rgoto	MIDIO_BANK_WritePage_End
	
MIDIO_BANK_WritePageInt_SectE
	rcall	MIDIO_BANK_Calc_EEPROMAddr	; calculate EEPROM address
MIDIO_BANK_WritePageInt_SectELp
	;; don't write if address is 0xff (used to save the device ID)
	incf	EEADR, W
	bnz	MIDIO_BANK_WritePageInt_SectE_Ok
	incf	EEADR, F
	rgoto	MIDIO_BANK_WritePageInt_SectENxt
MIDIO_BANK_WritePageInt_SectE_Ok
	movf	EEADR, W
	andlw	0x3f
	movf	PLUSW1, W
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
MIDIO_BANK_WritePageInt_SectENxt
	movf	EEADR, W
	andlw	0x3f
	bnz	MIDIO_BANK_WritePageInt_SectELp

MIDIO_BANK_WritePage_End
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
MIDIO_BANK_Read
MIDIO_BANK_ReadInternal
	;; branch depending on address:
	;; 0x000-0x3ff: SectFL, internal flash memory
	;; 0x400-0x4ff: SectE, internal EEPROM memory
	;; 0x500-0x5ff: reserved, read zero
	movlw	0x03
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_ReadInternal_SectFL
	movlw	0x04
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_ReadInternal_SectE
	movlw	0x00
	rgoto	MIDIO_BANK_Read_End

MIDIO_BANK_ReadInternal_SectFL
MIDIO_BANK_ReadInternal_SectFU
	rcall	MIDIO_BANK_Calc_FlashAddr	; calculate flash address
	tblrd*+					; read from table
	movf	TABLAT, W			; get entry
	rgoto	MIDIO_BANK_Read_End
	
MIDIO_BANK_ReadInternal_SectE
	rcall	MIDIO_BANK_Calc_EEPROMAddr	; calculate EEPROM address
	call	MIOS_EEPROM_Read		; read from EEPROM
	;; here we could add an error exception handler

MIDIO_BANK_Read_End
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
MIDIO_BANK_ReadPage
	;; align MB_ADDRL
	movlw	0xc0
	andwf	MB_ADDRL, F

MIDIO_BANK_ReadPageInt
	;; branch depending on address:
	;; 0x000-0x3ff: SectFL, internal flash memory
	;; 0x400-0x4ff: SectE, internal EEPROM memory
	;; 0x500-0x5ff: reserved, read zero
	movlw	0x03
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_ReadPageInt_SectFL
	movlw	0x04
	cpfsgt	MB_ADDRH, ACCESS
	rgoto MIDIO_BANK_ReadPageInt_SectFL
	rgoto	MIDIO_BANK_ReadPage_Reserved

MIDIO_BANK_ReadPageInt_SectFL
MIDIO_BANK_ReadPageInt_SectFU
	rcall	MIDIO_BANK_Calc_FlashAddr	; calculate flash address
	;; pointer already in FSR1
	call	MIOS_FLASH_Read			; reads 64 bytes
	rgoto	MIDIO_BANK_ReadPage_End

MIDIO_BANK_ReadPage_Reserved
	clrf	EEADR
MIDIO_BANK_ReadPage_ReservedLoop
	movf	EEADR, W
	clrf	PLUSW1
	incf	EEADR, F
	BRA_IFCLR EEADR, 6, ACCESS, MIDIO_BANK_ReadPage_ReservedLoop
	rgoto	MIDIO_BANK_ReadPage_End

MIDIO_BANK_ReadPageInt_SectE
	rcall	MIDIO_BANK_Calc_EEPROMAddr	; calculate EEPROM address
MIDIO_BANK_ReadPageInt_SectELoop
	call	MIOS_EEPROM_Read		; read from EEPROM
	movwf	MIOS_PARAMETER3
	decf	EEADR, W
	andlw	0x3f
	movff	MIOS_PARAMETER3, PLUSW1
	movf	EEADR, W
	andlw	0x3f
	bnz	MIDIO_BANK_ReadPageInt_SectELoop
MIDIO_BANK_ReadPage_End
	;; increment 16bit MB address
	movlw	0x40
	addwf	MB_ADDRL, F
	skpnz
	incf	MB_ADDRH, F
	return


;; --------------------------------------------------------------------------
;;  This function calculates the flash address depending on MB_ADDR[LH]
;;  IN:  16bit address in MB_ADDR[LH]
;;  OUT: 16bit address in TBLPTR[LH]
;; --------------------------------------------------------------------------
MIDIO_BANK_Calc_FlashAddr
	;; the low byte is the same like MB_ADDRL
	movff	MB_ADDRL, TBLPTRL
	;; high byte: 
	;; lower sector (0x000-0x3ff): MIDIO_Presets_FLASH_Lower + MB_ADDRH
	movf	MB_ADDRH, W
	addlw	(MIDIO_Presets_FLASH_Lower >> 8)
	movwf	TBLPTRH
	return

;; --------------------------------------------------------------------------
;;  This function calculates the EEPROM address depending on MB_ADDR[LH]
;;  IN:  16bit address in MB_ADDR[LH]
;;  OUT: 16bit address in EEADR
;; --------------------------------------------------------------------------
MIDIO_BANK_Calc_EEPROMAddr
	;; EEPROM address = MB_ADDRL
	movff	MB_ADDRL, EEADR
	return

