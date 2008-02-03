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

	;; don't receive MIDI clock commands if in master mode
	movf	SEQ_BPM, W, BANKED
	bnz	SEQ_MIDI_NotifyRx_End


	;; notify clock event if received
	movlw	0xf8
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoClk
SEQ_MIDI_NotifyRx_Clk
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
	movlw	(1 << SEQ_REQ_START) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED
	rgoto	SEQ_MIDI_NotifyRx_Start_Cont
SEQ_MIDI_NotifyRx_NoStart

	movlw	0xfb
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoCont
SEQ_MIDI_NotifyRx_Cont
	movlw	(1 << SEQ_REQ_CONT) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED
SEQ_MIDI_NotifyRx_Start_Cont
	;; cancel all requested clocks
	clrf	SEQ_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SEQ_SENT_CLK_CTR, BANKED

	;; init the reference counters
	setf	SEQ_CLK_TICK_CTR, BANKED
	setf	SEQ_CLK_STEP_CTR, BANKED

	;; clear stop request
	bcf	SEQ_REQ, SEQ_REQ_STOP, BANKED

	return
SEQ_MIDI_NotifyRx_NoCont

	movlw	0xfc
	cpfseq	IRQ_TMP1, ACCESS
	rgoto SEQ_MIDI_NotifyRx_NoStop
SEQ_SYSEX_SysExCheck_Stop
	movlw	(1 << SEQ_REQ_STOP) | (1 << SEQ_REQ_NOECHO)
	iorwf	SEQ_REQ, F, BANKED
	return
SEQ_MIDI_NotifyRx_NoStop

SEQ_MIDI_NotifyRx_End
	return

;; --------------------------------------------------------------------------
;;  This function is called from USER_MPROC_NotifyReceivedByte to
;;  parse midi clock position events
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;;  Output:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1 (to make it cascadable)
;; --------------------------------------------------------------------------
SEQ_MIDI_ClkPosParser
	;; exit if realtime event has been received
	movlw	0xf8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto SEQ_MIDI_ClkPosParser_End

	;; ------------------------------------------------------------------
SEQ_MIDI_ClkPosParser_F2
	SET_BSR	SEQ_BASE

	;; clear state if status byte (like 0xf2...)
	btfsc	MIOS_PARAMETER1, 7
	clrf	SEQ_MIDIPOS_STATE, BANKED

	;; check sequence (state used as byte counter here)
	movf	SEQ_MIDIPOS_STATE, W, BANKED
	JUMPTABLE_2BYTES 3			; 3 states
	rgoto	SEQ_MIDI_ClkPosParser_F2_0	; checks for 0xf2
	rgoto	SEQ_MIDI_ClkPosParser_F2_LSB	; get position, low-byte
	rgoto	SEQ_MIDI_ClkPosParser_F2_MSB	; get position, high-byte

SEQ_MIDI_ClkPosParser_F2_0	; checks for 0xF2
	movlw	0xf2
	;; 	rgoto	MIDICLK_Handler_Syx_Check
SEQ_MIDI_ClkPosParser_Syx_Check
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto SEQ_MIDI_ClkPosParser_Syx_Inv

	;; if byte matches, increment the state number and exit handler
SEQ_MIDI_ClkPosParser_Syx_Match
	incf	SEQ_MIDIPOS_STATE, F, BANKED
	rgoto	SEQ_MIDI_ClkPosParser_End

	;; if byte doesn't match, clear state number and exit handler
SEQ_MIDI_ClkPosParser_Syx_Inv
	clrf	SEQ_MIDIPOS_STATE, BANKED
	rgoto	SEQ_MIDI_ClkPosParser_End


SEQ_MIDI_ClkPosParser_F2_LSB	; temp. store low-byte of position
	movff	MIOS_PARAMETER1, SEQ_MIDIPOS_LSB
	rgoto	SEQ_MIDI_ClkPosParser_Syx_Match
	
SEQ_MIDI_ClkPosParser_F2_MSB	; get high-byte and refresh clock position
	;; copy first bit of MSB to bit #7 of LSB
	btfsc	MIOS_PARAMETER1, 0
	bsf	SEQ_MIDIPOS_LSB, 7, BANKED
	;; and shift MSB to the right
	clrc
	rrf	MIOS_PARAMETER1, W
	movwf	SEQ_MIDIPOS_MSB, BANKED

	;; set the sequencer position
	call	SEQ_CORE_SetPos

	;; 	rgoto	SEQ_MIDI_ClkPosParser_End
	
	;; ------------------------------------------------------------------
