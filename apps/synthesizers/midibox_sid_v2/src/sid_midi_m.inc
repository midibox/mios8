; $Id$
;
; MIDIbox SID
; MIDI Interface part for Multi Engine
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
SID_MIDI_M_NoteOn
	;; go through all midi voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED	; loop counter
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_M_NoteOn_Loop

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_M_NoteOn_Loop_Next

	;; if first MIDI voice: set velocity value
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	bnz	SID_MIDI_M_NoteOn_Not0
SID_MIDI_M_NoteOn_0
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_VEL
	call	SID_KNOB_SetValue
SID_MIDI_M_NoteOn_Not0

	;; dedicated velocity assignment for instrument
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER2	; high byte
	clrc
	rlf	MIOS_PARAMETER2, F
	clrf	MIOS_PARAMETER1				; low byte
	swapf	SID_CURRENT_MIDIVOICE, W, BANKED	; current instrument
	andlw	0x70
	iorlw	0x03
	movwf	MIOS_PARAMETER3
	rcall	SID_MIDI_M_Hlp_FSR2_Ins			; pointer to instrument -> FSR2
	movlw	SID_Ix_M_Vx_VELOCITY_ASSGN		; velocity assignment
	movf	PLUSW2, W
	call	SID_PARIN_Set16

	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	movlw	SID_Ix_Vx_ARP_MODE
	movff	PLUSW2, TMP1

	;; SID_Ix_Vx_ARP_SPEED_DIV of instrument -> TMP2
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	movff	PLUSW2, TMP2

	;; copy SID_Ix_M_Vx_FLAGS2 to temporary register SID_MIDI_FLAGS
	movlw	SID_Ix_M_Vx_FLAGS2
	movff	PLUSW2, SID_MIDI_FLAGS

	;; branch depending on Normal/Arp mode
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_M_NoteOn_Loop_Arp
SID_MIDI_M_NoteOn_Loop_Norm
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
	call	SID_MIDI_Hlp_PushNote

	;; switch off gate if not in legato or WTO mode
	BRA_IFSET SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_WT_ONLY, BANKED, SID_MIDI_M_NoteOn_Ok_NoGateOff
SID_MIDI_M_NoteOn_Ok_GateOff
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_LEGATO, BANKED, SID_MIDI_M_Hlp_GateOff
SID_MIDI_M_NoteOn_Ok_NoGateOff

	;; call note-on handler
	rcall	SID_MIDI_M_Hlp_NoteOn
	rgoto	SID_MIDI_M_NoteOn_Loop_Next

	
SID_MIDI_M_NoteOn_Loop_Arp
	;; get new voice (or use the already allocated one)
	call	SID_MIDI_M_Hlp_GetVoice
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
	;; 	rgoto	SID_MIDI_M_NoteOn_Loop_Next
	
SID_MIDI_M_NoteOn_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_M_NoteOn_Loop

SID_MIDI_M_NoteOn_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note Off event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_M_NoteOff
	SET_BSR	SID_BASE

	;; go through all midi voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED		; loop counter
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_M_NoteOff_Loop

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_M_NoteOff_Loop_Next

	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	rcall	SID_MIDI_M_Hlp_FSR2_Ins
	movlw	SID_Ix_Vx_ARP_MODE
	movff	PLUSW2, TMP1

	;; SID_Ix_Vx_ARP_SPEED_DIV of instrument -> TMP2
	movlw	SID_Ix_Vx_ARP_SPEED_DIV
	movff	PLUSW2, TMP2

	;; copy SID_Ix_M_Vx_FLAGS2 to temporary register SID_MIDI_FLAGS
	movlw	SID_Ix_M_Vx_FLAGS2
	movff	PLUSW2, SID_MIDI_FLAGS

	;; pop note from WT stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	call	SID_MIDI_Hlp_PopWT

	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F
	movff	INDF2, TMP3	; save current #0 entry in TMP3 for later use

	;; branch depending on Normal/Arp mode
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_M_NoteOff_Loop_Arp
SID_MIDI_M_NoteOff_Loop_Norm
	;; pop note from stack (pointer to stack in FSR2)
	call	SID_MIDI_Hlp_PopNote
	bnz	SID_MIDI_M_NoteOff_Loop_Next ; ZERO flag cleared: note not found!

	movf	TMP3, W		; restore note
	rcall	SID_MIDI_M_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_M_Hlp_NoteOn
	rgoto	SID_MIDI_M_NoteOff_Loop_Next


