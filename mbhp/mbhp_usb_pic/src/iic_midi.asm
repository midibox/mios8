; $Id$
;
;  MIDI Driver for MBHP_IIC_MIDI module
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=18f4550
	radix	dec
#include <p18f4550.inc>
#include "macros.inc"

; ==========================================================================

	;; export lables
	global	IIC_MIDI_Init
	global	IIC_MIDI_Receive
	global	IIC_MIDI_Transmit
	global	IIC_MIDI_TransmitStart
	global	IIC_MIDI_TransmitByte
	global	IIC_MIDI_TransmitStop

	global	IIC_MIDI_AVAILABLE_SLAVES
	global	IIC_MIDI_SLAVE

	global	_iic_midi_rx_package
	global	_iic_midi_tx_package

	;; import
	extern	IIC_Stop
	extern	IIC_Start
	extern	IIC_NakSend
	extern	IIC_AckSend
	extern	IIC_ByteSend
	extern	IIC_ByteReceive

	extern	IIC_STAT

; ==========================================================================

;; base address and max number of IIC slaves
#define IIC_MIDI_BASE_ADDR	0x10

;; should be 0 if no IIC_MIDI device connected
;; can be pre-defined in MAKEFILE.SPEC
;; Changes here have no effect (since IIC_MIDI_MAX_NUMBER is always defined!)
#ifndef IIC_MIDI_MAX_NUMBER
#define IIC_MIDI_MAX_NUMBER	0
#endif

;; RI_N lines (note: ADC will be turned off in IIC_MIDI_Init)
PORT_RI_N_SLAVE0	EQU     PORTA
TRIS_RI_N_SLAVE0	EQU	TRISA
PIN_RI_N_SLAVE0		EQU     0

PORT_RI_N_SLAVE1	EQU     PORTA
TRIS_RI_N_SLAVE1	EQU	TRISA
PIN_RI_N_SLAVE1		EQU     1

PORT_RI_N_SLAVE2	EQU     PORTA
TRIS_RI_N_SLAVE2	EQU	TRISA
PIN_RI_N_SLAVE2		EQU     2

PORT_RI_N_SLAVE3	EQU     PORTA
TRIS_RI_N_SLAVE3	EQU	TRISA
PIN_RI_N_SLAVE3		EQU     3

; ==========================================================================

accessram		udata_acs
IIC_MIDI_AVAILABLE_SLAVES res	1
IIC_MIDI_SLAVE		res	1

IIC_MIDI_TMP		res	1
TX_RETRY_CTR		res	1
BYTES_TO_SEND		res	1

_iic_midi_rx_package	res	4
_iic_midi_tx_package	res	4

; ==========================================================================
IIC_MIDI code
; ==========================================================================

;; this table contains the number of bytes which have to be sent within a MIDI packet
BYTES_TO_SEND_TABLE
	;;	taken from Table 4-1 of USB MIDI spec v1.0:
	;;      +------------------------------- 0: invalid/reserved event
	;;      | +----------------------------- 1: invalid/reserved event
	;;      | | +--------------------------- 2: two-byte system common messages like MTC, Song Select, etc.
	;;      | | | +------------------------- 3: three-byte system common messages like SPP, etc.
	;;      | | | | +----------------------- 4: SysEx starts or continues
	;;      | | | | | +--------------------- 5: SysEx ends with following single byte
	;;      | | | | | | +------------------- 6: SysEx ends with following two bytes
	;;      | | | | | | | +----------------- 7: SysEx ends with following three bytes
	;;      | | | | | | | | +--------------- 8: Note Off
	;;      | | | | | | | | | +------------- 9: Note On
	;;      | | | | | | | | | | +----------- a: Poly-Key Press
	;;      | | | | | | | | | | | +--------- b: Control Change
	;;      | | | | | | | | | | | | +------- c: Program Change
	;;      | | | | | | | | | | | | | +----- d: Channel Pressure
	;;      | | | | | | | | | | | | | | +--- e: PitchBend Change
	;;      | | | | | | | | | | | | | | | +- f: single byte like MIDI Clock/Start/Stop/Continue
	;;      | | | | | | | | | | | | | | | |
	db	0,0,2,3,3,1,2,3,3,3,3,3,2,2,3,1

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_Init
;;  DESCRIPTION: should be called after core reset to determine, which
;;  IIC MIDI slaves are connected to the core
;;  IN:	  -
;;  OUT:  detected slaves in IIC_MIDI_AVAILABLE_SLAVES
;;  USES: -
;; --------------------------------------------------------------------------
IIC_MIDI_Init
#if IIC_MIDI_MAX_NUMBER == 0
	clrf	IIC_MIDI_AVAILABLE_SLAVES
	return
