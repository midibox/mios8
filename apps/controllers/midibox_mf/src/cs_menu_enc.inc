; $Id$
;
; Control Surface Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: incrementer of menu encoder in WREG
;; --------------------------------------------------------------------------
CS_MENU_ENC_Handler
	;; store incrementer in TMP3
	movwf	TMP3

	return
