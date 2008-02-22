; $Id$
;
; MIDIbox CV
; MIDI Interface part
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  CV Mode Flags
;; ==========================================================================

CV_STAT_VOICE_ACTIVE	EQU	0
CV_STAT_GATE_SET_REQ	EQU	1
CV_STAT_GATE_CLR_REQ	EQU	2
CV_STAT_CV_UPDATE_REQ	EQU	3
CV_STAT_PORTA_ACTIVE	EQU	4

CV_MIDI_MODE_NOTE		EQU	0x00
CV_MIDI_MODE_VELOCITY		EQU	0x01
CV_MIDI_MODE_AFTERTOUCH		EQU	0x02
CV_MIDI_MODE_CC			EQU	0x03
CV_MIDI_MODE_NRPN		EQU	0x04
CV_MIDI_MODE_PITCHBENDER	EQU	0x05


CV_MIDI_MODE_FLAG_LEGATO	EQU	4 ; if set, legato mode is activated
CV_MIDI_MODE_FLAG_POLY		EQU	5 ; if set, poly mode is enabled


;; --------------------------------------------------------------------------
;;  This function is called by USER_MIDI_NotifyReceivedEvent when a complete 
;;  MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_NotifyReceivedEvent
	;; store the last received MIDI event for the display menu
	movff	MIOS_PARAMETER1, CV_LAST_RX_0
	movff	MIOS_PARAMETER2, CV_LAST_RX_1
	movff	MIOS_PARAMETER3, CV_LAST_RX_2

	;; branch to appr. CV routine depending on received event
	swapf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CV_MIDI_NoteOff
	rgoto	CV_MIDI_NoteOn
	rgoto	CV_MIDI_AfterTouch
	rgoto	CV_MIDI_CC
	rgoto	CV_MIDI_ProgramChange
	rgoto	CV_MIDI_PolyAfterTouch
	rgoto	CV_MIDI_PitchBender
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note On event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_NoteOn
	movf	MIOS_PARAMETER3, W	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	CV_MIDI_NoteOff

	SET_BSR	CV_BASE			; prepare BSR for CV register access

	;; TODO: dynamic voice allocation
	;; currently only the poly mode flag of first voice will be taken into account
	;; voices are allocated in accending order
	;; plan: same handling like MBFM/MBSID (oldest note will be killed first)
	;; we could even allow to define multiple voice queues, e.g. for 4*2 or 2*4 polyphony over different MIDI channels
	movff	CV_C1_MIDI_MODE, CV_MIDI_MODE
	BRA_IFCLR CV_MIDI_MODE, CV_MIDI_MODE_FLAG_POLY, BANKED, CV_MIDI_NoteOn_MonoMode
CV_MIDI_NoteOn_PolyMode
	;; in poly mode we only react on MIDI channel of channel 1!
	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	CV_C1_MIDI_CHANNEL, BANKED
	return
	
	;; in poly mode: determine free voice
	SET_BSR	CV_C1_NOTE_STACK_0
	movlw	0x01
	movf	CV_C1_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x02
	movf	CV_C2_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x04
	movf	CV_C3_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x08
	movf	CV_C4_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x10
	movf	CV_C5_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x20
	movf	CV_C6_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x40
	movf	CV_C7_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	movlw	0x80
	movf	CV_C8_NOTE_STACK_0, F, BANKED
	bz	CV_MIDI_NoteOn_Poly_Cont
	SET_BSR	CV_BASE
	rgoto	CV_MIDI_NoteOn_Poly_Failed
CV_MIDI_NoteOn_Poly_Cont
	SET_BSR	CV_BASE
	movwf	TMP5		; TMP5 contains the voices which should be played
	rgoto	CV_MIDI_NoteOn_Cont

CV_MIDI_NoteOn_MonoMode
	;; check for the assigned MIDI channels, result in TMP5
	rcall	CV_MIDI_GetAssignedChannels
	rcall	CV_MIDI_GetAssignedVoices
	;; leave routine if no voice is assigned to channel
	skpnz
	return

