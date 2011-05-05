; $Id$
;
;  UART IO Routines
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
	global	UART_Init
	global	UART_SpeakJetInit

	global	UART_RxBufferUsed
	global	UART_RxBufferFree
	global	UART_RxBufferPut
	global	UART_RxBufferGet

	global	UART_TxBufferUsed
	global	UART_TxBufferFree
	global	UART_TxBufferPut
	global	UART_TxBufferGet

	global	UART_TxBufferFlush
	global	UART_TxSendDirect

	global	UART_LEDS_Handler

	global	UART_TX_BUFFER_HEAD	; (for fast access from IRQ handler)
	global	UART_TX_BUFFER_TAIL

; ==========================================================================

gpr0			udata
UART_RX_BUFFER_TMP	res	1
UART_RX_BUFFER_HEAD	res	1
UART_RX_BUFFER_TAIL	res	1
UART_RX_BUFFER_CTR	res	1
UART_TX_BUFFER_TMP	res	1
UART_TX_BUFFER_HEAD	res	1
UART_TX_BUFFER_TAIL	res	1
UART_TX_BUFFER_CTR	res	1
UART_DELAY_CTR		res	1

LED_TX_CTR		res	1
LED_RX_CTR		res	1

gpr2			udata	0x110
#define UART_RX_BUFFER_SIZE 0x60
UART_RX_BUFFER		res	UART_RX_BUFFER_SIZE

#define UART_TX_BUFFER_SIZE 0x60
gpr3			udata	0x190
UART_TX_BUFFER		res	UART_TX_BUFFER_SIZE


; ==========================================================================
UART	code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_Init
;;  DESCRIPTION: initialises the UART interface: baudrate will be set,
;;  Rx and Tx buffer will be emptied
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
UART_Init
	;; set baudrate at PIC side
	SWITCHBANK_0_1
	movlw	(1 << TXEN) | (1 << BRGH)
	movwf	TXSTA

#if 0
#if INTERNAL_OSC
  	movlw	51		; 9600 baud @8MHz
#else
  	movlw	129		; 9600 baud @20MHz
#endif
#else
#if INTERNAL_OSC
  	movlw	25		; 19200 baud @8MHz
#else
  	movlw	64		; 19200 baud @20MHz
#endif
#endif
	movwf	SPBRG
	SWITCHBANK_1_0

	; Configure Receive Status and Control Register
        movf    RCREG, W
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	; initialize ringbuffers
	clrf	UART_RX_BUFFER_TAIL
	clrf	UART_RX_BUFFER_HEAD
	clrf	UART_RX_BUFFER_CTR

	clrf	UART_TX_BUFFER_TAIL
	clrf	UART_TX_BUFFER_HEAD
	clrf	UART_TX_BUFFER_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_SpeakJetInit
;;  DESCRIPTION: initialises the SpeakJet baudrate by using the detection
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
UART_SpeakJetInit
	;; reset SpeakJet, enter baud rate configure mode
	bcf	SPEAKJET_RST_N_PORT, SPEAKJET_RST_N_PIN	; low active!
	bsf	SPEAKJET_M0_PORT, SPEAKJET_M0_PIN	; enter demomode
	bcf	SPEAKJET_M1_PORT, SPEAKJET_M1_PIN	; baud rate configure mode

	;; initial delay
	call	UART_Init_Delay

	;; release reset
	call	UART_Init_Delay
	bsf	SPEAKJET_RST_N_PORT, SPEAKJET_RST_N_PIN	; low active!

	;; we need some (!!!) delay here before SpeakJet is ready!
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay

	;; (we hear a "sonar ping" sound now!)

	;; continue with baudrate detection: send 0x55 to SpeakJet
	movlw	0x55		; write to TXREG directly - should only be done during initialisation phase!
	movwf	TXREG

	;; wait until byte has been sent
	SWITCHBANK_0_1
UART_SpeakJetInit_TxPoll
	btfss	TXSTA, TRMT
	goto	UART_SpeakJetInit_TxPoll
	SWITCHBANK_1_0

	;; wait > 2 mS (just an assumption, it could also work without this delay)
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay
	call	UART_Init_Delay

	;; go back to normal mode
	bcf	SPEAKJET_M0_PORT, SPEAKJET_M0_PIN	; suspend demomode
	bsf	SPEAKJET_M1_PORT, SPEAKJET_M1_PIN	; leave baud rate configure mode

	return

;; subroutine which causes a delay of ca. 150 uS @ 20 MHz
UART_Init_Delay
	clrwdt
	clrf	UART_DELAY_CTR
