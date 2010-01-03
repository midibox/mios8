; $Id$
;
; MIDIbox SID
; Bassline Engine
;  
; Re-uses LFO/ENV/Note/Ext handler of Multi Engine
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
;;  Bassline Engine handler called by SIDSE_Handler (sid_se.inc)
;; --------------------------------------------------------------------------
SIDSE_B_Handler
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; branch depending on cycle
	BRA_IFSET SID_STAT, SID_STAT_SE_CYCLE, ACCESS, SIDSE_B_Handler_Cycle2

SIDSE_B_Handler_Cycle1
	bsf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the second cycle

	;; clear "allocated" notification for external AOUTs - flags will be set if X2A function active
	clrf	SID_SE_EXT_ALLOCATED, BANKED

	;; clear modulation targets
	movlw	((SID_MOD_TARG_CLEAR_END-SID_MOD_TARG_CLEAR_BEGIN)+1) & 0xff
	movwf	IRQ_TMP1
	lfsr	FSR0, SID_MOD_TARG_CLEAR_BEGIN
SIDSE_B_Handler_ModClearLoop
	clrf	POSTINC0
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_B_Handler_ModClearLoop


	;; ------------------------------------------------------------------
	;; 1st Cycle: LFOs
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_LFO_MACRO MACRO patch_base, lfo_base, voice_assignment
	lfsr	FSR0, patch_base
	lfsr	FSR1, lfo_base
	movlw	voice_assignment
	movwf	SID_CURRENT_VOICE_ASSG
	call	SIDSE_M_LFO	; re-used from multi engine!
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	;; TODO note: LFO could also use the accent flag? Maybe a separate option?

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_LFO_L
	SIDSE_B_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_LFO1_MODE, SID_LFO1_BASE, 0x01
	SIDSE_B_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_LFO2_MODE, SID_LFO2_BASE, 0x01

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_LFO_NotR
	movlw	3*2
	movwf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_LFO_R
	SIDSE_B_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_LFO1_MODE, SID_LFO7_BASE, 0x02
	SIDSE_B_HANDLER_LFO_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_LFO2_MODE, SID_LFO8_BASE, 0x02
SIDSE_B_Handler_LFO_NotR

	;; ------------------------------------------------------------------
	;; 1st Cycle: Envelopes
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_ENV_MACRO MACRO patch_base, voice_base, env_base
	;; copy accent flag from voice to SID_SE_STATE.SID_SE_STATE_ACCENT
	bcf	SID_SE_STATE, SID_SE_STATE_ACCENT, BANKED
	movff	voice_base + SID_Vx_STATE, WREG
	andlw	(1 << SID_V_STATE_ACCENT)
	skpz
	bsf	SID_SE_STATE, SID_SE_STATE_ACCENT, BANKED

	lfsr	FSR0, patch_base
	lfsr	FSR1, env_base
	call	SIDSE_M_ENV	; re-used from multi engine!
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_B_HANDLER_ENV_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE, SID_ENV1_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_ENV_NotR
	movlw	3
	movwf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_ENV_R
	SIDSE_B_HANDLER_ENV_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE, SID_ENV4_BASE
SIDSE_B_Handler_ENV_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: LED Matrix Update
	;; ------------------------------------------------------------------
	call	SIDSE_B_CS_LM


	rgoto	SIDSE_B_Handler_End


SIDSE_B_Handler_Cycle2
	bcf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the first cycle

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Clock Handler
	;; ------------------------------------------------------------------
	call	SIDSE_Clk


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Sequencer
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_SEQ_MACRO MACRO patch_base, voice_base, seq_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	lfsr	FSR2, seq_base
	rcall	SIDSE_B_SEQ
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_B_HANDLER_SEQ_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE, SID_SEQ1_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_SEQ_NotR
	SIDSE_B_HANDLER_SEQ_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE, SID_SEQ2_BASE
SIDSE_B_Handler_SEQ_NotR

	;; clear all WT/SEQ requests
	clrf	SID_SE_TRG_EVNT_U, BANKED


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Arp Handler
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_ARP_MACRO MACRO patch_base, voice_base, midi_voice_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	lfsr	FSR2, midi_voice_base
	call	SIDSE_Arp
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_Arp_L
	SIDSE_B_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE, SID_MV1_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_Arp_NotR
	movlw	3
	movwf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_Arp_R
	SIDSE_B_HANDLER_ARP_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE, SID_MV2_BASE
