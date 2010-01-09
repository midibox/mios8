; $Id$
;
; MIDIbox SID
; MIDI Interface part for Drum Engine
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
SID_MIDI_D_NoteOn

	;; drum engine works only with settings of first MIDI voice
	;; note that SID_CURRENT_MIDIVOICE has a different meaning here.
	;; It doesn't select the MVx record, but identifies the drum instrument
	lfsr	FSR0, SID_MV1_BASE

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_D_NoteOn_End

	;; push note into stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_PushNote

	;; normalize note (1..24) -> SID_CURRENT_MIDIVOICE
	movf	SID_MIDI_PARAMETER1, W
	rcall	SID_MIDI_D_Hlp_GetNote
	movwf	SID_CURRENT_MIDIVOICE, BANKED

	;; branch depending on SEQ/Normal mode
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	andlw	(1 << SID_I_V_SEQ_ON)
	bnz	SID_MIDI_D_NoteOn_Seq
SID_MIDI_D_NoteOn_Norm
	;; ignore notes >= 17
	movlw	17
	cpfslt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_D_NoteOn_End

	;; forward velocity to Knob handler
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_VEL
	call	SID_KNOB_SetValue

	;; dedicated velocity assignment for instrument
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER2	; high byte
	clrc
	rlf	MIOS_PARAMETER2, F
	clrf	MIOS_PARAMETER1				; low byte
	decf	SID_CURRENT_MIDIVOICE, W, BANKED	; current instrument
	swapf	WREG, W
	andlw	0xf0
	iorlw	0x03
	movwf	MIOS_PARAMETER3
	call	SID_SE_D_Hlp_FSR2_Ins			; pointer to instrument -> FSR2
	movlw	SID_Ix_Dx_VELOCITY_ASSGN		; velocity assignment
	movf	PLUSW2, W
	call	SID_PARIN_Set16

	;; call note-on handler
	movff	SID_MIDI_PARAMETER2, MIOS_PARAMETER2	; expects velcoity in MIOS_PARAMETER2 to control accent flag
	call	SID_SE_D_NoteOn		; also used by sound engine, therefore located there
	rgoto	SID_MIDI_D_NoteOn_End

SID_MIDI_D_NoteOn_Seq
	;; forward velocity to Knob handler
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_VEL
	call	SID_KNOB_SetValue

	;; reset sequencer if voice was not active before

	;; only used in master mode
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SID_MIDI_D_NoteOn_Seq_NoR

	movff	SID_SEQ1_BASE + SID_SEQx_MISC, WREG
	BRA_IFSET WREG, SID_SEQ_MISC_SEQ_RUNNING, ACCESS, SID_MIDI_D_NoteOn_Seq_NoR
