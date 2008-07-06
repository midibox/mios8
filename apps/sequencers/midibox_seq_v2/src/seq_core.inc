; $Id$
;
; MIDIboxSEQ
; Sequencer core functions
;
; Activate this #define to measure the performance with a scope
; (connect the probe to J14)
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
SEQ_REQ_CC_DUMP			EQU	3	; request CC dump
SEQ_REQ_NOECHO			EQU	4	; don't echo 0xfa 0xfb 0xfc to MIDI out
SEQ_REQ_SYNCHED_PATTERN_CHANGE	EQU	5	; pattern change should be synched to beat

SEQ_STATE_RUN			EQU	0	; set if sequencer runs
SEQ_STATE_PAUSE			EQU	1	; set if pause
SEQ_STATE_MANUAL_TRIGGER	EQU	2	; manual trigger active
SEQ_STATE_SONG			EQU	3	; set in song mode
SEQ_STATE_REMOTE		EQU	4	; used by MIDI Remote function
SEQ_STATE_FORCE_EEPROM		EQU	5	; if set, step event will always be read from EEPROM

SEQ_TRKSTATE_PLAY_STEP		EQU	0	; temp. flag, if set, the step will be played
SEQ_TRKSTATE_BACKWARD		EQU	1	; static flag, if set, the track will be played in backward direction
SEQ_TRKSTATE_PINGPONG		EQU	2	; temp. flag, if set, the track is in pingpong mode
SEQ_TRKSTATE_LEGATO		EQU	3	; temp. flag, if set, a note will be played after ON event for a proper legato
SEQ_TRKSTATE_FIRST_CLK		EQU	4	; don't increment on the first clock event
SEQ_TRKSTATE_STRETCH_GL		EQU	5	; stretch flag, set when gatelength >= 23
SEQ_TRKSTATE_RETRIGGER		EQU	6	; retrigger flag
SEQ_TRKSTATE_POS_RESET		EQU	7	; set by seq_midi.inc if position of ARP/Transpose tracks should be reset

SEQ_TRKMODE_OFF			EQU	0x00	; not bits, but values!
SEQ_TRKMODE_NORMAL		EQU	0x01	; dont change the order!
SEQ_TRKMODE_TRANSPOSE		EQU	0x02	; it must be ensured that:
SEQ_TRKMODE_TRANSPOSE_HOLD	EQU	0x03	;    o 0x00 switches the track of
SEQ_TRKMODE_ARPEGGIATOR		EQU	0x04	;    o 0x06 turns the track to "drum mode"
SEQ_TRKMODE_ARPEGGIATOR_HOLD	EQU	0x05	;    o 0x07 turns the track to "controller mode"
SEQ_TRKMODE_DRUMS		EQU	0x06	;    o bit #1 enables the transpose mode
SEQ_TRKMODE_CONTROLLER		EQU	0x07	;    o bit #2 enables the arpeggiator mode

SEQ_TRKMODE_DIR_FORWARD		EQU	0x00	; not bits, but values!
SEQ_TRKMODE_DIR_BACKWARD	EQU	0x01
SEQ_TRKMODE_DIR_PINGPONG	EQU	0x02
SEQ_TRKMODE_DIR_RANDOM		EQU	0x03

SEQ_MODE0_STEP_EDIT		EQU	0	; we are in step edit mode
SEQ_MODE0_MUTE			EQU	1	; change mute status with GP buttons
SEQ_MODE0_PATTERN		EQU	2	; change pattern with GP buttons
SEQ_MODE0_SONG			EQU	3	; we are in song mode
SEQ_MODE0_MENU			EQU	4	; change menu with GP buttons
SEQ_MODE0_FAST_ENCODERS		EQU	5	; encoders in fast speed mode
SEQ_MODE0_CHANGE_ALL_STEPS	EQU	6	; change all steps
SEQ_MODE0_SOLO			EQU	7	; play current track solo
SEQ_MODE0_MODIFY_ACCENT		EQU	SEQ_MODE0_FAST_ENCODERS	; drum mode only! assigned to the "fast" button

SEQ_MODE1_METRONOME		EQU	0	; enable metronome
SEQ_MODE1_SCRUB			EQU	1	; enable scrub mode
SEQ_MODE1_RECORD		EQU	2	; we are in step record menu

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called by USER_Init to initialize the sequencer
;; --------------------------------------------------------------------------
SEQ_CORE_Init
	SET_BSR	SEQ_BASE
	clrf	SEQ_PATTERN, BANKED

	;; init transpose/arpeggiator channel
	movlw	(DEFAULT_SEQUENCER_CHANNEL - 1) & 0xff
	movwf	SEQ_MIDI_CHANNEL, BANKED

	;; restart pseudo random generator
	movlw	0x42
	movwf	SEQ_RANDOM_SEED_L, BANKED
	movwf	SEQ_RANDOM_SEED_H, BANKED

	;; select track 1 by default
	movlw	0xf1
	movwf	SEQ_SELECTED_TRKS, BANKED

	;; select layer 1 by default
	movlw	0x01
	movwf	SEQ_SELECTED_LAYERS, BANKED

	;; go into edit mode by default
	movlw	(1 << SEQ_MODE0_STEP_EDIT)
	movwf	SEQ_MODE0, BANKED

	;; disable song
	movlw	0x80
	movwf	SEQ_SONG, BANKED

	;; select patterns
	clrf	SEQ_PATTERN0, BANKED	; Trk  1- 4: starts with A1
	movlw	0x80
	movwf	SEQ_PATTERN1, BANKED	; Trk  5- 8: disabled after power-on
	movwf	SEQ_PATTERN2, BANKED	; Trk  9-12: disabled after power-on
	movwf	SEQ_PATTERN3, BANKED	; Trk 13-16: disabled after power-on

	;; initial transpose and arp hold keys
	movlw	0x3c		; C-3
	movwf	SEQ_NOTE_STACK_0_HOLD, BANKED
	movwf	SEQ_ARP_NOTE_0_HOLD, BANKED
	movlw	0x40		; E-3
	movwf	SEQ_ARP_NOTE_1_HOLD, BANKED
	movlw	0x43		; G-3
	movwf	SEQ_ARP_NOTE_2_HOLD, BANKED
	movlw	0x48		; C-4
	movwf	SEQ_ARP_NOTE_3_HOLD, BANKED

	;; restore dump
	call	SEQ_DUMP_RestoreAll

	;; reset song
	call	SEQ_SONG_Reset

	;; reset sequencer and exit
	rgoto	SEQ_CORE_Reset


;; --------------------------------------------------------------------------
;;  Reset sequencer position
;; --------------------------------------------------------------------------
SEQ_CORE_Reset
	;; play off events of all tracks
	rcall	SEQ_CORE_Hlp_PlayAllOffEvnts

	SET_BSR SEQ_BASE
	movf	SEQ_BPM, W, BANKED
	bz	SEQ_CORE_Reset_NotMaster
