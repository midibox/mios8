; $Id$
;
; MIDIbox SID
; Multi Engine
;  
; Note: Bassline engine re-uses LFO/ENV/Note/Ext handler of Multi Engine!
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
;;  Multi Engine handler called by SIDSE_Handler (sid_se.inc)
;; --------------------------------------------------------------------------
SIDSE_M_Handler
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; branch depending on cycle
	BRA_IFSET SID_STAT, SID_STAT_SE_CYCLE, ACCESS, SIDSE_M_Handler_Cycle2

SIDSE_M_Handler_Cycle1
	bsf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the second cycle

	;; clear "allocated" notification for external AOUTs - flags will be set if X2A function active
	clrf	SID_SE_EXT_ALLOCATED, BANKED

	;; clear modulation targets
	movlw	((SID_MOD_TARG_CLEAR_END-SID_MOD_TARG_CLEAR_BEGIN)+1) & 0xff
	movwf	IRQ_TMP1
	lfsr	FSR0, SID_MOD_TARG_CLEAR_BEGIN
SIDSE_M_Handler_ModClearLoop
	clrf	POSTINC0
	decfsz	IRQ_TMP1, F
	rgoto	SIDSE_M_Handler_ModClearLoop


	;; ------------------------------------------------------------------
	;; 1st Cycle: LFOs
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_LFO_MACRO MACRO voice_base, patch_lfo_offset, lfo_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr
	movlw	SID_Ix_M_Vx_VOICE_ASSGN
	movff	PLUSW0, SID_CURRENT_VOICE_ASSG
	movlw	patch_lfo_offset
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	lfsr	FSR1, lfo_base
	call	SIDSE_M_LFO
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_M_Handler_LFO_L
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V1_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO1_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V1_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO2_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V2_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO3_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V2_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO4_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V3_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO5_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDL_V3_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO6_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_Handler_LFO_NotR
SIDSE_M_Handler_LFO_R
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V1_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO7_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V1_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO8_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V2_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO9_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V2_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO10_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V3_BASE, SID_Ix_M_Vx_LFO1_MODE, SID_LFO11_BASE
	SIDSE_M_HANDLER_LFO_MACRO SIDR_V3_BASE, SID_Ix_M_Vx_LFO2_MODE, SID_LFO12_BASE
SIDSE_M_Handler_LFO_NotR

	;; ------------------------------------------------------------------
	;; 1st Cycle: Envelopes
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_ENV_MACRO MACRO voice_base, env_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr
	lfsr	FSR1, env_base
	call	SIDSE_M_ENV
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	;; accent feature not used by multi engine
	bcf	SID_SE_STATE, SID_SE_STATE_ACCENT, BANKED

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_M_HANDLER_ENV_MACRO SIDL_V1_BASE, SID_ENV1_BASE
	SIDSE_M_HANDLER_ENV_MACRO SIDL_V2_BASE, SID_ENV2_BASE
	SIDSE_M_HANDLER_ENV_MACRO SIDL_V3_BASE, SID_ENV3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_Handler_ENV_NotR
SIDSE_M_Handler_ENV_R
	SIDSE_M_HANDLER_ENV_MACRO SIDR_V1_BASE, SID_ENV4_BASE
	SIDSE_M_HANDLER_ENV_MACRO SIDR_V2_BASE, SID_ENV5_BASE
	SIDSE_M_HANDLER_ENV_MACRO SIDR_V3_BASE, SID_ENV6_BASE
SIDSE_M_Handler_ENV_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: LED Matrix Update
	;; ------------------------------------------------------------------
	call	SIDSE_M_CS_LM

	rgoto	SIDSE_M_Handler_End


SIDSE_M_Handler_Cycle2
	bcf	SID_STAT, SID_STAT_SE_CYCLE	; on next handler call we want to process the first cycle

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Clock Handler
	;; ------------------------------------------------------------------
	call	SIDSE_Clk


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Wavetable Handler
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_WT_MACRO MACRO patch_base, wt_base
	lfsr	FSR0, patch_base
	lfsr	FSR2, wt_base
	rcall	SIDSE_M_WT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I1_BASE, SID_WT1_BASE
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I2_BASE, SID_WT2_BASE
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I3_BASE, SID_WT3_BASE
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I4_BASE, SID_WT4_BASE
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I5_BASE, SID_WT5_BASE
	SIDSE_M_HANDLER_WT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I6_BASE, SID_WT6_BASE

	;; clear all WT requests
	clrf	SID_SE_TRG_EVNT_U, BANKED


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Arp Handler
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_ARP_MACRO MACRO midi_voice_base
	lfsr	FSR2, midi_voice_base
	rcall	SIDSE_M_Hlp_GetAssignedVoicePtr
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr
	call	SIDSE_Arp
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_M_Handler_Arp_L
	SIDSE_M_HANDLER_ARP_MACRO SID_MV1_BASE
	SIDSE_M_HANDLER_ARP_MACRO SID_MV2_BASE
	SIDSE_M_HANDLER_ARP_MACRO SID_MV3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_Handler_Arp_NotR
SIDSE_M_Handler_Arp_R
	SIDSE_M_HANDLER_ARP_MACRO SID_MV4_BASE
	SIDSE_M_HANDLER_ARP_MACRO SID_MV5_BASE
	SIDSE_M_HANDLER_ARP_MACRO SID_MV6_BASE
SIDSE_M_Handler_Arp_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: check ENV/LFO/Note Sync requests
	;; ------------------------------------------------------------------
	call	SIDSE_M_Sync


	;; ------------------------------------------------------------------
	;; 2nd Cycle: Voices
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_NOTE_MACRO MACRO voice_base
	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr
	rcall	SIDSE_M_Note
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_M_Handler_Note_L
	SIDSE_M_HANDLER_NOTE_MACRO SIDL_V1_BASE
	SIDSE_M_HANDLER_NOTE_MACRO SIDL_V2_BASE
	SIDSE_M_HANDLER_NOTE_MACRO SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_Handler_Note_NotR
SIDSE_M_Handler_Note_R
	SIDSE_M_HANDLER_NOTE_MACRO SIDR_V1_BASE
	SIDSE_M_HANDLER_NOTE_MACRO SIDR_V2_BASE
	SIDSE_M_HANDLER_NOTE_MACRO SIDR_V3_BASE
