; $Id$
;
; AOUT driver for daisychained MAX525
; 
; This include file provides following functions:
;    o AOUT_Init:       initializes the connected AOUT modules
;    o AOUT_PinSet:     user function to set a pin to a given 12-bit value
;    o AOUT_Pin7bitSet: user function to set a pin to a given 7-bit value
;    o AOUT_GatePinSet: user function to set the gate pin
;
; The pins to which the first AOUT module is connected have to be defined here:
;
#define AOUT_LAT_CS	LATC	; The chip select pin CS#
#define AOUT_TRIS_CS	TRISC	; is connected to Port C.5 
#define AOUT_PIN_CS	5	; (CANNOT be shared with other outputs!)
;
#define AOUT_LAT_DIN	LATC	; The data input pin DIN
#define AOUT_TRIS_DIN	TRISC	; is connected to Port C.4 
#define AOUT_PIN_DIN	4	; (can be shared with other outputs)
;
#define AOUT_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define AOUT_TRIS_SCLK	TRISD	; is connected to Port D.0
#define AOUT_PIN_SCLK	0	; (can be shared with other outputs)
;
; Number of connected MAX525
;
#define AOUT_NUMBER_OF_SR 2	; by default, 2 x MAX525 are connected to the daisychain
;				; -> makes 8 AOUTs
;
; called from IRQ handler or not?
#define AOUT_CALLED_FROM_ISR 1
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_Init
	;; enable pin drivers
	bcf	AOUT_TRIS_CS, AOUT_PIN_CS
	bcf	AOUT_TRIS_DIN, AOUT_PIN_DIN
	bcf	AOUT_TRIS_SCLK, AOUT_PIN_SCLK

	;; clear gate pins
	clrf	MIOS_PARAMETER3	; used as loop counter here
AOUT_InitGateLoop
	clrf	MIOS_PARAMETER1	; set gate pin to 0
	movf	MIOS_PARAMETER3, W
	rcall	AOUT_GatePinSet
	incf	MIOS_PARAMETER3, F
	movlw	AOUT_NUMBER_OF_SR-1
	cpfsgt	MIOS_PARAMETER3, ACCESS
	rgoto AOUT_InitGateLoop

	;; set voltages to 0V
	clrf	MIOS_PARAMETER3	; used as loop counter here
AOUT_InitVoutLoop
	clrf	MIOS_PARAMETER1	; set voltage to 0V
	clrf	MIOS_PARAMETER2
	movf	MIOS_PARAMETER3, W
	rcall	AOUT_PinSet
	incf	MIOS_PARAMETER3, F
	movlw	(AOUT_NUMBER_OF_SR*4)-1
	cpfsgt	MIOS_PARAMETER3, ACCESS
	rgoto AOUT_InitVoutLoop

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_PinSet
;;  DESCRIPTION: This function sets a single AOUT pin to a given 12-bit value
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER[12] (low/high byte)
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_PinSet
	;; temporary save number of pin here
	SET_BSR	AOUT_SR_NUMBER
	movwf	AOUT_SR_NUMBER, BANKED

	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movff	MIOS_PARAMETER1, AOUT_SR_L	; transfer DAC low byte to SR_L

	;; channel number = AOUT_SR_NUMBER << 6
	swapf	AOUT_SR_NUMBER, W, BANKED
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xc0
	
	iorlw	0x30				; set C1=1, C0=1

	iorwf	MIOS_PARAMETER2, W		; OR with DAC high byte
	movwf	AOUT_SR_H, BANKED		; result in SR_H

	;; fix the shift register number now: AOUT_SR_NUMBER >> 2
	clrc
	rrf	AOUT_SR_NUMBER, F, BANKED
	clrc
	rrf	AOUT_SR_NUMBER, F, BANKED
	;; load to shift register
	rgoto	AOUT_LoadSR


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_Pin7bitSet
;;  DESCRIPTION: This function converts a given 7-bit value to a 12-bit value
;;  and thereafter calls AOUT_PinSet
;;  IN:   pin number in WREG, 7bit pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_Pin7bitSet
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

	;; load pin number again and continue at AOUT_PinSet
	movf	AOUT_SR_NUMBER, W, BANKED
	rgoto	AOUT_PinSet


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_GatePinSet
;;  DESCRIPTION: This function sets the gate pin of a MAX525 to a given value
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_GatePinSet
	;; save number of SR (which corresponds to the gate pin number)
	SET_BSR	AOUT_SR_NUMBER
	movwf	AOUT_SR_NUMBER, BANKED

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	AOUT_SR_L, BANKED
	movlw	0x20				; set C1=1
	btfsc	MIOS_PARAMETER1, 0; set A0=1 if MIOS_PARAMETER1[0] is set
	iorlw 0x40	
	movwf	AOUT_SR_H, BANKED
	;; load to shift register
	rgoto	AOUT_LoadSR


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_LoadSR
;;  DESCRIPTION: This function loads a MAX525 shift register directly
;;  IN:   o number of MAX525 in AOUT_SR_NUMBER
;;        o low byte of SR value in AOUT_SR_L
;;        o high byte of SR value in AOUT_SR_H
;;  OUT:  -
;;  USES: BSR --- AOUT_SR_[LH] contains unknown values after the load procedure
;; --------------------------------------------------------------------------
AOUT_LoadSR
#if AOUT_CALLED_FROM_ISR == 0
	IRQ_DISABLE				; disable interrupts
