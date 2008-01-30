; $Id$
;
;  MIDI Driver for internal USART
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	INT_MIDI_Init
	global	INT_MIDI_Parse
	global	INT_MIDI_Receive
	global	INT_MIDI_Transmit

	global	_INT_MIDI_Init		; (for C, declaration in int_midi.h)
	global	_INT_MIDI_Parse
	global	_INT_MIDI_Receive
	global	_INT_MIDI_Transmit

	global	_int_midi_rx_package
	global	_int_midi_tx_package

	;; import lables
	extern	_sysex_owner
	extern	_ROUTER_Rx_INT0

; ==========================================================================

;; assembly/C compatible defines
#include "router_ports.h"

; ==========================================================================

accessram		udata_acs
BYTES_TO_SEND		res	1
MIDI_BYTE		res	1

_int_midi_rx_package	res	4
_int_midi_tx_package	res	4

;; define the MIDI record structure (size and offsets)
#define MIDI_IN_SIZE		7
#define MIDI_INx_BYTE0		0
#define MIDI_INx_BYTE1		1
#define MIDI_INx_BYTE2		2
#define MIDI_INx_RUNNING_STATUS	3
#define MIDI_INx_EXPECTED_BYTES	4
#define MIDI_INx_WAIT_BYTES	5
#define MIDI_INx_SYSEX_CTR	6

midi_record		udata
MIDI_IN_INTERNAL	res	MIDI_IN_SIZE

; ==========================================================================
INT_MIDI code
; ==========================================================================

;; this table contains the number of expected bytes (-1) for a MIDI event
EXPECTED_BYTES_COMMON_TABLE
	;;	8x  9x  Ax  Bx  Cx  Dx  Ex  Fx
	db	3,  3,  3,  3,  2,  2,  3,  0; (see below)

;; this table contains the number of expected bytes (-1) for System events
EXPECTED_BYTES_SYSTEM_TABLE
	;;	F0  F1  F2  F3  F4  F5  F6  F7  F8  F9  FA  FB  FC  FD  FE  FF
	db	1,  2,  3,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1

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
;;  FUNCTION: INT_MIDI_Init
;;  DECLARATION: void INT_MIDI_Init(void)
;;  DESCRIPTION: initializes this package
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
_INT_MIDI_Init			; (for C, declaration in int_midi.h)
INT_MIDI_Init
	movlw	MIDI_IN_SIZE
	movwf	PRODL
	lfsr	FSR1, MIDI_IN_INTERNAL
INT_MIDI_InitRAMLoop
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	INT_MIDI_InitRAMLoop

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: INT_MIDI_Parse
;;  DECLARATION: void INT_MIDI_Parse(unsigned char value)
;;  DESCRIPTION: parses for incoming MIDI events
;;               function should be called from MPROC_NotifyReceivedByte
;;  IN:	  incoming byte in WREG
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
_INT_MIDI_Parse			; (for C, declaration in int_midi.h)
INT_MIDI_Parse
	movwf	MIDI_BYTE		; store byte -> MIDI_BYTE

	;; set pointer to parser record
	lfsr	FSR1, MIDI_IN_INTERNAL

	;; new status byte?
	btfss	MIDI_BYTE, 7
	rgoto	INT_MIDI_Parse_NoNewStatus
INT_MIDI_Parse_NewStatus
	;; messages >= 0xf8 don't affect the running status and can just be forwarded
	movlw	0xf8
	cpfslt	MIDI_BYTE, ACCESS
	rgoto	INT_MIDI_Parse_GE_F8
INT_MIDI_Parse_LT_F8
	;; store running status
	movlw	MIDI_INx_RUNNING_STATUS
	movff	MIDI_BYTE, PLUSW1
	;; get number of expected bytes
	TABLE_ADDR EXPECTED_BYTES_COMMON_TABLE
	swapf	MIDI_BYTE, W
	andlw	0x07
	TABLE_ADD_W
	tblrd*
	movf	TABLAT, W
	bz	INT_MIDI_Parse_System
INT_MIDI_Parse_NoSystem
	;; "normal" MIDI event - just store number of expected bytes (1 byte has already been received!)
	decf	TABLAT, F
	movlw	MIDI_INx_EXPECTED_BYTES
	movff	TABLAT, PLUSW1
	movlw	MIDI_INx_WAIT_BYTES		; store same number in WAIT_BYTES counter
	movff	TABLAT, PLUSW1

	rgoto	INT_MIDI_Parse_End
	