SID_MIDI_D_NoteOn_Seq_R
	movlw	0x40
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SID_MIDI_D_NoteOn_Seq_NoR

	;; select new sequence, ensure that sequence number is within one octave
	;; (it's already between two octaves)
	decf	SID_CURRENT_MIDIVOICE, W, BANKED
	movwf	PRODL
	movlw	12-1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_D_NoteOn_Seq_OctOk
SID_MIDI_D_NoteOn_Seq_OctFix
	movlw	-12
	addwf	PRODL, F
SID_MIDI_D_NoteOn_Seq_OctOk
	movf	PRODL, W
	rcall	SID_MIDI_D_Hlp_SetSeq

	;; enable sequencer
	SET_BSR	SID_SEQ1_BASE
	bsf	SID_SEQ1_BASE + SID_SEQx_MISC, SID_SEQ_MISC_SEQ_RUNNING, BANKED
	SET_BSR	SID_BASE

	;; 	rgoto	SID_MIDI_D_NoteOn_End

SID_MIDI_D_NoteOn_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note Off event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_D_NoteOff
	SET_BSR	SID_BASE

	;; drum engine works only with settings of first MIDI voice
	;; note that SID_CURRENT_MIDIVOICE has a different meaning here.
	;; It doesn't select the MVx record, but identifies the drum instrument
	lfsr	FSR0, SID_MV1_BASE

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_D_NoteOff_End

	;; branch depending on SEQ/Normal mode
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_SPEED, WREG
	andlw	(1 << SID_I_V_SEQ_ON)
	bnz	SID_MIDI_D_NoteOff_Seq

SID_MIDI_D_NoteOff_Norm
	;; pop note from stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_PopNote
	;; 	bnz	SID_MIDI_D_NoteOff_End ; ZERO flag cleared: note not found!
	;; not here! we are able to play more drums than note stack depth!

	;; normalize note (1..24) -> SID_CURRENT_MIDIVOICE
	movf	SID_MIDI_PARAMETER1, W
	rcall	SID_MIDI_D_Hlp_GetNote
	movwf	SID_CURRENT_MIDIVOICE, BANKED

	;; ignore notes >= 17
	movlw	17
	cpfslt	SID_CURRENT_MIDIVOICE, BANKED
	rgoto SID_MIDI_D_NoteOff_End

	;; call note-off handler
	call	SID_SE_D_NoteOff		; also used by sound engine, therefore located there
	rgoto	SID_MIDI_D_NoteOff_End

SID_MIDI_D_NoteOff_Seq

	;; pop note from stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_NOTE_STACK_0
	call	SID_MIDI_Hlp_PopNote
	bnz	SID_MIDI_D_NoteOff_End ; ZERO flag cleared: note not found!

	;; select sequence if there is still a note in stack
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bz	SID_MIDI_D_NoteOff_Seq_NoNew
SID_MIDI_D_NoteOff_Seq_New
	rcall	SID_MIDI_D_Hlp_GetNote
	addlw	-1
	rcall	SID_MIDI_D_Hlp_SetSeq
SID_MIDI_D_NoteOff_Seq_NoNew

	;; disable sequencer if invalid sequence is selected (seq off)

	;; only used in master mode
	BRA_IFSET SID_STAT, SID_STAT_CLK_SLAVE, ACCESS, SID_MIDI_D_NoteOff_Loop_Seq_NoDs

	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM, WREG
	andlw	0xf8
	bz	SID_MIDI_D_NoteOff_Loop_Seq_NoDs
SID_MIDI_D_NoteOff_Loop_Seq_Dis
	SET_BSR	SID_SEQ1_BASE
	bcf	SID_SEQ1_BASE + SID_SEQx_MISC, SID_SEQ_MISC_SEQ_RUNNING, BANKED
	SET_BSR	SID_BASE
SID_MIDI_D_NoteOff_Loop_Seq_NoDs

	;; 	rgoto	SID_MIDI_D_NoteOff_End
	
SID_MIDI_D_NoteOff_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a PitchBender event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o 8bit PitchBender value in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_D_PitchBender
	SET_BSR	SID_BASE
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a CC event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o CC number in SID_MIDI_PARAMETER1
;;     o CC value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_D_CC
	SET_BSR	SID_BASE

	;; exit if MIDI channel doesn't match (only check for first MIDI voice)
	movff	SID_MV1_BASE + SID_MVx_MIDI_CHANNEL, WREG
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_D_End

	;; if CC#06 (NRPN data LSB) received, forward to parameter handler
	movlw	0x06
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_D_CC_NoNRPNDataH
SID_MIDI_D_CC_NRPNDataH
	;; prepare MIOS_PARAMETER3 (selection options)
	clrf	MIOS_PARAMETER3		; (L/R selection done in NRPN function)
	;; (current instrument in MIOS_PARAMETER3[7:4] always 0 - thats ok, as all parameters are accessible with alternative number
	call	SID_PARIN_SetNRPN
	rgoto	SID_MIDI_D_End
SID_MIDI_D_CC_NoNRPNDataH

SID_MIDI_D_End
	return


;; --------------------------------------------------------------------------
;;  help routines for Drum Engine
;; --------------------------------------------------------------------------


;; --------------------------------------------------------------------------
;; Help function which returns the normalized note (1..24)
;; IN: MIDI Note in WREG
;; --------------------------------------------------------------------------
SID_MIDI_D_Hlp_GetNote
	;; store MIDI note in PRODL
	movwf	PRODL

	;; add MIDI voice based transpose value (ensure that within range 0x00..0x7f)
	movlw	SID_MVx_TRANSPOSE
	movf	PLUSW0, W
	addlw	-0x40
	BRA_IFSET WREG, 7, ACCESS, SID_MIDI_D_Hlp_GetNote_TrnN
SID_MIDI_D_Hlp_GetNote_TrnP
	addwf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, SID_MIDI_D_Hlp_GetNote_Trn_Cont
	addlw	-12
	rgoto	SID_MIDI_D_Hlp_GetNote_TrnP
SID_MIDI_D_Hlp_GetNote_TrnN
	addwf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, SID_MIDI_D_Hlp_GetNote_Trn_Cont
	movlw	12
	rgoto	SID_MIDI_D_Hlp_GetNote_TrnN
SID_MIDI_D_Hlp_GetNote_Trn_Cont

	;; determine instrument based on note number
	;; remove 2*octave from note
SID_MIDI_D_Hlp_GetNote_RemoveOct
	movlw	24 - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_D_Hlp_GetNote_OctOk
	movlw	-24
	addwf	PRODL, F
	rgoto	SID_MIDI_D_Hlp_GetNote_RemoveOct

SID_MIDI_D_Hlp_GetNote_OctOk

	;; return drum note number
	incf	PRODL, W
	return


;; --------------------------------------------------------------------------
;; Help function which selects the sequence based on played note
;; IN: normalized note in WREG, voice base in FSR1, midi voice base in FSR0
;; --------------------------------------------------------------------------
SID_MIDI_D_Hlp_SetSeq
	;; store drum number in PRODL
	movwf	PRODL

	;; if number >= 8: set to 8 (sequence off)
	movlw	8
	cpfslt	PRODL, ACCESS
	movwf PRODL

	;; set new sequence

	;; patch/shadow buffer
	movff	PRODL, SID_PATCH_BUFFER + SID_Ix_D_SEQ_NUM
	movff	PRODL, SID_PATCH_BUFFER_SHADOW + SID_Ix_D_SEQ_NUM

	;; forward to edit buffer if SID selected
	call	CS_MENU_MS_GetSIDNumber
	xorwf	CS_MENU_EDIT_BUFFER_SID, W
	skpnz
	movff	PRODL, SID_EDIT_BUFFER + SID_Ix_D_SEQ_NUM

	return
