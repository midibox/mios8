; $Id$
;
; MIDIbox SID
; Software Synthesizer Engine
;  
; Activate this #define to measure the performance with a scope
; the used port has to be specified here - comment out for no measuring
;#define SID_SE_MEASURE_PERFORMANCE_PORT   LATC, 3
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Idea for ENV Curve Parameter and OSC synchronization by Jess D. Skov-Nielsen
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  SID Flags
;; ==========================================================================

;; MIDI Voice Control Flags
SID_MV_ARP_STATE_ARP_ACTIVE	EQU	0 ; set if at least one arp is active
SID_MV_ARP_STATE_ARP_UP		EQU	1 ; arp order is incremented
SID_MV_ARP_STATE_SYNC_ARP	EQU	2 ; syncs the arpeggiator when a new note is played
SID_MV_ARP_STATE_HOLD_SAVED	EQU	3 ; notifies if hold mode was active (notes should be disabled on 1->0 transition)

SID_V_STATE_VOICE_ACTIVE	EQU	0
SID_V_STATE_VOICE_DISABLED	EQU	1
SID_V_STATE_GATE_ACTIVE		EQU	2
SID_V_STATE_GATE_SET_REQ	EQU	3
SID_V_STATE_GATE_CLR_REQ	EQU	4
SID_V_STATE_PORTA_ACTIVE	EQU	5
SID_V_STATE_ACCENT		EQU	6
SID_V_STATE_SLIDE		EQU	7

SID_I_OPT1_FLAGS_ABW		EQU	0	; ADSR bug workaround

SID_I_L_FLAGS1_LEGATO		EQU	0	; switches between mono/legato mode
SID_I_L_FLAGS1_WT_ONLY		EQU	1	; notes played by WT only
SID_I_L_FLAGS1_SUS_KEY		EQU	2	; enables SusKey

SID_I_M_V_FLAGS2_LEGATO		EQU	0	; multi engine only: switches between mono/legato mode when poly not enabled
SID_I_M_V_FLAGS2_WT_ONLY	EQU	1	; multi engine only: notes played by WT only
SID_I_M_V_FLAGS2_SUS_KEY	EQU	2	; multi engine only: enables SusKey
SID_I_M_V_FLAGS2_POLY		EQU	3	; multi engine only: enables Poly mode
SID_I_M_V_FLAGS2_PHASE		EQU	4	; multi engine only: oscillator phase synchronisation

SID_I_B_V_FLAGS2_LEGATO		EQU	0	; bassline engine only: switches between mono/legato mode when poly not enabled
SID_I_B_V_FLAGS2_WT_ONLY	EQU	1	; bassline engine only: notes played by WT only
SID_I_B_V_FLAGS2_SUS_KEY	EQU	2	; bassline engine only: enables SusKey
SID_I_B_V_FLAGS2_PHASE		EQU	4	; bassline engine only: oscillator phase synchronisation

SID_I_V_FLAGS1_PORTA_CTG	EQU	0	; constant time glide
SID_I_V_FLAGS1_PORTA_GLISSANDO	EQU	1	; glissando
SID_I_V_FLAGS1_GSA		EQU	2	; gate stays active
SID_I_V_FLAGS1_VASG_0		EQU	4	; only relevant for drum engine: voice assignment, bit 0
SID_I_V_FLAGS1_VASG_1		EQU	5	; only relevant for drum engine: voice assignment, bit 1
SID_I_V_FLAGS1_VASG_2		EQU	6	; only relevant for drum engine: voice assignment, bit 2
SID_I_V_FLAGS1_VASG_3		EQU	7	; only relevant for drum engine: voice assignment, bit 3

SID_I_V_ARP_MODE_ENABLE		EQU	0	; enables arp
SID_I_V_ARP_MODE_DIR_DOWN	EQU	1	; arp dir Up/Down
SID_I_V_ARP_MODE_DIR_ALT	EQU	2	; arp dir U&D/D&U
SID_I_V_ARP_MODE_DIR_ALT2	EQU	3	; arp dir U&D/D&U second option (border notes played twice) - dir mode 6 and 7 will select random
SID_I_V_ARP_MODE_SORTED		EQU	4	; select sorted stack
SID_I_V_ARP_MODE_HOLD		EQU	5	; select hold stack
SID_I_V_ARP_MODE_DIV_SYNC	EQU	6	; disables divider reset on new notes
SID_I_V_ARP_MODE_CAC		EQU	7	; constant arp cycle

SID_I_V_ARP_SPEED_DIV_ONESHOT	EQU	7	; oneshot mode

SID_I_V_SEQ_ON			EQU	6	; drum engine only: sequencer on/off flag
SID_I_V_SEQ_SYNC16		EQU	7	; bassline/drum engine only: 16step sync function

SID_SEQ_MISC_SUBCTR		EQU	0	; [2:0] counts from 0 to 6
SID_SEQ_MISC_SEQ_ON		EQU	3	; stores sequencer on flag
SID_SEQ_MISC_SEQ_RUNNING	EQU	4	; stores sequencer running flag (only used in drum mode)

SID_I_F_FIP_ON			EQU	7	; located in high byte of cutoff frequency: filter interpolation flag

SID_I_LFO_MODE_ENABLE		EQU	0
SID_I_LFO_MODE_SYNC_M		EQU	1	; sync LFO (only for multi and bassline engine - lead engine uses trigger matrix!)
SID_I_LFO_MODE_CLKSYNC		EQU	2
SID_I_LFO_MODE_ONESHOT		EQU	3
SID_I_LFO_MODE_WAVEFORM0	EQU	4
SID_I_LFO_MODE_WAVEFORM1	EQU	5
SID_I_LFO_MODE_WAVEFORM2	EQU	6
SID_I_LFO_MODE_WAVEFORM3	EQU	7

SID_I_ENV_MODE_LOOP_B0		EQU	0	; loop begin (3 bit)
SID_I_ENV_MODE_LOOP_B1		EQU	1
SID_I_ENV_MODE_LOOP_B2		EQU	2
SID_I_ENV_MODE_LOOP_E0		EQU	4	; loop end (3 bit)
SID_I_ENV_MODE_LOOP_E1		EQU	5
SID_I_ENV_MODE_LOOP_E2		EQU	6
SID_I_ENV_MODE_CLKSYNC		EQU	7

SID_I_ENV_MODE_CURVE_ATT	EQU	4	; only used by Multi/Bassline engine: curve assigned to attack
SID_I_ENV_MODE_CURVE_DEC	EQU	5	; only used by Multi/Bassline engine: curve assigned to sustain
SID_I_ENV_MODE_CURVE_REL	EQU	6	; only used by Multi/Bassline engine: curve assigned to release

SID_SE_STATE_LFO_OVERRUN	EQU	0	; used by LFO handler to notify an overrun
SID_SE_STATE_WT_NEW_STEP_REQ	EQU	0	; used by WT handler to request new step (shared flag with LFO)
SID_SE_STATE_ARP_NEW_NOTE_REQ	EQU	0	; used by ARP handler to request new note (shared flag with LFO)
SID_SE_STATE_ARP_FIRST_NOTE_REQ	EQU	1	; used by ARP handler to request the first note (oneshot function)
SID_SE_STATE_ARP_GATE_CLR_REQ	EQU	2	; used by ARP handler to request a gate clear
SID_SE_STATE_GLOBAL_CLK_EVENT	EQU	3	; temporary "global clock event" flag
SID_SE_STATE_MIDI_CLK_FA_REQ	EQU	4	; "MIDI clock start" request flag
SID_SE_STATE_MIDI_CLK_FB_REQ	EQU	5	; "MIDI clock continue" request flag
SID_SE_STATE_MIDI_CLK_FC_REQ	EQU	6	; "MIDI clock stop" request flag
SID_SE_STATE_ACCENT		EQU	7	; used by multi/bassline ENV handler to increase depth on accent

SID_ENV_STATE_ATTACK1		EQU	0
SID_ENV_STATE_ATTACK2		EQU	1
SID_ENV_STATE_DECAY1		EQU	2
SID_ENV_STATE_DECAY2		EQU	3
SID_ENV_STATE_SUSTAIN		EQU	4
SID_ENV_STATE_RELEASE1		EQU	5
SID_ENV_STATE_RELEASE2		EQU	6

SID_ENS_CTRL1_CLK_SLAVE		EQU	0	; switch between MIDI clock master/slave
SID_ENS_CTRL1_CLK_AUTO		EQU	1	; automatic switching between master/slave
SID_ENS_CTRL1_MONO		EQU	6	; SIDs are played mono

SID_ENS_CTRL2_F2A		EQU	0	; forward Filter CutOff/Resonance to AOUTs
SID_ENS_CTRL2_V2A		EQU	1	; forward Volume to AOUTs
SID_ENS_CTRL2_P2A		EQU	2	; forward Pulsewidth to AOUTs
SID_ENS_CTRL2_K2A		EQU	3	; forward Key Values to AOUTs and Gates to Digital Out
SID_ENS_CTRL2_O2A		EQU	4	; forward Oscillator Values to AOUTs and Gates to Digital Out

SID_TRG_TARGET_L_O1L		EQU	0	; OSC1 Left
SID_TRG_TARGET_L_O2L		EQU	1	; OSC2 Left
SID_TRG_TARGET_L_O3L		EQU	2	; OSC3 Left
SID_TRG_TARGET_L_O1R		EQU	3	; OSC1 Right
SID_TRG_TARGET_L_O2R		EQU	4	; OSC2 Right
SID_TRG_TARGET_L_O3R		EQU	5	; OSC3 Right
SID_TRG_TARGET_L_E1A		EQU	6	; ENV1 Attack
SID_TRG_TARGET_L_E2A		EQU	7	; ENV2 Attack
SID_TRG_TARGET_H_E1R		EQU	0	; ENV1 Release
SID_TRG_TARGET_H_E2R		EQU	1	; ENV2 Release
SID_TRG_TARGET_H_L1		EQU	2	; LFO1 sync
SID_TRG_TARGET_H_L2		EQU	3	; LFO2 sync
SID_TRG_TARGET_H_L3		EQU	4	; LFO3 sync
SID_TRG_TARGET_H_L4		EQU	5	; LFO4 sync
SID_TRG_TARGET_H_L5		EQU	6	; LFO5 sync
SID_TRG_TARGET_H_L6		EQU	7	; LFO6 sync
SID_TRG_TARGET_U_W1R		EQU	0	; WT1 reset
SID_TRG_TARGET_U_W2R		EQU	1	; WT2 reset
SID_TRG_TARGET_U_W3R		EQU	2	; WT3 reset
SID_TRG_TARGET_U_W4R		EQU	3	; WT4 reset
SID_TRG_TARGET_U_W1S		EQU	4	; WT1 step
SID_TRG_TARGET_U_W2S		EQU	5	; WT2 step
SID_TRG_TARGET_U_W3S		EQU	6	; WT3 step
SID_TRG_TARGET_U_W4S		EQU	7	; WT4 step

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  SID Sound Engine Handler: Software Synthesizer part for the SID
;;  called by User Timer every 1 mS
;; --------------------------------------------------------------------------
SIDSE_Handler
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; return immediately if engine has been disabled
	btfsc	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

	;; second possibility (see comment in main.inc)
	btfsc	SID_STAT, SID_STAT_ENGINE_DISABLE_LEVEL0
	return

#ifdef SID_SE_MEASURE_PERFORMANCE_PORT
	bsf	SID_SE_MEASURE_PERFORMANCE_PORT
#endif

	;; save FSR0[LH] and PROD[LH] - we are in an interrupt routine
	movff	FSR0L, SAVED_FSR0L
	movff	FSR0H, SAVED_FSR0H
	movff	PRODL, SAVED_PRODL
	movff	PRODH, SAVED_PRODH

	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SIDSE_Handler_23
SIDSE_Handler_01
	BRA_IFSET WREG, 0, ACCESS, SIDSE_Handler_1
SIDSE_Handler_0
	call	SIDSE_L_Handler
	rgoto	SIDSE_Handler_Cont
SIDSE_Handler_1
	call	SIDSE_B_Handler
	rgoto	SIDSE_Handler_Cont

SIDSE_Handler_23
	BRA_IFSET WREG, 0, ACCESS, SIDSE_Handler_3
SIDSE_Handler_2
	call	SIDSE_D_Handler
	rgoto	SIDSE_Handler_Cont
SIDSE_Handler_3
	call	SIDSE_M_Handler
	rgoto	SIDSE_Handler_Cont

SIDSE_Handler_Cont

	;; thats all
	movff	SAVED_FSR0L, FSR0L		; copy back saved FSR0[LH]
	movff	SAVED_FSR0H, FSR0H
	movff	SAVED_PRODL, PRODL		; copy back saved PROD[LH]
	movff	SAVED_PRODH, PRODH

#ifdef SID_SE_MEASURE_PERFORMANCE_PORT
	bcf	SID_SE_MEASURE_PERFORMANCE_PORT
#endif

	return




;; --------------------------------------------------------------------------
;; This function handles the global clock
;; --------------------------------------------------------------------------
SIDSE_Clk
	;; by default no clock event
	bcf	SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED

	;; increment the clock counter, used to measure the delay between two F8 events
	;; see also USER_MIDI_NotifyRx
	;; ensure that it doesn't overrun
	incf	SID_INCOMING_CLK_CTR, W, BANKED
	skpz
	incf	SID_INCOMING_CLK_CTR, F, BANKED

#if 0
	;; try: if no slave clock is received, increment sent clock counter slowly
	;; (disabled by default - works fine, but is inconsistent)
	movf	SID_INCOMING_CLK_CTR, W, BANKED
	andlw	0x3f
	skpnz
	incf	SID_CLK_REQ_CTR, F, BANKED
#endif

	;; now determine master/slave flag depending on environment setup
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_CLK_AUTO, ACCESS, SIDSE_Clk_ModeAuto
	BRA_IFSET WREG, SID_ENS_CTRL1_CLK_SLAVE, ACCESS, SIDSE_Clk_ModeSlave
SIDSE_Clk_ModeMaster
	bcf	SID_STAT, SID_STAT_CLK_SLAVE
	rgoto	SIDSE_Clk_Mode_Cont
SIDSE_Clk_ModeSlave
	bsf	SID_STAT, SID_STAT_CLK_SLAVE
	rgoto	SIDSE_Clk_Mode_Cont
SIDSE_Clk_ModeAuto
	;; slave mode so long INCOMING_CLK_CTR != 0xff
	bcf	SID_STAT, SID_STAT_CLK_SLAVE
	incf	SID_INCOMING_CLK_CTR, W, BANKED
	skpz
	bsf	SID_STAT, SID_STAT_CLK_SLAVE
	;; 	rgoto	SIDSE_Clk_Mode_Cont
SIDSE_Clk_Mode_Cont

	;; decrement sent clock delay, send interpolated clock events 3 times
	decf	SID_SENT_CLK_DELAY, F, BANKED
	bnz	SIDSE_Clk_NoSlaveTrigger