SIDSE_M_Handler_Note_NotR

	;; ------------------------------------------------------------------
	;; 2nd Cycle: Filter
	;; ------------------------------------------------------------------

SIDSE_M_Handler_Filter_L
	clrf	SID_SE_ELEMENT_NUM, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_S1F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL1_L
	lfsr	FSR2, SIDL_BASE
	call	SIDSE_Filter

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_Handler_Filter_NotR
SIDSE_M_Handler_Filter_R
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_S2F_BASE
	lfsr	FSR1, SID_MOD_TARG_FIL2_L
	lfsr	FSR2, SIDR_BASE
	call	SIDSE_Filter
SIDSE_M_Handler_Filter_NotR


	;; ------------------------------------------------------------------
	;; 2nd Cycle: AOUTs (External/Extensions)
	;; ------------------------------------------------------------------
SIDSE_M_HANDLER_EXT_MACRO MACRO patch_base
	lfsr	FSR0, patch_base
	call	SIDSE_BDM_EXT
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR1_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR2_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR3_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR4_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR5_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR6_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR7_L
	SIDSE_M_HANDLER_EXT_MACRO SID_PATCH_BUFFER_SHADOW + SID_Ix_EXT_PAR8_L


	;; ------------------------------------------------------------------
	;; 2nd Cycle: External Switches
	;; ------------------------------------------------------------------
	call	SIDSE_EXT_Switches

	
	;; ------------------------------------------------------------------
	;; call temporary routine which updates static SID registers
	;; ------------------------------------------------------------------

	call	SIDSE_M_UpdateStatRegs


	;; ------------------------------------------------------------------
	;; clear FA/FB and FC synch request
	;; ------------------------------------------------------------------
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED

SIDSE_M_Handler_End
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_M_GetAssignedInsPtr
;;  DESCRIPTION: returns pointer to SID_Ix_M_Iy_BASE depending on assigned
;;  MIDI voice
;;  IN: pointer to SIDx_Vx_BASE in FSR1
;;  OUT: pointer to patch in FSR0
;; --------------------------------------------------------------------------
SIDSE_M_Hlp_GetAssignedInsPtr
	movlw	SID_Vx_ASSIGNED_MV
	movf	PLUSW1, W
	mullw	SID_Ix_M_I2_BASE - SID_Ix_M_I1_BASE
	lfsr	FSR0, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_M_GetAssignedVoicePtr
;;  DESCRIPTION: returns pointer to SID_Vx_BASE depending on assigned voice
;;  IN: pointer to SIDx_MVx_BASE in FSR2
;;  OUT: pointer to voice in FSR1
;; --------------------------------------------------------------------------
SIDSE_M_Hlp_GetAssignedVoicePtr
	movlw	SID_MVx_LAST_VOICE
	movf	PLUSW2, W
	mullw	SID_Vx_RECORD_LEN
	lfsr	FSR1, SIDL_V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	return


;; --------------------------------------------------------------------------
;;  Updates static SID registers
;;  (temporary)
;; --------------------------------------------------------------------------
SIDSE_M_UpdateStatRegs
SIDSE_M_UPDATESTAT_MACRO	MACRO voice_base, sid_base, swinsid_mode, swinsid_phase
	LOCAL	SIDSE_M_UpdateStatRegs_NoADSR

	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr

	movlw	0x09
	andwf	sid_base + SIDx_V1_CTRL, F, BANKED
	movlw	SID_Ix_Vx_WAVEFORM
	swapf	PLUSW0, W
	andlw	0xf6
	btfsc	WREG, 7; ensure that noise will not "lock on" like described in 6581 spec
	andlw 0x8f 
	iorwf	sid_base + SIDx_V1_CTRL, F, BANKED

	;; if ABW (ADSR bug workaround) function active, update ADSR register only when the gate is set
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFSET WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_M_UpdateStatRegs_NoADSR
	movlw	SID_Ix_Vx_AD
	movff	PLUSW0, sid_base + SIDx_V1_ENV_AD	
	movlw	SID_Ix_Vx_SR
	movff	PLUSW0, sid_base + SIDx_V1_ENV_SR
SIDSE_M_UpdateStatRegs_NoADSR
#if ENABLE_SWINSID
	movlw	SID_Ix_Vx_SWINSID_MODE
	movff	PLUSW0, swinsid_mode
	movlw	SID_Ix_Vx_SWINSID_PHASE
	movff	PLUSW0, swinsid_phase
#endif	
	
	ENDM

	SET_BSR	SIDL_BASE

	SIDSE_M_UPDATESTAT_MACRO SIDL_V1_BASE, SIDL_BASE + SIDx_V1_FRQ_L, SIDL_BASE + SIDx_SWINSID_V1_MODE, SIDL_BASE + SIDx_SWINSID_V1_PHASE
	SIDSE_M_UPDATESTAT_MACRO SIDL_V2_BASE, SIDL_BASE + SIDx_V2_FRQ_L, SIDL_BASE + SIDx_SWINSID_V2_MODE, SIDL_BASE + SIDx_SWINSID_V2_PHASE
	SIDSE_M_UPDATESTAT_MACRO SIDL_V3_BASE, SIDL_BASE + SIDx_V3_FRQ_L, SIDL_BASE + SIDx_SWINSID_V3_MODE, SIDL_BASE + SIDx_SWINSID_V3_PHASE

	;; in Mono mode: copy *ALL* SID registers (not only the static ones) over to SIDR
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_UpdateStatRegs_Mono
SIDSE_M_UpdateStatRegs_Stereo
	;; stereo mode:	most SIDs registers already updated, copy over the static ones
	SIDSE_M_UPDATESTAT_MACRO SIDR_V1_BASE, SIDR_BASE + SIDx_V1_FRQ_L, SIDR_BASE + SIDx_SWINSID_V1_MODE, SIDR_BASE + SIDx_SWINSID_V1_PHASE
	SIDSE_M_UPDATESTAT_MACRO SIDR_V2_BASE, SIDR_BASE + SIDx_V2_FRQ_L, SIDR_BASE + SIDx_SWINSID_V2_MODE, SIDR_BASE + SIDx_SWINSID_V2_PHASE
	SIDSE_M_UPDATESTAT_MACRO SIDR_V3_BASE, SIDR_BASE + SIDx_V3_FRQ_L, SIDR_BASE + SIDx_SWINSID_V3_MODE, SIDR_BASE + SIDx_SWINSID_V3_PHASE
	rgoto	SIDSE_M_UpdateStatRegs_End


