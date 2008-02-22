; $Id$
;
; MIDIbox SID
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
;;  This function is called by SID_MPROC when a complete MIDI event has been
;;  received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_NotifyReceivedEvent
	;; branch to appr. SID routine depending on received event
	swapf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	SID_MIDI_NoteOff
	rgoto	SID_MIDI_NoteOn
	rgoto	SID_MIDI_AfterTouch
	rgoto	SID_MIDI_CC
	rgoto	SID_MIDI_ProgramChange
	rgoto	SID_MIDI_PolyAfterTouch
	rgoto	SID_MIDI_PitchBender
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note On event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_NoteOn
	movf	MIOS_PARAMETER3, W	; branch to NoteOff if velocity is zero
	skpnz
	rgoto	SID_MIDI_NoteOff

	SET_BSR	SID_BASE		; prepare BSR for SID register access

	BRA_IFCLR SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED, SID_MIDI_NoteOn_MonoMode
SID_MIDI_NoteOn_PolyMode
	;; in poly mode we only react on MIDI channel of voice 1!
	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return
	
	;; in poly mode: determine free voice
	movlw	0x01
	movf	SID_V1_BASE + SID_Vx_NOTE_STACK_0, F, BANKED
	bz	SID_MIDI_NoteOn_Poly_Cont
	movlw	0x02
	movf	SID_V2_BASE + SID_Vx_NOTE_STACK_0, F, BANKED
	bz	SID_MIDI_NoteOn_Poly_Cont
	movlw	0x04
	movf	SID_V3_BASE + SID_Vx_NOTE_STACK_0, F, BANKED
	bz	SID_MIDI_NoteOn_Poly_Cont
	rgoto	SID_MIDI_NoteOn_Poly_Failed
SID_MIDI_NoteOn_Poly_Cont
	movwf	TMP5		; TMP5 contains the voices which should be played
	rgoto	SID_MIDI_NoteOn_Start_Handlers

SID_MIDI_NoteOn_MonoMode
	;; check for the assigned MIDI channels, result in TMP5
	rcall	SID_MIDI_GetAssignedChannels
	rcall	SID_MIDI_GetAssignedVoices
	;; leave routine if no voice is assigned to channel
	skpnz
	return

SID_MIDI_NoteOn_Start_Handlers
	IRQ_DISABLE

	;; --[ Voice 1 Handler ]--
SID_MIDI_NoteOn_V1
	BRA_IFCLR TMP5, 0, ACCESS, SID_MIDI_NoteOn_V1_Failed
	lfsr	FSR1, SID_V1_BASE + SID_Vx_NOTE_STACK_0		; push note to stack
	rcall	SID_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOn_V1_Failed	; exit if note already in stack
	lfsr	FSR0, SID_V1_BASE
	movf	MIOS_PARAMETER2, W				; note which should be disabled
	RCALL_IFSET SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED, SID_MIDI_Hlp_GateOff	; request gate-off if !legato
	rcall	SID_MIDI_Hlp_NoteOn				; call note-on handler
	lfsr	FSR2, SID_V1_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOn_V1_Failed

	;; --[ Voice 2 Handler ]--
SID_MIDI_NoteOn_V2
	BRA_IFCLR TMP5, 1, ACCESS, SID_MIDI_NoteOn_V2_Failed
	lfsr	FSR1, SID_V2_BASE + SID_Vx_NOTE_STACK_0		; push note to stack
	rcall	SID_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOn_V2_Failed	; exit if note already in stack
	lfsr	FSR0, SID_V2_BASE
	movf	MIOS_PARAMETER2, W				; note which should be disabled
	RCALL_IFSET SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED, SID_MIDI_Hlp_GateOff	; request gate-off if !legato
	rcall	SID_MIDI_Hlp_NoteOn				; call note-on handler
	lfsr	FSR2, SID_V2_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOn_V2_Failed

	;; --[ Voice 3 Handler ]--
