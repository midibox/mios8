; $Id$
;
; AOUT LC driver
; 
; Special Variation of the official "aout.inc" module for
; MIDIbox SID to achive best performance!
; This module works with one AOUT_LC module maximum
; 
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; The pins to which the first MBHP_AOUT_LC module is connected have to be defined here:
;
#define SID_AOUT_LC_LAT_RCLK	LATC	; The latch enable input
#define SID_AOUT_LC_TRIS_RCLK	TRISC	; is connected to Port C.3 
#define SID_AOUT_LC_PIN_RCLK	3	; (CANNOT be shared with other outputs!)
;
#define SID_AOUT_LC_LAT_DOUT	LATC	; The data pin
#define SID_AOUT_LC_TRIS_DOUT	TRISC	; is connected to Port C.1
#define SID_AOUT_LC_PIN_DOUT	1	; (can be shared with other outputs)
;
#define SID_AOUT_LC_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define SID_AOUT_LC_TRIS_SCLK	TRISC	; is connected to Port C.0
#define SID_AOUT_LC_PIN_SCLK	0	; (can be shared with other outputs)
;

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_LC_Init
;;  DESCRIPTION: This function initializes all connected 74HC595
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
SID_AOUT_LC_Init
	;; enable pin drivers
	bcf	SID_AOUT_LC_TRIS_RCLK, SID_AOUT_LC_PIN_RCLK
	bcf	SID_AOUT_LC_TRIS_DOUT, SID_AOUT_LC_PIN_DOUT
	bcf	SID_AOUT_LC_TRIS_SCLK, SID_AOUT_LC_PIN_SCLK

	;; set voltages to 0V
	lfsr	FSR0, AOUT0_L
	movlw	8
	movwf	MIOS_PARAMETER3	; used as loop counter here
SID_AOUT_LC_InitVoutLoop
	clrf	POSTINC0
	movlw	0x80
	movwf	POSTINC0
	decfsz	MIOS_PARAMETER3, F
	rgoto	SID_AOUT_LC_InitVoutLoop

	;; update the AOUT pins
	rcall	SID_AOUT_LC_Update

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_LC_Load2SR
;;  DESCRIPTION: This function loads a 16bit value into the shift register
;;  IN:   o low byte of SR value in TMP1
;;        o high byte of SR value in TMP2
;;  OUT:  -
;;  USES: TMP[12345]
;; --------------------------------------------------------------------------
SID_AOUT_LC_Load2SR
        bcf     SID_AOUT_LC_LAT_SCLK, SID_AOUT_LC_PIN_SCLK	; clear clock

	;; superfast transfer with unrolled loop (takes some memory, but guarantees the
	;; lowest system load :)
SID_AOUT_LC_WRITE_BIT MACRO reg, bit
	bcf	SID_AOUT_LC_LAT_DOUT, SID_AOUT_LC_PIN_DOUT	; set out pin depending on register content (reg.bit)
	btfsc	reg, bit
	bsf	SID_AOUT_LC_LAT_DOUT, SID_AOUT_LC_PIN_DOUT
	nop
        bsf     SID_AOUT_LC_LAT_SCLK, SID_AOUT_LC_PIN_SCLK	; rising clock edge
        bcf     SID_AOUT_LC_LAT_SCLK, SID_AOUT_LC_PIN_SCLK	; falling clock edge
	ENDM

	SID_AOUT_LC_WRITE_BIT TMP1, 7
	SID_AOUT_LC_WRITE_BIT TMP1, 6
	SID_AOUT_LC_WRITE_BIT TMP1, 5
	SID_AOUT_LC_WRITE_BIT TMP1, 4
	SID_AOUT_LC_WRITE_BIT TMP1, 3
	SID_AOUT_LC_WRITE_BIT TMP1, 2
	SID_AOUT_LC_WRITE_BIT TMP1, 1
	SID_AOUT_LC_WRITE_BIT TMP1, 0

	SID_AOUT_LC_WRITE_BIT TMP2, 7
	SID_AOUT_LC_WRITE_BIT TMP2, 6
	SID_AOUT_LC_WRITE_BIT TMP2, 5
	SID_AOUT_LC_WRITE_BIT TMP2, 4
	SID_AOUT_LC_WRITE_BIT TMP2, 3
	SID_AOUT_LC_WRITE_BIT TMP2, 2
	SID_AOUT_LC_WRITE_BIT TMP2, 1
	SID_AOUT_LC_WRITE_BIT TMP2, 0

        bsf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	SID_AOUT_LC_LAT_DOUT, SID_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; release latch

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_LC_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SID_AOUT_LC_Update
SID_AOUT_LC_Update_Loop
	clrf	MIOS_PARAMETER2		; bit 0 indicates if one of the SRs has to be updated

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	;; cutoff
	swapf	AOUT0_L, W		; transfer low/high byte of first SR to TMP[12]
	andlw	0xf0
	movwf	TMP1

	swapf	AOUT0_L, W
	andlw	0x0f
	movwf	TMP2

	swapf	AOUT0_H, W
	andlw	0xf0
	iorwf	TMP2, F

	;; resonance
	movf	AOUT1_H, W
	andlw	0x0f
	iorwf	TMP1, F

	btfss	AOUT0_H, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0
	btfss	AOUT1_H, 7		; notify if AOUT value has been changed
	bsf	MIOS_PARAMETER2, 0

	;; notify that upload has been done
	bsf	AOUT0_H, 7
	bsf	AOUT1_H, 7

	IRQ_ENABLE			; enable interrupts

	;; load SRs if at least one of the AOUT values has been changed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, SID_AOUT_LC_Update_Loop_Next

	rcall	SID_AOUT_LC_Load2SR

SID_AOUT_LC_Update_Loop_Next

SID_AOUT_LC_Update_Gates	
	;; do nothing if gate bits have not been changed (lower 4 bits == upper 4 bits)
	swapf	AOUT_GATE, W
	xorwf	AOUT_GATE, W
	andlw	0x0f
	bz	SID_AOUT_LC_UpdateGates_End

	IRQ_DISABLE			; disable interrupts to ensure data consistency
	;; transfer the lower 4 bits to the upper 4 bits
	movlw	0x0f
	andwf	AOUT_GATE, F
	swapf	AOUT_GATE, W
	iorwf	AOUT_GATE, F
	IRQ_ENABLE			; enable interrupts

SID_AOUT_LC_UpdateGates_End
SID_AOUT_LC_Update_End

	return