SIDSE_B_Handler_Arp_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: check ENV/LFO/Note Sync requests
	;; ------------------------------------------------------------------
	call	SIDSE_B_Sync


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Voices
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_NOTE_MACRO MACRO patch_base, voice_base
	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_B_Note
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_Note_L
	;; transfer note values and state flags from OSC1 (master) to OSC2 (slave)
	movff	SIDL_V1_BASE + SID_Vx_STATE,       SIDL_V2_BASE + SID_Vx_STATE
	movff	SIDL_V1_BASE + SID_Vx_ASSIGNED_MV, SIDL_V2_BASE + SID_Vx_ASSIGNED_MV
	movff	SIDL_V1_BASE + SID_Vx_NOTE,        SIDL_V2_BASE + SID_Vx_NOTE
	movff	SIDL_V1_BASE + SID_Vx_PLAYED_NOTE, SIDL_V2_BASE + SID_Vx_PLAYED_NOTE

	;; transfer note values and state flags from OSC1 (master) to OSC3 (slave)
	movff	SIDL_V1_BASE + SID_Vx_STATE,       SIDL_V3_BASE + SID_Vx_STATE
	movff	SIDL_V1_BASE + SID_Vx_ASSIGNED_MV, SIDL_V3_BASE + SID_Vx_ASSIGNED_MV
	movff	SIDL_V1_BASE + SID_Vx_NOTE,        SIDL_V3_BASE + SID_Vx_NOTE
	movff	SIDL_V1_BASE + SID_Vx_PLAYED_NOTE, SIDL_V3_BASE + SID_Vx_PLAYED_NOTE

	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE
	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V2_BASE
	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_Note_NotR
	movlw	3
	movwf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_Handler_Note_R
	;; transfer note values and state flags from OSC1 (master) to OSC2 (slave)
	movff	SIDR_V1_BASE + SID_Vx_STATE,       SIDR_V2_BASE + SID_Vx_STATE
	movff	SIDR_V1_BASE + SID_Vx_ASSIGNED_MV, SIDR_V2_BASE + SID_Vx_ASSIGNED_MV
	movff	SIDR_V1_BASE + SID_Vx_NOTE,        SIDR_V2_BASE + SID_Vx_NOTE
	movff	SIDR_V1_BASE + SID_Vx_PLAYED_NOTE, SIDR_V2_BASE + SID_Vx_PLAYED_NOTE

	;; transfer note values and state flags from OSC1 (master) to OSC3 (slave)
	movff	SIDR_V1_BASE + SID_Vx_STATE,       SIDR_V3_BASE + SID_Vx_STATE
	movff	SIDR_V1_BASE + SID_Vx_ASSIGNED_MV, SIDR_V3_BASE + SID_Vx_ASSIGNED_MV
	movff	SIDR_V1_BASE + SID_Vx_NOTE,        SIDR_V3_BASE + SID_Vx_NOTE
	movff	SIDR_V1_BASE + SID_Vx_PLAYED_NOTE, SIDR_V3_BASE + SID_Vx_PLAYED_NOTE

	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE
	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V2_BASE
	SIDSE_B_HANDLER_NOTE_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V3_BASE
SIDSE_B_Handler_Note_NotR

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Filter
	;; ------------------------------------------------------------------

SIDSE_B_Handler_Filter_L
	clrf	SID_SE_ELEMENT_NUM, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL1_L
	lfsr	FSR2, SIDL_BASE
	call	SIDSE_Filter

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_Handler_Filter_NotR
SIDSE_B_Handler_Filter_R
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL2_L
	lfsr	FSR2, SIDR_BASE
	call	SIDSE_Filter
SIDSE_B_Handler_Filter_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: AOUTs (External/Extensions)
	;; ------------------------------------------------------------------
SIDSE_B_HANDLER_EXT_MACRO MACRO patch_base
	lfsr	FSR0, patch_base
	call	SIDSE_BDM_EXT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR1_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR2_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR3_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR4_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR5_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR6_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR7_L
	SIDSE_B_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR8_L


	;; ------------------------------------------------------------------
	;; 2nd Cycle: External Switches
	;; ------------------------------------------------------------------
	call	SIDSE_EXT_Switches

	
	;; ------------------------------------------------------------------
	;; call temporary routine which updates static SID registers
	;; ------------------------------------------------------------------

	call	SIDSE_B_UpdateStatRegs


	;; ------------------------------------------------------------------
	;; clear FA/FB and FC synch request
	;; ------------------------------------------------------------------
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED

SIDSE_B_Handler_End
	return