SEQ_MIDI_ClkPosParser_End
	;; return reveived byte
	movf	MIOS_PARAMETER1, W
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
	;; in record mode: forward event to CS_M_RECORD
	SET_BSR	SEQ_BASE
	CALL_IFSET SEQ_MODE1, SEQ_MODE1_RECORD, BANKED, CS_M_RECORD_NotifyEvent

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
	;; exit if receive channel disabled
#if DEFAULT_SEQUENCER_CHANNEL == 0
	return
#endif

	movf	MIOS_PARAMETER3, W	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	SEQ_MIDI_NoteOff

	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access

	;; only continue of MIDI channel matches
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	SEQ_MIDI_CHANNEL, W, BANKED
	skpz
	return

	;; if MIDI remote key has been pressed
	movlw	DEFAULT_MIDI_REMOTE_KEY
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_NoteOn_NoRemote
SEQ_MIDI_NoteOn_Remote
	;; activate MIDI remote function and exit
	bsf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	return
SEQ_MIDI_NoteOn_NoRemote

	;; if MIDI remote function active, branch to SEQ_REMOTE_Handler
	GOTO_IFSET SEQ_STATE, SEQ_STATE_REMOTE, BANKED, SEQ_REMOTE_Handler

	
	;; if no note in note stack anymore, reset ARP position
	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_ARP_RESET, BANKED, SEQ_MIDI_NoteOn_NoArpReset
SEQ_MIDI_NoteOn_ArpReset
	movf	SEQ_NOTE_STACK_0, W, BANKED
	skpnz
	rcall	SEQ_MIDI_Hlp_ChkPosReset
SEQ_MIDI_NoteOn_NoArpReset

	;; pop note from hold stack (if it is already stored)
	lfsr	FSR1, SEQ_NOTE_STACK_0_HOLD
	rcall	SEQ_MIDI_Hlp_PopNote

	;; push note to normal stack
	lfsr	FSR1, SEQ_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PushNote

	;; copy common stack to hold stack
	lfsr	FSR0, SEQ_NOTE_STACK_0
	lfsr	FSR1, SEQ_NOTE_STACK_0_HOLD
	rcall	SEQ_MIDI_Hlp_CopyStack

	;; goto arpeggiator handler which sorts the notes
	rgoto	SEQ_MIDI_Arp_Handler

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note Off event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SEQ_MIDI_NoteOff
	;; exit if receive channel disabled
#if DEFAULT_SEQUENCER_CHANNEL == 0
	return
#endif

	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access

	;; only continue of MIDI channel matches
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	SEQ_MIDI_CHANNEL, W, BANKED
	skpz
	return

	;; if MIDI remote key has been released
	movlw	DEFAULT_MIDI_REMOTE_KEY
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SEQ_MIDI_NoteOff_NoRemote
SEQ_MIDI_NoteOff_Remote
	;; deactivate MIDI remote function
	bcf	SEQ_STATE, SEQ_STATE_REMOTE, BANKED
	;; send depress event to all buttons and exit
	goto	SEQ_REMOTE_AllOff
SEQ_MIDI_NoteOff_NoRemote

	;; ensure that velocity value is zero
	clrf	MIOS_PARAMETER3

	;; if MIDI remote function active, branch to SEQ_REMOTE_Handler
	GOTO_IFSET SEQ_STATE, SEQ_STATE_REMOTE, BANKED, SEQ_REMOTE_Handler

	;; pop note from stack
	lfsr	FSR1, SEQ_NOTE_STACK_0
	rcall	SEQ_MIDI_Hlp_PopNote

	;; exit if note has not been found
	btfsc	WREG, 0
	return

	;; goto arpeggiator handler which sorts the notes
	rgoto	SEQ_MIDI_Arp_Handler


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
	;; check if a CC event at one of the Layer/CFG channels
	;; has been received
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	TMP1