SIDSE_Clk_SlaveTrigger
	movf	SID_SENT_CLK_CTR, W, BANKED
	xorlw	0x03
	bz	SIDSE_Clk_NoSlaveTrigger
	incf	SID_SENT_CLK_CTR, F, BANKED
	incf	SID_CLK_REQ_CTR, F, BANKED
	rrf	SID_INCOMING_CLK_DELAY, W, BANKED
	rrf	WREG, W
	andlw	0x3f
	movwf	SID_SENT_CLK_DELAY, BANKED
SIDSE_Clk_NoSlaveTrigger

	;; handle FA event (MIDI clock start)
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_Clk_NoFA
SIDSE_Clk_FA
	;; request not cleared here, because it's also used by Arp and sequencer
	;; must be cleared at end of update cycle
	;; 	bcf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED
	;; reset counters
	clrf	SID_SE_GLOBAL_CLK_CTR, BANKED
	;; propagate LFO/Env/WT sync via trigger matrix
	;; (static for Multi Engine)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	SIDSE_Clk_FA_Lead
SIDSE_Clk_FA_Multi
	;; no reset - this disturbs sequence recordings
#if 0
	movlw	0xfc		; LFO reset
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movlw	0x3f		; WT reset for all 6 instruments
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
#endif
	rgoto	SIDSE_Clk_FA_Multi_Cont

SIDSE_Clk_FA_Lead
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_MSt_BASE + 0, WREG
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_MSt_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_MSt_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SIDSE_Clk_FA_Multi_Cont
SIDSE_Clk_NoFA

	;; handle F8 event (MIDI clock)
	movf	SID_CLK_REQ_CTR, W, BANKED
	bz	SIDSE_Clk_NoF8