CV_MIDI_NoteOn_Cont

	clrf	CV_CURRENT_CHANNEL, BANKED		; loop counter
CV_MIDI_NoteOn_Loop
	movf	CV_CURRENT_CHANNEL, W, BANKED		; check if voice selected
	call	MIOS_HLP_GetBitORMask
	andwf	TMP5, W
	bz	CV_MIDI_NoteOn_Loop_Next
	lfsr	FSR0, CV_C1_MIDI_MODE			; MIDI mode -> CV_MIDI_MODE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, CV_MIDI_MODE
	lfsr	FSR1, CV_C1_NOTE_STACK_0		; push note to stack
	movf	CV_CURRENT_CHANNEL, W, BANKED
	mullw	CV_NOTE_STACK_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	rcall	CV_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, CV_MIDI_NoteOn_Loop_Next	; exit if note already in stack
	movf	MIOS_PARAMETER2, W			; note which should be disabled
	RCALL_IFCLR CV_MIDI_MODE, CV_MIDI_MODE_FLAG_LEGATO, BANKED, CV_MIDI_Hlp_GateOff; request gate-off if !legato
	rcall	CV_MIDI_Hlp_NoteOn			; call note-on handler
	lfsr	FSR0, CV_C1_VELOCITY			; save velocity
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	MIOS_PARAMETER3, PLUSW0

	;; only if note mode selected: trigger meter counter (for CS display)
	movf	CV_MIDI_MODE, W, BANKED
	andlw	0x0f
	xorlw	CV_MIDI_MODE_NOTE
	bz	CV_MIDI_NoteOn_Loop_Meter
	movf	CV_MIDI_MODE, W, BANKED
	andlw	0x0f
	xorlw	CV_MIDI_MODE_VELOCITY
	bnz	CV_MIDI_NoteOn_Loop_Next
CV_MIDI_NoteOn_Loop_Meter
	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN
	movf	CV_CURRENT_CHANNEL, W, BANKED
	setf	PLUSW0
CV_MIDI_NoteOn_Loop_Next
	incf	CV_CURRENT_CHANNEL, F, BANKED		; loop until last voice (7) reached
	BRA_IFCLR CV_CURRENT_CHANNEL, 3, BANKED, CV_MIDI_NoteOn_Loop

CV_MIDI_NoteOn_Poly_Failed
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note Off event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_NoteOff
	SET_BSR	CV_BASE			; prepare BSR for CV register access

	;; TODO: dynamic voice allocation
	;; currently only the poly mode flag of first voice will be taken into account
	;; voices are allocated in accending order
	;; plan: same handling like MBFM/MBSID (oldest note will be killed first)
	;; we could even allow to define multiple voice queues, e.g. for 4*2 or 2*4 polyphony over different MIDI channels
	movff	CV_C1_MIDI_MODE, CV_MIDI_MODE
	BRA_IFCLR CV_MIDI_MODE, CV_MIDI_MODE_FLAG_POLY, BANKED, CV_MIDI_NoteOff_MonoMode
CV_MIDI_NoteOff_PolyMode
	;; in poly mode, handle all channels
	movlw	0xff
	movwf	TMP5
	rgoto	CV_MIDI_NoteOff_Cont
CV_MIDI_NoteOff_MonoMode
	;; check for the assigned MIDI channels, result in TMP5
	rcall	CV_MIDI_GetAssignedChannels
	;; 	rcall	CV_MIDI_GetAssignedVoices ; (not so optimal if split points modified during notes are played)
	;; leave routine if no voice is assigned to channel
	skpnz
	return

CV_MIDI_NoteOff_Cont

	clrf	CV_CURRENT_CHANNEL, BANKED		; loop counter
