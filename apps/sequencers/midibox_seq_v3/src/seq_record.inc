; $Id$
;
; MIDIbox SEQ
; Record Function
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called by SEQ_MIDI_NotifyReceivedEvent when a complete 
;;  MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_RECORD_NotifyEvent
	SET_BSR	SEQ_BASE

	;; select track and step
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; determine pointer to SEQ_TRKVAR -> FSR2
	mullw	SEQ_TRKVARRECORD_LENGTH
	lfsr	FSR2, SEQ_TRKVAR0
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	
	;; copy MIOS_PARAMETER[123] to SEQ_EVNT[123], the values are modified
	movff	MIOS_PARAMETER1, SEQ_EVNT0
	movff	MIOS_PARAMETER2, SEQ_EVNT1
	movff	MIOS_PARAMETER3, SEQ_EVNT2

	;; the length is 0x11 (75%) by default
	movlw	0x11
	movwf	SEQ_EVNTL, BANKED

	;; only continue of MIDI channel matches
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	skpnz
	rgoto	SEQ_RECORD_NotifyEvent_End
	addlw	-1
	xorwf	SEQ_EVNT0, W, BANKED
	andlw	0x0f
	skpz
	rgoto	SEQ_RECORD_NotifyEvent_End

	;; branch depending on Note/CC - ignore all other events
	movf	SEQ_EVNT0, W
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_RECORD_NotifyEventChk_CC
	movf	SEQ_EVNT0, W
	andlw	0xe0
	xorlw	0x80
	bz	SEQ_RECORD_NotifyEventChk_Note
	rgoto	SEQ_RECORD_NotifyEvent_End

SEQ_RECORD_NotifyEventChk_Note
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED

	;; if we've received a note off event, convert it to 0x9x with velocity 0
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x80
	bnz	SEQ_RECORD_NotifyEventChk_NoOff
	bsf	SEQ_EVNT0, 4, BANKED; 0x8x -> 0x9x
	clrf	SEQ_EVNT2, BANKED
SEQ_RECORD_NotifyEventChk_NoOff

	;; don't record event if velocity == 0 (Note Off)
	movf	SEQ_EVNT2, W, BANKED
	bnz	SEQ_RECORD_NotifyEventChk_Cont

SEQ_RECORD_NotifyEventChk_NOff
	;; if note number matches with last recorded EVNT1, release NOTE_ACTIVE flag
	movf	SEQ_EVNT1, W, BANKED
	cpfseq	SEQ_RECORD_CURRENT_EVNT1, BANKED
	rgoto SEQ_RECORD_NotifyEventChk_NOffS
	call	SEQ_FSR_TrkVarState2
	bcf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE
	bsf	INDF1, SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP

SEQ_RECORD_NotifyEventChk_NOffS
	;; play off events and skip recording
	call	SEQ_CORE_Hlp_PlayTrkOffEvnts
	rgoto	SEQ_RECORD_NotifyEvent_Skip


SEQ_RECORD_NotifyEventChk_CC
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED
	;; 	rgoto	SEQ_RECORD_NotifyEventChk_Cont
SEQ_RECORD_NotifyEventChk_Cont

	;; temporary store last record step in -> TMP1
	movff	SEQ_RECORD_STEP, TMP1
	;; determine step number if not in step mode
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, SEQ_RECORD_NotifyEvent_NoNewStp
	;; new step if sequencer is running
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_RECORD_NotifyEvent_NewStep
	;; clear step number if autostart enabled
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, SEQ_RECORD_NotifyEvent_NoNewStp
SEQ_RECORD_NotifyEvent_ClrStep
	clrf	SEQ_RECORD_STEP, BANKED
	rgoto	SEQ_RECORD_NotifyEvent_ClrStpC
SEQ_RECORD_NotifyEvent_NewStep
	call	SEQ_FSR_TrkVarStep
	movff	INDF1, SEQ_RECORD_STEP
SEQ_RECORD_NotifyEvent_NoNewStp
SEQ_RECORD_NotifyEvent_ClrStpC

	;; if new step: clear layer counter (for polyphonic recording)
	movf	TMP1, W
	xorwf	SEQ_RECORD_STEP, W, BANKED
	skpz
	clrf	SEQ_RECORD_POLY_CTR, BANKED

	;; transfer step number to SEQ_EVNTS
	movff	SEQ_RECORD_STEP, SEQ_EVNTS

	;; save current note/CC number and clear length counter
	movff	SEQ_EVNT1, SEQ_RECORD_CURRENT_EVNT1
	clrf	SEQ_RECORD_LENGTH_CTR, BANKED

	;; notify that note/CC is held
	call	SEQ_FSR_TrkVarState2
	bsf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE

	;; save the event in current step, loop 4 times (for event mode which store multiple events)
	clrf	SEQ_EVNT_NUMBER, BANKED
