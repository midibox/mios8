; $Id$
;
; MIDIbox SID
; MIDI Interface part for Bassline Engine
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note On event to the synthesizer
;;  Input:
;;     o MIDI Voice in SID_CURRENT_MIDI_VOICE
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_B_NoteOn
	;; go through four midi voices
	;; 1 and 2 used to play the bassline/select the sequence
	;; 3 and 4 used to transpose a sequence
	clrf	SID_CURRENT_MIDIVOICE, BANKED	; loop counter
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_B_NoteOn_Loop

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_B_NoteOn_Loop_Next

	;; branch depending on voice
	BRA_IFCLR SID_CURRENT_MIDIVOICE, 1, BANKED, SID_MIDI_B_NoteOn_Loop_Voice
SID_MIDI_B_NoteOn_Loop_Trans
	;; determine pointer to note stack -> FSR2
	call	SID_MIDI_Hlp_GetNoteStackFSR2
	;; check the active note
	call	SID_MIDI_Hlp_CheckActiveNote
	bz	SID_MIDI_B_NoteOn_Loop_TransNClr
SID_MIDI_B_NoteOn_Loop_TransClr
	call	SID_MIDI_Hlp_ClrStack	; clear note stack (for the case that some notes are marked with bit #7 (key released)
SID_MIDI_B_NoteOn_Loop_TransNClr
	;; push note into stack
#if 0
	call	SID_MIDI_Hlp_PushArpHold ; push at the end of stack
#else
	call	SID_MIDI_Hlp_PushNote ; push at the beginning of stack
#endif
	rgoto	SID_MIDI_B_NoteOn_Loop_Next

SID_MIDI_B_NoteOn_Loop_Voice
	;; set velocity value
	;; L/R selection depending on MIDI voice number
	movff	CS_MENU_SELECTED_SID_LR, TMP1		; taken from CS_MENU_SELECTED_SID_LR... dirty :-/
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_SELECTED_SID_LR
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_VEL
	call	SID_KNOB_SetValue
	movff	TMP1, CS_MENU_SELECTED_SID_LR

	;; get pointer to instrument -> FSR2
	rcall	SID_MIDI_B_Hlp_FSR2_Ins

	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	movlw	SID_Ix_Vx_ARP_MODE
	movff	PLUSW2, TMP1

	;; SID_Ix_Vx_ARP_SPEED_DIV of instrument -> TMP2
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	movff	PLUSW2, TMP2

	;; copy SID_Ix_B_Vx_FLAGS2 to temporary register SID_MIDI_FLAGS
	movlw	SID_Ix_B_Vx_FLAGS2
	movff	PLUSW2, SID_MIDI_FLAGS

	;; calculate pointer to voice (SIDL/SIDR Voice 1)
	rcall	SID_MIDI_B_Hlp_FSR1_Voice

	;; branch depending on Normal/Arp/Sequencer mode
	BRA_IFSET SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, SID_MIDI_B_NoteOn_Loop_Seq
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_B_NoteOn_Loop_Arp
SID_MIDI_B_NoteOn_Loop_Norm
	;; push note into WT stack
	call	SID_MIDI_Hlp_GetNoteStackFSR2
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	call	SID_MIDI_Hlp_PushWT

	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F

	;; push note into stack
	rcall	SID_MIDI_Hlp_PushNote

	;; switch off gate if not in legato mode
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_LEGATO, BANKED, SID_MIDI_B_Hlp_GateOff

	;; call note-on handler
	rcall	SID_MIDI_B_Hlp_NoteOn
	rgoto	SID_MIDI_B_NoteOn_Loop_Next
	
SID_MIDI_B_NoteOn_Loop_Arp
	;; save voice number in midivoice (instrument) record
	movlw	SID_MVx_LAST_VOICE
	movff	SID_CURRENT_VOICE, PLUSW0
	;; save midivoice (instrument) number in voice record
	movlw	SID_Vx_ASSIGNED_MV
	movff	SID_CURRENT_MIDIVOICE, PLUSW1
	;; determine pointer to note stack -> FSR2
	call	SID_MIDI_Hlp_GetNoteStackFSR2
	;; call Arp handler
	call	SID_MIDI_Hlp_ArpNoteOn
	rgoto	SID_MIDI_B_NoteOn_Loop_Next


SID_MIDI_B_NoteOn_Loop_Seq
	;; push note into WT stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	call	SID_MIDI_Hlp_PushWT

	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F

	;; push note into stack
	rcall	SID_MIDI_Hlp_PushNote

	;; save midivoice (instrument) number in voice record
	movlw	SID_Vx_ASSIGNED_MV
	movff	SID_CURRENT_MIDIVOICE, PLUSW1

	;; reset sequencer if voice was not active before
	;; do this with both voices for proper synchronisation!

	;; only used in master mode
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SID_MIDI_B_NoteOn_Loop_Seq_NoR01

	movff	SIDL_V1_BASE + SID_Vx_STATE, WREG
	BRA_IFSET WREG, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_B_NoteOn_Loop_Seq_NoR0
SID_MIDI_B_NoteOn_Loop_Seq_R0
	movlw	0x01
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SID_MIDI_B_NoteOn_Loop_Seq_NoR0

	movff	SIDR_V1_BASE + SID_Vx_STATE, WREG
	BRA_IFSET WREG, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_B_NoteOn_Loop_Seq_NoR1
SID_MIDI_B_NoteOn_Loop_Seq_R1
	movlw	0x02
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SID_MIDI_B_NoteOn_Loop_Seq_NoR1
SID_MIDI_B_NoteOn_Loop_Seq_NoR01

	;; select new sequence
	rcall	SID_MIDI_B_Hlp_SetSeq

	;; always set voice active flag of both voices to ensure that they are in sync
	;; ensure that this is only done for instruments where WTO (sequence) is selected
	SET_BSR	SIDL_V1_BASE
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	btfsc	WREG, SID_I_B_V_FLAGS2_WT_ONLY
	bsf	SIDL_V1_BASE + SID_Vx_STATE, SID_V_STATE_VOICE_ACTIVE

	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	btfsc	WREG, SID_I_B_V_FLAGS2_WT_ONLY
	bsf	SIDR_V1_BASE + SID_Vx_STATE, SID_V_STATE_VOICE_ACTIVE
	SET_BSR	SID_BASE
	;; 	rgoto	SID_MIDI_B_NoteOn_Loop_Next

	
SID_MIDI_B_NoteOn_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	4 - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_B_NoteOn_Loop

SID_MIDI_B_NoteOn_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note Off event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_B_NoteOff
	SET_BSR	SID_BASE

	;; go through four midi voices
	;; 1 and 2 used to play the bassline/select the sequence
	;; 3 and 4 used to transpose a sequence
	clrf	SID_CURRENT_MIDIVOICE, BANKED		; loop counter
	clrf	SID_CURRENT_VOICE, BANKED	; corresponding voice number (0 or 3)
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_B_NoteOff_Loop

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_B_NoteOff_Loop_Next

	;; branch depending on voice
	BRA_IFCLR SID_CURRENT_MIDIVOICE, 1, BANKED, SID_MIDI_B_NoteOff_Loop_Voice
SID_MIDI_B_NoteOff_Loop_Trans
	;; determine pointer to note stack -> FSR2
	call	SID_MIDI_Hlp_GetNoteStackFSR2
	;; pop note from stack
	call	SID_MIDI_Hlp_PopArpHold
	;; TODO: add new algorithm which shifts the de-activated note to the end of the stack
	;; this would allow following handling (example):
	;; Press C-3: plays sequence with base note
	;; Press E-3 in addition: transposes sequence by +4
	;; Depress E-3: currently holds the transpose - desired behaviour: play C-3 (base note) again, since the key is still pressed
	rgoto	SID_MIDI_B_NoteOff_Loop_Next

SID_MIDI_B_NoteOff_Loop_Voice
	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	rcall	SID_MIDI_B_Hlp_FSR2_Ins
	movlw	SID_Ix_Vx_ARP_MODE
	movff	PLUSW2, TMP1

	;; SID_Ix_Vx_ARP_SPEED_DIV of instrument -> TMP2
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	movff	PLUSW2, TMP2

	;; copy SID_Ix_B_Vx_FLAGS2 to temporary register SID_MIDI_FLAGS
	movlw	SID_Ix_B_Vx_FLAGS2
	movff	PLUSW2, SID_MIDI_FLAGS

	;; pop note from WT stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	call	SID_MIDI_Hlp_PopWT

	;; calculate pointer to voice (SIDL/SIDR Voice 1)
	rcall	SID_MIDI_B_Hlp_FSR1_Voice
	
	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F
	movff	INDF2, TMP3	; save current #0 entry in TMP3 for later use

	;; branch depending on Normal/Arp/Sequencer mode
	BRA_IFSET SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_WT_ONLY, BANKED, SID_MIDI_B_NoteOff_Loop_Seq
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_B_NoteOff_Loop_Arp
SID_MIDI_B_NoteOff_Loop_Norm
	;; pop note from stack (pointer to stack in FSR2)
	rcall	SID_MIDI_Hlp_PopNote

	movf	TMP3, W		; restore note
	rcall	SID_MIDI_B_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_B_Hlp_NoteOn
	rgoto	SID_MIDI_B_NoteOff_Loop_Next


SID_MIDI_B_NoteOff_Loop_Arp
	;; call Arp handler
	call	SID_MIDI_Hlp_ArpNoteOff
	rgoto	SID_MIDI_B_NoteOff_Loop_Next
	

SID_MIDI_B_NoteOff_Loop_Seq
	;; pop note from stack (pointer to stack in FSR2)
	rcall	SID_MIDI_Hlp_PopNote

	;; FSR2 -> pointer to instrument patch
	rcall	SID_MIDI_B_Hlp_FSR2_Ins

	;; select sequence if there is still a note in stack
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	skpz
	rcall	SID_MIDI_B_Hlp_SetSeq

;; disabled --- hold mode always activated, sequence can be disabled with key G#/A/A#/B
#if 0
	;; clear "voice active" flag if no note played and no sequence active
	rcall	SID_MIDI_B_Hlp_FSR2_Ins
	movlw	SID_Ix_B_Vx_SEQ_SPEED
	BRA_IFSET PLUSW2, SID_I_V_SEQ_HOLD, ACCESS, SID_MIDI_B_NoteOff_Loop_Seq_NoS
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bnz	SID_MIDI_B_NoteOff_Loop_Seq_NoS
SID_MIDI_B_NoteOff_Loop_Seq_S
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
SID_MIDI_B_NoteOff_Loop_Seq_NoS
#endif

	;; disable voice active flag of both voices if both are playing invalid sequences (seq off)
	;; only used in master mode
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SID_MIDI_B_NoteOff_Loop_Seq_NoDs

	;; always set voice active flag of both voices to ensure that they are in sync
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM, WREG
	andlw	0xf8
	bz	SID_MIDI_B_NoteOff_Loop_Seq_NoDs
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM, WREG
	andlw	0xf8
	bz	SID_MIDI_B_NoteOff_Loop_Seq_NoDs
SID_MIDI_B_NoteOff_Loop_Seq_Dis
	;; ensure that this is only done for instruments where WTO (sequence) is selected
	SET_BSR	SIDL_V1_BASE
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	btfsc	WREG, SID_I_B_V_FLAGS2_WT_ONLY
	bcf	SIDL_V1_BASE + SID_Vx_STATE, SID_V_STATE_VOICE_ACTIVE, BANKED

	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_FLAGS2, WREG
	btfsc	WREG, SID_I_B_V_FLAGS2_WT_ONLY
	bcf	SIDR_V1_BASE + SID_Vx_STATE, SID_V_STATE_VOICE_ACTIVE, BANKED
	SET_BSR	SID_BASE
SID_MIDI_B_NoteOff_Loop_Seq_NoDs

	;; 	rgoto	SID_MIDI_B_NoteOff_Loop_Next

	
SID_MIDI_B_NoteOff_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	4 - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_B_NoteOff_Loop

SID_MIDI_B_NoteOff_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a PitchBender event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o 8bit PitchBender value in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_B_PitchBender
	;; first MIDI voice has already been handled in sid_midi.inc
	;; now check for MIDI channel of second voice

	;; forward to knob handler (only checks for MIDI channel of first MIDI voice)
	lfsr	FSR0, SID_MV2_BASE

	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_B_PitchBender_NoKnob
SID_MIDI_B_PitchBender_Knob
	;; select second instrument
	movff	CS_MENU_SELECTED_SID_LR, TMP1		; taken from CS_MENU_SELECTED_SID_LR... dirty :-/
	movlw	0x02
	movwf	CS_MENU_SELECTED_SID_LR

	;; copy converted pitch bender into mod matrix source
	movff	SID_MIDI_PARAMETER1, MIOS_PARAMETER1
	movlw	SID_KNOB_PB
	call	SID_KNOB_SetValue
	movff	TMP1, CS_MENU_SELECTED_SID_LR
SID_MIDI_B_PitchBender_NoKnob
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a CC event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o CC number in SID_MIDI_PARAMETER1
;;     o CC value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_B_CC
	SET_BSR	SID_BASE

	;; go through all midi voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED	; loop counter
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_B_CC_Loop
	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_B_CC_Loop_Next

	;; if CC#06 (NRPN data LSB) received, forward to parameter handler
	movlw	0x06
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_B_CC_NoNRPNDataH
SID_MIDI_B_CC_NRPNDataH
	;; prepare MIOS_PARAMETER3 (selection options)
	swapf	SID_CURRENT_MIDIVOICE, W, BANKED	; expecting instrument in MIOS_PARAMETER3[6:4]
	movwf	MIOS_PARAMETER3		; (L/R selection done in NRPN function)
	call	SID_PARIN_SetNRPN
	rgoto	SID_MIDI_B_CC_Loop_Next
SID_MIDI_B_CC_NoNRPNDataH

	;; if CC#64 (Sustain) received, set/clear sustain flags and release notes if required
	movlw	0x40
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_B_CC_Not64
SID_MIDI_B_CC_64
	;; TODO: not supported yet
SID_MIDI_B_CC_Not64

SID_MIDI_B_CC_Loop_Next

	;; extra: ignore if MIDI channel of second bassline matches with first bassline
	;; (parameter transfer already done)
	movff	SID_MV1_BASE+SID_MVx_MIDI_CHANNEL, PRODL
	movff	SID_MV2_BASE+SID_MVx_MIDI_CHANNEL, WREG
	xorwf	PRODL, W
	bz	SID_MIDI_B_CC_Loop_End

	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	2-1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_B_CC_Loop

SID_MIDI_B_CC_Loop_End
	return


;; --------------------------------------------------------------------------
;;  help routines for Bassline Engine
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; Note On help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     MIDI voice number in SID_CURRENT_MIDIVOICE
;;     Voice number in SID_CURRENT_VOICE
;; ALSO USED BY SID_PATCH_Init !
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_NoteOn
	;; save note into SID_Vx_NOTE and SID_Vx_PLAYED_NOTE (last one is relevant for MIDI handler)
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	movlw	SID_Vx_NOTE
	movff	PRODL, PLUSW1
	movlw	SID_Vx_PLAYED_NOTE
	movff	PRODL, PLUSW1

	;; sus-key activated?
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_SUS_KEY, BANKED, SID_MIDI_B_Hlp_NoteOn_NoSusKey
SID_MIDI_B_Hlp_NoteOn_SusKey
	;; in suy-key mode, we activate portamento only if at least one key is played
	movlw	SID_MVx_NOTE_STACK_PTR
	decf	PLUSW0, W
	bz	SID_MIDI_B_Hlp_NoteOn_NoSusKeyPr
SID_MIDI_B_Hlp_NoteOn_NoSusKey
	;; omit portamento if first key played after patch initialisation
	movlw	SID_Vx_STATE2
	BRA_IFCLR PLUSW1, SID_V_STATE2_PORTA_INITIALIZED, ACCESS, SID_MIDI_B_Hlp_NoteOn_NoSusKeyPr
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
SID_MIDI_B_Hlp_NoteOn_NoSusKeyPr

	;; next key will allow portamento
	movlw	SID_Vx_STATE2
	bsf	PLUSW1, SID_V_STATE2_PORTA_INITIALIZED

	;; skip the rest if legato and voice already active
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_LEGATO, BANKED, SID_MIDI_B_Hlp_NoteOn_NoLegato
SID_MIDI_B_Hlp_NoteOn_Legato
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_B_Hlp_NoteOn_NoGate
SID_MIDI_B_Hlp_NoteOn_NoLegato

	;; request gate bit
	rcall	SID_MIDI_B_Hlp_GateOn

SID_MIDI_B_Hlp_NoteOn_NoGate

	;; don't sync if WTO mode or legato mode and current note is first note
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_LEGATO, BANKED, SID_MIDI_B_Hlp_NoteOn_NOnTrg
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	movf	SID_MIDI_PARAMETER1, W, BANKED
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_B_Hlp_NoteOn_NoNOnTrg
	movlw	SID_MVx_NOTE_STACK_1
	movf	PLUSW0, W
	bnz	SID_MIDI_B_Hlp_NoteOn_NoNOnTrg
SID_MIDI_B_Hlp_NoteOn_NOnTrg
SID_MIDI_B_Hlp_NoteOn_NoNOnTrg

	;; save voice number in midivoice (instrument) record
	movlw	SID_MVx_LAST_VOICE
	movff	SID_CURRENT_VOICE, PLUSW0

	;; save midivoice (instrument) number in voice record
	movlw	SID_Vx_ASSIGNED_MV
	movff	SID_CURRENT_MIDIVOICE, PLUSW1

	return

;; --------------------------------------------------------------------------
;; Note Off help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;; OUT: returns 0x01 if gate should be retriggered (mono mode, more than one note was played)
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!
	movwf	TABLAT

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	movf	TABLAT, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_B_Hlp_NoteOff_End
	BRA_IFSET SID_MIDI_FLAGS, SID_I_B_V_FLAGS2_LEGATO, BANKED, SID_MIDI_B_Hlp_NoteOff_NoGOff
	rcall	SID_MIDI_B_Hlp_GateOff
SID_MIDI_B_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if there is still a note in the stack, play new note with NoteOn function (checked by caller)
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bz	SID_MIDI_B_Hlp_NoteOff_GOff

	;; activate portamento (will be ignored by Pitch handler if no portamento active - important for SusKey function to have it here!)
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE

	retlw	0x01		; return, request Note On!

SID_MIDI_B_Hlp_NoteOff_GOff
	;; else request gate clear bit
	rcall	SID_MIDI_B_Hlp_GateOff

SID_MIDI_B_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!


;; --------------------------------------------------------------------------
;; Gate On help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     voice number in SID_CURRENT_VOICE
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_GateOn
	;; set "voice active" flag
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE

	;; request gate
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED

	;; request LFO re-sync
	movf	SID_CURRENT_VOICE, W, BANKED
	addlw	2
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	SID_CURRENT_VOICE, W, BANKED
	movlw	0x01
	skpz
	movlw	0x02
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; ENV attack phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVA, F, BANKED

	;; set accent flag depending on velocity (set when velocity >= 0x40)
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_ACCENT
	btfsc	SID_MIDI_PARAMETER2, 6, BANKED
	bsf	PLUSW1, SID_V_STATE_ACCENT

SID_MIDI_B_Hlp_GateOn_End
	return

;; --------------------------------------------------------------------------
;; Gate Off help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     note in SID_MIDI_PARAMETER1, velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_GateOff
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_B_Hlp_GateOff_End

	movlw	SID_Vx_PLAYED_NOTE
	movf	PLUSW1, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_B_Hlp_GateOff_End

	;; request gate off if not disabled via trigger matrix
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; request gate clear via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; ENV release phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED

SID_MIDI_B_Hlp_GateOff_End
	return	


;; --------------------------------------------------------------------------
;; Help function which returns a pointer to the SID voice of a MIDI voice in FSR1
;; IN: SID_CURRENT_MIDIVOICE
;; OUT: pointer to voice in FSR1, voice number in SID_CURRENT_VOICE
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_FSR1_Voice
	;; MIDI instrument 0 -> SIDL_V1_VOICE (voice #0)
	;; MIDI instrument 1 -> SIDR_V1_VOICE (voice #3)

	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	skpz
	movlw	3
	movwf	SID_CURRENT_VOICE, BANKED

	lfsr	FSR1, SIDL_V1_BASE
	skpz
	lfsr	FSR1, SIDR_V1_BASE
	return


;; --------------------------------------------------------------------------
;; Help function which returns a pointer to the assigned instrument
;; of a MIDI voice in FSR2
;; IN: SID_CURRENT_MIDIVOICE
;; OUT: pointer to instrument in FSR2
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_FSR2_Ins
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_Ix_B_S2V1_BASE-SID_Ix_B_S1V1_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	return


;; --------------------------------------------------------------------------
;; Help function which selects the sequence based on played note
;; IN: SID_CURRENT_MIDIVOICE (0 or 1), voice base in FSR1, midi voice base in FSR0
;; --------------------------------------------------------------------------
SID_MIDI_B_Hlp_SetSeq
	;; get first note
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL

	;; add MIDI voice based transpose value (ensure that within range 0x00..0x7f)
	movlw	SID_MVx_TRANSPOSE
	movf	PLUSW0, W
	addlw	-0x40
	BRA_IFSET WREG, 7, ACCESS, SID_MIDI_B_Hlp_SetSeq_TrnN
SID_MIDI_B_Hlp_SetSeq_TrnP
	addwf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, SID_MIDI_B_Hlp_SetSeq_Trn_Cont
	addlw	-12
	rgoto	SID_MIDI_B_Hlp_SetSeq_TrnP
SID_MIDI_B_Hlp_SetSeq_TrnN
	addwf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, SID_MIDI_B_Hlp_SetSeq_Trn_Cont
	movlw	12
	rgoto	SID_MIDI_B_Hlp_SetSeq_TrnN
SID_MIDI_B_Hlp_SetSeq_Trn_Cont

	;; store note in voice record
	movlw	SID_Vx_PLAYED_NOTE
	movff	PRODL, PLUSW1
	

	;; determine sequence based on note number
	;; remove octave from note
SID_MIDI_B_Hlp_SetSeq_RemoveOct
	movlw	12 - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_B_Hlp_SetSeq_OctOk
	movlw	-12
	addwf	PRODL, F
	rgoto	SID_MIDI_B_Hlp_SetSeq_RemoveOct

SID_MIDI_B_Hlp_SetSeq_OctOk

	;; if note number >= 8: set to 8 (sequence off)
	movlw	8
	cpfslt	PRODL, ACCESS
	movwf PRODL

	;; set new sequence
	BRA_IFSET SID_CURRENT_MIDIVOICE, 0, BANKED, SID_MIDI_B_Hlp_SetSeq_1
SID_MIDI_B_Hlp_SetSeq_0
	;; patch/shadow buffer
	movff	PRODL, SID_PATCH_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM
	movff	PRODL, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM

	;; forward to edit buffer if SID selected
	call	CS_MENU_MS_GetSIDNumber
	xorwf	CS_MENU_EDIT_BUFFER_SID, W
	skpnz
	movff	PRODL, SID_EDIT_BUFFER + SID_Ix_B_S1V1_BASE + SID_Ix_B_Vx_SEQ_NUM

	return

SID_MIDI_B_Hlp_SetSeq_1
	;; patch/shadow buffer
	movff	PRODL, SID_PATCH_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM
	movff	PRODL, SID_PATCH_BUFFER_SHADOW + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM

	;; forward to edit buffer if SID selected
	call	CS_MENU_MS_GetSIDNumber
	xorwf	CS_MENU_EDIT_BUFFER_SID, W
	skpnz
	movff	PRODL, SID_EDIT_BUFFER + SID_Ix_B_S2V1_BASE + SID_Ix_B_Vx_SEQ_NUM

	return
	