#if DEFAULT_SEQUENCER_CHANNEL
	;; Morphing controller
	movf	TMP1, W			; check for channel
	xorlw	DEFAULT_SEQUENCER_CHANNEL-1
	bnz	SEQ_MIDI_CC_NotMorph
	decf	MIOS_PARAMETER2, W	; check for modulation controller (CC#1)
	bnz	SEQ_MIDI_CC_NotMorph
SEQ_MIDI_CC_Morph
	movff	MIOS_PARAMETER3, SEQ_MORPH_VALUE
SEQ_MIDI_CC_NotMorph
#endif

	;; the CC functions expect
	;; -> CC number in WREG and
	;; -> CC value in MIOS_PARAMETER1
	movff	MIOS_PARAMETER3, MIOS_PARAMETER1

#if DEFAULT_MIDI_CHANNEL_LAYER_A
	xorlw	DEFAULT_MIDI_CHANNEL_LAYER_A-1
	bnz	SEQ_MIDI_CC_NotLayerA
SEQ_MIDI_CC_LayerA
	movf	MIOS_PARAMETER2, W
	goto	SEQ_CCIN_SetLayerA
SEQ_MIDI_CC_NotLayerA
#endif

#if DEFAULT_MIDI_CHANNEL_LAYER_B
	movf	TMP1, W
	xorlw	DEFAULT_MIDI_CHANNEL_LAYER_B-1
	bnz	SEQ_MIDI_CC_NotLayerB
SEQ_MIDI_CC_LayerB
	movf	MIOS_PARAMETER2, W
	goto	SEQ_CCIN_SetLayerB
SEQ_MIDI_CC_NotLayerB
#endif
	
#if DEFAULT_MIDI_CHANNEL_LAYER_C
	movf	TMP1, W
	xorlw	DEFAULT_MIDI_CHANNEL_LAYER_C-1
	bnz	SEQ_MIDI_CC_NotLayerC
SEQ_MIDI_CC_LayerC
	movf	MIOS_PARAMETER2, W
	goto	SEQ_CCIN_SetLayerC
SEQ_MIDI_CC_NotLayerC
#endif

#if DEFAULT_MIDI_CHANNEL_CFG	
	movf	TMP1, W
	xorlw	DEFAULT_MIDI_CHANNEL_CFG-1
	bnz	SEQ_MIDI_CC_NotCFG
SEQ_MIDI_CC_CFG
	movf	MIOS_PARAMETER2, W
	goto	SEQ_CCIN_Set
SEQ_MIDI_CC_NotCFG
#endif

	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Program Change event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second  MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_MIDI_ProgramChange
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
	call	SEQ_PTYPE_ChkArpMode	; check for arpeggiator/hold
	bnz	SEQ_MIDI_Hlp_ChkPosResetLoopNext
SEQ_MIDI_Hlp_ChkPosResetLoopOk
	call	SEQ_FSR_TrkState	; pointer to SEQ_TRKSTATEx -> FSR1
	bsf	INDF1, SEQ_TRKSTATE_POS_RESET
SEQ_MIDI_Hlp_ChkPosResetLoopNext
	incf	SEQ_EVNTT, F		; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_MIDI_Hlp_ChkPosResetLoop
	return

	;; ------------------------------------------------------------------
	;; arpeggiator handler
SEQ_MIDI_Arp_Handler
	;; normal arpeggiator notes
	lfsr	FSR0, SEQ_ARP_NOTE_0
	lfsr	FSR1, SEQ_NOTE_STACK_0
	rcall	SEQ_MIDI_Arp_SingleHandler

	;; "hold" arpeggiator notes
	lfsr	FSR0, SEQ_ARP_NOTE_0_HOLD
	lfsr	FSR1, SEQ_NOTE_STACK_0_HOLD
	;; 	rcall	SEQ_MIDI_Arp_SingleHandler

SEQ_MIDI_Arp_SingleHandler
	movff	FSR0L, TMP1	; save pointer to ARP_NOTE_0 in TMP1
	;; clear all current entries
	clrf	POSTINC0	; (SEQ_ARP_NOTE_0)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_1)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_2)
	clrf	POSTINC0	; (SEQ_ARP_NOTE_3)
	movff	TMP1, FSR0L	; restore pointer to ARP_NOTE_0 from TMP1

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