SEQ_CORE_Reset_Master
	;; cancel all requested clocks
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED

	;; init the reference counters
	setf	SEQ_CLK_TICK_CTR, BANKED
	setf	SEQ_CLK_STEP_CTR, BANKED

	;; clear all sequencer requests (i.E. a stop event!)
	clrf	SEQ_REQ, BANKED
SEQ_CORE_Reset_NotMaster

	lfsr	FSR0, SEQ_TRK0
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_ResetLoop
	;; don't increment on the first clock event
	movlw	SEQ_TRKSTATEx
	bsf	PLUSW0, SEQ_TRKSTATE_FIRST_CLK

	;; branch depending on forward/backward direction
	movlw	SEQ_TRKMODEx
	swapf	PLUSW0, W	; SEQ_TRKMODEx[7..4] contains direction
	andlw	0x0f
	xorlw	SEQ_TRKMODE_DIR_BACKWARD
	bnz	SEQ_CORE_ResetLoop_F
SEQ_CORE_ResetLoop_B
	;; Backward direction
	;; set "backward" flag
	movlw	SEQ_TRKSTATEx
	bsf	PLUSW0, SEQ_TRKSTATE_BACKWARD

	;; ???
	movlw	0x0f
	movwf	TMP1
	movlw	SEQ_TRKPOSx
	movff	TMP1, PLUSW0
	;; determine max value and copy it to SEQ_TRKSTEPx
	;; get SEQ_TRKDIVLENx[3..0] (track length)
	movlw	SEQ_TRKDIVLENx
	movf	PLUSW0, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1
	movlw	SEQ_TRKSTEPx
	movff	TMP1, PLUSW0
	rgoto	SEQ_CORE_ResetLoop_Next
SEQ_CORE_ResetLoop_F
	;; Forward/PingPong/Random direction
	;; clear "backward" flag
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_BACKWARD
	;; reset TRKSTEPx
	movlw	SEQ_TRKSTEPx
	clrf	PLUSW0
	;; reset TRKRETRGx
	movlw	SEQ_TRKRETRGx
	clrf	PLUSW0
	;; reset SEQ_TRKPOSx depending on direction
	;; by default: reset to 0x00, forward direction
	movlw	SEQ_TRKPOSx
	clrf	PLUSW0

SEQ_CORE_ResetLoop_Next
	;; increment SEQ_BASE until last track is reached
	movlw	SEQ_TRKRECORD_LENGTH	; switch to next record
	addwf	FSR0L, F
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
	rcall	SEQ_CORE_Cont			; process the continue event
SEQ_CORE_Tick_NoCont

	;; ---[ check if a stop event has been requested ]---
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_REQ, SEQ_REQ_STOP, BANKED, SEQ_CORE_Tick_NoStop
	bcf	SEQ_REQ, SEQ_REQ_STOP, BANKED	; clear request
	rcall	SEQ_CORE_Stop			; process the continue event
SEQ_CORE_Tick_NoStop

	;; ---[ check if a clock event has been requested ]---
SEQ_CORE_Tick_ClkLoop
	SET_BSR	SEQ_BASE
	movf	SEQ_CLK_REQ_CTR, W, BANKED
	bz	SEQ_CORE_Tick_NoClk
SEQ_CORE_Tick_Clk
	;; note: the clock counter ensures that a clock event never get lost
	;; see also the header information of seq_bpm.inc
	decf	SEQ_CLK_REQ_CTR, F, BANKED
	;; don't execute clock if sequencer not running
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Tick_NoClk
	BRA_IFSET SEQ_STATE, SEQ_STATE_PAUSE, BANKED, SEQ_CORE_Tick_NoClk
	rcall	SEQ_CORE_Clk			; process clock event
SEQ_CORE_Tick_NoClk

	;; ---[ generate new pseudo random number ]---
	rcall	SEQ_CORE_GenRandomNumber

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
	;; send FA if echo allowed
	movlw	0xfa
	CALL_IFCLR SEQ_REQ, SEQ_REQ_NOECHO, BANKED, MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; reset song
	call	SEQ_SONG_Reset

	;; reset pattern
	rcall	SEQ_CORE_Reset

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
	;; send FB if echo allowed
	movlw	0xfb
	CALL_IFCLR SEQ_REQ, SEQ_REQ_NOECHO, BANKED, MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

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
	;; send FC if echo allowed
	movlw	0xfc
	CALL_IFCLR SEQ_REQ, SEQ_REQ_NOECHO, BANKED, MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE

	;; clear hold'ed notes
#if 0	; (disabled - doesn't make fun when playing live)
	clrf	SEQ_NOTE_STACK_0_HOLD, BANKED
	clrf	SEQ_NOTE_STACK_1_HOLD, BANKED
	clrf	SEQ_NOTE_STACK_2_HOLD, BANKED
	clrf	SEQ_NOTE_STACK_3_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_0_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_1_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_2_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_3_HOLD, BANKED
#endif

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
	;; prepare SEQ_CURRENT_STEP (-1)
	SET_BSR	SEQ_BASE
	decf	SEQ_CURRENT_STEP, W, BANKED
	andlw	0x0f
	movwf	TMP1

	;; set flag and position in every track record
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_TRK0
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_ManualTriggerLoop
	movlw	SEQ_TRKPOSx
	movff	TMP1, PLUSW0
	movlw	SEQ_TRKSTEPx
	clrf	PLUSW0
	movlw	SEQ_TRKRETRGx
	clrf	PLUSW0
	movlw	SEQ_TRKRECORD_LENGTH	; switch to next record
	addwf	FSR0L, F
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_ManualTriggerLoop

	;; init the reference counters
	setf	SEQ_CLK_TICK_CTR, BANKED
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

#if SEQ_CORE_MEASURE_PERFORMANCE
	bsf	PORTD, 4
#endif

	;; ------------------------------------------------------------------
	;; increment reference counters (if "first clk" flag not set)
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_Reference
	;; increment tick counter
	incf	SEQ_CLK_TICK_CTR, F, BANKED
	bz	SEQ_CORE_Clk_Reference_ResetT
	movlw	(4*6)-1		; we are working with 4 times resolution
	cpfsgt	SEQ_CLK_TICK_CTR, BANKED
	rgoto SEQ_CORE_Clk_Reference_NoOv
SEQ_CORE_Clk_Reference_ResetT
	;; clear tick counter
	clrf	SEQ_CLK_TICK_CTR, BANKED
	;; if step display update enabled, request display update
	btfsc	CS_STAT, CS_STAT_STEP_DISPLAY_UPDATE
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; increment step counter
	incf	SEQ_CLK_STEP_CTR, F, BANKED
	movlw	16-1
	cpfsgt	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_Reference_NoOv
SEQ_CORE_Clk_Reference_ResetS
	;; clear step counter
	clrf	SEQ_CLK_STEP_CTR, BANKED
