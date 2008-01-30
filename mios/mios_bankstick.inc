; $Id$
;
; MIOS BankStick routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

#define MIOS_BANKSTICK_16BIT 1

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_Write
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_Write(unsigned int addr, unsigned char value)
;;  DESCRIPTION: writes a byte into BankStick. <BR>
;;  If verify mode has been enabled with MIOS_BANKSTICK_CtrlSet, write access will 
;;  be skipped if content is equal to the byte which should be written<BR>
;;  Returned Error Status:<BR>
;;     0x00: no error<BR>
;;     0x01: byte mismatch (write failed) -- only set if verify enabled<BR>
;;     0x02: BankStick not available
;;  IN:	  byte in WREG, address in MIOS_PARAMETER[12] (0x0000-0xffff)
;;  C_IN: byte in <value>, address in <addr> (0x0000-0xffff)
;;  OUT:  error status in WREG
;;        MIOS_PARAMETER[12] will be incremented
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] cleared if write failed
;;  C_OUT: error status
;;        MIOS_BOX_STAT.BS_AVAILABLE cleared if write failed
;;  USES: BSR, MIOS_PARAMETER[12]
;;  EXAMPLE:
;;
;;	;; write 0x47 0x11 to address 0x3000-0x3001 of the BankStick
;;	movlw	0x00			; store low-byte of address in 
;;	movwf	MIOS_PARAMETER1		; MIOS_PARAMETER1
;;	movlw	0x30			; store high-byte of address in 
;;	movwf	MIOS_PARAMETER2		; MIOS_PARAMETER2
;;	movlw	0x47			; write 0x47 to 0x3000
;;	call	MIOS_BANKSTICK_Write	; (address will be incremented
;;					; after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;	movlw	0x11			; write 0x11 to 0x3001
;;	call	MIOS_BANKSTICK_Write	; (address will be incremented
;;					;  after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;
;;  C_EXAMPLE:
;;
;;	unsigned char error = 0;
;;
;;	// write 0x47 0x11 to address 0x3000-0x3001 of the BankStick
;;	error |= MIOS_BANKSTICK_Write(0x3000, 0x47);
;;	error |= MIOS_BANKSTICK_Write(0x3001, 0x11);
;;	
;;	if( error ) {
;;	  // here you could do some error handling
;;	}
;;
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_Write
	SET_BSR	MIOS_TMP2
	movwf	MIOS_TMP2, BANKED

	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, MIOS_BANKSTICK_Write_NoVerifyR

	rcall	MIOS_BANKSTICK_Read		; don't write if value is equal
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_Write_NA
	cpfseq	MIOS_TMP2, BANKED
	rgoto MIOS_BANKSTICK_WriteNow
	movlw	0x00
	rgoto	MIOS_BANKSTICK_Write_End

MIOS_BANKSTICK_WriteNow
	decf	MIOS_PARAMETER1, F		; decrement MIOS_PARAMETER[12]
	skpc
	decf	MIOS_PARAMETER2, F

MIOS_BANKSTICK_Write_NoVerifyR
	movf	MIOS_TMP2, W, BANKED	; get byte

	rcall	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
#if MIOS_BANKSTICK_16BIT == 1
	movf	MIOS_PARAMETER2, W	; send high address
	rcall	MIOS_IIC_ByteSend
#endif
	movf	MIOS_PARAMETER1, W	; send low address
	rcall	MIOS_IIC_ByteSend

	incf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER[12]
	skpnz
	incf	MIOS_PARAMETER2, F

	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_Write_NA

	movf	MIOS_TMP2, W, BANKED	; send byte which should be written into EEPROM
	rcall	MIOS_IIC_ByteSend
	
	rcall	MIOS_IIC_Stop		; stop IIC

	;; wait until write cycle is finished
MIOS_BANKSTICK_Write_Poll
	;; memo: time out counter should be inserted here!
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE

	rcall	MIOS_IIC_Start
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
	rcall	MIOS_IIC_Stop		; stop IIC

	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_Write_Poll


	movlw	0x00
	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, MIOS_BANKSTICK_Write_NoVerify2

	decf	MIOS_PARAMETER1, F		; decrement MIOS_PARAMETER[12]
	skpc
	decf	MIOS_PARAMETER2, F

	rcall	MIOS_BANKSTICK_Read		; read address again
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_Write_NA
	cpfseq	MIOS_TMP2, BANKED
	rgoto MIOS_BANKSTICK_Write_Error
	movlw	0x00				; error status = OK
	rgoto	MIOS_BANKSTICK_Write_End
MIOS_BANKSTICK_Write_NA
	movlw	0x02				; error status = not available
	rgoto	MIOS_BANKSTICK_Write_End
MIOS_BANKSTICK_Write_Error
	movlw	0x01				; error status = failed
	;; 	rgoto	MIOS_BANKSTICK_Write_End
MIOS_BANKSTICK_Write_End
MIOS_BANKSTICK_Write_NoVerify2
	andlw	0xff				; update STATUS register
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_WritePage
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_WritePage(unsigned int addr, unsigned char *buffer)
;;  DESCRIPTION: writes a 64 bytes page into BankStick. 
;;  If verify mode has been enabled with MIOS_BANKSTICK_CtrlSet, write access will 
;;  be skipped if content is equal to the byte which should be written
;;  Returned Error Status:
;;     0x00: no error
;;     0x01: byte mismatch (write failed) -- only set if verify enabled
;;     0x02: BankStick not available
;;  IN:	  pointer to write buffer (64 bytes) in FSR1
;;        address in MIOS_PARAMETER[12] (0x0000-0xffc0)
;;  C_IN: pointer to write buffer (64 bytes) in <buffer>
;;        address in <addr> (0x0000-0xffc0)
;;  OUT:  error status in WREG
;;        MIOS_PARAMETER[12] will be incremented to next page address (+64)
;;        FSR1 will be left untouched
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] is cleared if write failed
;;  C_OUT: error status in WREG
;;        MIOS_BOX_STAT.BS_AVAILABLE is cleared if write failed
;;  USES: BSR, MIOS_PARAMETER[12]
;;  EXAMPLE:
;;
;;	;; write a page of 64 bytes to BankStick memory at address 0x1240
;;	movlw	0x40			; store low-byte of address in
;;	movwf	MIOS_PARAMETER1		; MIOS_PARAMETER1
;;	movlw	0x12			; store high-byte of address in
;;	movwf	MIOS_PARAMETER2		; MIOS_PARAMETER2
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM which
;;					: contains some data
;;	call	MIOS_BANKSTICK_WritePage; initiate the write
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;
;;  C_EXAMPLE:
;;
;;	unsigned char buffer[64];
;;	unsigned char i;
;;	
;;	// fill buffer with some bytes
;;	for(i=0; i<64; ++i)
;;	  buffer[i] = i;
;;	
;;	// write a page of 64 bytes to BankStick memory at address 0x3000
;;	if( MIOS_BANKSTICK_WritePage(0x3000, buffer) ) {
;;	  // here you could do some error handling
;;	}
;;	
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_WritePage
	SET_BSR	MIOS_TMP2

	;; align address
	movlw	0xc0
	andwf	MIOS_PARAMETER1, F

	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, MIOS_BANKSTICK_WritePage_NoVR

	;; first compare buffer with BankStick content
	clrf	MIOS_TMP2, BANKED
MIOS_BANKSTICK_WritePage_CLoop1
	rcall	MIOS_BANKSTICK_Read		; don't write if value is equal
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_NA
	movwf	MIOS_PARAMETER3
	movf	MIOS_TMP2, W, BANKED
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER3, W
	bnz	MIOS_BANKSTICK_WritePageNow
	incf	MIOS_TMP2, F, BANKED
	BRA_IFCLR MIOS_TMP2, 6, BANKED, MIOS_BANKSTICK_WritePage_CLoop1

	;; no mismatches: don't write, error status = ok
	movlw	0x00
	rgoto	MIOS_BANKSTICK_WritePage_End

MIOS_BANKSTICK_WritePageNow
	;; fix pointer
	decf	MIOS_PARAMETER1, F
	skpc
	decf	MIOS_PARAMETER2, F
	movlw	0xc0
	andwf	MIOS_PARAMETER1, F

MIOS_BANKSTICK_WritePage_NoVR
	rcall	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
#if MIOS_BANKSTICK_16BIT == 1
	movf	MIOS_PARAMETER2, W	; send high address
	rcall	MIOS_IIC_ByteSend
#endif
	movf	MIOS_PARAMETER1, W	; send low address
	rcall	MIOS_IIC_ByteSend

	movlw	0x40
	addwf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER[12]
	movlw	0x00
	addwfc	MIOS_PARAMETER2, F

	;; write 64 bytes into buffer
	clrf	MIOS_TMP2, BANKED
MIOS_BANKSTICK_WritePage_WLoop
	movf	MIOS_TMP2, W, BANKED	; send byte which should be written into EEPROM
	movf	PLUSW1, W
	rcall	MIOS_IIC_ByteSend
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_NA
	incf	MIOS_TMP2, F, BANKED
	BRA_IFCLR MIOS_TMP2, 6, BANKED, MIOS_BANKSTICK_WritePage_WLoop
	
	rcall	MIOS_IIC_Stop		; stop IIC
	
	;; wait until write cycle is finished
MIOS_BANKSTICK_WritePage_Poll
	;; memo: time out counter should be inserted here!
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE

	rcall	MIOS_IIC_Start
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
	rcall	MIOS_IIC_Stop		; stop IIC

	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_Poll


	movlw	0x00
	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, MIOS_BANKSTICK_WritePage_NoV2

	;; fix pointer
	decf	MIOS_PARAMETER1, F
	skpc
	decf	MIOS_PARAMETER2, F
	movlw	0xc0
	andwf	MIOS_PARAMETER1, F

	;; first compare buffer with BankStick content
	clrf	MIOS_TMP2, BANKED
MIOS_BANKSTICK_WritePage_CLoop2
	rcall	MIOS_BANKSTICK_Read		; don't write if value is equal
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_NA
	movwf	MIOS_PARAMETER3
	movf	MIOS_TMP2, W, BANKED
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER3, W
	bnz	MIOS_BANKSTICK_WritePage_Error
	incf	MIOS_TMP2, F, BANKED
	BRA_IFCLR MIOS_TMP2, 6, BANKED, MIOS_BANKSTICK_WritePage_CLoop2
	movlw	0x00				; error status = OK
	rgoto	MIOS_BANKSTICK_WritePage_End
MIOS_BANKSTICK_WritePage_NA
	movlw	0x02				; error status = not available
	rgoto	MIOS_BANKSTICK_WritePage_End
MIOS_BANKSTICK_WritePage_Error
	movlw	0x01				; error status = failed
	;; 	rgoto	MIOS_BANKSTICK_WritePage_End
MIOS_BANKSTICK_WritePage_End
MIOS_BANKSTICK_WritePage_NoV2
	andlw	0xff				; update STATUS register
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_Read
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_Read(unsigned int addr)
;;  DESCRIPTION: reads a byte from BankStick.
;;  IN:	  address in MIOS_PARAMETER[12] (0x0000-0xffff)
;;  C_IN: address in <addr> (0x0000-0xffff)
;;  OUT:  BankStick content in WREG
;;        MIOS_PARAMETER[12] will be incremented
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] is zero if read failed
;;  C_OUT:  BankStick content
;;        MIOS_BOX_STAT.BS_AVAILABLE cleared if read failed
;;  USES: BSR, MIOS_PARAMETER[12]
;;  EXAMPLE:
;;
;;	;; load a byte from address 0x7fff (the last in a 32k BankStick)
;;	movlw	0xff			; initialize pointer to BankStick
;;					; address
;;	movwf	MIOS_PARAMETER1		; -> 0x7fff
;;	movlw	0x7f
;;	movwf	MIOS_PARAMETER2
;;	call	MIOS_BANKSTICK_Read	; load byte from BankStick
;;					; branch to error exception
;;					; routine if neccesary
;;	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, BankStickNotAvl
;;	
;;  C_EXAMPLE:
;;
;;	// load a byte from address 0x7fff (the last in a 32k BankStick)
;;	value = MIOS_BANKSTICK_Read(0x7fff);
;;	
;;	if( !MIOS_BOX_STAT.BS_AVAILABLE ) {
;;	  // here you could do some error handling
;;	}
;;	
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_Read
	rcall	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
