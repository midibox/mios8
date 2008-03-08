; $Id$
;
;  Source Code of IIC UART Bridge MBHP_IIC_SPEAKJET
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

;; ==========================================================================
;;  Chip Configuration
;; ==========================================================================

#if INTERNAL_OSC
	__CONFIG    _CONFIG1, _CP_OFF & _CCP1_RB0 & _DEBUG_OFF & _WRT_PROTECT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _MCLR_OFF & _PWRTE_OFF & _WDT_ON & _INTRC_IO
#else
	__CONFIG    _CONFIG1, _CP_OFF & _CCP1_RB0 & _DEBUG_OFF & _WRT_PROTECT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _MCLR_OFF & _PWRTE_OFF & _WDT_ON & _HS_OSC
#endif

	__CONFIG    _CONFIG2, _IESO_OFF & _FCMEN_OFF

; ==========================================================================

	;; import lables
	extern	IIC_Init
	extern	IIC_Init_GetAddress
	extern	IIC_Handler

	extern	UART_Init
	extern	UART_SpeakJetInit
	extern	UART_TxBufferGet
	extern	UART_TxBufferPut
	extern	UART_RxBufferGet
	extern	UART_RxBufferPut
	extern	UART_RxBufferUsed
	extern	UART_LEDS_Handler
	extern	UART_TX_BUFFER_HEAD
	extern	UART_TX_BUFFER_TAIL

; ==========================================================================
shared_ram		udata_shr
W_TMP			res	1
STATUS_TMP		res	1
FSR_TMP			res	1

; ==========================================================================
VECTORS	code
; ==========================================================================

;; ---[ boot vector ]---
BOOT
	org	0x0000
	goto	START

;; ---[ irq vector ]---
IRQ
	org	0x0004
	goto	IRQ_Entry

; ==========================================================================
MAIN	code
; ==========================================================================
IRQ_Entry
	movwf	W_TMP
	swapf	STATUS, W
	clrf	STATUS		; switch to bank 0
	movwf	STATUS_TMP
	movf	FSR, W
	movwf	FSR_TMP

IRQ_Handler

	;; check for UART Receive IRQ
	btfss	PIR1, RCIF
	goto	IRQ_Handler_Rx_End
IRQ_Handler_Rx
	bcf	PIR1, RCIF		; clear interrupt flag
	movf	RCREG, W		; put received byte into buffer
	call	UART_RxBufferPut
IRQ_Handler_Rx_End

	;; check for UART Transmit IRQ
	SWITCHBANK_0_1
	btfss	PIE1, TXIE
	goto	IRQ_Handler_Tx_End
	SWITCHBANK_1_0
	btfss	PIR1, TXIF
	goto	IRQ_Handler_Tx_End
IRQ_Handler_Tx
	movf	UART_TX_BUFFER_HEAD, W	; check if new byte has to be sent
	xorwf	UART_TX_BUFFER_TAIL, W
	bnz	IRQ_Handler_Tx_NextByte
IRQ_Handler_Tx_NoNewByte
	SWITCHBANK_0_1
	bcf	PIE1, TXIE		; prevent interrupts until this flag is set again by UART_SendByte
	goto	IRQ_Handler_Tx_End
IRQ_Handler_Tx_NextByte
	call	UART_TxBufferGet	; get byte from Tx buffer
	movwf	TXREG			; send it
IRQ_Handler_Tx_End
	SWITCHBANK_1_0

IRQ_End
	movf	FSR_TMP, W
	movwf	FSR
	swapf	STATUS_TMP, W
	movwf	STATUS
	swapf	W_TMP, F
	swapf	W_TMP, W
	retfie
	

; ==========================================================================
START
INIT
	;; initialize ports
	movlw	PORTA_VALUE
	movwf	PORTA
	movlw	PORTB_VALUE
	movwf	PORTB

	SWITCHBANK_0_1
	movlw	TRISA_VALUE
	movwf	TRISA
	movlw	TRISB_VALUE
	movwf	TRISB

	clrf	ANSEL			; disable analog inputs
	movlw	0x07			; disable comparator inputs
	movwf	CMCON
	SWITCHBANK_1_0

	SWITCHBANK_0_1
	movlw	0xff		; Prescaler for Watchdog, Max Prescaling, Enable PullUps on PortB, 
	movwf	OPTION_REG	; Interrupt on falling Edge @ RA4, Timer0 Enabled

#if INTERNAL_OSC
	movlw	0x70		; select 8 MHz
	movwf	OSCCON
Init_WaitOscStable		; wait until oscillator runs stable
	btfss	OSCCON, IOFS
	goto	Init_WaitOscStable
#endif
	SWITCHBANK_1_0

	movf	PCLATH, W	; store pointer to both condition tables
	iorlw	0x07
	movwf	PCLATH

	;; initialize UART
	call	UART_Init

	;; initialize SpeakJet baudrate
	call	UART_SpeakJetInit

	;; initialize IIC
	call	IIC_Init

	;; initialize timer1
        movlw   (1 << TMR1ON)           ; turn on Timer1
        movwf   T1CON

	;; initialize IRQs
	SWITCHBANK_0_1
        movlw   (1 << RCIE)			; enable UART Receive
        movwf   PIE1				; Tx IRQ will be enabled dynamically
        clrf    PIE2				; clear all other IRQs
	SWITCHBANK_1_0
        bcf     RCSTA, CREN
        movf    RCREG, W
        bsf     RCSTA, CREN
                
        ; enable maskable IRQs
        movlw   (1 << GIE) | (1 << PEIE)
        movwf   INTCON

	;; turn on power LED
#if LED_PWR_PORT
	bcf	LED_PWR_PORT, LED_PWR_PIN
#endif


MAINLOOP
	;; feed the watchdog
	clrwdt

	;; call UART LED handler on timer1 interrupt
	btfss	PIR1, TMR1IF
	goto	MAINLOOP_NoTimer1Event
MAINLOOP_Timer1Event
	bcf	PIR1, TMR1IF
#if INTERNAL_OSC
	movlw	0xf8		; every 1.024 mS (@8 MHz)
#else
	movlw	0xec		; every 1.024 mS (@20 MHz)
#endif
	addwf	TMR1H, F
	call	UART_LEDS_Handler
MAINLOOP_NoTimer1Event

	;; call IIC handler in iic.inc on IIC interrupt
MAINLOOP_IIC_Loop
	IRQ_DISABLE		; disable interrupts to ensure that IIC routine won't be disturbed!
	btfss	PIR1, SSPIF
	goto	MAINLOOP_NoIICEvent
MAINLOOP_IICEvent
	bcf	PIR1, SSPIF	; clear SSP flag
	call	IIC_Handler	; call IIC handler in iic.inc
MAINLOOP_NoIICEvent
	IRQ_ENABLE

	;; loop until stop bit has been set
	;; this is to ensure best performance (no sporatic SSPOVs -> which lead to retries) on transmits
	SWITCHBANK_0_1
	movf	SSPSTAT, W
	SWITCHBANK_1_0
	andlw	1 << P
	bz	MAINLOOP_IIC_Loop

	;; determine device address (allow to change it on-the-fly without device reset)
	call	IIC_Init_GetAddress

	;; forward incoming data at UART Rx to Tx (simple merging of IIC and UART Rx data)
	call	UART_RxBufferUsed
	bz	MAINLOOP_NoRxData
MAINLOOP_RxData
	call	UART_RxBufferGet
	call	UART_TxBufferPut
MAINLOOP_NoRxData

	goto	MAINLOOP

;; ==========================================================================
	END