SID_MIDI_NoteOn_V3
	BRA_IFCLR TMP5, 2, ACCESS, SID_MIDI_NoteOn_V3_Failed
	lfsr	FSR1, SID_V3_BASE + SID_Vx_NOTE_STACK_0		; push note to stack
	rcall	SID_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOn_V3_Failed	; exit if note already in stack
	lfsr	FSR0, SID_V3_BASE
	movf	MIOS_PARAMETER2, W				; note which should be disabled
	RCALL_IFSET SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED, SID_MIDI_Hlp_GateOff	; request gate-off if !legato
	rcall	SID_MIDI_Hlp_NoteOn				; call note-on handler
	lfsr	FSR2, SID_V3_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOn_V3_Failed

SID_MIDI_NoteOn_Poly_Failed

	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Note Off event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_NoteOff
	SET_BSR	SID_BASE		; prepare BSR for SID register access

	;; ensure that velocity is cleared
	clrf	MIOS_PARAMETER3

	BRA_IFCLR SID_PLAY_MODE, SID_PLAY_MODE_POLY, BANKED, SID_MIDI_NoteOff_MonoMode
SID_MIDI_NoteOff_PolyMode
	;; in poly mode, handle all channels
	movlw	0x07
	movwf	TMP5
	rgoto	SID_MIDI_NoteOff_Start_Handlers
SID_MIDI_NoteOff_MonoMode
	;; check for the assigned MIDI channels, result in TMP5
	rcall	SID_MIDI_GetAssignedChannels
	;; 	rcall	SID_MIDI_GetAssignedVoices ; (not so optimal if split points modified during notes are played)
	;; leave routine if no voice is assigned to channel
	skpnz
	return

SID_MIDI_NoteOff_Start_Handlers
	IRQ_DISABLE

	;; --[ Voice 1 Handler ]--
SID_MIDI_NoteOff_V1
	BRA_IFCLR TMP5, 0, ACCESS, SID_MIDI_NoteOff_V1_NotFnd
	lfsr	FSR1, SID_V1_BASE + SID_Vx_NOTE_STACK_0		; pop note from stack
	movff	INDF1, TMP3					; save current #0 entry in TMP3 for later use
	rcall	SID_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOff_V1_NotFnd
	lfsr	FSR0, SID_V1_BASE
	movf	TMP3, W						; restore note
	rcall	SID_MIDI_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_Hlp_NoteOn
	lfsr	FSR2, SID_V1_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOff_V1_NotFnd

	;; --[ Voice 2 Handler ]--
SID_MIDI_NoteOff_V2
	BRA_IFCLR TMP5, 1, ACCESS, SID_MIDI_NoteOff_V2_NotFnd
	lfsr	FSR1, SID_V2_BASE + SID_Vx_NOTE_STACK_0		; pop note from stack
	movff	INDF1, TMP3					; save current #0 entry in TMP3 for later use
	rcall	SID_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOff_V2_NotFnd
	lfsr	FSR0, SID_V2_BASE
	movf	TMP3, W						; restore note
	rcall	SID_MIDI_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_Hlp_NoteOn
	lfsr	FSR2, SID_V2_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOff_V2_NotFnd

	;; --[ Voice 3 Handler ]--
SID_MIDI_NoteOff_V3
	BRA_IFCLR TMP5, 2, ACCESS, SID_MIDI_NoteOff_V3_NotFnd
	lfsr	FSR1, SID_V3_BASE + SID_Vx_NOTE_STACK_0		; pop note from stack
	movff	INDF1, TMP3					; save current #0 entry in TMP3 for later use
	rcall	SID_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, SID_MIDI_NoteOff_V3_NotFnd
	lfsr	FSR0, SID_V3_BASE
	movf	TMP3, W						; restore note
	rcall	SID_MIDI_Hlp_NoteOff
	RCALL_IFSET WREG, 0, ACCESS, SID_MIDI_Hlp_NoteOn
	lfsr	FSR2, SID_V3_BASE				; sort notes for arpeggios
	rcall	SID_MIDI_Arp_Sorter
