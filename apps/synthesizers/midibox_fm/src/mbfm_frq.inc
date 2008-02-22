; $Id$
;
; MIDIbox FM
; Access to the frequency tables
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;; In: note index (0x00-0x7f) in WREG
;; Out: FNUML in WREG and MIOS_PARAMETER1, FNUMH+BLOCK in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_FRQ_Get
	clrc
	rlf	WREG, W
	addlw	MBFM_FRQ_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MBFM_FRQ_TABLE >> 8
MBFM_FRQ_ALT_Get_Cont
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2
	movf	MIOS_PARAMETER1, W
	return

; ==========================================================================

	;; the table is located in mbfm_frq_table.inc

; ==========================================================================

;; --------------------------------------------------------------------------
;; In: note index (0x00-0x7f) in WREG
;; Out: FNUML in WREG and MIOS_PARAMETER1, FNUMH+BLOCK in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBFM_FRQ_ALT_Get
	clrc
	rlf	WREG, W
	addlw	MBFM_FRQ_TABLE_ALT & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	MBFM_FRQ_TABLE_ALT >> 8
	rgoto	MBFM_FRQ_ALT_Get_Cont

; ==========================================================================

	;; the table is located in mbfm_frq_table_alt.inc
