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
SEQ_CC_OFFSET_MIDI_CHANNEL	EQU	0x00
SEQ_CC_OFFSET_MIDI_EVNT0	EQU	0x04
SEQ_CC_OFFSET_MODE		EQU	0x08
SEQ_CC_OFFSET_DIRECTION		EQU	0x0c
SEQ_CC_OFFSET_CLK_DIVIDER	EQU	0x10
SEQ_CC_OFFSET_LENGTH		EQU	0x14
SEQ_CC_OFFSET_ASSIGNS_E01	EQU	0x18
SEQ_CC_OFFSET_ASSIGNS_L		EQU	0x1c
SEQ_CC_OFFSET_TRANSPOSE_OCT	EQU	0x20
SEQ_CC_OFFSET_TRANSPOSE_SEMI	EQU	0x24
SEQ_CC_OFFSET_GROOVE_VALUE	EQU	0x28
SEQ_CC_OFFSET_GROOVE_STYLE	EQU	0x2c
SEQ_CC_OFFSET_MUTE_1_4		EQU	0x30
SEQ_CC_OFFSET_MUTE_5_8		EQU	0x34
SEQ_CC_OFFSET_MUTE_9_12		EQU	0x38
SEQ_CC_OFFSET_MUTE_13_16	EQU	0x3c
SEQ_CC_OFFSET_MORPH_MODE	EQU	0x40
SEQ_CC_OFFSET_HUMANIZE_VALUE	EQU	0x48
SEQ_CC_OFFSET_HUMANIZE_MODE	EQU	0x4c

SEQ_CC_OFFSET_LAST_SAVED_ENTRY	EQU	0x43	; (for seq_dump.inc)

SEQ_CC_OFFSET_CFG_L		EQU	0x78
SEQ_CC_OFFSET_CFG_H		EQU	0x79
SEQ_CC_OFFSET_BPM_L		EQU	0x7a
SEQ_CC_OFFSET_BPM_H		EQU	0x7b
SEQ_CC_OFFSET_METRONOME_CHN	EQU	0x7c
SEQ_CC_OFFSET_DEVICE_ID		EQU	0x7f

; ==========================================================================

SET_CC_NOP		EQU	0x00
SET_CC_4L		EQU	0x01
SET_CC_4U		EQU	0x02
SET_CC_CFG_4L		EQU	0x03
SET_CC_CFG_4U		EQU	0x04
SET_CC_BPM_4L		EQU	0x05
SET_CC_BPM_4U		EQU	0x06
SET_CC_GLOBAL_4L	EQU	0x07
SET_CC_GLOBAL_4U	EQU	0x08

;; --------------------------------------------------------------------------
;;  IN: index of CC parameter in WREG
;;  OUT: reads CC table and returns command and parameter
;; --------------------------------------------------------------------------
SEQ_CC_TABLE_Get
	SET_BSR	SEQ_BASE		; prepare BSR for SEQ register access
	lfsr	FSR1, SEQ_TRK0		; prepare FSR1

	clrc
	rlf	WREG, W
	addlw	SEQ_CC_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	SEQ_CC_TABLE >> 8
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2	; -> command
	tblrd*+
	movff	TABLAT, FSR1L		; -> parameter
	return


; ==========================================================================

SEQ_CC_TABLE
	;; 0x00-0x07
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKEVNT0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKEVNT0x) & 0xff

	;; 0x08-0x0f
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKMODEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKMODEx) & 0xff

	;; 0x10-0x17
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKDIVLENx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKDIVLENx) & 0xff

	;; 0x18-0x1f
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKASSGNx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKASSGNx) & 0xff

	;; 0x20-0x27
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKTRANSPx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKTRANSPx) & 0xff

	;; 0x28-0x2f
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKGROOVEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKGROOVEx) & 0xff

	;; 0x30-0x3f
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKMUTED0x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKMUTED1x) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKMUTED1x) & 0xff

	;; 0x40-0x4f
	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKMORPHx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKMORPHx) & 0xff

	db	SET_CC_4L,		(SEQ_TRK0 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK1 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK2 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4L,		(SEQ_TRK3 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK0 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK1 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK2 + SEQ_TRKHUMANIZEx) & 0xff
	db	SET_CC_4U,		(SEQ_TRK3 + SEQ_TRKHUMANIZEx) & 0xff

	;; 0x50-0x5f
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00

	;; 0x60-0x6f
	db	SET_CC_GLOBAL_4L,	SEQ_MORPH_PATTERN0 & 0xff
	db	SET_CC_GLOBAL_4U,	SEQ_MORPH_PATTERN0 & 0xff
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00

	;; 0x70-0x77
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00

	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00
	db	SET_CC_NOP,		0x00

	;; 0x78-0x7f
	db	SET_CC_CFG_4L,		SEQ_CFG0 & 0xff
	db	SET_CC_CFG_4U,		SEQ_CFG0 & 0xff
	db	SET_CC_BPM_4L,		SEQ_BPM & 0xff
	db	SET_CC_BPM_4U,		SEQ_BPM & 0xff
	db	SET_CC_GLOBAL_4L,	SEQ_METRONOME_CHANNEL & 0xff
	db	SET_CC_NOP,		0x00

	db	SET_CC_NOP,		0x00	; (cannot be overwritten)
	db	SET_CC_NOP,		0x00	; (cannot be overwritten)

; ==========================================================================