SID_MIDI_NoteOff_V3_NotFnd

	IRQ_ENABLE
	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a PitchBender event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_PitchBender
	SET_BSR	SID_BASE

	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return

	rlf	MIOS_PARAMETER3, W
	andlw	0xfe
	xorlw	0x80
	movwf	SID_V1_BASE + SID_Vx_PITCHBENDER, BANKED
	movwf	SID_V2_BASE + SID_Vx_PITCHBENDER, BANKED
	movwf	SID_V3_BASE + SID_Vx_PITCHBENDER, BANKED
	return

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Controller event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_CC
	SET_BSR	SID_BASE

	;; special treatment for CC#0 (bank change)
	movf	MIOS_PARAMETER2, W
	bnz	SID_MIDI_CC_No00
SID_MIDI_CC_00
	;; exit if bank number >= 8
	movlw	0x08
	cpfslt	MIOS_PARAMETER3, ACCESS
	return

	movff	MIOS_PARAMETER3, SID_BANK

#if CS_ENABLED
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_MIDI_CC_00_NoCS
#endif
	;; notify the patch change to the control surface
	goto	CS_MENU_MS_NotifyBankChange
SID_MIDI_CC_00_NoCS
#endif
	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return

	call	SID_PATCH_Init
	goto	USER_DISPLAY_Init


SID_MIDI_CC_No00
	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return

	;; forward CC to CCIN_Set routine
	movff	MIOS_PARAMETER3, MIOS_PARAMETER1
	movf	MIOS_PARAMETER2, W
	call	SID_CCIN_Set

	;; update parameters in CS (only in edit mode)
#if CS_ENABLED
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_MIDI_CC_NoCS
#endif
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_EDIT, ACCESS, SID_MIDI_CC_NoUpdate
	bsf	CS_STAT, CS_STAT_UPDATE_PARAMETERS
	call	CS_MENU_LED_Update	; update LEDs
SID_MIDI_CC_NoUpdate
SID_MIDI_CC_NoCS
#endif

	return


;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Program Change event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second  MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_ProgramChange
#if CS_ENABLED
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_MIDI_ProgramChange_NoCS
#endif
	;; notify the patch change to the control surface
	goto	CS_MENU_MS_NotifyProgramChange

SID_MIDI_ProgramChange_NoCS
#endif

	SET_BSR	SID_BASE

	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return

	movff	MIOS_PARAMETER2, SID_PATCH
	call	SID_PATCH_Init
	goto	USER_DISPLAY_Init

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Poly Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_PolyAfterTouch
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3
	rgoto	SID_MIDI_AfterTouch

;; --------------------------------------------------------------------------
;;  This function is rcalled to forward a Aftertouch event to the synthesizer
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_AfterTouch
	SET_BSR	SID_BASE

	movf	MIOS_PARAMETER1, W	; leave routine if MIDI channel doesn't match
	andlw	0x0f
	cpfseq	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, BANKED
	return

	movf	MIOS_PARAMETER3, W
_SID_MIDI_AfterTouch
	movwf	MIOS_PARAMETER1
	SET_BSR	SID_BASE		; prepare BSR for SID register access
	lfsr	FSR1, SID_LAST_AFTERTOUCH; prepare FSR1
	goto	SID_CCIN_Cmd_AFTERTOUCH	; set aftertouch value


;; --------------------------------------------------------------------------
;;  help routines
;; --------------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Push a note into the stack
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_PushNote
	clrf	TMP1
	;; do nothing if note is already stored in note stack
SID_MIDI_Hlp_PushNote_CheckLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	skpnz
	rgoto	SID_MIDI_Hlp_PushNote_Failed       ; leave note routine if note already stored
	incf	TMP1, F
	movlw	SID_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SID_MIDI_Hlp_PushNote_CheckLoop
	
	;; shift right note stack 
	movlw	(SID_NOTE_STACK_LEN-2)
	movwf	TMP1
SID_MIDI_Hlp_PushNote_ShiftLoop
	movf	TMP1, W
	movff	PLUSW1, TMP2
	incf	TMP1, W
	movff	TMP2, PLUSW1
	decf	TMP1, F
	incf	TMP1, W
	bnz	SID_MIDI_Hlp_PushNote_ShiftLoop

	;; store new note at offset 0
	movff	MIOS_PARAMETER2, INDF1

	retlw	0x00		; return 0x00 as error status

