; $Id$
;
; MIDIbox SID
; MIDI Interface part for Lead Engine
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; enable this switch for superpoly handling as described in MIDIbox forum
#define SUPERPOLY_EXPERIMENT 1

;; --------------------------------------------------------------------------
;;  This function is called to forward a Note On event to the synthesizer
;;  Input:
;;     o MIDI Voice in SID_CURRENT_MIDI_VOICE
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_L_NoteOn
	SET_BSR	SID_BASE

	;; lead engine works only with settings of first MIDI voice
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_L_NoteOn_End

#if SUPERPOLY_EXPERIMENT
	;; enabled when SID_ENSx_SUPERPOLY_CTRL[2:0] > 0 and number <= SID number
	movff	SID_LOCAL_ENS + SID_ENSx_SUPERPOLY_CTRL, WREG
	andlw	0x07
	bz	SID_MIDI_L_NoteOn_NoSuperPoly
	cpfslt	SID_MIDI_DEVICE, ACCESS
	rgoto SID_MIDI_L_NoteOn_NoSuperPoly
SID_MIDI_L_NoteOn_SuperPoly
	;; get the next free voice
	swapf	WREG, W				; use default voice assignment mode, number of voices given by SUPERPOLY_CTRL[2:0], copied to VOICE_ASSG[6:4]
	movwf	SID_CURRENT_VOICE_ASSG, BANKED	
	call	SID_VOICE_Get			; call voice allocation handler, new voice in SID_CURRENT_VOICE
	;; store note number of voice locally (required for release mechanism)
	lfsr	FSR1, SID_SUPERPOLY_VOICE_NOTE_TAB
	movf	SID_CURRENT_VOICE, W, BANKED
	movff	SID_MIDI_PARAMETER1, PLUSW1
	;; exit if voice doesn't match with device number
	movf	SID_CURRENT_VOICE, W, BANKED
	cpfseq	SID_MIDI_DEVICE, ACCESS
	rgoto SID_MIDI_L_NoteOn_End
	;; note stack has to be emptied to avoid "hanging notes" - the final superpoly
	;; implementation has to stack *all* incoming notes, not only the ones played by the SID voice
	SET_BSR	SID_MV1_BASE
	clrf	SID_MV1_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV2_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV3_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV4_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV5_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV6_BASE + SID_MVx_NOTE_STACK_PTR, BANKED
	clrf	SID_MV1_BASE + SID_MVx_NOTE_STACK_0, BANKED
	clrf	SID_MV2_BASE + SID_MVx_NOTE_STACK_0, BANKED
	clrf	SID_MV3_BASE + SID_MVx_NOTE_STACK_0, BANKED
	clrf	SID_MV4_BASE + SID_MVx_NOTE_STACK_0, BANKED
	clrf	SID_MV5_BASE + SID_MVx_NOTE_STACK_0, BANKED
	clrf	SID_MV6_BASE + SID_MVx_NOTE_STACK_0, BANKED
	SET_BSR	SID_BASE
SID_MIDI_L_NoteOn_NoSuperPoly
#endif

	;; copy note number and velocity into mod matrix source
	;; convert key number to 15bit signed value, centered at 60 (middle-C)
	movf	SID_MIDI_PARAMETER1, W
	addlw	-60
	movwf	SID_MOD_SRC_L_KEY_H, BANKED
	movlw	0x00
	btfsc	SID_MOD_SRC_L_KEY_H, 7, BANKED
	movlw 0xff
	movwf	SID_MOD_SRC_L_KEY_L, BANKED

	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_VEL
	call	SID_KNOB_SetValue

	;; copy patch flags to temporary register SID_MIDI_FLAGS
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_FLAGS1, SID_MIDI_FLAGS

	;; go through all voices
	lfsr	FSR0, SID_MV1_BASE
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_MIDI_L_NoteOn_Loop
	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE + SID_Ix_Vx_ARP_MODE
	movf	SID_CURRENT_VOICE, W, BANKED
	mullw	SID_Ix_L_S1V2_BASE-SID_Ix_L_S1V1_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movff	INDF2, TMP1

	;; push note into WT stack
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_VOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	call	SID_MIDI_Hlp_PushWT

	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F

	;; branch depending on Normal/Arp mode
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_L_NoteOn_Loop_Arp
SID_MIDI_L_NoteOn_Loop_Norm
	;; push note into stack
	call	SID_MIDI_Hlp_PushNote

	;; switch off gate if not in legato or WTO mode
	BRA_IFSET SID_MIDI_FLAGS, SID_I_L_FLAGS1_WT_ONLY, BANKED, SID_MIDI_L_NoteOn_Ok_NoGateOff
