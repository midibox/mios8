; $Id$
;
; AOUT_NG driver for TLV5630
; 
; This include file provides following functions:
;    o AOUT_Init:       initializes the connected AOUT modules
;    o AOUT_PinSet:     user function to set a pin to a given 12-bit value
;    o AOUT_Pin7bitSet: user function to set a pin to a given 7-bit value
;
; The pins to which the first AOUT module is connected have to be defined here:
;
#define AOUT_NG_LAT_CS		LATC	; The chip select pin CS#
#define AOUT_NG_TRIS_CS		TRISC	; is connected to Port C.5 
#define AOUT_NG_PIN_CS		5	; (CANNOT be shared with other outputs!)
;
#define AOUT_NG_LAT_DIN		LATD	; The data input pin DIN
#define AOUT_NG_TRIS_DIN	TRISD	; is connected to Port D.4 (AND NOT C.4 like the standard wiring!)
#define AOUT_NG_PIN_DIN		4	; (can be shared with other outputs)
;
#define AOUT_NG_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define AOUT_NG_TRIS_SCLK	TRISD	; is connected to Port D.5
#define AOUT_NG_PIN_SCLK	5	; (can be shared with other outputs)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_NG_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_NG_Init
	;; enable pin drivers
	bcf	AOUT_NG_TRIS_CS, AOUT_NG_PIN_CS
	bcf	AOUT_NG_TRIS_DIN, AOUT_NG_PIN_DIN
	bcf	AOUT_NG_TRIS_SCLK, AOUT_NG_PIN_SCLK

	;; deactivate chip select
	bsf	AOUT_NG_LAT_CS, AOUT_NG_PIN_CS

	;; set voltages to 0V
	clrf	MIOS_PARAMETER3	; used as loop counter here
AOUT_NG_InitVoutLoop
	clrf	MIOS_PARAMETER1	; set voltage to 0V
	clrf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER3, W
	rcall	AOUT_NG_PinSet
	incf	MIOS_PARAMETER3, F
	movlw	8
	cpfsgt	MIOS_PARAMETER3, ACCESS
	rgoto	AOUT_NG_InitVoutLoop

	;; initialize CTRL0
	movlw	(1 << 3) | (3 << 1) ; DO=1 (DOUT Enable), R=3 (internal reference, 2V)
	SET_BSR	AOUT_SR_L
	movwf	AOUT_SR_L, BANKED
	movlw	0x8 << 4
	movwf	AOUT_SR_H, BANKED
	rcall	AOUT_NG_Load2SR
	
	;; initialize CTRL1
	movlw	0x00
	SET_BSR	AOUT_SR_L
	movwf	AOUT_SR_L, BANKED
	movlw	0x9 << 4
	movwf	AOUT_SR_H, BANKED
	rcall	AOUT_NG_Load2SR

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_NG_PinSet
;;  DESCRIPTION: This function sets a single AOUT_NG pin to a given 12-bit value
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER[12] (low/high byte)
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_NG_PinSet
	;; temporary save number of pin here
	SET_BSR	AOUT_SR_NUMBER
	movwf	AOUT_SR_NUMBER, BANKED

	;; commands to load and update the DAC register:
	;; bit [15] = 0, bit [14:12] channel number, bit [11:0] DAC value
	movff	MIOS_PARAMETER1, AOUT_SR_L	; transfer DAC low byte to SR_L

	;; channel number = AOUT_SR_NUMBER << 4
	swapf	AOUT_SR_NUMBER, W, BANKED
	andlw	0x70
	iorwf	MIOS_PARAMETER2, W		; OR with DAC high byte
	movwf	AOUT_SR_H, BANKED		; result in SR_H

	;; load to shift register
	rgoto	AOUT_NG_Load2SR


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_NG_Pin7bitSet
;;  DESCRIPTION: This function converts a given 7-bit value to a 12-bit value
;;  and thereafter calls AOUT_NG_PinSet
;;  IN:   pin number in WREG, 7bit pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_NG_Pin7bitSet
	;; temporary save number of pin here
	SET_BSR	AOUT_SR_NUMBER
	movwf	AOUT_SR_NUMBER, BANKED

	;; convert 7-bit to 12-bit
	rlf	MIOS_PARAMETER1, W
	swapf	WREG, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2

	rlf	MIOS_PARAMETER1, W
	swapf	WREG, W
	andlw	0xf0
	movwf	MIOS_PARAMETER1

	;; load pin number again and continue at AOUT_NG_PinSet
	movf	AOUT_SR_NUMBER, W, BANKED
	rgoto	AOUT_NG_PinSet


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_NG_GatePinSet
;;  DESCRIPTION: This function sets the gate pin of a MAX525 to a given value
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_NG_GatePinSet
	;; save number of SR (which corresponds to the gate pin number)
	SET_BSR	AOUT_SR_NUMBER
	movwf	AOUT_SR_NUMBER, BANKED

	;; ignore pins >= 2
	movlw	2
	cpfslt	AOUT_SR_NUMBER, BANKED
	return

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	AOUT_SR_L, BANKED
	movlw	0x20				; set C1=1
	btfsc	MIOS_PARAMETER1, 0		; set A0=1 if MIOS_PARAMETER1[0] is set
	iorlw	0x40	
	movwf	AOUT_SR_H, BANKED
	;; load to shift register
	rgoto	AOUT_NG_Load2SR

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_NG_Load2SR
;;  DESCRIPTION: This function loads a value into a single TLV5630
;;  IN:   o low byte of first SR value in AOUT_SR_L
;;        o high byte of first SR value in AOUT_SR_H
;;  OUT:  -
;;  USES: AOUT_SR_BIT_CTR and AOUT_SR_[LH]
;; --------------------------------------------------------------------------
AOUT_NG_Load2SR
	;; (for the case that interrupt driven pins are used)

        bcf	AOUT_NG_LAT_SCLK, AOUT_NG_PIN_SCLK	; clear clock
	bcf	AOUT_NG_LAT_CS, AOUT_NG_PIN_CS		; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	SET_BSR	AOUT_SR_BIT_CTR
	movlw	16	; init loop counter
	movwf	AOUT_SR_BIT_CTR, BANKED
AOUT_NG_Load2SR_Loop
	bcf	AOUT_NG_LAT_DIN, AOUT_NG_PIN_DIN	; set DIN depending on current MSB
	btfsc	AOUT_SR_H, 7, BANKED
	bsf	AOUT_NG_LAT_DIN, AOUT_NG_PIN_DIN
	rlf	AOUT_SR_L, F, BANKED			; start to shift the 16-bit value
	bsf	AOUT_NG_LAT_SCLK, AOUT_NG_PIN_SCLK	; rising clock edge
	rlf	AOUT_SR_H, F, BANKED			; second step for the 16-bit shift
	bcf	AOUT_NG_LAT_SCLK, AOUT_NG_PIN_SCLK	; falling clock edge
	decfsz	AOUT_SR_BIT_CTR, F, BANKED		; loop 16 times
	rgoto	AOUT_NG_Load2SR_Loop

	bsf	AOUT_NG_LAT_CS, AOUT_NG_PIN_CS		; deactivate chip select

	return
