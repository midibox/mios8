; $Id$
;
; MIDIbox SID
; MIDI Interface part
;
; Activate this #define to measure the performance with a scope
; the used port has to be specified here - comment out for no measuring
;#define SID_ME_MEASURE_PERFORMANCE_PORT   LATC, 3
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; experimental arp behaviour for improved playing:
;; even when HOLD mode not active, a note off doesn't remove notes in stack
;; the notes of released keys will be removed from stack once a *new* note is played
#define EXP_ARP_BEHAVIOUR 1
	
;; --------------------------------------------------------------------------
;;  This function is called by SID_MPROC when a complete MIDI event has been
;;  received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
SID_MIDI_NotifyReceivedEvent
	;; store channel in SID_CURRENT_CHANNEL
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movff	WREG, SID_CURRENT_CHANNEL

	;; store MIDI event byte 2 and 3 in SID_MIDI_PARAMETER[12]
	movff	MIOS_PARAMETER2, SID_MIDI_PARAMETER1
	movff	MIOS_PARAMETER3, SID_MIDI_PARAMETER2

#ifdef SID_ME_MEASURE_PERFORMANCE_PORT
	bsf	SID_ME_MEASURE_PERFORMANCE_PORT
#endif

	;; disable IRQs to avoid inconsistencies and to allow the use of FSR2
	IRQ_DISABLE

	;; call MIDI event handler depending on event type
	rcall	SID_MIDI_NotifyReceivedEventJmp
	
	;; enable IRQs again
	IRQ_ENABLE

#ifdef SID_ME_MEASURE_PERFORMANCE_PORT
	bcf	SID_ME_MEASURE_PERFORMANCE_PORT
#endif

	return

SID_MIDI_NotifyReceivedEventJmp
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
;;  This function is called to forward a Note On event to the synthesizer
;;  Input:
;;     o MIDI Voice in SID_CURRENT_MIDI_VOICE
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_NoteOn
	SET_BSR	SID_BASE
	movf	SID_MIDI_PARAMETER2, W, BANKED	; branch to NoteOff if velocity is zero
	bz	SID_MIDI_NoteOff

	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	JUMPTABLE_4BYTES_UNSECURE
	goto	SID_MIDI_L_NoteOn
	goto	SID_MIDI_B_NoteOn
	goto	SID_MIDI_D_NoteOn
	goto	SID_MIDI_M_NoteOn


;; --------------------------------------------------------------------------
;;  This function is called to forward a Note Off event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o note number in SID_MIDI_PARAMETER1
;;     o velocity in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_NoteOff
	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	JUMPTABLE_4BYTES_UNSECURE
	goto	SID_MIDI_L_NoteOff
	goto	SID_MIDI_B_NoteOff
	goto	SID_MIDI_D_NoteOff
	goto	SID_MIDI_M_NoteOff


;; --------------------------------------------------------------------------
;;  This function is called to forward a PitchBender event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o low byte in SID_MIDI_PARAMETER1
;;     o high byte in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_PitchBender
	;; derive 8bit value from PitchBender L/H byte
	SET_BSR	SID_BASE
	clrc
	btfsc	SID_MIDI_PARAMETER1, 6, BANKED
	setc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	SID_MIDI_PARAMETER1, BANKED

	;; forward to knob handler (only checks for MIDI channel of first MIDI voice)
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE

	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_PitchBender_NoKnob
SID_MIDI_PitchBender_Knob
	;; special measure for bassline: select first instrument
	movff	CS_MENU_SELECTED_SID_LR, TMP1		; taken from CS_MENU_SELECTED_SID_LR... dirty :-/
	movlw	0x01
	movwf	CS_MENU_SELECTED_SID_LR

	;; copy converted pitch bender into mod matrix source
	movff	SID_MIDI_PARAMETER1, MIOS_PARAMETER1
	movlw	SID_KNOB_PB
	call	SID_KNOB_SetValue
	movff	TMP1, CS_MENU_SELECTED_SID_LR
SID_MIDI_PitchBender_NoKnob

	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	JUMPTABLE_4BYTES_UNSECURE
	goto	SID_MIDI_L_PitchBender
	goto	SID_MIDI_B_PitchBender
	goto	SID_MIDI_D_PitchBender
	goto	SID_MIDI_M_PitchBender