#if MIOS_BANKSTICK_16BIT == 1
	movf	MIOS_PARAMETER2, W	; send high address
	rcall	MIOS_IIC_ByteSend
#endif
	movf	MIOS_PARAMETER1, W	; send low address
	rcall	MIOS_IIC_ByteSend

	rcall	MIOS_IIC_Start		; start IIC again
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa1
	rcall	MIOS_IIC_ByteSend
	rcall	MIOS_IIC_ByteReceive	; read byte #0
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED
	rcall	MIOS_IIC_NakSend
	rcall	MIOS_IIC_Stop		; stop IIC

	incf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER[12]
	skpnz
	incf	MIOS_PARAMETER2, F

	movf	MIOS_TMP1, W, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_ReadPage
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_ReadPage(unsigned int addr, unsigned char *buffer)
;;  DESCRIPTION: reads a 64 bytes page from BankStick. 
;;  IN:	  pointer to read buffer (64 bytes) in FSR1
;;        address in MIOS_PARAMETER[12] (0x0000-0xffc0)
;;  C_IN: pointer to read buffer (64 bytes) in <buffer><BR>
;;        address in <addr> (0x0000-0xffc0)
;;  OUT:  BankStick content in read buffer
;;        MIOS_PARAMETER[12] will be incremented to next page address (+64)
;;        FSR1 will be left untouched
;;        MIOS_BOX_STAT[MIOS_BOX_STAT_BS_AVAILABLE] is also zero if write failed
;;  C_OUT: BankStick content in read buffer
;;        MIOS_BOX_STAT.BS_AVAILABLE is also zero if write failed
;;  USES: BSR, MIOS_PARAMETER[12]
;;  EXAMPLE:
;;
;;	;; read a page of 64 bytes from BankStick memory at address 0x1240
;;	movlw	0x40			; store low-byte of address in
;;	movwf	MIOS_PARAMETER1		; MIOS_PARAMETER1
;;	movlw	0x12			; store high-byte of address in
;;	movwf	MIOS_PARAMETER2		; MIOS_PARAMETER2
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM which
;;					: contains some data
;;	call	MIOS_BANKSTICK_ReadPage	; start the page read
;;	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, BankStickNotAvl
;;
;;  C_EXAMPLE:
;;
;;	unsigned char buffer[64];
;;	
;;	// read a page of 64 bytes from BankStick memory at address 0x1240
;;	MIOS_BANKSTICK_ReadPage(0x1240, buffer);
;;
;;	if( !MIOS_BOX_STAT.BS_AVAILABLE ) {
;;	  // here you could do some error handling
;;	}
;;	
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_ReadPage
	;; align address
	movlw	0xc0
	andwf	MIOS_PARAMETER1, F

	rcall	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	rcall	MIOS_IIC_ByteSend
