; $Id$
;
;  MIDI Driver
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

#define MIDI_RX_BUFFER_SIZE	0x80
#define MIDI_TX_BUFFER_SIZE	0x80

; Number of update cycles (* 1mS) the LEDs will stay active on a Rx/Tx event
#define MIDI_LEDS_RXTX_DELAY	30

; note: LED outputs won't be set if port is specified with 0
LED_RX_PORT	EQU	PORTB
LED_RX_PIN	EQU	6
LED_TX_PORT	EQU	PORTB
LED_TX_PIN	EQU	7


midi_rx			udata
MIDI_RX_BUFFER		res	MIDI_RX_BUFFER_SIZE

midi_tx			udata
MIDI_TX_BUFFER		res	MIDI_TX_BUFFER_SIZE

accessram		udata_acs
MIDI_RX_BUFFER_TMP	res	1
MIDI_RX_BUFFER_HEAD	res	1
MIDI_RX_BUFFER_TAIL	res	1
MIDI_TX_BUFFER_TMP	res	1
MIDI_TX_BUFFER_HEAD	res	1
MIDI_TX_BUFFER_TAIL	res	1

LED_TX_CTR		res	1
LED_RX_CTR		res	1

; ==========================================================================
MIDI	code
; ==========================================================================

MIDI_Init
	; set baudrate (31250 baud @ 48MHz)
	movlw	(1 << TXEN)
	movwf	TXSTA
  	movlw	23
	movwf	SPBRG

	; Configure Receive Status and Control Register
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	;; enable receiver IRQ
	bsf	PIE1, RCIE

	;; make sure that USART Receive Buffer is empty
	bcf	RCSTA, CREN
	movf	RCREG, W
	bsf	RCSTA, CREN

	; initialize ringbuffers
	clrf	MIDI_RX_BUFFER_HEAD
	clrf	MIDI_RX_BUFFER_TAIL

	clrf	MIDI_TX_BUFFER_HEAD
	clrf	MIDI_TX_BUFFER_TAIL

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
;;	bz	ReceivedSomething
;; 
;; --------------------------------------------------------------------------
MIDI_RxBufferUsed
	movf	MIDI_RX_BUFFER_HEAD, W
	cpfslt	MIDI_RX_BUFFER_TAIL, ACCESS
	rgoto	MIDI_RxBufferUsed_Geq
	sublw	MIDI_RX_BUFFER_SIZE
	addwf	MIDI_RX_BUFFER_TAIL, W
	return
MIDI_RxBufferUsed_Geq
	subwf	MIDI_RX_BUFFER_TAIL, W
MIDI_RxBufferUsed_End
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
	rcall	MIDI_RxBufferUsed
	sublw	MIDI_RX_BUFFER_SIZE - 1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferPut
;;  DESCRIPTION: puts a byte onto the receive buffer
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR1
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
	cpfseq	MIDI_RX_BUFFER_HEAD, ACCESS
	rgoto	MIDI_RxBufferPut_Ok
	return					; avoid overrun!
MIDI_RxBufferPut_Ok
	movwf	MIDI_RX_BUFFER_TAIL		; save new pointer
	lfsr	FSR1, MIDI_RX_BUFFER		; prepare FSR
	addwf	FSR1L, F			; add tail offset
	movf	MIDI_RX_BUFFER_TMP, W		; push byte onto buffer
	movwf	INDF1

	;; trigger LED
	movlw	MIDI_LEDS_RXTX_DELAY
	movwf	LED_RX_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RxBufferGet
;;  DESCRIPTION: gets a byte from the receive buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR1
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
	bnz	MIDI_RxBufferGet_NoReset
MIDI_RxBufferGet_Reset
	clrf	MIDI_RX_BUFFER_HEAD
MIDI_RxBufferGet_NoReset
	lfsr	FSR1, MIDI_RX_BUFFER
	movf	MIDI_RX_BUFFER_HEAD, W
	addwf	FSR1L, F
	movf	INDF1, W
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
	movf	MIDI_TX_BUFFER_HEAD, W
	cpfslt	MIDI_TX_BUFFER_TAIL, ACCESS
	rgoto	MIDI_TxBufferUsed_Geq
	sublw	MIDI_TX_BUFFER_SIZE
	addwf	MIDI_TX_BUFFER_TAIL, W
	return
MIDI_TxBufferUsed_Geq
	subwf	MIDI_TX_BUFFER_TAIL, W
MIDI_TxBufferUsed_End
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
	rcall	MIDI_TxBufferUsed
	sublw	MIDI_TX_BUFFER_SIZE - 1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferPut
;;  DESCRIPTION: puts a byte onto the transmit buffer.
;;  If Tx buffer is full, the function will be suspended until one byte has been
;;  transmitted via MIDI.
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR1
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
	cpfseq	MIDI_TX_BUFFER_HEAD, ACCESS
	rgoto	MIDI_TxBufferPut_Ok
#if 0
	return					; prevent overrun
#else
	movf	MIDI_TX_BUFFER_TMP, W
	rgoto	MIDI_TxBufferPut		; repeat until buffer is free again
#endif
MIDI_TxBufferPut_Ok
	IRQ_DISABLE				; disable interrupts to avoid a malfunction when a Tx IRQ happens here
	movwf	MIDI_TX_BUFFER_TAIL		; save new pointer

	lfsr	FSR1, MIDI_TX_BUFFER		; prepare FSR
	addwf	FSR1L, F			; add tail offset
	movf	MIDI_TX_BUFFER_TMP, W		; push byte onto buffer
	movwf	INDF1

	bsf	PIE1, TXIE			; (re-)enable transmit IRQ
	IRQ_ENABLE				; enable interrupts again

	movlw	MIDI_LEDS_RXTX_DELAY	; trigger LED
	movwf	LED_TX_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferGet
;;  DESCRIPTION: gets a byte from the transmit buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR1
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
	bnz	MIDI_TxBufferGet_NoReset
MIDI_TxBufferGet_Reset
	clrf	MIDI_TX_BUFFER_HEAD
MIDI_TxBufferGet_NoReset
	lfsr	FSR1, MIDI_TX_BUFFER
	movf	MIDI_TX_BUFFER_HEAD, W
	addwf	FSR1L, F
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxBufferFlush
;;  DESCRIPTION: waits until all MIDI bytes in the Tx buffer have been transmitted
;;  IN:	  -
;;  OUT:  -
;;  USES: FSR1
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
	rcall	MIDI_TxBufferUsed
	bnz	MIDI_TxBufferFlush_WaitTx
	;; wait until byte has been sent
MIDI_TxBufferFlush_Sync
	btfss	TXSTA, TRMT
	rgoto	MIDI_TxBufferFlush_Sync	
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_TxSendDirect
;;  DESCRIPTION: sends unbuffered byte - only for debugging!
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MIDI_TxSendDirect
MIDI_TxSendDirect_Poll
	btfss	TXSTA, TRMT
	rgoto	MIDI_TxSendDirect_Poll
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