CV_MIDI_NoteOff_Loop
	movf	CV_CURRENT_CHANNEL, W, BANKED		; check if voice selected
	call	MIOS_HLP_GetBitORMask
	andwf	TMP5, W
	bz	CV_MIDI_NoteOff_Loop_Next
	lfsr	FSR0, CV_C1_MIDI_MODE			; MIDI mode -> CV_MIDI_MODE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	PLUSW0, CV_MIDI_MODE
	lfsr	FSR1, CV_C1_NOTE_STACK_0	; pop note from stack
	movf	CV_CURRENT_CHANNEL, W, BANKED
	mullw	CV_NOTE_STACK_LEN
	movf	PRODL, W
	addwf	FSR1L, F
	movff	INDF1, TMP3	; save current #0 entry in TMP3 for later use
	rcall	CV_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, CV_MIDI_NoteOff_Loop_Next
	movf	TMP3, W		; restore note
	rcall	CV_MIDI_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, CV_MIDI_Hlp_NoteOn
CV_MIDI_NoteOff_Loop_Next
	incf	CV_CURRENT_CHANNEL, F, BANKED		; loop until last voice (7) reached
	BRA_IFCLR CV_CURRENT_CHANNEL, 3, BANKED, CV_MIDI_NoteOff_Loop

	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a PitchBender event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_PitchBender
	;; extract channel -> TMP1
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

	;; convert pitch bender to signed 16-bit value -> TMP[23]
	clrc
	rlf	MIOS_PARAMETER3, W
	xorlw	0x80
	btfsc	MIOS_PARAMETER2, 6
	iorlw	0x01
	movwf	TMP3

	rlf	MIOS_PARAMETER2, W
	rlf	WREG, W
	andlw	0xfc
	movwf	TMP2

	;; go through all 8 voices
	clrf	TMP4
CV_MIDI_PitchBender_Loop
	lfsr	FSR0, CV_C1_MIDI_CHANNEL
	movf	TMP4, W
	movf	PLUSW0, W
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_PitchBender_Loop_Next

	lfsr	FSR0, CV_C1_PITCHBENDER_L
	movf	TMP4, W
	movff	TMP2, PLUSW0

	lfsr	FSR0, CV_C1_PITCHBENDER_H
	movf	TMP4, W
	movff	TMP3, PLUSW0

	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	TMP4, W
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	CV_MIDI_MODE_PITCHBENDER
	bz	CV_MIDI_PitchBender_Loop_UpReq
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	TMP4, W
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	CV_MIDI_MODE_NOTE
	bnz	CV_MIDI_PitchBender_Loop_Next

CV_MIDI_PitchBender_Loop_UpReq
	lfsr	FSR0, CV_C1_STAT
	movf	TMP4, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN		; trigger meter counter (for CS display)
	movf	TMP4, W
	setf	PLUSW0
CV_MIDI_PitchBender_Loop_Next
	incf	TMP4, F
	BRA_IFCLR TMP4, 3, ACCESS, CV_MIDI_PitchBender_Loop

CV_MIDI_PitchBender_End
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Controller event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_CC
	;; extract channel -> TMP1
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

	;; convert CC number to 16-bit value -> TMP[23]
	clrc
	rlf	MIOS_PARAMETER3, W
	andlw	0xfe
	movwf	TMP3
	clrf	TMP2

	;; go through all 8 voices
	clrf	TMP4
CV_MIDI_CC_Loop
	lfsr	FSR0, CV_C1_MIDI_CHANNEL
	movf	TMP4, W
	movf	PLUSW0, W
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_CC_Loop_Next

	movlw	0x62
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_CC_Loop_NoNRPN_L
CV_MIDI_CC_Loop_NRPN_L
	lfsr	FSR0, CV_C1_NRPN_LSB
	movf	TMP4, W
	movff	MIOS_PARAMETER3, PLUSW0
