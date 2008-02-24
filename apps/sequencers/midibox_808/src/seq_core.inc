; $Id$
;
; MIDIboxSEQ
; Sequencer core functions
;
; Activate this #define to measure the performance with a scope
; (connect the probe to RC.5)
#define SEQ_CORE_MEASURE_PERFORMANCE 0
;
; MEMO performance measurings show:
;   o best case (no event): ca 5 uS
;   o worst case (4 tracks are playing arpeggios): ca. 150 uS + delay caused by display update (ca. 200 uS)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; flags and values of SEQ core registers

;; ==========================================================================

SEQ_NUMBER_TRACKS		EQU	16	; number of tracks played by the sequencer

;; ==========================================================================

SEQ_REQ_START			EQU	0	; start request (send 0xfa)
SEQ_REQ_CONT			EQU	1	; continue request (send 0xfb)
SEQ_REQ_STOP			EQU	2	; stop request (send 0xfc)
SEQ_REQ_SONG_POS		EQU	3	; update of song position pointer (0xf2)
SEQ_REQ_CC_DUMP			EQU	4	; request CC dump
SEQ_REQ_NOECHO			EQU	5	; don't echo 0xfa 0xfb 0xfc to MIDI out
SEQ_REQ_SYNCHED_PATTERN_CHANGE	EQU	6	; pattern change should be synched to beat
SEQ_REQ_START_AUTO_SLAVE_REQ	EQU	7	; extension for SEQ_REQ_START: re-init BPM generator

SEQ_STATE_RUN			EQU	0	; set if sequencer runs
SEQ_STATE_PAUSE			EQU	1	; set if pause
SEQ_STATE_MANUAL_TRIGGER	EQU	2	; manual trigger active
SEQ_STATE_SONG			EQU	3	; set in song mode
SEQ_STATE_REMOTE		EQU	4	; used by MIDI Remote function
SEQ_STATE_FORCE_EEPROM		EQU	5	; if set, step event will always be read from EEPROM
SEQ_STATE_SET_LEN		EQU	6	; help flag for OnEvent loop

SEQ_TRKVARSTATE_PLAY_STEP	EQU	0	; temp. flag, if set, the step will be played
SEQ_TRKVARSTATE_BACKWARD	EQU	1	; static flag, if set, the track will be played in backward direction
SEQ_TRKVARSTATE_LEGATO		EQU	2	; temp. flag, if set, a note will be played after ON event for a proper legato
SEQ_TRKVARSTATE_FIRST_CLK	EQU	3	; don't increment on the first clock event
SEQ_TRKVARSTATE_STRETCH_GL	EQU	4	; stretch flag, set when gatelength >= 23
SEQ_TRKVARSTATE_RETRIGGER	EQU	5	; retrigger flag
SEQ_TRKVARSTATE_POS_RESET	EQU	6	; set by seq_midi.inc if position of ARP/Transpose tracks should be reset
SEQ_TRKVARSTATE_DISABLED	EQU	7	; set if no pattern is selected to avoid editing of trigger/layer values

SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE EQU	0	; set so long a note/cc is held (for note length measuring)
SEQ_TRKVARSTATE2_SYNC_MEASURE	 EQU	1	; temporary request for synch to measure (used during pattern switching)

SEQ_TRKDIR_FORWARD		EQU	0x00	; not bits, but values!
SEQ_TRKDIR_BACKWARD		EQU	0x01
SEQ_TRKDIR_PINGPONG		EQU	0x02
SEQ_TRKDIR_PENDULUM		EQU	0x03
SEQ_TRKDIR_RANDOM_DIR		EQU	0x04
SEQ_TRKDIR_RANDOM_STEP		EQU	0x05
SEQ_TRKDIR_RANDOM_D_S		EQU	0x06

SEQ_TRKDIV_SYNCH_TO_MEASURE	EQU	6	; synch to 4/4 measure
SEQ_TRKDIV_TRIPLETS		EQU	7	; play triplets

SEQ_RECORD_MODE_STEP		EQU	0	; if 0: live record, if 1: step record
SEQ_RECORD_MODE_AUTOSTART	EQU	1	; if 1: start sequencer with first key

SEQ_MODE0_STEP_EDIT		EQU	0	; we are in step edit mode
SEQ_MODE0_MUTE			EQU	1	; change mute status with GP buttons
SEQ_MODE0_PATTERN		EQU	2	; change pattern with GP buttons
SEQ_MODE0_SONG			EQU	3	; we are in song mode
SEQ_MODE0_ALT			EQU	4	; ALT button pressed
SEQ_MODE0_FAST_ENCODERS		EQU	5	; encoders in fast speed mode
SEQ_MODE0_CHANGE_ALL_STEPS	EQU	6	; change all steps
SEQ_MODE0_SOLO			EQU	7	; play current track solo

SEQ_MODE1_METRONOME		EQU	0	; enable metronome
SEQ_MODE1_SCRUB			EQU	1	; enable scrub mode
SEQ_MODE1_RECORD		EQU	2	; we are in step record menu
SEQ_MODE1_MUTE_HOLD		EQU	3	; if mute button is held
SEQ_MODE1_MUTE_HOLD_MULTISEL	EQU	4	; used within the MUTE menu to select multiple tracks so long the mute button is held
SEQ_MODE1_EDIT_HOLD		EQU	5	; the same for EDIT button
SEQ_MODE1_EDIT_HOLD_MULTISEL	EQU	6	; the same for EDIT button
SEQ_MODE1_SHIFT			EQU	7	; shift button function

SEQ_MODE2_FWD_HOLD		EQU	0	; FWD button hold
SEQ_MODE2_REW_HOLD		EQU	1	; REW button hold
SEQ_MODE2_LIVE_MODE		EQU	2	; if live play mode active
SEQ_MODE2_LIVEPLAY		EQU	2	; currently assigned to loop button (therefore same flag): liveplay mode
SEQ_MODE2_AUX_SELECTED		EQU	3	; if aux layer is selected

;; --------------------------------------------------------------------------
;;  This function is called by USER_Init to initialize the sequencer
;; --------------------------------------------------------------------------
SEQ_CORE_Init
	SET_BSR	SEQ_BASE

	;; restart pseudo random generator
	movlw	0x42
	movwf	SEQ_RANDOM_SEED_L, BANKED
	movwf	SEQ_RANDOM_SEED_H, BANKED

	;; select track 1 by default
	movlw	0
	call	CS_M_HLP_SetSelectedTrk

	;; select section A by default, assume that buttons deselected
	movlw	(1 << 0) | 0x30
	movwf	SEQ_SELECTED_AB, BANKED

	;; go into edit mode by default
	movlw	(1 << SEQ_MODE0_STEP_EDIT)
	movwf	SEQ_MODE0, BANKED

	;; init record mode
	clrf	SEQ_RECORD_MODE, BANKED

	;; disable song
	movlw	0x80
	movwf	SEQ_SONG, BANKED

	;; select default patterns (A1/C1/E1/G1) when first bankstick available
	clrf	SEQ_PATTERN_SYNC_TRANSFER_REQ, BANKED

	movlw	0x00
	movwf	SEQ_PATTERN0, BANKED
	movwf	SEQ_NEXT_PATTERN0, BANKED
	clrf	SEQ_PATTERN_BANK0, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK0, BANKED

	;; clear pattern chain
	call	SEQ_CORE_ChainClr

	;; restore dump
	call	SEQ_DUMP_RestoreAll

	;; update disable flags
	call	SEQ_CORE_UpdateTrkDisable

	;; reset song
	call	SEQ_SONG_Reset

	;; reset sequencer and exit
	rgoto	SEQ_CORE_Reset


;; --------------------------------------------------------------------------
;;  Reset sequencer position
;; --------------------------------------------------------------------------
SEQ_CORE_Reset
	;; play off events of all tracks
	call	SEQ_CORE_Hlp_PlayAllOffEvnts

	;; init the reference counters
	SET_BSR SEQ_BASE
	setf	SEQ_CLK_TICK6_CTR, BANKED
	setf	SEQ_CLK_TICK4_CTR, BANKED
	setf	SEQ_CLK_STEP_CTR, BANKED

	BRA_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_CORE_Reset_NotMaster
