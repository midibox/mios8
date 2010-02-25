; $Id: sdcard_file.asm 378 2008-07-06 20:38:32Z tk $
;
;  Wrapper for sdcard_file.inc
;  To be used for relocatable programs
;
; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	SDCARD_FILE_Init
	global	SDCARD_FILE_Open
	global	SDCARD_FILE_Seek

	global	_SDCARD_FILE_Init
	global	_SDCARD_FILE_Open
	global	_SDCARD_FILE_Seek

	global	_sdcard_file_mode
	global	_sdcard_file_cluster
	global	_sdcard_file_sector
	global	_sdcard_file_dir_entry_num
	global	_sdcard_file_dir_entry_ptr

	global	_sdcard_file_sector0
	global	_sdcard_file_sector_fat
	global	_sdcard_file_sector_root
	global	_sdcard_file_root_sectors
	global	_sdcard_file_sector_cl2
	global	_sdcard_file_cluster_size

	global	_sdcard_file_seek_offset
	global	_sdcard_file_seek_cluster


	;; import labels from sdcard module
	extern	SDCARD_SectorRead
	extern	_sdcard_address
	extern	_sdcard_buffer_p0
	extern	_sdcard_buffer_p1


; ==========================================================================
;  Declare variables 
; ==========================================================================

sdcard_file_vars	udata
_sdcard_file_mode		res	1
_sdcard_file_name_ptr		res	3
_sdcard_file_cluster		res	2
_sdcard_file_sector		res	4
_sdcard_file_dir_entry_num	res	2
_sdcard_file_dir_entry_ptr	res	3

_sdcard_file_sector0		res	2 ; (was 4)
_sdcard_file_sector_fat		res	2 ; (was 4)
_sdcard_file_sector_root	res	2 ; (was 4)
_sdcard_file_root_sectors	res	1
_sdcard_file_sector_cl2		res	2 ; (was 4)
_sdcard_file_cluster_size	res	1

_sdcard_file_seek_offset	res	4
_sdcard_file_seek_cluster	res	4


; ==========================================================================
SDCARD_FILE code
#include <sdcard_file.inc>

; ==========================================================================
	;; C function wrappers

;_SDCARD_FILE_Init ; (no parameters)

_SDCARD_FILE_Open
	SET_BSR	_sdcard_file_mode
        movwf   _sdcard_file_mode
        movff   FSR0L, FSR2L
        movff   PREINC2, _sdcard_file_name_ptr+0
        movff   PREINC2, _sdcard_file_name_ptr+1
	movff	PREINC2, _sdcard_file_name_ptr+2

	movff	PREINC2, _sdcard_file_dir_entry_num+0
	movff	PREINC2, _sdcard_file_dir_entry_num+1

        ;; SDCC passes a 24bit pointer, the MSB (now in _sdcard_file_name_ptr+2) bit 6 and 7 tells us, where the data is located:
        ;; 00 -> code
        ;; 01 -> EEPROM (not supported by MIOS_LCD_PrintCString yet)
        ;; 10 -> data
        ;; 11 -> unimplemented

	;; so: _sdcard_file_name_ptr+2.7 tells us, if filename located in RAM

	rgoto	SDCARD_FILE_Open


_SDCARD_FILE_Seek
	SET_BSR	_sdcard_file_seek_offset
        movwf   _sdcard_file_seek_offset+0, BANKED
        movff   FSR0L, FSR2L
        movff   PREINC2, _sdcard_file_seek_offset+1
        movff   PREINC2, _sdcard_file_seek_offset+2
        movff   PREINC2, _sdcard_file_seek_offset+3

	rgoto	SDCARD_FILE_Seek

; ==========================================================================

	END
