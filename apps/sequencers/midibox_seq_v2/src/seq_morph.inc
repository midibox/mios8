; $Id$
;
; MIDIboxSEQ
; Sequencer morph functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function is called from SEQ_CORE if an event should be morphed
;;  IN: SEQ_EVNTS: step number (0-15 for MIDIboxSEQ)
;;      SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      SEQ_EVNT[012]: the MIDI event
;;      SEQ_EVNTL: the gatelength
;;      SEQ_MORPH_PATTERN[0-3]: the target pattern
;;      SEQ_MORPH_VALUE: the morph value
;;  OUT: morphed values in SEQ_EVNT[12L]
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_MORPH_Event
	SET_BSR	SEQ_BASE

	;; branch depending on morph mode
	call	SEQ_FSR_TrkMorph0
	movf	INDF1, W
	andlw	0x0f
	bz	SEQ_MORPH_Event_End
	addlw	-1
	bz	SEQ_MORPH_Event_S


	;; morph with threshold value
SEQ_MORPH_Event_T
	;; get event threshold level
	decf	SEQ_EVNTTHRS, W, BANKED
	;; compare it with morph value, branch to the end if value less than threshold level
	cpfsgt	SEQ_MORPH_VALUE, BANKED
	rgoto SEQ_MORPH_Event_T_FixL
	;; if morph value crossed threshold level, fetch alternative event and exit
	lfsr	FSR1, SEQ_MORPH_PATTERN0
	rrf	SEQ_EVNTT, W
	rrf	WREG, W
	andlw	0x03
	movff	PLUSW1, SEQ_PATTERN
	bsf	SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED	; force read from EEPROM
	call	SEQ_EVNT_Get_PredefPattern			; fetch alternative event
	bcf	SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED	; release force

SEQ_MORPH_Event_T_FixL
	;; fix the "real" gatelength to 0x10
	movlw	0x10
	movwf	SEQ_EVNTL, BANKED
	rgoto	SEQ_MORPH_Event_TS_End


	;; morph smoothly
SEQ_MORPH_Event_S
	;; don't morph if morphvalue is 0
	movf	SEQ_MORPH_VALUE, W, BANKED
	bz	SEQ_MORPH_Event_End

	;; don't morph if no event is played
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_MORPH_Event_End

	;; save SEQ_EVNT[12L] in SEQ_MORPH_EVNT[12L]
	movff	SEQ_EVNT1, SEQ_MORPH_EVNT1
	movff	SEQ_EVNT2, SEQ_MORPH_EVNT2
	movff	SEQ_EVNTL, SEQ_MORPH_EVNTL

	;; select morph pattern and fetch new events
	lfsr	FSR1, SEQ_MORPH_PATTERN0
	rrf	SEQ_EVNTT, W
	rrf	WREG, W
	andlw	0x03
	movff	PLUSW1, SEQ_PATTERN				; set new pattern
	bsf	SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED	; force read from EEPROM
	call	SEQ_EVNT_Get_V1
	call	SEQ_EVNT_Get_V2
	call	SEQ_EVNT_Get_Length
	bcf	SEQ_STATE, SEQ_STATE_FORCE_EEPROM, BANKED	; release force

	;; don't modify if morph value at maximum
	movf	SEQ_MORPH_VALUE, W, BANKED
	xorlw	0x7f
	bz	SEQ_MORPH_Event_TS_End

	;; ok, do it here!
	movff	SEQ_MORPH_EVNT1, SEQ_MORPH_MIN_VALUE
	movff	SEQ_EVNT1, SEQ_MORPH_MAX_VALUE
	movf	SEQ_MORPH_VALUE, W, BANKED
	rcall	SEQ_MORPH_ScaleValue
	movwf	SEQ_EVNT1, BANKED

	movff	SEQ_MORPH_EVNT2, SEQ_MORPH_MIN_VALUE
	movff	SEQ_EVNT2, SEQ_MORPH_MAX_VALUE
	movf	SEQ_MORPH_VALUE, W, BANKED
	rcall	SEQ_MORPH_ScaleValue
	movwf	SEQ_EVNT2, BANKED

	movff	SEQ_MORPH_EVNTL, SEQ_MORPH_MIN_VALUE
	movff	SEQ_EVNTL, SEQ_MORPH_MAX_VALUE
	movf	SEQ_MORPH_VALUE, W, BANKED
	rcall	SEQ_MORPH_ScaleValue
	movwf	SEQ_EVNTL, BANKED