;; --------------------------------------------------------------------------
;;  Updates static SID registers
;;  (temporary)
;; --------------------------------------------------------------------------
SIDSE_B_UpdateStatRegs
SIDSE_B_UPDATESTAT_MACRO	MACRO patch_base, voice_base, sid_base, swinsid_mode, swinsid_phase
	LOCAL	SIDSE_B_UpdateStatRegs_NoADSR
	LOCAL	SIDSE_B_UpdateStatRegs_NoAcc
	LOCAL	SIDSE_B_UpdateStatRegs_V2Enabled
	LOCAL	SIDSE_B_UpdateStatRegs_V2Cont
	LOCAL	SIDSE_B_UpdateStatRegs_V3Enabled
	LOCAL	SIDSE_B_UpdateStatRegs_V3Cont

	lfsr	FSR0, patch_base
	lfsr	FSR1, voice_base

	;; voice 1 oscillator
	movlw	0x09
	andwf	sid_base + SIDx_V1_CTRL, F, BANKED
	movlw	SID_Ix_Vx_WAVEFORM
	swapf	PLUSW0, W
	andlw	0xf6
#if DEFAULT_ENABLE_UPPER_WAVEFORMS == 0
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw	0x8f
#endif
	iorwf	sid_base + SIDx_V1_CTRL, F, BANKED

	;; voice 2 oscillator has own waveform and voice enable flag
	;; no gate/sync if waveform disabled
	movlw	SID_Ix_B_V2_WAVEFORM
	movf	PLUSW0, W
	andlw	0x0f
	bnz	SIDSE_B_UpdateStatRegs_V2Enabled
	movlw	0x10		; (select triangle waveform to keep the oscillator silent)
	movwf	sid_base + SIDx_V2_CTRL, BANKED
	rgoto	SIDSE_B_UpdateStatRegs_V2Cont
SIDSE_B_UpdateStatRegs_V2Enabled
	movlw	0x09
	andwf	sid_base + SIDx_V2_CTRL, F, BANKED
	movlw	SID_Ix_B_V2_WAVEFORM
	swapf	PLUSW0, W
	andlw	0xf6
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw	0x8f 
	iorwf	sid_base + SIDx_V2_CTRL, F, BANKED
SIDSE_B_UpdateStatRegs_V2Cont

	;; voice 3 oscillator has own waveform and voice enable flag
	movlw	SID_Ix_B_V3_WAVEFORM
	movf	PLUSW0, W
	andlw	0x0f
	bnz	SIDSE_B_UpdateStatRegs_V3Enabled
	movlw	0x10		; (select triangle waveform to keep the oscillator silent)
	movwf	sid_base + SIDx_V3_CTRL, BANKED
	rgoto	SIDSE_B_UpdateStatRegs_V3Cont
SIDSE_B_UpdateStatRegs_V3Enabled
	movlw	0x09
	andwf	sid_base + SIDx_V3_CTRL, F, BANKED
	movlw	SID_Ix_B_V3_WAVEFORM
	swapf	PLUSW0, W
	andlw	0xf6
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw	0x8f 
	iorwf	sid_base + SIDx_V3_CTRL, F, BANKED
SIDSE_B_UpdateStatRegs_V3Cont
	
	;; if ABW (ADSR bug workaround) function active, update ADSR register only when the gate is set
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFSET WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_B_UpdateStatRegs_NoADSR
	movlw	SID_Ix_Vx_AD
	movff	PLUSW0, sid_base + SIDx_V1_ENV_AD	
	movff	PLUSW0, sid_base + SIDx_V2_ENV_AD
	movff	PLUSW0, sid_base + SIDx_V3_ENV_AD
	movlw	SID_Ix_Vx_SR
	movff	PLUSW0, sid_base + SIDx_V1_ENV_SR
	movff	PLUSW0, sid_base + SIDx_V2_ENV_SR
	movff	PLUSW0, sid_base + SIDx_V3_ENV_SR

	;; accent handling: force sustain to maximum if accent active
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_ACCENT, ACCESS, SIDSE_B_UpdateStatRegs_NoAcc
	movlw	0xf0
	iorwf	sid_base + SIDx_V1_ENV_SR, F, BANKED
	iorwf	sid_base + SIDx_V2_ENV_SR, F, BANKED
	iorwf	sid_base + SIDx_V3_ENV_SR, F, BANKED
SIDSE_B_UpdateStatRegs_NoAcc
SIDSE_B_UpdateStatRegs_NoADSR
#if DEFAULT_ENABLE_SWINSID
	movlw	SID_Ix_Vx_SWINSID_MODE
	movff	PLUSW0, swinsid_mode
	movlw	SID_Ix_Vx_SWINSID_PHASE
	movff	PLUSW0, swinsid_phase
