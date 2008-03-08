; $Id$
;
;  MIDI Protocol Routines
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
	global	PROT_Init
	global	PROT_Tick

	global	PROT_RX_PACKAGE
	global	PROT_TX_PACKAGE

	;; import lables
	extern	MIDI_RxBufferUsed
	extern	MIDI_RxBufferGet
	extern	MIDI_TxBufferFree
	extern	MIDI_TxBufferPut

	extern	FLASH_ReadInc


; ==========================================================================

gpr0			udata
MIDI_BYTE		res	1
MIDI_IN_BYTE0		res	1
MIDI_IN_BYTE1		res	1
MIDI_IN_BYTE2		res	1
MIDI_IN_RUNNING_STATUS	res	1
MIDI_IN_EXPECTED_BYTES	res	1
MIDI_IN_WAIT_BYTES	res	1
MIDI_IN_SYSEX_CTR	res	1

PROT_RX_PACKAGE		res	4
PROT_TX_PACKAGE		res	4

; ==========================================================================
PROT code
; ==========================================================================

;; this table contains the number of expected bytes (-1) for a MIDI event
EXPECTED_BYTES_COMMON_TABLE
	;;	8x  9x  Ax  Bx  Cx  Dx  Ex  Fx
	dw	3,  3,  3,  3,  2,  2,  3,  0; (see below)

;; this table contains the number of expected bytes (-1) for System events
EXPECTED_BYTES_SYSTEM_TABLE
	;;	F0  F1  F2  F3  F4  F5  F6  F7  F8  F9  FA  FB  FC  FD  FE  FF
	dw	1,  2,  3,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1

;; this table contains the number of bytes which have to be sent within a USB MIDI packet
USBCLS_BYTES_TO_SEND_TABLE
	;;	see Table 4-1 of USB MIDI spec v1.0:
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
	dw	0,0,2,3,3,1,2,3,3,3,3,3,2,2,3,1

;; --------------------------------------------------------------------------
;;  FUNCTION: PROT_Init
;;  DESCRIPTION: initialises the MIDI protocol handler
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
PROT_Init
	clrf	MIDI_IN_BYTE0
	clrf	MIDI_IN_BYTE1
	clrf	MIDI_IN_BYTE2
	clrf	MIDI_IN_RUNNING_STATUS
	clrf	MIDI_IN_EXPECTED_BYTES
	clrf	MIDI_IN_WAIT_BYTES
	clrf	MIDI_IN_SYSEX_CTR

	clrf	PROT_RX_PACKAGE+0
	clrf	PROT_TX_PACKAGE+0

	bsf	RI_N_PORT, RI_N_PIN	; set RI_N=1 (receive interrupt) pin - it's low active!

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: PROT_Tick
;;  DESCRIPTION: handles the MIDI protocol
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
PROT_Tick

PROT_Tick_Rx
	;; exit if nothing in receive buffer
	call	MIDI_RxBufferUsed
	bz	PROT_Tick_Rx_End

	;; skip if package not taken by IIC yet
	movf	PROT_RX_PACKAGE+0, W
	bnz	PROT_Tick_Rx_End

	;; get next byte
	call	MIDI_RxBufferGet
	movwf	MIDI_BYTE

	;; new status byte?
	btfss	MIDI_BYTE, 7
	goto	PROT_Tick_Rx_NoNewStatus
PROT_Tick_Rx_NewStatus
	;; messages >= 0xf8 don't affect the running status and can just be forwarded
	movf	MIDI_BYTE, W
	andlw	0xf8		; matches with 0xf8..0xff
	xorlw	0xf8
	bz	PROT_Tick_Rx_GE_F8
PROT_Tick_Rx_LT_F8
	;; store running status
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_RUNNING_STATUS
	;; get number of expected bytes
	FLASH_ADDR EXPECTED_BYTES_COMMON_TABLE
	swapf	MIDI_BYTE, W
	andlw	0x07
	FLASH_ADD_W
	call	FLASH_ReadInc
	bz	PROT_Tick_Rx_System
PROT_Tick_Rx_NoSystem
	;; "normal" MIDI event - just store number of expected bytes (1 byte has already been received!)
	addlw	-1
	movwf	MIDI_IN_EXPECTED_BYTES
	movwf	MIDI_IN_WAIT_BYTES		; store same number in WAIT_BYTES counter

	goto	PROT_Tick_Rx_End
	
