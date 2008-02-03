; $Id$
;
; MIDIboxSEQ
; MIDI Interface part (used for transposer/arpeggiator)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called from USER_MIDI_NotifyRx when a MIDI byte has 
;;  been received. It's used to check for MIDI clock events
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o received byte in WREG
;; --------------------------------------------------------------------------
SEQ_MIDI_NotifyRx
	movwf	IRQ_TMP1
	SET_BSR	SEQ_BASE

	;; notify clock event if received
	movlw	0xf8
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoClk
SEQ_MIDI_NotifyRx_Clk
	;; check for MIDI slave switching
	RCALL_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_MIDI_NotifyRx_Hlp_SlvChk
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_MIDI_NotifyRx_Master

	;; we've measured a new delay between two F8 events
	movf	SEQ_INCOMING_CLK_CTR, W, BANKED
	movwf	SEQ_INCOMING_CLK_DELAY, BANKED
	clrf	SEQ_INCOMING_CLK_CTR, BANKED

	;; increment clock counter by 4 - SEQ_SENT_CLK_CTR and ensure that the clock won't be echoed
	movf	SEQ_SENT_CLK_CTR, W, BANKED
	sublw	4
	addwf	SEQ_CLK_REQ_CTR, F, BANKED

	;; clear interpolation clock counter and get new SENT_CLK delay
	clrf	SEQ_SENT_CLK_CTR, BANKED
	rrf	SEQ_INCOMING_CLK_DELAY, W, BANKED
	rrf	WREG, W
	andlw	0x3f
	movwf	SEQ_SENT_CLK_DELAY, BANKED

	;; (more infos can be found in the header of seq_bpm.inc)
	return
SEQ_MIDI_NotifyRx_NoClk


	movlw	0xfa
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoStart
SEQ_MIDI_NotifyRx_Start
	;; check for MIDI slave switching
	RCALL_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_MIDI_NotifyRx_Hlp_SlvChk
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_MIDI_NotifyRx_Master

	;; request sequencer start
	movlw	(1 << SEQ_REQ_START) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED

	;; cancel all requested clocks
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED

	rgoto	SEQ_MIDI_NotifyRx_Start_Cont
SEQ_MIDI_NotifyRx_NoStart

	movlw	0xfb
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoCont
SEQ_MIDI_NotifyRx_Cont
	;; check for MIDI slave switching
	RCALL_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_MIDI_NotifyRx_Hlp_SlvChk
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_MIDI_NotifyRx_Master

	;; request sequencer continue
	movlw	(1 << SEQ_REQ_CONT) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED
SEQ_MIDI_NotifyRx_Start_Cont

	;; clear stop request
	bcf	SEQ_REQ, SEQ_REQ_STOP, BANKED

	return
SEQ_MIDI_NotifyRx_NoCont

	movlw	0xfc
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoStop
SEQ_SYSEX_SysExCheck_Stop
	;; check for MIDI slave switching
	RCALL_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, SEQ_MIDI_NotifyRx_Hlp_SlvChk
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, SEQ_MIDI_NotifyRx_Master

	movlw	(1 << SEQ_REQ_STOP) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED
	return
SEQ_MIDI_NotifyRx_NoStop


	;; relevant for master/slave: receive song position counter
SEQ_MIDI_NotifyRx_Master
SEQ_MIDI_NotifyRx_SPos

	;; exit if realtime event has been received
	movlw	0xf8
	cpfslt	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_SPos_End

	;; clear state if status byte (like 0xf2...)
	btfsc	IRQ_TMP1, 7
	clrf	SEQ_MIDIPOS_STATE, BANKED

	;; check sequence (state used as byte counter here)
	movf	SEQ_MIDIPOS_STATE, W, BANKED
	bz	SEQ_MIDI_NotifyRx_SPos_F2_0	; checks for 0xf2
	addlw	-1
	bz	SEQ_MIDI_NotifyRx_SPos_F2_LSB	; get position, low-byte
	addlw	-1
	bz	SEQ_MIDI_NotifyRx_SPos_F2_MSB	; get position, high-byte
	;; this position should never be reached - handle it like checking for F2
SEQ_MIDI_NotifyRx_SPos_F2_0	; checks for 0xF2
	movlw	0xf2
	;; 	rgoto	SEQ_MIDI_NotifyRx_SPos_Syx_Check
SEQ_MIDI_NotifyRx_SPos_Syx_Check
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_SPos_Syx_Inv

	;; if byte matches, increment the state number and exit handler