SEQ_MORPH_Event_TS_End
	;; restore edit pattern number
	movff	SEQ_PATTERN0, SEQ_PATTERN
	bcf	SEQ_PATTERN, 7, BANKED	; (SEQ_PATTERN: bit #7 must be cleared)

SEQ_MORPH_Event_End
	return


;; --------------------------------------------------------------------------
;;  This function scales a 7bit value depending on a min and max value
;;  If the min value is greater than the max value, they will be
;;  automatically converted to realise a reversed scaling
;;  Input:
;;     o 7bit value in WREG
;;     o min value in SEQ_MORPH_MIN_VALUE
;;     o max value in SEQ_MORPH_MAX_VALUE
;;  Output:
;;     o scaled value in WREG and MIOS_PARAMETER1
;;  USES: MIOS_PARAMETER1 and MIOS_PARAMETER2
;; --------------------------------------------------------------------------
SEQ_MORPH_ScaleValue
	;; save pot value in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	SET_BSR	SEQ_BASE

	;; send min value if min == max
	movf	SEQ_MORPH_MIN_VALUE, W, BANKED
	cpfseq	SEQ_MORPH_MAX_VALUE, BANKED
	rgoto SEQ_MORPH_ScaleValueDo
	movwf	MIOS_PARAMETER1
	rgoto	SEQ_MORPH_ScaleValue_End

SEQ_MORPH_ScaleValueDo
	;; set MIOS_PARAMETER2[0] if min > max
	bcf	MIOS_PARAMETER2, 0
	movf	SEQ_MORPH_MAX_VALUE, W, BANKED
	cpfsgt	SEQ_MORPH_MIN_VALUE, BANKED
	rgoto SEQ_MORPH_ScaleValue_NoConv
	bsf	MIOS_PARAMETER2, 0
SEQ_MORPH_ScaleValue_NoConv

	;; scaled value-1 = ((current value+1) * (max-min+1)) / 128
	;; swap max/min if MIOS_PARAMETER2[0] set

	;; multiply current value * (max-min+1)
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_MORPH_ScaleValue_Inv
SEQ_MORPH_ScaleValue_NoInv
	movf	SEQ_MORPH_MIN_VALUE, W, BANKED
	subwf	SEQ_MORPH_MAX_VALUE, W, BANKED
	rgoto	SEQ_MORPH_ScaleValue_Cont1
SEQ_MORPH_ScaleValue_Inv
	movf	SEQ_MORPH_MAX_VALUE, W, BANKED
	subwf	SEQ_MORPH_MIN_VALUE, W, BANKED
	;; 	rgoto	SEQ_MORPH_ScaleValue_Cont1
SEQ_MORPH_ScaleValue_Cont1
	addlw	1
	mulwf	MIOS_PARAMETER1, ACCESS	; multiply with current value
	;; divide result by 128 (result >> 7)
	;; good trick: just shift the upper bit of the low byte into the high byte
	rlf	PRODL, W
	rlf	PRODH, W
	andlw	0x7f
	;; add min or max value depending on MIOS_PARAMETER2[0]
	btfss	MIOS_PARAMETER2, 0
	addwf	SEQ_MORPH_MIN_VALUE, W, BANKED
	btfsc	MIOS_PARAMETER2, 0
	addwf	SEQ_MORPH_MAX_VALUE, W, BANKED
	;; store result in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
SEQ_MORPH_ScaleValue_End
	movf	MIOS_PARAMETER1, W

	;; return immediately if inversion bit not set
	btfss	MIOS_PARAMETER2, 0
	return

	;; else inverse the result
	subwf	SEQ_MORPH_MIN_VALUE, W, BANKED
	addwf	SEQ_MORPH_MAX_VALUE, W, BANKED
	return

