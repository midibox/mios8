; $Id$
;
; Source Code of MIOS Bootloader
;
; ==========================================================================
;
;  Copyright 1998-2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; ==========================================================================
;;  Select the default PIC ID configuration here
;; ==========================================================================

#define PICID_DEVICE_ID   0x00	; the device ID (0x00-0x7f)

#define PICID_LCD_TYPE    0	; 0 == CLCD
				; 1 == KS0107/KS0108 or HD61202 based graphical LCD
				; 6 == MIDI link to a LCD or MBHP_TV
				; 7 == custom LCD driver

#define PICID_TO_COM_FLAG 0	; IMPORTANT: never alternate this flag if you don't
				; own a to-COM interface, otherwise you won't be
				; able to reset this flag w/o a PIC programmer

#define PICID_IIC_MIDI_ADDR 0x00 ; if != 0x00 and != 0xff, MBHP_IIC_MIDI slave 
				; with the given address will be used as second MIDI OUT

;; MEMO:
;; .hex file must contain following entry for IIC slave address 0x10
;; :080000000000000000100000E8

#ifdef __18F452
	LIST R=DEC
#include <p18f452.inc>
#define PIC_DERIVATIVE_CODE_SIZE	0x08000
#define PIC_DERIVATIVE_EEPROM_SIZE	0x100
#endif

#ifdef __18F4620
	LIST R=DEC
#include <p18f4620.inc>
#define PIC_DERIVATIVE_CODE_SIZE	0x10000
#define PIC_DERIVATIVE_EEPROM_SIZE	0x400
#endif
	
#ifdef __18F4520
	LIST R=DEC
#include <p18f4520.inc>
#define PIC_DERIVATIVE_CODE_SIZE	0x08000
#define PIC_DERIVATIVE_EEPROM_SIZE	0x100
#endif

#ifdef __18F4682
	LIST R=DEC
#include <p18f4682.inc>
#define PIC_DERIVATIVE_CODE_SIZE	0x14000
#define PIC_DERIVATIVE_EEPROM_SIZE	0x400
#endif

#ifdef __18F4685
	LIST R=DEC
#include <p18f4685.inc>
#define PIC_DERIVATIVE_CODE_SIZE	0x18000
#define PIC_DERIVATIVE_EEPROM_SIZE	0x400
#endif
	
;; ==========================================================================
;;  Chip Configuration
;; ==========================================================================

		__IDLOCS    _IDLOC0, 0x00
		__IDLOCS    _IDLOC1, 0x00
		__IDLOCS    _IDLOC2, 0x00
		__IDLOCS    _IDLOC3, 0x00
		__IDLOCS    _IDLOC4, 0x00
		__IDLOCS    _IDLOC5, PICID_IIC_MIDI_ADDR
		__IDLOCS    _IDLOC6, ((PICID_LCD_TYPE & 7) << 4) | ((PICID_TO_COM_FLAG & 1) << 0)
		__IDLOCS    _IDLOC7, ((PICID_DEVICE_ID & 0x7f) << 0)

#ifdef __18F452
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
#endif

#ifdef __18F4620
	        __CONFIG    _CONFIG1H, _FCMEN_OFF_1H & _IESO_OFF_1H & _OSC_HSPLL_1H
		__CONFIG    _CONFIG2L, _BOREN_ON_2L & _PWRT_OFF_2L & _BORV_1_2L
		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_512_2H
		__CONFIG    _CONFIG3H, _CCP2MX_PORTC_3H & _PBADEN_OFF_3H & _LPT1OSC_OFF_3H
		__CONFIG    _CONFIG4L, _STVREN_ON_4L & _LVP_OFF_4L & _DEBUG_OFF_4L & _XINST_OFF_4L
		__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L 
		__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
		__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
		__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
		__CONFIG    _CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
		__CONFIG    _CONFIG7H, _EBTRB_OFF_7H
#endif

#ifdef __18F4520
 	        __CONFIG    _CONFIG1H, _IESO_OFF_1H & _FCMEN_OFF_1H & _OSC_HSPLL_1H
		__CONFIG    _CONFIG2L, _BOREN_ON_2L & _BORV_1_2L & _PWRT_OFF_2L
		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_512_2H
		__CONFIG    _CONFIG3H, _CCP2MX_PORTC_3H
		__CONFIG    _CONFIG4L, _XINST_OFF_4L & _STVREN_ON_4L & _LVP_OFF_4L & _DEBUG_OFF_4L
		__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L 
		__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
		__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
		__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
		__CONFIG    _CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
		__CONFIG    _CONFIG7H, _EBTRB_OFF_7H
#endif