SEQ_MIDI_NotifyRx_SPos_Syx_Match
	incf	SEQ_MIDIPOS_STATE, F, BANKED
	rgoto	SEQ_MIDI_NotifyRx_SPos_End

	;; if byte doesn't match, clear state number and exit handler
SEQ_MIDI_NotifyRx_SPos_Syx_Inv
	clrf	SEQ_MIDIPOS_STATE, BANKED
	rgoto	SEQ_MIDI_NotifyRx_SPos_End


SEQ_MIDI_NotifyRx_SPos_F2_LSB	; temp. store low-byte of position
	movff	IRQ_TMP1, SEQ_MIDIPOS_LSB
	rgoto	SEQ_MIDI_NotifyRx_SPos_Syx_Match
	
SEQ_MIDI_NotifyRx_SPos_F2_MSB	; get high-byte and refresh clock position
	;; copy first bit of MSB to bit #7 of LSB
	btfsc	IRQ_TMP1, 0
	bsf	SEQ_MIDIPOS_LSB, 7, BANKED
	;; and shift MSB to the right
	clrc
	rrf	IRQ_TMP1, W
	movwf	SEQ_MIDIPOS_MSB, BANKED

	;; cancel all requested clocks
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED

	;; request new sequencer position
	bsf	SEQ_REQ, SEQ_REQ_SONG_POS, BANKED
	;; 	rgoto	SEQ_MIDI_NotifyRx_SPos_End
	
SEQ_MIDI_NotifyRx_SPos_End
SEQ_MIDI_NotifyRx_End
	return


;; help function to switch to slave mode if auto slave is enabled
SEQ_MIDI_NotifyRx_Hlp_SlvChk
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	return
	bsf	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	bsf	SEQ_REQ, SEQ_REQ_START_AUTO_SLAVE_REQ, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function is called by USER_MIDI_NotifyReceivedEvent when a complete 
;;  MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_NotifyReceivedEvent
	;; branch to appr. SEQ routine depending on received event
	swapf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SEQ_MIDI_NoteOff
	rgoto	SEQ_MIDI_NoteOn
	rgoto	SEQ_MIDI_AfterTouch
	rgoto	SEQ_MIDI_CC
	rgoto	SEQ_MIDI_ProgramChange
	rgoto	SEQ_MIDI_PolyAfterTouch
	rgoto	SEQ_MIDI_PitchBender
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note On event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_NoteOn
	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access

	movf	MIOS_PARAMETER3, W	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	SEQ_MIDI_NoteOff

	;; prevent that incoming note-on events will be forwarded to transpose/
	;; arpeggiator function while record mode is active
	btfsc	SEQ_MODE1, SEQ_MODE1_RECORD, BANKED
	return

	;; exit if receive channel disabled
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	skpnz
	return

	;; only continue if MIDI channel matches (channel == 0 -> disabled)
	decf	SEQ_MIDI_CHANNEL, W, BANKED
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	skpz
	return

SEQ_MIDI_NoteOn_FromLoopback
	;; split note?
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED, SEQ_MIDI_NoteOn_NoSplit
	movf	SEQ_MIDI_SPLIT_NOTE, W, BANKED
	bz	SEQ_MIDI_NoteOn_NoSplit
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_NoteOn_Arp
	rgoto	SEQ_MIDI_NoteOn_Transpose
SEQ_MIDI_NoteOn_NoSplit
	rcall	SEQ_MIDI_NoteOn_Transpose
	rgoto	SEQ_MIDI_NoteOn_Arp

	;; ------------------------------------------------------------------
SEQ_MIDI_NoteOn_Transpose
	;; push note into to transpose stack
	lfsr	FSR1, SEQ_T_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PushNote

	;; copy new value to hold register
	movf	SEQ_T_NOTE_STACK_0, W, BANKED
	movwf	SEQ_T_NOTE_STACK_HOLD

	;; change root of scale (will only be selected if enabled in the OPT menu)
	;; we have to normalize the note to 0..11
SEQ_MIDI_NoteOn_Transpose_Loop
	addlw	-12
	BRA_IFCLR WREG, 7, ACCESS, SEQ_MIDI_NoteOn_Transpose_Loop
	addlw	12
	movwf	SEQ_GLOBAL_SCALE_KEYB_ROOT, BANKED
	return

	;; ------------------------------------------------------------------