SIDSE_Clk_F8
	;; decrement counter by one (if there are more requests, they will be handled on next handler invocation
	decf	SID_CLK_REQ_CTR, F, BANKED
	;; if slave: continue at inc routine
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SIDSE_Clk_Inc
SIDSE_Clk_NoF8

	;; if slave: no Inc (only on F8 event)
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SIDSE_Clk_NoInc

	;; check timer0 overrun flag
	BRA_IFCLR INTCON, TMR0IF, ACCESS, SIDSE_Clk_NoInc
	bcf	INTCON, TMR0IF			; clear overrun flag
	SET_BSR	TIMER0_RELOAD_L
	bcf	T0CON, TMR0ON
	movf	TIMER0_RELOAD_L, W, BANKED	; (dummy add to update TMR0H, and to get carry flag of addition)
	addwf	TMR0L, W			; (update TMR0H)
	movf	TIMER0_RELOAD_H, W, BANKED
	addwfc	TMR0H, F
	movf	TIMER0_RELOAD_L, W, BANKED
	addwf	TMR0L, F			; 16bit register update takes place with this write
	bsf	T0CON, TMR0ON
	SET_BSR	SID_BASE
SIDSE_Clk_Inc

	;; increment global clock counter (for Clk/4 and Clk/16 divider)
	incf	SID_SE_GLOBAL_CLK_CTR, F, BANKED

	;; notify clock event
	bsf	SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED

	;; clock propagation
	;; (static for Multi Engine)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	SIDSE_Clk_Reset_Lead
SIDSE_Clk_Reset_Multi
	movlw	0x80		; only step WTs (global flag bit #7 in multi mode)
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
	rgoto	SIDSE_Clk_Reset_Multi_Cont
	
SIDSE_Clk_Reset_Lead

	;; propagate clock event to trigger matrix
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_Clk_BASE
	rcall	SIDSE_Clk_Hlp_PropClkEvent

	;; propagate clock/4 event to trigger matrix on each 4th clock
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_Cl4_BASE
	movf	SID_SE_GLOBAL_CLK_CTR, W, BANKED
	andlw	0x03
	skpnz
	rcall	SIDSE_Clk_Hlp_PropClkEvent

	;; propagate clock/16 event to trigger matrix on each 16th clock
	lfsr	FSR1, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_C16_BASE
	movf	SID_SE_GLOBAL_CLK_CTR, W, BANKED
	andlw	0x0f
	skpnz
	rcall	SIDSE_Clk_Hlp_PropClkEvent

SIDSE_Clk_Reset_Multi_Cont
SIDSE_Clk_NoReset
SIDSE_Clk_NoInc

SIDSE_Clk_End
	return


;; expecting pointer to SID_Ix_L_TRG_Clk*_BASE in FSR1
SIDSE_Clk_Hlp_PropClkEvent
	movf	POSTINC1, W
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movf	POSTINC1, W
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	POSTINC1, W
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
	return



;; --------------------------------------------------------------------------
;; This function handles the arpeggiators
;; IN: pointer to SID_Ix_e_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     pointer to SIDx_MVx_BASE in FSR2 (MIDI voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_Arp
	;; TODO: portamento handling on note overlaps

	;; clear temporary arp flags
	bcf	SID_SE_STATE, SID_SE_STATE_ARP_NEW_NOTE_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_ARP_FIRST_NOTE_REQ, BANKED
	bcf	SID_SE_STATE, SID_SE_STATE_ARP_GATE_CLR_REQ, BANKED

	;; check if arp sync requested
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_Arp_Sync
	movlw	SID_MVx_ARP_STATE
	BRA_IFCLR PLUSW2, SID_MV_ARP_STATE_SYNC_ARP, ACCESS, SIDSE_Arp_NoSync
SIDSE_Arp_Sync
	;; set arp counters to max value (forces proper reset)
	movlw	SID_MVx_ARP_NOTE_CTR
	setf	PLUSW2
	movlw	SID_MVx_ARP_OCT_CTR
	setf	PLUSW2

	;; reset ARP up flag (will be set to 1 with first note)
	movlw	SID_MVx_ARP_STATE
	bcf	PLUSW2, SID_MV_ARP_STATE_ARP_UP

	;; request first note (for oneshot function)
	bsf	SID_SE_STATE, SID_SE_STATE_ARP_FIRST_NOTE_REQ, BANKED

	;; reset divider if not disabled or if arp synch on MIDI clock start event
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED, SIDSE_Arp_DivReset
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_DIV_SYNC, ACCESS, SIDSE_Arp_NoDivReset
SIDSE_Arp_DivReset
	movlw	SID_MVx_ARP_DIV_CTR
	setf	PLUSW2
	movlw	SID_MVx_ARP_GL_CTR
	setf	PLUSW2
	;; request new note
	bsf	SID_SE_STATE, SID_SE_STATE_ARP_NEW_NOTE_REQ, BANKED
SIDSE_Arp_NoDivReset
SIDSE_Arp_NoSync

	;; if clock sync event:
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_GLOBAL_CLK_EVENT, BANKED, SIDSE_Arp_NoClk
SIDSE_Arp_Clk
	;; increment clock divider
	;; reset divider if it already has reached the target value

	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFCLR PLUSW0, SID_I_V_ARP_MODE_CAC, ACCESS, SIDSE_Arp_Clk_NoCAC
SIDSE_Arp_Clk_CAC
	;; in CAC mode: increment depending on number of pressed keys
	movlw	SID_MVx_NOTE_STACK_PTR	; using note of MV1 note stack
	movf	PLUSW2, W
	skpnz
	movlw	1
	movwf	PRODL
	rgoto	SIDSE_Arp_Clk_CAC_Cont

SIDSE_Arp_Clk_NoCAC
	;; if CAC disabled: increment only by one
	movlw	1
	movwf	PRODL

SIDSE_Arp_Clk_CAC_Cont
	;; add to div counter, temp. store result in PRODL
	movlw	SID_MVx_ARP_DIV_CTR
	movf	PLUSW2, W
	addwf	PRODL, F

	;; max value -> PRODH
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	movf	PLUSW0, W
	andlw	0x3f
	addlw	1
	movwf	PRODH

	;; now subtract PRODH from PRODL until PRODL is < PRODH
SIDSE_Arp_Clk_FixLoop
	decf	PRODH, W
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_Arp_Clk_FixLoop_Break
	movf	PRODH, W
	subwf	PRODL, F	; F-WREG

	;; request new note
	bsf	SID_SE_STATE, SID_SE_STATE_ARP_NEW_NOTE_REQ, BANKED

	;; request gate clear if voice not active anymore (required if Gln>=Speed)
	movlw	SID_Vx_STATE
	btfss	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bsf	SID_SE_STATE, SID_SE_STATE_ARP_GATE_CLR_REQ, BANKED

	rgoto	SIDSE_Arp_Clk_FixLoop

SIDSE_Arp_Clk_FixLoop_Break

	;; transfer new divider value into Vx register
	movlw	SID_MVx_ARP_DIV_CTR
	movff	PRODL, PLUSW2


	;; increment gatelength counter
	;; reset counter if it already has reached the target value
	movlw	SID_MVx_ARP_GL_CTR
	incf	PLUSW2, F
	decf	PLUSW2, W
	movwf	PRODL
	movlw	SID_Ix_Vx_ARP_GL_RNG
	movf	PLUSW0, W
	andlw	0x1f
	addlw	1
	cpfslt	PRODL, ACCESS
	rgoto SIDSE_Arp_Clk_GlReset
	rgoto	SIDSE_Arp_Clk_NoGlReset
SIDSE_Arp_Clk_GlReset
	;; reset counter
	movlw	SID_MVx_ARP_GL_CTR
	clrf	PLUSW2

	;; request gate clear
	bsf	SID_SE_STATE, SID_SE_STATE_ARP_GATE_CLR_REQ, BANKED
SIDSE_Arp_Clk_NoGlReset

SIDSE_Arp_NoClk


	;; check if HOLD mode has been deactivated - disable notes in this case
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_HOLD, ACCESS, SIDSE_Arp_Hold_Active
SIDSE_Arp_Hold_NotActive
	;; check if HOLD flag was active before
	movlw	SID_MVx_ARP_STATE
	BRA_IFCLR PLUSW2, SID_MV_ARP_STATE_HOLD_SAVED, ACCESS, SIDSE_Arp_Hold_Cont
	;; clear HOLD flag and disable all notes
	bcf	PLUSW2, SID_MV_ARP_STATE_HOLD_SAVED
	rgoto	SIDSE_Arp_NotActive

SIDSE_Arp_Hold_Active
	;; store HOLD flag in MIDI voice record
	movlw	SID_MVx_ARP_STATE
	bsf	PLUSW2, SID_MV_ARP_STATE_HOLD_SAVED
SIDSE_Arp_Hold_Cont


	;; skip the rest if arp is disabled
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_ENABLE, ACCESS, SIDSE_Arp_Active
SIDSE_Arp_NotActive
	;; check if arp was active before (for proper 1->0 transition when ARP is disabled)
	movlw	SID_MVx_ARP_STATE
	BRA_IFCLR PLUSW2, SID_MV_ARP_STATE_ARP_ACTIVE, ACCESS, SIDSE_Arp_End

	;; notify that arp is not active anymore
	bcf	PLUSW2, SID_MV_ARP_STATE_ARP_ACTIVE

	;; clear note stack (especially important in HOLD mode!)
	movlw	SID_MVx_NOTE_STACK_PTR
	movwf	PRODL
SIDSE_Arp_NotActive_ClrLoop
	movf	PRODL, W
	clrf	PLUSW2
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_0 + SID_MVx_NOTE_STACK_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SIDSE_Arp_NotActive_ClrLoop

	;; propagate Note Off through trigger matrix
	;; (static for Multi Engine)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	SIDSE_Arp_NotActive_ClrLoop_L
SIDSE_Arp_NotActive_ClrLoop_M
	;; only ENV release phase
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED
	rgoto	SIDSE_Arp_NotActive_ClrLoop_C
SIDSE_Arp_NotActive_ClrLoop_L
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 0, WREG
	andlw	0xc0		; (Gates handled seperately by Arp)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SIDSE_Arp_NotActive_ClrLoop_C

	;; request gate clear
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	rgoto	SIDSE_Arp_End


SIDSE_Arp_Active
	;; notify that arp is active (for proper 1->0 transition when ARP is disabled)
	movlw	SID_MVx_ARP_STATE
	bsf	PLUSW2, SID_MV_ARP_STATE_ARP_ACTIVE

	;; check if voice not active anymore (not valid in HOLD mode) or gate clear has been requested
	;; skip voice active check in hold mode
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_HOLD, ACCESS, SIDSE_Arp_GateClr_Hold
	;; skip voice active check in voice sync mode
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_DIV_SYNC, ACCESS, SIDSE_Arp_GateClr_Sync
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SIDSE_Arp_GateClr
SIDSE_Arp_GateClr_Sync
SIDSE_Arp_GateClr_Hold
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_ARP_GATE_CLR_REQ, BANKED, SIDSE_Arp_NoGateClr
SIDSE_Arp_GateClr
	;; forward this to note handler if gate is not already deactivated
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SIDSE_Arp_NoGateClr
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	;; Note Off ENV/LFO/... synchronisation via trigger matrix
	;; (static for Multi Engine)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	SIDSE_Arp_GateClr_Lead
SIDSE_Arp_GateClr_Multi
	;; only ENV release phase
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED
	rgoto	SIDSE_Arp_GateClr_Multi_Cont
SIDSE_Arp_GateClr_Lead	
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 0, WREG
	andlw	0xc0		; (Gates handled seperately by Arp)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SIDSE_Arp_GateClr_Multi_Cont
SIDSE_Arp_NoGateClr

	;; check if a new arp note has been requested
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_ARP_NEW_NOTE_REQ, BANKED, SIDSE_Arp_NoNewNote
SIDSE_Arp_NewNote
	;; skip if note counter is 0xaa (oneshot mode)
	movlw	SID_MVx_ARP_NOTE_CTR
	movf	PLUSW2, W
	xorlw	0xaa
	skpnz
	rgoto	SIDSE_Arp_NoNewNote

	;; increment arp counter
	movlw	SID_MVx_ARP_NOTE_CTR
	incf	PLUSW2, F

	;; reset gatelength counter
	movlw	SID_MVx_ARP_GL_CTR
	clrf	PLUSW2

	;; if max value of arp note counter reached, reset it
	movlw	SID_MVx_ARP_NOTE_CTR
	movff	PLUSW2, PRODL
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfslt	PRODL, ACCESS
	clrf PRODL

	;; if note is zero, reset arp note counter
	movf	PRODL, W
	addlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW2, W
	skpnz
	clrf	PRODL

	;; don't play if note still zero
	movf	PRODL, W
	addlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW2, W
	skpnz
	rgoto	SIDSE_Arp_NoNewNote

	;; store new arp note counter
	movlw	SID_MVx_ARP_NOTE_CTR
	movff	PRODL, PLUSW2

	;; dir mode 6 (and 7) selects random direction
	movlw	SID_Ix_Vx_ARP_MODE
	rrf	PLUSW0, W
	andlw	0x06
	xorlw	0x06
	bz	SIDSE_Arp_NewNote_Rnd

	;; in DIR_ALT mode: branch depending on Up/Down flag
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_DIR_ALT2, ACCESS, SIDSE_Arp_NewNote_AltDir
	BRA_IFCLR PLUSW0, SID_I_V_ARP_MODE_DIR_ALT, ACCESS, SIDSE_Arp_NewNote_NoAltDir
SIDSE_Arp_NewNote_AltDir
	;; toggle ARP_UP flag each time the arp note counter is zero
	movf	PRODL, W
	bnz	SIDSE_Arp_NewNote_AltDir_NoTog
SIDSE_Arp_NewNote_AltDir_Tog
	movlw	SID_MVx_ARP_STATE
	btg	PLUSW2, SID_MV_ARP_STATE_ARP_UP

	;; if not ALT2 mode: increment PRODL to prevent double played notes
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_DIR_ALT2, ACCESS, SIDSE_Arp_NewNote_AltDir_Alt2
SIDSE_Arp_NewNote_AltDirNoAlt2
	incf	PRODL, F
SIDSE_Arp_NewNote_AltDir_Alt2

	;; reset PRODL again if note is zero (only one note played)
	movf	PRODL, W
	addlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW2, W
	skpnz
	clrf	PRODL

	;; store updated arp note counter
	movlw	SID_MVx_ARP_NOTE_CTR
	movff	PRODL, PLUSW2
SIDSE_Arp_NewNote_AltDir_NoTog

	;; branch depending on Arp Up/Down and Alt Up/Down flag
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW0, SID_I_V_ARP_MODE_DIR_DOWN, ACCESS, SIDSE_Arp_NewNote_AltDir_D
SIDSE_Arp_NewNote_AltDir_U
	movlw	SID_MVx_ARP_STATE
	BRA_IFSET PLUSW2, SID_MV_ARP_STATE_ARP_UP, ACCESS, SIDSE_Arp_NewNote_Up
	rgoto	SIDSE_Arp_NewNote_Down
SIDSE_Arp_NewNote_AltDir_D
	movlw	SID_MVx_ARP_STATE
	BRA_IFSET PLUSW2, SID_MV_ARP_STATE_ARP_UP, ACCESS, SIDSE_Arp_NewNote_Down
	rgoto	SIDSE_Arp_NewNote_Up

SIDSE_Arp_NewNote_NoAltDir

	;; branch depending on direction
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFCLR PLUSW0, SID_I_V_ARP_MODE_DIR_DOWN, ACCESS, SIDSE_Arp_NewNote_Up
SIDSE_Arp_NewNote_Down
	;; offset to last note stack entry: PTR-1-PRODL
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW2, W
	bz	SIDSE_Arp_NewNote_Down_NoNote
	addlw	-1
	movwf	PRODH
	movf	PRODL, W
	subwf	PRODH, W
	movwf	PRODL
SIDSE_Arp_NewNote_Down_NoNote
	rgoto	SIDSE_Arp_NewNote_Down_Cont

SIDSE_Arp_NewNote_Rnd
	;; generate new random number
	call	SID_RND_GenRandomNumber
	;; scale between 0 and SID_MVx_NOTE_STACK_PTR-1
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW2, W	; (ok, no dec!)
	mulwf	SID_RANDOM_SEED_L, BANKED
	movff	PRODH, PRODL	; new pointer in PRODL

SIDSE_Arp_NewNote_Down_Cont
SIDSE_Arp_NewNote_Up

	;; now check for oneshot mode: if note is 0, or if note and oct counter is 0, stop here
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	BRA_IFCLR PLUSW0, SID_I_V_ARP_SPEED_DIV_ONESHOT, ACCESS, SIDSE_Arp_NewNote_NoOneShot
SIDSE_Arp_NewNote_OneShot
	BRA_IFSET SID_SE_STATE, SID_SE_STATE_ARP_FIRST_NOTE_REQ, BANKED, SIDSE_Arp_NewNote_NoOneShot
	movf	PRODL, W
	addlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW2, W
	bz	SIDSE_Arp_NewNote_OneShot_Ok
	movf	PRODL, W
	movlw	SID_MVx_ARP_NOTE_CTR
	movf	PLUSW2, W
	bnz	SIDSE_Arp_NewNote_NoOneShot
	movlw	SID_MVx_ARP_OCT_CTR
	movf	PLUSW2, W
	bnz	SIDSE_Arp_NewNote_NoOneShot

SIDSE_Arp_NewNote_OneShot_Ok
	;; set note counter to 0xaa to stop ARP until next reset
	movlw	0xaa
	movwf	PRODL
	movlw	SID_MVx_ARP_NOTE_CTR
	movff	PRODL, PLUSW2
	rgoto	SIDSE_Arp_NoNewNote
SIDSE_Arp_NewNote_NoOneShot

	;; store new arp note if != 0
	movf	PRODL, W
	addlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW2, W
	andlw	0x7f
	movwf	PRODH
	movlw	SID_Vx_ARP_NOTE
	skpz
	movff	PRODH, PLUSW1

	;; if counter is 0, increase octave until max value is reached
	movf	PRODL, W
	bnz	SIDSE_Arp_NewNote_NoOctInc
SIDSE_Arp_NewNote_OctInc
	movlw	SID_MVx_ARP_OCT_CTR
	incf	PLUSW2, F
	decf	PLUSW2, W
	movwf	PRODL
	movlw	SID_Ix_Vx_ARP_GL_RNG
	swapf	PLUSW0, W
	rrf	WREG, W
	andlw	0x07
	cpfslt	PRODL, ACCESS
	rgoto SIDSE_Arp_NewNote_OctReset
	rgoto	SIDSE_Arp_NewNote_NoOctReset
SIDSE_Arp_NewNote_OctReset
	;; reset octave counter
	movlw	SID_MVx_ARP_OCT_CTR
	clrf	PLUSW2
SIDSE_Arp_NewNote_NoOctReset

SIDSE_Arp_NewNote_NoOctInc
	;; transpose note by 12*octave counter
	movlw	SID_MVx_ARP_OCT_CTR
	movf	PLUSW2, W
	bz	SIDSE_Arp_NewNote_OctNoTrans
	mullw	12
SIDSE_Arp_NewNote_OctTrans
	movlw	SID_Vx_ARP_NOTE
	movf	PLUSW1, W
	addwf	PRODL, W
	movwf	PRODH

	;; if >0x6b, decrement by 12 until we are in 0x00..0x6b range again
	;; (range 0x6c..0x7f sets frequency to 0xffff...)
SIDSE_Arp_NewNote_OctTransSat
	movlw	0x6b
	cpfsgt	PRODH, ACCESS
	rgoto SIDSE_Arp_NewNote_OctTransOk
	movlw	-12
	addwf	PRODH, F
	rgoto	SIDSE_Arp_NewNote_OctTransSat
SIDSE_Arp_NewNote_OctTransOk
	movlw	SID_Vx_ARP_NOTE
	movff	PRODH, PLUSW1
SIDSE_Arp_NewNote_OctNoTrans

	;; forward gate set request if voice is active and gate not active
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SIDSE_Arp_NewNote_NoOn
SIDSE_Arp_NewNote_NoVAChk
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_CLR_REQ	; ensure that gate won't be cleared by previous CLR_REQ
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_ACTIVE, ACCESS, SIDSE_Arp_NewNote_NoOn
SIDSE_Arp_NewNote_On
	;; set gate
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; Note On ENV/LFO/... synchronisation via trigger matrix
	;; (static for Multi Engine)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bz	SIDSE_Arp_NewNote_Lead
SIDSE_Arp_NewNote_Multi
	;; only ENV attack, LFO sync and WT reset
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVA, F, BANKED

	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	2
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED

	movlw	0x3f
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
	;; TODO: optional LFO resync!
	rgoto	SIDSE_Arp_NewNote_Cont

SIDSE_Arp_NewNote_Lead
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 0, WREG
	andlw	0xc0		; (gates handled seperately)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SIDSE_Arp_NewNote_Cont
SIDSE_Arp_NewNote_NoOn

SIDSE_Arp_NoNewNote

SIDSE_Arp_End
	;; clear arp sync flag
	movlw	SID_MVx_ARP_STATE
	bcf	PLUSW2, SID_MV_ARP_STATE_SYNC_ARP

	return



;; --------------------------------------------------------------------------
;; This function handles the Voice Gate
;; IN: pointer to SID_Ix_e_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;;     If SIDSE_Gate_Drums is called: bit #4 of Waveform (disable bit) in IRQ_TMP1
;; OUT: ZERO flag set if pitch should be changed
;; --------------------------------------------------------------------------
SIDSE_Gate
	;; small variation between Lead/Bassline/Multi Engine, and Drum Engine:
	;; for Drum Engine the waveform is predefined in model table, and not
	;; part of the patch (drum engine has to call SIDSE_Gate_Drums with
	;; waveform[4] in IRQ_TMP1)
	movlw	SID_Ix_Vx_WAVEFORM
	movff	PLUSW0, IRQ_TMP1

SIDSE_Gate_Drums		; expecting waveform[4] in IRQ_TMP1

	;; transfer pointer to SIDx_Vx_FRQ_L register -> FSR2
	call	SIDSE_Hlp_GetSIDFrqPtr

	;; voice disable handling (allows to turn on/off voice via waveform parameter)
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_DISABLED, ACCESS, SIDSE_Gate_VoiceNotDisabled
SIDSE_Gate_VoiceDisabled
	BRA_IFSET IRQ_TMP1, 4, ACCESS, SIDSE_Gate_Voice_Cont	; IRQ_TMP1 == SID_Ix_Vx_WAVEFORM
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_DISABLED
	btfsc	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bsf	PLUSW1, SID_V_STATE_GATE_SET_REQ
	rgoto	SIDSE_Gate_Voice_Cont
SIDSE_Gate_VoiceNotDisabled
	BRA_IFCLR IRQ_TMP1, 4, ACCESS, SIDSE_Gate_Voice_Cont	; IRQ_TMP1 == SID_Ix_Vx_WAVEFORM
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_DISABLED
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ
	;; 	rgoto	SIDSE_Gate_Voice_Cont
SIDSE_Gate_Voice_Cont

	;; if gate not active: ignore clear request
	movlw	SID_Vx_STATE
	btfss	PLUSW1, SID_V_STATE_GATE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; gate set/clear request?
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_CLR_REQ, ACCESS, SIDSE_Gate_ClrReq
	BRA_IFSET PLUSW1, SID_V_STATE_GATE_SET_REQ, ACCESS, SIDSE_Gate_SetReq
	rgoto	SIDSE_Gate_Skip
SIDSE_Gate_ClrReq
	bcf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	movlw	SIDx_V1_CTRL
	bcf	PLUSW2, 3	; SIDx_Vx_CTRL.3 is the test bit which allows to sync an oscillator

	;; clear SID gate flag (SIDx_Vx_CTRL.0) if GSA (gate stays active) function not enabled
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFSET PLUSW0, SID_I_V_FLAGS1_GSA, ACCESS, SIDSE_Gate_ClrReq_GSA
SIDSE_Gate_ClrReq_NoGSA
	movlw	SIDx_V1_CTRL
	bcf	PLUSW2, 0
SIDSE_Gate_ClrReq_GSA

	;; gate not active anymore
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_ACTIVE

	;; sync with SID_SR handler
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_SR_UPDATE_SYNC, F, BANKED	; (cleared by SID_SR_Handler)

	rgoto	SIDSE_Gate_End

SIDSE_Gate_SetReq
	;; skip so long SRs haven't been updated
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_SR_UPDATE_SYNC, W, BANKED
	bnz	SIDSE_Gate_Skip

	;; don't set gate if oscillator disabled
	BRA_IFSET IRQ_TMP1, 4, ACCESS, SIDSE_Gate_SetReqSkp	; IRQ_TMP1 == SID_Ix_Vx_WAVEFORM

	;; temporary shift FSR1 to SID_Vx_SET_DELAY_CTR_L for easier handling
	movlw	SID_Vx_SET_DELAY_CTR_L
	addwf	FSR1L, F

	;; delay note so long 16bit delay counter != 0
	movf	POSTINC1, W
	iorwf	POSTDEC1, W
	bz	SIDSE_Gate_SetReq_NoDelay
SIDSE_Gate_SetReq_Delay
	;; increment counter, set it to zero on overrun (no delay anymore)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x02
	bnz	SIDSE_Gate_SetReq_Delay_LBM
SIDSE_Gate_SetReq_Delay_D
	clrf	IRQ_TMP3	; not available for drum engine
	rgoto	SIDSE_Gate_SetReq_Delay_Cont
SIDSE_Gate_SetReq_Delay_LBM
	movlw	SID_Ix_Vx_DELAY
	movff	PLUSW0, IRQ_TMP3; incrementer is the same like used for envelopes
SIDSE_Gate_SetReq_Delay_Cont

	;; if ABW (ADSR bug workaround) active: use at least 30 ms delay
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFCLR WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_Gate_SetReq_DelayNoA
SIDSE_Gate_SetReq_DelayABW
	movlw	25
	addwf	IRQ_TMP3, F
	skpnc
	setf	IRQ_TMP3
SIDSE_Gate_SetReq_DelayNoA

	movlw	0x80		; curve *must* be disabled!
	call	SIDSE_Hlp_ENV_GetBendedValue	; incrementer in MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	addwf	POSTINC1, F
	movf	MIOS_PARAMETER2, W
	addwfc	POSTDEC1, F
	bc	SIDSE_Gate_SetReq_DelayOv
	movlw	-SID_Vx_SET_DELAY_CTR_L	; switch back to SID_Vx_CTR_L
	addwf	FSR1L, F
#if 0
	rgoto	SIDSE_Gate_End
#else
	;; experimental: don't modify frequency so long delay is active!
	;; this is especially to avoid, that a new note frequency will already be set before the envelope is released
	rgoto	SIDSE_Gate_End_NoPitch
#endif
SIDSE_Gate_SetReq_DelayOv
	;; overrun: clear counter to disable delay
	clrf	POSTINC1
	clrf	POSTDEC1
SIDSE_Gate_SetReq_NoDelay
	movlw	-SID_Vx_SET_DELAY_CTR_L	; switch back to SID_Vx_CTR_L
	addwf	FSR1L, F

	;; now acknowledge the set request
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; if ABW (ADSR bug workaround) function active: update ADSR registers now!
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_OPT1_FLAGS, WREG
	BRA_IFCLR WREG, SID_I_OPT1_FLAGS_ABW, ACCESS, SIDSE_Gate_SetReq_NoABW
SIDSE_Gate_SetReq_ABW
	movlw	SID_Ix_Vx_AD
	movff	PLUSW0, PRODL
	movlw	SIDx_V1_ENV_AD
	movff	PRODL, PLUSW2

	;; accent only used in bassline and drum mode
	;; force sustain to maximum if accent active
	movlw	SID_Ix_Vx_SR
	movff	PLUSW0, PRODL
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_ACCENT, ACCESS, SIDSE_Gate_SetReq_ABW_NoAcc
SIDSE_Gate_SetReq_ABW_Acc
	movlw	0xf0
	iorwf	PRODL, F
SIDSE_Gate_SetReq_ABW_NoAcc
	movlw	SIDx_V1_ENV_SR
	movff	PRODL, PLUSW2
SIDSE_Gate_SetReq_NoABW

	;; branch for phase synchronisation: set test flag instead of gate
	;; a special handler in sid_sr.inc will take care about the rest

	;; different handling depending on selected engine:	
	;; lead engine: global sync for all voices with phase shift
	;; multi engine has individual flag for each instrument
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SIDSE_Gate_SetReq_Sync_DM
SIDSE_Gate_SetReq_Sync_LB
	BRA_IFSET WREG, 0, ACCESS, SIDSE_Gate_SetReq_Sync_B
SIDSE_Gate_SetReq_Sync_L
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_OSC_PHASE, WREG
	andlw	0xff
	bz	SIDSE_Gate_SetReq_NoSync
	rgoto	SIDSE_Gate_SetReq_Sync_Cont

SIDSE_Gate_SetReq_Sync_B
	movlw	SID_Ix_B_Vx_FLAGS2
	BRA_IFCLR PLUSW0, SID_I_M_V_FLAGS2_PHASE, ACCESS, SIDSE_Gate_SetReq_NoSync
	rgoto	SIDSE_Gate_SetReq_NoSync

SIDSE_Gate_SetReq_Sync_DM
	BRA_IFSET WREG, 0, ACCESS, SIDSE_Gate_SetReq_Sync_M
SIDSE_Gate_SetReq_Sync_D
	;; synchronisation requested in SIDSE_D_NOTE_Restart
	rgoto	SIDSE_Gate_SetReq_NoSync

SIDSE_Gate_SetReq_Sync_M
	movlw	SID_Ix_M_Vx_FLAGS2
	BRA_IFCLR PLUSW0, SID_I_M_V_FLAGS2_PHASE, ACCESS, SIDSE_Gate_SetReq_NoSync
	;; 	rgoto	SIDSE_Gate_SetReq_Sync_Cont

SIDSE_Gate_SetReq_Sync_Cont
	;; set SID test flag (SIDx_Vx_CTRL.3), ensure that gate flag not set
	movlw	SIDx_V1_CTRL
	bsf	PLUSW2, 3
	bcf	PLUSW2, 0

	;; notify sync request
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_PHASE_SYNC_REQ, F, BANKED

	rgoto	SIDSE_Gate_SetReq_NoGate

SIDSE_Gate_SetReq_NoSync
	;; set SID gate flag (SIDx_Vx_CTRL.0)
	movlw	SIDx_V1_CTRL
	bsf	PLUSW2, 0

SIDSE_Gate_SetReq_NoGate
SIDSE_Gate_SetReqSkp
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_ACTIVE

SIDSE_Gate_End
SIDSE_Gate_Skip
	iorlw	0xff		; clear ZERO flag (-> pitch should be changed)
	return


SIDSE_Gate_End_NoPitch
	andlw	0x00		; set ZERO flag (-> pitch should not be changed)
	return


;; --------------------------------------------------------------------------
;; This function handles the Voice Pitch
;; IN: pointer to SID_Ix_e_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_Pitch

	;; ------------------------------------------------------------------
	;; Transpose MIDI Note
	;; ------------------------------------------------------------------
SIDSE_Pitch_Transp
	;; if arp mode is active: use SID_Vx_ARP_NOTE, otherwise SID_Vx_NOTE
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFCLR PLUSW0, SID_I_V_ARP_MODE_ENABLE, ACCESS, SIDSE_Pitch_Transp_NoArp
SIDSE_Pitch_Transp_Arp
	movlw	SID_Vx_ARP_NOTE
	rgoto	SIDSE_Pitch_Transp_Cont
SIDSE_Pitch_Transp_NoArp
	movlw	SID_Vx_NOTE
	;; 	rgoto	SIDSE_Pitch_Transp_Cont
SIDSE_Pitch_Transp_Cont
	movff	PLUSW1, SID_SE_TRANSPOSED_NOTE

	;; pointer to MIDI note -> FSR2
	lfsr	FSR2, SID_MV1_BASE
	movlw	SID_Vx_ASSIGNED_MV
	movf	PLUSW1, W
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F

	;; add voice based transpose to MIDI voice based transpose and saturate
	;; if lead engine: transpose value taken from first MIDI voice!
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SIDSE_Pitch_Transp_NoBassline
SIDSE_Pitch_Transp_Bassline
	;; if WTO enabled, note is not transposed via MIDI voice (instead the sequence number will be transposed)
	movlw	SID_Ix_B_Vx_FLAGS2
	BRA_IFCLR PLUSW0, SID_I_B_V_FLAGS2_WT_ONLY, ACCESS, SIDSE_Pitch_Transp_NoLead
	movlw	0x40
	movwf	IRQ_TMP1
	rgoto	SIDSE_Pitch_Transp_Bassline_Seq
SIDSE_Pitch_Transp_NoBassline
	xorlw	0x01
	bnz	SIDSE_Pitch_Transp_NoLead
SIDSE_Pitch_Transp_Lead
	lfsr	FSR2, SID_MV1_BASE
SIDSE_Pitch_Transp_NoLead
	movlw	SID_MVx_TRANSPOSE
	movff	PLUSW2, IRQ_TMP1
SIDSE_Pitch_Transp_Bassline_Seq
	movlw	SID_Ix_Vx_TRANSPOSE
	movf	PLUSW0, W
	addwf	IRQ_TMP1, W
	sublw	0x80
	xorlw	0xff
	movwf	IRQ_TMP1

	;; transpose note
	addwf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	addlw	1
	BRA_IFCLR WREG, 7, ACCESS, SIDSE_Pitch_Transp_NoOverflow
SIDSE_Pitch_Transp_Overflow
	btfss	IRQ_TMP1, 7; pos saturation
	movlw 0x7f	
	btfsc	IRQ_TMP1, 7; neg saturation
	movlw 0x00	
SIDSE_Pitch_Transp_NoOverflow
	movwf	SID_SE_TRANSPOSED_NOTE, BANKED


	;; ------------------------------------------------------------------
	;; Glissando Handling
	;; ------------------------------------------------------------------
	movlw	SID_Vx_OLD_TRANSP_NOTE		; store new transposed note?
	movf	PLUSW1, W
	cpfseq	SID_SE_TRANSPOSED_NOTE, BANKED
	rgoto SIDSE_Pitch_GlissNewNote
	rgoto	SIDSE_Pitch_GlissNoNewNote
SIDSE_Pitch_GlissNewNote
	movlw	SID_Vx_OLD_TRANSP_NOTE		; store new transposed note!
	movff	SID_SE_TRANSPOSED_NOTE, PLUSW1

	;; init portamento/glissando counter if glissando active
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFCLR PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_GlissNoNewNote
	movlw	SID_Vx_PORTA_CTR_L
	setf	PLUSW1		; force overrun
	movlw	SID_Vx_PORTA_CTR_H
	setf	PLUSW1
SIDSE_Pitch_GlissNoNewNote

	;; portamento active?
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_PORTA_ACTIVE, ACCESS, SIDSE_Pitch_Gliss_Final

	;; glissando active?
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFCLR PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_Gliss_Final	

	;; get portamento multiplier from envelope table -> MUL_A
	;; this one is used for "constant time glide" and "normal portamento"
	movlw	SID_Ix_Vx_PORTAMENTO
	movf	PLUSW0, W
	skpnz
	rgoto	SIDSE_Pitch_Gliss_Final	; if rate has been changed to zero, set target frequency and finish portamento
	clrc
	rrf	WREG, W		; make it faster
	TABLE_ADDR_MUL_W SID_ENV_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MUL_A_[LH]
	movff	TABLAT, MUL_A_L
	tblrd*+
	movff	TABLAT, MUL_A_H

	;; increment portamento counter by MUL_A_[LH], result in MUL_A_[LH]
	movlw	SID_Vx_PORTA_CTR_L
	movf	PLUSW1, W
	addwf	MUL_A_L, F, BANKED
	movlw	SID_Vx_PORTA_CTR_H
	movf	PLUSW1, W
	addwfc	MUL_A_H, F, BANKED
	bc	SIDSE_Pitch_Gliss_Ov
SIDSE_Pitch_Gliss_NoOv
	;; store new counter and continue with current note
	movlw	SID_Vx_PORTA_CTR_L
	movff	MUL_A_L, PLUSW1
	movlw	SID_Vx_PORTA_CTR_H
	movff	MUL_A_H, PLUSW1
	movlw	SID_Vx_TRANSP_NOTE
	movff	PLUSW1, SID_SE_TRANSPOSED_NOTE
	rgoto	SIDSE_Pitch_Gliss_End

SIDSE_Pitch_Gliss_Ov
	;; reset portamento/glissando counter
	movlw	SID_Vx_PORTA_CTR_L
	clrf	PLUSW1
	movlw	SID_Vx_PORTA_CTR_H
	clrf	PLUSW1

	;; increment/decrement note
	movlw	SID_Vx_TRANSP_NOTE
	movf	PLUSW1, W
	movwf	IRQ_TMP1
	cpfsgt	SID_SE_TRANSPOSED_NOTE, BANKED
	rgoto SIDSE_Pitch_Gliss_Dec
SIDSE_Pitch_Gliss_Inc
	incf	IRQ_TMP1, F	; increment note and check if final value reached
	rgoto	SIDSE_Pitch_Gliss_Inc_Cont
SIDSE_Pitch_Gliss_Dec
	decf	IRQ_TMP1, F	; decrement note and check if final value reached
	;; 	rgoto	SIDSE_Pitch_Gliss_Inc_Cont

SIDSE_Pitch_Gliss_Inc_Cont
	BRA_IFSET IRQ_TMP1, 7, ACCESS, SIDSE_Pitch_Gliss_Final
	movf	IRQ_TMP1, W
	cpfseq	SID_SE_TRANSPOSED_NOTE, BANKED
	rgoto SIDSE_Pitch_Gliss_NewNote
	rgoto	SIDSE_Pitch_Gliss_Final
SIDSE_Pitch_Gliss_NewNote
	movff	IRQ_TMP1, SID_SE_TRANSPOSED_NOTE
	movlw	SID_Vx_TRANSP_NOTE
	movff	SID_SE_TRANSPOSED_NOTE, PLUSW1
	rgoto	SIDSE_Pitch_Gliss_End

SIDSE_Pitch_Gliss_Final
	movlw	SID_Vx_OLD_TRANSP_NOTE	; target note reached
	movff	PLUSW1, SID_SE_TRANSPOSED_NOTE
	movlw	SID_Vx_TRANSP_NOTE
	movff	SID_SE_TRANSPOSED_NOTE, PLUSW1

	;; deactivate porta active gate if glissando active
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFCLR PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_Gliss_End
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
SIDSE_Pitch_Gliss_End

	;; ------------------------------------------------------------------
	;; Determine Target frequency depending on transposed note
	;; ------------------------------------------------------------------
SIDSE_Pitch_InitTargetFrq
	;; use linear value if O2A activated
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL2_O2A, ACCESS, SIDSE_Pitch_InitTargetFrq_NoO2A
SIDSE_Pitch_InitTargetFrq_O2A
	clrf	SID_SE_TARGET_FRQ_L, BANKED
	clrc
	rlf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	movwf	SID_SE_TARGET_FRQ_H, BANKED
	rgoto	SIDSE_Pitch_InitTargetFrq_O2A_C

SIDSE_Pitch_InitTargetFrq_NoO2A
	movf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to SID_SE_TARGET_FRQ_[LH]
	movff	TABLAT, SID_SE_TARGET_FRQ_L
	tblrd*+
	movff	TABLAT, SID_SE_TARGET_FRQ_H
SIDSE_Pitch_InitTargetFrq_O2A_C

	;; ------------------------------------------------------------------
	;; Increase/Decrease target frequency by pitchrange depending on
	;; Pitchbender and Finetune value
	;; ------------------------------------------------------------------

	;; skip Pitchbender+Finetune processing if PITCHRANGE == zero
	movlw	SID_Ix_Vx_PITCHRANGE
	movf	PLUSW0, W
	skpnz
	rgoto	SIDSE_Pitch_NoOffset
SIDSE_Pitch_Offset
	;; determine scale multipler (9 bit signed value) -> MUL_B_L
	clrf	MUL_B_L, BANKED
	clrf	MUL_B_H, BANKED

	;; multi/bassline/drum engine: take pitchbender value from MIDI voice (pointer still in FSR2)
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	BRA_IFSET WREG, 1, ACCESS, SIDSE_Pitch_PB_DM
SIDSE_Pitch_PB_LB
	BRA_IFSET WREG, 0, ACCESS, SIDSE_Pitch_PB_B
SIDSE_Pitch_PB_L
	movlw	SID_Vx_PITCHBENDER
	movf	PLUSW1, W
	rgoto	SIDSE_Pitch_PB_Cont

SIDSE_Pitch_PB_B
SIDSE_Pitch_PB_DM
	movlw	SID_MVx_PITCHBENDER
	movf	PLUSW2, W
	;; 	rgoto	SIDSE_Pitch_PB_Cont

SIDSE_Pitch_PB_Cont
	movwf	IRQ_TMP1
	bz	SIDSE_Pitch_NoPitchBender
SIDSE_Pitch_PitchBender
	clrc
	rlf	IRQ_TMP1, W
	addwf	MUL_B_L, F, BANKED
	movlw	0x00
	btfsc	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_B_H, F, BANKED
SIDSE_Pitch_NoPitchBender

	movlw	SID_Ix_Vx_FINETUNE
	movf	PLUSW0, W
	movwf	IRQ_TMP1
	xorlw	0x80
	bz	SIDSE_Pitch_NoFinetune
SIDSE_Pitch_Finetune
	clrc
	rlf	IRQ_TMP1, W
	addwf	MUL_B_L, F, BANKED
	movlw	0x00
	btfss	IRQ_TMP1, 7
	movlw 0xff
	addwfc	MUL_B_H, F, BANKED
SIDSE_Pitch_NoFinetune

	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	bnz	SIDSE_Pitch_NoDetune
	SET_BSR	SID_PATCH_BUFFER_SHADOW
	movf	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_OSC_DETUNE, W, BANKED
	SET_BSR	SID_BASE
	bz	SIDSE_Pitch_NoDetune
SIDSE_Pitch_Detune
	movwf	IRQ_TMP1
	;; additional detuning depending on SID channel and oscillator
	;; Left OSC1: +detune/4
	;; Right OSC1: -detune/4
	;; Left OSC2: +detune
	;; Right OSC2: -detune
	;; Left OSC3: -detune
	;; Right OSC3: +detune

	BRA_IFSET SID_SE_ELEMENT_NUM, 2, BANKED, SIDSE_Pitch_Detune_45
SIDSE_Pitch_Detune_0123
	BRA_IFSET SID_SE_ELEMENT_NUM, 1, BANKED, SIDSE_Pitch_Detune_23
SIDSE_Pitch_Detune_01
	BRA_IFSET SID_SE_ELEMENT_NUM, 0, BANKED, SIDSE_Pitch_Detune_1
SIDSE_Pitch_Detune_0	; SIDL, OSC1
	;; in mono mode: don't detune OSC1, so that at least one oscillator runs at the target frequency!
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	BRA_IFSET WREG, SID_ENS_CTRL1_MONO, ACCESS, SIDSE_Pitch_NoDetune
	clrc
	rrf	IRQ_TMP1, F
	clrc
	rrf	IRQ_TMP1, F
	rgoto	SIDSE_Pitch_Detune_Add
SIDSE_Pitch_Detune_1	; SIDL, OSC2
	rgoto	SIDSE_Pitch_Detune_Add

SIDSE_Pitch_Detune_23
	BRA_IFSET SID_SE_ELEMENT_NUM, 0, BANKED, SIDSE_Pitch_Detune_3
SIDSE_Pitch_Detune_2	; SIDL, OSC3
	rgoto	SIDSE_Pitch_Detune_Sub
SIDSE_Pitch_Detune_3	; SIDR, OSC1
	clrc
	rrf	IRQ_TMP1, F
	clrc
	rrf	IRQ_TMP1, F
	rgoto	SIDSE_Pitch_Detune_Sub
	
SIDSE_Pitch_Detune_45
	BRA_IFSET SID_SE_ELEMENT_NUM, 0, BANKED, SIDSE_Pitch_Detune_5
SIDSE_Pitch_Detune_4	; SIDR, OSC2
	rgoto	SIDSE_Pitch_Detune_Sub
SIDSE_Pitch_Detune_5	; SIDR, OSC3
	rgoto	SIDSE_Pitch_Detune_Add

SIDSE_Pitch_Detune_Sub
	comf	IRQ_TMP1, W
	addlw	1
	addwf	MUL_B_L, F, BANKED
	movf	IRQ_TMP1, W
	skpz
	movlw	0xff
	addwfc	MUL_B_H, F, BANKED
	rgoto	SIDSE_Pitch_Detune_Cont

SIDSE_Pitch_Detune_Add
	movf	IRQ_TMP1, W
	addwf	MUL_B_L, F, BANKED
	movlw	0x00
	addwfc	MUL_B_H, F, BANKED
	;; 	rgoto	SIDSE_Pitch_Detune_Cont
SIDSE_Pitch_Detune_Cont
SIDSE_Pitch_NoDetune


	;; skip tuning if scale multiplier is zero
	movf	MUL_B_L, W, BANKED
	iorwf	MUL_B_H, W, BANKED
	bz	SIDSE_Pitch_NoOffset

	;; get f_in[target], save it in MIOS_PARAMETER[12]
	;; add pitchrange depending on direction with saturation
	BRA_IFSET MUL_B_H, 7, BANKED, SIDSE_Pitch_Frq_Dec
SIDSE_Pitch_Frq_Inc
	movlw	SID_Ix_Vx_PITCHRANGE
	movf	PLUSW0, W
	addwf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	btfsc	WREG, 7
	movlw 0x7f
	rgoto	SIDSE_Pitch_Frq_Inc_Cont
SIDSE_Pitch_Frq_Dec
	movlw	SID_Ix_Vx_PITCHRANGE
	movf	PLUSW0, W
	subwf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	btfsc	WREG, 7
	movlw 0x00
SIDSE_Pitch_Frq_Inc_Cont

	;; use linear value if O2A activated
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, PRODL
	BRA_IFCLR PRODL, SID_ENS_CTRL2_O2A, ACCESS, SIDSE_Pitch_Frq_TuneFrq_NoO2A
SIDSE_Pitch_Frq_TuneFrq_O2A
	clrf	MIOS_PARAMETER1
	clrc
	rlf	WREG, W
	movwf	MIOS_PARAMETER2
	rgoto	SIDSE_Pitch_Frq_TuneFrq_O2A_C

SIDSE_Pitch_Frq_TuneFrq_NoO2A
	addlw	21			; due to the new frequency table we have to transpose
	btfsc	WREG, 7; the note value
	movlw 0x7f	
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MIOS_PARAMETER[12]
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2
SIDSE_Pitch_Frq_TuneFrq_O2A_C


	;; add and multiply to target frequency
SIDSE_Pitch_Frq_AddMul
	BRA_IFSET MUL_B_H, 7, BANKED, SIDSE_Pitch_Frq_AddMul_Neg
SIDSE_Pitch_Frq_AddMul_Pos
	;; calc MUL_A_[LH] = MIOS_PARAMETER[12] - SID_Vx_FRQ_[LH]
	movf	SID_SE_TARGET_FRQ_L, W, BANKED
	subwf	MIOS_PARAMETER1, W
	movwf	MUL_A_L, BANKED
	movf	SID_SE_TARGET_FRQ_H, W, BANKED
	subwfb	MIOS_PARAMETER2, W
	movwf	MUL_A_H, BANKED

	;; ensure that bit #15 (sign) not set
	BRA_IFCLR MUL_A_H, 7, BANKED, SIDSE_Pitch_Frq_AddMul_Cont
	setf	MUL_A_L, BANKED
	movlw	0x7f
	movwf	MUL_A_H, BANKED
	rgoto	SIDSE_Pitch_Frq_AddMul_Cont

SIDSE_Pitch_Frq_AddMul_Neg
	;; calc MUL_A_[LH] = SID_Vx_FRQ_[LH] - MIOS_PARAMETER[12]
	movf	MIOS_PARAMETER1, W
	subwf	SID_SE_TARGET_FRQ_L, W, BANKED
	movwf	MUL_A_L, BANKED
	movf	MIOS_PARAMETER2, W
	subwfb	SID_SE_TARGET_FRQ_H, W, BANKED
	movwf	MUL_A_H, BANKED
	;; 	rgoto	SIDSE_Pitch_Frq_AddMul_Cont

SIDSE_Pitch_Frq_AddMul_Cont
	;; calc MUL_R_[12] = MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16_SIGNED

	;; SID_Vx_FRQ += signed result[23:8]
	movf	MUL_R_1, W, BANKED
	addwf	SID_SE_TARGET_FRQ_L, F, BANKED
	movf	MUL_R_2, W, BANKED
	addwfc	SID_SE_TARGET_FRQ_H, F, BANKED

SIDSE_Pitch_NoOffset
	;; store target frequency in Vx
	movlw	SID_Vx_TARGET_FRQ_L
	movff	SID_SE_TARGET_FRQ_L, PLUSW1
	movlw	SID_Vx_TARGET_FRQ_H
	movff	SID_SE_TARGET_FRQ_H, PLUSW1


	;; ------------------------------------------------------------------
	;; pitch modulation
	;; ------------------------------------------------------------------
	rcall	SIDSE_Hlp_Pitch_Mod

	;; ------------------------------------------------------------------
	;; Portamento Handling
	;; ------------------------------------------------------------------

	;; transfer pointer to SIDx_Vx_FRQ_L register -> FSR2
	call	SIDSE_Hlp_GetSIDFrqPtr

SIDSE_Pitch_Porta
	;; whenever target frequency has been changed, update portamento frequency
	movlw	SID_Vx_OLD_TARGET_FRQ_L
	movf	PLUSW1, W
	cpfseq	SID_SE_TARGET_FRQ_L, BANKED
	rgoto SIDSE_Pitch_Porta_Update
	movlw	SID_Vx_OLD_TARGET_FRQ_H
	movf	PLUSW1, W
	cpfseq	SID_SE_TARGET_FRQ_H, BANKED
	rgoto SIDSE_Pitch_Porta_Update
	rgoto	SIDSE_Pitch_Porta_NoUpdate
SIDSE_Pitch_Porta_Update
	;; memorize new target frequency
	movlw	SID_Vx_OLD_TARGET_FRQ_L
	movff	SID_SE_TARGET_FRQ_L, PLUSW1
	movlw	SID_Vx_OLD_TARGET_FRQ_H
	movff	SID_SE_TARGET_FRQ_H, PLUSW1

	;; skip portamento effect if current frequency is 0 (after patch change)
	movf	POSTINC2, W
	iorwf	POSTDEC2, W
	skpnz
	rgoto	SIDSE_Pitch_Porta_Final
	
	;; store current frequency (SIDx_FRQ_[LH]) in SID_Vx_PORTA_FRQ_[LH]
	movlw	SID_Vx_PORTA_FRQ_L
	movff	POSTINC2, PLUSW1
	movlw	SID_Vx_PORTA_FRQ_H
	movff	POSTDEC2, PLUSW1

	;; skip if glissando active
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFSET PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_Porta_NoUpdate

	;; reset portamento counter
	movlw	SID_Vx_PORTA_CTR_L
	clrf	PLUSW1
	movlw	SID_Vx_PORTA_CTR_H
	clrf	PLUSW1
SIDSE_Pitch_Porta_NoUpdate

	;; skip the rest if portamento not active
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_PORTA_ACTIVE, ACCESS, SIDSE_Pitch_Porta_Final

	;; skip if glissando active
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFSET PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_Porta_Final

	;; get portamento multiplier from envelope table -> MUL_A
	;; this one is used for "constant time glide" and "normal portamento"
	movlw	SID_Ix_Vx_PORTAMENTO
	movf	PLUSW0, W
	skpnz
	rgoto	SIDSE_Pitch_Porta_Final	; if rate has been changed to zero, set target frequency and finish portamento
	TABLE_ADDR_MUL_W SID_ENV_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MUL_A_[LH]
	movff	TABLAT, MUL_A_L
	tblrd*+
	movff	TABLAT, MUL_A_H

	;; branch depending on portamento mode
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFCLR PLUSW0, SID_I_V_FLAGS1_PORTA_CTG, ACCESS, SIDSE_Pitch_Porta_Norm

	;; ------------------------------------------------------------------
	;; constant glide time
SIDSE_Pitch_Porta_Const

	;; increment portamento counter by MUL_A_[LH], result in MUL_A_[LH]
	movlw	SID_Vx_PORTA_CTR_L
	movf	PLUSW1, W
	addwf	MUL_A_L, F, BANKED
	movlw	SID_Vx_PORTA_CTR_H
	movf	PLUSW1, W
	addwfc	MUL_A_H, F, BANKED
	bnc	SIDSE_Pitch_Porta_Const_NoOv
SIDSE_Pitch_Porta_Const_Ov
	;; if value >= 0xffff target reached, saturate
	setf	MUL_A_L, BANKED
	setf	MUL_A_H, BANKED
SIDSE_Pitch_Porta_Const_NoOv

	;; transfer back new counter value to original registers
	;; portamento counter value -> MUL_A_[LH]
	movlw	SID_Vx_PORTA_CTR_L
	movff	MUL_A_L, PLUSW1
	movlw	SID_Vx_PORTA_CTR_H
	movff	MUL_A_H, PLUSW1

	;; get difference between target and previous frequency -> IRQ_TMP[12]
	movlw	SID_Vx_PORTA_FRQ_L
	movf	PLUSW1, W
	subwf	SID_SE_TARGET_FRQ_L, W, BANKED
	movwf	IRQ_TMP1
	movlw	SID_Vx_PORTA_FRQ_H
	movf	PLUSW1, W
	subwfb	SID_SE_TARGET_FRQ_H, W, BANKED
	movwf	IRQ_TMP2

	;; convert IRQ_TMP[12] to absolute value
	call	SIDSE_Hlp_GetAbs16
	;; result in IRQ_TMP[12], sign in IRQ_TMP3[0]
	;; increment four to ensure that target will be reached
	movlw	4
	addwf	IRQ_TMP1, F
	movlw	0
	addwfc	IRQ_TMP2, F
	movff	IRQ_TMP1, MUL_B_L
	movff	IRQ_TMP2, MUL_B_H

	;; calc MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)

	;; branch depending on direction
	BRA_IFSET IRQ_TMP3, 0, ACCESS, SIDSE_Pitch_Porta_Const_Down
SIDSE_Pitch_Porta_Const_Up
	;; add scaled value to starting frequency
	movlw	SID_Vx_PORTA_FRQ_L
	movf	PLUSW1, W
	addwf	MUL_R_2, W, BANKED
	movwf	POSTINC2

	movlw	SID_Vx_PORTA_FRQ_H
	movf	PLUSW1, W
	addwfc	MUL_R_3, W, BANKED
	movwf	POSTDEC2

	;; continue at normal portamento routine
	rgoto	SIDSE_Pitch_Porta_Const_Up_Cont

SIDSE_Pitch_Porta_Const_Down
	;; subtract scaled value from starting frequency
	movlw	SID_Vx_PORTA_FRQ_L
	movff	PLUSW1, POSTINC2
	movlw	SID_Vx_PORTA_FRQ_H
	movff	PLUSW1, POSTDEC2

	movf	MUL_R_2, W, BANKED
	subwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	subwfb	POSTDEC2, F

	;; continue at normal portamento routine
	rgoto	SIDSE_Pitch_Porta_Const_Down_C


	;; ------------------------------------------------------------------
	;; "normal" portamento mode (non-constant glide time)
SIDSE_Pitch_Porta_Norm
	;; get current frequency -> MUL_B
	movff	POSTINC2, MUL_B_L
	movff	POSTDEC2, MUL_B_H

	;; multiply MUL_A * MUL_B
	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)
	;; ensure that result is != 0
	movf	MUL_R_2, W, BANKED
	iorwf	MUL_R_3, W, BANKED
	skpnz
	incf	MUL_R_2, F, BANKED

	;; SID_Vx_FRQ += result (depending on Portamento Direction)

	;; branch depending on portamento direction
	;; check if value > current value
	movf	SID_SE_TARGET_FRQ_L, W, BANKED
	subwf	POSTINC2, W
	movf	SID_SE_TARGET_FRQ_H, W, BANKED
	subwfb	POSTDEC2, W
	bc	SIDSE_Pitch_Porta_Norm_Down

SIDSE_Pitch_Porta_Norm_Up
	movf	MUL_R_2, W
	addwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	addwfc	POSTDEC2, F

SIDSE_Pitch_Porta_Const_Up_Cont		; re-used by "constant glide" option
	;; check if value > MAX_VALUE
	movf	POSTINC2, W
	subwf	SID_SE_TARGET_FRQ_L, W, BANKED
	movf	POSTDEC2, W
	subwfb	SID_SE_TARGET_FRQ_H, W, BANKED
	bc	SIDSE_Pitch_Porta_End	; branch to end if MAX_VALUE not reached
	rgoto	SIDSE_Pitch_Porta_Final	; final value reached

SIDSE_Pitch_Porta_Norm_Down
	movf	MUL_R_2, W
	subwf	POSTINC2, F
	movf	MUL_R_3, W, BANKED
	subwfb	POSTDEC2, F

SIDSE_Pitch_Porta_Const_Down_C	; re-used by "constant glide" option
	;; check if value < MIN_VALUE
	movf	SID_SE_TARGET_FRQ_L, W, BANKED
	subwf	POSTINC2, W
	movf	SID_SE_TARGET_FRQ_H, W, BANKED
	subwfb	POSTDEC2, W
	bc	SIDSE_Pitch_Porta_End	; branch to end if MIN_VALUE not reached
	;; final value reached

SIDSE_Pitch_Porta_Final
	;; copy target frequency to SIDx_FRQ_[LH] and finish portamento
	movff	SID_SE_TARGET_FRQ_L, POSTINC2
	movff	SID_SE_TARGET_FRQ_H, POSTDEC2

	;; deactivate porta active gate if glissando not active
	movlw	SID_Ix_Vx_FLAGS1
	BRA_IFSET PLUSW0, SID_I_V_FLAGS1_PORTA_GLISSANDO, ACCESS, SIDSE_Pitch_Porta_End
	movlw	SID_Vx_STATE
 	bcf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
SIDSE_Pitch_Porta_End


	;; ------------------------------------------------------------------
	;; K2A Function
	;; ------------------------------------------------------------------

	;; forward key value to AOUT if enabled
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL2_K2A, ACCESS, SIDSE_Pitch_Key_NoAOUT
SIDSE_Pitch_Key_AOUT
	;; get channel assignment (0: channel disabled)
	movlw	LOW(SIDSE_KEY_EXT_ASSG_TABLE)
	addwf	SID_SE_ELEMENT_NUM, W, BANKED
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SIDSE_KEY_EXT_ASSG_TABLE)
	addwfc	TBLPTRH, F
	clrf    TBLPTRU
        movlw   UPPER(SIDSE_KEY_EXT_ASSG_TABLE)
	addwfc	TBLPTRU, F
	tblrd*+
	movf	TABLAT, W
	bz	SIDSE_Pitch_Key_NoAOUT	; channel assignment disabled

	;; copy key value to MIOS_PARAMETER[12]
	clrf	MIOS_PARAMETER1
	clrc
	rlf	SID_SE_TRANSPOSED_NOTE, W, BANKED
	movwf	MIOS_PARAMETER2
	
	;; forward MIOS_PARAMETER[12] to channel #TABLAT-1
	decf	TABLAT, W
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by K2A function
	decf	TABLAT, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_Pitch_Key_NoAOUT

	;; ------------------------------------------------------------------
	;; O2A Function
	;; ------------------------------------------------------------------

	;; forward key value to AOUT if enabled
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL2_O2A, ACCESS, SIDSE_Pitch_Osc_NoAOUT
SIDSE_Pitch_Osc_AOUT
	;; get channel assignment (0: channel disabled)
	movlw	LOW(SIDSE_OSC_EXT_ASSG_TABLE)
	addwf	SID_SE_ELEMENT_NUM, W, BANKED
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SIDSE_OSC_EXT_ASSG_TABLE)
	addwfc	TBLPTRH, F
	clrf    TBLPTRU
        movlw   UPPER(SIDSE_OSC_EXT_ASSG_TABLE)
	addwfc	TBLPTRU, F
	tblrd*+
	movf	TABLAT, W
	bz	SIDSE_Pitch_Osc_NoAOUT	; channel assignment disabled

	;; copy oscillator frequency to MIOS_PARAMETER[12]
	movff	POSTINC2, MIOS_PARAMETER1
	movff	POSTDEC2, MIOS_PARAMETER2

	;; forward MIOS_PARAMETER[12] to channel #TABLAT-1
	decf	TABLAT, W
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by O2A function
	decf	TABLAT, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_Pitch_Osc_NoAOUT

	return


	;; AOUT assignments for K2A