SID_MIDI_Hlp_PushNote_Failed
	retlw	0x01		; return 0x01 as error status

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Pop a note from the stack
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_PopNote
	; search for note entry with the same number, erase it and push the entries behind
	clrf	TMP1
SID_MIDI_Hlp_PopNote_SearchLoop
	movf	TMP1, W
	movf	PLUSW1, W
	xorwf	MIOS_PARAMETER2, W
	bz	SID_MIDI_Hlp_PopNote_Found
	incf	TMP1, F
	movlw	SID_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SID_MIDI_Hlp_PopNote_SearchLoop
	rgoto	SID_MIDI_Hlp_PopNote_Failed
SID_MIDI_Hlp_PopNote_Found

	;; push the entries behind the found entry
SID_MIDI_Hlp_PopNote_ShiftLoop
	incf	TMP1, W
	movff	PLUSW1, TMP2
	movf	TMP1, W
	movff	TMP2, PLUSW1
	incf	TMP1, F
	movlw	SID_NOTE_STACK_LEN
	cpfseq	TMP1, ACCESS
	rgoto SID_MIDI_Hlp_PopNote_ShiftLoop
	;; clear the last entry
	movlw	SID_NOTE_STACK_LEN-1
	clrf	PLUSW1
	retlw	0x00		; return with 0x00: note deleted from stack

SID_MIDI_Hlp_PopNote_Failed
	retlw	0x01		; return with 0x01: note not found in stack

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Note On help function
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_NoteOn
	BRA_IFCLR SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED, SID_MIDI_Hlp_NoteOn_NoNewNote
SID_MIDI_Hlp_NoteOn_NewNote
	movlw	SID_Vx_NOTE
	movff	PLUSW0, TABLAT
	movff	INDF1, PLUSW0
SID_MIDI_Hlp_NoteOn_NoNewNote

	;; if sus-key enabled, skip enable portamento when only one key pressed
	BRA_IFCLR SID_PLAY_MODE, SID_PLAY_MODE_SUS_KEY, BANKED, SID_MIDI_Hlp_NoteOn_SusKeyPor

	;; special case: don't disable portamento on a note off event
	movf	MIOS_PARAMETER3, W
	bz	SID_MIDI_Hlp_NoteOn_SusKeySkip

	movlw	SID_Vx_MODE
	bcf	PLUSW0, SID_MODE_PORTA_ENABLE

	movlw	0x01
	movf	PLUSW1, W
	bz	SID_MIDI_Hlp_NoteOn_SusKeyNoPor
SID_MIDI_Hlp_NoteOn_SusKeyPor
	BRA_IFSET SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED, SID_MIDI_Hlp_NoteOn_SusKeyPor_NC
	;; enable portamento if rate is > 0
	movlw	SID_Vx_PORTA_RATE
	movf	PLUSW0, W
	bz	SID_MIDI_Hlp_NoteOn_SusKeyNoPor
SID_MIDI_Hlp_NoteOn_SusKeyPor_NC
	movlw	SID_Vx_MODE
	bsf	PLUSW0, SID_MODE_PORTA_ENABLE

	;; store current frequency in SID_Vx_PORTA_FRQ_L
	lfsr	FSR2, SID_V1_FRQ_L
	movf	FSR0L, W
	xorlw	SID_V2_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V2_FRQ_L
	movf	FSR0L, W
	xorlw	SID_V3_BASE & 0xff
	skpnz
	lfsr	FSR2, SID_V3_FRQ_L

	movlw	SID_Vx_PORTA_FRQ_L
	movff	POSTINC2, PLUSW0
	movlw	SID_Vx_PORTA_FRQ_H
	movff	POSTDEC2, PLUSW0

	;; reset ENV2 if SE flag enabled
	CALL_IFSET SID_SE_OPTION, SE_OPTION_ENV2PORTA, BANKED, SID_SW_Hlp_ENV2Reset