#endif	
	ENDM

	SET_BSR	SIDL_BASE

	SIDSE_B_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE, SIDL_BASE + SIDx_V1_FRQ_L, SIDL_BASE + SIDx_SWINSID_V1_MODE, SIDL_BASE + SIDx_SWINSID_V1_PHASE

	;; in Mono mode: copy *ALL* SID registers (not only the static ones) over to SIDR
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_UpdateStatRegs_Mono
SIDSE_B_UpdateStatRegs_Stereo
	;; stereo mode:	most SIDs registers already updated, copy over the static ones
	SIDSE_B_UPDATESTAT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE, SIDR_BASE + SIDx_V1_FRQ_L, SIDR_BASE + SIDx_SWINSID_V1_MODE, SIDR_BASE + SIDx_SWINSID_V1_PHASE
	rgoto	SIDSE_B_UpdateStatRegs_End


SIDSE_B_UpdateStatRegs_Mono
	;; mono mode: copy over *all* SID registers of SIDL to SIDR
	lfsr	FSR1, SIDL_BASE
	lfsr	FSR2, SIDR_BASE
#if DEFAULT_ENABLE_SWINSID
	movlw	0x20
#else
	movlw	SIDx_MODE_VOL+1
#endif
	movwf	PRODL		; (loop counter)
SIDSE_B_UpdateStatRegs_Mono_Loop
	movff	POSTINC1, POSTINC2
	decfsz	PRODL, F
	rgoto	SIDSE_B_UpdateStatRegs_Mono_Loop

SIDSE_B_UpdateStatRegs_End
	SET_BSR	SID_BASE
	return


;; --------------------------------------------------------------------------
;; This function handles the notes (gate/pitch/PW)
;; IN: pointer to SID_Ix_e_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_B_Note
	call	SIDSE_Gate	; gate handler
	skpz			; returns ZERO flag cleared if pitch should not be changed
   	call	SIDSE_Pitch	; pitch handler
	goto	SIDSE_PW	; pulsewidth handler


;; --------------------------------------------------------------------------
;; This function handles the sequences
;; IN: pointer to SID_Ix_B_SxVy_BASE in FSR0 (patch record)
;;     pointer to SID_Vx_BASE in FSR1 (voice record)
;;     pointer to SID_SEQx_BASE in FSR2 (seq record)
;;     SEQ number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_B_SEQ
	;; clear all gates and deselect sequence if MIDI clock stop requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED, SIDSE_B_SEQ_NoClkStop
SIDSE_B_SEQ_ClkStop
	;; stop sequence
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	;; clear gates
	rgoto	SIDSE_B_SEQ_Off_ClrGate
SIDSE_B_SEQ_NoClkStop

	;; exit if WTO mode not set
	;; clear gate if WTO just has been disabled (for proper transitions)
	movlw	SID_Ix_B_Vx_FLAGS2
	BRA_IFSET PLUSW0, SID_I_B_V_FLAGS2_WT_ONLY, ACCESS, SIDSE_B_SEQ_On
SIDSE_B_SEQ_Off
	;; check if sequencer was disabled before
	movlw	SID_SEQx_MISC
	btfss	PLUSW2, SID_SEQ_MISC_SEQ_ON
	return
	bcf	PLUSW2, SID_SEQ_MISC_SEQ_ON
	
SIDSE_B_SEQ_Off_ClrGate
	;; clear gate and exit
	;; request gate clear
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	return


SIDSE_B_SEQ_On

	;; exit if arp is enabled
	movlw	SID_Ix_Vx_ARP_MODE
	btfsc	PLUSW0, SID_I_V_ARP_MODE_ENABLE
	return

	;; clear temporary SEQ flags
	bcf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED

	;; check if SEQ reset requested or FA event or sequencer was disabled before (transition Seq off->on)
	movlw	SID_SEQx_MISC
	BRA_IFCLR PLUSW2, SID_SEQ_MISC_SEQ_ON, ACCESS, SIDSE_B_SEQ_DivReset

	;; check for FA event
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_B_SEQ_DivReset

	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_TRG_EVNT_U, W, BANKED
	bz	SIDSE_B_SEQ_NoDivReset
