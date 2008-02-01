; $Id$
;
; Wrapper for GLCD fonts
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================
; Export Label
; ==========================================================================
	global	_GLCD_FONT_BIG

; ==========================================================================
; Start code section and include font
; ==========================================================================
GLCD_FONT CODE
_GLCD_FONT_BIG
#include "glcd_font_big.inc"
	END