SID_MIDI_Hlp_NoteOn_SusKeyNoPor
SID_MIDI_Hlp_NoteOn_SusKeySkip

	;; always re-init arpeggiator (in mono as well as in legato mode)
	movlw	SID_Vx_ARP_CTR
	clrf	PLUSW0
	movlw	SID_Vx_ARP_NOTE_NUMBER
	clrf	PLUSW0		; (next increment will play the second note)

	;; skip the rest if legato mode and current note is first note
	BRA_IFSET SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED, SID_MIDI_Hlp_NoteOn_TrgGateNL
	movf	MIOS_PARAMETER2, W
	cpfseq	INDF1, ACCESS
	rgoto SID_MIDI_Hlp_NoteOn_TrgGateLSkp
	movlw	0x01
	movf	PLUSW1, W
	bnz	SID_MIDI_Hlp_NoteOn_TrgGateLSkp
SID_MIDI_Hlp_NoteOn_TrgGateNL

	;; set accent flag depending on velocity
	movlw	SID_Vx_MODE
	bcf	PLUSW0, SID_MODE_ACCENT
	btfsc	MIOS_PARAMETER3, 6
	bsf	PLUSW0, SID_MODE_ACCENT

	;; request gate bit
	rcall	SID_MIDI_Hlp_GateOn

	;; only for voice1: handle with velocity (works ok in mono mode, not ok in poly and split mode)
	movf	FSR0L, W
	xorlw	SID_V1_BASE & 0xff
	bnz	SID_MIDI_Hlp_NoteOn_NoVel
	;; ---[ BEGIN handle velocity ]---
	movff	MIOS_PARAMETER1, TMP1	; store MIOS_PARAMETER1
	movff	MIOS_PARAMETER2, TMP2	; store MIOS_PARAMETER2
	movf	MIOS_PARAMETER3, W	; copy velocity value to MIOS_PARAMETER1
	bz	SID_MIDI_Hlp_NoteOn_NoVel; no velocity on note off!
	movwf	MIOS_PARAMETER1
	SET_BSR	SID_BASE		; prepare BSR for SID register access
	lfsr	FSR1, SID_LAST_VELOCITY	; prepare FSR1
	call	SID_CCIN_Cmd_VELOCITY	; set velocity value
	movff	TMP1, MIOS_PARAMETER1	; restore MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2	; restore MIOS_PARAMETER2
	;; ---[ END handle velocity ]---
SID_MIDI_Hlp_NoteOn_NoVel

SID_MIDI_Hlp_NoteOn_TrgGateLSkp
	return


	;; ------------------------------------------------------------------
	;; Note Off help function
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_NoteOff
	;; last note number of #0 (before pop) in WREG!

	;; if not in legato mode and current note-off number equal to last entry #0: gate off
	BRA_IFCLR SID_PLAY_MODE, SID_PLAY_MODE_LEGATO_OFF, BANKED, SID_MIDI_Hlp_NoteOff_NoGOff
	cpfseq	MIOS_PARAMETER2, ACCESS
	rgoto SID_MIDI_Hlp_NoteOff_End
	rcall	SID_MIDI_Hlp_GateOff
SID_MIDI_Hlp_NoteOff_NoGOff
	;; ------------------------------------------------------------------

	;; if still note available, play new note in NoteOn Section
	movf	INDF1, W
	skpz
	retlw	0x01		; return, request Note On!

	;; else request gate clear bit
	rcall	SID_MIDI_Hlp_GateOff
