; $Id$
;
; MIDIbox FM
; Sound Engine
;
; Activate this #define to measure the performance with a scope
; (connect the probe to J14)
#define MBFM_SE_MEASURE_PERFORMANCE 0
;
; force worst case execution time
; (1: worst case if no LFO/EG value change, 2: total worst case)
#define MBFM_SE_MEASURE_WORST_CASE 0
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Handler
;;  DESCRIPTION: This handler is called every millisecond to process the
;;  software implemented synth engine features
;;  OUT:  -
;;  USES: IRQ_TMP[12345] and MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
MBFM_SE_Handler
	;; exit if engine is disabled
	btfsc	MBFM_STAT, MBFM_STAT_ENGINE_DISABLE
	return

	clrwdt			; feed watchdog

	SET_BSR	MBFM_BASE

#if MBFM_SE_MEASURE_PERFORMANCE
	bsf	PORTD, 4
#endif

	;; we have two update cycles:
	;; first cycle when MBFM_SE_STAT_CYCLE2 cleared: process voice 0-2
	;; second cycle when MBFM_SE_STAT_CYCLE2 set: process voice 3-6
	;; once MBFM_SE_STAT_CYCLE2 toggles to 0 again, MBFM_SE_STAT_UPDATE_REGS will be set to synch with MBFM_REG_Update

	btg	MBFM_SE_STAT, MBFM_SE_STAT_CYCLE2, BANKED
	btfsc	MBFM_SE_STAT, MBFM_SE_STAT_CYCLE2, BANKED
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_UPDATE_REGS, BANKED


	movff	PRODL, SAVED_PRODL	; save PROD[LH] - we are in an interrupt routine
	movff	PRODH, SAVED_PRODH
	movff	FSR0L, SAVED_FSR0L	; save FSR0[LH] - we are in an interrupt routine
	movff	FSR0H, SAVED_FSR0H

	;; generate new pseudo-random number
	call	MBFM_RND_GenRandomNumber

	lfsr	FSR1, MBFM_V1_BASE
	BRA_IFCLR MBFM_SE_STAT, MBFM_SE_STAT_CYCLE2, BANKED, MBFM_SE_Handler_NotCycle2
	movlw	(MBFM_Vx_NUM/2) * MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F	
MBFM_SE_Handler_NotCycle2

	clrf	MBFM_SE_VOICE_CTR, BANKED
MBFM_SE_Handler_Loop
	;; calc pointer to assigned instrument (if instrument is 0xff, voice is not used)
	movlw	MBFM_Vx_ASSIGNED_INSTR
	incf	PLUSW1, W
	bz	MBFM_SE_Handler_Loop_Next

	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	mullw	MBFM_Ix_RECORD_LEN
	lfsr	FSR0, MBFM_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F

	;; request the update of all voice registers if VOICE_UPDATE is set
	movlw	MBFM_Vx_STAT
	BRA_IFCLR PLUSW1, MBFM_STAT_VOICE_REFRESH, ACCESS, MBFM_SE_Handler_NoVoiceRefresh
	bcf	PLUSW1, MBFM_STAT_VOICE_REFRESH	; clear request
MBFM_SE_Handler_VoiceRefresh
	movlw	MBFM_Vx_UPDATE0
	setf	PLUSW1
	bcf	PLUSW1, 7
	movlw	MBFM_Vx_UPDATE1
	setf	PLUSW1
	bcf	PLUSW1, 7
MBFM_SE_Handler_NoVoiceRefresh

	;; clear the "value changed" flag
	bcf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED

	movlw	MBFM_Vx_STAT
	BRA_IFCLR PLUSW1, MBFM_STAT_GATE_CLR_REQ, ACCESS, MBFM_SE_Handler_Loop_NoGateClr
	bcf	PLUSW1, MBFM_STAT_GATE_CLR_REQ	; clear request
MBFM_SE_Handler_Loop_GateClr
	movlw	MBFM_Vx_STAT			; sync with MBFM_REG_Update
	bsf	PLUSW1, MBFM_STAT_UPDATE_NOT_DONE
	;; clear gate bit in FNUMH register and request update
	movlw	MBFM_Vx_FNUMH
	bcf	PLUSW1, 5
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_FNUMH
	;; release EG(s)
	call	MBFM_SE_EG_Release
	;; notify that mod values have been changed
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED
	rgoto	MBFM_SE_Handler_Loop_Gate_End
MBFM_SE_Handler_Loop_NoGateClr

	movlw	MBFM_Vx_STAT			; wait until last update done
	BRA_IFSET PLUSW1, MBFM_STAT_UPDATE_NOT_DONE, ACCESS, MBFM_SE_Handler_Loop_Gate_End
	BRA_IFCLR PLUSW1, MBFM_STAT_GATE_SET_REQ, ACCESS, MBFM_SE_Handler_Loop_NoGateSet
	bcf	PLUSW1, MBFM_STAT_GATE_SET_REQ		; clear request
MBFM_SE_Handler_Loop_GateSet
	;; set gate bit in FNUMH register and request update
	movlw	MBFM_Vx_FNUMH
	bsf	PLUSW1, 5
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_FNUMH
	;; sync LFOs
	rcall	MBFM_SE_LFO_Sync
	;; restart EG(s)
	call	MBFM_SE_EG_Restart
	;; notify that mod values have been changed
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED
#if DISPLAY_PERFORMANCE == 0
	;; service dedicated meter for instrument
	lfsr	FSR2, METER_VALUE_I1
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	andlw	0x03
	addwf	FSR2L, F
	movlw	0xff
	movwf	INDF2
#endif

	;; 	rgoto	MBFM_SE_Handler_Loop_Gate_End
MBFM_SE_Handler_Loop_NoGateSet
MBFM_SE_Handler_Loop_Gate_End

	;; the FRQ_SWEEP_ACTIVE flag will be set if portamento/LFO/EG active
	bcf	MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED
	;; sweep frequency of portamento enabled
#if MBFM_SE_MEASURE_WORST_CASE < 1
	movlw	MBFM_Vx_STAT
	btfsc	PLUSW1, MBFM_STAT_PORTA_ACTIVE
#endif
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED

	;; call LFO and EG handlers
	rcall	MBFM_SE_LFO
	rcall	MBFM_SE_EG

	;; Note: MBFM_SE_Note and MBFM_SE_Pitch can force the MOD_VALUE_CHANGED flag!
	movlw	MBFM_Vx_STAT
	BRA_IFCLR PLUSW1, MBFM_STAT_NOTE_UPDATE_REQ, ACCESS, MBFM_SE_Handler_Loop_Note_End
	bcf	PLUSW1, MBFM_STAT_NOTE_UPDATE_REQ	; clear request
MBFM_SE_Handler_Loop_Note
	rcall	MBFM_SE_Note			; get note and transfer to FNUML/H registers
MBFM_SE_Handler_Loop_Note_End

	;; update KSL_TL registers
	rcall	MBFM_SE_Vol

	BRA_IFCLR MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED, MBFM_SE_Handler_Loop_Frq_End
MBFM_SE_Handler_Loop_Frq
	rcall	MBFM_SE_Pitch			; modify FNUML/H registers
MBFM_SE_Handler_Loop_Frq_End
	
MBFM_SE_Handler_Loop_Next
	;; increment base pointer to next voice and loop until last voice reached
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	MBFM_SE_VOICE_CTR, F, BANKED
	movlw	(MBFM_Vx_NUM/2)-1
	cpfsgt	MBFM_SE_VOICE_CTR, BANKED
	rgoto MBFM_SE_Handler_Loop


	;; do some additional things at the first clock cycle
	BRA_IFSET MBFM_SE_STAT, MBFM_SE_STAT_UPDATE_REGS, BANKED, MBFM_SE_Handler_NoAOUTUpdate
MBFM_SE_Handler_AOUTUpdate
#if AOUT_INTERFACE_TYPE
	call	MBFM_SE_AOUT
#endif
MBFM_SE_Handler_NoAOUTUpdate

	;; do some additional things at the last clock cycle
	BRA_IFCLR MBFM_SE_STAT, MBFM_SE_STAT_UPDATE_REGS, BANKED, MBFM_SE_Handler_NoDrUpdate
MBFM_SE_Handler_DrUpdate
	call	MBFM_SE_DrumFrqDec
	call	MBFM_SE_Clk
	call	MBFM_SE_Wt
MBFM_SE_Handler_NoDrUpdate

	movff	SAVED_FSR0L, FSR0L		; copy back saved FSR0[LH]
	movff	SAVED_FSR0H, FSR0H
	movff	SAVED_PRODL, PRODL		; copy back saved PROD[LH]
	movff	SAVED_PRODH, PRODH

#if MBFM_SE_MEASURE_PERFORMANCE
	bcf	PORTD, 4
#endif

	return


	
;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Note
;;  DESCRIPTION: help function which handles the note/pitchbender value
;; --------------------------------------------------------------------------
MBFM_SE_Note
	;; save note in IRQ_TMP5
	movlw	MBFM_Vx_NOTE
	movff	PLUSW1, IRQ_TMP5

	;; transpose it
	movlw	MBFM_Ix_TRANSPOSE
	movf	PLUSW0, W
	sublw	0x40
	xorlw	0xff
	addlw	1
	addwf	IRQ_TMP5, F
	BRA_IFCLR IRQ_TMP5, 7, ACCESS, MBFM_SE_Note_NoTrnsOv
	;; saturate on overflow
	movlw	MBFM_Ix_TRANSPOSE
	BRA_IFCLR PLUSW0, 6, ACCESS, MBFM_SE_Note_TrnsOvNeg
MBFM_SE_Note_TrnsOvPos
	movlw	0x7f
	movwf	IRQ_TMP5
 	rgoto	MBFM_SE_Note_NoTrnsOv
MBFM_SE_Note_TrnsOvNeg
	clrf	IRQ_TMP5
	;; 	rgoto	MBFM_SE_Note_NoTrnsOv
MBFM_SE_Note_NoTrnsOv
	
	;; if frequency sweep active, or finetune/pitchbender is used, use less accurate frq table
	BRA_IFSET MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED, MBFM_SE_Note_Mod
	movlw	MBFM_Ix_PITCHRANGE
	movf	PLUSW0, W
	bnz	MBFM_SE_Note_Mod