SIDSE_B_SEQ_DivReset
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
	movlw	SID_Ix_B_Vx_SEQ_NUM
	swapf	PLUSW0, W
	andlw	0x70
	addlw	-1
	movwf	PRODL
	movlw	SID_SEQx_POS
	movff	PRODL, PLUSW2

	;; ensure that slide flag is cleared
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_SLIDE
SIDSE_B_SEQ_NoDivReset

	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED, SIDSE_B_SEQ_ClkCont
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_B_SEQ_NoClkStart
SIDSE_B_SEQ_ClkCont
SIDSE_B_SEQ_ClkStart
	;; enable voice
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
SIDSE_B_SEQ_NoClkStart

	;; memorize sequencer on flag
	movlw	SID_SEQx_MISC
	bsf	PLUSW2, SID_SEQ_MISC_SEQ_ON

	;; check for clock sync event
	BRA_IFCLR SID_SE_TRG_EVNT_U, 7, BANKED, SIDSE_B_SEQ_NoClk
SIDSE_B_SEQ_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value
	movlw	SID_SEQx_DIV_CTR
	incf	PLUSW2, W
	movwf	PRODL
	bz	SIDSE_B_SEQ_Clk_Ov

	;; reset once max value is reached
	movlw	SID_Ix_B_Vx_SEQ_SPEED
	movf	PLUSW0, W
	andlw	0x3f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_B_SEQ_Clk_NoOv
SIDSE_B_SEQ_Clk_Ov
	clrf	PRODL
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_B_SEQ_Clk_NoOv
	;; transfer new divider value into SEQx register
	movlw	SID_SEQx_DIV_CTR
	movff	PRODL, PLUSW2
SIDSE_B_SEQ_NoClk

	;; skip if no new step requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED, SIDSE_B_SEQ_End

SIDSE_B_SEQ_NextStep
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
	bz	SIDSE_B_SEQ_NextStep_GateSet
	xorlw	4
	skpnz
	rgoto	SIDSE_B_SEQ_NextStep_GateClr
	movlw	6-1
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_B_SEQ_NextStep_End

SIDSE_B_SEQ_NextStep_GateSet
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
	bz	SIDSE_B_SEQ_NextStep_NoSync16
	movlw	SID_Ix_B_Vx_SEQ_SPEED
	BRA_IFCLR PLUSW0, SID_I_V_SEQ_SYNC16, ACCESS, SIDSE_B_SEQ_NextStep_NoSync16
SIDSE_B_SEQ_NextStep_Sync16
	movlw	SID_SEQx_POS
	movf	PLUSW2, W
	rgoto	SIDSE_B_SEQ_NextStep_Sync16_Cont
SIDSE_B_SEQ_NextStep_NoSync16
	movlw	SID_Ix_B_Vx_SEQ_NUM
	swapf	PLUSW0, W
SIDSE_B_SEQ_NextStep_Sync16_Cont
	andlw	0x70
	iorwf	PRODL, F


	movf	PRODL, W
	andlw	0x70
	movwf	PRODH
	movlw	SID_Ix_B_Vx_SEQ_LENGTH
	movf	PLUSW0, W
	andlw	0x0f
	iorwf	PRODH, W
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_B_SEQ_NextStep_NoOv
SIDSE_B_SEQ_NextStep_Ov
	movlw	0x70
	andwf	PRODH, W
	movwf	PRODL
SIDSE_B_SEQ_NextStep_NoOv
	;; transfer back to position counter
	movlw	SID_SEQx_POS
	movff	PRODL, PLUSW2

	;; "play" the step
SIDSE_B_SEQ_PlayStep

	;; gate off (without slide) if invalid song number (stop feature - seq >= 8)
	movlw	SID_Ix_B_Vx_SEQ_NUM
	movf	PLUSW0, W
	andlw	0xf8
	bz	SIDSE_B_SEQ_PlayStep_NoStop
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SIDSE_B_SEQ_NextStep_GateClrForc;ed
	rgoto	SIDSE_B_SEQ_NextStep_End
SIDSE_B_SEQ_PlayStep_NoStop

	;; get pointer to note/par value and store it in MIOS_PARAMETER[12]
	movff	FSR1L, PRODL	; temporary using FSR1 for this
	movff	FSR1H, PRODH
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_SEQUENCES
	movlw	SID_SEQx_POS
	movf	PLUSW2, W
	addwf	FSR1L, F
	movff	INDF1, MIOS_PARAMETER1
	movlw	0x80
	addwf	FSR1L, F
	movff	INDF1, MIOS_PARAMETER2
	movff	PRODL, FSR1L	; restore FSR1
	movff	PRODH, FSR1H

	;; determine note
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	addlw	0x3c

	;; add octave
	;; coding of MIOS_PARAMETER1[5:4]
	;; 0=-, 1=Up, 2=Dwn, 3=Up2
	BRA_IFSET MIOS_PARAMETER1, 4, ACCESS, SIDSE_B_SEQ_PlayStep_OctUp
	btfsc	MIOS_PARAMETER1, 5; Dwn
	addlw -0x0c	
	rgoto	SIDSE_B_SEQ_PlayStep_Oct_Cont