SID_MIDI_Hlp_NoteOff_End
	retlw	0x00		; return, request NO Note On!

	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; Gate On help function
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_GateOn
	clrc
	movlw	SID_Vx_NOTE_DELAY
	rlf	PLUSW0, W
	movwf	TABLAT
	movlw	SID_Vx_NOTE_DELAY_CTR
	movff	TABLAT, PLUSW0

	movlw	SID_Vx_MODE
	btfsc	SID_PLAY_MODE, SID_PLAY_MODE_ONLY_WT_OFF, BANKED
	bsf	PLUSW0, SID_MODE_GATE_SET_REQ
	bsf	PLUSW0, SID_MODE_VOICE_ACTIVE

	;; reset wavetable handler (only voice 1 can do this!)
	movf	FSR0L, W
	xorlw	SID_V1_BASE & 0xff
	skpnz
	bsf	SID_WT_STATE, WT_STATE_RESET, BANKED

	return

	;; ------------------------------------------------------------------
	;; Gate Off help function
	;; ------------------------------------------------------------------
SID_MIDI_Hlp_GateOff
	clrc
	movlw	SID_Vx_NOTE_DELAY
	rlf	PLUSW0, W
	movwf	TABLAT
	movlw	SID_Vx_NOTE_DELAY_CTR
	movff	TABLAT, PLUSW0

	movlw	SID_Vx_MODE
	bcf	PLUSW0, SID_MODE_GATE_SET_REQ
	bsf	PLUSW0, SID_MODE_GATE_CLR_REQ
	bcf	PLUSW0, SID_MODE_VOICE_ACTIVE
	return

	;; ------------------------------------------------------------------
	;; for Note On/Note Off in Mono mode
	;; MIDI channel in MIOS_PARAMETER1[0..3]
	;; result in TMP5
SID_MIDI_GetAssignedChannels
	clrf	TMP5		; TMP5 contains the voices which should be played
	
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, W, BANKED
	skpnz
	bsf	TMP5, 0		; play voice 1

	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	SID_V2_BASE + SID_Vx_MIDI_CHANNEL, W, BANKED
	skpnz
	bsf	TMP5, 1		; play voice 2

	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	xorwf	SID_V3_BASE + SID_Vx_MIDI_CHANNEL, W, BANKED
	skpnz
	bsf	TMP5, 2		; play voice 3

	movf	TMP5, W
	return

	;; ------------------------------------------------------------------
	;; for Note On Off in Mono mode
	;; note number in MIOS_PARAMETER2
	;; result will be ANDed to TMP5 --- SID_MIDI_GetAssignedChannels should be called before!