#if MBFM_SE_MEASURE_WORST_CASE < 1
	rgoto	MBFM_SE_Note_HighAcc
#endif

	;; less accuracy (for modulated notes)
MBFM_SE_Note_Mod
	;; force modulation so that FNUM[LH] will be updated
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED

	;; save target frequency in MBFM_SE_TARGET_FRQ_[LH]
	movf	IRQ_TMP5, W			; IRQ_TMP5 contains the transposed note
	call	MBFM_FRQ_ALT_Get		; get FNUM[LH] from table, result in MIOS_PARAMETER[12]
	movff	MIOS_PARAMETER1, MBFM_SE_TARGET_FRQ_L
	movff	MIOS_PARAMETER2, MBFM_SE_TARGET_FRQ_H

	;; pitch result stored in MUL_A_[LH]
	clrf	MUL_A_L, BANKED
	clrf	MUL_A_H, BANKED

	;; calc MUL_A_[LH] = pitchbender+finetune (9-bit signed)
	movlw	MBFM_Ix_PITCHBENDER
	movf	PLUSW0, W
	movwf	IRQ_TMP1
	bz	MBFM_SE_Note_Mod_NoPP
MBFM_SE_Note_Mod_PP
	addwf	MUL_A_L, F, BANKED
	movlw	0x00
	btfsc	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_A_H, F, BANKED
MBFM_SE_Note_Mod_NoPP

	movlw	MBFM_Ix_FINETUNE
	clrc
	rlf	PLUSW0, W
	xorlw	0x80
	movwf	IRQ_TMP1
	bz	MBFM_SE_Note_Mod_NoFT
MBFM_SE_Note_Mod_FT
	addwf	MUL_A_L, F, BANKED
	movlw	0x00
	btfsc	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_A_H, F, BANKED
MBFM_SE_Note_Mod_NoFT

	;; unisono feature only possible in poly mode
	movlw	MBFM_Ix_CTRL1
	BRA_IFCLR PLUSW0, MBFM_CTRL1_POLY, ACCESS, MBFM_SE_Note_Mod_NoUS
	movlw	MBFM_Ix_UNISONO
	movf	PLUSW0, W
	movwf	IRQ_TMP1
	bz	MBFM_SE_Note_Mod_NoUS
MBFM_SE_Note_Mod_US
	movlw	MBFM_Vx_STAT
	BRA_IFCLR PLUSW1, MBFM_STAT_UNISONO_2ND_VOICE, ACCESS, MBFM_SE_Note_Mod_US_Not2nd
MBFM_SE_Note_Mod_US_2nd
	comf	IRQ_TMP1, F
MBFM_SE_Note_Mod_US_Not2nd
	movf	IRQ_TMP1, W
	addwf	MUL_A_L, F, BANKED
	movlw	0x00
	btfsc	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_A_H, F, BANKED
MBFM_SE_Note_Mod_NoUS

	;; skip tuning if no frequency sweep and MUL_A_[LH] == zero
	movf	MUL_A_L, W, BANKED
	iorwf	MUL_A_H, W, BANKED
	bnz	MBFM_SE_Note_Mod_ABS16
	BRA_IFSET MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED, MBFM_SE_Note_Mod_AddMulCont
#if MBFM_SE_MEASURE_WORST_CASE < 1
	rgoto	MBFM_SE_Note_HighAcc
#endif

	;; else convert MUL_A_[LH] to unsigned integer, keep sign in IRQ_TMP1[0]
MBFM_SE_Note_Mod_ABS16
	clrf	IRQ_TMP1
	clrc
	BRA_IFCLR MUL_A_H, 7, BANKED, MBFM_SE_Note_Mod_ABS16_Pos
MBFM_SE_Note_Mod_ABS16_Neg
	bsf	IRQ_TMP1, 0	; save sign in IRQ_TMP1[0]
	comf	MUL_A_L, F, BANKED
	comf	MUL_A_H, F, BANKED
	setc
MBFM_SE_Note_Mod_ABS16_Pos
	;; *2
	rlf	MUL_A_L, F, BANKED
	rlf	MUL_A_H, F, BANKED
	
	;; add pitchrange depending on direction with saturation
	BRA_IFSET IRQ_TMP1, 0, ACCESS, MBFM_SE_Note_Mod_PDec
MBFM_SE_Note_Mod_PInc
	movlw	MBFM_Ix_PITCHRANGE
	movf	PLUSW0, W
	addwf	IRQ_TMP5, W	; IRQ_TMP5 contains the transposed note
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	MBFM_SE_Note_Mod_PCont
MBFM_SE_Note_Mod_PDec
	movlw	MBFM_Ix_PITCHRANGE
	movf	PLUSW0, W
	subwf	IRQ_TMP5, W	; IRQ_TMP5 contains the transposed note
	btfsc	WREG, 7
	movlw 0x00
	;; 	rgoto	MBFM_SE_Note_Mod_PCont
MBFM_SE_Note_Mod_PCont

	;; get note +/- pitchrange
	call	MBFM_FRQ_ALT_Get		; get FNUM[LH] from table, result in MIOS_PARAMETER[12]

	;; add and multiply to target frequency
	BRA_IFCLR IRQ_TMP1, 0, ACCESS, MBFM_SE_Note_Mod_AddMulP
MBFM_SE_Note_Mod_AddMulN
	;; calc MUL_B_[LH] = MBFM_Vx_FRQ_[LH] - MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	subwf	MBFM_SE_TARGET_FRQ_L, W, BANKED
	movwf	MUL_B_L, BANKED
	movf	MIOS_PARAMETER2, W
	subwfb	MBFM_SE_TARGET_FRQ_H, W, BANKED
	movwf	MUL_B_H, BANKED

	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; MBFM_Vx_FRQ -= result
	movf	MUL_R_1, W, BANKED
	subwf	MBFM_SE_TARGET_FRQ_L, F, BANKED
	movf	MUL_R_2, W, BANKED
	subwfb	MBFM_SE_TARGET_FRQ_H, F, BANKED
	rgoto	MBFM_SE_Note_Mod_AddMulCont

MBFM_SE_Note_Mod_AddMulP
	;; calc MUL_B_[LH] = MIOS_PARAMETER[12] - MBFM_Vx_FRQ_[LH]
	movf	MBFM_SE_TARGET_FRQ_L, W, BANKED
	subwf	MIOS_PARAMETER1, W
	movwf	MUL_B_L, BANKED
	movf	MBFM_SE_TARGET_FRQ_H, W, BANKED
	subwfb	MIOS_PARAMETER2, W
	movwf	MUL_B_H, BANKED

	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; MBFM_Vx_FRQ += result
	movf	MUL_R_1, W, BANKED
	addwf	MBFM_SE_TARGET_FRQ_L, F, BANKED
	movf	MUL_R_2, W, BANKED
	addwfc	MBFM_SE_TARGET_FRQ_H, F, BANKED
	;; 	rgoto	MBFM_SE_Note_Mod_AddMulCont

MBFM_SE_Note_Mod_AddMulCont

	;; copy target frequency to MBFM_Vx_TARGET_FRQ_[LH]
	movlw	MBFM_Vx_TARGET_FRQ_L
	movff	MBFM_SE_TARGET_FRQ_L, PLUSW1
	movlw	MBFM_Vx_TARGET_FRQ_H
	movff	MBFM_SE_TARGET_FRQ_H, PLUSW1

	;; thats all - don't update FNUM if frequency will be sweeped
	BRA_IFSET MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED, MBFM_SE_Note_End

	;; save current frequency in MBFM_Vx_FRQ_[LH]
	movlw	MBFM_Vx_FRQ_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	MBFM_Vx_FRQ_H
	movff	MIOS_PARAMETER2, PLUSW1

	;; copy target frequency to MIOS_PARAMETER[12]
	movff	MBFM_SE_TARGET_FRQ_L, MIOS_PARAMETER1
	movff	MBFM_SE_TARGET_FRQ_H, MIOS_PARAMETER2

	;; convert MIOS_PARAMETER[12] to FNUM format
	rcall	MBFM_SE_Frq2Fnum

	rgoto	MBFM_SE_Note_Cont


	;; high accuracy (for common notes)
MBFM_SE_Note_HighAcc
	;; save target frequency in MBFM_Vx_TARGET_FRQ_[LH] and MBFM_Vx_FRQ_[LH]
	movf	IRQ_TMP5, W			; IRQ_TMP5 contains the transposed note
	call	MBFM_FRQ_ALT_Get		; get FNUM[LH] from table, result in MIOS_PARAMETER[12]
	movlw	MBFM_Vx_TARGET_FRQ_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	MBFM_Vx_TARGET_FRQ_H
	movff	MIOS_PARAMETER2, PLUSW1

	;; save current frequency in MBFM_Vx_FRQ_[LH]
	movlw	MBFM_Vx_FRQ_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	MBFM_Vx_FRQ_H
	movff	MIOS_PARAMETER2, PLUSW1

	;; ge high accuracy frequency
	movf	IRQ_TMP5, W			; IRQ_TMP5 contains the transposed note
	call	MBFM_FRQ_Get			; get FNUM[LH] from table, result in MIOS_PARAMETER[12]
	;; 	rgoto	MBFM_SE_Note_Cont

