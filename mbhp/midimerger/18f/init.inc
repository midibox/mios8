; $Id: $
;
; Init Routines
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

Init
	clrwdt

	;; ------------------------------------------------------------------
	;;  initialize the SRAM
	;; ------------------------------------------------------------------
	lfsr	FSR0, 0x000
Init_ClearRAMLoop
	clrf	POSTINC0
	movlw	0x06
	cpfseq	FSR0H, ACCESS
	rgoto Init_ClearRAMLoop

	;; ------------------------------------------------------------------
	;;  initialize the Ports
	;; ------------------------------------------------------------------
	bcf	INTCON2, NOT_RBPU	; Enable Pull-Ups of PortB

	movlw	PORTA_VALUE
	movwf	PORTA
	movlw	PORTB_VALUE
	movwf	PORTB
	movlw	PORTC_VALUE
	movwf	PORTC
	movlw	PORTD_VALUE
	movwf	PORTD
	movlw	PORTE_VALUE
	movwf	PORTE
	
	movlw	TRISA_VALUE
	movwf	TRISA
	movlw	TRISB_VALUE
	movwf	TRISB
	movlw	TRISC_VALUE
	movwf	TRISC
	movlw	TRISD_VALUE
	movwf	TRISD
	movlw	TRISE_VALUE
	movwf	TRISE

	movlw	0x07			; PortA and E: All Pins are digital pins
	movwf	ADCON1

	;; turn on power LED
#if LED_PWR_PORT
	bcf	LED_PWR_PORT, LED_PWR_PIN
#endif

	;; ------------------------------------------------------------------
	;;  initialize the USART
	;; ------------------------------------------------------------------
	movlw	(1 << TXEN)	; set baudrate:	31250
	movwf	TXSTA
  	movlw	2*0x0a-1
	movwf	SPBRG

	; Configure Receive Status and Control Register
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	movlw	RINGBUFFER1 & 0xff
	movwf	RINGBUFFER1_A
	movwf	RINGBUFFER1_B
	
	;; ------------------------------------------------------------------
	;;  initialize the SUSART
	;; ------------------------------------------------------------------
	clrf	SUART_CTRL
	clrf	SUART_IN_STATE
	clrf	SUART_IN_B
	clrf	SUART_IN

	movlw	(1 << TMR0ON) | (1 << T0CS) | (1 << T0SE) | (1 << T08BIT) | (1 << PSA) | 0x00	; internal clock source, 8 bit, prescaler 1:2
	movwf	T0CON
	movlw	0xff		; preload for TMR0 (interrupt on falling edge)
	movwf	TMR0L
	bcf	INTCON, T0IF	; clear Timer0 Flag

	movlw	RINGBUFFER0 & 0xff
	movwf	RINGBUFFER0_A
	movwf	RINGBUFFER0_B

	;; ------------------------------------------------------------------
	;;  initialize the running states of the MIDI handler (mainloop.asm)
	;; ------------------------------------------------------------------
	clrf	MIDI0_RUNSTATUS
	clrf	MIDI1_RUNSTATUS
	setf	MIDIO_RUNSTATUS


	;; ------------------------------------------------------------------
	;; init LED counters
	;; ------------------------------------------------------------------
	clrf	LED_CTR_RX0
	clrf	LED_CTR_RX1
	clrf	LED_CTR_TX


	;; ------------------------------------------------------------------
	;;  initialize interrupts
	;; ------------------------------------------------------------------

        movlw   (1 << RCIE)		; enable UART Receive IRQ
        movwf   PIE1
        clrf    PIE2                    ; clear all other IRQs

	;; make sure that USART Receive Buffer is empty
        bcf     RCSTA, CREN
        movf    RCREG, W
        bsf     RCSTA, CREN

        ; enable maskable IRQs
        movlw   (1 << GIE) | (1 << INT0E) | (1 << PEIE) | (1 << T0IE)
        movwf   INTCON

	return

;; ==========================================================================