;; --------------------------------------------------------------------------
;;  This function is called to forward a Controller event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o CC number in SID_MIDI_PARAMETER1
;;     o CC value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_CC

	;; special treatment for CC#0 (bank change)
	SET_BSR	SID_BASE
	movf	SID_MIDI_PARAMETER1, W, BANKED
	bnz	SID_MIDI_CC_No00
SID_MIDI_CC_00
	;; exit if channel doesn't match

	;; exit if bank number >= 8
	movlw	0x08
	cpfslt	SID_MIDI_PARAMETER2, BANKED
	return

	movff	MIOS_PARAMETER3, SID_BANK

	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_MIDI_CC_00_NoCS
	;; notify the patch change to the control surface
	goto	CS_MENU_MS_NotifyBankChange

SID_MIDI_CC_00_NoCS

	;; exit routine if MIDI channel doesn't match
	movf	SID_CURRENT_CHANNEL, W, BANKED
	cpfseq	SID_MV1_BASE + SID_MVx_MIDI_CHANNEL, BANKED
	return

	;; transfer EEPROM/BankStick content into patch buffer and re-init patch
	call	SID_PATCH_LoadPatchBuffer
	goto	USER_DISPLAY_Init

SID_MIDI_CC_No00

	;; NRPN address/LSB?
	;; always stored independent from channel for more consistent usage
	;; (e.g. for Multi Engine, where up to 6 instruments could be assigned
	;; to different MIDI channels)

	;; check if NRPN address (LSB) has been received
	movlw	0x62
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_CC_NoNRPNAddrL
SID_MIDI_CC_NRPNAddrL
	lfsr	FSR1, NRPN_ADDRESS_LSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movff	SID_MIDI_PARAMETER2, PLUSW1
	return
SID_MIDI_CC_NoNRPNAddrL

	;; check if NRPN address (MSB) has been received
	movlw	0x63
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_CC_NoNRPNAddrH
SID_MIDI_CC_NRPNAddrH
	lfsr	FSR1, NRPN_ADDRESS_MSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movff	SID_MIDI_PARAMETER2, PLUSW1
	return
SID_MIDI_CC_NoNRPNAddrH

	;; check if NRPN value (LSB) has been received
	movlw	0x26
	cpfseq	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_CC_NoNRPNDataL
SID_MIDI_CC_NRPNDataL
	lfsr	FSR1, NRPN_DATA_LSB
	movf	SID_CURRENT_CHANNEL, W, BANKED
	movff	SID_MIDI_PARAMETER2, PLUSW1
	return
SID_MIDI_CC_NoNRPNDataL

	;; NRPN value MSB is engine specific and therefore handled in sid_midi_*.inc


	;; check for knob values
	;; (only receiving on channel of first MIDI voice)
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE

	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_CC_NoKnob
SID_MIDI_CC_Knob
	;; Modwheel = Knob #1
	decf	SID_MIDI_PARAMETER1, W
	bnz	SID_MIDI_CC_Knob_NotMdW
SID_MIDI_CC_Knob_MdW
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_1
	goto	SID_KNOB_SetValue
SID_MIDI_CC_Knob_NotMdW

	;; CC17..CC20 = Knob #2-5
	movlw	0x10-1
	cpfsgt	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_CC_Knob_NotK2345
	movlw	0x13+1
	cpfslt	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_CC_Knob_NotK2345
SID_MIDI_CC_Knob_2345
	clrc
	rlf	SID_MIDI_PARAMETER2, W, BANKED
	movwf	MIOS_PARAMETER1
	movf	SID_MIDI_PARAMETER1, W, BANKED
	addlw	-0x10 + SID_KNOB_2
	goto	SID_KNOB_SetValue
SID_MIDI_CC_Knob_NotK2345

SID_MIDI_CC_NoKnob


	;; branch depending on engine
	movff	SID_PATCH_BUFFER_SHADOW + SID_Ix_ENGINE, WREG
	andlw	0x03
	JUMPTABLE_4BYTES_UNSECURE
	goto	SID_MIDI_L_CC
	goto	SID_MIDI_B_CC
	goto	SID_MIDI_D_CC
	goto	SID_MIDI_M_CC


;; --------------------------------------------------------------------------
;;  This function is called to forward a Program Change event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o program number in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_ProgramChange
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_MIDI_L_ProgramChange_NoCS
	;; notify the patch change to the control surface
	goto	CS_MENU_MS_NotifyProgramChange
SID_MIDI_L_ProgramChange_NoCS
	return


