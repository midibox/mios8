; $Id$
;
; MIDIbox SID
; Lead Engine
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
;;  Lead Engine handler called by SIDSE_Handler (sid_se.inc)
;; --------------------------------------------------------------------------
SIDSE_L_Handler
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; branch depending on cycle
	BRA_IFSET SID_STAT, SID_STAT_SE_CYCLE, ACCESS, SIDSE_L_Handler_Cycle2

SIDSE_L_Handler_Cycle1
	bsf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the second cycle

	;; clear "allocated" notification for external AOUTs - flags will be set if X2A function active
	clrf	SID_SE_EXT_ALLOCATED, BANKED


	;; ------------------------------------------------------------------
	;; 1st Cycle: LFOs
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_LFO_MACRO MACRO patch_base, lfo_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, lfo_base
	call	SIDSE_L_LFO
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO1_BASE, SID_LFO1_BASE
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO2_BASE, SID_LFO2_BASE
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO3_BASE, SID_LFO3_BASE
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO4_BASE, SID_LFO4_BASE
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO5_BASE, SID_LFO5_BASE
	SIDSE_L_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO6_BASE, SID_LFO6_BASE


	;; ------------------------------------------------------------------
	;; 1st Cycle: Envelopes
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_ENV_MACRO MACRO patch_base, env_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, env_base
	call	SIDSE_L_ENV
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_HANDLER_ENV_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_ENV1_BASE, SID_ENV1_BASE
	SIDSE_L_HANDLER_ENV_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_ENV2_BASE, SID_ENV2_BASE


	;; ------------------------------------------------------------------
	;; 1st Cycle: Modulation Matrix
	;; ------------------------------------------------------------------

	;; clear modulation targets
	movlw	((SID_MOD_TARG_CLEAR_END-SID_MOD_TARG_CLEAR_BEGIN)+1) & 0xff
	movwf	IRQ_TMP1
	lfsr	FSR0, SID_MOD_TARG_CLEAR_BEGIN
SIDSE_L_Handler_ModClearLoop
	clrf	POSTINC0
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_L_Handler_ModClearLoop

	;; since this isn't done anywhere else:
#if 0
	;; convert key number of voice1 to 15bit signed value, centered at 60 (middle-C)
	movff	SIDL_V1_BASE + SID_Vx_TRANSP_NOTE, WREG
	addlw	-60
	movwf	SID_MOD_SRC_L_KEY_H, BANKED
	movlw	0x00
	btfsc	SID_MOD_SRC_L_KEY_H, 7, BANKED
	movlw 0xff
	movwf	SID_MOD_SRC_L_KEY_L, BANKED
#else
	;; modulation only in positive direction
	clrf	SID_MOD_SRC_L_KEY_L, BANKED
	movff	SIDL_V1_BASE + SID_Vx_TRANSP_NOTE, SID_MOD_SRC_L_KEY_H
#endif

	;; copy knob1 to MDW source
	;; in distance to Knob1, this one goes only into positive direction
	clrf	SID_MOD_SRC_L_MDW_L, BANKED
	movf	SID_MOD_SRC_L_KNOB1_H, W, BANKED
	addlw	0x80
	rrf	WREG, W
	andlw	0x7f
	movwf	SID_MOD_SRC_L_MDW_H, BANKED


SIDSE_L_HANDLER_MOD_MACRO MACRO patch_base
	lfsr	FSR0, patch_base
	call	SIDSE_L_MOD
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD1_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD2_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD3_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD4_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD5_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD6_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD7_BASE
	SIDSE_L_HANDLER_MOD_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_MOD8_BASE

	rgoto	SIDSE_L_Handler_End


SIDSE_L_Handler_Cycle2
	bcf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the first cycle


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Clock Handler
	;; ------------------------------------------------------------------
	call	SIDSE_Clk


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Wavetable Handler
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_WT_MACRO MACRO patch_base, wt_base
	lfsr	FSR0, patch_base
	lfsr	FSR2, wt_base
   	rcall	SIDSE_L_WT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_WT1_BASE, SID_WT1_BASE
	SIDSE_L_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_WT2_BASE, SID_WT2_BASE
	SIDSE_L_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_WT3_BASE, SID_WT3_BASE
	SIDSE_L_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_WT4_BASE, SID_WT4_BASE

	;; clear all WT requests
	clrf	SID_SE_TRG_EVNT_U, BANKED


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Arp Handler
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_ARP_MACRO MACRO patch_base, voice_base, midi_voice_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	lfsr	FSR2, midi_voice_base
	call	SIDSE_Arp
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_L_Handler_Arp_L
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE, SIDL_V1_BASE, SID_MV1_BASE
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V2_BASE, SIDL_V2_BASE, SID_MV2_BASE
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V3_BASE, SIDL_V3_BASE, SID_MV3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_L_Handler_Arp_NotR
SIDSE_L_Handler_Arp_R
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V1_BASE, SIDR_V1_BASE, SID_MV4_BASE
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V2_BASE, SIDR_V2_BASE, SID_MV5_BASE
	SIDSE_L_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V3_BASE, SIDR_V3_BASE, SID_MV6_BASE
SIDSE_L_Handler_Arp_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: check ENV/LFO/Note Sync requests
	;; ------------------------------------------------------------------
	call	SIDSE_L_Sync


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Voices
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_NOTE_MACRO MACRO patch_base, voice_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_L_Note
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_L_Handler_Note_L
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE, SIDL_V1_BASE
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V2_BASE, SIDL_V2_BASE
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V3_BASE, SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_L_Handler_Note_NotR
SIDSE_L_Handler_Note_R
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V1_BASE, SIDR_V1_BASE
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V2_BASE, SIDR_V2_BASE
	SIDSE_L_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V3_BASE, SIDR_V3_BASE
SIDSE_L_Handler_Note_NotR

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Filter
	;; ------------------------------------------------------------------

SIDSE_L_Handler_Filter_L
	clrf	SID_SE_ELEMENT_NUM, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL1_L
	lfsr	FSR2, SIDL_BASE
	call	SIDSE_Filter

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_L_Handler_Filter_NotR
SIDSE_L_Handler_Filter_R
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL2_L
	lfsr	FSR2, SIDR_BASE
	call	SIDSE_Filter
SIDSE_L_Handler_Filter_NotR

	;; ------------------------------------------------------------------
	;; 2nd Cycle: AOUTs (External/Extensions)
	;; ------------------------------------------------------------------
SIDSE_L_HANDLER_EXT_MACRO MACRO patch_base, mod_base
	lfsr	FSR0, patch_base
	lfsr	FSR2, mod_base
	call	SIDSE_L_EXT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR1_L, SID_MOD_TARG_EXT1_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR2_L, SID_MOD_TARG_EXT2_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR3_L, SID_MOD_TARG_EXT3_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR4_L, SID_MOD_TARG_EXT4_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR5_L, SID_MOD_TARG_EXT5_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR6_L, SID_MOD_TARG_EXT6_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR7_L, SID_MOD_TARG_EXT7_L
	SIDSE_L_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR8_L, SID_MOD_TARG_EXT8_L


	;; ------------------------------------------------------------------
	;; 2nd Cycle: External Switches
	;; ------------------------------------------------------------------
	call	SIDSE_EXT_Switches

	
	;; ------------------------------------------------------------------
	;; call temporary routine which updates static SID registers
	;; ------------------------------------------------------------------

	rcall	SIDSE_L_UpdateStatRegs


	;; ------------------------------------------------------------------
	;; 2nd Cycle: LED Matrix Update
	;; ------------------------------------------------------------------
	call	SIDSE_L_CS_LM


	;; ------------------------------------------------------------------
	;; clear FA/FB and FC synch request
	;; ------------------------------------------------------------------
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED

SIDSE_L_Handler_End
	return


;; --------------------------------------------------------------------------
;;  Updates static SID registers
;;  (temporary)
;; --------------------------------------------------------------------------
SIDSE_L_UpdateStatRegs
SIDSE_UPDATESTAT_MACRO	MACRO p_base, sid_base, swinsid_mode, swinsid_phase
	LOCAL	SIDSE_L_UpdateStatRegs_NoADSR

	lfsr	FSR0, p_base

	movlw	0x09
	andwf	sid_base + SIDx_V1_CTRL, F, BANKED
	movlw	SID_Ix_Vx_WAVEFORM
	swapf	PLUSW0, W
	andlw	0xf6
#if DEFAULT_ENABLE_UPPER_WAVEFORMS == 0
	;; disable to allow nice effects in conjunction with testbit
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw	0x8f
#else
#if 0
	;; optional WT hack - see http://www.midibox.org/forum/index.php/topic,13898.0.html
	;; and http://www.dekadence64.org/sidwav.txt
	;; In order to test this, use following WT sequence assigned to Waveform of OSC1 (or OSC1/2/3)
	;; 00: 0F
	;; 01: 00
	;; 02: 08
	;; Begin: 00, End: 08, Loop: 00, Speed: 1..4
	skpz
	bcf	sid_base + SIDx_V1_CTRL, 3 ; ensure that testflag not set
	skpnz
	iorlw	0x08		; set testflag if all waveforms disabled
#endif
#endif
	iorwf	sid_base + SIDx_V1_CTRL, F, BANKED

	;; if ABW (ADSR bug workaround) function active, update ADSR register only when the gate is set
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFSET WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_L_UpdateStatRegs_NoADSR
	movlw	SID_Ix_Vx_AD
	movff	PLUSW0, sid_base + SIDx_V1_ENV_AD	
	movlw	SID_Ix_Vx_SR
	movff	PLUSW0, sid_base + SIDx_V1_ENV_SR
SIDSE_L_UpdateStatRegs_NoADSR
#if DEFAULT_ENABLE_SWINSID
	movlw	SID_Ix_Vx_SWINSID_MODE
	movff	PLUSW0, swinsid_mode
	movlw	SID_Ix_Vx_SWINSID_PHASE
	movff	PLUSW0, swinsid_phase
