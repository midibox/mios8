; $Id$
;
; AOUT driver for daisychained MAX525
; 
; Special Variation of the official "aout.inc" module for
; MIDIbox CV to achive best performance!
; This module works with 2 MAX525 maximum, two pins
; are refreshed at the same time, CS/DIN/SCLK pinning different,
; interrupts are not blocked
; 
; The pins to which the first CV_AOUT module is connected have to be defined here:
;
#if 1
	;; common AOUT connections
#define CV_AOUT_LAT_CS		LATC	; The chip select pin CS#
#define CV_AOUT_TRIS_CS		TRISC	; is connected to Port C.5
#define CV_AOUT_PIN_CS		5	; (CANNOT be shared with other outputs!)
;
#define CV_AOUT_LAT_DIN		LATC	; The data input pin DIN
#define CV_AOUT_TRIS_DIN	TRISC	; is connected to Port C.4
#define CV_AOUT_PIN_DIN		4	; (can be shared with other outputs)
;
#define CV_AOUT_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define CV_AOUT_TRIS_SCLK	TRISD	; is connected to Port D.5
#define CV_AOUT_PIN_SCLK	5	; (can be shared with other outputs)
#else
	;; MIDIbox SEQ Layout
#define CV_AOUT_LAT_CS		LATC	; The chip select pin CS#
#define CV_AOUT_TRIS_CS		TRISC	; is connected to Port C.5
#define CV_AOUT_PIN_CS		5	; (CANNOT be shared with other outputs!)
;
#define CV_AOUT_LAT_DIN		LATD	; The data input pin DIN
#define CV_AOUT_TRIS_DIN	TRISD	; is connected to Port D.4
#define CV_AOUT_PIN_DIN		4	; (can be shared with other outputs)
;
#define CV_AOUT_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define CV_AOUT_TRIS_SCLK	TRISD	; is connected to Port D.5
#define CV_AOUT_PIN_SCLK	5	; (can be shared with other outputs)
#endif
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_Init
;;  DESCRIPTION: This function initializes all connected MAX525
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
CV_AOUT_Init
	;; enable pin drivers
	bcf	CV_AOUT_TRIS_CS, CV_AOUT_PIN_CS
	bcf	CV_AOUT_TRIS_DIN, CV_AOUT_PIN_DIN
	bcf	CV_AOUT_TRIS_SCLK, CV_AOUT_PIN_SCLK

	;; clear the gate pins
	clrf	AOUT_GATE
	setf	AOUT_GATE_LAST

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
CV_AOUT_InitVoutLoop
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
	decfsz	MIOS_PARAMETER3, F
	rgoto	CV_AOUT_InitVoutLoop

	;; update the AOUT pins
	rcall	CV_AOUT_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_Load2SR
;;  DESCRIPTION: This function loads two MAX525 shift registers at once
;;  IN:   o low byte of first SR value in TMP1
;;        o high byte of first SR value in TMP2
;;        o low byte of second SR value in TMP3
;;        o high byte of second SR value in TMP4
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
CV_AOUT_Load2SR
        bcf	CV_AOUT_LAT_SCLK, CV_AOUT_PIN_SCLK	; clear clock
	bcf	CV_AOUT_LAT_CS, CV_AOUT_PIN_CS	; activate chip select

	;; NOTE: you will notice that the instructions are sometimes arranged
	;; in a special order to ensure proper output signals - for example:
	;; between a rising and a falling SCLK edge there is at least one
	;; other instruction to ensure that the high pulse of the SCLK
	;; is longer than 100 nS (the MAX525 datasheet specifies at least 40 nS)

	movlw	32				; init loop counter
	movwf	TMP5