CV_MIDI_CC_Loop_NoNRPN_L

	movlw	0x63
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_CC_Loop_NoNRPN_H
CV_MIDI_CC_Loop_NRPN_H
	lfsr	FSR0, CV_C1_NRPN_MSB
	movf	TMP4, W
	movff	MIOS_PARAMETER3, PLUSW0
CV_MIDI_CC_Loop_NoNRPN_H

	movlw	0x06
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_CC_Loop_NoNRPN_D_H
CV_MIDI_CC_Loop_NRPN_D_H
	;; only continue if NRPN address matches with selected CC number
	rcall	CV_MIDI_CC_Hlp_CheckNRPN
	bnz	CV_MIDI_CC_Loop_NoNRPN_D_H

	lfsr	FSR0, CV_C1_CC_H
	movf	TMP4, W
	movff	MIOS_PARAMETER3, PLUSW0
	clrc
	rlf	PLUSW0, F

	lfsr	FSR0, CV_C1_STAT
	movf	TMP4, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN		; trigger meter counter (for CS display)
	movf	TMP4, W
	setf	PLUSW0
CV_MIDI_CC_Loop_NoNRPN_D_H

	movlw	0x26
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_CC_Loop_NoNRPN_D_L
CV_MIDI_CC_Loop_NRPN_D_L
	;; only continue if NRPN address matches with selected CC number
	rcall	CV_MIDI_CC_Hlp_CheckNRPN
	bnz	CV_MIDI_CC_Loop_NoNRPN_D_L

	lfsr	FSR0, CV_C1_CC_L
	lfsr	FSR1, CV_C1_CC_H
	movf	TMP4, W
	addwf	FSR0L, F
	addwf	FSR1L, F

	movff	MIOS_PARAMETER3, INDF0
	clrc
	rlf	INDF0, F
	clrc
	rlf	INDF0, F

	bcf	INDF1, 0
	skpnc
	bsf	INDF1, 0

	lfsr	FSR0, CV_C1_STAT
	movf	TMP4, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN		; trigger meter counter (for CS display)
	movf	TMP4, W
	setf	PLUSW0
CV_MIDI_CC_Loop_NoNRPN_D_L

CV_MIDI_CC_Loop_CC
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	TMP4, W
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	CV_MIDI_MODE_CC
	bnz	CV_MIDI_CC_Loop_NoCC

	lfsr	FSR0, CV_C1_CC_NUMBER
	movf	TMP4, W
	movf	PLUSW0, W
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_CC_Loop_NoCC

	lfsr	FSR0, CV_C1_CC_L
	movf	TMP4, W
	movff	TMP2, PLUSW0

	lfsr	FSR0, CV_C1_CC_H
	movf	TMP4, W
	movff	TMP3, PLUSW0

	lfsr	FSR0, CV_C1_STAT
	movf	TMP4, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN		; trigger meter counter (for CS display)
	movf	TMP4, W
	setf	PLUSW0
CV_MIDI_CC_Loop_NoCC
CV_MIDI_CC_Loop_Next
	incf	TMP4, F
	BRA_IFCLR TMP4, 3, ACCESS, CV_MIDI_CC_Loop

CV_MIDI_CC_End
	return


;; --------------------------------------------------------------------------
;; help routine to check if the selected NRPN is valid
;; ZERO flag set if NRPN valid
CV_MIDI_CC_Hlp_CheckNRPN
	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	TMP4, W
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	CV_MIDI_MODE_NRPN
	bnz	CV_MIDI_CC_Hlp_CheckNRPN_End

	lfsr	FSR0, CV_C1_NRPN_MSB		; MSB must be zero
	movf	TMP4, W
	movf	PLUSW0, W
	bnz	CV_MIDI_CC_Hlp_CheckNRPN_End

	lfsr	FSR0, CV_C1_NRPN_LSB		; LSB must match with selected CC number
	movf	TMP4, W
	movff	PLUSW0, TMP5
	lfsr	FSR0, CV_C1_CC_NUMBER
	movf	PLUSW0, W
	xorwf	TMP5, W