SEQ_RECORD_NotifyEvent_SaveLoop
	call	SEQ_LAYER_SaveEvnt
	incf	SEQ_EVNT_NUMBER, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER, BANKED
	rgoto SEQ_RECORD_NotifyEvent_SaveLoop

	;; step mode: increment step number
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, SEQ_RECORD_NotifyEvent_NotStep
SEQ_RECORD_NotifyEvent_IncStep
	rcall	SEQ_RECORD_StepInc
SEQ_RECORD_NotifyEvent_NotStep

	;; if autostart enabled and sequencer not running: request start
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_RECORD_NotifyEvent_NoStart
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, SEQ_RECORD_NotifyEvent_NoStart
	bsf	SEQ_REQ, SEQ_REQ_START, BANKED
SEQ_RECORD_NotifyEvent_NoStart

	;; send stored events
	;; start event loop (each track can play up to 4 MIDI events)
	clrf	SEQ_EVNT_NUMBER_CTR, BANKED
SEQ_RECORD_NotifyEvent_SLoop
	;; get event order and store in in SEQ_EVNT_NUMBER
	movf	SEQ_EVNT_NUMBER_CTR, W, BANKED
	call	SEQ_LAYER_GetEvntPrio
	movwf	SEQ_EVNT_NUMBER, BANKED

	;; play off event (if there is still one in the queue)
	call	SEQ_CORE_Hlp_PlayOffEvnt

	;; don't play if return value is 15
	xorlw	15
	skpnz
	rgoto	SEQ_RECORD_NotifyEvent_SLoopNxt

	;; get MIDI event depending on SEQ_EVNT_NUMBER
	call	SEQ_LAYER_GetEvnt
	;; events now in SEQ_EVNT[012L]

	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_RECORD_NotifyEvent_SLoopNxt
	movwf	MIDI_EVNT0

	movf	SEQ_EVNT1, W, BANKED
	bz	SEQ_RECORD_NotifyEvent_SLoopNxt
	movwf	MIDI_EVNT1

	movff	SEQ_EVNT2, MIDI_EVNT_VALUE

	call	SEQ_FSR_TrkChn
	swapf	INDF1, W
	andlw	0x07
	movwf	MIDI_EVNT_PORT

	;; only send if received MIDI event matches with send event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, SEQ_RECORD_NotifyEvent_SLoop_CC
SEQ_RECORD_NotifyEvent_SLoop_N
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xe0
	xorlw	0x80
	bnz	SEQ_RECORD_NotifyEvent_SLoopNxt	
	rgoto	SEQ_RECORD_NotifyEvent_SLoopSnd

SEQ_RECORD_NotifyEvent_SLoop_CC
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bnz	SEQ_RECORD_NotifyEvent_SLoopNxt	
	;; 	rgoto	SEQ_RECORD_NotifyEvent_SLoopSnd

SEQ_RECORD_NotifyEvent_SLoopSnd
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE

	;; if note (no CC): store OFF event
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bz	SEQ_RECORD_NotifyEvent_SLoopNoO
SEQ_RECORD_NotifyEvent_SLoopOff
	call	SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr
	movff	MIDI_EVNT_PORT, INDF1
	call	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	movff	SEQ_EVNT0, INDF1
	call	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	movff	SEQ_EVNT1, INDF1

	;; set gatelength to maximum (we are waiting for Note Off)
	movlw	SEQ_TRKVARQUEUEL_Lx
	setf	PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	setf	PLUSW2
SEQ_RECORD_NotifyEvent_SLoopNoO

SEQ_RECORD_NotifyEvent_SLoopNxt
	;; loop 4 times to play all 4 MIDI events of a track
	SET_BSR	SEQ_BASE
	incf	SEQ_EVNT_NUMBER_CTR, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER_CTR, BANKED
	rgoto SEQ_RECORD_NotifyEvent_SLoop


SEQ_RECORD_NotifyEvent_Skip
	;; update current step and select screen half
	rcall	SEQ_RECORD_StepUpdate

	;; print message
	call	SEQ_MSG_Start

SEQ_RECORD_NotifyEvent_End
	return


;; --------------------------------------------------------------------------
;;  This function increments the record step
;; --------------------------------------------------------------------------
SEQ_RECORD_StepInc
	SET_BSR	SEQ_BASE

	;; increment step number
	incf	SEQ_RECORD_STEP, F, BANKED
	;; wrap on overrun
	call	SEQ_FSR_TrkLen
	movf	INDF1, W
	cpfsgt	SEQ_RECORD_STEP, BANKED
	rgoto SEQ_RECORD_StepInc_NoOv
	clrf	SEQ_RECORD_STEP, BANKED
SEQ_RECORD_StepInc_NoOv
	rgoto	SEQ_RECORD_StepUpdate

;; --------------------------------------------------------------------------
;;  This function updates the active step value + the 1-16/17-32 screen
;; --------------------------------------------------------------------------
SEQ_RECORD_StepUpdate
	;; set new current step
	movff	SEQ_RECORD_STEP, SEQ_CURRENT_STEP

	;; select step 1-16/17-32
	SET_BSR	SEQ_BASE
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	btfsc	SEQ_CURRENT_STEP, 4, BANKED
	bsf	SEQ_SELECTED_STEPS, 0, BANKED

	return

