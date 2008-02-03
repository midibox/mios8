; $Id$
;
; AOUT multiplexer driver
; 
; This include file provides following functions:
;    o AMUX_Init:       initializes the AMUX driver
;    o AMUX_Handler:    called by USER_Timer to update the s&h units periodically
;
;
; DRIVER HAS NOT BEEN FINISHED YET --- WILL BE SPEED OPTIMIZED IN THE FUTURE
;
;
#define AMUX_LAT_RCLK	LATD ;AOUT_LAT_CS
#define AMUX_TRIS_RCLK	TRISD ;AOUT_TRIS_CS
#define AMUX_PIN_RCLK	4 ;AOUT_PIN_CS
;
#define AMUX_LAT_DOUT	AOUT_LAT_DIN
#define AMUX_TRIS_DOUT	AOUT_TRIS_DIN
#define AMUX_PIN_DOUT	AOUT_PIN_DIN
;
#define AMUX_LAT_SCLK	AOUT_LAT_SCLK
#define AMUX_TRIS_SCLK	AOUT_TRIS_SCLK
#define AMUX_PIN_SCLK	AOUT_PIN_SCLK
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: AMUX_Init
;;  DESCRIPTION: This function initializes the multiplexer driver
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AMUX_Init
	clrf	AMUX_CTR

	bcf	AMUX_LAT_RCLK, AMUX_PIN_RCLK
	bcf	AMUX_TRIS_RCLK, AMUX_PIN_RCLK

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: AMUX_Handler
;;  DESCRIPTION: This function has to be called periodically from USER_Timer
;;  to update the sample & hold units
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
AMUX_LOAD_SRS	MACRO	channel
	ENDM

AMUX_Handler
	incf	AMUX_CTR, F
	movlw	DEFAULT_NUMBER_AOUTS
	cpfslt	AMUX_CTR, ACCESS
	clrf AMUX_CTR

	movf	AMUX_CTR, W
	iorlw	0x08
	call	AMUX_WriteSR

	lfsr	FSR2, MB64_POT_VALUES_00

	movlw	(0 << 6) | 0x30	; channel #0 and #4
	movwf	MIOS_PARAMETER1
	movlw	0x00
	call	AMUX_WriteAOUT2

	movlw	(1 << 6) | 0x30	; channel #1 and #5
	movwf	MIOS_PARAMETER1
	movlw	0x08
	call	AMUX_WriteAOUT2

	movlw	(2 << 6) | 0x30	; channel #2 and #6
	movwf	MIOS_PARAMETER1
	movlw	0x10
	call	AMUX_WriteAOUT2

	movlw	(3 << 6) | 0x30	; channel #3 and #7
	movwf	MIOS_PARAMETER1
	movlw	0x18
	call	AMUX_WriteAOUT2

	;; wait until AOUTs settled
	movlw	80
	movwf	IRQ_TMP1
AMUX_Handler_InhLoop1
	decfsz	IRQ_TMP1, F
	rgoto	AMUX_Handler_InhLoop1

	;; strobe Inhibit signal
	movf	AMUX_CTR, W
	call	AMUX_WriteSR

	;; for ca. 1-5 us (according to datasheet)
	movlw	10
	movwf	IRQ_TMP1
AMUX_Handler_InhLoop2
	decfsz	IRQ_TMP1, F
	rgoto	AMUX_Handler_InhLoop2

	movf	AMUX_CTR, W
	iorlw	0x08
	call	AMUX_WriteSR

AMUX_Handler_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AMUX_WriteSR
;;  DESCRIPTION: This function loads the DOUT shift register
;;  IN:   data in WREG
;;  OUT:  -
;;  USES: MIOS_PARAMETER[12], BSR
;; --------------------------------------------------------------------------
AMUX_WriteSR
	movwf	MIOS_PARAMETER1			; save value in MIOS_PARAMETER1
        bcf     AMUX_LAT_SCLK, AMUX_PIN_SCLK	; clear clock
	clrf	MIOS_PARAMETER2			; MIOS_PARAMETER2 used as shift counter
AMUX_WriteSRLoop
	bcf	AMUX_LAT_DOUT, AMUX_PIN_DOUT	; set out pin depending on register content (reg.bit)
	btfsc	MIOS_PARAMETER1, 7
	bsf	AMUX_LAT_DOUT, AMUX_PIN_DOUT
	rlf	MIOS_PARAMETER1, F
        bsf     AMUX_LAT_SCLK, AMUX_PIN_SCLK	; rising clock edge
	incf	MIOS_PARAMETER2, F
        bcf     AMUX_LAT_SCLK, AMUX_PIN_SCLK	; falling clock edge
	BRA_IFCLR MIOS_PARAMETER2, 3, ACCESS, AMUX_WriteSRLoop
	bcf	AMUX_LAT_DOUT, AMUX_PIN_DOUT	; clear out pin (standby)

        bsf     AMUX_LAT_RCLK, AMUX_PIN_RCLK	; latch value
	nop
        bcf     AMUX_LAT_RCLK, AMUX_PIN_RCLK	; release latch
	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: AMUX_WriteAOUT2
;;  DESCRIPTION: This function loads two MAX525 shift registers at once
;;  IN:   pot offset in WREG
;;        ((channel number << 6) | 0x30) in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: IRQ_TMP[12345]
;; --------------------------------------------------------------------------
AMUX_WriteAOUT2
	;; save pot offset in IRQ_TMP5
	movwf	IRQ_TMP5

	;; convert 7-bit value for first SR
	addwf	AMUX_CTR, W
	movf	PLUSW2, W

	movwf	IRQ_TMP1
	rlf	IRQ_TMP1, W
	swapf	WREG, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER1, W
	movwf	IRQ_TMP2

	rlf	IRQ_TMP1, W
	swapf	WREG, W
	andlw	0xf0
	movwf	IRQ_TMP1


	;; convert 7-bit value for second SR
	movf	IRQ_TMP5, W
	addlw	0x20
	addwf	AMUX_CTR, W
	movf	PLUSW2, W

	movwf	IRQ_TMP3
	rlf	IRQ_TMP3, W
	swapf	WREG, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER1, W
	movwf	IRQ_TMP4

	rlf	IRQ_TMP3, W
	swapf	WREG, W
	andlw	0xf0
	movwf	IRQ_TMP3


	;; write SRs
        bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; clear clock
	bcf	AOUT_LAT_CS, AOUT_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	movlw	32				; init loop counter
	movwf	IRQ_TMP5
AMUX_WriteAOUT2_Loop
	bcf	AOUT_LAT_DIN, AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	IRQ_TMP4, 7
	bsf	AOUT_LAT_DIN, AOUT_PIN_DIN
	rlf	IRQ_TMP1, F				; start to shift the 32-bit value
	rlf	IRQ_TMP2, F				; second step for the 32-bit shift
	bsf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; rising clock edge
	rlf	IRQ_TMP3, F				; third step for the 32-bit shift
	rlf	IRQ_TMP4, F				; last step for the 32-bit shift
	bcf	AOUT_LAT_SCLK, AOUT_PIN_SCLK	; falling clock edge
	decfsz	IRQ_TMP5, F				; loop 32 times
	rgoto	AMUX_WriteAOUT2_Loop

	bsf	AOUT_LAT_CS, AOUT_PIN_CS; deactivate chip select

	return