MBFM_SE_Note_Cont
MBFM_SE_Pitch_Cont
	;; copy MIOS_PARAMETER[12] to FNUM[LH]
	movlw	MBFM_Vx_FNUML
	movff	MIOS_PARAMETER1, PLUSW1

	;; (don't overwrite gate bit)
	movlw	MBFM_Vx_FNUMH
	movf	PLUSW1, W
	andlw	0x20
	iorwf	MIOS_PARAMETER2, F
	movlw	MBFM_Vx_FNUMH
	movff	MIOS_PARAMETER2, PLUSW1

	;; request update of FNUML and FNUMH register
	movlw	MBFM_Vx_UPDATE0
	bsf	PLUSW1, MBFM_UPDATE0_FNUML
	bsf	PLUSW1, MBFM_UPDATE0_FNUMH

MBFM_SE_Note_End
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Pitch
;;  DESCRIPTION: help function which handles frequency sweeps
;; --------------------------------------------------------------------------
MBFM_SE_Pitch
	;; copy target frequency to MBFM_SE_TARGET_FRQ_[LH]
	movlw	MBFM_Vx_TARGET_FRQ_L
	movff	PLUSW1, MBFM_SE_TARGET_FRQ_L
	movlw	MBFM_Vx_TARGET_FRQ_H
	movff	PLUSW1, MBFM_SE_TARGET_FRQ_H

	;; copy current frequency to MBFM_SE_FRQ_[LH]
	movlw	MBFM_Vx_FRQ_L
	movff	PLUSW1, MBFM_SE_FRQ_L
	movlw	MBFM_Vx_FRQ_H
	movff	PLUSW1, MBFM_SE_FRQ_H

	;; don't handle portamento if not active
	movlw	MBFM_Vx_STAT
#if MBFM_SE_MEASURE_WORST_CASE < 1
	BRA_IFCLR PLUSW1, MBFM_STAT_PORTA_ACTIVE, ACCESS, MBFM_SE_Pitch_NoPorta
#endif
MBFM_SE_Pitch_Porta
	;; force modulation so long portamento is active
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED

	;; multiply rate with current frequency
	;; get portamento multiplier from envelope table -> MUL_A
	movlw	MBFM_Ix_PORTA_RATE
	movf	PLUSW0, W
	call	MBFM_EG_TABLE_Get
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2

	;; get current frequency -> MUL_A and MBFM_SE_FRQ_[LH]
	movff	MBFM_SE_FRQ_L, MUL_A_L
	movff	MBFM_SE_FRQ_H, MUL_A_H
	movff	MIOS_PARAMETER1, MUL_B_L
	movff	MIOS_PARAMETER2, MUL_B_H

	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)
	;; ensure that result is != 0
	movf	MUL_R_2, W, BANKED
	iorwf	MUL_R_3, W, BANKED
	skpnz
	incf	MUL_R_2, F, BANKED

	;; MBFM_Vx_FRQ += result (depending on Portamento Direction)
	;; branch depending on portamento direction
	;; check if value > current value
	movf	MBFM_SE_TARGET_FRQ_L, W, BANKED
	subwf	MBFM_SE_FRQ_L, W, BANKED
	movf	MBFM_SE_TARGET_FRQ_H, W, BANKED
	subwfb	MBFM_SE_FRQ_H, W, BANKED
	bc	MBFM_SE_Pitch_Porta_Down

MBFM_SE_Pitch_Porta_Up
	movf	MUL_R_2, W, BANKED
	addwf	MBFM_SE_FRQ_L, F, BANKED
	movf	MUL_R_3, W, BANKED
	addwfc	MBFM_SE_FRQ_H, F, BANKED

	;; check if value > MAX_VALUE
	movf	MBFM_SE_FRQ_L, W, BANKED
	subwf	MBFM_SE_TARGET_FRQ_L, W, BANKED
	movf	MBFM_SE_FRQ_H, W, BANKED
	subwfb	MBFM_SE_TARGET_FRQ_H, W, BANKED
	bc	MBFM_SE_Pitch_Porta_End	; branch to end if MAX_VALUE not reached
	rgoto	MBFM_SE_Pitch_Porta_Up_Cont	; else copy MAX_VALUE into value and finish portamento

MBFM_SE_Pitch_Porta_Down
	movf	MUL_R_2, W, BANKED
	subwf	MBFM_SE_FRQ_L, F, BANKED
	movf	MUL_R_3, W, BANKED
	subwfb	MBFM_SE_FRQ_H, F, BANKED

	;; check if value < MIN_VALUE
	movf	MBFM_SE_TARGET_FRQ_L, W, BANKED
	subwf	MBFM_SE_FRQ_L, W, BANKED
	movf	MBFM_SE_TARGET_FRQ_H, W, BANKED
	subwfb	MBFM_SE_FRQ_H, W, BANKED
	bc	MBFM_SE_Pitch_Porta_End	; branch to end if MIN_VALUE not reached
	; else copy target frequency into SE_FRQ and finish portamento
MBFM_SE_Pitch_Porta_Up_Cont
	movlw	MBFM_Vx_STAT
	bcf	PLUSW1, MBFM_STAT_PORTA_ACTIVE

MBFM_SE_Pitch_NoPorta
	movff	MBFM_SE_TARGET_FRQ_L, MBFM_SE_FRQ_L
	movff	MBFM_SE_TARGET_FRQ_H, MBFM_SE_FRQ_H
MBFM_SE_Pitch_Porta_End


	;; exit if no modulation (neither portamento, nor LFO/EG)
#if MBFM_SE_MEASURE_WORST_CASE < 2
	btfss	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED
	return
#endif


	;; save current frequency in MBFM_Vx_FRQ_[LH]
	movlw	MBFM_Vx_FRQ_L
	movff	MBFM_SE_FRQ_L, PLUSW1
	movlw	MBFM_Vx_FRQ_H
	movff	MBFM_SE_FRQ_H, PLUSW1

	;; add modulation sources to frequency
	;; signed value in MBFM_SE_MOD_RESULT_[LHU]
	clrf	MBFM_SE_MOD_RESULT_L, BANKED
	clrf	MBFM_SE_MOD_RESULT_H, BANKED
	clrf	MBFM_SE_MOD_RESULT_U, BANKED

MBFM_SE_Pitch_Mod
	;; add LFO1 value if depth != 0x40
	movlw	MBFM_Ix_LFO1_DEPTH_P
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Pitch_NoLFO1
#endif
MBFM_SE_Pitch_LFO1
	movlw	MBFM_Ix_LFO1_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO1_STAT
	movff	PLUSW1, IRQ_TMP3
	movlw	MBFM_Vx_LFO1_VALUE
	movf	PLUSW1, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_Pitch_NoLFO1

	;; add LFO2 value if requested
	movlw	MBFM_Ix_LFO2_DEPTH_P
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Pitch_NoLFO2
#endif
MBFM_SE_Pitch_LFO2
	movlw	MBFM_Ix_LFO2_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO2_STAT
	movff	PLUSW1, IRQ_TMP3
	movlw	MBFM_Vx_LFO2_VALUE
	movf	PLUSW1, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_Pitch_NoLFO2

	;; add EG5 value if requested
	movlw	MBFM_Ix_EG5_DEPTH_P
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Pitch_NoEG5
#endif
MBFM_SE_Pitch_EG5
	movlw	MBFM_Vx_EG5_CTR_H
	movf	PLUSW1, W
	rcall	MBFM_SE_EG_Hlp_Waveform
MBFM_SE_Pitch_NoEG5

	;; add to frequency and copy final result to MIOS_PARAMETER[12]
#if 1
	;; new in MBFM V1.1d: decrease intensity of pitch depth because of multiple user complaints
	;; note: more than 2 bit doesn't lead to better effects due to reduced frq resolution
	clrc
	btfsc	MBFM_SE_MOD_RESULT_U, 7, BANKED
	setc
	rrf	MBFM_SE_MOD_RESULT_U, F, BANKED
	rrf	MBFM_SE_MOD_RESULT_H, F, BANKED
	rrf	MBFM_SE_MOD_RESULT_L, F, BANKED

	clrc
	btfsc	MBFM_SE_MOD_RESULT_U, 7, BANKED
	setc
	rrf	MBFM_SE_MOD_RESULT_U, F, BANKED
	rrf	MBFM_SE_MOD_RESULT_H, F, BANKED
	rrf	MBFM_SE_MOD_RESULT_L, F, BANKED
#endif

	movf	MBFM_SE_FRQ_L, W, BANKED
	addwf	MBFM_SE_MOD_RESULT_L, W, BANKED
	movwf	MIOS_PARAMETER1
	movf	MBFM_SE_FRQ_H, W, BANKED
	addwfc	MBFM_SE_MOD_RESULT_H, W, BANKED
	movwf	MIOS_PARAMETER2
	movlw	0x00
	addwfc	MBFM_SE_MOD_RESULT_U, W, BANKED
	bz	MBFM_SE_Pitch_SatCont

	;; saturate
MBFM_SE_Pitch_Sat
	BRA_IFSET MBFM_SE_MOD_RESULT_U, 7, BANKED, MBFM_SE_Pitch_SatNeg
MBFM_SE_Pitch_SatPos
	setf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	rgoto	MBFM_SE_Pitch_SatCont

MBFM_SE_Pitch_SatNeg
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
MBFM_SE_Pitch_SatCont

	;; convert MIOS_PARAMETER[12] to FNUM format
	rcall	MBFM_SE_Frq2Fnum

	;; transfer FNUM
	rgoto	MBFM_SE_Pitch_Cont




;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Fnum2Frq
;;  DESCRIPTION: help function to convert FNUM format to
;;  internal frequency format
;;  IN: FNUM value in MIOS_PARAMETER[12]
;;  OUT: converted value in MIOS_PARAMETER[12]
;;  USES: -
;; --------------------------------------------------------------------------
	;; not used (dead code)
#if 0
MBFM_SE_Fnum2Frq
	;; FRQ = FNUM << 6
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F
	
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F
	
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F
	
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F
	
	rlf	MIOS_PARAMETER1, F
	rlf	MIOS_PARAMETER2, F
	
	rlf	MIOS_PARAMETER1, W
	andlw	0xc0
	movwf	MIOS_PARAMETER1
	rlf	MIOS_PARAMETER2, F
	return
#endif

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Frq2Fnum
;;  DESCRIPTION: help function to convert internal frequency format to FNUM
;;  IN: frequency value in MIOS_PARAMETER[12]
;;  OUT: converted value in MIOS_PARAMETER[12]
;;  USES: -
;; --------------------------------------------------------------------------
MBFM_SE_Frq2Fnum
	;; (FNUM >> 6) | (block << 10)
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F

	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F

	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F

	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F

	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F

	rrf	MIOS_PARAMETER2, W
	andlw	0x03
	iorlw	5 << 2			; block bitfield is always 5!
	movwf	MIOS_PARAMETER2
	rrf	MIOS_PARAMETER1, F
	
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Vol
;;  DESCRIPTION: help function which handles the volume register
;; --------------------------------------------------------------------------
MBFM_SE_Vol
	;; using a macro to optimize for speed
