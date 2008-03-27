; $Id$
;
; Main Loop
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================

	;; Ringbuffer Handling Macro
	;; we are using this macro twice for both ringbuffers
	;; why no subroutine? Because we have enough code space and want
	;; to process the events as fast as possible

;; ==========================================================================

BUFFER_HANDLER MACRO RINGBUFFERx_A, RINGBUFFERx_B, RINGBUFFERx, RINGBUFFERx_END, MIDIx_RUNSTATUS, LED_RXx_PORT, LED_RXx_PIN, LED_CTR_RXx
	LOCAL	Buffer_LoopReset, Buffer_Loop, Buffer_Loop_TimeOut, Buffer_Loop_NoTimeOut, Buffer_ReceiveHandler
	LOCAL	Buffer_NoReset, Buffer_RunningData, Buffer_ForwardSysEx, Buffer_GotCommand, Buffer_GotSystemCommand, Buffer_GotCommandLTF8, Buffer_GotOneByteSyx
	LOCAL	Buffer_SendEnd, Buffer_Loop_End
	LOCAL	Buffer_Loop_Rx0LED, Buffer_Loop_NoRx0LED
	LOCAL	Buffer_Loop_Rx1LED, Buffer_Loop_NoRx1LED
	LOCAL	Buffer_Loop_TxLED, Buffer_Loop_NoTxLED

	clrf	MIDI_EXPBYTES			; clear number of expected bytes

Buffer_LoopReset

	clrf	MIDI_TIMEOUT_LOW		; load timeout counter
	clrf	MIDI_TIMEOUT_HIGH
	movlw	0xe0
	movwf	MIDI_TIMEOUT_HHIGH

Buffer_Loop
	clrwdt					; feed the watchdog

	;; handle LED counters
	movf	LED_CTR_RX0, W
	bz	Buffer_Loop_NoRx0LED
Buffer_Loop_Rx0LED
	decf	LED_CTR_RX0, F
	bnz	Buffer_Loop_NoRx0LED
	bsf	LED_RX0_PORT, LED_RX0_PIN	; turn LED off
Buffer_Loop_NoRx0LED
	
	movf	LED_CTR_RX1, W
	bz	Buffer_Loop_NoRx1LED
Buffer_Loop_Rx1LED
	decf	LED_CTR_RX1, F
	bnz	Buffer_Loop_NoRx1LED
	bsf	LED_RX1_PORT, LED_RX1_PIN	; turn LED off
Buffer_Loop_NoRx1LED
	
	movf	LED_CTR_TX, W
	bz	Buffer_Loop_NoTxLED
Buffer_Loop_TxLED
	decf	LED_CTR_TX, F
	bnz	Buffer_Loop_NoTxLED
	bsf	LED_TX_PORT, LED_TX_PIN		; turn LED off
Buffer_Loop_NoTxLED
	
	;; check for time out
	incf	MIDI_TIMEOUT_LOW, F
	skpnz
	incf	MIDI_TIMEOUT_HIGH, F
	skpnz
	incf	MIDI_TIMEOUT_HHIGH, F
	
	movf	MIDI_TIMEOUT_LOW, W
	bnz	Buffer_Loop_NoTimeOut
	movf	MIDI_TIMEOUT_HIGH, W
	bnz	Buffer_Loop_NoTimeOut
	movf	MIDI_TIMEOUT_HHIGH, W
	bz	Buffer_Loop_TimeOut	;; Time Out !!!

Buffer_Loop_NoTimeOut
	;; check for new/next byte in Ringbuffer
	movf	RINGBUFFERx_A, W
	xorwf	RINGBUFFERx_B, W
	bnz	Buffer_ReceiveHandler

	;; if no new byte: loop if waiting for complete MIDI event
	movf	MIDI_EXPBYTES, W
	bnz	Buffer_Loop

	goto	Buffer_Loop_End	; no new value and not waiting for complete event

Buffer_ReceiveHandler	
	;; get byte of UART Ringbuffer
	incf	RINGBUFFERx_B, F
	movf	RINGBUFFERx_B, W
	xorlw	RINGBUFFERx_END+1
	bnz	Buffer_NoReset
	movlw	RINGBUFFERx
	movwf	RINGBUFFERx_B	