SIDSE_B_SEQ_PlayStep_OctUp
	addlw 0x0c				; Up
	btfsc	MIOS_PARAMETER1, 5; Up2
	addlw 0x0c	
SIDSE_B_SEQ_PlayStep_Oct_Cont
	movwf	PRODL

	;; transfer to SID_Vx_NOTE
	movlw	SID_Vx_NOTE
	movff	PRODL, PLUSW1

	;; set accent and LFO sync flag
	;; ignore if slide has been set by previous step
	;; (important for SID sustain: transition from sustain < 0xf to 0xf will reset the VCA)
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_SLIDE, ACCESS, SIDSE_B_SEQ_PlayStep_Sld
SIDSE_B_SEQ_PlayStep_NoSld
	bcf	PLUSW1, SID_V_STATE_ACCENT
	btfsc	MIOS_PARAMETER2, 7
	bsf	PLUSW1, SID_V_STATE_ACCENT

	;; request LFO sync (will be gated by SID_I_LFO_MODE_SYNC_M flag in SIDSE_M_SYNC)
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	rlf	PRODL, W
	andlw	0xfe
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED

	clrc
	rlf	WREG, W
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
SIDSE_B_SEQ_PlayStep_Sld

	;; activate portamento if slide has been set by previous step
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
	btfsc	PLUSW1, SID_V_STATE_SLIDE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE

	;; set slide flag of current step
	;;  	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_SLIDE
	btfsc	MIOS_PARAMETER1, 6
	bsf	PLUSW1, SID_V_STATE_SLIDE

	;; no gate request if gate flag not set
	BRA_IFCLR MIOS_PARAMETER1, 7, ACCESS, SIDSE_B_SEQ_PlayStep_NoOn
	
	;; gate set request if voice is active and gate not active
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SIDSE_B_SEQ_PlayStep_NoOn
SIDSE_B_SEQ_PlayStep_NoVAChk
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_CLR_REQ	; ensure that gate won't be cleared by previous CLR_REQ
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SIDSE_B_SEQ_PlayStep_NoOn
SIDSE_B_SEQ_PlayStep_On
	;; set gate
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; only ENV attack and LFO sync
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVA, F, BANKED

	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	addlw	2
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	;; TODO: optional LFO resync!
SIDSE_B_SEQ_PlayStep_NoOn

	;; parameter track:
	;; parameter already in MIOS_PARAMETER2!
	;; transfer to MIOS_PARAMETER1, add 0x80 for absolute values
	movf	MIOS_PARAMETER2, W
	addlw	0x80
	movwf	MIOS_PARAMETER1

	;; store SIDs which should be modified in MIOS_PARAMETER3[1:0], and instrument in MIOS_PARAMETER[5:4]
	movlw	0x11
	movf	SID_SE_ELEMENT_NUM, F, BANKED
	skpz
	movlw	0x22
	movwf	MIOS_PARAMETER3
	;; call WT Parameter Handler
	movlw	SID_Ix_B_Vx_SEQ_ASSGN
	movf	PLUSW0, W
	call	SID_PARIN_SetWT

	rgoto	SIDSE_B_SEQ_NextStep_End


SIDSE_B_SEQ_NextStep_GateClr
	;; don't clear gate if slide flag set
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_SLIDE, ACCESS, SIDSE_B_SEQ_NextStep_End

SIDSE_B_SEQ_NextStep_GateClrForc;ed 
	;; request gate clear
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; ENV release phase
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED

	;; 	rgoto	SIDSE_B_SEQ_NextStep_End

SIDSE_B_SEQ_NextStep_End
SIDSE_B_SEQ_End
	return



;; --------------------------------------------------------------------------
;; This function syncs the LFOs/ENVs and Notes
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_B_Sync

SIDSE_B_SYNC_ENV_MACRO	MACRO	evnt_a, evnt_a_flag, evnt_r, evnt_r_flag, env_base
	lfsr	FSR1, env_base
	CALL_IFSET evnt_r, evnt_r_flag, BANKED, SIDSE_M_ENV_TrgRelease
	bcf	evnt_r, evnt_r_flag, BANKED
	CALL_IFSET evnt_a, evnt_a_flag, BANKED, SIDSE_M_ENV_Restart
	bcf	evnt_a, evnt_a_flag, BANKED
	ENDM

	SIDSE_B_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 0, SID_SE_TRG_EVNT_ENVR, 0, SID_ENV1_BASE
	SIDSE_B_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 3, SID_SE_TRG_EVNT_ENVR, 3, SID_ENV4_BASE


