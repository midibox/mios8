; $Id$
;
; Reset Vector
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	;; note: bootloader jumps to 0x0400 after timeout
	org	0x0400
	goto	MIOS_STARTUP
	dw	0x4711		; magic number - if != 0x4711, the bootloader won't branch to 0x0400!
