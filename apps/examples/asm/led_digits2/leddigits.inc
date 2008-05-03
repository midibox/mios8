; $Id$
;
; LED Digits Example 2
;
; Drives 16 digits with 4 shift registers (DOUTX4)
; See also LED Digits Example 1 which drives 8 digits with 3 shift registers (DOUTX3)
;
; This include file provides following functions:
;    o LEDDIGITS_Init:   initializes all digits
;    o LEDDIGITS_Update: see function header
;
; NOTE: some registers have to be inserted in app_defines.h to get this
;       driver working:
;
;LEDDIGITS_CTR		EQU	0x010	; used by J5_DIN_Update as multiplex counter
;LEDDIGITS_0		EQU	0x011	; stores digit #0
;
;LEDDIGITS_15		EQU	0x021	; stores digit #15
;
;       they have to be located to free register addresses!
;
; --> define the DOUT registers which are connected to the LED digits here:
; --> NOTE: we are counting from zero, means: first shift register is number 0, the second is number 1, ...
#define LEDDIGITS_SR_SEGMENTS_1		0	; shift register which drives the segments of digit 7-0 (right side)
#define LEDDIGITS_SR_SELECT_1		1	; shift register which selects the digits 7-0
#define LEDDIGITS_SR_SEGMENTS_2		2	; shift register which drives the segments of digit 15-8 (left side)
#define LEDDIGITS_SR_SELECT_2		3	; shift register which selects the digits 15-8
;
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: LEDDIGITS_Init
;;  DESCRIPTION: This function initializes the LED digit registers
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
LEDDIGITS_Init
	;; clear all digits
	lfsr	FSR0, LEDDIGITS_0	; start with digit #0
	movlw	LEDDIGITS_NUMBER	; how much digits have to be initialized?
	movwf	TMP1			; using TMP1 as loop counter
LEDDIGITS_Init_Loop
	clrf	PLUSW0			; clear register and increment pointer
	decfsz	TMP1, F			; decrement loop counter, skip if zero
	rgoto	LEDDIGITS_Init_Loop	; (loop until counter is zero)

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: LEDDIGITS_Update
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
LEDDIGITS_Update
	;; increment LED digit counter, wrap at 8
	;; (0, 1, 2, ..., 7, 0, 1, 2, ...)
	SET_BSR	LEDDIGITS_CTR		; (use bank mechanism so that the registers can be located at addresses >= 0x80)
	incf	LEDDIGITS_CTR, W, BANKED
	andlw	0x07
	movwf	LEDDIGITS_CTR, BANKED

	;; in fact we are serving two seperate leddigit networks with 2 DOUT registers each
	;; in order to reduce the coding effort, we are defining a generic macro
	;; which is used two times with different parameters
LEDDIGITS_MACRO MACRO LEDDIGITS_SR_SELECT_x, LEDDIGITS_SR_SEGMENTS_x, LEDDIGITS_first

	;; get shift register pattern for the select line:
	;;  LED digit 0 -> b'10000000'
	;;  LED digit 1 -> b'01000000'
	;;  LED digit 2 -> b'00100000'
	;;  LED digit 3 -> b'00010000'
	;;  LED digit 4 -> b'00001000'
	;;  LED digit 5 -> b'00000100'
	;;  LED digit 6 -> b'00000010'
	;;  LED digit 7 -> b'00000001'
	;; which is delivered by LEDDIGITS_SelectGet
	SET_BSR	LEDDIGITS_CTR
	movf	LEDDIGITS_CTR, W, BANKED	; add offset
	rcall	LEDDIGITS_SelectGet

	;; move select pattern to the appr. shift register
	movwf	MIOS_PARAMETER1			; MIOS_PARAMETER1 contains the values
	movlw	LEDDIGITS_SR_SELECT_x		; WREG contains the number of the "select" shift register
	call	MIOS_DOUT_SRSet			; set DOUT register

	;; now determine the pattern for the selected digit
	lfsr	FSR2, LEDDIGITS_first		; get pointer to digit value stored in register
	SET_BSR	LEDDIGITS_CTR
	movf	LEDDIGITS_CTR, W, BANKED	; add offset
	addwf	FSR2L, F
	movf	INDF2, W			; get value of LEDDIGITS_x
	rcall	LEDDIGITS_PatternGet		; and convert it to a pattern (7 segment code)

	;; activate dot if bit 7 is set
	btfsc	INDF2, 7
	andlw b'01111111'

	;; move segment pattern to the appr. shift register
	movwf	MIOS_PARAMETER1			; MIOS_PARAMETER1 contains the values
	movlw	LEDDIGITS_SR_SEGMENTS_x		; WREG contains the number of the "segments" shift register
	call	MIOS_DOUT_SRSet			; set DOUT register
	ENDM



	;; now we are using this macro two times with different parameters
	;; NOTE: in this way, even more digits could be handled - up to 64!!!	
	LEDDIGITS_MACRO	LEDDIGITS_SR_SELECT_1, LEDDIGITS_SR_SEGMENTS_1, LEDDIGITS_0
	LEDDIGITS_MACRO	LEDDIGITS_SR_SELECT_2, LEDDIGITS_SR_SEGMENTS_2, LEDDIGITS_8
	
	;; thats all
	return


	;; help routine which returns the select lines depending on the counter
LEDDIGITS_SelectGet
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
LEDDIGITS_PatternGet
	andlw	0x1f		; 32 patterns are supported here
	JUMPTABLE_2BYTES_UNSECURE
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
; NOTE: the dod (h) will be set automatically by the driver above when bit 7 is set
                ; habcdefg
	retlw	b'10000001' ; 0
	retlw	b'11001111' ; 1
	retlw	b'10010010' ; 2
	retlw	b'10000110' ; 3
	retlw	b'11001100' ; 4
	retlw	b'10100100' ; 5
	retlw	b'10100000' ; 6
	retlw	b'10001111' ; 7
	retlw	b'10000000' ; 8
	retlw	b'10000100' ; 9
	retlw	b'10001000' ; A
	retlw	b'11100000' ; B
	retlw	b'10110001' ; C
	retlw	b'11000010' ; D
	retlw	b'10110000' ; E
	retlw	b'10111000' ; F

	retlw	b'11111111' ; Digit Off
	retlw	b'01111111' ; Bar h
	retlw	b'10111111' ; Bar a
	retlw	b'11011111' ; Bar b
	retlw	b'11101111' ; Bar c
	retlw	b'11110111' ; Bar d
	retlw	b'11111011' ; Bar e
	retlw	b'11111101' ; Bar f
	retlw	b'11111110' ; Bar g
	retlw	b'11001111' ; Bar b+c
	retlw	b'11111001' ; Bar e+f
	retlw	b'11111111' ; Digit Off (here you could add your own patterns)
	retlw	b'11111111' ; Digit Off (here you could add your own patterns)
	retlw	b'11111111' ; Digit Off (here you could add your own patterns)
	retlw	b'11111111' ; Digit Off (here you could add your own patterns)
	retlw	b'11111111' ; Digit Off (here you could add your own patterns)