SEQ_CORE_Reset_Master
	;; clear all sequencer requests (i.E. a stop event!)
	clrf	SEQ_REQ, BANKED

	;; cancel all requested clocks (only relevant for master mode, for slave mode it's done in SEQ_MIDI_NotifyRx_Start)
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED
SEQ_CORE_Reset_NotMaster

	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_ResetLoop
	call	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	call	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2
 	call	SEQ_CORE_ResetTrkPos

	;; increment SEQ_EVNTT until last track is reached
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_ResetLoop


	;; stop manual trigger and pause mode (if enabled)
	bcf	SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED
	bcf	SEQ_STATE, SEQ_STATE_PAUSE, BANKED

	;; reset the BPM generator and exit
	movf	SEQ_BPM, W, BANKED
	call	SEQ_BPM_Set
	SET_BSR	SEQ_BASE

	return



;; --------------------------------------------------------------------------
;;  This function should be called on a MIDI clock/start/stop/cont event to
;;  control the auto slave detection
;; --------------------------------------------------------------------------
SEQ_CORE_ChkAutoSlave
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_CORE_ChkAutoSlave_End
	BRA_IFCLR SEQ_REQ, SEQ_REQ_START_AUTO_SLAVE_REQ, BANKED, SEQ_CORE_ChkAutoSlave_End
	bcf	SEQ_REQ, SEQ_REQ_START_AUTO_SLAVE_REQ, BANKED

	movlw	0x02
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	iorlw 0x01
	call	SEQ_BPM_ModeSet

SEQ_CORE_ChkAutoSlave_End
	return


;; --------------------------------------------------------------------------
;;  This function is called by USER_Tick when nothing else is to do
;; --------------------------------------------------------------------------
SEQ_CORE_Tick
	;; ---[ check if a start event has been requested ]---
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_REQ, SEQ_REQ_START, BANKED, SEQ_CORE_Tick_NoStart
	bcf	SEQ_REQ, SEQ_REQ_START, BANKED	; clear request
	rcall	SEQ_CORE_Start			; process the start event
SEQ_CORE_Tick_NoStart

	;; ---[ check if a continue event has been requested ]---
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_REQ, SEQ_REQ_CONT, BANKED, SEQ_CORE_Tick_NoCont
	bcf	SEQ_REQ, SEQ_REQ_CONT, BANKED	; clear request
	rcall	SEQ_CORE_ChkAutoSlave		; check for auto slave selection
	rcall	SEQ_CORE_Cont			; process the continue event
SEQ_CORE_Tick_NoCont

	;; ---[ check if a stop event has been requested ]---
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_REQ, SEQ_REQ_STOP, BANKED, SEQ_CORE_Tick_NoStop
	bcf	SEQ_REQ, SEQ_REQ_STOP, BANKED	; clear request
	rcall	SEQ_CORE_ChkAutoSlave		; check for auto slave selection
	rcall	SEQ_CORE_Stop			; process the continue event
SEQ_CORE_Tick_NoStop

	;; ---[ check if song position should be updated ]---
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_REQ, SEQ_REQ_SONG_POS, BANKED, SEQ_CORE_Tick_NoSongPos
	bcf	SEQ_REQ, SEQ_REQ_SONG_POS, BANKED; clear request
	rcall	SEQ_CORE_ChkAutoSlave		; check for auto slave selection
	call	SEQ_CORE_SetPos			; process the continue event
SEQ_CORE_Tick_NoSongPos

	;; ---[ check if a clock event has been requested ]---
SEQ_CORE_Tick_ClkLoop
	SET_BSR	SEQ_BASE
	movf	SEQ_CLK_REQ_CTR, W, BANKED
	bz	SEQ_CORE_Tick_NoClk
SEQ_CORE_Tick_Clk
	;; note: the clock counter ensures that a clock event never get lost
	;; see also the header information of seq_bpm.inc
	decf	SEQ_CLK_REQ_CTR, F, BANKED
	;; check for auto slave selection
	rcall	SEQ_CORE_ChkAutoSlave
	;; don't execute clock if sequencer not running
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Tick_NoClk
	BRA_IFSET SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_CORE_Tick_NoClk
	rcall	SEQ_CORE_Clk			; process clock event
SEQ_CORE_Tick_NoClk

	;; iterate until clock counter is zero
SEQ_CORE_Tick_End
	SET_BSR	SEQ_BASE
	movf	SEQ_CLK_REQ_CTR, W, BANKED
	bnz	SEQ_CORE_Tick


	;; update the EDIT/MUTE/PATTERN/SONG flag depending on menu
	IRQ_DISABLE
	movlw	~((1 << SEQ_MODE0_STEP_EDIT) | (1 << SEQ_MODE0_MUTE) | (1 << SEQ_MODE0_PATTERN) | (1 << SEQ_MODE0_SONG))
	andwf	SEQ_MODE0, F, BANKED
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_CORE_Tick_NotWithinMenu

	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_MUTE
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_MUTE, BANKED
	xorlw	CS_MENU_PAGE_PATTERN ^ CS_MENU_PAGE_MUTE
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	xorlw	CS_MENU_PAGE_PATTERN_B ^ CS_MENU_PAGE_PATTERN
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	xorlw	CS_MENU_PAGE_PATTERN_S ^ CS_MENU_PAGE_PATTERN_B
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	xorlw	CS_MENU_PAGE_SONG ^ CS_MENU_PAGE_PATTERN_S
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	;; NOTE: the LED handler flashes the LED when in song menu and song state not active

SEQ_CORE_Tick_NotWithinMenu
	;; if these three bits *not* set, we are in edit mode
	movlw	((1 << SEQ_MODE0_MUTE) | (1 << SEQ_MODE0_PATTERN) | (1 << SEQ_MODE0_SONG))
	andwf	SEQ_MODE0, W, BANKED
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_STEP_EDIT, BANKED

	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  start event handler
;; --------------------------------------------------------------------------
SEQ_CORE_Start
	SET_BSR	SEQ_BASE

	;; set start pin (if enabled)
#if DEFAULT_EXT_START_LAT
	bsf	DEFAULT_EXT_START_LAT, DEFAULT_EXT_START_PIN
#endif

	;; send FA
	movlw	0xfa
	call	SEQ_ROUTER_SendSync

	;; extra: a song could contain a STOP entry at the first position
	;; in order to find this out, we set the RUN flag here. If it has been cleared, we don't continue with the start procedure
	SET_BSR	SEQ_BASE
	bsf	SEQ_STATE, SEQ_STATE_RUN, BANKED

	;; if chain mode: change to first pattern
	clrf	SEQ_CHAIN_PLAY_PTR, BANKED
	call	SEQ_CORE_ChainNextPos

	;; reset song
	call	SEQ_SONG_Reset

	SET_BSR	SEQ_BASE	; (don't start - song sequencer has requested a stop)
	btfss	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

	;; reset patterns
	rcall	SEQ_CORE_Reset

	;; check if pattern should be changed
	rcall	SEQ_CORE_ChangePatternCheck

	;; finally set run state and notify that this is the first clock
	SET_BSR	SEQ_BASE
	bsf	SEQ_STATE, SEQ_STATE_RUN, BANKED

	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  continue event handler
;; --------------------------------------------------------------------------
SEQ_CORE_Cont
	SET_BSR	SEQ_BASE

	;; set start pin (if enabled)
#if DEFAULT_EXT_START_LAT
	bsf	DEFAULT_EXT_START_LAT, DEFAULT_EXT_START_PIN
#endif

	;; send FB
	movlw	0xfb
	call	SEQ_ROUTER_SendSync

	;; clear all sequencer requests (i.E. a stop event!)
	clrf	SEQ_REQ, BANKED
	;; and set run state
	bsf	SEQ_STATE, SEQ_STATE_RUN, BANKED
	;; stop manual trigger mode (if enabled)
	bcf	SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED
	;; clear pause flag
	bcf	SEQ_STATE, SEQ_STATE_PAUSE, BANKED
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  stop event handler
;; --------------------------------------------------------------------------
SEQ_CORE_Stop
	SET_BSR	SEQ_BASE

	;; clear start pin (if enabled)
#if DEFAULT_EXT_START_LAT
	bcf	DEFAULT_EXT_START_LAT, DEFAULT_EXT_START_PIN
#endif

	;; send FC
	movlw	0xfc
	call	SEQ_ROUTER_SendSync

	;; clear all sequencer requests (i.E. a stop event!)
	clrf	SEQ_REQ, BANKED
	;; and clear run state
	bcf	SEQ_STATE, SEQ_STATE_RUN, BANKED
#if DEFAULT_EXT_CLK_LAT
	;; ensure that external clock pin is cleared
	bcf	DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN
#endif
	;; stop manual trigger mode (if enabled)
	bcf	SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; play off events and exit
	rgoto	SEQ_CORE_Hlp_PlayAllOffEvnts


;; --------------------------------------------------------------------------
;;  pause function
;; --------------------------------------------------------------------------
SEQ_CORE_Pause
	;; toggle pause flag
	SET_BSR	SEQ_BASE
	btg	SEQ_STATE, SEQ_STATE_PAUSE
	;; play off events of all tracks
	rgoto	SEQ_CORE_Hlp_PlayAllOffEvnts


;; --------------------------------------------------------------------------
;;  manual trigger
;;  expects sequencer position in SEQ_CURRENT_STEP
;; --------------------------------------------------------------------------
SEQ_CORE_ManualTrigger
	;; set flag and position in every track record
	SET_BSR	SEQ_BASE
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_ManualTriggerLoop
	call	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	call	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; don't increment on the first clock event
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_FIRST_CLK
	bcf	PLUSW2, SEQ_TRKVARSTATE_RETRIGGER

	;; set starting position
	movlw	SEQ_TRKVARSTEPx
	movff	SEQ_CURRENT_STEP, PLUSW2

	;; clear divider and retrigger counters
	movlw	SEQ_TRKVARDIVCTRx
	clrf	PLUSW2
	movlw	SEQ_TRKVARRETRGx
	clrf	PLUSW2

	;; switch to next record
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_ManualTriggerLoop

	;; cancel all requested clocks
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED

	;; init the reference counters
	setf	SEQ_CLK_TICK6_CTR, BANKED
	setf	SEQ_CLK_TICK4_CTR, BANKED
	setf	SEQ_CLK_STEP_CTR, BANKED

	;; exit if sequencer is already running
	BRA_IFSET SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_CORE_ManualTrigger_Cont
	btfsc	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return

SEQ_CORE_ManualTrigger_Cont
	;; else request "manual trigger" (sequencer plays only one step and stops thereafter)
	bsf	SEQ_REQ, SEQ_REQ_NOECHO, BANKED
	call	SEQ_CORE_Cont

	;; set MANUAL_TRIGGER flags, so that the sequencer will stop before the next pos
	SET_BSR	SEQ_BASE
	bsf	SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED

	return


;; --------------------------------------------------------------------------
;;  clock event handler
;; --------------------------------------------------------------------------
SEQ_CORE_Clk
	SET_BSR	SEQ_BASE

#if SEQ_CORE_MEASURE_PERFORMANCE == 1
	bsf	LATC, 5
#endif

	;; ------------------------------------------------------------------
	;; Handle Global Internal Clock Divider
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Div
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_Div_Run
SEQ_CORE_Clk_Div_Stop
	clrf	SEQ_CORE_INT_CLK_DIVIDER, BANKED
	rgoto	SEQ_CORE_Clk_Div_Cont
SEQ_CORE_Clk_Div_Run
	movf	SEQ_CORE_INT_CLK_DIVIDER, W, BANKED
	bnz	SEQ_CORE_Clk_Div_Dec
SEQ_CORE_Clk_Div_Reload
	movf	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, W, BANKED
	call	MIOS_HLP_GetBitORMask
	addlw	-1
	movwf	SEQ_CORE_INT_CLK_DIVIDER, BANKED
	rgoto	SEQ_CORE_Clk_Div_Cont
SEQ_CORE_Clk_Div_Dec
	decf	SEQ_CORE_INT_CLK_DIVIDER, F, BANKED
	rgoto	SEQ_CORE_Clk_End
SEQ_CORE_Clk_Div_Cont

	;; ------------------------------------------------------------------
	;; increment reference counters (if "first clk" flag not set)
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Reference
	;; increment tick4 counter (only used for triplet notes - no step counting required)
	incf	SEQ_CLK_TICK4_CTR, W, BANKED
	andlw	0x03
	movwf	SEQ_CLK_TICK4_CTR, BANKED

	;; increment tick6 counter
	incf	SEQ_CLK_TICK6_CTR, F, BANKED
	bz	SEQ_CORE_Clk_Reference_ResetT
	movlw	6-1
	cpfsgt	SEQ_CLK_TICK6_CTR, BANKED
	rgoto SEQ_CORE_Clk_Reference_NoOv
SEQ_CORE_Clk_Reference_ResetT
	;; clear tick counter
	clrf	SEQ_CLK_TICK6_CTR, BANKED

	;; increment step counter
	incf	SEQ_CLK_STEP_CTR, F, BANKED
	movlw	64-1
	cpfsgt	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_Reference_NoOv
SEQ_CORE_Clk_Reference_ResetS
	;; clear step counter
	clrf	SEQ_CLK_STEP_CTR, BANKED

	;; in pattern mode: ensure that screen is updated on every new bar
	;; for consistent screen
	btfsc	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
SEQ_CORE_Clk_Reference_NoOv

	;; ------------------------------------------------------------------
	;; in manual trigger mode: request stop when last subtick of step reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_StopManual
	BRA_IFCLR SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_NoStopManual
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	mullw	6
	movf	PRODL, W
	addwf	SEQ_CLK_TICK6_CTR, W, BANKED
	movwf	TMP1
	movlw	23-2
	cpfsgt	TMP1, ACCESS
	rgoto SEQ_CORE_Clk_NoStopManual
	;; request sequencer stop (will also disable manual trigger mode)
	bsf	SEQ_REQ, SEQ_REQ_STOP, BANKED
	bsf	SEQ_REQ, SEQ_REQ_NOECHO, BANKED
SEQ_CORE_Clk_NoStopManual

	;; ------------------------------------------------------------------
	;; song or chain mode: request next pattern if sequencer running and last position reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextPS
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextPS_End
	movf	SEQ_CHAIN_TOP, W, BANKED
	bnz	SEQ_CORE_Clk_NextPS_Chain
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_NextPS_End
SEQ_CORE_Clk_NextPS_Chain
	movlw	6-2	; (this value has to be adjusted - depends on the time which is necessary to load 4 patterns)
	cpfseq	SEQ_CLK_TICK6_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
	movlw	64-2
	cpfseq	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
SEQ_CORE_Clk_NextPS_Request
	;; song mode has higher priority
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_NextPS_RequestChain
	call	SEQ_SONG_NextPos
	rgoto	SEQ_CORE_Clk_NextPS_End	
SEQ_CORE_Clk_NextPS_RequestChain
	call	SEQ_CORE_ChainNextPos
SEQ_CORE_Clk_NextPS_End	

	;; ------------------------------------------------------------------
	;; change to new pattern if requested
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextP
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextP_End
	movlw	6-1	; (this value has to be adjusted - depends on the time which is necessary to load 4 patterns)
	cpfseq	SEQ_CLK_TICK6_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextP_End
	BRA_IFCLR SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED, SEQ_CORE_Clk_NextP_NoSynch
SEQ_CORE_Clk_NextP_Synch
	movlw	64-2
	cpfseq	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextP_End
SEQ_CORE_Clk_NextP_NoSynch
	rcall	SEQ_CORE_ChangePatternCheck
SEQ_CORE_Clk_NextP_End	

	;; ------------------------------------------------------------------
	;; send F8 on every 4th internal tick (96ppqn resolution) if echo allowed and not in slave mode
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NoF8
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	mullw	6
	movf	PRODL, W
	addwf	SEQ_CLK_TICK6_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_CORE_Clk_NoF8
SEQ_CORE_Clk_F8
	;; send MIDI clock if in master mode
	;; 	BIFSET	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, rgoto SEQ_CORE_Clk_NoF8
	movlw	0xf8
	call	SEQ_ROUTER_SendSync
SEQ_CORE_Clk_NoF8

	;; ------------------------------------------------------------------
	;; Set external clock depending on divider
	;; ------------------------------------------------------------------
#if DEFAULT_EXT_CLK_LAT
SEQ_CORE_ExtClk
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_ExtClk_Run
SEQ_CORE_ExtClk_Stop
	clrf	SEQ_CORE_EXT_CLK_DIVIDER, BANKED	; ensure that pin set with next F8
	rgoto	SEQ_CORE_ExtClk_Clr
SEQ_CORE_ExtClk_Run
	movf	SEQ_CORE_EXT_CLK_DIVIDER, W, BANKED
	bnz	SEQ_CORE_ExtClk_DecDv
SEQ_CORE_ExtClk_Set
	bsf	DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN
	incf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, W, BANKED
	call	MIOS_HLP_GetBitORMask
	addlw	-1
	movwf	SEQ_CORE_EXT_CLK_DIVIDER, BANKED
	rgoto	SEQ_CORE_ExtClk_Cont
SEQ_CORE_ExtClk_DecDv
	decf	SEQ_CORE_EXT_CLK_DIVIDER, F, BANKED
SEQ_CORE_ExtClk_Clr
	bcf	DEFAULT_EXT_CLK_LAT, DEFAULT_EXT_CLK_PIN
SEQ_CORE_ExtClk_Cont
#endif

	;; ------------------------------------------------------------------
	;; play metronome if enabled
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Metro
	movf	SEQ_CLK_TICK6_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NoMetro

	;; play ON event on every 16th step (4th step with normal resolution)
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x0f
	bz	SEQ_CORE_Clk_Metro_On

	;; play OFF event on every 16th+4 step (4th+1 with normal resolution)
	xorlw	0x03
	bnz	SEQ_CORE_Clk_NoMetro
SEQ_CORE_Clk_Metro_Off
	movf	SEQ_METRONOME_OFF_EVNT0, W, BANKED
	bz	SEQ_CORE_Clk_NoMetro
	movwf	MIDI_EVNT0
	clrf	SEQ_METRONOME_OFF_EVNT0, BANKED	; (clear variable, so that Off event won't be played again)
	movff	SEQ_METRONOME_OFF_EVNT1, MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE
	rgoto	SEQ_CORE_Clk_Metro_Play
	

SEQ_CORE_Clk_Metro_On
	;; don't play ON event if metronome disabled
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED, SEQ_CORE_Clk_NoMetro

	;; prepare note status + channel
	movf	SEQ_METRONOME_CHANNEL, W, BANKED
	bz	SEQ_CORE_Clk_NoMetro
	addlw	-1
	andlw	0x0f
	iorlw	0x90
	movwf	MIDI_EVNT0	

	;; play special note on every 16th step
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_Metro_N16
SEQ_CORE_Clk_Metro_16
	movf	SEQ_METRONOME_NOTE_MEASURE, W, BANKED
	bz	SEQ_CORE_Clk_NoMetro
	movwf	MIDI_EVNT1
	movlw	127		; velocity on measure
	rgoto	SEQ_CORE_Clk_Metro_Cont
SEQ_CORE_Clk_Metro_N16
	movf	SEQ_METRONOME_NOTE_BEAT, W, BANKED
	bz	SEQ_CORE_Clk_NoMetro
	movwf	MIDI_EVNT1
	movlw	96		; velocity on beat
SEQ_CORE_Clk_Metro_Cont
	movwf	MIDI_EVNT_VALUE
	;; memorize byte #0 and #1 for the OFF function
	movff	MIDI_EVNT0, SEQ_METRONOME_OFF_EVNT0
	movff	MIDI_EVNT1, SEQ_METRONOME_OFF_EVNT1

SEQ_CORE_Clk_Metro_Play
	;; send MIDI event over default port
	clrf	MIDI_EVNT_PORT
	call	MIDI_EVNT_Send
	;; and fix BSR
	SET_BSR	SEQ_BASE
SEQ_CORE_Clk_NoMetro
	
#if SEQ_CORE_MEASURE_PERFORMANCE == 2
	bsf	LATC, 5
#endif

	;; ------------------------------------------------------------------
	;; starting the big clock loop which handles every single track seperately
	;; ------------------------------------------------------------------
	clrf	MIDI_EVNT_PORT0_RS		; clear running status of port 0 (so that it is ensured that we start with status event)
	clrf	SEQ_EVNTT, BANKED		; using SEQ_EVNTT as loop counter
SEQ_CORE_Clk_Loop
	rcall	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	rcall	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; ------------------------------------------------------------------
	;; play "off event" when decremented TRKVARQUEUEL_[LH]x reaches 0
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_OffEvent
	;; transfer length to PROD[LH] for easier calculation
	movlw	SEQ_TRKVARQUEUEL_Lx
	movff	PLUSW2, PRODL
	movlw	SEQ_TRKVARQUEUEL_Hx
	movf	PLUSW2, W
	movwf	PRODH

	;; skip if length (high and low byte) already zero
	iorwf	PRODL, W
	bz	SEQ_CORE_Clk_NoOffEvent

	;; decrement length counter and copy it back into TRKVAR registers
	decf	PRODL, F
	skpc
	decf	PRODH, F
	movlw	SEQ_TRKVARQUEUEL_Lx
	movff	PRODL, PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	movff	PRODH, PLUSW2

	;; skip if length hasn't reached zero
	movf	PRODH, W
	iorwf	PRODL, W
	bnz	SEQ_CORE_Clk_NoOffEvent

	;; play off events of the track
	rcall	SEQ_CORE_Hlp_PlayTrkOffEvnts
SEQ_CORE_Clk_NoOffEvent

	;; ------------------------------------------------------------------
	;; Decrement the delay counter (for multiple triggered steps)
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_DecReTrgCtr
	;; clear retrigger flag
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_RETRIGGER

	;; decrement delay counter (SEQ_TRKRETRGx[4..0]) so long the whole SEQ_TRKRETRGx register is > 0
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, F
	bz	SEQ_CORE_Clk_NoDecReTrgCtr
	decf	PLUSW2, F

	;; don't retrigger if repeat counter (SEQ_TRKRETRGx[6..5]) == 0
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, W
	andlw	0x60
	bz	SEQ_CORE_Clk_NoDecReTrgCtr

	;; no retrigger if delay counter (SEQ_TRKRETRGx[4..0]) > 0
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, W
	movwf	TABLAT
	andlw	0x1f
	bnz	SEQ_CORE_Clk_NoReTrg

SEQ_CORE_Clk_ReTrigger
	;; decrement repeat counter
	movlw	-0x20
	addwf	TABLAT, F

	;; if repeat counter > 0:
	;; set retrigger delay - it is stored in SEQ_TRKVARRETRGRELOADx, copy this value to SEQ_TRKRETRGx[4..0]
	movf	TABLAT, W
	andlw	0x60
	bz	SEQ_CORE_Clk_NoReTriggerReload
SEQ_CORE_Clk_ReTriggerReload
	movlw	SEQ_TRKVARRETRGRELOADx
	movf	PLUSW2, W
	andlw	0x1f
	iorwf	TABLAT, F
SEQ_CORE_Clk_NoReTriggerReload

	;; store result in RETRGx
	movlw	SEQ_TRKVARRETRGx
	movff	TABLAT, PLUSW2

	;; set retrigger flag
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_RETRIGGER
SEQ_CORE_Clk_NoDecReTrgCtr
SEQ_CORE_Clk_NoReTrg

	;; ------------------------------------------------------------------
	;; check if tick counter matches with PLYTICKx
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_ChkPlay
	;; select clock counter depending on triplet flag
	movlw	SEQ_TRKDIVx
	BRA_IFCLR PLUSW0, SEQ_TRKDIV_TRIPLETS, ACCESS, SEQ_CORE_Clk_ChkPlay_6
SEQ_CORE_Clk_ChkPlay_4
	movf	SEQ_CLK_TICK4_CTR, W, BANKED
	bz	SEQ_CORE_Clk_Step
	rgoto	SEQ_CORE_Clk_ChkPlay_Cont

SEQ_CORE_Clk_ChkPlay_6
	;; next step once step tick counter reached zero
	movf	SEQ_CLK_TICK6_CTR, W, BANKED
	bz	SEQ_CORE_Clk_Step
	;; 	rgoto	SEQ_CORE_Clk_ChkPlay_Cont

SEQ_CORE_Clk_ChkPlay_Cont
	;; force to play ON event if retrigger flag set
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_OnEvent

	;; play ON event if event has been delayed
	;; decrement counter until it has reached zero
	movlw	SEQ_TRKVARPLYTICKx
	movf	PLUSW2, F
	bz	SEQ_CORE_Clk_ChkPlay_NotPosted
	decf	PLUSW2, F
	skpnz
	rgoto	SEQ_CORE_Clk_Step_Posted
SEQ_CORE_Clk_ChkPlay_NotPosted

	;; else continue with next track
	rgoto	SEQ_CORE_Clk_NextTrack

	;; ------------------------------------------------------------------
	;; Play new note immediately if manual trigger has been requested
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_ManualTrigger
	;; 	BIFSET	SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, rgoto SEQ_CORE_Clk_OnEvent

	;; ------------------------------------------------------------------
	;; Determine track position of next step
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Step
	;; check for "synch to measure" flag - reset step counter if master track has reached zero again
	movlw	SEQ_TRKVARSTATE2x	; (set on pattern changes in song or "pattern_synch" mode)
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE, ACCESS, SEQ_CORE_Clk_Step_CtrResetForced
	movlw	SEQ_TRKDIVx		; (optional flag in clock divider menu)
	BRA_IFCLR PLUSW0, SEQ_TRKDIV_SYNCH_TO_MEASURE, ACCESS, SEQ_CORE_Clk_Step_NoCtrReset
SEQ_CORE_Clk_Step_CtrResetForced
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_Step_NoCtrReset
	movf	SEQ_CLK_TICK6_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_Step_NoCtrReset
SEQ_CORE_Clk_Step_CtrReset
	rcall	SEQ_CORE_ResetTrkPos

	movlw	SEQ_TRKVARSTATE2x
	bcf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
SEQ_CORE_Clk_Step_NoCtrReset

	;; don't increment on the first clock event
	movlw	SEQ_TRKVARSTATEx
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE_FIRST_CLK, ACCESS, SEQ_CORE_Clk_Step_No1st
SEQ_CORE_Clk_Step_1st
	bcf	PLUSW2, SEQ_TRKVARSTATE_FIRST_CLK
	bsf	PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP
	rgoto	SEQ_CORE_Clk_Step_NoIncDec
SEQ_CORE_Clk_Step_No1st

	;; increment SEQ_TRKDIVCTRx and save new value in TMP1
	movlw	SEQ_TRKVARDIVCTRx
	incf	PLUSW2, F
	movff	PLUSW2, TMP1

	;; check for max value which depends on divider setting
	movlw	SEQ_TRKDIVx
	movf	PLUSW0, W
	andlw	0x3f
	movwf	TMP2		; we need this to determine the TAKE_NEXT_STEP flag -> TMP2
	addlw	1
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_Clk_Step_IncDec

	;; if max value not reached ensure that the next step will not be played
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP

	;; live record: increment length counter
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_CORE_Clk_Step_NoRecLCtr
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, SEQ_CORE_Clk_Step_NoRecLCtr
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE, ACCESS, SEQ_CORE_Clk_Step_NoRecLCtr
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_CORE_Clk_Step_NoRecLCtr
SEQ_CORE_Clk_Step_RecLCtr
	movlw	6*4
	cpfslt	SEQ_RECORD_LENGTH_CTR, BANKED
	rgoto SEQ_CORE_Clk_Step_NoRecLCtr
	movlw	4
	addwf	SEQ_RECORD_LENGTH_CTR, BANKED
SEQ_CORE_Clk_Step_NoRecLCtr

	rgoto	SEQ_CORE_Clk_Step_NoIncDec

SEQ_CORE_Clk_Step_IncDec
	;; save current position (for multi triggers)
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, SEQ_CORE_TRK_LASTPOS

	clrf	SEQ_SKIP_STEP_PROTECTION_CTR, BANKED
SEQ_CORE_Clk_Step_IncDecLoop
	;; clear divider counter
	movlw	SEQ_TRKVARDIVCTRx
	clrf	PLUSW2

	;; determine next step depending on direction mode
	call	SEQ_CORE_NextStep

	;; step number -> SEQ_EVNTS
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW2, W
	movwf	SEQ_EVNTS, BANKED

	;; check if step should be skipped
	call	SEQ_TRG_SkipGet
	bz	SEQ_CORE_Clk_Step_IncDecLoopBrk

	;; try skipping up to 32 times
	incf	SEQ_SKIP_STEP_PROTECTION_CTR, F, BANKED
	BRA_IFCLR SEQ_SKIP_STEP_PROTECTION_CTR, 5, BANKED, SEQ_CORE_Clk_Step_IncDecLoop

	;; skip whole track if more than 32 skips
	rgoto	SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_Step_IncDecLoopBrk

	;; call record hook
	CALL_IFSET SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, SEQ_CORE_RecStep

SEQ_CORE_Clk_Step_NoIncDec

	;; ------------------------------------------------------------------
	;; if step is played: determine on which tick the step should be played
	;; ------------------------------------------------------------------
	movlw	SEQ_TRKVARSTATEx
	CALL_IFSET PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP, ACCESS, SEQ_GROOVE_GetDelay
	;; result in SEQ_TRKVARPLYTICKx	

	;; if SEQ_TRKVARPLYTICKx != 0, post playing the step
	movlw	SEQ_TRKVARPLYTICKx
	movf	PLUSW2, W
	bz	SEQ_CORE_Clk_Step_NotPosted
	rgoto	SEQ_CORE_Clk_NextTrack

SEQ_CORE_Clk_Step_Posted
	movlw	SEQ_TRKVARSTATEx	; play step
	bsf	PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP
SEQ_CORE_Clk_Step_NotPosted

	;; ------------------------------------------------------------------
	;; Don't send new event so long it hasn't been synched to measure
	;; (this is only relevant for dynamic sync on pattern changes in song or "pattern synch" mode or "chain" mode!)
	;; ------------------------------------------------------------------
#if 0
	;; TK: let's check if it works better without mute (because of Nudge function...)
	movf	SEQ_CHAIN_TOP, W
	bnz	SEQ_CORE_Clk_Step_SyncMute
#endif
	;; 	BRA_IFSET SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED, SEQ_CORE_Clk_Step_SyncMute
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_Step_NoSyncMute
SEQ_CORE_Clk_Step_SyncMute
	movlw	SEQ_TRKVARSTATE2x	; (set on pattern changes in song mode)
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE, ACCESS, SEQ_CORE_Clk_Muted
SEQ_CORE_Clk_Step_NoSyncMute

	;; ------------------------------------------------------------------
	;; Don't send new event if sequencer not in RUN mode
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_Muted

	;; ------------------------------------------------------------------
	;; else play "on event"
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_OnEvent

	;; check for manual trigger mode
	BRA_IFCLR SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_OnEvent_NM
	;; skip track if not selected for manual trigger
	call	CS_M_HLP_ChkTrkSelected
	bz	SEQ_CORE_Clk_OnEvent_NM
	rgoto	SEQ_CORE_Clk_Muted

SEQ_CORE_Clk_OnEvent_NM
	;; skip track if the RETRIGGER or PLAY_STEP flag not set (important for clock dividers > 1)
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_OnEventNMS
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP, ACCESS, SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_OnEventNMS
#if 0
	;; skip track if pattern disabled
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_DISABLED, ACCESS, SEQ_CORE_Clk_Muted
#endif
	;; skip track if muted
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_TRKS_MUTED0, W, BANKED
	btfsc	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_TRKS_MUTED1, W, BANKED
	skpz
	rgoto	SEQ_CORE_Clk_Muted

	;; skip if port assigned to track is muted
	movlw	SEQ_TRKCHNx
	swapf	PLUSW0, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PORTS_MUTED, W, BANKED
	skpz
	rgoto	SEQ_CORE_Clk_Muted

	;; skip track if solo and not selected track
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SOLO, BANKED, SEQ_CORE_Clk_OnEvent_NoSolo
SEQ_CORE_Clk_OnEvent_ChkSolo
	call	CS_M_HLP_ChkTrkSelected
	skpz
	rgoto	SEQ_CORE_Clk_Muted
SEQ_CORE_Clk_OnEvent_NoSolo

	;; ------------------------------------------------------------------

	;; get track events of current track/step
	;; track number already in SEQ_EVNTT
	movlw	SEQ_TRKVARSTEPx	; save step number (0-15) in SEQ_EVNTS
	movf	PLUSW2, W
	movwf	SEQ_EVNTS, BANKED

	;; play step randomly if "random gate" function is set
	call	SEQ_TRG_RandomGateGet
	bz	SEQ_CORE_Clk_OnEvent_NoRandomG
SEQ_CORE_Clk_OnEvent_Random
	rcall	SEQ_CORE_GenRandomNumber
	movlw	0x80
	cpfslt	SEQ_RANDOM_SEED_H, BANKED
	rgoto SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_OnEvent_NoRandomG

	bcf	SEQ_STATE, SEQ_STATE_SET_LEN, BANKED	; notifies that a new gate length has to be set

	;; don't play if retrigger flag not set and retrigger counter (SEQ_TRKVARRETGx[4..0]) > 0
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_OnEvent_ReTrg
SEQ_CORE_Clk_OnEvent_NoReTrg
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, W
	andlw	0x1f
	skpz
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEvent_ReTrg

	;; get MIDI event
	call	SEQ_LAYER_GetEvnt
	;; events now in SEQ_EVNT[012L]

	;; don't handle multiple events per step if event is 0 (muted)
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_CORE_Clk_ReTrgNotMuted
SEQ_CORE_Clk_ReTrgMuted
	movlw	SEQ_TRKVARRETRGx		; clear retrigger counter
	clrf	PLUSW2
	rgoto	SEQ_CORE_Clk_SendEvent_SkipReTrg
SEQ_CORE_Clk_ReTrgNotMuted

	;; preload SEQ_TRKVARRETRGx and SEQ_TRKVARRETRGRELOADx if RETRIGGER flag is not set
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_ReTrgNoReload
SEQ_CORE_Clk_ReTrgReload
	;; clear it if SEQ_EVNTL[5..6] (retrigger counter) is 0 and exit retrigger handler (single note)
	movf	SEQ_EVNTL, W, BANKED
	andlw	0x60
	bnz	SEQ_CORE_Clk_ReTrgReloadOk
SEQ_CORE_Clk_ReTrgReloadClr
	movlw	SEQ_TRKVARRETRGx
	clrf	PLUSW2
	rgoto	SEQ_CORE_Clk_SendEvent_SkipReTrg

SEQ_CORE_Clk_ReTrgReloadOk
	movlw	SEQ_TRKVARRETRGx
	movff	SEQ_EVNTL, PLUSW2
	movf	SEQ_EVNTL, W, BANKED
	andlw	0x1f
	movwf	TABLAT
	movlw	SEQ_TRKVARRETRGRELOADx
	movff	TABLAT, PLUSW2
SEQ_CORE_Clk_ReTrgNoReload

SEQ_CORE_Clk_SendEvent_SkipReTrg

	;; groove it
	call	SEQ_GROOVE_Event

	;; check if we have to stretch the gatelength
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_CORE_Clk_NoStretch
	movlw	SEQ_TRKVARSTATEx
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE_STRETCH_GL, ACCESS, SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_Stretch
	;; get scaled gatelength -> PROD[LH]
	movf	SEQ_EVNTL, W, BANKED
	rcall	SEQ_CORE_ScaleLen

	movlw	SEQ_TRKVARQUEUEL_Lx
	movff	PRODL, PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	movff	PRODH, PLUSW2
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_NoStretch

	;; ------------------------------------------------------------------

	;; by default no legato (only if: Note event and new note != current note)
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_LEGATO

	;; check if event is skipped (Note: if note or velocity==0, CC: if CC# is 0)
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_CORE_Clk_SendEvent_CCChk

SEQ_CORE_Clk_SendEvent_NoteChk
	;; don't play if note number == 0x00
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_CORE_Clk_SendEvent_NoteChkOE
	;; don't play if velocity == 0x00
	movf	SEQ_EVNT2, W, BANKED
	bz	SEQ_CORE_Clk_SendEvent_NoteChkOE
	rgoto	SEQ_CORE_Clk_SendEvent_Chk_End
SEQ_CORE_Clk_SendEvent_NoteChkOE
	rgoto	SEQ_CORE_Clk_NoOnEvent

SEQ_CORE_Clk_SendEvent_CCChk
	;; don't play if CC number == 0x00
	movf	SEQ_EVNT1, W, BANKED
	skpnz
	rgoto	SEQ_CORE_Clk_NoOnEvent
	;; 	rgoto	SEQ_CORE_Clk_SendEvent_Chk_End

SEQ_CORE_Clk_SendEvent_Chk_End

	;; ------------------------------------------------------------------
	;; play the new MIDI event

	movff	SEQ_EVNT0, MIDI_EVNT0
	movff	SEQ_EVNT1, MIDI_EVNT1
 	movff	SEQ_EVNT2, MIDI_EVNT_VALUE

	;; get MIDI port (+1) from TRKINFO table
	;; if 0: MIDI output disabled
	movf	SEQ_EVNTT, W, BANKED
	call	SEQ_TRK_GetMIDIPort
	addlw	-1
	movwf	MIDI_EVNT_PORT	; we need this value again at SEQ_CORE_Clk_SendEvent_PNoOff
	addlw	1
	bz	SEQ_CORE_Clk_SendEvent_NoMIDI
SEQ_CORE_Clk_SendEvent_MIDI
	call	MIDI_EVNT_Send
SEQ_CORE_Clk_SendEvent_NoMIDI

	;; request to toggle trigger pin
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTT, W, BANKED
 	movff	SEQ_EVNT2, MIOS_PARAMETER1
	call	SEQ_TRK_ReqTrigger

	;; update the meter (for mute menu)
	rcall	SEQ_CORE_UpdateMeter

	SET_BSR	SEQ_BASE			; fix BSR

	;; if legato flag set: play off event
	movlw	SEQ_TRKVARSTATEx
	RCALL_IFSET PLUSW2, SEQ_TRKVARSTATE_LEGATO, ACCESS, SEQ_CORE_Hlp_PlayOffEvnt

	;; store port, off event and length in queue
	;; exception: CC and length == 0x1f
	;; another exception: MIDI_EVNT_PORT == 0xff
	incf	MIDI_EVNT_PORT, W
	bz	SEQ_CORE_Clk_SendEvent_PNoOff
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bnz	SEQ_CORE_Clk_SendEvent_NoCC
	movf	SEQ_EVNTL, W, BANKED
	xorlw	0x1f
	bz	SEQ_CORE_Clk_SendEvent_CCNoOff
SEQ_CORE_Clk_SendEvent_NoCC
	rcall	SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr
	movff	MIDI_EVNT_PORT, INDF1
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	movff	SEQ_EVNT0, INDF1
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	movff	SEQ_EVNT1, INDF1
SEQ_CORE_Clk_SendEvent_PNoOff
SEQ_CORE_Clk_SendEvent_CCNoOff

	;; notify that a new gate length has to be set
	bsf	SEQ_STATE, SEQ_STATE_SET_LEN, BANKED

	;; ---
SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEventPostponed
SEQ_CORE_Clk_OnEventPlayed
	
	;; set length if requested (event has been played)
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SET_LEN, BANKED, SEQ_CORE_Clk_NoNewLen
SEQ_CORE_Clk_NewLen
	;; get scaled gatelength -> PROD[LH]
	movf	SEQ_EVNTL, W, BANKED
	rcall	SEQ_CORE_ScaleLen

	;; if retrigger flag is set or SEQ_EVNTL[6..5] > 0, we have to determine a new gatelength
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_NewLenReTrg
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, W
	andlw	0x60
	bz	SEQ_CORE_Clk_NewLen_NoRe
SEQ_CORE_Clk_NewLenReTrg
	;; note length: length = retrigger delay / 2
	clrc
	rrf	PRODH, F
	rrf	PRODL, F
SEQ_CORE_Clk_NewLen_NoRe

	;; ensure that length is > 1
	movf	PRODH, W
	bnz	SEQ_CORE_Clk_NewLen_Not0
	movf	PRODL, W
	skpnz
	incf	PRODL, F
SEQ_CORE_Clk_NewLen_Not0

	movlw	SEQ_TRKVARQUEUEL_Lx
	movff	PRODL, PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	movff	PRODH, PLUSW2
SEQ_CORE_Clk_NoNewLen

	;; ------------------------------------------------------------------
	;; request stretching if original gatelength entry (SEQ_EVNTL) is >= 24 and < 32
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_STRETCH_GL
	movlw	24-1
	cpfsgt	SEQ_EVNTL, BANKED
	rgoto SEQ_CORE_Clk_NoStretchReq
	movlw	32
	cpfslt	SEQ_EVNTL, BANKED
	rgoto SEQ_CORE_Clk_NoStretchReq
	movlw	SEQ_TRKVARQUEUEL_Lx
	setf	PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	setf	PLUSW2
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_STRETCH_GL
	movlw	SEQ_TRKVARRETRGx	; ensure that note won't be retriggered
	clrf	PLUSW2
	movlw	SEQ_TRKVARRETRGRELOADx
	clrf	PLUSW2
SEQ_CORE_Clk_NoStretchReq

	;; ------------------------------------------------------------------
	;; finally switch to next track until last track reached
	;; ------------------------------------------------------------------
	rgoto	SEQ_CORE_Clk_NextTrack

	;; ------------------------------------------------------------------
	;; all muted or switched off tracks should send note off events if
	;; required
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Muted
	;; skip if length already zero
	movlw	SEQ_TRKVARQUEUEL_Lx
	movf	PLUSW2, W
	bnz	SEQ_CORE_Clk_Muted_PlyOff
	movlw	SEQ_TRKVARQUEUEL_Hx
	movf	PLUSW2, W
	bnz	SEQ_CORE_Clk_Muted_PlyOff
	rgoto	SEQ_CORE_Clk_Muted_NoOff
SEQ_CORE_Clk_Muted_PlyOff
	rcall	SEQ_CORE_Hlp_PlayTrkOffEvnts
SEQ_CORE_Clk_Muted_NoOff

	;; ------------------------------------------------------------------
	;; switch to next track until last track reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextTrack
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_Clk_Loop

	;; ------------------------------------------------------------------

	;; now we can trigger requested pins at the same moment
	call	SEQ_TRK_SetTriggers

	;; ------------------------------------------------------------------

SEQ_CORE_Clk_End
	;; finally...

#if SEQ_CORE_MEASURE_PERFORMANCE
	bcf	LATC, 5
#endif
	return

	
;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKx
;;  IN: SEQ_EVNTT
;;  OUT: pointer in FSR0
;; --------------------------------------------------------------------------
SEQ_CORE_Calc_TRKx_FSR0
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTT, W, BANKED
	mullw	SEQ_TRKRECORD_LENGTH
	lfsr	FSR0, SEQ_TRK0
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKVARx
;;  IN: SEQ_EVNTT
;;  OUT: pointer in FSR2
;; --------------------------------------------------------------------------
SEQ_CORE_Calc_TRKVARx_FSR2
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTT, W, BANKED
	mullw	SEQ_TRKVARRECORD_LENGTH
	lfsr	FSR2, SEQ_TRKVAR0
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	return

;; --------------------------------------------------------------------------
;;  set the sequencer position via MIDI (F2 event)
;;  IN: low-byte in SEQ_MIDIPOS_LSB, high-byte in SEQ_MIDIPOS_MSB
;;  USES: TMP[45]
;; --------------------------------------------------------------------------
SEQ_CORE_SetPos
	;; reset song
	call	SEQ_SONG_Reset

	;; reset pattern
	rcall	SEQ_CORE_Reset

	;; in song mode:
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_SetPos_NoSong
SEQ_CORE_SetPos_Song
	;; increment song position counter until selected pos reached
	swapf	SEQ_MIDIPOS_LSB, W, BANKED
	andlw	0x0f
	movwf	TMP4
	swapf	SEQ_MIDIPOS_MSB, W, BANKED
	andlw	0xf0
	iorwf	TMP4, F
	swapf	SEQ_MIDIPOS_MSB, W, BANKED
	andlw	0x0f
	movwf	TMP5

	;; if TMP4 and TMP5 zero: fetch current settings, else increment
	movf	TMP4, W
	iorwf	TMP5, W
	bnz	SEQ_CORE_SetPos_SongIncLoop
SEQ_CORE_SetPos_SongNoInc
	call	SEQ_SONG_FetchPos
	rgoto	SEQ_CORE_SetPos_SongEnd

SEQ_CORE_SetPos_SongIncLoop
	clrwdt			; feed the watchdog (just to ensure...)
	call	SEQ_SONG_NextPos
	decf	TMP4, F 
	skpc
	decf	TMP5, F

	movf	TMP4, W
	iorwf	TMP5, W
	bnz	SEQ_CORE_SetPos_SongIncLoop
	
SEQ_CORE_SetPos_SongEnd
SEQ_CORE_SetPos_NoSong

	;; copy sequencer pos to reference counter
	SET_BSR	SEQ_BASE
	movf	SEQ_MIDIPOS_LSB, W, BANKED
	clrc
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x3c
	movwf	SEQ_CLK_STEP_CTR, BANKED
	decf	SEQ_CLK_STEP_CTR, F, BANKED
	setf	SEQ_CLK_TICK6_CTR, BANKED
	setf	SEQ_CLK_TICK4_CTR, BANKED

	;; set sequencer pos on all tracks
	movf	SEQ_MIDIPOS_LSB, W, BANKED
	andlw	0x1f
	movwf	TMP1

	;; store value in SEQ_TRKVARSTEPx
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_SetPos_Loop
	rcall	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	rcall	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; TODO: handle TRKVARSTEPx correctly when in backward mode
	rcall	SEQ_CORE_Hlp_GetLastAB
	andlw	0x1f		; TODO: issue with determining the length when A+B is selected!
	movwf	TMP2

	;; take tracklength into account! (new pos in TMP1)
	movff	TMP1, TMP3
SEQ_CORE_SetPos_LoopI
	movf	TMP2, W
	cpfsgt	TMP3, ACCESS
	rgoto SEQ_CORE_SetPos_LoopI_Break
	incf	TMP2, W
	subwf	TMP3, F
	rgoto	SEQ_CORE_SetPos_LoopI

SEQ_CORE_SetPos_LoopI_Break
	movlw	SEQ_TRKVARSTEPx
	movff	TMP3, PLUSW2

	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_SetPos_Loop

	return


;; --------------------------------------------------------------------------
;;  Use this function to change to new patterns
;;  IN: new pattern/bank numbers in SEQ_PATTERN/SEQ_PATTERN_BANK
;;  OUT: next pattern requests in SEQ_NEXT_PATTERNx/SEQ_NEXT_PATTERN_BANKx when patterns not equal
;;  USES: TMP1
;; --------------------------------------------------------------------------
	;; note: in song mode, we are using SEQ_CORE_ChangePattern_Song instead
	;; to skip the measure and pattern/bank check
	;; -> SEQ_CORE_ChangePattern_Song
SEQ_CORE_ChangePatternSynched
	SET_BSR	SEQ_BASE
	movf	SEQ_CHAIN_TOP, W
	skpz
	bsf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED

#if 0
	btfsc	SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED
	bsf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED
#endif

	;; check if SEQ_PATTERN_BANK != SEQ_NEXT_PATTERN_BANKx
	movf	SEQ_NEXT_PATTERN_BANK0, W, BANKED
	cpfseq	SEQ_PATTERN_BANK, BANKED
	rgoto SEQ_CORE_ChangePattern_Chng

	;; check if SEQ_PATTERN != SEQ_NEXT_PATTERNx
	movf	SEQ_NEXT_PATTERN0, W, BANKED
	cpfseq	SEQ_PATTERN, BANKED
	rgoto SEQ_CORE_ChangePattern_Chng

	;; no change
	rgoto	SEQ_CORE_ChangePattern_End

SEQ_CORE_ChangePattern_Song
SEQ_CORE_ChangePattern_Chng
	;; copy SEQ_PATTERN -> SEQ_NEXT_PATTERNx
	movff	SEQ_PATTERN, SEQ_NEXT_PATTERN0

	;; copy SEQ_PATTERN_BANK -> SEQ_NEXT_PATTERN_BANKx
	movff	SEQ_PATTERN_BANK, SEQ_NEXT_PATTERN_BANK0

	;; change immediately if sequencer not running
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_ChangePattern_Sync
SEQ_CORE_ChangePattern_NoSync
	;; set request flag
	bsf	SEQ_PATTERN_SYNC_TRANSFER_REQ, 0, BANKED
	rgoto	SEQ_CORE_ChangePattern_End

SEQ_CORE_ChangePattern_Sync
	bcf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED	; (not synched)

	;; change to new pattern immediately
	rcall	SEQ_CORE_ChangePattern

SEQ_CORE_ChangePattern_End
	return


;; --------------------------------------------------------------------------
;;  This function changes to a new pattern immediately
;;  IN: new pattern in SEQ_NEXT_PATTERNx/SEQ_NEXT_PATTERN_BANKx
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePattern
	;; clear request flag
	SET_BSR	SEQ_BASE
	bcf	SEQ_PATTERN_SYNC_TRANSFER_REQ, 0, BANKED

	;; transfer SEQ_NEXT_PATTERN_BANKx to SEQ_PATTERN_BANKx
	movff	SEQ_NEXT_PATTERN_BANK0, SEQ_PATTERN_BANK0

	;; transfer SEQ_NEXT_PATTERN*x to SEQ_PATTERN_*x
	movff	SEQ_NEXT_PATTERN0, SEQ_PATTERN0

	;; restore tracks
	call	SEQ_DUMP_RestoreTrks

	;; update disable flags and exit
	rgoto	SEQ_CORE_UpdateTrkDisable


;; --------------------------------------------------------------------------
;;  This function is used in SEQ_CORE_Tick to check, if the sequencer has
;;  to switch to new patterns
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePatternCheck
	SET_BSR	SEQ_BASE

	;; check if pattern has already been updated (SEQ_PATTERN_SYNC_TRANSFER_REQ[g] cleared)
	BRA_IFCLR SEQ_PATTERN_SYNC_TRANSFER_REQ, 0, BANKED, SEQ_CORE_ChangePatternCheckLoopN

	rcall	SEQ_CORE_ChangePattern

SEQ_CORE_ChangePatternCheckLoopN
	;; clear synch request
	bcf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED

	return

;; --------------------------------------------------------------------------
;;  This function should be called whenever a new pattern has been selected
;;  It updates the SEQ_TRKVARSTATE_DISABLE flag depending on SEQ_PATTERNx[7]
;;  and the availability of the selected bank
;;  In addition, it sets the "synch to measure" flag which ensures, that the
;;  clock divider will be reset properly once the next measure has been reached
;;  (only relevant for song mode)
;; --------------------------------------------------------------------------
SEQ_CORE_UpdateTrkDisable
	SET_BSR	SEQ_BASE

	;; temporary disable interrupts (since the interrupt handlers might also check the DISABLE flag)
	IRQ_DISABLE

	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_UpdateTrkDisableLoop
	call	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; clear the disable flag
	movlw	SEQ_TRKVARSTATEx + SEQ_TRKVARRECORD_LENGTH
	bcf	PLUSW2, SEQ_TRKVARSTATE_DISABLED

#if 0
	;; set the synch to measure flag (in song or "pattern synch" mode only)
	movf	SEQ_CHAIN_TOP, W
	bnz	SEQ_CORE_UpdateTrkDisable_Sync
	;; 	BRA_IFSET SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED, SEQ_CORE_UpdateTrkDisable_Sync
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_UpdateTrkDisable_NoSync
#endif
SEQ_CORE_UpdateTrkDisable_Sync
	movlw	SEQ_TRKVARSTATE2x
	bsf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
SEQ_CORE_UpdateTrkDisable_NoSync

	;; set the flags when whole pattern has been disabled (SEQ_PATTERNx[7] set)
	BRA_IFSET SEQ_PATTERN0, 7, BANKED, SEQ_CORE_UpdateTrkDisable_Ok

	;; set the flags if bankstick not available, bank != 0 and pattern number != 0
	movf	SEQ_PATTERN_BANK0, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_CORE_UpdateTrkDisable_Not

	;; bankstick not available - disable if bank != 0
	movf	SEQ_PATTERN_BANK0, W, BANKED
	bnz	SEQ_CORE_UpdateTrkDisable_Ok

	;; or disable if bank == 0 and pattern != 0
	movf	SEQ_PATTERN0, W, BANKED
	bz	SEQ_CORE_UpdateTrkDisable_Not
	;; 	rgoto	SEQ_CORE_UpdateTrkDisable_Match

SEQ_CORE_UpdateTrkDisable_Ok
	;; set the disable flag
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_DISABLED

SEQ_CORE_UpdateTrkDisable_Not
	;; loop 16 times
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto	SEQ_CORE_UpdateTrkDisableLoop

	;; interrupts can be enabled again
	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE0
;;  IN: SEQ_EVNTT
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE0_BEGIN

SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C
	movf	SEQ_EVNTT, W, BANKED
	mullw	4
	movf	PRODL, W
	return

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE1
;;  IN: SEQ_EVNTT
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE1_BEGIN
	rgoto	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUEP
;;  IN: SEQ_EVNTT
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUEP_BEGIN
	rgoto	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C

;; --------------------------------------------------------------------------
;;  sends the content of SEQ_TRKQUEUExx via MIDI and clears the queue
;;  IN: SEQ_EVNTT, pointer to SEQ_TRKVARx in FSR2
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_PlayOffEvnt
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr	; send first byte if != 0
	movf	INDF1, W
	bz	SEQ_CORE_Hlp_PlayOffEvnt_Skip
	movwf	MIDI_EVNT0

	rcall	SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr	; determine port
	movf	INDF1, W
	movwf	MIDI_EVNT_PORT

	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr	; send second byte
	movf	INDF1, W
	movwf	MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE			; send 0x00
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE			; fix BSR

SEQ_CORE_Hlp_PlayOffEvnt_Skip
	;; clear queue and length
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	clrf	INDF1
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	clrf	INDF1
	movlw	SEQ_TRKVARQUEUEL_Lx
	clrf	PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	clrf	PLUSW2
	return

;; --------------------------------------------------------------------------
;;  This function plays the off events of a selected track
;;  IN: 
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_PlayTrkOffEvnts
	;; (called 4 times in MBSEQ V3 - function exists for compatibility reasons)
	rgoto	SEQ_CORE_Hlp_PlayOffEvnt


;; --------------------------------------------------------------------------
;;  This function plays the off events of all tracks
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_PlayAllOffEvnts
	;; play off events
	SET_BSR	SEQ_BASE
	lfsr	FSR2, SEQ_TRKVAR0
	clrf	SEQ_EVNTT, BANKED

SEQ_CORE_Hlp_PlayAllOffEvntsOL
	clrwdt
	rcall	SEQ_CORE_Hlp_PlayOffEvnt; play off event of selected track

	movlw	SEQ_TRKVARRECORD_LENGTH	; switch to next record
	addwf	FSR2L, F
	movlw	0
	addwfc	FSR2H, F
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_Hlp_PlayAllOffEvntsOL

	;; also for the metronome
	movf	SEQ_METRONOME_OFF_EVNT0, W, BANKED
	bz	SEQ_CORE_Hlp_PlayAllOffEvnts_End
	movff	SEQ_METRONOME_OFF_EVNT0, MIDI_EVNT0
	clrf	SEQ_METRONOME_OFF_EVNT0, BANKED; (don't play it again)
	movff	SEQ_METRONOME_OFF_EVNT1, MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE
	clrf	MIDI_EVNT_PORT	; (always default port)
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE	
SEQ_CORE_Hlp_PlayAllOffEvnts_End
	return

;; --------------------------------------------------------------------------
;;  This function generates a new random number
;;  OUT: new random number in SEQ_RANDOM_SEED_[LH]
;; --------------------------------------------------------------------------
SEQ_CORE_GenRandomNumber
	SET_BSR	SEQ_BASE
	movf	SEQ_RANDOM_SEED_L, W, BANKED
	mulwf	SEQ_RANDOM_SEED_H, BANKED
	movf	TMR0L, W
	addwf	PRODL, W
	movwf	SEQ_RANDOM_SEED_L, BANKED
	movlw	0x69
	addwfc	TMR1L, W
	addwfc	PRODH, W
	movwf	SEQ_RANDOM_SEED_H, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function updates the meter which is displayed in MUTE menu page
;; IN: MIDI event in MIDI_EVNT[01] and MIDI_EVNT_VALUE
;;     track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_CORE_UpdateMeter
	SET_BSR	SEQ_BASE

	lfsr	FSR1, CS_MENU_METER_CTR_BEGIN
	movf	SEQ_EVNTT, W, BANKED
	addwf	FSR1L, F

	;; if note event: update depending on velocity, otherwise always use max value
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bz	SEQ_CORE_UpdateMeter_Note
SEQ_CORE_UpdateMeter_NoNote
	movlw	0x7f
	rgoto	SEQ_CORE_UpdateMeter_Cont

SEQ_CORE_UpdateMeter_Note
	movf	MIDI_EVNT_VALUE, W	; (velocity)
	;; 	rgoto	SEQ_CORE_UpdateMeter_Cont

SEQ_CORE_UpdateMeter_Cont
	movwf	INDF1

	;; set liveplay meter flag as well
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_EVNTT, 3, BANKED
	iorwf	SEQ_LIVEPLAY_METERS0, F, BANKED
	btfsc	SEQ_EVNTT, 3, BANKED
	iorwf	SEQ_LIVEPLAY_METERS1, F, BANKED

	return

;; --------------------------------------------------------------------------
;;  Determine next step depending on direction mode
;;  IN: pointer to SEQ_TRKx in FSR0, pointer to SEQ_TRKVARx in FSR2
;;  OUT: SEQ_TRKVARSTEPx:  next step
;;       SEQ_TRKVARDIVCTRx: divider counter always cleared
;;       SEQ_TRKVARSTATEx: PINGPONG and PLAY_STEP modified
;; --------------------------------------------------------------------------
	;; define the bits of TMP3
#define SEQ_CORE_NEXTSTEP_TMP3_PINGPONG 0
#define SEQ_CORE_NEXTSTEP_TMP3_PENDULUM 1
#define SEQ_CORE_NEXTSTEP_TMP3_SAVESTEP 2
#define SEQ_CORE_NEXTSTEP_TMP3_REVERSE  3

SEQ_CORE_NextStep_Reverse
	;; (for nudge Rew function)
	movlw	(1 << SEQ_CORE_NEXTSTEP_TMP3_REVERSE)
	movwf	TMP3
	rgoto	SEQ_CORE_NextStep_Reverse_Cont

SEQ_CORE_NextStep
	;; clear TMP3, which holds some temporary flags (see definitions above)
	clrf	TMP3
SEQ_CORE_NextStep_Reverse_Cont

	;; extra for MB808: AB section handling
	;; if A+B selected: no special handling
	;; if A selected: switch immediately to A section if current step is located in B section
	;; if B selected: switch immediately to B section if current step is located in A section
	movf	SEQ_SELECTED_AB, W
	andlw	0x03
	addlw	-1
	bz	SEQ_CORE_NextStep_SectionA
	addlw	-1
	bz	SEQ_CORE_NextStep_SectionB
	rgoto	SEQ_CORE_NextStep_Section_Cont
SEQ_CORE_NextStep_SectionA
	movlw	SEQ_TRKVARSTEPx
	bcf	PLUSW2, 5	; select step 0..31
	movlw	SEQ_TRKVARSTEPSAVEDx
	bcf	PLUSW2, 5	; select step 0..31
	rgoto	SEQ_CORE_NextStep_Section_Cont
	
SEQ_CORE_NextStep_SectionB
	movlw	SEQ_TRKVARSTEPx
	bsf	PLUSW2, 5	; select step 32..63
	movlw	SEQ_TRKVARSTEPSAVEDx
	bsf	PLUSW2, 5	; select step 32..63
	;; 	rgoto	SEQ_CORE_NextStep_Section_Cont

SEQ_CORE_NextStep_Section_Cont


	;; notify that a step should be played
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_PLAY_STEP


	;; ignore progression parameters if position should be reset
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_PrgFwd_Cont

	;; progression parameters:
	;; increment and check forward counter
	movlw	SEQ_TRKVARSTEPFWDx
	incf	PLUSW2, F
	decf	PLUSW2, W
	movwf	TMP1
	;; max value located in TRKDIR2x[3:0]
	movlw	SEQ_TRKDIR2x
	movf	PLUSW0, W
	andlw	0x0f
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_NextStep_NoPrgFwd
	rgoto	SEQ_CORE_NextStep_PrgFwd_Cont
	
SEQ_CORE_NextStep_NoPrgFwd
	;; reset forward counter
	movlw	SEQ_TRKVARSTEPFWDx
	clrf	PLUSW2

	;; jump back? - located in TRKDIR2x[6:4]
	movlw	SEQ_TRKDIR2x
	swapf	PLUSW0, W
	andlw	0x07
	bz	SEQ_CORE_NextStep_NoPrgBck
SEQ_CORE_NextStep_PrgBck
	movwf	TMP5		; using TMP5 as loop counter (should not be used anywhere else within this routine!!!)
	movlw	SEQ_TRKVARSTATEx; (reverse direction)
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CORE_NextStep_PrgBckFwdLoop
SEQ_CORE_NextStep_PrgBckBckLoop
	rcall	SEQ_CORE_NextStep_Bck
	decfsz	TMP5, F
	rgoto	SEQ_CORE_NextStep_PrgBckBckLoop
	rgoto	SEQ_CORE_NextStep_PrgBck_Cont

SEQ_CORE_NextStep_PrgBckFwdLoop
	rcall	SEQ_CORE_NextStep_Fwd
	decfsz	TMP5, F
	rgoto	SEQ_CORE_NextStep_PrgBckFwdLoop
	;; 	rgoto	SEQ_CORE_NextStep_PrgBck_Cont

SEQ_CORE_NextStep_PrgBck_Cont
SEQ_CORE_NextStep_NoPrgBck

	;; increment and check replay counter
SEQ_CORE_NextStep_ReplayChk
	movlw	SEQ_TRKVARSTEPREPLYx
	incf	PLUSW2, F
	decf	PLUSW2, W
	movwf	TMP1
	;; max value located in TRKDIR1x[6:4]
	movlw	SEQ_TRKDIR1x
	swapf	PLUSW0, W
	andlw	0x07
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_NextStep_NoReplay
	;; back to saved position and exit
	movlw	SEQ_TRKVARSTEPSAVEDx
	movff	PLUSW2, TMP2
	movlw	SEQ_TRKVARSTEPx
	movff	TMP2, PLUSW2
	rgoto	SEQ_CORE_NextStep_End
SEQ_CORE_NextStep_NoReplay
	;; reset replay counter
	movlw	SEQ_TRKVARSTEPREPLYx
	clrf	PLUSW2
	;; request save position
	bsf	TMP3, SEQ_CORE_NEXTSTEP_TMP3_SAVESTEP

SEQ_CORE_NextStep_PrgFwd_Cont

	;; continue depending on direction
	movlw	SEQ_TRKDIR1x
	movf	PLUSW0, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_CORE_NextStep_Fwd	; SEQ_TRKDIR_FORWARD
	rgoto	SEQ_CORE_NextStep_Bck	; SEQ_TRKDIR_BACKWARD
	rgoto	SEQ_CORE_NextStep_PP	; SEQ_TRKDIR_PINGPONG
	rgoto	SEQ_CORE_NextStep_Pen	; SEQ_TRKDIR_PENDULUM
	rgoto	SEQ_CORE_NextStep_RD	; SEQ_TRKDIR_RANDOM_DIR
	rgoto	SEQ_CORE_NextStep_RS	; SEQ_TRKDIR_RANDOM_STEP
	rgoto	SEQ_CORE_NextStep_RDS	; SEQ_TRKDIR_RANDOM_D_S
	rgoto	SEQ_CORE_NextStep_RDS	; dummy
	rgoto	SEQ_CORE_NextStep_RDS	; dummy


	;; ---[ Forward ]----------------------------------------------------
SEQ_CORE_NextStep_Fwd
	;; if reverse flag set, branch to _Bck
	BRA_IFSET TMP3, SEQ_CORE_NEXTSTEP_TMP3_REVERSE, ACCESS, SEQ_CORE_NextStep_FwdReverse
SEQ_CORE_NextStep_BckReverse

	;; store current direction
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; jump to first step if reset has been requested
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_FwdR

	;; jump to first (loop) step if last step has been reached
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, TMP1
	rcall	SEQ_CORE_Hlp_GetLastAB
	andlw	0x3f
	cpfslt	TMP1, ACCESS
	rgoto	SEQ_CORE_NextStep_FwdR
	movwf	TMP2		; store last step in TMP2 for PingPong comparison

	;; otherwise increment step
	movlw	SEQ_TRKVARSTEPx
	incf	PLUSW2, F

	;; in pingpong mode: turn direction if last step has been reached after this increment
	movf	PLUSW2, W	; current step
	xorwf	TMP2, W		; last step
	bnz	SEQ_CORE_NextStep_Fwd_NoPPT

	BRA_IFCLR TMP3, SEQ_CORE_NEXTSTEP_TMP3_PINGPONG, ACCESS, SEQ_CORE_NextStep_Fwd_NoPP
SEQ_CORE_NextStep_Fwd_PP
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
SEQ_CORE_NextStep_Fwd_NoPPT
SEQ_CORE_NextStep_Fwd_NoPP
	rgoto	SEQ_CORE_NextStep_Cont

	;; reset to first loop step
SEQ_CORE_NextStep_FwdR
	;; in pendulum mode: switch to backward direction
	BRA_IFCLR TMP3, SEQ_CORE_NEXTSTEP_TMP3_PENDULUM, ACCESS, SEQ_CORE_NextStep_Fwd_NoPen
SEQ_CORE_NextStep_Fwd_Pen
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
	rgoto	SEQ_CORE_NextStep_Cont

SEQ_CORE_NextStep_Fwd_NoPen
	;; set first step
	rcall	SEQ_CORE_Hlp_GetLoopAB
	andlw	0x3f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW2
	rgoto	SEQ_CORE_NextStep_Cont


	;; ---[ Backward ]---------------------------------------------------
SEQ_CORE_NextStep_Bck
	;; if reverse flag set, branch to _Fwd
	BRA_IFSET TMP3, SEQ_CORE_NEXTSTEP_TMP3_REVERSE, ACCESS, SEQ_CORE_NextStep_BckReverse
SEQ_CORE_NextStep_FwdReverse

	;; store current direction
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; jump to last step if reset has been requested
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_BckR

	;; jump to last step if first loop step has been reached
	movlw	SEQ_TRKLOOPAx		; loop point of A section
	btfsc	SEQ_SELECTED_AB, 1, BANKED
	movlw	SEQ_TRKLOOPBx		; loop point of B section
	movf	PLUSW0, W
	andlw	0x3f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW2, W
	xorwf	PRODL, W
	bz	SEQ_CORE_NextStep_BckR

	;; otherwise decrement step
	movlw	SEQ_TRKVARSTEPx
	decf	PLUSW2, F

	;; in pingpong mode: turn direction if first loop step has been reached after this decrement
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW2, W
	xorwf	PRODL, W
	bnz	SEQ_CORE_NextStep_Bck_NoPPT
	BRA_IFCLR TMP3, SEQ_CORE_NEXTSTEP_TMP3_PINGPONG, ACCESS, SEQ_CORE_NextStep_Bck_NoPP
SEQ_CORE_NextStep_Bck_PP
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
SEQ_CORE_NextStep_Bck_NoPPT
SEQ_CORE_NextStep_Bck_NoPP
	rgoto	SEQ_CORE_NextStep_Cont

	;; reset to last step
SEQ_CORE_NextStep_BckR
	;; in pendulum mode: switch to forward direction
	BRA_IFCLR TMP3, SEQ_CORE_NEXTSTEP_TMP3_PENDULUM, ACCESS, SEQ_CORE_NextStep_Bck_NoPen
SEQ_CORE_NextStep_Bck_Pen
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
	rgoto	SEQ_CORE_NextStep_Cont
SEQ_CORE_NextStep_Bck_NoPen
	;; set last step (depending on track length)
	rcall	SEQ_CORE_Hlp_GetLastAB
	andlw	0x3f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW2

	rgoto	SEQ_CORE_NextStep_Cont
SEQ_CORE_NextStep_BckP

	;; ---[ Ping Pong ]--------------------------------------------------
SEQ_CORE_NextStep_PP
	;; notify that we are in ping pong mode
	bsf	TMP3, SEQ_CORE_NEXTSTEP_TMP3_PINGPONG

	;; branch to _Fwd/_Bck depending on current direction
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CORE_NextStep_Bck
	rgoto	SEQ_CORE_NextStep_Fwd

	;; ---[ Pendulum ]---------------------------------------------------
SEQ_CORE_NextStep_Pen
	;; notify that we are in pendulum mode
	bsf	TMP3, SEQ_CORE_NEXTSTEP_TMP3_PENDULUM

	;; branch to _Fwd/_Bck depending on current direction
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CORE_NextStep_Bck
	rgoto	SEQ_CORE_NextStep_Fwd

	;; ---[ Random Direction ]-------------------------------------------
SEQ_CORE_NextStep_RD
	rcall	SEQ_CORE_GenRandomNumber	; generate a new random number

	;; set new random direction
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
	btfsc	SEQ_RANDOM_SEED_L, 4, BANKED
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CORE_NextStep_Bck
	rgoto	SEQ_CORE_NextStep_Fwd

	;; ---[ Random Step ]------------------------------------------------
SEQ_CORE_NextStep_RS
	;; iterate until new random step within given track range (from loop to length) has been found
	movlw	SEQ_TRKLOOPAx		; loop point of A section
	btfsc	SEQ_SELECTED_AB, 1, BANKED
	movlw	SEQ_TRKLOOPBx		; loop point of B section
	movf	PLUSW0, W
	andlw	0x3f
	movwf	TMP1
	rcall	SEQ_CORE_Hlp_GetLastAB
	andlw	0x3f
	movwf	TMP2

	;; no special measure of TMP1 == TMP2
	movwf	PRODL
	xorwf	TMP1, W
	bz	SEQ_CORE_NextStep_RS_Found

SEQ_CORE_NextStep_RS_Loop
	rcall	SEQ_CORE_GenRandomNumber	; generate a new random number
	movf	SEQ_RANDOM_SEED_H, W, BANKED
	andlw	0x1f
	movwf	PRODL
	movf	TMP1, W
	bz	SEQ_CORE_NextStep_RS_Loop_IgnS
	addlw	-1
	cpfsgt	PRODL, ACCESS
	rgoto SEQ_CORE_NextStep_RS_Loop
SEQ_CORE_NextStep_RS_Loop_IgnS
	incf	TMP2, W
	cpfslt	PRODL, ACCESS
	rgoto SEQ_CORE_NextStep_RS_Loop

SEQ_CORE_NextStep_RS_Found
	movlw	SEQ_TRKVARSTEPx			; step within range found
	movff	PRODL, PLUSW2

	rgoto	SEQ_CORE_NextStep_Cont
	

	;; ---[ Random Direction + Step ]------------------------------------
SEQ_CORE_NextStep_RDS
	;; we continue with a propability of 50%
	;; we jump to a new step with a propability of 25%
	;; we change the direction with a propability of 25%
	rcall	SEQ_CORE_GenRandomNumber	; generate a new random number
	BRA_IFSET SEQ_RANDOM_SEED_H, 7, BANKED, SEQ_CORE_NextStep_RDS_Cont
	BRA_IFSET SEQ_RANDOM_SEED_H, 6, BANKED, SEQ_CORE_NextStep_RS
	rgoto	SEQ_CORE_NextStep_RD
SEQ_CORE_NextStep_RDS_Cont
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_BACKWARD, ACCESS, SEQ_CORE_NextStep_Bck
	rgoto	SEQ_CORE_NextStep_Fwd

	
SEQ_CORE_NextStep_Cont
	;; re-init LAST_POS variable if position reset was requested
	movlw	SEQ_TRKVARSTATEx
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_NoPosR
	bcf	PLUSW2, SEQ_TRKVARSTATE_POS_RESET
SEQ_CORE_NextStep_PosR
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, SEQ_CORE_TRK_LASTPOS
	movlw	SEQ_TRKVARSTEPREPLYx
	clrf	PLUSW2
	movlw	SEQ_TRKVARSTEPFWDx
	clrf	PLUSW2
	movlw	SEQ_TRKVARSTEPSAVEDx
	clrf	PLUSW2
SEQ_CORE_NextStep_NoPosR

	;; check if new step should be saved (for repeat mechanism)
	BRA_IFCLR TMP3, SEQ_CORE_NEXTSTEP_TMP3_SAVESTEP, ACCESS, SEQ_CORE_NextStep_NoSave
SEQ_CORE_NextStep_Save
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, TMP2
	movlw	SEQ_TRKVARSTEPSAVEDx
	movff	TMP2, PLUSW2
SEQ_CORE_NextStep_NoSave

SEQ_CORE_NextStep_End
	return


;; --------------------------------------------------------------------------
;;  Resets the track position to the first step
;;  IN: pointer to SEQ_TRKx in FSR0, pointer to SEQ_TRKVARx in FSR2
;; --------------------------------------------------------------------------
SEQ_CORE_ResetTrkPos
	;; don't increment on the first clock event
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_FIRST_CLK

	;; reset step REPLAY and FWD counters
	movlw	SEQ_TRKVARSTEPREPLYx
	clrf	PLUSW2
	movlw	SEQ_TRKVARSTEPFWDx
	clrf	PLUSW2

	;; reset clock divider
	movlw	SEQ_TRKVARDIVCTRx
	clrf	PLUSW2

	;; reset retrigger counter
	movlw	SEQ_TRKVARRETRGx
	clrf	PLUSW2

	;; reset record state and other record specific variables
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, SEQ_CORE_ResetTrkPos_RASSkip	; (MEMO: don't know another way yet to avoid that the note length of the first note won't be recorded in autostart mode)
	movlw	SEQ_TRKVARSTATE2x
	bcf	PLUSW2, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE
	clrf	SEQ_RECORD_STEP, BANKED
	clrf	SEQ_RECORD_LENGTH_CTR, BANKED
SEQ_CORE_ResetTrkPos_RASSkip

	;; branch depending on forward/backward direction
	movlw	SEQ_TRKDIR1x
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	SEQ_TRKDIR_BACKWARD
	bnz	SEQ_CORE_ResetTrkPos_F
SEQ_CORE_ResetTrkPos_B
	;; Backward direction
	;; set "backward" flag
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; determine track position and copy it to SEQ_TRKVARSTEPx
	;; it's located in SEQ_TRKLAST[AB]x[5..0] (last step)
	rcall	SEQ_CORE_Hlp_GetLastAB
	andlw	0x3f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW2
	rgoto	SEQ_CORE_ResetTrkPos_B_Cont

SEQ_CORE_ResetTrkPos_F
	;; Forward/PingPong/Random direction
	;; clear "backward" flag
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; reset track position
	movlw	SEQ_TRKVARSTEPx
	clrf	PLUSW2

SEQ_CORE_ResetTrkPos_B_Cont
	;; save position (for repeat function)
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, PRODL
	movlw	SEQ_TRKVARSTEPSAVEDx
	movff	PRODL, PLUSW2

	return


;; --------------------------------------------------------------------------
;;  This function returns the last step of a track depending on A/B selection:
;;  A selected -> return content of TRKLASTAx
;;  B selected -> return content of TRKLASTBx
;;  A+B selected -> return content of TRKLASTBx (end is at section B)
;;  IN: pointer to SEQ_TRKx in FSR0
;;  OUT: loop step
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetLastAB
	SET_BSR	SEQ_BASE

	movf	SEQ_SELECTED_AB, W, BANKED
	skpnz
	movlw	0x01		; (invalid selection...)
	andlw	0x03
	addlw	-1
	bz	SEQ_CORE_Hlp_GetLastAB_A
	addlw	-1
	bz	SEQ_CORE_Hlp_GetLastAB_B
	;; A+B - same as B

SEQ_CORE_Hlp_GetLastAB_B
	movlw	SEQ_TRKLASTBx		; last step of B section
	movf	PLUSW0, W
	return

SEQ_CORE_Hlp_GetLastAB_A
	movlw	SEQ_TRKLASTAx		; last step of A section
	movf	PLUSW0, W
	return


;; --------------------------------------------------------------------------
;;  This function returns the loop point of a track depending on A/B selection:
;;  A selected -> return content of TRKLOOPAx
;;  B selected -> return content of TRKLOOPBx
;;  A+B selected -> return content of TRKLOOPAx (restart at section A)
;;  IN: pointer to SEQ_TRKx in FSR0
;;  OUT: loop step
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetLoopAB
	SET_BSR	SEQ_BASE

	movf	SEQ_SELECTED_AB, W, BANKED
	skpnz
	movlw	0x01		; (invalid selection...)
	andlw	0x03
	addlw	-1
	bz	SEQ_CORE_Hlp_GetLoopAB_A
	addlw	-1
	bz	SEQ_CORE_Hlp_GetLoopAB_B
	;; A+B - same as A

SEQ_CORE_Hlp_GetLoopAB_A
	movlw	SEQ_TRKLOOPAx		; loop point of A section
	movf	PLUSW0, W
	return

SEQ_CORE_Hlp_GetLoopAB_B
	movlw	SEQ_TRKLOOPBx		; loop point of A section
	movf	PLUSW0, W
	return


;; --------------------------------------------------------------------------
;;  This function returns the gatelength scaled over the clock divider value
;;  IN: gatelength in WREG, pointer to SEQ_TRKx in FSR0
;;  OUT: scaled gatelength in PROD[LH]
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SEQ_CORE_ScaleLen
	;; calculate gatelength
	;; it's a scaled value, so that the relation between configured length and step resolution matches
	;; length can be varied from 1 to 24
	;; divider can be varied from 1..64
	;; with divider value 4 a step has a resolution of 24 ticks
	;; calculation examples:
	;; divider  1:   6 = 24 *  0.25 = 24 *  1/4
	;; divider  2:  12 = 24 *  0.5  = 24 *  2/4
	;; divider  4:  24 = 24 *  1    = 24 *  4/4
	;; divider  8:  48 = 24 *  2    = 24 *  8/4
	;; divider 16:  96 = 24 *  4    = 24 * 16/4
	;; divider 32: 192 = 24 *  8    = 24 * 32/4
	;; divider 64: 384 = 24 * 16    = 24 * 64/4
	;; -> scaled length = length * divider / 4

	;; this is what we are doing here:

	andlw	0x1f		; mask out 5bit value
	movwf	PRODL		; temporary store result in PRODL

	movlw	SEQ_TRKDIVx	; (0..63)
	movf	PLUSW0, W
	andlw	0x3f
	addlw	1		; (1..64)

	mulwf	PRODL, ACCESS	; multiply with length

	clrc
	rrf	PRODH, F	; divide result by 4
	rrf	PRODL, F
	clrc
	rrf	PRODH, F
	rrf	PRODL, F

	;; ensure that length is at least 1
	movf	PRODL, W
	iorwf	PRODH, W
	skpnz
	incf	PRODL, F

	return

;; --------------------------------------------------------------------------
;;  This function is called from SEQ_CORE_Clk_Step when record mode is active
;;  and a new step is played - it can modify SEQ_EVNTL of the current and
;;  previous step depending on the gate flag SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE
;;  IN: current step in SEQ_EVNTS
;;      previous step in SEQ_CORE_TRK_LASTPOS
;;      pointer to SEQ_TRKVARx in FSR2
;; --------------------------------------------------------------------------
SEQ_CORE_RecStep
	SET_BSR	SEQ_BASE

	;; only relevant for live recording mode
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, SEQ_CORE_RecStep_End

	;; check if event active
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE, ACCESS, SEQ_CORE_RecStep_NoGate
SEQ_CORE_RecStep_Gate
	;; clear gate of new step (overwrite function)
	clrf	SEQ_EVNT1, BANKED	; (not relevant)
	clrf	SEQ_EVNT2, BANKED
	call	SEQ_LAYER_SaveEvnt
SEQ_CORE_RecStep_NoGate

SEQ_CORE_RecStep_End
	return


;; --------------------------------------------------------------------------
;;  This function is called when all tracks should be "nudged" backward
;; --------------------------------------------------------------------------
SEQ_CORE_NudgeRew
	;; loop through all tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_NudgeRew_Loop
	call	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	call	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; select previous step (change takes place on next clock)
	call	SEQ_CORE_NextStep_Reverse
#if 1
	;; usability check
	call	SEQ_CORE_NextStep_Reverse
	call	SEQ_CORE_NextStep_Reverse
	call	SEQ_CORE_NextStep_Reverse
#endif
	
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto	SEQ_CORE_NudgeRew_Loop
	

	;; decrement step counter
	movlw	-4*4		; (4 times resolution)
	addwf	SEQ_CLK_STEP_CTR, F, BANKED

	;; reset on underrun
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0xc0
	bz	SEQ_CORE_NudgeRew_NoOver
SEQ_CORE_NudgeRew_Over
	movlw	0x3f
	andwf	SEQ_CLK_STEP_CTR, F, BANKED
	;; (here we could switch to previous pos)
	;; 	call	SEQ_CORE_ChainPrevPos
SEQ_CORE_NudgeRew_NoOver

	return

;; --------------------------------------------------------------------------
;;  This function is called when all tracks should be "nudged" forward
;; --------------------------------------------------------------------------
SEQ_CORE_NudgeFwd
	;; loop through all tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_NudgeFwd_Loop
	call	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	call	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; jump 4 steps (change takes place on next clock)
	call	SEQ_CORE_NextStep
	call	SEQ_CORE_NextStep
	call	SEQ_CORE_NextStep
	call	SEQ_CORE_NextStep

	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto	SEQ_CORE_NudgeFwd_Loop


	;; increment step counter
	movlw	4*4		; (4 times resolution)
	addwf	SEQ_CLK_STEP_CTR, F, BANKED

	;; reset on overrun (and increment pattern if in chain mode)
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0xc0
	bz	SEQ_CORE_NudgeFwd_NoOver
SEQ_CORE_NudgeFwd_Over
	movlw	0x3f
	andwf	SEQ_CLK_STEP_CTR, F, BANKED
	call	SEQ_CORE_ChainNextPos
SEQ_CORE_NudgeFwd_NoOver

	
	return


;; --------------------------------------------------------------------------
;;  This function clears the pattern chain
;; --------------------------------------------------------------------------
SEQ_CORE_ChainClr
	SET_BSR	SEQ_BASE

	clrf	PRODL
	lfsr	FSR1, SEQ_PATTERN_CHAIN
SEQ_CORE_ChainClrLoop
	clrf	POSTINC1
	incf	PRODL, F
	bnz	SEQ_CORE_ChainClrLoop

	clrf	SEQ_CHAIN_PLAY_PTR, BANKED
	clrf	SEQ_CHAIN_TOP, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function adds a pattern/bank to the chain
;;  IN: pattern in SEQ_PATTERN, bank in SEQ_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_CORE_ChainAdd
	SET_BSR	SEQ_BASE

	;; exit if top of chain reached
	movf	SEQ_CHAIN_TOP, W
	xorlw	0x7f
	bz	SEQ_CORE_ChainAdd_End

	;; determine pointer to chain entry
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrc
	rlf	SEQ_CHAIN_TOP, W, BANKED
	addwf	FSR1L, F

	;; add bank and pattern to chain
	movff	SEQ_PATTERN_BANK, POSTINC1
	movff	SEQ_PATTERN, POSTINC1

	;; increment top pointer
	incf	SEQ_CHAIN_TOP, F, BANKED

SEQ_CORE_ChainAdd_End
	return


;; --------------------------------------------------------------------------
;;  This function changes SEQ_PATTERN0/SEQ_PATTERN_BANK0 to the next chain
;;  entry.
;; --------------------------------------------------------------------------
SEQ_CORE_ChainNextPos
	SET_BSR	SEQ_BASE

	movf	SEQ_CHAIN_TOP, W
	bz	SEQ_CORE_ChainNextPos_End

	;; determine pointer to chain entry
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrc
	rlf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	addwf	FSR1L, F

	;; get bank and pattern from chain
	movff	POSTINC1, SEQ_PATTERN_BANK
	movff	POSTINC1, SEQ_PATTERN

	;; increment play pointer and wrap on top entry
	incf	SEQ_CHAIN_PLAY_PTR, F, BANKED
	movf	SEQ_CHAIN_TOP, W, BANKED
	cpfslt	SEQ_CHAIN_PLAY_PTR, BANKED
	clrf	SEQ_CHAIN_PLAY_PTR

	;; compare with currently played pattern
	;; don't change if identical
	movf	SEQ_PATTERN0, W, BANKED
	xorwf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bnz	SEQ_CORE_ChainNextPos_New
	movf	SEQ_PATTERN_BANK0, W, BANKED
	xorwf	SEQ_PATTERN_BANK, W, BANKED
	bnz	SEQ_CORE_ChainNextPos_New
	rgoto	SEQ_CORE_ChainNextPos_Cont

SEQ_CORE_ChainNextPos_New
	;; change to new pattern immediately
	movff	SEQ_PATTERN, SEQ_NEXT_PATTERN0
	movff	SEQ_PATTERN_BANK, SEQ_NEXT_PATTERN_BANK0
	rcall	SEQ_CORE_ChangePattern
	;; 	rgoto	SEQ_CORE_ChainNextPos_Cont

SEQ_CORE_ChainNextPos_Cont
	;; if oneshot mode: clear chain if last position reached
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_CHAIN_ONESHOT, BANKED, SEQ_CORE_ChainNextPos_NoClear
	movf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	bnz	SEQ_CORE_ChainNextPos_NoClear
SEQ_CORE_ChainNextPos_Clear
	rcall	SEQ_CORE_ChainClr
SEQ_CORE_ChainNextPos_NoClear

SEQ_CORE_ChainNextPos_End
	return
