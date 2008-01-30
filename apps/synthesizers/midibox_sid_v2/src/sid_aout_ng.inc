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
#define SID_AOUT_NG_LAT_CS	LATC	; The chip select pin CS#
#define SID_AOUT_NG_TRIS_CS	TRISC	; is connected to Port C.3 
#define SID_AOUT_NG_PIN_CS	3	; (CANNOT be shared with other outputs!)
;
#define SID_AOUT_NG_LAT_DIN	LATC	; The data input pin DIN
#define SID_AOUT_NG_TRIS_DIN	TRISC	; is connected to Port C.1
#define SID_AOUT_NG_PIN_DIN	1	; (can be shared with other outputs)
;
#define SID_AOUT_NG_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define SID_AOUT_NG_TRIS_SCLK	TRISC	; is connected to Port C.0
#define SID_AOUT_NG_PIN_SCLK	0	; (can be shared with other outputs)
;

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_NG_Init
;;  DESCRIPTION: This function initializes the TLV5630
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
SID_AOUT_NG_Init
	;; enable pin drivers
	bcf	SID_AOUT_NG_TRIS_CS, SID_AOUT_NG_PIN_CS
	bcf	SID_AOUT_NG_TRIS_DIN, SID_AOUT_NG_PIN_DIN
	bcf	SID_AOUT_NG_TRIS_SCLK, SID_AOUT_NG_PIN_SCLK

	;; deactivate chip select
	bsf	SID_AOUT_NG_LAT_CS, SID_AOUT_NG_PIN_CS

	;; clear the AOUT gate pins
	SET_BSR	AOUT_GATE
	clrf	AOUT_GATE, BANKED

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
SID_AOUT_NG_InitVoutLoop
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
	decfsz	MIOS_PARAMETER3, F
	rgoto	SID_AOUT_NG_InitVoutLoop

	;; update the AOUT pins
	rcall	SID_AOUT_NG_Update

	;; initialize CTRL0
	movlw	(1 << 3) | (3 << 1) ; DO=1 (DOUT Enable), R=3 (internal reference, 2V)
	movwf	TMP1
	movlw	0x8 << 4
	movwf	TMP2
	rcall	SID_AOUT_NG_Load2SR
	
	;; initialize CTRL1
	movlw	0x00
	movwf	TMP1
	movlw	0x9 << 4
	movwf	TMP2
	rcall	SID_AOUT_NG_Load2SR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_NG_Load2SR
;;  DESCRIPTION: This function loads a value into a single TLV5630
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
SID_AOUT_NG_Load2SR
        bcf	SID_AOUT_NG_LAT_SCLK, SID_AOUT_NG_PIN_SCLK	; clear clock
	bcf	SID_AOUT_NG_LAT_CS, SID_AOUT_NG_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS

	movlw	16	; init loop counter
	movwf	TMP5
SID_AOUT_NG_Load2SR_Loop
	bcf	SID_AOUT_NG_LAT_DIN, SID_AOUT_NG_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP2, 7
	bsf	SID_AOUT_NG_LAT_DIN, SID_AOUT_NG_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	bsf	SID_AOUT_NG_LAT_SCLK, SID_AOUT_NG_PIN_SCLK	; rising clock edge
	rlf	TMP2, F				; second step for the 32-bit shift
	bcf	SID_AOUT_NG_LAT_SCLK, SID_AOUT_NG_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 16 times
	rgoto	SID_AOUT_NG_Load2SR_Loop

	bsf	SID_AOUT_NG_LAT_CS, SID_AOUT_NG_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_NG_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
SID_AOUT_NG_Update
	lfsr	FSR0, AOUT0_L
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x10, 0x20, .., 0x70 (optimization for code below)
	movff	SID_LOCAL_ENS+SID_ENSx_AOUT_INVERTED, MIOS_PARAMETER3	; inversion flags for all 8 AOUTs
SID_AOUT_NG_Update_Loop
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
	BRA_IFSET INDF0, 7, ACCESS, SID_AOUT_NG_Update_Loop_Next

	IRQ_ENABLE			; enable interrupts

	rcall	SID_AOUT_NG_Load2SR

	;; notify that upload has been done
	bsf	INDF0, 7

SID_AOUT_NG_Update_Loop_Next
	IRQ_ENABLE			; enable interrupts
	;; increment to next low bytes
	movf	POSTINC0, W
	;; shiftright inversion flags
	rrf	MIOS_PARAMETER3, F
	;; increment 0x10 to loop counter until it reaches 0x80
	movlw	0x10
	addwf	MIOS_PARAMETER1, F
	BRA_IFCLR MIOS_PARAMETER1, 7, ACCESS, SID_AOUT_NG_Update_Loop

SID_AOUT_NG_Update_Gates	
	;; do nothing if gate bits have not been changed
	SET_BSR	AOUT_GATE
	movf	AOUT_GATE, W, BANKED
	xorwf	AOUT_GATE_SHADOW, W, BANKED
	bz	SID_AOUT_NG_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; optionally forward to DOUT
#if DEFAULT_EXT_SWITCH_DOUT
	movff	AOUT_GATE, MIOS_PARAMETER1
	movlw	(DEFAULT_EXT_SWITCH_DOUT-1) & 0x0f
	call	MIOS_DOUT_SRSet
#endif

	;; optionally forward to J5
	SET_BSR	AOUT_GATE
#if DEFAULT_J5_FUNCTION == 3
	movf	AOUT_GATE, W, BANKED
	call	J5_DOUT_Set
#endif
	
	;; update shadow register
	movf	AOUT_GATE, W, BANKED
	movwf	AOUT_GATE_SHADOW, BANKED
	IRQ_ENABLE			; enable interrupts

SID_AOUT_NG_UpdateGates_End
SID_AOUT_NG_Update_End

	return
