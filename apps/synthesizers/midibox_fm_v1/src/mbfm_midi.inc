; $Id$
;
; MIDIbox FM
; MIDI Interface part
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_MIDI_NotifyReceivedEvent when a complete 
;;  MIDI event has been received
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o velocity in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_NotifyReceivedEvent
	SET_BSR	MBFM_BASE			; prepare BSR for MBFM register access

	;; store channel in MBFM_CURRENT_CHANNEL
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movff	WREG, MBFM_CURRENT_CHANNEL

	;; store MIDI event byte 2 and 3 in MBFM_MIDI_PARAMETER[12]
	movff	MIOS_PARAMETER2, MBFM_MIDI_PARAMETER1
	movff	MIOS_PARAMETER3, MBFM_MIDI_PARAMETER2

	;; disable IRQs to avoid inconsistencies and to allow the use of FSR2
	IRQ_DISABLE

	;; call MIDI event handler depending on event type
	rcall	MBFM_MIDI_NotifyReceivedEventJmp
	
	;; enable IRQs again
	IRQ_ENABLE

	return

MBFM_MIDI_NotifyReceivedEventJmp
	;; branch to appr. MBFM routine depending on received event
	swapf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MBFM_MIDI_NoteOff
	rgoto	MBFM_MIDI_NoteOn
	rgoto	MBFM_MIDI_AfterTouch
	rgoto	MBFM_MIDI_CC
	rgoto	MBFM_MIDI_ProgramChange
	rgoto	MBFM_MIDI_PolyAfterTouch
	rgoto	MBFM_MIDI_PitchBender
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note On event to the synthesizer
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o velocity in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_NoteOn
	movf	MBFM_MIDI_PARAMETER2, W, BANKED	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	MBFM_MIDI_NoteOff

	;; drum special
	movf	MBFM_DRUM_MIDI_CHANNEL, W, BANKED
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_NoteOnNoDrum
MBFM_MIDI_NoteOnDrum
	call	MBFM_MIDI_DrumOn	
MBFM_MIDI_NoteOnNoDrum


	clrf	MBFM_CURRENT_INSTRUMENT, BANKED		; loop counter
	lfsr	FSR0, MBFM_I1_BASE
MBFM_MIDI_NoteOn_Loop
	;; check if MIDI channel matches
	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_NoteOn_Loop_Next

	;; get split points
	movlw	MBFM_Ix_SPLIT_LOWER
	movff	PLUSW0, PRODL
	movlw	MBFM_Ix_SPLIT_UPPER
	movff	PLUSW0, PRODH

	;; don't split if both are zero
	movf	PRODL, W
	iorwf	PRODH, W
	bz	MBFM_MIDI_NoteOn_Ok

	;; check lower split point
	movf	PRODL, W
	cpfslt	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_NoteOn_Loop_LOk
	rgoto	MBFM_MIDI_NoteOn_Loop_Next
MBFM_MIDI_NoteOn_Loop_LOk

	;; check upper split point
	movf	PRODH, W
	cpfsgt	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_NoteOn_UOk
	rgoto	MBFM_MIDI_NoteOn_Loop_Next
MBFM_MIDI_NoteOn_UOk
	
MBFM_MIDI_NoteOn_Ok
	;; push note to stack (pointer to stack in FSR2)
	movf	FSR0L, W
	addlw	MBFM_Ix_NOTE_STACK_0
	movwf	FSR2L
	movf	FSR0H, W
	skpnc
	incf	FSR0H, W
	movwf	FSR2H
	rcall	MBFM_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, MBFM_MIDI_NoteOn_Loop_Next	; exit if note already in stack
	movlw	MBFM_Ix_CTRL1				; switch off gate if not in legato mode
	RCALL_IFCLR PLUSW0, MBFM_CTRL1_LEGATO, ACCESS, MBFM_MIDI_Hlp_GateOff
	rcall	MBFM_MIDI_Hlp_NoteOn			; call note-on handler
MBFM_MIDI_NoteOn_Loop_Next
	movlw	MBFM_Ix_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED		; loop until last voice (7) reached
	movlw	MBFM_Ix_NUM-1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_NoteOn_Loop

	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note Off event to the synthesizer
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o velocity in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_NoteOff
	;; drum special
	movf	MBFM_DRUM_MIDI_CHANNEL, W, BANKED
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_NoteOffNoDrum
MBFM_MIDI_NoteOffDrum
	call	MBFM_MIDI_DrumOff
MBFM_MIDI_NoteOffNoDrum

	clrf	MBFM_CURRENT_INSTRUMENT, BANKED		; loop counter
	lfsr	FSR0, MBFM_I1_BASE
MBFM_MIDI_NoteOff_Loop
	;; check if MIDI channel matches
	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_NoteOff_Loop_Next

	;; pop note from stack (pointer to stack in FSR2)
	movf	FSR0L, W
	addlw	MBFM_Ix_NOTE_STACK_0
	movwf	FSR2L
	movf	FSR0H, W
	skpnc
	incf	FSR0H, W
	movwf	FSR2H
	movff	INDF2, TMP3	; save current #0 entry in TMP3 for later use
	rcall	MBFM_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, MBFM_MIDI_NoteOff_Loop_Next
	movf	TMP3, W		; restore note
	rcall	MBFM_MIDI_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, MBFM_MIDI_Hlp_NoteOn