SEQ_MIDI_NoteOn_Arp
	;; if no note in note stack anymore, reset position of all tracks where the RESET mode flag is set
	movf	SEQ_A_NOTE_STACK_0, W, BANKED
	skpnz
	rcall	SEQ_MIDI_Hlp_ChkPosReset

	;; push note into to arpeggiator stack
	lfsr	FSR1, SEQ_A_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PushNote

	;; call arpeggiator handler which sorts the notes
	rcall	SEQ_MIDI_Arp_Handler

	;; "hold" arpeggiator notes
	;; clear hold registers if only one note active
	movf	SEQ_ARP_NOTE_1_SORTED, W, BANKED
	bnz	SEQ_MIDI_NoteOn_Arp_NoClr
SEQ_MIDI_NoteOn_Arp_Clr
	clrf	SEQ_ARP_NOTE_0_SORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_0_UNSORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_1_SORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_1_UNSORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_2_SORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_2_UNSORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_3_SORTED_HOLD, BANKED
	clrf	SEQ_ARP_NOTE_3_UNSORTED_HOLD, BANKED
SEQ_MIDI_NoteOn_Arp_NoClr

	;; copy all allocated registers (sorted)
	movf	SEQ_ARP_NOTE_0_SORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_0_SORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_1_SORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_1_SORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_2_SORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_2_SORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_3_SORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_3_SORTED_HOLD, BANKED

	;; copy all allocated registers (unsorted)
	movf	SEQ_ARP_NOTE_0_UNSORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_0_UNSORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_1_UNSORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_1_UNSORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_2_UNSORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_2_UNSORTED_HOLD, BANKED

	movf	SEQ_ARP_NOTE_3_UNSORTED, W, BANKED
	skpz
	movwf	SEQ_ARP_NOTE_3_UNSORTED_HOLD, BANKED
	return



;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note Off event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_NoteOff
	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access

	;; exit if receive channel disabled
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	skpnz
	return

	;; only continue if MIDI channel matches (channel == 0 -> disabled)
	addlw	-1
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	skpz
	return

	;; ensure that velocity value is zero
	clrf	MIOS_PARAMETER3

SEQ_MIDI_NoteOff_FromLoopback
	;; split note?
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED, SEQ_MIDI_NoteOff_NoSplit
	movf	SEQ_MIDI_SPLIT_NOTE, W, BANKED
	bz	SEQ_MIDI_NoteOff_NoSplit
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_NoteOff_Arp
	rgoto	SEQ_MIDI_NoteOff_Transpose
SEQ_MIDI_NoteOff_NoSplit
	rcall	SEQ_MIDI_NoteOff_Transpose
	rgoto	SEQ_MIDI_NoteOff_Arp

	;; ------------------------------------------------------------------
SEQ_MIDI_NoteOff_Transpose
	;; pop note from transpose stack
	lfsr	FSR1, SEQ_T_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PopNote

	;; skip if note has not been found
	BRA_IFSET WREG, 0, ACCESS, SEQ_MIDI_NoteOff_Transpose_End

	;; copy new value to hold register if != 0
	movf	SEQ_T_NOTE_STACK_0, W, BANKED
	skpz
	movwf	SEQ_T_NOTE_STACK_HOLD, BANKED
SEQ_MIDI_NoteOff_Transpose_End
	return

	;; ------------------------------------------------------------------
SEQ_MIDI_NoteOff_Arp
	;; pop note from arpeggiator stack
	lfsr	FSR1, SEQ_A_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PopNote

	;; skip if note has not been found
	BRA_IFSET WREG, 0, ACCESS, SEQ_MIDI_NoteOff_Arp_End

	;; goto arpeggiator handler which sorts the notes
	rcall	SEQ_MIDI_Arp_Handler

SEQ_MIDI_NoteOff_Arp_End
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a PitchBender event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_PitchBender
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Controller event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_CC
	;; store MIDI channel in TMP1
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

	;; Morphing controller - check for channel and CC number
	movf	SEQ_MORPH_CHANNEL, W, BANKED
	bz	SEQ_MIDI_CC_NotMorph
	addlw	-1
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_CC_NotMorph
	movf	SEQ_MORPH_CC, W, BANKED
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_NotMorph
SEQ_MIDI_CC_Morph
	movff	MIOS_PARAMETER3, SEQ_MORPH_VALUE