SEQ_CORE_Clk_Reference_NoOv

	;; ------------------------------------------------------------------
	;; in manual trigger mode: request stop when last subtick of step reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_StopManual
	BRA_IFCLR SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_NoStopManual
	movlw	(4*6)-2
	cpfsgt	SEQ_CLK_TICK_CTR, BANKED
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

	movlw	(4*6)-10	; (this value has to be adjusted - depends on the time which is necessary to load 4 patterns)
	cpfseq	SEQ_CLK_TICK_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
	movlw	16-1
	cpfseq	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextPS_End
SEQ_CORE_Clk_NextPS_Request
	call	SEQ_SONG_NextPos
SEQ_CORE_Clk_NextPS_End	

	;; ------------------------------------------------------------------
	;; change to new pattern if requested
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextP
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextP_End
	movlw	(4*6)-9	; (this value has to be adjusted - depends on the time which is necessary to load 4 patterns)
	cpfseq	SEQ_CLK_TICK_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextP_End
	BRA_IFCLR SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED, SEQ_CORE_Clk_NextP_NoSynch
SEQ_CORE_Clk_NextP_Synch
	movlw	16-1
	cpfseq	SEQ_CLK_STEP_CTR, BANKED
	rgoto SEQ_CORE_Clk_NextP_End
SEQ_CORE_Clk_NextP_NoSynch
	rcall	SEQ_CORE_ChangePatternCheck
SEQ_CORE_Clk_NextP_End	

	;; ------------------------------------------------------------------
	;; send F8 on every 4th internal clock if echo allowed and not in slave mode
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NoF8
	movf	SEQ_CLK_TICK_CTR, W, BANKED
	andlw	0x03
	bnz	SEQ_CORE_Clk_NoF8
SEQ_CORE_Clk_F8
	;; send MIDI clock if BPM != 0 (master mode)
	movf	SEQ_BPM, W, BANKED
	bz	SEQ_CORE_Clk_NoF8
	movlw	0xf8
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	SEQ_BASE
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
	movf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, W, BANKED
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
	movf	SEQ_CLK_TICK_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NoMetro

	;; play ON event on every 4th step
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	andlw	0x03
	bz	SEQ_CORE_Clk_Metro_On
	;; play OFF event on every 4th+1 step
	xorlw	0x01
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
	movlw	0x90
	iorwf	SEQ_METRONOME_CHANNEL, W, BANKED
	movwf	MIDI_EVNT0	

	;; play special note on every 16th step
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_Metro_N16
SEQ_CORE_Clk_Metro_16
	movlw	DEFAULT_METRONOME_NOTE_MEASURE
	movwf	MIDI_EVNT1
	movlw	DEFAULT_METRONOME_VEL_MEASURE
	rgoto	SEQ_CORE_Clk_Metro_Cont
SEQ_CORE_Clk_Metro_N16
	movlw	DEFAULT_METRONOME_NOTE_BEAT
	movwf	MIDI_EVNT1
	movlw	DEFAULT_METRONOME_VEL_BEAT
SEQ_CORE_Clk_Metro_Cont
	movwf	MIDI_EVNT_VALUE
	;; memorize byte #0 and #1 for the OFF function
	movff	MIDI_EVNT0, SEQ_METRONOME_OFF_EVNT0
	movff	MIDI_EVNT1, SEQ_METRONOME_OFF_EVNT1

SEQ_CORE_Clk_Metro_Play
	;; send MIDI event
	call	MIDI_EVNT_Send
	;; and fix BSR
	SET_BSR	SEQ_BASE
SEQ_CORE_Clk_NoMetro
	

	;; ------------------------------------------------------------------
	;; starting the big clock loop which handles every single track separately
	;; ------------------------------------------------------------------
	lfsr	FSR0, SEQ_TRK0
	clrf	SEQ_EVNTT, BANKED	; using SEQ_EVNTT as loop counter
SEQ_CORE_Clk_Loop

	;; ------------------------------------------------------------------
	;; play "off event" when TRKQUEUELx is 1
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_OffEvent
	;; skip if length already zero
	movlw	SEQ_TRKQUEUELx
	movf	PLUSW0, W
	bz	SEQ_CORE_Clk_NoOffEvent
	;; decrement length counter
	movlw	SEQ_TRKQUEUELx
	decf	PLUSW0, F
	;; skip if length not zero
	bnz	SEQ_CORE_Clk_NoOffEvent
	;; else play off events
	clrf	SEQ_EVNTD, BANKED		; starting with drum layer A
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNTD, F, BANKED		; drum layer B (only relevant in drum mode)
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNTD, F, BANKED		; drum layer C (only relevant in drum mode)
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
SEQ_CORE_Clk_NoOffEvent

	;; ------------------------------------------------------------------
	;; if reference tick counter is zero, set new groove value
	;; ------------------------------------------------------------------
	movf	SEQ_CLK_TICK_CTR, W, BANKED
	bnz	SEQ_CORE_Clk_NoNewGroove
SEQ_CORE_Clk_NewGroove
	;; determine PLYTICK[3:0] value for this step
	rcall	SEQ_CORE_Hlp_SetGroove	; (sets SEQ_TRKPLYTICKx[3:0])
SEQ_CORE_Clk_NoNewGroove

	;; ------------------------------------------------------------------
	;; Decrement the delay counter (for multiple triggered steps)
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_DecReTrgCtr
	;; clear retrigger flag
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_RETRIGGER

	;; decrement delay counter (SEQ_TRKRETRGx[4..0]) so long the whole SEQ_TRKRETRGx register is > 0
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, F
	bz	SEQ_CORE_Clk_NoDecReTrgCtr
	decf	PLUSW0, F

	;; don't retrigger if repeat counter (SEQ_TRKRETRGx[6..5]) == 0
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, W
	andlw	0x60
	bz	SEQ_CORE_Clk_NoDecReTrgCtr

	;; no retrigger if delay counter (SEQ_TRKRETRGx[4..0]) > 0
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, W
	movwf	TABLAT
	andlw	0x1f
	bnz	SEQ_CORE_Clk_NoReTrg

SEQ_CORE_Clk_ReTrigger
	;; decrement repeat counter and write it to SEQ_TRKRETRGx
	movlw	-0x20
	addwf	TABLAT, F
	movlw	SEQ_TRKRETRGx
	movff	TABLAT, PLUSW0

	;; set retrigger flag
	movlw	SEQ_TRKSTATEx
	bsf	PLUSW0, SEQ_TRKSTATE_RETRIGGER
SEQ_CORE_Clk_NoDecReTrgCtr
SEQ_CORE_Clk_NoReTrg

	;; ------------------------------------------------------------------
	;; check if tick counter matches with PLYTICK[3:0]
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_ChkPlay
	movlw	SEQ_TRKPLYTICKx
	movf	PLUSW0, W
	andlw	0x0f
	xorwf	SEQ_CLK_TICK_CTR, W, BANKED
	bz	SEQ_CORE_Clk_NextStep

	;; force to play ON event if retrigger flag set
	movlw	SEQ_TRKSTATEx
	BRA_IFSET PLUSW0, SEQ_TRKSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_OnEvent

	;; else continue with next track
	rgoto	SEQ_CORE_Clk_NextTrack

	;; ------------------------------------------------------------------
	;; Play new note immediately if manual trigger has been requested
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_ManualTrigger
	BRA_IFSET SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_OnEvent

	;; ------------------------------------------------------------------
	;; Determine track position of next step
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextStep
	;; don't increment on the first clock event
	movlw	SEQ_TRKSTATEx
	BRA_IFCLR PLUSW0, SEQ_TRKSTATE_FIRST_CLK, ACCESS, SEQ_CORE_Clk_NextStep_No1st
