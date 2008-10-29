;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.0 #4818 (Jan 18 2008)
; This file generated Wed Oct 29 12:45:28 2008
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f452

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _preset_load
	global _preset_store
	global _inout_map_build
	global _screen_print
	global _current_screen_set
	global _current_bus_set
	global _current_preset_set
	global _timed_function_start
	global _timed_function_stop
	global _led_flash_start
	global _led_flash_set
	global _current_screen_num
	global _current_preset_num
	global _current_bus_num
	global _current_shift_state
	global _timed_function
	global _timer_ticks_count
	global _led_flash_led
	global _led_flash_count
	global _led_flash_ticks
	global _current_preset
	global _midi_inout_map
	global _Init
	global _preset_init
	global _Timer
	global _Tick
	global _DIN_NotifyToggle
	global _MPROC_NotifyReceivedByte
	global _MPROC_NotifyReceivedEvnt
	global _DISPLAY_Init
	global _DISPLAY_Tick
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
	extern _MIOS_DOUT_PinSet
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
FSR0L equ 0xfe1 ;; normaly 0xfe9, changed by mios-gpasm
FSR0H equ 0xfe2 ;; normaly 0xfea, changed by mios-gpasm
FSR1L equ 0xfe9 ;; normaly 0xfe1, changed by mios-gpasm
FSR2L	equ	0xfd9
INDF0 equ 0xfe7 ;; normaly 0xfef, changed by mios-gpasm
POSTINC0 equ 0xfe6 ;; normaly 0xfee, changed by mios-gpasm
POSTDEC1 equ 0xfed ;; normaly 0xfe5, changed by mios-gpasm
PREINC1 equ 0xfec ;; normaly 0xfe4, changed by mios-gpasm
PLUSW2	equ	0xfdb
PRODL	equ	0xff3
PRODH	equ	0xff4


	idata
_current_screen_num	db	0x00
_current_preset_num	db	0x00
_current_bus_num	db	0x00
_current_shift_state	db	0x00
_timed_function	db	0x00
_timer_ticks_count	db	0x00, 0x00
_led_flash_led	db	0x00
_led_flash_count	db	0x00
_led_flash_ticks	db	0x00
_MPROC_NotifyReceivedByte_fx_status_1_1	db	0x00


; Internal registers
.registers udata_ovr 0x0010 ;; normaly 0x0000, changed by mios-gpasm
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

udata_main_0	udata
_current_preset	res	64

udata_main_1	udata
_midi_inout_map	res	256

udata_main_2	udata
_inout_map_build_outputs_1_1	res	32

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_main__AIN_NotifyChange	code
_AIN_NotifyChange:
;	.line	409; main.c	void AIN_NotifyChange(unsigned char pin, unsigned int pin_value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	410; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__ENC_NotifyChange	code
_ENC_NotifyChange:
;	.line	406; main.c	void ENC_NotifyChange(unsigned char encoder, char incrementer) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	407; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Finish	code
_SR_Service_Finish:
;	.line	404; main.c	}
	RETURN	

; ; Starting pCode block
S_main__SR_Service_Prepare	code
_SR_Service_Prepare:
;	.line	401; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyTimeout	code
_MPROC_NotifyTimeout:
;	.line	398; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyFoundEvent	code
_MPROC_NotifyFoundEvent:
;	.line	394; main.c	void MPROC_NotifyFoundEvent(unsigned entry, unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	395; main.c	}
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Tick	code
_DISPLAY_Tick:
;	.line	392; main.c	}
	RETURN	

; ; Starting pCode block
S_main__DISPLAY_Init	code
_DISPLAY_Init:
;	.line	389; main.c	}
	RETURN	

