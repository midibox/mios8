; $Id: seq_midi.inc 97 2008-02-17 18:32:55Z tk $
;
; MIDIboxSEQ
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
;;  This function is called by SEQ_ROUTER_In when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_RECORD_NotifyEvent
	SET_BSR	SEQ_BASE

	;; copy MIOS_PARAMETER[123] to SEQ_EVNT[123], the values are modified
	movff	MIOS_PARAMETER1, SEQ_EVNT0
	movff	MIOS_PARAMETER2, SEQ_EVNT1
	movff	MIOS_PARAMETER3, SEQ_EVNT2

	;; the length is 0x11 (75%) by default
	movlw	0x11
	movwf	SEQ_EVNTL, BANKED

	;; continue if note on event has been received
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bz	SEQ_RECORD_NotifyEvent_Note
	;; and if note off event has been received (force velocity to 0x00 in this case)
	xorlw	0x80 ^ 0x90
	bnz	SEQ_RECORD_NotifyEvent_End
	clrf	SEQ_EVNT2, BANKED
SEQ_RECORD_NotifyEvent_Note

	;; go through list of 16 tracks
	;; search for matching MIDI channel and note definition
	clrf	SEQ_EVNTT, BANKED
SEQ_RECORD_NotifyEvent_Loop
	movf	SEQ_EVNTT, W, BANKED		; check if MIDI port enabled
	call	SEQ_TRK_GetMIDIPort
	bz	SEQ_RECORD_NotifyEvent_Loop_Next

	call	SEQ_FSR_TrkChn			; check for matching MIDI channel
	movf	INDF1, W
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	SEQ_RECORD_NotifyEvent_Loop_Next
	
	call	SEQ_FSR_TrkNote			; check for matching MIDI note
	movf	INDF1, W
	xorwf	MIOS_PARAMETER2, W
	bnz	SEQ_RECORD_NotifyEvent_Loop_Next

	;; found matching event
	rcall	SEQ_RECORD_It

SEQ_RECORD_NotifyEvent_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, SEQ_RECORD_NotifyEvent_Loop


SEQ_RECORD_NotifyEvent_Skip
	;; update current step and select screen half
	rcall	SEQ_RECORD_StepUpdate

SEQ_RECORD_NotifyEvent_End
	return


;; --------------------------------------------------------------------------
;;  This function updates the active step value + the 1-16/17-32 screen
;; --------------------------------------------------------------------------
SEQ_RECORD_StepUpdate
	;; set new current step
	movff	SEQ_RECORD_STEP, SEQ_CURRENT_STEP
	
	;; select step 1-16/17-32/33-48/49-64
	SET_BSR	SEQ_BASE
	swapf	SEQ_CURRENT_STEP, W, BANKED
	movwf	SEQ_SELECTED_STEPS, BANKED

	return

;; --------------------------------------------------------------------------
;;  This function records the played event in current step
;;  IN: SEQ_EVNT[012] and SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_RECORD_It
	;; don't record note off events, but deactivate note
	movf	SEQ_EVNT2, W, BANKED
	bnz	SEQ_RECORD_It_NoteOn
SEQ_RECORD_It_NoteOff
	;; release active note
	call	SEQ_FSR_TrkVarState2
	bcf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE
SEQ_RECORD_ItChk_NOffS
	return

SEQ_RECORD_It_NoteOn
	;; new step if sequencer is running
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_RECORD_It_NewStep
	;; clear step number if autostart enabled
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, SEQ_RECORD_It_NoNewStp
SEQ_RECORD_It_ClrStep
	clrf	SEQ_RECORD_STEP, BANKED
	rgoto	SEQ_RECORD_It_ClrStpC
SEQ_RECORD_It_NewStep
	call	SEQ_FSR_TrkVarStep
	movff	INDF1, SEQ_RECORD_STEP
SEQ_RECORD_It_NoNewStp
SEQ_RECORD_It_ClrStpC

	;; transfer step number to SEQ_EVNTS
	movff	SEQ_RECORD_STEP, SEQ_EVNTS

	;; clear length counter
	clrf	SEQ_RECORD_LENGTH_CTR, BANKED

	;; notify that note is held
	call	SEQ_FSR_TrkVarState2
	bsf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE

	;; save the event in current step
	call	SEQ_LAYER_SaveEvnt

	;; if autostart enabled and sequencer not running: request start
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_RECORD_It_NoStart
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, SEQ_RECORD_It_NoStart
	bsf	SEQ_REQ, SEQ_REQ_START, BANKED
SEQ_RECORD_It_NoStart

	return
