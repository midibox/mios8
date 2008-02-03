; $Id$
;
; MIDIboxSEQ
; Sequencer event return function
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function returns the three event bytes + the event length
;;  depending on track, step number and track assignments
;;  IN: SEQ_EVNTS: step number (0-15 for MIDIboxSEQ)
;;      SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      SEQ_STATE[SEQ_STATE_FORCE_EEPROM]: if set, the pattern will always be read from EEPROM
;;      SEQ_EVNTD: drum layer number (0-2) --- only used in DRUM mode
;;      SEQ_PTYPE: parameter type
;;  OUT: the MIDI event in SEQ_EVNT[012]
;;       the gatelength in SEQ_EVNTL
;;       the unmodified gatelength in SEQ_EVNTTHRS (for threshold morph mode)
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_EVNT_Get
	;; get assignments of current track (depending on SEQ_EVNTT) and save it in SEQ_EVNTASSGN
	call	SEQ_FSR_TrkAssgn
	movff	INDF1, SEQ_EVNTASSGN

	;; select SEQ_PATTERN depending on track
	rcall	SEQ_EVNT_Get_Pattern

	;; whole pattern disabled (muted) if SEQ_PATTERNx[7] set
	BRA_IFSET WREG, 7, ACCESS, SEQ_EVNT_Get_Muted

	;; direct jump-in for morph function (see seq_morph.inc, SEQ_MORPH_Event)
SEQ_EVNT_Get_PredefPattern
	SET_BSR	SEQ_BASE

	;; save step offset (within the pattern) in SEQ_EVNTOFFSET
	swapf	SEQ_EVNTT, W, BANKED
	andlw	0x30
	iorwf	SEQ_EVNTS, W, BANKED
	andlw	0x3f
	movwf	SEQ_EVNTOFFSET

	rcall	SEQ_EVNT_Check_Muted
	bz	SEQ_EVNT_Get_NotMuted

SEQ_EVNT_Get_Muted
	;; clear event
	clrf	SEQ_EVNT0, BANKED
	clrf	SEQ_EVNT1, BANKED
	clrf	SEQ_EVNT2, BANKED
	
	;; fetch gatelength and exit
	rcall	SEQ_EVNT_Get_Length
	rgoto	SEQ_EVNT_Get_End

SEQ_EVNT_Get_NotMuted
	;; prepare BSR for BANKED accesses
	SET_BSR	SEQ_BASE

	rcall	SEQ_EVNT_Get_V0
	rcall	SEQ_EVNT_Get_V1
	rcall	SEQ_EVNT_Get_V2
	rcall	SEQ_EVNT_Get_Length

