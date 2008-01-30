; $Id$
;
; MIOS FLASH routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_FLASH_Write
;;  C_DECLARATION: unsigned char MIOS_FLASH_Write(code char *addr, char *buffer)
;;  DESCRIPTION: writes 64 bytes into FLASH memory<BR>
;;  Write access will be skipped if content is equal to the bytes in the
;;  buffer. Writes to MIOS prgram space will be prevented:<BR>
;;  PIC18F452:  0x0000-0x2fff not writable
;;  PIC18F4620:	0x0000-0x2fff not writable
;;  Returned Error Status:<BR>
;;     0x00: no error<BR>
;;     0x01: byte mismatch (write failed)<BR>
;;     0x02: access error (memory protected)
;;  IN:	  pointer to write buffer (64 bytes) in FSR1
;;        flash address in TBLPRT_[LH] (must be aligned to 64 byte page)
;;  C_IN: pointer to write buffer (64 bytes) in <buffer>
;;        flash address in <addr> (must be aligned to 64 byte page)
;;  OUT:  error status in WREG and MIOS_PARAMETER1
;;        TBLPTR will be incremented to next page address (+64)
;;	  FSR1 will be left untouched
;;  C_OUT: returns error status
;;  USES: BSR, TBLPTR[LH], TABLAT, EECON1, EECON2
;;  EXAMPLE:
;; 
;;	;; write a page of 64 bytes to flash memory at address 0x7000
;;	TABLE_ADDR 0x7000		; load address into TABLPTR[LH]
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM which
;;                                      ; contains some data
;;	call	MIOS_FLASH_Write	; initiate the write
;;	bnz	FlashProgrammingFailed	; branch to your exception handler
;;                                      ; if necessary
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
;;	// write a page of 64 bytes to flash memory at address 0x7000
;;	if( MIOS_FLASH_Write(0x7000, buffer) ) {
;;	  // error handler
;;	}
;;    
;; --------------------------------------------------------------------------
MIOS_FLASH_Write
	SET_BSR	MIOS_TMP1

	;; align TBLPTRL
	movlw	0xc0
	andwf	TBLPTRL, F

	;; prevent writes to addresses > PIC code size
	;; we calculate with 4k blocks, accordingly we have to compare it against TBLPTRU[3:0]:TBLPTRH[7:4]
	swapf	TBLPTRU, W
	andlw	0xf0
	movwf	MIOS_TMP1, BANKED
	swapf	TBLPTRH, W
	andlw	0x0f
	iorwf	MIOS_TMP1, F, BANKED
	movlw	(PIC_DERIVATIVE_CODE_SIZE >> 12) & 0xff
	cpfslt	MIOS_TMP1, BANKED
	rgoto MIOS_FLASH_Write_AccessError

	;; prevent writes to addresses < 0x3000
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	movf	TBLPTRU, W
	bnz	MIOS_FLASH_Write_AccessOk
#endif
	movlw	0x2f
	cpfsgt	TBLPTRH, ACCESS
	rgoto MIOS_FLASH_Write_AccessError

MIOS_FLASH_Write_AccessOk
	;; first compare buffer with Flash content
	clrf	MIOS_TMP1, BANKED
MIOS_FLASH_Write_CompareLoop1
	tblrd*+
	movf	MIOS_TMP1, W, BANKED
	movf	PLUSW1, W
	xorwf	TABLAT, W
	bnz	MIOS_FLASH_Write_CompareMismatch
	incf	MIOS_TMP1, F, BANKED
	BRA_IFCLR MIOS_TMP1, 6, BANKED, MIOS_FLASH_Write_CompareLoop1

	;; no mismatches: don't write, error status = ok
	movlw	0x00
	rgoto	MIOS_FLASH_Write_End

MIOS_FLASH_Write_CompareMismatch
	tblrd*-				; fix table pointer
	movlw	0xc0
	andwf	TBLPTRL, F

	;; all interrupts have to be disabled during write
	;; see PIC18F452 B2 silicon errata
	IRQ_DISABLE			; as IRQs are disabled here, we can use the IRQ tmp registers for
	movff	PIE1, IRQ_TMP1		; saving the control registers :)
	movff	PIE2, IRQ_TMP2
	movff	INTCON, IRQ_TMP3
	movff	INTCON2, IRQ_TMP4
	movff	INTCON3, IRQ_TMP5
	clrf	PIE1
	clrf	PIE2
	clrf	INTCON
	clrf	INTCON2
	clrf	INTCON3
	