MBFM_SE_VOL_OPx	MACRO OPx, Vx_OPx_VOL, Ix_OPx_KSL_TL, Vx_OPx_KSL_TL, UPDATE1_OPx_KSL_TL
	LOCAL	MBFM_SE_VOL_OPx_Change
	LOCAL	MBFM_SE_VOL_OPx_NoChange

	movlw	OPx			; copy number of operator to MBFM_SE_OP
	movwf	MBFM_SE_OP, BANKED
	movlw	Vx_OPx_VOL		; copy volume to MBFM_SE_MOD_VALUE
	movff	PLUSW1, MBFM_SE_MOD_VALUE
	rcall	MBFM_SE_Vol_Mod		; modulate
	movlw	Ix_OPx_KSL_TL		; copy KSL to bit 7..6
	movf	PLUSW0, W
	andlw	0xc0
	iorwf	MBFM_SE_MOD_VALUE, F, BANKED
	movlw	Vx_OPx_KSL_TL		; compare old with new value
	movf	PLUSW1, W
	xorwf	MBFM_SE_MOD_VALUE, W, BANKED
	bz	MBFM_SE_VOL_OPx_NoChange
MBFM_SE_VOL_OPx_Change
	movlw	Vx_OPx_KSL_TL		; copy to target register if not equal
	movff	MBFM_SE_MOD_VALUE, PLUSW1
	movlw	MBFM_Vx_UPDATE1		; and request update
	bsf	PLUSW1, UPDATE1_OPx_KSL_TL
MBFM_SE_VOL_OPx_NoChange
	ENDM

	;; do nothing if none of the LFO/EG values have been changed
#if MBFM_SE_MEASURE_WORST_CASE < 2
	BRA_IFCLR MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED, MBFM_SE_Vol_End
#endif

	MBFM_SE_VOL_OPx 0, MBFM_Vx_OP1_VOL, MBFM_Ix_OP1_KSL_TL, MBFM_Vx_OP1_KSL_TL, MBFM_UPDATE1_OP1_KSL_TL
	MBFM_SE_VOL_OPx 1, MBFM_Vx_OP2_VOL, MBFM_Ix_OP2_KSL_TL, MBFM_Vx_OP2_KSL_TL, MBFM_UPDATE1_OP2_KSL_TL
	MBFM_SE_VOL_OPx 2, MBFM_Vx_OP3_VOL, MBFM_Ix_OP3_KSL_TL, MBFM_Vx_OP3_KSL_TL, MBFM_UPDATE1_OP3_KSL_TL
	MBFM_SE_VOL_OPx 3, MBFM_Vx_OP4_VOL, MBFM_Ix_OP4_KSL_TL, MBFM_Vx_OP4_KSL_TL, MBFM_UPDATE1_OP4_KSL_TL

MBFM_SE_Vol_End
	return

;; subroutine which modulates the volume
;; IN: current volume in MBFM_SE_MOD_VALUE
;;     current operator in MBFM_SE_OP
;; OUT: modulated volume in MBFM_SE_MOD_VALUE
MBFM_SE_Vol_Mod
	;; signed value in MBFM_SE_MOD_RESULT_[LHU]
	clrf	MBFM_SE_MOD_RESULT_L, BANKED
	clrf	MBFM_SE_MOD_RESULT_H, BANKED
	clrf	MBFM_SE_MOD_RESULT_U, BANKED

	;; handle LFO1
	movlw	MBFM_Ix_LFO1_DEPTH_V1
	addwf	MBFM_SE_OP, W, BANKED
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Vol_Mod_NoLFO1
#endif
MBFM_SE_Vol_Mod_LFO1
	movlw	MBFM_Ix_LFO1_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO1_STAT
	movff	PLUSW1, IRQ_TMP3
	movlw	MBFM_Vx_LFO1_VALUE
	movf	PLUSW1, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_Vol_Mod_NoLFO1

	;; handle LFO2
	movlw	MBFM_Ix_LFO2_DEPTH_V1
	addwf	MBFM_SE_OP, W, BANKED
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Vol_Mod_NoLFO2
#endif
MBFM_SE_Vol_Mod_LFO2
	movlw	MBFM_Ix_LFO2_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO2_STAT
	movff	PLUSW1, IRQ_TMP3
	movlw	MBFM_Vx_LFO2_VALUE
	movf	PLUSW1, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_Vol_Mod_NoLFO2

	;; handle EG5
	movlw	MBFM_Ix_EG5_DEPTH_V1
	addwf	MBFM_SE_OP, W, BANKED
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Vol_Mod_NoEG5
#endif
MBFM_SE_Vol_Mod_EG5
	movlw	MBFM_Vx_EG5_CTR_H
	movf	PLUSW1, W
	rcall	MBFM_SE_EG_Hlp_Waveform
MBFM_SE_Vol_Mod_NoEG5

	;; do nothing if no modulation
	movf	MBFM_SE_MOD_RESULT_H, W, BANKED
	iorwf	MBFM_SE_MOD_RESULT_U, W, BANKED
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_Vol_Mod_End
#endif
	comf	MBFM_SE_MOD_VALUE, W, BANKED	; invert volume and left-align
	rlf	WREG, W
	rlf	WREG, W
	iorlw	0x03

	;; add to volume
	addwf	MBFM_SE_MOD_RESULT_H, W, BANKED
	movwf	MBFM_SE_MOD_VALUE, BANKED
	movlw	0x00
	addwfc	MBFM_SE_MOD_RESULT_U, W, BANKED
	bz	MBFM_SE_Vol_Mod_SatCont

	;; saturate
MBFM_SE_Vol_Mod_Sat
	BRA_IFSET MBFM_SE_MOD_RESULT_U, 7, BANKED, MBFM_SE_Vol_Mod_SatNeg
MBFM_SE_Vol_Mod_SatPos
	setf	MBFM_SE_MOD_VALUE, BANKED
	rgoto	MBFM_SE_Vol_Mod_SatCont

MBFM_SE_Vol_Mod_SatNeg
	clrf	MBFM_SE_MOD_VALUE, BANKED
	;; 	rgoto	MBFM_SE_Vol_Mod_SatCont
	
MBFM_SE_Vol_Mod_SatCont

	comf	MBFM_SE_MOD_VALUE, W, BANKED	; invert volume and right-align
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x3f
	movwf	MBFM_SE_MOD_VALUE, BANKED

MBFM_SE_Vol_Mod_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_LFO
;;  DESCRIPTION: help function which handles the LFOs
;; --------------------------------------------------------------------------
MBFM_SE_LFO
	;; using a macro to optimize for speed
MBFM_SE_LFOx MACRO Vx_LFOx_STAT, Vx_LFOx_CTR, Vx_LFOx_VALUE, Ix_LFOx_MODE, Ix_LFOx_DELAY, Vx_LFOx_RATE, Ix_LFOx_DEPTH_P
	LOCAL	MBFM_SE_LFOx_Pitch
	LOCAL	MBFM_SE_LFOx_NoPitch
	LOCAL	MBFM_SE_LFOx_Inc
	LOCAL	MBFM_SE_LFOx_Dec
	LOCAL	MBFM_SE_LFOx_Cont
	LOCAL	MBFM_SE_LFOx_End

	;; don't process if LFO not enabled
	movlw	Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, 0, ACCESS, MBFM_SE_LFOx_End

	;; enable frequency sweep branch if pitch depth != 0x40
	movlw	Ix_LFOx_DEPTH_P
	movf	PLUSW0, W
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_LFOx_NoPitch
#endif
MBFM_SE_LFOx_Pitch
	;; enable frequency update
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED
MBFM_SE_LFOx_NoPitch

	;; increment step counter
	movlw	Vx_LFOx_CTR
	incf	PLUSW1, F
	movff	PLUSW1, IRQ_TMP1


	;; get CTR/ADD entry from LFO table depending on LFO rate
	movlw	Vx_LFOx_RATE
	movf	PLUSW1, W
	call	MBFM_LFO_TABLE_Get
	;; result: CTR value in WREG and MIOS_PARAMETER1, ADD value in MIOS_PARAMETER2

	;; exit if max step counter value (CTR) not reached
	movf	MIOS_PARAMETER1, W
	subwf	IRQ_TMP1, W
	bnc	MBFM_SE_LFOx_End

	;; else clear step counter
	movlw	Vx_LFOx_CTR
	clrf	PLUSW1

	;; copy Vx_LFOx_VALUE to IRQ_TMP1
	movlw	Vx_LFOx_VALUE
	movff	PLUSW1, IRQ_TMP1

	;; add or subtract ADD value to linear LFO value
	movlw	Vx_LFOx_STAT
	BRA_IFSET PLUSW1, MBFM_LFO_STAT_DEC, ACCESS, MBFM_SE_LFOx_Dec
MBFM_SE_LFOx_Inc
	movf	MIOS_PARAMETER2, W		; ADD value
	addwf	IRQ_TMP1, F			; add to linear LFO value
	bnc	MBFM_SE_LFOx_Cont		; skip next if max value (0xff) not reached
	comf	IRQ_TMP1, W			; subtract the missing ticks
	addwf	IRQ_TMP1, F
	movlw	Vx_LFOx_STAT
	bsf	PLUSW1, MBFM_LFO_STAT_DEC	; switch to decrement
	rgoto	MBFM_SE_LFOx_Cont

MBFM_SE_LFOx_Dec
	movf	MIOS_PARAMETER2, W		; ADD value
	subwf	IRQ_TMP1, F			; decrement from linear LFO value
	bc	MBFM_SE_LFOx_Cont		; skip next if min value (0x00) not reached
	comf	IRQ_TMP1, W			; add the missing ticks
	addlw	1
	addwf	IRQ_TMP1, F
	movlw	Vx_LFOx_STAT
	bcf	PLUSW1, MBFM_LFO_STAT_DEC	; switch to increment
	;; 	rgoto	MBFM_SE_LFOx_Cont

MBFM_SE_LFOx_Cont

	;; exit if LFO value has not been changed
	movlw	Vx_LFOx_VALUE
	movf	PLUSW1, W
	xorwf	IRQ_TMP1, W
	bz	MBFM_SE_LFOx_End

	;; else notify change
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED

	;; copy IRQ_TMP1 to Vx_LFOx_VALUE
	movlw	Vx_LFOx_VALUE
	movff	IRQ_TMP1, PLUSW1