SID_MIDI_GETASSIGNEDx MACRO FLAG_Vx, SID_Vx_BASE
	LOCAL	SID_MIDI_GETASSIGNEDx_LOk
	LOCAL	SID_MIDI_GETASSIGNEDx_LOff
	LOCAL	SID_MIDI_GETASSIGNEDx_End

	;; handle split points
	movf	SID_Vx_BASE + SID_Vx_SPLIT_LOWER, W, BANKED	; (don't split if 0)
	iorwf	SID_Vx_BASE + SID_Vx_SPLIT_UPPER, W, BANKED
	bz	SID_MIDI_GETASSIGNEDx_End

	movf	SID_Vx_BASE + SID_Vx_SPLIT_LOWER, W, BANKED
	cpfslt	MIOS_PARAMETER2, ACCESS
	rgoto SID_MIDI_GETASSIGNEDx_LOk
	rgoto	SID_MIDI_GETASSIGNEDx_LOff
SID_MIDI_GETASSIGNEDx_LOk
	movf	SID_Vx_BASE + SID_Vx_SPLIT_UPPER, W, BANKED
	cpfsgt	MIOS_PARAMETER2, ACCESS
	rgoto SID_MIDI_GETASSIGNEDx_End
SID_MIDI_GETASSIGNEDx_LOff
	bcf	TMP5, FLAG_Vx		; don't play voice
SID_MIDI_GETASSIGNEDx_End
	ENDM

SID_MIDI_GetAssignedVoices
	SID_MIDI_GETASSIGNEDx 0, SID_V1_BASE
	SID_MIDI_GETASSIGNEDx 1, SID_V2_BASE
	SID_MIDI_GETASSIGNEDx 2, SID_V3_BASE

	movf	TMP5, W
	return

	;; ------------------------------------------------------------------
	;; arpeggiator sorter
	;; expecting base pointer to voice record in FSR2
SID_MIDI_Arp_Sorter
	;; SID_Vx_ARP_NOTE_0 -> FSR0
	movff	FSR2H, FSR0H
	movf	FSR2L, W
	addlw	SID_Vx_ARP_NOTE_0
	movwf	FSR0L

	;; SID_Vx_NOTE_STACK_0 -> FSR1
	movff	FSR2H, FSR1H
	movf	FSR2L, W
	addlw	SID_Vx_NOTE_STACK_0
	movwf	FSR1L

	movff	FSR0L, TMP1	; save pointer to ARP_NOTE_0 in TMP1
	;; clear all current entries
	clrf	POSTINC0	; (SID_Vx_ARP_NOTE_0)
	clrf	POSTINC0	; (SID_Vx_ARP_NOTE_1)
	clrf	POSTINC0	; (SID_Vx_ARP_NOTE_2)
	clrf	POSTINC0	; (SID_Vx_ARP_NOTE_3)
	movff	TMP1, FSR0L	; restore pointer to ARP_NOTE_0 from TMP1

	movf	INDF1, W	; (SID_Vx_NOTE_STACK_0)
	bz	SID_MIDI_Arp_Sorter_End

	movf	POSTINC1, W	; (SID_Vx_NOTE_STACK_0)
	rcall	SID_MIDI_ARP_Sorter_Add
	movf	POSTINC1, W	; (SID_Vx_NOTE_STACK_1)
	rcall	SID_MIDI_ARP_Sorter_Add
	movf	POSTINC1, W	; (SID_Vx_NOTE_STACK_2)
	rcall	SID_MIDI_ARP_Sorter_Add
	movf	POSTINC1, W	; (SID_Vx_NOTE_STACK_3)
	rcall	SID_MIDI_ARP_Sorter_Add

	;; if rate is > 0, and arp has been reset: copy first arp note into SID_Vx_NOTE
	movlw	SID_Vx_ARP_RATE
	movf	PLUSW2, W
	bz	SID_MIDI_Arp_Sorter_End
	movlw	SID_Vx_ARP_CTR
	movf	PLUSW2, W
	bnz	SID_MIDI_Arp_Sorter_End
	movlw	SID_Vx_ARP_NOTE_NUMBER
	movf	PLUSW2, W
	bnz	SID_MIDI_Arp_Sorter_End

	movlw	SID_Vx_ARP_NOTE_0
	movff	PLUSW2, TABLAT
	movlw	SID_Vx_NOTE
	movff	TABLAT, PLUSW2

SID_MIDI_Arp_Sorter_End
	return

;; ---
	;; add to ARP note buffer, sort automatically from lowest to highest note
SID_MIDI_ARP_Sorter_Add
	skpnz			; only add notes > 0
	return

	movwf	TMP1		; store new note number in TMP2
	clrf	TMP2		; TMP2 used as loop counter
SID_MIDI_ARP_Sorter_Loop
	movf	TMP2, W
	movf	PLUSW0, W
	bz	SID_MIDI_ARP_Sorter_Push; the fourth note will ever be pushed as the appr. byte is zero
	subwf	TMP1, W
	bnc	SID_MIDI_ARP_Sorter_Push
	incf	TMP2, F
	BRA_IFCLR TMP2, 2, ACCESS, SID_MIDI_ARP_Sorter_Loop
	return			; this case never happens

SID_MIDI_ARP_Sorter_Push
	movf	TMP2, W		; fourth note: no shift required
	xorlw	0x03
	bz	SID_MIDI_ARP_Sorter_PushN
	movlw	0x02
	movwf	TMP3
SID_MIDI_ARP_Sorter_PushL
	movf	TMP3, W
	movff	PLUSW0, TMP4
	addlw	1
	movff	TMP4, PLUSW0
	movf	TMP2, W
	xorwf	TMP3, W
	bz	SID_MIDI_ARP_Sorter_PushN
	decf	TMP3, F
	rgoto	SID_MIDI_ARP_Sorter_PushL

SID_MIDI_ARP_Sorter_PushN
	movf	TMP2, W
	movff	TMP1, PLUSW0
	return