Buffer_NoReset
	movf	RINGBUFFERx_B, W
	movwf	FSR

	;; turn on RX[01] LED and set delay counter to max value
	bcf	LED_RXx_PORT, LED_RXx_PIN
	movlw	0xff
	movwf	LED_CTR_RXx

	;; turn on TX LED and set delay counter to max value
	bcf	LED_TX_PORT, LED_TX_PIN
	movlw	0xff
	movwf	LED_CTR_TX

	;; analyse incoming byte
	GOTO_IFSET INDF, 7, Buffer_GotCommand

	movf	MIDIx_RUNSTATUS, W	; on sysex: don't take care for the expected bytes
	xorlw	0xf0
	bz	Buffer_ForwardSysEx

	movf	MIDI_EXPBYTES, W
	bnz	Buffer_RunningData

	movf	MIDIx_RUNSTATUS, W	; get number of expected bytes
	call	MIDI_GetCMax
	movwf	MIDI_EXPBYTES		; store it in MIDI_EXPBYTES

#if RUNNING_STATUS_OPTIMIZATION == 1
	;; check if MIDI Runstatus has to be send
	movf	MIDIx_RUNSTATUS, W
	xorwf	MIDIO_RUNSTATUS, W
	bz	Buffer_RunningData	; running status still active, don't send it again
#endif

	movf	MIDIx_RUNSTATUS, W	; new running status - store it
	movwf	MIDIO_RUNSTATUS
	call	_MIDI_SendByte		; and send it

Buffer_RunningData
	decf	MIDI_EXPBYTES, F
Buffer_ForwardSysEx
	movf	INDF, W			; send received byte
	call	_MIDI_SendByte
	goto	Buffer_SendEnd

Buffer_GotCommand
	;; Command >= 0xf8?
	movf	INDF, W
	andlw	0xf8
	xorlw	0xf8
	bnz	Buffer_GotCommandLTF8

Buffer_GotSystemCommand
	;; System >= F8: don't change Running Status and number of expected bytes!
	movf	INDF, W			; get received byte
	call	_MIDI_SendByte		; and send it out
	goto	Buffer_SendEnd

Buffer_GotCommandLTF8
	;; command less than F8: store running status, send byte, calc number of expected bytes
	movf	INDF, W			; get received byte
	movwf	MIDIx_RUNSTATUS
	movwf	MIDIO_RUNSTATUS
	call	_MIDI_SendByte		; and send it out

	movf	INDF, W			; get number of expected bytes
	call	MIDI_GetCMax
	movwf	MIDI_EXPBYTES		; store it in MIDI_EXPBYTES
	
Buffer_SendEnd

	;; reset Time Out counter and loop again
	goto	Buffer_LoopReset

	;; we got a Time Out?
Buffer_Loop_TimeOut
#if 1
	movlw	0xff
	call	_MIDI_SendByte
#endif
	
Buffer_Loop_End
	ENDM

;; ==========================================================================

MainLoop
	;; feed the watchdog
	clrwdt
	BUFFER_HANDLER RINGBUFFER0_A, RINGBUFFER0_B, RINGBUFFER0, RINGBUFFER0_END, MIDI0_RUNSTATUS, LED_RX0_PORT, LED_RX0_PIN, LED_CTR_RX0
 	BUFFER_HANDLER RINGBUFFER1_A, RINGBUFFER1_B, RINGBUFFER1, RINGBUFFER1_END, MIDI1_RUNSTATUS, LED_RX1_PORT, LED_RX1_PIN, LED_CTR_RX1

        ;; check for MIDI receive buffer overrun
MainLoop_UART
	GOTO_IFCLR RCSTA, OERR, MainLoop_UART_NoOERR
MainLoop_UART_OERR
        bcf     RCSTA, CREN     ; re-enable receiver
        movf    RCREG, W
        bsf     RCSTA, CREN
MainLoop_UART_NoOERR

	goto	MainLoop 