SEQ_CORE_Clk_NextStep_1st
	bcf	PLUSW0, SEQ_TRKSTATE_FIRST_CLK
	bsf	PLUSW0, SEQ_TRKSTATE_PLAY_STEP
	rgoto	SEQ_CORE_Clk_NextStep_NoIncDec

SEQ_CORE_Clk_NextStep_No1st
	;; increment SEQ_TRKSTEPx and save new value in TMP1
	movlw	SEQ_TRKSTEPx
	incf	PLUSW0, F
	movff	PLUSW0, TMP1
	;; check for max value which depends on divider setting
	movlw	SEQ_TRKDIVLENx
	movf	PLUSW0, W
#if DEFAULT_BPM_DIVIDER_BEHAVIOUR == 0
	;; 0->1, 1->2, 2->4, 3->8, 4->16... hey, thats simply the output of MIOS_HLP_GetBitORMask
	call	MIOS_HLP_GetBitORMask
#else
	andlw	0x0f
	addlw	1
#endif
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_Clk_NextStep_IncDec
	;; if max value not reached ensure that the next step will not be played
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_PLAY_STEP
	rgoto	SEQ_CORE_Clk_NextStep_NoIncDec

SEQ_CORE_Clk_NextStep_IncDec
	;; save current position (for multi triggers)
	movlw	SEQ_TRKPOSx
	movff	PLUSW0, SEQ_CORE_TRK_LASTPOS

	;; if reset has been requested: clear ARP key position counter
	movlw	SEQ_TRKSTATEx
	BRA_IFCLR PLUSW0, SEQ_TRKSTATE_POS_RESET, ACCESS, SEQ_CORE_Clk_NextStep_IncDec_NoR
SEQ_CORE_Clk_NextStep_IncDec_R
	movlw	SEQ_TRKPLYTICKx
	bcf	PLUSW0, 4
	bcf	PLUSW0, 5
SEQ_CORE_Clk_NextStep_IncDec_NoR


	;; clear frame counter and increment/decrement SEQ_TRKPOSx if max value reached
	movlw	SEQ_TRKSTEPx
	clrf	PLUSW0
	;; store pointer to SEQ_TRKSTATEx in FSR1
	lfsr	FSR1, SEQ_TRK0
	movf	FSR0L, W	; (track offset)
	addlw	SEQ_TRKSTATEx
	movwf	FSR1L
	;; clear ping-pong mode flag (will be set later if this mode is enabled)
	bcf	INDF1, SEQ_TRKSTATE_PINGPONG
	;; notify that a step should be played
	bsf	INDF1, SEQ_TRKSTATE_PLAY_STEP
	;; continue depending on direction
	;; SEQ_TKMODEx[7..4] contains the direction
	movlw	SEQ_TRKMODEx
	swapf	PLUSW0, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_CORE_Clk_NextStep_F
	rgoto	SEQ_CORE_Clk_NextStep_B
	rgoto	SEQ_CORE_Clk_NextStep_P
	rgoto	SEQ_CORE_Clk_NextStep_R
	
SEQ_CORE_Clk_NextStep_F
	movlw	SEQ_TRKSTATEx
	BRA_IFSET PLUSW0, SEQ_TRKSTATE_POS_RESET, ACCESS, SEQ_CORE_Clk_NextStep_FR
	movlw	SEQ_TRKPOSx
	movff	PLUSW0, TMP1
	;; step length stored in SEQ_TRKDIVLENx[7..4]
	movlw	SEQ_TRKDIVLENx
	swapf	PLUSW0, W
	andlw	0x0f
	cpfslt	TMP1, ACCESS
	rgoto SEQ_CORE_Clk_NextStep_FR
	movlw	SEQ_TRKPOSx
	incf	PLUSW0, F
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextStep_C

	;; TODO: synchronized "next pattern"
	rgoto	SEQ_CORE_Clk_NextStep_C

SEQ_CORE_Clk_NextStep_FR
	BRA_IFSET INDF1, SEQ_TRKSTATE_PINGPONG, ACCESS, SEQ_CORE_Clk_NextStep_FP
SEQ_CORE_Clk_NextStep_CF
	movlw	SEQ_TRKPOSx
	clrf	PLUSW0
	movlw	SEQ_TRKSTATEx
	btfsc	PLUSW0, SEQ_TRKSTATE_POS_RESET
	clrf	SEQ_CORE_TRK_LASTPOS, BANKED
	rgoto	SEQ_CORE_Clk_NextStep_C
SEQ_CORE_Clk_NextStep_FP
	bsf	INDF1, SEQ_TRKSTATE_BACKWARD
	rgoto	SEQ_CORE_Clk_NextStep_C

SEQ_CORE_Clk_NextStep_B
	movlw	SEQ_TRKSTATEx
	BRA_IFSET PLUSW0, SEQ_TRKSTATE_POS_RESET, ACCESS, SEQ_CORE_Clk_NextStep_BR
	movlw	SEQ_TRKPOSx
	movf	PLUSW0, W
	bz	SEQ_CORE_Clk_NextStep_BR
	movlw	SEQ_TRKPOSx
	decf	PLUSW0, F
	rgoto	SEQ_CORE_Clk_NextStep_C
SEQ_CORE_Clk_NextStep_BR
	BRA_IFSET INDF1, SEQ_TRKSTATE_PINGPONG, ACCESS, SEQ_CORE_Clk_NextStep_BP
SEQ_CORE_Clk_NextStep_CB
	movlw	SEQ_TRKDIVLENx
	swapf	PLUSW0, W
	andlw	0x0f
	movwf	TMP1
	movlw	SEQ_TRKPOSx
	movff	TMP1, PLUSW0
	btfsc	INDF1, SEQ_TRKSTATE_POS_RESET
	movff	TMP1, SEQ_CORE_TRK_LASTPOS
	rgoto	SEQ_CORE_Clk_NextStep_C
SEQ_CORE_Clk_NextStep_BP
	bcf	INDF1, SEQ_TRKSTATE_BACKWARD
	rgoto	SEQ_CORE_Clk_NextStep_C
SEQ_CORE_Clk_NextStep_P
	bsf	INDF1, SEQ_TRKSTATE_PINGPONG
SEQ_CORE_Clk_NextStep_R_C
	BRA_IFSET INDF1, SEQ_TRKSTATE_BACKWARD, ACCESS, SEQ_CORE_Clk_NextStep_B
	rgoto	SEQ_CORE_Clk_NextStep_F