SEQ_MIDI_CC_NotMorph


	;; Exit the rest if MIDI channel doesn't match
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	bz	SEQ_MIDI_CC_End
	addlw	-1
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_CC_End

	;; check if CC#3 has been received
	movlw	0x03
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_NoCC3
SEQ_MIDI_CC_CC3
	movff	MIOS_PARAMETER3, SEQ_GLOBAL_SCALE	; CC#3 controls the global scale
	rgoto	SEQ_MIDI_CC_End
SEQ_MIDI_CC_NoCC3

	;; check if NRPN address (LSB) has been received
	movlw	0x62
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_NoNRPNAddrL
SEQ_MIDI_CC_NRPNAddrL
	movff	MIOS_PARAMETER3, SEQ_NRPN_ADDRESS_LSB	; used to select the parameter
	rgoto	SEQ_MIDI_CC_End
SEQ_MIDI_CC_NoNRPNAddrL

	;; check if NRPN address (MSB) has been received
	movlw	0x63
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_NoNRPNAddrH
SEQ_MIDI_CC_NRPNAddrH
	movff	MIOS_PARAMETER3, SEQ_NRPN_ADDRESS_MSB	; used to select the track
	rgoto	SEQ_MIDI_CC_End
SEQ_MIDI_CC_NoNRPNAddrH

	;; check if NRPN value (MSB) has been received
	movlw	0x06
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_NoNRPNData
SEQ_MIDI_CC_NRPNData
	;; skip if MSB (track number) >= 16
	movf	SEQ_NRPN_ADDRESS_MSB, W, BANKED
	andlw	0xf0
	bnz	SEQ_MIDI_CC_NoNRPNData

	movff	SEQ_NRPN_ADDRESS_MSB, MIOS_PARAMETER2	; track number
	movff	MIOS_PARAMETER3, MIOS_PARAMETER1	; parameter value
	movf	SEQ_NRPN_ADDRESS_LSB, W, BANKED		; parameter number
	call	SEQ_CCIN_Set
	rgoto	SEQ_MIDI_CC_End
SEQ_MIDI_CC_NoNRPNData

