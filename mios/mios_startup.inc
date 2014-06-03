; $Id$
;
; MIOS system startup routine
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_Init
;;  C_DECLARATION: void Init(void)
;;  DESCRIPTION: This function is called by MIOS after startup to initialize the 
;;  application
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_Tick
;;  C_DECLARATION: void Tick(void)
;;  DESCRIPTION: This function is called by MIOS in the mainloop when 
;;  nothing else is to do
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

MIOS_STARTUP
	clrwdt

	;; ------------------------------------------------------------------
	;;  initialize the SRAM
	;; ------------------------------------------------------------------
	lfsr	FSR0, 0x000
MIOS_STARTUP_ClearRAMLoop
	clrf	POSTINC0
	movlw	LOW(PIC_DERIVATIVE_RAM_SIZE)
	cpfseq	FSR0L, ACCESS
	rgoto MIOS_STARTUP_ClearRAMLoop
	movlw	HIGH(PIC_DERIVATIVE_RAM_SIZE)
	cpfseq	FSR0H, ACCESS
	rgoto MIOS_STARTUP_ClearRAMLoop

	;; ------------------------------------------------------------------
	;;  initialize the Ports
	;; ------------------------------------------------------------------
	bcf	INTCON2, NOT_RBPU	; Enable Pull-Ups of PortB

	clrf	PORTA			; Reset PortA
	clrf	PORTB			; Reset PortB
	movlw	0x40			; Reset PortC (TX Pin = 1)
	movwf	PORTC
	clrf	PORTD			; Reset PortD
	clrf	PORTE			; Reset PortE

	movlw	b'11101111'		; all PORTA pins beside if RA4 are tristate
	movwf	TRISA
#if PIC_DERIVATIVE_SET_LCD_4BIT	; for CAN derivatives, where Tx/Rx is located at RB2/RB3
	movlw	0x0f
	movwf	TRISB
#else
	clrf	TRISB			; PORTB driver enabled by default (display data port)
#endif
	movlw	b'10000000'		; Rx Pin tristate, all other outputs
	movwf	TRISC
	movlw	b'00000010'		; DIN pin tristate, all other outputs
	movwf	TRISD
	movlw	b'00000111'		; all PORTE pins are tristate
	movwf	TRISE

#if PIC_DERIVATIVE_NEW_ADC	; PIC18F4620, PIC18F4520, ...
	movlw	0x07					; PortA and E: All Pins are analog pins
	movwf	ADCON1
	movlw	(1 << ADFM) | (0 << 3) | (6 << 0)	; right justified results, ACQT=20 AD, ADCS=FOSC/64
	movwf	ADCON2
#else				; PIC18F452
	movlw	(1 << ADFM) | (1 << ADCS2) | 0x00 	; PortA and E: All Pins are analog pins
	movwf	ADCON1
#endif

