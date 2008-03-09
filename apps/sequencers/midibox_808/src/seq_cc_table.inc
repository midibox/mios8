; $Id$
;
; MIDIboxSEQ
; CC Table
; used by seq_ccin.inc and seq_ccout.inc
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; predefined CC numbers
SEQ_CC_OFFSET_MODE		EQU	0x20
SEQ_CC_OFFSET_MODE_FLAGS	EQU	0x21
SEQ_CC_OFFSET_MIDI_NOTE		EQU	0x22
SEQ_CC_OFFSET_MIDI_VEL		EQU	0x24
SEQ_CC_OFFSET_MIDI_VELA		EQU	0x25
SEQ_CC_OFFSET_MIDI_CHANNEL	EQU	0x26
SEQ_CC_OFFSET_MIDI_PORT		EQU	0x27

SEQ_CC_OFFSET_DIRECTION		EQU	0x28
SEQ_CC_OFFSET_DIRPAR_REPLAY	EQU	0x29
SEQ_CC_OFFSET_DIRPAR_FORWARD	EQU	0x2a
SEQ_CC_OFFSET_DIRPAR_JMPBCK	EQU	0x2b
SEQ_CC_OFFSET_CLK_DIVIDER	EQU	0x2c
SEQ_CC_OFFSET_LAST		EQU	0x2d
SEQ_CC_OFFSET_LOOP		EQU	0x2e
SEQ_CC_OFFSET_CLKDIV_FLAGS	EQU	0x2f

SEQ_CC_OFFSET_GROOVE_VALUE	EQU	0x32
SEQ_CC_OFFSET_GROOVE_STYLE	EQU	0x33
SEQ_CC_OFFSET_MORPH_MODE	EQU	0x34
SEQ_CC_OFFSET_MORPH_SPARE	EQU	0x35
SEQ_CC_OFFSET_HUMANIZE_VALUE	EQU	0x36
SEQ_CC_OFFSET_HUMANIZE_MODE	EQU	0x37

SEQ_CC_OFFSET_AUX_ASSGN		EQU	0x38

; ==========================================================================

SET_CC_NOP		EQU	0x00
SET_CC_7		EQU	0x01
SET_CC_6L		EQU	0x02
SET_CC_3L		EQU	0x03
SET_CC_4L		EQU	0x04
SET_CC_4U		EQU	0x05
SET_CC_B10		EQU	0x06
SET_CC_B32		EQU	0x07
SET_CC_B54		EQU	0x08
SET_CC_B76		EQU	0x09

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG, track number in MIOS_PARAMETER2
;;  OUT: reads CC table and returns command and parameter
;; --------------------------------------------------------------------------
SEQ_CC_TABLE_Get
	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access

	clrc
	rlf	WREG, W
	addlw	SEQ_CC_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SEQ_CC_TABLE >> 8
	addwfc	TBLPTRH, F

	lfsr	FSR1, SEQ_TRK0		; prepare FSR1
	movf	MIOS_PARAMETER2, W	; add track offset
	mullw	SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> command
	tblrd*+
	movf	TABLAT, W		; -> parameter
	addwf	FSR1L, F
	return


; ==========================================================================

SEQ_CC_TABLE
	;; 0x00-0x0f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x10-0x1f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x20-0x27
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_7,		SEQ_TRKNOTEx
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_7,		SEQ_TRKVELx
	db	SET_CC_7,		SEQ_TRKVELAx
	db	SET_CC_4L,		SEQ_TRKCHNx
	db	SET_CC_4U,		SEQ_TRKCHNx

	;; 0x28-0x2f
	db	SET_CC_4L,		SEQ_TRKDIR1x
	db	SET_CC_4U,		SEQ_TRKDIR1x
	db	SET_CC_4L,		SEQ_TRKDIR2x
	db	SET_CC_4U,		SEQ_TRKDIR2x
	db	SET_CC_6L,		SEQ_TRKDIVx
	db	SET_CC_7,		SEQ_TRKLASTx
	db	SET_CC_7,		SEQ_TRKLOOPx
	db	SET_CC_B76,		SEQ_TRKDIVx
	
	;; 0x30-0x3f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_4L,		SEQ_TRKGROOVEx
	db	SET_CC_4U,		SEQ_TRKGROOVEx
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_4L,		SEQ_TRKASSGNx
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x40-0x4f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x50-0x5f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x60-0x6f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
	;; 0x70-0x7f
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	db	SET_CC_NOP,		0		; spare
	
; ==========================================================================
