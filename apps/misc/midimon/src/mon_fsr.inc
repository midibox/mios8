; $Id$
;
; MIDImon FSR functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function calculates the pointer to a trace memory entry
;;  IN: number of entry in MON_CURRENT_ENTRY_PTR
;;  OUT: pointer to first byte of entry in FSR0
;; --------------------------------------------------------------------------
MON_FSR_TraceEntry
	lfsr	FSR0, MON_TRACE_MEM_BEGIN
	movf	MON_CURRENT_ENTRY_PTR, W
	mullw	3
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	return
