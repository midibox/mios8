;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.0 #4818 (Jan 18 2008)
; This file generated Thu Nov 20 04:04:03 2008
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f452

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _enc
	global _enc_changed
	global _Init
	global _Tick
	global _Timer
	global _print_enc_values
	global _DISPLAY_Init
	global _DISPLAY_Tick
	global _MPROC_NotifyReceivedEvnt
	global _MPROC_NotifyFoundEvent
	global _MPROC_NotifyTimeout
	global _MPROC_NotifyReceivedByte
	global _SR_Service_Prepare
	global _SR_Service_Finish
	global _DIN_NotifyToggle
	global _ENC_NotifyChange
	global _AIN_NotifyChange
	global _mios_enc_pin_table

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern _MIOS_BOX_CFG0
	extern _MIOS_BOX_CFG1
	extern _MIOS_BOX_STAT
	extern _MIOS_PARAMETER1
	extern _MIOS_PARAMETER2
	extern _MIOS_PARAMETER3
	extern _PORTAbits
	extern _PORTBbits
	extern _PORTCbits
	extern _PORTDbits
	extern _PORTEbits
	extern _LATAbits
	extern _LATBbits
	extern _LATCbits
	extern _LATDbits
	extern _LATEbits
	extern _TRISAbits
	extern _TRISBbits
	extern _TRISCbits
	extern _TRISDbits
	extern _TRISEbits
	extern _PIE1bits
	extern _PIR1bits
	extern _IPR1bits
	extern _PIE2bits
	extern _PIR2bits
	extern _IPR2bits
	extern _EECON1bits
	extern _RCSTAbits
	extern _TXSTAbits
	extern _T3CONbits
	extern _CCP2CONbits
	extern _CCP1CONbits
	extern _ADCON1bits
	extern _ADCON0bits
	extern _SSPCON2bits
	extern _SSPCON1bits
	extern _SSPSTATbits
	extern _T2CONbits
	extern _T1CONbits
	extern _RCONbits
	extern _WDTCONbits
	extern _LVDCONbits
	extern _OSCCONbits
	extern _T0CONbits
	extern _STATUSbits
	extern _INTCON3bits
	extern _INTCON2bits
	extern _INTCONbits
	extern _STKPTRbits
	extern _PORTA
	extern _PORTB
	extern _PORTC
	extern _PORTD
	extern _PORTE
	extern _LATA
	extern _LATB
	extern _LATC
	extern _LATD
	extern _LATE
	extern _TRISA
	extern _TRISB
	extern _TRISC
	extern _TRISD
	extern _TRISE
	extern _PIE1
	extern _PIR1
	extern _IPR1
	extern _PIE2
	extern _PIR2
	extern _IPR2
	extern _EECON1
	extern _EECON2
	extern _EEDATA
	extern _EEADR
	extern _RCSTA
	extern _TXSTA
	extern _TXREG
	extern _RCREG
	extern _SPBRG
	extern _T3CON
	extern _TMR3L
	extern _TMR3H
	extern _CCP2CON
	extern _CCPR2L
	extern _CCPR2H
	extern _CCP1CON
	extern _CCPR1L
	extern _CCPR1H
	extern _ADCON1
	extern _ADCON0
	extern _ADRESL
	extern _ADRESH
	extern _SSPCON2
	extern _SSPCON1
	extern _SSPSTAT
	extern _SSPADD
	extern _SSPBUF
	extern _T2CON
	extern _PR2
	extern _TMR2
	extern _T1CON
	extern _TMR1L
	extern _TMR1H
	extern _RCON
	extern _WDTCON
	extern _LVDCON
	extern _OSCCON
	extern _T0CON
	extern _TMR0L
	extern _TMR0H
	extern _STATUS
	extern _FSR2L
	extern _FSR2H
	extern _PLUSW2
	extern _PREINC2
	extern _POSTDEC2
	extern _POSTINC2
	extern _INDF2
	extern _BSR
	extern _FSR1L
	extern _FSR1H
	extern _PLUSW1
	extern _PREINC1
	extern _POSTDEC1
	extern _POSTINC1
	extern _INDF1
	extern _WREG
	extern _FSR0L
	extern _FSR0H
	extern _PLUSW0
	extern _PREINC0
	extern _POSTDEC0
	extern _POSTINC0
	extern _INDF0
	extern _INTCON3
	extern _INTCON2
	extern _INTCON
	extern _PRODL
	extern _PRODH
	extern _TABLAT
	extern _TBLPTRL
	extern _TBLPTRH
	extern _TBLPTRU
	extern _PCL
	extern _PCLATH
	extern _PCLATU
	extern _STKPTR
	extern _TOSL
	extern _TOSH
	extern _TOSU
	extern _MIOS_ENC_SpeedSet
	extern _MIOS_SRIO_NumberSet
	extern _MIOS_SRIO_UpdateFrqSet
	extern _MIOS_SRIO_DebounceSet
	extern _MIOS_LCD_Clear
	extern _MIOS_LCD_CursorSet
	extern _MIOS_LCD_PrintBCD3
	extern _MIOS_LCD_PrintChar
	extern _mios_mproc_event_table
	extern _MIOS_MPROC_EVENT_TABLE
	extern _MIOS_ENC_PIN_TABLE
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PLUSW2	equ	0xfdb
PRODL	equ	0xff3


	idata
