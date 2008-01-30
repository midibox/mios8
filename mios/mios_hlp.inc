; $Id$
;
; Misc Help Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_GetIndex_2bytes
;;  C_DECLARATION: not available in C
;;  DESCRIPTION: this help function can be used for jumptables which
;;  contain 2-byte instructions (-> rgoto).
;;  Note that the JUMPTABLE_2BYTES macro allows to use this function
;;  securely without the danger that the code behind the table will
;;  be executed if the index number in WREG is greater than the number
;;  of entries
;;  IN: index in WREG
;;  OUT: jump to indexed entry
;;  USES: -
;;  EXAMPLE:	
;;	;; index is already in WREG
;;	JUMPTABLE_2BYTES 4	; (4 entries)
;;	rgoto	Function_for_WREG_00
;;	rgoto	Function_for_WREG_01
;;	rgoto	Function_for_WREG_02
;;	rgoto	Function_for_WREG_03
;;
;;Function_for_WREG_00
;;	;; ...
;;Function_for_WREG_01
;;	;; ...
;;Function_for_WREG_02
;;	;; ...
;;Function_for_WREG_03
;;	;; ...
;; 
;; --------------------------------------------------------------------------
MIOS_HLP_GetIndex_2bytes
	clrc
        rlf	WREG, W
        addwf   TOSL, F
	skpnc
        incf    TOSH, F
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	skpnc
        incf    TOSU, F
#endif
        return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_GetIndex_4bytes
;;  C_DECLARATION: not available in C
;;  DESCRIPTION: this help function can be used for jumptables which
;;  contain 4-byte instructions (-> goto).
;;  Note that the JUMPTABLE_4BYTES macro allows to use this function
;;  securely without the danger that the code behind the table will
;;  be executed if the index number in WREG is greater than the number
;;  of entries
;;  IN: index in WREG
;;  OUT: jump to indexed entry
;;  USES: -
;;  EXAMPLE:	
;;	;; index is already in WREG
;;	JUMPTABLE_4BYTES 4	; (4 entries)
;;	goto	Function_for_WREG_00
;;	goto	Function_for_WREG_01
;;	goto	Function_for_WREG_02
;;	goto	Function_for_WREG_03
;;
;;	;; far away from this jumptable (so that rgoto cannot be used...)
;; 
;;Function_for_WREG_00
;;	;; ...
;;Function_for_WREG_01
;;	;; ...
;;Function_for_WREG_02
;;	;; ...
;;Function_for_WREG_03
;;	;; ...
;; 
;; --------------------------------------------------------------------------
MIOS_HLP_GetIndex_4bytes
	clrc
        rlf	WREG, F
	clrc
        rlf	WREG, W
        addwf   TOSL, F
	skpnc
        incf    TOSH, F
#if PIC_DERIVATIVE_CODE_SIZE > 0x10000
	skpnc
        incf    TOSU, F
#endif
        return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_AddressFromTable
;;  C_DECLARATION: not available in C
;;  DESCRIPTION: this help function reads a pointer from a table and
;;  moves it to TBLPTR. On this way linked lists can be realized
;;  NOTE: TBLPTRU not read or changed. Table should be located within a 64k segment!
;;  IN: pointer to entry in TBLPTR[LH]
;;  OUT: new pointer in TBLPTR[LH]
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_HLP_AddressFromTable
	tblrd*+
	movff	TABLAT, MIOS_AFT_PTRL
	tblrd*+
	movff	MIOS_AFT_PTRL, TBLPTRL
	movff	TABLAT, TBLPTRH
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_IndirectJump
;;  C_DECLARATION: not available in C
;;  DESCRIPTION: this help function reads a pointer from a table and
;;  jumps to this pointer
;;  NOTE: TBLPTRU not read or changed. Table and target address should be located within a 64k segment!
;;  IN: pointer to entry in TBLPTR[LH]
;;  OUT: jumps indirectly to [TBLPTR[LH]]
;;  USES: WREG
;; --------------------------------------------------------------------------
MIOS_HLP_IndirectJump
	IRQ_DISABLE
	push
	tblrd*+
	movf	TABLAT, W
	movwf	TOSL
	tblrd*+
	movf	TABLAT, W
	movwf	TOSH
	IRQ_ENABLE
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_GetBitANDMask
;;  C_DECLARATION: unsigned char MIOS_HLP_GetBitANDMask(unsigned char value)
;;  DESCRIPTION: this help function is usefull for bit manipulations
;;  IN:	see map below
;;  C_IN: see map below
;;  OUT: 0x00 -> b'11111110'
;;       0x01 -> b'11111101'
;;       0x02 -> b'11111011'
;;       0x03 -> b'11110111'
;;       0x04 -> b'11101111'
;;       0x05 -> b'11011111'
;;       0x06 -> b'10111111'
;;       0x07 -> b'01111111'
;;  C_OUT: 0x00 -> 0b11111110
;;       0x01 -> 0b11111101
;;       0x02 -> 0b11111011
;;       0x03 -> 0b11110111
;;       0x04 -> 0b11101111
;;       0x05 -> 0b11011111
;;       0x06 -> 0b10111111
;;       0x07 -> 0b01111111
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_HLP_GetBitANDMask
	BRA_IFSET WREG, 2, ACCESS, MIOS_HLP_GetBitANDMask_4567
