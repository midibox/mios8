;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.0 #4818 (Jan 18 2008)
; This file generated Tue Sep 16 06:43:12 2008
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f452

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _current_screen_num
	global _current_preset_num
	global _current_bus_num
	global _current_preset
	global _midi_inout_map
	global _timer_ticks_count
	global _store_preset
	global _load_preset
	global _screen_init
	global _preset_init
	global _Init
	global _Timer
	global _MPROC_NotifyReceivedByte
	global _MPROC_NotifyReceivedEvnt
	global _DIN_NotifyToggle
	global _DISPLAY_Init
	global _DISPLAY_Tick
	global _Tick
	global _MPROC_NotifyFoundEvent
	global _MPROC_NotifyTimeout
	global _SR_Service_Prepare
	global _SR_Service_Finish
	global _ENC_NotifyChange
	global _AIN_NotifyChange
	global _int_bit_ormask

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
	extern _MIOS_MIDI_TxBufferPut
	extern _MIOS_DIN_PinGet
	extern _MIOS_DOUT_SRSet
	extern _MIOS_SRIO_NumberSet
	extern _MIOS_SRIO_UpdateFrqSet
	extern _MIOS_SRIO_DebounceSet
	extern _MIOS_EEPROM_Read
	extern _MIOS_EEPROM_Write
	extern _MIOS_BANKSTICK_CtrlSet
	extern _MIOS_BANKSTICK_WritePage
	extern _MIOS_BANKSTICK_ReadPage
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
PCL	equ	0xff9
PCLATH	equ	0xffa
PCLATU	equ	0xffb
WREG	equ	0xfe8
TBLPTRL	equ	0xff6
TBLPTRH	equ	0xff7
TBLPTRU	equ	0xff8
TABLAT	equ	0xff5
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTINC0	equ	0xfee
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PLUSW2	equ	0xfdb
PRODL	equ	0xff3
PRODH	equ	0xff4


	idata
_current_screen_num	db	0x00
_current_preset_num	db	0x00
_current_bus_num	db	0x00
_timer_ticks_count	db	0x00
_MPROC_NotifyReceivedByte_fx_status_1_1	db	0x00


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
r0x09	res	1
r0x0a	res	1
r0x0b	res	1
r0x0c	res	1
r0x0d	res	1
r0x0e	res	1
r0x0f	res	1
r0x10	res	1
r0x11	res	1
r0x12	res	1
r0x13	res	1
r0x14	res	1
r0x15	res	1

udata_main_0	udata
_current_preset	res	64

udata_main_1	udata
_midi_inout_map	res	256

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_main__AIN_NotifyChange	code
_AIN_NotifyChange:
;	.line	353; main.c	void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	354; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__ENC_NotifyChange	code
_ENC_NotifyChange:
;	.line	347; main.c	void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	348; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Finish	code
_SR_Service_Finish:
;	.line	340; main.c	}
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Prepare	code
_SR_Service_Prepare:
;	.line	334; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyTimeout	code
_MPROC_NotifyTimeout:
;	.line	327; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyFoundEvent	code
_MPROC_NotifyFoundEvent:
;	.line	319; main.c	void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	320; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__Tick	code
_Tick:
;	.line	313; main.c	}
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Tick	code
_DISPLAY_Tick:
;	.line	307; main.c	}
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Init	code
_DISPLAY_Init:
;	.line	300; main.c	}
	RETURN	

; ; Starting pCode block
S_main__DIN_NotifyToggle	code
_DIN_NotifyToggle:
;	.line	221; main.c	void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
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
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x0a, POSTDEC1
	MOVFF	r0x0b, POSTDEC1
	MOVFF	r0x0c, POSTDEC1
	MOVFF	r0x0d, POSTDEC1
	MOVFF	r0x0e, POSTDEC1
	MOVFF	r0x0f, POSTDEC1
	MOVFF	r0x10, POSTDEC1
	MOVFF	r0x11, POSTDEC1
	MOVFF	r0x12, POSTDEC1
	MOVWF	r0x00
	MOVLW	0x02
	MOVFF	PLUSW2, r0x01
;	.line	223; main.c	MIOS_TIMER_Stop();//stops the init request countdown
	CALL	_MIOS_TIMER_Stop
;	.line	224; main.c	if(pin_value) return;//button up, nothing more to do	
	MOVF	r0x01, W
	BZ	_00255_DS_
	GOTO	_00292_DS_
