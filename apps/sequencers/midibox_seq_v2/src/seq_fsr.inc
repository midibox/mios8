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
;;  This function returns a FSR0 pointer to the pot layer value depending on
;;  SEQ_EVNTS (step pos) and SEQ_EVNTT (track) and the currently selected layer
;; --------------------------------------------------------------------------
SEQ_FSR_PotValue
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_POT_VALUES_A_00		; if Layer A selected
	btfsc	SEQ_SELECTED_LAYERS, 1, BANKED
	lfsr	FSR0, SEQ_POT_VALUES_B_00		; if Layer B selected
	btfsc	SEQ_SELECTED_LAYERS, 2, BANKED
	lfsr	FSR0, SEQ_POT_VALUES_C_00		; if Layer C selected
	movf	SEQ_EVNTS, W, BANKED			; add step offset
	andlw	0x0f
	addwf	FSR0L, F
	swapf	SEQ_EVNTT, W, BANKED
	andlw	0x30
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 (!) pointer to the pot layer value
;;  depending on WREG[1:0]
;; --------------------------------------------------------------------------
SEQ_FSR_PotValueWREG
	lfsr	FSR1, SEQ_POT_VALUES_A_00
	btfsc	WREG, 0
	lfsr	FSR1, SEQ_POT_VALUES_B_00
	btfsc	WREG, 1
	lfsr	FSR1, SEQ_POT_VALUES_C_00
	return

;; --------------------------------------------------------------------------
;;  used by various SEQ_FSR_Trk* functions which selects an item of the
;;  trackrecord
;; --------------------------------------------------------------------------
SEQ_FSR_TrkXXX
	movff	SEQ_EVNTT, WREG
	mullw	SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	return

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKASSGNx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkAssgn
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKASSGNx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKTRANSPx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkTransp
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKTRANSPx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKEVNT0_x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkEvnt0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKEVNT0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKDIVLENx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkDivLen
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKDIVLENx
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
SEQ_FSR_TrkState
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKSTATEx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKPOSx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkPos
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKPOSx
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKMUTED0x depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkMuted0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKMUTED0x
	rgoto	SEQ_FSR_TrkXXX

;; --------------------------------------------------------------------------
;;  This function returns a FSR1 pointer to the SEQ_TRKMORPHx depending on
;;  SEQ_EVNTT.
;; --------------------------------------------------------------------------
SEQ_FSR_TrkMorph0
	lfsr	FSR1, SEQ_TRK0 + SEQ_TRKMORPHx
	rgoto	SEQ_FSR_TrkXXX




;; --------------------------------------------------------------------------
;;  returns the offset to SEQ_D_TRKx_y depending on SEQ_EVNTT and SEQ_EVNTD
;; --------------------------------------------------------------------------
SEQ_FSR_D_Trkx_y
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_D_TRK0_A
	btfsc	SEQ_EVNTD, 0, BANKED
	lfsr	FSR1, SEQ_D_TRK0_B
	btfsc	SEQ_EVNTD, 1, BANKED
	lfsr	FSR1, SEQ_D_TRK0_C

	movf	SEQ_EVNTT, W, BANKED
	mullw	SEQ_D_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	return