CV_AOUT_Load2SR_Loop
	bcf	CV_AOUT_LAT_DIN, CV_AOUT_PIN_DIN	; set DIN depending on current MSB
	btfsc	TMP4, 7
	bsf	CV_AOUT_LAT_DIN, CV_AOUT_PIN_DIN
	rlf	TMP1, F				; start to shift the 32-bit value
	rlf	TMP2, F				; second step for the 32-bit shift
	bsf	CV_AOUT_LAT_SCLK, CV_AOUT_PIN_SCLK	; rising clock edge
	rlf	TMP3, F				; third step for the 32-bit shift
	rlf	TMP4, F				; last step for the 32-bit shift
	bcf	CV_AOUT_LAT_SCLK, CV_AOUT_PIN_SCLK	; falling clock edge
	decfsz	TMP5, F				; loop 32 times
	rgoto	CV_AOUT_Load2SR_Loop

	bsf	CV_AOUT_LAT_CS, CV_AOUT_PIN_CS; deactivate chip select

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: CV_AOUT_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
CV_AOUT_Update
	lfsr	FSR0, AOUT0_L
	lfsr	FSR1, AOUT4_L
	clrf	MIOS_PARAMETER1		; used as loop counter here
					; counts: 0x00, 0x40, 0x80, 0xc0 (optimization for code below)
CV_AOUT_Update_Loop
	clrf	MIOS_PARAMETER2		; bit 0 indicates if one of the SRs has to be updated

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; first SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movff	POSTINC0, TMP1		; transfer low/high byte of first SR to TMP[12]
	movf	INDF0, W
	andlw	0x0f
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP2

	btfss	INDF0, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0
	
	;; second SR
	;; commands to load and update the DAC register:
	;; A1=channel number[1], A0=channel number[0], C1=1, C0=1
	movff	POSTINC1, TMP3		; transfer low/high byte of second SR to TMP[34]
	movf	INDF1, W
	andlw	0x0f
	iorlw	0x30
	iorwf	MIOS_PARAMETER1, W
	movwf	TMP4

	btfss	INDF1, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0

	IRQ_ENABLE			; enable interrupts

	;; load SRs if at least one of the AOUT values has been changed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CV_AOUT_Update_Loop_Next

	rcall	CV_AOUT_Load2SR
	;; notify that upload has been done
	bsf	INDF0, 7
	bsf	INDF1, 7

CV_AOUT_Update_Loop_Next
	;; increment to next low bytes
	movf	POSTINC0, W
	movf	POSTINC1, W
	;; increment 0x40 to loop counter until it reaches 0x00 again
	movlw	0x40
	addwf	MIOS_PARAMETER1, F
	bnz	CV_AOUT_Update_Loop

CV_AOUT_Update_Gates	
	;; do nothing if gate bits have not been changed
	movf	AOUT_GATE, W
	xorwf	AOUT_GATE_LAST, W
	bz	CV_AOUT_UpdateGates_End

	movff	AOUT_GATE, AOUT_GATE_LAST

	SET_BSR	CV_BASE

	;; set J5 pins if enabled
#if ENABLE_J5
	movf	AOUT_GATE, W
	xorwf	CV_GATE_POLARITY, W		; change polarity if required
	call	J5_DOUT_Set
#endif	

	;; for AOUT module: save gate[1:0] temporary in PRODL[1:0], take polarity into account
	movf	AOUT_GATE, W
	xorwf	CV_GATE_POLARITY, W		; change polarity if required
	movwf	PRODL

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP1			; transfer low/high byte to first SR to TMP[12]
	movlw	0x20
	btfsc	PRODL, 0
	iorlw 0x40
	movwf	TMP2

	;; commands to set the UPO:
	;; UPO=low:  A1=0, A0=0, C1=1, C0=0
	;; UPO=high: A1=0, A0=1, C1=1, C0=0
	clrf	TMP3			; transfer low/high byte to second SR to TMP[34]
	movlw	0x20
	btfsc	PRODL, 1
	iorlw 0x40
	movwf	TMP4

	;; load SRs and exit
	rcall	CV_AOUT_Load2SR

CV_AOUT_UpdateGates_End
CV_AOUT_Update_End

	return