INT_MIDI_Parse_System
	;; get number of expected byte from system table
	TABLE_ADDR EXPECTED_BYTES_SYSTEM_TABLE
	movf	MIDI_BYTE, W
	andlw	0x0f
	TABLE_ADD_W
	tblrd*
	decf	TABLAT, F
	movlw	MIDI_INx_EXPECTED_BYTES
	movff	TABLAT, PLUSW1
	movlw	MIDI_INx_WAIT_BYTES		; store same number in WAIT_BYTES counter
	movff	TABLAT, PLUSW1

	;; special treatment for F0
	movf	MIDI_BYTE, W
	xorlw	0xf0
	bnz	INT_MIDI_Parse_System_NoF0
INT_MIDI_Parse_System_F0	
	movlw	MIDI_INx_BYTE0	; midix->byte0 only used by SysEx handler for continuous data streams!
	movff	MIDI_BYTE, PLUSW1
	movlw	MIDI_INx_SYSEX_CTR	; set counter = 1
	clrf	PLUSW1
	incf	PLUSW1, F
	rgoto	INT_MIDI_Parse_End

INT_MIDI_Parse_System_NoF0
	;; special treatment for F7
	movf	MIDI_BYTE, W
	xorlw	0xf7
	bnz	INT_MIDI_Parse_System_NoF7
INT_MIDI_Parse_System_F7
	;; branch depending on number of unsent SysEx bytes
	movlw	MIDI_INx_SYSEX_CTR
	movff	PLUSW1, TABLAT

	clrf	PLUSW1		; ensure that next F7 will just send F7

	movf	TABLAT, W
	bz	INT_MIDI_Parse_System_F7_0
	addlw	-1
	bz	INT_MIDI_Parse_System_F7_1
	rgoto	INT_MIDI_Parse_System_F7_2
INT_MIDI_Parse_System_F7_0
	movlw	0x05			; 5: SysEx ends with single byte
	movwf	_int_midi_rx_package+0	; CIN
	movlw	0xf7
	movwf	_int_midi_rx_package+1	; first byte
	clrf	_int_midi_rx_package+2	; second byte
	clrf	_int_midi_rx_package+3	; third byte
	rgoto	INT_MIDI_Parse_SendPacket

INT_MIDI_Parse_System_F7_1
	movlw	0x06			; 6: SysEx ends with two bytes
	movwf	_int_midi_rx_package+0	; CIN
	movlw	MIDI_INx_BYTE0	
	movff	PLUSW1, _int_midi_rx_package+1	; first byte
	movlw	0xf7
	movwf	_int_midi_rx_package+2	; second byte
	clrf	_int_midi_rx_package+3	; third byte
	rgoto	INT_MIDI_Parse_SendPacket

INT_MIDI_Parse_System_F7_2
	movlw	0x07			; 7: SysEx ends with three bytes
	movwf	_int_midi_rx_package+0	; CIN
	movlw	MIDI_INx_BYTE0	
	movff	PLUSW1, _int_midi_rx_package+1	; first byte
	movlw	MIDI_INx_BYTE1
	movff	PLUSW1, _int_midi_rx_package+2	; second byte
	movlw	0xf7
	movwf	_int_midi_rx_package+3	; third byte
	rgoto	INT_MIDI_Parse_SendPacket

INT_MIDI_Parse_System_NoF7
	rgoto	INT_MIDI_Parse_End

INT_MIDI_Parse_GE_F8
	;; realtime messages don't change the running status and can be sent immediately
	movlw	0x0f			; f: send single byte
	movwf	_int_midi_rx_package+0	; CIN
	movf	MIDI_BYTE, W
	movwf	_int_midi_rx_package+1	; first byte
	clrf	_int_midi_rx_package+2	; second byte
	clrf	_int_midi_rx_package+3	; third byte
	rgoto	INT_MIDI_Parse_SendPacket


INT_MIDI_Parse_NoNewStatus
	;; special treatment for sysex streams
	movlw	MIDI_INx_RUNNING_STATUS
	movf	PLUSW1, W
	xorlw	0xf0
	bnz	INT_MIDI_Parse_NoRS_F0
INT_MIDI_Parse_RS_F0
	movlw	MIDI_INx_SYSEX_CTR	; increment sysex counter and store incoming byte in appr. register
	incf	PLUSW1, F
	decf	PLUSW1, W
	bz	INT_MIDI_Parse_RS_F0_1
	addlw	-1
	bz	INT_MIDI_Parse_RS_F0_2
	rgoto	INT_MIDI_Parse_RS_F0_3