UART_Init_Delay_Loop
	decfsz	UART_DELAY_CTR, F
	goto	UART_Init_Delay_Loop
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_RxBufferUsed
;;  DESCRIPTION: returns number of used bytes in receive buffer
;;  IN:	  -
;;  OUT:  number of used bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; branch if something has been received via UART
;;	call	UART_RxBufferUsed
;;	bnz	ReceivedSomething
;; 
;; --------------------------------------------------------------------------
UART_RxBufferUsed
	movf	UART_RX_BUFFER_CTR, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_RxBufferFree
;;  DESCRIPTION: returns number of free bytes in receive buffer
;;  IN:	  -
;;  OUT:  number of free bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; put a UART byte into the Rx Buffer if it isn't full
;;	call	UART_RxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (UART Clock)
;;	call	UART_RxBufferPut
;;
;; --------------------------------------------------------------------------
UART_RxBufferFree
	movf	UART_RX_BUFFER_CTR, W
	sublw	UART_RX_BUFFER_SIZE-1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_RxBufferPut
;;  DESCRIPTION: puts a byte onto the receive buffer
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; put a UART byte into the Rx Buffer if it isn't full
;;	movlw	0xf8	; (UART Clock)
;;	call	UART_RxBufferPut
;;
;; --------------------------------------------------------------------------
UART_RxBufferPut
	movwf	UART_RX_BUFFER_TMP		; save byte which should be sent

	incf	UART_RX_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	UART_RX_BUFFER_SIZE
	skpz
	xorlw	UART_RX_BUFFER_SIZE		; XOR back to old value
	movwf	FSR
	xorwf	UART_RX_BUFFER_HEAD, W
	skpnz
	return					; avoid overrun!

	movf	FSR, W				; prepare buffer pointer
	movwf	UART_RX_BUFFER_TAIL
	addlw	LOW(UART_RX_BUFFER)
	movwf	FSR
	movf	UART_RX_BUFFER_TMP, W		; push byte onto buffer
	SWITCH_FSR_UPPER
	movwf	INDF
	SWITCH_FSR_LOWER
	incf	UART_RX_BUFFER_CTR, F

	;; trigger LED
	movlw	UART_LEDS_RXTX_DELAY
	movwf	LED_RX_CTR

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_RxBufferGet
;;  DESCRIPTION: gets a byte from the receive buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; get a UART byte from the Rx Buffer if something has been received
;;	call	UART_RxBufferUsed
;;	bz	NothingReceived
;;	call	UART_RxBufferGet
;;
;; --------------------------------------------------------------------------
UART_RxBufferGet
	incf	UART_RX_BUFFER_HEAD, F
	movf	UART_RX_BUFFER_HEAD, W
	xorlw	UART_RX_BUFFER_SIZE
	skpnz
	clrf	UART_RX_BUFFER_HEAD

	movf	UART_RX_BUFFER_HEAD, W
	addlw	LOW(UART_RX_BUFFER)
	movwf	FSR
	SWITCH_FSR_UPPER
	movf	INDF, W
	SWITCH_FSR_LOWER
	decf	UART_RX_BUFFER_CTR, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxBufferUsed
;;  DESCRIPTION: returns number of used bytes in buffer
;;  IN:	  -
;;  OUT:  number of used bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; branch if something has been put into the Tx Buffer
;;	call	UART_TxBufferUsed
;;	bz	ReceivedSomething
;; 
;; --------------------------------------------------------------------------
UART_TxBufferUsed
	movf	UART_TX_BUFFER_CTR, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxBufferFree
;;  DESCRIPTION: returns number of free bytes in transmit buffer
;;  IN:	  -
;;  OUT:  number of free bytes in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; put a UART byte into the Tx Buffer if it isn't full
;;	call	UART_TxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (UART Clock)
;;	call	UART_TxBufferPut
;;
;; --------------------------------------------------------------------------
UART_TxBufferFree
	movf	UART_TX_BUFFER_CTR, W
	sublw	UART_TX_BUFFER_SIZE-1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxBufferPut
;;  DESCRIPTION: puts a byte onto the transmit buffer.
;;  If Tx buffer is full, function will be suspended until one byte has been
;;  transmitted via UART
;;  IN:	  byte in WREG
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; put a UART byte into the Tx Buffer if it isn't full
;;	movlw	0xf8	; (UART Clock)
;;	call	UART_TxBufferPut
;;
;; --------------------------------------------------------------------------
UART_TxBufferPut
	movwf	UART_TX_BUFFER_TMP		; save byte which should be sent

	incf	UART_TX_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	UART_TX_BUFFER_SIZE
	skpz
	xorlw	UART_TX_BUFFER_SIZE		; XOR back to old value
	movwf	FSR
	xorwf	UART_TX_BUFFER_HEAD, W
	bnz	UART_TxBufferPut_Ok

	;; repeat until buffer is free again
	IRQ_ENABLE		; (we must ensure that interrupts are enabled!)
	movf	UART_TX_BUFFER_TMP, W
	goto	UART_TxBufferPut

