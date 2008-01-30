; $Id$
;
; MIOS EEPROM routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_EEPROM_Write
;;  C_DECLARATION: unsigned char MIOS_EEPROM_Write(unsigned char addr, unsigned char value)
;;  DESCRIPTION: writes a byte into EEPROM. <BR>
;;  Write access will be skipped if content is equal to the byte which should
;;  be written<BR>
;;  Returned Error Status:<BR>
;;     0x00: no error<BR>
;;     0x01: byte mismatch (write failed)
;;  IN:	  byte in WREG, address in EEADR (0x00-0xff) and EEADRH (0-3, PIC18F4620 only)
;;  C_IN: byte in <value>, address in <addr> (0x00-0xff), high byte in EEADRH (0-3, PIC18F4620 only)
;;  OUT:  error status in WREG and MIOS_PARAMETER1
;;        EEADR will be incremented
;;        EEADRH (PIC18F4620) will *not* be incremented on EEADR overrun!
;;  C_OUT: error status
;;  USES: BSR, EEADR, EEDATA, EECON1, EECON2
;;  EXAMPLES:
;;
;;	;; write 0x47 0x11 into EEPROM at address 0x80/0x81
;;	movlw	0x80			; write 0x80 to address register
;;	movwf	EEADR
;;	movlw	0x47			; write 0x47 to 0x80
;;	call	MIOS_EEPROM_Write	; (EEADR will be incremented)
;;	bnz	EEPROMProgrammingFailed	; branch to your exception handler
;;                                      ; if necessary
;;	movlw	0x11			; write 0x11 to 0x81
;;	call	MIOS_EEPROM_Write	; (EEADR will be incremented)
;;	bnz	EEPROMProgrammingFailed	; branch to your exception handler
;;                                      ; if necessary
;;
;;  C_EXAMPLE:
;;
;;	unsigned char error = 0;
;;
;;	// write 0x47 0x11 into EEPROM at address 0x80/0x81
;;	error |= MIOS_EEPROM_Write(0x80, 0x47);
;;	error |= MIOS_EEPROM_Write(0x81, 0x11);
;;	
;;	if( error ) {
;;	  // here you could do some error handling
;;	}
;;
;;	
;;	
;;	// note: it's very unlikely that an error happens when writing
;;	// to internal EEPROM, therefore you can use following shortcut:
;;	
;;	// write 0x47 0x11 into EEPROM at address 0x80/0x81
;;	MIOS_EEPROM_Write(0x80, 0x47);
;;	MIOS_EEPROM_Write(0x81, 0x11);
;;	
;; --------------------------------------------------------------------------
MIOS_EEPROM_Write
	;; NOTE: don't use MIOS_TMP2 within this function, it's used by MIOS_EEPROM_WritePage!

	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED

	rcall	MIOS_EEPROM_Read		; don't write if value is equal
	cpfseq	MIOS_TMP1, BANKED
	rgoto MIOS_EEPROM_WriteNow
	movlw	0x00
	rgoto	MIOS_EEPROM_Write_End

