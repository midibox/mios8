; $Id$
;
; AOUT driver for TLV5630
; 
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; The pins to which the MBHP_AOUT_NG module is connected have to be defined here:
;
#define MBFM_AOUT_NG_LAT_CS	LATC	; The chip select pin CS#
#define MBFM_AOUT_NG_TRIS_CS	TRISC	; is connected to Port C.3 
#define MBFM_AOUT_NG_PIN_CS	3	; (CANNOT be shared with other outputs!)
;
#define MBFM_AOUT_NG_LAT_DIN	LATC	; The data input pin DIN
#define MBFM_AOUT_NG_TRIS_DIN	TRISC	; is connected to Port C.1
#define MBFM_AOUT_NG_PIN_DIN	1	; (can be shared with other outputs)
;
#define MBFM_AOUT_NG_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define MBFM_AOUT_NG_TRIS_SCLK	TRISC	; is connected to Port C.0
#define MBFM_AOUT_NG_PIN_SCLK	0	; (can be shared with other outputs)

; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_NG_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MBFM_AOUT_NG_Init
	;; enable pin drivers
	bcf	MBFM_AOUT_NG_TRIS_CS, MBFM_AOUT_NG_PIN_CS
	bcf	MBFM_AOUT_NG_TRIS_DIN, MBFM_AOUT_NG_PIN_DIN
	bcf	MBFM_AOUT_NG_TRIS_SCLK, MBFM_AOUT_NG_PIN_SCLK

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
MBFM_AOUT_NG_InitVoutLoop
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
	rgoto	MBFM_AOUT_NG_InitVoutLoop

	;; update the AOUT pins
	rcall	MBFM_AOUT_NG_Update

	;; initialize CTRL0
	movlw	(1 << 3) | (3 << 1) ; DO=1 (DOUT Enable), R=3 (internal reference, 2V)
	movwf	TMP1
	movlw	0x8 << 4
	movwf	TMP2
	rcall	MBFM_AOUT_NG_Load2SR
	
	;; initialize CTRL1
	movlw	0x00
	movwf	TMP1
	movlw	0x9 << 4
	movwf	TMP2
	rcall	MBFM_AOUT_NG_Load2SR

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_NG_Load2SR
;;  DESCRIPTION: This function loads a value into a single TLV5630
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
MBFM_AOUT_NG_Load2SR
        bcf	MBFM_AOUT_NG_LAT_SCLK, MBFM_AOUT_NG_PIN_SCLK	; clear clock
	bcf	MBFM_AOUT_NG_LAT_CS, MBFM_AOUT_NG_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	movlw	16	; init loop counter
	movwf	TMP5
MBFM_AOUT_NG_Load2SR_Loop
	bcf	MBFM_AOUT_NG_LAT_DIN, MBFM_AOUT_NG_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP2, 7
	bsf	MBFM_AOUT_NG_LAT_DIN, MBFM_AOUT_NG_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	bsf	MBFM_AOUT_NG_LAT_SCLK, MBFM_AOUT_NG_PIN_SCLK	; rising clock edge
	rlf	TMP2, F				; second step for the 32-bit shift
	bcf	MBFM_AOUT_NG_LAT_SCLK, MBFM_AOUT_NG_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 16 times
	rgoto	MBFM_AOUT_NG_Load2SR_Loop

	bsf	MBFM_AOUT_NG_LAT_CS, MBFM_AOUT_NG_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_NG_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MBFM_AOUT_NG_Update
	lfsr	FSR0, AOUT1_BASE + AOUTx_L
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x10, 0x20, .., 0x70 (optimization for code below)
	movlw	0x00		; soft-configurable inversion currently not supported for MBFM
	movwf	MIOS_PARAMETER3	; store inversion flags in MIOS_PARAMETER3
MBFM_AOUT_NG_Update_Loop
	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; commands to load and update the DAC register:
	;; bit [15] = 0, bit [14:12] channel number, bit [11:0] DAC value
	movf	POSTINC0, W		; transfer low/high byte of first SR to TMP[12]
	btfsc	MIOS_PARAMETER3, 0; invert if inversion flag is set
	xorlw 0xff	
	movwf	TMP1
	movf	INDF0, W
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 0; invert if inversion flag is set
	xorlw 0x0f	
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP2

	;; load SR if AOUT value has been changed
	BRA_IFSET INDF0, 7, ACCESS, MBFM_AOUT_NG_Update_Loop_Next

	IRQ_ENABLE			; enable interrupts

	rcall	MBFM_AOUT_NG_Load2SR

	;; notify that upload has been done
	bsf	INDF0, 7

MBFM_AOUT_NG_Update_Loop_Next
	IRQ_ENABLE			; enable interrupts
	;; increment to next low byte
	movlw	AOUT_RECORD_LEN-1	; (currently FSR0 points to AOUTx_H
	addwf	FSR0L, F
	;; shiftright inversion flags
	rrf	MIOS_PARAMETER3, F
	;; increment 0x10 to loop counter until it reaches 0x80
	movlw	0x10
	addwf	MIOS_PARAMETER1, F
	BRA_IFCLR MIOS_PARAMETER1, 7, ACCESS, MBFM_AOUT_NG_Update_Loop


MBFM_AOUT_NG_Update_Gates	
	;; do nothing if gate bits have not been changed (lower 4 bits == upper 4 bits)
	swapf	AOUT_GATE, W
	xorwf	AOUT_GATE, W
	andlw	0x0f
	bz	MBFM_AOUT_NG_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency
	;; transfer the lower 4 bits to the upper 4 bits
	movlw	0x0f
	andwf	AOUT_GATE, F
	swapf	AOUT_GATE, W
	iorwf	AOUT_GATE, F
	IRQ_ENABLE			; enable interrupts

MBFM_AOUT_NG_UpdateGates_End
MBFM_AOUT_NG_Update_End

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
