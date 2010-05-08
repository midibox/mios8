; $Id$
;
;  Wrapper for blm_scalar.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

;; grrrr! "#if defined(BLM_SCALAR_NO_DEBOUNCING) && BLM_SCALAR_NO_DEBOUNCING == 0" not supported by GPASM
;; therefore we create a helper #define
#ifdef BLM_SCALAR_NO_DEBOUNCING
#define _BLM_SCALAR_NO_DEBOUNCING BLM_SCALAR_NO_DEBOUNCING
#else
#define _BLM_SCALAR_NO_DEBOUNCING 1
#endif

; ==========================================================================

	;; export lables
	global	BLM_SCALAR_Init
	global	BLM_SCALAR_PrepareCol
	global	BLM_SCALAR_GetRow
	global	BLM_SCALAR_ButtonHandler

	global	_BLM_SCALAR_Init		; (compiler removes the "_", see blm_scalar.h)
	global	_BLM_SCALAR_PrepareCol
	global	_BLM_SCALAR_GetRow
	global	_BLM_SCALAR_ButtonHandler

	global	_blm_scalar_button_column
	global	_blm_scalar_button_row
	global	_blm_scalar_button_value

;; grrrr! "#if defined(BLM_SCALAR_NO_DEBOUNCING) && BLM_SCALAR_NO_DEBOUNCING == 0" not supported by GPASM
;; therefore this strange construct!
#if _BLM_SCALAR_NO_DEBOUNCING == 0
	global	_blm_scalar_button_debounce_delay
#endif

	global	_blm_scalar_row_green
	global	_blm_scalar_row_red

	;; import lables
	extern	_BLM_SCALAR_NotifyToggle	; located in external program


; ==========================================================================
;  Declare variables 
; ==========================================================================

blm_scalar_vars		udata		; (no access ram required, these variables can be located anywhere)

_blm_scalar_button_column	res	1	; exported to C, therefore an "_" has been added
_blm_scalar_button_row		res	1
_blm_scalar_button_value	res	1

#if _BLM_SCALAR_NO_DEBOUNCING == 0
_blm_scalar_button_debounce_delay res	1
blm_scalar_button_debounce_ctr	res	5*64 ; TODO: won't work since value > 256, we should create blocks of 64 bytes
#endif

blm_scalar_selected_column	res	1
blm_scalar_button_column_ctr	res	1

blm_scalar_button_row_values	res	5*8
blm_scalar_button_row_changed	res	5*8

_blm_scalar_row_green		res	5*8
_blm_scalar_row_red		res	5*8

; ==========================================================================
BLM_SCALAR code
#include <blm_scalar.inc>

; ==========================================================================
	;; C function wrappers

;_BLM_SCALAR_Init		; (no parameters)
;_BLM_SCALAR_PrepareCol	; (no parameters)
;_BLM_SCALAR_GetRow		; (no parameters)
;_BLM_SCALAR_ButtonHandler	; (no parameters)

; ==========================================================================
	END