CV_MIDI_CC_Hlp_CheckNRPN_End
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Program Change event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second  MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
CV_MIDI_ProgramChange
#if DEFAULT_PATCH_CHANGE_CHANNEL
	;; check for MIDI channel
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorlw	(DEFAULT_PATCH_CHANGE_CHANNEL-1) & 0x0f
	bnz	CV_MIDI_ProgramChange_End

	;; change patch
	movf	MIOS_PARAMETER2, W
	movwf	CV_PATCH

	;; if patch != 0 and no BankStick connected: print message
	bz	CV_MIDI_ProgramChange_Ok
CV_MIDI_ProgramChange_Ext
	call	CV_BANK_CheckPatchValid
	bz	CV_MIDI_ProgramChange_Ok

CV_MIDI_ProgramChange_Err
	;; ignore patch change
	return
CV_MIDI_ProgramChange_Ok

	;; load new patch
	call	CV_DUMP_RestoreAll

	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
#endif
CV_MIDI_ProgramChange_End
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Poly Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
CV_MIDI_PolyAfterTouch
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3
	rgoto	CV_MIDI_AfterTouch

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CV_MIDI_AfterTouch
	;; extract channel -> TMP1
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

	;; go through all 8 voices
	clrf	TMP4
CV_MIDI_AfterTouch_Loop
	lfsr	FSR0, CV_C1_MIDI_CHANNEL
	movf	TMP4, W
	movf	PLUSW0, W
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_AfterTouch_Loop_Next

	lfsr	FSR0, CV_C1_AFTERTOUCH
	movf	TMP4, W
	movff	MIOS_PARAMETER3, PLUSW0

	lfsr	FSR0, CV_C1_MIDI_MODE
	movf	TMP4, W
	movf	PLUSW0, W
	andlw	0x0f
	xorlw	CV_MIDI_MODE_AFTERTOUCH
	bnz	CV_MIDI_AfterTouch_Loop_Next

	lfsr	FSR0, CV_C1_STAT
	movf	TMP4, W
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

	lfsr	FSR0, CS_MENU_METER_CTR_BEGIN		; trigger meter counter (for CS display)
	movf	TMP4, W
	setf	PLUSW0
CV_MIDI_AfterTouch_Loop_Next
	incf	TMP4, F
	BRA_IFCLR TMP4, 3, ACCESS, CV_MIDI_AfterTouch_Loop

CV_MIDI_AfterTouch_End
	return



;; --------------------------------------------------------------------------
;;  help routines
;; --------------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Push a note into the stack
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_PushNote
	clrf	TMP1
	;; do nothing if note is already stored in note stack
CV_MIDI_Hlp_PushNote_CheckLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	skpnz
	rgoto	CV_MIDI_Hlp_PushNote_Failed       ; leave note routine if note already stored
	incf	TMP1, F
	movlw	CV_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_Hlp_PushNote_CheckLoop
	
	;; shift right note stack 
	movlw	(CV_NOTE_STACK_LEN-2)
	movwf	TMP1
CV_MIDI_Hlp_PushNote_ShiftLoop
	movf	TMP1, W
	movff	PLUSW1, TMP2
	incf	TMP1, W
	movff	TMP2, PLUSW1
	decf	TMP1, F
	incf	TMP1, W
	bnz	CV_MIDI_Hlp_PushNote_ShiftLoop

	;; store new note at offset 0
	movff	MIOS_PARAMETER2, INDF1

	retlw	0x00		; return 0x00 as error status

CV_MIDI_Hlp_PushNote_Failed
	retlw	0x01		; return 0x01 as error status

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Pop a note from the stack
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_PopNote
	; search for note entry with the same number, erase it and push the entries behind
	clrf	TMP1