_enc_changed	db	0x00


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1
r0x04	res	1
r0x05	res	1
r0x06	res	1
r0x07	res	1
r0x08	res	1

udata_main_0	udata
_enc	res	4

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; ; Starting pCode block for absolute Ival
S_main__mios_enc_pin_table	code	0X003280
_mios_enc_pin_table:
	DB	0x00, 0x81, 0x02, 0x81, 0x04, 0x81, 0x06, 0x81, 0xff, 0xff, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	DB	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

; I code from now on!
; ; Starting pCode block
S_main__AIN_NotifyChange	code
_AIN_NotifyChange:
;	.line	169; main.c	void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	171; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__ENC_NotifyChange	code
_ENC_NotifyChange:
;	.line	158; main.c	void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x06, POSTDEC1
	MOVFF	r0x07, POSTDEC1
	MOVFF	r0x08, POSTDEC1
	MOVWF	r0x00
	MOVLW	0x02
	MOVFF	PLUSW2, r0x01
;	.line	159; main.c	if(incrementer > 0)
	MOVF	r0x01, W
	ADDLW	0x80
	ADDLW	0x7f
	BNC	_00169_DS_
;	.line	160; main.c	enc[encoder] = (127 - enc[encoder] > incrementer) ? enc[encoder] + incrementer : 127;
	MOVLW	LOW(_enc)
	ADDWF	r0x00, W
	MOVWF	r0x02
	CLRF	r0x03
	MOVLW	HIGH(_enc)
	ADDWFC	r0x03, F
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	INDF0, r0x04
	MOVFF	r0x04, r0x05
	CLRF	r0x06
	MOVF	r0x05, W
	SUBLW	0x7f
	MOVWF	r0x05
	MOVLW	0x00
	SUBFWB	r0x06, F
	MOVFF	r0x01, r0x07
	CLRF	r0x08
	BTFSC	r0x01, 7
	SETF	r0x08
	MOVF	r0x08, W
	ADDLW	0x80
	MOVWF	PRODL
	MOVF	r0x06, W
	ADDLW	0x80
	SUBWF	PRODL, W
	BNZ	_00181_DS_
	MOVF	r0x05, W
	SUBWF	r0x07, W
_00181_DS_:
	BC	_00173_DS_
	MOVF	r0x01, W
	ADDWF	r0x04, F
	BRA	_00174_DS_
_00173_DS_:
	MOVLW	0x7f
	MOVWF	r0x04
_00174_DS_:
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	r0x04, INDF0
	BRA	_00170_DS_
_00169_DS_:
;	.line	162; main.c	enc[encoder] = ((-enc[encoder]) < incrementer) ? enc[encoder] + incrementer : 0;
	MOVLW	LOW(_enc)
	ADDWF	r0x00, F
	MOVLW	HIGH(_enc)
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x02, FSR0H
	MOVFF	INDF0, r0x03
	MOVFF	r0x03, r0x04
	CLRF	r0x05
	COMF	r0x05, F
	NEGF	r0x04
	BTFSC	STATUS, 2
	INCF	r0x05, F
	MOVFF	r0x01, r0x06
	CLRF	r0x07
	BTFSC	r0x01, 7
	SETF	r0x07
	MOVF	r0x05, W
	ADDLW	0x80
	MOVWF	PRODL
	MOVF	r0x07, W
	ADDLW	0x80
	SUBWF	PRODL, W
	BNZ	_00183_DS_
	MOVF	r0x06, W
	SUBWF	r0x04, W
_00183_DS_:
	BC	_00175_DS_
	MOVF	r0x03, W
	ADDWF	r0x01, F
	BRA	_00176_DS_
_00175_DS_:
	CLRF	r0x01
_00176_DS_:
	MOVFF	r0x00, FSR0L
	MOVFF	r0x02, FSR0H
	MOVFF	r0x01, INDF0
