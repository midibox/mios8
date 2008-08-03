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
	;; free: 4
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
SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP EQU	1	; for length...
SEQ_TRKVARSTATE2_SYNC_MEASURE	 EQU	2	; temporary request for synch to measure (used during pattern switching)
SEQ_TRKVARSTATE2_HNDSHK_DIS	EQU	3	; for LED matrix display: 1 if track is not played due to handshake

SEQ_TRKMODE_OFF			EQU	0x00	; not bits, but values!
SEQ_TRKMODE_NORMAL		EQU	0x01
SEQ_TRKMODE_TRANSPOSE		EQU	0x02
SEQ_TRKMODE_ARPEGGIATOR		EQU	0x03

SEQ_TRKMODE_FLAG_UNSORTED	EQU	3	; sort mode for arpeggiator
SEQ_TRKMODE_FLAG_HOLD		EQU	4	; hold mode for transpose/arpeggiator
SEQ_TRKMODE_FLAG_RESTART	EQU	5	; track restart on key press
SEQ_TRKMODE_FLAG_FORCE_SCALE	EQU	6	; note values are forced to scale
SEQ_TRKMODE_FLAG_SUSTAIN	EQU	7	; events are sustained

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
SEQ_RECORD_MODE_REC_LAYER_A	EQU	4	; if 1: record layer A
SEQ_RECORD_MODE_REC_LAYER_B	EQU	5	; if 1: record layer B
SEQ_RECORD_MODE_REC_LAYER_C	EQU	6	; if 1: record layer C
SEQ_RECORD_MODE_REC_CC		EQU	7	; if 1: CC should be recorded, otherwise a note

	;; (order of flags has to be aligned with LED output routine in seq_leds.inc!)
SEQ_MODE0_STEP_EDIT		EQU	0	; we are in step edit mode
SEQ_MODE0_MUTE			EQU	1	; change mute status with GP buttons
SEQ_MODE0_PATTERN		EQU	2	; change pattern with GP buttons
SEQ_MODE0_SONG			EQU	3	; we are in song mode
SEQ_MODE0_MENU			EQU	4	; change menu with GP buttons
SEQ_MODE0_FAST_ENCODERS		EQU	5	; encoders in fast speed mode
SEQ_MODE0_CHANGE_ALL_STEPS	EQU	6	; change all steps
SEQ_MODE0_SOLO			EQU	7	; play current track solo

SEQ_MODE1_METRONOME		EQU	0	; enable metronome
SEQ_MODE1_SCRUB			EQU	1	; enable scrub mode
SEQ_MODE1_RECORD		EQU	2	; we are in step record menu
SEQ_MODE1_MUTE_HOLD		EQU	3	; if mute button is held
SEQ_MODE1_MUTE_HOLD_MULTISEL	EQU	4	; used within the MUTE menu to select multiple tracks so long the mute button is held

SEQ_TRKCHAIN_12			EQU	0	; track 1/2 chained
SEQ_TRKCHAIN_34			EQU	1	; track 3/4 chained
SEQ_TRKCHAIN_1234		EQU	2	; track 1/2/3/4 chained

;; MEMO chain coding:
;;   - stored in SEQ_TRKCHAINx of track 1 only
;;   - chaining only possible within pattern (track 1/2/3/4, 5/6/7/8, etc...)
;;   - TRK1:SEQ_TRKCHAINx[0]: if set, chain between track 1 and 2
;;   - TRK1:SEQ_TRKCHAINx[1]: if set, chain between track 3 and 4
;;   - TRK1:SEQ_TRKCHAINx[2]: if set, chain between track 1/2/3/4
;;   - bit 2 has priority over bit 0/1
;;   - each pattern group has a handshake register:
;;     SEQ_CHAINHNDSHK_G0
;;     ...
;;     SEQ_CHAINHNDSHK_G3
;;     bit 0..3 are set at the end of a track run depending on the chain mode,
;;     a chained tracks waits until it's flag is set
;;     a sequencer reset/restart will set the flags of the track which should start
;;     if no chaining is used, all flags are 1
;;     (complete handling is documented in the source code ;-)
;; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called by USER_Init to initialize the sequencer
;; --------------------------------------------------------------------------
SEQ_CORE_Init
	SET_BSR	SEQ_BASE

	;; restart pseudo random generator
	movlw	0x42
	movwf	SEQ_RANDOM_SEED_L, BANKED
	movwf	SEQ_RANDOM_SEED_H, BANKED

	;; select group 0 by default
	clrf	SEQ_SELECTED_GROUP, BANKED

	;; select track 1 by default
	movlw	0xf1
	movwf	SEQ_SELECTED_TRKS_G0, BANKED
	movlw	0xf0
	movwf	SEQ_SELECTED_TRKS_G1, BANKED
	movwf	SEQ_SELECTED_TRKS_G2, BANKED
	movwf	SEQ_SELECTED_TRKS_G3, BANKED

	;; select layer 1 by default
	movlw	0x01
	movwf	SEQ_SELECTED_LAYERS, BANKED
	movwf	SEQ_SELECTED_TRG_LAYERS, BANKED

	;; go into edit mode by default
	movlw	(1 << SEQ_MODE0_STEP_EDIT)
	movwf	SEQ_MODE0, BANKED

	;; init record mode
	movlw	(1 << SEQ_RECORD_MODE_REC_LAYER_A) | (1 << SEQ_RECORD_MODE_REC_LAYER_B) | (1 << SEQ_RECORD_MODE_REC_LAYER_C)
	movwf	SEQ_RECORD_MODE, BANKED

	;; initial transpose and arp hold keys
	movlw	0x3c		; C-3
	movwf	SEQ_T_NOTE_STACK_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_0_SORTED_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_0_UNSORTED_HOLD, BANKED
	movlw	0x40		; E-3
	movwf	SEQ_ARP_NOTE_1_SORTED_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_1_UNSORTED_HOLD, BANKED
	movlw	0x43		; G-3
	movwf	SEQ_ARP_NOTE_2_SORTED_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_2_UNSORTED_HOLD, BANKED
	movlw	0x48		; C-4
	movwf	SEQ_ARP_NOTE_3_SORTED_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_3_UNSORTED_HOLD, BANKED

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

	movf	SEQ_BANKSTICK_STATUS, W, BANKED
	andlw	0x01
	bz	SEQ_CORE_Init_NoBS
SEQ_CORE_Init_BS
	movlw	0x10
	movwf	SEQ_PATTERN1, BANKED
	movwf	SEQ_NEXT_PATTERN1, BANKED
	clrf	SEQ_PATTERN_BANK1, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK1, BANKED

	movlw	0x20
	movwf	SEQ_PATTERN2, BANKED
	movwf	SEQ_NEXT_PATTERN2, BANKED
	clrf	SEQ_PATTERN_BANK2, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK2, BANKED

	movlw	0x30
	movwf	SEQ_PATTERN3, BANKED
	movwf	SEQ_NEXT_PATTERN3, BANKED
	clrf	SEQ_PATTERN_BANK3, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK3, BANKED

	;; restore dump
	call	SEQ_DUMP_RestoreAll

	rgoto	SEQ_CORE_Init_BS_Cont

