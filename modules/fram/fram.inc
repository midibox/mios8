; FRAM Module
;
; ==========================================================================
;
;  Copyright 2009 Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; If set to 1, mios_iic functions will be used (@400kHZ)
#ifndef FRAM_MIOS_IIC
#define FRAM_MIOS_IIC 0
#endif

; If set to 1, multiplexing will be enabled, bits 4 and 5 of the device address
; will be used to multiplex (bit 5 MSB, bit 4 LSB)
#ifndef FRAM_MULTIPLEX_ENABLE
#define FRAM_MULTIPLEX_ENABLE 0
#endif

#if FRAM_MULTIPLEX_ENABLE==1

; Multiplexer MSB Port/Pin
#ifndef FRAM_MULTIPLEX_PORT_MSB
#define FRAM_MULTIPLEX_PORT_MSB PORTC
#define FRAM_MULTIPLEX_TRIS_MSB TRISC
#define FRAM_MULTIPLEX_PIN_MSB 5
#endif

; Multiplexer LSB Port/Pin
#ifndef FRAM_MULTIPLEX_PORT_LSB
#define FRAM_MULTIPLEX_PORT_LSB PORTC
#define FRAM_MULTIPLEX_TRIS_LSB TRISC
#define FRAM_MULTIPLEX_PIN_LSB 4
#endif

#endif


;includes 1MHZ IIC driver if needed. maps functions either from mios_iic or fram_iic
#if FRAM_MIOS_IIC==0

#include "fram_iic.inc"

#define IIC_Start FRAM_IIC_Start
#define IIC_Stop FRAM_IIC_Stop
#define IIC_AckSend FRAM_IIC_AckSend
#define IIC_NakSend FRAM_IIC_NakSend
#define IIC_ByteReceive FRAM_IIC_ByteReceive
#define IIC_ByteSend FRAM_IIC_ByteSend

#else

#define IIC_Start MIOS_IIC_Start
#define IIC_Stop MIOS_IIC_Stop
#define IIC_AckSend MIOS_IIC_AckSend
#define IIC_NakSend MIOS_IIC_NakSend
#define IIC_ByteReceive MIOS_IIC_ByteReceive
#define IIC_ByteSend MIOS_IIC_ByteSend

#endif

;sends byte and returns error status (WREG = 0, ERR_CODE) on NAK received
IIC_ByteSend_ReturnOnFail MACRO ERR_CODE
	call IIC_ByteSend; sets WREG to 0x00 on NAK received, otherwise not zero
	; set and return error-code on NAK
	bnz $+0xA
	movlw ERR_CODE
	movff WREG,FRAM_ERROR
	retlw 0x00
	ENDM


#define FRAM_SlaveID_Mask 0xA0 ;high nibble is the slave ID (b1010)

;; --------------------------------------------------------------------------
;; -- These are the high-level functions. Use them if you know the amount
;; -- of data you want to read/write in advance. Using these functions, you
;; -- don't have to take care about shared pin conflicts. Error status will
;; -- be available in FRAM_ERROR after each function call (0 for no error).
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadByte
;;  DESCRIPTION: Reads one byte from address@device_address.
;;
;;  IN:  device select address in WREG (<5:4>multiplex <3:1>chip select)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;  OUT: Byte value in WREG if no error. On error, FRAM_ERROR != 0
;;
;;  USES: FRAM_REG, FRAM_ERROR, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_ReadByte
	bsf WREG,0 ;set read mode in device address
	rcall FRAM_Begin
	bz FRAM_End_Stop ;FRAM_Begin failed, stop and return
	call IIC_ByteReceive
	call IIC_NakSend
	bra FRAM_End_Stop
	
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteByte
;;  DESCRIPTION: Writes one byte to address@device_address.
;;
;;  IN:  device select address in WREG (<5:4>multiplex <3:1>chip select)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;			Byte value in MIOS_PARAMETER3
;;
;;  OUT: WREG == 0x00 on error, else not 0x00. Error code in FRAM_ERROR
;;
;;  USES: FRAM_REG, FRAM_ERROR, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_WriteByte
	bcf WREG,0 ;set write mode in device address
	rcall FRAM_Begin
	bz FRAM_End_Stop ;FRAM_Begin failed, stop and return
	movf MIOS_PARAMETER3,W ;value to WREG
	rcall FRAM_WriteByte_Cont
	bra FRAM_End_Stop
	
	
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadBuf
;;  DESCRIPTION: Reads count bytes from address@device_address to a buffer.
;;
;;  IN:  device select address in WREG (<5:4>multiplex <3:1>chip select)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;			Byte count in MIOS_PARAMETER3 (0x00 for 256 bytes)
;;
;;			Buffer start address in FSR1
;;
;;  OUT: WREG == 0x00 on error, else not 0x00. Error code in FRAM_ERROR
;;
;;  USES: FRAM_REG, FRAM_ERROR, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_ReadBuf
	bsf WREG,0 ;set read mode in device address
	rcall FRAM_Begin
	bz FRAM_End_Stop ;FRAM_Begin failed, stop and return
	rcall FRAM_ReadBuf_Loop
	call IIC_NakSend
	bra FRAM_End_Stop
	
	
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteBuf
;;  DESCRIPTION: Writes count bytes from a buffer to address@device_address.
;;
;;  IN:  device select address in WREG (<5:4>multiplex <3:1>chip select)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;			Byte count in MIOS_PARAMETER3 (0x00 for 256 bytes)
;;
;;			Buffer start address in FSR1
;;
;;  OUT: WREG == 0x00 on error, else not 0x00. Error code in FRAM_ERROR
;;
;;  USES: FRAM_REG, FRAM_ERROR, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_WriteBuf
	bcf WREG,0 ;set write mode in device address
	rcall FRAM_Begin
	bz FRAM_End_Stop ;FRAM_Begin failed, stop and return
	rcall FRAM_WriteBuf_Loop ;will set error status by itself
	bra FRAM_End_Stop
	
	
	

