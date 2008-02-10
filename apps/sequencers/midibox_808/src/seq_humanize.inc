; $Id$
;
; MIDIboxSEQ
; Sequencer humanize functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function is called from SEQ_CORE if an event should be humanized
;;  IN: SEQ_EVNTS: step number (0-15 for MIDIboxSEQ)
;;      SEQ_EVNTT: track number (0-15 for MIDIboxSEQ)
;;      FSR0: pointer to track record
;;      FSR2: pointer to track variables
;;      SEQ_EVNT[012]: the MIDI event
;;      SEQ_EVNTL: the gatelength
;;  OUT: humanized values in SEQ_EVNT[12L]
;;  USES: FSR1, TMP[12]
;; --------------------------------------------------------------------------
SEQ_HUMANIZE_Event
	SET_BSR	SEQ_BASE

	;; don't humanize if EVNT0 is zero
	movf	SEQ_EVNT0, W, BANKED
	bz	SEQ_HUMANIZE_Event_End

	;; if "random value" trigger set, force humanizer to intensity "7"
	call	SEQ_TRG_RandomValueGet
	bz	SEQ_HUMANIZE_Event_NoTrigger
SEQ_HUMANIZE_Event_Trigger
	movlw	0x17		; only note number varied
	rgoto	SEQ_HUMANIZE_Event_Trigger_Cont

SEQ_HUMANIZE_Event_NoTrigger	
	;; check if humanizer active
	movlw	SEQ_TRKHUMANIZEx
	movf	PLUSW0, W
SEQ_HUMANIZE_Event_Trigger_Cont
	movwf	TMP1
	andlw	0x0f
	movwf	TMP2

	;; TMP1 contains the humanize mode
	;; TMP1[4] set:	modify 2nd byte
	;; TMP1[5] set: modify 3rd byte
	;; TMP1[6] set: modify gatelength

	;; TMP2 contains the humanize intensity

	movf	TMP2, W
	bz	SEQ_HUMANIZE_Event_End

	;; EVNT1: only if note event
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bnz	SEQ_HUMANIZE_Event_NoNote
SEQ_HUMANIZE_Event_Note
	lfsr	FSR1, SEQ_EVNT1
	RCALL_IFSET TMP1, 4, ACCESS, SEQ_HUMANIZE_ModifyNote
SEQ_HUMANIZE_Event_NoNote

SEQ_HUMANIZE_Event_VelCC
	lfsr	FSR1, SEQ_EVNT2
	RCALL_IFSET TMP1, 5, ACCESS, SEQ_HUMANIZE_Modify
	
SEQ_HUMANIZE_Event_Length
	lfsr	FSR1, SEQ_EVNTL
	RCALL_IFSET TMP1, 6, ACCESS, SEQ_HUMANIZE_Modify
	
SEQ_HUMANIZE_Event_End
	return


;; --------------------------------------------------------------------------
;;  this function applies a random value to a byte
;;  IN: pointer to byte in FSR1
;;      intensity in TMP1
;;  OUT: modified byte
;; --------------------------------------------------------------------------
SEQ_HUMANIZE_ModifyNote
	call	SEQ_CORE_GenRandomNumber	; generate new random number

	;; scale depending on intensity
	rlf	TMP2, W
	andlw	0x1e
	rgoto	SEQ_HUMANIZE_Modify_Cont

SEQ_HUMANIZE_Modify
	call	SEQ_CORE_GenRandomNumber	; generate new random number

	;; scale depending on intensity
	rlf	TMP2, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x78

SEQ_HUMANIZE_Modify_Cont
	mulwf	SEQ_RANDOM_SEED_L, BANKED	; scaled result in PRODH

	;; add/subtract depending on SEQ_RANDOM_SEED_H[0]
	BRA_IFCLR SEQ_RANDOM_SEED_H, 0, BANKED, SEQ_HUMANIZE_Modify_Add
SEQ_HUMANIZE_Modify_Sub
	movf	PRODH, W
	subwf	INDF1, W
	;; saturate on overflow
	btfsc	WREG, 7
	movlw 0x00
	movwf	INDF1

	;; value should never be 0x00
	movf	INDF1, W
	skpnz
	incf	INDF1, F
	return

SEQ_HUMANIZE_Modify_Add
	movf	PRODH, W
	addwf	INDF1, W
	;; saturate on overflow
	btfsc	WREG, 7
	movlw 0x7f
	movwf	INDF1
	return