SID_MIDI_L_NoteOn_Ok_GateOff
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_L_FLAGS1_LEGATO, BANKED, SID_MIDI_L_Hlp_GateOff
SID_MIDI_L_NoteOn_Ok_NoGateOff

	;; call note-on handler
	rcall	SID_MIDI_L_Hlp_NoteOn
	rgoto	SID_MIDI_L_NoteOn_Loop_Next

	
SID_MIDI_L_NoteOn_Loop_Arp
	call	SID_MIDI_Hlp_ArpNoteOn
	;; 	rgoto	SID_MIDI_L_NoteOn_Loop_Next
	
SID_MIDI_L_NoteOn_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_MIDI_L_NoteOn_Loop

SID_MIDI_L_NoteOn_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note Off event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_L_NoteOff
	SET_BSR	SID_BASE

	;; lead engine works only with settings of first MIDI voice
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE

	;; check if MIDI channel and split zone matches
	call	SID_MIDI_Hlp_ChkChnAndSplit
	bnz	SID_MIDI_L_NoteOff_End

#if SUPERPOLY_EXPERIMENT
	;; enabled when SID_ENSx_SUPERPOLY_CTRL[2:0] > 0 and number <= SID number
	movff	SID_LOCAL_ENS + SID_ENSx_SUPERPOLY_CTRL, WREG
	andlw	0x07
	bz	SID_MIDI_L_NoteOff_NoSuperPoly
	cpfslt	SID_MIDI_DEVICE, ACCESS
	rgoto SID_MIDI_L_NoteOff_NoSuperPoly
SID_MIDI_L_NoteOff_SuperPoly
	;; check MIDI note -> voice assignment
	lfsr	FSR1, SID_SUPERPOLY_VOICE_NOTE_TAB
	clrf	SID_CURRENT_VOICE, BANKED
SID_MIDI_L_NoteOff_SuperPolyLoop
	movf	INDF1, W
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	bz	SID_MIDI_L_NoteOff_SP_NoteFound
	movf	POSTINC1, W			; switch to next table entry
	incf	SID_CURRENT_VOICE, F, BANKED	; switch to next voice
	BRA_IFCLR SID_CURRENT_VOICE, 2, BANKED, SID_MIDI_L_NoteOff_SuperPolyLoop
	rgoto	SID_MIDI_L_NoteOff_End		; note not played

