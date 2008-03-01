; $Id$
;
;  Wrapper for blm.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

;; grrrr! "#if defined(BLM_NO_DEBOUNCING) && BLM_NO_DEBOUNCING == 0" not supported by GPASM
;; therefore we create a helper #define
#ifdef BLM_NO_DEBOUNCING
#define _BLM_NO_DEBOUNCING BLM_NO_DEBOUNCING
#else
#define _BLM_NO_DEBOUNCING 0
#endif

; ==========================================================================

	;; export lables
	global	BLM_Init
	global	BLM_PrepareCol
	global	BLM_GetRow
	global	BLM_ButtonHandler

	global	_BLM_Init		; (compiler removes the "_", see blm.h)
	global	_BLM_PrepareCol
	global	_BLM_GetRow
	global	_BLM_ButtonHandler

	global	_blm_button_column
	global	_blm_button_row
	global	_blm_button_value

;; grrrr! "#if defined(BLM_NO_DEBOUNCING) && BLM_NO_DEBOUNCING == 0" not supported by GPASM
;; therefore this strange construct!
#if _BLM_NO_DEBOUNCING == 0
	global	_blm_button_debounce_delay
#endif

	global	_blm_row_green
	global	_blm_row_red

	;; import lables
	extern	_BLM_NotifyToggle	; located in external program


; ==========================================================================
;  Declare variables 
; ==========================================================================

blm_vars		udata		; (no access ram required, these variables can be located anywhere)

_blm_button_column	res	1	; exported to C, therefore an "_" has been added
_blm_button_row		res	1
_blm_button_value	res	1

#if _BLM_NO_DEBOUNCING == 0
_blm_button_debounce_delay res	1
blm_button_debounce_ctr	res	64
#endif

blm_selected_column	res	1
blm_button_column_ctr	res	1

blm_button_row_values	res	8
blm_button_row_changed	res	8

_blm_row_green		res	8
_blm_row_red		res	8

; ==========================================================================
BLM code
#include <blm.inc>

; ==========================================================================
	;; C function wrappers

;_BLM_Init		; (no parameters)
;_BLM_PrepareCol	; (no parameters)
;_BLM_GetRow		; (no parameters)
;_BLM_ButtonHandler	; (no parameters)

; ==========================================================================
	END