;; --------------------------------------------------------------------------
;;  This function is called to forward a Poly Aftertouch event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o Note number in SID_MIDI_PARAMETER1
;;     o Aftertouch value in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
SID_MIDI_PolyAfterTouch
	movff	SID_MIDI_PARAMETER2, SID_MIDI_PARAMETER1
	rgoto	SID_MIDI_AfterTouch

;; --------------------------------------------------------------------------
;;  This function is called to forward a Aftertouch event to the synthesizer
;;  Input:
;;     o MIDI channel in SID_CURRENT_CHANNEL
;;     o Aftertouch value in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_AfterTouch
	;; only check channel of first MIDI voice
	SET_BSR	SID_BASE
	clrf	SID_CURRENT_MIDIVOICE, BANKED
	lfsr	FSR0, SID_MV1_BASE

	;; check for MIDI channel
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_AfterTouch_End

	;; forward to knob handler
	clrc
	rlf	SID_MIDI_PARAMETER1, W, BANKED
	movwf	MIOS_PARAMETER1
	movlw	SID_KNOB_ATH
	call	SID_KNOB_SetValue

SID_MIDI_AfterTouch_End
	return


;; --------------------------------------------------------------------------
;;  help routines used by all engines
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; Push a note into the note stack
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; OUT: ZERO flag cleared if note already stored
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PushNote
	clrf	PRODL
	;; do nothing if note is already stored in note stack
SID_MIDI_Hlp_PushNote_CheckLoop
	movf	PRODL, W
	movf	PLUSW2, W
	bz	SID_MIDI_Hlp_PushNote_StackNew	; new note
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	skpnz
	rgoto	SID_MIDI_Hlp_PushNote_Failed	; leave note routine if note already stored
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PushNote_CheckLoop
	rgoto	SID_MIDI_Hlp_PushNote_StackFull

SID_MIDI_Hlp_PushNote_StackNew
	;; increment stack pointer
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	incf	PLUSW2, F
SID_MIDI_Hlp_PushNote_StackFull

	;; shift right note stack 
	movlw	(SID_MVx_NOTE_STACK_LEN-2)
	movwf	PRODL
SID_MIDI_Hlp_PushNote_ShiftLoop
	movf	PRODL, W
	movff	PLUSW2, PRODH
	incf	PRODL, W
	movff	PRODH, PLUSW2
	decf	PRODL, F
	incf	PRODL, W
	bnz	SID_MIDI_Hlp_PushNote_ShiftLoop

	;; store new note at offset 0
	movff	SID_MIDI_PARAMETER1, INDF2

	andlw	0x00		; return 0x00 as error status (ZERO flag set)
	return

SID_MIDI_Hlp_PushNote_Failed
	iorlw	0xff		; return 0xff as error status (ZERO flag cleared)
	return


;; --------------------------------------------------------------------------
;; Pop a note from the note stack
;; Pop a note from the arp stack (same approach)
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; OUT: ZERO flag cleared if note already stored
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PopNote
SID_MIDI_Hlp_PopArp
SID_MIDI_Hlp_PopArpS
	; search for note entry with the same number, erase it and push the entries behind
	clrf	PRODL
SID_MIDI_Hlp_PopNote_SearchLoop
	movf	PRODL, W
	movf	PLUSW2, W
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	bz	SID_MIDI_Hlp_PopNote_Found
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PopNote_SearchLoop
	rgoto	SID_MIDI_Hlp_PopNote_Failed
SID_MIDI_Hlp_PopNote_Found

	;; push the entries behind the found entry
SID_MIDI_Hlp_PopNote_ShiftLoop
	incf	PRODL, W
	movff	PLUSW2, PRODH
	movf	PRODL, W
	movff	PRODH, PLUSW2
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PopNote_ShiftLoop
	;; clear the last entry
	movlw	SID_MVx_NOTE_STACK_LEN-1
	clrf	PLUSW2
	;; decrement stack pointer
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	decf	PLUSW2, F
	andlw	0x00		; return 0x00 as error status (ZERO flag set)
	return

SID_MIDI_Hlp_PopNote_Failed
	iorlw	0xff		; return 0xff as error status (ZERO flag cleared)
	return


;; --------------------------------------------------------------------------
;; Special Pop function for Arp Hold mode
;; the note won't be removed from stack, but it will be marked with bit 7
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; OUT: ZERO flag cleared if no matching note has been found
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PopArpHold
	; search for note entry with the same number, mark it with bit 7
	clrf	PRODL