SID_MIDI_L_NoteOff_SP_NoteFound
	clrf	INDF1				; clear note number (so, it's free again)

	;; release voice
	call	SID_VOICE_Release	; expects voice number in SID_CURRENT_VOICE

	;; exit if note is not played by "my" voice
	movf	SID_CURRENT_VOICE, W, BANKED
	cpfseq	SID_MIDI_DEVICE, ACCESS
	rgoto SID_MIDI_L_NoteOff_End
SID_MIDI_L_NoteOff_NoSuperPoly
#endif

	;; copy patch flags to temporary register SID_MIDI_FLAGS
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_FLAGS1, SID_MIDI_FLAGS

	;; go through all voices
	lfsr	FSR0, SID_MV1_BASE
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_MIDI_L_NoteOff_Loop
	;; SID_Ix_Vx_ARP_MODE of instrument -> TMP1
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW + SID_Ix_L_S1V1_BASE + SID_Ix_Vx_ARP_MODE
	movf	SID_CURRENT_VOICE, W, BANKED
	mullw	SID_Ix_L_S1V2_BASE-SID_Ix_L_S1V1_BASE
	movf	PRODL, W
	addwf	FSR2L, F
	movff	INDF2, TMP1

	;; pop note from WT stack if sustain function not active
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_WT_STACK_0
	movf	SID_CURRENT_VOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	CALL_IFCLR SID_MIDIVOICE_SUSTAIN, 0, BANKED, SID_MIDI_Hlp_PopWT

	;; determine pointer to note stack -> FSR2
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F
	movff	INDF2, TMP3	; save current #0 entry in TMP3 for later use

	;; if sustain active: mark note, but don't stop it
	;; this works for normal and arp mode
	BRA_IFCLR SID_MIDIVOICE_SUSTAIN, 0, BANKED, SID_MIDI_L_NoteOff_Loop_NoSus
	;; if not in arp mode: sustain only relevant if only one active note in stack
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_L_NoteOff_Loop_Sus
	clrf	PRODL		; loop counter
	clrf	PRODH		; active note counter
SID_MIDI_L_NoteOff_Loop_SusChkLp
	movf	PRODL, W
	movf	PLUSW2, F
	bz	SID_MIDI_L_NoteOff_Loop_SusCLpNx
	btfss	PLUSW2, 7
	incf	PRODH, F
SID_MIDI_L_NoteOff_Loop_SusCLpNx
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_L_NoteOff_Loop_SusChkLp
	;; no special sustain if more than one note is played
	movlw	2
	cpfslt	PRODH, ACCESS
	rgoto SID_MIDI_L_NoteOff_Loop_NoSus
SID_MIDI_L_NoteOff_Loop_Sus
	rcall	SID_MIDI_Hlp_PopArpHold
	rgoto	SID_MIDI_L_NoteOff_Loop_Next
SID_MIDI_L_NoteOff_Loop_NoSus

	;; branch depending on Normal/Arp mode
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_ENABLE, ACCESS, SID_MIDI_L_NoteOff_Loop_Arp
SID_MIDI_L_NoteOff_Loop_Norm

	;; pop note from stack (pointer to stack in FSR2)
	rcall	SID_MIDI_Hlp_PopNote

	movf	TMP3, W		; restore note
	rcall	SID_MIDI_L_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_L_Hlp_NoteOn
	rgoto	SID_MIDI_L_NoteOff_Loop_Next


SID_MIDI_L_NoteOff_Loop_Arp
	call	SID_MIDI_Hlp_ArpNoteOff
	;; 	rgoto	SID_MIDI_L_NoteOff_Loop_Next
	
SID_MIDI_L_NoteOff_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_MIDI_L_NoteOff_Loop

SID_MIDI_L_NoteOff_End
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a PitchBender event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o 8bit PitchBender value in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_L_PitchBender
	;; nothing else to do (pitchbender knob already handled in sid_midi.inc)
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a CC event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o CC number in SID_MIDI_PARAMETER1
;;     o CC value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_L_CC
	SET_BSR	SID_BASE

	;; exit if MIDI channel doesn't match (only check for first MIDI voice)
	movff	SID_MV1_BASE + SID_MVx_MIDI_CHANNEL, WREG
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_L_End

	;; if CC#06 (NRPN data LSB) received, forward to parameter handler
	movlw	0x06
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_L_CC_NoNRPNDataH
SID_MIDI_L_CC_NRPNDataH
	;; prepare MIOS_PARAMETER3 (selection options)
	clrf	MIOS_PARAMETER3		; (L/R selection done in NRPN function)
	call	SID_PARIN_SetNRPN
	rgoto	SID_MIDI_L_End
SID_MIDI_L_CC_NoNRPNDataH

	;; if CC#64 (Sustain) received, set/clear sustain flags and release notes if required
	movlw	0x40
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_L_CC_Not64
SID_MIDI_L_CC_64
	;; sustain enabled if CC value >= 0x40
	BRA_IFSET SID_MIDI_PARAMETER2, 6, BANKED, SID_MIDI_L_CC_64_1
SID_MIDI_L_CC_64_0
	;; clear all flags and disable all notes
	clrf	SID_MIDIVOICE_SUSTAIN, BANKED
	;; go through all voices
	lfsr	FSR0, SID_MV1_BASE
	lfsr	FSR1, SIDL_V1_BASE
	clrf	SID_CURRENT_VOICE, BANKED
SID_MIDI_L_CC_64_0_Loop
	;; clear WT stack
	;; (TODO: this needs some more work - it would be nice if the WT sequencer would
	;; still play when sustain is released for continous sequences)
	movff	FSR0H, FSR2H
	movlw	SID_MVx_WT_STACK_3
	addwf	FSR0L, W
	movwf	FSR2L
	clrf	POSTDEC2
	clrf	POSTDEC2
	clrf	POSTDEC2
	clrf	INDF2
	;; clear all non-active notes (which have been released during sustain was active)
	movlw	SID_MVx_NOTE_STACK_0 - SID_MVx_WT_STACK_0
	addwf	FSR2L, F
	rcall	SID_MIDI_Hlp_ClrNonActive
	;; initiate note off
	movlw	SID_Vx_PLAYED_NOTE	; dirty: fake note --- SID_MIDI_PARAMETER1 has to be overwritten!!!
	movf	PLUSW1, W
	movwf	SID_MIDI_PARAMETER1, BANKED
	;; (skip if note hasn't been changed)
	movlw	SID_MVx_NOTE_STACK_0
	movf	PLUSW0, W
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	bz	SID_MIDI_L_CC_64_0_Loop_Next
	movf	SID_MIDI_PARAMETER1, W, BANKED
	rcall	SID_MIDI_L_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_L_Hlp_NoteOn
SID_MIDI_L_CC_64_0_Loop_Next
	movlw	SID_MVx_RECORD_LEN
	addwf	FSR0L, F
	movlw	SID_Vx_RECORD_LEN
	addwf	FSR1L, F
	incf	SID_CURRENT_VOICE, F, BANKED
	movlw	SID_Vx_NUM - 1
	cpfsgt	SID_CURRENT_VOICE, BANKED
	rgoto SID_MIDI_L_CC_64_0_Loop

	rgoto	SID_MIDI_L_CC_64_End
SID_MIDI_L_CC_64_1
	;; set first flag - notes won't be removed from note stack anymore until sustain is disabled again
	movlw	0x01
	movwf	SID_MIDIVOICE_SUSTAIN, BANKED
	;; 	rgoto	SID_MIDI_L_CC_64_End

SID_MIDI_L_CC_64_End
SID_MIDI_L_CC_Not64

SID_MIDI_L_End
	return


;; --------------------------------------------------------------------------
;;  help routines for lead engine
;; --------------------------------------------------------------------------


;; --------------------------------------------------------------------------
;; Note On help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     voice number in SID_CURRENT_VOICE
;; ALSO USED BY SID_PATCH_Init !
;; --------------------------------------------------------------------------
SID_MIDI_L_Hlp_NoteOn
	;; save note into SID_Vx_NOTE (if not in WTO mode) and SID_Vx_PLAYED_NOTE (last one is relevant for MIDI handler)
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	bcf	PRODL, 7	; (ensure that non-active flag is cleared)
	BRA_IFSET SID_MIDI_FLAGS, SID_I_L_FLAGS1_WT_ONLY, BANKED, SID_MIDI_L_Hlp_NoteOn_WTO
SID_MIDI_L_Hlp_NoteOn_NotWTO
	movlw	SID_Vx_NOTE
	movff	PRODL, PLUSW1
SID_MIDI_L_Hlp_NoteOn_WTO
	movlw	SID_Vx_PLAYED_NOTE
	movff	PRODL, PLUSW1

	;; sus-key activated?
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_L_FLAGS1_SUS_KEY, BANKED, SID_MIDI_L_Hlp_NoteOn_NoSusKey
SID_MIDI_L_Hlp_NoteOn_SusKey
	;; in suy-key mode, we activate portamento only if voice not already active
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_L_Hlp_NoteOn_NoSusKeyPr
SID_MIDI_L_Hlp_NoteOn_NoSusKey
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_PORTA_ACTIVE
SID_MIDI_L_Hlp_NoteOn_NoSusKeyPr

	;; skip the rest if legato and voice already active
	BRA_IFCLR SID_MIDI_FLAGS, SID_I_L_FLAGS1_LEGATO, BANKED, SID_MIDI_L_Hlp_NoteOn_NoLegato
SID_MIDI_L_Hlp_NoteOn_Legato
	movlw	SID_Vx_STATE
	BRA_IFSET PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_L_Hlp_NoteOn_End
SID_MIDI_L_Hlp_NoteOn_NoLegato

	;; request gate bit
	rcall	SID_MIDI_L_Hlp_GateOn

	;; don't sync if legato mode and current note is first note
	RCALL_IFCLR SID_MIDI_FLAGS, SID_I_L_FLAGS1_LEGATO, BANKED, SID_MIDI_L_Hlp_NoteOn_NOnTrg
	movlw	SID_MVx_NOTE_STACK_0
	movff	PLUSW0, PRODL
	movf	SID_MIDI_PARAMETER1, W, BANKED
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_L_Hlp_NoteOn_NoNOnTrg
	movlw	SID_MVx_NOTE_STACK_1
	movf	PLUSW0, W
	bnz	SID_MIDI_L_Hlp_NoteOn_NoNOnTrg
SID_MIDI_L_Hlp_NoteOn_NOnTrg

	;; WTO mode: only trigger WT reset/step sync
	BRA_IFSET SID_MIDI_FLAGS, SID_I_L_FLAGS1_WT_ONLY, BANKED, SID_MIDI_L_Hlp_NoteOn_NOnTrg_WTO

	;; determine mask for Note On
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorlw	0xc0
	movwf	PRODL
	;; ENV/LFO/... synchronisation via trigger matrix
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 0, WREG
	andwf	PRODL, W	; only selected voice can trigger NoteOn
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED

SID_MIDI_L_Hlp_NoteOn_NOnTrg_WTO
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOn_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SID_MIDI_L_Hlp_NoteOn_NoNOnTrg

SID_MIDI_L_Hlp_NoteOn_End
	return


;; --------------------------------------------------------------------------
;; Note Off help function
;; IN: Pointer to SID_MVx_BASE in FSR0
;; OUT: returns 0x01 if gate should be retriggered (mono mode, more than one note was played)
;; --------------------------------------------------------------------------
SID_MIDI_L_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!
	movwf	TABLAT

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	movf	TABLAT, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_L_Hlp_NoteOff_End
	BRA_IFSET SID_MIDI_FLAGS, SID_I_L_FLAGS1_LEGATO, BANKED, SID_MIDI_L_Hlp_NoteOff_NoGOff
	rcall	SID_MIDI_L_Hlp_GateOff
SID_MIDI_L_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if there is still a note in the stack, play new note with NoteOn function (checked by caller)
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	skpz
	retlw	0x01		; return, request Note On!

SID_MIDI_L_Hlp_NoteOff_GOff
	;; else request gate clear bit
	rcall	SID_MIDI_L_Hlp_GateOff

	;; ENV/LFO/... synchronisation via trigger matrix
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 0, WREG
	andlw	0xc0		; (Gates handled seperately in SID_MIDI_L_Hlp_GateOff)
	iorwf	SID_SE_TRG_EVNT_L, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 1, WREG
	iorwf	SID_SE_TRG_EVNT_H, F, BANKED
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 2, WREG
	iorwf	SID_SE_TRG_EVNT_U, F, BANKED
SID_MIDI_L_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!


;; --------------------------------------------------------------------------
;; Gate On help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     voice number in SID_CURRENT_VOICE
;; --------------------------------------------------------------------------
SID_MIDI_L_Hlp_GateOn
	;; set "voice active" flag
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE

SID_MIDI_L_Hlp_GateOn_End
	return

;; --------------------------------------------------------------------------
;; Gate Off help function
;; IN: pointer to voice in FSR1, pointer to MIDI voice in FSR0
;;     voice in SID_CURRENT_VOICE
;;     note in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_L_Hlp_GateOff
	;; nothing to do if voice not active anymore
	movlw	SID_Vx_STATE
	BRA_IFCLR PLUSW1, SID_V_STATE_VOICE_ACTIVE, ACCESS, SID_MIDI_L_Hlp_GateOff_End

#if 0
	;; clashes with sustain function (different note is played when still held after sustain-off)
	;; don't know why this check should be relevant for lead engine, it's required for multi engine
	;; maybe an artefact...
	movlw	SID_Vx_PLAYED_NOTE
	movf	PLUSW1, W
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_L_Hlp_GateOff_End
#endif

	;; request gate off if not disabled via trigger matrix
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
	bcf	PLUSW1, SID_V_STATE_GATE_SET_REQ

	;; request gate clear via trigger matrix
	movf	SID_CURRENT_VOICE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	PRODL
	SET_BSR	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE
	andwf	SID_PATCH_BUFFER_SHADOW + SID_Ix_L_TRG_NOf_BASE + 0, W, BANKED
	SET_BSR	SID_BASE
	movlw	SID_Vx_STATE
	skpz
	bsf	PLUSW1, SID_V_STATE_GATE_CLR_REQ

	;; remove gate set request
	comf	PRODL, W
	andwf	SID_SE_TRG_EVNT_L, F, BANKED

SID_MIDI_L_Hlp_GateOff_End
	return	
