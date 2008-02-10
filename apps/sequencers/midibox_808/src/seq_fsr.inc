; $Id$
;
; MIDIboxSEQ
; FSR pointer return functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  used by various SEQ_FSR_Trk* functions which selects an item of the
;;  trackrecord
;; --------------------------------------------------------------------------
SEQ_FSR_TrkXXX
	movff	SEQ_EVNTT, WREG
	mullw	SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

SEQ_FSR_TrkVarXXX
	movff	SEQ_EVNTT, WREG
	mullw	SEQ_TRKVARRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKTRANSPx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkTransp
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKTRANSPx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKEVNT_x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkEvnt
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKEVNTx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKCHN_x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkChn
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKCHNx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKEVNTCONST1x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkEvntConst1
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKEVNTCONST1x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKDIVx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkDiv
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKDIVx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKLOOPx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkLoop
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKLOOPx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKLENx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkLen
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKLENx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKMODEx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkMode
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKMODEx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKSTATEx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkVarState
	lfsr	FSR1, SEQ_TRKVAR0 + SEQ_TRKVARSTATEx
	rgoto	SEQ_FSR_TrkVarXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKSTATE2x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkVarState2
	lfsr	FSR1, SEQ_TRKVAR0 + SEQ_TRKVARSTATE2x
	rgoto	SEQ_FSR_TrkVarXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKSTEPx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkVarStep
	lfsr	FSR1, SEQ_TRKVAR0 + SEQ_TRKVARSTEPx
	rgoto	SEQ_FSR_TrkVarXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKASSGN0x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkAssgn0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKASSGN0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKASSGN1x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkAssgn1
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKASSGN1x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKTRGA_0x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkTrgA_0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKTRGA_0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKTRGB_0x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkTrgB_0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKTRGB_0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKTRGC_0x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkTrgC_0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKTRGC_0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKMORPHx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkMorph0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKMORPHx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKCHAINx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkChainx
	SET_BSR	SEQ_BASE
	
	BRA_IFSET SEQ_EVNTT, 3, BANKED, SEQ_FSR_TrkChainx_P23
SEQ_FSR_TrkChainx_P01
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKCHAINx
	btfsc	SEQ_EVNTT, 2, BANKED
	lfsr	FSR1, SEQ_TRK4 + SEQ_TRKCHAINx
	return

SEQ_FSR_TrkChainx_P23
	lfsr	FSR1, SEQ_TRK8 + SEQ_TRKCHAINx
	btfsc	SEQ_EVNTT, 2, BANKED
	lfsr	FSR1, SEQ_TRK12 + SEQ_TRKCHAINx
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKMORPH_PATTERNx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkMorphPatternx
	SET_BSR	SEQ_BASE
	
	BRA_IFSET SEQ_EVNTT, 3, BANKED, SEQ_FSR_TrkMorphPatternx_P23
SEQ_FSR_TrkMorphPatternx_P01
	lfsr	FSR1, SEQ_TRK1 + SEQ_TRKMORPH_PATTERNx
	btfsc	SEQ_EVNTT, 2, BANKED
	lfsr	FSR1, SEQ_TRK5 + SEQ_TRKMORPH_PATTERNx
	return

SEQ_FSR_TrkMorphPatternx_P23
	lfsr	FSR1, SEQ_TRK9 + SEQ_TRKMORPH_PATTERNx
	btfsc	SEQ_EVNTT, 2, BANKED
	lfsr	FSR1, SEQ_TRK13 + SEQ_TRKMORPH_PATTERNx
	return