;; --------------------------------------------------------------------------
;; -- These are the low-level functions. Use them if you don't know the
;; -- amount of data you want to read or write in advance (e.g. the ammount
;; -- of data needed depends on the data you will read).
;; -- Note that you have to take care not to call any other functions that
;; -- use the same pins as the FRAM-devices between FRAM_Begin and FRAM_End. 
;; --Refer README.txt for more information.
;; -- Always start a transaction with FRAM_Start and end it with FRAM_Stop. 
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_Begin 
;;  DESCRIPTION: Begins a read or write transaction, selects chip and memory
;;  start address. After this call, subsequent reads/writes according to the
;;  selected mode (r/w) can be performed.
;;  If you share the IIC port (FRAM_MIOS_IIC==1), you have to call FRAM_End
;;  before you call some other IIC related function! (see also README.txt)
;;
;;  IN:  chip select address and r/w mode in WREG 
;;       (<5:4>multiplex <3:1>chip select <0:0>mode, 1 for read, 0 for write)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;  OUT: WREG  0x00 on error (NAK received), else not 0x00. 
;;			Error code in FRAM_ERROR
;;
;;  USES: FRAM_REG, FRAM_ERROR, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_Begin
	;clear error status register
	SET_BSR FRAM_ERROR
	clrf FRAM_ERROR,BANKED
	;set FRAM_REG
	SET_BSR FRAM_REG
	movwf FRAM_REG,BANKED ;move addr/mode to FRAM_REG
	bcf FRAM_REG,7,BANKED ;clear first-byte-read flag
	bsf FRAM_REG,6,BANKED ;set session flag
	; set multiplexer address if enabled
	#if FRAM_MULTIPLEX_ENABLE!=0
	bcf FRAM_MULTIPLEX_TRIS_MSB,FRAM_MULTIPLEX_PIN_MSB; Multiplex MSB -> output
	bcf FRAM_MULTIPLEX_TRIS_LSB,FRAM_MULTIPLEX_PIN_LSB; Multiplex LSB -> output
	bcf FRAM_MULTIPLEX_PORT_MSB,FRAM_MULTIPLEX_PIN_MSB; Multiplex MSB -> 0
	bcf FRAM_MULTIPLEX_PORT_LSB,FRAM_MULTIPLEX_PIN_LSB; Multiplex LSB -> 0
	btfsc WREG,5; Multiplex MSB -> 1 if set
	bsf FRAM_MULTIPLEX_PORT_MSB,FRAM_MULTIPLEX_PIN_MSB
	btfsc WREG,4; If LSB set, Multiplex LSB -> 1  if set
	bsf FRAM_MULTIPLEX_PORT_LSB,FRAM_MULTIPLEX_PIN_LSB	
	#endif
	; set WREG high nibble to slave_id, mode to write (always required to set address latch)
	andlw 0x0E ; clear high nibble and LSB (write mode)
	iorlw FRAM_SlaveID_Mask ; set high nibble to slave id
	; start condition and write slave address
	call IIC_Start
	IIC_ByteSend_ReturnOnFail 0x01
	; send memory address MSB and LSB
	movf MIOS_PARAMETER1,W ;MSB
	IIC_ByteSend_ReturnOnFail 0x02
	movf MIOS_PARAMETER2,W ;LSB
	IIC_ByteSend_ReturnOnFail 0x04
	; if read operation is indicated, continue, else return
	SET_BSR FRAM_REG
	btfss FRAM_REG,0,BANKED
	return
	; address latch is set, indicated read operation now
	movf FRAM_REG,W,BANKED
	andlw 0x0F ; clear device address high nibble
	iorlw FRAM_SlaveID_Mask ; set slave id to high nibble of device address
	call IIC_Start ;start condition
	IIC_ByteSend_ReturnOnFail 0x08 ;device address for read operation
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_End 
;;  DESCRIPTION: Ends a read or write transaction. Releases the IIC port (stop).
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: FRAM_REG
;; --------------------------------------------------------------------------
_FRAM_End
FRAM_End
	SET_BSR FRAM_REG
	;if continous write operation, or no byte was read since begin, just send IIC_Stop
	btfsc FRAM_REG,0,BANKED
	btfss FRAM_REG,7,BANKED
	bra FRAM_End_Stop
	;to stop continous read operation, a NAK has to be sent before stop
	call IIC_NakSend