MBFM_MIDI_NoteOff_Loop_Next
	movlw	MBFM_Ix_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED		; loop until last voice (7) reached
	movlw	MBFM_Ix_NUM-1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_NoteOff_Loop

	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a PitchBender event to the synthesizer
;;  Input:
;;     o low byte in MBFM_MIDI_PARAMETER1
;;     o high byte in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_PitchBender
	;; convert pitch bender to signed 8-bit value -> MBFM_MIDI_PARAMETER1
	rlf	MBFM_MIDI_PARAMETER2, W, BANKED
	andlw	0xfe
	xorlw	0x80
	movwf	MBFM_MIDI_PARAMETER1, BANKED

	;; go through all instruments
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
	lfsr	FSR0, MBFM_I1_BASE
MBFM_MIDI_PitchBender_LoopO
	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_PitchBender_LoopO_Next

	;; copy pitchbender value
	movlw	MBFM_Ix_PITCHBENDER
	movff	MBFM_MIDI_PARAMETER1, PLUSW0

	;; go through all voices which are assigned to this instrument
	lfsr	FSR1, MBFM_V1_BASE
	clrf	MBFM_CURRENT_VOICE, BANKED
MBFM_MIDI_PitchBender_LoopI
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	cpfseq	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_PitchBender_LoopI_Next

	;; request note update
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_NOTE_UPDATE_REQ

MBFM_MIDI_PitchBender_LoopI_Next
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	MBFM_CURRENT_VOICE, F, BANKED
	movlw	MBFM_Vx_NUM - 1
	cpfsgt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_MIDI_PitchBender_LoopI

MBFM_MIDI_PitchBender_LoopO_Next
	movlw	MBFM_Ix_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM - 1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_PitchBender_LoopO

MBFM_MIDI_PitchBender_End
	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Controller event to the synthesizer
;;  Input:
;;     o CC number in MBFM_MIDI_PARAMETER1
;;     o CC value in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_CC
	;; drum special
	movf	MBFM_DRUM_MIDI_CHANNEL, W, BANKED
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_CCNoDrum
	;; ensure that CC < 0x70
	movlw	0x70
	cpfslt	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_CCNoDrum
MBFM_MIDI_CCDrum
	;; set CC parameter
	movff	MBFM_MIDI_PARAMETER2, MIOS_PARAMETER1
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	iorlw	0x80
	call	MBFM_PARIN_Set
