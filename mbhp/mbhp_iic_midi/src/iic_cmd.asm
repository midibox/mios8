; $Id$
;
;  IIC Command Handler
;
;  Called from IIC_Handler if command mode has been selected with the 0xff token
;
;  Following sequences will *not* call the command handler:
;     - 0xff 0x00 (Exit Command Mode)
;     - 0xff 0xff (Send 0xff via MIDI and exit Command Mode)
;
;  All other sequences like:
;    0xff <cmd> <data1> <data2> <data3> ... <data_n>
;  Will call the command handler whenever a new data byte has been received
;  (command in IIC_CMD, received data byte in WREG)
;  
;  Command mode will be exit once the command handler returns 0x00
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

	;; export lables
	global	IIC_CMD_Handler

	;; import lables
	extern	IIC_CMD
	extern	MIDI_TxBufferPut

; ==========================================================================

gpr0			udata
IIC_CMD_DATA		res	1

; ==========================================================================
IIC_CMD	code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_CMD_Handler
;;  DESCRIPTION: see header of this file
;;  IN:	  command in IIC_CMD, data in WREG
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
IIC_CMD_Handler
 	movwf	IIC_CMD_DATA			; store received data

#if 0
	;; for testing only!
	movlw	0xb0
	call	MIDI_TxBufferPut	; put into MIDI Tx buffer
	movf	IIC_CMD, W
	call	MIDI_TxBufferPut	; put into MIDI Tx buffer
	movf	IIC_CMD_DATA, W
	call	MIDI_TxBufferPut	; put into MIDI Tx buffer
#endif

	;; nothing to do - return 0
	retlw	0x00

; ==========================================================================
	END
