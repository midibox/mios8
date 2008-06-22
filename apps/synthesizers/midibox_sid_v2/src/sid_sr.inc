; $Id$
;
; MIDIbox SID
; SID Shift Register Service Routine
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Idea for Oscillator Phase Offset approach by Wilba
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


;; --------------------------------------------------------------------------
;;  Initialize the MBHP_SID module
;; --------------------------------------------------------------------------
SID_SR_Init
	;; reset the SIDs
	lfsr	FSR0, SIDL_SHADOW_BASE
	lfsr	FSR1, SIDR_SHADOW_BASE
	clrf	MIOS_PARAMETER1
	movlw	0x00
	rcall	SID_SR_TransferL
	rgoto	SID_SR_TransferR
	;; reset will be released with first call of SID_SR_Handler

;; --------------------------------------------------------------------------
;;  Call this function to force an refresh of all SID registers
;; --------------------------------------------------------------------------
SID_SR_Refresh
	IRQ_DISABLE				; disable interrupts to avoid inconsistencies

	;; just modify the shadow registers, so that the content is not equal to the main register set
#if ENABLE_SWINSID
	movlw	0x20				; number of registers: 0x20
#else
	movlw	SIDx_MODE_VOL+1			; number of registers (0x19)
#endif
	movwf	TMP1				; TMP1 is the loop counter
	lfsr	FSR0, SIDL_BASE-1		; base address of SIDL registers
	lfsr	FSR1, SIDR_BASE-1		; base address of SIDR registers
SID_SR_Refresh_Loop
	incf	PREINC0, W
	movwf	PRODL
	movlw	LOW(SIDL_SHADOW_BASE-SIDL_BASE)
	movff	PRODL, PLUSW0

	incf	PREINC1, W
	movwf	PRODL
	movlw	LOW(SIDR_SHADOW_BASE-SIDR_BASE)
	movff	PRODL, PLUSW1

	decfsz	TMP1, F
	rgoto	SID_SR_Refresh_Loop

	;; set test flags for proper oscillator synchronisation
	;; can be disabled with the DOR flag
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_DOR, ACCESS, SID_SR_Refresh_NoOSCReset
SID_SR_Refresh_OSCReset
	SET_BSR	SIDL_BASE
	bsf	SIDL_BASE + SIDx_V1_CTRL, 3, BANKED
	bsf	SIDL_BASE + SIDx_V2_CTRL, 3, BANKED
	bsf	SIDL_BASE + SIDx_V3_CTRL, 3, BANKED
	bsf	SIDR_BASE + SIDx_V1_CTRL, 3, BANKED
	bsf	SIDR_BASE + SIDx_V2_CTRL, 3, BANKED
	bsf	SIDR_BASE + SIDx_V3_CTRL, 3, BANKED
SID_SR_Refresh_NoOSCReset

	;; call transfer routine
	rcall	_SID_SR_Handler

	;; clear test flag and update again
	SET_BSR	SIDL_BASE
	bcf	SIDL_BASE + SIDx_V1_CTRL, 3, BANKED
	bcf	SIDL_BASE + SIDx_V2_CTRL, 3, BANKED
	bcf	SIDL_BASE + SIDx_V3_CTRL, 3, BANKED
	bcf	SIDR_BASE + SIDx_V1_CTRL, 3, BANKED
	bcf	SIDR_BASE + SIDx_V2_CTRL, 3, BANKED
	bcf	SIDR_BASE + SIDx_V3_CTRL, 3, BANKED

	rgoto	_SID_SR_Handler


;; --------------------------------------------------------------------------
;;  Check for changes in SID registers, transfer values to SID
;; --------------------------------------------------------------------------
SID_SR_Handler
	btfsc	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

_SID_SR_Handler
	;; superfast unrolled loop with direct register accesses for best performance
	;; note that the control registers (which contain the gate flag) are not written
	;; before the other OSC registers have been initialized

SID_SR_HANDLER_MACRO MACRO reg
	LOCAL	SID_SR_HANDLER_MACRO_NEqu
	LOCAL	SID_SR_HANDLER_MACRO_End

	movf	SIDL_BASE + reg, W, BANKED	; branch if SIDL and SIDR register not equal
	cpfseq	SIDR_BASE + reg, BANKED
	rgoto	SID_SR_HANDLER_MACRO_NEqu
	movf	SIDL_SHADOW_BASE + reg, W, BANKED
	cpfseq	SIDR_SHADOW_BASE + reg, BANKED
	rgoto	SID_SR_HANDLER_MACRO_NEqu

	movf	SIDL_BASE + reg, W, BANKED	; transfer value to both SIDs if it has been changed
	cpfseq	SIDL_SHADOW_BASE + reg, BANKED
	rcall	SID_SR_TransferB

	rgoto	SID_SR_HANDLER_MACRO_End

