; $Id$
;
; MIDIbox SID
; SID Shift Register Service Routine
;
; ==========================================================================
;
;  Copyright 2005 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
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
SID_SR_LAT_WR2	EQU	LATD
SID_SR_PIN_WR2	EQU	4		; Pin D.4
;
; ==========================================================================
;
; Copyright (C) 2003  Thorsten Klose (tk@midibox.org)
;                     http://www.uCApps.de
; 
; ==========================================================================
; 
; This file is part of SIDplayer
;
; SIDplayer is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; SIDplayer is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with SIDplayer; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	;; reset will be released with first call of SID_SR_Handler

;; --------------------------------------------------------------------------
;;  Check for changes in SID registers, transfer values to SID
;; --------------------------------------------------------------------------
SID_SR_Handler
	movlw	SID_MODE_VOL-SID_BASE+1		; number of registers (0x18)
	movwf	TMP1				; TMP1 is the loop counter
	lfsr	FSR0, SID_BASE			; store base address of SID registers in FSR0
	lfsr	FSR1, SID_SHADOW_BASE		; store base address of shadow registers in FSR1

	IRQ_DISABLE				; disable interrupts

SID_SR_Loop
	movff	POSTINC0, MIOS_PARAMETER2	; store value of SID in MIOS_PARAMETER1 and increment FSR0
	movf	INDF1, W			; get content of appr. shadow register
	cpfseq	MIOS_PARAMETER2, ACCESS; transfer to SID if not equal
	rgoto SID_SR_Transfer	
	rgoto	SID_SR_Next			; skip following code if equal

SID_SR_Transfer
	;; register change: write value into shadow register and transfer it to SID
	movff	MIOS_PARAMETER2, INDF1		; store value in shadow register
	
	decf	FSR0L, W			; extract address offset
	addlw	-(SID_BASE & 0xff)
	iorlw	0xe0				; (note: reset line must stay 1)
	movwf	MIOS_PARAMETER1			; store in address register
	rcall	SID_SR_Write			; transfer to SID

SID_SR_Next
	movf	POSTINC1, W			; increment shadow register

	decfsz	TMP1, F				; decrement loop counter until it is zero
	rgoto	SID_SR_Loop

	IRQ_ENABLE

	return
	
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

	;; synchronize with rising edge of SID clock to avoid setup or hold violation
	;; note: due to pipeline effects, the "bcf" will be executed 3 instruction cycles after
	;; the polling loop. Therefore we are waiting for the falling edge
	btfss	PORTC, 2; wait for falling clock edge
	bra $-2		
	btfsc	PORTC, 2
	bra $-2
	bcf	SID_SR_LAT_WR, SID_SR_PIN_WR	; enable write (MBHP_SID: chip select)
	bcf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; enable chip select of second MBHP_SID module
	bra	$+2				; to ensure compatibility with on-board oscillator,
	bra	$+2				; wait for 1.2 uS (> one SID clock cycle)
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	bsf	SID_SR_LAT_WR, SID_SR_PIN_WR	; disable write (MBHP_SID: chip select)
	bsf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; disable chip select of second MBHP_SID module
	
	return
	