SIDSE_M_UpdateStatRegs_Mono
	;; mono mode: copy over *all* SID registers of SIDL to SIDR
	lfsr	FSR1, SIDL_BASE
	lfsr	FSR2, SIDR_BASE
#if ENABLE_SWINSID
	movlw	0x20
#else
	movlw	SIDx_MODE_VOL+1
#endif
	movwf	PRODL		; (loop counter)
SIDSE_M_UpdateStatRegs_Mono_Loop
	movff	POSTINC1, POSTINC2
	decfsz	PRODL, F
	rgoto	SIDSE_M_UpdateStatRegs_Mono_Loop

SIDSE_M_UpdateStatRegs_End
	SET_BSR	SID_BASE
	return


;; --------------------------------------------------------------------------
;; This function handles the notes (gate/pitch/PW)
;; IN: pointer to SID_Ix_M_Iy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_M_Note
	call	SIDSE_Gate	; gate handler
	skpz			; returns ZERO flag cleared if pitch should not be changed
   	call	SIDSE_Pitch	; pitch handler
	goto	SIDSE_PW	; pulsewidth handler


;; --------------------------------------------------------------------------
;; This function handles the wavetables
;; IN: pointer to SID_Ix_M_Iy_BASE in FSR0 (patch record)
;;     pointer to SID_WTx_BASE in FSR2 (wt record)
;;     WT number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_M_WT
	;; clear temporary WT flags
	bcf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED

	;; check if WT reset requested
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_TRG_EVNT_U, W, BANKED
	bz	SIDSE_M_WT_NoDivReset
SIDSE_M_WT_DivReset
	;; next clock event will increment div to 0
	movlw	SID_WTx_DIV_CTR
	setf	PLUSW2
	;; next step will increment to start position
	movlw	SID_Ix_M_Vx_WT_BEGIN
	movf	PLUSW0, W
	andlw	0x7f
	addlw	-1
	movwf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_M_WT_NoDivReset

	;; check for clock sync event
	BRA_IFCLR SID_SE_TRG_EVNT_U, 7, BANKED, SIDSE_M_WT_NoClk
SIDSE_M_WT_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value
	movlw	SID_WTx_DIV_CTR
	incf	PLUSW2, W
	movwf	PRODL

	;; reset once max value is reached
	movlw	SID_Ix_M_Vx_WT_SPEED
	movf	PLUSW0, W
	andlw	0x3f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_M_WT_Clk_NoOv
SIDSE_M_WT_Clk_Ov
	clrf	PRODL
	;; request next step
	bsf	SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED
SIDSE_M_WT_Clk_NoOv
	;; transfer new divider value into WTx register
	movlw	SID_WTx_DIV_CTR
	movff	PRODL, PLUSW2
SIDSE_M_WT_NoClk

	;; skip if no new step requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_WT_NEW_STEP_REQ, BANKED, SIDSE_M_WT_End

	;; skip if position is 0xaa (notifies oneshot -> WT stopped)
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	xorlw	0xaa
	bz	SIDSE_M_WT_End

SIDSE_M_WT_NextStep
	;; increment position counter, reset at end position
	movlw	SID_WTx_POS
	incf	PLUSW2, W
	movwf	PRODL
	movlw	SID_Ix_M_Vx_WT_END
	movf	PLUSW0, W
	andlw	0x7f
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_M_WT_NextStep_NoOv
SIDSE_M_WT_NextStep_Ov
	;; if one-shot mode: set position to 0xaa, WT is stopped now!
	movlw	SID_Ix_M_Vx_WT_LOOP
	BRA_IFCLR PLUSW0, 7, ACCESS, SIDSE_M_WT_NextStep_Ov_NoOneShot
SIDSE_M_WT_NextStep_Ov_OneShot
	movlw	0xaa
	movwf	PRODL
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2
	rgoto	SIDSE_M_WT_End

SIDSE_M_WT_NextStep_Ov_NoOneShot
	movlw	SID_Ix_M_Vx_WT_LOOP
	movf	PLUSW0, W
	andlw	0x7f
	movwf	PRODL
SIDSE_M_WT_NextStep_NoOv
	;; transfer back to position counter
	movlw	SID_WTx_POS
	movff	PRODL, PLUSW2

	;; "play" the step
	;; (entry point for SIDSE_M_WT_MOD_...)
SIDSE_M_WT_PlayStep
	;; get pointer to WT value
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_WAVETABLE
	movlw	SID_WTx_POS
	movf	PLUSW2, W
	andlw	0x7f
	addwf	FSR1L, F

	;; store value in MIOS_PARAMETER1
	movff	INDF1, MIOS_PARAMETER1

	;; store SIDs which should be modified in MIOS_PARAMETER3[1:0], and instrument in MIOS_PARAMETER[6:4]
	swapf	SID_SE_ELEMENT_NUM, W, BANKED
	andlw	0xf0
	iorlw	0x03
	movwf	MIOS_PARAMETER3
	;; call WT Parameter Handler
	movlw	SID_Ix_M_Vx_WT_ASSGN
	movf	PLUSW0, W
	call	SID_PARIN_SetWT

SIDSE_M_WT_End
	return


;; --------------------------------------------------------------------------
;; This function syncs the LFOs/ENVs and Notes
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_M_Sync

SIDSE_M_SYNC_ENV_MACRO	MACRO	evnt_a, evnt_a_flag, evnt_r, evnt_r_flag, env_base
	lfsr	FSR1, env_base
	CALL_IFSET evnt_r, evnt_r_flag, BANKED, SIDSE_M_ENV_TrgRelease
	bcf	evnt_r, evnt_r_flag, BANKED
	CALL_IFSET evnt_a, evnt_a_flag, BANKED, SIDSE_M_ENV_Restart
	bcf	evnt_a, evnt_a_flag, BANKED
	ENDM

	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 0, SID_SE_TRG_EVNT_ENVR, 0, SID_ENV1_BASE
	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 1, SID_SE_TRG_EVNT_ENVR, 1, SID_ENV2_BASE
	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 2, SID_SE_TRG_EVNT_ENVR, 2, SID_ENV3_BASE
	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 3, SID_SE_TRG_EVNT_ENVR, 3, SID_ENV4_BASE
	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 4, SID_SE_TRG_EVNT_ENVR, 4, SID_ENV5_BASE
	SIDSE_M_SYNC_ENV_MACRO SID_SE_TRG_EVNT_ENVA, 5, SID_SE_TRG_EVNT_ENVR, 5, SID_ENV6_BASE


