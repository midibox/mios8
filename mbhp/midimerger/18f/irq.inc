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

#if MIOS_BSL_FLAG
	org	0x0408
#else
	org	0x0008
#endif

	;; save FSR2[LH]
	movff	FSR2L, SAVED_FSR2L
	movff	FSR2H, SAVED_FSR2H

IRQCheck
	; which IRQ received?
	GOTO_IFSET INTCON, T0IF, ACCESS, IRQ_Timer0		; Timer0 IRQ?
	GOTO_IFSET PIR1, RCIF, ACCESS, IRQ_UART_Receive	; UART Receive IRQ?
IRQEnd

	;; restore FSR2[LH]
	movff	SAVED_FSR2L, FSR2L
	movff	SAVED_FSR2H, FSR2H

        retfie	FAST

;; ==========================================================================
;;  IRQ Handler for Timer0, called on a falling edge on RA4 (Trigger Mode) OR
;;  every 32 us when MIDI Event is received by SUART (Receiver Mode)
;; ==========================================================================
IRQ_Timer0
	;; branch depending on timer status
	GOTO_IFSET SUART_CTRL, SUART_CTRL_RECEIVING, ACCESS, IRQ_Timer0_Receiver

IRQ_Timer0_Trigger
	GOTO_IFSET PORT_SUART_IN, PIN_SUART_IN, ACCESS, IRQ_Timer0_End

	;; ok, we've regognized the start bit, now disable any further external IRQ and enable internal clock
	movlw	TMR0_RELOAD-50	; reload Timer0 (sub 50 to shift the sample point more to the middle of the pulses
	movwf	TMR0L
	bcf	INTCON, T0IF	; clear Timer0 Flag

	bcf	T0CON, PSA	; enable prescaler
	bcf	T0CON, T0CS	; Route Internal Clock to Timer0
	clrf	SUART_IN_STATE	; clear SUART Counter
	bsf	SUART_CTRL, SUART_CTRL_RECEIVING

	goto	IRQ_Timer0_End



IRQ_Timer0_Receiver
	movlw	TMR0_RELOAD	; reload Timer0
	addwf	TMR0L, F
	bcf	INTCON, T0IF	; clear Timer0 Flag	

#if DEBUG_MODE
	bsf	LED_PWR_PORT, LED_PWR_PIN
#endif

	rrf	SUART_IN, F	; shift right IN buffer
	bcf	SUART_IN, 7	; copy received bit to Bit 7 of SUART buffer
	btfsc	PORT_SUART_IN, PIN_SUART_IN
	bsf	SUART_IN, 7

	incf	SUART_IN_STATE, F	; increment SUART counter, finish routine if 8 bits received
	GOTO_IFCLR SUART_IN_STATE, 3, ACCESS, IRQ_Timer0_End

	;; 8 bits received - voila, a new byte

	;; disable Timer0 IRQ, enable External IRQ again
	bsf	T0CON, T0CS	; Route RA.4 to Timer0
	bsf	T0CON, PSA	; disable prescaler
	movlw	0xff		; reload TMR0
	movwf	TMR0L
	bcf	INTCON, T0IF	; clear Timer0 Flag	
	bcf	SUART_CTRL, SUART_CTRL_RECEIVING

	;; put the byte into the IN buffer
	
	incf	RINGBUFFER0_A, F
	movf	RINGBUFFER0_A, W
	xorlw	(RINGBUFFER0_END+1) & 0xff
	bnz	IRQ_Timer0_NoRingAReset
	movlw	(RINGBUFFER0 & 0xff)
	movwf	RINGBUFFER0_A
IRQ_Timer0_NoRingAReset
	lfsr	FSR2, RINGBUFFER0
	movf	RINGBUFFER0_A, W
	addwf	FSR2L, F
	movlw	0x00
	addwfc	FSR2H, F
		
	movf	SUART_IN, W
	movwf	INDF2

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
	xorlw	(RINGBUFFER1_END+1) & 0xff
	bnz	IRQ_UART_NoRingAReset
	movlw	(RINGBUFFER1 & 0xff)
	movwf	RINGBUFFER1_A
IRQ_UART_NoRingAReset
	lfsr	FSR2, RINGBUFFER1
	movf	RINGBUFFER1_A, W
	addwf	FSR2L, F
	movlw	0x00
	addwfc	FSR2H, F
		
	movf	RCREG, W
	movwf	INDF2

IRQ_UART_End
	goto	IRQCheck