#ifdef __18F4682
	        __CONFIG    _CONFIG1H, _FCMEN_OFF_1H & _IESO_OFF_1H & _OSC_HSPLL_1H
		__CONFIG    _CONFIG2L, _BOREN_BOHW_2L & _PWRT_OFF_2L & _BORV_1_2L
		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_512_2H
		__CONFIG    _CONFIG3H, _MCLRE_ON_3H & _PBADEN_OFF_3H & _LPT1OSC_OFF_3H
		__CONFIG    _CONFIG4L, _STVREN_ON_4L & _LVP_OFF_4L & _DEBUG_OFF_4L & _XINST_OFF_4L
		__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L & _CP4_OFF_5L
		__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
		__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
		__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
		__CONFIG    _CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
		__CONFIG    _CONFIG7H, _EBTRB_OFF_7H
#endif

#ifdef __18F4685
	        __CONFIG    _CONFIG1H, _FCMEN_OFF_1H & _IESO_OFF_1H & _OSC_HSPLL_1H
		__CONFIG    _CONFIG2L, _BOREN_BOHW_2L & _PWRT_OFF_2L & _BORV_1_2L
		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_512_2H
		__CONFIG    _CONFIG3H, _MCLRE_ON_3H & _PBADEN_OFF_3H & _LPT1OSC_OFF_3H
		__CONFIG    _CONFIG4L, _STVREN_ON_4L & _LVP_OFF_4L & _DEBUG_OFF_4L & _XINST_OFF_4L
		__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L & _CP4_OFF_5L & _CP5_OFF_5L 
		__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
		__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L 
		__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H
		__CONFIG    _CONFIG7L, _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L
		__CONFIG    _CONFIG7H, _EBTRB_OFF_7H
#endif


;; ==========================================================================
;;  Registers
;; ==========================================================================

;; Pins of SRIO Shift Registers
MIOS_SRIO_LAT_SCLK	EQU	LATD	; Pin D.3
MIOS_SRIO_PIN_SCLK	EQU	3
MIOS_SRIO_LAT_RCLK	EQU	LATD	; Pin D.2
MIOS_SRIO_PIN_RCLK	EQU	2
MIOS_SRIO_PORT_DIN	EQU	PORTD	; Pin D.1
MIOS_SRIO_PIN_DIN	EQU	1
MIOS_SRIO_LAT_DOUT	EQU	LATD	; Pin D.0
MIOS_SRIO_PIN_DOUT	EQU	0

;; Pins of MF Shift Registers
MIOS_MF_LAT_RCLK	EQU	LATC	; Pin C.0
MIOS_MF_PIN_RCLK	EQU	0
MIOS_MF_LAT_SCLK	EQU	LATC	; Pin C.1
MIOS_MF_PIN_SCLK	EQU	1
MIOS_MF_LAT_DOUT	EQU	LATC	; Pin C.2
MIOS_MF_PIN_DOUT	EQU	2

;; Configuration register (matches with register layout of MIOS itself)
MIOS_BOX_CFG0		EQU	0x000
MIOS_BOX_CFG0_TO_HOST	EQU	6	; if 1, MIDI interface will run with 38400 baud instead of 31250
MIOS_BOX_CFG0_20MHz	EQU	7	; if 1, it is assumed that box is running with 20 MHz, else with 40 MHz

;; variables used by BSL
DEVICE_ID       	EQU  	0x001

CHECKSUM        	EQU  	0x002

TMP1			EQU  	0x003
TMP2			EQU  	0x004
TMP3			EQU  	0x005

BYTE_NUMBER     	EQU  	0x006
RECEIVED_BYTE   	EQU  	0x007

SYX_U			EQU  	0x008
SYX_H			EQU  	0x009
SYX_L			EQU  	0x00a

SYX_BIT7_CTR		EQU  	0x00b
SYX_BIT8_CTR		EQU  	0x00c
SYX_BYTE7       	EQU  	0x00d
SYX_BYTE8       	EQU  	0x00e

BYTE_CTR_H      	EQU  	0x010
BYTE_CTR_L      	EQU  	0x011

FLASH_BLOCK_CTR		EQU  	0x012
FLASH_ROW_CTR		EQU  	0x013
FLASH_BUFFER_CTR 	EQU 	0x014

TIMEOUT_CTR_U		EQU  	0x020
TIMEOUT_CTR_H		EQU  	0x021
TIMEOUT_CTR_L		EQU  	0x022

ADDRESS_EXTENSION	EQU	0x023

UART_TMP		EQU	0x024

IIC_MIDI_ADDR		EQU	0x025
IIC_MIDI_TMP		EQU	0x026
IIC_MIDI_RETRY_CTR	EQU	0x027

IIC_STAT		EQU	0x028
IIC_CTR			EQU	0x029
IIC_BUFFER		EQU	0x02a