SIDSE_M_SYNC_LFO_MACRO	MACRO	evnt, evnt_flag, voice_base, lfo1_base, lfo2_base
	LOCAL	SIDSE_M_SYNC_LFO_Skip
	LOCAL	SIDSE_M_SYNC_LFO1_Skip
	LOCAL	SIDSE_M_SYNC_LFO2_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_M_SYNC_LFO_Skip
	bcf	evnt, evnt_flag, BANKED

	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr

	movlw	SID_Ix_M_Vx_LFO1_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_M_SYNC_LFO1_Skip
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	lfsr	FSR1, lfo1_base
	call	SIDSE_M_LFO_Restart
SIDSE_M_SYNC_LFO1_Skip

	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr

	movlw	SID_Ix_M_Vx_LFO2_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_SYNC_M, ACCESS, SIDSE_M_SYNC_LFO2_Skip
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	lfsr	FSR1, lfo2_base
	call	SIDSE_M_LFO_Restart
SIDSE_M_SYNC_LFO2_Skip

SIDSE_M_SYNC_LFO_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_M_SYNC_LFO_NotL
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L1, SIDL_V1_BASE, SID_LFO1_BASE, SID_LFO2_BASE
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L2, SIDL_V2_BASE, SID_LFO3_BASE, SID_LFO4_BASE
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L3, SIDL_V3_BASE, SID_LFO5_BASE, SID_LFO6_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_SYNC_LFO_NotR
SIDSE_M_SYNC_LFO_R
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L4, SIDR_V1_BASE, SID_LFO7_BASE, SID_LFO8_BASE
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L5, SIDR_V2_BASE, SID_LFO9_BASE, SID_LFO10_BASE
	SIDSE_M_SYNC_LFO_MACRO SID_SE_TRG_EVNT_H, SID_TRG_TARGET_H_L6, SIDR_V3_BASE, SID_LFO11_BASE, SID_LFO12_BASE
SIDSE_M_SYNC_LFO_NotR


SIDSE_M_SYNC_NOTE_MACRO	MACRO	evnt, evnt_flag, voice_base
	LOCAL	SIDSE_M_SYNC_NOTE_Skip

	BRA_IFCLR evnt, evnt_flag, BANKED, SIDSE_M_SYNC_NOTE_Skip
	lfsr	FSR1, voice_base
	rcall	SIDSE_M_Hlp_GetAssignedInsPtr
	call	SIDSE_M_NOTE_Restart
	bcf	evnt, evnt_flag, BANKED
SIDSE_M_SYNC_NOTE_Skip
	incf	SID_SE_ELEMENT_NUM, F, BANKED
	ENDM

	clrf	SID_SE_ELEMENT_NUM, BANKED
SIDSE_M_SYNC_Note_L
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1L, SIDL_V1_BASE
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2L, SIDL_V2_BASE
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3L, SIDL_V3_BASE

	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_M_SYNC_Note_NotR
SIDSE_M_SYNC_Note_R
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O1R, SIDR_V1_BASE
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O2R, SIDR_V2_BASE
	SIDSE_M_SYNC_NOTE_MACRO SID_SE_TRG_EVNT_L, SID_TRG_TARGET_L_O3R, SIDR_V3_BASE
SIDSE_M_SYNC_Note_NotR

	return


;; --------------------------------------------------------------------------
;; This function handles the LFOs (the SIDSE_M_LFO_Restart function restarts a LFO)
;; IN: pointer to SID_Ix_LFOx_BASE in FSR0 (patch record)
;;     pointer to SID_LFOx_BASE in FSR1
;;     LFO number in SID_SE_ELEMENT_NUM
;;     Voice assignment in SID_CURRENT_VOICE_ASSG
;; --------------------------------------------------------------------------
SIDSE_M_LFO_Restart
	;; if oneshot mode: don't take sync mode into account if we already had a overrun condition
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_M_LFO_Restart_NoOneShot
SIDSE_M_LFO_Restart_OneShot
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_LFO_OVERRUN, W, BANKED
	bnz	SIDSE_M_LFO_Restart_OneShot_Cont
	;; no overrun occured yet - check for sync
SIDSE_M_LFO_Restart_NoOneShot

SIDSE_M_LFO_Restart_OneShot_Cont
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
	rgoto	SIDSE_M_LFO_Restart_Cont

;; --------------------------------------------------------------------------
SIDSE_M_LFO			; normal LFO entry
	;; skip if LFO not enabled
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ENABLE, ACCESS, SIDSE_M_LFO_End

	;; temporary shift FSR1 to SID_LFOx_DELAY_CTR_L for easier handling
	movlw	SID_LFOx_DELAY_CTR_L
	addwf	FSR1L, F

	;; delay LFO so long 16bit delay counter != 0
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	bz	SIDSE_M_LFO_NoDelay
SIDSE_M_LFO_Delay
	;; increment counter, set it to zero on overrun (no delay anymore)
	movlw	SID_Ix_LFOx_DELAY
	movff	PLUSW0, IRQ_TMP3; incrementer is the same like used for envelopes
	movlw	0x80		; curve *must* be disabled!
	call	SIDSE_Hlp_ENV_GetBendedValue	; incrementer in MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	addwf	POSTINC1, F
	movf	MIOS_PARAMETER2, W
	addwfc	POSTDEC1, F
	bc	SIDSE_M_LFO_DelayOv
	rgoto	SIDSE_M_LFO_End
SIDSE_M_LFO_DelayOv
	;; overrun: clear counter to disable delay
	clrf	POSTINC1
	clrf	POSTDEC1
SIDSE_M_LFO_NoDelay
	movlw	-SID_LFOx_DELAY_CTR_L	; switch back to SID_LFOx_CTR_L
	addwf	FSR1L, F

	;; implementation: a LFO is just a 16bit ramp which is mapped to a waveform

	;; clear LFO overrun flag
	bcf	SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED

	;; in oneshot mode: check if overrun already occured
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_M_LFO_NoOneShotChk
SIDSE_M_LFO_OneShotChk
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_LFO_OVERRUN, W, BANKED
	bz	SIDSE_M_LFO_NoOneShotChk
	;; set counter to maximum value and continue at waveform calculation
	setf	POSTINC1
	setf	POSTDEC1
	rgoto	SIDSE_M_LFO_OneShot_Cont