FRAM_End_Stop
	call IIC_Stop
	clrf FRAM_REG,BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadByte
;;  DESCRIPTION: Reads a byte from FRAM. The internal address latch of the FRAM
;;  will be increased by one.
;; 
;;  IN:   -
;;  OUT:  read byte in WREG
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_ReadByte_Cont
FRAM_ReadByte_Cont
	SET_BSR FRAM_REG
	;if this is the first byte read since begin, skip ACK send
	btfsc FRAM_REG,7,BANKED
	call IIC_AckSend
	bsf FRAM_REG,7,BANKED ; set first byte read flag
	call IIC_ByteReceive
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteByte
;;  DESCRIPTION: Writes one byte to the FRAM. The internal address latch of the
;;  FRAM will be increased by one.
;; 
;;  IN:  Byte to write in WREG
;;  OUT: WREG 0x00 on error (NAK received), else not 0x00
;;			Error code in FRAM_ERROR
;;  USES: FRAM_ERROR,FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_WriteByte_Cont
FRAM_WriteByte_Cont
	IIC_ByteSend_ReturnOnFail 0x10
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadBuf
;;  DESCRIPTION: Reads count bytes from FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  Byte count in WREG (0x00 for 256 bytes), pointer to buffer in FSR1
;;  OUT:  -
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
FRAM_ReadBuf_Cont
	SET_BSR FRAM_REG
	;if this is the first byte read since begin, skip ACK send
	btfsc FRAM_REG,7,BANKED
	call IIC_AckSend
	bsf FRAM_REG,7,BANKED ; set first byte read flag
	movwf MIOS_PARAMETER3; byte count to MIOS_PARAMETER3
FRAM_ReadBuf_Loop
	call IIC_ByteReceive
	movwf	POSTINC1 ; move byte to buffer
	dcfsnz MIOS_PARAMETER3,F ; decrement MIOS_PARAMETER3, if zero return, else send ACK and read next byte
	return 
	call IIC_AckSend
	bra FRAM_ReadBuf_Loop
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteBuf
;;  DESCRIPTION: Writes count bytes to FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  Byte count in WREG (0x00 for 256 bytes), pointer to buffer in FSR1
;;  OUT: 0x00 on error (NAK received), else not 0x00
;;			Error code in FRAM_ERROR
;;  USES: FRAM_ERROR,FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
FRAM_WriteBuf_Cont
	movwf MIOS_PARAMETER3; byte count to MIOS_PARAMETER3
FRAM_WriteBuf_Loop
	movf POSTINC1,W; move byte from buffer to WREG
	IIC_ByteSend_ReturnOnFail 0x10
	decfsz MIOS_PARAMETER3,F ; decrement MIOS_PARAMETER3, write next byte if not zero
	bra FRAM_WriteBuf_Loop
	return
	
	