MIDI_BUFFER     	EQU  	0x100
MIDI_BUFFER_END 	EQU  	0x4ff

;; ==========================================================================
;;  IIC specific settings
;; ==========================================================================

;; IIC Pins
#define IIC_PORT_SCL	PORTD
#define IIC_LAT_SCL	LATD
#define IIC_TRIS_SCL	TRISD
#define IIC_PIN_SCL	5
#define IIC_PORT_SDA	PORTA
#define IIC_LAT_SDA	LATA
#define IIC_TRIS_SDA	TRISA
#define IIC_PIN_SDA	4

;; bit 0 of IIC_STAT indicates if a slave is available
#define IIC_STAT_SLAVE_AVAILABLE	0

;; optional switch to stretch the SCL
#define STRETCH_SCL	0

;; if RA4 is not an open drain driver, set this to 0 (e.g. PIC18F4620)
;; in this version, we always disable it to get an identical behaviour
#define RA4_IS_OPEN_DRAIN 0

;; ==========================================================================
;;  Include Files
;; ==========================================================================

#include <macros.h>

;; ==========================================================================
;;  Main program
;; ==========================================================================

	;; ==================================================================
	;;  Boot and IRQ Vectors
	;; ==================================================================
	org	0x0000
	goto	Start
	goto	Start

	org	0x0008
	goto	0x0408

	;; ==================================================================
	;;  Linked Boot and IRQ Vector (will be overwritten by MIOS)
	;; ==================================================================
	org	0x0400
	goto	0x0000
	dw	0xeeee		; magic number is: 0x4711 - without this code, the bootloader won't branch to 0x0400
	;; note: 0x4711 results into the instruction "rlncf 0x11, F, BANKED", an unusual code

	org	0x0408
