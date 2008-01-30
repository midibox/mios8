; $Id$
;
; AOUT driver for daisychained MAX525
; 
; Special Variation of the official "aout.inc" module for
; MIDIbox SID to achive best performance!
; This module works with 2 MAX525 maximum, two pins
; are refreshed at the same time, CS/DIN/SCLK pinning different,
; interrupts are not blocked
; 
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; The pins to which the first MBHP_AOUT module is connected have to be defined here:
;
#define SID_AOUT_LAT_CS		LATC	; The chip select pin CS#
#define SID_AOUT_TRIS_CS	TRISC	; is connected to Port C.3 
#define SID_AOUT_PIN_CS		3	; (CANNOT be shared with other outputs!)
;
#define SID_AOUT_LAT_DIN	LATC	; The data input pin DIN
#define SID_AOUT_TRIS_DIN	TRISC	; is connected to Port C.1
#define SID_AOUT_PIN_DIN	1	; (can be shared with other outputs)
;
#define SID_AOUT_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define SID_AOUT_TRIS_SCLK	TRISC	; is connected to Port C.0
#define SID_AOUT_PIN_SCLK	0	; (can be shared with other outputs)
;

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
SID_AOUT_Init
	;; enable pin drivers
	bcf	SID_AOUT_TRIS_CS, SID_AOUT_PIN_CS
	bcf	SID_AOUT_TRIS_DIN, SID_AOUT_PIN_DIN
	bcf	SID_AOUT_TRIS_SCLK, SID_AOUT_PIN_SCLK

	;; deactivate chip select
	bsf	SID_AOUT_LAT_CS, SID_AOUT_PIN_CS

	;; clear the AOUT gate pins
	SET_BSR	AOUT_GATE
	clrf	AOUT_GATE, BANKED

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
SID_AOUT_InitVoutLoop
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
	decfsz	MIOS_PARAMETER3, F
	rgoto	SID_AOUT_InitVoutLoop

	;; update the AOUT pins
	rcall	SID_AOUT_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_Load2SR
;;  DESCRIPTION: This function loads two MAX525 shift registers at once
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;        o low byte of second SR value in TMP3
;;        o high byte of second SR value in TMP4
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
SID_AOUT_Load2SR
        bcf	SID_AOUT_LAT_SCLK, SID_AOUT_PIN_SCLK	; clear clock
	bcf	SID_AOUT_LAT_CS, SID_AOUT_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	movlw	32				; init loop counter
	movwf	TMP5
SID_AOUT_Load2SR_Loop
	bcf	SID_AOUT_LAT_DIN, SID_AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP4, 7
	bsf	SID_AOUT_LAT_DIN, SID_AOUT_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	rlf	TMP2, F				; second step for the 32-bit shift
	bsf	SID_AOUT_LAT_SCLK, SID_AOUT_PIN_SCLK	; rising clock edge
	rlf	TMP3, F				; third step for the 32-bit shift
	rlf	TMP4, F				; last step for the 32-bit shift
	bcf	SID_AOUT_LAT_SCLK, SID_AOUT_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 32 times
	rgoto	SID_AOUT_Load2SR_Loop

	bsf	SID_AOUT_LAT_CS, SID_AOUT_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
SID_AOUT_Update
	lfsr	FSR0, AOUT0_L
	lfsr	FSR1, AOUT4_L
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x40, 0x80, 0xc0 (optimization for code below)
	movff	SID_LOCAL_ENS+SID_ENSx_AOUT_INVERTED, MIOS_PARAMETER3	; inversion flags for all 8 AOUTs
SID_AOUT_Update_Loop
	clrf	MIOS_PARAMETER2		; bit 0 indicates if one of the SRs has to be updated

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; first SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movf	POSTINC0, W		; transfer low/high byte of first SR to TMP[12]
	btfsc	MIOS_PARAMETER3, 0; invert if inversion flag is set
	xorlw 0xff	
	movwf	TMP1
	movf	INDF0, W
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 0; invert if inversion flag is set
	xorlw 0x0f	
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP2

	btfss	INDF0, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0

	;; second SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movf	POSTINC1, W		; transfer low/high byte of second SR to TMP[34]
	btfsc	MIOS_PARAMETER3, 4; invert if inversion flag is set
	xorlw 0xff	
	movwf	TMP3
	movf	INDF1, W
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 4; invert if inversion flag is set
	xorlw 0x0f	
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP4

	btfss	INDF1, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0

	IRQ_ENABLE			; enable interrupts

	;; load SRs if at least one of the AOUT values has been changed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SID_AOUT_Update_Loop_Next

	rcall	SID_AOUT_Load2SR
	;; notify that upload has been done
	bsf	INDF0, 7
	bsf	INDF1, 7

SID_AOUT_Update_Loop_Next
	;; increment to next low bytes
	movf	POSTINC0, W
	movf	POSTINC1, W
	;; shiftright inversion flags
	rrf	MIOS_PARAMETER3, F
	;; increment 0x40 to loop counter until it reaches 0x00 again
	movlw	0x40
	addwf	MIOS_PARAMETER1, F
	bnz	SID_AOUT_Update_Loop

SID_AOUT_Update_Gates
	;; do nothing if gate bits have not been changed
	SET_BSR	AOUT_GATE
	movf	AOUT_GATE, W, BANKED
	xorwf	AOUT_GATE_SHADOW, W, BANKED
	bz	SID_AOUT_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP1			; transfer low/high byte to first SR to TMP[12]
	movlw	0x20
	btfsc	AOUT_GATE, 0, BANKED
	iorlw 0x40
	movwf	TMP2

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP3			; transfer low/high byte to second SR to TMP[34]
	movlw	0x20
	btfsc	AOUT_GATE, 1, BANKED
	iorlw 0x40
	movwf	TMP4

	;; load SRs and exit
	rcall	SID_AOUT_Load2SR

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

SID_AOUT_UpdateGates_End
SID_AOUT_Update_End

	return