#endif

	call	IIC_Stop

	;; disable the ADC which allocates the analog pins
	;; these pins are used to poll the RI_N lines
#ifdef __18F4550
	movlw	0x0f		; PIC18F4550
#else
	movlw	0x07		; PIC18F452
#endif
	movwf	ADCON1

	;; enable inputs
	bsf	TRIS_RI_N_SLAVE0, PIN_RI_N_SLAVE0
	bsf	TRIS_RI_N_SLAVE1, PIN_RI_N_SLAVE1
	bsf	TRIS_RI_N_SLAVE2, PIN_RI_N_SLAVE2
	bsf	TRIS_RI_N_SLAVE3, PIN_RI_N_SLAVE3

	;; detect all available slaves	
	clrf	IIC_MIDI_AVAILABLE_SLAVES
	clrf	IIC_MIDI_SLAVE
IIC_MIDI_Init_DetectLoop
	;; try to detect the slave three times
	call	IIC_MIDI_Receive_Internal
	btfsc	IIC_STAT, 0
	rgoto	IIC_MIDI_Init_DetectLoop_Avail

	call	IIC_MIDI_Receive_Internal
	btfsc	IIC_STAT, 0
	rgoto	IIC_MIDI_Init_DetectLoop_Avail

	call	IIC_MIDI_Receive_Internal
	btfsc	IIC_STAT, 0
	rgoto	IIC_MIDI_Init_DetectLoop_Avail

	rgoto	IIC_MIDI_Init_DetectLoop_Next

IIC_MIDI_Init_DetectLoop_Avail
	;; set appr. flag
	movf	IIC_MIDI_SLAVE, W
	call	IIC_MIDI_GetSlaveMask
	iorwf	IIC_MIDI_AVAILABLE_SLAVES, F

IIC_MIDI_Init_DetectLoop_Next
	incf	IIC_MIDI_SLAVE, F
	movlw	IIC_MIDI_MAX_NUMBER-1
	cpfsgt	IIC_MIDI_SLAVE, ACCESS
	rgoto	IIC_MIDI_Init_DetectLoop

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_Receive
;;  DESCRIPTION: receives a package from a MBHP_IIC_MIDI slave
;;  IN:	  MIDI slave number (0-3) in IIC_MIDI_SLAVE
;;  OUT:  4 bytes in _iic_midi_rx_package,
;;        WREG contains _iic_midi_rx_package+0
;;        if WREG/_iic_midi_rx_package+0 == 0, no data has been received
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
IIC_MIDI_Receive
	;; clear package content
	clrf	_iic_midi_rx_package+0
	clrf	_iic_midi_rx_package+1
	clrf	_iic_midi_rx_package+2
	clrf	_iic_midi_rx_package+3

	;; skip if slave not available
	call	IIC_MIDI_GetSlaveMask
	andwf	IIC_MIDI_AVAILABLE_SLAVES, W
	bz	IIC_MIDI_Receive_Disabled

	;; check RI line
	call	IIC_MIDI_GetRI
	bz	IIC_MIDI_Receive_NoRI