SEQ_CORE_Init_NoBS
	movlw	0x00
	movwf	SEQ_PATTERN1, BANKED
	movwf	SEQ_PATTERN2, BANKED
	movwf	SEQ_PATTERN3, BANKED

	clrf	SEQ_PATTERN_BANK1, BANKED
	clrf	SEQ_PATTERN_BANK2, BANKED
	clrf	SEQ_PATTERN_BANK3, BANKED

	clrf	SEQ_NEXT_PATTERN1, BANKED
	clrf	SEQ_NEXT_PATTERN2, BANKED
	clrf	SEQ_NEXT_PATTERN3, BANKED

	clrf	SEQ_NEXT_PATTERN_BANK1, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK2, BANKED
	clrf	SEQ_NEXT_PATTERN_BANK3, BANKED

	;; restore dump
	call	SEQ_DUMP_RestoreAll

	;; change channels and mutes
	movlw	0x04
	movwf	SEQ_EVNTT, BANKED
SEQ_CORE_Init_NoBS_Loop
	call	SEQ_FSR_TrkChn
	movff	SEQ_EVNTT, INDF1
	call	SEQ_FSR_TrkTrgA_0
	clrf	POSTINC1
	clrf	POSTINC1
	clrf	POSTINC1
	clrf	POSTINC1

	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, SEQ_CORE_Init_NoBS_Loop
SEQ_CORE_Init_BS_Cont

	;; update disable flags
	clrf	SEQ_GROUP, BANKED
SEQ_CORE_Init_UpdateTrkDisLoop
	call	SEQ_CORE_UpdateTrkDisable; update disable flags
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_CORE_Init_UpdateTrkDisLoop

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
	setf	SEQ_CLK_SUBSTEP_CTR, BANKED
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

	;; initialize handshake flags
	clrf	SEQ_GROUP, BANKED
SEQ_CORE_ResetHndshkLoop
	call	SEQ_CORE_ResetHndShk
	incf	SEQ_GROUP, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_GROUP, BANKED
	rgoto SEQ_CORE_ResetHndshkLoop

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


	;; update the EDIT/MUTE/PATTERN/SONG/RECORD flag depending on menu
	IRQ_DISABLE
	movlw	~((1 << SEQ_MODE0_STEP_EDIT) | (1 << SEQ_MODE0_MUTE) | (1 << SEQ_MODE0_PATTERN) | (1 << SEQ_MODE0_SONG))
	andwf	SEQ_MODE0, F, BANKED
	movlw	~((1 << SEQ_MODE1_RECORD))
	andwf	SEQ_MODE1, F, BANKED
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_CORE_Tick_NotWithinMenu

	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_MUTE
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_MUTE, BANKED

	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_PATTERN
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_PATTERN, BANKED

	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_SONG
	skpnz
	bsf	SEQ_MODE0, SEQ_MODE0_SONG, BANKED
	;; NOTE: the LED handler flashes the LED when in song menu and song state not active

	movf	CS_MENU_POS, W
	xorlw	CS_MENU_PAGE_RECORD
	skpnz
	bsf	SEQ_MODE1, SEQ_MODE1_RECORD, BANKED

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
	setf	SEQ_CLK_SUBSTEP_CTR, BANKED
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

	;; increment substep counter (0-3)
	incf	SEQ_CLK_SUBSTEP_CTR, F, BANKED
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_CORE_Clk_Reference_NoOv
SEQ_CORE_Clk_Reference_ResetS
	;; clear substep counter and increment step counter (0-255)
	clrf	SEQ_CLK_SUBSTEP_CTR, BANKED
	incf	SEQ_CLK_STEP_CTR, F, BANKED

	;; reset clock step counter once we reached max. steps per measure value
	movf	SEQ_STEPS_PER_MEASURE, W, BANKED
	cpfsgt	SEQ_CLK_STEP_CTR, BANKED
	rgoto	SEQ_CORE_Clk_Reference_NoOv
	clrf	SEQ_CLK_STEP_CTR, BANKED
SEQ_CORE_Clk_Reference_NoOv

	;; ------------------------------------------------------------------
	;; in manual trigger mode: request stop when last subtick of step reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_StopManual
	BRA_IFCLR SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_NoStopManual
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
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
	;; song mode: request next pattern if sequencer running and last position reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextPS
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextPS_End
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_NextPS_End

	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NextPS_End
	movlw	6-2	; (this value has to be adjusted - depends on the time which is necessary to load 4 patterns)
	cpfseq	SEQ_CLK_TICK6_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
	movlw	2
	cpfseq	SEQ_CLK_SUBSTEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
SEQ_CORE_Clk_NextPS_Request
	call	SEQ_SONG_NextPos
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
#if 0
	;; in song mode: wait until loop counter has been reseted
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_NextP_Synch_NoSong
SEQ_CORE_Clk_NextP_Synch_Song
	movf	SEQ_SONG_LOOP, W, BANKED
	xorwf	SEQ_SONG_LOOP_MAX, W, BANKED
	bnz	SEQ_CORE_Clk_NextP_End
SEQ_CORE_Clk_NextP_Synch_NoSong
#else
	;; TK: disabled - this improper change was introduced at a time the "max steps per measure" parameter
	;; was not available. There are better ways now to control pattern change after x steps!
#endif
	;; pattern change at last tick before next measure is reached
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
	xorlw	2
	bnz	SEQ_CORE_Clk_NextP_End
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NextP_End
SEQ_CORE_Clk_NextP_NoSynch
	rcall	SEQ_CORE_ChangePatternCheck
SEQ_CORE_Clk_NextP_End	

	;; ------------------------------------------------------------------
	;; send F8 on every 4th internal tick (96ppqn resolution) if echo allowed and not in slave mode
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NoF8
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
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
	movf	SEQ_CLK_SUBSTEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NoMetro

	;; play ON event on every 4th step
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	bz	SEQ_CORE_Clk_Metro_On

	;; play OFF event on every 4th+4 step (4th+1 with normal resolution)
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

	;; play special note on every measure start
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
	;; starting the big clock loop which handles every single track separately
	;; ------------------------------------------------------------------
	movff	SEQ_CHAINHNDSHK_G0, SEQ_CHAINHNDSHK_NEXT_G0	; prepare temporary handshake registers
	movff	SEQ_CHAINHNDSHK_G1, SEQ_CHAINHNDSHK_NEXT_G1	; the *CHAINHNDSHK_NEXT* registers will be copied
	movff	SEQ_CHAINHNDSHK_G2, SEQ_CHAINHNDSHK_NEXT_G2	; back to CHAINHNDSHK at the end of this
	movff	SEQ_CHAINHNDSHK_G3, SEQ_CHAINHNDSHK_NEXT_G3	; loop - this is to avoid inconsistencies

	clrf	SEQ_EVNTT, BANKED	; using SEQ_EVNTT as loop counter
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
	;; skip track if chaining handshake flag not set
	;; update HNDSHK_DIS flag here (only relevant for LED display)
	movlw	SEQ_TRKVARSTATE2x
	bsf	PLUSW2, SEQ_TRKVARSTATE2_HNDSHK_DIS
	call	SEQ_CORE_Hlp_ChkHndShk
	skpnz
	rgoto	SEQ_CORE_Clk_NextTrack
	movlw	SEQ_TRKVARSTATE2x
	bcf	PLUSW2, SEQ_TRKVARSTATE2_HNDSHK_DIS

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

	;; if reset has been requested: clear ARP key position counter
	movlw	SEQ_TRKVARSTATEx
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_Clk_Step_IncDec_NoR
SEQ_CORE_Clk_Step_IncDec_R
	movlw	SEQ_TRKVARARPPOSx
	clrf	PLUSW2
