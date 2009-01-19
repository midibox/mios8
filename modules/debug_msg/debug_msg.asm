; $Id$
;
;  Wrapper for debug_msg.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	DEBUG_MSG_SendHeader
	global	DEBUG_MSG_SendFooter
	global	DEBUG_MSG_SendChar
	;;  global	DEBUG_MSG_SendCString  ; not available for ASM-only version as TMP registers are overwritten
	;; for assembly programs, use DEBUG_MSG_SendASMString instead, which is optimized for this use model!
	global	DEBUG_MSG_SendBCD1
	global	DEBUG_MSG_SendBCD2
	global	DEBUG_MSG_SendBCD3
	global	DEBUG_MSG_SendBCD4
	global	DEBUG_MSG_SendBCD5
	global	DEBUG_MSG_SendHex1
	global	DEBUG_MSG_SendHex2

	global	_DEBUG_MSG_SendHeader
	global	_DEBUG_MSG_SendFooter
	global	_DEBUG_MSG_SendChar
	global	_DEBUG_MSG_SendCString
	global	_DEBUG_MSG_SendBCD1
	global	_DEBUG_MSG_SendBCD2
	global	_DEBUG_MSG_SendBCD3
	global	_DEBUG_MSG_SendBCD4
	global	_DEBUG_MSG_SendBCD5
	global	_DEBUG_MSG_SendHex1
	global	_DEBUG_MSG_SendHex2


; ==========================================================================
;  Declare variables 
; ==========================================================================

; ==========================================================================
DEBUG_MSG code
#include <debug_msg.inc>

; ==========================================================================
	;; C function wrappers

	;; functions which get no or only a single argument are not listed here
	;; (no parameter conversion required)

_DEBUG_MSG_SendBCD4
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	goto	DEBUG_MSG_SendBCD4

_DEBUG_MSG_SendBCD5
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	goto	DEBUG_MSG_SendBCD5


_DEBUG_MSG_SendCString
	;; transfer pointer into TBLPTR[LHU]
	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movf	PREINC2, W

	;; SDCC passes a 24bit pointer, the MSB (now in WREG) bit 6 and 7 tells us, where the data is located:
	;; 00 -> code
	;; 01 -> EEPROM (not supported by DEBUG_MSG_PrintCString yet)
	;; 10 -> data
	;; 11 -> unimplemented

	btfsc	WREG, 7
	bra	_DEBUG_MSG_SendCString_RAM
_DEBUG_MSG_SendCString_Code
	movwf	TBLPTRU
_DEBUG_MSG_SendCStringLoop_Code
	tblrd*+
	movf	TABLAT, W
	bz	_DEBUG_MSG_SendCString_Code_End
	call	DEBUG_MSG_SendChar
	bra	_DEBUG_MSG_SendCStringLoop_Code
_DEBUG_MSG_SendCString_Code_End
	return

_DEBUG_MSG_SendCString_RAM
	;; using FSR2 as temporary pointer - store current FSR2 into TMP[12]
	movff	FSR2L, TMP1
	movff	FSR2H, TMP2
	movff	TBLPTRL, FSR2L
	movff	TBLPTRH, FSR2H
_DEBUG_MSG_SendCStringLoop_RAM
	movf	POSTINC2, W
	bz	_DEBUG_MSG_SendCString_RAM_End
	call	DEBUG_MSG_SendChar
	bra	_DEBUG_MSG_SendCStringLoop_RAM
_DEBUG_MSG_SendCString_RAM_End
	;; restore FSR2
	movff	TMP1, FSR2L
	movff	TMP2, FSR2H
	return


; ==========================================================================
	END