MBFM_MIDI_CCNoDrum

	;; go through all instruments
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
MBFM_MIDI_CC_Loop
	rcall	MBFM_MIDI_Hlp_GetINSBase_FSR0

	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_CC_Loop_Next

	;; special treatment on bank change (CC#00)
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	bnz	MBFM_MIDI_CC_Loop_NotCC00
	
MBFM_MIDI_CC_Loop_CC00
	;; don't change if bank number >= 8
	movlw	0x08
	cpfslt	MBFM_MIDI_PARAMETER2, BANKED
	rgoto MBFM_MIDI_CC_Loop_Next

	;; set new bank number and initialize instrument
	movlw	MBFM_Ix_BANK
	movff	MBFM_MIDI_PARAMETER2, PLUSW0
	call	MBFM_INSTR_Init
	rgoto	MBFM_MIDI_CC_Loop_Next

MBFM_MIDI_CC_Loop_NotCC00
	;; set CC parameter
	movff	MBFM_MIDI_PARAMETER2, MIOS_PARAMETER1
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	call	MBFM_PARIN_Set

MBFM_MIDI_CC_Loop_Next
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM - 1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_CC_Loop

MBFM_MIDI_CC_End
	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Program Change event to the synthesizer
;;  Input:
;;     o program number in MBFM_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
MBFM_MIDI_ProgramChange

	;; drum special
	movf	MBFM_DRUM_MIDI_CHANNEL, W, BANKED
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_ProgramChangeNoDrum
MBFM_MIDI_ProgramChangeDrum
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	andlw	0x0f
	movwf	MBFM_DRUMSET
	swapf	MBFM_MIDI_PARAMETER1, W, BANKED
	andlw	0x07
	movwf	MBFM_DBANK
	call	MBFM_DRUM_Init
MBFM_MIDI_ProgramChangeNoDrum

	;; go through all instruments
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
MBFM_MIDI_ProgramChangeLoop
	lfsr	FSR0, MBFM_I1_BASE
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	
	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_ProgramChangeLoop_Next

	;; set new patch number
	movlw	MBFM_Ix_PATCH
	movff	MBFM_MIDI_PARAMETER1, PLUSW0

	;; initialize instrument
	call	MBFM_INSTR_Init

MBFM_MIDI_ProgramChangeLoop_Next
	SET_BSR	MBFM_BASE
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM-1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_ProgramChangeLoop

	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Poly Aftertouch event to the synthesizer
;;  Input:
;;     o aftertouch value in MBFM_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
MBFM_MIDI_PolyAfterTouch
	movff	MBFM_MIDI_PARAMETER1, MBFM_MIDI_PARAMETER2
	rgoto	MBFM_MIDI_AfterTouch

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Aftertouch event to the synthesizer
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o aftertouch value in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_AfterTouch
	;; go through all instruments
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
MBFM_MIDI_AfterTouch_Loop
	rcall	MBFM_MIDI_Hlp_GetINSBase_FSR0

	movlw	MBFM_Ix_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	MBFM_CURRENT_CHANNEL, BANKED
	rgoto MBFM_MIDI_AfterTouch_Loop_Next

	movlw	MBFM_Ix_AFTERTOUCH
	movff	MBFM_MIDI_PARAMETER2, PLUSW0

	;; update assigned parameter
	call	MBFM_PARIN_Cmd_AFTERTOUCH_MIDI

MBFM_MIDI_AfterTouch_Loop_Next
	movlw	MBFM_Ix_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	movlw	MBFM_Ix_NUM - 1
	cpfsgt	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_AfterTouch_Loop

MBFM_MIDI_AfterTouch_End
	return



;; --------------------------------------------------------------------------
;;  help routines
;; --------------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Push a note into the stack
	;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_PushNote
	clrf	TMP1
	;; do nothing if note is already stored in note stack
MBFM_MIDI_Hlp_PushNote_CheckLoop
	movf	TMP1, W
	movf	PLUSW2, W
	xorwf	MBFM_MIDI_PARAMETER1, W, BANKED
	skpnz
	rgoto	MBFM_MIDI_Hlp_PushNote_Failed       ; leave note routine if note already stored
	incf	TMP1, F
	movlw	MBFM_Ix_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto MBFM_MIDI_Hlp_PushNote_CheckLoop
	
	;; shift right note stack 
	movlw	(MBFM_Ix_NOTE_STACK_LEN-2)
	movwf	TMP1
MBFM_MIDI_Hlp_PushNote_ShiftLoop
	movf	TMP1, W
	movff	PLUSW2, TMP2
	incf	TMP1, W
	movff	TMP2, PLUSW2
	decf	TMP1, F
	incf	TMP1, W
	bnz	MBFM_MIDI_Hlp_PushNote_ShiftLoop

	;; store new note at offset 0
	movff	MBFM_MIDI_PARAMETER1, INDF2

	retlw	0x00		; return 0x00 as error status

MBFM_MIDI_Hlp_PushNote_Failed
	retlw	0x01		; return 0x01 as error status

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Pop a note from the stack
	;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_PopNote
	; search for note entry with the same number, erase it and push the entries behind
	clrf	TMP1
MBFM_MIDI_Hlp_PopNote_SearchLoop
	movf	TMP1, W
	movf	PLUSW2, W
	xorwf	MBFM_MIDI_PARAMETER1, W, BANKED
	bz	MBFM_MIDI_Hlp_PopNote_Found
	incf	TMP1, F
	movlw	MBFM_Ix_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto MBFM_MIDI_Hlp_PopNote_SearchLoop
	rgoto	MBFM_MIDI_Hlp_PopNote_Failed
MBFM_MIDI_Hlp_PopNote_Found

	;; push the entries behind the found entry
MBFM_MIDI_Hlp_PopNote_ShiftLoop
	incf	TMP1, W
	movff	PLUSW2, TMP2
	movf	TMP1, W
	movff	TMP2, PLUSW2
	incf	TMP1, F
	movlw	MBFM_Ix_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto MBFM_MIDI_Hlp_PopNote_ShiftLoop
	;; clear the last entry
	movlw	MBFM_Ix_NOTE_STACK_LEN-1
	clrf	PLUSW2
	retlw	0x00		; return with 0x00: note deleted from stack

MBFM_MIDI_Hlp_PopNote_Failed
	retlw	0x01		; return with 0x01: note not found in stack

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Note On help function
	;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_NoteOn
	;; save current frequency of last voice in TMP[12]
	;; (for proper portamento in poly mode)
	movlw	MBFM_Ix_LAST_VOICE
	movf	PLUSW0, W
	mullw	MBFM_Vx_RECORD_LEN
	lfsr	FSR1, MBFM_V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F
	movlw	MBFM_Vx_FRQ_L
	movff	PLUSW1, TMP1
	movlw	MBFM_Vx_FRQ_H
	movff	PLUSW1, TMP2

	rcall	MBFM_MIDI_Hlp_NoteOn2
	movlw	MBFM_Vx_STAT
	bcf	PLUSW1, MBFM_STAT_UNISONO_2ND_VOICE

	;; unisono feature only possible in poly mode
	movlw	MBFM_Ix_CTRL1
	BRA_IFCLR PLUSW0, MBFM_CTRL1_POLY, ACCESS, MBFM_MIDI_Hlp_NoteOn_NoUnisono
	movlw	MBFM_Ix_UNISONO
	movf	PLUSW0, W
	bz	MBFM_MIDI_Hlp_NoteOn_NoUnisono
MBFM_MIDI_Hlp_NoteOn_Unisono
	rcall	MBFM_MIDI_Hlp_NoteOn_Poly
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_UNISONO_2ND_VOICE	
MBFM_MIDI_Hlp_NoteOn_NoUnisono
	return



MBFM_MIDI_Hlp_NoteOn2
	;; in poly mode we prefer to get a voice which is free or was played the longest time
	movlw	MBFM_Ix_CTRL1
	BRA_IFCLR PLUSW0, MBFM_CTRL1_POLY, ACCESS, MBFM_MIDI_Hlp_NoteOn_NotPoly
MBFM_MIDI_Hlp_NoteOn_Poly
	call	MBFM_VOICE_Get
	rgoto	MBFM_MIDI_Hlp_NoteOn_Cont
MBFM_MIDI_Hlp_NoteOn_NotPoly
	;; in mono/legato mode we prefer the last played voice (if still available - if not it doesn't matter)
	movlw	MBFM_Ix_LAST_VOICE
	movf	PLUSW0, W
	call	MBFM_VOICE_GetLast
	;; 	rgoto	MBFM_MIDI_Hlp_NoteOn_Cont
MBFM_MIDI_Hlp_NoteOn_Cont
	;; pointer to voice in FSR1
	;; voice number in MBFM_CURRENT_VOICE

	;; save voice number in instrument record
	movlw	MBFM_Ix_LAST_VOICE
	movff	MBFM_CURRENT_VOICE, PLUSW0

	;; write back current frequency
	movlw	MBFM_Vx_FRQ_L
	movff	TMP1, PLUSW1
	movlw	MBFM_Vx_FRQ_H
	movff	TMP2, PLUSW1

	;; refresh voice if instrument has been changed
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	xorwf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	bz	MBFM_MIDI_Hlp_NoteOn_NoRefresh
MBFM_MIDI_Hlp_NoteOn_Refresh
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_VOICE_REFRESH
MBFM_MIDI_Hlp_NoteOn_NoRefresh

	;; save instrument number in MBFM_Vx_ASSIGNED_INSTR (set bit #7!)
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movff	MBFM_CURRENT_INSTRUMENT, PLUSW1

	;; save note into MBFM_Vx_NOTE
	movlw	MBFM_Vx_NOTE
	movff	INDF2, PLUSW1

	;; save initial velocity in Ix_VELOCITY
	movlw	MBFM_Ix_VELOCITY
	movff	MBFM_MIDI_PARAMETER2, PLUSW0

	;; update assigned velocity parameter
	movff	FSR1L, TMP4	; save FSR1 (will be overwritten)
	movff	FSR1H, TMP5
	call	MBFM_PARIN_Cmd_VELOCITY_MIDI
	movff	TMP4, FSR1L	; restore FSR1
	movff	TMP5, FSR1H
	rcall	MBFM_MIDI_Hlp_GetINSBase_FSR0	; restore FSR0

	;; for volume:
	;; branch depending on velocity mode
	movlw	MBFM_Ix_CTRLE
	BRA_IFSET PLUSW0, MBFM_CTRLE_VEL_MODE1, ACCESS, MBFM_MIDI_Hlp_NoteOn_Vel23
MBFM_MIDI_Hlp_NoteOn_Vel01
	BRA_IFSET PLUSW0, MBFM_CTRLE_VEL_MODE0, ACCESS, MBFM_MIDI_Hlp_NoteOn_Vel1
MBFM_MIDI_Hlp_NoteOn_Vel0
	;; flat velocity
	movlw	0x40
	movwf	TMP1
	rgoto	MBFM_MIDI_Hlp_NoteOn_Vel_Cont

MBFM_MIDI_Hlp_NoteOn_Vel1
	;; hard velocity
	clrc
	rrf	MBFM_MIDI_PARAMETER2, W, BANKED
	addlw	0x20
	movwf	TMP1
	rgoto	MBFM_MIDI_Hlp_NoteOn_Vel_Cont

MBFM_MIDI_Hlp_NoteOn_Vel23
	;; soft velocity
	movff	MBFM_MIDI_PARAMETER2, TMP1
	;; 	rgoto	MBFM_MIDI_Hlp_NoteOn_Vel_Cont

MBFM_MIDI_Hlp_NoteOn_Vel_Cont
	movlw	MBFM_Vx_VELOCITY
	movff	TMP1, PLUSW1

	;; update voice volume
	rcall	MBFM_MIDI_Hlp_Vol

	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_NOTE_UPDATE_REQ

	;; sus-key activated?
	movlw	MBFM_Ix_CTRL1
	BRA_IFCLR PLUSW0, MBFM_CTRL1_SUS_KEY, ACCESS, MBFM_MIDI_Hlp_NoteOn_SusKeyPor
MBFM_MIDI_Hlp_NoteOn_SusKey
	;; special portamento gate: active, when more than one note is played
	movlw	MBFM_Vx_STAT
	bcf	PLUSW1, MBFM_STAT_PORTA_ACTIVE

	movlw	0x01
	movf	PLUSW2, W
	bz	MBFM_MIDI_Hlp_NoteOn_SusKeyNoPor
MBFM_MIDI_Hlp_NoteOn_SusKeyPor
	movlw	MBFM_Ix_PORTA_RATE
	movf	PLUSW0, W
	bz	MBFM_MIDI_Hlp_NoteOn_SusKeyNoPor
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_PORTA_ACTIVE
MBFM_MIDI_Hlp_NoteOn_SusKeyNoPor

	;; skip the rest if legato mode and current note is first note
	movlw	MBFM_Ix_CTRL1
	BRA_IFCLR PLUSW0, MBFM_CTRL1_LEGATO, ACCESS, MBFM_MIDI_Hlp_NoteOn_TrgrGateNL
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	cpfseq	INDF2, ACCESS
	rgoto MBFM_MIDI_Hlp_NoteOn_TrgrGateLSk
	movlw	0x01
	movf	PLUSW2, W
	bnz	MBFM_MIDI_Hlp_NoteOn_TrgrGateLSk
MBFM_MIDI_Hlp_NoteOn_TrgrGateNL

	;; request gate bit
	rcall	MBFM_MIDI_Hlp_GateOn
MBFM_MIDI_Hlp_NoteOn_TrgrGateLSk
	return

	;; ------------------------------------------------------------------
	;; Note Off help Function
	;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!
	movwf	TABLAT

	;; always gate off and no new note on if in poly mode
	movlw	MBFM_Ix_CTRL1
	BRA_IFSET PLUSW0, MBFM_CTRL1_POLY, ACCESS, MBFM_MIDI_Hlp_NoteOff_GOff

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	movlw	MBFM_Ix_CTRL1
	BRA_IFSET PLUSW0, MBFM_CTRL1_LEGATO, ACCESS, MBFM_MIDI_Hlp_NoteOff_NoGOff
	movf	TABLAT, W
	cpfseq	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_Hlp_NoteOff_End
	rcall	MBFM_MIDI_Hlp_GateOff
MBFM_MIDI_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if already note available, play new note in NoteOn Section
	movf	INDF2, W
	skpz
	retlw	0x01		; return, request Note On!

MBFM_MIDI_Hlp_NoteOff_GOff
	;; else request gate clear bit
	rcall	MBFM_MIDI_Hlp_GateOff
MBFM_MIDI_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!

;; ------------------------------------------------------------------
;; Gate On help function
;; IN: pointer to free voice in FSR1
;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_GateOn
	movlw	MBFM_Vx_STAT
	bsf	PLUSW1, MBFM_STAT_GATE_CLR_REQ
	bsf	PLUSW1, MBFM_STAT_GATE_SET_REQ
	bsf	PLUSW1, MBFM_STAT_VOICE_ACTIVE

	;; clear WT step flag
	movlw	MBFM_Ix_WT_STATE
	bcf	PLUSW0, MBFM_WT_STATE_STEP_DONE

	;; sync WT if not in notestep/freerunning mode
	movlw	MBFM_Ix_CTRL2
	btfsc	PLUSW0, MBFM_CTRL2_SYNC_WT_NOTESTEP
	return
	btfsc	PLUSW0, MBFM_CTRL2_SYNC_WT_FREERUNNING
	return
	movlw	MBFM_Ix_WT_STATE
	bsf	PLUSW0, MBFM_WT_STATE_RESET
	return

;; ------------------------------------------------------------------
;; Gate Off help function
;; IN: instrument in MBFM_CURRENT_INSTRUMENT
;;     note in MBFM_MIDI_PARAMETER1
;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_GateOff
	;; go through all voices which are assigned to the current instrument and note
	lfsr	FSR1, MBFM_V1_BASE
	clrf	MBFM_CURRENT_VOICE, BANKED
MBFM_MIDI_Hlp_GateOff_Loop
	movlw	MBFM_Vx_STAT
	BRA_IFCLR PLUSW1, MBFM_STAT_VOICE_ACTIVE, ACCESS, MBFM_MIDI_Hlp_GateOff_Loop_Next
	movlw	MBFM_Vx_ASSIGNED_INSTR
	movf	PLUSW1, W
	cpfseq	MBFM_CURRENT_INSTRUMENT, BANKED
	rgoto MBFM_MIDI_Hlp_GateOff_Loop_Next
	movlw	MBFM_Vx_NOTE
	movf	PLUSW1, W
	cpfseq	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_Hlp_GateOff_Loop_Next

	;; release voice
	call	MBFM_VOICE_Release

	;; request gate off
	movlw	MBFM_Vx_STAT
	bcf	PLUSW1, MBFM_STAT_GATE_SET_REQ
	bsf	PLUSW1, MBFM_STAT_GATE_CLR_REQ
	bcf	PLUSW1, MBFM_STAT_VOICE_ACTIVE

MBFM_MIDI_Hlp_GateOff_Loop_Next
	movlw	MBFM_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	MBFM_CURRENT_VOICE, F, BANKED
	movlw	MBFM_Vx_NUM - 1
	cpfsgt	MBFM_CURRENT_VOICE, BANKED
	rgoto MBFM_MIDI_Hlp_GateOff_Loop

	return	


;; ------------------------------------------------------------------
;; Volume help function - updates the MBFM_Vx_VOL registers
;; IN: pointer to instrument in FSR0
;;     pointer to voice in FSR1
;; USES: PROD[LH] and TABLAT as temporary registers
;; ------------------------------------------------------------------
MBFM_MIDI_Hlp_Vol
	;; get volume addend derived from (velocity * volume) / 128
	movlw	MBFM_Ix_VOLUME
	movff	PLUSW0, TABLAT
	movlw	MBFM_Vx_VELOCITY
	movf	PLUSW1, W
	mulwf	TABLAT, ACCESS
	rlf	PRODL, W
	rlf	PRODH, W
	andlw	0x7f
	call	MBFM_VOL_TABLE_Get	; volume addend in MIOS_PARAMETER1

	;; always add to OP4 TL
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP4
	rcall	MBFM_MIDI_Hlp_Vol_Add

	;; branch depending on connections
	movlw	MBFM_Ix_OP12_CH_FB_CNT
	BRA_IFSET PLUSW0, 0, ACCESS, MBFM_MIDI_Hlp_Vol_C13
MBFM_MIDI_Hlp_Vol_C02
	addlw	0x01
	BRA_IFSET PLUSW0, 0, ACCESS, MBFM_MIDI_Hlp_Vol_C2
MBFM_MIDI_Hlp_Vol_C0
	;; only OP4 at the end
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP1
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP2
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP3
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rgoto	MBFM_MIDI_Hlp_Vol_End

MBFM_MIDI_Hlp_Vol_C2
	;; OP2 and OP4 at output
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP1
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP2
	rcall	MBFM_MIDI_Hlp_Vol_Add
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP3
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rgoto	MBFM_MIDI_Hlp_Vol_End

MBFM_MIDI_Hlp_Vol_C13
	addlw	0x01
	BRA_IFSET PLUSW0, 0, ACCESS, MBFM_MIDI_Hlp_Vol_C3
MBFM_MIDI_Hlp_Vol_C1
	;; OP1 and OP4 at output
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP1
	rcall	MBFM_MIDI_Hlp_Vol_Add
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP2
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP3
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rgoto	MBFM_MIDI_Hlp_Vol_End

MBFM_MIDI_Hlp_Vol_C3
	;; OP1, OP3 and OP4 at output
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP1
	rcall	MBFM_MIDI_Hlp_Vol_Add
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP2
	rcall	MBFM_MIDI_Hlp_Vol_Copy
	rcall	MBFM_MIDI_Hlp_Vol_PreloadOP3
	rcall	MBFM_MIDI_Hlp_Vol_Add
	;; 	rgoto	MBFM_MIDI_Hlp_Vol_End

MBFM_MIDI_Hlp_Vol_End

	return


;; ---
MBFM_MIDI_Hlp_Vol_Add
	movf	PRODL, W	; MBFM_Ix_OPx_KSL_TL
	comf	PLUSW0, W
	andlw	0x3f
	addwf	MIOS_PARAMETER1, W
	movwf	TABLAT

	;; saturate
	;; 	movf	TABLAT, W
	andlw	0xc0
	bz	MBFM_MIDI_Hlp_Vol_Add_NoSat
MBFM_MIDI_Hlp_Vol_Add_Sat
	movlw	0x3f
	btfsc	MIOS_PARAMETER1, 7
	movlw 0x00
	movwf	TABLAT
MBFM_MIDI_Hlp_Vol_Add_NoSat

	movlw	0x3f
	xorwf	TABLAT, F

	;; copy to voice volume register
	movf	PRODH, W	; MBFM_Vx_OPx_VOL
	movff	TABLAT, PLUSW1

	return


MBFM_MIDI_Hlp_Vol_Copy
	movf	PRODL, W
	movff	PLUSW0, TABLAT
	movlw	0x3f
	andwf	TABLAT, W
	movf	PRODH, W
	movff	TABLAT, PLUSW1
	return

MBFM_MIDI_Hlp_Vol_PreloadOP1
	movlw	MBFM_Ix_OP1_KSL_TL
	movwf	PRODL
	movlw	MBFM_Vx_OP1_VOL
	movwf	PRODH
	return

MBFM_MIDI_Hlp_Vol_PreloadOP2
	movlw	MBFM_Ix_OP2_KSL_TL
	movwf	PRODL
	movlw	MBFM_Vx_OP2_VOL
	movwf	PRODH
	return

MBFM_MIDI_Hlp_Vol_PreloadOP3
	movlw	MBFM_Ix_OP3_KSL_TL
	movwf	PRODL
	movlw	MBFM_Vx_OP3_VOL
	movwf	PRODH
	return

MBFM_MIDI_Hlp_Vol_PreloadOP4
	movlw	MBFM_Ix_OP4_KSL_TL
	movwf	PRODL
	movlw	MBFM_Vx_OP4_VOL
	movwf	PRODH
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Drum On event to the synthesizer
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o velocity in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_DrumOn
	;; get selected drums
	rcall	MBFM_MIDI_Drum_GetSel
	;; result in TMP1:
	;; TMP1[0]: BD
	;; TMP1[1]: HH Open
	;; TMP1[2]: HH Close
	;; TMP1[3]: SD
	;; TMP1[4]: Tom
	;; TMP1[5]: Cymbal

	BRA_IFCLR TMP1, 0, ACCESS, MBFM_MIDI_DrumOn_NotBD
MBFM_MIDI_DrumOn_BD
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	subwf	MBFM_DR_BD_SPLIT_LOWER, W, BANKED
	addwf	MBFM_DR_BD_FRQ, W, BANKED
	call	MBFM_DRUM_SetBDFrq

	movff	MBFM_DR_BD_M_KSL_TL, PRODL
	movlw	0x50
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol
	
	movff	MBFM_DR_BD_C_KSL_TL, PRODL
	movlw	0x53
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol
	
	movlw	0x10
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotBD

	BRA_IFCLR TMP1, 1, ACCESS, MBFM_MIDI_DrumOn_NotHHO
MBFM_MIDI_DrumOn_HHO
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	subwf	MBFM_DR_HH_O_SPLIT_LOWER, W, BANKED
	addwf	MBFM_DR_HH_FRQ, W, BANKED
	call	MBFM_DRUM_SetHHFrq

	movff	MBFM_DR_HH_O_KSL_TL, PRODL
	movlw	0x51
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol

	;; switch the remaining parameters to "HH open"
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_OPEN
	
	movlw	0x01
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotHHO

	BRA_IFCLR TMP1, 2, ACCESS, MBFM_MIDI_DrumOn_NotHHC
MBFM_MIDI_DrumOn_HHC
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	subwf	MBFM_DR_HH_C_SPLIT_LOWER, W, BANKED
	addwf	MBFM_DR_HH_FRQ, W, BANKED
	call	MBFM_DRUM_SetHHFrq

	;; ToDo: take envelope from second HH parameter set
	movff	MBFM_DR_HH_C_KSL_TL, PRODL
	movlw	0x51
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol

	;; switch the remaining parameters to "HH close"
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_HH_CLOSE
	
	movlw	0x01
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotHHC

	BRA_IFCLR TMP1, 3, ACCESS, MBFM_MIDI_DrumOn_NotSD
MBFM_MIDI_DrumOn_SD
	movff	MBFM_DR_SD_KSL_TL, PRODL
	movlw	0x54
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol

	movlw	0x08
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotSD

	BRA_IFCLR TMP1, 4, ACCESS, MBFM_MIDI_DrumOn_NotTOM
MBFM_MIDI_DrumOn_TOM
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	subwf	MBFM_DR_TOM_SPLIT_LOWER, W, BANKED
	addwf	MBFM_DR_TOM_FRQ, W, BANKED
	call	MBFM_DRUM_SetTOMFrq

	movff	MBFM_DR_TOM_KSL_TL, PRODL
	movlw	0x52
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol
	
	movlw	0x04
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotTOM

	BRA_IFCLR TMP1, 5, ACCESS, MBFM_MIDI_DrumOn_NotCYM
MBFM_MIDI_DrumOn_CYM
	movf	MBFM_MIDI_PARAMETER1, W, BANKED
	subwf	MBFM_DR_TOM_SPLIT_LOWER, W, BANKED
	addwf	MBFM_DR_TOM_FRQ, W, BANKED
	call	MBFM_DRUM_SetTOMFrq

	movff	MBFM_DR_CYM_KSL_TL, PRODL
	movlw	0x55
	rcall	MBFM_MIDI_DrumOn_Hlp_Vol
	
	movlw	0x02
	rcall	MBFM_MIDI_DrumOn_SetGate
MBFM_MIDI_DrumOn_NotCYM

	return


;; Help functions for Drum-On
MBFM_MIDI_DrumOn_SetGate
	iorwf	MBFM_DRUM_GATES, F, BANKED
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_GATES, BANKED
	return


	;; expects velocity in MBFM_MIDI_PARAMETER2
	;; TL in PRODL
	;; OPL3 register in WREG
MBFM_MIDI_DrumOn_Hlp_Vol
	;; save OPL3 register address in PRODH
	movwf	PRODH

	;; get volume addend
	movf	MBFM_MIDI_PARAMETER2, W, BANKED
	call	MBFM_VOL_TABLE_Get	; volume addend in MIOS_PARAMETER1

	;; convert TL to volume
	comf	PRODL, W
	andlw	0x3f
	addwf	MIOS_PARAMETER1, W
	movwf	TABLAT

	;; saturate
	;; 	movf	TABLAT, W
	andlw	0xc0
	bz	MBFM_MIDI_DrumOn_Hlp_Vol_NoSat
MBFM_MIDI_DrumOn_Hlp_Vol_Sat
	movlw	0x3f
	btfsc	MIOS_PARAMETER1, 7
	movlw 0x00
	movwf	TABLAT
MBFM_MIDI_DrumOn_Hlp_Vol_NoSat

	;; write to KSL_TL register
	movff	PRODH, MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movf	TABLAT, W
	xorlw	0x3f
	andlw	0x3f
	call	MBFM_REG_Write
	goto	MBFM_REG_WaitOPL		; wait for 32 OPL3 clock cycles (ca. 2.2 uS)


;; --------------------------------------------------------------------------
;;  This function is called to forward a Drum Off event to the synthesizer
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;     o velocity in MBFM_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_MIDI_DrumOff
	;; get selected drums
	rcall	MBFM_MIDI_Drum_GetSel
	;; result in TMP1:
	;; TMP1[0]: BD
	;; TMP1[1]: HH Open
	;; TMP1[2]: HH Close
	;; TMP1[3]: SD
	;; TMP1[4]: Tom
	;; TMP1[5]: Cymbal

	BRA_IFCLR TMP1, 0, ACCESS, MBFM_MIDI_DrumOff_NotBD
MBFM_MIDI_DrumOff_BD
	movlw	~0x10
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotBD

	BRA_IFCLR TMP1, 1, ACCESS, MBFM_MIDI_DrumOff_NotHHO
MBFM_MIDI_DrumOff_HHO
	movlw	~0x01
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotHHO

	BRA_IFCLR TMP1, 2, ACCESS, MBFM_MIDI_DrumOff_NotHHC
MBFM_MIDI_DrumOff_HHC
	movlw	~0x01
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotHHC

	BRA_IFCLR TMP1, 3, ACCESS, MBFM_MIDI_DrumOff_NotSD
MBFM_MIDI_DrumOff_SD
	movlw	~0x08
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotSD

	BRA_IFCLR TMP1, 4, ACCESS, MBFM_MIDI_DrumOff_NotTOM
MBFM_MIDI_DrumOff_TOM
	movlw	~0x04
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotTOM

	BRA_IFCLR TMP1, 5, ACCESS, MBFM_MIDI_DrumOff_NotCYM
MBFM_MIDI_DrumOff_CYM
	movlw	~0x02
	rcall	MBFM_MIDI_DrumOff_ClrGate
MBFM_MIDI_DrumOff_NotCYM
	return



;; Help functions for Drum-Off
MBFM_MIDI_DrumOff_ClrGate
	andwf	MBFM_DRUM_GATES, F, BANKED
	bsf	MBFM_UPDATE_DR, MBFM_UPDATE_DR_GATES, BANKED
	return


;; --------------------------------------------------------------------------
;;  This function returns the selected drums depending on the note number
;;  Input:
;;     o note number in MBFM_MIDI_PARAMETER1
;;  Output:
;;     o selected drum in TMP1
;;	 TMP1[0]: BD
;;	 TMP1[1]: HH Open
;;	 TMP1[2]: HH Close
;;	 TMP1[3]: SD
;;	 TMP1[4]: Tom
;;	 TMP1[5]: Cymbal
;; --------------------------------------------------------------------------
MBFM_MIDI_Drum_GetSel
	;; get pointer to first split point
	lfsr	FSR1, MBFM_DR_BD_SPLIT_LOWER

	clrf	TMP1	; contains the flag
	clrf	TMP2	; loop 6 times
MBFM_MIDI_Drum_GetSelLoop
	;; check lower split point
	movf	POSTINC1, W
	cpfslt	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_Drum_GetSelLoop_LOk
	movf	POSTINC1, W	; (dummy increment)
	rgoto	MBFM_MIDI_Drum_GetSelLoop_Next
MBFM_MIDI_Drum_GetSelLoop_LOk
	;; check upper split point
	movf	POSTINC1, W
	cpfsgt	MBFM_MIDI_PARAMETER1, BANKED
	rgoto MBFM_MIDI_Drum_GetSelLoop_UOk
	rgoto	MBFM_MIDI_Drum_GetSelLoop_Next
MBFM_MIDI_Drum_GetSelLoop_UOk
	;; set flag
	movf	TMP2, W
	call	MIOS_HLP_GetBitORMask
	iorwf	TMP1, F
MBFM_MIDI_Drum_GetSelLoop_Next
	incf	TMP2, F
	movlw	6-1
	cpfsgt	TMP2, ACCESS
	rgoto MBFM_MIDI_Drum_GetSelLoop

	return


;; --------------------------------------------------------------------------
;;  help function which returns the Ix base in FSR0 depending on MBFM_CURRENT_INSTRUMENT
MBFM_MIDI_Hlp_GetINSBase_FSR0
	lfsr	FSR0, MBFM_I1_BASE
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	mullw	MBFM_Ix_RECORD_LEN
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return
