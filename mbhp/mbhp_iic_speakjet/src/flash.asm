; $Id$
;
;  Flash Access Routines
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=16f88
	radix	dec
#include "p16f88.inc"
#include "macros.inc"
#include "globals.inc"

; ==========================================================================

	;; export lables
	global	FLASH_ReadInc

; ==========================================================================
FLASH code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: FLASH_ReadInc
;;  DESCRIPTION: Read data from FLASH and increments address pointer
;;  IN:	  address in EEADR[H]
;;  OUT:  8bit LSB in EEDATA and WREG, 6bit MSB in EEDATH
;;  USES: -
;; --------------------------------------------------------------------------
FLASH_ReadInc
	SWITCHBANK_0_3
	bsf	EECON1, EEPGD		; point to program memory
	bsf	EECON1, RD		; FLASH Read
	nop
	nop
	SWITCHBANK_3_2

	incf	EEADR, F
        skpnz
        incf    EEADRH, F

	movf	EEDATA, W
	SWITCHBANK_2_0
	return

; ==========================================================================
	END
