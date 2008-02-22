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
#define CV_AOUT_NG_LAT_CS	LATC	; The chip select pin CS#
#define CV_AOUT_NG_TRIS_CS	TRISC	; is connected to Port C.5
#define CV_AOUT_NG_PIN_CS	5	; (CANNOT be shared with other outputs!)
;
#define CV_AOUT_NG_LAT_DIN	LATC	; The data input pin DIN
#define CV_AOUT_NG_TRIS_DIN	TRISC	; is connected to Port C.4
#define CV_AOUT_NG_PIN_DIN	4	; (can be shared with other outputs)
;
#define CV_AOUT_NG_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define CV_AOUT_NG_TRIS_SCLK	TRISD	; is connected to Port D.5
#define CV_AOUT_NG_PIN_SCLK	5	; (can be shared with other outputs)

; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_NG_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
CV_AOUT_NG_Init
	;; enable pin drivers
	bcf	CV_AOUT_NG_TRIS_CS, CV_AOUT_NG_PIN_CS
	bcf	CV_AOUT_NG_TRIS_DIN, CV_AOUT_NG_PIN_DIN
	bcf	CV_AOUT_NG_TRIS_SCLK, CV_AOUT_NG_PIN_SCLK

	;; clear the gate pins
	clrf	AOUT_GATE
	setf	AOUT_GATE_LAST

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
CV_AOUT_NG_InitVoutLoop
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
	decfsz	MIOS_PARAMETER3, F
	rgoto	CV_AOUT_NG_InitVoutLoop

	;; update the AOUT pins
	rcall	CV_AOUT_NG_Update

	;; initialize CTRL0
	movlw	(1 << 3) | (3 << 1) ; DO=1 (DOUT Enable), R=3 (internal reference, 2V)
	movwf	TMP1
	movlw	0x8 << 4
	movwf	TMP2
	rcall	CV_AOUT_NG_Load2SR
	
	;; initialize CTRL1
	movlw	0x00
	movwf	TMP1
	movlw	0x9 << 4
	movwf	TMP2
	rcall	CV_AOUT_NG_Load2SR

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_NG_Load2SR
;;  DESCRIPTION: This function loads a value into a single TLV5630
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
CV_AOUT_NG_Load2SR
        bcf	CV_AOUT_NG_LAT_SCLK, CV_AOUT_NG_PIN_SCLK	; clear clock
	bcf	CV_AOUT_NG_LAT_CS, CV_AOUT_NG_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	movlw	16	; init loop counter
	movwf	TMP5
CV_AOUT_NG_Load2SR_Loop
	bcf	CV_AOUT_NG_LAT_DIN, CV_AOUT_NG_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP2, 7
	bsf	CV_AOUT_NG_LAT_DIN, CV_AOUT_NG_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	bsf	CV_AOUT_NG_LAT_SCLK, CV_AOUT_NG_PIN_SCLK	; rising clock edge
	rlf	TMP2, F				; second step for the 32-bit shift
	bcf	CV_AOUT_NG_LAT_SCLK, CV_AOUT_NG_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 16 times
	rgoto	CV_AOUT_NG_Load2SR_Loop

	bsf	CV_AOUT_NG_LAT_CS, CV_AOUT_NG_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_NG_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
CV_AOUT_NG_Update
	lfsr	FSR0, AOUT0_L
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x10, 0x20, .., 0x70 (optimization for code below)
CV_AOUT_NG_Update_Loop
	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; commands to load and update the DAC register:
	;; bit [15] = 0, bit [14:12] channel number, bit [11:0] DAC value
	movff	POSTINC0, TMP1		; transfer low/high byte of first SR to TMP[12]
	movf	INDF0, W
	andlw	0x0f
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP2

	;; load SR if AOUT value has been changed
	BRA_IFSET INDF0, 7, ACCESS, CV_AOUT_NG_Update_Loop_Next

	IRQ_ENABLE			; enable interrupts

	rcall	CV_AOUT_NG_Load2SR

	;; notify that upload has been done
	bsf	INDF0, 7

CV_AOUT_NG_Update_Loop_Next
	IRQ_ENABLE			; enable interrupts
	;; increment to next low byte
	movf	POSTINC0, W
	;; increment 0x10 to loop counter until it reaches 0x80
	movlw	0x10
	addwf	MIOS_PARAMETER1, F
	BRA_IFCLR MIOS_PARAMETER1, 7, ACCESS, CV_AOUT_NG_Update_Loop


CV_AOUT_NG_Update_Gates	
	;; do nothing if gate bits have not been changed
	movf	AOUT_GATE, W
	xorwf	AOUT_GATE_LAST, W
	bz	CV_AOUT_NG_UpdateGates_End

	movff	AOUT_GATE, AOUT_GATE_LAST

	SET_BSR	CV_BASE
	;; set J5 pins if enabled
#if ENABLE_J5
	movf	AOUT_GATE, W
	xorwf	CV_GATE_POLARITY, W		; change polarity if required
	call	J5_DOUT_Set
#endif	

CV_AOUT_NG_UpdateGates_End
CV_AOUT_NG_Update_End

	return