MIOS_HLP_GetBitANDMask_0123
	BRA_IFSET WREG, 1, ACCESS, MIOS_HLP_GetBitANDMask_23
MIOS_HLP_GetBitANDMask_01
	btfss	WREG, 0
	retlw	b'11111110'
	retlw	b'11111101'
MIOS_HLP_GetBitANDMask_23
	btfss	WREG, 0
	retlw	b'11111011'
	retlw	b'11110111'

MIOS_HLP_GetBitANDMask_4567
	BRA_IFSET WREG, 1, ACCESS, MIOS_HLP_GetBitANDMask_67
MIOS_HLP_GetBitANDMask_45
	btfss	WREG, 0
	retlw	b'11101111'
	retlw	b'11011111'
MIOS_HLP_GetBitANDMask_67
	btfss	WREG, 0
	retlw	b'10111111'
	retlw	b'01111111'

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_GetBitORMask
;;  C_DECLARATION: unsigned char MIOS_HLP_GetBitORMask(unsigned char value);
;;  DESCRIPTION: this help function is usefull for bit manipulations
;;  IN:	see map below
;;  C_IN: see map below
;;  OUT: 0x00 -> b'00000001'
;;       0x01 -> b'00000010'
;;       0x02 -> b'00000100'
;;       0x03 -> b'00001000'
;;       0x04 -> b'00010000'
;;       0x05 -> b'00100000'
;;       0x06 -> b'01000000'
;;       0x07 -> b'10000000'
;;  C_OUT: 0x00 -> 0b00000001
;;       0x01 -> 0b00000010
;;       0x02 -> 0b00000100
;;       0x03 -> 0b00001000
;;       0x04 -> 0b00010000
;;       0x05 -> 0b00100000
;;       0x06 -> 0b01000000
;;       0x07 -> 0b10000000
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_HLP_GetBitORMask
	BRA_IFSET WREG, 2, ACCESS, MIOS_HLP_GetBitORMask_4567
MIOS_HLP_GetBitORMask_0123
	BRA_IFSET WREG, 1, ACCESS, MIOS_HLP_GetBitORMask_23
MIOS_HLP_GetBitORMask_01
	btfss	WREG, 0
	retlw	b'00000001'
	retlw	b'00000010'
MIOS_HLP_GetBitORMask_23
	btfss	WREG, 0
	retlw	b'00000100'
	retlw	b'00001000'

MIOS_HLP_GetBitORMask_4567
	BRA_IFSET WREG, 1, ACCESS, MIOS_HLP_GetBitORMask_67
MIOS_HLP_GetBitORMask_45
	btfss	WREG, 0
	retlw	b'00010000'
	retlw	b'00100000'