MBFM_SE_LFOx_End
	ENDM


MBFM_SE_LFOx_MOD MACRO Vx_LFOx_STAT, Vx_LFOx_VALUE, Ix_LFOx_MODE, Ix_LFOx_DEPTH_L, Ix_LFOy_RATE, Vx_LFOy_RATE, Ix_EG5_DEPTH_L, Vx_EG5_CTR_H
	LOCAL	MBFM_SE_LFOx_MOD_End
	LOCAL	MBFM_SE_LFOx_MOD_LFOx
	LOCAL	MBFM_SE_LFOx_MOD_NoLFOx
	LOCAL	MBFM_SE_LFOx_MOD_EG5
	LOCAL	MBFM_SE_LFOx_MOD_NoEG5
	LOCAL	MBFM_SE_LFOx_MOD_Sat
	LOCAL	MBFM_SE_LFOx_MOD_SatPos
	LOCAL	MBFM_SE_LFOx_MOD_SatNeg
	LOCAL	MBFM_SE_LFOx_MOD_SatCont

	;; instrument rate -> MBFM_SE_MOD_VALUE (left aligned)
	clrc
	movlw	Ix_LFOy_RATE
	rlf	PLUSW0, W
	movwf	MBFM_SE_MOD_VALUE, BANKED

	;; signed value in MBFM_SE_MOD_RESULT_[LHU]
	clrf	MBFM_SE_MOD_RESULT_L, BANKED
	clrf	MBFM_SE_MOD_RESULT_H, BANKED
	clrf	MBFM_SE_MOD_RESULT_U, BANKED

	;; check LFOx depth
	movlw	Ix_LFOx_DEPTH_L
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_LFOx_MOD_NoLFOx
#endif
MBFM_SE_LFOx_MOD_LFOx
	movlw	Ix_LFOx_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	Vx_LFOx_STAT
	movff	PLUSW1, IRQ_TMP3
	movlw	Vx_LFOx_VALUE
	movf	PLUSW1, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_LFOx_MOD_NoLFOx

  IF Ix_EG5_DEPTH_L > 0
	;; check EG5 depth
	movlw	Ix_EG5_DEPTH_L
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_LFOx_MOD_NoEG5
#endif
MBFM_SE_LFOx_MOD_EG5
	movlw	Vx_EG5_CTR_H
	movf	PLUSW1, W
	rcall	MBFM_SE_EG_Hlp_Waveform
MBFM_SE_LFOx_MOD_NoEG5
  ENDIF

	;; add to rate
	movf	MBFM_SE_MOD_RESULT_H, W, BANKED
	addwf	MBFM_SE_MOD_VALUE, F, BANKED
	movlw	0x00
	addwfc	MBFM_SE_MOD_RESULT_U, W, BANKED
	bz	MBFM_SE_LFOx_MOD_SatCont

	;; saturate
MBFM_SE_LFOx_MOD_Sat
	BRA_IFSET MBFM_SE_MOD_RESULT_U, 7, BANKED, MBFM_SE_LFOx_MOD_SatNeg
MBFM_SE_LFOx_MOD_SatPos
	setf	MBFM_SE_MOD_VALUE, BANKED
	rgoto	MBFM_SE_LFOx_MOD_SatCont

MBFM_SE_LFOx_MOD_SatNeg
	clrf	MBFM_SE_MOD_VALUE, BANKED
	;; 	rgoto	MBFM_SE_LFOx_MOD_SatCont
	
MBFM_SE_LFOx_MOD_SatCont

MBFM_SE_LFOx_MOD_End
	clrc					; right-align mod value and copy to voice rate register
	rrf	MBFM_SE_MOD_VALUE, F, BANKED
	movlw	Vx_LFOy_RATE
	movff	MBFM_SE_MOD_VALUE, PLUSW1
	
	ENDM


	;; check for MIDI sync
	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW0, MBFM_CTRL2_SYNC_LFO, ACCESS, MBFM_SE_LFO_NoMSync
	btfss	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_2, BANKED
	return
MBFM_SE_LFO_NoMSync

	;; LFOx/EG5 -> LFOy modulation
	MBFM_SE_LFOx_MOD MBFM_Vx_LFO1_STAT, MBFM_Vx_LFO1_VALUE, MBFM_Ix_LFO1_MODE, MBFM_Ix_LFO1_DEPTH_L, MBFM_Ix_LFO2_RATE, MBFM_Vx_LFO2_RATE, 0, 0
	MBFM_SE_LFOx_MOD MBFM_Vx_LFO2_STAT, MBFM_Vx_LFO2_VALUE, MBFM_Ix_LFO2_MODE, MBFM_Ix_LFO2_DEPTH_L, MBFM_Ix_LFO1_RATE, MBFM_Vx_LFO1_RATE, MBFM_Ix_EG5_DEPTH_L, MBFM_Vx_EG5_CTR_H

	;; we have two LFOs
	MBFM_SE_LFOx MBFM_Vx_LFO1_STAT, MBFM_Vx_LFO1_CTR, MBFM_Vx_LFO1_VALUE, MBFM_Ix_LFO1_MODE, MBFM_Ix_LFO1_DELAY, MBFM_Vx_LFO1_RATE, MBFM_Ix_LFO1_DEPTH_P
	MBFM_SE_LFOx MBFM_Vx_LFO2_STAT, MBFM_Vx_LFO2_CTR, MBFM_Vx_LFO2_VALUE, MBFM_Ix_LFO2_MODE, MBFM_Ix_LFO2_DELAY, MBFM_Vx_LFO2_RATE, MBFM_Ix_LFO2_DEPTH_P


	return



;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_LFO_Sync
;;  DESCRIPTION: help function which syncs the LFOs to the gate
;; --------------------------------------------------------------------------
MBFM_SE_LFO_Sync
	;; sync LFO1 if enabled
	movlw	MBFM_Ix_LFO1_MODE
	BRA_IFCLR PLUSW0, 2, ACCESS, MBFM_SE_LFO_Sync_LFO1_Not
MBFM_SE_LFO_Sync_LFO1
	movlw	MBFM_Ix_LFO1_PHASE
	movff	PLUSW0, IRQ_TMP1

	movlw	MBFM_Vx_LFO1_STAT
	bcf	PLUSW1, MBFM_LFO_STAT_DEC
	btfsc	IRQ_TMP1, 6
	bsf	PLUSW1, MBFM_LFO_STAT_DEC

	rlf	IRQ_TMP1, W
	rlf	WREG, W
	andlw	0xfc
	movwf	IRQ_TMP2
	btfsc	IRQ_TMP1, 6
	comf	IRQ_TMP2, F

	movlw	MBFM_Vx_LFO1_VALUE
	movff	IRQ_TMP2, PLUSW1
	movlw	MBFM_Vx_LFO1_CTR
	clrf	PLUSW1
MBFM_SE_LFO_Sync_LFO1_Not

	;; sync LFO2 if enabled
	movlw	MBFM_Ix_LFO2_MODE
	BRA_IFCLR PLUSW0, 2, ACCESS, MBFM_SE_LFO_Sync_LFO2_Not
MBFM_SE_LFO_Sync_LFO2
	movlw	MBFM_Ix_LFO2_PHASE
	movff	PLUSW0, IRQ_TMP1

	movlw	MBFM_Vx_LFO2_STAT
	bcf	PLUSW1, MBFM_LFO_STAT_DEC
	btfsc	IRQ_TMP1, 6
	bsf	PLUSW1, MBFM_LFO_STAT_DEC

	rlf	IRQ_TMP1, W
	rlf	WREG, W
	andlw	0xfc
	movwf	IRQ_TMP2
	btfsc	IRQ_TMP1, 6
	comf	IRQ_TMP2, F

	movlw	MBFM_Vx_LFO2_VALUE
	movff	IRQ_TMP2, PLUSW1
	movlw	MBFM_Vx_LFO2_CTR
	clrf	PLUSW1
MBFM_SE_LFO_Sync_LFO2_Not

	return

;; --------------------------------------------------------------------------
;; Help Function which returns the absolute part of a 7-bit signed value
;; IN:  7-bit signed value in WREG
;; OUT:	absolute value (0x00-0x3f) in WREG
;; --------------------------------------------------------------------------
MBFM_SE_Hlp_Abs7
	movf	WREG, W
	skpnz
	addlw	1
	btfss	WREG, 6
	sublw 0x40
	andlw	0x3f
	return

;; --------------------------------------------------------------------------
;; Help Function for ENV Waveforms (resuses the LFO Waveform routine)
;; In:  Vx_EGx_VALUE in WREG
;;      Ix_EGx_DEPTH in IRQ_TMP2, 
;; Out: Result in PROD[LH]
;; --------------------------------------------------------------------------
MBFM_SE_EG_Hlp_Waveform
	;; set mode to 0x10, don't overwrite WREG
	clrf	IRQ_TMP1
	bsf	IRQ_TMP1, 4	; (triangle waveform)

	clrf	IRQ_TMP3	; (DEC flag always 0)

	;; convert linear envelope value
	rrf	WREG, W
	andlw	0x7f
	btfsc	IRQ_TMP2, 6; shift positive values to > 0x0000
	addlw 0x80	
	btfss	IRQ_TMP2, 6; inversion if depth < 0x40
	xorlw 0x7f	
	rgoto	MBFM_SE_EG_Hlp_Waveform_Cont

;; --------------------------------------------------------------------------
;; Help Function for LFO and ENV Waveforms
;; In:  Vx_LFO_VALUE in WREG
;;	Ix_LFOx_MODE in IRQ_TMP1
;;      Ix_LFOx_DEPTH in IRQ_TMP2, 
;;      Vx_LFOx_STAT in IRQ_TMP3 
;; Out: 24-bit Result will be added to MBFM_SE_MOD_RESULT_[LHU]
;; --------------------------------------------------------------------------
MBFM_SE_LFO_Hlp_Waveform
	;; invert if negative depth (<0x40)
	btfss	IRQ_TMP2, 6
	xorlw 0xff

	;; envelope waveform routine continues here