CV_MIDI_Hlp_PopNote_SearchLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	bz	CV_MIDI_Hlp_PopNote_Found
	incf	TMP1, F
	movlw	CV_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_Hlp_PopNote_SearchLoop
	rgoto	CV_MIDI_Hlp_PopNote_Failed
CV_MIDI_Hlp_PopNote_Found

	;; push the entries behind the found entry
CV_MIDI_Hlp_PopNote_ShiftLoop
	incf	TMP1, W
	movff	PLUSW1, TMP2
	movf	TMP1, W
	movff	TMP2, PLUSW1
	incf	TMP1, F
	movlw	CV_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto CV_MIDI_Hlp_PopNote_ShiftLoop
	;; clear the last entry
	movlw	CV_NOTE_STACK_LEN-1
	clrf	PLUSW1
	retlw	0x00		; return with 0x00: note deleted from stack

CV_MIDI_Hlp_PopNote_Failed
	retlw	0x01		; return with 0x01: note not found in stack

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Note On Help function (voice number 0-7 in CV_CURRENT_CHANNEL)
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_NoteOn
	lfsr	FSR0, CV_C1_NOTE
	movf	CV_CURRENT_CHANNEL, W, BANKED
	movff	INDF1, PLUSW0

	lfsr	FSR0, CV_C1_STAT
	movf	CV_CURRENT_CHANNEL, W, BANKED
	bsf	PLUSW0, CV_STAT_CV_UPDATE_REQ

CV_MIDI_Hlp_NoteOn_SusKey
	;; special portamento gate: active, when more than one note is played
	bcf	PLUSW0, CV_STAT_PORTA_ACTIVE

	movlw	0x01
	movf	PLUSW1, W
	bz	CV_MIDI_Hlp_NoteOn_SusKeyNoPorta
CV_MIDI_Hlp_NoteOn_SusKeyPorta
	lfsr	FSR0, CV_C1_STAT
	movf	CV_CURRENT_CHANNEL, W, BANKED
	bsf	PLUSW0, CV_STAT_PORTA_ACTIVE
CV_MIDI_Hlp_NoteOn_SusKeyNoPorta

	;; skip the rest if legato mode and current note is first note
	BRA_IFCLR CV_MIDI_MODE, CV_MIDI_MODE_FLAG_LEGATO, BANKED, CV_MIDI_Hlp_NoteOn_TriggerGateNL
	movf	MIOS_PARAMETER2, W
	cpfseq	INDF1, ACCESS
	rgoto CV_MIDI_Hlp_NoteOn_TrgrGateLSkp
	movlw	0x01
	movf	PLUSW1, W
	bnz	CV_MIDI_Hlp_NoteOn_TrgrGateLSkp
CV_MIDI_Hlp_NoteOn_TriggerGateNL

	;; request gate bit
	rcall	CV_MIDI_Hlp_GateOn
CV_MIDI_Hlp_NoteOn_TrgrGateLSkp
	return

	;; ------------------------------------------------------------------
	;; Note Off Help Function (voice number 0-7 in CV_CURRENT_CHANNEL)
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	BRA_IFSET CV_MIDI_MODE, CV_MIDI_MODE_FLAG_LEGATO, BANKED, CV_MIDI_Hlp_NoteOff_NoGOff
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_Hlp_NoteOff_End
	rcall	CV_MIDI_Hlp_GateOff
CV_MIDI_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if already note available, play new note in NoteOn Section
	movf	INDF1, W
	skpz
	retlw	0x01		; return, request Note On!

	;; else request gate clear bit
	rcall	CV_MIDI_Hlp_GateOff
CV_MIDI_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!

	;; ------------------------------------------------------------------
	;; Gate On help function (voice number 0-7 in CV_CURRENT_CHANNEL)
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_GateOn
	lfsr	FSR0, CV_C1_STAT
	movf	CV_CURRENT_CHANNEL, W, BANKED
	bsf	PLUSW0, CV_STAT_GATE_SET_REQ
	bsf	PLUSW0, CV_STAT_VOICE_ACTIVE
	return

	;; ------------------------------------------------------------------
	;; Gate Off help function (voice number 0-7 in CV_CURRENT_CHANNEL)
	;; ------------------------------------------------------------------