SIDSE_M_LFO_NoOneShotChk

	;; if clock sync enabled: only increment on clock events
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_CLKSYNC, ACCESS, SIDSE_M_LFO_NoClkSync
SIDSE_M_LFO_ClkSync
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED, SIDSE_M_LFO_NoClk_Cont
SIDSE_M_LFO_NoClkSync

	;; increment 16bit counter by given rate
	movlw	SID_Ix_LFOx_RATE
	movff	PLUSW0, IRQ_TMP1

	;; if LFO synched via clock, replace 245-255 by MIDI clock optimized incrementers
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_CLKSYNC, ACCESS, SIDSE_M_LFO_Rate_NoClkSync
SIDSE_M_LFO_Rate_ClkSync
	movf	IRQ_TMP1, W
	addlw	-245
	bnc	SIDSE_M_LFO_Rate_NoClkSyncValues
	TABLE_ADDR_MUL_W SID_LFO_TABLE_MCLK, 2
	rgoto	SIDSE_M_LFO_Rate_ClkSync_Cont
SIDSE_M_LFO_Rate_NoClkSyncValues
SIDSE_M_LFO_Rate_NoClkSync

	movf	IRQ_TMP1, W
	TABLE_ADDR_MUL_W SID_LFO_TABLE, 2
SIDSE_M_LFO_Rate_ClkSync_Cont
	tblrd*+
	movf	TABLAT, W
	addwf	POSTINC1, F	; SID_LFOx_CTR_L
	tblrd*+
	movf	TABLAT, W
	addwfc	POSTDEC1, F	; SID_LFOx_CTR_H
	bnc	SIDSE_M_LFO_NoOverrun

SIDSE_M_LFO_Overrun
	;; required for S&H function below, will be cleared with each run
	bsf	SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED

	;; required for one-shot mode, will be cleared with next gate trigger
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_LFO_OVERRUN, F, BANKED

	;; in oneshot mode: ensure that counter is set to maximum value
	movlw	SID_Ix_LFOx_MODE
	BRA_IFCLR PLUSW0, SID_I_LFO_MODE_ONESHOT, ACCESS, SIDSE_M_LFO_OverrunNoOneShot
SIDSE_M_LFO_OverrunOneShot
	setf	POSTINC1
	setf	POSTDEC1
SIDSE_M_LFO_OverrunNoOneShot
SIDSE_M_LFO_NoOverrun

SIDSE_M_LFO_OneShot_Cont		; entry point for oneshot function (if overrun already occured)
SIDSE_M_LFO_NoClk_Cont		; entry point for clock sync function (no clock event)
SIDSE_M_LFO_Restart_Cont		; entry point for restart function

	;; temporary store SID_LFOx_CTR_[LH] in IRQ_TMP[12]
	movff	POSTINC1, IRQ_TMP1
	movff	POSTDEC1, IRQ_TMP2

	;; map to waveform
	;; result in MUL_A_[LH]
	movlw	SID_Ix_LFOx_MODE ; waveform selection in bit [6:4]
	swapf	PLUSW0, W
	BRA_IFSET WREG, 3, ACCESS, SIDSE_M_LFO_Wav89ABCDEF
SIDSE_M_LFO_Wav01234567
	BRA_IFSET WREG, 2, ACCESS, SIDSE_M_LFO_Wav4567
SIDSE_M_LFO_Wav0123
	BRA_IFSET WREG, 1, ACCESS, SIDSE_M_LFO_Wav23
SIDSE_M_LFO_Wav01
	BRA_IFSET WREG, 0, ACCESS, SIDSE_M_LFO_Wav1

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav0	; Sine
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

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_M_LFO_Wav0_NoNeg
SIDSE_M_LFO_Wav0_Neg
	comf	MUL_A_L, F, BANKED
	comf	MUL_A_H, F, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
SIDSE_M_LFO_Wav0_NoNeg

	rgoto	SIDSE_M_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav1	; Triangle
	;; similar to sine, but linear waveform
	clrc
	rlf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED
	rlf	IRQ_TMP2, W
	andlw	0x7f
	movwf	MUL_A_H, BANKED

	BRA_IFCLR IRQ_TMP2, 6, ACCESS, SIDSE_M_LFO_Wav1_NoNeg1
SIDSE_M_LFO_Wav1_Neg1
	comf	MUL_A_L, BANKED
	comf	MUL_A_H, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
	bcf	MUL_A_H, 7, BANKED
SIDSE_M_LFO_Wav1_NoNeg1

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_M_LFO_Wav1_NoNeg2
SIDSE_M_LFO_Wav1_Neg2
	comf	MUL_A_L, F, BANKED
	comf	MUL_A_H, F, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
SIDSE_M_LFO_Wav1_NoNeg2

	rgoto	SIDSE_M_LFO_Wav_Cont

SIDSE_M_LFO_Wav23
	BRA_IFSET WREG, 0, ACCESS, SIDSE_M_LFO_Wav3
	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav2	; Saw
	;; this one is simple - it's "counter-0x8000"
	movlw	0x00
	subwf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED
	movlw	0x80
	subwfb	IRQ_TMP2, W
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_M_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav3	; Pulse
	;; this one is even more simple
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0xff
	movwf	MUL_A_L, BANKED
	movlw	0x80
	btfsc	IRQ_TMP2, 7
	movlw 0x7f
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_M_LFO_Wav_Cont

SIDSE_M_LFO_Wav4567
	BRA_IFSET WREG, 1, ACCESS, SIDSE_M_LFO_Wav67
SIDSE_M_LFO_Wav45
	BRA_IFSET WREG, 0, ACCESS, SIDSE_M_LFO_Wav5

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav4	; Random
	;; only on LFO overrun:
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_LFO_OVERRUN, BANKED, SIDSE_M_LFO_End

	;; generate new random number and copy it to MUL_A_[LH]
	call	SID_RND_GenRandomNumber
	movff	SID_RANDOM_SEED_L, MUL_A_L
	movff	SID_RANDOM_SEED_L, MUL_A_H

	rgoto	SIDSE_M_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav5	; positive sine
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

	rgoto	SIDSE_M_LFO_Wav_Cont