IIC_MIDI_Receive_Internal
	call	IIC_Start		; start IIC
	rlf	IIC_MIDI_SLAVE, W	; send address | 1 (read transfer)
	andlw	0x0e
	addlw	IIC_MIDI_BASE_ADDR | 1
	call	IIC_ByteSend

	btfss	IIC_STAT, 0		; skip if IIC slave not available
	rgoto	IIC_MIDI_Receive_NotAvail

	call	IIC_ByteReceive		; read byte #0
	movwf	_iic_midi_rx_package+0
	bz	IIC_MIDI_Receive_BufferEmpty	; -> got 0: package is empty
	call	IIC_AckSend		; send acknowledge

	call	IIC_ByteReceive		; read byte #1
	movwf	_iic_midi_rx_package+1
	call	IIC_AckSend		; send acknowledge

	call	IIC_ByteReceive		; read byte #2
	movwf	_iic_midi_rx_package+2
	call	IIC_AckSend		; send acknowledge

	call	IIC_ByteReceive		; read byte #3
	movwf	_iic_midi_rx_package+3

IIC_MIDI_Receive_BufferEmpty
IIC_MIDI_Receive_NotAvail
 	call	IIC_NakSend		; send Nak to abort transfer
	call	IIC_Stop		; stop IIC
IIC_MIDI_Receive_Disabled
IIC_MIDI_Receive_NoRI
	movf	_iic_midi_rx_package+0, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_Transmit
;;  DESCRIPTION: sends a package to a IIC slave
;;  IN:	  MIDI slave number (0-3) in IIC_MIDI_SLAVE
;;        package in _iic_midi_tx_package
;;  OUT:  returns 0 if invalid package or slave not available
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
IIC_MIDI_Transmit
	TABLE_ADDR BYTES_TO_SEND_TABLE ; get number of bytes -> TABLAT
	movf	_iic_midi_tx_package+0, W
	andlw	0x0f
	TABLE_ADD_W
	tblrd*

	movf	TABLAT, W		; skip checks below if no byte to send
	bz	IIC_MIDI_Transmit_End
	movwf	BYTES_TO_SEND

	clrf	TX_RETRY_CTR		; retry 256 times
	;; check if buffer free (returns 0 if not -> retry!), and start the transfer
IIC_MIDI_Transmit_Retry
	call	IIC_MIDI_TransmitStart
	bnz	IIC_MIDI_Transmit_StartOk
	decfsz	TX_RETRY_CTR, F
	rgoto	IIC_MIDI_Transmit_Retry
	andlw	0x00			; abort transfer after 256 retries, set ZERO flag
	rgoto	IIC_MIDI_Transmit_End
IIC_MIDI_Transmit_StartOk

	movlw	0			; send first byte
	cpfsgt	BYTES_TO_SEND, ACCESS
 	rgoto	IIC_MIDI_Transmit_LT1
	movf	_iic_midi_tx_package+1, W
	call	IIC_MIDI_TransmitByte
IIC_MIDI_Transmit_LT1

	movlw	1			; send second byte
	cpfsgt	BYTES_TO_SEND, ACCESS
 	rgoto	IIC_MIDI_Transmit_LT2
	movf	_iic_midi_tx_package+2, W
	call	IIC_MIDI_TransmitByte
IIC_MIDI_Transmit_LT2

	movlw	2			; send third byte
	cpfsgt	BYTES_TO_SEND, ACCESS
	rgoto	IIC_MIDI_Transmit_LT3
	movf	_iic_midi_tx_package+3, W
	call	IIC_MIDI_TransmitByte
IIC_MIDI_Transmit_LT3

	call	IIC_MIDI_TransmitStop

	iorlw	0xff		; ensure that ZERO flag not set!

