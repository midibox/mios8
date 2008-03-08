; $Id$
;
;  Source Code of MBHP_USB_PIC
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
#include "usb_defs.inc"

; ==========================================================================

	;; import lables
	extern	MIDI_Init
	extern	IIC_MIDI_Init
	extern	USBDRV_Init
	extern	USBDRV_Tick
	extern	USBCLS_USBMIDITick

	extern	MIDI_TxBufferGet
	extern	MIDI_RxBufferPut
	extern	MIDI_TX_BUFFER_HEAD
	extern	MIDI_TX_BUFFER_TAIL

; ==========================================================================
accessram		udata_acs
IRQ_W_TEMP		res	1
IRQ_STATUS_TEMP		res	1
IRQ_BSR_TEMP		res	1
FSR1L_TEMP		res	1
FSR1H_TEMP		res	1

; ==========================================================================
MAIN	code
; ==========================================================================

;; ---[ jump vectors to the program range for the case that no USB bootloader is used ]---
ROOT
	org	0x0000
	goto	BOOT
	goto	BOOT
	org	0x0008
	goto	IRQ
	goto	IRQ

;; ---[ boot vector ]---
BOOT
	org	0x0800
	goto	START
	goto	START

;; ---[ irq vector ]---
IRQ
	org	0x0808
	movff	WREG, IRQ_W_TEMP		; workaround for IRQ erratum
	movff	STATUS, IRQ_STATUS_TEMP
	movff	BSR, IRQ_BSR_TEMP
	movff	FSR1L, FSR1L_TEMP
	movff	FSR1H, FSR1H_TEMP

IRQ_Handler
	;; which IRQ received?
	btfsc	PIR1, RCIF	; UART Receive IRQ?
	rgoto	IRQ_UART_Rx

	btfss	PIE1, TXIE
	rgoto	IRQ_Handler_NoTxIRQ
IRQ_Handler_TxIRQ
	btfsc	PIR1, TXIF	; UART Transmit IRQ?
	rgoto	IRQ_UART_Tx
IRQ_Handler_NoTxIRQ

IRQ_Handler_End
	movff	FSR1L_TEMP, FSR1L
	movff	FSR1H_TEMP, FSR1H

	movff	IRQ_BSR_TEMP, BSR
	movff	IRQ_W_TEMP, WREG
	movff	IRQ_STATUS_TEMP, STATUS
        retfie

; =========
IRQ_UART_Tx
	movf	MIDI_TX_BUFFER_HEAD, W		; check if new byte has to be sent
	xorwf	MIDI_TX_BUFFER_TAIL, W
	bnz	IRQ_UART_Tx_NextByte

	bcf	PIE1, TXIE			; prevent interrupts until this flag is set again by MIDI_TxBufferPut

IRQ_UART_Tx_End	
	rgoto	IRQ_Handler			; return to interrupt handler

IRQ_UART_Tx_NextByte
	call	MIDI_TxBufferGet		; get byte from Tx buffer
	movwf	TXREG				; send it
	rgoto	IRQ_Handler			; return to interrupt handler

; =========
IRQ_UART_Rx
#if 1
	btfsc	RCSTA, FERR
	btg	LATB, 0		; for debugging
#endif

	;; overrun error (RCSTA, OERR) checked in mainloop!

	movf	RCREG, W			; get received byte
	bcf	PIR1, RCIF			; clear interrupt request flag

	btfss	RCSTA, FERR			; skip if frame error!
	call	MIDI_RxBufferPut		; store received byte in buffer
	rgoto	IRQ_Handler


; ==========================================================================
START
INIT
	clrwdt

	;; ------------------------------------------------------------------
	;;  initialize the SRAM
	;; ------------------------------------------------------------------
	lfsr	FSR0, 0x000
INIT_ClearRAMLoop
	clrf	POSTINC0
	movlw	0x08
	cpfseq	FSR0H, ACCESS
	rgoto	INIT_ClearRAMLoop

	;; MBHP_CORE like initialization
	bsf	INTCON2, NOT_RBPU	; Enable Pull-Ups of PortB
	clrf	PORTA
	clrf	PORTB
	movlw	0x40			; (Tx Pin = 1)
	movwf	PORTC
	clrf	PORTD
	clrf	PORTE

	movlw	0xef			; all PORTA pins beside if RA4 are tristate
	movwf	TRISA
	clrf	TRISB			; PORTB driver enabled by default (display data port)
	movlw	0x80			; Rx Pin tristate, all other outputs
	movwf	TRISC
	movlw	0x02			; DIN pin tristate, all other outputs
	movwf	TRISD
	movlw	0x07			; all PORTE pins are tristate
	movwf	TRISE

	movlw	0x0f			; PortA and E: All Pins are analog pins
	movwf	ADCON1			; (done again in IIC_MIDI_Init)

	;; init MIDI driver
	call	MIDI_Init

	;; init IIC MIDI driver
	call	IIC_MIDI_Init

	;; init USB driver
	call	USBDRV_Init

	; enable maskable IRQs
	bsf	INTCON, PEIE
	bsf	INTCON, GIE


MAINLOOP
	clrwdt

	;; call USB handlers
MAINLOOP_USB
	call	USBDRV_Tick
	call	USBCLS_USBMIDITick

	;; check for MIDI receive buffer overrun
MAINLOOP_UART
	btfss	RCSTA, OERR
	rgoto	MAINLOOP_UART_NoOERR
MAINLOOP_UART_OERR
#if 1
	btg	LATB, 1		; for debugging
#endif
	bcf	RCSTA, CREN	; re-enable receiver
	movf	RCREG, W
	bsf	RCSTA, CREN
MAINLOOP_UART_NoOERR
	goto	MAINLOOP
	
; ==========================================================================
	END