SIDSE_M_LFO_Wav67
	BRA_IFSET WREG, 0, ACCESS, SIDSE_M_LFO_Wav7

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav6	; positive triangle
	movff	IRQ_TMP1, MUL_A_L
	movff	IRQ_TMP2, MUL_A_H

	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_M_LFO_Wav6_NoNeg
SIDSE_M_LFO_Wav6_Neg
	comf	MUL_A_L, BANKED
	comf	MUL_A_H, BANKED
	incf	MUL_A_L, F, BANKED
	skpnz
	incf	MUL_A_H, F, BANKED
	bcf	MUL_A_H, 7, BANKED
SIDSE_M_LFO_Wav6_NoNeg

	rgoto	SIDSE_M_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav7	; positive saw
	clrc
	rrf	IRQ_TMP2, W
	movwf	MUL_A_H, BANKED
	rrf	IRQ_TMP1, W
	movwf	MUL_A_L, BANKED

	rgoto	SIDSE_M_LFO_Wav_Cont

	;; ------------------------------------------------------------------
SIDSE_M_LFO_Wav89ABCDEF	; positive pulse
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0xff
	movwf	MUL_A_L, BANKED
	movlw	0x00
	btfsc	IRQ_TMP2, 7
	movlw 0x7f
	movwf	MUL_A_H, BANKED

	rgoto	SIDSE_M_LFO_Wav_Cont
SIDSE_M_LFO_Wav_Cont


	;; result in MUL_A_[LH]
	;; range: +/- 0x7fff

SIDSE_M_LFO_MOD_Pitch
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_LFO1_DEPTH_P-SID_Ix_M_Vx_LFO1_MODE
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_LFO_MOD_Pitch_End
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; pointer to SID_MOD_TARG_PITCHx_L -> FSR2
	lfsr	FSR2, SID_MOD_TARG_PITCH1_L
	clrc
	rrf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	addwf	FSR2L, F

	;; reduce intensity of pitch modulation
	clrc
	btfsc	MUL_R_3, 7
	setc
	rrf	MUL_R_3, F
	rrf	MUL_R_2, F
	rrf	MUL_R_1, F

	clrc
	btfsc	MUL_R_3, 7
	setc
	rrf	MUL_R_3, F
	rrf	MUL_R_2, F
	rrf	MUL_R_1, F

	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
SIDSE_M_LFO_MOD_Pitch_End


SIDSE_M_LFO_MOD_PW
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_LFO1_DEPTH_PW-SID_Ix_M_Vx_LFO1_MODE
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_LFO_MOD_PW_End
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; pointer to SID_MOD_TARG_PWx_L -> FSR2
	lfsr	FSR2, SID_MOD_TARG_PW1_L
	clrc
	rrf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	addwf	FSR2L, F

	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
SIDSE_M_LFO_MOD_PW_End


SIDSE_M_LFO_MOD_FIL
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_LFO1_DEPTH_F-SID_Ix_M_Vx_LFO1_MODE
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_LFO_MOD_FIL_End
#if 0
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; branch depending on voice
	movlw	2*3
	cpfslt	SID_SE_ELEMENT_NUM, BANKED
	rgoto SIDSE_M_LFO_MOD_FIL_R
SIDSE_M_LFO_MOD_FIL_L
	rcall	SIDSE_M_LFO_MOD_FIL_TrgL_Sub
	rgoto	SIDSE_M_LFO_MOD_FIL_End

SIDSE_M_LFO_MOD_FIL_R
	rcall	SIDSE_M_LFO_MOD_FIL_TrgR_Sub
	;; 	rgoto	SIDSE_M_LFO_MOD_FIL_End
#else

	;; TK: intention is to ensure, that only the LFO of the last assigned voice
	;; modulates the filter for more logical results (the results were not obvious
	;; when all 6 LFOs modulated the filters...)
	
	;; get assigned MIDI voice
	lfsr	FSR2, SIDL_V1_BASE
	rrf	SID_SE_ELEMENT_NUM, W, BANKED
	andlw	0x7f
	movwf	TABLAT		; (temporary store voice number in TABLAT)
	mullw	SID_Vx_RECORD_LEN
	movf	PRODL, W
	addlw	SID_Vx_ASSIGNED_MV
	addwf	FSR2L, F
	movf	INDF2, W

	;; get last voice of MIDI instrument
	lfsr	FSR2, SID_MV1_BASE
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addlw	SID_MVx_LAST_VOICE
	addwf	FSR2L, F
	movf	INDF2, W

	;; don't modulate if current LFO not part of last voice
	cpfseq	TABLAT, ACCESS
	rgoto SIDSE_M_LFO_MOD_FIL_End

	;; scale depth
	movlw	SID_Ix_M_Vx_LFO1_DEPTH_F-SID_Ix_M_Vx_LFO1_MODE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ScaleDepth	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; add to modulation target depending on instrument specific voice assignment
	movf	SID_CURRENT_VOICE_ASSG, W, BANKED
	andlw	0x0f
	bz	SIDSE_M_LFO_MOD_FIL_TrgLR  ; LR
	addlw	-1
	bz	SIDSE_M_LFO_MOD_FIL_TrgL   ; L only
	addlw	-1
	bz	SIDSE_M_LFO_MOD_FIL_TrgR   ; R only
	addlw	-1
	bz	SIDSE_M_LFO_MOD_FIL_TrgL   ; O1
	addlw	-1
	bz	SIDSE_M_LFO_MOD_FIL_TrgL   ; O2
	addlw	-1
	bz	SIDSE_M_LFO_MOD_FIL_TrgL   ; O3
	rgoto	SIDSE_M_LFO_MOD_FIL_TrgR   ; O4..O6
	
SIDSE_M_LFO_MOD_FIL_TrgLR
	rcall	SIDSE_M_LFO_MOD_FIL_TrgL_Sub
	rcall	SIDSE_M_LFO_MOD_FIL_TrgR_Sub
	rgoto	SIDSE_M_LFO_MOD_FIL_End

SIDSE_M_LFO_MOD_FIL_TrgL
	rcall	SIDSE_M_LFO_MOD_FIL_TrgL_Sub
	rgoto	SIDSE_M_LFO_MOD_FIL_End

