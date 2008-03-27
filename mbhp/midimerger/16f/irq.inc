; $Id$
;
; IRQ Vector and Service Routines
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; ==========================================================================
;;  IRQ Vector
;; ==========================================================================

	ORG	0x0004

IRQ
	movwf	W_TMP
	swapf	STATUS, W
	clrf	STATUS		; switch to bank 0
	movwf	STATUS_TMP
	movf	FSR, W
	movwf	FSR_TMP

IRQCheck
	; which IRQ received?
	GOTO_IFSET INTCON, T0IF, IRQ_Timer0		; Timer0 IRQ?
	GOTO_IFSET PIR1, RCIF, IRQ_UART_Receive		; UART Receive IRQ?
IRQEnd
	movf	FSR_TMP, W
	movwf	FSR
	swapf	STATUS_TMP, W
	movwf	STATUS
	swapf	W_TMP, F
	swapf	W_TMP, W
	retfie

;; ==========================================================================
;;  IRQ Handler for Timer0, called on a falling edge on RA4 (Trigger Mode) OR
;;  every 32 us when MIDI Event is received by SUART (Receiver Mode)
;; ==========================================================================
IRQ_Timer0
	;; branch depending on timer status
	GOTO_IFSET SUART_CTRL, SUART_CTRL_RECEIVING, IRQ_Timer0_Receiver

IRQ_Timer0_Trigger
	GOTO_IFSET PORT_SUART_IN, PIN_SUART_IN, IRQ_Timer0_End

	;; ok, we've regognized the start bit, now disable any further external IRQ and enable internal clock
	movlw	TMR0_RELOAD+TMR0_SAMPLE_OFFSET	; reload Timer0 (add offset to shift the sample point more to the middle of the pulses)
	movwf	TMR0
	bcf	INTCON, T0IF	; clear Timer0 Flag

	SWITCHBANK_0_1
	bcf	OPTION_REG, T0CS; Route Internal Clock to Timer0
	SWITCHBANK_1_0
	clrf	SUART_IN_STATE	; clear SUART Counter
	bsf	SUART_CTRL, SUART_CTRL_RECEIVING

	goto	IRQ_Timer0_End



IRQ_Timer0_Receiver
	movlw	TMR0_RELOAD	; reload Timer0
	addwf	TMR0, F
	bcf	INTCON, T0IF	; clear Timer0 Flag	

#if DEBUG_MODE
 	bsf	LED_PWR_PORT, LED_PWR_PIN
#endif
	
	rrf	SUART_IN, F	; shift right IN buffer
	bcf	SUART_IN, 7	; copy received bit to Bit 7 of SUART buffer
	btfsc	PORT_SUART_IN, PIN_SUART_IN
	bsf	SUART_IN, 7

	incf	SUART_IN_STATE, F	; increment SUART counter, finish routine if 8 bits received
	GOTO_IFCLR SUART_IN_STATE, 3, IRQ_Timer0_End

	;; 8 bits received - voila, a new byte

	;; disable Timer0 IRQ, enable External IRQ again
	SWITCHBANK_0_1
	bsf	OPTION_REG, T0CS; Route RA.4 to Timer0
	SWITCHBANK_1_0
	movlw	0xff		; reload TMR0
	movwf	TMR0
	bcf	INTCON, T0IF	; clear Timer0 Flag	
	bcf	SUART_CTRL, SUART_CTRL_RECEIVING

	;; put the byte into the IN buffer
	incf	RINGBUFFER0_A, F
	movf	RINGBUFFER0_A, W
	xorlw	RINGBUFFER0_END+1
	bnz	IRQ_Timer0_NoRingAReset
	movlw	RINGBUFFER0
	movwf	RINGBUFFER0_A
IRQ_Timer0_NoRingAReset
	movf	RINGBUFFER0_A, W
	movwf	FSR
	movf	SUART_IN, W
	movwf	INDF

IRQ_Timer0_End
#if DEBUG_MODE
	bcf	LED_PWR_PORT, LED_PWR_PIN
#endif
	goto	IRQCheck

	
;; ==========================================================================
;;  IRQ Handler for UART Receiver
;; ==========================================================================
IRQ_UART_Receive
	incf	RINGBUFFER1_A, F
	movf	RINGBUFFER1_A, W
	xorlw	RINGBUFFER1_END+1
	bnz	IRQ_UART_NoRingAReset
	movlw	RINGBUFFER1
	movwf	RINGBUFFER1_A
IRQ_UART_NoRingAReset
	movf	RINGBUFFER1_A, W
	movwf	FSR
	movf	RCREG, W
	movwf	INDF

IRQ_UART_End
	goto	IRQCheck
