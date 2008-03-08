; $Id$
;
;  IIC (Slave) Driver
;
; ==========================================================================
;  device address of IIC slave (the ID selected at RB[6:7] is ored to [2:1] of the address
;  Note: MUST be an even number, because bit #0 indicates a read/write operation
; ==========================================================================
IIC_DEVICE_ADDRESS	EQU	0x20
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=16f88
	radix	dec
#include "p16f88.inc"
#include "macros.inc"
#include "globals.inc"

; ==========================================================================

	;; export lables
	global	IIC_Init
	global	IIC_Init_GetAddress
	global	IIC_Handler

	extern	UART_TxBufferFree
	extern	UART_TxBufferPut

; ==========================================================================

gpr0			udata
IIC_TMP			res	1
IIC_DATA		res	1
IIC_READ_CTR		res	1

IIC_CMP_CTR		res	1

; ==========================================================================
IIC	code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Init
;;  DESCRIPTION: initialises the IIC interface
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
IIC_Init
	SWITCHBANK_0_1
	bsf PCON, NOT_POR
	bsf PCON, NOT_BOR
	SWITCHBANK_1_0

	;; setup SSP module for 7-bit
	movlw	0x36
	movwf	SSPCON

	;; determine address (depending on IDx pins)
	call	IIC_Init_GetAddress

	SWITCHBANK_0_1
	clrf	SSPSTAT
	SWITCHBANK_1_0

	;; reset read counter
	clrf	IIC_READ_CTR
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Init_GetAddress
;;  DESCRIPTION: initialises the IIC address only - can be called from the
;;  mainloop to allow the change on-the-fly without device reset
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
IIC_Init_GetAddress
	movlw	IIC_DEVICE_ADDRESS

	btfsc	ID0_PORT, ID0_PIN
	iorlw	0x02
	btfsc	ID1_PORT, ID1_PIN
	iorlw	0x04

	SWITCHBANK_0_1
	movwf	SSPADD
	SWITCHBANK_1_0
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Handler
;;  DESCRIPTION: handles IIC transactions, should be called from the mainloop
;;               on IIC events.
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
IIC_Handler
	;; get value of SSPSTAT and store it in IIC_TMP
	SWITCHBANK_0_1
	movf	SSPSTAT, W
	SWITCHBANK_1_0
	movwf	IIC_TMP

	;; note: in distance to AN734 we don't check the start bit here,
	;; because there are cases where the last byte has been received
	;; and the stop bit is already set

	;; branch depending on BF flag (BF=1 -> write, otherwise read operation)
	btfss	IIC_TMP, BF
	goto	IIC_Handler_Read

IIC_Handler_Write

	;; check for SSPOV flag - if overrun happens, wait for stop flag, thereafter clear SSPOV flag
	btfss	SSPCON, SSPOV
	goto	IIC_Handler_Write_NoOV
	SWITCHBANK_0_1
IIC_Handler_Write_OVLoop
	IRQ_ENABLE			; give interrupts a chance to get serviced
	IRQ_DISABLE
	btfss	SSPSTAT, P
	goto	IIC_Handler_Write_OVLoop
	SWITCHBANK_1_0
	bcf	SSPCON, SSPOV		; first clear SSPOV, then BF bit
	movf	SSPBUF, W
	goto	IIC_Handler_End

IIC_Handler_Write_NoOV

	movf	SSPBUF, W		; Get the byte from the SSP (very early!)
	movwf	IIC_DATA		; and save it in IIC_DATA

	;; branch depending on D_A flag (D_A=0 -> address, otherwise data byte)
	btfsc	IIC_TMP, D_A
	goto	IIC_Handler_WriteB

IIC_Handler_WriteA	; (write phase, address)
	;; nothing more to do
	goto	IIC_Handler_Write_End

IIC_Handler_WriteB	; (write phase, data)
	;; set overrun flag if Tx buffer cannot get more than 3 bytes
   	call	UART_TxBufferFree
	addlw	-3
	skpc
  	bsf	SSPCON, SSPOV

	;; set overrun flag if speakjet buffer is "half full"
	btfsc	SPEAKJET_BHFULL_PORT, SPEAKJET_BHFULL_PIN
	bsf	SSPCON, SSPOV

	;; put byte into UART Tx buffer
	movf	IIC_DATA, W
	call	UART_TxBufferPut

IIC_Handler_WriteB_End
	;; check for overrun - if this happens, wait for stop bit and clear the SSPOV flag
	btfss	SSPCON, SSPOV
	goto	IIC_Handler_Write_End	; exit handler

	SWITCHBANK_0_1
IIC_Handler_WriteB_OVLoop
	IRQ_ENABLE			; give interrupts a chance to get serviced
	IRQ_DISABLE
	btfss	SSPSTAT, P
	goto	IIC_Handler_WriteB_OVLoop
	SWITCHBANK_1_0

	;; first clear SSPOV, then BF bit
	bcf	SSPCON, SSPOV
	movf	SSPBUF, W
	goto	IIC_Handler_End

IIC_Handler_Write_End
	;; fast path if SSPIF and BF flag set
	btfss	PIR1, SSPIF
	goto	IIC_Handler_End	; exit handler
	SWITCHBANK_0_1
	movf	SSPSTAT, W
	SWITCHBANK_1_0
	movwf	IIC_TMP
	btfss	IIC_TMP, BF
	goto	IIC_Handler_End	; exit handler

	bcf	PIR1, SSPIF	; clear SSP flag
	IRQ_DISABLE		; disable interrupts (for faster execution)
	goto	IIC_Handler_Write
	


IIC_Handler_Read
	IRQ_ENABLE

	;; branch depending on D_A flag (D_A=0 -> address, otherwise data byte)
	btfsc	IIC_TMP, D_A
	goto	IIC_Handler_ReadB

IIC_Handler_ReadA	; (read phase, address)
	;; clear read counter and always return 0 (IIC read not supported)
	clrf	IIC_READ_CTR
	movlw	0x00		; always 0
	call	IIC_Write
	goto	IIC_Handler_Read_End	; exit read handler

IIC_Handler_ReadB	; (write phase, data)
	;; branch depending on R_W flag (R_W = 1 -> data byte should be read, otherwise NAK has been received)
	btfss	IIC_TMP, R_W
	goto	IIC_Handler_ReadNAK

IIC_Handler_ReadACK
	bsf	RI_N_PORT, RI_N_PIN	; set RI_N=1 (receive interrupt) pin - it's low active!
					; the pin was set to 0 in prot.asm

	;; increment read counter and return 0 (IIC read not supported)
	incf	IIC_READ_CTR, F
	movlw	0x00		; always 0
	call	IIC_Write

IIC_Handler_ReadACK_NoClr
	goto	IIC_Handler_Read_End	; exit read handler

IIC_Handler_ReadNAK
	bsf	SSPCON, CKP		; release clock
	;; 	goto	IIC_Handler_Read_End	; exit read handler

IIC_Handler_Read_End

IIC_Handler_End
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Write
;;  DESCRIPTION: sends a byte over IIC bus
;;  IN:	  byte to send in WREG
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
IIC_Write
	SWITCHBANK_0_1
IIC_Write_Wait
	btfsc	SSPSTAT, BF		; wait until buffer empty
	goto	IIC_Write_Wait
	SWITCHBANK_1_0
IIC_Write_Now
	bcf	SSPCON, WCOL		; Clear the WCOL flag.
	movwf	SSPBUF			; Write the byte in WREG
	btfss	SSPCON, WCOL		; write again on collision
	goto	IIC_Write_Now
	bsf	SSPCON, CKP		; release clock
	return

; ==========================================================================
	END
