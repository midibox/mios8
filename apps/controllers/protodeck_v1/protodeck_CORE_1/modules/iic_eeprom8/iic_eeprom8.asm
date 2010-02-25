; $Id: iic_eeprom8.asm 415 2008-07-29 22:06:15Z tk $
;
;  Wrapper for iic_eeprom8.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	IIC_EEPROM8_Write
	global	IIC_EEPROM8_Read

	global	_IIC_EEPROM8_Write
	global	_IIC_EEPROM8_Read

; ==========================================================================
;  Declare variables 
; ==========================================================================

; ==========================================================================
IIC_EEPROM8 code
#include <iic_eeprom8.inc>

; ==========================================================================
	;; C function wrappers

_IIC_EEPROM8_Write
	movwf	MIOS_PARAMETER1
        movff   FSR0L, FSR2L
        movf    PREINC2, W
	rgoto	IIC_EEPROM8_Write
	
_IIC_EEPROM8_Read
	movwf	MIOS_PARAMETER1
	rgoto	IIC_EEPROM8_Read

; ==========================================================================
	END