SIDSE_M_LFO_MOD_FIL_TrgR
	rcall	SIDSE_M_LFO_MOD_FIL_TrgR_Sub
	;; 	rgoto	SIDSE_M_LFO_MOD_FIL_End
#endif
SIDSE_M_LFO_MOD_FIL_End

SIDSE_M_LFO_End
	return

;; subroutine for adding modulation result to FIL1_L target
SIDSE_M_LFO_MOD_FIL_TrgL_Sub
	;; transfer to FIL1
	lfsr	FSR2, SID_MOD_TARG_FIL1_L
	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
	return
	rgoto	SIDSE_M_LFO_MOD_FIL_End

;; subroutine for adding modulation result to FIL1_R target
SIDSE_M_LFO_MOD_FIL_TrgR_Sub
	;; transfer to FIL2
	lfsr	FSR2, SID_MOD_TARG_FIL2_L
	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
	return
	

;; --------------------------------------------------------------------------
;; This function handles the Envelopes
;; IN: pointer to SID_Ix_M_ENVx_BASE in FSR0 (patch record)
;;     pointer to SID_ENVx_BASE in FSR1
;;     ENV number in SID_SE_ELEMENT_NUM
;;     SID_SE_STATE.SID_SE_STATE_ACCENT flag increases depth (only used by bassline)
;; --------------------------------------------------------------------------
SIDSE_M_ENV
	;; if clock sync enabled: only increment on clock events
	movlw	SID_Ix_M_Vx_ENV_MODE
	BRA_IFCLR PLUSW0, SID_I_ENV_MODE_CLKSYNC, ACCESS, SIDSE_M_ENV_NoClkSync
SIDSE_M_ENV_ClkSync
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED, SIDSE_M_ENV_ClkSync_Cont
	;; transfer current ENV counter into MIOS_PARAMETER[12]
	movff	POSTINC1, MIOS_PARAMETER1
	movff	POSTDEC1, MIOS_PARAMETER2
	rgoto	SIDSE_M_ENV_Cont
SIDSE_M_ENV_NoClkSync
SIDSE_M_ENV_ClkSync_Cont

	;; branch depending on EG state
	movlw	SID_ENVx_STATE
	BRA_IFSET PLUSW1, SID_ENV_STATE_RELEASE2, ACCESS, SIDSE_M_ENV_End
	BRA_IFSET PLUSW1, SID_ENV_STATE_RELEASE1, ACCESS, SIDSE_M_ENV_Release
	BRA_IFCLR PLUSW1, SID_ENV_STATE_ATTACK1, ACCESS, SIDSE_M_ENV_End
	BRA_IFSET PLUSW1, SID_ENV_STATE_SUSTAIN, ACCESS, SIDSE_M_ENV_Sustain
	BRA_IFSET PLUSW1, SID_ENV_STATE_DECAY2, ACCESS, SIDSE_M_ENV_Decay

SIDSE_M_ENV_Attack

	;; in attack phase we have to increase the EG counter
	;; attack rate in IRQ_TMP3
	movlw	SID_Ix_M_Vx_ENV_ATTACK
	movf	PLUSW0, W
	movwf	IRQ_TMP3
	;; get final rate depending on curve setting
	movlw	SID_Ix_M_Vx_ENV_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; final attack level is 0xff
	movlw	0xff
	movwf	IRQ_TMP1
	;; increment counter
	call	SIDSE_Hlp_ENV_IncrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_M_ENV_Cont

	;; switch to next phase
	movlw	SID_ENVx_STATE
	bsf	PLUSW1, SID_ENV_STATE_DECAY2
	rgoto	SIDSE_M_ENV_Cont

SIDSE_M_ENV_Decay
	;; in decay phase we have to increase/decrease the EG counter depending on the current level
	;; decay rate in WREG
	;; if accent flag set, use alternative ENC_DECAY_A parameter (bassline only)
	movlw	SID_Ix_M_Vx_ENV_DECAY
	btfsc	SID_SE_STATE, SID_SE_STATE_ACCENT, BANKED
	movlw SID_Ix_B_Vx_ENV_DECAY_A
	movff	PLUSW0, IRQ_TMP3

	;; get final rate depending on curve setting
	movlw	SID_Ix_M_Vx_ENV_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; result: low byte in MIOS_PARAMETER1, high byte in MIOS_PARAMETER2
	;; store final sustain in IRQ_TMP1
	movlw	SID_Ix_M_Vx_ENV_SUSTAIN
	movff	PLUSW0, IRQ_TMP1
	;; incr/decr counter
	call	SIDSE_Hlp_ENV_IncrDecrCtr
	;; we've reached the level once the zero bit set
	bnz	SIDSE_M_ENV_Cont

	;; switch to next phase
	movlw	SID_ENVx_STATE
	bsf	PLUSW1, SID_ENV_STATE_SUSTAIN


SIDSE_M_ENV_Sustain
	;; write sustain value into counter (again)
	movlw	SID_Ix_M_Vx_ENV_SUSTAIN
	movff	PLUSW0, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	rgoto	SIDSE_M_ENV_Cont

	
SIDSE_M_ENV_Release
	;; in the release phase we have to decrease the EG counter until it reaches 0
	;; release rate in WREG
	movlw	SID_Ix_M_Vx_ENV_RELEASE
	movff	PLUSW0, IRQ_TMP3
	;; get release rate depending on curve setting
	movlw	SID_Ix_M_Vx_ENV_CURVE
	movf	PLUSW0, W
	call	SIDSE_Hlp_ENV_GetBendedValue
	;; target level: 0
	clrf	IRQ_TMP1
	;; decr counter
	call	SIDSE_Hlp_ENV_DecrCtr

	;; we've reached the level once the zero bit set
	bnz	SIDSE_M_ENV_Cont

	;; switch to final phase
	movlw	SID_ENVx_STATE
	bsf	PLUSW1, SID_ENV_STATE_RELEASE2

	;; 	rgoto	SIDSE_M_ENV_Cont

SIDSE_M_ENV_Cont
	;; copy MIOS_PARAMETER[12] to SID_ENVx_CTR_[LH]
	movff	MIOS_PARAMETER1, POSTINC1
	movff	MIOS_PARAMETER2, POSTDEC1

	;; skip ENV mapping if result is zero
	movf	MIOS_PARAMETER1, W
	iorwf	MIOS_PARAMETER2, W
	bz	SIDSE_M_ENV_Cont_Zero

	;; transfer ENV value to MUL_A_[LH], range: +/- 0x7fff
	clrc
	rrf	MIOS_PARAMETER2, W
	movwf	MUL_A_H, BANKED
	rrf	MIOS_PARAMETER1, W
	movwf	MUL_A_L, BANKED