#endif
	ENDM

	SET_BSR	SIDL_BASE

	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE, SIDL_BASE + SIDx_V1_FRQ_L, SIDL_BASE + SIDx_SWINSID_V1_MODE, SIDL_BASE + SIDx_SWINSID_V1_PHASE
	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V2_BASE, SIDL_BASE + SIDx_V2_FRQ_L, SIDL_BASE + SIDx_SWINSID_V2_MODE, SIDL_BASE + SIDx_SWINSID_V2_PHASE
	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V3_BASE, SIDL_BASE + SIDx_V3_FRQ_L, SIDL_BASE + SIDx_SWINSID_V3_MODE, SIDL_BASE + SIDx_SWINSID_V3_PHASE

	;; in Mono mode: copy *ALL* SID registers (not only the static ones) over to SIDR
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_L_UpdateStatRegs_Mono
SIDSE_L_UpdateStatRegs_Stereo
	;; stereo mode:	most SIDs registers already updated, copy over the static ones
	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V1_BASE, SIDR_BASE + SIDx_V1_FRQ_L, SIDR_BASE + SIDx_SWINSID_V1_MODE, SIDR_BASE + SIDx_SWINSID_V1_PHASE
	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V2_BASE, SIDR_BASE + SIDx_V2_FRQ_L, SIDR_BASE + SIDx_SWINSID_V2_MODE, SIDR_BASE + SIDx_SWINSID_V2_PHASE
	SIDSE_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V3_BASE, SIDR_BASE + SIDx_V3_FRQ_L, SIDR_BASE + SIDx_SWINSID_V3_MODE, SIDR_BASE + SIDx_SWINSID_V3_PHASE
	rgoto	SIDSE_L_UpdateStatRegs_End


SIDSE_L_UpdateStatRegs_Mono
	;; mono mode: copy over *all* SID registers of SIDL to SIDR
	lfsr	FSR1, SIDL_BASE
	lfsr	FSR2, SIDR_BASE
#if DEFAULT_ENABLE_SWINSID
	movlw	0x20
#else
	movlw	SIDx_MODE_VOL+1
#endif
	movwf	PRODL		; (loop counter)
SIDSE_L_UpdateStatRegs_Mono_Loop
	movff	POSTINC1, POSTINC2
	decfsz	PRODL, F
	rgoto	SIDSE_L_UpdateStatRegs_Mono_Loop

SIDSE_L_UpdateStatRegs_End
	SET_BSR	SID_BASE
	return


;; --------------------------------------------------------------------------
;; This function handles the notes (gate/pitch/PW)
;; IN: pointer to SID_Ix_e_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_Note
	call	SIDSE_Gate	; gate handler
	skpz			; returns ZERO flag cleared if pitch should not be changed
   	call	SIDSE_Pitch	; pitch handler
	goto	SIDSE_PW	; pulsewidth handler