SEQ_CORE_Clk_NextStep_R
	;; set new random direction
	bcf	INDF1, SEQ_TRKSTATE_BACKWARD
	btfsc	SEQ_RANDOM_SEED_L, 4, BANKED
	bsf	INDF1, SEQ_TRKSTATE_BACKWARD
	rgoto	SEQ_CORE_Clk_NextStep_R_C
	
SEQ_CORE_Clk_NextStep_C
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_POS_RESET

SEQ_CORE_Clk_NextStep_NoIncDec

	;; ------------------------------------------------------------------
	;; Don't send new event if sequencer not in RUN mode
	;; ------------------------------------------------------------------
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Clk_NextTrack

	;; ------------------------------------------------------------------
	;; else play "on event"
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_OnEvent
	;; check for manual trigger mode
	BRA_IFCLR SEQ_STATE, SEQ_STATE_MANUAL_TRIGGER, BANKED, SEQ_CORE_Clk_OnEvent_NM
	;; skip track if not selected for manual trigger
	BRA_IFSET SEQ_EVNTT, 3, BANKED, SEQ_CORE_Clk_NextTrack
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	andlw	0x0f
	bnz	SEQ_CORE_Clk_OnEventNMS
	rgoto	SEQ_CORE_Clk_NextTrack
	
SEQ_CORE_Clk_OnEvent_NM
	;; skip track if muted
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_TRKS_MUTED0, W, BANKED
	btfsc	SEQ_EVNTT, 3, BANKED
	andwf	SEQ_TRKS_MUTED1, W, BANKED
	skpz
	rgoto	SEQ_CORE_Clk_NextTrack
	
	;; skip track if the PLAY_STEP flag not set (important for clock dividers > 1)
	movlw	SEQ_TRKSTATEx
	BRA_IFCLR PLUSW0, SEQ_TRKSTATE_PLAY_STEP, ACCESS, SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_OnEventNMS

	;; skip track if solo and not selected track
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SOLO, BANKED, SEQ_CORE_Clk_OnEvent_NoSolo
SEQ_CORE_Clk_OnEvent_ChkSolo
	movf	SEQ_EVNTT, W, BANKED
	andlw	0xfc
	skpz
	rgoto	SEQ_CORE_Clk_NextTrack
	movf	SEQ_EVNTT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_TRKS, W, BANKED
	skpnz
	rgoto	SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_OnEvent_NoSolo

	;; skip track if mode is OFF
	movlw	SEQ_TRKMODEx
	movf	PLUSW0, W
	andlw	0x0f
	skpnz
	rgoto	SEQ_CORE_Clk_NextTrack

	;; skip track if mode is CONTROLLER
	xorlw	SEQ_TRKMODE_CONTROLLER
	skpnz
	rgoto	SEQ_CORE_Clk_NextTrack

	;; ------------------------------------------------------------------

	;; get parameter type
	call	SEQ_PTYPE_Get	; result in SEQ_PTYPE

	;; get track events of current track/step
	;; track number already in SEQ_EVNTT
	movlw	SEQ_TRKPOSx	; save step number (0-15) in SEQ_EVNTS
	movf	PLUSW0, W
	movwf	SEQ_EVNTS, BANKED

	clrf	SEQ_EVNTD, BANKED
SEQ_CORE_Clk_OnEvent_DrumLoop	;	this loop is only taken on drums (three notes have to be played)

	;; if in drum mode: check for layer specific solo
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_CORE_Clk_OnEvent_NoDrumSolo
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_SOLO, BANKED, SEQ_CORE_Clk_OnEvent_NoDrumSolo
SEQ_CORE_Clk_OnEvent_ChkDrumSolo
	movf	SEQ_EVNTD, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_SELECTED_LAYERS, W, BANKED
	skpnz
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEvent_NoDrumSolo
	
	call	SEQ_EVNT_Get
	;; events now in SEQ_EVNT[012L]

	;; no retrigger in drum mode
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_CORE_Clk_SendEvent_SkipReTrg

	;; don't play if retrigger counter (SEQ_TRKRETGx[4..0]) > 0
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, W
	andlw	0x1f
	skpz
	rgoto	SEQ_CORE_Clk_NoOnEvent

	;; don't handle multiple events per step if event is 0 (muted)
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_CORE_Clk_ReTrgNotMuted
SEQ_CORE_Clk_ReTrgMuted
	movlw	SEQ_TRKRETRGx		; clear retrigger counter
	clrf	PLUSW0
	rgoto	SEQ_CORE_Clk_SendEvent_SkipReTrg
SEQ_CORE_Clk_ReTrgNotMuted

	;; reload complete SEQ_TRKRETRGx if RETRIGGER flag is not set
	movlw	SEQ_TRKSTATEx
	BRA_IFSET PLUSW0, SEQ_TRKSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_ReTrgNoReload
SEQ_CORE_Clk_ReTrgReload
	;; clear it if SEQ_EVNTHRS[5..6] (retrigger counter) is 0 and exit retrigger handler (single note)
	movf	SEQ_EVNTTHRS, W, BANKED
	andlw	0x60
	bnz	SEQ_CORE_Clk_ReTrgReloadOk
SEQ_CORE_Clk_ReTrgReloadClr
	movlw	SEQ_TRKRETRGx
	clrf	PLUSW0
	rgoto	SEQ_CORE_Clk_SendEvent_SkipReTrg

SEQ_CORE_Clk_ReTrgReloadOk
	movlw	SEQ_TRKRETRGx
	movff	SEQ_EVNTTHRS, PLUSW0
SEQ_CORE_Clk_ReTrgNoReload

	;; if retrigger counter is > 0:
	;; set retrigger delay - it is stored in SEQ_EVNTTHRS[4..0], copy this value to SEQ_TRKRETRGx[4..0]
	movf	SEQ_EVNTTHRS, W, BANKED
	andlw	0x1f
	movwf	TABLAT
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, W
	andlw	0xe0
	bz	SEQ_CORE_Clk_ReTrgNoNewDelay
SEQ_CORE_Clk_ReTrgNewDelay
	iorwf	TABLAT, F
	movlw	SEQ_TRKRETRGx
	movff	TABLAT, PLUSW0
SEQ_CORE_Clk_ReTrgNoNewDelay

	;; if retrigger flag is set or SEQ_EVNTTHRS[6..5] > 0, we have to determine a new gatelength
	movlw	SEQ_TRKSTATEx
	BRA_IFSET PLUSW0, SEQ_TRKSTATE_RETRIGGER, ACCESS, SEQ_CORE_Clk_ReTrgFixLen
	movlw	SEQ_TRKRETRGx
	movf	PLUSW0, W
	andlw	0x60
	bz	SEQ_CORE_Clk_ReTrgDontFixLen
SEQ_CORE_Clk_ReTrgFixLen
	;; note length: length = retrigger delay / 2
	rrf	SEQ_EVNTTHRS, W, BANKED
	andlw	0x0f
	skpnz			; length must be > 0
	movlw	0x01
	movwf	SEQ_EVNTL, BANKED