SIDSE_KEY_EXT_ASSG_TABLE
	db	DEFAULT_K2A_OSC1_L_AOUT, DEFAULT_K2A_OSC2_L_AOUT
	db	DEFAULT_K2A_OSC3_L_AOUT, DEFAULT_K2A_OSC1_R_AOUT
	db	DEFAULT_K2A_OSC2_L_AOUT, DEFAULT_K2A_OSC3_R_AOUT

	;; AOUT assignments for O2A
SIDSE_OSC_EXT_ASSG_TABLE
	db	DEFAULT_O2A_OSC1_L_AOUT, DEFAULT_O2A_OSC2_L_AOUT
	db	DEFAULT_O2A_OSC3_L_AOUT, DEFAULT_O2A_OSC1_R_AOUT
	db	DEFAULT_O2A_OSC2_L_AOUT, DEFAULT_O2A_OSC3_R_AOUT


;; --------------------------------------------------------------------------
;; This function handles the Pulsewidth of a voice
;; IN: pointer to SID_Ix_L_SxVy_BASE in FSR0 (patch record)
;;     pointer to SIDx_Vx_BASE in FSR1 (voice record)
;;     Voice number in SID_SE_ELEMENT_NUM
;;     If SIDSE_PW_Drums is called: 16bit pulsewidth in IRQ_TMP[12]
;; --------------------------------------------------------------------------
SIDSE_PW
	;; small variation between Lead/Bassline/Multi Engine, and Drum Engine:
	;; for Drum Engine the pulsewidth is predefined in model table, and not
	;; part of the patch (drum engine has to call SIDSE_PW_Drums with
	;; 16bit pulsewidth in IRQ_TMP[12]

	;; store 12bit pulsewidth in IRQ_TMP[12]
	movlw	SID_Ix_Vx_PULSEWIDTH_L
	movff	PLUSW0, IRQ_TMP1
	movlw	SID_Ix_Vx_PULSEWIDTH_H
	movf	PLUSW0, W
	andlw	0x0f
	movwf	IRQ_TMP2

	;; extend to 16bit
	swapf	IRQ_TMP2, F
	swapf	IRQ_TMP1, W
	andlw	0x0f
	iorwf	IRQ_TMP2, F
	swapf	IRQ_TMP1, W
	andlw	0xf0
	movwf	IRQ_TMP1

SIDSE_PW_Drums
	;; transfer pointer to SIDx_Vx_PW_L register -> FSR2
	call	SIDSE_Hlp_GetSIDPWPtr

	;; calculate pointer to MOD target array -> FSR2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_PW1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR1, SID_MOD_TARG_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	;; add modulation value to pulsewidth
	movf	POSTINC1, W
	addwf	IRQ_TMP1, F
	movf	POSTINC1, W
	addwfc	IRQ_TMP2, F

	;; saturate
	BRA_IFSET INDF1, 7, ACCESS, SIDSE_PW_Mod_SatNeg
SIDSE_PW_Mod_SatPos
	movf	INDF1, W
	bnz	SIDSE_PW_Mod_SatPos_Sat
	bnc	SIDSE_PW_Mod_Sat_NoSat
SIDSE_PW_Mod_SatPos_Sat
	setf	IRQ_TMP1
	setf	IRQ_TMP2
	rgoto	SIDSE_PW_Mod_Sat_Cont
SIDSE_PW_Mod_SatNeg
	comf	INDF1, W
	bnz	SIDSE_PW_Mod_SatNeg_Sat
	bc	SIDSE_PW_Mod_Sat_NoSat
SIDSE_PW_Mod_SatNeg_Sat
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
	;; 	rgoto	SIDSE_PW_Mod_Sat_Cont
SIDSE_PW_Mod_Sat_Cont
SIDSE_PW_Mod_Sat_NoSat

	;; convert back to 12bit value -> IRQ_TMP[34]
	swapf	IRQ_TMP1, W
	andlw	0x0f
	movwf	IRQ_TMP3
	swapf	IRQ_TMP2, W
	andlw	0xf0
	iorwf	IRQ_TMP3, F
	swapf	IRQ_TMP2, W
	andlw	0x0f
	movwf	IRQ_TMP4

	;; transfer to SID registers
	movff	IRQ_TMP3, POSTINC2
	movff	IRQ_TMP4, POSTDEC2

	;; forward pulsewidth to AOUT if enabled
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL2_P2A, ACCESS, SIDSE_PW_NoAOUT
SIDSE_PW_AOUT
	;; get channel assignment (0: channel disabled)
	movlw	LOW(SIDSE_PW_EXT_ASSG_TABLE)
	addwf	SID_SE_ELEMENT_NUM, W, BANKED
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SIDSE_PW_EXT_ASSG_TABLE)
	addwfc	TBLPTRH, F
	clrf    TBLPTRU
        movlw   UPPER(SIDSE_PW_EXT_ASSG_TABLE)
	addwfc	TBLPTRU, F
	tblrd*+
	movf	TABLAT, W
	bz	SIDSE_PW_NoAOUT		; channel assignment disabled
	
	;; forward IRQ_TMP[12] to channel #TABLAT-1
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movff	IRQ_TMP2, MIOS_PARAMETER2
	decf	TABLAT, W
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by P2A function
	decf	TABLAT, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_PW_NoAOUT
	return


	;; AOUT assignments for P2A
