; Wrapper for fram.inc
; To be used for relocatable programs
;
; ==========================================================================

	radix	dec

#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; If set to 1, mios_iic functions will be used (@400kHZ)
#ifndef FRAM_MIOS_IIC
#define FRAM_MIOS_IIC 0
#endif

; ==========================================================================
; Import/Export Labels
; ==========================================================================
	
	; export label for ASM applications
	global FRAM_Begin
	global FRAM_End
	global FRAM_ReadByte
	global FRAM_WriteByte
	global FRAM_ReadBuf
	global FRAM_WriteBuf
	
	;FRAM_REG is set by FRAM_Begin:
	;<7:7>first byte read flag, zero before first byte is read
	;<6:6>Session Flag, set to 1 in FRAM_Begin -> FRAM_REG != 0 during a session.
	;<5:4>multiplex addr, 5 MSB, 4 LSB
	;<3:1>chip select (for FM24C512 LSB selects one of two memory blocks)
	;<0:0>mode, 1 for read, 0 for write
	;FRAM_End sets FRAM_REG to zero. You can use it to check if there's a 
	;pending session, in this case you have to init FRAM_REG by yourself.
	;Never change it during a session (FRAM_Begin -> FRAM_End)!!
	global FRAM_REG
	

	; (for C, declaration in fram.h)
	global _FRAM_Begin
	global _FRAM_End
	global _FRAM_ReadByte
	global _FRAM_WriteByte
	global _FRAM_ReadBuf
	global _FRAM_WriteBuf
	
	global _FRAM_REG

; ==========================================================================
;  Declare variables 
; ==========================================================================

; this buffer is only needed when fram_iic.inc is used instead of mios_iic
; (FRAM_MIOS_IIC = 0)
#if FRAM_MIOS_IIC==0
FRAM_IIC_VARS UDATA
FRAM_IIC_BUF RES 1
#endif

FRAM_VARS UDATA
_FRAM_REG
FRAM_REG RES 1


FRAM CODE

#include "fram.inc"

; ==========================C function wrappers===============================

_FRAM_Begin
	movff FSR0L, FSR2L
	movff PREINC2,MIOS_PARAMETER2 ;MOVE address LSB
	movff PREINC2,MIOS_PARAMETER1 ;MOVE address MSB
	rlncf WREG,W ; shift left device-address
	iorwf PREINC2,W ; set r/w bit
	bra FRAM_Begin

_FRAM_ReadBuf
	movff FSR0L, FSR2L
	movff PREINC2, FSR1L ; move least significant byte of buffer pointer to FSR1L
	movff PREINC2, FSR1H	; move least most byte of buffer pointer to FSR1H
	bra FRAM_ReadBuf_Cont

_FRAM_WriteBuf
	movff FSR0L, FSR2L
	movff PREINC2, FSR1L ; move least significant byte of buffer pointer to FSR1L
	movff PREINC2, FSR1H	; move least most byte of buffer pointer to FSR1H
	bra FRAM_WriteBuf_Cont
	
; ==========================================================================
	END