#if PIC_DERIVATIVE_CMCON_INIT	; PIC18F4620, PIC18F4520, ...
	movlw	0x07		; turn off comparators
	movwf	CMCON		; (reset value of PIC18F4620/PIC18F4520, but not of PIC18F4685 and PIC18F4682
#endif

	;; ------------------------------------------------------------------
	;;  initialize the auto settings
	;; ------------------------------------------------------------------
	call	MIOS_MIDI_DeviceIDAutoSet
	call	MIOS_MIDI_InterfaceAutoSet
	call	MIOS_LCD_TypeAutoSet

	;; ------------------------------------------------------------------
	;;  initialize the MIDI interface
	;; ------------------------------------------------------------------
	call	MIOS_MIDI_Init

	;; ------------------------------------------------------------------
	;;  start the timers
	;; ------------------------------------------------------------------
	;; switch on timer0 (don't make difference on 20/40 MHz clock due to performance reasons
#if PIC_DERIVATIVE_T08BIT_INVERTED	; e.g. PIC18F4620
 	movlw	(1 << TMR0ON) | (1 << T016BIT) | 0x01	; internal clock source, 8 bit, prescaler 1:4
#else				; all others
	movlw	(1 << TMR0ON) | (1 << T08BIT) | 0x01	; internal clock source, 8 bit, prescaler 1:4
#endif
	movwf	T0CON

	;; switch on timer1
	movlw	(1 << TMR1ON)
	movwf	T1CON

	;; timer3 configured by user
	bcf	T3CON, TMR3ON

	;; ------------------------------------------------------------------
	;;  enable the interrupts
	;; ------------------------------------------------------------------
        movlw   (1 << RCIE) | (1 << TMR1IE)		; enable UART Receive and Timer1 IRQ
							;; Tx IRQ will be enabled dynamically
        movwf   PIE1
	movlw	(1 << TMR3IE)				; enable Timer3 IRQ
	movwf	PIE2

	;; make sure that USART Receive Buffer is empty
        bcf     RCSTA, CREN
        movf    RCREG, W
        bsf     RCSTA, CREN

	;; enable maskable IRQs
	;; T0IE now enabled separately by AIN driver - so long not enabled, Timer0 is free as additional resource
        movlw   (1 << GIE) | (1 << PEIE)
	;;         movlw   (1 << GIE) | (1 << PEIE) | (1 << T0IE)
        movwf   INTCON

	;; ------------------------------------------------------------------
	;;  configure the default LCD offsets
	;; ------------------------------------------------------------------
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x40
	movwf	MIOS_PARAMETER2
	movlw	0x14
	movwf	MIOS_PARAMETER3
	movlw	0x54
	call	MIOS_LCD_YAddressSet

	TABLE_ADDR MIOS_GLCD_FONT	; init font
	call	MIOS_GLCD_FontInit

	;; ------------------------------------------------------------------
	;;  call the user init routine now!
	;; ------------------------------------------------------------------
	call	USER_Init

	;; ------------------------------------------------------------------
	;;  initialize the LCD
	;; ------------------------------------------------------------------
	call	MIOS_LCD_Init

	;; ------------------------------------------------------------------
	;;  print copyright message
	;; ------------------------------------------------------------------
	;; clear display
	call	MIOS_LCD_Clear

	;; print two lines
	TABLE_ADDR MIOS_STARTUP_COPYRIGHT_0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString

	;; wait for 1.5 seconds
	movlw	6
	movwf	TMP1
MIOS_STARTUP_CopyrightWait
	clrwdt
	movlw	250
	call	MIOS_Delay
	decfsz	TMP1, F
	rgoto	MIOS_STARTUP_CopyrightWait

	call	MIOS_LCD_Clear

	;; ------------------------------------------------------------------
	;;  clear CHANGED flags of buttons
	;; ------------------------------------------------------------------
	movlw	0x10
	movwf	TMP1
	lfsr	FSR0, MIOS_SR_DIN_CHANGED_0
MIOS_STARTUP_ClearDINChngdLoop
	clrf	POSTINC0
	decfsz	TMP1, F
	rgoto	MIOS_STARTUP_ClearDINChngdLoop

	;; ------------------------------------------------------------------
	;;  clear the upper DIN state range (mainly used by the encoders)
	;; ------------------------------------------------------------------
	movlw	0x80
	movwf	TMP1
	lfsr	FSR0, MIOS_DIN_REG1_00
MIOS_STARTUP_ClearDINReg1Loop
	clrf	POSTINC0
	decfsz	TMP1, F
	rgoto	MIOS_STARTUP_ClearDINReg1Loop

	;; ------------------------------------------------------------------
	;;  clear CHANGED flags of pots
	;; ------------------------------------------------------------------
	movlw	0x40		; checked: also ok for MF driver
	movwf	TMP1
	lfsr	FSR0, MIOS_AIN_RESULT_H_00
MIOS_STARTUP_ClearPotValLoop
	bcf	POSTINC0, 7
	decfsz	TMP1, F
	rgoto	MIOS_STARTUP_ClearPotValLoop

	;; ------------------------------------------------------------------
	;;  init display counter
	;; ------------------------------------------------------------------
	movlw	0x01
	SET_BSR	MIOS_MESSAGE_CTR
	movwf	MIOS_MESSAGE_CTR, BANKED

	;; ------------------------------------------------------------------
	;;  finally branch to MIOS mainloop
	;; ------------------------------------------------------------------
	goto	MIOS_Mainloop


MIOS_STARTUP_COPYRIGHT_0 MIOS_VERSION_STR ; (defined in main.asm)
MIOS_STARTUP_COPYRIGHT_1 STRING 16, 0x40, "(C) 2014 T.Klose"