MBFM_SE_EG_Hlp_Waveform_Cont
	;; save LFO value in IRQ_TMP4
	movwf	IRQ_TMP4

	;; stretch depth
	movf	IRQ_TMP2, W		; convert 7-bit signed to 6-bit absolute
	skpnz
	addlw	1
	btfss	WREG, 6
	sublw 0x40
	andlw	0x3f
	movwf	IRQ_TMP2
	;; when > 0x10:	multiply by 2, when >= 0x20: multiply by 4
	BRA_IFSET IRQ_TMP2, 5, ACCESS, MBFM_SE_EG_Hlp_Waveform_Str4
	BRA_IFSET IRQ_TMP2, 4, ACCESS, MBFM_SE_EG_Hlp_Waveform_Str2
	rgoto	MBFM_SE_EG_Hlp_Waveform_Str0
MBFM_SE_EG_Hlp_Waveform_Str4
	clrc
	rlf	IRQ_TMP2, F
MBFM_SE_EG_Hlp_Waveform_Str2
	clrc
	rlf	IRQ_TMP2, F
MBFM_SE_EG_Hlp_Waveform_Str0

	;; branch depending on selected waveform
	BRA_IFSET IRQ_TMP1, 6, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_4567
MBFM_SE_LFO_Hlp_WFBranch_0123
	BRA_IFSET IRQ_TMP1, 5, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_23
MBFM_SE_LFO_Hlp_WFBranch_01
	BRA_IFSET IRQ_TMP1, 4, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_1
MBFM_SE_LFO_Hlp_WFBranch_0
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Sin
MBFM_SE_LFO_Hlp_WFBranch_1
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Tri
	
MBFM_SE_LFO_Hlp_WFBranch_23
	BRA_IFSET IRQ_TMP1, 4, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_3
MBFM_SE_LFO_Hlp_WFBranch_2
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Saw
MBFM_SE_LFO_Hlp_WFBranch_3
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Pul

MBFM_SE_LFO_Hlp_WFBranch_4567
	BRA_IFSET IRQ_TMP1, 5, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_67
MBFM_SE_LFO_Hlp_WFBranch_45
	BRA_IFSET IRQ_TMP1, 4, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_5
MBFM_SE_LFO_Hlp_WFBranch_4
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Ran
MBFM_SE_LFO_Hlp_WFBranch_5
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Sin

MBFM_SE_LFO_Hlp_WFBranch_67
	BRA_IFSET IRQ_TMP1, 4, ACCESS, MBFM_SE_LFO_Hlp_WFBranch_7
MBFM_SE_LFO_Hlp_WFBranch_6
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Sin
MBFM_SE_LFO_Hlp_WFBranch_7
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Sin

MBFM_SE_LFO_Hlp_WFBranch_Sin
	movf	IRQ_TMP4, W
	call	MBFM_SIN_TABLE_Get
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Cont

MBFM_SE_LFO_Hlp_WFBranch_Tri
	;; Triangle: return unsigned value
	movf	IRQ_TMP4, W
	btfss	IRQ_TMP4, 7
	xorlw	0x7f
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Cont

MBFM_SE_LFO_Hlp_WFBranch_Saw
	;; Sawtooth: x/2, MODE_DECINC is the eight bit
	rrf	IRQ_TMP4, W
	andlw	0x7f
	btfss	IRQ_TMP3, MBFM_LFO_STAT_DEC
	iorlw	0x80
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Cont
MBFM_SE_LFO_Hlp_WFBranch_Pul
	;; Pulse: 0x00 when Dec, 0xff when Inc, take inversion bit also into account
	movlw	0x00
	btfsc	IRQ_TMP3, MBFM_LFO_STAT_DEC
	movlw 0xff
	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Cont
MBFM_SE_LFO_Hlp_WFBranch_Ran
	;; just get pieces of the random number
	movf	MBFM_RANDOM_SEED_H, W, BANKED
	andlw	0x55
	movwf	IRQ_TMP1
	movf	MBFM_RANDOM_SEED_L, W, BANKED
	andlw	0xaa
	iorwf	IRQ_TMP1, W
	;; 	rgoto	MBFM_SE_LFO_Hlp_WFBranch_Cont

MBFM_SE_LFO_Hlp_WFBranch_Cont
	movwf	IRQ_TMP1

	;; process scaling (depth * scaled value)
	andlw	0x7f		; remove sign from value
	clrc
	rlf	WREG, W
	mulwf	IRQ_TMP2	; multiply with depth (in IRQ_TMP2)
	;; unsigned 16-bit result in PROD[LH]

	clrf	IRQ_TMP3	; the upper byte
	;; invert if DECINC flag not set
	BRA_IFSET IRQ_TMP1, 7, ACCESS, MBFM_SE_LFO_Hlp_Waveform_End
	comf	PRODL, F
	comf	PRODH, F
	setf	IRQ_TMP3
MBFM_SE_LFO_Hlp_Waveform_End

	;; add to 24-bit modulation result
	movf	PRODL, W
	addwf	MBFM_SE_MOD_RESULT_L, F, BANKED
	movf	PRODH, W
	addwfc	MBFM_SE_MOD_RESULT_H, F, BANKED
	movf	IRQ_TMP3, W
	addwfc	MBFM_SE_MOD_RESULT_U, F, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_EG
;;  DESCRIPTION: help function which handles the EG
;; --------------------------------------------------------------------------
MBFM_SE_EG
	;; check for MIDI sync
	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW0, MBFM_CTRL2_SYNC_EG, ACCESS, MBFM_SE_EG_NoMSync
	btfss	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_2, BANKED
	return
MBFM_SE_EG_NoMSync

	;; prepare call of MBFM_SE_EG_GetBendedValue
	movlw	MBFM_Vx_EG5_CTR_H
	movff	PLUSW1, IRQ_TMP1
	movlw	MBFM_Ix_EG5_CURVE
	movff	PLUSW0, IRQ_TMP2

	;; branch depending on EG state
	movlw	MBFM_Vx_EG5_STAT
	BRA_IFSET PLUSW1, MBFM_EG_STAT_RELEASE, ACCESS, MBFM_SE_EG_PRelease
	BRA_IFCLR PLUSW1, MBFM_EG_STAT_ATTACK, ACCESS, MBFM_SE_EG_End
	BRA_IFSET PLUSW1, MBFM_EG_STAT_SUSTAIN, ACCESS, MBFM_SE_EG_PSustain
	BRA_IFSET PLUSW1, MBFM_EG_STAT_DECAY2, ACCESS, MBFM_SE_EG_PDecay2
	BRA_IFSET PLUSW1, MBFM_EG_STAT_DECAY1, ACCESS, MBFM_SE_EG_PDecay1

MBFM_SE_EG_PAttack
	;; in attack phase we have to increase the EG counter
	;; attack rate in IRQ_TMP3
	movlw	MBFM_Ix_EG5_ATTACK
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	MBFM_Ix_EG5_MODE
	movf	PLUSW0, W
	andlw	1 << 0		; attack flag
	rcall	MBFM_SE_EG_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final attack level in IRQ_TMP1
	movlw	MBFM_Ix_EG5_ATTLVL
	movff	PLUSW0, IRQ_TMP1
	;; increment counter
	rcall	MBFM_SE_EG_IncrCtr
	;; we've reached the level once the zero bit set
	bnz	MBFM_SE_EG_Cont

	;; switch to decay1 phase
	movlw	MBFM_Vx_EG5_STAT
	bsf	PLUSW1, MBFM_EG_STAT_DECAY1
	rgoto	MBFM_SE_EG_Cont


MBFM_SE_EG_PDecay1
	;; in decay1 phase we have to increase/decrease the EG counter depending on the current level
	;; decay1 rate in IRQ_TMP3
	movlw	MBFM_Ix_EG5_DECAY1
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	MBFM_Ix_EG5_MODE
	movf	PLUSW0, W
	andlw	1 << 1		; decay1 flag
	rcall	MBFM_SE_EG_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final decay level in IRQ_TMP1
	movlw	MBFM_Ix_EG5_DECLVL
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	rcall	MBFM_SE_EG_IncrDecrCtr
	;; we've reached the level once the zero bit set
	bnz	MBFM_SE_EG_Cont

	;; switch to decay2 phase
	movlw	MBFM_Vx_EG5_STAT
	bsf	PLUSW1, MBFM_EG_STAT_DECAY2
	rgoto	MBFM_SE_EG_Cont


MBFM_SE_EG_PDecay2
	;; in decay2 phase we have to increase/decrease the EG counter depending on the current level
	;; decay2 rate in WREG
	movlw	MBFM_Ix_EG5_DECAY2
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	MBFM_Ix_EG5_MODE
	movf	PLUSW0, W
	andlw	1 << 2		; decay2 flag
	rcall	MBFM_SE_EG_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final sustain in IRQ_TMP1
	movlw	MBFM_Ix_EG5_SUSTAIN
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	rcall	MBFM_SE_EG_IncrDecrCtr
	;; we've reached the level once the zero bit set
	bnz	MBFM_SE_EG_Cont

MBFM_SE_EG_PSustain
	;; switch to sustain phase
	movlw	MBFM_Vx_EG5_STAT
	bsf	PLUSW1, MBFM_EG_STAT_SUSTAIN

	;; write sustain value into counter (again)
	movlw	MBFM_Ix_EG5_SUSTAIN
	rlf	PLUSW0, W
	andlw	0xfe
	movwf	MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	rgoto	MBFM_SE_EG_Cont

	
MBFM_SE_EG_PRelease
	;; in release phase we have to decrease the EG counter until it reaches 0
	;; release rate in WREG
	movlw	MBFM_Ix_EG5_RELEASE
	movff	PLUSW0, IRQ_TMP3
	;; get release rate depending on curve setting
	movlw	MBFM_Ix_EG5_MODE
	movf	PLUSW0, W
	andlw	1 << 3		; release flag
	rcall	MBFM_SE_EG_GetBendedValue
	;; result: low byte in WREG and MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final sustain in IRQ_TMP1
	clrf	IRQ_TMP1
	;; decr counter
	rcall	MBFM_SE_EG_DecrCtr

