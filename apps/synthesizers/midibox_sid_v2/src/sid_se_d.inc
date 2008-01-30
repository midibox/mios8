; $Id$
;
; MIDIbox SID
; Drum Engine
;  
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Idea for ENV Curve Parameter and OSC synchronization by Jess D. Skov-Nielsen
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Drum Engine handler called by SIDSE_Handler (sid_se.inc)
;; --------------------------------------------------------------------------
SIDSE_D_Handler
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; branch depending on cycle
	BRA_IFSET SID_STAT, SID_STAT_SE_CYCLE, ACCESS, SIDSE_D_Handler_Cycle2

SIDSE_D_Handler_Cycle1
	bsf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the second cycle

	;; clear "allocated" notification for external AOUTs - flags will be set if X2A function active
	clrf	SID_SE_EXT_ALLOCATED, BANKED

	;; clear modulation targets
	movlw	((SID_MOD_TARG_CLEAR_END-SID_MOD_TARG_CLEAR_BEGIN)+1) & 0xff
	movwf	IRQ_TMP1
	lfsr	FSR0, SID_MOD_TARG_CLEAR_BEGIN
SIDSE_D_Handler_ModClearLoop
	clrf	POSTINC0
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_D_Handler_ModClearLoop

	;; ------------------------------------------------------------------
	;; 1st Cycle: Clock Handler
	;; ------------------------------------------------------------------
	call	SIDSE_Clk

	;; ------------------------------------------------------------------
	;; 1st Cycle: Sequencer Handler
	;; ------------------------------------------------------------------
	lfsr	FSR2, SID_SEQ1_BASE
	rcall	SIDSE_D_SEQ

	;; clear all SEQ requests (don't touch WT requests)
	SET_BSR	SID_BASE	; just to ensure
	movlw	0x3f
	andwf	SID_SE_TRG_EVNT_U, F, BANKED


	;; ------------------------------------------------------------------
	;; 2nd Cycle: LED Matrix Update
	;; ------------------------------------------------------------------
	call	SIDSE_D_CS_LM


	;; ------------------------------------------------------------------
	;; clear FA/FB and FC synch request
	;; ------------------------------------------------------------------
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED


	rgoto	SIDSE_D_Handler_End


SIDSE_D_Handler_Cycle2
	bcf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the first cycle

	;; ------------------------------------------------------------------
	;; 2nd Cycle: check ENV/LFO/Note Sync requests
	;; ------------------------------------------------------------------
	call	SIDSE_D_Sync

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Wavetable Handler
	;; ------------------------------------------------------------------
SIDSE_D_HANDLER_WT_MACRO MACRO voice_base, wt_base
	lfsr	FSR1, voice_base
	lfsr	FSR2, wt_base
	rcall	SIDSE_D_WT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_D_HANDLER_WT_MACRO SIDL_V1_BASE, SID_WT1_BASE
	SIDSE_D_HANDLER_WT_MACRO SIDL_V2_BASE, SID_WT2_BASE
	SIDSE_D_HANDLER_WT_MACRO SIDL_V3_BASE, SID_WT3_BASE
	SIDSE_D_HANDLER_WT_MACRO SIDR_V1_BASE, SID_WT4_BASE
	SIDSE_D_HANDLER_WT_MACRO SIDR_V2_BASE, SID_WT5_BASE
	SIDSE_D_HANDLER_WT_MACRO SIDR_V3_BASE, SID_WT6_BASE

	;; clear all WT requests (don't touch SEQ requests)
	movlw	0xc0
	andwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Voices
	;; ------------------------------------------------------------------
SIDSE_D_HANDLER_NOTE_MACRO MACRO voice_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_D_Hlp_GetAssignedInsPtr
	rcall	SIDSE_D_Note
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_D_Handler_Note_L
	SIDSE_D_HANDLER_NOTE_MACRO SIDL_V1_BASE
	SIDSE_D_HANDLER_NOTE_MACRO SIDL_V2_BASE
	SIDSE_D_HANDLER_NOTE_MACRO SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_D_Handler_Note_NotR
SIDSE_D_Handler_Note_R
	SIDSE_D_HANDLER_NOTE_MACRO SIDR_V1_BASE
	SIDSE_D_HANDLER_NOTE_MACRO SIDR_V2_BASE
	SIDSE_D_HANDLER_NOTE_MACRO SIDR_V3_BASE
SIDSE_D_Handler_Note_NotR

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Filter
	;; ------------------------------------------------------------------

SIDSE_D_Handler_Filter_L
	clrf	SID_SE_ELEMENT_NUM, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_D_S1F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL1_L
	lfsr	FSR2, SIDL_BASE
	call	SIDSE_Filter

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_D_Handler_Filter_NotR
SIDSE_D_Handler_Filter_R
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_S2F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL2_L
	lfsr	FSR2, SIDR_BASE
	call	SIDSE_Filter
SIDSE_D_Handler_Filter_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: AOUTs (External/Extensions)
	;; ------------------------------------------------------------------
SIDSE_D_HANDLER_EXT_MACRO MACRO patch_base, aout_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, aout_base
	call	SIDSE_BDM_EXT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR1_L, AOUT0_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR2_L, AOUT1_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR3_L, AOUT2_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR4_L, AOUT3_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR5_L, AOUT4_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR6_L, AOUT5_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR7_L, AOUT6_L
	SIDSE_D_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR8_L, AOUT7_L


	;; ------------------------------------------------------------------
	;; 2nd Cycle: External Switches
	;; ------------------------------------------------------------------
	call	SIDSE_EXT_Switches

	
	;; ------------------------------------------------------------------
	;; call temporary routine which updates static SID registers
	;; ------------------------------------------------------------------

	call	SIDSE_D_UpdateStatRegs

SIDSE_D_Handler_End
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_D_GetAssignedInsPtr
;;  DESCRIPTION: returns pointer to SID_Ix_D_Iy_BASE depending on assigned
;;  MIDI voice
;;  IN: pointer to SIDx_Vx_BASE in FSR1
;;  OUT: pointer to patch in FSR0
;; --------------------------------------------------------------------------
SIDSE_D_Hlp_GetAssignedInsPtr
	movlw	SID_Vx_DRUM	; (1..16)
	decf	PLUSW1, W
	andlw	0x0f
	mullw	SID_Ix_D_I2_BASE - SID_Ix_D_I1_BASE
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return


;; --------------------------------------------------------------------------
;;  Updates static SID registers
;;  (temporary)
;; --------------------------------------------------------------------------
SIDSE_D_UpdateStatRegs
SIDSE_D_UPDATESTAT_MACRO	MACRO voice_base, sid_base
	LOCAL	SIDSE_D_UpdateStatRegs_NoWave
	LOCAL	SIDSE_D_UpdateStatRegs_NoAcc

	lfsr	FSR1, voice_base

	;; pointer to instrument -> FSR0
	rcall	SIDSE_D_Hlp_GetAssignedInsPtr

	;; wait until initial gate_delay has passed (hard-sync, for ABW feature)
	movlw	SID_Vx_SET_DELAY_CTR_L
	movf	PLUSW1, W
	bnz	SIDSE_D_UpdateStatRegs_NoWave
	movlw	SID_Vx_SET_DELAY_CTR_H
	movf	PLUSW1, W
	bnz	SIDSE_D_UpdateStatRegs_NoWave

	movlw	0x09
	andwf	sid_base + SIDx_V1_CTRL, F, BANKED
	movlw	SID_Vx_D_MODEL_WAVEFORM
	swapf	PLUSW1, W
	andlw	0xf6
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw 0x8f 
	iorwf	sid_base + SIDx_V1_CTRL, F, BANKED
SIDSE_D_UpdateStatRegs_NoWave

	movlw	SID_Ix_Dx_AD
	movff	PLUSW0, sid_base + SIDx_V1_ENV_AD	
	movlw	SID_Ix_Dx_SR
	movff	PLUSW0, sid_base + SIDx_V1_ENV_SR

	;; accent handling: force sustain to maximum if accent active
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_ACCENT, ACCESS, SIDSE_D_UpdateStatRegs_NoAcc
	movlw	0xf0
	iorwf	sid_base + SIDx_V1_ENV_SR, F, BANKED
SIDSE_D_UpdateStatRegs_NoAcc
	
	ENDM

	SET_BSR	SIDL_BASE

	SIDSE_D_UPDATESTAT_MACRO SIDL_V1_BASE, SIDL_BASE + SIDx_V1_FRQ_L
	SIDSE_D_UPDATESTAT_MACRO SIDL_V2_BASE, SIDL_BASE + SIDx_V2_FRQ_L
	SIDSE_D_UPDATESTAT_MACRO SIDL_V3_BASE, SIDL_BASE + SIDx_V3_FRQ_L

	;; in Mono mode: copy *ALL* SID registers (not only the static ones) over to SIDR
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_D_UpdateStatRegs_Mono
SIDSE_D_UpdateStatRegs_Stereo
	;; stereo mode:	most SIDs registers already updated, copy over the static ones
	SIDSE_D_UPDATESTAT_MACRO SIDR_V1_BASE, SIDR_BASE + SIDx_V1_FRQ_L
	SIDSE_D_UPDATESTAT_MACRO SIDR_V2_BASE, SIDR_BASE + SIDx_V2_FRQ_L
	SIDSE_D_UPDATESTAT_MACRO SIDR_V3_BASE, SIDR_BASE + SIDx_V3_FRQ_L
	rgoto	SIDSE_D_UpdateStatRegs_End


SIDSE_D_UpdateStatRegs_Mono
	;; mono mode: copy over *all* SID registers of SIDL to SIDR
	lfsr	FSR1, SIDL_BASE
	lfsr	FSR2, SIDR_BASE
#if ENABLE_SWINSID
	movlw	0x20
#else
	movlw	SIDx_MODE_VOL+1
#endif
	movwf	PRODL		; (loop counter)
SIDSE_D_UpdateStatRegs_Mono_Loop
	movff	POSTINC1, POSTINC2
	decfsz	PRODL, F
	rgoto	SIDSE_D_UpdateStatRegs_Mono_Loop

SIDSE_D_UpdateStatRegs_End
	SET_BSR	SID_BASE
	return

;; --------------------------------------------------------------------------
;; This function handles the notes (gate/pitch/PW)
;; IN: pointer to SID_Ix_D_Iy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_D_Note
	clrf	IRQ_TMP1	; drum gate handler expects disable bit (#4) in IRQ_TMP1
	call	SIDSE_Gate_Drums ; gate handler
	skpz			; returns ZERO flag cleared if pitch should not be changed
   	rcall	SIDSE_D_Pitch	; pitch handler

	;; control the delayed gate clear request
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SIDSE_D_Note_NoGate
SIDSE_D_Note_Gate
	;; temporary shift FSR1 to SID_Vx_CLR_DELAY_CTR_L for easier handling
	movlw	SID_Vx_CLR_DELAY_CTR_L
	addwf	FSR1L, F

	;; play note so long 16bit delay counter != 0
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	bz	SIDSE_D_Note_GateClr_NoDelay
SIDSE_D_Note_GateClr_Delay
	;; increment counter, set it to zero on overrun (no delay anymore)
	;; delay is a combination of model based gatelength and PAR1
	;; if model based gatelength == 0, skip this feature
	movlw	SID_Vx_D_MODEL_GATELENGTH-SID_Vx_CLR_DELAY_CTR_L
	movf	PLUSW1, W
	bz	SIDSE_D_Note_GateClr_NoDelay
	movwf	IRQ_TMP3	; incrementer is the same like used for envelopes

	movlw	0x80		; curve *must* be disabled!
	call	SIDSE_Hlp_ENV_GetBendedValue	; incrementer in MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	addwf	POSTINC1, F
	movf	MIOS_PARAMETER2, W
	addwfc	POSTDEC1, F
	bc	SIDSE_D_Note_GateClr_DelayOv
	rgoto	SIDSE_D_Note_GateClr_End

SIDSE_D_Note_GateClr_DelayOv
	;; overrun: clear counter to disable delay
	clrf	POSTINC1
	clrf	POSTDEC1
	;; request gate clear and deactivate voice active state (new note can be played)
	movlw	SID_Vx_STATE-SID_Vx_CLR_DELAY_CTR_L
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
SIDSE_D_Note_GateClr_NoDelay
SIDSE_D_Note_GateClr_End
	movlw	-SID_Vx_CLR_DELAY_CTR_L	; switch back to SID_Vx_CTR_L
	addwf	FSR1L, F
SIDSE_D_Note_NoGate

	;; call pulse width handler
	call	SIDSE_D_PW

	return

;; --------------------------------------------------------------------------
;; This function handles the Voice Pitch
;; Special (simplified) version for Drum Engine
;; IN: pointer to SID_Ix_e_Iy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_D_Pitch

	;; ------------------------------------------------------------------
	;; get base note
	;; ------------------------------------------------------------------

	;; get base note from Vx_D_MODEL_BASENOTE entry
	movlw	SID_Vx_D_MODEL_BASENOTE
	movff	PLUSW1, SID_SE_TRANSPOSED_NOTE

	;; ------------------------------------------------------------------
	;; Determine Target frequency depending on transposed note
	;; ------------------------------------------------------------------
	movf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to SID_SE_TARGET_FRQ_[LH]
	movff	TABLAT, SID_SE_TARGET_FRQ_L
	tblrd*+
	movff	TABLAT, SID_SE_TARGET_FRQ_H

	;; ------------------------------------------------------------------
	;; Increase/Decrease target frequency by pitchrange depending on
	;; Finetune value
	;; ------------------------------------------------------------------

	;; determine scale multipler (9 bit signed value) -> MUL_B_L
	clrf	MUL_B_L, BANKED
	clrf	MUL_B_H, BANKED

	movlw	SID_Ix_Dx_TUNE
	movf	PLUSW0, W
	movwf	IRQ_TMP1
	xorlw	0x80
	bz	SIDSE_D_Pitch_NoOffset
SIDSE_D_Pitch_Finetune
	clrc
	rlf	IRQ_TMP1, W
	addwf	MUL_B_L, F, BANKED
	movlw	0x00
	btfss	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_B_H, F, BANKED
SIDSE_D_Pitch_NoFinetune

	;; get f_in[target], save it in MIOS_PARAMETER[12]
	;; add pitchrange (24) depending on direction with saturation
	BRA_IFSET MUL_B_H, 7, BANKED, SIDSE_D_Pitch_Frq_Dec
SIDSE_D_Pitch_Frq_Inc
	movlw	24
	addwf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	SIDSE_D_Pitch_Frq_Inc_Cont
SIDSE_D_Pitch_Frq_Dec
	movlw	24
	subwf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	btfsc	WREG, 7
	movlw 0x00
SIDSE_D_Pitch_Frq_Inc_Cont

	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MIOS_PARAMETER[12]
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2

	;; add and multiply to target frequency
SIDSE_D_Pitch_Frq_AddMul
	BRA_IFSET MUL_B_H, 7, BANKED, SIDSE_D_Pitch_Frq_AddMul_Neg
SIDSE_D_Pitch_Frq_AddMul_Pos
	;; calc MUL_A_[LH] = MIOS_PARAMETER[12] - SID_Vx_FRQ_[LH]
	movf	SID_SE_TARGET_FRQ_L, W, BANKED
	subwf	MIOS_PARAMETER1, W
	movwf	MUL_A_L, BANKED
	movf	SID_SE_TARGET_FRQ_H, W, BANKED
	subwfb	MIOS_PARAMETER2, W
	movwf	MUL_A_H, BANKED

	;; ensure that bit #15 (sign) not set
	BRA_IFCLR MUL_A_H, 7, BANKED, SIDSE_D_Pitch_Frq_AddMul_Cont
	setf	MUL_A_L, BANKED
	movlw	0x7f
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_D_Pitch_Frq_AddMul_Cont

SIDSE_D_Pitch_Frq_AddMul_Neg
	;; calc MUL_A_[LH] = SID_Vx_FRQ_[LH] - MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	subwf	SID_SE_TARGET_FRQ_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	MIOS_PARAMETER2, W
	subwfb	SID_SE_TARGET_FRQ_H, W, BANKED
	movwf	MUL_A_H, BANKED
	;; 	rgoto	SIDSE_D_Pitch_Frq_AddMul_Cont

SIDSE_D_Pitch_Frq_AddMul_Cont
	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16_SIGNED

	;; SID_Vx_FRQ += signed result[23:8]
	movf	MUL_R_1, W, BANKED
	addwf	SID_SE_TARGET_FRQ_L, F, BANKED
	movf	MUL_R_2, W, BANKED
	addwfc	SID_SE_TARGET_FRQ_H, F, BANKED

SIDSE_D_Pitch_NoOffset

	;; ------------------------------------------------------------------
	;; store target frequency in Vx
	;; ------------------------------------------------------------------
	movlw	SID_Vx_TARGET_FRQ_L
	movff	SID_SE_TARGET_FRQ_L, PLUSW1
	movlw	SID_Vx_TARGET_FRQ_H
	movff	SID_SE_TARGET_FRQ_H, PLUSW1


	;; ------------------------------------------------------------------
	;; pitch modulation
	;; ------------------------------------------------------------------
	call	SIDSE_Hlp_Pitch_Mod

	;; ------------------------------------------------------------------
	;; Copy Target frequency into SID Frequency registers
	;; ------------------------------------------------------------------

	;; transfer pointer to SIDx_Vx_FRQ_L register -> FSR2
	call	SIDSE_Hlp_GetSIDFrqPtr

	;; copy target frequency to SIDx_FRQ_[LH] and finish portamento
	movff	SID_SE_TARGET_FRQ_L, POSTINC2
	movff	SID_SE_TARGET_FRQ_H, POSTDEC2

	return


;; --------------------------------------------------------------------------
;; This function handles the Pulsewidth of a voice
;; Special (simplified) version for Drum Engine
;; IN: pointer to SID_Ix_D_Iy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_D_PW
	;; drum specific PW function expects 16bit pulsewidth in IRQ_TMP[12]
	clrf	IRQ_TMP1
	movlw	SID_Vx_D_MODEL_PULSEWIDTH
	movff	PLUSW1, IRQ_TMP2

	goto	SIDSE_PW_Drums

;; --------------------------------------------------------------------------
;; This function syncs the LFOs/ENVs and Notes
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_D_Sync
#if 0
SIDSE_D_SYNC_ENV_MACRO	MACRO	evnt_a, evnt_a_flag, evnt_r, evnt_r_flag, env_base
	lfsr	FSR1, env_base
	CALL_IFSET evnt_r, evnt_r_flag, BANKED, SIDSE_D_ENV_TrgRelease
	bcf	evnt_r, evnt_r_flag, BANKED
	CALL_IFSET evnt_a, evnt_a_flag, BANKED, SIDSE_D_ENV_Restart
	bcf	evnt_a, evnt_a_flag, BANKED
	ENDM

	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 0, SID_SE_TRG_EVNT_ENVR, 0, SID_ENV1_BASE
	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 1, SID_SE_TRG_EVNT_ENVR, 1, SID_ENV2_BASE
	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 2, SID_SE_TRG_EVNT_ENVR, 2, SID_ENV3_BASE
	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 3, SID_SE_TRG_EVNT_ENVR, 3, SID_ENV4_BASE
	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 4, SID_SE_TRG_EVNT_ENVR, 4, SID_ENV5_BASE
	SIDSE_D_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 5, SID_SE_TRG_EVNT_ENVR, 5, SID_ENV6_BASE


SIDSE_D_SYNC_LFO_MACRO	MACRO	evnt, evnt_flag, voice_base, lfo1_base, lfo2_base
	LOCAL	SIDSE_D_SYNC_LFO_Skip
	LOCAL	SIDSE_D_SYNC_LFO1_Skip
	LOCAL	SIDSE_D_SYNC_LFO2_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_D_SYNC_LFO_Skip
	bcf	evnt, evnt_flag, BANKED

	lfsr	FSR1, voice_base
	rcall	SIDSE_D_Hlp_GetAssignedInsPtr

	movlw	SID_Ix_M_Vx_LFO1_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_D_SYNC_LFO1_Skip
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	lfsr	FSR1, lfo1_base
	call	SIDSE_D_LFO_Restart
SIDSE_D_SYNC_LFO1_Skip

	lfsr	FSR1, voice_base
	rcall	SIDSE_D_Hlp_GetAssignedInsPtr

	movlw	SID_Ix_M_Vx_LFO2_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_D_SYNC_LFO2_Skip
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	lfsr	FSR1, lfo2_base
	call	SIDSE_D_LFO_Restart
SIDSE_D_SYNC_LFO2_Skip

SIDSE_D_SYNC_LFO_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_D_SYNC_LFO_NotL
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L1, SIDL_V1_BASE, SID_LFO1_BASE, SID_LFO2_BASE
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L2, SIDL_V2_BASE, SID_LFO3_BASE, SID_LFO4_BASE
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L3, SIDL_V3_BASE, SID_LFO5_BASE, SID_LFO6_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_D_SYNC_LFO_NotR
SIDSE_D_SYNC_LFO_R
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L4, SIDR_V1_BASE, SID_LFO7_BASE, SID_LFO8_BASE
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L5, SIDR_V2_BASE, SID_LFO9_BASE, SID_LFO10_BASE
	SIDSE_D_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L6, SIDR_V3_BASE, SID_LFO11_BASE, SID_LFO12_BASE
SIDSE_D_SYNC_LFO_NotR
#endif

SIDSE_D_SYNC_NOTE_MACRO	MACRO	evnt, evnt_flag, voice_base
	LOCAL	SIDSE_D_SYNC_NOTE_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_D_SYNC_NOTE_Skip
	lfsr	FSR1, voice_base
	rcall	SIDSE_D_Hlp_GetAssignedInsPtr
	call	SIDSE_D_NOTE_Restart
	bcf	evnt, evnt_flag, BANKED
SIDSE_D_SYNC_NOTE_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_D_SYNC_Note_L
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1L, SIDL_V1_BASE
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2L, SIDL_V2_BASE
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3L, SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_D_SYNC_Note_NotR
SIDSE_D_SYNC_Note_R
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1R, SIDR_V1_BASE
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2R, SIDR_V2_BASE
	SIDSE_D_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3R, SIDR_V3_BASE
SIDSE_D_SYNC_Note_NotR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_D_Note_Restart
;;  DESCRIPTION: help function which restarts the delay counter for voices
;;               and requests an active gate
;;  IN: patch base in FSR0, voice base in FSR1, osc number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_D_NOTE_Restart
	;; request gate if voice is active (and request clear for proper ADSR handling)
	;; set gate only if voice is active!
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	btfsc	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; no voice delay by default
	clrf	PRODL
	clrf	PRODH

	;; delay if ABW (ADSR bug workaround) option active
	;; this feature works different for drum engine: test flag is set and waveform/frequency are cleared instead
	;; of clearing the ADSR registers - this approach is called "hard-sync"
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFCLR WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_D_NOTE_Restart_NoABW
SIDSE_D_NOTE_Restart_ABW
	movlw	0x01
	movwf	PRODL

	;; clear waveform register and set test flag
	call	SIDSE_Hlp_GetSIDFrqPtr

	movlw	0x09
	movwf	TABLAT
	movlw	SIDx_V1_CTRL
	movff	TABLAT, PLUSW2

	;; request oscillator reset
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_PHASE_SYNC_REQ, F, BANKED
SIDSE_D_NOTE_Restart_NoABW

	movlw	SID_Vx_SET_DELAY_CTR_L
	movff	PRODL, PLUSW1
	movlw	SID_Vx_SET_DELAY_CTR_H
	movff	PRODH, PLUSW1

	;; initialize CLR delay counter (could depend on drum type later)
	movlw	0x01
	movwf	PRODL
	movlw	SID_Vx_CLR_DELAY_CTR_L
	movff	PRODL, PLUSW1
	movlw	SID_Vx_CLR_DELAY_CTR_H
	movff	PRODH, PLUSW1
	return


;; --------------------------------------------------------------------------
;; This function handles the wavetables
;; IN: pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     pointer to SID_WTx_BASE in FSR2 (wt record)
;;     WT number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_D_WT
	;; clear temporary WT flags
	bcf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED

	;; check if WT reset requested
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_TRG_EVNT_U, W, BANKED
	bz	SIDSE_D_WT_NoDivReset
SIDSE_D_WT_DivReset
	;; next clock event will increment div to 0
	movlw	SID_WTx_DIV_CTR
	setf	PLUSW2
	;; next step will increment to start position
	setf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_D_WT_NoDivReset

	;; check for clock sync event
#if 0
	BRA_IFCLR SID_SE_TRG_EVNT_U, 7, BANKED, SIDSE_D_WT_NoClk
#else
	;; clock with each update cycle, so that we are independent from the selected BPM rate
#endif
SIDSE_D_WT_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value
	movlw	SID_WTx_DIV_CTR
	incf	PLUSW2, W
	movwf	PRODL

	;; reset once max value is reached
	movlw	SID_Vx_D_MODEL_WT_SPEED
	movf	PLUSW1, W
	andlw	0x7f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_D_WT_Clk_NoOv
SIDSE_D_WT_Clk_Ov
	clrf	PRODL
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_D_WT_Clk_NoOv
	;; transfer new divider value into WTx register
	movlw	SID_WTx_DIV_CTR
	movff	PRODL, PLUSW2
SIDSE_D_WT_NoClk

	;; skip if no new step requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED, SIDSE_D_WT_End

	;; skip if position is 0xaa (notifies oneshot -> WT stopped)
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	xorlw	0xaa
	bz	SIDSE_D_WT_End

	;; wait until initial gate delay has passed (for ABW feature)
	movlw	SID_Vx_SET_DELAY_CTR_L
	movf	PLUSW1, W
	bnz	SIDSE_D_WT_End
	movlw	SID_Vx_SET_DELAY_CTR_H
	movf	PLUSW1, W
	bnz	SIDSE_D_WT_End

SIDSE_D_WT_NextStep
	;; increment position counter, reset at end position
	movlw	SID_WTx_POS
	incf	PLUSW2, W
	movwf	PRODL
	movlw	SID_Vx_D_MODEL_WT_END
	movf	PLUSW1, W
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_D_WT_NextStep_NoOv
SIDSE_D_WT_NextStep_Ov
	;; if one-shot mode: set position to 0xaa, WT is stopped now!
	movlw	SID_Vx_D_MODEL_WT_LOOP
	BRA_IFCLR PLUSW1, 7, ACCESS, SIDSE_D_WT_NextStep_Ov_NoOneShot
SIDSE_D_WT_NextStep_Ov_OneShot
	movlw	0xaa
	movwf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
	rgoto	SIDSE_D_WT_End

SIDSE_D_WT_NextStep_Ov_NoOneShot
	movlw	SID_Vx_D_MODEL_WT_LOOP
	movf	PLUSW1, W
	andlw	0x7f
	movwf	PRODL
SIDSE_D_WT_NextStep_NoOv
	;; transfer back to position counter
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2

	;; "play" the step
SIDSE_D_WT_PlayStep
	;; get pointer to WT value
	movlw	SID_Vx_D_MODEL_TBLPTRL
	movff	PLUSW1, TBLPTRL
	movlw	SID_Vx_D_MODEL_TBLPTRH
	movff	PLUSW1, TBLPTRH

	movlw	SID_WTx_POS
	movf	PLUSW2, W
	clrc
	rlf	WREG, W
	addlw	12		; (WT offset in model table)
	addwf	TBLPTRL, F
	movlw	0x00
	addwfc	TBLPTRH, F
	movlw	UPPER(SID_DMODEL_TABLE)	; (in sid_dmodel.inc it is ensured, that tables are within 64k block)
	movwf	TBLPTRU

	;; transfer new note to SID_Vx_D_MODEL_BASENOTE
	tblrd*+

	;; if bit #7 set: add PAR3/2 and saturate
	BRA_IFCLR TABLAT, 7, ACCESS, SIDSE_D_WT_PlayStep_NoPar3
SIDSE_D_WT_PlayStep_Par3
	bcf	TABLAT, 7	; ensure that this bit is cleared when transfered to note register

	movlw	SID_Vx_D_MODEL_PAR3
	rrf	PLUSW1, W	; (patch pointer in FSR2 here.. normaly in FSR0)
	andlw	0x7f
	addlw	-0x40
	movwf	PRODL

	addwf	TABLAT, W
	BRA_IFSET PRODL, 7, ACCESS, SIDSE_D_WT_PlayStep_NSatN
SIDSE_D_WT_PlayStep_NSatP
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	SIDSE_D_WT_PlayStep_NSat_Cont
SIDSE_D_WT_PlayStep_NSatN
	btfsc	WREG, 7
	movlw 0x00
	;; 	rgoto	SIDSE_D_WT_PlayStep_NSat_Cont
SIDSE_D_WT_PlayStep_NSat_Cont
	movwf	TABLAT

SIDSE_D_WT_PlayStep_NoPar3
	movlw	SID_Vx_D_MODEL_BASENOTE
	movff	TABLAT, PLUSW1

	;; transfer new waveform to SID_Vx_D_MODEL_WAVEFORM
	tblrd*+
	movlw	SID_Vx_D_MODEL_WAVEFORM
	movff	TABLAT, PLUSW1

SIDSE_D_WT_End
	return


;; --------------------------------------------------------------------------
;; This function handles the drum sequences
;; IN: pointer to SID_SEQx_BASE in FSR2 (seq record)
;; --------------------------------------------------------------------------
SIDSE_D_SEQ
	;; clear all gates and deselect sequence if MIDI clock stop requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED, SIDSE_D_SEQ_NoClkStop
SIDSE_D_SEQ_ClkStop
	;; stop sequencer
	movlw	SID_SEQx_MISC
	bcf	PLUSW2, SID_SEQ_MISC_SEQ_RUNNING
	;; clear gates
	rgoto	SIDSE_D_SEQ_NextStep_GateClr
SIDSE_D_SEQ_NoClkStop

	;; exit if not in sequencer mode
	;; clear gate if sequencer just has been disabled (for proper transitions)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	andlw	(1 << SID_I_V_SEQ_ON)
	bnz	SIDSE_D_SEQ_On
SIDSE_D_SEQ_Off
	;; turn off sequencer
	movlw	SID_SEQx_MISC
	btfss	PLUSW2, SID_SEQ_MISC_SEQ_ON
	return
	bcf	PLUSW2, SID_SEQ_MISC_SEQ_ON

	;; clear gate and exit
	rgoto	SIDSE_D_SEQ_NextStep_GateClr

SIDSE_D_SEQ_On

	;; clear temporary WT flags
	bcf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED

	;; check if WT reset requested or FA event or sequencer was disabled before (transition Seq off->on)
	movlw	SID_SEQx_MISC
	BRA_IFCLR PLUSW2, SID_SEQ_MISC_SEQ_ON, ACCESS, SIDSE_D_SEQ_DivReset

	;; check for FA event
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_D_SEQ_DivReset

	BRA_IFCLR SID_SE_TRG_EVNT_U, 6, BANKED, SIDSE_D_SEQ_NoDivReset
SIDSE_D_SEQ_DivReset
	;; next clock event will increment div to 0
	movlw	SID_SEQx_DIV_CTR
	setf	PLUSW2
	movlw	SID_SEQx_MISC	; bitfield [2:0] used for subcounter
	movf	PLUSW2, W
	iorlw	0x07
	movwf	PRODL
	movlw	SID_SEQx_MISC
	movff	PRODL, PLUSW2

	;; next step will increment to start position
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, WREG
	swapf	WREG, W
	andlw	0x70
	addlw	-1
	movwf	PRODL
	movlw	SID_SEQx_POS
	movff	PRODL, PLUSW2
SIDSE_D_SEQ_NoDivReset

	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED, SIDSE_D_SEQ_ClkCont
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_D_SEQ_NoClkStart
SIDSE_D_SEQ_ClkCont
SIDSE_D_SEQ_ClkStart
	;; start sequencer
	movlw	SID_SEQx_MISC
	bsf	PLUSW2, SID_SEQ_MISC_SEQ_RUNNING
SIDSE_D_SEQ_NoClkStart

	;; memorize sequencer on flag
	movlw	SID_SEQx_MISC
	bsf	PLUSW2, SID_SEQ_MISC_SEQ_ON

	;; check for clock sync event
	BRA_IFCLR SID_SE_TRG_EVNT_U, 7, BANKED, SIDSE_D_SEQ_NoClk
SIDSE_D_SEQ_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value
	movlw	SID_SEQx_DIV_CTR
	incf	PLUSW2, W
	movwf	PRODL
	bz	SIDSE_D_SEQ_Clk_Ov

	;; reset once max value is reached
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	andlw	0x3f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_D_SEQ_Clk_NoOv
SIDSE_D_SEQ_Clk_Ov
	clrf	PRODL
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_D_SEQ_Clk_NoOv
	;; transfer new divider value into SEQx register
	movlw	SID_SEQx_DIV_CTR
	movff	PRODL, PLUSW2
SIDSE_D_SEQ_NoClk

	;; skip if no new step requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED, SIDSE_D_SEQ_End

SIDSE_D_SEQ_NextStep
	;; increment subctr
	movlw	SID_SEQx_MISC
	incf	PLUSW2, W
	andlw	0x07	; (bit [2:0])
	movwf	PRODL

	movlw	SID_SEQx_MISC
	movf	PLUSW2, W
	andlw	0xf8
	iorwf	PRODL, W
	movwf	PRODH
	movlw	SID_SEQx_MISC
	movff	PRODH, PLUSW2

	;; if 0: new note & gate set
	;; if 4: gate clear
	;; if >=6: reset to 1, new note & gate set
	movf	PRODL, W
	bz	SIDSE_D_SEQ_NextStep_GateSet
	xorlw	4
	skpnz
	rgoto	SIDSE_D_SEQ_NextStep_GateClr
	movlw	6-1
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_D_SEQ_NextStep_End

SIDSE_D_SEQ_NextStep_GateSet
	movlw	SID_SEQx_MISC ; (if PRODL was >= 6 before...)
	movf	PLUSW2, W
	andlw	0xf8		; counter located at [2:0]
	movwf	PRODL
	movlw	SID_SEQx_MISC
	movff	PRODL, PLUSW2

	;; increment position counter, reset at end position
	movlw	SID_SEQx_POS
	incf	PLUSW2, W
	andlw	0x0f
	movwf	PRODL

	;; change to new sequence number immediately if SYNC16 flag not set or first step reached
	bz	SIDSE_D_SEQ_NextStep_NoSync16
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	BRA_IFCLR WREG, SID_I_V_SEQ_SYNC16, ACCESS, SIDSE_D_SEQ_NextStep_NoSync16
SIDSE_D_SEQ_NextStep_Sync16
	movlw	SID_SEQx_POS
	movf	PLUSW2, W
	rgoto	SIDSE_D_SEQ_NextStep_Sync16_Cont
SIDSE_D_SEQ_NextStep_NoSync16
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, WREG
	swapf	WREG, W
SIDSE_D_SEQ_NextStep_Sync16_Cont
	andlw	0x70
	iorwf	PRODL, F


	movf	PRODL, W
	andlw	0x70
	movwf	PRODH
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_LENGTH, WREG
	andlw	0x0f
	iorwf	PRODH, W
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_D_SEQ_NextStep_NoOv
SIDSE_D_SEQ_NextStep_Ov
	movlw	0x70
	andwf	PRODH, W
	movwf	PRODL
SIDSE_D_SEQ_NextStep_NoOv
	;; transfer back to position counter
	movlw	SID_SEQx_POS
	movff	PRODL, PLUSW2

	;; "play" the step
SIDSE_D_SEQ_PlayStep

	;; gate off if sequencer not running
	movlw	SID_SEQx_MISC
	BRA_IFCLR PLUSW2, SID_SEQ_MISC_SEQ_RUNNING, ACCESS, SIDSE_D_SEQ_PlayStep_Stop

	;; gate off if invalid song number (stop feature - seq >= 8)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, WREG
	andlw	0xf8
	bz	SIDSE_D_SEQ_PlayStep_NoStop

SIDSE_D_SEQ_PlayStep_Stop
	rgoto	SIDSE_D_SEQ_NextStep_GateClrForc;ed

SIDSE_D_SEQ_PlayStep_NoStop

	;; loop through all 8 tracks
	movlw	1		; using IRQ_TMP1 as loop counter
	movwf	IRQ_TMP1
SIDSE_D_SEQ_SetStep_Loop
	;; select initial drum instrument
	movff	IRQ_TMP1, SID_CURRENT_MIDIVOICE

	;; calculate pointer to gate/accent and secondary flag
	SET_BSR	SID_SEQ1_BASE	; dirty code (only SEQ1 can be handled), but allows us to work independent from FSR2

	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQUENCES
	rlf	SID_SEQ1_BASE + SID_SEQx_POS, W	; song offset
	andlw	0xe0
	addwf	FSR0L, F
	decf	IRQ_TMP1, W	; track offset
	clrc			; *4 (note: IRQ_TMP1 is already multiplied by 2, so that only one left shift is required)
	rlf	WREG, W
	addwf	FSR0L, F
	movlw	0		; step offset (upper)
	btfsc	SID_SEQ1_BASE + SID_SEQx_POS, 3, BANKED
	movlw	2
	addwf	FSR0L, F

	movf	SID_SEQ1_BASE + SID_SEQx_POS, W, BANKED	; step mask -> PRODL
	call	MIOS_HLP_GetBitORMask
	movwf	PRODL

	SET_BSR	SID_BASE

	;; coding:
	;; Gate  Accent  Result
	;;    0       0  Don't play note
	;;    1       0  Play Note w/o accent
	;;    0       1  Play Note w/ accent
	;;    1       1  Play Secondary Note
	andwf	POSTINC0, W
	bnz	SIDSE_D_SEQ_SetStep_Loop_G1
SIDSE_D_SEQ_SetStep_Loop_G0
	movf	POSTDEC0, W
	andwf	PRODL, W
	bnz	SIDSE_D_SEQ_SetStep_Loop_G0A1
SIDSE_D_SEQ_SetStep_Loop_G0A0
	rgoto	SIDSE_D_SEQ_SetStep_LoopNext	; skip note
SIDSE_D_SEQ_SetStep_Loop_G0A1
	;; play note with accent set
	setf	MIOS_PARAMETER2
	rgoto	SIDSE_D_SEQ_SetStep_Loop_Cont

SIDSE_D_SEQ_SetStep_Loop_G1
	movf	POSTDEC0, W
	andwf	PRODL, W
	bnz	SIDSE_D_SEQ_SetStep_Loop_G1A1
SIDSE_D_SEQ_SetStep_Loop_G1A0
	;; play note with accent cleared
	clrf	MIOS_PARAMETER2
	rgoto	SIDSE_D_SEQ_SetStep_Loop_Cont

SIDSE_D_SEQ_SetStep_Loop_G1A1
	;; play secondary note with accent cleared
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	clrf	MIOS_PARAMETER2
	;; 	rgoto	SIDSE_D_SEQ_SetStep_Loop_Cont

SIDSE_D_SEQ_SetStep_Loop_Cont
	movf	POSTINC0, W	; note played when either gate or accent flag set
	iorwf	INDF0, W
	movwf	PRODL
	andwf	PRODL, W
	bz	SIDSE_D_SEQ_SetStep_LoopNext

	;; play step
 	call	SID_SE_D_NoteOn

SIDSE_D_SEQ_SetStep_LoopNext
	incf	IRQ_TMP1, F
	incf	IRQ_TMP1, F	; (no bug, the secondary voice is selected within loop if gate and accent flag set)
	movlw	17-1
	cpfsgt	IRQ_TMP1, ACCESS
	rgoto SIDSE_D_SEQ_SetStep_Loop

	rgoto	SIDSE_D_SEQ_NextStep_End


SIDSE_D_SEQ_NextStep_GateClr
SIDSE_D_SEQ_NextStep_GateClrForc;ed
	;; request note off for all voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED
  	call	SID_SE_D_NoteOff
	;; 	rgoto	SIDSE_D_SEQ_NextStep_End

SIDSE_D_SEQ_NextStep_End
SIDSE_D_SEQ_End
	return


;; --------------------------------------------------------------------------
;; Note On help function
;; IN: Instrument Number (1..16) in SID_CURRENT_MIDIVOICE
;;     Velocity in MIOS_PARAMETER2 (if >64, accent flag will be set)
;; --------------------------------------------------------------------------
SID_SE_D_NoteOn
	SET_BSR	SID_BASE

	;; pointer to drum instrument -> FSR2
	rcall	SID_SE_D_Hlp_FSR2_Ins

	;; get voice assignment mode for SID_VOICE_Get(Last) function
	movlw	SID_Ix_Dx_FLAGS1
	swapf	PLUSW2, W
	andlw	0x0f
	movwf	SID_CURRENT_VOICE_ASSG, BANKED

	;; number of voices depends on stereo/mono mode
	SET_BSR	SID_LOCAL_ENS
	movlw	0x60
	btfsc	SID_LOCAL_ENS + SID_ENSx_CTRL1, SID_ENS_CTRL1_MONO, BANKED
	movlw	0x30
	SET_BSR	SID_BASE
	iorwf	SID_CURRENT_VOICE_ASSG, F, BANKED

	;; check if drum instrument already played, in this case use the old voice
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_SE_D_NoteOn_SrchILoop
	movlw	SID_Vx_DRUM
	movf	PLUSW1, W
	cpfseq	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_SE_D_NoteOn_SrchINext
	;; check if voice assignment still valid (could have been changed meanwhile)
	movf	SID_CURRENT_VOICE_ASSG, W, BANKED
	andlw	0x0f
	bz	SID_SE_D_NoteOn_SrchILoop_Ok	; all
	addlw	-1
	bz	SID_SE_D_NoteOn_SrchILoop_ChkL	; Left SID only
	addlw	-1
	bz	SID_SE_D_NoteOn_SrchILoop_ChkR	; Right SID only
	addlw	-1
	rgoto	SID_SE_D_NoteOn_SrchILoop_ChkN

SID_SE_D_NoteOn_SrchILoop_ChkL
	movlw	3
	cpfslt	SID_CURRENT_VOICE, BANKED
	rgoto SID_SE_D_NoteOn_SrchINext
	rgoto	SID_SE_D_NoteOn_SrchILoop_Ok
SID_SE_D_NoteOn_SrchILoop_ChkR
	movlw	3-1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_SE_D_NoteOn_SrchINext
	rgoto	SID_SE_D_NoteOn_SrchILoop_Ok
SID_SE_D_NoteOn_SrchILoop_ChkN
	cpfseq	SID_CURRENT_VOICE, BANKED
	rgoto SID_SE_D_NoteOn_SrchINext
	;; 	rgoto	SID_SE_D_NoteOn_SrchILoop_Ok

SID_SE_D_NoteOn_SrchILoop_Ok
	;; get new voice (re-use old voice if it already has been allocated by same instrument)
	movf	SID_CURRENT_VOICE, W, BANKED
	call	SID_VOICE_GetLast
	rgoto	SID_SE_D_NoteOn_SrchILoopBreak
SID_SE_D_NoteOn_SrchINext
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	swapf	SID_CURRENT_VOICE_ASSG, W, BANKED	; number of voices located in CURRENT_VOICE_ASSG[7:4]
	andlw	0x0f
	addlw	-1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_SE_D_NoteOn_SrchILoop

SID_SE_D_NoteOn_SrchI_Failed
	;; instrument not found - get new voice
	call	SID_VOICE_Get
SID_SE_D_NoteOn_SrchILoopBreak	
	;; pointer to voice in FSR1
	;; voice number in SID_CURRENT_VOICE

	;; save instrument number into SID_Vx_DRUM
	movlw	SID_Vx_DRUM
	movff	SID_CURRENT_MIDIVOICE, PLUSW1

	;; save pointer to drum model into SID_Vx_D_MODEL_TBLPTR[LH]
	;; ensure that no invalid table entry will be selected
	movlw	SID_Ix_Dx_MODEL
	movff	PLUSW2, PRODL
	movlw	SID_DMODEL_NUM
	cpfslt	PRODL, ACCESS
	clrf PRODL

	movf	PRODL, W
	TABLE_ADDR_MUL_W SID_DMODEL_TABLE, 2
	movlw	SID_Vx_D_MODEL_TBLPTRL
	tblrd*+
	movff	TABLAT, PLUSW1
	movff	TABLAT, PRODL
	movlw	SID_Vx_D_MODEL_TBLPTRH
	tblrd*+
	movff	TABLAT, PLUSW1
	movff	TABLAT, PRODH

	;; now switch to model entry
	movf	PRODL, W
	addlw	4		; offset +4 (skip model string entry)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; store type in SID_Vx_D_MODEL_TYPE
	tblrd*+
	movlw	SID_Vx_D_MODEL_TYPE
	movff	TABLAT, PLUSW1

	;; store base note in SID_Vx_MODEL_BASENOTE
	tblrd*+
	movlw	SID_Vx_D_MODEL_BASENOTE
	movff	TABLAT, PLUSW1

	;; store base note in SID_Vx_MODEL_GATELENGTH
	tblrd*+			; if != 0, use PAR1 to add offset
	movf	TABLAT, W
	bz	SID_SE_D_NoteOn_GL_Sat_Cont

	;; add PAR1/4 and saturate
	movlw	SID_Ix_Dx_PAR1
	rrf	PLUSW2, W	; (patch pointer in FSR2 here.. normaly in FSR0)
	rrf	WREG, W
	andlw	0x3f
	addlw	-0x20
	movwf	PRODL

	addwf	TABLAT, W
	BRA_IFSET PRODL, 7, ACCESS, SID_SE_D_NoteOn_GL_SatN
SID_SE_D_NoteOn_GL_SatP
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	SID_SE_D_NoteOn_GL_Sat_Cont
SID_SE_D_NoteOn_GL_SatN
	btfsc	WREG, 7
	movlw 0x00
	;; ensure that gatelength is never 0 (otherwise gate clear delay feature would be deactivated)
	andlw	0xff
	skpnz
	addlw	1
	;; 	rgoto	SID_SE_D_NoteOn_GL_Sat_Cont
SID_SE_D_NoteOn_GL_Sat_Cont
	movwf	TABLAT
	movlw	SID_Vx_D_MODEL_GATELENGTH
	movff	TABLAT, PLUSW1

	;; store waveform in SID_Vx_MODEL_WAVEFORM
	tblrd*+
	movlw	SID_Vx_D_MODEL_WAVEFORM
	movff	TABLAT, PLUSW1

	;; store pulsewidth in SID_Vx_MODEL_PULSEWIDTH
	tblrd*+
	movlw	SID_Vx_D_MODEL_PULSEWIDTH
	movff	TABLAT, PLUSW1

	;; store WT speed in SID_Vx_MODE_WT_SPEED
	tblrd*+

	;; add PAR2/4 and saturate
	movlw	SID_Ix_Dx_PAR2
	rrf	PLUSW2, W	; (patch pointer in FSR2 here.. normaly in FSR0)
	rrf	WREG, W
	andlw	0x3f
	addlw	-0x20
	movwf	PRODL

	addwf	TABLAT, W
	BRA_IFSET PRODL, 7, ACCESS, SID_SE_D_NoteOn_WTSpd_SatN
SID_SE_D_NoteOn_WTSpd_SatP
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	SID_SE_D_NoteOn_WTSpd_Sat_Cont
SID_SE_D_NoteOn_WTSpd_SatN
	btfsc	WREG, 7
	movlw 0x00
	;; 	rgoto	SID_SE_D_NoteOn_WTSpd_Sat_Cont
SID_SE_D_NoteOn_WTSpd_Sat_Cont
	movwf	TABLAT
	movlw	SID_Vx_D_MODEL_WT_SPEED
	movff	TABLAT, PLUSW1


	;; store WT end step in SID_Vx_MODE_WT_END
	tblrd*+
	movlw	SID_Vx_D_MODEL_WT_END
	movff	TABLAT, PLUSW1

	;; store WT loop step in SID_Vx_MODE_WT_LOOP
	tblrd*+
	movlw	SID_Vx_D_MODEL_WT_LOOP
	movff	TABLAT, PLUSW1

	;; transfer PAR3 to voice array (temporary solution)
	movlw	SID_Ix_Dx_PAR3
	movff	PLUSW2, TABLAT	; (patch pointer in FSR2 here.. normaly in FSR0)
	movlw	SID_Vx_D_MODEL_PAR3
	movff	TABLAT, PLUSW1

	;; set/clear accent depending on velocity
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_ACCENT
	btfsc	MIOS_PARAMETER2, 6	; value >64
	bsf	PLUSW1, SID_V_STATE_ACCENT

	;; request gate bit if note not active yet
#if 0				; conflict with delayed gate clear handling
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_SE_D_NoteOn_NoGate
#endif
SID_SE_D_NoteOn_Gate
	;; set "voice active" flag
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE

	;; request gate
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED

	;; request LFO re-sync and WT reset
	movf	SID_CURRENT_VOICE, W, BANKED
	addlw	2
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; ENV attack phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVA, F, BANKED
SID_SE_D_NoteOn_NoGate

SID_SE_D_NoteOn_End
	return

;; --------------------------------------------------------------------------
;; Note Off help function
;; IN: Instrument Number (1..16) in SID_CURRENT_MIDIVOICE
;;     If SID_CURRENT_MIDIVOICE == 0: Note Off for all voices!
;; --------------------------------------------------------------------------
SID_SE_D_NoteOff
	SET_BSR	SID_BASE

	;; go through all voices which are assigned to the current instrument and note
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_SE_D_NoteOff_Loop
#if 0				; conflict with delayed gate clear handling
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_SE_D_NoteOff_Loop_Next
#endif
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	bz	SID_SE_D_NoteOff_Loop_All
	movlw	SID_Vx_DRUM
	movf	PLUSW1, W
	cpfseq	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_SE_D_NoteOff_Loop_Next
SID_SE_D_NoteOff_Loop_All

	;; release voice
	call	SID_VOICE_Release

	;; the rest is only required if gatelength not controlled from drum model
	movlw	SID_Vx_D_MODEL_GATELENGTH
	movf	PLUSW1, W
	bnz	SID_SE_D_NoteOff_Loop_Next

	;; request gate off if not disabled via trigger matrix
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; ENV release phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED

SID_SE_D_NoteOff_Loop_Next
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_SE_D_NoteOff_Loop

	return	

;; --------------------------------------------------------------------------
;; Help function which returns a pointer to the assigned instrument
;; of a MIDI voice in FSR2
;; IN: SID_CURRENT_MIDIVOICE
;; OUT: pointer to instrument in FSR2
;; --------------------------------------------------------------------------
SID_SE_D_Hlp_FSR2_Ins
	decf	SID_CURRENT_MIDIVOICE, W, BANKED	; drum instrument number
	andlw	0x0f
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I1_BASE
	mullw	SID_Ix_D_I2_BASE-SID_Ix_D_I1_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	return



;; --------------------------------------------------------------------------
;; This function updates the LED matrix
;; For basslines, this is interrupt driven for fastest update time
;; only used when master is selected
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_D_CS_LM

	;; if slave is selected: exit (update done in CS_MENU_LED_Update_ModMatrix)
	btfss	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; branch depending on normal/meter mode
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, SIDSE_D_CS_LM_Meter
SIDSE_D_CS_LM_Normal

	;; don't activate level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; clear matrix
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movlw	8
	movwf	IRQ_TMP1
SIDSE_D_CS_LM_Normal_ClearLoop
	clrf	POSTINC1
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_D_CS_LM_Normal_ClearLoop

	;; set sequencer position in horizontal line if sequencer enabled and valid pos
	SET_BSR	SID_PATCH_BUFFER_SHADOW
	BRA_IFCLR SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, SID_I_V_SEQ_ON, BANKED, SIDSE_D_CS_LM_Normal_D_Dis
	movf	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	SIDSE_D_CS_LM_Normal_D_Dis
SIDSE_D_CS_LM_Normal_D
	SET_BSR	SID_SEQ1_BASE
	movf	SID_SEQ1_BASE + SID_SEQx_POS, W, BANKED
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	IRQ_TMP2
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN

	;; check if sequencer is running
	movf	SID_SEQ1_BASE + SID_SEQx_MISC, W, BANKED
	andlw	(1 << SID_SEQ_MISC_SEQ_RUNNING)
	bnz	SIDSE_D_CS_LM_Normal_D_Act
SIDSE_D_CS_LM_Normal_D_NAct
	;; static bar
	movlw	8
	movwf	IRQ_TMP1
SIDSE_D_CS_LM_Normal_D_Loop
	movff	IRQ_TMP2, POSTINC1
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_D_CS_LM_Normal_D_Loop
	rgoto	SIDSE_D_CS_LM_Normal_D_End

SIDSE_D_CS_LM_Normal_D_Act 
#if 1	; disable here if you don't like this effect
	;; skip on gate off time for rythmic display
	movf	SID_SEQ1_BASE + SID_SEQx_MISC, W, BANKED
	andlw	0x07
	movwf	IRQ_TMP1
	movlw	5
	cpfslt	IRQ_TMP1, ACCESS
	rgoto SIDSE_D_CS_LM_Normal_D_Dis
#endif

	;; only print to at seq position (16 steps -> 8 LEDs :-/)
	rrf	SID_SEQ1_BASE + SID_SEQx_POS, W, BANKED
	andlw	0x07
	addwf	FSR1L, F
	movff	IRQ_TMP2, INDF1
SIDSE_D_CS_LM_Normal_D_End
SIDSE_D_CS_LM_Normal_D_Dis

	SET_BSR	SID_BASE		
	return

SIDSE_D_CS_LM_Meter
	;; use same function as for multi engine
	goto	SIDSE_D_CS_LM_Meter_ReUse