SID_MIDI_Hlp_PopArpHold_Loop
	movf	PRODL, W
	movf	PLUSW2, W
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	bz	SID_MIDI_Hlp_PopArpHold_Found
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PopArpHold_Loop
	iorlw	0xff		; return 0xff - nothing to mark
	return

SID_MIDI_Hlp_PopArpHold_Found
	;; mark note
	movf	PRODL, W
	bsf	PLUSW2, 7
	andlw	0x00		; return 0x00 - note found and marked
	return


;; --------------------------------------------------------------------------
;; Push a note into the arp stack
;; in difference to SID_MIDI_Hlp_PushNote, the note will be added at the end
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; OUT: ZERO flag cleared if note already stored
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PushArp
	clrf	PRODL
	;; do nothing if note is already stored in arp stack
	;; break at first entry which is zero
SID_MIDI_Hlp_PushArp_CheckLoop
	movf	PRODL, W
	movf	PLUSW2, W
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushArp_Break
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushArp_Failed       ; leave note routine if note already stored
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PushArp_CheckLoop
	rgoto	SID_MIDI_Hlp_PushArp_Failed	; stack full
SID_MIDI_Hlp_PushArp_Break

	;; add note to empty stack slot
	movf	PRODL, W
	movff	SID_MIDI_PARAMETER1, PLUSW2

	;; increment stack pointer
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	incf	PLUSW2, F

	andlw	0x00		; return 0x00 as error status (ZERO flag set)
	return

SID_MIDI_Hlp_PushArp_Failed
	iorlw	0xff		; return 0xff as error status (ZERO flag cleared)
	return


;; --------------------------------------------------------------------------
;; Push a note into the sorted arp stack
;; in difference to SID_MIDI_Hlp_PushArp, we have a sorted order (low -> high)
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PushArpS
	clrf	PRODL		; PRODL used as loop counter
SID_MIDI_Hlp_PushArpS_Loop
	movf	PRODL, W		
	movf	PLUSW2, W
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushArpS_Push	; check for last entry (=0)
	xorwf	SID_MIDI_PARAMETER1, W, BANKED	; ignore if note is already in stack (-> hold mode)
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushArpS_End
	xorwf	SID_MIDI_PARAMETER1, W, BANKED	; check if new note is >= current note
	subwf	SID_MIDI_PARAMETER1, W, BANKED
	bnc	SID_MIDI_Hlp_PushArpS_Push
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PushArpS_Loop
	return

SID_MIDI_Hlp_PushArpS_Push
	;; increment stack pointer
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	incf	PLUSW2, F
	;; ensure that pointer never > SID_MVx_NOTE_STACK_LEN
	movf	PLUSW2, W
	xorlw	SID_MVx_NOTE_STACK_LEN+1
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	skpnz
	decf	PLUSW2, F

	movf	PRODL, W		; max note: no shift required
	xorlw	SID_MVx_NOTE_STACK_LEN-1
	bz	SID_MIDI_Hlp_PushArpS_PushN

	movlw	SID_MVx_NOTE_STACK_LEN-2
	movwf	PRODH
SID_MIDI_Hlp_PushArpS_PushL
	movf	PRODH, W
	movff	PLUSW2, TABLAT
	addlw	1
	movff	TABLAT, PLUSW2
	movf	PRODL, W
	xorwf	PRODH, W
	bz	SID_MIDI_Hlp_PushArpS_PushN
	decf	PRODH, F
	rgoto	SID_MIDI_Hlp_PushArpS_PushL

SID_MIDI_Hlp_PushArpS_PushN
	movf	PRODL, W
	movff	SID_MIDI_PARAMETER1, PLUSW2
SID_MIDI_Hlp_PushArpS_End
	return


;; --------------------------------------------------------------------------
;; This function checks if the note stack is empty, or only contains
;; released notes
;; OUT: ZERO flag set if at least one note is played
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_CheckActiveNote
	movlw	0
	movwf	PRODL
	movf	PLUSW2, W	; check if first value is 0 (no note in stack)
	bz	SID_MIDI_Hlp_CheckActiveNote_Clr