IRQ_Dummy
	goto	IRQ_Dummy	; (endless loop so long MIOS hasn't been uploaded)


	;; ==================================================================
	;;  BSL starts at 0x000c
	;; ==================================================================
	org	0x000c
Start

	;; ==================================================================
	;;  read frequency and MIDI interface type depending from PIC ID/config
	;; ==================================================================
	clrf	MIOS_BOX_CFG0		; clear box status

Check_FrequencyCfg
	TABLE_ADDR_FULL _CONFIG1H	; derive PLL enabled/disabled from config sector
	tblrd*+				; read high byte of first entry
	movf	TABLAT, W		; extract FOSC[2..0]
	andlw	0x07			; if FOSC == 6, PLL is running
	xorlw	0x06
	skpnz
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_20MHz; clear 20 MHz flag in this case

Fetch_DeviceID
	TABLE_ADDR_FULL _IDLOC7		; derive device ID from ID sector
	tblrd*-				; get first byte
	movf	TABLAT, W		; copy 7-bit value to DEVICE_ID
	andlw	0x7f
	movwf	DEVICE_ID

Fetch_ToHostFlag
	tblrd*-				; read to-host flag from ID sector
	btfsc	TABLAT, 0
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST

Fetch_IICAddr
	tblrd*-				; read IIC slave address
	movf	TABLAT, W
	movwf	IIC_MIDI_ADDR
	addlw	1			; clear if 0xff (workaround for programmers which don't support ID writing)
	skpnz
	clrf	IIC_MIDI_ADDR

	;; ==================================================================
	;;  Initialize the IO ports
	;; ==================================================================
Init_Ports
	bcf	INTCON2, NOT_RBPU	; Enable Pull-Ups of PortB

	clrf	PORTA			; Reset PortA
	clrf	PORTB			; Reset PortB
	movlw	0x40			; Reset PortC (TX Pin = 1)
	movwf	PORTC
	clrf	PORTD			; Reset PortD
	clrf	PORTE			; Reset PortE

	movlw	b'11101111'		; PortA: all inputs
	movwf	TRISA
	setf	TRISB			; PortB: all inputs (pull-ups activated)
	movlw	b'10000000'		; PortC: C.7 is inputs
	movwf	TRISC
	movlw	b'00000010'		; PortD: D.1 is input
	movwf	TRISD
	movlw	b'00000111'		; PortE: all inputs
	movwf	TRISE


	;; ==================================================================
	;;  Reset the serial shift registers (SRIO and MF only)
	;; ==================================================================
Init_SRs
        bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK
	bcf	MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK
        bcf	MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bcf	MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK

	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	bcf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT
	movlw	0x80
	movwf	TMP1
Init_SRs_Loop
	bsf	MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bsf	MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
	bcf	MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bcf	MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
	decfsz	TMP1, F
	rgoto	Init_SRs_Loop

	;; latch enable
        bsf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK
	bsf	MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK
	
	;; latch disable
        bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK
	bcf	MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK


	;; ==================================================================
	;;  Init serial interface
	;; ==================================================================	
	movlw	(1 << TXEN) | (1 << BRGH)
	movwf	TXSTA
	GOTO_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST, ACCESS, Init_USART_MIDIBaudrate
Init_USART_RS232Baudrate
  	movlw	2*0x21-1			; set baudrate to 38400 baud
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_20MHz
	movlw 0x21-1
	goto	Init_USART_Cont

Init_USART_MIDIBaudrate
	movlw	8*0x0a-1			; set baudrate to 31250 baud
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_20MHz
	movlw 4*0x0a-1
	;; 	goto	Init_USART_Cont
Init_USART_Cont
	movwf	SPBRG

	
	movlw	(1 << SPEN) | (1 << CREN)	; configure receive status and control register
	movwf	RCSTA

        bcf     RCSTA, CREN			;; make sure that USART Receive Buffer is empty
        movf    RCREG, W
        bsf     RCSTA, CREN

	bcf	PIR1, RCIF			; clear received flag in PIR1


	;; ==================================================================
	;;  Init IIC MIDI interface if IIC_MIDI_ADDR != 0x00
	;;  (or != 0xff - the code above has already cleared IIC_MIDI_ADDR in this case)
	;; ==================================================================
Init_IIC_MIDI
	bcf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE	; disable slave
	movf	IIC_MIDI_ADDR, W
	bz	Init_IIC_MIDI_End

	bsf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE	; force slave available

	rcall	IIC_Stop	; stop IIC (sets SDA/SCL properly)

	;; send 0xfe to check if slave is connected to core
	movlw	0xfe
	rcall	IIC_MIDI_Send
	;; now IIC_STAT, IIC_STAT_SLAVE_AVAILABLE should have a proper state
Init_IIC_MIDI_End	

	;; ==================================================================
	;;  Send SysEx upload request message
	;; ==================================================================	
	rcall	SYSEX_SendHeader
	movlw	0x01
	movwf	CHECKSUM
	rcall	SYSEX_SendFooter


	;; ==================================================================
	;;  The main loop
	;; ==================================================================	
MainLoop
MainLoop_WaitSequence_ResetAll

	movlw	0x30
	movwf	TIMEOUT_CTR_U
	clrf	TIMEOUT_CTR_H
	clrf	TIMEOUT_CTR_L

MainLoop_WaitSequence_Reset
	clrf	BYTE_NUMBER


MainLoop_WaitSequence
	clrwdt
	movf	BYTE_NUMBER, W
	bnz	MainLoop_WaitSequence_Rec

	;; first byte with timeout counter, all others with watchdog
MainLoop_WaitSequence_RecLoop
	clrwdt
	BRA_IFSET PIR1, RCIF, ACCESS, MainLoop_WaitSequence_Rec
	decfsz	TIMEOUT_CTR_L, F
	rgoto	MainLoop_WaitSequence_RecLoop
	decfsz	TIMEOUT_CTR_H, F
	rgoto	MainLoop_WaitSequence_RecLoop
	decfsz	TIMEOUT_CTR_U, F
	rgoto	MainLoop_WaitSequence_RecLoop

	;; check magic number, must be 0x4711
	TABLE_ADDR 0x0404
	tblrd*+
	movlw	0x11
	cpfseq	TABLAT, ACCESS
	reset
	tblrd*+
	movlw	0x47
	cpfseq	TABLAT, ACCESS
	reset	

	;; branch to firmware
	clrwdt
	goto	0x00400


	;; ==================================================================
	;;  This handler parses incoming bytes
	;; ==================================================================	
MainLoop_WaitSequence_Rec
	rcall	UART_ReceiveByte

	;; ignore realtime messages
	movlw	0xf8
	cpfslt	RECEIVED_BYTE, ACCESS
	rgoto MainLoop_WaitSequence_RecLoop

	movf	BYTE_NUMBER, W
	rcall	SYSEX_NextExpectedByte
	xorwf	RECEIVED_BYTE, W
 	bnz	MainLoop_WaitSequence_Reset

	incf	BYTE_NUMBER, F

	movlw	0x06
	cpfseq	BYTE_NUMBER, ACCESS
	rgoto MainLoop_WaitSequence

	rcall	SYSEX_Receive_Hlp
	movwf	ADDRESS_EXTENSION
	andlw	0x8f			; bit 6..4 contain the address extension
	xorlw	0x02
	bnz	MainLoop_WaitSequence_Reset

	swapf	ADDRESS_EXTENSION, W	; align the address extension correctly
	andlw	0x07
	movwf	ADDRESS_EXTENSION

	clrf	CHECKSUM

	rcall	SYSEX_Receive_Hlp
	movwf	SYX_H
	addwf	CHECKSUM, F
	rcall	SYSEX_Receive_Hlp
	movwf	SYX_L
	addwf	CHECKSUM, F
	rcall	SYSEX_ConvertAddr_Hlp
	movff	SYX_U, TBLPTRU
	movff	SYX_H, TBLPTRH
	movff	SYX_L, TBLPTRL

	rcall	SYSEX_Receive_Hlp
	movwf	SYX_H
	addwf	CHECKSUM, F
	rcall	SYSEX_Receive_Hlp
	movwf	SYX_L
	addwf	CHECKSUM, F
	rcall	SYSEX_ConvertAddr_Hlp
	
	movff	SYX_H, BYTE_CTR_H
	movff	SYX_L, BYTE_CTR_L

	lfsr	FSR0, MIDI_BUFFER

	movlw	0x08
	movwf	SYX_BIT8_CTR

	;; ==================================================================
	;;  Block Receiver Routine
	;; ==================================================================	
MainLoop_ReceiveBlock
	rcall	SYSEX_Receive_Hlp
	addwf	CHECKSUM, F

	movwf	SYX_BYTE7
	clrc
	rlf	SYX_BYTE7, F
	movlw	0x07
	movwf	SYX_BIT7_CTR
MainLoop_Bit7Shift
	rlf	SYX_BYTE7, F
	rlf	SYX_BYTE8, F
	dcfsnz	SYX_BIT8_CTR, F
	rgoto	MainLoop_BitShift_NewWord
MainLoop_BitShift_NewWordCont
	decfsz	SYX_BIT7_CTR, F
	rgoto	MainLoop_Bit7Shift

	rgoto	MainLoop_ReceiveBlock
	
	
MainLoop_BitShift_NewWord
	movlw	(MIDI_BUFFER_END+2) & 0xff
	cpfseq	FSR0L, ACCESS
	rgoto MainLoop_BitShift_NewWord_NoOv
	movlw	(MIDI_BUFFER_END+2) >> 8
	cpfseq	FSR0H, ACCESS
	rgoto MainLoop_BitShift_NewWord_NoOv
	rgoto	MainLoop_WaitSequence_ResetAll

MainLoop_BitShift_NewWord_NoOv
	movff	SYX_BYTE8, POSTINC0

	movlw	0x08
	movwf	SYX_BIT8_CTR

	decf	SYX_L, F
	skpc
	decf	SYX_H, F
	skpc
	decf	SYX_U, F

	movf	SYX_L, W
	iorwf	SYX_H, W
	iorwf	SYX_U, W
	bnz	MainLoop_BitShift_NewWordCont

	;; ==================================================================
	;;  Checksum calculation
	;; ==================================================================	
MainLoop_BlockReceived
	rcall	SYSEX_Receive_Hlp

	movf	CHECKSUM, W
	sublw	0
	andlw	0x7f
	movwf	CHECKSUM

	movf	CHECKSUM, W
	xorwf	RECEIVED_BYTE, W
	bz	Mainloop_ChecksumOk

Mainloop_ChecksumFailed
	rcall	SYSEX_SendHeader	; send disacknowledge with wrong checksum
	movlw	0x0e
	rcall	UART_SendByte
	movlw	0x03
	rcall	UART_SendByte
	rcall	SYSEX_SendFooter
	reset				; and reset device!

Mainloop_ChecksumOk
	clrwdt

	;; ==================================================================
	;;  EEPROM/Flash Programming Routine
	;; ==================================================================	
Mainloop_Write
	;; if TBLPTRH == 0x80 (18F4620: 0x80..0x83): write to EEPROM
	BRA_IFCLR TBLPTRH, 7, ACCESS, FLASH_Write

EEPROM_Write
	;; EEPROM: address must be within 0x00-0xff (0x000-0x3ff) (PIC18F4620: 0x000-0x3ff), etc...
	movlw	HIGH(PIC_DERIVATIVE_EEPROM_SIZE) | 0x80
	cpfslt	TBLPTRH, ACCESS
	rgoto Mainloop_Write_EndError

	movf	ADDRESS_EXTENSION, W
	bnz	Mainloop_Write_EndError
	movf	TBLPTRU, W
	bnz	Mainloop_Write_EndError

	;; don't write if more than 256 bytes have been received
	movf	BYTE_CTR_H, W
	andlw	0xfe
	bnz	Mainloop_Write_EndError

	lfsr	FSR0, MIDI_BUFFER
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	movff	TBLPTRH, EEADRH
#endif
	movff	TBLPTRL, EEADR
EEPROM_WriteLoop
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	clrwdt			; the programming time is longer on newer derivatives with large EEPROM
#endif

	movf	POSTINC0, W
	movwf	EEDATA			; store byte to write into EEDATA

	bcf	EECON1, EEPGD		; point to data memory
	bsf	EECON1, WREN		; EEPROM write enable
	movlw	0x55			; unlock sequence
	movwf	EECON2
	movlw	0xaa
	movwf	EECON2
	bsf	EECON1, WR		; EEPROM write
EEPROM_WriteLoopPoll
	GOTO_IFSET EECON1, WR, ACCESS, EEPROM_WriteLoopPoll

	incf	EEADR, F
#if PIC_DERIVATIVE_EEPROM_SIZE > 0x100
	skpnz
	incf	EEADRH, F
#endif
	decfsz	BYTE_CTR_L, F
	rgoto	EEPROM_WriteLoop

	rgoto	Mainloop_Write_EndOk
	

	
FLASH_Write
	;; add address extension to TBLPTR[7] and TBLPTRU
	btfsc	ADDRESS_EXTENSION, 0
	bsf	TBLPTRH, 7
	rrf	ADDRESS_EXTENSION, W
	andlw	0x03
	movwf	TBLPTRU

	;; if TBLPTRH & 0xfc == 0x00: skip write (because this is the bootloader area)
	bnz	FLASH_Write_NoBSLAddrCheck	; (if TBLPTRU != 0)
	movf	TBLPTRH, W
	andlw	0xfc
	bz	Mainloop_Write_EndError
FLASH_Write_NoBSLAddrCheck

	;; prevent writes to addresses > PIC code size
	;; we calculate with 4k blocks, accordingly we have to compare it against TBLPTRU[3:0]:TBLPTRH[7:4]
	swapf	TBLPTRU, W
	andlw	0xf0
	movwf	PRODL
	swapf	TBLPTRH, W
	andlw	0x0f
	iorwf	PRODL, F
	movlw	(PIC_DERIVATIVE_CODE_SIZE >> 12) & 0xff
	cpfslt	PRODL, ACCESS
	rgoto Mainloop_Write_EndError


	;; write buffer into flash
	lfsr	FSR0, MIDI_BUFFER
	;; how much blocks have to be erased: BYTE_CTR_[LH] >> 6
	movff	BYTE_CTR_H, FLASH_BLOCK_CTR
	movff	BYTE_CTR_L, TMP1
	rlf	TMP1, F
	rlf	FLASH_BLOCK_CTR, F
	rlf	TMP1, F
	rlf	FLASH_BLOCK_CTR, F

FLASH_Write_Loop
FLASH_Write_EraseBlock
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH program memory
	bsf	EECON1, WREN		; enable write to memory
	bsf	EECON1, FREE		; enable Row Erase operation
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start erase (CPU stall)
	tblrd*-				; dummy read decrement
FLASH_WriteBlock

	movlw	0x08
	movwf	FLASH_ROW_CTR		; 8 * 8 bytes row to write
FLASH_WriteBlockLoop
	movlw	0x08
	movwf	FLASH_BUFFER_CTR	; number of bytes in 128 bit buffer
FLASH_WriteBlock_ToHRegs
	movff	POSTINC0, TABLAT	; transfer byte into table latch
	tblwt+*				; write data, perform a short write
	decfsz	FLASH_BUFFER_CTR, F	; loop until buffers are full
	rgoto	FLASH_WriteBlock_ToHRegs

FLASH_ProgramMemory
	bsf	EECON1, EEPGD		; point to FLASH program memory
	bcf	EECON1, CFGS		; access FLASH program memory
	bsf	EECON1, WREN		; enable write to memory
	movlw	0x55			; write 55h
	movwf	EECON2
	movlw	0xaa			; write AAh
	movwf	EECON2
	bsf	EECON1, WR		; start program (CPU stall)

	decfsz	FLASH_ROW_CTR, F
	rgoto	FLASH_WriteBlockLoop

	bcf	EECON1, WREN		; disable write to memory

	tblrd*+				; dummy read increment

	decfsz	FLASH_BLOCK_CTR, F
	rgoto	FLASH_Write_Loop

Mainloop_Write_EndOk
	rcall	SYSEX_SendHeader	; send confirm header with checksum
	movlw	0x0f
	rcall	UART_SendByte
	rcall	SYSEX_SendFooter
	rgoto	WaitEnd

Mainloop_Write_EndError
	movlw	0x05			; send error response
	rcall	SYSEX_SendError
	rgoto	WaitEnd

WaitEnd
	goto	MainLoop


;; --------------------------------------------------------------------------
;; Help Routine for receiving SysEx
;; --------------------------------------------------------------------------
SYSEX_Receive_Hlp
	clrwdt
	rcall	UART_ReceiveByte
	movlw	0xf8		; ignore realtime messages
	cpfslt	RECEIVED_BYTE, ACCESS
	rgoto SYSEX_Receive_Hlp
	movf	RECEIVED_BYTE, W
	BRA_IFSET RECEIVED_BYTE, 7, ACCESS, SYSEX_Receive_Hlp_Fail
	return

SYSEX_Receive_Hlp_Fail
	pop	; Quick & Dirty return
	rgoto	MainLoop_WaitSequence_ResetAll
	
;; --------------------------------------------------------------------------
;; Help routine which converts an SysEx address to PIC address
;; --------------------------------------------------------------------------
SYSEX_ConvertAddr_Hlp
	clrc
	rlf	SYX_L, F
	clrf	SYX_U
	clrc
	rlf	SYX_L, F
	rlf	SYX_H, F
	rlf	SYX_U, F
	clrc
	rlf	SYX_L, F
	rlf	SYX_H, F
	rlf	SYX_U, F
	return

;; --------------------------------------------------------------------------
;; This routine sends the SysEx header
;; --------------------------------------------------------------------------
SYSEX_SendHeader
	clrf	TMP1
SYSEX_SendHeader_Loop
	movf	TMP1, W
	rcall	SYSEX_NextExpectedByte
	rcall	UART_SendByte
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto SYSEX_SendHeader_Loop
	return

;; --------------------------------------------------------------------------
;; This routine sends the SysEx footer
;; --------------------------------------------------------------------------
SYSEX_SendFooter
	movf	CHECKSUM, W
	rcall	UART_SendByte
	movlw	0xf7
	rgoto	UART_SendByte

;; --------------------------------------------------------------------------
;; sends an error message and waits until last byte transmitted
;; --------------------------------------------------------------------------
SYSEX_SendError
	movwf	PRODL			; store error number
	rcall	SYSEX_SendHeader
	movlw	0x0e
	rcall	UART_SendByte
	movf	PRODL, W
	rcall	UART_SendByte
	rcall	SYSEX_SendFooter

	;; wait until transceiver is unbusy
SYSEX_SendError_Wait
	BRA_IFCLR TXSTA, TRMT, ACCESS, SYSEX_SendError_Wait

	return

;; --------------------------------------------------------------------------
;; Returns a byte of the SysEx header (selected with WREG)
;; --------------------------------------------------------------------------
SYSEX_NextExpectedByte
	andlw	0x07
	call	HLP_GetIndex_2bytes
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x40		; Bootloader ID (40 - the ultimative number -2)
	movf	DEVICE_ID, W	; device ID
	return
	retlw	0x00		; (DUMMY)

;; --------------------------------------------------------------------------
;;  HLP GetIndex 2 bytes - help routine for indirect jumps
;;  In:	 jump index in W
;;  Out: jump to the appr address behind the call to this function
;; --------------------------------------------------------------------------
HLP_GetIndex_2bytes
        rlncf   WREG, W
        addwf   TOSL, F
        btfsc   STATUS, C
        incf    TOSH, F
        btfsc   STATUS, C
        incf    TOSU, F
        return

;; --------------------------------------------------------------------------
;;  UART Receive Byte: receives a byte from the UART, 
;;  resets the device on overrun/frame error or watchdog overrun
;;  In:	 -
;;  Out: received byte in W and RECEIVED_BYTE
;; --------------------------------------------------------------------------
UART_ReceiveByte
	BRA_IFSET RCSTA, OERR, ACCESS, UART_ReceiveByte_OERR	; detect overrun error
UART_ReceiveBytePoll
	BRA_IFCLR PIR1, RCIF, ACCESS, UART_ReceiveBytePoll		; wait until new byte has been received
	BRA_IFSET RCSTA, FERR, ACCESS, UART_ReceiveByte_FERR	; detect frame error
	movf	RCREG, W					; return content of receive register
	movwf	RECEIVED_BYTE
	return

	;; on overrun error: send error 0x0b and reset
UART_ReceiveByte_OERR
	movlw	0x0b
	rcall	SYSEX_SendError
	reset
	
	;; on frame error: send error 0x0c and reset
UART_ReceiveByte_FERR
	movlw	0x0c
	rcall	SYSEX_SendError
	reset
	
;; --------------------------------------------------------------------------
;;  UART Send Byte: sends byte over UART
;;  In:	 byte to send in W
;;  Out: sent byte
;; --------------------------------------------------------------------------
UART_SendByte
	movwf	UART_TMP		; always try to send the same byte via IIC
	rcall	IIC_MIDI_Send		; (function skips transfer if IIC slave not available)
	movf	UART_TMP, W
UART_SendBytePoll
	BRA_IFCLR TXSTA, TRMT, ACCESS, UART_SendBytePoll		; wait until transceiver is unbusy
	movwf	TXREG						; send new byte and return immediately
	return

;; --------------------------------------------------------------------------
;;  IIC MIDI Send function
;;  In:	 byte to send in W
;;  Out: sent byte
;; --------------------------------------------------------------------------
IIC_MIDI_Send
	;; temporary store value
	movwf	IIC_MIDI_TMP

	;; exit if interface not available
	btfss	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE
	return

	;; clear timeout counter (256 tries - more than enough!)
	clrf	IIC_MIDI_RETRY_CTR

IIC_MIDI_SendRestart
	rcall	IIC_MIDI_TransmitStart	; start transfer
	BRA_IFSET IIC_STAT, IIC_STAT_SLAVE_AVAILABLE, ACCESS, IIC_MIDI_SendTry
IIC_MIDI_SendRetry
	rcall	IIC_Stop		; stop transfer
	decfsz	IIC_MIDI_RETRY_CTR, F	; restart until counter reaches zero
	rgoto	IIC_MIDI_SendRestart

	;; give up!
	bcf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE ; notify that slave is not available
	rgoto	IIC_MIDI_Send_End

IIC_MIDI_SendTry
	;; try to send byte
	movf	IIC_MIDI_TMP, W
	rcall	IIC_ByteSend

	;; retry if no acknowledge received
	BRA_IFCLR IIC_STAT, IIC_STAT_SLAVE_AVAILABLE, ACCESS, IIC_MIDI_SendRetry

	;; stop transfer
	rcall	IIC_Stop

IIC_MIDI_Send_End
	return

;; --------------------------------------------------------------------------

IIC_MIDI_TransmitStart
	rcall	IIC_Start		; start IIC
	movf	IIC_MIDI_ADDR, W	; send address (write transfer)
	andlw	0xfe			; ensure that read flag not set
	rgoto	IIC_ByteSend
	;; (check IIC_STAT, IIC_STAT_SLAVE_AVAILABLE after this function call!)

;; --------------------------------------------------------------------------
;; low-level IIC routines (reduced version of MIOS_IIC_*)
;; --------------------------------------------------------------------------
IIC_Start
	bsf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE

	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
#if RA4_IS_OPEN_DRAIN == 0
	bcf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = output
#endif
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
	rcall	IIC_Delay_600ns
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
	return

;; ---
IIC_Stop
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
	rcall	IIC_Delay_600ns
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
	return

;; ---
IIC_Delay_600ns_Stretch
	;; 2 cycles for the call
#if STRETCH_SCL == 0
	nop	; 2 cycles
	nop
#else
	;; if clock stretching enabled:	
	bsf	IIC_TRIS_SCL, IIC_PIN_SCL
IIC_Delay_600ns_StretchLoop
	btfss	IIC_PORT_SCL, IIC_PIN_SCL		; (spike filter...)
	rgoto	IIC_Delay_600ns_StretchLoop
	btfss	IIC_PORT_SCL, IIC_PIN_SCL
	rgoto	IIC_Delay_600ns_StretchLoop
	btfss	IIC_PORT_SCL, IIC_PIN_SCL
	rgoto	IIC_Delay_600ns_StretchLoop
	bcf	IIC_TRIS_SCL, IIC_PIN_SCL
#endif
	return	; 2 cycles for the return

;; ---
IIC_Delay_600ns
	;; 2 cycles for the call
	nop		; 1 cycle for the nop
	;; 2 cycles for the return
	;; one for the following instruction
	return

;; ---
IIC_Delay_1300ns
	;; 2 cycles for the call
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	;; 2 cycles for the return
	;; one for the following instruction
	return

;; ---
IIC_ByteSend
	movwf	IIC_BUFFER			; store value
	movlw	0x08				; loop 8 times
	movwf	IIC_CTR
IIC_ByteSendLoop
	btfsc	IIC_BUFFER, 7
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
	btfss	IIC_BUFFER, 7
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
	nop						; relax
	nop
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
							; (wait > 1.3 us)
	rcall	IIC_Delay_600ns
	rlf	IIC_BUFFER, F			; shift left value
	decfsz	IIC_CTR, F			; loop
	rgoto	IIC_ByteSendLoop

	;; check acknowledge
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA = input
#if RA4_IS_OPEN_DRAIN == 0
	bsf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = input
#endif
	nop
	nop
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch

	btfsc	IIC_PORT_SDA, IIC_PIN_SDA
	bcf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE ; notify that slave is not available

	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
#if RA4_IS_OPEN_DRAIN == 0
	bcf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = output
#endif
	rgoto	IIC_Delay_1300ns

;; ==========================================================================
	END