SIDSE_PW_EXT_ASSG_TABLE
	db	DEFAULT_P2A_OSC1_L_AOUT, DEFAULT_P2A_OSC2_L_AOUT
	db	DEFAULT_P2A_OSC3_L_AOUT, DEFAULT_P2A_OSC1_R_AOUT
	db	DEFAULT_P2A_OSC2_L_AOUT, DEFAULT_P2A_OSC3_R_AOUT

;; --------------------------------------------------------------------------
;; This function handles the Filter
;; IN: pointer to SID_Ix_L_SxF_BASE in FSR0 (patch record)
;;     pointer to SID_MOD_TARG_FILx_L in FSR1 (modulation target)
;;     pointer to SIDx_BASE in FSR2 (SID registers)
;;     Filter number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_Filter
	;; store 12bit cutoff in IRQ_TMP[12]
	movlw	SID_Ix_L_Fx_CUTOFF_L
	movff	PLUSW0, IRQ_TMP1
	movlw	SID_Ix_L_Fx_CUTOFF_H
	movf	PLUSW0, W
	andlw	0x0f
	movwf	IRQ_TMP2

	;; extend to 16bit
	swapf	IRQ_TMP2, F
	swapf	IRQ_TMP1, W
	andlw	0x0f
	iorwf	IRQ_TMP2, F
	swapf	IRQ_TMP1, W
	andlw	0xf0
	movwf	IRQ_TMP1

	;; pointer to SID_MOD_TARG_FILx_L already in FSR1
	;; multiplty target value by 2 (for easy to realize extreme modulation results)
	;; -> IRQ_TMP[345]
	movf	INDF1, W
	addwf	POSTINC1, W
	movwf	IRQ_TMP3
	movf	INDF1, W
	addwfc	POSTINC1, W
	movwf	IRQ_TMP4
	movf	INDF1, W
	addwfc	POSTINC1, W
	movwf	IRQ_TMP5
	
	;; add modulation value to cutoff
	movf	IRQ_TMP3, W
	addwf	IRQ_TMP1, F
	movf	IRQ_TMP4, W
	addwfc	IRQ_TMP2, F

	;; saturate
	BRA_IFSET IRQ_TMP5, 7, ACCESS, SIDSE_Filter_Mod_SatNeg