;; --------------------------------------------------------------------------
;; This function handles the wavetables
;; IN: pointer to SID_Ix_WTx_BASE in FSR0 (patch record)
;;     pointer to SID_WTx_BASE in FSR2 (wt record)
;;     WT number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_WT
	;; if MOD control flag (START[7] set, control position from modulation matrix
	movlw	SID_Ix_WTx_BEGIN
	BRA_IFCLR PLUSW0, 7, ACCESS, SIDSE_L_WT_NoMOD
SIDSE_L_WT_MOD
	;; determine range: END-BEGIN+1 -> IRQ_TMP5
	;; start value -> IRQ_TMP4 (either END or BEGIN, depending on polarity)
	movlw	SID_Ix_WTx_END
	movf	PLUSW0, W
	andlw	0x7f
	movwf	IRQ_TMP5
	bcf	IRQ_TMP5, 7
	movlw	SID_Ix_WTx_BEGIN
	movf	PLUSW0, W
	andlw	0x7f
	movwf	IRQ_TMP4
	subwf	IRQ_TMP5, F
	;; n+1 complement if begin greater than end
	BRA_IFCLR IRQ_TMP5, 7, ACCESS, SIDSE_L_WT_MOD_NoRngInv
SIDSE_L_WT_MOD_RngInv
	comf	IRQ_TMP5, F
	incf	IRQ_TMP5, F
	movlw	SID_Ix_WTx_END
	movf	PLUSW0, W
	andlw	0x7f
	movwf	IRQ_TMP4
SIDSE_L_WT_MOD_NoRngInv
	incf	IRQ_TMP5, F

	;; determine pointer to modulation target value -> FSR1
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_WT1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR1, SID_MOD_TARG_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	;; convert modulation target value to 8bit absolute value
	movff	POSTINC1, IRQ_TMP1
	movff	POSTINC1, IRQ_TMP2
	movff	POSTINC1, IRQ_TMP3

	movlw	0x80
	addwf	IRQ_TMP2, F

	;; saturate
	BRA_IFSET IRQ_TMP3, 7, ACCESS, SIDSE_L_WT_MOD_SatNeg
SIDSE_L_WT_MOD_SatPos
	movf	IRQ_TMP3, W
	bnz	SIDSE_L_WT_MOD_SatPos_Sat
	bnc	SIDSE_L_WT_MOD_NoSat
SIDSE_L_WT_MOD_SatPos_Sat
	setf	IRQ_TMP2
	rgoto	SIDSE_L_WT_MOD_Sat_Cont
SIDSE_L_WT_MOD_SatNeg
	comf	IRQ_TMP3, W
	bnz	SIDSE_L_WT_MOD_SatNeg_Sat
	bc	SIDSE_L_WT_MOD_NoSat
SIDSE_L_WT_MOD_SatNeg_Sat
	clrf	IRQ_TMP2
	;; 	rgoto	SIDSE_L_WT_MOD_Sat_Cont
SIDSE_L_WT_MOD_NoSat
SIDSE_L_WT_MOD_Sat_Cont

	;; determine position
	movf	IRQ_TMP5, W		; range
	mulwf	IRQ_TMP2, ACCESS	; modulation value
	movf	PRODH, W		; scaled value
 	addwf	IRQ_TMP4, F		; add to begin position

	;; play new step if position has been changed
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	xorwf	IRQ_TMP4, W
	bz	SIDSE_L_WT_End
	
	movlw	SID_WTx_POS
	movff	IRQ_TMP4, PLUSW2
	rgoto	SIDSE_L_WT_PlayStep



	;; normal WT mode
SIDSE_L_WT_NoMOD
	;; clear temporary WT flags
	bcf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED

	;; check if WT reset requested
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	SID_TRG_TARGET_U_W1R
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_TRG_EVNT_U, W, BANKED
	bz	SIDSE_L_WT_NoDivReset
SIDSE_L_WT_DivReset
	;; next clock event will increment div to 0
	movlw	SID_WTx_DIV_CTR
	setf	PLUSW2
	;; next step will increment to start position
	movlw	SID_Ix_WTx_BEGIN
	movf	PLUSW0, W
	andlw	0x7f
	addlw	-1
	movwf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
SIDSE_L_WT_NoDivReset

	;; check for clock sync event
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	SID_TRG_TARGET_U_W1S
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_TRG_EVNT_U, W, BANKED
	bz	SIDSE_L_WT_NoClk
SIDSE_L_WT_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value
	movlw	SID_WTx_DIV_CTR
	incf	PLUSW2, W
	movwf	PRODL
	bz	SIDSE_L_WT_Clk_Ov

	;; reset once max value is reached
	movlw	SID_Ix_WTx_SPEED
	movf	PLUSW0, W
	andlw	0x3f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_L_WT_Clk_NoOv
SIDSE_L_WT_Clk_Ov
	clrf	PRODL
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_L_WT_Clk_NoOv
	;; transfer new divider value into WTx register
	movlw	SID_WTx_DIV_CTR
	movff	PRODL, PLUSW2
SIDSE_L_WT_NoClk

	;; skip if no new step requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED, SIDSE_L_WT_End

	;; skip if position is 0xaa (notifies oneshot -> WT stopped)
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	xorlw	0xaa
	bz	SIDSE_L_WT_End

SIDSE_L_WT_NextStep
	;; increment position counter, reset at end position
	movlw	SID_WTx_POS
	incf	PLUSW2, W
	movwf	PRODL
	movlw	SID_Ix_WTx_END
	movf	PLUSW0, W
	andlw	0x7f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_L_WT_NextStep_NoOv
SIDSE_L_WT_NextStep_Ov
	;; if one-shot mode: set position to 0xaa, WT is stopped now!
	movlw	SID_Ix_WTx_LOOP
	BRA_IFCLR PLUSW0, 7, ACCESS, SIDSE_L_WT_NextStep_Ov_NoOneShot
SIDSE_L_WT_NextStep_Ov_OneShot
	movlw	0xaa
	movwf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
	rgoto	SIDSE_L_WT_End

SIDSE_L_WT_NextStep_Ov_NoOneShot
	movlw	SID_Ix_WTx_LOOP
	movf	PLUSW0, W
	andlw	0x7f
	movwf	PRODL
SIDSE_L_WT_NextStep_NoOv
	;; transfer back to position counter
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2

	;; "play" the step
	;; (entry point for SIDSE_L_WT_MOD_...)
SIDSE_L_WT_PlayStep
	;; get pointer to WT value
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_WAVETABLE
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	andlw	0x7f
	addwf	FSR1L, F

	;; store value in MIOS_PARAMETER1
	movff	INDF1, MIOS_PARAMETER1


	;; forward to MOD matrix
	lfsr	FSR2, SID_MOD_SRC_L_WT1_L
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addwf	FSR2L, F
	;; branch depending on absolute (+0000..+7F00) and relative (-8000..+7FFF)
	BRA_IFSET MIOS_PARAMETER1, 7, ACCESS, SIDSE_L_WT_PlayStep_ModAbs
SIDSE_L_WT_PlayStep_ModRel
	;; convert to 15bit signed
	clrf	POSTINC2	; SID_MOD_SRC_L_WTx_VALUE_L
	clrc
	rlf	MIOS_PARAMETER1, W
	addlw	-0x80
	movwf	POSTDEC2	; SID_MOD_SRC_L_WTx_VALUE_H
	rgoto	SIDSE_L_WT_PlayStep_Mod_Cont

SIDSE_L_WT_PlayStep_ModAbs
	clrf	POSTINC2	; SID_MOD_SRC_L_WTx_VALUE_L
	movf	MIOS_PARAMETER1, W
	andlw	0x7f
	movwf	POSTDEC2	; SID_MOD_SRC_L_WTx_VALUE_H
	;; 	rgoto	SIDSE_L_WT_PlayStep_Mod_Cont
SIDSE_L_WT_PlayStep_Mod_Cont


	;; store SIDs which should be modified in MIOS_PARAMETER3[1:0]
	clrf	MIOS_PARAMETER3
	movlw	SID_Ix_WTx_SPEED
	btfsc	PLUSW0, 6
	bsf	MIOS_PARAMETER3, 0
	btfsc	PLUSW0, 7
	bsf	MIOS_PARAMETER3, 1
	;; call WT Parameter Handler
	movlw	SID_Ix_WTx_ASSGN
	movf	PLUSW0, W
	call	SID_PARIN_SetWT

SIDSE_L_WT_End
	return



;; --------------------------------------------------------------------------
;; This function syncs the LFOs/ENVs and Notes
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_L_Sync
SIDSE_L_SYNC_ENV_MACRO	MACRO	evnt_a, evnt_a_flag, evnt_r, evnt_r_flag, patch_base, env_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, env_base
	CALL_IFSET evnt_r, evnt_r_flag, BANKED, SIDSE_L_ENV_TrgRelease
	bcf	evnt_r, evnt_r_flag, BANKED
	CALL_IFSET evnt_a, evnt_a_flag, BANKED, SIDSE_L_ENV_Restart
	bcf	evnt_a, evnt_a_flag, BANKED
	ENDM

	SIDSE_L_SYNC_ENV_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_E1A, SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_E1R, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_ENV1_BASE, SID_ENV1_BASE
	SIDSE_L_SYNC_ENV_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_E2A, SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_E2R, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_ENV2_BASE, SID_ENV2_BASE


SIDSE_L_SYNC_LFO_MACRO	MACRO	evnt, evnt_flag, patch_base, lfo_base
	LOCAL	SIDSE_L_SYNC_LFO_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_L_SYNC_LFO_Skip
	lfsr	FSR0, patch_base
	lfsr	FSR1, lfo_base
	call	SIDSE_L_LFO_Restart
	bcf	evnt, evnt_flag, BANKED
SIDSE_L_SYNC_LFO_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L1, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO1_BASE, SID_LFO1_BASE
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L2, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO2_BASE, SID_LFO2_BASE
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L3, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO3_BASE, SID_LFO3_BASE
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L4, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO4_BASE, SID_LFO4_BASE
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L5, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO5_BASE, SID_LFO5_BASE
	SIDSE_L_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L6, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_LFO6_BASE, SID_LFO6_BASE


SIDSE_L_SYNC_NOTE_MACRO	MACRO	evnt, evnt_flag, patch_base, voice_base
	LOCAL	SIDSE_L_SYNC_NOTE_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_L_SYNC_NOTE_Skip
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	call	SIDSE_L_NOTE_Restart
	bcf	evnt, evnt_flag, BANKED
SIDSE_L_SYNC_NOTE_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1L, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE, SIDL_V1_BASE
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2L, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V2_BASE, SIDL_V2_BASE
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3L, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V3_BASE, SIDL_V3_BASE
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1R, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V1_BASE, SIDR_V1_BASE
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2R, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V2_BASE, SIDR_V2_BASE
	SIDSE_L_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3R, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S2V3_BASE, SIDR_V3_BASE

	return


;; --------------------------------------------------------------------------
;; This function handles the AOUTs
;; IN: pointer to SID_Ix_EXT_PARx_L in FSR0 (patch record)
;;     pointer to AOUTx_L in FSR1 (aout target)
;;     pointer to SID_MOD_TARG_EXTx_L in FSR2 (modulation value)
;;     EXT number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_EXT
	;; skip if any F2X function has already updated the AOUTx register
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_EXT_ALLOCATED, W, BANKED
	bnz	SIDSE_L_EXT_End

	;; store 16bit offset in MIOS_PARAMETER[12]
	movff	POSTINC0, MIOS_PARAMETER1
	movff	POSTDEC0, MIOS_PARAMETER2

	;; pointer to SID_MOD_TARG_EXTx_L already in FSR2
	;; add modulation value to offset
	movf	POSTINC2, W
	addwf	MIOS_PARAMETER1, F
	movf	POSTINC2, W
	addwfc	MIOS_PARAMETER2, F

	;; saturate
	BRA_IFSET INDF2, 7, ACCESS, SIDSE_L_EXT_Mod_SatNeg
SIDSE_L_EXT_Mod_SatPos
	movf	INDF2, W
	bnz	SIDSE_L_EXT_Mod_SatPos_Sat
	bnc	SIDSE_L_EXT_Mod_Sat_NoSat
SIDSE_L_EXT_Mod_SatPos_Sat
	setf	MIOS_PARAMETER1
	setf	MIOS_PARAMETER2
	rgoto	SIDSE_L_EXT_Mod_Sat_Cont
SIDSE_L_EXT_Mod_SatNeg
	comf	INDF2, W
	bnz	SIDSE_L_EXT_Mod_SatNeg_Sat
	bc	SIDSE_L_EXT_Mod_Sat_NoSat
SIDSE_L_EXT_Mod_SatNeg_Sat
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	;; 	rgoto	SIDSE_L_EXT_Mod_Sat_Cont
SIDSE_L_EXT_Mod_Sat_Cont
SIDSE_L_EXT_Mod_Sat_NoSat

	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

SIDSE_L_EXT_End
	return



;; --------------------------------------------------------------------------
;; This function handles the LFOs (the SIDSE_L_LFO_Restart function restarts a LFO)
;; IN: pointer to SID_Ix_LFOx_BASE in FSR0 (patch record)
;;     pointer to SID_LFOx_BASE in FSR1
;;     LFO number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_LFO_Restart
	;; if oneshot mode: don't take sync mode into account if we already had a overrun condition
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_L_LFO_Restart_NoOneShot
SIDSE_L_LFO_Restart_OneShot
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_LFO_OVERRUN, W, BANKED
	bnz	SIDSE_L_LFO_Restart_OneShot_Cont
	;; no overrun occured yet - check for sync
SIDSE_L_LFO_Restart_NoOneShot

SIDSE_L_LFO_Restart_OneShot_Cont
	;; reset counter (take phase into account)
	movlw	0x00
	movwf	POSTINC1	; SID_LFOx_CTR_L
	movlw	SID_Ix_LFOx_PHASE
	movff	PLUSW0, POSTDEC1; SID_LFOx_CTR_H

	;; clear overrun flag (for oneshot mode)
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	SID_SE_LFO_OVERRUN, F, BANKED

	;; check if LFO should be delayed - set delay counter to 0x0001 in this case, else to 0x0000
	movlw	SID_LFOx_DELAY_CTR_H	; high byte always reset to 0
	clrf	PLUSW1

	movlw	SID_Ix_LFOx_DELAY
	clrf	PRODL			; low byte set to 1 if delay != 0
	movf	PLUSW0, W
	skpz
	incf	PRODL, F
	movlw	SID_LFOx_DELAY_CTR_L
	movff	PRODL, PLUSW1

	;; continue at waveform calculation
	rgoto	SIDSE_L_LFO_Restart_Cont

;; --------------------------------------------------------------------------
SIDSE_L_LFO			; normal LFO entry
	;; set wave register to initial value and skip LFO if not enabled
	movlw	SID_Ix_LFOx_MODE
	BRA_IFSET PLUSW0, SID_I_LFO_MODE_ENABLE, ACCESS, SIDSE_L_LFO_Enabled
SIDSE_L_LFO_Disabled
	lfsr	FSR2, SID_MOD_SRC_L_LFO1_L
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addwf	FSR2L, F
	clrf	POSTINC2	; SID_MOD_SRC_L_LFOx_VALUE_L
	clrf	POSTDEC2	; SID_MOD_SRC_L_LFOx_VALUE_H
	rgoto	SIDSE_L_LFO_End
SIDSE_L_LFO_Enabled

	;; temporary shift FSR1 to SID_LFOx_DELAY_CTR_L for easier handling
	movlw	SID_LFOx_DELAY_CTR_L
	addwf	FSR1L, F

	;; delay LFO so long 16bit delay counter != 0
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	bz	SIDSE_L_LFO_NoDelay
SIDSE_L_LFO_Delay
	;; increment counter, set it to zero on overrun (no delay anymore)
	movlw	SID_Ix_LFOx_DELAY
	movff	PLUSW0, IRQ_TMP3; incrementer is the same like used for envelopes
	movlw	0x80		; curve *must* be disabled!
	call	SIDSE_Hlp_ENV_GetBendedValue	; incrementer in MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	addwf	POSTINC1, F
	movf	MIOS_PARAMETER2, W
	addwfc	POSTDEC1, F
	bc	SIDSE_L_LFO_DelayOv
	rgoto	SIDSE_L_LFO_End
SIDSE_L_LFO_DelayOv
	;; overrun: clear counter to disable delay
	clrf	POSTINC1
	clrf	POSTDEC1
SIDSE_L_LFO_NoDelay
	movlw	-SID_LFOx_DELAY_CTR_L	; switch back to SID_LFOx_CTR_L
	addwf	FSR1L, F

	;; implementation: a LFO is just a 16bit ramp which is mapped to a waveform

	;; clear LFO overrun flag
	bcf	SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED

	;; in oneshot mode: check if overrun already occured
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_L_LFO_NoOneShotChk
SIDSE_L_LFO_OneShotChk
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_LFO_OVERRUN, W, BANKED
	bz	SIDSE_L_LFO_NoOneShotChk
	;; set counter to maximum value and continue at waveform calculation
	setf	POSTINC1
	setf	POSTDEC1
	rgoto	SIDSE_L_LFO_OneShot_Cont
SIDSE_L_LFO_NoOneShotChk

	;; if clock sync enabled: only increment on each 6th clock event
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_CLKSYNC, ACCESS, SIDSE_L_LFO_NoClkSync
SIDSE_L_LFO_ClkSync
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED, SIDSE_L_LFO_NoClk_Cont
	movf	SID_SE_GLOBAL_CLK_CTR, W, BANKED
	andlw	0x0f
	bnz	SIDSE_L_LFO_NoClk_Cont
SIDSE_L_LFO_NoClkSync

	;; increment 16bit counter by given rate
	movlw	SID_Ix_LFOx_RATE
	movff	PLUSW0, IRQ_TMP1

	;; rate can be modulated

	;; calculate pointer to MOD target array -> FSR2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_LR1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR2, SID_MOD_TARG_BASE
	movf	PRODL, W
	addlw	1		; select SID_MOD_TARG_LRx_H
	addwf	FSR2L, F
	movf	POSTINC2, W
	addwf	IRQ_TMP1, F

	;; saturate
	BRA_IFSET INDF2, 7, ACCESS, SIDSE_L_LFO_Rate_SatNeg
SIDSE_L_LFO_RateNeg_SatPos
	movf	INDF2, W
	bnz	SIDSE_L_LFO_RateNeg_SatPos_Sat
	bnc	SIDSE_L_LFO_RateNeg_NoSat
SIDSE_L_LFO_RateNeg_SatPos_Sat
	setf	IRQ_TMP1
	rgoto	SIDSE_L_LFO_RateNeg_Sat_Cont
SIDSE_L_LFO_Rate_SatNeg
	comf	INDF2, W
	bnz	SIDSE_L_LFO_Rate_SatNeg_Sat
	bc	SIDSE_L_LFO_RateNeg_NoSat
SIDSE_L_LFO_Rate_SatNeg_Sat
	clrf	IRQ_TMP1
	;; 	rgoto	SIDSE_L_LFO_RateNeg_Sat_Cont
SIDSE_L_LFO_RateNeg_Sat_Cont
SIDSE_L_LFO_RateNeg_NoSat

	;; if LFO synched via clock, replace 245-255 by MIDI clock optimized incrementers
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_CLKSYNC, ACCESS, SIDSE_L_LFO_Rate_NoClkSync
SIDSE_L_LFO_Rate_ClkSync
	movf	IRQ_TMP1, W
	addlw	-245
	bnc	SIDSE_L_LFO_Rate_NoClkSyncValues
	TABLE_ADDR_MUL_W SID_LFO_TABLE_MCLK, 2
	rgoto	SIDSE_L_LFO_Rate_ClkSync_Cont
SIDSE_L_LFO_Rate_NoClkSyncValues
SIDSE_L_LFO_Rate_NoClkSync

	movf	IRQ_TMP1, W
	TABLE_ADDR_MUL_W SID_LFO_TABLE, 2
SIDSE_L_LFO_Rate_ClkSync_Cont
	tblrd*+
	movf	TABLAT, W
	addwf	POSTINC1, F	; SID_LFOx_CTR_L
	tblrd*+
	movf	TABLAT, W
	addwfc	POSTDEC1, F	; SID_LFOx_CTR_H
	bnc	SIDSE_L_LFO_NoOverrun

SIDSE_L_LFO_Overrun
	;; required for S&H function below, will be cleared with each run
	bsf	SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED

	;; propagate overrun to trigger matrix (using FSR2 as working pointer)
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_L1P_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; required for one-shot mode, will be cleared with next gate trigger
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_LFO_OVERRUN, F, BANKED

	;; in oneshot mode: ensure that counter is set to maximum value
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_L_LFO_OverrunNoOneShot
SIDSE_L_LFO_OverrunOneShot
	setf	POSTINC1
	setf	POSTDEC1
SIDSE_L_LFO_OverrunNoOneShot
SIDSE_L_LFO_NoOverrun

SIDSE_L_LFO_OneShot_Cont		; entry point for oneshot function (if overrun already occured)
SIDSE_L_LFO_NoClk_Cont		; entry point for clock sync function (no clock event)
SIDSE_L_LFO_Restart_Cont		; entry point for restart function

#if 0
	;; DISABLED - for the case that depth is modulated we need to calculate the whole waveform

	;; skip the rest if depth is 0 (no waveform calculation required)
	movlw	SID_Ix_LFOx_DEPTH
	movf	PLUSW0, W
	xorlw	0x80
	skpnz
	rgoto	SIDSE_L_LFO_Zero
#endif

	;; temporary store SID_LFOx_CTR_[LH] in IRQ_TMP[12]
	movff	POSTINC1, IRQ_TMP1
	movff	POSTDEC1, IRQ_TMP2

	;; map to waveform
	;; result in MUL_A_[LH]
	movlw	SID_Ix_LFOx_MODE ; waveform selection in bit [6:4]
	swapf	PLUSW0, W
	BRA_IFSET WREG, 3, ACCESS, SIDSE_L_LFO_Wav89ABCDEF
SIDSE_L_LFO_Wav01234567
	BRA_IFSET WREG, 2, ACCESS, SIDSE_L_LFO_Wav4567
SIDSE_L_LFO_Wav0123
	BRA_IFSET WREG, 1, ACCESS, SIDSE_L_LFO_Wav23
SIDSE_L_LFO_Wav01
	BRA_IFSET WREG, 0, ACCESS, SIDSE_L_LFO_Wav1

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav0	; Sine
	;; sine table contains a quarter of a sine,
	;; we have to negate/mirror it depending on the mapped counter value
	rlf	IRQ_TMP1, W
	rlf	IRQ_TMP2, W
	btfsc	IRQ_TMP2, 6
	xorlw 0x7f
	andlw	0x7f
	TABLE_ADDR_MUL_W SID_SIN_TABLE, 2
	tblrd*+
	movff	TABLAT, MUL_A_L
	tblrd*+
	movff	TABLAT, MUL_A_H

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_L_LFO_Wav0_NoNeg
SIDSE_L_LFO_Wav0_Neg
	comf	MUL_A_L, F, BANKED
	comf	MUL_A_H, F, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
SIDSE_L_LFO_Wav0_NoNeg

	rgoto	SIDSE_L_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav1	; Triangle
	;; similar to sine, but linear waveform
	clrc
	rlf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED
	rlf	IRQ_TMP2, W
	andlw	0x7f
	movwf	MUL_A_H, BANKED

	BRA_IFCLR IRQ_TMP2, 6, ACCESS, SIDSE_L_LFO_Wav1_NoNeg1
SIDSE_L_LFO_Wav1_Neg1
	comf	MUL_A_L, BANKED
	comf	MUL_A_H, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
	bcf	MUL_A_H, 7, BANKED
SIDSE_L_LFO_Wav1_NoNeg1

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_L_LFO_Wav1_NoNeg2
SIDSE_L_LFO_Wav1_Neg2
	comf	MUL_A_L, F, BANKED
	comf	MUL_A_H, F, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
SIDSE_L_LFO_Wav1_NoNeg2

	rgoto	SIDSE_L_LFO_Wav_Cont

SIDSE_L_LFO_Wav23
	BRA_IFSET WREG, 0, ACCESS, SIDSE_L_LFO_Wav3
	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav2	; Saw
	;; this one is simple - it's "counter-0x8000"
	movlw	0x00
	subwf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED
	movlw	0x80
	subwfb	IRQ_TMP2, W
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_L_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav3	; Pulse
	;; this one is even more simple
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0xff
	movwf	MUL_A_L, BANKED
	movlw	0x80
	btfsc	IRQ_TMP2, 7
	movlw 0x7f
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_L_LFO_Wav_Cont

SIDSE_L_LFO_Wav4567
	BRA_IFSET WREG, 1, ACCESS, SIDSE_L_LFO_Wav67
SIDSE_L_LFO_Wav45
	BRA_IFSET WREG, 0, ACCESS, SIDSE_L_LFO_Wav5

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav4	; Random
	;; only on LFO overrun:
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED, SIDSE_L_LFO_End

	;; generate new random number and copy it to MUL_A_[LH]
	call	SID_RND_GenRandomNumber
	movff	SID_RANDOM_SEED_L, MUL_A_L
	movff	SID_RANDOM_SEED_L, MUL_A_H

	rgoto	SIDSE_L_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav5	; positive sine
	;; sine table contains a quarter of a sine,
	;; we have to negate/mirror it depending on the mapped counter value
	movf	IRQ_TMP2, W
	btfsc	IRQ_TMP2, 7
	xorlw 0x7f
	andlw	0x7f
	TABLE_ADDR_MUL_W SID_SIN_TABLE, 2
	tblrd*+
	movff	TABLAT, MUL_A_L
	tblrd*+
	movff	TABLAT, MUL_A_H

	rgoto	SIDSE_L_LFO_Wav_Cont


SIDSE_L_LFO_Wav67
	BRA_IFSET WREG, 0, ACCESS, SIDSE_L_LFO_Wav7

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav6	; positive triangle
	movff	IRQ_TMP1, MUL_A_L
	movff	IRQ_TMP2, MUL_A_H

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_L_LFO_Wav6_NoNeg
SIDSE_L_LFO_Wav6_Neg
	comf	MUL_A_L, BANKED
	comf	MUL_A_H, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
	bcf	MUL_A_H, 7, BANKED
SIDSE_L_LFO_Wav6_NoNeg

	rgoto	SIDSE_L_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav7	; positive saw
	clrc
	rrf	IRQ_TMP2, W
	movwf	MUL_A_H, BANKED
	rrf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED

	rgoto	SIDSE_L_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_L_LFO_Wav89ABCDEF	; positive pulse
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0xff
	movwf	MUL_A_L, BANKED
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0x7f
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_L_LFO_Wav_Cont

SIDSE_L_LFO_Wav_Cont

	;; scale by LFO depth
	movlw	SID_Ix_LFOx_DEPTH
	movff	PLUSW0, IRQ_TMP1

	;; depth can be modulated
	;; calculate pointer to MOD target array -> FSR2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_LD1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR1, SID_MOD_TARG_BASE
	movf	PRODL, W
	addlw	1		; select SID_MOD_TARG_LDx_H
	addwf	FSR1L, F
	movf	POSTINC1, W
	addwf	IRQ_TMP1, F

	;; saturate
	BRA_IFSET INDF1, 7, ACCESS, SIDSE_L_LFO_Depth_SatNeg
SIDSE_L_LFO_DepthNeg_SatPos
	movf	INDF1, W
	bnz	SIDSE_L_LFO_DepthNeg_SatPos_Sat
	bnc	SIDSE_L_LFO_DepthNeg_NoSat
SIDSE_L_LFO_DepthNeg_SatPos_Sat
	setf	IRQ_TMP1
	rgoto	SIDSE_L_LFO_DepthNeg_Sat_Cont
SIDSE_L_LFO_Depth_SatNeg
	comf	INDF1, W
	bnz	SIDSE_L_LFO_Depth_SatNeg_Sat
	bc	SIDSE_L_LFO_DepthNeg_NoSat
SIDSE_L_LFO_Depth_SatNeg_Sat
	clrf	IRQ_TMP1
	;; 	rgoto	SIDSE_L_LFO_DepthNeg_Sat_Cont
SIDSE_L_LFO_DepthNeg_Sat_Cont
SIDSE_L_LFO_DepthNeg_NoSat
	movf	IRQ_TMP1, W
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[12], uses IRQ_TMP1
	rgoto	SIDSE_L_LFO_NotZero

SIDSE_L_LFO_Zero
	clrf	MUL_R_1, BANKED
	clrf	MUL_R_2, BANKED

SIDSE_L_LFO_NotZero
	;; finally transfer result to SID_MOD_SRC_L_LFO1_[LH]
	;; range: +/- 0x7fff
	lfsr	FSR2, SID_MOD_SRC_L_LFO1_L
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addwf	FSR2L, F
	movff	MUL_R_1, POSTINC2
	movff	MUL_R_2, POSTDEC2

SIDSE_L_LFO_End
	return
	

;; --------------------------------------------------------------------------
;; This function handles the Envelopes
;; IN: pointer to SID_Ix_L_ENVx_BASE in FSR0 (patch record)
;;     pointer to SID_ENVx_BASE in FSR1
;;     ENV number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_ENV
	;; if clock sync enabled: only increment on clock events
	movlw	SID_Ix_L_ENVx_MODE
	BRA_IFCLR PLUSW0, SID_I_ENV_MODE_CLKSYNC, ACCESS, SIDSE_L_ENV_NoClkSync
SIDSE_L_ENV_ClkSync
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED, SIDSE_L_ENV_ClkSync_Skp
	movf	SID_SE_GLOBAL_CLK_CTR, W, BANKED
	andlw	0x0f
	bz	SIDSE_L_ENV_ClkSync_Cont
SIDSE_L_ENV_ClkSync_Skp
	;; transfer current ENV counter into MIOS_PARAMETER[12]
	movff	POSTINC1, MIOS_PARAMETER1
	movff	POSTDEC1, MIOS_PARAMETER2
	rgoto	SIDSE_L_ENV_Cont
SIDSE_L_ENV_NoClkSync
SIDSE_L_ENV_ClkSync_Cont

	;; branch depending on EG state
	movlw	SID_ENVx_STATE
	BRA_IFSET PLUSW1, SID_ENV_STATE_RELEASE2, ACCESS, SIDSE_L_ENV_Release2
	BRA_IFSET PLUSW1, SID_ENV_STATE_RELEASE1, ACCESS, SIDSE_L_ENV_Release1
	BRA_IFCLR PLUSW1, SID_ENV_STATE_ATTACK1, ACCESS, SIDSE_L_ENV_End
	BRA_IFSET PLUSW1, SID_ENV_STATE_SUSTAIN, ACCESS, SIDSE_L_ENV_Sustain
	BRA_IFSET PLUSW1, SID_ENV_STATE_DECAY2, ACCESS, SIDSE_L_ENV_Decay2
	BRA_IFSET PLUSW1, SID_ENV_STATE_DECAY1, ACCESS, SIDSE_L_ENV_Decay1
	BRA_IFSET PLUSW1, SID_ENV_STATE_ATTACK2, ACCESS, SIDSE_L_ENV_Attack2

SIDSE_L_ENV_Attack1

	;; temporary shift FSR1 to SID_ENVx_DELAY_CTR_L for easier handling
	movlw	SID_ENVx_DELAY_CTR_L
	addwf	FSR1L, F

	;; delay ENV so long 16bit delay counter != 0
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	bz	SIDSE_L_ENV_Attack1_NoDelay
SIDSE_L_ENV_Attack1_Delay
	;; increment counter, set it to zero on overrun (no delay anymore)
	movlw	SID_Ix_L_ENVx_DELAY
	movff	PLUSW0, IRQ_TMP3; incrementer is the same like used for envelopes
	movlw	0x80		; curve *must* be disabled!
	call	SIDSE_Hlp_ENV_GetBendedValue	; incrementer in MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	addwf	POSTINC1, F
	movf	MIOS_PARAMETER2, W
	addwfc	POSTDEC1, F
	bc	SIDSE_L_ENV_Attack1_DelayOv
	rgoto	SIDSE_L_ENV_End
SIDSE_L_ENV_Attack1_DelayOv
	;; overrun: clear counter to disable delay
	clrf	POSTINC1
	clrf	POSTDEC1
SIDSE_L_ENV_Attack1_NoDelay
	movlw	-SID_ENVx_DELAY_CTR_L	; switch back to SID_ENVx_CTR_L
	addwf	FSR1L, F


	;; in attack1 phase we have to increase the EG counter
	;; attack rate in IRQ_TMP3
	movlw	SID_Ix_L_ENVx_ATTACK1
	movf	PLUSW0, W
	movwf	IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	SID_Ix_L_ENVx_ATT_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final attack level in IRQ_TMP1
	movlw	SID_Ix_L_ENVx_ATTLVL
	movff	PLUSW0, IRQ_TMP1
	;; increment counter
	call	SIDSE_Hlp_ENV_IncrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_L_ENV_Cont

	;; switch to next phase
	movlw	SID_ENV_STATE_ATTACK1
	rcall	SIDSE_L_ENV_SetPhase

	rgoto	SIDSE_L_ENV_Cont


SIDSE_L_ENV_Attack2
	;; in attack1 phase we have to increase the EG counter
	;; attack rate in IRQ_TMP3
	movlw	SID_Ix_L_ENVx_ATTACK2
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	SID_Ix_L_ENVx_ATT_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; final attack level is 0xff
	movlw	0xff
	movwf	IRQ_TMP1
	;; increment counter
	call	SIDSE_Hlp_ENV_IncrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_L_ENV_Cont

	;; switch to next phase
	movlw	SID_ENV_STATE_ATTACK2
	rcall	SIDSE_L_ENV_SetPhase

	rgoto	SIDSE_L_ENV_Cont


SIDSE_L_ENV_Decay1
	;; in decay1 phase we have to increase/decrease the EG counter depending on the current level
	;; decay1 rate in IRQ_TMP3
	movlw	SID_Ix_L_ENVx_DECAY1
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	SID_Ix_L_ENVx_DEC_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final decay level in IRQ_TMP1
	movlw	SID_Ix_L_ENVx_DECLVL
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	call	SIDSE_Hlp_ENV_IncrDecrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_L_ENV_Cont

	;; switch to next phase
	movlw	SID_ENV_STATE_DECAY1
	rcall	SIDSE_L_ENV_SetPhase

	rgoto	SIDSE_L_ENV_Cont


SIDSE_L_ENV_Decay2
	;; in decay2 phase we have to increase/decrease the EG counter depending on the current level
	;; decay2 rate in WREG
	movlw	SID_Ix_L_ENVx_DECAY2
	movff	PLUSW0, IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	SID_Ix_L_ENVx_DEC_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final sustain in IRQ_TMP1
	movlw	SID_Ix_L_ENVx_SUSTAIN
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	call	SIDSE_Hlp_ENV_IncrDecrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_L_ENV_Cont

	;; propagate sustain phase to trigger matrix (using FSR2 as working pointer)
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_E1S_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	POSTINC2, W
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; switch to next phase
	movlw	SID_ENV_STATE_DECAY2
	rcall	SIDSE_L_ENV_SetPhase

	movlw	SID_ENVx_STATE
	BRA_IFCLR PLUSW1, SID_ENV_STATE_SUSTAIN, ACCESS, SIDSE_L_ENV_Cont

SIDSE_L_ENV_Sustain
	;; write sustain value into counter (again)
	movlw	SID_Ix_L_ENVx_SUSTAIN
	movff	PLUSW0, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	;; loop in this phase
	movlw	SID_ENV_STATE_SUSTAIN
	rcall	SIDSE_L_ENV_SetPhase

	rgoto	SIDSE_L_ENV_Cont

	
SIDSE_L_ENV_Release1
	;; in release phase we have to decrease the EG counter until it reaches the sustain level
	;; release rate in WREG
	movlw	SID_Ix_L_ENVx_RELEASE1
	movff	PLUSW0, IRQ_TMP3
	;; get release rate depending on curve setting
	movlw	SID_Ix_L_ENVx_REL_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store release level in IRQ_TMP1
	movlw	SID_Ix_L_ENVx_RELLVL
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	call	SIDSE_Hlp_ENV_IncrDecrCtr

	;; we've reached the level once the zero bit set
	bnz	SIDSE_L_ENV_Cont

	;; switch to next phase
	movlw	SID_ENV_STATE_RELEASE1
	rcall	SIDSE_L_ENV_SetPhase

	rgoto	SIDSE_L_ENV_Cont

SIDSE_L_ENV_Release2
	;; in the final release phase we have to decrease the EG counter until it reaches 0
	;; release rate in WREG
	movlw	SID_Ix_L_ENVx_RELEASE2
	movff	PLUSW0, IRQ_TMP3
	;; get release rate depending on curve setting
	movlw	SID_Ix_L_ENVx_REL_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; target level: 0
	clrf	IRQ_TMP1
	;; decr counter
	call	SIDSE_Hlp_ENV_DecrCtr

	;; switch to next phase (if required)
	movlw	SID_ENV_STATE_RELEASE2
	rcall	SIDSE_L_ENV_SetPhase

	;; 	rgoto	SIDSE_L_ENV_Cont

SIDSE_L_ENV_Cont
	;; copy MIOS_PARAMETER[12] to SID_ENVx_CTR_[LH]
	movff	MIOS_PARAMETER1, POSTINC1
	movff	MIOS_PARAMETER2, POSTDEC1

	clrc
	rrf	MIOS_PARAMETER2, W
	movwf	MUL_A_H, BANKED
	rrf	MIOS_PARAMETER1, W
	movwf	MUL_A_L, BANKED

	;; skip ENV mapping if result is zero (write zero directly into registers)
	movf	MIOS_PARAMETER1, W
	iorwf	MIOS_PARAMETER2, W
	bz	SIDSE_L_ENV_Cont_Zero

	;; scale by ENV depth (skip if depth is zero)
	movlw	SID_Ix_L_ENVx_DEPTH
	movf	PLUSW0, W
	xorlw	0x80
	bz	SIDSE_L_ENV_Cont_Zero
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[12], uses IRQ_TMP1
	rgoto	SIDSE_L_ENV_Cont_NotZero

SIDSE_L_ENV_Cont_Zero
	clrf	MUL_R_1, BANKED	; result is zero - optimized
	clrf	MUL_R_2, BANKED

SIDSE_L_ENV_Cont_NotZero
	;; finally transfer result to SID_MOD_SRC_L_ENVx_VALUE_[LH]
	;; range: +/- 0x7fff
	lfsr	FSR2, SID_MOD_SRC_L_ENV1_L
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addwf	FSR2L, F
	movff	MUL_R_1, POSTINC2
	movff	MUL_R_2, POSTDEC2

SIDSE_L_ENV_End
	return


;; --------------------------------------------------------------------------
;; This function handles the Modulation Matrix
;; IN: pointer to SID_Ix_MODx_BASE in FSR0 (patch record)
;;     MOD number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_MOD
	;; skip if depth is zero
	movlw	SID_Ix_MODx_DEPTH
	movf	PLUSW0, W
	xorlw	0x80
	skpnz
	rgoto	SIDSE_L_MOD_End

	;; copy first source to SID_SE_MOD_SRC1_[LH]
	movlw	SID_Ix_MODx_SRC1
	movf	PLUSW0, W
	bz	SIDSE_L_MOD_Src1_Disabled
	BRA_IFSET WREG, 7, ACCESS, SIDSE_L_MOD_Src1_Const
SIDSE_L_MOD_Src1
	lfsr	FSR2, SID_MOD_SRC_BASE
	addlw	-1
	rlf	WREG, W
	andlw	0xfe
	addlw	1
	addwf	FSR2L, F
	clrc
	rrf	POSTDEC2, W
	movwf	SID_SE_MOD_SRC1_H, BANKED
	rrf	INDF2, W
	movwf	SID_SE_MOD_SRC1_L, BANKED
	btfsc	SID_SE_MOD_SRC1_H, 6, BANKED
	bsf	SID_SE_MOD_SRC1_H, 7, BANKED
	;; range: +/- 0x3fff
	rgoto	SIDSE_L_MOD_Src1_Cont

SIDSE_L_MOD_Src1_Const
	;; range 0x00..0x7f -> +0x0000..0x03f80
	rrf	WREG, W
	andlw	0x3f
	movwf	SID_SE_MOD_SRC1_H, BANKED
	clrf	SID_SE_MOD_SRC1_L, BANKED
	rrf	SID_SE_MOD_SRC1_L, F, BANKED
	rgoto	SIDSE_L_MOD_Src1_Cont
	
SIDSE_L_MOD_Src1_Disabled
	clrf	SID_SE_MOD_SRC1_L, BANKED
	clrf	SID_SE_MOD_SRC1_H, BANKED
	;; 	rgoto	SIDSE_L_MOD_Src1_Cont
SIDSE_L_MOD_Src1_Cont

	;; copy second source to SID_SE_MOD_SRC2_[LH]
	movlw	SID_Ix_MODx_SRC2
	movf	PLUSW0, W
	bz	SIDSE_L_MOD_Src2_Disabled
	BRA_IFSET WREG, 7, ACCESS, SIDSE_L_MOD_Src2_Const
SIDSE_L_MOD_Src2
	lfsr	FSR2, SID_MOD_SRC_BASE
	addlw	-1
	rlf	WREG, W
	andlw	0xfe
	addlw	1
	addwf	FSR2L, F
	clrc
	rrf	POSTDEC2, W
	movwf	SID_SE_MOD_SRC2_H, BANKED
	rrf	INDF2, W
	movwf	SID_SE_MOD_SRC2_L, BANKED
	btfsc	SID_SE_MOD_SRC2_H, 6, BANKED
	bsf	SID_SE_MOD_SRC2_H, 7, BANKED
	;; range: +/- 0x3fff
	rgoto	SIDSE_L_MOD_Src2_Cont

SIDSE_L_MOD_Src2_Const
	;; range 0x00..0x7f -> +0x0000..0x03f80
	rrf	WREG, W
	andlw	0x3f
	movwf	SID_SE_MOD_SRC2_H, BANKED
	clrf	SID_SE_MOD_SRC2_L, BANKED
	rrf	SID_SE_MOD_SRC2_L, F, BANKED
	rgoto	SIDSE_L_MOD_Src2_Cont
	
SIDSE_L_MOD_Src2_Disabled
	clrf	SID_SE_MOD_SRC2_L, BANKED
	clrf	SID_SE_MOD_SRC2_H, BANKED
	;; 	rgoto	SIDSE_L_MOD_Src2_Cont
SIDSE_L_MOD_Src2_Cont

	;; apply selected operation
	movlw	SID_Ix_MODx_OP
	movf	PLUSW0, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SIDSE_L_MOD_OP_0
	rgoto	SIDSE_L_MOD_OP_1
	rgoto	SIDSE_L_MOD_OP_2
	rgoto	SIDSE_L_MOD_OP_3
	rgoto	SIDSE_L_MOD_OP_4
	rgoto	SIDSE_L_MOD_OP_5
	rgoto	SIDSE_L_MOD_OP_6
	rgoto	SIDSE_L_MOD_OP_7
	rgoto	SIDSE_L_MOD_OP_8
	rgoto	SIDSE_L_MOD_OP_9
	rgoto	SIDSE_L_MOD_OP_10
	rgoto	SIDSE_L_MOD_OP_11
	rgoto	SIDSE_L_MOD_OP_12
	rgoto	SIDSE_L_MOD_OP_13
	rgoto	SIDSE_L_MOD_OP_14
	rgoto	SIDSE_L_MOD_OP_15

SIDSE_L_MOD_OP_0	; disabled
	clrf	MUL_A_L, BANKED
	clrf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont
	
SIDSE_L_MOD_OP_1	; SRC1 only
	movff	SID_SE_MOD_SRC1_L, MUL_A_L
	movff	SID_SE_MOD_SRC1_H, MUL_A_H
	rgoto	SIDSE_L_MOD_OP_Cont
	
SIDSE_L_MOD_OP_2	; SRC2 only
	movff	SID_SE_MOD_SRC2_L, MUL_A_L
	movff	SID_SE_MOD_SRC2_H, MUL_A_H
	rgoto	SIDSE_L_MOD_OP_Cont
	
SIDSE_L_MOD_OP_3	; SRC1+SRC2
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	addwf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	addwfc	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_4	; SRC1-SRC2
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	subwf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	subwfb	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_5	; SRC1*SRC2
	clrc
	rlf	SID_SE_MOD_SRC1_L, W, BANKED
	movwf	MUL_A_L, BANKED
	rlf	SID_SE_MOD_SRC1_H, W, BANKED
	movwf	MUL_A_H, BANKED

	clrc
	rlf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_B_L, BANKED
	rlf	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_B_H, BANKED
	
	call	MATH_MUL16_16_SIGNED

	movff	MUL_R_2, MUL_A_L
	movff	MUL_R_3, MUL_A_H
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_6	; XOR
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	xorwf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	xorwf	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_7	; OR
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	iorwf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	iorwf	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_8	; AND
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	andwf	SID_SE_MOD_SRC2_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	andwf	SID_SE_MOD_SRC2_H, W, BANKED
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_9	; Min
	;; check if SRC1 < SRC2
SIDSE_L_MOD_OP_9_CmpH
	movf	SID_SE_MOD_SRC2_H, W, BANKED	; signed value comparison
	xorlw	0x80
	movwf	IRQ_TMP1
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	xorlw	0x80
	subwf	IRQ_TMP1, W
	bnz	SIDSE_L_MOD_OP_9_SkipCmpL
SIDSE_L_MOD_OP_9_CmpL
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	subwf	SID_SE_MOD_SRC2_L, W, BANKED
SIDSE_L_MOD_OP_9_SkipCmpL
	bc	SIDSE_L_MOD_OP_1	; SRC1 is less -> SRC1 only
	rgoto	SIDSE_L_MOD_OP_2	; SRC2 is less -> SRC2 only

SIDSE_L_MOD_OP_10	; Max
	;; check if SRC1 > SRC2
SIDSE_L_MOD_OP_10_CmpH
	movf	SID_SE_MOD_SRC2_H, W, BANKED	; signed value comparison
	xorlw	0x80
	movwf	IRQ_TMP1
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	xorlw	0x80
	subwf	IRQ_TMP1, W
	bnz	SIDSE_L_MOD_OP_10_SkipCmpL
SIDSE_L_MOD_OP_10_CmpL
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	subwf	SID_SE_MOD_SRC2_L, W, BANKED
SIDSE_L_MOD_OP_10_SkipCmpL
	bc	SIDSE_L_MOD_OP_2	; SRC2 is greater -> SRC2 only
	rgoto	SIDSE_L_MOD_OP_1	; SRC1 is greater -> SRC1 only

SIDSE_L_MOD_OP_11
	;; check if SRC1 < SRC2
SIDSE_L_MOD_OP_11_CmpH
	movf	SID_SE_MOD_SRC2_H, W, BANKED	; signed value comparison
	xorlw	0x80
	movwf	IRQ_TMP1
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	xorlw	0x80
	subwf	IRQ_TMP1, W
	bnz	SIDSE_L_MOD_OP_11_SkipCmpL
SIDSE_L_MOD_OP_11_CmpL
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	subwf	SID_SE_MOD_SRC2_L, W, BANKED
SIDSE_L_MOD_OP_11_SkipCmpL
	bc	SIDSE_L_MOD_OP_Cont_High	; SRC1 is less -> high value
	rgoto	SIDSE_L_MOD_OP_Cont_Low	; SRC2 is less -> low value

SIDSE_L_MOD_OP_12
	;; check if SRC1 > SRC2
SIDSE_L_MOD_OP_12_CmpH
	movf	SID_SE_MOD_SRC2_H, W, BANKED	; signed value comparison
	xorlw	0x80
	movwf	IRQ_TMP1
	movf	SID_SE_MOD_SRC1_H, W, BANKED
	xorlw	0x80
	subwf	IRQ_TMP1, W
	bnz	SIDSE_L_MOD_OP_12_SkipCmpL
SIDSE_L_MOD_OP_12_CmpL
	movf	SID_SE_MOD_SRC1_L, W, BANKED
	subwf	SID_SE_MOD_SRC2_L, W, BANKED
SIDSE_L_MOD_OP_12_SkipCmpL
	bc	SIDSE_L_MOD_OP_Cont_Low	; SRC1 is less -> low value
	rgoto	SIDSE_L_MOD_OP_Cont_High	; SRC2 is less -> high value

SIDSE_L_MOD_OP_13
	;; check if SRC1 = SRC2
	;; (only high bytes are checked for lower granularity)
	movf	SID_SE_MOD_SRC2_H, W, BANKED
	xorwf	SID_SE_MOD_SRC1_H, W, BANKED
	bz	SIDSE_L_MOD_OP_Cont_High	; high bytes equal -> high value
	rgoto	SIDSE_L_MOD_OP_Cont_Low	; high bytes not equal -> low value

SIDSE_L_MOD_OP_14
	;; S&H - SRC1 will be sampled whenever SRC2 changes from a negative to a positive value

	;; determine OR/AND mask for MOD element -> IRQ_TMP1
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	IRQ_TMP1

	;; determine if SRC2 is negative or positive - store NEG flag at correct bit position in IRQ_TMP2
	btfss	SID_SE_MOD_SRC2_H, 7, BANKED
	movlw	0x00
	movwf	IRQ_TMP2

	;; check for 0 transition, if not: forward old value (HOLD)
	movf	SID_SE_MOD_TRANSITION, W, BANKED
	andwf	IRQ_TMP1, W
	xorwf	IRQ_TMP2, W
	bz	SIDSE_L_MOD_OP_14_Hold

SIDSE_L_MOD_OP_14_Sample
	;; store new NEG flag
	comf	IRQ_TMP1, W
	andwf	SID_SE_MOD_TRANSITION, F, BANKED
	movf	IRQ_TMP2, W
	iorwf	SID_SE_MOD_TRANSITION, F, BANKED

	;; still hold when we are in negative range now
	;; (only sample on negative->positive transition)
	movf	IRQ_TMP2, W
	bnz	SIDSE_L_MOD_OP_14_Hold

	;; take new SRC1 value
	rgoto	SIDSE_L_MOD_OP_1

SIDSE_L_MOD_OP_14_Hold
	;; take old MOD value
	lfsr	FSR2, SID_MOD_SRC_BASE
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	SID_MOD_SRC_L_MOD1_L-SID_MOD_SRC_BASE
	addwf	FSR2L, F
	movff	POSTINC2, MUL_A_L
	movff	POSTINC2, MUL_A_H

	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_15	; reserved
	rgoto	SIDSE_L_MOD_OP_0


SIDSE_L_MOD_OP_Cont_High		; constant high value
	setf	MUL_A_L, BANKED
	movlw	0x7f
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_Cont_Low		; constant low value
	clrf	MUL_A_L, BANKED
#if 0
	movlw	0x80
	movwf	MUL_A_H, BANKED
#else
	clrf	MUL_A_H, BANKED	; TK: 0 instead of full negative value is more useful
#endif
	;; 	rgoto	SIDSE_L_MOD_OP_Cont

SIDSE_L_MOD_OP_Cont

	;; store in modulator source array for feedbacks
	;; use value w/o depth, this has two advantages:
	;; - maximum resolution when forwarding the data value
	;; - original MOD value can be taken for sample&hold feature
	;; but it has also a disadvantage:
	;; - the user could think it is a bug when depth doesn't affect the feedback MOD value...
	lfsr	FSR2, SID_MOD_SRC_BASE
	clrc
	rlf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	SID_MOD_SRC_L_MOD1_L-SID_MOD_SRC_BASE
	addwf	FSR2L, F
	movff	MUL_A_L, POSTINC2
	movff	MUL_A_H, POSTINC2

	;; skip if modulation result is zero
	movf	MUL_A_L, W, BANKED
	iorwf	MUL_A_H, W, BANKED
	skpnz
	rgoto	SIDSE_L_MOD_End

	;; scale by MOD depth		; (zero check already at top of MOD function)
	movlw	SID_Ix_MODx_DEPTH
	movf	PLUSW0, W
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[12], uses IRQ_TMP1

	;; store in METER_VALUES array for optional visualisation on the modulation matrix
#if DEFAULT_LEDMATRIX_MODE != 2
	;; not if sammichSID display mode is selected
	movff	MUL_R_2, MIOS_PARAMETER1
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	CS_MENU_MATRIX_MeterSet
	;; uses FSR1 !!!
#endif
	
	;; multiply by two (to allow full range sweeps)
	rlf	MUL_R_1, F, BANKED
	rlf	MUL_R_2, F, BANKED
	rlf	MUL_R_3, F, BANKED

	;; copy to MOD result registers
	movff	MUL_R_1, SID_SE_MOD_RES1_L
	movff	MUL_R_2, SID_SE_MOD_RES1_H
	movff	MUL_R_3, SID_SE_MOD_RES1_U
	movff	MUL_R_1, SID_SE_MOD_RES2_L
	movff	MUL_R_2, SID_SE_MOD_RES2_H
	movff	MUL_R_3, SID_SE_MOD_RES2_U

	;; invert result1 if requested
	movlw	SID_Ix_MODx_OP
	BRA_IFCLR PLUSW0, 6, ACCESS, SIDSE_L_MOD_OP_Res1_NoInv
SIDSE_L_MOD_OP_Res1_Inv
	comf	SID_SE_MOD_RES1_L, F, BANKED
	comf	SID_SE_MOD_RES1_H, F, BANKED
	comf	SID_SE_MOD_RES1_U, F, BANKED
	incf	SID_SE_MOD_RES1_L, F, BANKED
	skpnz
	incf	SID_SE_MOD_RES1_H, F, BANKED
	skpnz
	incf	SID_SE_MOD_RES1_U, F, BANKED
SIDSE_L_MOD_OP_Res1_NoInv

	;; invert result2 if requested
	movlw	SID_Ix_MODx_OP
	BRA_IFCLR PLUSW0, 7, ACCESS, SIDSE_L_MOD_OP_Res2_NoInv
SIDSE_L_MOD_OP_Res2_Inv
	comf	SID_SE_MOD_RES2_L, F, BANKED
	comf	SID_SE_MOD_RES2_H, F, BANKED
	comf	SID_SE_MOD_RES2_U, F, BANKED
	incf	SID_SE_MOD_RES2_L, F, BANKED
	skpnz
	incf	SID_SE_MOD_RES2_H, F, BANKED
	skpnz
	incf	SID_SE_MOD_RES2_U, F, BANKED
SIDSE_L_MOD_OP_Res2_NoInv

	;; add result to modulation target array
	movlw	SID_Ix_MODx_TARG_X1
	movf	PLUSW0, W
	bz	SIDSE_L_MOD_Targ1_Disabled
SIDSE_L_MOD_Targ1
	lfsr	FSR2, SID_MOD_TARG_BASE
	addlw	-1
	mullw	3
	lfsr	FSR2, SID_MOD_TARG_BASE
	movf	PRODL, W
	andlw	0x7f		; avoid access outside target array!
	addwf	FSR2L, F
	rcall	SIDSE_L_MOD_Hlp_AddTarget1
SIDSE_L_MOD_Targ1_Disabled

	;; add result to modulation target array
	movlw	SID_Ix_MODx_TARG_X2
	movf	PLUSW0, W
	bz	SIDSE_L_MOD_Targ2_Disabled
SIDSE_L_MOD_Targ2
	addlw	-1
	mullw	3
	lfsr	FSR2, SID_MOD_TARG_BASE
	movf	PRODL, W
	andlw	0x7f		; avoid access outside target array!
	addwf	FSR2L, F
	rcall	SIDSE_L_MOD_Hlp_AddTarget2
SIDSE_L_MOD_Targ2_Disabled

	;; add to additional SIDL/R targets

SIDSE_L_MOD_COPYTARG_MACRO MACRO func, SID_MOD_TARG_x, flag
	LOCAL	SIDSE_L_MOD_COPYTARG_MACRO_Skip
	BRA_IFCLR IRQ_TMP1, flag, ACCESS, SIDSE_L_MOD_COPYTARG_MACRO_Skip
	lfsr	FSR2, SID_MOD_TARG_x
	rcall	func
SIDSE_L_MOD_COPYTARG_MACRO_Skip
	ENDM


	movlw	SID_Ix_MODx_TARG_L
	movff	PLUSW0, IRQ_TMP1
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PITCH1_L, 0
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PITCH2_L, 1
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PITCH3_L, 2
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PW1_L, 3
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PW2_L, 4
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_PW3_L, 5
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_FIL1_L, 6
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget1, SID_MOD_TARG_VOL1_L, 7

	movlw	SID_Ix_MODx_TARG_R
	movff	PLUSW0, IRQ_TMP1
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PITCH4_L, 0
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PITCH5_L, 1
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PITCH6_L, 2
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PW4_L, 3
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PW5_L, 4
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_PW6_L, 5
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_FIL2_L, 6
	SIDSE_L_MOD_COPYTARG_MACRO SIDSE_L_MOD_Hlp_AddTarget2, SID_MOD_TARG_VOL2_L, 7

SIDSE_L_MOD_End
	return

;; help function which is used to add the result to a specific target (pointer in FSR2)
SIDSE_L_MOD_Hlp_AddTarget1
	movf	SID_SE_MOD_RES1_L, W, BANKED
	addwf	POSTINC2, F
	movf	SID_SE_MOD_RES1_H, W, BANKED
	addwfc	POSTINC2, F
	movf	SID_SE_MOD_RES1_U, W, BANKED
	addwfc	POSTINC2, F
	return

SIDSE_L_MOD_Hlp_AddTarget2
	movf	SID_SE_MOD_RES2_L, W, BANKED
	addwf	POSTINC2, F
	movf	SID_SE_MOD_RES2_H, W, BANKED
	addwfc	POSTINC2, F
	movf	SID_SE_MOD_RES2_U, W, BANKED
	addwfc	POSTINC2, F
	return


;; --------------------------------------------------------------------------
;; Help Function for SIDSE_L_ENV for switching to a new phase
;; Takes also the loop point into account
;; IN:  pointer to SID_Ix_L_ENVx_BASE in FSR0 (patch record)
;;      pointer to SID_ENVx_BASE in FSR1
;;      current loop point in WREG (corresponds to SID_ENV_STATE_XXX flag)
;; OUT:	changed SID_ENVx_STATE
;; --------------------------------------------------------------------------
SIDSE_L_ENV_SetPhase
	movwf	PRODL		; temporary store phase number in PRODL and PRODH
	movwf	PRODH		; (for sustain check)

	;; check for loop point
	movlw	SID_Ix_L_ENVx_MODE
	swapf	PLUSW0, W
	andlw	0x07
	addlw	-1
	xorwf	PRODL, W	; check with current phase
	bnz	SIDSE_L_ENV_SetPhase_NoLoop
SIDSE_L_ENV_SetPhase_Loop
	movlw	SID_Ix_L_ENVx_MODE
	movf	PLUSW0, W
	andlw	0x07
	bz	SIDSE_L_ENV_SetPhase_NoLoop
	addlw	-2
	movwf	PRODL
SIDSE_L_ENV_SetPhase_NoLoop

	;; if we are in sustain, and there is no loop point in the sustain phase, don't switch to next phase
	movf	PRODH, W
	xorlw	SID_ENV_STATE_SUSTAIN
	bnz	SIDSE_L_ENV_SetPhase_NoSustain
	movf	PRODL, W
	xorlw	SID_ENV_STATE_SUSTAIN
	bz	SIDSE_L_ENV_SetPhase_Sustain
SIDSE_L_ENV_SetPhase_NoSustain
	;; switch to next phase
	incf	PRODL, F
SIDSE_L_ENV_SetPhase_Sustain

	;; branch depending on new phase
	BRA_IFSET PRODL, 2, ACCESS, SIDSE_L_ENV_SetPhase_4567
SIDSE_L_ENV_SetPhase_0123
	BRA_IFSET PRODL, 1, ACCESS, SIDSE_L_ENV_SetPhase_23
SIDSE_L_ENV_SetPhase_01
	movlw	0x01
	btfsc	PRODL, 0
	movlw 0x03
	rgoto	SIDSE_L_ENV_SetPhase_Cont
SIDSE_L_ENV_SetPhase_23
	movlw	0x07
	btfsc	PRODL, 0
	movlw 0x0f
	rgoto	SIDSE_L_ENV_SetPhase_Cont

SIDSE_L_ENV_SetPhase_4567
	BRA_IFSET PRODL, 1, ACCESS, SIDSE_L_ENV_SetPhase_67
SIDSE_L_ENV_SetPhase_45
	movlw	0x1f
	btfsc	PRODL, 0
	movlw 0x3f
	rgoto	SIDSE_L_ENV_SetPhase_Cont
SIDSE_L_ENV_SetPhase_67
	movlw	0x7f
	;; 	rgoto	SIDSE_L_ENV_SetPhase_Cont
SIDSE_L_ENV_SetPhase_Cont
	movwf	PRODL
	movlw	SID_ENVx_STATE
	movff	PRODL, PLUSW1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_L_ENV_Restart
;;  DESCRIPTION: help function which restarts a single ENV
;;  IN: SID_PATCH_BUFFER_SHADOW + SID_Ix_ENV[123456]_BASE in FSR0
;;      SID_ENV[123456]_BASE in FSR1
;; --------------------------------------------------------------------------
SIDSE_L_ENV_Restart
	movlw	(1 << SID_ENV_STATE_ATTACK1)	; start at attack phase
	movwf	PRODL
	movlw	SID_ENVx_STATE
	movff	PRODL, PLUSW1

	;; check if ENV should be delayed - set delay counter to 0x0001 in this case, else to 0x0000
	movlw	SID_ENVx_DELAY_CTR_H	; high byte always reset to 0
	clrf	PLUSW1

	movlw	SID_Ix_L_ENVx_DELAY
	clrf	PRODL			; low byte set to 1 if delay != 0
	movf	PLUSW0, W
	skpz
	incf	PRODL, F
	movlw	SID_ENVx_DELAY_CTR_L
	movff	PRODL, PLUSW1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_L_ENV_TrgRelease
;;  DESCRIPTION: help function which starts release phase of a single ENV
;;  IN: SID_ENV[123456]_BASE in FSR1
;; --------------------------------------------------------------------------
SIDSE_L_ENV_TrgRelease
	movlw	SID_ENVx_STATE
	bsf	PLUSW1, SID_ENV_STATE_RELEASE1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_L_Note_Restart
;;  DESCRIPTION: help function which restarts the delay counter for voices
;;               and requests an active gate
;;  IN: patch base in FSR0, voice base in FSR1, osc number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_L_NOTE_Restart
	;; request gate if voice is active (and request clear for proper ADSR handling)
	;; set gate only if voice is active!
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	btfsc	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; check if voice should be delayed - set delay counter to 0x0001 in this case, else to 0x0000
	movlw	SID_Vx_SET_DELAY_CTR_H	; high byte always reset to 0
	clrf	PLUSW1

	movlw	SID_Ix_Vx_DELAY
	clrf	PRODL			; low byte set to 1 if delay != 0
	movf	PLUSW0, W
	skpz
	incf	PRODL, F

	;; delay also if ABW (ADSR bug workaround) option active
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFCLR WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_L_NOTE_Restart_NoABW
SIDSE_L_NOTE_Restart_ABW
	movlw	0x01
	movwf	PRODL

	;; clear ADSR registers, so that the envelope gets completely released
	call	SIDSE_Hlp_GetSIDFrqPtr
	movlw	SIDx_V1_ENV_AD
	clrf	PLUSW2
	movlw	SIDx_V1_ENV_SR
	clrf	PLUSW2
SIDSE_L_NOTE_Restart_NoABW

	movlw	SID_Vx_SET_DELAY_CTR_L
	movff	PRODL, PLUSW1
	return



;; --------------------------------------------------------------------------
;; This function updates the LED matrix
;; For lead engine, this is interrupt driven for fastest update time
;; only used when master is selected
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_L_CS_LM

	;; if slave is selected: exit (update done in CS_MENU_LED_Update_ModMatrix)
	btfss	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; branch depending on normal/meter mode
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, SIDSE_L_CS_LM_Meter
SIDSE_L_CS_LM_Normal

	;; matrix is updated in CS_MENU_LED_Update_ModMatrix
	;; no special code required here
	return

SIDSE_L_CS_LM_Meter

#if DEFAULT_LEDMATRIX_MODE != 2
	;; meter values are updated in SIDSE_L_MOD_OP_Cont
	;; no special code required here
	return
#else
	;; use same function as for bassline engine
	goto	SIDSE_L_CS_LM_Meter_ReUse
#endif