INT_MIDI_Parse_RS_F0_1
	movlw	MIDI_INx_BYTE0
	movff	MIDI_BYTE, PLUSW1
	rgoto	INT_MIDI_Parse_End

INT_MIDI_Parse_RS_F0_2
	movlw	MIDI_INx_BYTE1
	movff	MIDI_BYTE, PLUSW1
	rgoto	INT_MIDI_Parse_End

INT_MIDI_Parse_RS_F0_3
	movlw	MIDI_INx_BYTE2
	movff	MIDI_BYTE, PLUSW1

	;; clear counter
	movlw	MIDI_INx_SYSEX_CTR
	clrf	PLUSW1

	;; send three byte sysex package
	movlw	0x04			; 4: SysEx starts and continues
	movwf	_int_midi_rx_package+0	; CIN
	movlw	MIDI_INx_BYTE0	
	movff	PLUSW1, _int_midi_rx_package+1	; first byte
	movlw	MIDI_INx_BYTE1
	movff	PLUSW1, _int_midi_rx_package+2	; second byte
	movlw	MIDI_INx_BYTE2
	movff	PLUSW1, _int_midi_rx_package+3	; third byte
	rgoto	INT_MIDI_Parse_SendPacket


INT_MIDI_Parse_NoRS_F0
	;; reload WAIT_BYTES counter?
	movlw	MIDI_INx_WAIT_BYTES
	movf	PLUSW1, W
	bnz	INT_MIDI_Parse_NoWBRel
INT_MIDI_Parse_WBRel
	movlw	MIDI_INx_EXPECTED_BYTES
	movff	PLUSW1, PRODL
	movlw	MIDI_INx_WAIT_BYTES
	movff	PRODL, PLUSW1
INT_MIDI_Parse_NoWBRel
	;; always decrement counter (no error - we've already received the second byte!)
	movlw	MIDI_INx_WAIT_BYTES
	decf	PLUSW1, F

	;; store new byte
	movlw	MIDI_INx_EXPECTED_BYTES
	decf	PLUSW1, W
	bnz	INT_MIDI_Parse_ExpNE1
INT_MIDI_Parse_ExpEQ1	; 1 byte expected
	movlw	MIDI_INx_BYTE1
	movff	MIDI_BYTE, PLUSW1
	movlw	MIDI_INx_BYTE2
	clrf	PLUSW1
	rgoto	INT_MIDI_Parse_ExpEQ1_Cont
INT_MIDI_Parse_ExpNE1	; 2 bytes expected
	movlw	MIDI_INx_WAIT_BYTES
	movf	PLUSW1, W
	movlw	MIDI_INx_BYTE1
	skpnz
	movlw	MIDI_INx_BYTE2
	movff	MIDI_BYTE, PLUSW1
INT_MIDI_Parse_ExpEQ1_Cont

	;; put received bytes into packet once WAIT_BYTES is 0
	movlw	MIDI_INx_WAIT_BYTES
	movf	PLUSW1, W
	bnz	INT_MIDI_Parse_NotComplete
INT_MIDI_Parse_Complete
	;; write header (CIN)
	movlw	MIDI_INx_RUNNING_STATUS
	movf	PLUSW1, W
	andlw	0xf0
	xorlw	0xf0
	bz	INT_MIDI_Parse_CompFx
INT_MIDI_Parse_CompNotFx
	movlw	MIDI_INx_RUNNING_STATUS	; common MIDI event
	swapf	PLUSW1, W
	andlw	0x0f
	rgoto	INT_MIDI_Parse_Comp_Cont
	
INT_MIDI_Parse_CompFx
	;; determine how many bytes should be transmitted
	movlw	MIDI_INx_EXPECTED_BYTES
	movf	PLUSW1, W
	bz	INT_MIDI_Parse_CompFx_1
	addlw	-1
	bz	INT_MIDI_Parse_CompFx_2
	rgoto	INT_MIDI_Parse_CompFx_3

INT_MIDI_Parse_CompFx_1
	movlw	0x05		; SysEx common with one byte
	rgoto	INT_MIDI_Parse_Comp_Cont

INT_MIDI_Parse_CompFx_2
	movlw	0x02		; SysEx common with two bytes
	rgoto	INT_MIDI_Parse_Comp_Cont

INT_MIDI_Parse_CompFx_3
	movlw	0x03		; SysEx common with three bytes
	;; 	rgoto	INT_MIDI_Parse_Comp_Cont

INT_MIDI_Parse_Comp_Cont
	movwf	_int_midi_rx_package+0	; CIN

	movlw	MIDI_INx_RUNNING_STATUS	; first byte
	movff	PLUSW1, _int_midi_rx_package+1
	movlw	MIDI_INx_BYTE1		; second byte
	movff	PLUSW1, _int_midi_rx_package+2
	movlw	MIDI_INx_BYTE2		; third byte
	movff	PLUSW1, _int_midi_rx_package+3

INT_MIDI_Parse_SendPacket
	;; call the package hook (available outside this file!)

	movff	_int_midi_rx_package+3, POSTDEC0
	movff	_int_midi_rx_package+2, POSTDEC0
	movff	_int_midi_rx_package+1, POSTDEC0
	movf	_int_midi_rx_package+0, W
	call	_ROUTER_Rx_INT0
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)

