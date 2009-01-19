; $Id$
;
;  Help functions to send debug messages to MIOS Terminal via SysEx
;
; ==========================================================================
;
;  Copyright 2009 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendHeader
;;  DESCRIPTION: sends the SysEx header
;;  IN:	  -
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendHeader	; for C
DEBUG_MSG_SendHeader
	;; this SysEx command will be parsed by MIOS Terminal to print out text messages
	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7e
	call	MIOS_MIDI_TxBufferPut
	movlw	0x32
	call	MIOS_MIDI_TxBufferPut
	call	MIOS_MIDI_DeviceIDGet
	call	MIOS_MIDI_TxBufferPut
	movlw	0x0d
	call	MIOS_MIDI_TxBufferPut
	movlw	0x40
	call	MIOS_MIDI_TxBufferPut

	;; following characters will be print on terminal
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendFooter
;;  DESCRIPTION: sends the SysEx footer
;;  IN:	  -
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendFooter	; for C
DEBUG_MSG_SendFooter
	;; finish SysEx stream - MIOS Terminal will print the received characters now
	movlw	0xf7
	call	MIOS_MIDI_TxBufferPut
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendChar
;;  DESCRIPTION: sends a single character
;;  IN:	  Character in WREG
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendChar	; for C
DEBUG_MSG_SendChar
	andlw	0x7f	; ensure that MSB is cleared, otherwise we would violate the MIDI protocol
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendASMString
;;  DESCRIPTION: used in assembly programs to send a string
;;               see example in README.txt for usage
;;  IN:	  zero-terminated string after call instruction
;;  OUT:  -
;;  USES: BSR, FSR1, TBLPTR[LHU], TABLAT
;; --------------------------------------------------------------------------
DEBUG_MSG_SendASMString
	;; we expect that the zero-terminated string is located behind the "call" instruction
	;; transfer pointer to TBLPTR[LHU]
        movff   TOSL, TBLPTRL
        movff   TOSH, TBLPTRH
        movff   TOSU, TBLPTRU

	;; print until zero is read
DEBUG_MSG_SendASMStringLoop
	tblrd*+
	movf	TABLAT, W
	bz	DEBUG_MSG_SendASMStringLoop_End
	rcall	DEBUG_MSG_SendChar
	rgoto	DEBUG_MSG_SendASMStringLoop
DEBUG_MSG_SendASMStringLoop_End

        ;; return pointer now in TBLPTR[LH]

	;; ensure that it is aligned to next instruction
	movf	TBLPTRL, W
	andlw	0x01
	bz	DEBUG_MSG_SendASMString_NoIAlign
	incf	TBLPTRL, F
	skpnz
	incf	TBLPTRH, F
	skpnz
	incf	TBLPTRU, F
DEBUG_MSG_SendASMString_NoIAlign

        IRQ_DISABLE
        movf    TBLPTRL, W
        movwf   TOSL
        movf    TBLPTRH, W
        movwf   TOSH
        movf    TBLPTRU, W
        movwf   TOSU
        IRQ_ENABLE
        return


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendBCD1
;;  DESCRIPTION: sends a 8-bit value as BCD (decimal value) -- one digit only
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendHex1
;;  DESCRIPTION: sends a 4-bit hex value
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendBCD1	; for C
DEBUG_MSG_SendBCD1
_DEBUG_MSG_SendHex1	; for C
DEBUG_MSG_SendHex1
	andlw	0x0f
	addlw	-0x0a
	skpnc
	addlw	0x07
	addlw	0x3a
	rgoto	DEBUG_MSG_SendChar


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendHex2
;;  DESCRIPTION: sends a 8-bit hex value
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1, PRODL (!)
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendHex2	; for C
DEBUG_MSG_SendHex2
	movwf	PRODL
	swapf	PRODL, W
	rcall	DEBUG_MSG_SendBCD1
	movf	PRODL, W
	rgoto	DEBUG_MSG_SendBCD1


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendBCD2
;;  DESCRIPTION: prints a 8-bit value as BCD (decimal value) -- two digits only
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendBCD2	; for C
DEBUG_MSG_SendBCD2
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	rgoto	DEBUG_MSG_SendBCD2_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendBCD3
;;  DESCRIPTION: prints a 8-bit value as BCD (decimal value) -- all three digits
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
_DEBUG_MSG_SendBCD3	; for C
DEBUG_MSG_SendBCD3
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	rgoto	DEBUG_MSG_SendBCD3_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendBCD4
;;  DESCRIPTION: prints a 16-bit as BCD (decimal value) -- four digits
;;  IN:   Low-byte in WREG, High-byte in MIOS_PARAMETER1
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1, MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
;_DEBUG_MSG_SendBCD4	; "unsigned int" parameter converted in debug_msg.asm
DEBUG_MSG_SendBCD4
	call	MIOS_HLP_Dec2BCD
	rgoto	DEBUG_MSG_SendBCD4_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendBCD5
;;  DESCRIPTION: prints a 16-bit as BCD (decimal value) -- five digits
;;  IN:   Low-byte in WREG, High-byte in MIOS_PARAMETER1
;;  IN:	  Value in WREG
;;  OUT:  -
;;  USES: BSR, FSR1, MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
;_DEBUG_MSG_SendBCD5	; "unsigned int" parameter converted in debug_msg.asm
DEBUG_MSG_SendBCD5
	call	MIOS_HLP_Dec2BCD

;; -----------------
DEBUG_MSG_SendBCD5_Cont
	movf	MIOS_PARAMETER3, W
	andlw	0x0f
	bnz	DEBUG_MSG_SendBCD_D5

	movlw	' '
	rcall	DEBUG_MSG_SendChar

DEBUG_MSG_SendBCD4_Cont
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	bnz	DEBUG_MSG_SendBCD_D4

	movlw	' '
	rcall	DEBUG_MSG_SendChar

DEBUG_MSG_SendBCD3_Cont
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	bnz	DEBUG_MSG_SendBCD_D3

	movlw	' '
	rcall	DEBUG_MSG_SendChar

DEBUG_MSG_SendBCD2_Cont
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	DEBUG_MSG_SendBCD_D2

	movlw	' '
	rcall	DEBUG_MSG_SendChar

DEBUG_MSG_SendBCD1_Cont
	rgoto	DEBUG_MSG_SendBCD_D1
	

	;; ---
DEBUG_MSG_SendBCD_D5
	rcall	DEBUG_MSG_SendBCD1
DEBUG_MSG_SendBCD_D4
	swapf	MIOS_PARAMETER2, W
	rcall	DEBUG_MSG_SendBCD1
DEBUG_MSG_SendBCD_D3
	movf	MIOS_PARAMETER2, W
	rcall	DEBUG_MSG_SendBCD1
DEBUG_MSG_SendBCD_D2
	swapf	MIOS_PARAMETER1, W
	rcall	DEBUG_MSG_SendBCD1
DEBUG_MSG_SendBCD_D1
	movf	MIOS_PARAMETER1, W
	rgoto	DEBUG_MSG_SendBCD1


;; --------------------------------------------------------------------------
;;  FUNCTION: DEBUG_MSG_SendCString
;;  C_DECLARATION: void DEBUG_MSG_SendCString(char *str)
;;  DESCRIPTION: sends a 0-terminated string --- only provided by the C Wrapper!
;;  C_IN: Pointer to 0-terminated String in <str>
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