SEQ_CORE_Clk_ReTrgDontFixLen
SEQ_CORE_Clk_SendEvent_SkipReTrg

	;; morph it
	call	SEQ_MORPH_Event

	;; humanize it
	call	SEQ_HUMANIZE_Event

	;; transpose if not in arpeggiator or drum mode
	movlw	SEQ_TRKMODEx
	CALL_IFCLR PLUSW0, 2, ACCESS, SEQ_EVNT_Transpose

	;; if event == 0, check if we have to stretch the gatelength
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_CORE_Clk_NoStretch	; not in drum mode
	movf	SEQ_EVNT0, W, BANKED
	bnz	SEQ_CORE_Clk_NoStretch
	movlw	SEQ_TRKSTATEx
	BRA_IFCLR PLUSW0, SEQ_TRKSTATE_STRETCH_GL, ACCESS, SEQ_CORE_Clk_NextTrack
SEQ_CORE_Clk_Stretch
	movlw	SEQ_TRKQUEUELx
	movff	SEQ_EVNTL, PLUSW0
	rgoto	SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_NoStretch

	;; ------------------------------------------------------------------

	;; by default no legato (only if: Note event and new note != current note)
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_LEGATO

	;; special handling on notelike events
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_CORE_Clk_SendEvent_NoVChk

	;; don't play if note number == 0x00
	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_CORE_Clk_NoOnEvent
	;; don't play if velocity == 0x00
	movf	SEQ_EVNT2, W, BANKED
	bz	SEQ_CORE_Clk_NoOnEvent

SEQ_CORE_Clk_SendEvent_NoVChk
	;; check for drum mode
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_CORE_Clk_SendEvent_NM_Ply
	;; check for transpose or arpeggiator function
	movlw	SEQ_TRKMODEx
	BRA_IFSET PLUSW0, 1, ACCESS, SEQ_CORE_Clk_SendEvent_Trn
	BRA_IFSET PLUSW0, 2, ACCESS, SEQ_CORE_Clk_SendEvent_Arp
	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply

	;; ------------------------------------------------------------------

SEQ_CORE_Clk_SendEvent_Trn
	BRA_IFSET PLUSW0, 0, ACCESS, SEQ_CORE_Clk_SendEvent_NM3
SEQ_CORE_Clk_SendEvent_NM2	; SEQ_TRKMODE_TRANSPOSE
	;; select last played note
	movf	SEQ_NOTE_STACK_0, W, BANKED
	rgoto	SEQ_CORE_Clk_SendEvent_NM2_C

SEQ_CORE_Clk_SendEvent_NM3	; SEQ_TRKMODE_TRANSPOSE_HOLD
	;; select last played note (hold mode)
	movf	SEQ_NOTE_STACK_0_HOLD, W, BANKED
SEQ_CORE_Clk_SendEvent_NM2_C

	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_CORE_Clk_SendEvent_Trn_N

	;; Transpose CC
SEQ_CORE_Clk_SendEvent_Trn_CC
	;; subtract base note (C-3)
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
	bz	SEQ_CORE_Clk_NoOnEvent

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
	BRA_IFSET PLUSW0, 0, ACCESS, SEQ_CORE_Clk_SendEvent_NM5
SEQ_CORE_Clk_SendEvent_NM4	; SEQ_TRKMODE_ARPEGGIATOR
	lfsr	FSR1, SEQ_ARP_NOTE_0
	rgoto	SEQ_CORE_Clk_SendEvent_NM4_C

SEQ_CORE_Clk_SendEvent_NM5	; SEQ_TRKMODE_ARPEGGIATOR_HOLD
	lfsr	FSR1, SEQ_ARP_NOTE_0_HOLD
SEQ_CORE_Clk_SendEvent_NM4_C

	;; arpeggiator cannot work with CC's
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_CORE_Clk_SendEvent_NM_Ply

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
	call	SEQ_EVNT_Transpose
	;; don't play on over/underrun
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_CORE_Clk_NextTrack

	;; 	rgoto	SEQ_CORE_Clk_SendEvent_NM_Ply

	;; ------------------------------------------------------------------

SEQ_CORE_Clk_SendEvent_NM_Ply
	;; skip this if no notelike event
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_CORE_Clk_SendEvent_NoNEvnt

	;; don't check if no note in queue (evnt0 == 0)
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr	; first byte equal?
	movf	INDF1, W
	bz	SEQ_CORE_Clk_SendEvent_NoLeg

	;; if current note equal to new note, send off event now
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
	movlw	SEQ_TRKSTATEx
	bsf	PLUSW0, SEQ_TRKSTATE_LEGATO
SEQ_CORE_Clk_SendEvent_NoLeg

SEQ_CORE_Clk_SendEvent_NoNEvnt

	;; ------------------------------------------------------------------

	;; play the new MIDI event
	movff	SEQ_EVNT0, MIDI_EVNT0
	movff	SEQ_EVNT1, MIDI_EVNT1
 	movff	SEQ_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE			; fix BSR

	;; if legato flag set: play off event
	movlw	SEQ_TRKSTATEx
	RCALL_IFSET PLUSW0, SEQ_TRKSTATE_LEGATO, ACCESS, SEQ_CORE_Hlp_PlayOffEvnt

	;; store off event and length in queue
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	movff	SEQ_EVNT0, INDF1
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	movff	SEQ_EVNT1, INDF1

	;; length only set once --- so, if we are in drum mode, and SEQ_EVNTD > 0, skip this part
	movf	SEQ_EVNTD, W, BANKED
	bnz	SEQ_CORE_Clk_OnEvent_NoNewLen
SEQ_CORE_Clk_OnEvent_NewLen
	;; set new gatelength, ensure that it is > 1
	movf	SEQ_EVNTL, W, BANKED
	skpnz
	incf	SEQ_EVNTL, F, BANKED

	movlw	SEQ_TRKQUEUELx
	movff	SEQ_EVNTL, PLUSW0
SEQ_CORE_Clk_OnEvent_NoNewLen

	;; ---
SEQ_CORE_Clk_NoOnEvent
SEQ_CORE_Clk_OnEventPostponed
SEQ_CORE_Clk_OnEventPlayed

	;; ------------------------------------------------------------------

	;; loop 3 times in drum mode
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_CORE_Clk_NoDrumLoop
	incf	SEQ_EVNTD, F, BANKED
	movlw	3-1
	cpfsgt	SEQ_EVNTD, BANKED
	rgoto SEQ_CORE_Clk_OnEvent_DrumLoop
SEQ_CORE_Clk_NoDrumLoop

	;; request stretching if original gatelength entry (SEQ_EVNTTHRS) is > 24 and < 32
	movlw	SEQ_TRKSTATEx
	bcf	PLUSW0, SEQ_TRKSTATE_STRETCH_GL
	movlw	24
	cpfsgt	SEQ_EVNTTHRS, BANKED
	rgoto SEQ_CORE_Clk_NoStretchReq
	movlw	32
	cpfslt	SEQ_EVNTTHRS, BANKED
	rgoto SEQ_CORE_Clk_NoStretchReq
	movlw	SEQ_TRKQUEUELx
	setf	PLUSW0
	movlw	SEQ_TRKSTATEx
	bsf	PLUSW0, SEQ_TRKSTATE_STRETCH_GL
	movlw	SEQ_TRKRETRGx	; ensure that note won't be retriggered
	clrf	PLUSW0