; ; Starting pCode block
S_main__MPROC_NotifyReceivedEvnt	code
_MPROC_NotifyReceivedEvnt:
;	.line	370; main.c	void MPROC_NotifyReceivedEvnt(unsigned char evnt0, unsigned char evnt1, unsigned char evnt2) __wparam{
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
;	.line	372; main.c	unsigned char evnt_type = evnt0 & 0xf0;
	MOVLW	0xf0
	ANDWF	r0x00, W
	MOVWF	r0x03
;	.line	373; main.c	unsigned char in_chn = evnt0 & 0x0f;
	MOVLW	0x0f
	ANDWF	r0x00, F
;	.line	374; main.c	unsigned char evnt2_send = !(evnt_type == 0xc0 || evnt_type == 0xd0);
	MOVF	r0x03, W
	XORLW	0xc0
	BZ	_00407_DS_
	MOVF	r0x03, W
	XORLW	0xd0
	BZ	_00407_DS_
	CLRF	r0x04
	BRA	_00408_DS_
_00407_DS_:
	MOVLW	0x01
	MOVWF	r0x04
_00408_DS_:
	MOVF	r0x04, W
	BSF	STATUS, 0
	TSTFSZ	WREG
	BCF	STATUS, 0
	CLRF	r0x04
	RLCF	r0x04, F
;	.line	375; main.c	for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
	CLRF	r0x05
_00400_DS_:
	MOVLW	0x10
	SUBWF	r0x05, W
	BC	_00404_DS_
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
	BZ	_00404_DS_
;	.line	376; main.c	MIOS_MIDI_TxBufferPut(evnt_type+midi_inout_map[in_chn][i]);
	MOVF	r0x03, W
	ADDWF	r0x06, F
	MOVF	r0x06, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	377; main.c	MIOS_MIDI_TxBufferPut(evnt1);
	MOVF	r0x01, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	378; main.c	if(evnt2_send)
	MOVF	r0x04, W
	BZ	_00402_DS_
;	.line	379; main.c	MIOS_MIDI_TxBufferPut(evnt2);
	MOVF	r0x02, W
	CALL	_MIOS_MIDI_TxBufferPut
_00402_DS_:
;	.line	375; main.c	for(i=0;i<16 && midi_inout_map[in_chn][i]!=-1;i++){
	INCF	r0x05, F
	BRA	_00400_DS_
_00404_DS_:
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
;	.line	342; main.c	void MPROC_NotifyReceivedByte(unsigned char byte) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVWF	r0x00
;	.line	344; main.c	if(byte >= 0xf0){//system status byte
	MOVLW	0xf0
	SUBWF	r0x00, W
	BTG	STATUS, 0
	CLRF	r0x01
	RLCF	r0x01, F
	MOVF	r0x01, W
	BTFSS	STATUS, 2
	BRA	_00379_DS_
;	.line	345; main.c	MIOS_MIDI_TxBufferPut(byte);
	MOVF	r0x00, W
	CALL	_MIOS_MIDI_TxBufferPut
;	.line	347; main.c	switch(byte){
	MOVF	r0x01, W
	BNZ	_00372_DS_
	MOVLW	0xf4
	SUBWF	r0x00, W
	BC	_00372_DS_
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
	MOVLW	UPPER(_00391_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00391_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00391_DS_)
	ADDWF	r0x02, F
	MOVF	r0x03, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x02, W
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVWF	PCL
_00391_DS_:
	GOTO	_00371_DS_
	GOTO	_00369_DS_
	GOTO	_00370_DS_
	GOTO	_00369_DS_
_00369_DS_:
;	.line	350; main.c	fx_status = 1;
	MOVLW	0x01
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	351; main.c	break; 	
	BRA	_00381_DS_
_00370_DS_:
;	.line	353; main.c	fx_status = 2;				
	MOVLW	0x02
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	354; main.c	break;
	BRA	_00381_DS_
_00371_DS_:
;	.line	356; main.c	fx_status = 0xff;
	MOVLW	0xff
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
	MOVWF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	357; main.c	break;
	BRA	_00381_DS_
_00372_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	359; main.c	fx_status = 0;
	CLRF	_MPROC_NotifyReceivedByte_fx_status_1_1, B
;	.line	360; main.c	}   
	BRA	_00381_DS_
_00379_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	362; main.c	else if(fx_status){
	MOVF	_MPROC_NotifyReceivedByte_fx_status_1_1, W, B
	BZ	_00381_DS_
;	.line	363; main.c	MIOS_MIDI_TxBufferPut(byte);
	MOVF	r0x00, W
	CALL	_MIOS_MIDI_TxBufferPut
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	364; main.c	if(fx_status!=0xff)
	MOVF	_MPROC_NotifyReceivedByte_fx_status_1_1, W, B
	XORLW	0xff
	BZ	_00381_DS_
_00393_DS_:
	BANKSEL	_MPROC_NotifyReceivedByte_fx_status_1_1
;	.line	365; main.c	fx_status--;
	DECF	_MPROC_NotifyReceivedByte_fx_status_1_1, F, B
_00381_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__DIN_NotifyToggle	code
_DIN_NotifyToggle:
;	.line	273; main.c	void DIN_NotifyToggle(unsigned char pin, unsigned char pin_value) __wparam{
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
	MOVWF	r0x00
	MOVLW	0x02
	MOVFF	PLUSW2, r0x01
	BANKSEL	_timed_function
;	.line	275; main.c	if(timed_function==1)
	MOVF	_timed_function, W, B
	XORLW	0x01
	BNZ	_00297_DS_
;	.line	276; main.c	timed_function_stop();//stop clear request on every button change
	CALL	_timed_function_stop
	BRA	_00298_DS_
_00297_DS_:
	BANKSEL	_timed_function
;	.line	277; main.c	else if(timed_function==2 && !pin_value)
	MOVF	_timed_function, W, B
	XORLW	0x02
	BNZ	_00298_DS_
	MOVF	r0x01, W
	BNZ	_00298_DS_
;	.line	278; main.c	return;//button push will only be processed when no LED's are flashing.
	GOTO	_00326_DS_
_00298_DS_:
;	.line	279; main.c	if(pin > 7 && !pin_value){//this is a value button. only handle if button is pushed
	MOVLW	0x08
	SUBWF	r0x00, W
	BTFSS	STATUS, 0
	BRA	_00323_DS_
	MOVF	r0x01, W
	BTFSS	STATUS, 2
	BRA	_00323_DS_
;	.line	280; main.c	value = pin - 8;
	MOVLW	0xf8
	ADDWF	r0x00, W
	MOVWF	r0x02
;	.line	281; main.c	switch(current_screen_num){
	MOVLW	0x04
	BANKSEL	_current_screen_num
	SUBWF	_current_screen_num, W, B
	BTFSC	STATUS, 0
	BRA	_00324_DS_
	MOVFF	r0x0b, POSTDEC1
	MOVFF	r0x0c, POSTDEC1
	CLRF	r0x0c
; removed redundant BANKSEL
	RLCF	_current_screen_num, W, B
	RLCF	r0x0c, F
	RLCF	WREG, W
	RLCF	r0x0c, F
	ANDLW	0xfc
	MOVWF	r0x0b
	MOVLW	UPPER(_00359_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00359_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00359_DS_)
	ADDWF	r0x0b, F
	MOVF	r0x0c, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x0b, W
	MOVFF	PREINC1, r0x0c
	MOVFF	PREINC1, r0x0b
	MOVWF	PCL
_00359_DS_:
	GOTO	_00299_DS_
	GOTO	_00303_DS_
	GOTO	_00304_DS_
	GOTO	_00308_DS_
_00299_DS_:
;	.line	283; main.c	if (current_shift_state & int_bit_ormask[0]){
	MOVLW	LOW(_int_bit_ormask)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_int_bit_ormask)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_int_bit_ormask)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	MOVFF	_current_shift_state, r0x05
	CLRF	r0x06
	MOVF	r0x05, W
	ANDWF	r0x03, F
	MOVF	r0x06, W
	ANDWF	r0x04, F
	MOVF	r0x03, W
	IORWF	r0x04, W
	BZ	_00301_DS_
	BANKSEL	_current_preset_num
;	.line	284; main.c	preset_store(current_preset_num);
	MOVF	_current_preset_num, W, B
	CALL	_preset_store
;	.line	285; main.c	led_flash_start(current_preset_num+8,0,50);//LED off for half a second to indicate save
	MOVLW	0x08
	BANKSEL	_current_preset_num
	ADDWF	_current_preset_num, W, B
	MOVWF	r0x03
	MOVLW	0x32
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	CALL	_led_flash_start
	MOVLW	0x02
	ADDWF	FSR1L, F
	BRA	_00324_DS_
_00301_DS_:
;	.line	288; main.c	current_preset_set(value);
	MOVF	r0x02, W
	CALL	_current_preset_set
;	.line	289; main.c	break;			
	BRA	_00324_DS_
_00303_DS_:
;	.line	291; main.c	current_bus_set(value);
	MOVF	r0x02, W
	CALL	_current_bus_set
;	.line	292; main.c	break;			
	BRA	_00324_DS_
_00304_DS_:
;	.line	294; main.c	if (current_shift_state & int_bit_ormask[2]){//add/remove input chanel
	MOVLW	LOW(_int_bit_ormask + 4)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_int_bit_ormask + 4)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_int_bit_ormask + 4)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	MOVFF	_current_shift_state, r0x05
	CLRF	r0x06
	MOVF	r0x05, W
	ANDWF	r0x03, F
	MOVF	r0x06, W
	ANDWF	r0x04, F
	MOVF	r0x03, W
	IORWF	r0x04, W
	BTFSC	STATUS, 2
	BRA	_00306_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	295; main.c	current_preset[current_bus_num].inputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x03
	MOVLW	LOW(_current_preset)
	ADDWF	r0x03, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x04
	ADDWFC	r0x04, F
	MOVFF	r0x03, r0x05
	MOVFF	r0x04, r0x06
;	.line	296; main.c	(current_preset[current_bus_num].inputs & int_bit_ormask[value])?
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	POSTINC0, r0x03
	MOVFF	INDF0, r0x04
; ;multiply lit val:0x02 by variable r0x02 and store in r0x07
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x02, W
	MOVWF	r0x07
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x07, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x09
	ADDWFC	r0x09, F
	MOVFF	r0x07, TBLPTRL
	MOVFF	r0x08, TBLPTRH
	MOVFF	r0x09, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	TBLRD*+	
	MOVFF	TABLAT, r0x08
	MOVF	r0x07, W
	ANDWF	r0x03, W
	MOVWF	r0x09
	MOVF	r0x08, W
	ANDWF	r0x04, W
	MOVWF	r0x0a
	MOVF	r0x09, W
	IORWF	r0x0a, W
	BZ	_00328_DS_
;	.line	297; main.c	(current_preset[current_bus_num].inputs ^ int_bit_ormask[value]):
	MOVF	r0x07, W
	XORWF	r0x03, W
	MOVWF	r0x09
	MOVF	r0x08, W
	XORWF	r0x04, W
	MOVWF	r0x0a
	BRA	_00329_DS_
_00328_DS_:
;	.line	298; main.c	(current_preset[current_bus_num].inputs | int_bit_ormask[value]);
	MOVF	r0x07, W
	IORWF	r0x03, W
	MOVWF	r0x09
	MOVF	r0x08, W
	IORWF	r0x04, W
	MOVWF	r0x0a
_00329_DS_:
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVFF	r0x09, POSTINC0
	MOVFF	r0x0a, INDF0
	BRA	_00307_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00306_DS_:
	BANKSEL	_current_bus_num
;	.line	301; main.c	current_preset[current_bus_num].inputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x03
	MOVLW	LOW(_current_preset)
	ADDWF	r0x03, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x04
	ADDWFC	r0x04, F
	MOVFF	r0x03, r0x05
	MOVFF	r0x04, r0x06
;	.line	302; main.c	(current_preset[current_bus_num].inputs==int_bit_ormask[value]) ? 0x0000 :int_bit_ormask[value];
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	POSTINC0, r0x03
	MOVFF	INDF0, r0x04
; ;multiply lit val:0x02 by variable r0x02 and store in r0x07
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x02, W
	MOVWF	r0x07
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x07, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x09
	ADDWFC	r0x09, F
	MOVFF	r0x07, TBLPTRL
	MOVFF	r0x08, TBLPTRH
	MOVFF	r0x09, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	TBLRD*+	
	MOVFF	TABLAT, r0x08
	MOVF	r0x03, W
	XORWF	r0x07, W
	BNZ	_00360_DS_
	MOVF	r0x04, W
	XORWF	r0x08, W
	BZ	_00361_DS_
_00360_DS_:
	BRA	_00330_DS_
_00361_DS_:
	CLRF	r0x03
	CLRF	r0x04
	BRA	_00331_DS_
_00330_DS_:
	MOVFF	r0x07, r0x03
	MOVFF	r0x08, r0x04
_00331_DS_:
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVFF	r0x03, POSTINC0
	MOVFF	r0x04, INDF0
_00307_DS_:
;	.line	304; main.c	inout_map_build();
	CALL	_inout_map_build
;	.line	305; main.c	break;			
	BRA	_00324_DS_
_00308_DS_:
;	.line	307; main.c	if (current_shift_state & int_bit_ormask[3]){//set/unset output chanel
	MOVLW	LOW(_int_bit_ormask + 6)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_int_bit_ormask + 6)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_int_bit_ormask + 6)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	MOVFF	_current_shift_state, r0x05
	CLRF	r0x06
	MOVF	r0x05, W
	ANDWF	r0x03, F
	MOVF	r0x06, W
	ANDWF	r0x04, F
	MOVF	r0x03, W
	IORWF	r0x04, W
	BTFSC	STATUS, 2
	BRA	_00310_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_current_bus_num
;	.line	308; main.c	current_preset[current_bus_num].outputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x03
	MOVLW	LOW(_current_preset)
	ADDWF	r0x03, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x04
	ADDWFC	r0x04, F
	MOVLW	0x02
	ADDWF	r0x03, F
	BTFSC	STATUS, 0
	INCF	r0x04, F
;	.line	309; main.c	(current_preset[current_bus_num].outputs & int_bit_ormask[value])?
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	POSTINC0, r0x05
	MOVFF	INDF0, r0x06
; ;multiply lit val:0x02 by variable r0x02 and store in r0x07
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x02, W
	MOVWF	r0x07
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x07, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x09
	ADDWFC	r0x09, F
	MOVFF	r0x07, TBLPTRL
	MOVFF	r0x08, TBLPTRH
	MOVFF	r0x09, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	TBLRD*+	
	MOVFF	TABLAT, r0x08
	MOVF	r0x07, W
	ANDWF	r0x05, W
	MOVWF	r0x09
	MOVF	r0x08, W
	ANDWF	r0x06, W
	MOVWF	r0x0a
	MOVF	r0x09, W
	IORWF	r0x0a, W
	BZ	_00332_DS_
;	.line	310; main.c	(current_preset[current_bus_num].outputs ^ int_bit_ormask[value]):
	MOVF	r0x07, W
	XORWF	r0x05, W
	MOVWF	r0x09
	MOVF	r0x08, W
	XORWF	r0x06, W
	MOVWF	r0x0a
	BRA	_00333_DS_
_00332_DS_:
;	.line	311; main.c	(current_preset[current_bus_num].outputs | int_bit_ormask[value]);
	MOVF	r0x07, W
	IORWF	r0x05, W
	MOVWF	r0x09
	MOVF	r0x08, W
	IORWF	r0x06, W
	MOVWF	r0x0a
_00333_DS_:
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	r0x09, POSTINC0
	MOVFF	r0x0a, INDF0
	BRA	_00311_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00310_DS_:
	BANKSEL	_current_bus_num
;	.line	314; main.c	current_preset[current_bus_num].outputs = 
	MOVF	_current_bus_num, W, B
	MULLW	0x04
	MOVFF	PRODL, r0x03
	MOVLW	LOW(_current_preset)
	ADDWF	r0x03, F
	MOVLW	HIGH(_current_preset)
	CLRF	r0x04
	ADDWFC	r0x04, F
	MOVLW	0x02
	ADDWF	r0x03, F
	BTFSC	STATUS, 0
	INCF	r0x04, F
;	.line	315; main.c	(current_preset[current_bus_num].outputs==int_bit_ormask[value]) ? 
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	POSTINC0, r0x05
	MOVFF	INDF0, r0x06
; ;multiply lit val:0x02 by variable r0x02 and store in r0x02
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BCF	STATUS, 0
	RLCF	r0x02, F
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x02, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x02, TBLPTRL
	MOVFF	r0x07, TBLPTRH
	MOVFF	r0x08, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	TBLRD*+	
	MOVFF	TABLAT, r0x07
	MOVF	r0x05, W
	XORWF	r0x02, W
	BNZ	_00362_DS_
	MOVF	r0x06, W
	XORWF	r0x07, W
	BZ	_00363_DS_
_00362_DS_:
	BRA	_00334_DS_
_00363_DS_:
;	.line	316; main.c	0x0000 :int_bit_ormask[value];
	CLRF	r0x05
	CLRF	r0x06
	BRA	_00335_DS_
_00334_DS_:
	MOVFF	r0x02, r0x05
	MOVFF	r0x07, r0x06
_00335_DS_:
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	r0x05, POSTINC0
	MOVFF	r0x06, INDF0
_00311_DS_:
;	.line	318; main.c	inout_map_build();
	CALL	_inout_map_build
;	.line	320; main.c	}
	BRA	_00324_DS_
_00323_DS_:
;	.line	322; main.c	else if(pin < 4){
	MOVLW	0x04
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	BRA	_00324_DS_
;	.line	323; main.c	if(pin_value)
	MOVF	r0x01, W
	BZ	_00318_DS_
; ;multiply lit val:0x02 by variable r0x00 and store in r0x01
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
;	.line	324; main.c	current_shift_state &= ~int_bit_ormask[pin];//remove shift flag
	BCF	STATUS, 0
	RLCF	r0x00, W
	MOVWF	r0x01
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x01, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x03
	ADDWFC	r0x03, F
	MOVFF	r0x01, TBLPTRL
	MOVFF	r0x02, TBLPTRH
	MOVFF	r0x03, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	COMF	r0x01, F
	MOVF	r0x01, W
	BANKSEL	_current_shift_state
	ANDWF	_current_shift_state, F, B
	BRA	_00324_DS_
_00318_DS_:
	BANKSEL	_current_shift_state
;	.line	326; main.c	if(!current_shift_state){
	MOVF	_current_shift_state, W, B
	BNZ	_00316_DS_
;	.line	327; main.c	current_screen_set(pin);
	MOVF	r0x00, W
	CALL	_current_screen_set
;	.line	328; main.c	if(pin < 2)//start init request countdown
	MOVLW	0x02
	SUBWF	r0x00, W
	BC	_00316_DS_
;	.line	329; main.c	timed_function_start(1);
	MOVLW	0x01
	CALL	_timed_function_start
; ;multiply lit val:0x02 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00316_DS_:
;	.line	331; main.c	current_shift_state |= int_bit_ormask[pin];//add shift flag
	BCF	STATUS, 0
	RLCF	r0x00, F
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x00, F
	MOVLW	HIGH(_int_bit_ormask)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVLW	UPPER(_int_bit_ormask)
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, TBLPTRL
	MOVFF	r0x01, TBLPTRH
	MOVFF	r0x02, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	MOVFF	_current_shift_state, r0x02
	CLRF	r0x03
	MOVF	r0x02, W
	IORWF	r0x00, F
	MOVF	r0x03, W
	IORWF	r0x01, F
	MOVF	r0x00, W
	BANKSEL	_current_shift_state
	MOVWF	_current_shift_state, B
_00324_DS_:
;	.line	334; main.c	screen_print();
	CALL	_screen_print
_00326_DS_:
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
S_main__Tick	code
_Tick:
;	.line	235; main.c	void Tick(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_timed_function
;	.line	237; main.c	if(!timed_function)
	MOVF	_timed_function, W, B
	BNZ	_00252_DS_
;	.line	238; main.c	return;
	BRA	_00270_DS_
_00252_DS_:
	BANKSEL	_timed_function
;	.line	239; main.c	if(timed_function==1 && (timer_ticks_count >= init_request_ticks)){
	MOVF	_timed_function, W, B
	XORLW	0x01
	BZ	_00283_DS_
	BRA	_00263_DS_
_00283_DS_:
	MOVLW	0x01
	BANKSEL	(_timer_ticks_count + 1)
	SUBWF	(_timer_ticks_count + 1), W, B
	BNZ	_00284_DS_
	MOVLW	0x2c
; removed redundant BANKSEL
	SUBWF	_timer_ticks_count, W, B
_00284_DS_:
	BNC	_00263_DS_
;	.line	240; main.c	timed_function_stop();
	CALL	_timed_function_stop
	BANKSEL	_current_screen_num
;	.line	241; main.c	if(!current_screen_num){//screen is preset
	MOVF	_current_screen_num, W, B
	BNZ	_00254_DS_
;	.line	242; main.c	preset_init(0);
	MOVLW	0x00
	CALL	_preset_init
;	.line	243; main.c	preset_store(0);
	MOVLW	0x00
	CALL	_preset_store
;	.line	244; main.c	preset_init(1);
	MOVLW	0x01
	CALL	_preset_init
;	.line	245; main.c	for(i=1;i<16;i++)
	MOVLW	0x01
	MOVWF	r0x00
_00266_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BC	_00269_DS_
;	.line	246; main.c	preset_store(i);
	MOVF	r0x00, W
	CALL	_preset_store
;	.line	245; main.c	for(i=1;i<16;i++)
	INCF	r0x00, F
	BRA	_00266_DS_
_00269_DS_:
;	.line	247; main.c	current_preset_set(0);
	MOVLW	0x00
	CALL	_current_preset_set
;	.line	248; main.c	current_bus_set(0);
	MOVLW	0x00
	CALL	_current_bus_set
;	.line	249; main.c	inout_map_build();
	CALL	_inout_map_build
;	.line	250; main.c	led_flash_start(0xff,5,10);//flash 5 times
	MOVLW	0x0a
	MOVWF	POSTDEC1
	MOVLW	0x05
	MOVWF	POSTDEC1
	MOVLW	0xff
	CALL	_led_flash_start
	MOVLW	0x02
	ADDWF	FSR1L, F
	BRA	_00270_DS_
_00254_DS_:
;	.line	253; main.c	preset_init(1);//empty current preset
	MOVLW	0x01
	CALL	_preset_init
;	.line	254; main.c	current_bus_set(0);
	MOVLW	0x00
	CALL	_current_bus_set
;	.line	255; main.c	led_flash_start(0xff,2,10);//flash 2 times
	MOVLW	0x0a
	MOVWF	POSTDEC1
	MOVLW	0x02
	MOVWF	POSTDEC1
	MOVLW	0xff
	CALL	_led_flash_start
	MOVLW	0x02
	ADDWF	FSR1L, F
	BRA	_00270_DS_
_00263_DS_:
	BANKSEL	_timed_function
;	.line	258; main.c	else if(timed_function==2 && (timer_ticks_count >= led_flash_ticks)){
	MOVF	_timed_function, W, B
	XORLW	0x02
	BNZ	_00270_DS_
	MOVFF	_led_flash_ticks, r0x00
	CLRF	r0x01
	MOVF	r0x01, W
	BANKSEL	(_timer_ticks_count + 1)
	SUBWF	(_timer_ticks_count + 1), W, B
	BNZ	_00288_DS_
	MOVF	r0x00, W
; removed redundant BANKSEL
	SUBWF	_timer_ticks_count, W, B
_00288_DS_:
	BNC	_00270_DS_
;	.line	259; main.c	if(led_flash_count > 1){
	MOVLW	0x02
	BANKSEL	_led_flash_count
	SUBWF	_led_flash_count, W, B
	BNC	_00257_DS_
;	.line	260; main.c	led_flash_set(led_flash_count-- & 0x01);
	MOVFF	_led_flash_count, r0x00
	DECF	_led_flash_count, F, B
	MOVLW	0x01
	ANDWF	r0x00, F
	MOVF	r0x00, W
	CALL	_led_flash_set
	BANKSEL	_timer_ticks_count
;	.line	261; main.c	timer_ticks_count = 0;
	CLRF	_timer_ticks_count, B
; removed redundant BANKSEL
	CLRF	(_timer_ticks_count + 1), B
	BRA	_00270_DS_
_00257_DS_:
;	.line	264; main.c	timed_function_stop();
	CALL	_timed_function_stop
;	.line	265; main.c	screen_print();
	CALL	_screen_print
_00270_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__led_flash_set	code
_led_flash_set:
;	.line	224; main.c	void led_flash_set(unsigned char state) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVWF	r0x00
	BANKSEL	_led_flash_led
;	.line	225; main.c	if(led_flash_led == 0xff){
	MOVF	_led_flash_led, W, B
	XORLW	0xff
	BNZ	_00237_DS_
;	.line	226; main.c	state = (state ? 0xff : 0x00);
	MOVF	r0x00, W
	BZ	_00241_DS_
	MOVLW	0xff
	MOVWF	r0x01
	BRA	_00242_DS_
_00241_DS_:
	CLRF	r0x01
_00242_DS_:
	MOVFF	r0x01, r0x00
;	.line	227; main.c	MIOS_DOUT_SRSet(0x01,state);
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	228; main.c	MIOS_DOUT_SRSet(0x02,state);
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00239_DS_
_00237_DS_:
;	.line	231; main.c	MIOS_DOUT_PinSet(led_flash_led,state);
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	BANKSEL	_led_flash_led
	MOVF	_led_flash_led, W, B
	CALL	_MIOS_DOUT_PinSet
	INCF	FSR1L, F
_00239_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__led_flash_start	code
_led_flash_start:
;	.line	216; main.c	void led_flash_start(unsigned char led,unsigned char times,unsigned char ticks) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_led_flash_led
	MOVWF	_led_flash_led, B
	MOVLW	0x02
	MOVFF	PLUSW2, r0x00
	MOVLW	0x03
	MOVFF	PLUSW2, _led_flash_ticks
;	.line	219; main.c	led_flash_count = (times<<1) + 1;
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x01
	INCF	r0x01, W
	BANKSEL	_led_flash_count
	MOVWF	_led_flash_count, B
;	.line	220; main.c	led_flash_set(0);
	MOVLW	0x00
	CALL	_led_flash_set
;	.line	221; main.c	timed_function_start(2);
	MOVLW	0x02
	CALL	_timed_function_start
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__timed_function_stop	code
_timed_function_stop:
;	.line	211; main.c	MIOS_TIMER_Stop();
	CALL	_MIOS_TIMER_Stop
	BANKSEL	_timed_function
;	.line	212; main.c	timed_function = 0;
	CLRF	_timed_function, B
	BANKSEL	_timer_ticks_count
;	.line	213; main.c	timer_ticks_count = 0;
	CLRF	_timer_ticks_count, B
; removed redundant BANKSEL
	CLRF	(_timer_ticks_count + 1), B
	RETURN	

; ; Starting pCode block
S_main__timed_function_start	code
_timed_function_start:
;	.line	204; main.c	void timed_function_start(unsigned char func) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	BANKSEL	_timed_function
	MOVWF	_timed_function, B
	BANKSEL	_timer_ticks_count
;	.line	205; main.c	timer_ticks_count=0;
	CLRF	_timer_ticks_count, B
; removed redundant BANKSEL
	CLRF	(_timer_ticks_count + 1), B
;	.line	207; main.c	MIOS_TIMER_Init(0x01,50000);
	MOVLW	0xc3
	MOVWF	POSTDEC1
	MOVLW	0x50
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_TIMER_Init
	MOVLW	0x02
	ADDWF	FSR1L, F
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__Timer	code
_Timer:
	BANKSEL	_timer_ticks_count
;	.line	200; main.c	timer_ticks_count++;
	INCF	_timer_ticks_count, F, B
	BNC	_10456_DS_
; removed redundant BANKSEL
	INCF	(_timer_ticks_count + 1), F, B
_10456_DS_:
	RETURN	

; ; Starting pCode block
S_main__current_preset_set	code
_current_preset_set:
;	.line	189; main.c	void current_preset_set(unsigned char value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVWF	r0x00
;	.line	190; main.c	current_preset_num = value;
	MOVFF	r0x00, _current_preset_num
;	.line	191; main.c	preset_load(value);
	MOVF	r0x00, W
	CALL	_preset_load
;	.line	192; main.c	inout_map_build();
	CALL	_inout_map_build
;	.line	193; main.c	MIOS_EEPROM_Write(0x01,value);
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__current_bus_set	code
_current_bus_set:
;	.line	184; main.c	void current_bus_set(unsigned char value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVWF	r0x00
;	.line	185; main.c	current_bus_num = value;
	MOVFF	r0x00, _current_bus_num
;	.line	186; main.c	MIOS_EEPROM_Write(0x02,value);
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__current_screen_set	code
_current_screen_set:
;	.line	179; main.c	void current_screen_set(unsigned char value) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	BANKSEL	_current_screen_num
	MOVWF	_current_screen_num, B
;	.line	181; main.c	MIOS_EEPROM_Write(0x00, current_screen_num);		
	MOVF	_current_screen_num, W, B
	MOVWF	POSTDEC1
	MOVLW	0x00
	CALL	_MIOS_EEPROM_Write
	INCF	FSR1L, F
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__screen_print	code
_screen_print:
;	.line	148; main.c	void screen_print(void) __wparam{
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_timed_function
;	.line	149; main.c	if(timed_function==2)
	MOVF	_timed_function, W, B
	XORLW	0x02
	BNZ	_00182_DS_
;	.line	150; main.c	return;//no screen print on LED flash
	BRA	_00194_DS_
_00182_DS_:
	BANKSEL	_current_screen_num
;	.line	152; main.c	MIOS_DOUT_SRSet(0x00,MIOS_HLP_GetBitORMask(current_screen_num));
	MOVF	_current_screen_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	154; main.c	MIOS_DOUT_SRSet(0x01,0x00);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	155; main.c	MIOS_DOUT_SRSet(0x02,0x00);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	156; main.c	switch(current_screen_num){
	MOVLW	0x04
	BANKSEL	_current_screen_num
	SUBWF	_current_screen_num, W, B
	BTFSC	STATUS, 0
	BRA	_00194_DS_
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
	MOVLW	UPPER(_00203_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00203_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00203_DS_)
	ADDWF	r0x02, F
	MOVF	r0x03, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x02, W
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVWF	PCL
_00203_DS_:
	GOTO	_00183_DS_
	GOTO	_00187_DS_
	GOTO	_00191_DS_
	GOTO	_00192_DS_
_00183_DS_:
;	.line	158; main.c	if(current_preset_num < 8)			
	MOVLW	0x08
	BANKSEL	_current_preset_num
	SUBWF	_current_preset_num, W, B
	BC	_00185_DS_
; removed redundant BANKSEL
;	.line	159; main.c	MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_preset_num));
	MOVF	_current_preset_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00194_DS_
_00185_DS_:
;	.line	161; main.c	MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_preset_num-8));				
	MOVLW	0xf8
	BANKSEL	_current_preset_num
	ADDWF	_current_preset_num, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
;	.line	162; main.c	break;
	BRA	_00194_DS_
_00187_DS_:
;	.line	164; main.c	if(current_bus_num < 8)			
	MOVLW	0x08
	BANKSEL	_current_bus_num
	SUBWF	_current_bus_num, W, B
	BC	_00189_DS_
; removed redundant BANKSEL
;	.line	165; main.c	MIOS_DOUT_SRSet(0x01,MIOS_HLP_GetBitORMask(current_bus_num));
	MOVF	_current_bus_num, W, B
	CALL	_MIOS_HLP_GetBitORMask
	MOVWF	r0x00
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	CALL	_MIOS_DOUT_SRSet
	INCF	FSR1L, F
	BRA	_00194_DS_
_00189_DS_:
;	.line	167; main.c	MIOS_DOUT_SRSet(0x02,MIOS_HLP_GetBitORMask(current_bus_num-8));							
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
;	.line	168; main.c	break;
	BRA	_00194_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00191_DS_:
	BANKSEL	_current_bus_num
;	.line	170; main.c	MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].inputs & 0x00ff));
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
;	.line	171; main.c	MIOS_DOUT_SRSet(0x02,(unsigned char)((current_preset[current_bus_num].inputs >>8) & 0x00ff));
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
;	.line	172; main.c	break;
	BRA	_00194_DS_
; ;multiply lit val:0x04 by variable _current_bus_num and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00192_DS_:
	BANKSEL	_current_bus_num
;	.line	174; main.c	MIOS_DOUT_SRSet(0x01,(unsigned char)(current_preset[current_bus_num].outputs & 0x00ff));
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
;	.line	175; main.c	MIOS_DOUT_SRSet(0x02,(unsigned char)((current_preset[current_bus_num].outputs >>8) & 0x00ff));
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
_00194_DS_:
;	.line	176; main.c	}
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	RETURN	

; ; Starting pCode block
S_main__inout_map_build	code
_inout_map_build:
;	.line	124; main.c	void inout_map_build(void) __wparam{
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
;	.line	127; main.c	for(input=0;input<16;input++)
	CLRF	r0x00
	CLRF	r0x01
_00135_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BC	_00138_DS_
;	.line	128; main.c	outputs[input] = 0;
	MOVLW	LOW(_inout_map_build_outputs_1_1)
	ADDWF	r0x01, W
	MOVWF	r0x02
	CLRF	r0x03
	MOVLW	HIGH(_inout_map_build_outputs_1_1)
	ADDWFC	r0x03, F
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVLW	0x00
	MOVWF	POSTINC0
	MOVLW	0x00
	MOVWF	INDF0
;	.line	127; main.c	for(input=0;input<16;input++)
	INCF	r0x01, F
	INCF	r0x01, F
	INCF	r0x00, F
	BRA	_00135_DS_
_00138_DS_:
;	.line	129; main.c	for(input=0;input<16;input++){
	CLRF	r0x00
_00147_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	BRA	_00174_DS_
;	.line	130; main.c	for(bus=0;bus<16;bus++){
	CLRF	r0x01
	CLRF	r0x02
_00143_DS_:
	MOVLW	0x10
	SUBWF	r0x01, W
	BTFSC	STATUS, 0
	BRA	_00149_DS_
;	.line	131; main.c	for(input=0;input<16;input++){
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, W
	MOVWF	r0x03
	CLRF	r0x04
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x04, F
	CLRF	r0x05
	CLRF	r0x06
_00139_DS_:
	MOVLW	0x10
	SUBWF	r0x05, W
	BTFSC	STATUS, 0
	BRA	_00176_DS_
;	.line	132; main.c	if(current_preset[bus].inputs & int_bit_ormask[input])
	MOVFF	r0x03, FSR0L
	MOVFF	r0x04, FSR0H
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
	BZ	_00141_DS_
;	.line	133; main.c	outputs[input] |= current_preset[bus].outputs;
	MOVLW	LOW(_inout_map_build_outputs_1_1)
	ADDWF	r0x06, W
	MOVWF	r0x07
	CLRF	r0x08
	MOVLW	HIGH(_inout_map_build_outputs_1_1)
	ADDWFC	r0x08, F
	MOVFF	r0x07, FSR0L
	MOVFF	r0x08, FSR0H
	MOVFF	POSTINC0, r0x09
	MOVFF	INDF0, r0x0a
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, W
	MOVWF	r0x0b
	CLRF	r0x0c
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x0c, F
	MOVLW	0x02
	ADDWF	r0x0b, F
	BTFSC	STATUS, 0
	INCF	r0x0c, F
	MOVFF	r0x0b, FSR0L
	MOVFF	r0x0c, FSR0H
	MOVFF	POSTINC0, r0x0b
	MOVFF	INDF0, r0x0c
	MOVF	r0x0b, W
	IORWF	r0x09, F
	MOVF	r0x0c, W
	IORWF	r0x0a, F
	MOVFF	r0x07, FSR0L
	MOVFF	r0x08, FSR0H
	MOVFF	r0x09, POSTINC0
	MOVFF	r0x0a, INDF0
_00141_DS_:
;	.line	131; main.c	for(input=0;input<16;input++){
	INCF	r0x06, F
	INCF	r0x06, F
	INCF	r0x05, F
	BRA	_00139_DS_
_00176_DS_:
	MOVFF	r0x05, r0x00
;	.line	130; main.c	for(bus=0;bus<16;bus++){
	MOVLW	0x04
	ADDWF	r0x02, F
	INCF	r0x01, F
	BRA	_00143_DS_
_00149_DS_:
;	.line	129; main.c	for(input=0;input<16;input++){
	INCF	r0x00, F
	BRA	_00147_DS_
_00174_DS_:
;	.line	137; main.c	for(input=0;input<16;input++){
	CLRF	r0x00
	CLRF	r0x01
	CLRF	r0x02
	CLRF	r0x03
	CLRF	r0x04
	CLRF	r0x05
_00155_DS_:
	MOVLW	0x10
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	BRA	_00159_DS_
;	.line	139; main.c	for(output=0;output<16;output++)
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x01, W
	MOVWF	r0x06
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x02, W
	MOVWF	r0x07
	MOVLW	LOW(_inout_map_build_outputs_1_1)
	ADDWF	r0x03, W
	MOVWF	r0x08
	CLRF	r0x09
	MOVLW	HIGH(_inout_map_build_outputs_1_1)
	ADDWFC	r0x09, F
	CLRF	r0x0a
	CLRF	r0x0b
	CLRF	r0x0c
_00151_DS_:
	MOVLW	0x10
	SUBWF	r0x0b, W
	BC	_00154_DS_
;	.line	140; main.c	if(outputs[input]&int_bit_ormask[output])
	MOVFF	r0x08, FSR0L
	MOVFF	r0x09, FSR0H
	MOVFF	POSTINC0, r0x0d
	MOVFF	INDF0, r0x0e
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x0c, W
	MOVWF	r0x0f
	CLRF	r0x10
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x10, F
	CLRF	r0x11
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x11, F
	MOVFF	r0x0f, TBLPTRL
	MOVFF	r0x10, TBLPTRH
	MOVFF	r0x11, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x0f
	TBLRD*+	
	MOVFF	TABLAT, r0x10
	MOVF	r0x0f, W
	ANDWF	r0x0d, F
	MOVF	r0x10, W
	ANDWF	r0x0e, F
	MOVF	r0x0d, W
	IORWF	r0x0e, W
	BZ	_00153_DS_
;	.line	141; main.c	midi_inout_map[input][i++]=output;
	MOVFF	r0x0a, r0x0d
	INCF	r0x0a, F
	MOVF	r0x06, W
	ADDWF	r0x0d, F
	MOVF	r0x07, W
	CLRF	r0x0e
	ADDWFC	r0x0e, F
	MOVFF	r0x0d, FSR0L
	MOVFF	r0x0e, FSR0H
	MOVFF	r0x0b, INDF0
_00153_DS_:
;	.line	139; main.c	for(output=0;output<16;output++)
	INCF	r0x0c, F
	INCF	r0x0c, F
	INCF	r0x0b, F
	BRA	_00151_DS_
_00154_DS_:
;	.line	142; main.c	midi_inout_map[input][i] = 0xff;
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x04, W
	MOVWF	r0x06
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x05, W
	MOVWF	r0x07
	MOVF	r0x06, W
	ADDWF	r0x0a, F
	MOVF	r0x07, W
	CLRF	r0x08
	ADDWFC	r0x08, F
	MOVFF	r0x0a, FSR0L
	MOVFF	r0x08, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
;	.line	137; main.c	for(input=0;input<16;input++){
	MOVLW	0x10
	ADDWF	r0x01, F
	BTFSC	STATUS, 0
	INCF	r0x02, F
	INCF	r0x03, F
	INCF	r0x03, F
	MOVLW	0x10
	ADDWF	r0x04, F
	BTFSC	STATUS, 0
	INCF	r0x05, F
	INCF	r0x00, F
	BRA	_00155_DS_
_00159_DS_:
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
	RETURN	

; ; Starting pCode block
S_main__preset_init	code
_preset_init:
;	.line	115; main.c	void preset_init(unsigned char empty) __wparam{
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
	MOVWF	r0x00
;	.line	117; main.c	for(i=0;i<16;i++){
	CLRF	r0x01
	CLRF	r0x02
	CLRF	r0x03
	CLRF	r0x04
	CLRF	r0x05
	CLRF	r0x06
	CLRF	r0x07
_00117_DS_:
	MOVLW	0x10
	SUBWF	r0x01, W
	BTFSC	STATUS, 0
	BRA	_00121_DS_
;	.line	118; main.c	current_preset[i].inputs = current_preset[i].outputs = (empty ? 0 : int_bit_ormask[i]);
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, W
	MOVWF	r0x08
	CLRF	r0x09
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x09, F
	MOVLW	LOW(_current_preset)
	ADDWF	r0x02, W
	MOVWF	r0x0a
	CLRF	r0x0b
	MOVLW	HIGH(_current_preset)
	ADDWFC	r0x0b, F
	MOVLW	0x02
	ADDWF	r0x0a, F
	BTFSC	STATUS, 0
	INCF	r0x0b, F
	MOVF	r0x00, W
	BZ	_00123_DS_
	CLRF	r0x0c
	CLRF	r0x0d
	BRA	_00124_DS_
_00123_DS_:
	MOVLW	LOW(_int_bit_ormask)
	ADDWF	r0x03, W
	MOVWF	r0x0e
	CLRF	r0x0f
	MOVLW	HIGH(_int_bit_ormask)
	ADDWFC	r0x0f, F
	CLRF	r0x10
	MOVLW	UPPER(_int_bit_ormask)
	ADDWFC	r0x10, F
	MOVFF	r0x0e, TBLPTRL
	MOVFF	r0x0f, TBLPTRH
	MOVFF	r0x10, TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x0c
	TBLRD*+	
	MOVFF	TABLAT, r0x0d
_00124_DS_:
	MOVFF	r0x0a, FSR0L
	MOVFF	r0x0b, FSR0H
	MOVFF	r0x0c, POSTINC0
	MOVFF	r0x0d, INDF0
	MOVFF	r0x08, FSR0L
	MOVFF	r0x09, FSR0H
	MOVFF	r0x0c, POSTINC0
	MOVFF	r0x0d, INDF0
;	.line	119; main.c	midi_inout_map[i][0]= (empty ? 0xff : i);
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x04, W
	MOVWF	r0x08
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x05, W
	MOVWF	r0x09
	MOVF	r0x00, W
	BZ	_00125_DS_
	MOVLW	0xff
	MOVWF	r0x0a
	BRA	_00126_DS_
_00125_DS_:
	MOVFF	r0x01, r0x0a
_00126_DS_:
	MOVFF	r0x08, FSR0L
	MOVFF	r0x09, FSR0H
	MOVFF	r0x0a, INDF0
;	.line	120; main.c	midi_inout_map[i][1]=0xff;//terminate		
	MOVLW	LOW(_midi_inout_map)
	ADDWF	r0x06, W
	MOVWF	r0x08
	MOVLW	HIGH(_midi_inout_map)
	ADDWFC	r0x07, W
	MOVWF	r0x09
	INCF	r0x08, F
	BTFSC	STATUS, 0
	INCF	r0x09, F
	MOVFF	r0x08, FSR0L
	MOVFF	r0x09, FSR0H
	MOVLW	0xff
	MOVWF	INDF0
;	.line	117; main.c	for(i=0;i<16;i++){
	MOVLW	0x04
	ADDWF	r0x02, F
	INCF	r0x03, F
	INCF	r0x03, F
	MOVLW	0x10
	ADDWF	r0x04, F
	BTFSC	STATUS, 0
	INCF	r0x05, F
	MOVLW	0x10
	ADDWF	r0x06, F
	BTFSC	STATUS, 0
	INCF	r0x07, F
	INCF	r0x01, F
	BRA	_00117_DS_
_00121_DS_:
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
S_main__preset_store	code
_preset_store:
;	.line	109; main.c	void preset_store(unsigned char num) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVWF	r0x00
;	.line	110; main.c	unsigned int addr=num;
	CLRF	r0x01
;	.line	111; main.c	addr <<=6;
	RRCF	r0x01, F
	RRCF	r0x00, F
	RRCF	r0x01, F
	RRCF	r0x00, F
	RRCF	r0x01, W
	ANDLW	0xc0
	XORWF	r0x00, W
	XORWF	r0x00, F
	XORWF	r0x00, W
	MOVWF	r0x01
;	.line	112; main.c	MIOS_BANKSTICK_WritePage(addr,current_preset);	
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

; ; Starting pCode block
S_main__preset_load	code
_preset_load:
;	.line	102; main.c	void preset_load(unsigned char num) __wparam{
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVWF	r0x00
;	.line	103; main.c	unsigned int addr=num;
	CLRF	r0x01
;	.line	104; main.c	addr <<=6;
	RRCF	r0x01, F
	RRCF	r0x00, F
	RRCF	r0x01, F
	RRCF	r0x00, F
	RRCF	r0x01, W
	ANDLW	0xc0
	XORWF	r0x00, W
	XORWF	r0x00, F
	XORWF	r0x00, W
	MOVWF	r0x01
;	.line	105; main.c	MIOS_BANKSTICK_ReadPage(addr,current_preset);
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
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_main__Init	code
_Init:
;	.line	86; main.c	MIOS_SRIO_UpdateFrqSet(3); // ms
	MOVLW	0x03
	CALL	_MIOS_SRIO_UpdateFrqSet
;	.line	87; main.c	MIOS_SRIO_NumberSet(3);//need 20 inputs / outputs 
	MOVLW	0x03
	CALL	_MIOS_SRIO_NumberSet
;	.line	88; main.c	MIOS_SRIO_DebounceSet(20);
	MOVLW	0x14
	CALL	_MIOS_SRIO_DebounceSet
;	.line	89; main.c	MIOS_BANKSTICK_CtrlSet(0x40);//stick 0, verify disabled
	MOVLW	0x40
	CALL	_MIOS_BANKSTICK_CtrlSet
;	.line	91; main.c	current_screen_num = MIOS_EEPROM_Read(0x00);
	MOVLW	0x00
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_screen_num
	MOVWF	_current_screen_num, B
;	.line	92; main.c	current_preset_num = MIOS_EEPROM_Read(0x01);
	MOVLW	0x01
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_preset_num
	MOVWF	_current_preset_num, B
;	.line	93; main.c	current_bus_num = MIOS_EEPROM_Read(0x02);
	MOVLW	0x02
	CALL	_MIOS_EEPROM_Read
	BANKSEL	_current_bus_num
	MOVWF	_current_bus_num, B
	BANKSEL	_current_preset_num
;	.line	94; main.c	preset_load(current_preset_num);
	MOVF	_current_preset_num, W, B
	CALL	_preset_load
;	.line	96; main.c	screen_print();
	CALL	_screen_print
	RETURN	

; ; Starting pCode block for Ival
	code
_int_bit_ormask:
	DB	0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00
	DB	0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08
	DB	0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x80


; Statistics:
; code size:	 4064 (0x0fe0) bytes ( 3.10%)
;           	 2032 (0x07f0) words
; udata size:	  352 (0x0160) bytes (27.50%)
; access size:	   18 (0x0012) bytes


	end
