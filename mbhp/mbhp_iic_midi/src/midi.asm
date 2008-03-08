; $Id$
;
;  MIDI IO Routines
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
	global	MIDI_Init

	global	MIDI_RxBufferUsed
	global	MIDI_RxBufferFree
	global	MIDI_RxBufferPut
	global	MIDI_RxBufferGet

	global	MIDI_TxBufferUsed
	global	MIDI_TxBufferFree
	global	MIDI_TxBufferPut
	global	MIDI_TxBufferGet

	global	MIDI_TxBufferFlush
	global	MIDI_TxSendDirect

	global	MIDI_LEDS_Handler

	global	MIDI_TX_BUFFER_HEAD	; (for fast access from IRQ handler)
	global	MIDI_TX_BUFFER_TAIL

; ==========================================================================

gpr0			udata
MIDI_RX_BUFFER_TMP	res	1
MIDI_RX_BUFFER_HEAD	res	1
MIDI_RX_BUFFER_TAIL	res	1
MIDI_RX_BUFFER_CTR	res	1
MIDI_TX_BUFFER_TMP	res	1
MIDI_TX_BUFFER_HEAD	res	1
MIDI_TX_BUFFER_TAIL	res	1
MIDI_TX_BUFFER_CTR	res	1

LED_TX_CTR		res	1
LED_RX_CTR		res	1

gpr2			udata	0x110
#define MIDI_RX_BUFFER_SIZE 0x60
MIDI_RX_BUFFER		res	MIDI_RX_BUFFER_SIZE

#define MIDI_TX_BUFFER_SIZE 0x60
gpr3			udata	0x190
MIDI_TX_BUFFER		res	MIDI_TX_BUFFER_SIZE


; ==========================================================================
MIDI	code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_Init
;;  DESCRIPTION: initialises the MIDI interface: baudrate will be set,
;;  Rx and Tx buffer will be emptied
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIDI_Init
MIDI_Init_MIDIBaudrate
	SWITCHBANK_0_1				; set baudrate to 31250 baud
	movlw	(1 << TXEN)
	movwf	TXSTA
#if INTERNAL_OSC
  	movlw	0x03		; 31250 baud @8MHz
#else
  	movlw	0x09		; 31250 baud @20MHz
#endif
	movwf	SPBRG
	SWITCHBANK_1_0

	; Configure Receive Status and Control Register
        movf    RCREG, W
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	; initialize ringbuffers
	clrf	MIDI_RX_BUFFER_TAIL
	clrf	MIDI_RX_BUFFER_HEAD
	clrf	MIDI_RX_BUFFER_CTR

	clrf	MIDI_TX_BUFFER_TAIL
	clrf	MIDI_TX_BUFFER_HEAD
	clrf	MIDI_TX_BUFFER_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferUsed
;;  DESCRIPTION: returns number of used bytes in receive buffer
;;  IN:	  -
;;  OUT:  number of used bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; branch if something has been received via MIDI
;;	call	MIDI_RxBufferUsed
;;	bnz	ReceivedSomething
;; 
;; --------------------------------------------------------------------------
MIDI_RxBufferUsed
	movf	MIDI_RX_BUFFER_CTR, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferFree
;;  DESCRIPTION: returns number of free bytes in receive buffer
;;  IN:	  -
;;  OUT:  number of free bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Rx Buffer if it isn't full
;;	call	MIDI_RxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIDI_RxBufferPut
;;
;; --------------------------------------------------------------------------
MIDI_RxBufferFree
	movf	MIDI_RX_BUFFER_CTR, W
	sublw	MIDI_RX_BUFFER_SIZE-1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferPut
