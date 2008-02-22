; $Id$
;
; MIDIbox SID
; Wavetable Sequencer
;
; MEMO - when MIDI sync is enabled, following WT clock rates are usefull:
;  126:	8 steps = 1/4 note
;  123:	8 steps = 1/2 note
;  117:	8 steps = 1 note
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  SID Wavetable Handler
;; --------------------------------------------------------------------------
SID_WT_Handler
	SET_BSR	SID_BASE			; init BSR

	;; switch to WT patch
	movff	SID_PATCH, SID_WT_SAVED_PATCH
	movff	SID_WT_PATCH, SID_PATCH

	;; exit handler if wavetable rate == 0
	movf	SID_WT_RATE, W, BANKED
	skpnz
	rgoto	SID_WT_End

	;; reset requested?
	BRA_IFCLR SID_WT_STATE, WT_STATE_RESET, BANKED, SID_WT_NoReset
SID_WT_Reset
	bcf	SID_WT_STATE, WT_STATE_RESET, BANKED
	bcf	SID_WT_STATE, WT_STATE_STOP, BANKED
	bcf	SID_WT_STATE, WT_STATE_SLIDE, BANKED
	bcf	SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED
	clrf	SID_WT_303_TICK_CTR, BANKED

	;; clear counter & position and request first entry
	IRQ_DISABLE
	BRA_IFCLR SID_MIDI_SYNC, SID_MIDI_SYNC_WT_ARP, BANKED, SID_WT_Handler_IntClk
SID_WT_Handler_ExtClk
	comf	SID_WT_RATE, W, BANKED
	andlw	0x7f
	btfss	SID_SE_OPTION, SE_OPTION_TB303, BANKED
	addlw 1
	movwf	SID_WT_CTR, BANKED	
	rgoto	SID_WT_Handler_ExtClk_C
SID_WT_Handler_IntClk
	comf	SID_WT_RATE, W, BANKED	; copy inverted rate * 2 into CTR register
	movwf	SID_WT_CTR, BANKED
	clrc
	rlf	SID_WT_CTR, F, BANKED
SID_WT_Handler_ExtClk_C
	clrf	SID_WT_POS, BANKED
	movlw	0x01
	movwf	SID_WT_CLK_REQ_CTR, BANKED
	IRQ_ENABLE
SID_WT_NoReset


	;; handle all requested clocks
SID_WT_NextClkLoop
	movf	SID_WT_CLK_REQ_CTR, W, BANKED
	bz	SID_WT_End
	BRA_IFSET SID_WT_STATE, WT_STATE_STOP, BANKED, SID_WT_End
	decf	SID_WT_CLK_REQ_CTR, F, BANKED

	;; get first entry
	rcall	SID_WT_Hlp_CalcPos
	call	SID_BANK_Read
	movwf	TMP4
SID_WT_Loop
	;; branch depending on entry ID
	movf	TMP4, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SID_WT_Branch_Inc
	rgoto	SID_WT_Branch_Play
	rgoto	SID_WT_Branch_Goto
	rgoto	SID_WT_Branch_End
	
SID_WT_Branch_Play
	;; send MIDI clock if enabled for synchronization of external sequencers or similar
	movlw	0xf8
	CALL_IFSET SID_MIDI_SYNC, SID_MIDI_SYNC_SEND_CLK, BANKED, MIOS_MIDI_TxBufferPut
	SET_BSR	SID_BASE

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_WT_Branch_Play_NotTB303
SID_WT_Branch_Play_TB303
	;; in TB303 mode:
	;; increment tick counter, reset if counter >= 6
	incf	SID_WT_303_TICK_CTR, F, BANKED
	movlw	0x6
	cpfslt	SID_WT_303_TICK_CTR, BANKED
	clrf	SID_WT_303_TICK_CTR, BANKED

	;; play note when counter-1 == 0
	decf	SID_WT_303_TICK_CTR, W, BANKED
	bz	SID_WT_Branch_Play_TB303Note
	;; release gate when counter-1 == 3
	decf	SID_WT_303_TICK_CTR, W, BANKED
	xorlw	3
	bz	SID_WT_Branch_Play_TB303G0
	rgoto	SID_WT_NextClkLoop

SID_WT_Branch_Play_TB303G0
	IRQ_DISABLE
	;; exception: don't clear when slide is active
	BRA_IFCLR SID_WT_STATE, WT_STATE_SLIDE, BANKED, SID_WT_Branch_Play_TB303G0_Clr
SID_WT_Branch_Play_TB303G0_Slide
	bsf	SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED
	rgoto	SID_WT_Branch_Play_TB303G0_End

SID_WT_Branch_Play_TB303G0_Clr
	bcf	SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED
	bcf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_GATE_SET_REQ, BANKED
	bsf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_GATE_CLR_REQ, BANKED
	;; 	rgoto	SID_WT_Branch_Play_TB303G0_End

SID_WT_Branch_Play_TB303G0_End
	IRQ_ENABLE
	rgoto	SID_WT_NextClkLoop
SID_WT_Branch_Play_TB303Note
SID_WT_Branch_Play_NotTB303

	;; disable interrupts until all tracks have been handled!
	IRQ_DISABLE

	;; first parameter
	call	SID_BANK_Read
	movwf	TMP2

	BRA_IFCLR SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_WT_Branch_PlayP1_NotTB303