SIDSE_B_SYNC_LFO_MACRO	MACRO	evnt, evnt_flag, patch_base, lfo1_base, lfo2_base
	LOCAL	SIDSE_B_SYNC_LFO_Skip
	LOCAL	SIDSE_B_SYNC_LFO1_Skip
	LOCAL	SIDSE_B_SYNC_LFO2_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_B_SYNC_LFO_Skip
	bcf	evnt, evnt_flag, BANKED

	lfsr	FSR0, patch_base + SID_Ix_M_Vx_LFO1_MODE
	BRA_IFCLR INDF0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_B_SYNC_LFO1_Skip
	lfsr	FSR1, lfo1_base
	call	SIDSE_M_LFO_Restart	; re-used from multi engine
SIDSE_B_SYNC_LFO1_Skip

	incf	SID_SE_ELEMENT_NUM, F, BANKED

	lfsr	FSR0, patch_base + SID_Ix_M_Vx_LFO2_MODE
	BRA_IFCLR INDF0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_B_SYNC_LFO2_Skip
	lfsr	FSR1, lfo2_base
	call	SIDSE_M_LFO_Restart
SIDSE_B_SYNC_LFO2_Skip

SIDSE_B_SYNC_LFO_Skip
	ENDM

SIDSE_B_SYNC_LFO_L
	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_B_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L1, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SID_LFO1_BASE, SID_LFO2_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_SYNC_LFO_NotR
SIDSE_B_SYNC_LFO_R
	movlw	3*2
	movwf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_B_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L4, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SID_LFO7_BASE, SID_LFO8_BASE
SIDSE_B_SYNC_LFO_NotR


SIDSE_B_SYNC_NOTE_MACRO	MACRO	evnt, evnt_flag, patch_base, voice_base
	LOCAL	SIDSE_B_SYNC_NOTE_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_B_SYNC_NOTE_Skip
	lfsr	FSR1, voice_base
	lfsr	FSR0, patch_base
	call	SIDSE_M_NOTE_Restart	; re-used from multi engine
	bcf	evnt, evnt_flag, BANKED
SIDSE_B_SYNC_NOTE_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_B_SYNC_Note_L
	SIDSE_B_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1L, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE, SIDL_V1_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_B_SYNC_Note_NotR
SIDSE_B_SYNC_Note_R
	SIDSE_B_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1R, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE, SIDR_V1_BASE
SIDSE_B_SYNC_Note_NotR

	return



;; --------------------------------------------------------------------------
;; This function updates the LED matrix
;; For basslines, this is interrupt driven for fastest update time
;; only used when master is selected
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_B_CS_LM
	;; if slave is selected: exit (update done in CS_MENU_LED_Update_ModMatrix)
	btfss	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; branch depending on normal/meter mode
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, SIDSE_B_CS_LM_Meter
SIDSE_B_CS_LM_Normal

	;; don't activate level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; clear matrix
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	movlw	8
	movwf	IRQ_TMP1
SIDSE_B_CS_LM_Normal_ClearLoop
	clrf	POSTINC1
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_B_CS_LM_Normal_ClearLoop

	;; set sequencer position of left channel in horizontal line if sequencer enabled and valid pos
	SET_BSR	SID_PATCH_BUFFER_SHADOW
	BRA_IFCLR SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_FLAGS2, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, SIDSE_B_CS_LM_Normal_L_Dis
	movf	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	SIDSE_B_CS_LM_Normal_L_Dis
SIDSE_B_CS_LM_Normal_L
	SET_BSR	SID_SEQ1_BASE
	movf	SID_SEQ1_BASE + SID_SEQx_POS, W, BANKED
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	IRQ_TMP2
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN

	;; check if voice is active
	movff	SIDL_V1_BASE + SID_Vx_STATE, WREG
	andlw	(1 << SID_V_STATE_VOICE_ACTIVE)
	bnz	SIDSE_B_CS_LM_Normal_L_Act
SIDSE_B_CS_LM_Normal_L_NAct
	;; static bar
	movlw	8
	movwf	IRQ_TMP1
SIDSE_B_CS_LM_Normal_L_Loop
	movff	IRQ_TMP2, POSTINC1
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_B_CS_LM_Normal_L_Loop
	rgoto	SIDSE_B_CS_LM_Normal_L_End