MIOS_EEPROM_WriteNow
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	;; TK: no auto increment of EEADRH to avoid compatibility issues with older applications,
	;;     where EEADRH is not initialized (it doesn't exist on a PIC18F452)
	;; 	movf	EEADR, W
	;; 	skpnz
	;; 	decf	EEADRH, F
#endif
	decf	EEADR, F			; decrement EEADR
	

	movff	MIOS_TMP1, EEDATA	; move byte to EEDATA register

	IRQ_DISABLE				; disable interrupts like recommented in the Errata sheet

	bcf	EECON1, EEPGD			; point to data memory
	bsf	EECON1, WREN			; EEPROM write enable
	movlw	0x55				; unlock sequence
	movwf	EECON2
	movlw	0xaa
	movwf	EECON2
	bsf	EECON1, WR			; EEPROM write
EEPROM_Write_PollEECON
	GOTO_IFSET EECON1, WR, ACCESS, EEPROM_Write_PollEECON

	IRQ_ENABLE				; enable IRQs

	rcall	MIOS_EEPROM_Read		; read address again
	cpfseq	MIOS_TMP1, BANKED
	rgoto MIOS_EEPROM_Write_Error
	movlw	0x00				; error status = OK
	rgoto	MIOS_EEPROM_Write_End
MIOS_EEPROM_Write_Error
	movlw	0x01				; error status = failed
	;; 	rgoto	MIOS_EEPROM_Write_End
MIOS_EEPROM_Write_End
	movwf	MIOS_PARAMETER1
	andlw	0xff				; update STATUS register
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_EEPROM_WritePage
;;  C_DECLARATION: unsigned char MIOS_EEPROM_WritePage(unsigned char addr, unsigned char *buffer)
;;  DESCRIPTION: writes a 64 bytes page into EEPROM.<BR>
;;  The internal EEPROM of PIC18F452 doesn't provide a page write by itself,
;;  therefore this function calls MIOS_EEPROM_Write 64 times.<BR>
;;  Returned Error Status:<BR>
;;     0x00: no error<BR>
;;     0x01: byte mismatch (one or more writes failed)
;;  IN:	  pointer to write buffer (64 bytes) in FSR1
;;        address in EEADR (0x00-0xc0) and EEADRH (0-3, PIC18F4620 only)
;;  C_IN:  pointer to write buffer (64 bytes) in <buffer>
;;        address in <addr> (0x00-0xc0) and EEADRH (0-3, PIC18F4620 only)
;;  OUT:  error status in WREG and MIOS_PARAMETER1
;;        EEADR will be incremented to the next page address (+64)
;;        EEADRH (PIC18F4620) will *not* be incremented on EEADR overrun!
;;        FSR1 will be left untouched
;;  C_OUT: error status
;;  USES: BSR, EEADR, EEDATA, EECON1, EECON2
;;  EXAMPLE:
;;
;;	;; write a page of 64 bytes to EEPROM memory at address 0x80
;;	movlw	0x80			; set start address of page
;;      movwf   EEADR                   ; (must be aligned to 64)
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM which
;;					: contains some data
;;	call	MIOS_EEPROM_WritePage	; initiate the write
;;	bnz	EEPROMProgrammingFailed	; branch to your exception handler
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
;;	// write a page of 64 bytes to EEPROM memory at address 0x80
;;	MIOS_EEPROM_WritePage(0x80, buffer);
;;
;; --------------------------------------------------------------------------
MIOS_EEPROM_WritePage
	;; ensure that address is aligned to 0x40
	movlw	0xc0
	andwf	EEADR, F

	;; start loop
	SET_BSR	MIOS_TMP2
	clrf	MIOS_TMP2		; MIOS_TMP2 holds the error status
MIOS_EEPROM_WritePage_Loop
	movf	EEADR, W		; get byte from buffer
	andlw	0x3f
	movf	PLUSW1, W
	rcall	MIOS_EEPROM_Write	; write to EEPROM
	;; OR the result
	iorwf	MIOS_TMP2, F, BANKED
	;; loop until end of page reached
	movf	EEADR, W
	andlw	0x3f
	bnz	MIOS_EEPROM_WritePage_Loop

	;; copy write result into MIOS_PARAMETER1
	movf	MIOS_TMP2, W, BANKED
	movwf	MIOS_PARAMETER1

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_EEPROM_Read
;;  C_DECLARATION: unsigned char MIOS_EEPROM_Read(unsigned char addr)
;;  DESCRIPTION: reads a byte from EEPROM. 
;;  IN:	  address in EEADR (0x00-0xff) and EEADRH (0-3, PIC18F4620 only)
;;  C_IN: address in <addr> (0x00-0xff) and EEADRH (0-3, PIC18F4620 only)
;;  OUT:  EEPROM content in WREG
;;        EEADR will be incremented
;;        EEADRH (PIC18F4620) will *not* be incremented on EEADR overrun!
;;  C_OUT: returns EEPROM content
;;  USES: BSR, EEADR, EEDATA, EECON1, EECON2
;;  EXAMPLE:
;;
;;	;; load a byte from address 0x80
;;	movlw	0x80			; write 0x80 to address register
;;	movwf	EEADR
;;	call	MIOS_EEPROM_Read	; read from EEPROM
;;
;;  C_EXAMPLE:
;;
;;	// load a byte from address 0x80
;;	value = MIOS_EEPROM_Read(0x80);
;;
;; --------------------------------------------------------------------------
MIOS_EEPROM_Read
	;; NOTE: don't use MIOS_TMP2 within this function, it's used by MIOS_EEPROM_ReadPage!

	IRQ_DISABLE				; disable interrupts like recommented in the Errata sheet

	bcf	EECON1, EEPGD			; point to data memory
	bcf	EECON1, CFGS			; don't access config sector
	bsf	EECON1, RD			; EEPROM read
	movf	EEDATA, W			; get data word

	IRQ_ENABLE

	incf	EEADR, F			; increment EEADR
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	;; TK: no auto increment of EEADRH to avoid compatibility issues with older applications,
	;;     where EEADRH is not initialized (it doesn't exist on a PIC18F452)
	;; 	skpnz
	;; 	incf	EEADRH, F			; increment EEADRH on overrun
#endif
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_EEPROM_ReadPage
;;  C_DECLARATION: unsigned char MIOS_EEPROM_ReadPage(unsigned char addr, unsigned char *buffer)
;;  DESCRIPTION: reads a 64 bytes page from EEPROM<BR>
;;  The internal EEPROM of PIC18F452 doesn't provide a page read by itself,
;;  therefore this function calls MIOS_EEPROM_Read 64 times.
;;  IN:	  pointer to read buffer (64 bytes) in FSR1
;;        address in EEADR (0x00-0xc0) and EEADRH (0-3, PIC18F4620 only)
;;  C_IN: pointer to read buffer (64 bytes) in <buffer>
;;        address in <addr> (0x00-0xc0) and EEADRH (0-3, PIC18F4620 only)
;;  OUT:  EEPROM content in read buffer
;;        EEADR will be incremented to next page address (+64)
;;        EEADRH (PIC18F4620) will *not* be incremented on EEADR overrun!
;;        FSR1 will be left untouched
;;  C_OUT:  EEPROM content in read <buffer>
;;  USES: BSR, EEADR, EEDATA, EECON1, EECON2
;;  EXAMPLE:
;;
;;	;; read a page of 64 bytes from EEPROM at address 0x40
;;	movlw	0x40			; set start address of page
;;      movwf   EEADR                   ; (must be aligned to 64)
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM which
;;					: contains some data
;;	call	MIOS_EEPROM_ReadPage	; start the page read
;;
;;  C_EXAMPLE:
;;
;;	unsigned char buffer[64];
;;	
;;	// read a page of 64 bytes from EEPROM at address 0x40
;;	MIOS_EEPROM_ReadPage(0x40, buffer);
;;
;; --------------------------------------------------------------------------
MIOS_EEPROM_ReadPage
	;; ensure that address is aligned to 0x40
	movlw	0xc0
	andwf	EEADR, F

	;; start loop
MIOS_EEPROM_ReadPage_Loop
	rcall	MIOS_EEPROM_Read	; read from EEPROM
	SET_BSR	MIOS_TMP2
	movwf	MIOS_TMP2, BANKED
	decf	EEADR, W		; write return value into buffer
	andlw	0x3f
	movff	MIOS_TMP2, PLUSW1
	;; loop until end of page reached
	movf	EEADR, W
	andlw	0x3f
	bnz	MIOS_EEPROM_ReadPage_Loop

	return