PROT_Tick_Rx_System
	;; get number of expected byte from system table
	FLASH_ADDR EXPECTED_BYTES_SYSTEM_TABLE
	movf	MIDI_BYTE, W
	andlw	0x0f
	FLASH_ADD_W
	call	FLASH_ReadInc
	addlw	-1
	movwf	MIDI_IN_EXPECTED_BYTES
	movwf	MIDI_IN_WAIT_BYTES		; store same number in WAIT_BYTES counter

	;; special treatment for F0
	movf	MIDI_BYTE, W
	xorlw	0xf0
	bnz	PROT_Tick_Rx_System_NoF0
PROT_Tick_Rx_System_F0
	movf	MIDI_BYTE, W	; midix->byte0 only used by SysEx handler for continuous data streams!
	movwf	MIDI_IN_BYTE0
	movlw	1		; set counter = 1
	movwf	MIDI_IN_SYSEX_CTR	
	goto	PROT_Tick_Rx_End
	
PROT_Tick_Rx_System_NoF0
	;; special treatment for F7
	movf	MIDI_BYTE, W
	xorlw	0xf7
	bnz	PROT_Tick_Rx_System_NoF7
PROT_Tick_Rx_System_F7
	;; branch depending on number of unsent SysEx bytes
	movf	MIDI_IN_SYSEX_CTR, W	
	bz	PROT_Tick_Rx_System_F7_0
	addlw	-1
	bz	PROT_Tick_Rx_System_F7_1
	goto	PROT_Tick_Rx_System_F7_2
PROT_Tick_Rx_System_F7_0
	clrf	MIDI_IN_SYSEX_CTR	; ensure that next F7 will just send F7
	movlw	0x05			; 5: SysEx ends with single byte
	movwf	PROT_RX_PACKAGE+0	; CIN
	movlw	0xf7			; first byte
	movwf	PROT_RX_PACKAGE+1
	clrf	PROT_RX_PACKAGE+2	; second byte
	clrf	PROT_RX_PACKAGE+3	; third byte
	goto	PROT_Tick_Rx_SendPacket

PROT_Tick_Rx_System_F7_1
	clrf	MIDI_IN_SYSEX_CTR	; ensure that next F7 will just send F7
	movlw	0x06			; 6: SysEx ends with two bytes
	movwf	PROT_RX_PACKAGE+0	; CIN
	movf	MIDI_IN_BYTE0, W	; first byte
	movwf	PROT_RX_PACKAGE+1
	movlw	0xf7			; second byte
	movwf	PROT_RX_PACKAGE+2
	clrf	PROT_RX_PACKAGE+3	; third byte
	goto	PROT_Tick_Rx_SendPacket

PROT_Tick_Rx_System_F7_2
	clrf	MIDI_IN_SYSEX_CTR	; ensure that next F7 will just send F7
	movlw	0x07			; 7: SysEx ends with three bytes
	movwf	PROT_RX_PACKAGE+0	; CIN
	movf	MIDI_IN_BYTE0, W	; first byte
	movwf	PROT_RX_PACKAGE+1
	movf	MIDI_IN_BYTE1, W	; second byte
	movwf	PROT_RX_PACKAGE+2
	movlw	0xf7			; third byte
	movwf	PROT_RX_PACKAGE+3
	goto	PROT_Tick_Rx_SendPacket

PROT_Tick_Rx_System_NoF7
	goto	PROT_Tick_Rx_End

PROT_Tick_Rx_GE_F8
	;; realtime messages don't change the running status and can be sent immediately
	movlw	0x0f			; f: send single byte
	movwf	PROT_RX_PACKAGE+0	; CIN
	movf	MIDI_BYTE, W		; first byte
	movwf	PROT_RX_PACKAGE+1
	clrf	PROT_RX_PACKAGE+2	; second byte
	clrf	PROT_RX_PACKAGE+3	; third byte
	goto	PROT_Tick_Rx_SendPacket


PROT_Tick_Rx_NoNewStatus
	;; special treatment for sysex streams
	movf	MIDI_IN_RUNNING_STATUS, W
	xorlw	0xf0
	bnz	PROT_Tick_Rx_NoRS_F0
PROT_Tick_Rx_RS_F0
	incf	MIDI_IN_SYSEX_CTR, F	; increment sysex counter and store incoming byte in appr. register
	decf	MIDI_IN_SYSEX_CTR, W
	bz	PROT_Tick_Rx_RS_F0_1
	addlw	-1
	bz	PROT_Tick_Rx_RS_F0_2
	goto	PROT_Tick_Rx_RS_F0_3
PROT_Tick_Rx_RS_F0_1
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE0
	goto	PROT_Tick_Rx_End