IIC_MIDI_Transmit_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_TransmitStart
;;  DESCRIPTION: starts a transmission
;;  IN:	  MIDI slave number (0-3) in IIC_MIDI_SLAVE
;;  OUT:  returns 0 if slave not available (or buffer full)
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
IIC_MIDI_TransmitStart
	;; skip if slave not available
	call	IIC_MIDI_GetSlaveMask
	andwf	IIC_MIDI_AVAILABLE_SLAVES, W
	bz	IIC_MIDI_TransmitStart_Disabled

	call	IIC_Start		; start IIC
	rlf	IIC_MIDI_SLAVE, W	; send address (write transfer)
	andlw	0x0e
	addlw	IIC_MIDI_BASE_ADDR
	call	IIC_ByteSend

	btfss	IIC_STAT, 0		; return 0 if no acknowledge
	rgoto	IIC_MIDI_TransmitStart_NA

	iorlw	0xff			; return 0xff (ensure that ZERO flag not set!)
	return

IIC_MIDI_TransmitStart_NA		; received no acknowledge
	rcall	IIC_MIDI_TransmitStop	; stop IIC
IIC_MIDI_TransmitStart_Disabled
	andlw	0x00			; return 0 (ensure that ZERO flag set!)
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_TransmitByte
;;  DESCRIPTION: transmits a byte 
;;  IN:	  byte in WREG, MIDI slave number (0-3) in IIC_MIDI_SLAVE
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
IIC_MIDI_TransmitByte
	bsf	LATB, 1
	movwf	IIC_MIDI_TMP		; temporary store value

	;; check if 0xff (control byte) should be sent - if so, we have to
	;; send it twice (one for entering command mode, one for exiting
	;; command mode + sending a 0xff at MIDI Out)
	xorlw	0xff
	bnz	IIC_MIDI_TransmitByte_NotFF	
IIC_MIDI_TransmitByte_FF
	call	IIC_MIDI_TransmitByte_FFOne	; first time
	;; second time...

IIC_MIDI_TransmitByte_FFOne
IIC_MIDI_TransmitByte_NotFF
	clrf	TX_RETRY_CTR		; clear timeout counter (256 tries - more than enough!)

IIC_MIDI_TransmitByteRetry
	decfsz	TX_RETRY_CTR, F
	rgoto	IIC_MIDI_TransmitByteTry
	rgoto	IIC_MIDI_TransmitByte_End	; give up!

IIC_MIDI_TransmitByteTry
	movf	IIC_MIDI_TMP, W
	call	IIC_ByteSend
	btfsc	IIC_STAT, 0		; branch to end if acknowledge received
	rgoto	IIC_MIDI_TransmitByte_End

IIC_MIDI_TransmitByteRestart
	rcall	IIC_MIDI_TransmitStop	; restart until slaves acknowledges again
	rcall	IIC_MIDI_TransmitStart
	bnz	IIC_MIDI_TransmitByteRetry

	decfsz	TX_RETRY_CTR, F
	rgoto	IIC_MIDI_TransmitByteRestart
	;; give up!

IIC_MIDI_TransmitByte_End
	bcf	LATB, 1

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_TransmitStop
;;  DESCRIPTION: sends a stop condition to the slave
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
IIC_MIDI_TransmitStop
	goto	IIC_Stop

;; --------------------------------------------------------------------------
;; IN: slave number in IIC_MIDI_SLAVE
;; OUT: bit mask which selects the slave
IIC_MIDI_GetSlaveMask
	btfsc	IIC_MIDI_SLAVE, 2
	rgoto	IIC_MIDI_GetSlaveMask_4567
IIC_MIDI_GetSlaveMask_0123
	btfsc	IIC_MIDI_SLAVE, 1
	rgoto	IIC_MIDI_GetSlaveMask_23
IIC_MIDI_GetSlaveMask_01
	btfss	IIC_MIDI_SLAVE, 0
	retlw	1 << 0
	retlw	1 << 1

IIC_MIDI_GetSlaveMask_23
	btfss	IIC_MIDI_SLAVE, 0
	retlw	1 << 2
	retlw	1 << 3

IIC_MIDI_GetSlaveMask_4567
	btfsc	IIC_MIDI_SLAVE, 1
	rgoto	IIC_MIDI_GetSlaveMask_67
