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

LAST_PIN		EQU	0x011	; number of last toggled pin
LAST_PIN_VALUE		EQU	0x012	; value of last toggled pin

J5_DIN_STATUS		EQU	0x020	; used by J5_DIN_Update to store the current status
J5_DIN_STATUS_CHANGED	EQU	0x021	; used by J5_DIN_Update to notify that an input has been toggled

