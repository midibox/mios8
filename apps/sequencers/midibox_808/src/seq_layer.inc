; $Id$
;
; MIDIboxSEQ
; Layer specific functions
;
; This is the only place where we get informations about the layers
;
; If adaptions/extensions in the layers are made, they all have to be done
; here!
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns the complete MIDI event of the given track, step and layer
;;  IN: SEQ_EVNTT: track number (0-15)
;;      SEQ_EVNTS: step number (0-63)
;;  OUT: the MIDI event in SEQ_EVNT[012]
;;       the gatelength in SEQ_EVNTL
;;  USES: TBLPTR[LH], TABLAT, EEADR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_GetEvnt
	;; select SEQ_PATTERN/BANK depending on track
	rcall	SEQ_LAYER_Hlp_SetPattern

SEQ_LAYER_GetEvnt_AltPattern	; direct entry point for SEQ_MORPH: alternative pattern defined in SEQ_PATTERN/SEQ_PATTERN_BANK

	;; whole pattern disabled (muted) if SEQ_PATTERNx[7] set
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_PATTERN, 7, BANKED, SEQ_LAYER_GetEvnt_Clr

	;; status byte is always 0x9n
	movlw	0x90
	movwf	SEQ_EVNT0, BANKED

	;; add channel
	call	SEQ_FSR_TrkChn
	movf	INDF1, W
	andlw	0x0f
	iorwf	SEQ_EVNT0, F, BANKED

	;; don't play if step is muted
	call	SEQ_TRG_GateGet
	skpnz
	rgoto	SEQ_LAYER_GetEvnt_Clr

	;; get note number
	call	SEQ_FSR_TrkNote
	movff	INDF1, SEQ_EVNT1

	;; note velocity is constant value depending on accent flag
	call	SEQ_TRG_AccentGet
	bnz	SEQ_LAYER_GetEvnt_Accent
SEQ_LAYER_GetEvnt_NoAccent
	call	SEQ_FSR_TrkVel
	rgoto	SEQ_LAYER_GetEvnt_Accent_Cont
SEQ_LAYER_GetEvnt_Accent
	call	SEQ_FSR_TrkVelA
SEQ_LAYER_GetEvnt_Accent_Cont
	movff	INDF1, SEQ_EVNT2

	;; continue at event length
	rgoto	SEQ_LAYER_GetEvntL


	;; used when a certain layer shouldn't play an event
SEQ_LAYER_GetEvnt_Clr
	clrf	SEQ_EVNT0, BANKED
	clrf	SEQ_EVNT1, BANKED
	clrf	SEQ_EVNT2, BANKED

	;; if pattern disabled: set length to 0 as well and exit
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_LAYER_GetEvntL
SEQ_LAYER_GetEvnt_PtnDisabled
	clrf	SEQ_EVNTL, BANKED
	return

SEQ_LAYER_GetEvntL
	movlw	0x10			; fixed length
	movwf	SEQ_EVNTL, BANKED

	;; exit if event is 0
	movf	SEQ_EVNT0, W, BANKED
	skpnz
	return

	;; TODO: improve performance of such checks!
	;; TODO: implement micro delay

	;; force length to 0x32 (2x18) if flam1 flag is set
	call	SEQ_TRG_Flam1Get
	movlw	0x32
	skpz
	movwf	SEQ_EVNTL, BANKED

	;; force length to 0x2c (2x12) if flam2 flag is set
	call	SEQ_TRG_Flam2Get
	movlw	0x2c
	skpz
	movwf	SEQ_EVNTL, BANKED

	;; force length to 0x46 (3x6) if flam3 flag is set
	call	SEQ_TRG_Flam3Get
	movlw	0x46
	skpz
	movwf	SEQ_EVNTL, BANKED

	;; force length to 0x64 (4x4) if flam4 flag is set
	call	SEQ_TRG_Flam4Get
	movlw	0x44
	skpz
	movwf	SEQ_EVNTL, BANKED

	return


;; --------------------------------------------------------------------------
;;  This help function sets SEQ_PATTERN/SEQ_PATTERN_BANK depending on
;;  the selected track
;;  IN: SEQ_EVNTT: track number (0-3)
;;  OUT: SEQ_PATTERN and SEQ_PATTERN_BANK
;;  USES: FSR1
;; --------------------------------------------------------------------------
SEQ_LAYER_Hlp_SetPattern
	;; MB808 has only one pattern
	movff	SEQ_PATTERN0, SEQ_PATTERN
	movff	SEQ_PATTERN_BANK0, SEQ_PATTERN_BANK
	return


;; --------------------------------------------------------------------------
;;  This function saves a MIDI event to the given track and step
;;  IN: SEQ_EVNTT: track number (0-15)
;;      SEQ_EVNTS: step number (0-63)
;;      SEQ_EVNT[12] and SEQ_EVNTL: the event/length which should be saved
;;      SEQ_RECORD_MODE: various control flags
;;  USES: TBLPTR[LH], TABLAT, EEADR, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_LAYER_SaveEvnt
	SET_BSR	SEQ_BASE

	;; velocity 0: clear gate, otherwise set gate
	;; TODO: set Aux trigger depending on velocity?
	movf	SEQ_EVNT2, W, BANKED
	skpz
	movlw	1
	call	SEQ_TRG_GateSet

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
SEQ_LAYER_SaveEvntL
	;; TODO?
	return


;; --------------------------------------------------------------------------
;;  This function fetches a preset and copies it into the memory locations
;;  of the currently selected tracks
;; --------------------------------------------------------------------------
SEQ_LAYER_Init
	SET_BSR	SEQ_BASE

	;; go through all tracks
	clrf	SEQ_EVNTT, BANKED
SEQ_LAYER_InitLoop
	;; skip if track not selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	SEQ_LAYER_InitLoop_Next

	;; init cfg data
	rcall	SEQ_LAYER_InitCfg

	;; initialize layers
	rcall	SEQ_LAYER_InitLayer

SEQ_LAYER_InitLoop_Next
	;; increment until last track is reached
	incf	SEQ_EVNTT, F, BANKED	; increment track number
	movlw	SEQ_NUMBER_TRACKS-1
	cpfsgt	SEQ_EVNTT, BANKED
	rgoto SEQ_LAYER_InitLoop

	return



;; --------------------------------------------------------------------------
;;  This function only initializes the config area (also used by the CLR function
;;  in cs_m_util.inc)
;;  IN: track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_LAYER_InitCfg
	;; TODO
	return

;; --------------------------------------------------------------------------
;;  This function only initializes the layers (also used by the CLR function
;;  in cs_m_util.inc)
;;  IN: track number in SEQ_EVNTT
;; --------------------------------------------------------------------------
SEQ_LAYER_InitLayer
	SET_BSR	SEQ_BASE

	;; TODO
	return
