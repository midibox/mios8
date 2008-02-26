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

SEQ_FSR_TrgXXX
	movff	SEQ_EVNTT, WREG
	mullw	SEQ_TRGRECORD_LENGTH
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
;;  This function returns a FSR1 pointer to the SEQ_TRKCHN_x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkChn
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKCHNx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKNOTEx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkNote
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKNOTEx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKVELx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkVel
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKVELx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to SEQ_TRKVELAx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkVelA
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKVELAx
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
;;  This function returns a FSR1 pointer to the SEQ_TRKLASTx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkLast
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKLASTx
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
;;  This function returns a FSR1 pointer to the SEQ_TRKASSGNx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkAssgn
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKASSGNx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRGx+0 depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrgGate
	lfsr	FSR1, SEQ_TRG0 + 0
	rgoto	SEQ_FSR_TrgXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRGx+8 depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrgAux
	lfsr	FSR1, SEQ_TRG0 + 8
	rgoto	SEQ_FSR_TrgXXX