CV_MIDI_Hlp_GateOff
	lfsr	FSR0, CV_C1_STAT
	movf	CV_CURRENT_CHANNEL, W, BANKED
	bcf	PLUSW0, CV_STAT_GATE_SET_REQ
	bsf	PLUSW0, CV_STAT_GATE_CLR_REQ
	bcf	PLUSW0, CV_STAT_VOICE_ACTIVE
	return

	;; ------------------------------------------------------------------
	;; for Note On/Note Off in Mono mode
	;; MIDI channel in MIOS_PARAMETER1[0..3]
	;; result in TMP5
CV_MIDI_GetAssignedChannels
	clrf	TMP5		; TMP5 contains the voices which should be played

CV_MIDI_GETASSIGNEDCHNx MACRO FLAG_Vx, CV_Cx_MIDI_CHANNEL
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	CV_Cx_MIDI_CHANNEL, W, BANKED
	skpnz
	bsf	TMP5, FLAG_Vx
	ENDM

	CV_MIDI_GETASSIGNEDCHNx	0, CV_C1_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	1, CV_C2_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	2, CV_C3_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	3, CV_C4_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	4, CV_C5_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	5, CV_C6_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	6, CV_C7_MIDI_CHANNEL
	CV_MIDI_GETASSIGNEDCHNx	7, CV_C8_MIDI_CHANNEL

	movf	TMP5, W
	return

	;; ------------------------------------------------------------------
	;; for Note On Off in Mono mode
	;; note number in MIOS_PARAMETER2
	;; result will be ANDed to TMP5 --- CV_MIDI_GetAssignedChannels should be called before!
CV_MIDI_GETASSIGNEDx MACRO FLAG_Vx, CV_Cx_SPLIT_LOWER, CV_Cx_SPLIT_UPPER
	LOCAL	CV_MIDI_GETASSIGNEDx_LOk
	LOCAL	CV_MIDI_GETASSIGNEDx_LOff
	LOCAL	CV_MIDI_GETASSIGNEDx_End

	;; handle split points
	movf	CV_Cx_SPLIT_LOWER, W, BANKED	; (don't split if 0)
	iorwf	CV_Cx_SPLIT_UPPER, W, BANKED
	bz	CV_MIDI_GETASSIGNEDx_End

	movf	CV_Cx_SPLIT_LOWER, W, BANKED
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_GETASSIGNEDx_LOk
	rgoto	CV_MIDI_GETASSIGNEDx_LOff
CV_MIDI_GETASSIGNEDx_LOk
	movf	CV_Cx_SPLIT_UPPER, W, BANKED
	cpfsgt	MIOS_PARAMETER2, ACCESS
	rgoto CV_MIDI_GETASSIGNEDx_End
CV_MIDI_GETASSIGNEDx_LOff
	bcf	TMP5, FLAG_Vx		; don't play voice
CV_MIDI_GETASSIGNEDx_End
	ENDM

CV_MIDI_GetAssignedVoices
	CV_MIDI_GETASSIGNEDx 0, CV_C1_SPLIT_LOWER, CV_C1_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 1, CV_C2_SPLIT_LOWER, CV_C2_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 2, CV_C3_SPLIT_LOWER, CV_C3_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 3, CV_C4_SPLIT_LOWER, CV_C4_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 4, CV_C5_SPLIT_LOWER, CV_C5_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 5, CV_C6_SPLIT_LOWER, CV_C6_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 6, CV_C7_SPLIT_LOWER, CV_C7_SPLIT_UPPER
	CV_MIDI_GETASSIGNEDx 7, CV_C8_SPLIT_LOWER, CV_C8_SPLIT_UPPER

	movf	TMP5, W
	return
