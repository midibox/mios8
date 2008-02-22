; $Id$
;
; AOUT driver for daisychained MAX525
; 
; Special Variation of the official "aout.inc" module for
; MIDIbox FM to achive best performance!
; This module works with 2 MAX525 maximum, two pins
; are refreshed at the same time, CS/DIN/SCLK pinning different,
; interrupts are not blocked
; 
; The pins to which the first MBHP_AOUT_LC module is connected have to be defined here:
;
#define MBFM_AOUT_LAT_CS	LATC	; The chip select pin CS#
#define MBFM_AOUT_TRIS_CS	TRISC	; is connected to Port C.3 
#define MBFM_AOUT_PIN_CS	3	; (CANNOT be shared with other outputs!)
;
#define MBFM_AOUT_LAT_DIN	LATC	; The data input pin DIN
#define MBFM_AOUT_TRIS_DIN	TRISC	; is connected to Port C.1
#define MBFM_AOUT_PIN_DIN	1	; (can be shared with other outputs)
;
#define MBFM_AOUT_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define MBFM_AOUT_TRIS_SCLK	TRISC	; is connected to Port C.0
#define MBFM_AOUT_PIN_SCLK	0	; (can be shared with other outputs)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MBFM_AOUT_Init
	;; enable pin drivers
	bcf	MBFM_AOUT_TRIS_CS, MBFM_AOUT_PIN_CS
	bcf	MBFM_AOUT_TRIS_DIN, MBFM_AOUT_PIN_DIN
	bcf	MBFM_AOUT_TRIS_SCLK, MBFM_AOUT_PIN_SCLK

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
MBFM_AOUT_InitVoutLoop
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
	rgoto	MBFM_AOUT_InitVoutLoop

	;; update the AOUT pins
	rcall	MBFM_AOUT_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_Load2SR
;;  DESCRIPTION: This function loads two MAX525 shift registers at once
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;        o low byte of second SR value in TMP3
;;        o high byte of second SR value in TMP4
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
MBFM_AOUT_Load2SR
        bcf	MBFM_AOUT_LAT_SCLK, MBFM_AOUT_PIN_SCLK	; clear clock
	bcf	MBFM_AOUT_LAT_CS, MBFM_AOUT_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	movlw	32				; init loop counter
	movwf	TMP5
MBFM_AOUT_Load2SR_Loop
	bcf	MBFM_AOUT_LAT_DIN, MBFM_AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP4, 7
	bsf	MBFM_AOUT_LAT_DIN, MBFM_AOUT_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	rlf	TMP2, F				; second step for the 32-bit shift
	bsf	MBFM_AOUT_LAT_SCLK, MBFM_AOUT_PIN_SCLK	; rising clock edge
	rlf	TMP3, F				; third step for the 32-bit shift
	rlf	TMP4, F				; last step for the 32-bit shift
	bcf	MBFM_AOUT_LAT_SCLK, MBFM_AOUT_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 32 times
	rgoto	MBFM_AOUT_Load2SR_Loop

	bsf	MBFM_AOUT_LAT_CS, MBFM_AOUT_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MBFM_AOUT_Update
	lfsr	FSR0, AOUT1_BASE
	lfsr	FSR1, AOUT5_BASE
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x40, 0x80, 0xc0 (optimization for code below)
MBFM_AOUT_Update_Loop
	clrf	MIOS_PARAMETER2		; bit 0 indicates if one of the SRs has to be updated

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; first SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movlw	AOUTx_L			; transfer low/high byte of first SR to TMP[12]
	movff	PLUSW0, TMP1
	movlw	AOUTx_H
	movf	PLUSW0, W
	andlw	0x0f
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP2

	movlw	AOUTx_H
	btfss	PLUSW0, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0
	bsf	PLUSW0, 7		; notify that upload has been done

	;; second SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movlw	AOUTx_L			; transfer low/high byte of second SR to TMP[34]
	movff	PLUSW1, TMP3
	movlw	AOUTx_H
	movf	PLUSW1, W
	andlw	0x0f
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP4

	movlw	AOUTx_H
	btfss	PLUSW1, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0
	bsf	PLUSW1, 7		; notify that upload has been done

	IRQ_ENABLE			; enable interrupts

	;; load SRs if at least one of the AOUT values has been changed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, MBFM_AOUT_Update_Loop_Next

	rcall	MBFM_AOUT_Load2SR

MBFM_AOUT_Update_Loop_Next
	;; increment to next record
	movlw	AOUT_RECORD_LEN
	addwf	FSR0L, F
	addwf	FSR1L, F
	;; increment 0x40 to loop counter until it reaches 0x00 again
	movlw	0x40
	addwf	MIOS_PARAMETER1, F
	bnz	MBFM_AOUT_Update_Loop

MBFM_AOUT_Update_Gates	
	;; do nothing if gate bits have not been changed (lower 4 bits == upper 4 bits)
	swapf	AOUT_GATE, W
	xorwf	AOUT_GATE, W
	andlw	0x0f
	bz	MBFM_AOUT_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency
	;; transfer the lower 4 bits to the upper 4 bits
	movlw	0x0f
	andwf	AOUT_GATE, F
	swapf	AOUT_GATE, W
	iorwf	AOUT_GATE, F
	IRQ_ENABLE			; enable interrupts

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP1			; transfer low/high byte to first SR to TMP[12]
	movlw	0x20
	btfsc	AOUT_GATE, 0, ACCESS
	iorlw 0x40
	movwf	TMP2

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP3			; transfer low/high byte to second SR to TMP[34]
	movlw	0x20
	btfsc	AOUT_GATE, 1, ACCESS
	iorlw 0x40
	movwf	TMP4

	;; load SRs and exit
	rcall	MBFM_AOUT_Load2SR

MBFM_AOUT_UpdateGates_End
MBFM_AOUT_Update_End

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
