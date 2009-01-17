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

;sends byte and returns on NAK received
IIC_ByteSend_ReturnOnFail MACRO
	call IIC_ByteSend; sets WREG to 0x00 on NAK received, otherwise 0xFF
	btfss WREG,0; return on NAK
	return
	ENDM


#define slave_id_mask 0xA0 ;high nibble is the slave ID (b1010)

;; --------------------------------------------------------------------------
;; -- Always start a transaction with FRAM_Start and end it with FRAM_Stop. 
;; -- If you share a port with other IIC - devices (FRAM_MIOS_IIC==1), be sure
;; -- to always call FRAM_Stop before using other IIC related functions!
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_Begin 
;;  DESCRIPTION: Begins a read or write transaction, selects chip and memory
;;  start address. After this call, subsequent reads/writes according to the
;;  selected mode (r/w) can be performed.
;;  If you share the IIC port (FRAM_MIOS_IIC==1), you have to call FRAM_End
;;  before you call some other IIC related function!
;;
;;  IN:  chip select address and r/w mode in WREG 
;;       (<5:4>multiplex <3:1>chip select <0:0>mode, 1 for read, 0 for write)
;;       Chip select LSB selects one of two sectors for FM24C512, so you can
;;       look at a FM24C512 like it were two FM24C256 (see also readme).
;;
;;       memory address in MIOS_PARAMETER1 (MSB) and MIOS_PARAMETER2 (LSB)
;;       0x0000 - 0x7FFF or 0x0000 - 0x1FFF, see README
;;
;;  OUT: WREG  0x00 on error (NAK received), else 0xFF
;;
;;  USES: FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0) 
;; --------------------------------------------------------------------------
FRAM_Begin
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
	iorlw slave_id_mask ; set high nibble to slave id
	; start condition and write slave address
	call IIC_Start
	IIC_ByteSend_ReturnOnFail
	; send memory address MSB and LSB
	movf MIOS_PARAMETER1,W ;MSB
	IIC_ByteSend_ReturnOnFail
	movf MIOS_PARAMETER2,W ;LSB
	IIC_ByteSend_ReturnOnFail
	; if read operation is indicated, continue, else return
	SET_BSR FRAM_REG
	btfss FRAM_REG,0,BANKED
	return
	; address latch is set, indicated read operation now
	movf FRAM_REG,W,BANKED
	andlw 0x0F ; clear device address high nibble
	iorlw slave_id_mask ; set slave id to high nibble of device address
	call IIC_Start ;start condition
	call IIC_ByteSend ;device address for read operation
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
_FRAM_ReadByte
FRAM_ReadByte
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
;;  OUT:  WREG 0x00 on error, else 0xFF
;;  USES:  FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
_FRAM_WriteByte
FRAM_WriteByte
	call IIC_ByteSend
	return 
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_ReadBuf 
;;  DESCRIPTION: Reads size bytes from FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  buffer size in WREG (0x00 for 256 bytes), pointer to buffer in FSR1
;;  OUT:  -
;;  USES: PRODL,FRAM_REG, FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
FRAM_ReadBuf
	SET_BSR FRAM_REG
	;if this is the first byte read since begin, skip ACK send
	btfsc FRAM_REG,7,BANKED
	call IIC_AckSend
	bsf FRAM_REG,7,BANKED ; set first byte read flag
	movwf PRODL; byte count to PRODL
FRAM_ReadBuf_Loop
	call IIC_ByteReceive
	movwf	POSTINC1 ; move byte to buffer
	dcfsnz PRODL,F ; decrement PRODL, if zero return, else send ACK and read next byte
	return 
	call IIC_AckSend
	bra FRAM_ReadBuf_Loop
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_WriteBuf 
;;  DESCRIPTION: Writes size bytes to FRAM. The internal address latch of 
;;  the FRAM will be increased by size.
;; 
;;  IN:  buffer size in WREG (0x00 for 256 bytes), pointer to buffer in FSR1
;;  OUT:  0x00 on error, else 0xff
;;  USES: PRODL,FRAM_IIC_BUF (only if FRAM_MIOS_IIC==0)
;; --------------------------------------------------------------------------
FRAM_WriteBuf
	movwf PRODL; byte count to PRODL
FRAM_WriteBuf_Loop
	movf POSTINC1,W; move byte from buffer to WREG
	IIC_ByteSend_ReturnOnFail
	decfsz PRODL,F ; decrement PRODL, write next byte if not zero
	bra FRAM_WriteBuf_Loop	
	return
	
	