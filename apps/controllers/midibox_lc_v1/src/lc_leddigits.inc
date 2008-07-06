; $Id$
;
; MIDIbox LC LED Digits Driver
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: LC_LEDDIGITS_Update
;;  DESCRIPTION: This function should be called periodically by 
;;  USER_SR_Service_Prepare before the DOUT registers will be updated
;;  from MIOS
;;  IN:   -
;;  OUT:  -
;;  USES: -
;;  ISR:  yes! Never call this routine from the mainprogram, it must be
;;  called from an interrupt service routine like USER_SR_Service_Prepare
;;  because it's using FSR2 as pointer
;; --------------------------------------------------------------------------
LC_LEDDIGITS_Update
	;; increment LED digit counter, wrap at 8
	;; (0, 1, 2, ..., 7, 0, 1, 2, ...)
	SET_BSR	LC_LEDDIGITS_CTR		; (use bank mechanism so that the registers can be located at addresses >= 0x80)
	incf	LC_LEDDIGITS_CTR, W, BANKED
	andlw	0x07
	movwf	LC_LEDDIGITS_CTR, BANKED

	;; in fact we are serving two separate leddigit networks with 2 DOUT registers each
	;; in order to reduce the coding effort, we are defining a generic macro
	;; which is used two times with different parameters
LC_LEDDIGITS_MACRO MACRO LEDDIGITS_SR_SELECT_x, LEDDIGITS_SR_SEGMENTS_x, LC_LEDDIGITS_first

	;; get shift register pattern for the select line:
	;;  LED digit 0 -> b'10000000'
	;;  LED digit 1 -> b'01000000'
	;;  LED digit 2 -> b'00100000'
	;;  LED digit 3 -> b'00010000'
	;;  LED digit 4 -> b'00001000'
	;;  LED digit 5 -> b'00000100'
	;;  LED digit 6 -> b'00000010'
	;;  LED digit 7 -> b'00000001'
	;; which is delivered by LC_LEDDIGITS_SelectGet
	SET_BSR	LC_LEDDIGITS_CTR
	movf	LC_LEDDIGITS_CTR, W, BANKED	; add offset
	rcall	LC_LEDDIGITS_SelectGet

	;; move select pattern to the appr. shift register
	movwf	MIOS_PARAMETER1			; MIOS_PARAMETER1 contains the values
	movlw	LEDDIGITS_SR_SELECT_x		; WREG contains the number of the "select" shift register
	call	MIOS_DOUT_SRSet			; set DOUT register

	;; now determine the pattern for the selected digit
	lfsr	FSR2, LC_LEDDIGITS_first		; get pointer to digit value stored in register
	SET_BSR	LC_LEDDIGITS_CTR
	movf	LC_LEDDIGITS_CTR, W, BANKED	; add offset
	addwf	FSR2L, F
	movf	INDF2, W			; get value of LC_LEDDIGITS_x
	rcall	LC_LEDDIGITS_PatternGet		; and convert it to a pattern (7 segment code)

	;; activate dot if bit 6 is set
	btfsc	INDF2, 6
	andlw b'01111111'

	;; move segment pattern to the appr. shift register
	movwf	MIOS_PARAMETER1			; MIOS_PARAMETER1 contains the values
	movlw	LEDDIGITS_SR_SEGMENTS_x		; WREG contains the number of the "segments" shift register
	call	MIOS_DOUT_SRSet			; set DOUT register
	ENDM



	;; now we are using this macro two times with different parameters
	;; NOTE: in this way, even more digits could be handled - up to 64!!!	
	LC_LEDDIGITS_MACRO	LEDDIGITS_SR_SELECT_1, LEDDIGITS_SR_SEGMENTS_1, LC_MTC_DIGIT_0
	LC_LEDDIGITS_MACRO	LEDDIGITS_SR_SELECT_2, LEDDIGITS_SR_SEGMENTS_2, LC_MTC_DIGIT_8
	
	;; thats all
	return

	;; help routine which returns the select lines depending on the counter
LC_LEDDIGITS_SelectGet
	andlw	0x07		; ensure that the resulting number is in between 0x00 and 0x07 (8 entries)
	JUMPTABLE_2BYTES_UNSECURE
	retlw	b'10000000'	; select digit 0
	retlw	b'01000000'	; select digit 1
	retlw	b'00100000'	; select digit 2
	retlw	b'00010000'	; select digit 3
	retlw	b'00001000'	; select digit 4
	retlw	b'00000100'	; select digit 5
	retlw	b'00000010'	; select digit 6
	retlw	b'00000001'	; select digit 7



	;; help routine which converts a value to 7-segment code
LC_LEDDIGITS_PatternGet
	andlw	0x3f
	addlw	LC_LEDDIGITS_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	LC_LEDDIGITS_TABLE >> 8
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	return
	
;    a
;   ---
;  !   !
; f! g !b
;   ---
;  !   !
; e!   !c
;   ---
;    d   h
; 0 = on, 1 = off
; NOTE: the dod (h) will be set automatically by the driver above when bit 6 is 1

	;; due to MPASM imperfections we have to define 2 digits in one line! :-(

LC_LEDDIGITS_TABLE
                ; habcdefg
	db	b'11111111', b'10001000'
	db	b'11100000', b'10110001'
	db	b'11000010', b'10110000'
	db	b'10111000', b'10100001'
	db	b'11101000', b'11001111'
	db	b'11000011', b'11111000'
	db	b'11110001', b'10001001'
	db	b'11101010', b'11100010'

	db	b'10011000', b'10001100'
	db	b'11111010', b'10010010'
	db	b'11110000', b'11100011'
	db	b'11011000', b'11000001'
	db	b'11001000', b'11000100'
	db	b'10010010', b'10110001'
	db	b'11101100', b'10000111'
	db	b'10011101', b'11110111'

	db	b'11111111', b'11111111'
	db	b'11011101', b'10011100'
	db	b'10100100', b'10011000'
	db	b'10000010', b'11111101'
	db	b'10110001', b'10000111'
	db	b'10011100', b'11001110'
	db	b'11111011', b'11111110'
	db	b'11110111', b'11011010'

	db	b'10000001', b'11001111'
	db	b'10010010', b'10000110'
	db	b'11001100', b'10100100'
	db	b'10100000', b'10001111'
	db	b'10000000', b'10000100'
	db	b'11111111', b'10111011'
	db	b'11001110', b'11110110'
	db	b'11111000', b'10011010'

