; $Id$
;
; AOUT LC driver
; 
; Special Variation of the official "aout.inc" module for
; MIDIbox FM to achive best performance!
; This module works with one AOUT_LC module maximum
; 
; The pins to which the first MBHP_AOUT_LC module is connected have to be defined here:
;
#define MBFM_AOUT_LC_LAT_RCLK	LATC	; The latch enable input
#define MBFM_AOUT_LC_TRIS_RCLK	TRISC	; is connected to Port C.3 
#define MBFM_AOUT_LC_PIN_RCLK	3	; (CANNOT be shared with other outputs!)
;
#define MBFM_AOUT_LC_LAT_DOUT	LATC	; The data pin
#define MBFM_AOUT_LC_TRIS_DOUT	TRISC	; is connected to Port C.1
#define MBFM_AOUT_LC_PIN_DOUT	1	; (can be shared with other outputs)
;
#define MBFM_AOUT_LC_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define MBFM_AOUT_LC_TRIS_SCLK	TRISC	; is connected to Port C.0
#define MBFM_AOUT_LC_PIN_SCLK	0	; (can be shared with other outputs)

; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_LC_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MBFM_AOUT_LC_Init
	;; enable pin drivers
	bcf	MBFM_AOUT_LC_TRIS_RCLK, MBFM_AOUT_LC_PIN_RCLK
	bcf	MBFM_AOUT_LC_TRIS_DOUT, MBFM_AOUT_LC_PIN_DOUT
	bcf	MBFM_AOUT_LC_TRIS_SCLK, MBFM_AOUT_LC_PIN_SCLK

	;; clear the AOUT gate pins
	movlw	0xf0
	movwf	AOUT_GATE

	;; set voltages to 1/2 V_max
	movlw	0x07		; default assign flags (only taken by first AOUT)
	movwf	TMP1
	movlw	0x40		; 7-bit value of default offset
	movwf	TMP2
	movlw	0x08		; 12-bit value is 0x800
	movwf	TMP3
	lfsr	FSR0, AOUT1_BASE
	movlw	8
	movwf	TMP5		; used as loop counter here
MBFM_AOUT_LC_InitVoutLoop
	movlw	AOUTx_ASSIGN	; set default assigns
	movff	TMP1, PLUSW0
	movlw	0x10		; increment instrument number for default assigns (wrap at 4)
	addwf	TMP1, W
	andlw	0x30		; note: all other AOUTs are not assigned to LLE!
	movwf	TMP1

	movlw	AOUTx_OFFSET	; set default offset
	movff	TMP2, PLUSW0

	movlw	AOUTx_L		; clear low byte
	clrf	PLUSW0
	movlw	AOUTx_H		; copy init value to high byte
	movff	TMP3, PLUSW0

	movlw	AOUT_RECORD_LEN	; switch to next record
	addwf	FSR0L, F
	decfsz	TMP5, F		; loop
	rgoto	MBFM_AOUT_LC_InitVoutLoop

	;; update the AOUT pins
	rcall	MBFM_AOUT_LC_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_LC_Load2SR
;;  DESCRIPTION: This function loads a 16bit value into the shift register
;;  IN:   o low byte of SR value in TMP1
;;        o high byte of SR value in TMP2
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
MBFM_AOUT_LC_Load2SR
        bcf     MBFM_AOUT_LC_LAT_SCLK, MBFM_AOUT_LC_PIN_SCLK	; clear clock

	;; superfast transfer with unrolled loop (takes some memory, but guarantees the
	;; lowest system load :)
MBFM_AOUT_LC_WRITE_BIT MACRO reg, bit
	bcf	MBFM_AOUT_LC_LAT_DOUT, MBFM_AOUT_LC_PIN_DOUT	; set out pin depending on register content (reg.bit)
	btfsc	reg, bit
	bsf	MBFM_AOUT_LC_LAT_DOUT, MBFM_AOUT_LC_PIN_DOUT
	nop
        bsf     MBFM_AOUT_LC_LAT_SCLK, MBFM_AOUT_LC_PIN_SCLK	; rising clock edge
        bcf     MBFM_AOUT_LC_LAT_SCLK, MBFM_AOUT_LC_PIN_SCLK	; falling clock edge
	ENDM

	MBFM_AOUT_LC_WRITE_BIT TMP1, 7
	MBFM_AOUT_LC_WRITE_BIT TMP1, 6
	MBFM_AOUT_LC_WRITE_BIT TMP1, 5
	MBFM_AOUT_LC_WRITE_BIT TMP1, 4
	MBFM_AOUT_LC_WRITE_BIT TMP1, 3
	MBFM_AOUT_LC_WRITE_BIT TMP1, 2
	MBFM_AOUT_LC_WRITE_BIT TMP1, 1
	MBFM_AOUT_LC_WRITE_BIT TMP1, 0

	MBFM_AOUT_LC_WRITE_BIT TMP2, 7
	MBFM_AOUT_LC_WRITE_BIT TMP2, 6
	MBFM_AOUT_LC_WRITE_BIT TMP2, 5
	MBFM_AOUT_LC_WRITE_BIT TMP2, 4
	MBFM_AOUT_LC_WRITE_BIT TMP2, 3
	MBFM_AOUT_LC_WRITE_BIT TMP2, 2
	MBFM_AOUT_LC_WRITE_BIT TMP2, 1
	MBFM_AOUT_LC_WRITE_BIT TMP2, 0

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_LC_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MBFM_AOUT_LC_Update

	;; check if update required
	lfsr	FSR0, AOUT1_BASE + AOUTx_H
	clrf	TMP5
