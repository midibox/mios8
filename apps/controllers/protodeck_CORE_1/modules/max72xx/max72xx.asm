; $Id: max72xx.asm 233 2008-03-28 22:59:40Z tk $
;
;  Wrapper for max72xx.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	MAX72XX_Init
	global	MAX72XX_UpdateDigits
	global	MAX72XX_UpdateAllDigits

	global	MAX72XX_WriteReg
	global	MAX72XX_WriteRegs

	global	_MAX72XX_Init		; (for C, declaration in max72xx.h)
	global	_MAX72XX_UpdateDigits
	global	_MAX72XX_UpdateAllDigits

	global	_MAX72XX_WriteReg
	global	_MAX72XX_WriteRegs

	global	_max72xx_digits


; ==========================================================================
MAX72XX code
#include <max72xx.inc>

; ==========================================================================
	;; C function wrappers

;_MAX72XX_Init	; (no parameters)

;_MAX72XX_UpdateDigits		; (direct parameter passing via WREG)
;_MAX72XX_UpdateAllDigits	; (no parameters)

_MAX72XX_WriteReg		; (unsigned char, unsigned char, unsigned char)
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER2	; address
 	movff	PREINC2, MIOS_PARAMETER1	; data
	rgoto	MAX72XX_WriteReg

_MAX72XX_WriteRegs		; (unsigned char, unsigned char)
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1	; data
	rgoto	MAX72XX_WriteRegs

; ==========================================================================

; ==========================================================================
;  Declare variables 
; ==========================================================================

;; this is done at the bottom, since MAX72XX_CHAINLENGTH hasn't been defined earlier (it's done in max72xx.inc!)
max72xx_vars		udata
_max72xx_digits		res	MAX72XX_CHAINLENGTH*8

	END