SID_MIDI_Hlp_CheckActiveNoteLoop
	movf	PRODL, W	; if hold flag is active: check if value either 0x80 or 0x00
	movf	PLUSW2, W
	bz	SID_MIDI_Hlp_CheckActiveNote_Clr
	andlw	0x80
	bz	SID_MIDI_Hlp_CheckActiveNote_Not
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_CheckActiveNoteLoop
SID_MIDI_Hlp_CheckActiveNote_Clr
	iorlw	0xff		; clear ZERO flag - no active note in stack
	return

SID_MIDI_Hlp_CheckActiveNote_Not
	andlw	0x00		; set ZERO flag - found active note
	return


;; --------------------------------------------------------------------------
;; This function removes all non-active notes from the stack
;; OUT: ZERO flag set if at least one note is played
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_ClrNonActive
	movlw	0
	movwf	PRODL
	movf	PLUSW2, W	; check if first value is 0 (no note in stack)
	bz	SID_MIDI_Hlp_ClrNonActive_Clr

SID_MIDI_Hlp_ClrNonActiveLoop
	movf	PRODL, W	; if hold function is active: check if value either 0x80 or 0x00
	movf	PLUSW2, W
	bz	SID_MIDI_Hlp_ClrNonActive_Not
	andlw	0x80
	bz	SID_MIDI_Hlp_ClrNonActiveLoopNxt

	;; remove note from stack and loop again
SID_MIDI_Hlp_ClrNonActiveShLoop
	incf	PRODL, W
	movff	PLUSW2, PRODH
	movf	PRODL, W
	movff	PRODH, PLUSW2
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_ClrNonActiveShLoop
	;; clear the last entry
	movlw	SID_MVx_NOTE_STACK_LEN-1
	clrf	PLUSW2
	;; decrement stack pointer
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0
	decf	PLUSW2, F
	rgoto	SID_MIDI_Hlp_ClrNonActive

SID_MIDI_Hlp_ClrNonActiveLoopNxt
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN - 1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_ClrNonActiveLoop
	;; when we reached this point, at least one note has been found
	rgoto	SID_MIDI_Hlp_ClrNonActive_Not

SID_MIDI_Hlp_ClrNonActive_Clr
	iorlw	0xff		; clear ZERO flag - no active note in stack
	return

SID_MIDI_Hlp_ClrNonActive_Not
	andlw	0x00		; set ZERO flag - found active note
	return


;; --------------------------------------------------------------------------
;; Clears the note stack
;; expecting pointer to SID_MVx_NOTE_STACK_0 in FSR2
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_ClrStack
	movlw	SID_MVx_NOTE_STACK_PTR-SID_MVx_NOTE_STACK_0	; clear SID_MVx_NOTE_STACK_PTR
	clrf	PLUSW2
	
	clrf	PRODL		; clear all SID_MVx_NOTE_STACK_x entries
SID_MIDI_Hlp_ClrStack_Loop
	movf	PRODL, W
	clrf	PLUSW2
	incf	PRODL, F
	movlw	SID_MVx_NOTE_STACK_LEN-1
	cpfsgt	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_ClrStack_Loop
	return



;; --------------------------------------------------------------------------
;; used by multi and lead engine: note on when arp is active
;; IN: pointer to MIDI voice in FSR0
;;     pointer to voice in FSR1
;;     pointer to note stack in FSR2
;;     new note in SID_MIDI_PARAMETER1
;;     ARP_MODE flags in TABLAT
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_ArpNoteOn
#if EXP_ARP_BEHAVIOUR == 0
	;; if no note is played anymore, clear stack again (so that new notes can be filled in HOLD mode)
	rcall	SID_MIDI_Hlp_CheckActiveNote
#else
	BRA_IFSET TABLAT, SID_I_V_ARP_MODE_HOLD, ACCESS, SID_MIDI_Hlp_ArpNoteOn_Hold
	rcall	SID_MIDI_Hlp_ClrNonActive
	rgoto	SID_MIDI_Hlp_ArpNoteOn_Cont
SID_MIDI_Hlp_ArpNoteOn_Hold
	rcall	SID_MIDI_Hlp_CheckActiveNote
SID_MIDI_Hlp_ArpNoteOn_Cont
#endif
	bz	SID_MIDI_Hlp_ArpNoteOn_NoClr
