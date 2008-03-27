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

#ifdef __16F877
#include <p16f877.inc>
#endif

#ifdef __16F871
#include <p16f871.inc>
#endif

#ifdef __16F874
#include <p16f874.inc>
#endif

#ifdef __16F88
#include <p16f88.inc>
#endif


        ERRORLEVEL      -302    ; SUPPRESS BANK SELECTION MESSAGES
        ERRORLEVEL      -307    ; SUPPRESS PCLATH SELECTION MESSAGES
        ERRORLEVEL      -306    ; SUPPRESS PCLATH SELECTION MESSAGES

;; ==========================================================================
;;  Some Flags
;; ==========================================================================

#define DEBUG_MODE	1
#define RUNNING_STATUS_OPTIMIZATION 1

;; ==========================================================================
;;  Chip Configuration
;; ==========================================================================


#ifdef __16F88
	__CONFIG    _CONFIG1, _CP_OFF & _CCP1_RB0 & _DEBUG_OFF & _WRT_PROTECT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_ON & _MCLR_OFF & _PWRTE_OFF & _WDT_ON & _INTRC_IO

; reload value and sample offset for Timer0, used by software UART
TMR0_RELOAD		EQU     -64
TMR0_SAMPLE_OFFSET	EQU	16	; shifts the sample point more to the middle

; Pins of Software UART
PORT_SUART_IN	EQU	PORTA
PIN_SUART_IN    EQU     4               ; Pin A.4

PORTA_VALUE	EQU	b'00000111'	; Rx0/Rx1/Tx LED off
TRISA_VALUE	EQU	b'11110000'	; Setup Value for Tristate Drivers of PortA
PORTB_VALUE	EQU	b'00100001'	; Pwr LED off, TX Pin = 1
TRISB_VALUE	EQU	b'11010110'	; Setup Value for Tristate Drivers of PortB

;; to stay compatible with PIC18F67x
#define T0IE    TMR0IE
#define T0IF    TMR0IF

;; note: LED outputs won't be set if port is specified with 0
LED_RX0_PORT	EQU	PORTA
LED_RX0_PIN	EQU	2
LED_TX_PORT	EQU	PORTA
LED_TX_PIN	EQU	1
LED_RX1_PORT	EQU	PORTA
LED_RX1_PIN	EQU	0
LED_PWR_PORT	EQU	PORTB
LED_PWR_PIN	EQU	0
#else
        __CONFIG _HS_OSC & _WDT_ON & _PWRTE_ON & _CP_OFF & _BODEN_ON & _LVP_OFF

; reload value and sample offset for Timer0, used by software UART
TMR0_RELOAD		EQU     -160
TMR0_SAMPLE_OFFSET	EQU	16	; shifts the sample point more to the middle

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
LED_RX0_PORT	EQU	PORTC
LED_RX0_PIN	EQU	0
LED_TX_PORT	EQU	PORTC
LED_TX_PIN	EQU	1
LED_RX1_PORT	EQU	PORTC
LED_RX1_PIN	EQU	2
LED_PWR_PORT	EQU	PORTC
LED_PWR_PIN	EQU	3
#endif

;; ==========================================================================
;;  Ports
;; ==========================================================================


;; ==========================================================================
;;  Status Bytes & Bits
;; ==========================================================================

SUART_CTRL_RECEIVING     EQU 0          ; Bit 0: 0=waiting for startbit, 1=receiving

;; ==========================================================================
;;  Register File
;; ==========================================================================

SUART_CTRL      EQU     0x20
SUART_IN_STATE	EQU     0x21
SUART_IN_B      EQU     0x22
SUART_IN        EQU     0x23

RINGBUFFER0_A	EQU	0x24
RINGBUFFER0_B	EQU	0x25
RINGBUFFER1_A	EQU	0x26
RINGBUFFER1_B	EQU	0x27

MIDI_TIMEOUT_LOW EQU    0x28
MIDI_TIMEOUT_HIGH EQU   0x29
MIDI_TIMEOUT_HHIGH EQU  0x2a
MIDI_EXPBYTES	EQU	0x2b

MIDI0_RUNSTATUS	EQU	0x2c
MIDI1_RUNSTATUS	EQU	0x2d
MIDIO_RUNSTATUS EQU     0x2e

LED_CTR_RX0	EQU     0x2f
LED_CTR_RX1	EQU     0x30
LED_CTR_TX	EQU     0x31

;; 0x32-0x7b: SUART IN Ringbuffer
RINGBUFFER0     EQU     0x32
RINGBUFFER0_END	EQU	0x7b

TMP1            EQU     0x7c
FSR_TMP		EQU	0x7d
STATUS_TMP	EQU	0x7e
W_TMP		EQU	0x07f
W_TMP2		EQU	0x0ff
W_TMP3		EQU	0x17f
W_TMP4		EQU	0x1ff

;; ==========================================================================

;; 0xa0-0xef, 0xa0-0xef: UART IN Ringbuffer
RINGBUFFER1     EQU     0xa0
#ifdef __16F871
RINGBUFFER1_END	EQU	0xbf
#else
RINGBUFFER1_END	EQU	0xef
#endif