MBFM_SE_EG_Cont
	;; copy MIOS_PARAMETER[12] to MBFM_Vx_EG5_CTR_[LH]
	movlw	MBFM_Vx_EG5_CTR_L
	movff	MIOS_PARAMETER1, PLUSW1
	movlw	MBFM_Vx_EG5_CTR_H
	movff	MIOS_PARAMETER2, PLUSW1

	;; set the "value changed" flag
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_MOD_VALUE_CHANGED, BANKED

	;; enable frequency sweep branch if pitch depth != 0x40
	movlw	MBFM_Ix_EG5_DEPTH_P
	movf	PLUSW0, W
	xorlw	0x40
#if MBFM_SE_MEASURE_WORST_CASE < 1
	bz	MBFM_SE_EG_NoPitch
#endif
MBFM_SE_EG_Pitch
	;; enable frequency update
	bsf	MBFM_SE_STAT, MBFM_SE_STAT_FRQ_SWEEP_ACTIVE, BANKED
MBFM_SE_EG_NoPitch

MBFM_SE_EG_End
	return



;; --------------------------------------------------------------------------
;; Help Function for MBFM_SE_EG to increase or decrease the EG counter
;; IN:  incr value in MIOS_PARAMETER[12]
;;	target level in IRQ_TMP1
;; OUT:	zero flag set if max value reached
;; --------------------------------------------------------------------------
MBFM_SE_EG_IncrDecrCtr
	;; if current level > target level, branch to decr function
	movlw	MBFM_Vx_EG5_CTR_H
	rrf	PLUSW1, W
	andlw	0x7f
	cpfslt	IRQ_TMP1, ACCESS
	rgoto MBFM_SE_EG_IncrCtr
	rgoto	MBFM_SE_EG_DecrCtr