#if DEFAULT_MIDI_PATTERN_CHANGE_CC
	;; check for pattern change request (CC#112-CC#115)
	movf	MIOS_PARAMETER2, W
	andlw	0xfc
	xorlw	0x70
	bnz	SEQ_MIDI_CC_NoPatternChng
SEQ_MIDI_CC_PatternChng
	;; select group
	movf	MIOS_PARAMETER2, W
	andlw	0x03
	movwf	SEQ_GROUP, BANKED
	;; new pattern
	movf	MIOS_PARAMETER3, W
	movwf	SEQ_PATTERN, BANKED
	;; bank stays the same
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movf	PLUSW1, W
	andlw	0x07
	movwf	SEQ_PATTERN_BANK, BANKED
	;; request change
	call	SEQ_CORE_ChangePatternSynched
SEQ_MIDI_CC_NoPatternChng
#endif

SEQ_MIDI_CC_End
	return


	;; the CC loopback works different:
SEQ_MIDI_CC_FromLoopback
	;; always forward Morph CC independent from Channel
	movf	SEQ_MORPH_CC, W, BANKED
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_FromLoopback_NoMorph
SEQ_MIDI_CC_FromLoopback_Morph
	movff	MIOS_PARAMETER3, SEQ_MORPH_VALUE
SEQ_MIDI_CC_FromLoopback_NoMorph

	;; always forward scale independent from Channel
	movlw	0x03
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_CC_FromLoopback_NoCC3
SEQ_MIDI_CC_FromLoopback_CC3
	movff	MIOS_PARAMETER3, SEQ_GLOBAL_SCALE
	return
SEQ_MIDI_CC_FromLoopback_NoCC3

	;; extract channel
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

	movff	MIOS_PARAMETER3, MIOS_PARAMETER1	; parameter value
	movf	MIOS_PARAMETER2, W			; parameter number
	movff	TMP1, MIOS_PARAMETER2			; track number
	goto	SEQ_CCIN_Set	


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Program Change event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second  MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_MIDI_ProgramChange
#if DEFAULT_MIDI_SONG_CHANGE_VIA_PC
	;; select new song
	SET_BSR	SEQ_BASE
	movf	SEQ_SONG, W, BANKED
	andlw	0x80
	iorwf	MIOS_PARAMETER2, W
	movwf	SEQ_SONG, BANKED
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Poly Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_MIDI_PolyAfterTouch
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_AfterTouch
	return

;; --------------------------------------------------------------------------
;;  help routines
;; --------------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Copies the note stack (FSR0 -> FSR1)
	;; ------------------------------------------------------------------
SEQ_MIDI_Hlp_CopyStack
	clrf	TMP1
	;; do nothing if note is already stored in note stack
SEQ_MIDI_Hlp_CopyStackLoop
	movf	TMP1, W
	movff	PLUSW0, TMP2
	movff	TMP2, PLUSW1
	incf	TMP1, F
	movlw	SEQ_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_Hlp_CopyStackLoop
	return


	;; ------------------------------------------------------------------
	;; Push a note into the stack
	;; ------------------------------------------------------------------
SEQ_MIDI_Hlp_PushNote
	clrf	TMP1
	;; do nothing if note is already stored in note stack
SEQ_MIDI_Hlp_PushNote_CheckLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	skpnz
	rgoto	SEQ_MIDI_Hlp_PushNote_Failed       ; leave note routine if note already stored
	incf	TMP1, F
	movlw	SEQ_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_Hlp_PushNote_CheckLoop
	
	;; shift right note stack 
	movlw	(SEQ_NOTE_STACK_LEN-2)
	movwf	TMP1
SEQ_MIDI_Hlp_PushNote_ShiftLoop
	movf	TMP1, W
	movff	PLUSW1, TMP2
	incf	TMP1, W
	movff	TMP2, PLUSW1
	decf	TMP1, F
	incf	TMP1, W
	bnz	SEQ_MIDI_Hlp_PushNote_ShiftLoop

	;; store new note at offset 0
	movff	MIOS_PARAMETER2, INDF1

	retlw	0x00		; return 0x00 as error status

SEQ_MIDI_Hlp_PushNote_Failed
	retlw	0x01		; return 0x01 as error status

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Pop a note from the stack
	;; ------------------------------------------------------------------
SEQ_MIDI_Hlp_PopNote
	; search for note entry with the same number, erase it and push the entries behind
	clrf	TMP1
SEQ_MIDI_Hlp_PopNote_SearchLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	bz	SEQ_MIDI_Hlp_PopNote_Found
	incf	TMP1, F
	movlw	SEQ_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_Hlp_PopNote_SearchLoop
	rgoto	SEQ_MIDI_Hlp_PopNote_Failed
SEQ_MIDI_Hlp_PopNote_Found

	;; push the entries behind the found entry
SEQ_MIDI_Hlp_PopNote_ShiftLoop
	incf	TMP1, W
	movff	PLUSW1, TMP2
	movf	TMP1, W
	movff	TMP2, PLUSW1
	incf	TMP1, F
	movlw	SEQ_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SEQ_MIDI_Hlp_PopNote_ShiftLoop
	;; clear the last entry
	movlw	SEQ_NOTE_STACK_LEN-1
	clrf	PLUSW1
	retlw	0x00		; return with 0x00: note deleted from stack

SEQ_MIDI_Hlp_PopNote_Failed
	retlw	0x01		; return with 0x01: note not found in stack

	;; ------------------------------------------------------------------


	;; ------------------------------------------------------------------
	;; requests position reset on all tracks which are assigned to arp
SEQ_MIDI_Hlp_ChkPosReset
	clrf	SEQ_EVNTT, BANKED
SEQ_MIDI_Hlp_ChkPosResetLoop
	call	SEQ_FSR_TrkMode		; check for reset flag
	BRA_IFCLR INDF1, SEQ_TRKMODE_FLAG_RESTART, ACCESS, SEQ_MIDI_Hlp_ChkPosResetLoopNext
	call	SEQ_FSR_TrkVarState	; pointer to SEQ_TRKVARSTATEx -> FSR1
	bsf	INDF1, SEQ_TRKVARSTATE_POS_RESET
SEQ_MIDI_Hlp_ChkPosResetLoopNext
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_MIDI_Hlp_ChkPosResetLoop
	return

	;; ------------------------------------------------------------------
	;; arpeggiator handler
SEQ_MIDI_Arp_Handler
	;; unsorted notes: get from end of stack
	lfsr	FSR0, SEQ_ARP_NOTE_0_UNSORTED
	lfsr	FSR1, SEQ_A_NOTE_STACK_0
	movlw	SEQ_NOTE_STACK_LEN-1	; stack offset where to start the search
	movwf	PRODL
SEQ_MIDI_Arp_Handler_UnSrtLoop1
	movf	PRODL, W
	movf	PLUSW1, W
	bnz	SEQ_MIDI_Arp_Handler_UnSrt_C	; note found
	movf	PRODL, W
	bz	SEQ_MIDI_Arp_Handler_UnSrt_C	; stack empty
	decf	PRODL, F
	rgoto	SEQ_MIDI_Arp_Handler_UnSrtLoop1

SEQ_MIDI_Arp_Handler_UnSrt_C
	;; now copy last four entries of stack to unsorted note stack
	movlw	4			; increment 4 times
	movwf	PRODH
SEQ_MIDI_Arp_Handler_UnSrtLoop2
	;; if stack offset negative: clear note
	BRA_IFCLR PRODL, 7, ACCESS, SEQ_MIDI_Arp_Handler_UnSrtLoop2B
SEQ_MIDI_Arp_Handler_UnSrtLoop2A
	clrf	POSTINC0
	rgoto	SEQ_MIDI_Arp_Handler_UnSrtLoop2N
SEQ_MIDI_Arp_Handler_UnSrtLoop2B
	movf	PRODL, W		; arp stack offset
	movff	PLUSW1, POSTINC0	; copy arp stack -> ARP_NOTE_x_UNSORTED
SEQ_MIDI_Arp_Handler_UnSrtLoop2N	
	decf	PRODL, F		; next note
	decfsz	PRODH, F
	rgoto	SEQ_MIDI_Arp_Handler_UnSrtLoop2


	;; normal arpeggiator notes
	lfsr	FSR0, SEQ_ARP_NOTE_0_SORTED
	lfsr	FSR1, SEQ_A_NOTE_STACK_0

	;; clear all current entries
	clrf	POSTINC0	; (SEQ_ARP_NOTE_0)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_1)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_2)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_3)
	movlw	-4
	addwf	FSR0L, F	; restore pointer to ARP_NOTE_0

	movf	INDF1, W	; (SEQ_NOTE_STACK_0)
	bz	SEQ_MIDI_ARP_HANDLER_End

	movf	POSTINC1, W	; (SEQ_NOTE_STACK_0)
	rcall	SEQ_MIDI_ARP_SingleHandler_Add
	movf	POSTINC1, W	; (SEQ_NOTE_STACK_1)
	rcall	SEQ_MIDI_ARP_SingleHandler_Add
	movf	POSTINC1, W	; (SEQ_NOTE_STACK_2)
	rcall	SEQ_MIDI_ARP_SingleHandler_Add
	movf	POSTINC1, W	; (SEQ_NOTE_STACK_3)
	rcall	SEQ_MIDI_ARP_SingleHandler_Add

