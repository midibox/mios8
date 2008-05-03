; $Id$
;
; Specific Defines and variables of MIOS Application
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; free memory for user applications:	
	;; 0x010-0x37f

DISPLAY_UPDATE_REQ	EQU	0x010	; bit 0 will be set to request a display update

J5_STATUS		EQU	0x011	; contains the status of J5, controlled via MIDI