MIOS_HLP_GetBitORMask_67
	btfss	WREG, 0
	retlw	b'01000000'
	retlw	b'10000000'

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_16bitAddSaturate
;;  C_DECLARATION: unsigned char MIOS_HLP_16bitAddSaturate(unsigned char add_value, unsigned int *ptr, unsigned int max_value)
;;  DESCRIPTION: adds a signed 8-bit value to a 16 bit value and saturates<BR>
;;  That means: if the resulting value is greater than the given max value,
;;  the result will be saturated to the max value. If the resulting value
;;  is less than 0, the result will be saturated to 0<BR>
;;  Important: the 16-bit value must be aligned to an even word address
;;  (0x10, 0x12, 0x14, ...). First address contains the low-byte and 
;;  the second address contains the high-byte
;;  IN:	  8-bit signed value in WREG
;;	  pointer to low/high byte which should be modified in FSR1
;;        low-byte of max value in MIOS_PARAMETER1
;;        high-byte of max value in MIOS_PARAMETER2
;;  C_IN: 8-bit signed value in <add_value>
;;	  pointer to variable which should be modified in <ptr>
;;        max value in <max_value>
;;  OUT:  returns new absolute value in FSR1
;;        WREG and MIOS_PARAMETER1[0] is 1, if the value has been changed, 0 if it
;;           is equal to the old value
;;  C_OUT: writes new absolute value into *ptr
;;        returns 1, if the value has been changed, 0 if it
;;           is equal to the old value
;;  USES: BSR, INDF1 (FSR1 untouched), MIOS_PARAMETER[123]
;;  EXAMPLE:
;;
;;	;; subtract -5 from the 16-bit value
;;      ;; [FSR1] == INDF1 is 0x0002
;;      ;; max-value is 0x0fff
;;      movlw   0xff			; low-byte of max value
;;      movwf   MIOS_PARAMETER1
;;      movlw   0x0f			; high-byte of max value
;;      movwf   MIOS_PARAMETER2
;;	movlw	-5			; incrementer is -5
;;	call	MIOS_HLP_16bitAddSaturate
;;      ;; now INDF1 contains 0x0000 due to saturation
;;      ;; MIOS_PARAMETER1[0] is 1 since the value has been changed
;;
;;      ;; subtract -5 again
;;      ;; [FSR1] == INDF1 is 0x0000
;;      movlw   0xff			; low-byte of max value
;;      movwf   MIOS_PARAMETER1
;;      movlw   0x0f			; high-byte of max value
;;      movwf   MIOS_PARAMETER2
;;	movlw	-5			; incrementer is -5
;;	call	MIOS_HLP_16bitAddSaturate
;;      ;; now INDF1 contains 0x0000 due to saturation
;;      ;; MIOS_PARAMETER1[0] is 0 since the value has NOT been changed
;; 
;; 
;; 
;;	;; add 30 to the 16-bit value
;;      ;; [FSR1] == INDF1 is 0x0ffe
;;      ;; max-value is 0x0fff
;;      movlw   0xff			; low-byte of max value
;;      movwf   MIOS_PARAMETER1
;;      movlw   0x0f			; high-byte of max value
;;      movwf   MIOS_PARAMETER2
;;	movlw	30			; incrementer is 30
;;	call	MIOS_HLP_16bitAddSaturate
;;      ;; now INDF1 contains 0x0fff due to saturation
;;      ;; MIOS_PARAMETER1[0] is 1 since the value has been changed
;; 
;;  C_EXAMPLE:
;;
;;	// subtract -5 from the 16-bit value (unsigned int)
;;	MIOS_HLP_16bitAddSaturate(-5, &value, 0x0fff);
;; 
;; --------------------------------------------------------------------------
MIOS_HLP_16bitAddSaturate
	movwf	MIOS_PARAMETER3			; save incrementer in MIOS_PARAMETER3

	bcf	FSR1L, 0			; switch to low-byte
	movff	INDF1, MIOS_TMP1		; store low-byte in TMP_L
	bsf	FSR1L, 0			; switch to high-byte
	movff	INDF1, MIOS_TMP2		; store low-byte in TMP_H
	bcf	FSR1L, 0			; switch to low-byte

	addwf	INDF1, F			; add (or subtract if negative) to low-byte
	bsf	FSR1L, 0			; switch to high-byte
	movlw	0x00				; if positive add 0 to high-byte
	btfsc	MIOS_PARAMETER3, 7		; if negative add 0xff to high-byte
	movlw	0xff
	addwfc	INDF1, F			; add to high-byte

	;; now saturate the absolute value, depending on addition or subtraction
	BRA_IFSET MIOS_PARAMETER3, 7, ACCESS, MIOS_HLP_16bitAddSaturate_Dec
MIOS_HLP_16bitAddSaturate_Inc
	;; if positive: saturate with max-value
	;; check if max value has been reached: max - current < 0

	bcf	FSR1L, 0	; switch to low-byte

	;; absolute value (low-byte) now in INDF1
	;; max value (low-byte) in MIOS_PARAMETER[12]
	;; calc MIOS_PARAMETER[12] - INDF1 (LH)
	movf	INDF1, W
	subwf	MIOS_PARAMETER1, W
	bsf	FSR1L, 0	; switch to high-byte
	movf	INDF1, W
	subwfb	MIOS_PARAMETER2, W

	;; skip saturation if result is not negative
	bnn	MIOS_HLP_16bitAddSaturate_Cont

	;; else copy max value to absolute value
	movff	MIOS_PARAMETER2, INDF1
	bcf	FSR1L, 0	; switch to low-byte
	movff	MIOS_PARAMETER1, INDF1
		
	rgoto	MIOS_HLP_16bitAddSaturate_Cont
	
MIOS_HLP_16bitAddSaturate_Dec
	;; if negative (counter clockwise turn): saturate with zero
	BRA_IFCLR INDF1, 7, ACCESS, MIOS_HLP_16bitAddSaturate_Cont; (result not negative)
	clrf	INDF1		; clear high-byte
	bcf	FSR1L, 0	; switch to low-byte
	clrf	INDF1		; clear low-byte
	;; 	rgoto	HN_ENC_SendVPot_Cont	; continue