;;  DESCRIPTION: puts a byte onto the receive buffer
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Rx Buffer if it isn't full
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIDI_RxBufferPut
;;
;; --------------------------------------------------------------------------
MIDI_RxBufferPut
	movwf	MIDI_RX_BUFFER_TMP		; save byte which should be sent

	incf	MIDI_RX_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	MIDI_RX_BUFFER_SIZE
	skpz
	xorlw	MIDI_RX_BUFFER_SIZE		; XOR back to old value
	movwf	FSR
	xorwf	MIDI_RX_BUFFER_HEAD, W
	skpnz
	return					; avoid overrun!

	movf	FSR, W				; prepare buffer pointer
	movwf	MIDI_RX_BUFFER_TAIL
	addlw	LOW(MIDI_RX_BUFFER)
	movwf	FSR
	movf	MIDI_RX_BUFFER_TMP, W		; push byte onto buffer
	SWITCH_FSR_UPPER
	movwf	INDF
	SWITCH_FSR_LOWER
	incf	MIDI_RX_BUFFER_CTR, F

	;; trigger LED
	movlw	MIDI_LEDS_RXTX_DELAY
	movwf	LED_RX_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferGet
;;  DESCRIPTION: gets a byte from the receive buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; get a MIDI byte from the Rx Buffer if something has been received
;;	call	MIDI_RxBufferUsed
;;	bz	NothingReceived
;;	call	MIDI_RxBufferGet
;;
;; --------------------------------------------------------------------------
MIDI_RxBufferGet
	incf	MIDI_RX_BUFFER_HEAD, F
	movf	MIDI_RX_BUFFER_HEAD, W
	xorlw	MIDI_RX_BUFFER_SIZE
	skpnz
	clrf	MIDI_RX_BUFFER_HEAD

	movf	MIDI_RX_BUFFER_HEAD, W
	addlw	LOW(MIDI_RX_BUFFER)
	movwf	FSR
	SWITCH_FSR_UPPER
	movf	INDF, W
	SWITCH_FSR_LOWER
	decf	MIDI_RX_BUFFER_CTR, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferUsed
;;  DESCRIPTION: returns number of used bytes in buffer
;;  IN:	  -
;;  OUT:  number of used bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; branch if something has been put into the Tx Buffer
;;	call	MIDI_TxBufferUsed
;;	bz	ReceivedSomething
;; 
;; --------------------------------------------------------------------------
MIDI_TxBufferUsed
	movf	MIDI_TX_BUFFER_CTR, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferFree
;;  DESCRIPTION: returns number of free bytes in transmit buffer
;;  IN:	  -
;;  OUT:  number of free bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Tx Buffer if it isn't full
;;	call	MIDI_TxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIDI_TxBufferPut
;;
;; --------------------------------------------------------------------------
MIDI_TxBufferFree
	movf	MIDI_TX_BUFFER_CTR, W
	sublw	MIDI_TX_BUFFER_SIZE-1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferPut
;;  DESCRIPTION: puts a byte onto the transmit buffer.
;;  If Tx buffer is full, function will be suspended until one byte has been
;;  transmitted via MIDI
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Tx Buffer if it isn't full
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIDI_TxBufferPut
;;
;; --------------------------------------------------------------------------
MIDI_TxBufferPut
	movwf	MIDI_TX_BUFFER_TMP		; save byte which should be sent

	incf	MIDI_TX_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	MIDI_TX_BUFFER_SIZE
	skpz
	xorlw	MIDI_TX_BUFFER_SIZE		; XOR back to old value
	movwf	FSR
	xorwf	MIDI_TX_BUFFER_HEAD, W
	bnz	MIDI_TxBufferPut_Ok

	;; repeat until buffer is free again
	IRQ_ENABLE		; (we must ensure that interrupts are enabled!)
	movf	MIDI_TX_BUFFER_TMP, W
	goto	MIDI_TxBufferPut