SID_SR_HANDLER_MACRO_NEqu
	movf	SIDL_BASE + reg, W, BANKED	; transfer SIDL value if changed
	cpfseq	SIDL_SHADOW_BASE + reg, BANKED
	rcall	SID_SR_TransferL

	movf	SIDR_BASE + reg, W, BANKED	; transfer SIDR value if changed
	cpfseq	SIDR_SHADOW_BASE + reg, BANKED
	rcall	SID_SR_TransferR

SID_SR_HANDLER_MACRO_End
	incf	MIOS_PARAMETER1, F
	ENDM


SID_SR_Handler_Loop
	lfsr	FSR0, SIDL_SHADOW_BASE	; pointer to SIDL shadow registers
	lfsr	FSR1, SIDR_SHADOW_BASE	; pointer to SIDR shadow registers
	movlw	0xe0 | SIDx_V1_FRQ_L	; start address
	movwf	MIOS_PARAMETER1

	SET_BSR	SIDL_BASE
	IRQ_DISABLE			; disable interrupts to ensure consistency
	SID_SR_HANDLER_MACRO SIDx_V1_FRQ_L
	SID_SR_HANDLER_MACRO SIDx_V1_FRQ_H
	SID_SR_HANDLER_MACRO SIDx_V1_PW_L
	SID_SR_HANDLER_MACRO SIDx_V1_PW_H
	incf	MIOS_PARAMETER1, F	; skip CTRL register - transfered after OSC registers
	SID_SR_HANDLER_MACRO SIDx_V1_ENV_AD
	SID_SR_HANDLER_MACRO SIDx_V1_ENV_SR

	SID_SR_HANDLER_MACRO SIDx_V2_FRQ_L
	SID_SR_HANDLER_MACRO SIDx_V2_FRQ_H
	SID_SR_HANDLER_MACRO SIDx_V2_PW_L
	SID_SR_HANDLER_MACRO SIDx_V2_PW_H
	incf	MIOS_PARAMETER1, F	; skip CTRL register - transfered after OSC registers
	SID_SR_HANDLER_MACRO SIDx_V2_ENV_AD
	SID_SR_HANDLER_MACRO SIDx_V2_ENV_SR

	SID_SR_HANDLER_MACRO SIDx_V3_FRQ_L
	SID_SR_HANDLER_MACRO SIDx_V3_FRQ_H
	SID_SR_HANDLER_MACRO SIDx_V3_PW_L
	SID_SR_HANDLER_MACRO SIDx_V3_PW_H
	incf	MIOS_PARAMETER1, F	; skip CTRL register - transfered after OSC registers
	SID_SR_HANDLER_MACRO SIDx_V3_ENV_AD
	SID_SR_HANDLER_MACRO SIDx_V3_ENV_SR

#if ENABLE_SWINSID
	movlw	0xe0 | SIDx_SWINSID_V1_PHASE	; start address
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V1_PHASE
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V2_PHASE
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V3_PHASE
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V1_MODE
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V2_MODE
	SID_SR_HANDLER_MACRO SIDx_SWINSID_V3_MODE
#endif

	IRQ_ENABLE			; temporary enable interrupts for reduced IRQ latency
	IRQ_DISABLE

	movlw	0xe0 | SIDx_V1_CTRL	; now transfer V1_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V1_CTRL
	movlw	0xe0 | SIDx_V2_CTRL	; now transfer V2_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V2_CTRL
	movlw	0xe0 | SIDx_V3_CTRL	; now transfer V3_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V3_CTRL

	movlw	0xe0 | SIDx_FC_L	; transfer remaining registers
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_FC_L
	SID_SR_HANDLER_MACRO SIDx_FC_H
	SID_SR_HANDLER_MACRO SIDx_RES_FCHN
	SID_SR_HANDLER_MACRO SIDx_MODE_VOL

	;; check if phase sync has been requested
	SET_BSR	SID_BASE
	movf	SID_SE_PHASE_SYNC_REQ, W, BANKED
	bz	SID_SR_Handler_NoPhaseSync
SID_SR_Handler_PhaseSync
	;; if lead or bassline engine: variable phase, otherwise individual sync
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x02		; mask selects lead and bassline engine
	bnz	SID_SR_Handler_PhaseSync_M
SID_SR_Handler_PhaseSync_L
	rcall	SID_SR_VarPhase
  	rgoto	SID_SR_Handler_Loop	; update registers again

SID_SR_Handler_PhaseSync_M
	rcall	SID_SR_SyncPhase

