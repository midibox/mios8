; $Id$
;
; Experimental version of $MIOS_PATH/mios/mios_bankstick.inc, which allows
; non-blocking page writes into EEPROM
;
; Will be taken over into MIOS once the stability has been proven
;
; Non-blocking writes could be enabled via control flag later!
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
	;; temporary name to avoid name collision
MIOS_BANKSTICK_WritePage_NB
	SET_BSR	MIOS_BANKSTICK_TMP

	;; align address
	movlw	0xc0
	andwf	MIOS_PARAMETER1, F

	BRA_IFSET MIOS_BOX_CFG1, MIOS_BOX_CFG1_BS_DIS_VERIFY, ACCESS, MIOS_BANKSTICK_WritePage_NoVR

	;; first compare buffer with BankStick content
	clrf	MIOS_BANKSTICK_TMP, BANKED
MIOS_BANKSTICK_WritePage_CLoop1
	call	MIOS_BANKSTICK_Read		; don't write if value is equal
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_NA
	movwf	MIOS_PARAMETER3
	SET_BSR	MIOS_BANKSTICK_TMP
	movf	MIOS_BANKSTICK_TMP, W, BANKED
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER3, W
	bnz	MIOS_BANKSTICK_WritePageNow
	SET_BSR	MIOS_BANKSTICK_TMP
	incf	MIOS_BANKSTICK_TMP, F, BANKED
	BRA_IFCLR MIOS_BANKSTICK_TMP, 6, BANKED, MIOS_BANKSTICK_WritePage_CLoop1

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
	call	MIOS_IIC_Start		; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	call	MIOS_IIC_ByteSend
#if MIOS_BANKSTICK_16BIT == 1
	movf	MIOS_PARAMETER2, W	; send high address
	call	MIOS_IIC_ByteSend
#endif
	movf	MIOS_PARAMETER1, W	; send low address
	call	MIOS_IIC_ByteSend

	movlw	0x40
	addwf	MIOS_PARAMETER1, F		; increment MIOS_PARAMETER[12]
	movlw	0x00
	addwfc	MIOS_PARAMETER2, F

	;; write 64 bytes into buffer
	SET_BSR	MIOS_BANKSTICK_TMP
	clrf	MIOS_BANKSTICK_TMP, BANKED
MIOS_BANKSTICK_WritePage_WLoop
	SET_BSR	MIOS_BANKSTICK_TMP
	movf	MIOS_BANKSTICK_TMP, W, BANKED	; send byte which should be written into EEPROM
	movf	PLUSW1, W
	call	MIOS_IIC_ByteSend
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_BANKSTICK_WritePage_NA
	SET_BSR	MIOS_BANKSTICK_TMP
	incf	MIOS_BANKSTICK_TMP, F, BANKED
	BRA_IFCLR MIOS_BANKSTICK_TMP, 6, BANKED, MIOS_BANKSTICK_WritePage_WLoop
	
	call	MIOS_IIC_Stop		; stop IIC

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
;;  FUNCTION: MIOS_BANKSTICK_CheckAvail
;;  C_DECLARATION: unsigned char MIOS_BANKSTICK_CheckAvail(void)
;;  DESCRIPTION: Checks if the currently selected BankStick is connected, resp.
;;  ready again after a non-blocking write operation has been started
;;  Returned Status:
;;     0x00: BankStick not available
;;     0x01: BankStick available
;;  IN:	  -
;;  C_IN: -
;;  OUT:  BankStick status in WREG
;;        In addition, MIOS_BOX_STAT.BS_AVAILABLE is cleared if BankStick not available
;;  C_OUT: BankStick status
;;        In addition, MIOS_BOX_STAT.BS_AVAILABLE is cleared if BankStick not available
;;  USES: BSR, MIOS_PARAMETER[12]
;;  EXAMPLE:
;;
;;	;; TODO
;;					; if necessary
;;
;;  C_EXAMPLE:
;;
;;	// TODO
;;	
;; --------------------------------------------------------------------------
MIOS_BANKSTICK_CheckAvail
	;; memo: time out counter should be inserted here!
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE

	call	MIOS_IIC_Start
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0			; set address
	call	MIOS_IIC_ByteSend
	call	MIOS_IIC_Stop		; stop IIC

	;; return status
	movlw	0x00
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	movlw	0x01
	andlw	0xff		; (fix ZERO flag)
	return
