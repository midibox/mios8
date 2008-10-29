;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.0 #4818 (Jan 18 2008)
; This file generated Wed Oct 29 12:57:16 2008
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f452

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _count
	global _run
	global _led
	global _Init
	global _Tick
	global _Timer
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
	extern _MIOS_DOUT_PinGet
	extern _MIOS_DOUT_PinSet
	extern _MIOS_DOUT_SRSet
	extern _MIOS_SRIO_NumberSet
	extern _MIOS_SRIO_UpdateFrqSet
	extern _MIOS_SRIO_DebounceSet
	extern _MIOS_LCD_Clear
	extern _MIOS_LCD_CursorSet
	extern _MIOS_LCD_PrintBCD2
	extern _MIOS_LCD_PrintCString
	extern _MIOS_TIMER_Init
	extern _MIOS_TIMER_Stop
	extern _MIOS_HLP_GetBitORMask
	extern _mios_enc_pin_table
	extern _mios_mproc_event_table
	extern _MIOS_MPROC_EVENT_TABLE
	extern _MIOS_ENC_PIN_TABLE
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
WREG	equ	0xfe8
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PLUSW2	equ	0xfdb


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1

udata_main_0	udata
_led	res	2

udata_main_1	udata
_count	res	2

udata_main_2	udata
_run	res	2

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_main__AIN_NotifyChange	code
_AIN_NotifyChange:
;	.line	197; main.c	void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	199; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__ENC_NotifyChange	code
_ENC_NotifyChange:
;	.line	190; main.c	void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	192; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DIN_NotifyToggle	code
_DIN_NotifyToggle:
;	.line	174; main.c	void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVWF	r0x00
	MOVLW	0x02
	MOVFF	PLUSW2, r0x01
;	.line	175; main.c	if(pin_value || run) 
	MOVF	r0x01, W
	BNZ	_00198_DS_
	BANKSEL	_run
	MOVF	_run, W, B
; removed redundant BANKSEL
	IORWF	(_run + 1), W, B
	BZ	_00197_DS_
_00192_DS_:
;	.line	176; main.c	return; 
	BRA	_00198_DS_
_00197_DS_:
;	.line	177; main.c	if (din_to_dout && pin < num_douts)
	MOVLW	0x20
	SUBWF	r0x00, W
	BC	_00196_DS_
;	.line	178; main.c	MIOS_DOUT_PinSet(pin,!MIOS_DOUT_PinGet(pin));
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_PinGet
	MOVWF	r0x01
	MOVF	r0x01, W
	BSF	STATUS, 0
	TSTFSZ	WREG
	BCF	STATUS, 0
	CLRF	r0x01
	RLCF	r0x01, F
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_PinSet
	INCF	FSR1L, F
_00196_DS_:
;	.line	179; main.c	MIOS_LCD_Clear();
	CALL	_MIOS_LCD_Clear
;	.line	180; main.c	MIOS_LCD_CursorSet(0x00);
	MOVLW	0x00
	CALL	_MIOS_LCD_CursorSet
;	.line	181; main.c	MIOS_LCD_PrintCString("Pin ");
	MOVLW	UPPER(__str_1)
	MOVWF	POSTDEC1
	MOVLW	HIGH(__str_1)
	MOVWF	POSTDEC1
	MOVLW	LOW(__str_1)
	CALL	_MIOS_LCD_PrintCString
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	182; main.c	MIOS_LCD_PrintBCD2(pin);
	MOVF	r0x00, W
	CALL	_MIOS_LCD_PrintBCD2
_00198_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Finish	code
_SR_Service_Finish:
;	.line	168; main.c	}
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Prepare	code
_SR_Service_Prepare:
;	.line	161; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyReceivedByte	code
_MPROC_NotifyReceivedByte:
;	.line	152; main.c	void MPROC_NotifyReceivedByte(unsigned char byte) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	154; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyTimeout	code
_MPROC_NotifyTimeout:
;	.line	147; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyFoundEvent	code
_MPROC_NotifyFoundEvent:
;	.line	137; main.c	void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	139; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyReceivedEvnt	code
_MPROC_NotifyReceivedEvnt:
;	.line	129; main.c	void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	131; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Tick	code
_DISPLAY_Tick:
;	.line	124; main.c	}
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Init	code
_DISPLAY_Init:
;	.line	112; main.c	MIOS_LCD_Clear();	
	CALL	_MIOS_LCD_Clear