#endif
	;; (for the case that interrupt driven pins are used)

        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; clear clock
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select

	;; we have to load the shift register in three steps:
	;;  o first step: load a NOP into every shift register behind the selected shift register
	;;  o second step: load the command into the selected shift register
	;;  o last step: load a NOP into every shift register before the selected shift register
	;; means: with every call of this function, all shift registers are loaded

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	SET_BSR	AOUT_SR_CTR
	movlw	AOUT_NUMBER_OF_SR-1		; init outer loop counter
	movwf	AOUT_SR_CTR, BANKED
AOUT_LoadSR_OuterLoop
	clrf	AOUT_SR_BIT_CTR, BANKED		; clear inner loop counter
	
	;; if AOUT_SR_CTR == AOUT_SR_NUMBER: load data
	;; else load zeros (NOP command)
	movf	AOUT_SR_CTR, W, BANKED
	cpfseq	AOUT_SR_NUMBER, BANKED
	rgoto AOUT_LoadSR_NOP
AOUT_LoadSR_Value
AOUT_LoadSR_Value_Loop
	bcf	AOUT_LAT_DIN, AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	AOUT_SR_H, 7, BANKED
	bsf	AOUT_LAT_DIN, AOUT_PIN_DIN
	rlf	AOUT_SR_L, F, BANKED		; start to shift the 16-bit value
	bsf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; rising clock edge
	rlf	AOUT_SR_H, F, BANKED		; finish the 16-bit shift
	bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; falling clock edge
	incf	AOUT_SR_BIT_CTR, F, BANKED	; loop 16 times
	BRA_IFCLR AOUT_SR_BIT_CTR, 4, BANKED, AOUT_LoadSR_Value_Loop
	rgoto	AOUT_LoadSR_Next

AOUT_LoadSR_NOP
	;; a NOP command consists of 16 zeroes
	bcf	AOUT_LAT_DIN, AOUT_PIN_DIN	; clear DIN
AOUT_LoadSR_NOP_Loop
	bsf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; rising clock edge
	incf	AOUT_SR_BIT_CTR, F, BANKED	; loop 16 times
	bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; falling clock edge
	BRA_IFCLR AOUT_SR_BIT_CTR, 4, BANKED, AOUT_LoadSR_NOP_Loop
	;; 	rgoto	AOUT_LoadSR_Next

AOUT_LoadSR_Next
	;; loop until last register reached
	decf	AOUT_SR_CTR, F, BANKED
	bc	AOUT_LoadSR_OuterLoop

	bsf	AOUT_LAT_CS, AOUT_PIN_CS	; deactivate chip select

#if AOUT_CALLED_FROM_ISR == 0
	IRQ_ENABLE				; enable interrupts again
#endif
	return