SEQ_MIDI_ARP_HANDLER_End
	return

;; ---
	;; add to ARP note buffer, sort automatically from lowest to highest note
SEQ_MIDI_ARP_SingleHandler_Add
	skpnz			; only add notes > 0
	return

	movwf	TMP1		; store new note number in TMP2
	clrf	TMP2		; TMP2 used as loop counter
SEQ_MIDI_ARP_SingleHandler_Loop
	movf	TMP2, W
	movf	PLUSW0, W
	bz	SEQ_MIDI_ARP_SingleHandler_Push; the fourth note will ever be pushed as the appr. byte is zero
	subwf	TMP1, W
	bnc	SEQ_MIDI_ARP_SingleHandler_Push
	incf	TMP2, F
	BRA_IFCLR TMP2, 2, ACCESS, SEQ_MIDI_ARP_SingleHandler_Loop
	return			; this case never happens

SEQ_MIDI_ARP_SingleHandler_Push
	movf	TMP2, W		; fourth note: no shift required
	xorlw	0x03
	bz	SEQ_MIDI_ARP_SingleHandler_PushN
	movlw	0x02
	movwf	TMP3
SEQ_MIDI_ARP_SingleHandler_PushL
	movf	TMP3, W
	movff	PLUSW0, TMP4
	addlw	1
	movff	TMP4, PLUSW0
	movf	TMP2, W
	xorwf	TMP3, W
	bz	SEQ_MIDI_ARP_SingleHandler_PushN
	decf	TMP3, F
	rgoto	SEQ_MIDI_ARP_SingleHandler_PushL

SEQ_MIDI_ARP_SingleHandler_PushN
	movf	TMP2, W
	movff	TMP1, PLUSW0
	return
