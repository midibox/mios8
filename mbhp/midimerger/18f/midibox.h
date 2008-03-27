; $Id: $
;
; General Definitions
;
; ==========================================================================
;
;  Copyright 2003-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

 
;; ==========================================================================
;;  Assembler & PICstart directives
;; ==========================================================================

	LIST R=DEC
#include <p18f452.inc>

;; ==========================================================================
;;  Some Flags
;; ==========================================================================

#define PROTECTED_MODE	0
#define DEBUG_MODE	0
#define RUNNING_STATUS_OPTIMIZATION 1
#define SUART_WITH_38400_BAUD 0

;; ==========================================================================
;;  Chip Configuration
;; ==========================================================================

	        __CONFIG    _CONFIG1H, _OSCS_OFF_1H & _HSPLL_OSC_1H
		__CONFIG    _CONFIG2L, _BOR_ON_2L & _BORV_45_2L & _PWRT_OFF_2L
		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_128_2H
		__CONFIG    _CONFIG3H, _CCP2MX_ON_3H
		__CONFIG    _CONFIG4L, _STVR_ON_4L & _LVP_OFF_4L & _DEBUG_OFF_4L
		__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L 
		__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
		__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
		__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
		__CONFIG    _CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
		__CONFIG    _CONFIG7H, _EBTRB_OFF_7H

;; ==========================================================================
;;  Ports
;; ==========================================================================

; Pins of Software UART
PORT_SUART_IN	EQU	PORTA
PIN_SUART_IN    EQU     4               ; Pin A.4

PORTA_VALUE	EQU	b'00001000'	; SUART-Tx Pin = 1
TRISA_VALUE	EQU	b'00010000'	; Setup Value for Tristate Drivers of PortA

PORTE_VALUE	EQU	b'00000000'	; all drivers enabled
TRISE_VALUE	EQU	b'00000000'	; Setup Value for Tristate Drivers of PortE

TRISB_VALUE	EQU	b'00000000'	; Setup Value for Tristate Drivers of PortB
PORTB_VALUE	EQU	b'00000000'

PORTC_VALUE	EQU	b'01001111'	; Tx Pin = 1, all LEDs off
TRISC_VALUE	EQU	b'10000000'	; Setup Value for Tristate Drivers of PortC

PORTD_VALUE	EQU	b'00000000'	; all drivers enabled
TRISD_VALUE	EQU	b'00000000'	; Setup Value for Tristate Drivers of PortD

;; note: LED outputs won't be set if port is specified with 0
LED_RX0_PORT	EQU	LATC
LED_RX0_PIN	EQU	0
LED_TX_PORT	EQU	LATC
LED_TX_PIN	EQU	1
LED_RX1_PORT	EQU	LATC
LED_RX1_PIN	EQU	2
LED_PWR_PORT	EQU	LATC
LED_PWR_PIN	EQU	3


;; ==========================================================================
;;  Status Bytes & Bits
;; ==========================================================================

#if SUART_WITH_38400_BAUD 
TMR0_RELOAD     EQU     -130		; reload value for Timer0, 38400 Baud
#else
TMR0_RELOAD     EQU     -160		; reload value for Timer0, 31250 Baud
#endif

SUART_CTRL_RECEIVING     EQU 0          ; Bit 0: 0=waiting for startbit, 1=receiving

;; ==========================================================================
;;  Register File
;; ==========================================================================
SAVED_FSR2L	EQU	0x001
SAVED_FSR2H	EQU	0x002

SUART_CTRL      EQU     0x010
SUART_IN_STATE	EQU     0x011
SUART_IN_B      EQU     0x012
SUART_IN        EQU     0x013

RINGBUFFER0_A	EQU	0x014
RINGBUFFER0_B	EQU	0x015
RINGBUFFER1_A	EQU	0x016
RINGBUFFER1_B	EQU	0x017

MIDI_TIMEOUT_LOW EQU    0x018
MIDI_TIMEOUT_HIGH EQU   0x019
MIDI_TIMEOUT_HHIGH EQU  0x01a
MIDI_EXPBYTES	EQU	0x01b

MIDI0_RUNSTATUS	EQU	0x01c
MIDI1_RUNSTATUS	EQU	0x01d
MIDIO_RUNSTATUS EQU     0x01e

;; ==========================================================================

;; 0x100-0x1ff: SUART IN Ringbuffer
RINGBUFFER0     EQU     0x100
RINGBUFFER0_END	EQU	0x1ff

;; ==========================================================================

;; 0x200-0x2ff: UART IN Ringbuffer
RINGBUFFER1     EQU     0x200
RINGBUFFER1_END	EQU	0x2ff