SEQ_CORE_Clk_Step_IncDec_NoR

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
	;; (this is only relevant for dynamic sync on pattern changes in song or "pattern synch" mode!)
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_Clk_Step_NoSyncMute
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

	;; skip track if pattern disabled
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_DISABLED, ACCESS, SEQ_CORE_Clk_Muted

	;; skip track if mode is OFF
	movlw	SEQ_TRKMODEx
	movf	PLUSW0, W
	andlw	0x07
	skpnz
	rgoto	SEQ_CORE_Clk_Muted

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

	;; start event loop (each track can play up to 4 MIDI events)
	clrf	SEQ_EVNT_NUMBER_CTR, BANKED
	bcf	SEQ_STATE, SEQ_STATE_SET_LEN, BANKED	; notifies that a new gate length has to be set
SEQ_CORE_Clk_OnEvent_Loop
	;; get event order and store in in SEQ_EVNT_NUMBER
	movf	SEQ_EVNT_NUMBER_CTR, W, BANKED
	call	SEQ_LAYER_GetEvntPrio
	movwf	SEQ_EVNT_NUMBER, BANKED

	;; don't play if return value is 15
	xorlw	15
	skpnz
	rgoto	SEQ_CORE_Clk_NoOnEvent

	;; don't play if retrigger flag set and retrigger counter (SEQ_TRKVARRETGx[4..0]) > 0
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_OnEvent_ReTrg
SEQ_CORE_Clk_OnEvent_NoReTrg
	movlw	SEQ_TRKVARRETRGx
	movf	PLUSW2, W
	andlw	0x1f
	skpz
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEvent_ReTrg

	;; get MIDI event depending on SEQ_EVNT_NUMBER
	call	SEQ_LAYER_GetEvnt
	;; events now in SEQ_EVNT[012L]

	;; retrigger calculations only have to be done once in a event number loop, so only do this on first event which is sent
	movf	SEQ_EVNT_NUMBER, W, BANKED
	bnz	SEQ_CORE_Clk_SendEvent_SkipReTrg

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

	;; morph it
	call	SEQ_MORPH_Event

	;; humanize it
	call	SEQ_HUMANIZE_Event

	;; groove it
	call	SEQ_GROOVE_Event

	;; transpose if not in arpeggiator mode
	movlw	SEQ_TRKMODEx
	movf	PLUSW0, W
	andlw	0x07
	xorlw	SEQ_TRKMODE_ARPEGGIATOR
	skpz
	rcall	SEQ_CORE_Transpose

	;; if event == 0, check if we have to stretch the gatelength
	movf	SEQ_EVNT_NUMBER, W, BANKED	; (only has to be done during first round)
	bnz	SEQ_CORE_Clk_NoStretch
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

	;; extra check for off event: on chords, it could happen that some
	;; notes don't get an off event under special circumstances, ensure this here!
SEQ_CORE_Clk_SendEvent_NoteChkOE
	movf	SEQ_EVNT_NUMBER, W, BANKED	; if first note is played, and another has no new note value (transition from 4finger to 3finger chord or similar) - play off here
	bz	SEQ_CORE_Clk_SendEvent_NoteChkEE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SET_LEN, BANKED, SEQ_CORE_Clk_SendEvent_NoteChkEE
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
SEQ_CORE_Clk_SendEvent_NoteChkEE
	rgoto	SEQ_CORE_Clk_NoOnEvent

SEQ_CORE_Clk_SendEvent_CCChk
	;; don't play if CC number == 0x00
	movf	SEQ_EVNT1, W, BANKED
	skpnz
	rgoto	SEQ_CORE_Clk_NoOnEvent
	;; 	rgoto	SEQ_CORE_Clk_SendEvent_Chk_End

SEQ_CORE_Clk_SendEvent_Chk_End

	;; check for transpose or arpeggiator function
	movlw	SEQ_TRKMODEx
	movf	PLUSW0, W
	andlw	0x07
	xorlw	SEQ_TRKMODE_TRANSPOSE
	bz	SEQ_CORE_Clk_SendEvent_Trn	; transpose mode
	xorlw	SEQ_TRKMODE_ARPEGGIATOR ^ SEQ_TRKMODE_TRANSPOSE
	bz	SEQ_CORE_Clk_SendEvent_Arp	; arpeggiator mode
	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply	; normal mode

	;; ------------------------------------------------------------------
SEQ_CORE_Clk_SendEvent_Trn
	movlw	SEQ_TRKMODEx
	BRA_IFSET PLUSW0, SEQ_TRKMODE_FLAG_HOLD, ACCESS, SEQ_CORE_Clk_SendEvent_TrnH
	;; select last played note
	movf	SEQ_T_NOTE_STACK_0, W, BANKED
	rgoto	SEQ_CORE_Clk_SendEvent_Trn_C
SEQ_CORE_Clk_SendEvent_TrnH
	;; select last played note (hold mode)
	movf	SEQ_T_NOTE_STACK_HOLD, W, BANKED
SEQ_CORE_Clk_SendEvent_Trn_C
	movwf	PRODL		; temporary store note number in PRODL

	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bz	SEQ_CORE_Clk_SendEvent_Trn_N

	;; Transpose CC
SEQ_CORE_Clk_SendEvent_Trn_CC
	;; subtract base note (C-3)
	movf	PRODL, W	; (note number was stored in PRODL)
	addlw	-0x3c
	
	;; add WREG to CC value and saturate
	BRA_IFSET WREG, 7, ACCESS, SEQ_CORE_Clk_SendEvent_Trn_CCNeg
SEQ_CORE_Clk_SendEvent_Trn_CCPos
	addwf	SEQ_EVNT2, W, BANKED
	btfsc	WREG, 7
	movlw 0x7f
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply

SEQ_CORE_Clk_SendEvent_Trn_CCNeg
	addwf	SEQ_EVNT2, W, BANKED
	btfsc	WREG, 7
	movlw 0x00
	movwf	SEQ_EVNT2, BANKED
	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply


	;; Transpose Notes/Aftertouch/etc.