IIC_MIDI_GetSlaveMask_45
	btfss	IIC_MIDI_SLAVE, 0
	retlw	1 << 4
	retlw	1 << 5

IIC_MIDI_GetSlaveMask_67
	btfss	IIC_MIDI_SLAVE, 0
	retlw	1 << 6
	retlw	1 << 7

;; --------------------------------------------------------------------------
;; IN: slave number in IIC_MIDI_SLAVE
;; OUT: receive status: 0x00 if RI_N of the appr. slave is not active (=1)
IIC_MIDI_GetRI
	andlw	0x00		; (use andlw/iorlw to control the ZERO flag)
	btfsc	IIC_MIDI_SLAVE, 2
	rgoto	IIC_MIDI_GetRI_4567
IIC_MIDI_GetRI_0123
	btfsc	IIC_MIDI_SLAVE, 1
	rgoto	IIC_MIDI_GetRI_23
IIC_MIDI_GetRI_01
	btfsc	IIC_MIDI_SLAVE, 0
	rgoto	IIC_MIDI_GetRI_1
IIC_MIDI_GetRI_0
#if IIC_MIDI_MAX_NUMBER >= 1
	btfss	PORT_RI_N_SLAVE0, PIN_RI_N_SLAVE0	; the RI signal is low-active
	iorlw	0xff
#endif
	return
IIC_MIDI_GetRI_1
#if IIC_MIDI_MAX_NUMBER >= 2
	btfss	PORT_RI_N_SLAVE1, PIN_RI_N_SLAVE1	; the RI signal is low-active
	iorlw	0xff
#endif
	return

IIC_MIDI_GetRI_23
	btfsc	IIC_MIDI_SLAVE, 0
	rgoto	IIC_MIDI_GetRI_3
IIC_MIDI_GetRI_2
#if IIC_MIDI_MAX_NUMBER >= 3
	btfss	PORT_RI_N_SLAVE2, PIN_RI_N_SLAVE2	; the RI signal is low-active
	iorlw	0xff
#endif
	return
IIC_MIDI_GetRI_3
#if IIC_MIDI_MAX_NUMBER >= 4
	btfss	PORT_RI_N_SLAVE3, PIN_RI_N_SLAVE3	; the RI signal is low-active
	iorlw	0xff
#endif
	return

IIC_MIDI_GetRI_4567
	btfsc	IIC_MIDI_SLAVE, 1
	rgoto	IIC_MIDI_GetRI_67
IIC_MIDI_GetRI_45
	btfsc	IIC_MIDI_SLAVE, 0
	rgoto	IIC_MIDI_GetRI_5
IIC_MIDI_GetRI_4
#if IIC_MIDI_MAX_NUMBER >= 5
	btfss	PORT_RI_N_SLAVE4, PIN_RI_N_SLAVE4	; the RI signal is low-active
	iorlw	0xff
#endif
	return
IIC_MIDI_GetRI_5
#if IIC_MIDI_MAX_NUMBER >= 6
	btfss	PORT_RI_N_SLAVE5, PIN_RI_N_SLAVE5	; the RI signal is low-active
	iorlw	0xff
#endif
	return

IIC_MIDI_GetRI_67
	btfsc	IIC_MIDI_SLAVE, 0
	rgoto	IIC_MIDI_GetRI_7
IIC_MIDI_GetRI_6
#if IIC_MIDI_MAX_NUMBER >= 7
	btfss	PORT_RI_N_SLAVE6, PIN_RI_N_SLAVE6	; the RI signal is low-active
	iorlw	0xff
#endif
	return
IIC_MIDI_GetRI_7
#if IIC_MIDI_MAX_NUMBER >= 8
	btfss	PORT_RI_N_SLAVE7, PIN_RI_N_SLAVE7	; the RI signal is low-active
	iorlw	0xff
#endif
	return

; ==========================================================================
	END
