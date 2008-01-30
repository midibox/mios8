; $Id$
;
; AOUT LC driver
; 
; Special Variation of the official "aout_lc.inc" module for
; MIDIbox SID to achive best performance!
;
; This module works with four AOUT_LC modules maximum
; Prepared for configuration 4 * 12/4 bit (e.g. for 4 * CutOff/Resonance)
; 
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
	clrf	POSTINC0	; AOUTx_L
	clrf	POSTINC0	; AOUTx_H
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

#if 0
	;; done at the end of transfer
        bsf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	SID_AOUT_LC_LAT_DOUT, SID_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; release latch
#endif
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SID_AOUT_LC_Update
;;  DESCRIPTION: refreshes the AOUT pins if AOUT values have been changed
;;  OUT:  -
;;  USES: TMP[12345] and MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
SID_AOUT_LC_UPDATE_MACRO MACRO 	aout_a, aout_b
	;; take 12bit value from AOUT[0246]_[LH] -> TMP2 and TMP1[7:4]
	swapf	aout_a+0, W, BANKED
	andlw	0xf0
	btfsc	MIOS_PARAMETER3, 6; invert if inversion flag is set
	xorlw 0xf0	
	movwf	TMP1

	swapf	aout_a+0, W, BANKED
	andlw	0x0f
	movwf	TMP2

	swapf	aout_a+1, W, BANKED
	andlw	0xf0
	iorwf	TMP2, F

	btfsc	MIOS_PARAMETER3, 6		; invert if inversion flag is set
	comf	TMP2, F

	;; take 4bit value from AOUT[1357]_[H] -> TMP1[3:0]
	movf	aout_b+1, W, BANKED
	andlw	0x0f
	btfsc	MIOS_PARAMETER3, 7; invert if inversion flag is set
	xorlw 0x0f	
	iorwf	TMP1, F

	;; notify that values have been transfered
	bsf	aout_a+1, 7, BANKED
	bsf	aout_b+1, 7, BANKED

	rcall	SID_AOUT_LC_Load2SR

	;; leftshift inversion flags
	rlf	MIOS_PARAMETER3, F
	rlf	MIOS_PARAMETER3, F
	ENDM

SID_AOUT_LC_Update

	IRQ_DISABLE			; disable interrupts to ensure data consistency

	SET_BSR	AOUT0_L			; use banked accesses

	;; check if any AOUT value has been changed
	BRA_IFCLR AOUT0_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT1_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT2_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT3_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT4_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT5_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT6_H, 7, BANKED, SID_AOUT_LC_Update_Now
	BRA_IFCLR AOUT7_H, 7, BANKED, SID_AOUT_LC_Update_Now
	rgoto	SID_AOUT_LC_Update_Gates

SID_AOUT_LC_Update_Now
	;; transfer AOUT values
	movff	SID_LOCAL_ENS+SID_ENSx_AOUT_INVERTED, MIOS_PARAMETER3	; inversion flags for all 8 AOUTs
	SID_AOUT_LC_UPDATE_MACRO AOUT6_L, AOUT7_L
	SID_AOUT_LC_UPDATE_MACRO AOUT4_L, AOUT5_L
	SID_AOUT_LC_UPDATE_MACRO AOUT2_L, AOUT3_L
	SID_AOUT_LC_UPDATE_MACRO AOUT0_L, AOUT1_L

	;; latch values
        bsf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; latch SID values
	bcf	SID_AOUT_LC_LAT_DOUT, SID_AOUT_LC_PIN_DOUT	; clear out pin (standby)
        bcf     SID_AOUT_LC_LAT_RCLK, SID_AOUT_LC_PIN_RCLK	; release latch

	IRQ_ENABLE			; enable interrupts


SID_AOUT_LC_Update_Gates	
	;; do nothing if gate bits have not been changed
	SET_BSR	AOUT_GATE
	movf	AOUT_GATE, W, BANKED
	xorwf	AOUT_GATE_SHADOW, W, BANKED
	bz	SID_AOUT_LC_UpdateGates_End

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

SID_AOUT_LC_UpdateGates_End
SID_AOUT_LC_Update_End

	return