MBFM_AOUT_LC_UpdateCheckLoop
	BRA_IFCLR INDF0, 7, ACCESS, MBFM_AOUT_LC_UpdateCheckPassed
	movlw	AOUT_RECORD_LEN
	addwf	FSR0L, F
	incf	TMP5, F
	movlw	8
	cpfseq	TMP5, ACCESS
	rgoto MBFM_AOUT_LC_UpdateCheckLoop
	rgoto	MBFM_AOUT_LC_Update_Gates	; no update required

MBFM_AOUT_LC_UpdateCheckPassed

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	lfsr	FSR0, AOUT7_BASE + AOUTx_L
	lfsr	FSR1, AOUT8_BASE + AOUTx_L
	clrf	TMP5
MBFM_AOUT_LC_Update_Loop
	;; 12bit AOUT
	swapf	INDF0, W	; AOUT_L transfer low/high byte of first SR to TMP[12]
	andlw	0xf0
	movwf	TMP1

	swapf	POSTINC0, W	; AOUT_L
	andlw	0x0f
	movwf	TMP2

	swapf	INDF0, W	; AOUT_H
	andlw	0xf0
	iorwf	TMP2, F
	bsf	INDF0, 7	; notify that upload has been done
	movf	POSTDEC0, W

	;; 4bit AOUT
	movf	PREINC1, W	; AOUT_H
	andlw	0x0f
	iorwf	TMP1, F
	bsf	INDF1, 7	; notify that upload has been done
	movf	POSTDEC1, W

	;; load SRs
	rcall	MBFM_AOUT_LC_Load2SR

	movlw	-2*AOUT_RECORD_LEN
	addwf	FSR0L, F
	addwf	FSR1L, F

	incf	TMP5, F
	movlw	8/2
	cpfseq	TMP5, ACCESS
	rgoto MBFM_AOUT_LC_Update_Loop


        bsf     MBFM_AOUT_LC_LAT_RCLK, MBFM_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	MBFM_AOUT_LC_LAT_DOUT, MBFM_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     MBFM_AOUT_LC_LAT_RCLK, MBFM_AOUT_LC_PIN_RCLK	; release latch

	IRQ_ENABLE			; enable interrupts


MBFM_AOUT_LC_Update_Gates	
	;; do nothing if gate bits have not been changed (lower 4 bits == upper 4 bits)
	swapf	AOUT_GATE, W
	xorwf	AOUT_GATE, W
	andlw	0x0f
	bz	MBFM_AOUT_LC_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency
	;; transfer the lower 4 bits to the upper 4 bits
	movlw	0x0f
	andwf	AOUT_GATE, F
	swapf	AOUT_GATE, W
	iorwf	AOUT_GATE, F
	IRQ_ENABLE			; enable interrupts

MBFM_AOUT_LC_UpdateGates_End
MBFM_AOUT_LC_Update_End

	return





;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_ParGet
;;  DESCRIPTION: returns an AOUT parameter
;;  IN: parameter number in WREG
;;  OUT:  parameter value in WREG
;; --------------------------------------------------------------------------
MBFM_AOUT_ParGet
	lfsr	FSR1, AOUT_BASE
	movf	PLUSW1, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_ParSet
;;  DESCRIPTION: writes to an AOUT parameter
;;  IN: parameter number in WREG
;;      parameter value in MIOS_PARAMETER1
;;  OUT:  -
;; --------------------------------------------------------------------------
MBFM_AOUT_ParSet
	lfsr	FSR1, AOUT_BASE
	movff	MIOS_PARAMETER1, PLUSW1
	return