MIOS_HLP_16bitAddSaturate_Cont

	;; set MIOS_PARAMETER[1] if value has been changed
	clrf	MIOS_PARAMETER1
	SET_BSR	MIOS_TMP1

	bcf	FSR1L, 0			; switch to low-byte
	movf	INDF1, W
	xorwf	MIOS_TMP1, W, BANKED
	skpz
	bsf	MIOS_PARAMETER1, 0

	bsf	FSR1L, 0			; switch to high-byte
	movf	INDF1, W
	xorwf	MIOS_TMP2, W, BANKED
	skpz
	bsf	MIOS_PARAMETER1, 0

	bcf	FSR1L, 0			; switch to low-byte
	movf	MIOS_PARAMETER1, W		; get parameter

	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_HLP_Dec2BCD
;;  C_DECLARATION: void MIOS_HLP_Dec2BCD(unsigned int value)
;;  DESCRIPTION: converts a 16-bit decimal value to BCD
;;  IN:	  low-byte in WREG
;;	  high-byte in MIOS_PARAMETER1
;;  C_IN: 16bit value in <value>
;;  OUT:  rightmost digits (n1*10^1 and n0*10^0) in WREG and MIOS_PARAMETER1
;;        middle digits (n3*10^3 and n2*10^2) in MIOS_PARAMETER2
;;        leftmost digit (n5*10^4) in MIOS_PARAMETER3
;;  C_OUT:  rightmost digits (n1*10^1 and n0*10^0) in MIOS_PARAMETER1
;;        middle digits (n3*10^3 and n2*10^2) in MIOS_PARAMETER2
;;        leftmost digit (n5*10^4) in MIOS_PARAMETER3
;;  USES: BSR, MIOS_PARAMETER[123]
;;  EXAMPLE:
;;
;;	;; get the BCD code from 12345
;;      movlw	(12345) >> 8		; store high-byte in MIOS_PARAMETER1
;;	movwf	MIOS_PARAMETER1
;;	movlw	(12345) & 0xff		; store low-byte in WREG
;;	call	MIOS_HLP_Dec2BCD	; convert to BCD code
;;	;; now:
;;	;;    MIOS_PARAMETER3 contains the hex-value 0x01
;;	;;    MIOS_PARAMETER2 contains the hex-value 0x23
;;	;;    MIOS_PARAMETER1 contains the hex-value 0x45
;;	;;    WREG            contains also the hex-value 0x45
;;  C_EXAMPLE:
;;
;;	// get the BCD code from 12345
;;	MIOS_HLP_Dec2BCD(12345);
;;	// now:
;;	//    MIOS_PARAMETER3 contains the hex-value 0x01
;;	//    MIOS_PARAMETER2 contains the hex-value 0x23
;;	//    MIOS_PARAMETER1 contains the hex-value 0x45
;; --------------------------------------------------------------------------
MIOS_HLP_Dec2BCD
;; NOTE: code from AN544 of Microchip
;; --[ BEGIN MACRO ] --------------------------------------------------------
MIOS_HLP_Dec2BCD_ADJUST MACRO reg
	movlw   0x03
	addwf   reg, W, BANKED
	btfsc	WREG, 3		; test if result > 7
	movwf   reg
	movlw   0x30
	addwf   reg,W, BANKED
	btfsc	WREG, 7		; test if result > 7
	movwf   reg, BANKED     ; save as MSD
	ENDM
;; --[ END MACRO ] ----------------------------------------------------------

	SET_BSR	MIOS_TMP1

	;; copy value to MIOS_PARAMETER[12]
	movff	MIOS_PARAMETER1, MIOS_PARAMETER2
	movwf	MIOS_PARAMETER1

	clrc			; clear the carry bit
	movlw   0x10
	movwf   MIOS_PARAMETER3	; (used as counter)
	clrf    MIOS_TMP3, BANKED
	clrf    MIOS_TMP2, BANKED
	clrf    MIOS_TMP1, BANKED
MIOS_HLP_Dec2BCD_Loop
	rlf     MIOS_PARAMETER1, F
	rlf     MIOS_PARAMETER2, F
	rlf     MIOS_TMP1, F, BANKED
	rlf     MIOS_TMP2, F, BANKED
	rlf     MIOS_TMP3, F, BANKED

	dcfsnz  MIOS_PARAMETER3, F	; (used as counter)
	rgoto	MIOS_HLP_Dec2BCD_End

	MIOS_HLP_Dec2BCD_ADJUST	MIOS_TMP3
	MIOS_HLP_Dec2BCD_ADJUST	MIOS_TMP2
	MIOS_HLP_Dec2BCD_ADJUST	MIOS_TMP1
	rgoto   MIOS_HLP_Dec2BCD_Loop

MIOS_HLP_Dec2BCD_End
	movff	MIOS_TMP3, MIOS_PARAMETER3
	movff	MIOS_TMP2, MIOS_PARAMETER2
	movf	MIOS_TMP1, W, BANKED
	movwf	MIOS_PARAMETER1
	return