SEQ_CORE_Clk_NoStretchReq

	;; ------------------------------------------------------------------
	;; finally switch to next track until last track reached
	;; ------------------------------------------------------------------
SEQ_CORE_Clk_NextTrack
	movlw	SEQ_TRKRECORD_LENGTH	; switch to next record
	addwf	FSR0L, F
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_Clk_Loop
	;; ------------------------------------------------------------------

SEQ_CORE_Clk_End
	;; finally...
#if SEQ_CORE_MEASURE_PERFORMANCE
	bcf	PORTD, 4
#endif
	return

	
;; --------------------------------------------------------------------------
;;  set the sequencer position via MIDI (F2 event)
;;  IN: low-byte in SEQ_MIDIPOS_LSB, high-byte in SEQ_MIDIPOS_MSB
;;  USES: TMP[45]
;; --------------------------------------------------------------------------
SEQ_CORE_SetPos
	;; reset song
	rcall	SEQ_SONG_Reset

	;; reset pattern
	rcall	SEQ_CORE_Reset

	;; in song mode:
	SET_BSR	SEQ_BASE
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
	rcall	SEQ_SONG_FetchPos
	rgoto	SEQ_CORE_SetPos_SongEnd

SEQ_CORE_SetPos_SongIncLoop
	clrwdt			; feed the watchdog (just to ensure...)
	rcall	SEQ_SONG_NextPos
	decf	TMP4, F 
	skpc
	decf	TMP5, F
	bnz	SEQ_CORE_SetPos_SongIncLoop
	movf	TMP4, W
	bnz	SEQ_CORE_SetPos_SongIncLoop
	
SEQ_CORE_SetPos_SongEnd
SEQ_CORE_SetPos_NoSong

	;; copy sequencer pos-1 to reference counter
	SET_BSR	SEQ_BASE
	decf	SEQ_MIDIPOS_LSB, W, BANKED
	andlw	0x0f
	movwf	SEQ_CLK_STEP_CTR, BANKED

	;; set sequencer pos on all tracks
	movf	SEQ_MIDIPOS_LSB, W, BANKED
	andlw	0x0f
	movwf	TMP1

	;; store value in SEQ_TRKPOSx
	lfsr	FSR0, SEQ_TRK0
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_SetPos_Loop
	;; TODO: handle TRKPOSx correctly when in backward mode
	movlw	SEQ_TRKPOSx
	movff	TMP1, PLUSW0

	movlw	SEQ_TRKRECORD_LENGTH	; switch to next record
	addwf	FSR0L, F
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_SetPos_Loop

	return


;; --------------------------------------------------------------------------
;;  Use this function to change to new patterns
;;  IN: new pattern numbers in SEQ_PATTERNx
;;  OUT: next pattern requests in SEQ_NEXT_PATTERNx when patterns not equal
;;  USES: TMP1
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePatternSynched
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED, SEQ_CORE_ChangePatternSynchedSkp
	bsf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED
SEQ_CORE_ChangePatternSynchedSkp
SEQ_CORE_ChangePattern

	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_PATTERN0
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	clrf	SEQ_GROUP, BANKED
SEQ_CORE_ChangePattern_Loop
	movf	INDF1, W
	andlw	0x7f
	xorwf	INDF0, W
	bz	SEQ_CORE_ChangePattern_Loop_Next
	movff	INDF0, INDF1
SEQ_CORE_ChangePattern_Loop_Next
	movf	POSTINC0, W
	movf	POSTINC1, W
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_CORE_ChangePattern_Loop
	clrf	SEQ_GROUP, BANKED

	;; for save function: save PATTERN0
	movff	SEQ_PATTERN0, SEQ_SAVE_PATTERN

	;; if sequencer not running, change patterns immediately
	btfsc	SEQ_STATE, SEQ_STATE_RUN, BANKED
	return
	bcf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED	; (not synched)
 	call	SEQ_DUMP_RestoreGroups
	goto	SEQ_DUMP_RestorePots


;; --------------------------------------------------------------------------
;;  This function is used in SEQ_CORE_Tick to check, if the sequencer has
;;  to switch to new patterns
;; --------------------------------------------------------------------------
SEQ_CORE_ChangePatternCheck
	SET_BSR	SEQ_BASE
	clrf	SEQ_GROUP, BANKED
SEQ_CORE_ChangePatternCheckLoop
	;; select next pattern register
	lfsr	FSR0, SEQ_NEXT_PATTERN0

	;; check if pattern has already been updated (bit 7 set)
	movf	SEQ_GROUP, W, BANKED
	BRA_IFSET PLUSW0, 7, ACCESS, SEQ_CORE_ChangePatternCheckLoopN

	;; restore group
	movff	PLUSW0, SEQ_PATTERN
	call	SEQ_DUMP_RestoreTrk

	;; if group 0: restore also pot values
	SET_BSR	SEQ_BASE
	movf	SEQ_GROUP, W, BANKED
	skpnz
	call	SEQ_DUMP_RestorePots

	;; set bit 7 so that pattern won't be updated again until next request
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_NEXT_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	bsf	PLUSW0, 7
	
SEQ_CORE_ChangePatternCheckLoopN
	incf	SEQ_GROUP, F, BANKED
	BRA_IFCLR SEQ_GROUP, 2, BANKED, SEQ_CORE_ChangePatternCheckLoop

	;; clear synch request
	bcf	SEQ_REQ, SEQ_REQ_SYNCHED_PATTERN_CHANGE, BANKED
	;; SEQ_PATTERN must contain the edit pattern!
	movff	SEQ_PATTERN0, SEQ_PATTERN
	;; group should be zero!
	clrf	SEQ_GROUP, BANKED

	return

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE0
;;  IN: SEQ_EVNTT and SEQ_EVNTD
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE0_BEGIN

SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C
	movf	SEQ_EVNTT, W, BANKED
	mullw	3
	movf	PRODL, W
	addwf	SEQ_EVNTD, W, BANKED
	addwf	FSR1L, F
	return

;; --------------------------------------------------------------------------
;;  calculates the pointer to SEQ_TRKQUEUE1
;;  IN: SEQ_EVNTT and SEQ_EVNTD
;;  OUT: pointer in FSR1
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRKQUEUE1_BEGIN
	rgoto	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr_C

;; --------------------------------------------------------------------------
;;  sends the content of SEQ_TRKQUEUExx via MIDI and clears the queue
;;  IN: SEQ_EVNTT, SEQ_EVNTD, pointer to SEQ_TRKx in FSR0
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_PlayOffEvnt
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr	; send first byte if != 0
	movf	INDF1, W
	bz	SEQ_CORE_Hlp_PlayOffEvnt_Skip
	movwf	MIDI_EVNT0

	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr	; send second byte
	movf	INDF1, W
	movwf	MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE			; send 0x00
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE			; fix BSR

