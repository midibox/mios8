; $Id$
;
; SID Control Surface Modulation Matrix Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function drives the modulation matrix (8*7 LEDs)
;;  it's called periodically every SR update cycle
;;  by sid_srio.inc, USER_SR_Service_Prepare
;;
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Handler
	;; check if meters should be displayed
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	bsf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	movf	CS_MENU_MATRIX_SELECTED_COL, W		; don't display if one of the column buttons is pressed
	skpz
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP

	;; increment line counter, wrap at 8 (0, 1, 2, ... 7, 0, 1, 2, ...)
	incf	CS_MENU_MATRIX_CTR, W
	andlw	0x07
	movwf	CS_MENU_MATRIX_CTR

	;; on overrun, increment flash counter
	skpnz
	incf	CS_MENU_MATRIX_FLASH_CTR, F

	;; when CS_MENU_MODE_MATRIX_METER_DISP set:
	;;    get shift register pattern for the LED anodes
	;;    this results into following bitpattern
	;;     O1 Pitch column: b'00000001'
	;;     O2 Pitch column: b'00000010'
	;;     O3 Pitch column: b'00000100'
	;;     O1 PW column:    b'00001000'
	;;     O2 PW column:    b'00010000'
	;;     O3 PW column:    b'00100000'
	;;     Filter column:   b'01000000'
	;;    which is delivered by CS_MENU_MATRIX_CAGet
	;; 
	;; when CS_MENU_MODE_MATRIX_METER_DISP NOT set:
	;;    get shift register pattern for the LED cathodes and invert
	;;    this results into following bitpattern
	;;     E1 row: b'11111110'
	;;     E2 row: b'11111101'
	;;     L1 row: b'11111011'
	;;     L2 row: b'11110111'
	;;     L3 row: b'11101111'
	;;     L4 row: b'11011111'
	;;     L5 row: b'10111111'
	;;     L6 row: b'01111111'
	;;    which is delivered by CS_MENU_MATRIX_CAGet
	movf	CS_MENU_MATRIX_CTR, W
#if 0
	rcall	CS_MENU_MATRIX_CAGet
#else
	call	MIOS_HLP_GetBitORMask	; (identical)
#endif
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, CS_MENU_MATRIX_Handler_UseA
CS_MENU_MATRIX_Handler_UseC
	xorlw	0xff		; if cathodes are used: invert result
CS_MENU_MATRIX_Handler_UseA
	;; set pattern by calling MIOS_DOUT_SRSet
	movwf	MIOS_PARAMETER1
	movlw	MOD_MATRIX_CATHODES
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	movlw MOD_MATRIX_ANODES
	call	MIOS_DOUT_SRSet

	;; now load the anode shift registers to set the ring patterns
	;; the patterns are saved in CS_MENU_MATRIX_BEGIN[0..7]
	;; note: since this is an interrupt service routine, we have to use FSR2 instead of FSR0!
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, CS_MENU_MATRIX_Handler_NoM
CS_MENU_MATRIX_Handler_M
	lfsr	FSR2, SID_BASE	; get offset to "RVALUE" of modulation source
	movf	CS_MENU_MATRIX_CTR, W
	rcall	CS_MENU_MATRIX_MODOffGet
	addwf	FSR2L, F
	movf	INDF2, W
	rcall	CS_MENU_MATRIX_MODPatGet
	rgoto	CS_MENU_MATRIX_Handler_M_Cont

CS_MENU_MATRIX_Handler_NoM
	lfsr	FSR2, CS_MENU_MATRIX_BEGIN		; calc pointer to pattern
	movf	CS_MENU_MATRIX_CTR, W			; add row number to pointer
	movf	PLUSW2, W				; transfer value of [INDF2] to MIOS_PARAMETER1
CS_MENU_MATRIX_Handler_M_Cont

	movwf	MIOS_PARAMETER1
	;; flash if the same row is currently selected
	movf	CS_MENU_MATRIX_FLASH_CTR, W; every 8th cycle: invert columb
	andlw	0x38
	bnz	CS_MENU_MATRIX_Handler_NoFlash
	movf	CS_MENU_MATRIX_CTR, W
#if 0
	rcall	CS_MENU_MATRIX_CAGet
#else
	call	MIOS_HLP_GetBitORMask	; (identical)
#endif
	andwf	CS_MENU_MATRIX_SELECTED_COL, W
	bz	CS_MENU_MATRIX_Handler_NoFlash
	comf	MIOS_PARAMETER1, F
CS_MENU_MATRIX_Handler_NoFlash
	movlw	MOD_MATRIX_ANODES
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	movlw MOD_MATRIX_CATHODES
	call	MIOS_DOUT_SRSet

	;; thats all!
	return


#if 0
	;; help routine which returns the anode line which has to be set for the appr. column or row
	;; note: now we use the MIOS_HLP_GetBitORMask routine which delivers exactly the same results
CS_MENU_MATRIX_CAGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'00000001'
	retlw	b'00000010'
	retlw	b'00000100'
	retlw	b'00001000'
	retlw	b'00010000'
	retlw	b'00100000'
	retlw	b'01000000'
	retlw	b'10000000'
#endif

	;; help routine which returns offset to modulator value (for meter mode)
CS_MENU_MATRIX_MODOffGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	(SID_ENV1_BASE + SID_ENVx_RVALUE_H) & 0xff
	retlw	(SID_ENV2_BASE + SID_ENVx_RVALUE_H) & 0xff
	retlw	(SID_LFO1_BASE + SID_LFOx_RVALUE_H) & 0xff
	retlw	(SID_LFO2_BASE + SID_LFOx_RVALUE_H) & 0xff
	retlw	(SID_LFO3_BASE + SID_LFOx_RVALUE_H) & 0xff
	retlw	(SID_LFO4_BASE + SID_LFOx_RVALUE_H) & 0xff
	retlw	(SID_LFO5_BASE + SID_LFOx_RVALUE_H) & 0xff
	retlw	(SID_LFO6_BASE + SID_LFOx_RVALUE_H) & 0xff

	;; help routine which returns the modulation pattern for the vertical line (inverted!)
CS_MENU_MATRIX_MODPatGet
	addlw	0x80
	movwf	TABLAT		; all LEDs off if value around 0
	andlw	0xf0
	xorlw	0x80
	bnz	CS_MENU_MATRIX_MODPatGet_Not0
	retlw	0xff
CS_MENU_MATRIX_MODPatGet_0
CS_MENU_MATRIX_MODPatGet_Not0
	swapf	TABLAT, W
	rrf	WREG, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'00000111'
	retlw	b'01000111'
	retlw	b'01100111'
	retlw	b'01110111'
	retlw	b'01110111'
	retlw	b'01110011'
	retlw	b'01110001'
	retlw	b'01110000'