INT_MIDI_Parse_NotComplete

INT_MIDI_Parse_End
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: INT_MIDI_Receive
;;  DECLARATION: unsigned char INT_MIDI_Receive(unsigned char port)
;;  DESCRIPTION: receives a package from the internal USART
;;  IN:	  (port is currently ignored!)
;;  OUT:  4 bytes in _int_midi_rx_package,
;;        WREG contains _int_midi_rx_package+0
;;        if WREG/_int_midi_rx_package+0 == 0, no data has been received
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
_INT_MIDI_Receive		; (for C, declaration in int_midi.h)
INT_MIDI_Receive
	;; not used here, instead we are using INT_MIDI_Parse

	;; clear package content
	clrf	_int_midi_rx_package+0
	clrf	_int_midi_rx_package+1
	clrf	_int_midi_rx_package+2
	clrf	_int_midi_rx_package+3

	movf	_int_midi_rx_package+0, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: INT_MIDI_Transmit
;;  DECLARATION: unsigned char INT_MIDI_Transmit(unsigned char port, unsigned char ptype, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
;;  DESCRIPTION: sends a package to internal USART
;;  IN:	  package in _int_midi_tx_package, port is currently ignored!
;;  OUT:  returns 0 if invalid package
;;  USES: -
;;  EXAMPLE:
;; --------------------------------------------------------------------------
_INT_MIDI_Transmit			; (for C, declaration in int_midi.h)
	;; "port" in WREG is ignored here

	movff	FSR0L, FSR2L		; get package content from stack
	movff	PREINC2, _int_midi_tx_package+0
	movff	PREINC2, _int_midi_tx_package+1
	movff	PREINC2, _int_midi_tx_package+2
	movff	PREINC2, _int_midi_tx_package+3

INT_MIDI_Transmit
	call	MIOS_MIDI_TxBufferFree	; exit if buffer not free
	bz	INT_MIDI_Transmit_End

	TABLE_ADDR BYTES_TO_SEND_TABLE	; get number of bytes -> TABLAT
	movf	_int_midi_tx_package+0, W
	andlw	0x0f
	TABLE_ADD_W
	tblrd*

	movf	TABLAT, W		; skip checks below if no byte to send
	bz	INT_MIDI_Transmit_End
	movwf	BYTES_TO_SEND

	movlw	0			; send first byte
	cpfsgt	BYTES_TO_SEND, ACCESS
 	rgoto	INT_MIDI_Transmit_LT1
	movf	_int_midi_tx_package+1, W
	call	MIOS_MIDI_TxBufferPut
INT_MIDI_Transmit_LT1

	movlw	1			; send second byte
	cpfsgt	BYTES_TO_SEND, ACCESS
 	rgoto	INT_MIDI_Transmit_LT2
	movf	_int_midi_tx_package+2, W
	call	MIOS_MIDI_TxBufferPut
INT_MIDI_Transmit_LT2

	movlw	2			; send third byte
	cpfsgt	BYTES_TO_SEND, ACCESS
	rgoto	INT_MIDI_Transmit_LT3
	movf	_int_midi_tx_package+3, W
	call	MIOS_MIDI_TxBufferPut
INT_MIDI_Transmit_LT3

	iorlw	0xff		; ensure that ZERO flag not set!

INT_MIDI_Transmit_End
	return


; ==========================================================================
	END