;; --------------------------------------------------------------------------
;; Help Function for MBFM_SE_EG to increase the EG counter
;; IN:  incr value in MIOS_PARAMETER[12]
;;	max level in IRQ_TMP1
;;      
;; OUT:	zero flag set if max value reached
;; --------------------------------------------------------------------------
MBFM_SE_EG_IncrCtr
	;; multiply level by 2 (it's a 16-bit value, but only the upper 7 contain the value)
	clrc
	rlf	IRQ_TMP1, F

	;; add to EG counter
	movlw	MBFM_Vx_EG5_CTR_L
	movf	PLUSW1, W
	addwf	MIOS_PARAMETER1, F
	movlw	MBFM_Vx_EG5_CTR_H
	movf	PLUSW1, W
	addwfc	MIOS_PARAMETER2, F
	bc	MBFM_SE_EG_IncrCtr_Max

	;; if value >= level: switch to next phase
	movf	MIOS_PARAMETER1, W
	sublw	0x00
	movf	MIOS_PARAMETER2, W
	subwfb	IRQ_TMP1, W
	bc	MBFM_SE_EG_IncrCtr_NoMax

MBFM_SE_EG_IncrCtr_Max
	;; write level to MIOS_PARAMETER[12]
	movff	IRQ_TMP1, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	andlw	0x00		; set zero flag and exit
	return

MBFM_SE_EG_IncrCtr_NoMax
	iorlw	0xff		; clear zero flag and exit
	return


;; --------------------------------------------------------------------------
;; Help Function for MBFM_SE_EG to decrease the EG counter
;; IN:  decr value in MIOS_PARAMETER[12]
;;	min level in IRQ_TMP1
;; OUT:	zero flag set if min value reached
;; --------------------------------------------------------------------------
MBFM_SE_EG_DecrCtr
	;; multiply level by 2 (it's a 16-bit value, but only the upper 7 contain the value)
	clrc
	rlf	IRQ_TMP1, F

	;; subtraction from EG counter
	movlw	MBFM_Vx_EG5_CTR_L
	movff	PLUSW1, IRQ_TMP2
	movlw	MBFM_Vx_EG5_CTR_H
	movff	PLUSW1, IRQ_TMP3

	movf	MIOS_PARAMETER1, W
	subwf	IRQ_TMP2, W
	movwf	MIOS_PARAMETER1
	movf	MIOS_PARAMETER2, W
	subwfb	IRQ_TMP3, W
	movwf	MIOS_PARAMETER2
	bnc	MBFM_SE_EG_IncrCtr_Min

	;; if value < level: switch to next phase
	movlw	0x00
	subwf	MIOS_PARAMETER1, W
	movf	IRQ_TMP1, W
	subwfb	MIOS_PARAMETER2, W
	bc	MBFM_SE_EG_IncrCtr_NoMin

MBFM_SE_EG_IncrCtr_Min
	;; write level to MIOS_PARAMETER[12]
	movff	IRQ_TMP1, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	andlw	0x00		; set zero flag and exit
	return

MBFM_SE_EG_IncrCtr_NoMin
	iorlw	0xff		; clear zero flag and exit
	return

;; --------------------------------------------------------------------------
;; Help Function for MBFM_SE_EG
;; IN:  MBFM_Vx_EGx_CTR_H in IRQ_TMP1
;;	MBFM_Ix_EGx_CURVE in IRQ_TMP2
;;	MBFM_Ix_EGx_ATTACK/MBFM_Ix_EGx_DECAY or MBFM_Ix_EGx_SUSTAIN in IRQ_TMP3
;;      WREG != 0: use curve, WREG == 0: don't use curve parameter
;; OUT:	value which should be added to - or subtracted from - MBFM_Vx_CTR_[LH]
;;      low-byte in WREG and MIOS_PARAMETER1; high-byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_SE_EG_GetBendedValue
 	bnz	MBFM_SE_EG_GetBendedValue_Curve

	;; curve not selected, get value from EG_TABLE
	movf	IRQ_TMP3, W
	goto	MBFM_EG_TABLE_Get

MBFM_SE_EG_GetBendedValue_Curve
	;; return ENV_x_DECAY when ENV_x_CURVE == 0x40
	movlw	0x40
	cpfseq	IRQ_TMP2, ACCESS
	rgoto MBFM_SE_EG_GetBendedValue_UD
	comf	IRQ_TMP3, W
	rgoto	MBFM_SE_EG_GetBendedValue_Cont

MBFM_SE_EG_GetBendedValue_UD
	;; feedback: calculate ABS7(CURVE) * ENV_x_CTR_H
	movf	IRQ_TMP2, W		; get absolute value of curve parameter
	rcall	MBFM_SE_Hlp_Abs7
	mulwf	IRQ_TMP1, ACCESS	; multiply with current counter value
	
	;; when CURVE parameter < 0x40: bend down, else up
	BRA_IFCLR IRQ_TMP2, 6, ACCESS, MBFM_SE_EG_GetBendedValue_Down
MBFM_SE_EG_GetBendedValue_Up
	comf	IRQ_TMP3, F
	bcf	IRQ_TMP3, 7
	movf	PRODH, W
	subwf	IRQ_TMP3, W
	btfsc	WREG, 7
	movlw 0x00
	rgoto	MBFM_SE_EG_GetBendedValue_Cont

MBFM_SE_EG_GetBendedValue_Down
	comf	IRQ_TMP3, W
	andlw	0x7f
	addwf	PRODH, W
	btfsc	WREG, 7
	movlw 0x7f
	;; 	rgoto	MBFM_SE_EG_GetBendedValue_Cont

MBFM_SE_EG_GetBendedValue_Cont
	andlw	0x7f
	goto	MBFM_CURVE_TABLE_Get


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_EG_Restart
;;  DESCRIPTION: help function which restarts the EG(s)
;; --------------------------------------------------------------------------
MBFM_SE_EG_Restart
	movlw	(1 << MBFM_EG_STAT_ATTACK)
	movwf	IRQ_TMP1
	movlw	MBFM_Vx_EG5_STAT
	movff	IRQ_TMP1, PLUSW1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_EG_Release
;;  DESCRIPTION: help function which switches the EG(s) to release phase
;; --------------------------------------------------------------------------
MBFM_SE_EG_Release
	movlw	MBFM_Vx_EG5_STAT
	bsf	PLUSW1, MBFM_EG_STAT_RELEASE
	return




;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_DrumFrqDec
;;  DESCRIPTION: help function which handles the drum frequencies
;; --------------------------------------------------------------------------
MBFM_SE_DrumFrqDec
	movf	MBFM_DR_BD_FRQL, W, BANKED
	iorwf	MBFM_DR_BD_FRQH, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoBD
	movf	MBFM_DR_BD_DECR, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoBD
MBFM_SE_DrumFrqDec_BD
	lfsr	FSR1, MBFM_DR_BD_FRQL
	rcall	MBFM_SE_DrumFrqDec_Hlp
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_BD_FNUM, BANKED
MBFM_SE_DrumFrqDec_NoBD

	movf	MBFM_DR_HH_FRQL, W, BANKED
	iorwf	MBFM_DR_HH_FRQH, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoHH
	movf	MBFM_DR_HH_DECR, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoHH
MBFM_SE_DrumFrqDec_HH
	lfsr	FSR1, MBFM_DR_HH_FRQL
	rcall	MBFM_SE_DrumFrqDec_Hlp
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_FNUM, BANKED
MBFM_SE_DrumFrqDec_NoHH

	movf	MBFM_DR_TOM_FRQL, W, BANKED
	iorwf	MBFM_DR_TOM_FRQH, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoTOM
	movf	MBFM_DR_TOM_DECR, W, BANKED
	bz	MBFM_SE_DrumFrqDec_NoTOM
MBFM_SE_DrumFrqDec_TOM
	lfsr	FSR1, MBFM_DR_TOM_FRQL
	rcall	MBFM_SE_DrumFrqDec_Hlp
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_TOM_FNUM, BANKED
MBFM_SE_DrumFrqDec_NoTOM

	return


	;; decrement FRQ[LH] (in FSR1), zero on underrun
MBFM_SE_DrumFrqDec_Hlp
	subwf	POSTINC1, F
	movlw	0x00
	subwfb	INDF1, F
	BRA_IFCLR INDF1, 7, ACCESS, MBFM_SE_DrumFrqDec_Hlp_End
	clrf	POSTDEC1
	clrf	INDF1
MBFM_SE_DrumFrqDec_Hlp_End
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Clk
;;  DESCRIPTION: help function which handles the MIDI clock
;; --------------------------------------------------------------------------
MBFM_SE_Clk
	incf	MBFM_MIDI_SYNC_CTR, F, BANKED

	;; register last counter value on 0xf8 or if MBFM_MIDI_SYNC_CTR == 0xff (no clock received)
	bcf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_1, BANKED
	incf	MBFM_MIDI_SYNC_CTR, W, BANKED
	bz	MBFM_SE_Clk_F8
	BRA_IFCLR MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_F8, BANKED, MBFM_SE_Clk_NoF8
 	bcf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_F8, BANKED
MBFM_SE_Clk_F8
	bsf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_1, BANKED
	movff	MBFM_MIDI_SYNC_CTR, MBFM_MIDI_SYNC_CTR_REG
	clrf	MBFM_MIDI_SYNC_CTR, BANKED
MBFM_SE_Clk_NoF8

	;; handle with double resolution of SE clock
	bcf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_2, BANKED

	;; if MIDI sync enabled: clock LFOs/ENVs two times on every MIDI clock event
	movf	MBFM_MIDI_SYNC_CTR, W, BANKED
	skpnz
	bsf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_2, BANKED

	clrc
	rrf	MBFM_MIDI_SYNC_CTR_REG, W, BANKED
	xorwf	MBFM_MIDI_SYNC_CTR, W, BANKED
	skpnz
	bsf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_2, BANKED

	;; A MIDI clock start event restarts all LFOs and reset all WTs
	BRA_IFCLR MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_FA, BANKED, MBFM_SE_Clk_NoFA
	bcf	MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_FA, BANKED
MBFM_SE_Clk_FA
	SET_BSR	MBFM_I1_BASE + MBFM_Ix_WT_STATE
	bsf	MBFM_I1_BASE + MBFM_Ix_WT_STATE, MBFM_WT_STATE_RESET, BANKED
	SET_BSR	MBFM_I2_BASE + MBFM_Ix_WT_STATE
	bsf	MBFM_I2_BASE + MBFM_Ix_WT_STATE, MBFM_WT_STATE_RESET, BANKED
	SET_BSR	MBFM_I3_BASE + MBFM_Ix_WT_STATE
	bsf	MBFM_I3_BASE + MBFM_Ix_WT_STATE, MBFM_WT_STATE_RESET, BANKED
	SET_BSR	MBFM_I4_BASE + MBFM_Ix_WT_STATE
	bsf	MBFM_I4_BASE + MBFM_Ix_WT_STATE, MBFM_WT_STATE_RESET, BANKED

	;; 	call	MBFM_SE_EG_SyncAllLFOs
MBFM_SE_Clk_NoFA
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_Wt
;;  DESCRIPTION: help function which handles the wavetables
;; --------------------------------------------------------------------------
MBFM_SE_Wt
	lfsr	FSR0, MBFM_I1_BASE
	clrf	IRQ_TMP5
MBFM_SE_Wt_Loop
	;; wavetable handler: check for MIDI Sync
	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW0, MBFM_CTRL2_SYNC_WT, ACCESS, MBFM_SE_Wt_Step
	BRA_IFCLR MBFM_MIDI_SYNC, MBFM_MIDI_SYNC_TICK_1, BANKED, MBFM_SE_Wt_Loop_Next
MBFM_SE_Wt_Step
	movlw	MBFM_Ix_WT_RATE
	movf	PLUSW0, W
	bz	MBFM_SE_Wt_Loop_Next

	movlw	MBFM_Ix_WT_CTR
	movf	PLUSW0, W
	bnz	MBFM_SE_Wt_Next

	movlw	MBFM_Ix_CTRL2
	BRA_IFCLR PLUSW0, MBFM_CTRL2_SYNC_WT, ACCESS, MBFM_SE_Wt_IntClk
MBFM_SE_Wt_ExtClk
	movlw	MBFM_Ix_WT_RATE
	comf	PLUSW0, W
	andlw	0x7f
	addlw	1
	rgoto	MBFM_SE_Wt_ExtClk_C

MBFM_SE_Wt_IntClk
	movlw	MBFM_Ix_WT_RATE		; copy inverted rate * 2 into CTR register
	comf	PLUSW0, W
	clrc
	rlf	WREG, W

MBFM_SE_Wt_ExtClk_C
	movwf	IRQ_TMP1
	movlw	MBFM_Ix_WT_CTR
	movff	IRQ_TMP1, PLUSW0
	;; 	rgoto	MBFM_SE_Wt_PlayNext

MBFM_SE_Wt_PlayNext
	movlw	MBFM_Ix_WT_CLK_REQ_CTR
	incf	PLUSW0, F
	rgoto	MBFM_SE_Wt_Loop_Next

MBFM_SE_Wt_Next
	movlw	MBFM_Ix_WT_CTR
	decf	PLUSW0, F

MBFM_SE_Wt_Loop_Next
	movlw	MBFM_Ix_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	IRQ_TMP5, F
	movlw	MBFM_Ix_NUM - 1
	cpfsgt	IRQ_TMP5, ACCESS
	rgoto MBFM_SE_Wt_Loop

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_SE_AOUT
;;  DESCRIPTION: help function which handles the AOUTs
;; --------------------------------------------------------------------------
MBFM_SE_AOUT
	clrf	MBFM_SE_VOICE_CTR, BANKED	; using VOICE_CTR as AOUT counter
MBFM_SE_AOUT_Loop
	;; pointer to assigned instrument -> FSR0
	lfsr	FSR0, MBFM_I1_BASE
	movlw	MBFM_AOUTx_ASSIGN
	swapf	PLUSW1, W
	andlw	0x03
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwf	FSR0H, F

	;; pointer to MBFM_AOUT_BASE -> FSR1
	lfsr	FSR1, MBFM_AOUT_BASE
	rlf	MBFM_SE_VOICE_CTR, W, BANKED
	andlw	0xfe
	addwf	FSR1L, F

	;; pointer to voice -> FSR2
	lfsr	FSR2, MBFM_V1_BASE
	movlw	MBFM_Ix_LAST_VOICE
	movf	PLUSW0, W
	mullw	MBFM_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwf	FSR2H, F

	
	;; signed value in MBFM_SE_MOD_RESULT_[LHU]
	clrf	MBFM_SE_MOD_RESULT_L, BANKED
	clrf	MBFM_SE_MOD_RESULT_H, BANKED
	clrf	MBFM_SE_MOD_RESULT_U, BANKED

	;; handle LFO1
	movlw	MBFM_AOUTx_ASSIGN
	BRA_IFCLR PLUSW1, 0, ACCESS, MBFM_SE_AOUT_Mod_NoLFO1
	movlw	MBFM_Ix_LFO1_DEPTH_A
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
	bz	MBFM_SE_AOUT_Mod_NoLFO1
MBFM_SE_AOUT_Mod_LFO1
	movlw	MBFM_Ix_LFO1_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO1_STAT
	movff	PLUSW2, IRQ_TMP3
	movlw	MBFM_Vx_LFO1_VALUE
	movf	PLUSW2, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_AOUT_Mod_NoLFO1

	;; handle LFO2
	movlw	MBFM_AOUTx_ASSIGN
	BRA_IFCLR PLUSW1, 1, ACCESS, MBFM_SE_AOUT_Mod_NoLFO2
	movlw	MBFM_Ix_LFO2_DEPTH_A
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
	bz	MBFM_SE_AOUT_Mod_NoLFO2
MBFM_SE_AOUT_Mod_LFO2
	movlw	MBFM_Ix_LFO2_MODE
	movff	PLUSW0, IRQ_TMP1
	movlw	MBFM_Vx_LFO2_STAT
	movff	PLUSW2, IRQ_TMP3
	movlw	MBFM_Vx_LFO2_VALUE
	movf	PLUSW2, W
	rcall	MBFM_SE_LFO_Hlp_Waveform
MBFM_SE_AOUT_Mod_NoLFO2

	;; handle EG5
	movlw	MBFM_AOUTx_ASSIGN
	BRA_IFCLR PLUSW1, 2, ACCESS, MBFM_SE_AOUT_Mod_NoEG5
	movlw	MBFM_Ix_EG5_DEPTH_A
	movf	PLUSW0, W
	movwf	IRQ_TMP2
	xorlw	0x40
	bz	MBFM_SE_AOUT_Mod_NoEG5
MBFM_SE_AOUT_Mod_EG5
	movlw	MBFM_Vx_EG5_CTR_H
	movf	PLUSW2, W
	rcall	MBFM_SE_EG_Hlp_Waveform
MBFM_SE_AOUT_Mod_NoEG5

	;; add modulation value to left-aligned offset
	movlw	MBFM_AOUTx_OFFSET
	rlf	PLUSW1, W
	andlw	0xfe
	addwf	MBFM_SE_MOD_RESULT_H, F, BANKED
	movlw	0x00
	addwfc	MBFM_SE_MOD_RESULT_U, F, BANKED
	bz	MBFM_SE_AOUT_Mod_SatCont

	;; saturate
MBFM_SE_AOUT_Mod_Sat
	BRA_IFSET MBFM_SE_MOD_RESULT_U, 7, BANKED, MBFM_SE_AOUT_Mod_SatNeg
MBFM_SE_AOUT_Mod_SatPos
	setf	MBFM_SE_MOD_RESULT_L, BANKED
	setf	MBFM_SE_MOD_RESULT_H, BANKED
	rgoto	MBFM_SE_AOUT_Mod_SatCont

MBFM_SE_AOUT_Mod_SatNeg
	clrf	MBFM_SE_MOD_RESULT_L, BANKED
	clrf	MBFM_SE_MOD_RESULT_H, BANKED
	;; 	rgoto	MBFM_SE_AOUT_Mod_SatCont
	
MBFM_SE_AOUT_Mod_SatCont

	;; transfer modulation result to AOUT channel
	movff	MBFM_SE_MOD_RESULT_L, MIOS_PARAMETER1
	movff	MBFM_SE_MOD_RESULT_H, MIOS_PARAMETER2
	movf	MBFM_SE_VOICE_CTR, W, BANKED
 	call	AOUT_Pin16bitSet
	SET_BSR	MBFM_BASE

MBFM_SE_AOUT_Loop_Next
	;; loop until last AOUT channel reached
	incf	MBFM_SE_VOICE_CTR, F, BANKED
	BRA_IFCLR MBFM_SE_VOICE_CTR, 3, BANKED, MBFM_SE_AOUT_Loop

	return
