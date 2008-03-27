; $Id$
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
	movf	PCLATH, W	; store pointer to both condition tables
	iorlw	0x07
	movwf	PCLATH

	SWITCHBANK_0_1
	movlw	0x7f		; Prescaler for Watchdog, Max Prescaling, Enable PullUps on PortB, 
	movwf	OPTION_REG	; Interrupt on falling Edge @ RA4, Timer0 Enabled

#ifdef __16F88
	movlw	0x72		; select 8 MHz
	movwf	OSCCON
Init_WaitOscStable		; wait until oscillator runs stable
	GOTO_IFCLR OSCCON, IOFS, Init_WaitOscStable
#endif
	SWITCHBANK_1_0

	call	Init_Ports
	call	Init_USART
	call	Init_SUART

	;; init LED counters
	clrf	LED_CTR_RX0
	clrf	LED_CTR_RX1
	clrf	LED_CTR_TX

#if 0
	CALL_IFCLR STATUS, NOT_TO, ACCESS, Malfunction_Message ; On WDT Reset print Malfunction Message
#endif

	call	Init_IRQ

	return

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  Init Routine for Ports
;; --------------------------------------------------------------------------
Init_Ports
	movlw	PORTA_VALUE
	movwf	PORTA
	movlw	PORTB_VALUE
	movwf	PORTB
#ifndef __16F88
	movlw	PORTC_VALUE
	movwf	PORTC
	movlw	PORTD_VALUE
	movwf	PORTD
	movlw	PORTE_VALUE
	movwf	PORTE
#endif
	
	SWITCHBANK_0_1

	movlw	TRISA_VALUE
	movwf	TRISA
	movlw	TRISB_VALUE
	movwf	TRISB
	
#ifdef __16F88
	clrf	ANSEL			; disable analog inputs
	movlw	0x07			; disable comparator inputs
	movwf	CMCON
#else
	movlw	TRISC_VALUE
	movwf	TRISC
	movlw	TRISD_VALUE
	movwf	TRISD
	movlw	TRISE_VALUE
	movwf	TRISE

	movlw	0x07			; PortA and E: All Pins are digital pins
	movwf	ADCON1

#endif

	SWITCHBANK_1_0

	;; turn on power LED
#if LED_PWR_PORT
	bcf	LED_PWR_PORT, LED_PWR_PIN
#endif

	return

;; ==========================================================================
;;  Initialization Routine for IRQ Handler
;; ==========================================================================
Init_IRQ
	SWITCHBANK_0_1

        movlw   (1 << RCIE)		; enable UART Receive IRQ
        movwf   PIE1
        clrf    PIE2                    ; clear all other IRQs

	SWITCHBANK_1_0

        bcf     RCSTA, CREN
        movf    RCREG, W
        bsf     RCSTA, CREN
                
        ; enable maskable IRQs
        movlw   (1 << GIE) | (1 << PEIE) | (1 << T0IE)
        movwf   INTCON

        return

;; --------------------------------------------------------------------------
;;  Init Routine for USART
;; --------------------------------------------------------------------------
Init_USART
	SWITCHBANK_0_1
	movlw	(1 << TXEN)
	movwf	TXSTA
#ifdef __16F88
  	movlw	0x03		; 31250 baud @8MHz
#else
  	movlw	0x09		; 31250 baud @20MHz
#endif
	movwf	SPBRG
	SWITCHBANK_1_0

	; Configure Receive Status and Control Register
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	;; init ring buffers
	movlw	RINGBUFFER1
	movwf	RINGBUFFER1_A
	movwf	RINGBUFFER1_B

	return

	
;; --------------------------------------------------------------------------
;;  Init Routine for Software USART
;; --------------------------------------------------------------------------
Init_SUART
	clrf	SUART_CTRL
	clrf	SUART_IN_STATE
	clrf	SUART_IN_B
	clrf	SUART_IN
	
	movlw	0xff		; preload for TMR0 (interrupt on falling edge)
	movwf	TMR0
	bcf	INTCON, T0IF	; clear Timer0 Flag
	
	movlw	RINGBUFFER0
	movwf	RINGBUFFER0_A
	movwf	RINGBUFFER0_B

	return

;; --------------------------------------------------------------------------
;;  Init Routine for MIDI
;; --------------------------------------------------------------------------
Init_MIDI
	clrf	MIDI0_RUNSTATUS
	clrf	MIDI1_RUNSTATUS
	clrf	MIDIO_RUNSTATUS
	return