MIDI_TxBufferPut_Ok
	IRQ_DISABLE				; disable interrupts to avoid a malfunction when a Tx IRQ happens here
	movf	FSR, W				; prepare buffer pointer
	movwf	MIDI_TX_BUFFER_TAIL
	addlw	LOW(MIDI_TX_BUFFER)
	movwf	FSR
	movf	MIDI_TX_BUFFER_TMP, W		; push byte onto buffer
	SWITCH_FSR_UPPER
	movwf	INDF
	SWITCH_FSR_LOWER
	incf	MIDI_TX_BUFFER_CTR, F

	SWITCHBANK_0_1
	bsf	PIE1, TXIE			; (re-)enable transmit IRQ
	SWITCHBANK_1_0

	IRQ_ENABLE				; enable interrupts again

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferGet
;;  DESCRIPTION: gets a byte from the transmit buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; get a MIDI byte from the Tx Buffer if new byte is available
;;	call	MIDI_TxBufferUsed
;;	bz	NoByteAvailable
;;	call	MIDI_TxBufferGet
;;
;; --------------------------------------------------------------------------
MIDI_TxBufferGet
	incf	MIDI_TX_BUFFER_HEAD, F
	movf	MIDI_TX_BUFFER_HEAD, W
	xorlw	MIDI_TX_BUFFER_SIZE
	skpnz
	clrf	MIDI_TX_BUFFER_HEAD

	movf	MIDI_TX_BUFFER_HEAD, W
	addlw	LOW(MIDI_TX_BUFFER)
	movwf	FSR

	movlw	MIDI_LEDS_RXTX_DELAY	; trigger LED
	movwf	LED_TX_CTR

	SWITCH_FSR_UPPER
	movf	INDF, W
	SWITCH_FSR_LOWER
	decf	MIDI_TX_BUFFER_CTR, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferFlush
;;  DESCRIPTION: waits until all MIDI bytes in the Tx buffer have been transmitted
;;  IN:	  -
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; wait until buffer is empty, thereafter send a bunch of new bytes
;;	call	MIDI_TxBufferFlush
;;	movlw	0xf0
;;	call	MIDI_TxBufferPut
;;	;;  ...
;;
;; --------------------------------------------------------------------------
MIDI_TxBufferFlush
	clrwdt
MIDI_TxBufferFlush_WaitTx
	call	MIDI_TxBufferUsed
	bnz	MIDI_TxBufferFlush_WaitTx
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxSendDirect
;;  DESCRIPTION: sends unbuffered byte - only for debugging!
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIDI_TxSendDirect
	SWITCHBANK_0_1
MIDI_TxSendDirect_Poll
	btfss	TXSTA, TRMT
	goto	MIDI_TxSendDirect_Poll
	SWITCHBANK_1_0
	movwf	TXREG
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_LEDS_Handler
;;  DESCRIPTION: this function should be called periodically from a timer
;;  or the mainloop. It decrements the Rx/Tx counters and sets the LEDs 
;;  depending on the counter values
;; --------------------------------------------------------------------------
MIDI_LEDS_Handler

MIDI_LEDS_Handler_Rx
	movf	LED_RX_CTR, W
	bz	MIDI_LEDS_Handler_RxOff
MIDI_LEDS_Handler_RxOn
	decf	LED_RX_CTR, F
#if LED_RX_PORT
	bcf	LED_RX_PORT, LED_RX_PIN
#endif
	goto	MIDI_LEDS_Handler_Rx_End
MIDI_LEDS_Handler_RxOff
#if LED_RX_PORT
	bsf	LED_RX_PORT, LED_RX_PIN
#endif
MIDI_LEDS_Handler_Rx_End

MIDI_LEDS_Handler_Tx
	movf	LED_TX_CTR, W
	bz	MIDI_LEDS_Handler_TxOff
MIDI_LEDS_Handler_TxOn
	decf	LED_TX_CTR, F
#if LED_TX_PORT
	bcf	LED_TX_PORT, LED_TX_PIN
#endif
	goto	MIDI_LEDS_Handler_Tx_End
MIDI_LEDS_Handler_TxOff
#if LED_TX_PORT
	bsf	LED_TX_PORT, LED_TX_PIN
#endif
MIDI_LEDS_Handler_Tx_End

	return

; ==========================================================================
	END