SID_SR_Handler_NoPhaseSync

	;; sync with sound engine
	SET_BSR	SID_BASE
	clrf	SID_SE_SR_UPDATE_SYNC, BANKED

	;; enable interrupts again
	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  Transfer register value to left (L), right (R) or both (B) SIDs
;;  IN: address | 0xe0 (!) in MIOS_PARAMETER1, value in WREG
;;      pointer to SIDL shadow registers in FSR0
;;      pointer to SIDR shadow registers in FSR1
;; --------------------------------------------------------------------------
SID_SR_TransferL
	movwf	MIOS_PARAMETER2			; store value in MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W		; store value in SIDL shadow register
	andlw	0x1f
	movff	MIOS_PARAMETER2, PLUSW0

	rcall	SID_SR_Write_Sub		; transfer value

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


SID_SR_TransferR
	movwf	MIOS_PARAMETER2			; store value in MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W		; store value in SIDR shadow register
	andlw	0x1f
	movff	MIOS_PARAMETER2, PLUSW1

	rcall	SID_SR_Write_Sub		; transfer value

	;; synchronize with rising edge of SID clock to avoid setup or hold violation
	;; note: due to pipeline effects, the "bcf" will be executed 3 instruction cycles after
	;; the polling loop. Therefore we are waiting for the falling edge
	btfss	PORTC, 2; wait for falling clock edge
	bra $-2		
	btfsc	PORTC, 2
	bra $-2
	bcf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; enable write (MBHP_SID: chip select)
	bra	$+2				; to ensure compatibility with on-board oscillator,
	bra	$+2				; wait for 1.2 uS (> one SID clock cycle)
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	bsf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; disable write (MBHP_SID: chip select)
	return

	
SID_SR_TransferB
	movwf	MIOS_PARAMETER2			; store value in MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W		; store value in SIDL and SIDR shadow register
	andlw	0x1f
	movff	MIOS_PARAMETER2, PLUSW0
	movff	MIOS_PARAMETER2, PLUSW1

	rcall	SID_SR_Write_Sub		; transfer value

	;; synchronize with rising edge of SID clock to avoid setup or hold violation
	;; note: due to pipeline effects, the "bcf" will be executed 3 instruction cycles after
	;; the polling loop. Therefore we are waiting for the falling edge
	btfss	PORTC, 2; wait for falling clock edge
	bra $-2		
	btfsc	PORTC, 2
	bra $-2
	bcf	SID_SR_LAT_WR, SID_SR_PIN_WR	; enable write (MBHP_SID: chip select)
	bcf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; enable write (MBHP_SID: chip select)
	bra	$+2				; to ensure compatibility with on-board oscillator,
	bra	$+2				; wait for 1.2 uS (> one SID clock cycle)
	bra	$+2
	bra	$+2
	bra	$+2
	bra	$+2
	bsf	SID_SR_LAT_WR, SID_SR_PIN_WR	; disable write (MBHP_SID: chip select)
	bsf	SID_SR_LAT_WR2, SID_SR_PIN_WR2	; disable write (MBHP_SID: chip select)
	return

	
;; --------------------------------------------------------------------------
;;  SID Write: write to SID register
;; --------------------------------------------------------------------------
SID_SR_Write_Sub
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

	return