;	.line	113; main.c	MIOS_LCD_CursorSet(0x00);
	MOVLW	0x00
	CALL	_MIOS_LCD_CursorSet
;	.line	114; main.c	MIOS_LCD_PrintCString("Start DOUT Test..");
	MOVLW	UPPER(__str_3)
	MOVWF	POSTDEC1
	MOVLW	HIGH(__str_3)
	MOVWF	POSTDEC1
	MOVLW	LOW(__str_3)
	CALL	_MIOS_LCD_PrintCString
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	115; main.c	MIOS_TIMER_Init(0x02,50000);//10ms
	MOVLW	0xc3
	MOVWF	POSTDEC1
	MOVLW	0x50
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_TIMER_Init
	MOVLW	0x02
	ADDWF	FSR1L, F
	RETURN	

; ; Starting pCode block
S_main__Timer	code
_Timer:
;	.line	61; main.c	void Timer(void) __wparam
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	BANKSEL	_run
;	.line	64; main.c	if(run==0){
	MOVF	_run, W, B
; removed redundant BANKSEL
	IORWF	(_run + 1), W, B
	BNZ	_00116_DS_
;	.line	65; main.c	if(count++>100){
	MOVFF	_count, r0x00
	MOVFF	(_count + 1), r0x01
	BANKSEL	_count
	INCF	_count, F, B
	BNC	_10207_DS_
; removed redundant BANKSEL
	INCF	(_count + 1), F, B
_10207_DS_:
	MOVLW	0x00
	SUBWF	r0x01, W
	BNZ	_00145_DS_
	MOVLW	0x65
	SUBWF	r0x00, W
_00145_DS_:
	BNC	_00114_DS_
;	.line	66; main.c	run = 1;
	MOVLW	0x01
	BANKSEL	_run
	MOVWF	_run, B
; removed redundant BANKSEL
	CLRF	(_run + 1), B
	BANKSEL	_count
;	.line	67; main.c	count = 0;
	CLRF	_count, B
; removed redundant BANKSEL
	CLRF	(_count + 1), B
;	.line	68; main.c	MIOS_LCD_Clear();	
	CALL	_MIOS_LCD_Clear
;	.line	69; main.c	MIOS_LCD_CursorSet(0x00);
	MOVLW	0x00
	CALL	_MIOS_LCD_CursorSet
;	.line	70; main.c	MIOS_LCD_PrintCString("DOUT PinSet ");
	MOVLW	UPPER(__str_0)
	MOVWF	POSTDEC1
	MOVLW	HIGH(__str_0)
	MOVWF	POSTDEC1
	MOVLW	LOW(__str_0)
	CALL	_MIOS_LCD_PrintCString
	MOVLW	0x02
	ADDWF	FSR1L, F
_00114_DS_:
;	.line	72; main.c	return;
	BRA	_00134_DS_
_00116_DS_:
;	.line	75; main.c	if(count++>interval){
	MOVFF	_count, r0x00
	MOVFF	(_count + 1), r0x01
	BANKSEL	_count
	INCF	_count, F, B
	BNC	_20208_DS_
; removed redundant BANKSEL
	INCF	(_count + 1), F, B
_20208_DS_:
	MOVLW	0x00
	SUBWF	r0x01, W
	BNZ	_00146_DS_
	MOVLW	0x1f
	SUBWF	r0x00, W
_00146_DS_:
	BTFSS	STATUS, 0
	BRA	_00134_DS_
	BANKSEL	_count
;	.line	76; main.c	count=0;
	CLRF	_count, B
; removed redundant BANKSEL
	CLRF	(_count + 1), B
	BANKSEL	_run
;	.line	77; main.c	if(run==2){
	MOVF	_run, W, B
	XORLW	0x02
	BNZ	_00147_DS_
; removed redundant BANKSEL
	MOVF	(_run + 1), W, B
	BZ	_00148_DS_
_00147_DS_:
	BRA	_00123_DS_
_00148_DS_:
	BANKSEL	_led
;	.line	78; main.c	if (led)
	MOVF	_led, W, B
; removed redundant BANKSEL
	IORWF	(_led + 1), W, B
	BZ	_00118_DS_
;	.line	79; main.c	MIOS_DOUT_SRSet((led-1) >> 3 & 0x0F, 0x00);
	MOVLW	0xff
; removed redundant BANKSEL
	ADDWF	_led, W, B
	MOVWF	r0x00
	MOVLW	0xff
; removed redundant BANKSEL
	ADDWFC	(_led + 1), W, B
	MOVWF	r0x01
	BCF	STATUS, 0
	RRCF	r0x01, F
	RRCF	r0x00, F
	BCF	STATUS, 0
	RRCF	r0x01, F
	RRCF	r0x00, F
	BCF	STATUS, 0
	RRCF	r0x01, F
	RRCF	r0x00, F
	MOVLW	0x0f
	ANDWF	r0x00, F
	CLRF	r0x01
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00119_DS_
_00118_DS_:
;	.line	81; main.c	MIOS_DOUT_PinSet(num_douts-1,0);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x1f
	CALL	_MIOS_DOUT_PinSet
	INCF	FSR1L, F
_00119_DS_:
;	.line	82; main.c	MIOS_DOUT_SRSet(led >> 3 & 0x0F, MIOS_HLP_GetBitORMask(led & 0x07));
	BCF	STATUS, 0
	BANKSEL	(_led + 1)
	RRCF	(_led + 1), W, B
	MOVWF	r0x01
; removed redundant BANKSEL
	RRCF	_led, W, B
	MOVWF	r0x00
	BCF	STATUS, 0
	RRCF	r0x01, F
	RRCF	r0x00, F
	BCF	STATUS, 0
	RRCF	r0x01, F
	RRCF	r0x00, F
	MOVLW	0x0f
	ANDWF	r0x00, F
	CLRF	r0x01
	MOVLW	0x07
; removed redundant BANKSEL
	ANDWF	_led, W, B
	MOVWF	r0x01
	CLRF	r0x02
	MOVF	r0x01, W
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x01
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00124_DS_
_00123_DS_:
	BANKSEL	_led
;	.line	85; main.c	if (led)
	MOVF	_led, W, B
; removed redundant BANKSEL
	IORWF	(_led + 1), W, B
	BZ	_00121_DS_
; removed redundant BANKSEL
;	.line	86; main.c	MIOS_DOUT_PinSet(led-1,0);
	MOVF	_led, W, B
	MOVWF	r0x00
	DECF	r0x00, F
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_PinSet
	INCF	FSR1L, F
_00121_DS_:
	BANKSEL	_led
;	.line	87; main.c	MIOS_DOUT_PinSet(led,1);
	MOVF	_led, W, B
	MOVWF	r0x00
	MOVLW	0x01
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_DOUT_PinSet
	INCF	FSR1L, F
_00124_DS_:
;	.line	89; main.c	MIOS_LCD_CursorSet(0x40);
	MOVLW	0x40
	CALL	_MIOS_LCD_CursorSet
;	.line	90; main.c	MIOS_LCD_PrintCString("Pin ");
	MOVLW	UPPER(__str_1)
	MOVWF	POSTDEC1
	MOVLW	HIGH(__str_1)
	MOVWF	POSTDEC1
	MOVLW	LOW(__str_1)
	CALL	_MIOS_LCD_PrintCString
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	91; main.c	MIOS_LCD_PrintBCD2(led++);
	MOVFF	_led, r0x00
	MOVFF	(_led + 1), r0x01
	BANKSEL	_led
	INCF	_led, F, B
	BNC	_30209_DS_
; removed redundant BANKSEL
	INCF	(_led + 1), F, B
_30209_DS_:
	MOVF	r0x00, W
	CALL	_MIOS_LCD_PrintBCD2
;	.line	92; main.c	if((led == num_douts)&&(!testrun_srset || (run==2))){
	CLRF	r0x00
	BANKSEL	_led
	MOVF	_led, W, B
	XORLW	0x20
	BNZ	_00153_DS_
; removed redundant BANKSEL
	MOVF	(_led + 1), W, B
	BNZ	_00153_DS_
	INCF	r0x00, F
_00153_DS_:
	MOVF	r0x00, W
	BZ	_00128_DS_
	BANKSEL	_run
	MOVF	_run, W, B
	XORLW	0x02
	BNZ	_00154_DS_
; removed redundant BANKSEL
	MOVF	(_run + 1), W, B
	BZ	_00155_DS_
_00154_DS_:
	BRA	_00128_DS_
_00155_DS_:
;	.line	93; main.c	MIOS_TIMER_Stop();
	CALL	_MIOS_TIMER_Stop
	BANKSEL	_run
;	.line	94; main.c	run=0;
	CLRF	_run, B
; removed redundant BANKSEL
	CLRF	(_run + 1), B
	BRA	_00134_DS_
_00128_DS_:
;	.line	96; main.c	else if(led == num_douts){
	MOVF	r0x00, W
	BZ	_00134_DS_
;	.line	97; main.c	run=2;
	MOVLW	0x02
	BANKSEL	_run
	MOVWF	_run, B
; removed redundant BANKSEL
	CLRF	(_run + 1), B
	BANKSEL	_led
;	.line	98; main.c	led=0;
	CLRF	_led, B
; removed redundant BANKSEL
	CLRF	(_led + 1), B
;	.line	99; main.c	MIOS_LCD_Clear();	
	CALL	_MIOS_LCD_Clear
;	.line	100; main.c	MIOS_LCD_CursorSet(0x00);
	MOVLW	0x00
	CALL	_MIOS_LCD_CursorSet
;	.line	101; main.c	MIOS_LCD_PrintCString("DOUT SRSet BitOrMask");
	MOVLW	UPPER(__str_2)
	MOVWF	POSTDEC1
	MOVLW	HIGH(__str_2)
	MOVWF	POSTDEC1
	MOVLW	LOW(__str_2)
	CALL	_MIOS_LCD_PrintCString
	MOVLW	0x02
	ADDWF	FSR1L, F
_00134_DS_:
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__Tick	code
_Tick:
;	.line	55; main.c	}
	RETURN	

; ; Starting pCode block
S_main__Init	code
_Init:
;	.line	42; main.c	MIOS_SRIO_UpdateFrqSet(3); // ms
	MOVLW	0x03
	CALL	_MIOS_SRIO_UpdateFrqSet
;	.line	43; main.c	MIOS_SRIO_NumberSet(8);
	MOVLW	0x08
	CALL	_MIOS_SRIO_NumberSet
;	.line	44; main.c	MIOS_SRIO_DebounceSet(20);
	MOVLW	0x14
	CALL	_MIOS_SRIO_DebounceSet
	BANKSEL	_led
;	.line	45; main.c	led = 0;
	CLRF	_led, B
; removed redundant BANKSEL
	CLRF	(_led + 1), B
	BANKSEL	_count
;	.line	46; main.c	count = 0;
	CLRF	_count, B
; removed redundant BANKSEL
	CLRF	(_count + 1), B
	BANKSEL	_run
;	.line	47; main.c	run=0;
	CLRF	_run, B
; removed redundant BANKSEL
	CLRF	(_run + 1), B
	RETURN	

; ; Starting pCode block
__str_0:
	DB	0x44, 0x4f, 0x55, 0x54, 0x20, 0x50, 0x69, 0x6e, 0x53, 0x65, 0x74, 0x20
	DB	0x00
; ; Starting pCode block
__str_1:
	DB	0x50, 0x69, 0x6e, 0x20, 0x00
; ; Starting pCode block
__str_2:
	DB	0x44, 0x4f, 0x55, 0x54, 0x20, 0x53, 0x52, 0x53, 0x65, 0x74, 0x20, 0x42
	DB	0x69, 0x74, 0x4f, 0x72, 0x4d, 0x61, 0x73, 0x6b, 0x00
; ; Starting pCode block
__str_3:
	DB	0x53, 0x74, 0x61, 0x72, 0x74, 0x20, 0x44, 0x4f, 0x55, 0x54, 0x20, 0x54
	DB	0x65, 0x73, 0x74, 0x2e, 0x2e, 0x00


; Statistics:
; code size:	  772 (0x0304) bytes ( 0.59%)
;           	  386 (0x0182) words
; udata size:	    6 (0x0006) bytes ( 0.47%)
; access size:	    3 (0x0003) bytes


	end