SIDSE_M_ENV_MOD_Pitch
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_ENV_DEPTH_P
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_ENV_MOD_Pitch_End
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepthAcc	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; pointer to SID_MOD_TARG_PITCHx_L -> FSR2
	lfsr	FSR2, SID_MOD_TARG_PITCH1_L
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	addwf	FSR2L, F

	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
SIDSE_M_ENV_MOD_Pitch_End


SIDSE_M_ENV_MOD_PW
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_ENV_DEPTH_PW
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_ENV_MOD_PW_End
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepthAcc	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; pointer to SID_MOD_TARG_PWx_L -> FSR2
	lfsr	FSR2, SID_MOD_TARG_PW1_L
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	mullw	3
	movf	PRODL, W
	addwf	FSR2L, F

	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
SIDSE_M_ENV_MOD_PW_End


SIDSE_M_ENV_MOD_FIL
	;; scale MOD source by depth
	movlw	SID_Ix_M_Vx_ENV_DEPTH_F
	movf	PLUSW0, W
	xorlw	0x80		; skip if depth is 0x80 (-> 0)
	bz	SIDSE_M_ENV_MOD_FIL_End
	xorlw	0x80
	call	SIDSE_Hlp_ScaleDepthAcc	; 15bit signed value in MUL_A_[LH], result in MUL_R_[123], uses IRQ_TMP1

	;; branch depending on voice
	movlw	3
	cpfslt	SID_SE_ELEMENT_NUM, BANKED
	rgoto SIDSE_M_ENV_MOD_FIL_R
SIDSE_M_ENV_MOD_FIL_L
	;; transfer to FIL1
	lfsr	FSR2, SID_MOD_TARG_FIL1_L
	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
	rgoto	SIDSE_M_ENV_MOD_FIL_End

SIDSE_M_ENV_MOD_FIL_R
	;; transfer to FIL2
	lfsr	FSR2, SID_MOD_TARG_FIL2_L
	movf	MUL_R_1, W, BANKED
	addwf	POSTINC2, F
	movf	MUL_R_2, W, BANKED
	addwfc	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTINC2, F
	;; 	rgoto	SIDSE_M_ENV_MOD_FIL_End
SIDSE_M_ENV_MOD_FIL_End


SIDSE_M_ENV_Cont_Zero
SIDSE_M_ENV_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_M_ENV_Restart
;;  DESCRIPTION: help function which restarts a single ENV
;;  IN: SID_ENV[123456]_BASE in FSR1
;; --------------------------------------------------------------------------
SIDSE_M_ENV_Restart
	movlw	(1 << SID_ENV_STATE_ATTACK1)	; start at attack phase
	movwf	PRODL
	movlw	SID_ENVx_STATE
	movff	PRODL, PLUSW1

	;; no delay supported for multi engine - always clear delay counter
	movlw	SID_ENVx_DELAY_CTR_H	; high byte always reset to 0
	clrf	PLUSW1
	movlw	SID_ENVx_DELAY_CTR_L	; low byte always reset to 0
	clrf	PLUSW1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_M_ENV_TrgRelease
;;  DESCRIPTION: help function which starts release phase of a single ENV
;;  IN: SID_ENV[123456]_BASE in FSR1
;; --------------------------------------------------------------------------
SIDSE_M_ENV_TrgRelease
	movlw	SID_ENVx_STATE
	bsf	PLUSW1, SID_ENV_STATE_RELEASE1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_M_Note_Restart
;;  DESCRIPTION: help function which restarts the delay counter for voices
;;               and requests an active gate
;;  IN: patch base in FSR0, voice base in FSR1, osc number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_M_NOTE_Restart
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
	BRA_IFCLR WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_M_NOTE_Restart_NoABW
SIDSE_M_NOTE_Restart_ABW
	movlw	0x01
	movwf	PRODL

	;; clear ADSR registers, so that the envelope gets completely released
	call	SIDSE_Hlp_GetSIDFrqPtr
	movlw	SIDx_V1_ENV_AD
	clrf	PLUSW2
	movlw	SIDx_V1_ENV_SR
	clrf	PLUSW2
SIDSE_M_NOTE_Restart_NoABW

	movlw	SID_Vx_SET_DELAY_CTR_L
	movff	PRODL, PLUSW1
	return


;; --------------------------------------------------------------------------
;; This function updates the LED matrix
;; For basslines, this is interrupt driven for fastest update time
;; only used when master is selected
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_M_CS_LM

	;; if slave is selected: exit (update done in CS_MENU_LED_Update_ModMatrix)
	btfss	CS_MENU_SELECTED_SID_FLAGS, 0
	return

	;; exit if not in meter mode
	btfss	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	return

	;; also used by drum mode!
SIDSE_D_CS_LM_Meter_ReUse

	;; activate level meters
	bsf	CS_STAT2, CS_STAT2_LEVEL_METERS

	;; trigger level meter whenever gate is active
	SET_BSR	SIDL_BASE
SIDSE_M_CS_LM_L
	movlw	0x3f
	btfsc	SIDL_BASE + SIDx_V1_CTRL, 0, BANKED
	movff	WREG, METER_VALUES1
	btfsc	SIDL_BASE + SIDx_V2_CTRL, 0, BANKED
	movff	WREG, METER_VALUES2
	btfsc	SIDL_BASE + SIDx_V3_CTRL, 0, BANKED
	movff	WREG, METER_VALUES3

	movlw	0x3f
	btfsc	SIDR_BASE + SIDx_V1_CTRL, 0, BANKED
	movff	WREG, METER_VALUES4
	btfsc	SIDR_BASE + SIDx_V2_CTRL, 0, BANKED
	movff	WREG, METER_VALUES5
	btfsc	SIDR_BASE + SIDx_V3_CTRL, 0, BANKED
	movff	WREG, METER_VALUES6

	;; clear remaining meter values
	SET_BSR	METER_VALUES0
	clrf	METER_VALUES0, BANKED
	clrf	METER_VALUES7, BANKED
	SET_BSR	SID_BASE

	return