;; --------------------------------------------------------------------------
;;  Variable Oscillator Phase Offset for Lead Engine
;; --------------------------------------------------------------------------
SID_SR_VarPhase
	SET_BSR	SIDL_BASE

	;; temporary enable interrupts (especially to prevent MIDI receive buffer overflow)
	IRQ_ENABLE
	IRQ_DISABLE

	;; note: set the frequency to the maximum value (3.906kHz)
	;; this results into a period length of 256 uS
	;; now the offset between the oscillators can be adjusted by adding a n*1uS delay between the syncs
	movlw	0xe0 | SIDx_V1_FRQ_L
	movwf	MIOS_PARAMETER1
	movlw	LOW(65535)
	rcall	SID_SR_TransferB
	incf	MIOS_PARAMETER1, F
	movlw	HIGH(65535)
	rcall	SID_SR_TransferB

	movlw	0xe0 | SIDx_V2_FRQ_L
	movwf	MIOS_PARAMETER1
	movlw	LOW(65535)
	rcall	SID_SR_TransferB
	incf	MIOS_PARAMETER1, F
	movlw	HIGH(65535)
	rcall	SID_SR_TransferB

	movlw	0xe0 | SIDx_V3_FRQ_L
	movwf	MIOS_PARAMETER1
	movlw	LOW(65535)
	rcall	SID_SR_TransferB
	incf	MIOS_PARAMETER1, F
	movlw	HIGH(65535)
	rcall	SID_SR_TransferB

	;; change test flag -> gate
	bcf	SIDL_BASE + SIDx_V1_CTRL, 3, BANKED	; clear test flag
	bcf	SIDL_BASE + SIDx_V2_CTRL, 3, BANKED	; clear test flag
	bcf	SIDL_BASE + SIDx_V3_CTRL, 3, BANKED	; clear test flag
	bcf	SIDR_BASE + SIDx_V1_CTRL, 3, BANKED	; clear test flag
	bcf	SIDR_BASE + SIDx_V2_CTRL, 3, BANKED	; clear test flag
	bcf	SIDR_BASE + SIDx_V3_CTRL, 3, BANKED	; clear test flag

	;; set gate if sync was requested (accordingly test flag was set before)
	movff	SID_SE_PHASE_SYNC_REQ, WREG
	btfsc	WREG, 0
	bsf	SIDL_BASE + SIDx_V1_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 1
	bsf	SIDL_BASE + SIDx_V2_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 2
	bsf	SIDL_BASE + SIDx_V3_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 3
	bsf	SIDR_BASE + SIDx_V1_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 4
	bsf	SIDR_BASE + SIDx_V2_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 5
	bsf	SIDR_BASE + SIDx_V3_CTRL, 0, BANKED	; set gate flag

	;; update registers
	movlw	0xe0 | SIDx_V1_CTRL	; now transfer V1_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V1_CTRL

	;; add n*1uS delay
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_OSC_PHASE, WREG
	addlw	14	; compensation
	skpnc
	movlw	255
	movwf	TMP1
SID_SR_VarPhase_Loop1
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	decfsz	TMP1, F
	rgoto	SID_SR_VarPhase_Loop1

	movlw	0xe0 | SIDx_V2_CTRL	; now transfer V2_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V2_CTRL

	;; add n*1uS delay
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_OSC_PHASE, WREG
	addlw	14	; compensation
	skpnc
	movlw	255
	movwf	TMP1
SID_SR_VarPhase_Loop2
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	decfsz	TMP1, F
	rgoto	SID_SR_VarPhase_Loop2

	movlw	0xe0 | SIDx_V3_CTRL	; now transfer V3_CTRL register
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V3_CTRL

	;; clear sync requests
	SET_BSR	SID_BASE
	clrf	SID_SE_PHASE_SYNC_REQ, BANKED

	return



;; --------------------------------------------------------------------------
;;  Individual Oscillator Phase Reset for Multi, Bassline and Drum Engine
;; --------------------------------------------------------------------------
SID_SR_SyncPhase
	SET_BSR	SIDL_BASE

	;; change test flag -> gate
	movff	SID_SE_PHASE_SYNC_REQ, WREG
	btfsc	WREG, 0
	bcf	SIDL_BASE + SIDx_V1_CTRL, 3, BANKED	; clear test flag
	btfsc	WREG, 1
	bcf	SIDL_BASE + SIDx_V2_CTRL, 3, BANKED	; clear test flag
	btfsc	WREG, 2
	bcf	SIDL_BASE + SIDx_V3_CTRL, 3, BANKED	; clear test flag
	btfsc	WREG, 3
	bcf	SIDR_BASE + SIDx_V1_CTRL, 3, BANKED	; clear test flag
	btfsc	WREG, 4
	bcf	SIDR_BASE + SIDx_V2_CTRL, 3, BANKED	; clear test flag
	btfsc	WREG, 5
	bcf	SIDR_BASE + SIDx_V3_CTRL, 3, BANKED	; clear test flag

	;; set gate if sync was requested (accordingly test flag was set before)
	btfsc	WREG, 0
	bsf	SIDL_BASE + SIDx_V1_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 1
	bsf	SIDL_BASE + SIDx_V2_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 2
	bsf	SIDL_BASE + SIDx_V3_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 3
	bsf	SIDR_BASE + SIDx_V1_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 4
	bsf	SIDR_BASE + SIDx_V2_CTRL, 0, BANKED	; set gate flag
	btfsc	WREG, 5
	bsf	SIDR_BASE + SIDx_V3_CTRL, 0, BANKED	; set gate flag

	;; update registers
	movlw	0xe0 | SIDx_V1_CTRL
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V1_CTRL

	movlw	0xe0 | SIDx_V2_CTRL
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V2_CTRL

	movlw	0xe0 | SIDx_V3_CTRL
	movwf	MIOS_PARAMETER1
	SID_SR_HANDLER_MACRO SIDx_V3_CTRL

	;; clear sync requests
	SET_BSR	SID_BASE
	clrf	SID_SE_PHASE_SYNC_REQ, BANKED

	return
