; $Id$
;
; MIOS Application Tick
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; check if a MIDI clock has been requested
	;; if not, leave tick routine
	btfss	MMD_MIDI_CLOCK_REQUEST, 0
	return

	;; clear request
	bcf	MMD_MIDI_CLOCK_REQUEST, 0

	;; send MIDI clock
	movlw	0xf8
	call	MIOS_MIDI_TxBufferPut

	;; and call clock handler
	call	MMD_CORE_ClockHandler

	return

