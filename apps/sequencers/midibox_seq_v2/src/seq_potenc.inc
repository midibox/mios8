; $Id$
;
; MIDIboxSEQ
; Functions used by Pots and Enc handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by SEQ_POT_Handler and SEQ_ENC_Handler
;;  when a value has to be modified
;;     o track number in SEQ_EVNTT
;;     o step number in SEQ_EVNTS
;;     o parameter type in SEQ_PTYPE
;;     o abolute value in TMP1
;; --------------------------------------------------------------------------
SEQ_POTENC_Send

	;; get pointer to pot value register depending on selected layer and save value
	call	SEQ_FSR_PotValue

	;; get mute status
	call	SEQ_FSR_TrkMuted0

	;; branch depending on "change all" flag
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_DRUMS, BANKED, SEQ_POTENC_Send_NotAll	; (not supported for drums)
	BRA_IFCLR SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, SEQ_POTENC_Send_NotAll
SEQ_POTENC_Send_All
	;; copy value into all registers of the selected layer which are not muted
	movff	POSTINC1, TMP4
	movff	POSTINC1, TMP5

	movlw	0xf0		; (align address to first entry in layer)
	andwf	FSR0L, F
	clrf	TMP3
SEQ_POTENC_Send_AllLoop
	rrf	TMP5, F		; shift muted tracks
	rrf	TMP4, F
	skpc			; copy TMP1 to register value if mute flag not set
	movff	TMP1, INDF0

	movf	POSTINC0, W	; (increment pointer)
	incf	TMP3, F
	BRA_IFCLR TMP3, 4, ACCESS, SEQ_POTENC_Send_AllLoop
	rgoto	SEQ_POTENC_Send_Cont

SEQ_POTENC_Send_NotAll
	;; else copy value into single register
	movff	TMP1, INDF0
	;; if value == 0: mute step
	;; if value > 0: unmute step
	btfsc	SEQ_EVNTS, 3, BANKED
	movf	POSTINC1, W	; (increment FSR1 when step between 8 and 16)

	;; mute/unmute step if a not length and not morph threshold
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_LAYER_EVNTL, BANKED, SEQ_POTENC_Send_Cont
	BRA_IFSET SEQ_PTYPE, SEQ_PTYPE_THRESHOLD_VALUE, BANKED, SEQ_POTENC_Send_Cont
	;; force unmute if not a "note-like" event
	BRA_IFCLR SEQ_PTYPE, SEQ_PTYPE_NOTELIKE_EVENT, BANKED, SEQ_POTENC_Send_NotAll_UnMute
	;; else mute if value is zero, else unmute
	movf	TMP1, W
	bnz	SEQ_POTENC_Send_NotAll_UnMute
SEQ_POTENC_Send_NotAll_Mute
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
	rgoto	SEQ_POTENC_Send_Cont

SEQ_POTENC_Send_NotAll_UnMute
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F
	;; 	rgoto	SEQ_POTENC_Send_Cont
	
SEQ_POTENC_Send_Cont

	;; in controller mode send value immediately
	call	SEQ_FSR_TrkMode
	movf	INDF1, W
	andlw	0x0f
	xorlw	SEQ_TRKMODE_CONTROLLER
	bnz	SEQ_POTENC_Send_End
SEQ_POTENC_Send_ControllerMode
	;; load SEQ_TMP_EVNT[012L] depending on SEQ_EVNTT and SEQ_EVNTS
	call	SEQ_EVNT_Get
	;; send event
	call	MIOS_MIDI_BeginStream
	movff	SEQ_EVNT0, WREG
	call	MIOS_MIDI_TxBufferPut
	movff	SEQ_EVNT1, WREG
	call	MIOS_MIDI_TxBufferPut
	movff	SEQ_EVNT2, WREG
	call	MIOS_MIDI_TxBufferPut
	call	MIOS_MIDI_EndStream

SEQ_POTENC_Send_End
	return