MIOS_FLASH_Write_ErasePage
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH program memory
	bsf	EECON1, WREN		; enable write to memory
	bsf	EECON1, FREE		; enable Row Erase operation
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start erase (CPU stall)
	tblrd*-				; dummy read decrement

MIOS_FLASH_WritePage
	clrf	MIOS_TMP1, BANKED	; 8 * 8 bytes row to write
MIOS_FLASH_WritePageLoop
MIOS_FLASH_WritePage_ToHRegs
	movf	MIOS_TMP1, W, BANKED
	movff	PLUSW1, TABLAT		; transfer byte into table latch
	tblwt+*				; write data, perform a short write
	incf	MIOS_TMP1, F, BANKED; loop until buffers are full
	movf	MIOS_TMP1, W, BANKED
	andlw	0x07
	bnz	MIOS_FLASH_WritePage_ToHRegs

MIOS_FLASH_ProgramMemory
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH program memory
	bsf	EECON1, WREN		; enable write to memory
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start program (CPU stall)

	BRA_IFCLR MIOS_TMP1, 6, BANKED, MIOS_FLASH_WritePageLoop

	bcf	EECON1, WREN		; disable write to memory

	movff	IRQ_TMP1, PIE1		; restore IRQ control registers
	movff	IRQ_TMP2, PIE2
	movff	IRQ_TMP3, INTCON
	movff	IRQ_TMP4, INTCON2
	movff	IRQ_TMP5, INTCON3
	IRQ_ENABLE

	movlw	0xc0			; fix table pointer
	andwf	TBLPTRL, F

	;; compare buffer with Flash content - should be the same now!
	clrf	MIOS_TMP1, BANKED
MIOS_FLASH_Write_CompareLoop2
	tblrd*+
	movf	MIOS_TMP1, W, BANKED
	movf	PLUSW1, W
	xorwf	TABLAT, W
 	bnz	MIOS_FLASH_Write_Error
	incf	MIOS_TMP1, F, BANKED
	BRA_IFCLR MIOS_TMP1, 6, BANKED, MIOS_FLASH_Write_CompareLoop2

	movlw	0x00				; error status = OK
	rgoto	MIOS_FLASH_Write_End
MIOS_FLASH_Write_AccessError
	movlw	0x02
	rgoto	MIOS_FLASH_Write_End	
MIOS_FLASH_Write_Error
	movlw	0x01				; error status = failed
	;; 	rgoto	MIOS_FLASH_Write_End
MIOS_FLASH_Write_End

	movwf	MIOS_PARAMETER1
	andlw	0xff				; update STATUS register
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_FLASH_Read
;;  C_DECLARATION: unsigned char MIOS_FLASH_Read(code char *addr, char *buffer)
;;  DESCRIPTION: copies 64 bytes from FLASH memory to buffer
;;  IN:	  pointer to read buffer (64 bytes) in FSR1
;;        flash address in TBLPRT_[LH] (0x0000-0x7fc0, must be aligned to 64 byte page)
;;  C_IN: pointer to read buffer (64 bytes) in <buffer>
;;        flash address in <addr> (0x0000-0x7fc0, must be aligned to 64 byte page)
;;  OUT:  memory dump in read buffer
;;        TBLPTR will be incremented to next page address
;;        FSR1 will be left untouched
;;  C_OUT:  memory dump in <buffer>
;;  USES: BSR, TBLPTR[LH], TABLAT
;;  EXAMPLE:
;; 
;;	;; read a page of 64 bytes from flash memory at address 0x7000
;;	TABLE_ADDR 0x7000		; load address into TABLPTR[LH]
;;	lfsr	FSR1, 0x100		; a free 64 byte buffer in RAM
;;	call	MIOS_FLASH_Read		; initiate the read
;;	;; flash dump now in buffer from 0x100-0x13f
;;    
;;  C_EXAMPLE:
;; 
;;	unsigned char buffer[64];
;;	
;;	// read a page of 64 bytes from flash memory at address 0x7000
;;	MIOS_FLASH_Read(0x7000, buffer);
;;    
;; --------------------------------------------------------------------------
MIOS_FLASH_Read
	;; align TBLPTRL
	movlw	0xc0
	andwf	TBLPTRL, F

	SET_BSR	MIOS_TMP1
	clrf	MIOS_TMP1, BANKED
MIOS_FLASH_ReadLoop
	tblrd*+
	movf	MIOS_TMP1, W, BANKED
	movff	TABLAT, PLUSW1
	incf	MIOS_TMP1, F, BANKED
	BRA_IFCLR MIOS_TMP1, 6, BANKED, MIOS_FLASH_ReadLoop

	return

