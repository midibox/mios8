; $Id$
;
;  Wrapper for aout.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	AOUT_Init
	global	AOUT_ReInit
	global	AOUT_InvertedSet
	global	AOUT_InvertedGet
	global	AOUT_Pin16bitSet
	global	AOUT_Pin7bitSet
	global	AOUT_DigitalPinsSet
	global	AOUT_Update

	global	_AOUT_Init		; (for C, declaration in aout.h)
	global	_AOUT_ReInit
	global	_AOUT_InvertedSet
	global	_AOUT_InvertedGet
	global	_AOUT_Pin16bitSet
	global	_AOUT_Pin7bitSet
	global	_AOUT_DigitalPinsSet
	global	_AOUT_Update


; ==========================================================================
;  Declare variables 
; ==========================================================================

aout_vars		udata
AOUT_INVERTED		res	1
AOUT_UPDATE_REQ		res	1
AOUT_VALUES		res	2*8

; ==========================================================================
AOUT code
#include <aout.inc>

; ==========================================================================
	;; C function wrappers

;_AOUT_Init	; (no parameters)

;_AOUT_ReInit 	; (no parameters)

;_AOUT_InvertedSet	; (direct parameter passing via WREG)

;_AOUT_InvertedGet	; (direct parameter passing via WREG)

_AOUT_Pin16bitSet	; (unsigned char, unsigned int)
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
 	movff	PREINC2, MIOS_PARAMETER2
	rgoto	AOUT_Pin16bitSet

_AOUT_Pin7bitSet	; (unsigned char, unsigned char)
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	rgoto	AOUT_Pin7bitSet

;_AOUT_DigitalPinsSet	; (direct parameter passing via WREG)

;_AOUT_Update	; (no parameters)

; ==========================================================================
	END