SIDSE_Filter_Mod_SatPos
	movf	IRQ_TMP5, W
	bnz	SIDSE_Filter_Mod_SatPos_Sat
	bnc	SIDSE_Filter_Mod_Sat_NoSat
SIDSE_Filter_Mod_SatPos_Sat
	setf	IRQ_TMP1
	setf	IRQ_TMP2
	rgoto	SIDSE_Filter_Mod_Sat_Cont
SIDSE_Filter_Mod_SatNeg
	comf	IRQ_TMP5, W
	bnz	SIDSE_Filter_Mod_SatNeg_Sat
	bc	SIDSE_Filter_Mod_Sat_NoSat
SIDSE_Filter_Mod_SatNeg_Sat
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
	;; 	rgoto	SIDSE_Filter_Mod_Sat_Cont
SIDSE_Filter_Mod_Sat_Cont
SIDSE_Filter_Mod_Sat_NoSat

	;; in bassline mode: add keytracking * NOTE value
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	xorlw	0x01
	bnz	SIDSE_Filter_KTr_Skip
SIDSE_Filter_KTr
	movlw	SID_Ix_L_Fx_KEYTRACK
	movf	PLUSW0, W
	bz	SIDSE_Filter_KTr_Skip
	movwf	PRODL

	movff	SIDL_V1_BASE + SID_Vx_OLD_TRANSP_NOTE, WREG
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	movff	SIDR_V1_BASE + SID_Vx_OLD_TRANSP_NOTE, WREG
	andlw	0x7f
	mulwf	PRODL, ACCESS

	;; add to filter and saturate if required (value always positive)
	movf	PRODL, W
	addwf	IRQ_TMP1, F
	movf	PRODH, W
	addwfc	IRQ_TMP2, F
	bnc	SIDSE_Filter_KTr_NoSat	
SIDSE_Filter_KTr_Sat
	setf	IRQ_TMP1
	setf	IRQ_TMP2
SIDSE_Filter_KTr_NoSat
SIDSE_Filter_KTr_Skip


	;; copy SID_Gx_CALI_FIL[12]_[MIN|MAX]_[LH] into IRQ_TMP[34]/PROD[LH] depending on selected filter
	BRA_IFSET SID_SE_ELEMENT_NUM, 0, BANKED, SIDSE_Filter_MinMax2
SIDSE_Filter_MinMax1
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL1_MIN_L, IRQ_TMP3
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL1_MIN_H, IRQ_TMP4
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL1_MAX_L, PRODL
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL1_MAX_H, PRODH
	rgoto	SIDSE_Filter_MinMax_Cont
SIDSE_Filter_MinMax2
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL2_MIN_L, IRQ_TMP3
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL2_MIN_H, IRQ_TMP4
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL2_MAX_L, PRODL
	movff	SID_LOCAL_ENS + SID_ENSx_CALI_FIL2_MAX_H, PRODH
	;; 	rgoto	SIDSE_Filter_MinMax_Cont
SIDSE_Filter_MinMax_Cont

	;; scale between min/max value
	movf	IRQ_TMP3, W	; SID_Gx_CALI_FIL1_MIN_L
	subwf	PRODL, W	; SID_Gx_CALI_FIL1_MAX_L
	movff	WREG, MUL_A_L
	movf	IRQ_TMP4, W	; SID_Gx_CALI_FIL1_MIN_H
	subwfb	PRODH, W	; SID_Gx_CALI_FIL1_MAX_H
	movff	WREG, MUL_A_H
	movff	IRQ_TMP1, MUL_B_L
	movff	IRQ_TMP2, MUL_B_H
	;; calc MUL_A_[LH] * MUL_B_[LH]
	call	MATH_MUL16_16
	;; result in MUL_R_2 (low-byte) and MUL_R_3 (high-byte)
	movf	IRQ_TMP3, W	; SID_Gx_CALI_FIL1_MIN_L
	addwf	MUL_R_2, W, BANKED
	movwf	IRQ_TMP1
	movf	IRQ_TMP4, W	; SID_Gx_CALI_FIL1_MIN_H
	addwfc	MUL_R_3, W, BANKED
	movwf	IRQ_TMP2

	;; optional interpolation
	movlw	SID_Ix_L_Fx_CUTOFF_H
	BRA_IFCLR PLUSW0, SID_I_F_FIP_ON, ACCESS, SIDSE_Filter_NoIP
SIDSE_Filter_IP
	lfsr	FSR1, FIP1_BASE
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	skpz
	lfsr	FSR1, FIP2_BASE
	rcall	SIDSE_Hlp_IP
SIDSE_Filter_NoIP

	;; convert to 11bit value (SID format: 3 lowes bits in bit [2:0] of low-register)
	swapf	IRQ_TMP1, W
	rrf	WREG, W
	andlw	0x07
	movwf	IRQ_TMP3

	;; transfer to SID registers
	movlw	SIDx_FC_L
	movff	IRQ_TMP3, PLUSW2
	movlw	SIDx_FC_H
	movff	IRQ_TMP2, PLUSW2

	;; Forward CutOff and Resonance to AOUT if enabled
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, WREG
	BRA_IFCLR WREG, SID_ENS_CTRL2_F2A, ACCESS, SIDSE_Filter_NoAOUT
SIDSE_Filter_AOUT
	;; expecting 16bit value in IRQ_TMP[12] and pointer to AOUT register in FSR1