SIDSE_B_CS_LM_Normal_L_Act 
#if 1	; disable here if you don't like this effect
	;; skip on gate off time for rythmic display
	movf	SID_SEQ1_BASE + SID_SEQx_MISC, W, BANKED
	andlw	0x07
	movwf	IRQ_TMP1
	movlw	5
	cpfslt	IRQ_TMP1, ACCESS
	rgoto SIDSE_B_CS_LM_Normal_L_Dis
#endif

	;; only print to at seq position (16 steps -> 8 LEDs :-/)
	rrf	SID_SEQ1_BASE + SID_SEQx_POS, W, BANKED
	andlw	0x07
	addwf	FSR1L, F
	movff	IRQ_TMP2, INDF1
SIDSE_B_CS_LM_Normal_L_End
SIDSE_B_CS_LM_Normal_L_Dis


	;; set sequencer position of left channel in vertical line
	SET_BSR	SID_PATCH_BUFFER_SHADOW
	BRA_IFCLR SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_FLAGS2, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, SIDSE_B_CS_LM_Normal_R_Dis
	movf	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM, W, BANKED
	andlw	0xf8
	bnz	SIDSE_B_CS_LM_Normal_R_Dis
SIDSE_B_CS_LM_Normal_R
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	SET_BSR	SID_SEQ2_BASE
	swapf	SID_SEQ2_BASE + SID_SEQx_POS, W, BANKED
	andlw	0x07
	addwf	FSR1L, F

	;; check if voice is active
	movff	SIDR_V1_BASE + SID_Vx_STATE, WREG
	andlw	(1 << SID_V_STATE_VOICE_ACTIVE)
	bnz	SIDSE_B_CS_LM_Normal_R_Act
SIDSE_B_CS_LM_Normal_R_NAct
	;; static bar
	setf	INDF1
	rgoto	SIDSE_B_CS_LM_Normal_R_End

SIDSE_B_CS_LM_Normal_R_Act
#if 1	; disable here if you don't like this effect
	;; skip on gate off time for rythmic display
	movf	SID_SEQ2_BASE + SID_SEQx_MISC, W, BANKED
	andlw	0x07
	movwf	IRQ_TMP1
	movlw	5
	cpfslt	IRQ_TMP1, ACCESS
	rgoto SIDSE_B_CS_LM_Normal_R_Dis
#endif

	rrf	SID_SEQ2_BASE + SID_SEQx_POS, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
SIDSE_B_CS_LM_Normal_R_End
SIDSE_B_CS_LM_Normal_R_Dis

	SET_BSR	SID_BASE		
	return

SIDSE_B_CS_LM_Meter

	;; also used by lead engine in sammichSID display mode!
SIDSE_L_CS_LM_Meter_ReUse

	;; also used by multi engine!
SIDSE_M_CS_LM_Meter_ReUse

	;; don't activate level meters
	bcf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; show frequencies
SID_SE_B_METER_MACRO MACRO meter, Vx_STATE, Vx_LINEAR_FRQ_H, METER_VALUESx
	LOCAL	NoGate
	LOCAL	GateEnd

	movff	Vx_STATE, WREG
	BRA_IFCLR WREG, SID_V_STATE_GATE_ACTIVE, ACCESS, NoGate
	movff	Vx_LINEAR_FRQ_H, MIOS_PARAMETER1
	movlw	meter
	call	CS_MENU_MATRIX_MeterLinearSet
	rgoto	GateEnd
NoGate
	SET_BSR	METER_VALUESx
	clrf	METER_VALUESx, BANKED
	SET_BSR	SID_BASE	
GateEnd
	ENDM

	SID_SE_B_METER_MACRO 1, SIDL_V1_BASE + SID_Vx_STATE, SIDL_V1_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES1
	SID_SE_B_METER_MACRO 2, SIDL_V2_BASE + SID_Vx_STATE, SIDL_V2_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES2
	SID_SE_B_METER_MACRO 3, SIDL_V3_BASE + SID_Vx_STATE, SIDL_V3_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES3
	SID_SE_B_METER_MACRO 4, SIDR_V1_BASE + SID_Vx_STATE, SIDR_V1_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES4
	SID_SE_B_METER_MACRO 5, SIDR_V2_BASE + SID_Vx_STATE, SIDR_V2_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES5
	SID_SE_B_METER_MACRO 6, SIDR_V3_BASE + SID_Vx_STATE, SIDR_V3_BASE + SID_Vx_LINEAR_FRQ_H, METER_VALUES6

	;; clear remaining meter values
	SET_BSR	METER_VALUES0
	clrf	METER_VALUES0, BANKED
	clrf	METER_VALUES7, BANKED
	SET_BSR	SID_BASE
	return
