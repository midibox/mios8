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

	;; go through list of 16 tracks
	;; search for matching MIDI channel and note definition
	clrf	SEQ_EVNTT, BANKED
SEQ_MIDI_NoteOn_Loop
	movf	SEQ_EVNTT, W, BANKED		; check if MIDI port enabled
	call	SEQ_TRK_GetMIDIPort
	bz	SEQ_MIDI_NoteOn_Loop_Next

	call	SEQ_FSR_TrkChn			; check for matching MIDI channel
	movf	INDF1, W
	xorwf	MIOS_PARAMETER1, W
	andlw	0x0f
	bnz	SEQ_MIDI_NoteOn_Loop_Next
	
	call	SEQ_FSR_TrkNote			; check for matching MIDI note
	movf	INDF1, W
	xorwf	MIOS_PARAMETER2, W
	bnz	SEQ_MIDI_NoteOn_Loop_Next

	movf	SEQ_EVNTT, W, BANKED		; request trigger
	call	SEQ_TRK_ReqTrigger	

SEQ_MIDI_NoteOn_Loop_Next
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, SEQ_MIDI_NoteOn_Loop

	;; update all triggers
	call	SEQ_TRK_SetTriggers
	
SEQ_MIDI_NoteOn_FromLoopback
	;; TODO
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

	;; nothing else to do (drum trigger already cleared after 1 mS)

SEQ_MIDI_NoteOff_FromLoopback
	;; TODO
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

SEQ_MIDI_CC_End
	return


	;; the CC loopback works different:
SEQ_MIDI_CC_FromLoopback
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
#if DEFAULT_MIDI_PATTERN_CHANGE_PC
	;; new pattern in MIOS_PARAMETER2
	movff	MIOS_PARAMETER2, SEQ_PATTERN
	;; bank stays the same
	movff	SEQ_PATTERN_BANK0, SEQ_PATTERN_BANK
	;; request change
	call	SEQ_CORE_ChangePatternSynched
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

