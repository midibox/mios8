; $Id: iic_eeprom8.inc 415 2008-07-29 22:06:15Z tk $
;
;  Driver for reading/writing IIC EEPROMs with 8bit address range
;  (for accessing EEPROMs with 16bit address range, use the common
;   MIOS_BANKSTICK_* routines)
;
; ==========================================================================
;
;  Copyright 2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_EEPROM8_Write
;;  C_DECLARATION: unsigned char IIC_EEPROM8_Write(unsigned char addr, unsigned char value)
;;  DESCRIPTION: writes a byte into IIC EEPROM with 8bit address range<BR>
;;  write access will be skipped if content is equal to the byte which 
;;  should be written<BR>
;;  Returned Error Status:<BR>
;;     IIC_EEPROM8_ERROR_NONE (0x00): no error<BR>
;;     IIC_EEPROM8_ERROR_VERIFY (0x01): byte mismatch (write failed) -- only set if verify enabled<BR>
;;     IIC_EEPROM8_ERROR_NO_DEVICE (0x02): EEPROM not available
;;  IN:	  byte in WREG, address in MIOS_PARAMETER1 (0x00-0xff)
;;  C_IN: byte in <value>, address in <addr> (0x00-0xff)
;;  OUT:  error status in WREG
;;        MIOS_PARAMETER1 will be incremented
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] cleared if write failed
;;  C_OUT: error status
;;        MIOS_BOX_STAT.BS_AVAILABLE cleared if write failed
;;  USES: BSR, MIOS_PARAMETER1 and MIOS_PARAMETER2 as temporary variable (!)
;;  EXAMPLE:
;;
;;	;; write 0x47 0x11 to address 0x20-0x21 of the IIC EEPROM
;;	movlw	0x20			; store address in
;;	movwf	MIOS_PARAMETER1		; MIOS_PARAMETER1
;;	movlw	0x47			; write 0x47 to 0x20
;;	call	IIC_EEPROM8_Write	; (address will be incremented
;;					; after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;	movlw	0x11			; write 0x11 to 0x21
;;	call	IIC_EEPROM8_Write	; (address will be incremented
;;					;  after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;
;;  C_EXAMPLE:
;;
;;	unsigned char error = 0;
;;
;;	// write 0x47 0x11 to address 0x20-0x21 of the IIC EEPROM
;;	error |= IIC_EEPROM8_Write(0x20, 0x47);
;;	error |= IIC_EEPROM8_Write(0x21, 0x11);
;;	
;;	if( error ) {
;;	  // here you could do some error handling
;;	}
;;
;; --------------------------------------------------------------------------
IIC_EEPROM8_Write
	movwf	MIOS_PARAMETER2	; store byte which should be written into MIOS_PARAMETER2
	;; (TK: I don't really like to do this, because this is incompatible compared to MIOS_BANKSTICK*
	;;      so long I don't know a better method to avoid even more incompatibilities (e.g. using PRODL or TMP1)

	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, IIC_EEPROM8_Write_NoVerifyR

	rcall	IIC_EEPROM8_Read		; don't write if value is equal
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, IIC_EEPROM8_Write_NA
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto IIC_EEPROM8_WriteNow
	movlw	0x00
	rgoto	IIC_EEPROM8_Write_End

IIC_EEPROM8_WriteNow
	decf	MIOS_PARAMETER1, F		; decrement MIOS_PARAMETER1

IIC_EEPROM8_Write_NoVerifyR
	call	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	call	MIOS_IIC_ByteSend
	movf	MIOS_PARAMETER1, W	; send low address
	call	MIOS_IIC_ByteSend

	incf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER1

	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, IIC_EEPROM8_Write_NA

	movf	MIOS_PARAMETER2, W	; send byte which should be written into EEPROM
	call	MIOS_IIC_ByteSend
	
	call	MIOS_IIC_Stop		; stop IIC

	;; wait until write cycle is finished
IIC_EEPROM8_Write_Poll
	;; memo: time out counter should be inserted here!
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE

	call	MIOS_IIC_Start
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop		; stop IIC

	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, IIC_EEPROM8_Write_Poll


	movlw	0x00
	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, IIC_EEPROM8_Write_NoVerify2

	decf	MIOS_PARAMETER1, F		; decrement MIOS_PARAMETER1

	rcall	IIC_EEPROM8_Read		; read address again
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, IIC_EEPROM8_Write_NA
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto IIC_EEPROM8_Write_Error
	movlw	0x00				; error status = OK
	rgoto	IIC_EEPROM8_Write_End
IIC_EEPROM8_Write_NA
	movlw	0x02				; error status = not available
	rgoto	IIC_EEPROM8_Write_End
IIC_EEPROM8_Write_Error
	movlw	0x01				; error status = failed
	;; 	rgoto	IIC_EEPROM8_Write_End
IIC_EEPROM8_Write_End
IIC_EEPROM8_Write_NoVerify2
	andlw	0xff				; update STATUS register
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_EEPROM8_Read
;;  C_DECLARATION: unsigned char IIC_EEPROM8_Read(unsigned int addr)
;;  DESCRIPTION: reads a byte from IIC EEPROM with 8bit address
;;  IN:	  address in MIOS_PARAMETER1 (0x00-0xff)
;;  C_IN: address in <addr> (0x00-0xff)
;;  OUT:  EEPROM content in WREG
;;        MIOS_PARAMETER1 will be incremented
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] is zero if read failed
;;  C_OUT:  EEPROM content
;;        MIOS_BOX_STAT.BS_AVAILABLE cleared if read failed
;;  USES: BSR, MIOS_PARAMETER1 and MIOS_PARAMETER3 as temporary variable (!)
;;  EXAMPLE:
;;
;;	;; load a byte from address 0xff (the last in a 256 byte EEPROM)
;;	movlw	0xff			; initialize pointer to EEPROM
;;					; address
;;	movwf	MIOS_PARAMETER1		; -> 0xff
;;	call	IIC_EEPROM8_Read	; load byte from EEPROM
;;					; branch to error exception
;;					; routine if neccesary
;;	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, EEPROMNotAvl
;;	
;;  C_EXAMPLE:
;;
;;	// load a byte from address 0xff (the last in a 256 byte EEPROM)
;;	value = IIC_EEPROM8_Read(0xff);
;;	
;;	if( !MIOS_BOX_STAT.BS_AVAILABLE ) {
;;	  // here you could do some error handling
;;	}
;;	
;; --------------------------------------------------------------------------
IIC_EEPROM8_Read
	call	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	call	MIOS_IIC_ByteSend
	movf	MIOS_PARAMETER1, W	; send low address
	call	MIOS_IIC_ByteSend

	call	MIOS_IIC_Start		; start IIC again
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa1
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_ByteReceive	; read byte #0
	movwf	MIOS_PARAMETER3		; temporary store result in MIOS_PARAMETER3
	;; (TK: I don't really like to do this, because this is incompatible compared to MIOS_BANKSTICK*
	;;      so long I don't know a better method to avoid even more incompatibilities (e.g. using PRODL or TMP1)

	call	MIOS_IIC_NakSend
	call	MIOS_IIC_Stop		; stop IIC

	incf	MIOS_PARAMETER1, F	; increment MIOS_PARAMETER1

	movf	MIOS_PARAMETER3, W	; get read word

	return