SEQ_CORE_Clk_SendEvent_Trn_N
	;; do nothing if note is zero
	movf	PRODL, W	; (note number was stored in PRODL)
	skpnz
	rgoto	SEQ_CORE_Clk_NoOnEvent

	;; subtract base note (C-3)
	addlw	-0x3c

	;; add WREG to note number and exit this loop on overrun or zero (so that it won't be played)
	addwf	SEQ_EVNT1, W, BANKED
	BRA_IFSET WREG, 7, ACCESS, SEQ_CORE_Clk_NoOnEvent
	bz	SEQ_CORE_Clk_NoOnEvent
	movwf	SEQ_EVNT1, BANKED
	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply
	
	;; ------------------------------------------------------------------
	
SEQ_CORE_Clk_SendEvent_Arp
	movlw	SEQ_TRKMODEx
	BRA_IFSET PLUSW0, SEQ_TRKMODE_FLAG_HOLD, ACCESS, SEQ_CORE_Clk_SendEvent_ArpH
	lfsr	FSR1, SEQ_ARP_NOTE_0_SORTED
	btfsc	PLUSW0, SEQ_TRKMODE_FLAG_UNSORTED
	lfsr	FSR1, SEQ_ARP_NOTE_0_UNSORTED
	rgoto	SEQ_CORE_Clk_SendEvent_Arp_C
SEQ_CORE_Clk_SendEvent_ArpH
	lfsr	FSR1, SEQ_ARP_NOTE_0_SORTED_HOLD
	btfsc	PLUSW0, SEQ_TRKMODE_FLAG_UNSORTED
	lfsr	FSR1, SEQ_ARP_NOTE_0_UNSORTED_HOLD
SEQ_CORE_Clk_SendEvent_Arp_C

	;; arpeggiator cannot work with CC's
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bnz	SEQ_CORE_Clk_SendEvent_NM_Ply

	;; get the key from the played chor which should be sent to the arpeggiator
	rcall	SEQ_CORE_Hlp_GetArpKey
	bnz	SEQ_CORE_Clk_NM45_C
	;; no arp key hold: play off event (if it exists)
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_NM45_C

	;; save it temporary in TABLAT
	movwf	TABLAT

	;; transpose selected note octavewise (SEQ_EVNT[6..4])
	;; if multi-arp event is selected, get octave from SEQ_EVNT1[4:2]
	swapf	SEQ_EVNT1, W, BANKED
	andlw	0x0f
	addlw	-2
	bnc	SEQ_CORE_Clk_NM45_C_Multi
SEQ_CORE_Clk_NM45_C_NoMulti
	swapf	SEQ_EVNT1, W, BANKED
	rgoto	SEQ_CORE_Clk_NM45_C_Multi_C
SEQ_CORE_Clk_NM45_C_Multi
	rrf	SEQ_EVNT1, W, BANKED
	rrf	WREG, W
SEQ_CORE_Clk_NM45_C_Multi_C

	addlw	-4
	andlw	0x07
	btfsc	WREG, 2
	iorlw 0xf8
	mullw	12		; * octave
	movf	PRODL, W
	addwf	TABLAT, W	; add to played note
	BRA_IFSET WREG, 7, ACCESS, SEQ_CORE_Clk_NoOnEvent; don't play on transp. overflow
	movwf	SEQ_EVNT1, BANKED

	;; transpose again with track settings (octave/semitones)
	rcall	SEQ_CORE_Transpose
	;; don't play on over/underrun
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_CORE_Clk_NoOnEvent

	;; 	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply

	;; ------------------------------------------------------------------

SEQ_CORE_Clk_SendEvent_NM_Ply
	;; skip this if no note event
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bnz	SEQ_CORE_Clk_SendEvent_NoNEvnt

	;; check for "force to scale"
	call	SEQ_SCALE_Note

	;; don't check if no note in queue (evnt0 == 0)
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr	; first byte equal?
	movf	INDF1, W
	bz	SEQ_CORE_Clk_SendEvent_NoLeg

	;; if current note equal to new note, send off event AFTER new note
	xorwf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_CORE_Clk_SendEvent_NoOff

	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr	; second byte equal?
	movf	INDF1, W
	xorwf	SEQ_EVNT1, W, BANKED
	bnz	SEQ_CORE_Clk_SendEvent_NoOff

	rcall	SEQ_CORE_Hlp_PlayOffEvnt	; play off event
	rgoto	SEQ_CORE_Clk_SendEvent_NoLeg

SEQ_CORE_Clk_SendEvent_NoOff
	;; current note != old note: play off event later for a proper legato
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_LEGATO
SEQ_CORE_Clk_SendEvent_NoLeg

SEQ_CORE_Clk_SendEvent_NoNEvnt
	;; ------------------------------------------------------------------
	;; play the new MIDI event
	movlw	SEQ_TRKCHNx
	swapf	PLUSW0, W
	andlw	0x07
	movwf	MIDI_EVNT_PORT
	movff	SEQ_EVNT0, MIDI_EVNT0
	movff	SEQ_EVNT1, MIDI_EVNT1
 	movff	SEQ_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send

	;; update the meter (for mute menu)
	rcall	SEQ_CORE_UpdateMeter

	SET_BSR	SEQ_BASE			; fix BSR

	;; if legato flag set: play off event
	movlw	SEQ_TRKVARSTATEx
	RCALL_IFSET PLUSW2, SEQ_TRKVARSTATE_LEGATO, ACCESS, SEQ_CORE_Hlp_PlayOffEvnt

	;; store port, off event and length in queue
	;; exception: CC and length == 0x1f
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
SEQ_CORE_Clk_SendEvent_CCNoOff

	;; notify that a new gate length has to be set
	bsf	SEQ_STATE, SEQ_STATE_SET_LEN, BANKED

	;; ---
SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEventPostponed
SEQ_CORE_Clk_OnEventPlayed

	;; loop 4 times to play all 4 MIDI events of a track
	incf	SEQ_EVNT_NUMBER_CTR, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER_CTR, BANKED
	rgoto SEQ_CORE_Clk_OnEvent_Loop

	;; we are outside the OnEvent loop now

	;; ------------------------------------------------------------------
	;; stretch note until next note if sustain flag enabled
	movlw	SEQ_TRKMODEx
	BRA_IFCLR PLUSW0, SEQ_TRKMODE_FLAG_SUSTAIN, ACCESS, SEQ_CORE_Clk_NoSustain
SEQ_CORE_Clk_Sustain
	;; set gatelength to maximum
	movlw	SEQ_TRKVARQUEUEL_Lx
	setf	PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	setf	PLUSW2

	rgoto	SEQ_CORE_Clk_NoStretchReq	; (skip stretch check below)
SEQ_CORE_Clk_NoSustain
	
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
	;; quick fix: 
	;; reset "sync to measure" after first step has been processed
	;; this is required for handshaked patterns in song mode...
	;; temporary solution until a more elegant way has been found!
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NextTrack_NoSTMRes
	movf	SEQ_CLK_TICK6_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NextTrack_NoSTMRes
SEQ_CORE_Clk_NextTrack_STMRes
	movlw	SEQ_TRKVARSTATE2x
	bcf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
SEQ_CORE_Clk_NextTrack_NoSTMRes

	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_Clk_Loop

	;; transfer "next values" into handshake registers
	movff	SEQ_CHAINHNDSHK_NEXT_G0, SEQ_CHAINHNDSHK_G0
	movff	SEQ_CHAINHNDSHK_NEXT_G1, SEQ_CHAINHNDSHK_G1
	movff	SEQ_CHAINHNDSHK_NEXT_G2, SEQ_CHAINHNDSHK_G2
	movff	SEQ_CHAINHNDSHK_NEXT_G3, SEQ_CHAINHNDSHK_G3

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
	andlw	0x0f		; problem: how to handle max. steps per measure here???
	movwf	SEQ_CLK_STEP_CTR, BANKED
	decf	SEQ_CLK_STEP_CTR, F, BANKED
	setf	SEQ_CLK_SUBSTEP_CTR, BANKED
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
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
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
;;      group in SEQ_GROUP
;;  OUT: next pattern requests in SEQ_NEXT_PATTERNx/SEQ_NEXT_PATTERN_BANKx when patterns not equal
;;  USES: TMP1
;; --------------------------------------------------------------------------
	;; note: in song mode, we are using SEQ_CORE_ChangePattern_Song instead
	;; to skip the measure and pattern/bank check
	;; -> SEQ_CORE_ChangePattern_Song
SEQ_CORE_ChangePatternSynched
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED, SEQ_CORE_ChangePatternSynchedSkp
	bsf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED
SEQ_CORE_ChangePatternSynchedSkp

	;; check if SEQ_PATTERN_BANK != SEQ_NEXT_PATTERN_BANKx
	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	cpfseq	SEQ_PATTERN_BANK, BANKED
	rgoto SEQ_CORE_ChangePattern_Chng

	;; check if SEQ_PATTERN != SEQ_NEXT_PATTERNx
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	cpfseq	SEQ_PATTERN, BANKED
	rgoto SEQ_CORE_ChangePattern_Chng

	;; no change
	rgoto	SEQ_CORE_ChangePattern_End

SEQ_CORE_ChangePattern_Song
SEQ_CORE_ChangePattern_Chng
	;; copy SEQ_PATTERN -> SEQ_NEXT_PATTERNx
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movff	SEQ_PATTERN, PLUSW1

	;; copy SEQ_PATTERN_BANK -> SEQ_NEXT_PATTERN_BANKx
	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movff	SEQ_PATTERN_BANK, PLUSW1

	;; change immediately if sequencer not running
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_ChangePattern_Sync
SEQ_CORE_ChangePattern_NoSync
	;; set request flag
	movf	SEQ_GROUP, W, BANKED
	call	MIOS_HLP_GetBitORMask
 	iorwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, F, BANKED
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
;;      group in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePattern
	;; clear request flag
	SET_BSR	SEQ_BASE
	movf	SEQ_GROUP, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, F, BANKED

	;; transfer SEQ_NEXT_PATTERN_BANKx to SEQ_PATTERN_BANKx
	lfsr	FSR0, SEQ_NEXT_PATTERN_BANK0
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movff	PLUSW0, PRODL
	movff	PRODL, PLUSW1

	;; transfer SEQ_NEXT_PATTERN*x to SEQ_PATTERN_*x
	lfsr	FSR0, SEQ_NEXT_PATTERN0
	lfsr	FSR1, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movff	PLUSW0, PRODL
	movff	PRODL, PLUSW1

	;; restore group
	call	SEQ_DUMP_RestoreTrk
	call	SEQ_DUMP_RestorePots

	;; initialise handshake flags
	rcall	SEQ_CORE_ResetHndShk

	;; update disable flags and exit
	rgoto	SEQ_CORE_UpdateTrkDisable


;; --------------------------------------------------------------------------
;;  This function is used in SEQ_CORE_Tick to check, if the sequencer has
;;  to switch to new patterns
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePatternCheck
	SET_BSR	SEQ_BASE
	clrf	SEQ_GROUP, BANKED
SEQ_CORE_ChangePatternCheckLoop
	;; check if pattern has already been updated (SEQ_PATTERN_SYNC_TRANSFER_REQ[g] cleared)
	movf	SEQ_GROUP, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, W, BANKED
	bz	SEQ_CORE_ChangePatternCheckLoopN

	rcall	SEQ_CORE_ChangePattern

SEQ_CORE_ChangePatternCheckLoopN
	SET_BSR	SEQ_BASE
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_CORE_ChangePatternCheckLoop

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
;;  IN: group which should be updated in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_CORE_UpdateTrkDisable
	SET_BSR	SEQ_BASE

	;; pointer to SEQ_TRKVARx -> FSR2
	lfsr	FSR2, SEQ_TRKVAR0
	movf	SEQ_GROUP, W, BANKED
	mullw	4*SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F

	;; temporary disable interrupts (since the interrupt handlers might also check the DISABLE flag)
	IRQ_DISABLE

	;; clear the disable flags
	movlw	SEQ_TRKVARSTATEx + 0*SEQ_TRKVARRECORD_LENGTH
	bcf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 1*SEQ_TRKVARRECORD_LENGTH
	bcf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 2*SEQ_TRKVARRECORD_LENGTH
	bcf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 3*SEQ_TRKVARRECORD_LENGTH
	bcf	PLUSW2, SEQ_TRKVARSTATE_DISABLED

	;; set the synch to measure flag (in song or "pattern synch" mode only)
	;; TK: now used in all modes - it's important when switching between patterns with different timebase or progression parameters
#if 0
	BRA_IFSET SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED, SEQ_CORE_UpdateTrkDisable_Sync
	BRA_IFCLR SEQ_STATE, SEQ_STATE_SONG, BANKED, SEQ_CORE_UpdateTrkDisable_NoSync
#endif
SEQ_CORE_UpdateTrkDisable_Sync
	movlw	SEQ_TRKVARSTATE2x + 0*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
	movlw	SEQ_TRKVARSTATE2x + 1*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
	movlw	SEQ_TRKVARSTATE2x + 2*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
	movlw	SEQ_TRKVARSTATE2x + 3*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE2_SYNC_MEASURE
SEQ_CORE_UpdateTrkDisable_NoSync

	;; set the flags when whole pattern has been disabled (SEQ_PATTERNx[7] set)
	lfsr	FSR1, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	BRA_IFSET PLUSW1, 7, ACCESS, SEQ_CORE_UpdateTrkDisable_Ok

	;; set the flags if bankstick not available, bank != 0 and pattern number != 0
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_CORE_UpdateTrkDisable_Not

	;; bankstick not available - disable if bank != 0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	bnz	SEQ_CORE_UpdateTrkDisable_Ok

	;; or disable if bank == 0 and pattern != 0
	lfsr	FSR1, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	bz	SEQ_CORE_UpdateTrkDisable_Not
	;; 	rgoto	SEQ_CORE_UpdateTrkDisable_Match

SEQ_CORE_UpdateTrkDisable_Ok
	;; set the disable flags
	;; (using 4 flags for the same purpose - we could extent the functionality later)
	movlw	SEQ_TRKVARSTATEx + 0*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 1*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 2*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE_DISABLED
	movlw	SEQ_TRKVARSTATEx + 3*SEQ_TRKVARRECORD_LENGTH
	bsf	PLUSW2, SEQ_TRKVARSTATE_DISABLED

SEQ_CORE_UpdateTrkDisable_Not
	;; interrupts can be enabled again
	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE0
;;  IN: SEQ_EVNTT and SEQ_EVNT_NUMBER
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE0_BEGIN

SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C
	movf	SEQ_EVNTT, W, BANKED
	mullw	4
	movf	PRODL, W
	addwf	SEQ_EVNT_NUMBER, W, BANKED
	addwf	FSR1L, F
	return

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE1
;;  IN: SEQ_EVNTT and SEQ_EVNT_NUMBER
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE1_BEGIN
	rgoto	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUEP
;;  IN: SEQ_EVNTT and SEQ_EVNT_NUMBER
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUEP_BEGIN
	rgoto	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C

;; --------------------------------------------------------------------------
;;  sends the content of SEQ_TRKQUEUExx via MIDI and clears the queue
;;  IN: SEQ_EVNTT, SEQ_EVNT_NUMBER, pointer to SEQ_TRKVARx in FSR2
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
	clrf	SEQ_EVNT_NUMBER, BANKED
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNT_NUMBER, F, BANKED
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNT_NUMBER, F, BANKED
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNT_NUMBER, F, BANKED
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
	clrf	SEQ_EVNT_NUMBER, BANKED
SEQ_CORE_Hlp_PlayAllOffEvntsIL
	clrwdt
	rcall	SEQ_CORE_Hlp_PlayOffEvnt; play off event of selected track

	incf	SEQ_EVNT_NUMBER, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER, BANKED
	rgoto SEQ_CORE_Hlp_PlayAllOffEvntsIL

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
	return

;; --------------------------------------------------------------------------
;;  Determine next step depending on direction mode
;;  IN: pointer to SEQ_TRKx in FSR0, pointer to SEQ_TRKVARx in FSR2
;;  OUT: SEQ_TRKVARSTEPx:  next step
;;       SEQ_TRKVARDIVCTRx: divider counter always cleared
;;       SEQ_TRKVARSTATEx: PINGPONG and PLAY_STEP modified
;; --------------------------------------------------------------------------
SEQ_CORE_NextStep
	;; define the bits of TMP3
#define SEQ_CORE_NEXTSTEP_TMP3_PINGPONG 0
#define SEQ_CORE_NEXTSTEP_TMP3_PENDULUM 1
#define SEQ_CORE_NEXTSTEP_TMP3_SAVESTEP 2

	;; clear TMP3, which holds some temporary flags (see definitions above)
	clrf	TMP3

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
	;; store current direction
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; jump to first step if reset has been requested
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_FwdR

	;; jump to first (loop) step if last step has been reached
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, TMP1
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_NextStep_FwdR
	movwf	TMP2		; store last step in TMP2 for PingPong comparison

	;; otherwise increment step
	movlw	SEQ_TRKVARSTEPx
	incf	PLUSW2, F

	;; in pingpong mode: turn direction if last step has been reached after this increment
	;; if chaining enabled: toggle handshake flags
	movf	PLUSW2, W	; current step
	xorwf	TMP2, W		; last step
	bnz	SEQ_CORE_NextStep_Fwd_NoPPT

	;; handshake not in ping pong or pendulum mode (done in backward direction)
	BRA_IFSET TMP3, SEQ_CORE_NEXTSTEP_TMP3_PINGPONG, ACCESS, SEQ_CORE_NextStep_Fwd_NoHndShk
	BRA_IFSET TMP3, SEQ_CORE_NEXTSTEP_TMP3_PENDULUM, ACCESS, SEQ_CORE_NextStep_Fwd_NoHndShk
	rcall	SEQ_CORE_SetHndShk	; handshake handler
SEQ_CORE_NextStep_Fwd_NoHndShk

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
	movlw	SEQ_TRKLOOPx
	movf	PLUSW0, W
	andlw	0x1f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW2
	rgoto	SEQ_CORE_NextStep_Cont


	;; ---[ Backward ]---------------------------------------------------
SEQ_CORE_NextStep_Bck
	;; store current direction
	movlw	SEQ_TRKVARSTATEx
	bsf	PLUSW2, SEQ_TRKVARSTATE_BACKWARD

	;; jump to last step if reset has been requested
	movlw	SEQ_TRKVARSTATEx
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE_POS_RESET, ACCESS, SEQ_CORE_NextStep_BckR

	;; jump to last step if first loop step has been reached
	movlw	SEQ_TRKLOOPx
	movf	PLUSW0, W
	andlw	0x1f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW2, W
	xorwf	PRODL, W
	bz	SEQ_CORE_NextStep_BckR

	;; otherwise decrement step
	movlw	SEQ_TRKVARSTEPx
	decf	PLUSW2, F

	;; in pingpong mode: turn direction if first loop step has been reached after this decrement
	;; if chaining enabled: toggle handshake flags
	movlw	SEQ_TRKVARSTEPx
	movf	PLUSW2, W
	xorwf	PRODL, W
	bnz	SEQ_CORE_NextStep_Bck_NoPPT
	rcall	SEQ_CORE_SetHndShk	; handshake handler
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
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
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
	movlw	SEQ_TRKLOOPx
	movf	PLUSW0, W
	andlw	0x1f
	movwf	TMP1
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
	movwf	TMP2

	;; no special measure of TMP1 == TMP2
	movwf	PRODL
	xorwf	TMP1, W
	bz	SEQ_CORE_NextStep_RS_Found

	;; range = TMP2-TMP1+1 -> TMP3
	movf	TMP1, W
	subwf	TMP2, W
	addlw	1
	movwf	TMP3

	;; generate new random number, and scale over range
	rcall	SEQ_CORE_GenRandomNumber
	movf	SEQ_RANDOM_SEED_H, W, BANKED
	mulwf	TMP3, ACCESS

	;; new step now in PRODH, add offset (loop point)
	movf	PRODH, W
	addwf	TMP1, W
	movwf	PRODL

	;; store in TRKVARSTEP
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
	bcf	PLUSW2, SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP
	clrf	SEQ_RECORD_STEP, BANKED
	clrf	SEQ_RECORD_CURRENT_EVNT1, BANKED
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
	;; it's located in SEQ_TRKLENx[4..0] (track length)
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
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
;;  Resets the handshake flags of the selected track group
;;  IN: group (0-3) in SEQ_GROUP
;; --------------------------------------------------------------------------
SEQ_CORE_ResetHndShk
	SET_BSR	SEQ_BASE

	;; determine SEQ_EVNTT of first track of SEQ_GROUP
	rlf	SEQ_GROUP, W, BANKED
	rlf	WREG, W
	andlw	0x0c
	movwf	SEQ_EVNTT, BANKED

	;; chain configuration -> PRODL
	call	SEQ_FSR_TrkChainx
	movff	INDF1, PRODL

	;; pointer to register in FSR1
	lfsr	FSR1, SEQ_CHAINHNDSHK_G0
	movf	SEQ_GROUP, W, BANKED
	addwf	FSR1L, F

	movlw	0x01		; init SEQ_CHAINHNDSHK_Px (always start with track 1)
	movwf	INDF1

	movf	PRODL, W	; start track 2 when 12 and 1234 chaining not active
	andlw	(1 << SEQ_TRKCHAIN_12) | (1 << SEQ_TRKCHAIN_1234)
	skpnz
	bsf	INDF1, 1

	;; branch to the end if 1234 flag is active
	andlw	(1 << SEQ_TRKCHAIN_1234)
	bnz	SEQ_CORE_ResetHndShk_Cont

	;; now check settings of track 3
	movf	PRODL, W	; start track 3 if 1234 chaining not active
	andlw	(1 << SEQ_TRKCHAIN_1234)
	bnz	SEQ_CORE_ResetHndShk_Cont
	bsf	INDF1, 2

	movf	PRODL, W	; start track 4 when 34 and 1234 chaining not active
	andlw	(1 << SEQ_TRKCHAIN_34) | (1 << SEQ_TRKCHAIN_1234)
	skpnz
	bsf	INDF1, 3

SEQ_CORE_ResetHndShk_Cont
	;; reset positions of all tracks which are currently not played
SEQ_CORE_ResetHndShk_TrkLoop
	rcall	SEQ_CORE_Calc_TRKx_FSR0		; calculate pointer to SEQ_TRKx -> FSR0
	rcall	SEQ_CORE_Calc_TRKVARx_FSR2	; calculate pointer to SEQ_TRKVARx -> FSR2

	;; clear "HNDSHK_DIS" flag, which is set again if track is not played due to handshaking
	;; (only used for LED display)
	movlw	SEQ_TRKVARSTATE2x
	bcf	PLUSW2, SEQ_TRKVARSTATE2_HNDSHK_DIS

	lfsr	FSR1, SEQ_CHAINHNDSHK_G0
	movf	SEQ_GROUP, W, BANKED
	addwf	FSR1L, F
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	bnz	SEQ_CORE_ResetHndShk_TrkLoopNext

	;; set "HNDSHK_DIS" flag (only used for LED display)
	movlw	SEQ_TRKVARSTATE2x
	bsf	PLUSW2, SEQ_TRKVARSTATE2_HNDSHK_DIS

	;; clear position counters
 	rcall	SEQ_CORE_ResetTrkPos

	;; now we need to modify the steps, so that the last one is selected (and not the first one!)

	;; branch depending on forward/backward direction
	movlw	SEQ_TRKDIR1x
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	SEQ_TRKDIR_BACKWARD
	bnz	SEQ_CORE_ResetHndShk_TrkLoop_F
SEQ_CORE_ResetHndShk_TrkLoop_B
	;; select last position (step 1)
	movlw	SEQ_TRKVARSTEPx
	clrf	PLUSW2
	rgoto	SEQ_CORE_ResetHndShk_TrkLoop_B_C

SEQ_CORE_ResetHndShk_TrkLoop_F
	;; Forward/PingPong/Random direction
	;; determine track position and copy it to SEQ_TRKVARSTEPx
	;; it's located in SEQ_TRKLENx[4..0] (track length)
	movlw	SEQ_TRKLENx
	movf	PLUSW0, W
	andlw	0x1f
	movwf	PRODL
	movlw	SEQ_TRKVARSTEPx
	movff	PRODL, PLUSW2

SEQ_CORE_ResetHndShk_TrkLoop_B_C
	;; save position (for repeat function)
	movlw	SEQ_TRKVARSTEPx
	movff	PLUSW2, PRODL
	movlw	SEQ_TRKVARSTEPSAVEDx
	movff	PRODL, PLUSW2

	;; not the first clock anymore...
	movlw	SEQ_TRKVARSTATEx
	bcf	PLUSW2, SEQ_TRKVARSTATE_FIRST_CLK	

SEQ_CORE_ResetHndShk_TrkLoopNext
	incf	SEQ_EVNTT, F, BANKED
	movf	SEQ_EVNTT, W, BANKED	; 4 tracks
	andlw	0x03
	bnz	SEQ_CORE_ResetHndShk_TrkLoop
	return


;; --------------------------------------------------------------------------
;;  Used by SEQ_CORE_NextStep to toggle the handshake flags
;;  IN: track in SEQ_EVNTT, pointer to SEQ_TRKx in FSR0, pointer to SEQ_TRKVARx in FSR2
;;  OUT: new handshake flags in SEQ_CHAINHNDSHK_NEXT_Px
;;  USES: FSR1 as temporary pointer
;; --------------------------------------------------------------------------
SEQ_CORE_SetHndShk
	;; chain configuration -> PRODL
	call	SEQ_FSR_TrkChainx
	movff	INDF1, PRODL

	;; if track 2 or 3: copy PRODL[1] to PRODL[0] for easier handling
	BRA_IFCLR SEQ_EVNTT, 1, BANKED, SEQ_CORE_SetHndShk_NotT23
SEQ_CORE_SetHndShk_T23
	bcf	PRODL, 0
	btfsc	PRODL, 1
	bsf	PRODL, 0
SEQ_CORE_SetHndShk_NotT23
	
	;; set handshake flags depending on chaining mode
	movf	PRODL, W
	andlw	(1 << SEQ_TRKCHAIN_12) | (1 << SEQ_TRKCHAIN_1234)
	bz	SEQ_CORE_SetHndShk_End

	lfsr	FSR1, SEQ_CHAINHNDSHK_NEXT_G0; pointer to SEQ_CHAINHNDSHK_NEXT_Px in FSR1
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addwf	FSR1L, F

	BRA_IFSET PRODL, SEQ_TRKCHAIN_1234, ACCESS, SEQ_CORE_SetHndShk2
SEQ_CORE_SetHndShk1	; toggle between 1/2 and 3/4
	BRA_IFSET SEQ_EVNTT, 1, BANKED, SEQ_CORE_SetHndShk1_23
SEQ_CORE_SetHndShk1_01
	BRA_IFSET SEQ_EVNTT, 0, BANKED, SEQ_CORE_SetHndShk1_1
SEQ_CORE_SetHndShk1_0
	bcf	INDF1, 0
	bsf	INDF1, 1
	rgoto	SEQ_CORE_SetHndShk_End
SEQ_CORE_SetHndShk1_1
	bsf	INDF1, 0
	bcf	INDF1, 1
	rgoto	SEQ_CORE_SetHndShk_End

SEQ_CORE_SetHndShk1_23
	BRA_IFSET SEQ_EVNTT, 0, BANKED, SEQ_CORE_SetHndShk1_3
SEQ_CORE_SetHndShk1_2
	bcf	INDF1, 2
	bsf	INDF1, 3
	rgoto	SEQ_CORE_SetHndShk_End
SEQ_CORE_SetHndShk1_3
	bsf	INDF1, 2
	bcf	INDF1, 3
	rgoto	SEQ_CORE_SetHndShk_End
	
SEQ_CORE_SetHndShk2	; toggle between 1/2/3/4
	incf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	movwf	INDF1
	;; 	rgoto	SEQ_CORE_SetHndShk_End

SEQ_CORE_SetHndShk_End
	return

;; --------------------------------------------------------------------------
;;  This function returns 0 if the track chaining handshake flag is not set
;;  IN: track number in SEQ_EVNTT
;;  OUT: WREG == 0 if flag not set
;;  USES: FSR1 as temporary pointer
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_ChkHndShk
	lfsr	FSR1, SEQ_CHAINHNDSHK_G0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addwf	FSR1L, F
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
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
;;  this function transposes SEQ_EVNT1 if it is no controller, no program change,
;;  no pitch bender event
;;  IN: SEQ_EVNT0: status byte
;;      SEQ_EVNT1: note value
;;  OUT: transposed MIDI event in SEQ_EVNT1
;;       on overrun, SEQ_EVNT0 is forced to zero
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Transpose
	;; transpose only note events
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bnz	SEQ_CORE_Transpose_End

SEQ_CORE_Transpose_Oct
	;; add octave transpose value * 12 and note transpose value to SEQ_EVNT1
	;; force SEQ_EVNT0 to zero on overrun so that the event won't be played
	call	SEQ_FSR_TrkTransp	; (upper nibble)
	swapf	INDF1, W
	andlw	0x0f
	xorlw	-8 & 0x0f	; no octave transpose when -8
	bz	SEQ_CORE_Transpose_NoOct

	swapf	INDF1, W
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	mullw	12		; * octave
	movf	PRODL, W
	addwf	SEQ_EVNT1, F, BANKED
	btfsc	SEQ_EVNT1, 7, BANKED
	clrf	SEQ_EVNT0, BANKED
SEQ_CORE_Transpose_NoOct

SEQ_CORE_Transpose_Sem
	call	SEQ_FSR_TrkTransp	; (lower nibble)
	movf	INDF1, W
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	addwf	SEQ_EVNT1, F, BANKED
	btfsc	SEQ_EVNT1, 7, BANKED
	clrf	SEQ_EVNT0, BANKED
SEQ_CORE_Transpose_NoSem

SEQ_CORE_Transpose_End
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

	;; not relevant for CC events
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_CORE_RecStep_End

	;; check if current track is selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_CORE_RecStep_End

	;; check if event active
	movlw	SEQ_TRKVARSTATE2x
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE, ACCESS, SEQ_CORE_RecStep_NoGate
SEQ_CORE_RecStep_Gate
	movff	SEQ_EVNTS, SEQ_EVNT0
	movff	SEQ_CORE_TRK_LASTPOS, SEQ_EVNTS
	movlw	0x1f
	movwf	SEQ_EVNTL, BANKED
	call	SEQ_LAYER_SaveEvntL
	movff	SEQ_EVNT0, SEQ_EVNTS

	movlw	0
	call	SEQ_TRG_GateSet
	movlw	0x11
	movwf	SEQ_EVNTL, BANKED
	call	SEQ_LAYER_SaveEvntL
SEQ_CORE_RecStep_NoGate

	movlw	SEQ_TRKVARSTATE2x
	BRA_IFCLR PLUSW2, SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP, ACCESS, SEQ_CORE_RecStep_NoChkG
SEQ_CORE_RecStep_ChkG
	bcf	PLUSW2, SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP

	movlw	SEQ_TRKVARSTATE2x
	BRA_IFSET PLUSW2, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE, ACCESS, SEQ_CORE_RecStep_ChkG_NL
SEQ_CORE_RecStep_ChkG_L
	movff	SEQ_EVNTS, SEQ_EVNT0
	movff	SEQ_CORE_TRK_LASTPOS, SEQ_EVNTS
	movff	SEQ_RECORD_LENGTH_CTR, SEQ_EVNTL
	call	SEQ_LAYER_SaveEvntL
	movff	SEQ_EVNT0, SEQ_EVNTS
SEQ_CORE_RecStep_ChkG_NL

	call	SEQ_TRG_GateGet
	bnz	SEQ_CORE_RecStep_NoChkG
	movlw	0x4
	movwf	SEQ_EVNTL, BANKED
	call	SEQ_LAYER_SaveEvntL
SEQ_CORE_RecStep_NoChkG
SEQ_CORE_RecStep_End
	return


;; --------------------------------------------------------------------------
;;  This function returns the key from the played chord which should be sent
;;  to the arpeggiator
;;  IN: pointer to SEQ_TRKx in FSR0, pointer to chord stack in FSR1, pointer to SEQ_TRKVARx in FSR2
;;  OUT: note which should be sent to the arpeggiator in WREG
;;       0 if no note should be played
;;  USES: TABLAT as temporary register
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetArpKey
	;; check for multi ARP event, flag stored in PRODL[0]
	bcf	PRODL, 0
	swapf	SEQ_EVNT1, W, BANKED
	andlw	0x0f
	addlw	-2
	skpc
	bsf	PRODL, 0

	;; save selected note SEQ_EVNT1[3..2] into TABLAT
	rrf	SEQ_EVNT1, W, BANKED
	rrf	WREG, W
	andlw	0x03
	movwf	TABLAT

	;; 1 note is played, return
	;;    1: first note  (#0)
	;;    2: first note  (#0)
	;;    3: first note  (#0)
	;;    4: first note  (#0)

	;; 2 notes are played, return
	;;    1: first note  (#0)
	;;    2: second note (#1)
	;;    3: first note  (#0)
	;;    4: second note (#1)

	;; 3 notes are played, return
	;;    1: first note  (#0)
	;;    2: second note (#1)
	;;    3: third note  (#2)
	;;    4: first note  (#0)

	;; 4 notes are played, return
	;;    1: first note  (#0)
	;;    2: second note (#1)
	;;    3: third note  (#2)
	;;    4: fourth note (#3)

	;; how many notes are played?
	movlw	0x03
	movf	PLUSW1, W
	bnz	SEQ_CORE_Hlp_GetArpKey_4

	movlw	0x02
	movf	PLUSW1, W
	bnz	SEQ_CORE_Hlp_GetArpKey_3

	movlw	0x01
	movf	PLUSW1, W
	bnz	SEQ_CORE_Hlp_GetArpKey_2

SEQ_CORE_Hlp_GetArpKey_1
	BRA_IFCLR PRODL, 0, ACCESS, SEQ_CORE_Hlp_GetArpKey_1_NoMulti
SEQ_CORE_Hlp_GetArpKey_1_Multi
	movlw	0x01
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_1_NoMulti
	movf	INDF1, W
	return

SEQ_CORE_Hlp_GetArpKey_2
	BRA_IFCLR PRODL, 0, ACCESS, SEQ_CORE_Hlp_GetArpKey_2_NoMulti
SEQ_CORE_Hlp_GetArpKey_2_Multi
	movlw	0x02
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_2_NoMulti
	movlw	0x00
	btfsc	TABLAT, 0
	movlw 0x01
	movf	PLUSW1, W
	return

SEQ_CORE_Hlp_GetArpKey_3
	BRA_IFCLR PRODL, 0, ACCESS, SEQ_CORE_Hlp_GetArpKey_3_NoMulti
SEQ_CORE_Hlp_GetArpKey_3_Multi
	movlw	0x03
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_3_NoMulti
	movlw	0x00
	BRA_IFSET TABLAT, 1, ACCESS, SEQ_CORE_Hlp_GetArpKey_3_34
SEQ_CORE_Hlp_GetArpKey_3_12
	btfsc	TABLAT, 0
	movlw 0x01
	rgoto	SEQ_CORE_Hlp_GetArpKey_3_C
SEQ_CORE_Hlp_GetArpKey_3_34
	btfss	TABLAT, 0
	movlw 0x02
SEQ_CORE_Hlp_GetArpKey_3_C
	movf	PLUSW1, W
	return

SEQ_CORE_Hlp_GetArpKey_4
	BRA_IFCLR PRODL, 0, ACCESS, SEQ_CORE_Hlp_GetArpKey_4_NoMulti
SEQ_CORE_Hlp_GetArpKey_4_Multi
	movlw	0x04
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_4_NoMulti
	movf	TABLAT, W
	movf	PLUSW1, W
	return

SEQ_CORE_Hlp_GetArpKey_Multi
	movlw	SEQ_TRKVARARPPOSx ; select key
	movf	PLUSW2, W
	andlw	0x03
	movwf	PRODL		; PRODL[1:0] contains the selected note index

	movf	PLUSW1, W	; if current note is zero, key was released in the meantime -> overflow
	bz	SEQ_CORE_Hlp_GetArpKey_MultiOv0

	movf	PRODL, W
	addlw	0x01		; increment key number for next step, wrap on overflow (depends on number of pressed keys)
	cpfsgt	TABLAT, ACCESS
	rgoto SEQ_CORE_Hlp_GetArpKey_MultiOv
SEQ_CORE_Hlp_GetArpKey_MultiNoOv
	movwf	TABLAT		; save new index
	movlw	SEQ_TRKVARSTEPx	; restore last position so that this step will be played again
	movff	SEQ_CORE_TRK_LASTPOS, PLUSW2
	rgoto	SEQ_CORE_Hlp_GetArpKey_MultiOv_C

SEQ_CORE_Hlp_GetArpKey_MultiOv0
	clrf	PRODL
SEQ_CORE_Hlp_GetArpKey_MultiOv
	clrf	TABLAT
SEQ_CORE_Hlp_GetArpKey_MultiOv_C
	movlw	SEQ_TRKVARARPPOSx
	movff	TABLAT, PLUSW2

	;; finally return note which should be played
	movf	PRODL, W
	movf	PLUSW1, W
	return
