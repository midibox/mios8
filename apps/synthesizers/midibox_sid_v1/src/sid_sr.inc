; $Id$
;
; MIDIbox SID
; SID Shift Register Service Routine
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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


;; --------------------------------------------------------------------------
;;  Initialize the MBHP_SID module
;; --------------------------------------------------------------------------
SID_SR_Init
	;; reset the SID
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	rgoto	SID_SR_Write
	;; reset will be released with first call of SID_SR_Handler

;; --------------------------------------------------------------------------
;;  Check for changes in SID registers, transfer values to SID
;;  and handle Wavetable
;; --------------------------------------------------------------------------
SID_SR_Handler
	btfsc	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

	call	SID_WT_Handler			; call wavetable handler before continue

_SID_SR_Handler
SID_SR_Start

	TABLE_ADDR SID_SR_REGWRITE_TABLE	; contains order of register accesses
	movlw	SID_MODE_VOL-SID_BASE+1		; number of registers (0x18)
	movwf	TMP1				; TMP1 is the loop counter
	lfsr	FSR0, SID_BASE			; store base address of SID registers in FSR0
	lfsr	FSR1, SID_SHADOW_BASE		; store base address of shadow registers in FSR1

	IRQ_DISABLE				; disable interrupts

SID_SR_Loop
	tblrd*+
	movf	TABLAT, W
	movff	PLUSW0, MIOS_PARAMETER2		; store value of SID in MIOS_PARAMETER1 and increment FSR0
	BRA_IFSET SID_STAT, SID_STAT_FORCE_REFRESH, ACCESS, SID_SR_Transfer; don't skip if refresh has been forced
	movf	PLUSW1, W			; get content of appr. shadow register
	cpfseq	MIOS_PARAMETER2, ACCESS; transfer to SID if not equal
	rgoto SID_SR_Transfer	
  	rgoto	SID_SR_Next			; skip following code if equal

SID_SR_Transfer
	;; register change: write value into shadow register and transfer it to SID
	movf	TABLAT, W
	movff	MIOS_PARAMETER2, PLUSW1		; store value in shadow register
	
	movf	TABLAT, W			; extract address offset	
	iorlw	0xe0				; (note: reset line must stay 1)
	movwf	MIOS_PARAMETER1			; store in address register
	rcall	SID_SR_Write			; transfer to SID

SID_SR_Next
	decfsz	TMP1, F				; decrement loop counter until it is zero
	rgoto	SID_SR_Loop


	;; sync oscillator phase if enabled
	SET_BSR SID_BASE
	BRA_IFCLR SID_OSC_PHASE_SYNC, 0, BANKED, SID_SR_NoV1Sync
SID_SR_V1Sync
	lfsr	FSR0, SID_V1_CTRL
	lfsr	FSR1, SID_V1_CTRL + (SID_SHADOW_BASE-SID_BASE)
	rcall	SID_SR_OscPhaseSync
SID_SR_NoV1Sync

	BRA_IFCLR SID_OSC_PHASE_SYNC, 1, BANKED, SID_SR_NoV2Sync
SID_SR_V2Sync
	lfsr	FSR0, SID_V2_CTRL
	lfsr	FSR1, SID_V2_CTRL + (SID_SHADOW_BASE-SID_BASE)
	rcall	SID_SR_OscPhaseSync
SID_SR_NoV2Sync

	BRA_IFCLR SID_OSC_PHASE_SYNC, 2, BANKED, SID_SR_NoV3Sync
SID_SR_V3Sync
	lfsr	FSR0, SID_V3_CTRL
	lfsr	FSR1, SID_V3_CTRL + (SID_SHADOW_BASE-SID_BASE)
	rcall	SID_SR_OscPhaseSync
SID_SR_NoV3Sync

	IRQ_ENABLE				; enable interrupts again

	bcf	SID_STAT, SID_STAT_FORCE_REFRESH; clear refresh request (if it has been set before)

	SET_BSR	SID_BASE
	bcf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_SYNC_UPDATE, BANKED	; sync with SIDSW
	bcf	SID_V2_BASE + SID_Vx_MODE, SID_MODE_SYNC_UPDATE, BANKED
	bcf	SID_V3_BASE + SID_Vx_MODE, SID_MODE_SYNC_UPDATE, BANKED

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
	bra	$+2				; to ensure compatibility with on-board oscillator,
	bra	$+2				; wait for 1.2 uS (> one SID clock cycle)
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	bsf	SID_SR_LAT_WR, SID_SR_PIN_WR	; disable write (MBHP_SID: chip select)
	
	return
	
;; --------------------------------------------------------------------------
;;  SID Phase sync: called by the SID_SR_Handler after a register update
;;  expects pointer to SID_Vx_CTRL in FSR0 and to the appr. shadow register in FSR1
;;  checks if test bit set - if so, clears the test bit, sets the gate bit
;;  and transfers the updated control register to the SID
;; --------------------------------------------------------------------------
SID_SR_OscPhaseSync
	;; no phase sync if test bit not set
	btfss	INDF0, 3
	return

	;; clear test bit
	bcf	INDF0, 3
	;; set gate bit
	bsf	INDF0, 0

	;; transfer new value into shadow register
	movf	INDF0, W	; transfer new value into shadow register
	movwf	INDF1

	;; and transfer it to the SID
	movwf	MIOS_PARAMETER2	; data
	movf	FSR0L, W	; extract address offset
	addlw	-(SID_BASE & 0xff)
	iorlw	0xe0		; (note: reset line must stay 1)
	movwf	MIOS_PARAMETER1	; address
	rgoto	SID_SR_Write

;; --------------------------------------------------------------------------
;;  This routine writes to the SR register
;;  used by SID_SW_Note in sid_sw.inc
;;  expecting offset to AD register in WREG, and data in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SID_SR_Write_SR
	iorlw	0xe0			; refresh SID AD register
	movwf	MIOS_PARAMETER1		; address
	rgoto	SID_SR_Write

;; --------------------------------------------------------------------------
SID_SR_REGWRITE_TABLE
	;; order in which the SID registers are written
	;; note that the control register (which contains the gate flag) is not written before
	;; the other OSC registers have been initialized
	db	0x00, 0x01, 0x02, 0x03, 0x05, 0x06
	db	0x07, 0x08, 0x09, 0x0a, 0x0c, 0x0d
	db	0x0e, 0x0f, 0x10, 0x11, 0x13, 0x14
	db	0x04, 0x0b, 0x12, 0x15, 0x16, 0x17, 0x18