UART_TxBufferPut_Ok
	IRQ_DISABLE				; disable interrupts to avoid a malfunction when a Tx IRQ happens here
	movf	FSR, W				; prepare buffer pointer
	movwf	UART_TX_BUFFER_TAIL
	addlw	LOW(UART_TX_BUFFER)
	movwf	FSR
	movf	UART_TX_BUFFER_TMP, W		; push byte onto buffer
	SWITCH_FSR_UPPER
	movwf	INDF
	SWITCH_FSR_LOWER
	incf	UART_TX_BUFFER_CTR, F

	SWITCHBANK_0_1
	bsf	PIE1, TXIE			; (re-)enable transmit IRQ
	SWITCHBANK_1_0

	IRQ_ENABLE				; enable interrupts again

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxBufferGet
;;  DESCRIPTION: gets a byte from the transmit buffer
;;  IN:	  -
;;  OUT:  byte in WREG
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; get a UART byte from the Tx Buffer if new byte is available
;;	call	UART_TxBufferUsed
;;	bz	NoByteAvailable
;;	call	UART_TxBufferGet
;;
;; --------------------------------------------------------------------------
UART_TxBufferGet
	incf	UART_TX_BUFFER_HEAD, F
	movf	UART_TX_BUFFER_HEAD, W
	xorlw	UART_TX_BUFFER_SIZE
	skpnz
	clrf	UART_TX_BUFFER_HEAD

	movf	UART_TX_BUFFER_HEAD, W
	addlw	LOW(UART_TX_BUFFER)
	movwf	FSR

	movlw	UART_LEDS_RXTX_DELAY	; trigger LED
	movwf	LED_TX_CTR

	SWITCH_FSR_UPPER
	movf	INDF, W
	SWITCH_FSR_LOWER
	decf	UART_TX_BUFFER_CTR, F
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxBufferFlush
;;  DESCRIPTION: waits until all UART bytes in the Tx buffer have been transmitted
;;  IN:	  -
;;  OUT:  -
;;  USES: FSR
;;  EXAMPLE:
;;
;;	;; wait until buffer is empty, thereafter send a bunch of new bytes
;;	call	UART_TxBufferFlush
;;	movlw	0xf0
;;	call	UART_TxBufferPut
;;	;;  ...
;;
;; --------------------------------------------------------------------------
UART_TxBufferFlush
	clrwdt
UART_TxBufferFlush_WaitTx
	call	UART_TxBufferUsed
	bnz	UART_TxBufferFlush_WaitTx
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_TxSendDirect
;;  DESCRIPTION: sends unbuffered byte - only for debugging!
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
UART_TxSendDirect
	SWITCHBANK_0_1
UART_TxSendDirect_Poll
	btfss	TXSTA, TRMT
	goto	UART_TxSendDirect_Poll
	SWITCHBANK_1_0
	movwf	TXREG
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: UART_LEDS_Handler
;;  DESCRIPTION: this function should be called periodically from a timer
;;  or the mainloop. It decrements the Rx/Tx counters and sets the LEDs 
;;  depending on the counter values
;; --------------------------------------------------------------------------
UART_LEDS_Handler

UART_LEDS_Handler_Rx
	movf	LED_RX_CTR, W
	bz	UART_LEDS_Handler_RxOff
UART_LEDS_Handler_RxOn
	decf	LED_RX_CTR, F
#if LED_RX_PORT
	bcf	LED_RX_PORT, LED_RX_PIN
#endif
	goto	UART_LEDS_Handler_Rx_End
UART_LEDS_Handler_RxOff
#if LED_RX_PORT
	bsf	LED_RX_PORT, LED_RX_PIN
#endif
UART_LEDS_Handler_Rx_End

UART_LEDS_Handler_Tx
	movf	LED_TX_CTR, W
	bz	UART_LEDS_Handler_TxOff
UART_LEDS_Handler_TxOn
	decf	LED_TX_CTR, F
#if LED_TX_PORT
	bcf	LED_TX_PORT, LED_TX_PIN
#endif
	goto	UART_LEDS_Handler_Tx_End
UART_LEDS_Handler_TxOff
#if LED_TX_PORT
	bsf	LED_TX_PORT, LED_TX_PIN
#endif
UART_LEDS_Handler_Tx_End

	return

; ==========================================================================
	END