SEQ_CORE_Hlp_PlayOffEvnt_Skip
	;; clear queue
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	clrf	INDF1
	rcall	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	clrf	INDF1

	movlw	SEQ_TRKQUEUELx			; (only reset length if drum layer == 0)
	movf	SEQ_EVNTD, F, BANKED
	skpnz
	clrf	PLUSW0

	return

;; --------------------------------------------------------------------------
;;  This function plays the off events of all tracks
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_PlayAllOffEvnts
	;; play off events
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_TRK0
	clrf	SEQ_EVNTT, BANKED
SEQ_CORE_Hlp_PlayAllOffEvntsL
	clrf	SEQ_EVNTD, BANKED
	rcall	SEQ_CORE_Hlp_PlayOffEvnt; play off event of selected track
	incf	SEQ_EVNTD, F, BANKED	; only relevant in drum mode
	rcall	SEQ_CORE_Hlp_PlayOffEvnt
	incf	SEQ_EVNTD, F, BANKED	; only relevant in drum mode
	rcall	SEQ_CORE_Hlp_PlayOffEvnt

	movlw	SEQ_TRKRECORD_LENGTH	; switch to next record
	addwf	FSR0L, F
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_CORE_Hlp_PlayAllOffEvntsL

	;; also for the metronome
	movf	SEQ_METRONOME_OFF_EVNT0, W, BANKED
	bz	SEQ_CORE_Hlp_PlayAllOffEvnts_End
	movff	SEQ_METRONOME_OFF_EVNT0, MIDI_EVNT0
	clrf	SEQ_METRONOME_OFF_EVNT0, BANKED; (don't play it again)
	movff	SEQ_METRONOME_OFF_EVNT1, MIDI_EVNT1
	clrf	MIDI_EVNT_VALUE
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
	addwfc	PRODH, W
	movwf	SEQ_RANDOM_SEED_H, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function sets the number of clock ticks which should be played per step
;;  IN: pointer to SEQ_TRKx in FSR0
;;  OUT: tick number in SEQ_TRKPLYTICKx[3:0]
;;  USES: TABLAT as temporary register
;; --------------------------------------------------------------------------
SEQ_CORE_Hlp_SetGroove
	;; MEMO:
	;;    SEQ_TRKGROOVEx[3..0]: groove value
	;;    SEQ_TRKGROOVEx[7..4]: groove style

	movlw	SEQ_TRKGROOVEx
	movf	PLUSW0, W
	andlw	0x0f
	movwf	TABLAT

	;; branch depending on style
	movlw	SEQ_TRKGROOVEx
	BRA_IFSET PLUSW0, 4, ACCESS, SEQ_CORE_Hlp_SetGroove_S1

	;; ------------------------------------------------------------------
SEQ_CORE_Hlp_SetGroove_S0	; Style 0: Shuffle
	;; branch depending on (reference) position
	BRA_IFSET SEQ_CLK_STEP_CTR, 0, BANKED, SEQ_CORE_Hlp_SetGroove_S0_1

SEQ_CORE_Hlp_SetGroove_S0_0
	movlw	0x00
	rgoto	SEQ_CORE_Hlp_SetGroove_End
	
SEQ_CORE_Hlp_SetGroove_S0_1
	movf	TABLAT, W
	rgoto	SEQ_CORE_Hlp_SetGroove_End

	;; ------------------------------------------------------------------
SEQ_CORE_Hlp_SetGroove_S1	; Style 1: Inverted Shuffle
	;; branch depending on (reference) position
	BRA_IFCLR SEQ_CLK_STEP_CTR, 0, BANKED, SEQ_CORE_Hlp_SetGroove_S1_1

SEQ_CORE_Hlp_SetGroove_S1_0
	movlw	0x00
	rgoto	SEQ_CORE_Hlp_SetGroove_End
	
SEQ_CORE_Hlp_SetGroove_S1_1
	movf	TABLAT, W
	rgoto	SEQ_CORE_Hlp_SetGroove_End

	;; ------------------------------------------------------------------
SEQ_CORE_Hlp_SetGroove_End
	movwf	TABLAT
	movlw	SEQ_TRKPLYTICKx
	movf	PLUSW0, W
	andlw	0xf0
	movwf	PRODL
	movf	TABLAT, W
	andlw	0x0f
	iorwf	PRODL, F
	movlw	SEQ_TRKPLYTICKx
	movff	PRODL, PLUSW0
	return


;; --------------------------------------------------------------------------
;;  This function returns the key from the played chord which should be sent
;;  to the arpeggiator
;;  IN: pointer to SEQ_TRKx in FSR0, pointer to chord stack in FSR1
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
	movlw	0x10
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_1_NoMulti
	movf	INDF1, W
	return

SEQ_CORE_Hlp_GetArpKey_2
	BRA_IFCLR PRODL, 0, ACCESS, SEQ_CORE_Hlp_GetArpKey_2_NoMulti
SEQ_CORE_Hlp_GetArpKey_2_Multi
	movlw	0x20
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
	movlw	0x30
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
	movlw	0x40
	movwf	TABLAT
	rgoto	SEQ_CORE_Hlp_GetArpKey_Multi
SEQ_CORE_Hlp_GetArpKey_4_NoMulti
	movf	TABLAT, W
	movf	PLUSW1, W
	return

SEQ_CORE_Hlp_GetArpKey_Multi
	movlw	SEQ_TRKPLYTICKx	; select key
	movf	PLUSW0, W
	andlw	0x30
	movwf	PRODL		; PRODL[6:5] contains the selected note index

	swapf	WREG, W		; if current note is zero, key was released in the meantime -> overflow
	movf	PLUSW1, W
	bz	SEQ_CORE_Hlp_GetArpKey_MultiOv0

	movf	PRODL, W
	addlw	0x10		; increment key number for next step, wrap on overflow (depends on number of pressed keys)
	cpfsgt	TABLAT, ACCESS
	rgoto SEQ_CORE_Hlp_GetArpKey_MultiOv
SEQ_CORE_Hlp_GetArpKey_MultiNoOv
	movwf	TABLAT		; save new index
	movlw	SEQ_TRKPOSx	; restore last position so that this step will be played again
	movff	SEQ_CORE_TRK_LASTPOS, PLUSW0
	rgoto	SEQ_CORE_Hlp_GetArpKey_MultiOv_C

SEQ_CORE_Hlp_GetArpKey_MultiOv0
	clrf	PRODL
SEQ_CORE_Hlp_GetArpKey_MultiOv
	clrf	TABLAT
SEQ_CORE_Hlp_GetArpKey_MultiOv_C
	movlw	SEQ_TRKPLYTICKx
	movf	PLUSW0, W
	andlw	0x0f
	iorwf	TABLAT, F
	movlw	SEQ_TRKPLYTICKx
	movff	TABLAT, PLUSW0

	;; finally return note which should be played
	swapf	PRODL, W
	movf	PLUSW1, W
	return
