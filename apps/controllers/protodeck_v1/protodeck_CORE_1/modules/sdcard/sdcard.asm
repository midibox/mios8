; $Id: sdcard.asm 378 2008-07-06 20:38:32Z tk $
;
;  Wrapper for sdcard.inc
;  To be used for relocatable programs
;
; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	SDCARD_Init
	global	SDCARD_SectorRead
	global	SDCARD_SectorWrite

	global	_SDCARD_Init		; (for C, declaration in sdcard.h)
	global	_SDCARD_SectorRead
	global	_SDCARD_SectorWrite

	global	_sdcard_buffer_p0
	global	_sdcard_buffer_p1
	global	_sdcard_address

	global	_sdcard_response
	global	_sdcard_cmdcode
	global	_sdcard_crc



; ==========================================================================
;  Declare variables 
; ==========================================================================

;; buffer splitted over two pages - in order to allow more flexibility,
;; we don't use a continuous data block of 512 bytes, but 2 * 256 bytes
sdcard_buf_p0		udata
_sdcard_buffer_p0	res	0x100
sdcard_buf_p1		udata
_sdcard_buffer_p1	res	0x100

sdcard_vars		udata
_sdcard_mode		res	1

_sdcard_address		res	4

_sdcard_response	res	1
_sdcard_cmdcode		res	1
_sdcard_crc		res	1

; ==========================================================================
SDCARD code
#include <sdcard.inc>

; ==========================================================================
	;; C function wrappers

;_SDCARD_Init	; (no parameters)

_SDCARD_SectorRead
	movff	FSR0L, FSR2L
	movff	WREG, _sdcard_address+0
 	movff	PREINC2, _sdcard_address+1
 	movff	PREINC2, _sdcard_address+2
 	movff	PREINC2, _sdcard_address+3
	rgoto	SDCARD_SectorRead
	
_SDCARD_SectorWrite
	movff	FSR0L, FSR2L
	movff	WREG, _sdcard_address+0
 	movff	PREINC2, _sdcard_address+1
 	movff	PREINC2, _sdcard_address+2
 	movff	PREINC2, _sdcard_address+3
	rgoto	SDCARD_SectorWrite
	

; ==========================================================================

	END