SEQ_EVNT_Get_End
	;; restore edit pattern number
	movff	SEQ_PATTERN0, SEQ_PATTERN
	bcf	SEQ_PATTERN, 7, BANKED	; (SEQ_PATTERN: bit #7 must be cleared)

	return


;; --------------------------------------------------------------------------
;;  this help function selects a pattern depending on the track
;;  SEQ_PATTERN[7] is always cleared, but on the return value it is set
;;  for the case that the pattern is muted
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_Pattern
	SET_BSR	SEQ_BASE

	lfsr	FSR1, SEQ_PATTERN0
	rrf	SEQ_EVNTT, W, BANKED
	rrf	WREG, W
	andlw	0x03
	movf	PLUSW1, W
	movwf	SEQ_PATTERN, BANKED
	bcf	SEQ_PATTERN, 7, BANKED	; (SEQ_PATTERN: bit #7 must be cleared)
	return


;; --------------------------------------------------------------------------
;;  only useful for drums: this function returns a value of the drum record
;;  IN: offset to value within drum record in WREG
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_DValue
	movwf	TABLAT		; temporary save offset in TABLAT

	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_EVNT_Get_DValue_EE
	movf	SEQ_EVNTT, W, BANKED		; drum mute status of track 5-16 always in EEPROM
	andlw	0x0c
	bnz	SEQ_EVNT_Get_DValue_EE

SEQ_EVNT_Get_DValue_RAM
	call	SEQ_FSR_D_Trkx_y
	movf	TABLAT, W
	addwf	FSR1L, F
	movf	INDF1, W
	return
	
SEQ_EVNT_Get_DValue_EE
	movlw	SEQ_PRESETS_LAYER_A & 0xff
	btfsc	SEQ_EVNTD, 0, BANKED
	movlw	SEQ_PRESETS_LAYER_B & 0xff
	btfsc	SEQ_EVNTD, 1, BANKED
	movlw	SEQ_PRESETS_LAYER_C & 0xff
	movwf	EEADR

	movf	SEQ_EVNTT, W, BANKED
	andlw	0x03
	mullw	SEQ_D_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	TABLAT, W
	addwf	EEADR, F
	goto	SEQ_BANK_Read


;; --------------------------------------------------------------------------
;;  only useful for drums: this function returns either the mute or accent
;;  status from RAM or EEPROM
;;  IN: offset to MUTE/ACCENT within drum record in WREG
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_DMAValue
	movwf	TABLAT			; temporary save offset in TABLAT

	rrf	SEQ_EVNTS, W, BANKED	; add offset depending on current step
	rrf	WREG, W
	andlw	0x03
	addwf	TABLAT, W
	goto	SEQ_EVNT_Get_DValue


;; --------------------------------------------------------------------------
;;  this help function returns the mute status depending on selected track
;;  IN: SEQ_EVNTT, SEQ_EVNTD
;;  OUT: 0x00 if track is muted
;; --------------------------------------------------------------------------
SEQ_EVNT_Check_Muted
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_EVNT_Check_Muted_Drum

	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_EVNT_Check_Muted_EE
SEQ_EVNT_Check_Muted_RAM
	call	SEQ_FSR_TrkMuted0

	SET_BSR	SEQ_BASE		; MUTED0 or MUTED1?
	movf	INDF1, W
	btfsc	SEQ_EVNTS, 3, BANKED
	movf	PREINC1, W

	btfsc	SEQ_EVNTS, 2, BANKED
	swapf	WREG, W

	rgoto	SEQ_EVNT_Check_Muted_Cont

SEQ_EVNT_Check_Muted_EE
	movlw	SEQ_PRESETS_MUTE0 & 0xff
	movwf	EEADR
	rrf	SEQ_EVNTS, W, BANKED
	andlw	0x06
	btfsc	SEQ_EVNTT, 1, BANKED
	addlw 1
	addwf	EEADR, F
	call	SEQ_BANK_Read
	btfsc	SEQ_EVNTT, 0, BANKED
	swapf	WREG, W
	rgoto	SEQ_EVNT_Check_Muted_Cont


SEQ_EVNT_Check_Muted_Drum
	movlw	SEQ_D_TRKMUTED_0x
	rcall	SEQ_EVNT_Get_DMAValue
	;;  	rgoto	SEQ_EVNT_Check_Muted_Cont

SEQ_EVNT_Check_Muted_Cont
	andlw	0x0f
	movwf	TABLAT			; temporary store mute status it TABLAT
	
	movf	SEQ_EVNTS, W, BANKED	; compare with step mask
	andlw	0x03
	call	MIOS_HLP_GetBitORMask

	andwf	TABLAT, W		; value == 00 if event is not muted
	return


;; --------------------------------------------------------------------------
;;  this help function fetches the first MIDI event value and stores it SEQ_EVNT0
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_V0
	;; store first event byte in SEQ_EVNT0 (depending on SEQ_EVNTT)
	call	SEQ_FSR_TrkEvnt0
	movff	INDF1, SEQ_EVNT0
	return

;; --------------------------------------------------------------------------
;;  this help function fetches the second MIDI event value and stores it SEQ_EVNT1
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_V1
	SET_BSR	SEQ_BASE

	;; in drum mode the note number is derived from the track record
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_EVNT_Get_V1_NoDrum
SEQ_EVNT_Get_V1_Drum
	movlw	SEQ_D_TRKEVNT1		; get note number
	call	SEQ_EVNT_Get_DValue
	rgoto	SEQ_EVNT_Get_V1_Cont

SEQ_EVNT_Get_V1_NoDrum
	;; branch depending on RAM/EEPROM track
	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_EVNT_Get_V1_EE
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x0c
	bz	SEQ_EVNT_Get_V1_RAM
SEQ_EVNT_Get_V1_EE
	;; determine EEPROM offset of layer, add step offset and save value in SEQ_EVNT1
	movlw	0x00
	btfsc	SEQ_EVNTASSGN, 0, BANKED
	movlw	0x40
	btfsc	SEQ_EVNTASSGN, 1, BANKED
	movlw	0x80
	addwf	SEQ_EVNTOFFSET, W, BANKED
	movwf	EEADR
	call	SEQ_BANK_Read
	rgoto	SEQ_EVNT_Get_V1_Cont

SEQ_EVNT_Get_V1_RAM
	;; determine RAM offset of layer, add step offset and save value in SEQ_EVNT1
	movf	SEQ_EVNTASSGN, W, BANKED
	call	SEQ_FSR_PotValueWREG	; get FSR1 pointer to SEQ_POT_VALUES_[ABC]_00 depending on WREG[1:0]
	movf	SEQ_EVNTOFFSET, W, BANKED
	movf	PLUSW1, W

SEQ_EVNT_Get_V1_Cont
	andlw	0x7f
	movwf	SEQ_EVNT1, BANKED
	return

;; --------------------------------------------------------------------------
;;  this help function fetches the third MIDI event value and stores it SEQ_EVNT2
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_V2
	SET_BSR	SEQ_BASE

	;; in drum mode the velocity is derived from the track record
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_EVNT_Get_V2_NoDrum
SEQ_EVNT_Get_V2_Drum

	;; accent flag set?
	movlw	SEQ_D_TRKACC_0x
	rcall	SEQ_EVNT_Get_DMAValue
	andlw	0x0f
	movwf	TABLAT

	movf	SEQ_EVNTS, W, BANKED	; compare with step mask
	andlw	0x03
	call	MIOS_HLP_GetBitORMask

	andwf	TABLAT, W		; value == 00 if accent
	bnz	SEQ_EVNT_Get_V2_Drum_NoAcc

SEQ_EVNT_Get_V2_Drum_Acc
	movlw	SEQ_D_TRKEVNT2_A	; get accent velocity
	call	SEQ_EVNT_Get_DValue
	rgoto	SEQ_EVNT_Get_V2_Cont

SEQ_EVNT_Get_V2_Drum_NoAcc
	movlw	SEQ_D_TRKEVNT2_N	; get normal velocity
	call	SEQ_EVNT_Get_DValue
	rgoto	SEQ_EVNT_Get_V2_Cont



SEQ_EVNT_Get_V2_NoDrum
	;; branch depending on RAM/EEPROM track
	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_EVNT_Get_V2_EE
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x0c
	bz	SEQ_EVNT_Get_V2_RAM
SEQ_EVNT_Get_V2_EE
	;; determine EEPROM offset of layer, add step offset and save value in SEQ_EVNT2
	movlw	0x00
	btfsc	SEQ_EVNTASSGN, 2, BANKED
	movlw	0x40
	btfsc	SEQ_EVNTASSGN, 3, BANKED
	movlw	0x80
	addwf	SEQ_EVNTOFFSET, W, BANKED
	movwf	EEADR
	call	SEQ_BANK_Read
	rgoto	SEQ_EVNT_Get_V2_Cont

SEQ_EVNT_Get_V2_RAM
	;; determine RAM offset of layer, add step offset and save value in SEQ_EVNT2
	rrf	SEQ_EVNTASSGN, W, BANKED
	rrf	WREG, W
	call	SEQ_FSR_PotValueWREG	; get FSR1 pointer to SEQ_POT_VALUES_[ABC]_00 depending on WREG[1:0]
	movf	SEQ_EVNTOFFSET, W, BANKED
	movf	PLUSW1, W

SEQ_EVNT_Get_V2_Cont
	andlw	0x7f
	movwf	SEQ_EVNT2, BANKED
	return

;; --------------------------------------------------------------------------
;;  this help function fetches the gatelength and stores it SEQ_EVNTL
;; --------------------------------------------------------------------------
SEQ_EVNT_Get_Length	
	SET_BSR	SEQ_BASE

	;; in drum mode the length is derived from the track record
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_EVNT_Get_Length_NoDrum
SEQ_EVNT_Get_Length_Drum	
	movlw	SEQ_D_TRKEVNT_L		; get length
	call	SEQ_EVNT_Get_DValue
	rgoto	SEQ_EVNT_Get_Length_Cont

SEQ_EVNT_Get_Length_NoDrum
	;; branch depending on RAM/EEPROM track
	BRA_IFSET SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED, SEQ_EVNT_Get_Length_EE
	movf	SEQ_EVNTT, W, BANKED
	andlw	0x0c
	bz	SEQ_EVNT_Get_Length_RAM
SEQ_EVNT_Get_Length_EE
	;; determine EEPROM offset of layer, add step offset and save value in SEQ_EVNTL
	movlw	0x00
	btfsc	SEQ_EVNTASSGN, 4, BANKED
	movlw	0x40
	btfsc	SEQ_EVNTASSGN, 5, BANKED
	movlw	0x80
	addwf	SEQ_EVNTOFFSET, W, BANKED
	movwf	EEADR
	call	SEQ_BANK_Read
	rgoto	SEQ_EVNT_Get_Length_Cont

SEQ_EVNT_Get_Length_RAM
	;; determine RAM offset of layer, add step offset and save value in SEQ_EVNTL
	swapf	SEQ_EVNTASSGN, W, BANKED
	call	SEQ_FSR_PotValueWREG	; get FSR1 pointer to SEQ_POT_VALUES_[ABC]_00 depending on WREG[1:0]
	movf	SEQ_EVNTOFFSET, W, BANKED
	movf	PLUSW1, W

SEQ_EVNT_Get_Length_Cont
	movwf	SEQ_EVNTTHRS, BANKED
#if 0
	rrf	WREG, W
	rrf	WREG, W
#endif
	andlw	0x1f
	movwf	SEQ_EVNTL, BANKED
	return


;; --------------------------------------------------------------------------
;;  this function transposes SEQ_EVNT1 if it is no controller, no program change,
;;  no pitch bender event
;;  IN: SEQ_EVNT0: status byte
;;      SEQ_EVNT1: note value
;;      SEQ_PTYPE: parameter type
;;  OUT: transposed MIDI event in SEQ_EVNT1
;;       on overrun, SEQ_EVNT0 is forced to zero
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_EVNT_Transpose
	;; transpose only note-like events
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_EVNT_Transpose_End

SEQ_EVNT_Transpose_Oct
	;; add octave transpose value * 12 and note transpose value to SEQ_EVNT1
	;; force SEQ_EVNT0 to zero on overrun so that the event won't be played
	call	SEQ_FSR_TrkTransp
	movf	INDF1, W
	andlw	0x0f
	xorlw	-8 & 0x0f	; no octave transpose when -8
	bz	SEQ_EVNT_Transpose_NoOct

	movf	INDF1, W
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	mullw	12		; * octave
	movf	PRODL, W
	addwf	SEQ_EVNT1, F, BANKED
	btfsc	SEQ_EVNT1, 7, BANKED
	clrf	SEQ_EVNT0, BANKED
SEQ_EVNT_Transpose_NoOct

SEQ_EVNT_Transpose_Sem
	swapf	INDF1, W
	andlw	0x0f
	btfsc	WREG, 3
	iorlw 0xf0
	addwf	SEQ_EVNT1, F, BANKED
	btfsc	SEQ_EVNT1, 7, BANKED
	clrf	SEQ_EVNT0, BANKED
SEQ_EVNT_Transpose_NoSem

SEQ_EVNT_Transpose_End
	return


;; --------------------------------------------------------------------------
;;  this function saves an event
;;  IN: SEQ_EVNT[12] and SEQ_EVNTL: the event/length which will be saved
;;      SEQ_EVNTT: track number (0-3)
;;      SEQ_EVNTS: step number (0-15)
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_EVNT_Save
	;; get assignments of current track (depending on SEQ_EVNTT) and save it in SEQ_EVNTASSGN
	call	SEQ_FSR_TrkAssgn
	movff	INDF1, SEQ_EVNTASSGN

	;; store second event byte
	;; determine RAM offset of layer, add step offset and copy SEQ_EVNT1
	movf	SEQ_EVNTASSGN, W, BANKED
	rcall	SEQ_EVNT_Save_Hlp
	movff	SEQ_EVNT1, INDF1

	;; store third event byte
	;; determine RAM offset of layer, add step offset and copy SEQ_EVNT2
	rrf	SEQ_EVNTASSGN, W, BANKED
	rrf	WREG, W
	rcall	SEQ_EVNT_Save_Hlp
	movff	SEQ_EVNT2, INDF1

	;; determine RAM offset of layer, add step offset and copy SEQ_EVNTL
	swapf	SEQ_EVNTASSGN, W, BANKED
	rcall	SEQ_EVNT_Save_Hlp
	movff	SEQ_EVNTL, INDF1

	return

SEQ_EVNT_Save_Hlp
	call	SEQ_FSR_PotValueWREG	; get FSR1 pointer to SEQ_POT_VALUES_[ABC]_00 depending on WREG[1:0]
	movf	SEQ_EVNTS, W, BANKED			; add step offset
	andlw	0x0f
	addwf	FSR1L, F
	swapf	SEQ_EVNTT, W, BANKED
	andlw	0x30
	addwf	FSR1L, F
	return

