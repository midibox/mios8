; $Id$
;
; Reset Vector
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;;  Reset vector
;; ==========================================================================
#if MIOS_BSL_FLAG
	org	0x0400
	goto	Start		; will be overwritten by BSL
	dw	0x4711		; magic number - if != 0x4711, the bootloader won't branch to 0x0400!
#else
	org	0x0000
	goto	Start
	goto	Start
#endif