SID_MIDI_M_NoteOff_Loop_Arp
	;; determine pointer to voice -> FSR1
	movlw	SID_MVx_LAST_VOICE
	movf	PLUSW0, W
	mullw	SID_Vx_RECORD_LEN
	lfsr	FSR1, SIDL_V1_BASE
	movf	PRODL, W
	addwf	FSR1L, F

	;; call Arp handler
	call	SID_MIDI_Hlp_ArpNoteOff
	;; 	rgoto	SID_MIDI_M_NoteOff_Loop_Next
	
SID_MIDI_M_NoteOff_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_M_NoteOff_Loop

SID_MIDI_M_NoteOff_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a PitchBender event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o 8bit PitchBender value in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_M_PitchBender
	SET_BSR	SID_BASE

	;; go through all midi voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_M_PitchBender_LoopO
	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_M_PitchBender_LoopO_Nxt

	;; dedicated pitchbender assignment for instrument
	movff	SID_MIDI_PARAMETER1, MIOS_PARAMETER2	; high byte
	clrf	MIOS_PARAMETER1				; low byte
	swapf	SID_CURRENT_MIDIVOICE, W, BANKED	; current instrument
	andlw	0x70
	iorlw	0x03
	movwf	MIOS_PARAMETER3
	rcall	SID_MIDI_M_Hlp_FSR2_Ins			; pointer to instrument -> FSR2
	movlw	SID_Ix_M_Vx_PITCHBENDER_ASSGN		; pitchbender assignment
	movf	PLUSW2, W
	call	SID_PARIN_Set16

SID_MIDI_M_PitchBender_LoopO_Nxt
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	movlw	0x00
	addwfc	FSR0H, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	SID_MVx_NUM - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_M_PitchBender_LoopO

SID_MIDI_M_PitchBender_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a CC event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o CC number in SID_MIDI_PARAMETER1
;;     o CC value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_M_CC
	SET_BSR	SID_BASE

	;; go through all midi voices
	clrf	SID_CURRENT_MIDIVOICE, BANKED	; loop counter
	lfsr	FSR0, SID_MV1_BASE
SID_MIDI_M_CC_Loop
	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_M_CC_Loop_Next

	;; if CC#06 (NRPN data MSB) received, forward to parameter handler
	movlw	0x06
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_M_CC_NoNRPNDataH
SID_MIDI_M_CC_NRPNDataH
	;; prepare MIOS_PARAMETER3 (selection options)
	swapf	SID_CURRENT_MIDIVOICE, W, BANKED	; expecting instrument in MIOS_PARAMETER3[6:4]
	movwf	MIOS_PARAMETER3		; (L/R selection done in NRPN function)
	call	SID_PARIN_SetNRPN
	rgoto	SID_MIDI_M_CC_Loop_Next
SID_MIDI_M_CC_NoNRPNDataH

	;; if CC#64 (Sustain) received, set/clear sustain flags and release notes if required
	movlw	0x40
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_M_CC_Not64
SID_MIDI_M_CC_64
	;; TODO: not supported yet
SID_MIDI_M_CC_Not64

	;; handle remaining CCs
	swapf	SID_CURRENT_MIDIVOICE, W, BANKED	; expecting instrument in MIOS_PARAMETER3[6:4]
	andlw	0xf0
	movwf	MIOS_PARAMETER3
	call	SID_CC_TABLE_Set

SID_MIDI_M_CC_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	incf	SID_CURRENT_MIDIVOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_M_CC_Loop
	return


;; --------------------------------------------------------------------------
;;  help routines for Multi Engine
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; Note On help function
;; IN: Pointer to SID_MVx_BASE in FSR0
;;     MIDI voice number in SID_CURRENT_MIDIVOICE
;;     velocity in SID_MIDI_PARAMETER2
;; ALSO USED BY SID_PATCH_Init !
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_NoteOn
	;; get new voice
	rcall	SID_MIDI_M_Hlp_GetVoice
	;; pointer to voice in FSR1
	;; voice number in SID_CURRENT_VOICE

	;; save note into SID_Vx_NOTE (if not in WTO mode) and SID_Vx_PLAYED_NOTE (last one is relevant for MIDI handler)
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	BRA_IFSET SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_WT_ONLY, BANKED, SID_MIDI_M_Hlp_NoteOn_WTO
SID_MIDI_M_Hlp_NoteOn_NotWTO
	movlw	SID_Vx_NOTE
	movff	PRODL, PLUSW1
SID_MIDI_M_Hlp_NoteOn_WTO
	movlw	SID_Vx_PLAYED_NOTE
	movff	PRODL, PLUSW1

	;; sus-key activated?
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_SUS_KEY, BANKED, SID_MIDI_M_Hlp_NoteOn_NoSusKey
SID_MIDI_M_Hlp_NoteOn_SusKey
	;; in suy-key mode, we activate portamento only if at least one key is played
	movlw	SID_MVx_NOTE_STACK_PTR
	decf	PLUSW0, W
	bz	SID_MIDI_M_Hlp_NoteOn_NoSusKeyPr
