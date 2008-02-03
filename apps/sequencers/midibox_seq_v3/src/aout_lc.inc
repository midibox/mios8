; $Id$
;
; AOUT LC driver
; 
; The pins to which the first MBHP_AOUT_LC module is connected have to be defined here:
;
#define AOUT_LC_LAT_RCLK	LATC	; The latch enable input
#define AOUT_LC_TRIS_RCLK	TRISC	; is connected to Port C.5
#define AOUT_LC_PIN_RCLK	5	; (CANNOT be shared with other outputs!)
;
#define AOUT_LC_LAT_DOUT	LATD	; The data pin
#define AOUT_LC_TRIS_DOUT	TRISD	; is connected to Port D.4
#define AOUT_LC_PIN_DOUT	4	; (can be shared with other outputs)
;
#define AOUT_LC_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define AOUT_LC_TRIS_SCLK	TRISD	; is connected to Port D.5
#define AOUT_LC_PIN_SCLK	5	; (can be shared with other outputs)

;; number of AOUT_LC modules (1-4, all other values not allowed!)
#define AOUT_LC_NUMBER_OF_MODULES 4

; called from IRQ handler or not?
#define AOUT_LC_CALLED_FROM_ISR 0

;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_LC_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AOUT_LC_Init
	;; enable pin drivers
	bcf	AOUT_LC_TRIS_RCLK, AOUT_LC_PIN_RCLK
	bcf	AOUT_LC_TRIS_DOUT, AOUT_LC_PIN_DOUT
	bcf	AOUT_LC_TRIS_SCLK, AOUT_LC_PIN_SCLK

	;; set voltages to v/2
	lfsr	FSR1, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
AOUT_LC_InitVoutLoop
	movlw	0x80
	movwf	POSTINC1
	clrf	POSTINC1
	decfsz	MIOS_PARAMETER3, F
	rgoto	AOUT_LC_InitVoutLoop

	;; update the AOUT pins
	rcall	AOUT_LC_Update

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_LC_Load2SR
;;  DESCRIPTION: This function loads a 16bit value into the shift register
;;  IN:   o low byte of SR value in AOUT_SR_L
;;        o high byte of SR value in AOUT_SR_H
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
AOUT_LC_Load2SR
	SET_BSR	AOUT_SR_L

        bcf     AOUT_LC_LAT_SCLK, AOUT_LC_PIN_SCLK	; clear clock

	;; superfast transfer with unrolled loop (takes some memory, but guarantees the
	;; lowest system load :)
AOUT_LC_WRITE_BIT MACRO reg, bit
	bcf	AOUT_LC_LAT_DOUT, AOUT_LC_PIN_DOUT	; set out pin depending on register content (reg.bit)
	btfsc	reg, bit, BANKED
	bsf	AOUT_LC_LAT_DOUT, AOUT_LC_PIN_DOUT
	nop
        bsf     AOUT_LC_LAT_SCLK, AOUT_LC_PIN_SCLK	; rising clock edge
        bcf     AOUT_LC_LAT_SCLK, AOUT_LC_PIN_SCLK	; falling clock edge
	ENDM

	AOUT_LC_WRITE_BIT AOUT_SR_L, 7
	AOUT_LC_WRITE_BIT AOUT_SR_L, 6
	AOUT_LC_WRITE_BIT AOUT_SR_L, 5
	AOUT_LC_WRITE_BIT AOUT_SR_L, 4
	AOUT_LC_WRITE_BIT AOUT_SR_L, 3
	AOUT_LC_WRITE_BIT AOUT_SR_L, 2
	AOUT_LC_WRITE_BIT AOUT_SR_L, 1
	AOUT_LC_WRITE_BIT AOUT_SR_L, 0

	AOUT_LC_WRITE_BIT AOUT_SR_H, 7
	AOUT_LC_WRITE_BIT AOUT_SR_H, 6
	AOUT_LC_WRITE_BIT AOUT_SR_H, 5
	AOUT_LC_WRITE_BIT AOUT_SR_H, 4
	AOUT_LC_WRITE_BIT AOUT_SR_H, 3
	AOUT_LC_WRITE_BIT AOUT_SR_H, 2
	AOUT_LC_WRITE_BIT AOUT_SR_H, 1
	AOUT_LC_WRITE_BIT AOUT_SR_H, 0

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_LC_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
AOUT_LC_Update
	;; check if update required (bit #15 cleared)
	lfsr	FSR1, AOUT0_H
	clrf	TMP5
AOUT_LC_UpdateCheckLoop
	BRA_IFCLR INDF1, 7, ACCESS, AOUT_LC_UpdateCheckPassed
	movlw	2
	addwf	FSR1L, F
	incf	TMP5, F
	movlw	AOUT_LC_NUMBER_OF_MODULES*2
	cpfseq	TMP5, ACCESS
	rgoto AOUT_LC_UpdateCheckLoop
	rgoto	AOUT_LC_Update_End	; no update required
AOUT_LC_UpdateCheckPassed

#if AOUT_LC_CALLED_FROM_ISR == 0
	IRQ_DISABLE				; disable interrupts
#endif
	;; (for the case that interrupt driven pins are used)


	lfsr	FSR1, AOUT7_L
	clrf	TMP5
AOUT_LC_Update_Loop
	;; second 8bit AOUT
	movff	POSTINC1, AOUT_SR_L
	bsf	POSTINC1, 7	; notify that upload has been done

	;; switch to next entry
	movlw	-4
	addwf	FSR1L, F

	;; first 8bit AOUT
	movff	POSTINC1, AOUT_SR_H
	bsf	POSTINC1, 7	; notify that upload has been done

	;; switch to next entry
	movlw	-4
	addwf	FSR1L, F

	;; load SRs
	rcall	AOUT_LC_Load2SR

	incf	TMP5, F
	movlw	AOUT_LC_NUMBER_OF_MODULES
	cpfseq	TMP5, ACCESS
	rgoto AOUT_LC_Update_Loop

        bsf     AOUT_LC_LAT_RCLK, AOUT_LC_PIN_RCLK	; latch AOUT values
	bcf	AOUT_LC_LAT_DOUT, AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     AOUT_LC_LAT_RCLK, AOUT_LC_PIN_RCLK	; release latch

#if AOUT_LC_CALLED_FROM_ISR == 0
	IRQ_ENABLE				; enable interrupts again
#endif

AOUT_LC_Update_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: AOUT_LC_Pin7bitSet
;;  DESCRIPTION: This function sets a single AOUT_LC pin to a given 7-bit value
;;               The LSB is always 0
;;               After all AOUT_LC pins have been set, AOUT_LC_Update must be
;;               called in order to service the shift registers
;;  IN:   pin number in WREG, pin value in MIOS_PARAMETER1
;;  OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
AOUT_LC_Pin7bitSet
	;; calculate address to array
	lfsr	FSR1, AOUT0_L
	rlf	WREG, W
	andlw	0xfe
	addwf	FSR1L, F

	;; transfer 7bit value to array element (left-shift, LSB always 0)
	rlf	MIOS_PARAMETER1, W
	andlw	0xfe
	movwf	POSTINC1

	;; notify that value has been changed (AOUTx_H must be 0)
	bcf	POSTINC1, 7

	return
