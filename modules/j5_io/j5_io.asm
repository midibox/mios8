; $Id$
;
;  Wrapper for j5_io.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	J5_IO_Init
	global	J5_IO_Set
	global	J5_IO_PinSet
	global	J5_IO_Get
	global	J5_IO_PinGet

	global	_J5_IO_Init		; (for C, declaration in iic_midi.h)
	global	_J5_IO_Set
	global	_J5_IO_PinSet
	global	_J5_IO_Get
	global	_J5_IO_PinGet


; ==========================================================================
;  Declare variables 
; ==========================================================================


; ==========================================================================
J5_IO code
#include <j5_io.inc>

; ==========================================================================
	;; C function wrappers

;_J5_IO_Init	; (direct parameter passing via WREG)

;_J5_IO_Set	; (direct parameter passing via WREG)

_J5_IO_PinSet	; (unsigned char, unsigned char)
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	rgoto	J5_IO_PinSet

;_J5_IO_Get	; (direct parameter passing via WREG)

;_J5_IO_GetPin	; (direct parameter passing via WREG)

; ==========================================================================
	END