SID_MIDI_Hlp_ArpNoteOn_Clr
	movlw	SID_MVx_ARP_STATE	; set SYNC_ARP flag
	bsf	PLUSW0, SID_MV_ARP_STATE_SYNC_ARP
	rcall	SID_MIDI_Hlp_ClrStack	; clear note stack (for the case that some notes are marked with bit #7 (key released)
SID_MIDI_Hlp_ArpNoteOn_NoClr

	;; push note into stack - select handler depending on sort mode
	BRA_IFSET TABLAT, SID_I_V_ARP_MODE_SORTED, ACCESS, SID_MIDI_Hlp_ArpNoteOn_Sort
	rcall	SID_MIDI_Hlp_PushArp
	rgoto	SID_MIDI_Hlp_ArpNoteOn_NoSort
SID_MIDI_Hlp_ArpNoteOn_Sort
	rcall	SID_MIDI_Hlp_PushArpS
SID_MIDI_Hlp_ArpNoteOn_NoSort

	;; activate voice
	movlw	SID_Vx_STATE
	bsf	PLUSW1, SID_V_STATE_VOICE_ACTIVE

	;; remember note
	movlw	SID_Vx_PLAYED_NOTE
	movff	SID_MIDI_PARAMETER1, PLUSW1

	return

;; --------------------------------------------------------------------------
;; used by multi and lead engine: note off when arp is active
;; IN: pointer to MIDI voice in FSR0
;;     pointer to voice in FSR1
;;     pointer to note stack in FSR2
;;     note to be released in SID_MIDI_PARAMETER1
;;     ARP_MODE flags in TMP1
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_ArpNoteOff
#if EXP_ARP_BEHAVIOUR == 0
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_HOLD, ACCESS, SID_MIDI_Hlp_ArpNoteOff_Hold
	;; pop note from stack (pointer to stack in FSR2)
	rcall	SID_MIDI_Hlp_PopNote
	rgoto	SID_MIDI_Hlp_ArpNoteOff_Cont
SID_MIDI_Hlp_ArpNoteOff_Hold
	rcall	SID_MIDI_Hlp_PopArpHold
SID_MIDI_Hlp_ArpNoteOff_Cont

#else
	rcall	SID_MIDI_Hlp_PopArpHold
#endif

	;; release voice if no note in queue anymore
	BRA_IFSET TMP1, SID_I_V_ARP_MODE_HOLD, ACCESS, SID_MIDI_Hlp_ArpNoteOff_NoClr
#if EXP_ARP_BEHAVIOUR == 0
	movlw	SID_MVx_NOTE_STACK_PTR
	movf	PLUSW0, W
	bnz	SID_MIDI_Hlp_ArpNoteOff_NoClr
#else
	rcall	SID_MIDI_Hlp_CheckActiveNote
	bz	SID_MIDI_Hlp_ArpNoteOff_NoClr
#endif
SID_MIDI_Hlp_ArpNoteOff_Clr
	movlw	SID_Vx_STATE
	bcf	PLUSW1, SID_V_STATE_VOICE_ACTIVE
SID_MIDI_Hlp_ArpNoteOff_NoClr
	return


;; --------------------------------------------------------------------------
;; Pop a note from the WT stack
;; expecting pointer to SID_MVx_WT_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; OUT: ZERO flag cleared if note already stored
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PopWT
	; search for note entry with the same number, erase it and push the entries behind
	clrf	PRODL
SID_MIDI_Hlp_PopWT_SearchLoop
	movf	PRODL, W
	movf	PLUSW2, W
	xorwf	SID_MIDI_PARAMETER1, W, BANKED
	bz	SID_MIDI_Hlp_PopWT_Found
	incf	PRODL, F
	movlw	4
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PopWT_SearchLoop
	rgoto	SID_MIDI_Hlp_PopWT_Failed
SID_MIDI_Hlp_PopWT_Found

	;; push the entries behind the found entry
SID_MIDI_Hlp_PopWT_ShiftLoop
	incf	PRODL, W
	movff	PLUSW2, PRODH
	movf	PRODL, W
	movff	PRODH, PLUSW2
	incf	PRODL, F
	movlw	4
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PopWT_ShiftLoop
	;; clear the last entry
	movlw	4-1
	clrf	PLUSW2
	andlw	0x00		; return 0x00 as error status (ZERO flag set)
	return

SID_MIDI_Hlp_PopWT_Failed
	iorlw	0xff		; return 0xff as error status (ZERO flag cleared)
	return

;; --------------------------------------------------------------------------
;; Push a note into the sorted WT stack
;; expecting pointer to SID_MVx_WT_STACK_0 in FSR2 and note in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_PushWT
	clrf	PRODL		; PRODL used as loop counter
SID_MIDI_Hlp_PushWT_Loop
	movf	PRODL, W		
	movf	PLUSW2, W
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushWT_Push	; check for last entry (=0)
	xorwf	SID_MIDI_PARAMETER1, W, BANKED	; ignore if note is already in stack (-> hold mode)
	andlw	0x7f
	bz	SID_MIDI_Hlp_PushWT_End
	xorwf	SID_MIDI_PARAMETER1, W, BANKED	; check if new note is >= current note
	subwf	SID_MIDI_PARAMETER1, W, BANKED
	bnc	SID_MIDI_Hlp_PushWT_Push
	incf	PRODL, F
	movlw	4
	cpfseq	PRODL, ACCESS
	rgoto SID_MIDI_Hlp_PushWT_Loop
	return

SID_MIDI_Hlp_PushWT_Push
	movf	PRODL, W		; max note: no shift required
	xorlw	4-1
	bz	SID_MIDI_Hlp_PushWT_PushN

	movlw	4-2
	movwf	PRODH
SID_MIDI_Hlp_PushWT_PushL
	movf	PRODH, W
	movff	PLUSW2, TABLAT
	addlw	1
	movff	TABLAT, PLUSW2
	movf	PRODL, W
	xorwf	PRODH, W
	bz	SID_MIDI_Hlp_PushWT_PushN
	decf	PRODH, F
	rgoto	SID_MIDI_Hlp_PushWT_PushL

SID_MIDI_Hlp_PushWT_PushN
	movf	PRODL, W
	movff	SID_MIDI_PARAMETER1, PLUSW2
SID_MIDI_Hlp_PushWT_End
	return


;; --------------------------------------------------------------------------
;; Check MIDI Channel and Split Points
;; IN: current channel IN SID_CURRENT_CHANNEL
;;     current note in SID_MIDI_PARAMETER1
;;     SID_MVx_BASE in FSR0
;; OUT: zero flag set if channel and split zone matches
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_ChkChnAndSplit
	SET_BSR	SID_BASE

	;; check if MIDI channel matches
	movlw	SID_MVx_MIDI_CHANNEL
	movf	PLUSW0, W
	cpfseq	SID_CURRENT_CHANNEL, BANKED
	rgoto SID_MIDI_Hlp_ChkChnAndSplit_Fail

	;; get split points
	movlw	SID_MVx_SPLIT_LOWER
	movff	PLUSW0, PRODL
	movlw	SID_MVx_SPLIT_UPPER
	movff	PLUSW0, PRODH

	;; don't split if both are zero
	movf	PRODL, W
	iorwf	PRODH, W
	bz	SID_MIDI_Hlp_ChkChnAndSplit_Ok

	;; check lower split point
	movf	PRODL, W
	cpfslt	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_Hlp_ChkChnAndSplit_LOk
	rgoto	SID_MIDI_Hlp_ChkChnAndSplit_Fail
SID_MIDI_Hlp_ChkChnAndSplit_LOk

	;; check upper split point
	movf	PRODH, W
	cpfsgt	SID_MIDI_PARAMETER1, BANKED
	rgoto SID_MIDI_Hlp_ChkChnAndSplit_UOk
	rgoto	SID_MIDI_Hlp_ChkChnAndSplit_Fail
SID_MIDI_Hlp_ChkChnAndSplit_UOk

SID_MIDI_Hlp_ChkChnAndSplit_Ok
	andlw	0x00		; ZERO flag set: note within range
	return

SID_MIDI_Hlp_ChkChnAndSplit_Fail
	iorlw	0xff		; ZERO flag cleared: note not within range
	return

;; --------------------------------------------------------------------------
;; Check MIDI Channel and Split Points
;; IN: current channel IN SID_CURRENT_CHANNEL
;;     current note in SID_MIDI_PARAMETER1
;;     SID_MVx_BASE in FSR0
;; OUT: zero flag set if channel and split zone matches
;; --------------------------------------------------------------------------
SID_MIDI_Hlp_GetNoteStackFSR2
	lfsr	FSR2, SID_MV1_BASE + SID_MVx_NOTE_STACK_0
	movf	SID_CURRENT_MIDIVOICE, W, BANKED
	mullw	SID_MVx_RECORD_LEN
	movf	PRODL, W
	addwf	FSR2L, F
	return
