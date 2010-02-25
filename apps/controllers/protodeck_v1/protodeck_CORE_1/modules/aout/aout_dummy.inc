; $Id: aout_dummy.inc 146 2008-03-01 00:04:43Z tk $
;
; AOUT dummy driver (empty)
; 
; See aout.inc for description of the available functions
;
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Hardware specific initialisation (see aout.inc for details)
;; --------------------------------------------------------------------------
AOUT_InitHW
	;; nothing to do
	return


;; --------------------------------------------------------------------------
;;  Sets the digital pins (see aout.inc for details)
;; --------------------------------------------------------------------------
_AOUT_DigitalPinsSet		; (for C)
AOUT_DigitalPinsSet
	;; nothing to do
	return


;; --------------------------------------------------------------------------
;;  Update function (see aout.inc for details)
;; --------------------------------------------------------------------------
_AOUT_Update			; (for C)
AOUT_Update
	;; nothing to do
	return