#if DEFAULT_F2A_CUTOFF_L_AOUT == 0 || DEFAULT_F2A_CUTOFF_L_AOUT > 8
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_AOUT_NoCutOff
#endif
#if DEFAULT_F2A_CUTOFF_R_AOUT == 0 || DEFAULT_F2A_CUTOFF_R_AOUT > 8
	decf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_AOUT_NoCutOff
#endif

	;; forward IRQ_TMP[12] to channel predefined channel
	movff	IRQ_TMP1, MIOS_PARAMETER1
	movff	IRQ_TMP2, MIOS_PARAMETER2
	movlw	DEFAULT_F2A_CUTOFF_L_AOUT-1
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	movlw	DEFAULT_F2A_CUTOFF_R_AOUT-1
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by F2A function
	;; (for faster handling, do this for all enabled channels each time this function is called)
	movlw	0x00
#if DEFAULT_F2A_CUTOFF_L_AOUT
	iorlw	(1 << (DEFAULT_F2A_CUTOFF_L_AOUT-1))
#endif
#if DEFAULT_F2A_CUTOFF_R_AOUT
	iorlw	(1 << (DEFAULT_F2A_CUTOFF_R_AOUT-1))
#endif
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_Filter_AOUT_NoCutOff

#if DEFAULT_F2A_RESONANCE_L_AOUT == 0 || DEFAULT_F2A_RESONANCE_L_AOUT > 8
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_AOUT_NoResonance
#endif
#if DEFAULT_F2A_RESONANCE_R_AOUT == 0 || DEFAULT_F2A_RESONANCE_R_AOUT > 8
	decf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_AOUT_NoResonance
#endif

	;; resonance (8bit)
	movlw	SID_Ix_L_Fx_RESONANCE
	movff	PLUSW0, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1
	movlw	DEFAULT_F2A_RESONANCE_L_AOUT-1
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	movlw	DEFAULT_F2A_RESONANCE_R_AOUT-1
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by F2A function
	;; (for faster handling, do this for all enabled channels each time this function is called)
	movlw	0x00
#if DEFAULT_F2A_RESONANCE_L_AOUT
	iorlw	(1 << (DEFAULT_F2A_RESONANCE_L_AOUT-1))
#endif
#if DEFAULT_F2A_RESONANCE_R_AOUT
	iorlw	(1 << (DEFAULT_F2A_RESONANCE_R_AOUT-1))
#endif
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_Filter_AOUT_NoResonance
SIDSE_Filter_NoAOUT

	;; Channels and Resonance
	movlw	SID_Ix_L_Fx_CHN_MODE
	movf	PLUSW0, W
	andlw	0x0f
	movwf	PRODL
	movlw	SID_Ix_L_Fx_RESONANCE
	movf	PLUSW0, W
	andlw	0xf0
	iorwf	PRODL, F
	movlw	SIDx_RES_FCHN
	movff	PRODL, PLUSW2

	;; volume and filter mode
	
	;; volume can be modulated (result in IRQ_TMP[12])
	clrf	IRQ_TMP1
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_VOLUME, IRQ_TMP2
	clrc
	rlf	IRQ_TMP2, F

	;; calculate pointer to MOD target array -> FSR2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_VOL1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR1, SID_MOD_TARG_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movf	POSTINC1, W
	addwf	IRQ_TMP1, F
	movf	POSTINC1, W
	addwfc	IRQ_TMP2, F

	;; saturate
	BRA_IFSET INDF1, 7, ACCESS, SIDSE_Filter_Mod_Vol_SatNeg
SIDSE_Filter_Mod_Vol_SatPos
	movf	INDF1, W
	bnz	SIDSE_Filter_Mod_Vol_SatPos_Sat
	bnc	SIDSE_Filter_Mod_Vol_NoSat
SIDSE_Filter_Mod_Vol_SatPos_Sat
	setf	IRQ_TMP1
	setf	IRQ_TMP2
	rgoto	SIDSE_Filter_Mod_Vol_Sat_Cont
SIDSE_Filter_Mod_Vol_SatNeg
	comf	INDF1, W
	bnz	SIDSE_Filter_Mod_Vol_SatNeg_Sat
	bc	SIDSE_Filter_Mod_Vol_NoSat
SIDSE_Filter_Mod_Vol_SatNeg_Sat
	clrf	IRQ_TMP1
	clrf	IRQ_TMP2
	;; 	rgoto	SIDSE_Filter_Mod_Vol_Sat_Cont
SIDSE_Filter_Mod_Vol_Sat_Cont
SIDSE_Filter_Mod_Vol_NoSat

	;; copy SID_Gx_CTRL1 to IRQ_TMP3 - we need this to check if V2A is enabled
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL2, IRQ_TMP3

	;; patch volume -> PRODL
	;; volume always maximum when V2A enabled
	swapf	IRQ_TMP2, W
	andlw	0x0f
	btfsc	IRQ_TMP3, SID_ENS_CTRL2_V2A
	movlw 0x0f
	movwf	PRODL

	movlw	SID_Ix_L_Fx_CHN_MODE
	movf	PLUSW0, W
	andlw	0xf0
	iorwf	PRODL, F
	movlw	SIDx_MODE_VOL
	movff	PRODL, PLUSW2

	;; Forward Volume to AOUT if enabled
	BRA_IFCLR IRQ_TMP3, SID_ENS_CTRL2_V2A, ACCESS, SIDSE_Filter_Vol_NoAOUT
SIDSE_Filter_Vol_AOUT
	;; expecting 16bit value in IRQ_TMP[12] and pointer to AOUT register in FSR1
#if DEFAULT_V2A_VOLUME_L_AOUT == 0 || DEFAULT_V2A_VOLUME_L_AOUT > 8
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_Vol_NoAOUT
#endif
#if DEFAULT_V2A_VOLUME_R_AOUT == 0 || DEFAULT_V2A_VOLUME_R_AOUT > 8
	decf	SID_SE_ELEMENT_NUM, W, BANKED
	bz	SIDSE_Filter_Vol_NoAOUT
#endif

	movff	IRQ_TMP1, MIOS_PARAMETER1
	movff	IRQ_TMP2, MIOS_PARAMETER2
	movlw	DEFAULT_V2A_VOLUME_L_AOUT-1
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	movlw	DEFAULT_V2A_VOLUME_R_AOUT-1
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

	;; notify that channel has already been updated by V2A function
	;; (for faster handling, do this for all enabled channels each time this function is called)
	movlw	0x00
#if DEFAULT_V2A_VOLUME_L_AOUT
	iorlw	(1 << (DEFAULT_V2A_VOLUME_L_AOUT-1))
#endif
#if DEFAULT_V2A_VOLUME_R_AOUT
	iorlw	(1 << (DEFAULT_V2A_VOLUME_R_AOUT-1))
#endif
	iorwf	SID_SE_EXT_ALLOCATED, F, BANKED
SIDSE_Filter_Vol_NoAOUT

	return


;; --------------------------------------------------------------------------
;; This function handles the AOUTs
;; Used by Bassline/Drum and Multi Engine
;; IN: pointer to SID_Ix_EXT_PARx_L in FSR0 (patch record)
;;     EXT number in SID_SE_ELEMENT_NUM
;; --------------------------------------------------------------------------
SIDSE_BDM_EXT
	;; skip if any F2X function has already updated the AOUTx register
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SID_SE_EXT_ALLOCATED, W, BANKED
	bnz	SIDSE_BDM_EXT_End

	;; store 16bit offset in MIOS_PARAMETER[12]
	movff	POSTINC0, MIOS_PARAMETER1
	movff	POSTDEC0, MIOS_PARAMETER2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	call	AOUT_Pin16bitSet
	SET_BSR	SID_BASE

SIDSE_BDM_EXT_End
	return


;; --------------------------------------------------------------------------
;; This function updates the 8 external switches (called once per update cycle)
;; IN: -
;; --------------------------------------------------------------------------
SIDSE_EXT_Switches
	lfsr	FSR1, GATES			; for easier addressing

	;; copy switch state from patch to GATES
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_CUSTOM_SW, INDF1

	;; help macro to improve oversight
SIDSE_EXT_SWITCHES_FORWARD MACRO addr, flag, out
	;; overlay with various gate functions if enabled
  IF out && (out <= 8)
	bcf	INDF1, 	out-1
	btfsc	addr, flag, BANKED
	bsf	INDF1, 	out-1
  ENDIF
	ENDM

	SET_BSR	SIDL_V1_BASE			; BSR set to SIDx_Vx_BASE for direct access

	SIDSE_EXT_SWITCHES_FORWARD SIDL_V1_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC1_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V2_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC2_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V3_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC3_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V1_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC1_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V2_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC2_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V3_BASE + SID_Vx_STATE, SID_V_STATE_GATE_ACTIVE, DEFAULT_GATE_OSC3_R_OUT

	SIDSE_EXT_SWITCHES_FORWARD SIDL_V1_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC1_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V2_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC2_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V3_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC3_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V1_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC1_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V2_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC2_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V3_BASE + SID_Vx_STATE, SID_V_STATE_SLIDE, DEFAULT_SLIDE_OSC3_R_OUT

	SIDSE_EXT_SWITCHES_FORWARD SIDL_V1_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC1_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V2_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC2_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDL_V3_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC3_L_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V1_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC1_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V2_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC2_R_OUT
	SIDSE_EXT_SWITCHES_FORWARD SIDR_V3_BASE + SID_Vx_STATE, SID_V_STATE_ACCENT, DEFAULT_ACCENT_OSC3_R_OUT

	SET_BSR	SID_BASE			; switch BSR back to SID base
	return


;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  Help Functions which are used by all engines
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; Help Function: Scale Depth (7bit signed value scales 15bit signed value)
;; IN:  8bit signed depth value (+0x80) in WREG
;;      15bit signed value in MUL_A_[LH]
;; OUT: result in MUL_R_[123]
;; USES: TABLAT
;; --------------------------------------------------------------------------
SIDSE_Hlp_ScaleDepth
	movwf	TABLAT		; temporary store depth value in TABLAT

	rcall	SIDSE_Hlp_GetAbs8
	clrc
	rlf	WREG, W
	movwf	MUL_B_L, BANKED
	clrf	MUL_B_H, BANKED

	;; 16*16 multiplication, result in MUL_R_[0123]
	call	MATH_MUL16_16_SIGNED

	;; invert result if required
	BRA_IFSET TABLAT, 7, ACCESS, SIDSE_Hlp_ScaleDepth_Cont
SIDSE_Hlp_ScaleDepth_Neg
	comf	MUL_R_1, BANKED
	comf	MUL_R_2, BANKED
	comf	MUL_R_3, BANKED
	incf	MUL_R_1, F, BANKED
	skpnz
	incf	MUL_R_2, F, BANKED
	skpnz
	incf	MUL_R_3, F, BANKED
SIDSE_Hlp_ScaleDepth_Cont
	return


;; --------------------------------------------------------------------------
;; Like SIDSE_Hlp_ScaleDepth, but depth is increased/decreased by SID_Ix_Vx_ACCENT
;; if the SID_SE_STATE, SID_SE_STATE_ACCENT flag is set
;; IN:  8bit signed depth value (+0x80) in WREG
;;      15bit signed value in MUL_A_[LH]
;;      pointer to patch structure in FSR0
;; OUT: result in MUL_R_[123]
;; USES: TABLAT
;; --------------------------------------------------------------------------
SIDSE_Hlp_ScaleDepthAcc
	;; check accent flag
	BRA_IFCLR SID_SE_STATE, SID_SE_STATE_ACCENT, BANKED, SIDSE_Hlp_ScaleDepth

	movwf	MUL_R_1, BANKED	; (used as temporary register)

	;; increase/decrease value by 0x40 and saturate
	BRA_IFCLR WREG, 7, ACCESS, SIDSE_Hlp_ScaleDepthAcc_Neg
SIDSE_Hlp_ScaleDepthAcc_Pos
	movlw	SID_Ix_Vx_ACCENT
	movf	PLUSW0, W
	addwf	MUL_R_1, W, BANKED
	btfss	WREG, 7
	movlw 0xff
	rgoto	SIDSE_Hlp_ScaleDepth
SIDSE_Hlp_ScaleDepthAcc_Neg
	movlw	SID_Ix_Vx_ACCENT
	comf	PLUSW0, W
	addlw	1
	btfsc	WREG, 7
	movlw 0x00
	rgoto	SIDSE_Hlp_ScaleDepth

;; --------------------------------------------------------------------------
;; Help Function: Get absolute value
;; IN:  signed 16-bit value in IRQ_TMP[12]
;; Out: unsigned absolute value in IRQ_TMP[12]
;;      sign in IRQ_TMP3[0]
;; --------------------------------------------------------------------------
SIDSE_Hlp_GetAbs16
	;; convert IRQ_TMP[12] to unsigned integer, keep sign in IRQ_TMP3[0]
	clrf	IRQ_TMP3
	BRA_IFCLR IRQ_TMP2, 7, ACCESS, SIDSE_Hlp_GetABS16_Pos
SIDSE_Hlp_GetABS16_Neg
	bsf	IRQ_TMP3, 0	; memorize sign in IRQ_TMP3[0]
	comf	IRQ_TMP1, F
	comf	IRQ_TMP2, F
	incf	IRQ_TMP1, F
	skpnz
	incf	IRQ_TMP2, F
SIDSE_Hlp_GetABS16_Pos
	return


;; --------------------------------------------------------------------------
;; Help Function which returns the absolute value
;; IN:  8-bit signed value in WREG
;; OUT:	absolute value (0x00-0x7f) in WREG
;; --------------------------------------------------------------------------
SIDSE_Hlp_GetAbs8
	movf	WREG, W
	skpnz
	addlw	1
	btfss	WREG, 7
	sublw 0x80
	andlw	0x7f
	return

;; --------------------------------------------------------------------------
;; Help Function for SIDSE_ENV to increase or decrease the ENV counter
;; IN:  incr value in MIOS_PARAMETER[12]
;;	target level in IRQ_TMP1
;;      pointer to SID_ENVx_BASE in FSR1
;; OUT:	zero flag set if max value reached
;; --------------------------------------------------------------------------
SIDSE_Hlp_ENV_IncrDecrCtr
	;; if current level > target level, branch to decr function
	movlw	SID_ENVx_CTR_H
	movf	PLUSW1, W
	cpfslt	IRQ_TMP1, ACCESS
	rgoto SIDSE_Hlp_ENV_IncrCtr
	rgoto	SIDSE_Hlp_ENV_DecrCtr


;; --------------------------------------------------------------------------
;; Help Function for SIDSE_ENV to increase the ENV counter
;; IN:  incr value in MIOS_PARAMETER[12]
;;	max level in IRQ_TMP1
;;      pointer to SID_ENVx_BASE in FSR1
;; OUT:	zero flag set if max value reached
;; --------------------------------------------------------------------------
SIDSE_Hlp_ENV_IncrCtr
	;; add to ENV counter
	movf	POSTINC1, W	; SID_ENVx_CTR_L
	addwf	MIOS_PARAMETER1, F
	movf	POSTDEC1, W	; SID_ENVx_CTR_H
	addwfc	MIOS_PARAMETER2, F
	bc	SIDSE_Hlp_ENV_IncrCtr_Max

	;; if value >= level: switch to next phase
	movf	MIOS_PARAMETER1, W
	sublw	0x00
	movf	MIOS_PARAMETER2, W
	subwfb	IRQ_TMP1, W
	bc	SIDSE_Hlp_ENV_IncrCtr_NoMax

