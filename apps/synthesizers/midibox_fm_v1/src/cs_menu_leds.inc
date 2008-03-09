; $Id$
;
; MIDIbox FM Control Surface LED Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by the menu handler in cs_menu.inc
;;  when all LEDs should be updated
;;  The DOUT pin numbers are defined in cs_menu_io_tables.inc
;; --------------------------------------------------------------------------
CS_MENU_LED_Update
	;; clear temporary registers which hold some LED flags which are not saved anywhere else
	;; (see also CS_MENU_DOUT_TABLE)
	;; this quick & dirty method saves some memory...
	clrf	TMP1

	;; TMP1[1..0]: Instr/OP LED
	movlw	0x01
	btfsc	CS_MENU_MODE, CS_MENU_MODE_INS_OP
	movlw 0x02
	movwf	TMP1

	;; ----

	;; go through the DOUT map
	TABLE_ADDR CS_MENU_DOUT_TABLE
CS_MENU_LED_Update_Loop
	tblrd*+				; get low-byte of register address
	movf	TABLAT, W		; load it in WREG
	movwf	FSR0L			; and store it in FSR0L
	tblrd*+				; get high byte
	movff	TABLAT, FSR0H		; store it in FSR0H without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	CS_MENU_LED_Update_End	; in this case jump to the end

	tblrd*+				; get flag-number
	movf	TABLAT, W
	call	MIOS_HLP_GetBitORMask	; get appr. OR mask (1 of 8)
	andwf	INDF0, W		; AND this with the register specified in the table
	skpz				; LED value: 0 if result zero, 1 if result != zero
	movlw	0x01
	movwf	MIOS_PARAMETER1		; store result in MIOS_PARAMETER1
	tblrd*+				; read pin number from table
	movf	TABLAT, W		; get pin number
	CALL_IFCLR TABLAT, 7, ACCESS, MIOS_DOUT_PinSet	; and finally set the pin of SR if bit #7 cleared
	rgoto	CS_MENU_LED_Update_Loop	; next entry

CS_MENU_LED_Update_End
	return
