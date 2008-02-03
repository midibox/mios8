; $Id$
;
; MIDIboxSEQ
; Chord functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

CHORD_ENTRY MACRO a, b, c, d, str
	db	a & 0xff, b & 0xff, c & 0xff, d & 0xff, str
	ENDM

	;; note: chords are used together with the forced-to-scale feature in seq_scale.inc
	;; therefore only "major" chords have to be defined here, minor chords are selected with the scale
	;; if no key should be played, add -1
SEQ_CHORD_TABLE
	;;		 1   2   3   4   <----> (6 chars!)
	CHORD_ENTRY	 0,  4,  7, -1, "Maj.I "
	CHORD_ENTRY	 4,  7, 12, -1,	"Maj.II"
	CHORD_ENTRY	 7, 12, 16, -1,	"MajIII"
	CHORD_ENTRY	 0, -1, -1, -1,	"Root  "
	CHORD_ENTRY	 4, -1, -1, -1,	"3rd   "
	CHORD_ENTRY	 7, -1, -1, -1,	"5th   "
	CHORD_ENTRY	 0,  4, -1, -1,	"R.+3rd"
	CHORD_ENTRY	 0,  7, -1, -1,	"R.+5th"
	CHORD_ENTRY	 0,  4,  7,  9,	"Maj.6 "
	CHORD_ENTRY	 0,  4,  7, 11,	"Maj.7 "
	CHORD_ENTRY	 0,  4,  7, 12,	"Maj.8 "
	CHORD_ENTRY	 0,  4,  7, 14,	"Maj.9 "
	CHORD_ENTRY	 0,  7, 12, 16,	"Maj.10"
	CHORD_ENTRY	 0,  7, 12, 19,	"Maj.12"
	CHORD_ENTRY	 0,  5,  7, -1,	"Sus.4 "
	CHORD_ENTRY	 0,  4,  8, -1,	"Maj.+ "

;; --------------------------------------------------------------------------
;;  This function returns the transpose value of a chord with the given key
;;  number (0-3)
;;  IN: MIOS_PARAMETER1: key number 0-3
;;      WREG[3:0]: chord number
;;      WREG[6:4]: octave transpose -4..+3
;;  OUT: key number in WREG, if bit 7 set, no key
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_CHORD_Get
	movwf	TABLAT		; temporary stored in TABLAT (later in PRODL)

	;; get chord note from table
	andlw	0x0f
	mullw	4 + 6
	movf	PRODL, W
	addwf	MIOS_PARAMETER1, W
	movff	TABLAT, PRODL	; TABLAT->PRODL
	addlw	LOW(SEQ_CHORD_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_CHORD_TABLE)
	addwfc	TBLPTRH, F

	tblrd*+
	incf	TABLAT, W	; -1?
	bz	SEQ_CHORD_Get_End

	addlw	0x30-1			; add C-2 (-1 because of previous incf)
	rcall	SEQ_CHORD_Dec12UntilOk	; ensure that note between 0x00..0x7f
	movwf	TABLAT

	;; transpose octave
	swapf	PRODL, W
	andlw	0x07
	addlw	-4
	bz	SEQ_CHORD_TransposeNot
	BRA_IFSET WREG, 7, ACCESS, SEQ_CHORD_TransposeDec
SEQ_CHORD_TransposeInc
	mullw	12
	movf	PRODL, W
	addwf	TABLAT, W
	rcall	SEQ_CHORD_Dec12UntilOk
	rgoto	SEQ_CHORD_Transpose_Cont

SEQ_CHORD_TransposeDec
	mullw	12
	movf	PRODL, W
	addwf	TABLAT, W
	rcall	SEQ_CHORD_Inc12UntilOk
	rgoto	SEQ_CHORD_Transpose_Cont
	
SEQ_CHORD_TransposeNot
	movf	TABLAT, W
SEQ_CHORD_Transpose_Cont

SEQ_CHORD_Get_End
	return


;; --------------------------------------------------------------------------
;;  Decrements WREG by 12 until it reaches the range 0x00..0x7f
;; --------------------------------------------------------------------------
SEQ_CHORD_Dec12UntilOk
	btfss	WREG, 7
	return
	addlw	-12
	rgoto	SEQ_CHORD_Dec12UntilOk

;; --------------------------------------------------------------------------
;;  Increments WREG by 12 until it reaches the range 0x00..0x7f
;; --------------------------------------------------------------------------
SEQ_CHORD_Inc12UntilOk
	btfss	WREG, 7
	return
	addlw	12
	rgoto	SEQ_CHORD_Inc12UntilOk

;; --------------------------------------------------------------------------
;;  This function prints the chord name + octave
;;  IN: WREG[3:0]: chord number
;;      WREG[6:4]: octave transpose -4..+3
;;  OUT: 8 characters
;;  USES: BSR, TBLPTR[LH], TABLAT, PROD[LH]
;; --------------------------------------------------------------------------
SEQ_CHORD_PrintName
	movwf	TABLAT		; temporary stored in TABLAT (later in PRODL)

	;; get chord string from table
	andlw	0x0f
	mullw	4 + 6
	movf	PRODL, W
	addlw	4
	movff	TABLAT, PRODL	; TABLAT->PRODL
	addlw	LOW(SEQ_CHORD_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_CHORD_TABLE)
	addwfc	TBLPTRH, F

	movlw	6
	call	MIOS_LCD_PrintPreconfString

	;; print octave
	movlw	'/'
	call	MIOS_LCD_PrintChar

	swapf	PRODL, W
	andlw	0x07
	goto	MIOS_LCD_PrintBCD1
