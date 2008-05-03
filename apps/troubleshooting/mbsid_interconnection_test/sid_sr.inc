; $Id$
;
; MIDIbox SID
; SID Shift Register Service Routine
;
; define the pins to which the MBHPS_SID module is connected
;
SID_SR_LAT_SCLK	EQU	LATD
SID_SR_PIN_SCLK	EQU	5		; Pin D.5
SID_SR_LAT_RCLK	EQU	LATC
SID_SR_PIN_RCLK	EQU	4		; Pin C.4
SID_SR_LAT_OUT	EQU	LATD
SID_SR_PIN_OUT	EQU	6		; Pin D.6

SID_SR_LAT_WR	EQU	LATC
SID_SR_PIN_WR	EQU	5		; Pin C.5

; ==========================================================================
;
;  Copyright 2005 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Initialize the MBHP_SID module
;; --------------------------------------------------------------------------
SID_SR_Init
	;; reset the SID
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	goto	SID_SR_Write

	
;; --------------------------------------------------------------------------
;;  SID Write: write to SID register
;; --------------------------------------------------------------------------
SID_SR_Write
	;; SID signals:
	;; MIOS_PARAMETER2[7..0]: Data
	;; MIOS_PARAMETER1[4..0]: Address
	;; MIOS_PARAMETER1[5]   : Reset
	;; temporary used as counter: MIOS_PARAMETER3

        bcf     SID_SR_LAT_SCLK, SID_SR_PIN_SCLK	; clear clock

	;; superfast transfer with unrolled loop (takes some memory, but guarantees the
	;; lowest system load :)
SID_SR_WRITE_BIT MACRO reg, bit
	bcf	SID_SR_LAT_OUT, SID_SR_PIN_OUT	; set out pin depending on register content (reg.bit)
	btfsc	reg, bit
	bsf	SID_SR_LAT_OUT, SID_SR_PIN_OUT
        bsf     SID_SR_LAT_SCLK, SID_SR_PIN_SCLK	; rising clock edge
        bcf     SID_SR_LAT_SCLK, SID_SR_PIN_SCLK	; falling clock edge
	ENDM

	SID_SR_WRITE_BIT MIOS_PARAMETER2, 0
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 1
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 2
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 3
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 4
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 5
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 6
	SID_SR_WRITE_BIT MIOS_PARAMETER2, 7

	SID_SR_WRITE_BIT MIOS_PARAMETER1, 0
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 1
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 2
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 3
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 4
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 5
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 6
	SID_SR_WRITE_BIT MIOS_PARAMETER1, 7

        bsf     SID_SR_LAT_RCLK, SID_SR_PIN_RCLK	; latch SID values
	bcf	SID_SR_LAT_OUT, SID_SR_PIN_OUT	; clear out pin (standby)
        bcf     SID_SR_LAT_RCLK, SID_SR_PIN_RCLK	; release latch

	;; initiate a write and wait for 1.4 us (> one clock cycle)
	bcf	SID_SR_LAT_WR, SID_SR_PIN_WR
	movlw	0x03
	clrf	MIOS_PARAMETER3
SID_SR_WriteLoop3
	incf	MIOS_PARAMETER3, F
	cpfseq	MIOS_PARAMETER3, ACCESS
	rgoto SID_SR_WriteLoop3
	bsf	SID_SR_LAT_WR, SID_SR_PIN_WR
	
	return
