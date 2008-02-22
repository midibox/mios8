; $Id$
;
; Selection Matrix Table
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table defines the parameters which will be modified depending on
;;  the row/column of the selection matrix
;; 
;;  See also cs_menu_tables.inc for the available parameters
;;  
;;  Available Modes:
;;     0 - no special action
;;     1 - selects the operator depending on the moved encoder
;;     2 - selects LFO1
;;     3 - selects LFO2
;;     4 - selects BD Modulator
;;     5 - selects BD Carrier
;;     6 - selects HH Open
;;     7 - selects HH Close
s;; --------------------------------------------------------------------------
CSMATRIX_ENTRY MACRO mode, menu, off0, cur0, off1, cur1, off2, cur2, off3, cur3
	db	mode, menu, ((off0&0xf)<<4) | (cur0&0xf), ((off1&0xf)<<4) | (cur1&0xf), ((off2&0xf)<<4) | (cur2&0xf), ((off3&0xf)<<4) | (cur3&0xf)
	ENDM

CS_MENU_MATRIX_TABLE
	;; Column 1, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	0,     2,         0,     2,         0,     2,         0,     2		; Volume
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	0,     3,         0,     3,         0,     3,         0,     3		; Multiplier
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	0,     1,         0,     1,         0,     1,         0,     1		; Waveform
	CSMATRIX_ENTRY	0,	CS_MENU_INS,	0,     3,         0,     4,         0,     5,         0,     6		; Instrument

	;; Column 2, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	4,     4,         4,     4,         4,     4,         4,     4		; Attack
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	4,     5,         4,     5,         4,     5,         4,     5		; Decay
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	4,     6,         4,     6,         4,     6,         4,     6		; Sustain
	CSMATRIX_ENTRY	1,	CS_MENU_OP,	4,     7,         4,     7,         4,     7,         4,     7		; Release

	;; Column 3, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	2,	CS_MENU_LFO,	0,     1,         0,     3,         0,     4,         0,     5		; LFO1
	CSMATRIX_ENTRY	3,	CS_MENU_LFO,	0,     1,         0,     3,         0,     4,         0,     5		; LFO2
	CSMATRIX_ENTRY	0,	CS_MENU_EG,	0,     0,         0,     1,         0,     2,         0,     3		; EG5
	CSMATRIX_ENTRY	0,	CS_MENU_WT,	0,     0,         0,     2,         0,     3,         0,     4		; WT

	;; Column 4, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	0,	CS_MENU_USER,	0,     0,         0,     1,         0,     2,         0,     3		; USER1
	CSMATRIX_ENTRY	0,	CS_MENU_USER,	0,     4,         0,     5,         0,     6,         0,     7		; USER2
	CSMATRIX_ENTRY	0,	CS_MENU_USER,	8,     8,         8,     9,         8,    10,         8,    11		; USER3
	CSMATRIX_ENTRY	0,	CS_MENU_USER,	8,    12,         8,    13,         8,    14,         8,    15		; USER4

	;; Column 5, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	4,	CS_MENU_BD,	0,     1,         0,     2,         0,     3,         0,     5		; BD Modulator
	CSMATRIX_ENTRY	6,	CS_MENU_HH,	0,     1,         0,     2,         0,     3,         0,     5		; HH Open
	CSMATRIX_ENTRY	0,	CS_MENU_SD,	0,     0,         0,     1,         0,     2,         0,     4		; Snare
	CSMATRIX_ENTRY	0,	CS_MENU_TOM,	0,     0,         0,     1,         0,     2,         0,     4		; Tom

	;; Column 6, Row 1-4
	;;		mode	menu         offset/cursor #1, offset/cursor #2, offset/cursor #3, offset/cursor #4
	CSMATRIX_ENTRY	5,	CS_MENU_BD,	0,     1,         0,     2,         0,     3,         0,     5		; BD Carrier
	CSMATRIX_ENTRY	7,	CS_MENU_HH,	0,     1,         0,     2,         0,     3,         0,     5		; HH Close
	CSMATRIX_ENTRY	0,	CS_MENU_CYM,	0,     0,         0,     1,         0,     2,         0,     4		; Cymbal
	CSMATRIX_ENTRY	0,	CS_MENU_FRQ,	0,     0,         0,     1,         0,     2,         0,     4		; Frequency


;; --------------------------------------------------------------------------
;;  This function returns the pointer to the table entry depending on
;;  the selected row/column
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_TABLE_GetPtr
	;; get pointer to first entry
	TABLE_ADDR CS_MENU_MATRIX_TABLE

	;; column is stored in CS_MENU_MATRIX_SELECTED[6..4]
	swapf	CS_MENU_MATRIX_SELECTED, W
	andlw	0x07
	mullw	4

	;; row is stored in CS_MENU_MATRIX_SELECTED[3..0]
	movf	CS_MENU_MATRIX_SELECTED, W
	andlw	0x0f
	addwf	PRODL, W

	;; each table entry consists of 6 bytes
	mullw	6

	;; add offset to table pointer
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	return