SIDSE_Hlp_ENV_IncrCtr_Max
	;; write level to MIOS_PARAMETER[12]
	movff	IRQ_TMP1, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	andlw	0x00		; set zero flag and exit
	return

SIDSE_Hlp_ENV_IncrCtr_NoMax
	iorlw	0xff		; clear zero flag and exit
	return


;; --------------------------------------------------------------------------
;; Help Function for SIDSE_ENV to decrease the ENV counter
;; IN:  decr value in MIOS_PARAMETER[12]
;;	min level in IRQ_TMP1
;;      pointer to SID_ENVx_BASE in FSR1
;; OUT:	zero flag set if min value reached
;; --------------------------------------------------------------------------
SIDSE_Hlp_ENV_DecrCtr
	;; subtraction from ENV counter
	movf	MIOS_PARAMETER1, W
	subwf	POSTINC1, W	; SID_ENVx_CTR_L
	movwf	MIOS_PARAMETER1
	movf	MIOS_PARAMETER2, W
	subwfb	POSTDEC1, W	; SID_ENVx_CTR_H
	movwf	MIOS_PARAMETER2
	bnc	SIDSE_Hlp_ENV_DecrCtr_Min

	;; if value < level: switch to next phase
	movlw	0x00
	subwf	MIOS_PARAMETER1, W
	movf	IRQ_TMP1, W
	subwfb	MIOS_PARAMETER2, W
	bc	SIDSE_Hlp_ENV_DecrCtr_NoMin

SIDSE_Hlp_ENV_DecrCtr_Min
	;; write level to MIOS_PARAMETER[12]
	movff	IRQ_TMP1, MIOS_PARAMETER2
	clrf	MIOS_PARAMETER1

	andlw	0x00		; set zero flag and exit
	return

SIDSE_Hlp_ENV_DecrCtr_NoMin
	iorlw	0xff		; clear zero flag and exit
	return

;; --------------------------------------------------------------------------
;; Help Function for SIDSE_ENV
;; IN:  pointer to SID_Ix_L_ENVx_BASE in FSR0 (patch record)
;;      pointer to SID_ENVx_BASE in FSR1
;;      curve value in WREG (0x80: no curve)
;;	MBFM_Ix_ENVx_ATTACKx/MBFM_Ix_ENVx_DECAYx or MBFM_Ix_ENVx_SUSTAINx in IRQ_TMP3
;;
;;      Can also be used to determine the delay increment value
;;      In this case, only IRQ_TMP3 (Ix_xxx_Delay) is expected in IRQ_TMP3, and 0x80 in WREG (no curve!)
;; 
;; OUT:	value which should be added to - or subtracted from - SID_ENVx_CTR_[LH]
;;      low-byte in MIOS_PARAMETER1; high-byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SIDSE_Hlp_ENV_GetBendedValue
	;; get curve parameter and store it in IRQ_TMP1
	movwf	IRQ_TMP1
	xorlw	0x80		; if != 0x80, we apply the curve, otherwise we have a linear waveform
 	bnz	SIDSE_Hlp_ENV_GetBendedValue_Crv

	;; curve not selected, get value from ENV_TABLE
	movf	IRQ_TMP3, W
	TABLE_ADDR_MUL_W SID_ENV_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MUL_A_[LH]
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2
	return

SIDSE_Hlp_ENV_GetBendedValue_Crv
	;; copy current counter value to IRQ_TMP2
	movlw	SID_ENVx_CTR_H
	movff	PLUSW1, IRQ_TMP2

	;; rightshift IRQ_TMP3 (7bit value range)
	clrc
	rrf	IRQ_TMP3, F

	;; feedback: calculate ABS8(CURVE) * ENV_x_CTR_H
	movf	IRQ_TMP1, W		; get absolute value of curve parameter
	rcall	SIDSE_Hlp_GetAbs8
	btfsc	IRQ_TMP1, 7; invert if positive range (for more logical behaviour of positive/negative curve)
	xorlw 0x7f 
	mulwf	IRQ_TMP2, ACCESS	; multiply with current counter value
	
	;; when CURVE parameter < 0x80: bend down, else up
	BRA_IFCLR IRQ_TMP1, 7, ACCESS, SIDSE_Hlp_ENV_GetBendedValueDown
SIDSE_Hlp_ENV_GetBendedValueUp
	comf	IRQ_TMP3, F
	bcf	IRQ_TMP3, 7
	movf	PRODH, W
	subwf	IRQ_TMP3, W
	btfsc	WREG, 7
	movlw 0x00
	rgoto	SIDSE_Hlp_ENV_GetBendedValueCont

SIDSE_Hlp_ENV_GetBendedValueDown
	comf	IRQ_TMP3, W
	andlw	0x7f
	addwf	PRODH, W
	btfsc	WREG, 7
	movlw 0x7f
	;; 	rgoto	SIDSE_Hlp_ENV_GetBendedValue_Cont

SIDSE_Hlp_ENV_GetBendedValueCont
	andlw	0x7f
	TABLE_ADDR_MUL_W SID_FRQ_TABLE, 2	; determine table address
	tblrd*+				; transfer table entry to MIOS_PARAMETER[12]
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_Hlp_GetSIDFrqPtr
;;  DESCRIPTION: returns pointer to SIDx_V1_FRQ_L register
;;  IN: SID_SE_ELEMENT_NUM
;;  OUT: pointer in FSR2
;; --------------------------------------------------------------------------
SIDSE_Hlp_GetSIDFrqPtr
	BRA_IFSET SID_SE_ELEMENT_NUM, 2, BANKED, SIDSE_Hlp_GetSIDFrqPtr_4567
SIDSE_Hlp_GetSIDFrqPtr_0123
	BRA_IFSET SID_SE_ELEMENT_NUM, 1, BANKED, SIDSE_Hlp_GetSIDFrqPtr_23
SIDSE_Hlp_GetSIDFrqPtr_01
	lfsr	FSR2, SIDL_BASE + SIDx_V1_FRQ_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDL_BASE + SIDx_V2_FRQ_L
	return

SIDSE_Hlp_GetSIDFrqPtr_23
	lfsr	FSR2, SIDL_BASE + SIDx_V3_FRQ_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDR_BASE + SIDx_V1_FRQ_L
	return

SIDSE_Hlp_GetSIDFrqPtr_4567
	lfsr	FSR2, SIDR_BASE + SIDx_V2_FRQ_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDR_BASE + SIDx_V3_FRQ_L
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_Hlp_GetSIDPWPtr
;;  DESCRIPTION: returns pointer to SIDx_V1_PW_L register
;;  IN: SID_SE_ELEMENT_NUM
;;  OUT: pointer in FSR2
;; --------------------------------------------------------------------------
SIDSE_Hlp_GetSIDPWPtr
	BRA_IFSET SID_SE_ELEMENT_NUM, 2, BANKED, SIDSE_Hlp_GetSIDPWPtr_4567
SIDSE_Hlp_GetSIDPWPtr_0123
	BRA_IFSET SID_SE_ELEMENT_NUM, 1, BANKED, SIDSE_Hlp_GetSIDPWPtr_23
SIDSE_Hlp_GetSIDPWPtr_01
	lfsr	FSR2, SIDL_BASE + SIDx_V1_PW_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDL_BASE + SIDx_V2_PW_L
	return

SIDSE_Hlp_GetSIDPWPtr_23
	lfsr	FSR2, SIDL_BASE + SIDx_V3_PW_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDR_BASE + SIDx_V1_PW_L
	return

SIDSE_Hlp_GetSIDPWPtr_4567
	lfsr	FSR2, SIDR_BASE + SIDx_V2_PW_L
	btfsc	SID_SE_ELEMENT_NUM, 0, BANKED
	lfsr	FSR2, SIDR_BASE + SIDx_V3_PW_L
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SIDSE_Hlp_Pitch_Mod
;; IN: frequency in SID_SE_TARGET_FRQ_[LH]
;;     Signed Modulation offset in SID_MOD_TARG_PITCHx_[LHU]
;;     Voice number in SID_SE_ELEMENT_NUM
;; OUT: modulated frequency in SID_SE_TARGET_FRQ_[LH]
;; --------------------------------------------------------------------------
SIDSE_Hlp_Pitch_Mod
	;; calculate pointer to MOD target array -> FSR2
	movf	SID_SE_ELEMENT_NUM, W, BANKED
	addlw	(SID_MOD_TARG_PITCH1_L-SID_MOD_TARG_BASE)/3
	mullw	3
	lfsr	FSR2, SID_MOD_TARG_BASE
	movf	PRODL, W
	addwf	FSR2L, F

	;; add modulation value to frequency
	;; value is divided by four for better usage of the depth range
	movff	POSTINC2, PRODL
	movff	POSTINC2, PRODH
	movff	POSTINC2, TABLAT

	clrc
	btfsc	TABLAT, 7
	setc
	rrf	TABLAT, F
	rrf	PRODH, F
	rrf	PRODL, F

	clrc
	btfsc	TABLAT, 7
	setc
	rrf	TABLAT, F
	rrf	PRODH, F
	rrf	PRODL, F

	movf	PRODL, W
	addwf	SID_SE_TARGET_FRQ_L, F, BANKED
	movf	PRODH, W
	addwfc	SID_SE_TARGET_FRQ_H, F, BANKED

	;; saturate
	BRA_IFSET TABLAT, 7, ACCESS, SIDSE_Hlp_Pitch_Mod_SatNeg
SIDSE_Hlp_Pitch_Mod_SatPos
	movf	TABLAT, W
	bnz	SIDSE_Hlp_Pitch_Mod_SatPos_Sat
	bnc	SIDSE_Hlp_Pitch_Mod_NoSat
SIDSE_Hlp_Pitch_Mod_SatPos_Sat
	setf	SID_SE_TARGET_FRQ_L, BANKED
	setf	SID_SE_TARGET_FRQ_H, BANKED
	rgoto	SIDSE_Hlp_Pitch_Mod_Sat_Cont
SIDSE_Hlp_Pitch_Mod_SatNeg
	comf	TABLAT, W
	bnz	SIDSE_Hlp_Pitch_Mod_SatNeg_Sat
	bc	SIDSE_Hlp_Pitch_Mod_NoSat
SIDSE_Hlp_Pitch_Mod_SatNeg_Sat
	clrf	SID_SE_TARGET_FRQ_L, BANKED
	clrf	SID_SE_TARGET_FRQ_H, BANKED
	;; 	rgoto	SIDSE_Hlp_Pitch_Mod_Sat_Cont
SIDSE_Hlp_Pitch_Mod_Sat_Cont
SIDSE_Hlp_Pitch_Mod_NoSat
	return

;; --------------------------------------------------------------------------
;; Help Function which interpolates a 16bit value
;; IN:  base address of parameter set (FIPx_BASE) in FSR1
;;      target value in IRQ_TMP[12]
;; OUT:	interpolated value in IRQ_TMP[12]
;; USES: PROD[LH]
;; --------------------------------------------------------------------------
SIDSE_Hlp_IP
	;; shift-right target value (internally we have to calculate with a signed 16-bit value)
	clrc
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	;; check for new target value
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SIDSE_Hlp_IP_Change
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto SIDSE_Hlp_IP_Change
	rgoto	SIDSE_Hlp_IP_Cont
SIDSE_Hlp_IP_Change
	;; copy new target value
	movlw	FIPx_TARGET_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	FIPx_TARGET_VALUE_H
	movff	IRQ_TMP2, PLUSW1

	;; calculate difference between current value and new target value
	movlw	FIPx_VALUE_L
	movf	PLUSW1, W
	subwf	IRQ_TMP1, W
	movwf	IRQ_TMP1
	movlw	FIPx_VALUE_H
	movf	PLUSW1, W
	subwfb	IRQ_TMP2, W
	movwf	IRQ_TMP2

	;; value which will be added on each step: diverence / 8
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F
	rrf	IRQ_TMP2, F
	rrf	IRQ_TMP1, F

	movf	IRQ_TMP2, W
	andlw	0x1f
	btfsc	WREG, 4
	iorlw 0xe0
	movwf	IRQ_TMP2

	;; should be at least 1 (!)
	movf	IRQ_TMP1, W
	iorwf	IRQ_TMP2, W
	skpnz
	incf	IRQ_TMP1, F

	;; store divided value
	movlw	FIPx_DIV_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	FIPx_DIV_VALUE_H
	movff	IRQ_TMP2, PLUSW1

SIDSE_Hlp_IP_Cont

	;; step handler

	;; copy current filter value into IRQ_TMP[12]
	movlw	FIPx_VALUE_L
	movff	PLUSW1, IRQ_TMP1
	movlw	FIPx_VALUE_H
	movff	PLUSW1, IRQ_TMP2
	
	;; do nothing if target value already reached
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SIDSE_Hlp_IP_Cont_Do
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	cpfseq	IRQ_TMP2, ACCESS
	rgoto SIDSE_Hlp_IP_Cont_Do
	rgoto	SIDSE_Hlp_IP_End
SIDSE_Hlp_IP_Cont_Do

	;; add div value to current value
	movlw	FIPx_DIV_VALUE_L
	movf	PLUSW1, W
	addwf	IRQ_TMP1, F
	movlw	FIPx_DIV_VALUE_H
	movf	PLUSW1, W
	addwfc	IRQ_TMP2, F

	;; end reached on overflow
	BRA_IFSET IRQ_TMP2, 7, ACCESS, SIDSE_Hlp_IP_TargetReached

	;; end reached if value >= target value
	movlw	FIPx_TARGET_VALUE_L
	movf	PLUSW1, W
	subwf	IRQ_TMP1, W
	movlw	FIPx_TARGET_VALUE_H
	movf	PLUSW1, W
	subwfb	IRQ_TMP2, W

	;; branch depending on increment/decrement
	movlw	FIPx_DIV_VALUE_H
	BRA_IFSET PLUSW1, 7, ACCESS, SIDSE_Hlp_IP_Dec
SIDSE_Hlp_IP_Inc
	bnc	SIDSE_Hlp_IP_End
	;; 	rgoto	SIDSE_Hlp_IP_TargetReached

SIDSE_Hlp_IP_TargetReached
	movlw	FIPx_TARGET_VALUE_L
	movff	PLUSW1, IRQ_TMP1
	movlw	FIPx_TARGET_VALUE_H
	movff	PLUSW1, IRQ_TMP2
	rgoto	SIDSE_Hlp_IP_End
	
SIDSE_Hlp_IP_Dec
	bc	SIDSE_Hlp_IP_End
	rgoto	SIDSE_Hlp_IP_TargetReached

SIDSE_Hlp_IP_End
	;; copy back IRQ_TMP[12] to FIPx_VALUE_[LH]
	movlw	FIPx_VALUE_L
	movff	IRQ_TMP1, PLUSW1
	movlw	FIPx_VALUE_H
	movff	IRQ_TMP2, PLUSW1

	;; left-shift result (it's 16bit unsigned again)
	clrc
	rlf	IRQ_TMP1, F
	rlf	IRQ_TMP2, F

	return

