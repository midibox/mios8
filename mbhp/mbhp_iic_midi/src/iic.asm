; $Id$
;
;  IIC (Slave) Driver
;
; ==========================================================================
;  device address of IIC slave (the ID selected at RB[6:7] is ored to [2:1] of the address
;  Note: MUST be an even number, because bit #0 indicates a read/write operation
; ==========================================================================
IIC_DEVICE_ADDRESS	EQU	0x10
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
#include <p16f88.inc>
#include "macros.inc"
#include "globals.inc"

; ==========================================================================

#define IIC_CMD_STATE_NOT_ACTIVE	0
#define IIC_CMD_STATE_GET_CMD		1
#define IIC_CMD_STATE_STREAM		2

; ==========================================================================

	;; export lables
	global	IIC_Init
	global	IIC_Init_GetAddress
	global	IIC_Handler

	global	IIC_CMD

	;; import lables
	extern	IIC_CMD_Handler

	extern	MIDI_TxBufferFree
	extern	MIDI_TxBufferPut

	extern	PROT_RX_PACKAGE
	extern	PROT_TX_PACKAGE

; ==========================================================================

gpr0			udata
IIC_TMP			res	1
IIC_DATA		res	1
IIC_CMD_STATE		res	1
IIC_CMD			res	1
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

	;; reset IIC command state
	movlw	IIC_CMD_STATE_NOT_ACTIVE
	movwf	IIC_CMD_STATE

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
#if DBG_IICWR_PORT
	bsf	DBG_IICWR_PORT, DBG_IICWR_PIN
#endif

	;; branch depending on command state
	movf	IIC_CMD_STATE, W
	bz	IIC_Handler_WriteB_NoCmd	; IIC_CMD_STATE_NOT_ACTIVE
	addlw	-1
	bz	IIC_Handler_WriteB_CmdState1	; IIC_CMD_STATE_GET_CMD
	addlw	-1
	bz	IIC_Handler_WriteB_CmdState2	; IIC_CMD_STATE_STREAM
	goto	IIC_Handler_WriteB_CmdStateExit	; (illegal)

IIC_Handler_WriteB_CmdState1	; IIC_CMD_STATE_GET_CMD
	movlw	IIC_CMD_STATE_STREAM	; switch to next state
	movwf	IIC_CMD_STATE
	movf	IIC_DATA, W		; store received byte in IIC_CMD
	movwf	IIC_CMD			; store it in IIC_CMD

	;; if command = 0x00: exit command state
	bz	IIC_Handler_WriteB_CmdStateExit

	;; if command = 0xff: send 0xff and exit command state
	xorlw	0xff
	bnz	IIC_Handler_WriteB_End	; (otherwise stay in STREAM mode)
	movlw	0xff
	call	MIDI_TxBufferPut	; put into MIDI Tx buffer
	goto	IIC_Handler_WriteB_CmdStateExit

IIC_Handler_WriteB_CmdState2	; IIC_CMD_STATE_STREAM
	movf	IIC_DATA, W		; get received byte
	call	IIC_CMD_Handler		; and call command handler
	andlw	0xff			; check for 0x00 (-> exit command mode)
	bz	IIC_Handler_WriteB_CmdStateExit
	goto	IIC_Handler_WriteB_End

IIC_Handler_WriteB_CmdStateExit	; exits the command state
	movlw	IIC_CMD_STATE_NOT_ACTIVE
	movwf	IIC_CMD_STATE
	goto	IIC_Handler_WriteB_End

IIC_Handler_WriteB_NoCmd

	;; set overrun flag if Tx buffer cannot get more than 3 bytes
   	call	MIDI_TxBufferFree
	addlw	-3
	skpc
  	bsf	SSPCON, SSPOV

	;; go into command state if 0xff has been received
	movf	IIC_DATA, W
	xorlw	0xff
	bnz	IIC_Handler_WriteB_NewByte
IIC_Handler_WriteB_EnterCmd
	movlw	IIC_CMD_STATE_GET_CMD
	movwf	IIC_CMD_STATE
	goto	IIC_Handler_WriteB_End

IIC_Handler_WriteB_NewByte
	movf	IIC_DATA, W
	call	MIDI_TxBufferPut	; put into MIDI Tx buffer

IIC_Handler_WriteB_End
#if DBG_IICWR_PORT
	bcf	DBG_IICWR_PORT, DBG_IICWR_PIN
#endif

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
	;; clear read counter and return first byte of package
	clrf	IIC_READ_CTR
	movf	PROT_RX_PACKAGE+0, W
	call	IIC_Write
	goto	IIC_Handler_Read_End	; exit read handler

IIC_Handler_ReadB	; (write phase, data)
	;; branch depending on R_W flag (R_W = 1 -> data byte should be read, otherwise NAK has been received)
	btfss	IIC_TMP, R_W
	goto	IIC_Handler_ReadNAK

IIC_Handler_ReadACK
#if DBG_IICRD_PORT
	bsf	DBG_IICRD_PORT, DBG_IICRD_PIN
#endif

	bsf	RI_N_PORT, RI_N_PIN	; set RI_N=1 (receive interrupt) pin - it's low active!
					; the pin was set to 0 in prot.asm

	;; increment read counter and return value of package
	incf	IIC_READ_CTR, F
	movlw	PROT_RX_PACKAGE
	addwf	IIC_READ_CTR, W
	movwf	FSR
	movf	INDF, W
	call	IIC_Write

	;; clear package once we reached CTR >= 3
	incf	IIC_READ_CTR, W
	andlw	0xfc
	bz	IIC_Handler_ReadACK_NoClr
IIC_Handler_ReadACK_Clr
	clrf	PROT_RX_PACKAGE+0
	clrf	PROT_RX_PACKAGE+1
	clrf	PROT_RX_PACKAGE+2
	clrf	PROT_RX_PACKAGE+3

IIC_Handler_ReadACK_NoClr
#if DBG_IICRD_PORT
	bcf	DBG_IICRD_PORT, DBG_IICRD_PIN
#endif
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