SID_MIDI_M_Hlp_NoteOn_NoSusKey
	;; omit portamento if first key played after patch initialisation
	movlw	SID_Vx_STATE2
	BRA_IFCLR PLUSW1, SID_V_STATE2_PORTA_INITIALIZED, ACCESS, SID_MIDI_M_Hlp_NoteOn_NoSusKeyPr
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
SID_MIDI_M_Hlp_NoteOn_NoSusKeyPr

	;; next key will allow portamento
	movlw	SID_Vx_STATE2
	bsf	PLUSW1, SID_V_STATE2_PORTA_INITIALIZED

	;; skip the rest if legato and voice already active
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_LEGATO, BANKED, SID_MIDI_M_Hlp_NoteOn_NoLegato
SID_MIDI_M_Hlp_NoteOn_Legato
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_M_Hlp_NoteOn_NoGate
SID_MIDI_M_Hlp_NoteOn_NoLegato

	;; request gate bit
	rcall	SID_MIDI_M_Hlp_GateOn

SID_MIDI_M_Hlp_NoteOn_NoGate

	;; don't sync if WTO mode or legato mode and current note is first note
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_LEGATO, BANKED, SID_MIDI_M_Hlp_NoteOn_NOnTrg
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	movf	SID_MIDI_PARAMETER1, W, BANKED
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_M_Hlp_NoteOn_NoNOnTrg
	movlw	SID_MVx_NOTE_STACK_1
	movf	PLUSW0, W
	bnz	SID_MIDI_M_Hlp_NoteOn_NoNOnTrg
SID_MIDI_M_Hlp_NoteOn_NOnTrg
	;; TODO: ??? removed code???
SID_MIDI_M_Hlp_NoteOn_NoNOnTrg

	;; save voice number in midivoice (instrument) record
	movlw	SID_MVx_LAST_VOICE
	movff	SID_CURRENT_VOICE, PLUSW0

	;; save midivoice (instrument) number in voice record
	movlw	SID_Vx_ASSIGNED_MV
	movff	SID_CURRENT_MIDIVOICE, PLUSW1

	return

;; --------------------------------------------------------------------------
;; Note Off help function
;; IN: Pointer to SID_MVx_BASE in FSR0
;; OUT: returns 0x01 if gate should be retriggered (mono mode, more than one note was played)
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!
	movwf	TABLAT

	;; always gate off and no new note on if in poly mode
	BRA_IFSET SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_POLY, BANKED, SID_MIDI_M_Hlp_NoteOff_GOff

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	movf	TABLAT, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_M_Hlp_NoteOff_End
	BRA_IFSET SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_LEGATO, BANKED, SID_MIDI_M_Hlp_NoteOff_NoGOff
	rcall	SID_MIDI_M_Hlp_GateOff
SID_MIDI_M_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if there is still a note in the stack, play new note with NoteOn function (checked by caller)
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bz	SID_MIDI_M_Hlp_NoteOff_GOff

	;; activate portamento (will be ignored by Pitch handler if no portamento active - important for SusKey function to have it here!)
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE

	retlw	0x01		; return, request Note On!

SID_MIDI_M_Hlp_NoteOff_GOff
	;; else request gate clear bit
	rcall	SID_MIDI_M_Hlp_GateOff

SID_MIDI_M_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!


;; --------------------------------------------------------------------------
;; Gate On help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     voice number in SID_CURRENT_VOICE
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_GateOn
	;; set "voice active" flag
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE

	BRA_IFSET SID_MIDI_FLAGS, SID_I_L_FLAGS1_WT_ONLY, BANKED, SID_MIDI_M_Hlp_GateOn_End

	;; request gate
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED

	;; request LFO re-sync and WT reset
	movf	SID_CURRENT_VOICE, W, BANKED
	addlw	2
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED

	;; ENV attack phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVA, F, BANKED

SID_MIDI_M_Hlp_GateOn_End
	return

;; --------------------------------------------------------------------------
;; Gate Off help function
;; IN: instrument in SID_CURRENT_INSTRUMENT
;;     note in SID_MIDI_PARAMETER1
;;     pointer to MIDI voice in FSR0
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_GateOff
	;; go through all voices which are assigned to the current instrument and note
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_MIDI_M_Hlp_GateOff_Loop
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_M_Hlp_GateOff_Loop_Next
	movlw	SID_Vx_ASSIGNED_MV
	movf	PLUSW1, W
	cpfseq	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_M_Hlp_GateOff_Loop_Next
	movlw	SID_Vx_PLAYED_NOTE
	movf	PLUSW1, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_M_Hlp_GateOff_Loop_Next

	;; release voice
	call	SID_VOICE_Release

	;; request gate off if not disabled via trigger matrix
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; request gate clear
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; ENV release phase via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_SE_TRG_EVNT_ENVR, F, BANKED