_00170_DS_:
;	.line	163; main.c	enc_changed = 1;
	MOVLW	0x01
	BANKSEL	_enc_changed
	MOVWF	_enc_changed, B
	MOVFF	PREINC1, r0x08
	MOVFF	PREINC1, r0x07
	MOVFF	PREINC1, r0x06
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DIN_NotifyToggle	code
_DIN_NotifyToggle:
;	.line	149; main.c	void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	151; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Finish	code
_SR_Service_Finish:
;	.line	143; main.c	}
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Prepare	code
_SR_Service_Prepare:
;	.line	136; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyReceivedByte	code
_MPROC_NotifyReceivedByte:
;	.line	127; main.c	void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	129; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyTimeout	code
_MPROC_NotifyTimeout:
;	.line	122; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyFoundEvent	code
_MPROC_NotifyFoundEvent:
;	.line	112; main.c	void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	114; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyReceivedEvnt	code
_MPROC_NotifyReceivedEvnt:
;	.line	104; main.c	void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	106; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Tick	code
_DISPLAY_Tick:
	BANKSEL	_enc_changed
;	.line	97; main.c	if(enc_changed)
	MOVF	_enc_changed, W, B
	BZ	_00135_DS_
;	.line	98; main.c	print_enc_values();
	CALL	_print_enc_values
_00135_DS_:
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Init	code
_DISPLAY_Init:
;	.line	88; main.c	MIOS_LCD_Clear();	
	CALL	_MIOS_LCD_Clear
;	.line	89; main.c	print_enc_values();
	CALL	_print_enc_values
	RETURN	

; ; Starting pCode block
S_main__print_enc_values	code
_print_enc_values:
;	.line	77; main.c	void print_enc_values(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
;	.line	79; main.c	MIOS_LCD_CursorSet(0x00);
	MOVLW	0x00
	CALL	_MIOS_LCD_CursorSet
;	.line	80; main.c	for(i=0;i<4;i++){
	CLRF	r0x00
_00121_DS_:
	MOVLW	0x04
	SUBWF	r0x00, W
	BC	_00124_DS_
;	.line	81; main.c	MIOS_LCD_PrintBCD3(enc[i]);
	MOVLW	LOW(_enc)
	ADDWF	r0x00, W
	MOVWF	r0x01
	CLRF	r0x02
	MOVLW	HIGH(_enc)
	ADDWFC	r0x02, F
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	MOVFF	INDF0, r0x01
	MOVF	r0x01, W
	CALL	_MIOS_LCD_PrintBCD3
;	.line	82; main.c	MIOS_LCD_PrintChar(' ');
	MOVLW	0x20
	CALL	_MIOS_LCD_PrintChar
;	.line	80; main.c	for(i=0;i<4;i++){
	INCF	r0x00, F
	BRA	_00121_DS_
_00124_DS_:
	BANKSEL	_enc_changed
;	.line	84; main.c	enc_changed = 0;
	CLRF	_enc_changed, B
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__Timer	code
_Timer:
;	.line	69; main.c	}
	RETURN	

; ; Starting pCode block
S_main__Tick	code
_Tick:
;	.line	61; main.c	}
	RETURN	

; ; Starting pCode block
S_main__Init	code
_Init:
;	.line	43; main.c	void Init(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
;	.line	45; main.c	MIOS_SRIO_UpdateFrqSet(1); // ms
	MOVLW	0x01
	CALL	_MIOS_SRIO_UpdateFrqSet
;	.line	46; main.c	MIOS_SRIO_NumberSet(1);
	MOVLW	0x01
	CALL	_MIOS_SRIO_NumberSet
;	.line	47; main.c	MIOS_SRIO_DebounceSet(20);
	MOVLW	0x14
	CALL	_MIOS_SRIO_DebounceSet
;	.line	48; main.c	for(i=0;i<4;i++){
	CLRF	r0x00
_00105_DS_:
	MOVLW	0x04
	SUBWF	r0x00, W
	BC	_00108_DS_
;	.line	49; main.c	enc[i] = 0;
	MOVLW	LOW(_enc)
	ADDWF	r0x00, W
	MOVWF	r0x01
	CLRF	r0x02
	MOVLW	HIGH(_enc)
	ADDWFC	r0x02, F
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	MOVLW	0x00
	MOVWF	INDF0
;	.line	50; main.c	MIOS_ENC_SpeedSet(i, MIOS_ENC_SPEED_FAST, 4);
	MOVLW	0x04
	MOVWF	POSTDEC1
	MOVLW	0x02
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_ENC_SpeedSet
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	48; main.c	for(i=0;i<4;i++){
	INCF	r0x00, F
	BRA	_00105_DS_
_00108_DS_:
;	.line	52; main.c	TRISCbits.TRISC5 = 0;
	BCF	_TRISCbits, 5
;	.line	53; main.c	PORTCbits.RC5 = 1;
	BSF	_PORTCbits, 5
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	



; Statistics:
; code size:	  598 (0x0256) bytes ( 0.46%)
;           	  299 (0x012b) words
; udata size:	    4 (0x0004) bytes ( 0.31%)
; access size:	    9 (0x0009) bytes


	end