#if MIOS_BANKSTICK_16BIT == 1
	movf	MIOS_PARAMETER2, W	; send high address
	rcall	MIOS_IIC_ByteSend
#endif
	movf	MIOS_PARAMETER1, W	; send low address
	rcall	MIOS_IIC_ByteSend

	rcall	MIOS_IIC_Start		; start IIC again
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa1
	rcall	MIOS_IIC_ByteSend

	;; read 64 bytes
	SET_BSR	MIOS_TMP2
	clrf	MIOS_TMP2, BANKED
MIOS_BANKSTICK_ReadPage_Loop
	rcall	MIOS_IIC_ByteReceive	; read byte #0
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED
	movf	MIOS_TMP2, W, BANKED
	movff	MIOS_TMP1, PLUSW1
	incf	MIOS_TMP2, F, BANKED
	movf	MIOS_TMP2, W, BANKED
	andlw	0x3f
	bz	MIOS_BANKSTICK_ReadPage_LoopBrk
	rcall	MIOS_IIC_AckSend
	rgoto	MIOS_BANKSTICK_ReadPage_Loop
	
MIOS_BANKSTICK_ReadPage_LoopBrk
	rcall	MIOS_IIC_NakSend

	rcall	MIOS_IIC_Stop		; stop IIC

	movlw	0x40
	addwf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER[12] by 0x40
	skpnc
	incf	MIOS_PARAMETER2, F

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_CtrlSet
;;  C_DECLARATION: void MIOS_BANKSTICK_CtrlSet(unsigned char ctrl)
;;  DESCRIPTION: sets the BankStick control register<BR>
;;     bit 2-0: selects the BankStick (1 of 8, address defined with pin A0-A2 of the EEPROM)<BR>
;;     bit 7:   if set, the verify during BankStick write will be disabled
;;  IN:	  control register content in WREG
;;  C_IN: control register content in <ctrl>
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;	;; write 0x47 0x11 to address 0x3000-0x3001 of the BankStick number 7
;;	movlw	0x07			; select 8th BankStick
;;	call	MIOS_BANKSTICK_CtrlSet
;;	movlw	0x00			; store low-byte of address in 
;;	movwf	MIOS_PARAMETER1		; MIOS_PARAMETER1
;;	movlw	0x30			; store high-byte of address in 
;;	movwf	MIOS_PARAMETER2		; MIOS_PARAMETER2
;;	movlw	0x47			; write 0x47 to 0x3000
;;	call	MIOS_BANKSTICK_Write	; (address will be incremented
;;					; after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;	movlw	0x11			; write 0x11 to 0x3001
;;	call	MIOS_BANKSTICK_Write	; (address will be incremented
;;					; after write)
;;	bnz	BSProgrammingFailed	; branch to your exception handler
;;					; if necessary
;;	movlw	0x00			; select first BankStick again
;;	call	MIOS_BANKSTICK_CtrlSet	
;;  C_EXAMPLE:
;;
;;	unsigned char error = 0;
;;
;;	// write 0x47 0x11 to address 0x3000-0x3001 of the BankStick number 7
;;	MIOS_BANKSTICK_CtrlSet(0x07); // select 8th BankStick
;;	error |= MIOS_BANKSTICK_Write(0x3000, 0x47);
;;	error |= MIOS_BANKSTICK_Write(0x3001, 0x11);
;;
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_CtrlSet
	bcf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_A0
	bcf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_A1
	bcf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_A2
	bcf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY

	btfsc	WREG, 7
	bsf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY

	andlw	0x07
	iorwf	MIOS_BOX_CFG1, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_BANKSTICK_CtrlGet
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_CtrlGet(void)
;;  DESCRIPTION: returns the BankStick control status
;;  IN:  -
;;  C_IN:  -
;;  OUT:  WREG[2..0]: A2-A0
;;        WREG[7]: Verify disabled
;;  C_OUT:  bit [2..0]: A2-A0
;;        bit [7]: Verify disabled
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_CtrlGet
	movf	MIOS_BOX_CFG1, W
	andlw	0x07
	btfsc	MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY
	iorlw 0x80
	return