SID_MIDI_M_Hlp_GateOff_Loop_Next
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_MIDI_M_Hlp_GateOff_Loop

	return	


;; --------------------------------------------------------------------------
;; Help function which requests a new voice
;; IN: Pointer to SID_MVx_BASE in FSR0
;;     MIDI voice number in SID_CURRENT_MIDIVOICE
;; OUT: pointer to voice in FSR1
;;      voice number in SID_CURRENT_VOICE
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_GetVoice
	;; save current frequency of last voice in TMP[12]
	;; (for proper portamento in poly mode)
	lfsr	FSR1, SIDL_V1_BASE
	movlw	SID_MVx_LAST_VOICE
	movf	PLUSW0, W
	movwf	SID_SE_ELEMENT_NUM	; we know that interrupts are disabled here --- hopefully!
	mullw	SID_Vx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movlw	SID_Vx_OLD_TARGET_FRQ_L
	movff	PLUSW1, TMP1
	movlw	SID_Vx_OLD_TARGET_FRQ_H
	movff	PLUSW1, TMP2
	movlw	SID_Vx_OLD_TRANSP_NOTE
	movff	PLUSW1, TMP3
	movlw	SID_Vx_LINEAR_FRQ_L
	movff	PLUSW1, TMP4
	movlw	SID_Vx_LINEAR_FRQ_H
	movff	PLUSW1, TMP5

	;; get voice assignment mode for SID_VOICE_Get(Last) function
	rcall	SID_MIDI_M_Hlp_FSR2_Ins
	movlw	SID_Ix_M_Vx_VOICE_ASSGN
	movf	PLUSW2, W
	andlw	0x0f
	movwf	SID_CURRENT_VOICE_ASSG, BANKED

	;; number of voices depends on stereo/mono mode
	SET_BSR	SID_LOCAL_ENS
	movlw	0x60
	btfsc	SID_LOCAL_ENS + SID_ENSx_CTRL1, SID_ENS_CTRL1_MONO, BANKED
	movlw	0x30
	SET_BSR	SID_BASE
	iorwf	SID_CURRENT_VOICE_ASSG, F, BANKED

	;; branch depending on Poly/Mono mode
	;; if ARP is active, always use mono mode approach for voice allocation
	movlw	SID_Ix_Vx_ARP_MODE
	BRA_IFSET PLUSW2, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_M_Hlp_NoteOn_Mono

	BRA_IFCLR SID_MIDI_FLAGS, SID_I_M_V_FLAGS2_POLY, BANKED, SID_MIDI_M_Hlp_NoteOn_Mono
SID_MIDI_M_Hlp_NoteOn_Poly
	;; in poly mode we prefer to get a voice which is free or was played the longest time
	call	SID_VOICE_Get
	rgoto	SID_MIDI_M_Hlp_NoteOn_Cont

SID_MIDI_M_Hlp_NoteOn_Mono
	;; search the last played voice (if still available - if not it doesn't matter)
	movlw	SID_MVx_LAST_VOICE
	movf	PLUSW0, W
	call	SID_VOICE_GetLast
	;; 	rgoto	SID_MIDI_M_Hlp_NoteOn_Cont

SID_MIDI_M_Hlp_NoteOn_Cont
	;; pointer to voice in FSR1
	;; voice number in SID_CURRENT_VOICE

	;; write back current frequency
	movlw	SID_Vx_OLD_TARGET_FRQ_L
	movff	TMP1, PLUSW1
	movlw	SID_Vx_OLD_TARGET_FRQ_H
	movff	TMP2, PLUSW1
	movlw	SID_Vx_OLD_TRANSP_NOTE
	movff	TMP3, PLUSW1
	movlw	SID_Vx_LINEAR_FRQ_L
	movff	TMP4, PLUSW1
	movlw	SID_Vx_LINEAR_FRQ_H
	movff	TMP5, PLUSW1
	movlw	SID_Vx_STATE2
	bsf	PLUSW1, SID_V_STATE2_FORCE_FRQ_RECALC
	
	return


;; --------------------------------------------------------------------------
;; Help function which returns a pointer to the assigned instrument
;; of a MIDI voice in FSR2
;; IN: SID_CURRENT_MIDIVOICE
;; OUT: pointer to instrument in FSR2
;; --------------------------------------------------------------------------
SID_MIDI_M_Hlp_FSR2_Ins
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_M_I1_BASE
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_Ix_M_I2_BASE-SID_Ix_M_I1_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	return