PROT_Tick_Rx_RS_F0_2
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE1
	goto	PROT_Tick_Rx_End

PROT_Tick_Rx_RS_F0_3
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE2

	;; clear counter
	clrf	MIDI_IN_SYSEX_CTR

	;; send three byte sysex package
	movlw	0x04			; 4: SysEx starts and continues
	movwf	PROT_RX_PACKAGE+0	; CIN
	movf	MIDI_IN_BYTE0, W	; first byte
	movwf	PROT_RX_PACKAGE+1
	movf	MIDI_IN_BYTE1, W	; second byte
	movwf	PROT_RX_PACKAGE+2
	movf	MIDI_IN_BYTE2, W	; third byte
	movwf	PROT_RX_PACKAGE+3
	goto	PROT_Tick_Rx_SendPacket


PROT_Tick_Rx_NoRS_F0
	;; reload WAIT_BYTES counter?
	movf	MIDI_IN_WAIT_BYTES, W
	bnz	PROT_Tick_Rx_NoWBRel
PROT_Tick_Rx_WBRel
	movf	MIDI_IN_EXPECTED_BYTES, W
	movwf	MIDI_IN_WAIT_BYTES
PROT_Tick_Rx_NoWBRel
	;; always decrement counter (no error - we've already received the second byte!)
	decf	MIDI_IN_WAIT_BYTES, F

	;; store new byte
	decf	MIDI_IN_EXPECTED_BYTES, W
	bnz	PROT_Tick_Rx_ExpNE1
PROT_Tick_Rx_ExpEQ1	; 1 byte expected
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE1
	clrf	MIDI_IN_BYTE2
	goto	PROT_Tick_Rx_ExpEQ1_Cont
PROT_Tick_Rx_ExpNE1	; 2 bytes expected
	movf	MIDI_IN_WAIT_BYTES, W
	bz	PROT_Tick_Rx_ExpNE1_2
PROT_Tick_Rx_ExpNE1_1
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE1
	goto	PROT_Tick_Rx_ExpEQ1_Cont
PROT_Tick_Rx_ExpNE1_2
	movf	MIDI_BYTE, W
	movwf	MIDI_IN_BYTE2
	;; 	goto	PROT_Tick_Rx_ExpEQ1_Cont
PROT_Tick_Rx_ExpEQ1_Cont

	;; put received bytes into packet once WAIT_BYTES is 0
	movf	MIDI_IN_WAIT_BYTES, W
	bnz	PROT_Tick_Rx_NotComplete
PROT_Tick_Rx_Complete
	;; determine CIN
	movf	MIDI_IN_RUNNING_STATUS, W
	andlw	0xf0
	xorlw	0xf0
	bz	PROT_Tick_Rx_CompFx
PROT_Tick_Rx_CompNotFx
	swapf	MIDI_IN_RUNNING_STATUS, W	; common MIDI event
	andlw	0x0f
	goto	PROT_Tick_Rx_Comp_Cont
	
PROT_Tick_Rx_CompFx
	;; determine how many bytes should be transmitted
	movf	MIDI_IN_EXPECTED_BYTES, W
	bz	PROT_Tick_Rx_CompFx_1
	addlw	-1
	bz	PROT_Tick_Rx_CompFx_2
	goto	PROT_Tick_Rx_CompFx_3

PROT_Tick_Rx_CompFx_1
	movlw	0x05		; SysEx common with one byte
	goto	PROT_Tick_Rx_Comp_Cont

PROT_Tick_Rx_CompFx_2
	movlw	0x02		; SysEx common with two bytes
	goto	PROT_Tick_Rx_Comp_Cont

PROT_Tick_Rx_CompFx_3
	movlw	0x03		; SysEx common with three bytes
	;; 	goto	PROT_Tick_Rx_Comp_Cont

PROT_Tick_Rx_Comp_Cont
	movwf	PROT_RX_PACKAGE+0	; CIN
	movf	MIDI_IN_RUNNING_STATUS, W ; first byte
	movwf	PROT_RX_PACKAGE+1
	movf	MIDI_IN_BYTE1, W	; second byte
	movwf	PROT_RX_PACKAGE+2
	movf	MIDI_IN_BYTE2, W	; third byte
	movwf	PROT_RX_PACKAGE+3

PROT_Tick_Rx_SendPacket
	bcf	RI_N_PORT, RI_N_PIN	; set RI_N=0 (receive interrupt) pin - it's low active!
					; the pin is set back to 1 in iic.asm
PROT_Tick_Rx_NotComplete

PROT_Tick_Rx_End

	return

; ==========================================================================
	END