SID_WT_Branch_PlayP1_TB303
	;; set gate flag
	bcf	SID_WT_STATE, WT_STATE_GATE, BANKED
	btfsc	TMP2, 4
	bsf	SID_WT_STATE, WT_STATE_GATE, BANKED

	;; slide flag
	bcf	SID_WT_STATE, WT_STATE_SLIDE, BANKED
	btfsc	TMP2, 5
	bsf	SID_WT_STATE, WT_STATE_SLIDE, BANKED

	;; set accent flag of voice 1 if velocity >= 8
	bcf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_ACCENT
	btfsc	TMP2, 3
	bsf	SID_V1_BASE + SID_Vx_MODE, SID_MODE_ACCENT

	;; modify sustain if gate flag set and no active slide (which wouldn't retrigger the gate)
	BRA_IFCLR SID_WT_STATE, WT_STATE_GATE, BANKED, SID_WT_Branch_Play_TB303NoteNSus
	BRA_IFSET SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED, SID_WT_Branch_Play_TB303NoteNSus
	BRA_IFSET SID_SE_OPTION, SE_OPTION_GSA, BANKED, SID_WT_Branch_Play_TB303NoteNSus
SID_WT_Branch_Play_TB303NoteSus
	;; 0x08 + accent/2
	rlf	TMP2, W
	rlf	WREG, W
	addlw	0x08
	andlw	0x78

	movwf	TMP2
	movlw	57		; CC#57: Voice 1 Sustain
	movwf	TMP3
	rcall	SID_WT_Hlp_Play_P_Cont
SID_WT_Branch_Play_TB303NoteNSus
	rgoto	SID_WT_Branch_PlayP1_TB303Cont

SID_WT_Branch_PlayP1_NotTB303
	btfsc	TMP4, 4
	bsf	TMP2, 7
	movf	SID_ASSIGN_WT_P1, W, BANKED
	skpz
	rcall	SID_WT_Hlp_Play_P
SID_WT_Branch_PlayP1_TB303Cont

	;; second parameter
	bsf	SID_WT_STATE, WT_STATE_PLAY_2ND, BANKED	; will only be used in TB303 mode
	call	SID_BANK_Read
	movwf	TMP2
	btfsc	TMP4, 5
	bsf	TMP2, 7
	;; in TB303 mode: always assigned to CC#9 (play note of voice 1)
	movlw	9
	BRA_IFSET SID_SE_OPTION, SE_OPTION_TB303, BANKED, SID_WT_Branch_PlayP2_TB303
	;; else play assigned parameter
	movf	SID_ASSIGN_WT_P2, W, BANKED
	bz	SID_WT_Branch_PlayP2_Skip
SID_WT_Branch_PlayP2_TB303
	rcall	SID_WT_Hlp_Play_P
SID_WT_Branch_PlayP2_Skip

	;; third parameter
	call	SID_BANK_Read
	movwf	TMP2
	btfsc	TMP4, 6
	bsf	TMP2, 7
	movf	SID_ASSIGN_WT_P3, W, BANKED
	skpz
	rcall	SID_WT_Hlp_Play_P
	bcf	SID_WT_STATE, WT_STATE_PLAY_2ND, BANKED

	IRQ_ENABLE		; enable interrupts again

SID_WT_Branch_Play_End
	rgoto	SID_WT_Branch_Inc

	;; ---
SID_WT_Branch_End
	bsf	SID_WT_STATE, WT_STATE_STOP, BANKED
	rgoto	SID_WT_NextClkLoop
	
	;; ---
SID_WT_Branch_Goto
	bcf	SID_WT_STATE, WT_STATE_SLIDE, BANKED
	bcf	SID_WT_STATE, WT_STATE_SLIDE_PREV, BANKED

	call	SID_BANK_Read
	andlw	0x1f
	movwf	SID_WT_POS, BANKED
	rcall	SID_WT_Hlp_CalcPos
	call	SID_BANK_Read	; prevent endless loops
	movwf	TMP4
	andlw	0x03
	xorlw	0x02
	bz	SID_WT_Branch_End
	rgoto	SID_WT_Loop
	
	;; ---
SID_WT_Branch_Inc
	incf	SID_WT_POS, W, BANKED
	andlw	0x1f
	movwf	SID_WT_POS, BANKED
	rgoto	SID_WT_NextClkLoop

;; ---
SID_WT_End
	;; restore saved patch
	movff	SID_WT_SAVED_PATCH, SID_PATCH
	return

;; ---
	;; calculate address to WT entry
SID_WT_Hlp_CalcPos
	rlf	SID_WT_POS, W, BANKED
	rlf	WREG, W
	andlw	0x7c
	addlw	0x80
	movwf	EEADR
	return
;; ---
	;; play a WT entry
SID_WT_Hlp_Play_P
	movwf	TMP3	; save CC number in TMP3

	BRA_IFSET TMP2, 7, ACCESS, SID_WT_Hlp_Play_P_Cont

	;; add/subtract with saturation
	call	SID_CCOUT_Get

	BRA_IFSET TMP2, 6, ACCESS, SID_WT_Hlp_Play_P_Sub
SID_WT_Hlp_Play_P_Add
	addwf	TMP2, F
	BRA_IFCLR TMP2, 7, ACCESS, SID_WT_Hlp_Play_P_Cont
	movlw	0x7f
	movwf	TMP2	
	goto	SID_WT_Hlp_Play_P_Cont	
SID_WT_Hlp_Play_P_Sub
	bsf	TMP2, 7
	addwf	TMP2, F
	BRA_IFCLR TMP2, 7, ACCESS, SID_WT_Hlp_Play_P_Cont
	clrf	TMP2
	;; 	rgoto	SID_WT_Hlp_Play_P_Cont	

SID_WT_Hlp_Play_P_Cont
	movf	TMP2, W
	andlw	0x7f
	movwf	MIOS_PARAMETER1
	movf	TMP3, W
	goto	SID_CCIN_Set
