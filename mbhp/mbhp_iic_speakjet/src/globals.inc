; $Id$
;
; Global Settings
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; switch between internal 8 MHz clock and external 20 MHz clock
;; Note: for UART<->IIC transmission, 8 MHz is too slow!
#define INTERNAL_OSC	0

; ==========================================================================

; Number of update cycles (* 1mS) the LEDs will stay active on a Rx/Tx event
#define UART_LEDS_RXTX_DELAY	30

; ==========================================================================
 
PORTA_VALUE	EQU	b'00000011'	; Rx/Tx LED off
TRISA_VALUE	EQU	b'11100000'	; Setup Value for Tristate Drivers of PortA
PORTB_VALUE	EQU	b'00100001'	; Pwr LED off, TX Pin = 1
TRISB_VALUE	EQU	b'11010110'	; Setup Value for Tristate Drivers of PortB

RI_N_PORT	EQU	PORTB
RI_N_PIN	EQU	3

ID0_PORT	EQU	PORTB
ID0_PIN		EQU	6
ID1_PORT	EQU	PORTB
ID1_PIN		EQU	7

;
; note: LED outputs won't be set if port is specified with 0
;
LED_RX_PORT	EQU	PORTA
LED_RX_PIN	EQU	0
LED_TX_PORT	EQU	PORTA
LED_TX_PIN	EQU	1
LED_PWR_PORT	EQU	PORTB
LED_PWR_PIN	EQU	0

; SpeakJet control lines
SPEAKJET_RST_N_PORT	EQU	PORTA
SPEAKJET_RST_N_PIN	EQU	2
SPEAKJET_M1_PORT	EQU	PORTA
SPEAKJET_M1_PIN		EQU	3
SPEAKJET_M0_PORT	EQU	PORTA
SPEAKJET_M0_PIN		EQU	4
SPEAKJET_BHFULL_PORT	EQU	PORTA
SPEAKJET_BHFULL_PIN	EQU	5