_00255_DS_:
;	.line	225; main.c	if(pin > 7){
	MOVLW	0x08
	SUBWF	r0x00, W
	BTFSS	STATUS, 0
	GOTO	_00278_DS_
;	.line	226; main.c	value = pin - 8;
	MOVLW	0xf8
	ADDWF	r0x00, W
	MOVWF	r0x01
;	.line	227; main.c	switch(current_screen_num){
	MOVLW	0x04
	BANKSEL	_current_screen_num
	SUBWF	_current_screen_num, W, B
	BTFSC	STATUS, 0
	GOTO	_00279_DS_
	MOVFF	r0x13, POSTDEC1
	MOVFF	r0x14, POSTDEC1
	CLRF	r0x14
; removed redundant BANKSEL
	RLCF	_current_screen_num, W, B
	RLCF	r0x14, F
	RLCF	WREG, W
	RLCF	r0x14, F
	ANDLW	0xfc
	MOVWF	r0x13
	MOVLW	UPPER(_00327_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00327_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00327_DS_)
	ADDWF	r0x13, F
	MOVF	r0x14, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x13, W
	MOVFF	PREINC1, r0x14
	MOVFF	PREINC1, r0x13
	MOVWF	PCL
_00327_DS_:
	GOTO	_00256_DS_
	GOTO	_00257_DS_
	GOTO	_00258_DS_
	GOTO	_00264_DS_
_00256_DS_:
;	.line	229; main.c	current_preset_num = value;				
	MOVFF	r0x01, _current_preset_num
	BANKSEL	_current_preset_num
;	.line	230; main.c	load_preset(current_preset_num);
	MOVF	_current_preset_num, W, B
	CALL	_load_preset
	BANKSEL	_current_preset_num
;	.line	231; main.c	MIOS_EEPROM_Write(0x01,current_preset_num);
	MOVF	_current_preset_num, W, B
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
;	.line	232; main.c	break;			
	GOTO	_00279_DS_
_00257_DS_:
;	.line	234; main.c	current_bus_num = value;
	MOVFF	r0x01, _current_bus_num
	BANKSEL	_current_preset_num
;	.line	235; main.c	MIOS_EEPROM_Write(0x03,current_preset_num);
	MOVF	_current_preset_num, W, B
	MOVWF	POSTDEC1
	MOVLW	0x03
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
;	.line	236; main.c	break;			
	GOTO	_00279_DS_
_00258_DS_:
;	.line	238; main.c	if (MIOS_DIN_PinGet(0x02)){//set/unset input chanel
	MOVLW	0x02
	CALL	_MIOS_DIN_PinGet
	MOVWF	r0x02
	MOVF	r0x02, W
	BTFSC	STATUS, 2
	BRA	_00260_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	239; main.c	current_preset[current_bus_num].inputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x02
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x03
	ADDWFC	r0x03, F
	MOVFF	r0x02, r0x04
	MOVFF	r0x03, r0x05
;	.line	240; main.c	current_preset[current_bus_num].inputs==int_bit_ormask[value] ? 0x0000 :int_bit_ormask[value];
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	POSTINC0, r0x02
	MOVFF	INDF0, r0x03
; ;multiply lit val:0x02 by variable r0x01 and store in r0x06
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x01, W
	MOVWF	r0x06
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x06, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x06, TBLPTRL
	MOVFF	r0x07, TBLPTRH
	MOVFF	r0x08, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x06
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	MOVF	r0x02, W
	XORWF	r0x06, W
	BNZ	_00328_DS_
	MOVF	r0x03, W
	XORWF	r0x07, W
	BZ	_00329_DS_
_00328_DS_:
	BRA	_00294_DS_
_00329_DS_:
	CLRF	r0x02
	CLRF	r0x03
	BRA	_00295_DS_
_00294_DS_:
	MOVFF	r0x06, r0x02
	MOVFF	r0x07, r0x03
_00295_DS_:
	MOVFF	r0x04, FSR0L
	MOVFF	r0x05, FSR0H
	MOVFF	r0x02, POSTINC0
	MOVFF	r0x03, INDF0
	BRA	_00310_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00260_DS_:
	BANKSEL	_current_bus_num
;	.line	243; main.c	current_preset[current_bus_num].inputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x02
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x03
	ADDWFC	r0x03, F
	MOVFF	r0x02, r0x04
	MOVFF	r0x03, r0x05
;	.line	244; main.c	(current_preset[current_bus_num].inputs & int_bit_ormask[value])?
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	POSTINC0, r0x02
	MOVFF	INDF0, r0x03
; ;multiply lit val:0x02 by variable r0x01 and store in r0x06
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x01, W
	MOVWF	r0x06
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x06, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x06, TBLPTRL
	MOVFF	r0x07, TBLPTRH
	MOVFF	r0x08, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x06
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	MOVF	r0x06, W
	ANDWF	r0x02, W
	MOVWF	r0x08
	MOVF	r0x07, W
	ANDWF	r0x03, W
	MOVWF	r0x09
	MOVF	r0x08, W
	IORWF	r0x09, W
	BZ	_00296_DS_
;	.line	245; main.c	(current_preset[current_bus_num].inputs ^ int_bit_ormask[value]):
	MOVF	r0x06, W
	XORWF	r0x02, W
	MOVWF	r0x08
	MOVF	r0x07, W
	XORWF	r0x03, W
	MOVWF	r0x09
	BRA	_00297_DS_
_00296_DS_:
;	.line	246; main.c	(current_preset[current_bus_num].inputs | int_bit_ormask[value]);
	MOVF	r0x06, W
	IORWF	r0x02, W
	MOVWF	r0x08
	MOVF	r0x07, W
	IORWF	r0x03, W
	MOVWF	r0x09
_00297_DS_:
	MOVFF	r0x04, FSR0L
	MOVFF	r0x05, FSR0H
	MOVFF	r0x08, POSTINC0
	MOVFF	r0x09, INDF0
; ;multiply lit val:0x10 by variable r0x01 and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00310_DS_:
;	.line	250; main.c	for(chn_out=0;chn_out<16;chn_out++){
	MOVF	r0x01, W
	MULLW	0x10
	MOVFF	PRODL, r0x02
	MOVFF	PRODH, r0x03
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x02, F
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x03, F
	CLRF	r0x04
	CLRF	r0x05
	CLRF	r0x06
_00280_DS_:
	MOVLW	0x10
	SUBWF	r0x05, W
	BTFSC	STATUS, 0
	BRA	_00283_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x07
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	251; main.c	if(current_preset[current_bus_num].outputs & int_bit_ormask[chn_out])//this is a bus output
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x07
	MOVLW	LOW(_current_preset)
	ADDWF	r0x07, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVLW	0x02
	ADDWF	r0x07, F
	BTFSC	STATUS, 0
	INCF	r0x08, F
	MOVFF	r0x07, FSR0L
	MOVFF	r0x08, FSR0H
	MOVFF	POSTINC0, r0x07
	MOVFF	INDF0, r0x08
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x06, W
	MOVWF	r0x09
	CLRF	r0x0a
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x0a, F
	CLRF	r0x0b
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x0b, F
	MOVFF	r0x09, TBLPTRL
	MOVFF	r0x0a, TBLPTRH
	MOVFF	r0x0b, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x09
	TBLRD*+	
	MOVFF	TABLAT, r0x0a
	MOVF	r0x09, W
	ANDWF	r0x07, F
	MOVF	r0x0a, W
	ANDWF	r0x08, F
	MOVF	r0x07, W
	IORWF	r0x08, W
	BZ	_00282_DS_
;	.line	252; main.c	midi_inout_map[value][i++] = chn_out;
	MOVFF	r0x04, r0x07
	INCF	r0x04, F
	MOVF	r0x02, W
	ADDWF	r0x07, F
	MOVF	r0x03, W
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x07, FSR0L
	MOVFF	r0x08, FSR0H
	MOVFF	r0x05, INDF0
_00282_DS_:
;	.line	250; main.c	for(chn_out=0;chn_out<16;chn_out++){
	INCF	r0x06, F
	INCF	r0x06, F
	INCF	r0x05, F
	BRA	_00280_DS_
_00283_DS_:
;	.line	254; main.c	midi_inout_map[value][i] = -1;
	MOVF	r0x04, W
	ADDWF	r0x02, F
	CLRF	WREG
	ADDWFC	r0x03, F
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
;	.line	255; main.c	break;			
	BRA	_00279_DS_
_00264_DS_:
;	.line	257; main.c	if (MIOS_DIN_PinGet(0x03)){//set/unset output chanel
	MOVLW	0x03
	CALL	_MIOS_DIN_PinGet
	MOVWF	r0x02
	MOVF	r0x02, W
	BTFSC	STATUS, 2
	BRA	_00266_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	258; main.c	current_preset[current_bus_num].outputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x02
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x03
	ADDWFC	r0x03, F
	MOVLW	0x02
	ADDWF	r0x02, F
	BTFSC	STATUS, 0
	INCF	r0x03, F
;	.line	259; main.c	(current_preset[current_bus_num].outputs==int_bit_ormask[value]) ? 
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	POSTINC0, r0x04
	MOVFF	INDF0, r0x05
; ;multiply lit val:0x02 by variable r0x01 and store in r0x06
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x01, W
	MOVWF	r0x06
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x06, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x06, TBLPTRL
	MOVFF	r0x07, TBLPTRH
	MOVFF	r0x08, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x06
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	MOVF	r0x04, W
	XORWF	r0x06, W
	BNZ	_00331_DS_
	MOVF	r0x05, W
	XORWF	r0x07, W
	BZ	_00332_DS_
_00331_DS_:
	BRA	_00298_DS_
_00332_DS_:
;	.line	260; main.c	0x0000 :int_bit_ormask[value];
	CLRF	r0x04
	CLRF	r0x05
	BRA	_00299_DS_
_00298_DS_:
	MOVFF	r0x06, r0x04
	MOVFF	r0x07, r0x05
_00299_DS_:
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	r0x04, POSTINC0
	MOVFF	r0x05, INDF0
	BRA	_00319_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00266_DS_:
	BANKSEL	_current_bus_num
;	.line	263; main.c	current_preset[current_bus_num].outputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x02
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x03
	ADDWFC	r0x03, F
	MOVLW	0x02
	ADDWF	r0x02, F
	BTFSC	STATUS, 0
	INCF	r0x03, F
;	.line	264; main.c	(current_preset[current_bus_num].outputs & int_bit_ormask[value])?
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	POSTINC0, r0x04
	MOVFF	INDF0, r0x05
; ;multiply lit val:0x02 by variable r0x01 and store in r0x01
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x01, F
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x01, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x06
	ADDWFC	r0x06, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVFF	r0x01, TBLPTRL
	MOVFF	r0x06, TBLPTRH
	MOVFF	r0x07, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x06
	MOVF	r0x01, W
	ANDWF	r0x04, W
	MOVWF	r0x07
	MOVF	r0x06, W
	ANDWF	r0x05, W
	MOVWF	r0x08
	MOVF	r0x07, W
	IORWF	r0x08, W
	BZ	_00300_DS_
;	.line	265; main.c	(current_preset[current_bus_num].outputs ^ int_bit_ormask[value]):
	MOVF	r0x01, W
	XORWF	r0x04, W
	MOVWF	r0x07
	MOVF	r0x06, W
	XORWF	r0x05, W
	MOVWF	r0x08
	BRA	_00301_DS_
_00300_DS_:
;	.line	266; main.c	(current_preset[current_bus_num].outputs | int_bit_ormask[value]);
	MOVF	r0x01, W
	IORWF	r0x04, W
	MOVWF	r0x07
	MOVF	r0x06, W
	IORWF	r0x05, W
	MOVWF	r0x08
_00301_DS_:
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	r0x07, POSTINC0
	MOVFF	r0x08, INDF0
_00319_DS_:
;	.line	269; main.c	for(chn_in=0;chn_in<16;chn_in++){
	CLRF	r0x01
	CLRF	r0x02
	CLRF	r0x03
	CLRF	r0x04
	CLRF	r0x05
	CLRF	r0x06
_00288_DS_:
	MOVLW	0x10
	SUBWF	r0x01, W
	BTFSC	STATUS, 0
	BRA	_00279_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x07
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	270; main.c	if(current_preset[current_bus_num].inputs & int_bit_ormask[chn_in]){//this is a bus input
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x07
	MOVLW	LOW(_current_preset)
	ADDWF	r0x07, W
	MOVWF	r0x08
	CLRF	r0x09
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x09, F
	MOVFF	r0x08, FSR0L
	MOVFF	r0x09, FSR0H
	MOVFF	POSTINC0, r0x08
	MOVFF	INDF0, r0x09
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x06, W
	MOVWF	r0x0a
	CLRF	r0x0b
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x0b, F
	CLRF	r0x0c
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x0c, F
	MOVFF	r0x0a, TBLPTRL
	MOVFF	r0x0b, TBLPTRH
	MOVFF	r0x0c, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x0a
	TBLRD*+	
	MOVFF	TABLAT, r0x0b
	MOVF	r0x0a, W
	ANDWF	r0x08, F
	MOVF	r0x0b, W
	ANDWF	r0x09, F
	MOVF	r0x08, W
	IORWF	r0x09, W
	BTFSC	STATUS, 2
	BRA	_00290_DS_
;	.line	272; main.c	for(chn_out=0;chn_out<16;chn_out++){
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x02, W
	MOVWF	r0x08
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x03, W
	MOVWF	r0x09
	MOVLW	LOW(_current_preset)
	ADDWF	r0x07, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x0a
	ADDWFC	r0x0a, F
	MOVLW	0x02
	ADDWF	r0x07, F
	BTFSC	STATUS, 0
	INCF	r0x0a, F
	CLRF	r0x0b
	CLRF	r0x0c
	CLRF	r0x0d
_00284_DS_:
	MOVLW	0x10
	SUBWF	r0x0c, W
	BC	_00287_DS_
;	.line	273; main.c	if(current_preset[current_bus_num].outputs & int_bit_ormask[chn_out])//this is a bus output
	MOVFF	r0x07, FSR0L
	MOVFF	r0x0a, FSR0H
	MOVFF	POSTINC0, r0x0e
	MOVFF	INDF0, r0x0f
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x0d, W
	MOVWF	r0x10
	CLRF	r0x11
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x11, F
	CLRF	r0x12
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x12, F
	MOVFF	r0x10, TBLPTRL
	MOVFF	r0x11, TBLPTRH
	MOVFF	r0x12, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x10
	TBLRD*+	
	MOVFF	TABLAT, r0x11
	MOVF	r0x10, W
	ANDWF	r0x0e, F
	MOVF	r0x11, W
	ANDWF	r0x0f, F
	MOVF	r0x0e, W
	IORWF	r0x0f, W
	BZ	_00286_DS_
;	.line	274; main.c	midi_inout_map[chn_in][i++] = chn_out;
	MOVFF	r0x0b, r0x0e
	INCF	r0x0b, F
	MOVF	r0x08, W
	ADDWF	r0x0e, F
	MOVF	r0x09, W
	CLRF	r0x0f
	ADDWFC	r0x0f, F
	MOVFF	r0x0e, FSR0L
	MOVFF	r0x0f, FSR0H
	MOVFF	r0x0c, INDF0
_00286_DS_:
;	.line	272; main.c	for(chn_out=0;chn_out<16;chn_out++){
	INCF	r0x0d, F
	INCF	r0x0d, F
	INCF	r0x0c, F
	BRA	_00284_DS_
_00287_DS_:
;	.line	276; main.c	midi_inout_map[chn_in][i] = -1;
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x04, W
	MOVWF	r0x07
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x05, W
	MOVWF	r0x08
	MOVF	r0x07, W
	ADDWF	r0x0b, F
	MOVF	r0x08, W
	CLRF	r0x09
	ADDWFC	r0x09, F
	MOVFF	r0x0b, FSR0L
	MOVFF	r0x09, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
_00290_DS_:
;	.line	269; main.c	for(chn_in=0;chn_in<16;chn_in++){
	MOVLW	0x10
	ADDWF	r0x02, F
	BTFSC	STATUS, 0
	INCF	r0x03, F
	MOVLW	0x10
	ADDWF	r0x04, F
	BTFSC	STATUS, 0
	INCF	r0x05, F
	INCF	r0x06, F
	INCF	r0x06, F
	INCF	r0x01, F
	BRA	_00288_DS_
_00278_DS_:
;	.line	282; main.c	else if(pin < 4){
	MOVLW	0x04
	SUBWF	r0x00, W
	BC	_00279_DS_
;	.line	283; main.c	current_screen_num = pin;
	MOVFF	r0x00, _current_screen_num
	BANKSEL	_current_screen_num
;	.line	284; main.c	MIOS_EEPROM_Write(0x00, current_screen_num);
	MOVF	_current_screen_num, W, B
	MOVWF	POSTDEC1
	MOVLW	0x00
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
;	.line	285; main.c	if(pin < 2){//start init request countdown
	MOVLW	0x02
	SUBWF	r0x00, W
	BC	_00279_DS_
	BANKSEL	_timer_ticks_count
;	.line	286; main.c	timer_ticks_count = 0;			
	CLRF	_timer_ticks_count, B
;	.line	287; main.c	MIOS_TIMER_Init(0x03,0xffff);
	MOVLW	0xff
	MOVWF	POSTDEC1
	MOVLW	0xff
	MOVWF	POSTDEC1
	MOVLW	0x03
	CALL	_MIOS_TIMER_Init
	MOVLW	0x02
	ADDWF	FSR1L, F
_00279_DS_:
;	.line	290; main.c	screen_init();
	CALL	_screen_init
_00292_DS_:
	MOVFF	PREINC1, r0x12
	MOVFF	PREINC1, r0x11
	MOVFF	PREINC1, r0x10
	MOVFF	PREINC1, r0x0f
	MOVFF	PREINC1, r0x0e
	MOVFF	PREINC1, r0x0d
	MOVFF	PREINC1, r0x0c
	MOVFF	PREINC1, r0x0b
	MOVFF	PREINC1, r0x0a
	MOVFF	PREINC1, r0x09
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
S_main__MPROC_NotifyReceivedEvnt	code
_MPROC_NotifyReceivedEvnt:
;	.line	204; main.c	void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
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
	MOVWF	r0x00
	MOVLW	0x02
	MOVFF	PLUSW2, r0x01
	MOVLW	0x03
	MOVFF	PLUSW2, r0x02
;	.line	206; main.c	unsigned char evnt_type = evnt0 & 0xf0;
	MOVLW	0xf0
	ANDWF	r0x00, W
	MOVWF	r0x03
;	.line	207; main.c	unsigned char in_chn = evnt0 & 0x0f;
	MOVLW	0x0f
	ANDWF	r0x00, F
;	.line	208; main.c	unsigned char evnt2_send = !(evnt_type == 0xc0 || evnt_type == 0xd0);
	MOVF	r0x03, W
	XORLW	0xc0
	BZ	_00233_DS_
	MOVF	r0x03, W
	XORLW	0xd0
	BZ	_00233_DS_
	CLRF	r0x04
	BRA	_00234_DS_
_00233_DS_:
	MOVLW	0x01
	MOVWF	r0x04
_00234_DS_:
	MOVF	r0x04, W
	BSF	STATUS, 0
	TSTFSZ	WREG
	BCF	STATUS, 0
	CLRF	r0x04
	RLCF	r0x04, F
;	.line	209; main.c	for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
	CLRF	r0x05
_00226_DS_:
	MOVLW	0x10
	SUBWF	r0x05, W
	BC	_00230_DS_
; ;multiply lit val:0x10 by variable r0x00 and store in r0x06
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x00, W
	MULLW	0x10
	MOVFF	PRODL, r0x06
	MOVFF	PRODH, r0x07
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x06, F
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x07, F
	MOVF	r0x05, W
	ADDWF	r0x06, F
	CLRF	WREG
	ADDWFC	r0x07, F
	MOVFF	r0x06, FSR0L
	MOVFF	r0x07, FSR0H
	MOVFF	INDF0, r0x06
	MOVF	r0x06, W
	XORLW	0xff
	BZ	_00230_DS_
;	.line	210; main.c	MIOS_MIDI_TxBufferPut(evnt_type+midi_inout_map[in_chn][i]);
	MOVF	r0x03, W
	ADDWF	r0x06, F
	MOVF	r0x06, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	211; main.c	MIOS_MIDI_TxBufferPut(evnt1);
	MOVF	r0x01, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	212; main.c	if(evnt2_send)
	MOVF	r0x04, W
	BZ	_00228_DS_
;	.line	213; main.c	MIOS_MIDI_TxBufferPut(evnt2);
	MOVF	r0x02, W
	CALL	_MIOS_MIDI_TxBufferPut
_00228_DS_:
;	.line	209; main.c	for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
	INCF	r0x05, F
	BRA	_00226_DS_
_00230_DS_:
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
S_main__MPROC_NotifyReceivedByte	code
_MPROC_NotifyReceivedByte:
;	.line	173; main.c	void MPROC_NotifyReceivedByte(unsigned char byte) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVWF	r0x00
;	.line	176; main.c	if(byte >= 0xf0){//system status byte
	MOVLW	0xf0
	SUBWF	r0x00, W
	BTG	STATUS, 0
	CLRF	r0x01
	RLCF	r0x01, F
	MOVF	r0x01, W
	BTFSS	STATUS, 2
	BRA	_00205_DS_
;	.line	177; main.c	MIOS_MIDI_TxBufferPut(byte);
	MOVF	r0x00, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	179; main.c	switch(byte){
	MOVF	r0x01, W
	BNZ	_00198_DS_
	MOVLW	0xf4
	SUBWF	r0x00, W
	BC	_00198_DS_
	MOVLW	0x10
	ADDWF	r0x00, W
	MOVWF	r0x01
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	CLRF	r0x03
	RLCF	r0x01, W
	RLCF	r0x03, F
	RLCF	WREG, W
	RLCF	r0x03, F
	ANDLW	0xfc
	MOVWF	r0x02
	MOVLW	UPPER(_00217_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00217_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00217_DS_)
	ADDWF	r0x02, F
	MOVF	r0x03, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x02, W
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVWF	PCL
_00217_DS_:
	GOTO	_00197_DS_
	GOTO	_00195_DS_
	GOTO	_00196_DS_
	GOTO	_00195_DS_
_00195_DS_:
;	.line	182; main.c	fx_status = 1;
	MOVLW	0x01
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	183; main.c	break; 	
	BRA	_00207_DS_
_00196_DS_:
;	.line	185; main.c	fx_status = 2;				
	MOVLW	0x02
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	186; main.c	break;
	BRA	_00207_DS_
_00197_DS_:
;	.line	188; main.c	fx_status = 0xff;
	MOVLW	0xff
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	189; main.c	break;
	BRA	_00207_DS_
_00198_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	191; main.c	fx_status = 0;
	CLRF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	192; main.c	}   
	BRA	_00207_DS_
_00205_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	194; main.c	else if(fx_status){
	MOVF	_MPROC_NotifyReceivedByte_fx_status_1_1, W, B
	BZ	_00207_DS_
;	.line	195; main.c	MIOS_MIDI_TxBufferPut(byte);
	MOVF	r0x00, W
	CALL	_MIOS_MIDI_TxBufferPut
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	196; main.c	if(fx_status!=0xff)
	MOVF	_MPROC_NotifyReceivedByte_fx_status_1_1, W, B
	XORLW	0xff
	BZ	_00207_DS_
_00219_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	197; main.c	fx_status--;
	DECF	_MPROC_NotifyReceivedByte_fx_status_1_1, F, B
_00207_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__Timer	code
_Timer:
;	.line	147; main.c	void Timer(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	BANKSEL	_timer_ticks_count
;	.line	149; main.c	if(++timer_ticks_count >= init_request_ticks){
	INCF	_timer_ticks_count, F, B
	MOVLW	0x3c
; removed redundant BANKSEL
	SUBWF	_timer_ticks_count, W, B
	BNC	_00178_DS_
; removed redundant BANKSEL
;	.line	150; main.c	if(timer_ticks_count == init_request_ticks){
	MOVF	_timer_ticks_count, W, B
	XORLW	0x3c
	BNZ	_00170_DS_
;	.line	151; main.c	preset_init();//inits the current preset
	CALL	_preset_init
	BANKSEL	_current_preset_num
;	.line	152; main.c	if(!current_preset_num){//page is preset.. 
	MOVF	_current_preset_num, W, B
	BNZ	_00165_DS_
;	.line	153; main.c	for(i=0;i<16;i++)//inits all presets on bankstick(store reset preset)
	CLRF	r0x00
_00174_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BC	_00166_DS_
;	.line	154; main.c	store_preset(i);
	MOVF	r0x00, W
	CALL	_store_preset
;	.line	153; main.c	for(i=0;i<16;i++)//inits all presets on bankstick(store reset preset)
	INCF	r0x00, F
	BRA	_00174_DS_
_00165_DS_:
	BANKSEL	_current_preset_num
;	.line	157; main.c	store_preset(current_preset_num);//store the current (reset) preset
	MOVF	_current_preset_num, W, B
	CALL	_store_preset
_00166_DS_:
;	.line	159; main.c	MIOS_DOUT_SRSet(0x01,0xff);
	MOVLW	0xff
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	160; main.c	MIOS_DOUT_SRSet(0x02,0xff);
	MOVLW	0xff
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00178_DS_
_00170_DS_:
;	.line	162; main.c	else if(timer_ticks_count >= init_request_ticks+init_ledflash_ticks){
	MOVLW	0x43
	BANKSEL	_timer_ticks_count
	SUBWF	_timer_ticks_count, W, B
	BNC	_00178_DS_
;	.line	163; main.c	MIOS_TIMER_Stop();//stops the init request countdown
	CALL	_MIOS_TIMER_Stop
;	.line	164; main.c	screen_init();
	CALL	_screen_init
_00178_DS_:
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__Init	code
_Init:
;	.line	130; main.c	MIOS_SRIO_UpdateFrqSet(1); // ms
	MOVLW	0x01
	CALL	_MIOS_SRIO_UpdateFrqSet
;	.line	131; main.c	MIOS_SRIO_NumberSet(3);//need 20 inputs / outputs 
	MOVLW	0x03
	CALL	_MIOS_SRIO_NumberSet
;	.line	132; main.c	MIOS_SRIO_DebounceSet(10);
	MOVLW	0x0a
	CALL	_MIOS_SRIO_DebounceSet
;	.line	133; main.c	MIOS_BANKSTICK_CtrlSet(0x00);//stick 0, verify enabled
	MOVLW	0x00
	CALL	_MIOS_BANKSTICK_CtrlSet
;	.line	135; main.c	current_screen_num = MIOS_EEPROM_Read(0x00);
	MOVLW	0x00
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_screen_num
	MOVWF	_current_screen_num, B
;	.line	136; main.c	current_preset_num = MIOS_EEPROM_Read(0x01);
	MOVLW	0x01
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_preset_num
	MOVWF	_current_preset_num, B
;	.line	137; main.c	current_bus_num = MIOS_EEPROM_Read(0x02);
	MOVLW	0x02
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_bus_num
	MOVWF	_current_bus_num, B
	BANKSEL	_current_preset_num
;	.line	138; main.c	load_preset(current_preset_num);
	MOVF	_current_preset_num, W, B
	CALL	_load_preset
;	.line	140; main.c	screen_init();
	CALL	_screen_init
	RETURN	

; ; Starting pCode block
S_main__preset_init	code
_preset_init:
;	.line	115; main.c	void preset_init(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x06, POSTDEC1
	MOVFF	r0x07, POSTDEC1
	MOVFF	r0x08, POSTDEC1
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x0a, POSTDEC1
	MOVFF	r0x0b, POSTDEC1
;	.line	117; main.c	for(i=0;i<16;i++){
	CLRF	r0x00
	CLRF	r0x01
	CLRF	r0x02
	CLRF	r0x03
	CLRF	r0x04
_00152_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	BRA	_00155_DS_
;	.line	118; main.c	current_preset[i].inputs = current_preset[i].outputs = int_bit_ormask[i];
	MOVLW	LOW(_current_preset)
	ADDWF	r0x01, W
	MOVWF	r0x05
	CLRF	r0x06
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x06, F
	MOVLW	LOW(_current_preset)
	ADDWF	r0x01, W
	MOVWF	r0x07
	CLRF	r0x08
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x08, F
	MOVLW	0x02
	ADDWF	r0x07, F
	BTFSC	STATUS, 0
	INCF	r0x08, F
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x02, W
	MOVWF	r0x09
	CLRF	r0x0a
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x0a, F
	CLRF	r0x0b
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x0b, F
	MOVFF	r0x09, TBLPTRL
	MOVFF	r0x0a, TBLPTRH
	MOVFF	r0x0b, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x09
	TBLRD*+	
	MOVFF	TABLAT, r0x0a
	MOVFF	r0x07, FSR0L
	MOVFF	r0x08, FSR0H
	MOVFF	r0x09, POSTINC0
	MOVFF	r0x0a, INDF0
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVFF	r0x09, POSTINC0
	MOVFF	r0x0a, INDF0
;	.line	119; main.c	midi_inout_map[i][0]=i;
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x03, W
	MOVWF	r0x05
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x04, W
	MOVWF	r0x06
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVFF	r0x00, INDF0
;	.line	120; main.c	midi_inout_map[i][0]=-1;//terminate		
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
;	.line	117; main.c	for(i=0;i<16;i++){
	MOVLW	0x04
	ADDWF	r0x01, F
	INCF	r0x02, F
	INCF	r0x02, F
	MOVLW	0x10
	ADDWF	r0x03, F
	BTFSC	STATUS, 0
	INCF	r0x04, F
	INCF	r0x00, F
	BRA	_00152_DS_
_00155_DS_:
	BANKSEL	_current_preset_num
;	.line	122; main.c	store_preset(current_preset_num);	
	MOVF	_current_preset_num, W, B
	CALL	_store_preset
	MOVFF	PREINC1, r0x0b
	MOVFF	PREINC1, r0x0a
	MOVFF	PREINC1, r0x09
	MOVFF	PREINC1, r0x08
	MOVFF	PREINC1, r0x07
	MOVFF	PREINC1, r0x06
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__screen_init	code
_screen_init:
;	.line	85; main.c	void screen_init(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_current_screen_num
;	.line	87; main.c	MIOS_DOUT_SRSet(0x00,MIOS_HLP_GetBitORMask(current_screen_num));
	MOVF	_current_screen_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	89; main.c	MIOS_DOUT_SRSet(0x01,0x00);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	90; main.c	MIOS_DOUT_SRSet(0x02,0x00);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	91; main.c	switch(current_screen_num){
	MOVLW	0x04
	BANKSEL	_current_screen_num
	SUBWF	_current_screen_num, W, B
	BTFSC	STATUS, 0
	BRA	_00141_DS_
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	CLRF	r0x03
; removed redundant BANKSEL
	RLCF	_current_screen_num, W, B
	RLCF	r0x03, F
	RLCF	WREG, W
	RLCF	r0x03, F
	ANDLW	0xfc
	MOVWF	r0x02
	MOVLW	UPPER(_00147_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00147_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00147_DS_)
	ADDWF	r0x02, F
	MOVF	r0x03, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x02, W
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVWF	PCL
_00147_DS_:
	GOTO	_00130_DS_
	GOTO	_00134_DS_
	GOTO	_00138_DS_
	GOTO	_00139_DS_
_00130_DS_:
;	.line	93; main.c	if(current_preset_num < 8)			
	MOVLW	0x08
	BANKSEL	_current_preset_num
	SUBWF	_current_preset_num, W, B
	BC	_00132_DS_
	BANKSEL	_current_screen_num
;	.line	94; main.c	MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_screen_num));
	MOVF	_current_screen_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00141_DS_
_00132_DS_:
;	.line	96; main.c	MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_screen_num-8));				
	MOVLW	0xf8
	BANKSEL	_current_screen_num
	ADDWF	_current_screen_num, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	97; main.c	break;
	BRA	_00141_DS_
_00134_DS_:
;	.line	99; main.c	if(current_bus_num < 8)			
	MOVLW	0x08
	BANKSEL	_current_bus_num
	SUBWF	_current_bus_num, W, B
	BC	_00136_DS_
; removed redundant BANKSEL
;	.line	100; main.c	MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_bus_num));
	MOVF	_current_bus_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00141_DS_
_00136_DS_:
;	.line	102; main.c	MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_bus_num-8));							
	MOVLW	0xf8
	BANKSEL	_current_bus_num
	ADDWF	_current_bus_num, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	103; main.c	break;
	BRA	_00141_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00138_DS_:
	BANKSEL	_current_bus_num
;	.line	105; main.c	MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].inputs & 0x00ff));
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_current_preset)
	ADDWF	r0x00, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x01
	CLRF	r0x01
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	106; main.c	MIOS_DOUT_SRSet(0x02,(unsigned char)(current_preset[current_bus_num].inputs >>8 & 0x00ff));
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_current_preset)
	ADDWF	r0x00, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x01
	MOVF	r0x01, W
	MOVWF	r0x00
	CLRF	r0x01
	CLRF	r0x01
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	107; main.c	break;
	BRA	_00141_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00139_DS_:
	BANKSEL	_current_bus_num
;	.line	109; main.c	MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].outputs & 0x00ff));
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_current_preset)
	ADDWF	r0x00, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVLW	0x02
	ADDWF	r0x00, F
	BTFSC	STATUS, 0
	INCF	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x01
	CLRF	r0x01
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	110; main.c	MIOS_DOUT_SRSet(0x02,(unsigned char)(current_preset[current_bus_num].outputs >>8 & 0x00ff));
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_current_preset)
	ADDWF	r0x00, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVLW	0x02
	ADDWF	r0x00, F
	BTFSC	STATUS, 0
	INCF	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x01
	MOVF	r0x01, W
	MOVWF	r0x00
	CLRF	r0x01
	CLRF	r0x01
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
_00141_DS_:
;	.line	111; main.c	}
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__load_preset	code
_load_preset:
;	.line	67; main.c	void load_preset(unsigned char num) __wparam{
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
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x0a, POSTDEC1
	MOVFF	r0x0b, POSTDEC1
	MOVFF	r0x0c, POSTDEC1
	MOVFF	r0x0d, POSTDEC1
	MOVFF	r0x0e, POSTDEC1
	MOVFF	r0x0f, POSTDEC1
	MOVFF	r0x10, POSTDEC1
	MOVFF	r0x11, POSTDEC1
	MOVFF	r0x12, POSTDEC1
	MOVFF	r0x13, POSTDEC1
	MOVFF	r0x14, POSTDEC1
	MOVFF	r0x15, POSTDEC1
	MOVWF	r0x00
; ;multiply lit val:0x40 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
;	.line	69; main.c	MIOS_BANKSTICK_ReadPage(num*64, @current_preset);	
	MOVF	r0x00, W
	MULLW	0x40
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	MOVLW	HIGH(_current_preset)
	MOVWF	r0x03
	MOVLW	LOW(_current_preset)
	MOVWF	r0x02
	MOVLW	0x80
	MOVWF	r0x04
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_BANKSTICK_ReadPage
	MOVLW	0x04
	ADDWF	FSR1L, F
;	.line	71; main.c	for(bus=0;bus<16;bus++){
	CLRF	r0x00
	CLRF	r0x01
_00121_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	BRA	_00125_DS_
;	.line	72; main.c	for(chn_in=0;chn_in<16;chn_in++){
	MOVLW	LOW(_current_preset)
	ADDWF	r0x01, W
	MOVWF	r0x02
	CLRF	r0x03
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x03, F
	CLRF	r0x04
	CLRF	r0x05
	CLRF	r0x06
	CLRF	r0x07
	CLRF	r0x08
	CLRF	r0x09
_00117_DS_:
	MOVLW	0x10
	SUBWF	r0x04, W
	BTFSC	STATUS, 0
	BRA	_00123_DS_
;	.line	73; main.c	if(current_preset[bus].inputs & int_bit_ormask[chn_in]){				
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	POSTINC0, r0x0a
	MOVFF	INDF0, r0x0b
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x09, W
	MOVWF	r0x0c
	CLRF	r0x0d
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x0d, F
	CLRF	r0x0e
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x0e, F
	MOVFF	r0x0c, TBLPTRL
	MOVFF	r0x0d, TBLPTRH
	MOVFF	r0x0e, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x0c
	TBLRD*+	
	MOVFF	TABLAT, r0x0d
	MOVF	r0x0c, W
	ANDWF	r0x0a, F
	MOVF	r0x0d, W
	ANDWF	r0x0b, F
	MOVF	r0x0a, W
	IORWF	r0x0b, W
	BTFSC	STATUS, 2
	BRA	_00119_DS_
;	.line	75; main.c	for(chn_out=0;chn_out<16;chn_out++){
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x05, W
	MOVWF	r0x0a
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x06, W
	MOVWF	r0x0b
	MOVLW	LOW(_current_preset)
	ADDWF	r0x01, W
	MOVWF	r0x0c
	CLRF	r0x0d
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x0d, F
	MOVLW	0x02
	ADDWF	r0x0c, F
	BTFSC	STATUS, 0
	INCF	r0x0d, F
	CLRF	r0x0e
	CLRF	r0x0f
	CLRF	r0x10
_00113_DS_:
	MOVLW	0x10
	SUBWF	r0x0f, W
	BC	_00116_DS_
;	.line	76; main.c	if(current_preset[bus].outputs & int_bit_ormask[chn_out])
	MOVFF	r0x0c, FSR0L
	MOVFF	r0x0d, FSR0H
	MOVFF	POSTINC0, r0x11
	MOVFF	INDF0, r0x12
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x10, W
	MOVWF	r0x13
	CLRF	r0x14
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x14, F
	CLRF	r0x15
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x15, F
	MOVFF	r0x13, TBLPTRL
	MOVFF	r0x14, TBLPTRH
	MOVFF	r0x15, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x13
	TBLRD*+	
	MOVFF	TABLAT, r0x14
	MOVF	r0x13, W
	ANDWF	r0x11, F
	MOVF	r0x14, W
	ANDWF	r0x12, F
	MOVF	r0x11, W
	IORWF	r0x12, W
	BZ	_00115_DS_
;	.line	77; main.c	midi_inout_map[chn_in][i++] = chn_out;
	MOVFF	r0x0e, r0x11
	INCF	r0x0e, F
	MOVF	r0x0a, W
	ADDWF	r0x11, F
	MOVF	r0x0b, W
	CLRF	r0x12
	ADDWFC	r0x12, F
	MOVFF	r0x11, FSR0L
	MOVFF	r0x12, FSR0H
	MOVFF	r0x0f, INDF0
_00115_DS_:
;	.line	75; main.c	for(chn_out=0;chn_out<16;chn_out++){
	INCF	r0x10, F
	INCF	r0x10, F
	INCF	r0x0f, F
	BRA	_00113_DS_
_00116_DS_:
;	.line	79; main.c	midi_inout_map[chn_in][i] = -1;//terminate list
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x07, W
	MOVWF	r0x0a
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x08, W
	MOVWF	r0x0b
	MOVF	r0x0a, W
	ADDWF	r0x0e, F
	MOVF	r0x0b, W
	CLRF	r0x0c
	ADDWFC	r0x0c, F
	MOVFF	r0x0e, FSR0L
	MOVFF	r0x0c, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
_00119_DS_:
;	.line	72; main.c	for(chn_in=0;chn_in<16;chn_in++){
	MOVLW	0x10
	ADDWF	r0x05, F
	BTFSC	STATUS, 0
	INCF	r0x06, F
	MOVLW	0x10
	ADDWF	r0x07, F
	BTFSC	STATUS, 0
	INCF	r0x08, F
	INCF	r0x09, F
	INCF	r0x09, F
	INCF	r0x04, F
	BRA	_00117_DS_
_00123_DS_:
;	.line	71; main.c	for(bus=0;bus<16;bus++){
	MOVLW	0x04
	ADDWF	r0x01, F
	INCF	r0x00, F
	BRA	_00121_DS_
_00125_DS_:
	MOVFF	PREINC1, r0x15
	MOVFF	PREINC1, r0x14
	MOVFF	PREINC1, r0x13
	MOVFF	PREINC1, r0x12
	MOVFF	PREINC1, r0x11
	MOVFF	PREINC1, r0x10
	MOVFF	PREINC1, r0x0f
	MOVFF	PREINC1, r0x0e
	MOVFF	PREINC1, r0x0d
	MOVFF	PREINC1, r0x0c
	MOVFF	PREINC1, r0x0b
	MOVFF	PREINC1, r0x0a
	MOVFF	PREINC1, r0x09
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
S_main__store_preset	code
_store_preset:
;	.line	63; main.c	void store_preset(unsigned char num) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVWF	r0x00
; ;multiply lit val:0x40 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
;	.line	64; main.c	MIOS_BANKSTICK_WritePage(num*64, @current_preset);
	MOVF	r0x00, W
	MULLW	0x40
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	MOVLW	HIGH(_current_preset)
	MOVWF	r0x03
	MOVLW	LOW(_current_preset)
	MOVWF	r0x02
	MOVLW	0x80
	MOVWF	r0x04
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	CALL	_MIOS_BANKSTICK_WritePage
	MOVLW	0x04
	ADDWF	FSR1L, F
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block for Ival
	code
_int_bit_ormask:
	DB	0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00
	DB	0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08
	DB	0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x80


; Statistics:
; code size:	 3618 (0x0e22) bytes ( 2.76%)
;           	 1809 (0x0711) words
; udata size:	  320 (0x0140) bytes (25.00%)
; access size:	   22 (0x0016) bytes


	end
