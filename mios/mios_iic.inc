; $Id$
;
; MIOS IIC 1st layer routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; IIC Pins (SCL/SDA)
MIOS_IIC_PORT_SCL    EQU     PORTD
MIOS_IIC_LAT_SCL     EQU     LATD
MIOS_IIC_TRIS_SCL    EQU     TRISD
MIOS_IIC_PIN_SCL     EQU     5
MIOS_IIC_PORT_SDA    EQU     PORTA
MIOS_IIC_LAT_SDA     EQU     LATA
MIOS_IIC_TRIS_SDA    EQU     TRISA
MIOS_IIC_PIN_SDA     EQU     4

;; if RA4 is not an open drain driver, set this to 0 (e.g. PIC18F4620)
;; in this version, we always disable it to get an identical behaviour
#define RA4_IS_OPEN_DRAIN 0

;; --------------------------------------------------------------------------
;;  Delay Routines
;; --------------------------------------------------------------------------
MIOS_IIC_Delay_600ns_Stretch
	;; 2 cycles for the call
	btfss	MIOS_BOX_CFG1, MIOS_BOX_CFG1_IIC_STRETCH_CLK	; 1/2 for this bit test
	return			; 2 cycles for the return

	;; if clock stretching enabled:	
	bsf	MIOS_IIC_TRIS_SCL, MIOS_IIC_PIN_SCL
MIOS_IIC_Delay_600ns_StretchLoop
	BRA_IFCLR MIOS_IIC_PORT_SCL, MIOS_IIC_PIN_SCL, ACCESS, MIOS_IIC_Delay_600ns_StretchLoop
	bcf	MIOS_IIC_TRIS_SCL, MIOS_IIC_PIN_SCL
	return

MIOS_IIC_Delay_600ns
	;; 2 cycles for the call
	nop		; 1 cycle for the nop
	;; 2 cycles for the return
	;; one for the following instruction
	return

MIOS_IIC_Delay_1300ns
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

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_Start
;;  C_DECLARATION: void MIOS_IIC_Start(void)
;;  DESCRIPTION: sends the IIC start condition (SCL=1, SDA 1->0)
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;;  C_EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;; --------------------------------------------------------------------------
MIOS_IIC_Start
	bsf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
#if RA4_IS_OPEN_DRAIN == 0
	bcf	MIOS_IIC_TRIS_SDA, MIOS_IIC_PIN_SDA	; SDA = output
#endif
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 1
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch
	bcf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 0
	rcall	MIOS_IIC_Delay_600ns
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_Stop
;;  C_DECLARATION: void MIOS_IIC_Stop(void)
;;  DESCRIPTION: sends the IIC stop condition (SCL=0->1, SDA 0->1)
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;;  C_EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;; --------------------------------------------------------------------------
MIOS_IIC_Stop
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
	bcf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 0
	rcall	MIOS_IIC_Delay_600ns
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 1
	return

;; --------------------------------------------------------------------------
;;  Check Ack
;; --------------------------------------------------------------------------
MIOS_IIC_CheckAck
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA = input
#if RA4_IS_OPEN_DRAIN == 0
	bsf	MIOS_IIC_TRIS_SDA, MIOS_IIC_PIN_SDA	; SDA = input
#endif
	nop
	nop
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch
	btfsc	MIOS_IIC_PORT_SDA, MIOS_IIC_PIN_SDA
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE ; notify that EEPROM is not available
	
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
	rcall	MIOS_IIC_Delay_1300ns
#if RA4_IS_OPEN_DRAIN == 0
	bcf	MIOS_IIC_TRIS_SDA, MIOS_IIC_PIN_SDA	; SDA = output
#endif

	;; supported since MIOS V1.9: set ZERO flag if NAK received
	iorlw	0xff
	btfss	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	andlw	0x00	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_NakSend
;;  C_DECLARATION: void MIOS_IIC_NakSend(void)
;;  DESCRIPTION: sends a NAK (not acknowledge) to the slave(s)
;;  IN:	  -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;;  C_EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;; --------------------------------------------------------------------------
MIOS_IIC_NakSend
	;; currently SCL = 0 and SDA=1
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 1
	rgoto	MIOS_IIC_AckSend_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_AckSend
;;  C_DECLARATION: void MIOS_IIC_AckSend
;;  DESCRIPTION: sends a ACK (acknowledge) to the slave(s)
;;  IN:	  -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;;  C_EXAMPLE:
;;      see MIOS_IIC_ByteSend and MIOS_IIC_ByteReceive
;; --------------------------------------------------------------------------
MIOS_IIC_AckSend
	;; currently SCL = 0 and SDA=1
	bcf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 0
MIOS_IIC_AckSend_Cont
#if RA4_IS_OPEN_DRAIN == 0
	bcf	MIOS_IIC_TRIS_SDA, MIOS_IIC_PIN_SDA	; SDA = output
#endif
	nop						; wait > 100 ns
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
	rgoto	MIOS_IIC_Delay_1300ns
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_ByteSend
;;  C_DECLARATION: void MIOS_IIC_ByteSend(unsigned char b)
;;  DESCRIPTION: sends a byte over the IIC bus and checks for acknowledge.<BR>
;;  If the slave didn't send an acknowledge, the (MIOS_BOX_STAT_)BS_AVAILABLE
;;  flag in MIOS_BOX_STAT will be cleared.
;;  IN:	  byte which should be sent in WREG
;;  C_IN: byte which should be sent in <b>
;;  OUT:  WREG==0x00 if NAK has been received, otherwise != 0x00
;;        due to compatibility reasons, MIOS_BOX_STAT.MIOS_BOX_STAT_BS_AVAILABLE set so long ACK is received
;;  C_OUT:  returns 0x00 if NAK has been received, otherwise != 0x00
;;        due to compatibility reasons, mios_box_stat.BS_AVAILABLE is set so long ACK is received
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; send 0x34, 0x56, 0x78 to the IIC slave with ID 0x12
;;
;;	call	MIOS_IIC_Start		; start IIC
;;	movlw	0x12			; send device address
;;	call	MIOS_IIC_ByteSend	; bit #0 cleared to notify a write!!!
;;	movlw	0x34			; send first data byte
;;	call	MIOS_IIC_ByteSend
;;	movlw	0x56			; send second data byte
;;	call	MIOS_IIC_ByteSend
;;	movlw	0x78			; send third data byte
;;	call	MIOS_IIC_ByteSend
;;	call	MIOS_IIC_Stop		; stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;;
;;  An enhanced example with retry can be
;;  found at the MBHP_IIC_MIDI page
;;  C_EXAMPLE:
;;
;;	// send 0x34, 0x56, 0x78 to the IIC slave with ID 0x12
;;
;;	MIOS_IIC_Start();		// start IIC
;;	MIOS_IIC_ByteSend(0x12);	// send device address
;;	                            	// bit #0 cleared to notify a write!!!
;;	MIOS_IIC_ByteSend(0x34);	// send first data byte
;;	MIOS_IIC_ByteSend(0x56);	// send second data byte
;;	MIOS_IIC_ByteSend(0x78);	// send third data byte
;;	MIOS_IIC_Stop();		// stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;;
;;  An enhanced example with retry on NAK's can be
;;  found at the MBHP_IIC_MIDI page
;; --------------------------------------------------------------------------
MIOS_IIC_ByteSend
	SET_BSR	MIOS_IIC_BUFFER
	movwf	MIOS_IIC_BUFFER, BANKED		; store value
	movlw	0x08				; loop 8 times
	movwf	MIOS_IIC_CTR, BANKED
MIOS_IIC_ByteSendLoop
	btfsc	MIOS_IIC_BUFFER, 7, BANKED
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 1
	btfss	MIOS_IIC_BUFFER, 7, BANKED
	bcf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA -> 0
	nop						; relax
	nop
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
							; (wait > 1.3 us)
	rcall	MIOS_IIC_Delay_600ns
	rlf	MIOS_IIC_BUFFER, F, BANKED		; shift left value
	decfsz	MIOS_IIC_CTR, F, BANKED			; loop
	rgoto	MIOS_IIC_ByteSendLoop
	
	rgoto	MIOS_IIC_CheckAck		; check the acknowledge

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_ByteReceive
;;  C_DECLARATION: unsigned char MIOS_IIC_ByteReceive(void)
;;  DESCRIPTION: receives a byte from a IIC slave.
;;  IN:	  -
;;  C_IN:  -
;;  OUT: received byte in WREG
;;  C_OUT: received byte in WREG
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; receive three bytes from the IIC slave with ID 0x12
;;
;;	rcall	MIOS_IIC_Start		; start IIC
;;	movlw	0x12 | 1		; send device address - 
;;	call	MIOS_IIC_ByteSend	;    set bit #0 to notify a read!!!
;;	;; don't continue if IIC device not available 
;;	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, ReadFailed
;;
;;	call	MIOS_IIC_ByteReceive	; read first byte
;;      movwf   TMP1			; save it in TMP1
;;	call	MIOS_IIC_AckSend	; send acknowledge
;;	call	MIOS_IIC_ByteReceive	; read second byte
;;      movwf   TMP2			; save it in TMP2
;;	call	MIOS_IIC_AckSend	; send acknowledge
;;	call	MIOS_IIC_ByteReceive	; read third byte
;;      movwf   TMP3			; save it in TMP3
;;  ReadFailed:
;;	call	MIOS_IIC_NakSend	; send disacknowledge!!!
;;	call	MIOS_IIC_Stop		; stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;;  C_EXAMPLE:
;;
;;	// receive three bytes from the IIC slave with ID 0x12
;;
;;	MIOS_IIC_Start();		// start IIC
;;	MIOS_IIC_ByteSend(0x12 | 1);	// send device address - 
;;	                            	//    set bit #0 to notify a read!!!
;;	// don't continue if IIC device not available 
;;	if( MIOS_BOX_STAT.BS_AVAILABLE ) {
;;	  b0 = MIOS_IIC_ByteReceive();	// read first byte
;;	  MIOS_IIC_AckSend();		// send acknowledge
;;	  b1 = MIOS_IIC_ByteReceive();	// read second byte
;;	  MIOS_IIC_AckSend();		// send acknowledge
;;	  b2 = MIOS_IIC_ByteReceive();	// read third byte
;;	}
;;	MIOS_IIC_NakSend();		// send disacknowledge!!!
;;	MIOS_IIC_Stop();		// stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;; --------------------------------------------------------------------------
MIOS_IIC_ByteReceive
	SET_BSR	MIOS_IIC_BUFFER
	clrf	MIOS_IIC_BUFFER, BANKED
	bsf	MIOS_IIC_LAT_SDA, MIOS_IIC_PIN_SDA	; SDA = input
#if RA4_IS_OPEN_DRAIN == 0
	bsf	MIOS_IIC_TRIS_SDA, MIOS_IIC_PIN_SDA		; SDA = input
#endif
	
	movlw	0x08					; loop 8 times
	movwf	MIOS_IIC_CTR, BANKED
MIOS_IIC_ByteReceiveLoop
	nop						; relax
	nop
	bsf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 1
	rcall	MIOS_IIC_Delay_600ns_Stretch

	rlf	MIOS_IIC_BUFFER, F, BANKED		; shift left buffer
	bcf	MIOS_IIC_BUFFER, 0, BANKED		; copy status of SDA into rightmost bit
	btfsc	MIOS_IIC_PORT_SDA, MIOS_IIC_PIN_SDA
	bsf	MIOS_IIC_BUFFER, 0, BANKED
	
	bcf	MIOS_IIC_LAT_SCL, MIOS_IIC_PIN_SCL	; SCL -> 0
							; (wait > 1.3 us)
	rcall	MIOS_IIC_Delay_600ns
	decfsz	MIOS_IIC_CTR, F, BANKED			; loop
	rgoto	MIOS_IIC_ByteReceiveLoop

	movf	MIOS_IIC_BUFFER, W, BANKED		; now copy the received value into W
	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_CtrlSet
;;  C_DECLARATION: void MIOS_IIC_CtrlSet(unsigned char ctrl)
;;  DESCRIPTION: enables the "clock stretching" like specified in 
;;  the IIC specification http://www.semiconductors.philips.com/buses/i2c/
;;  which is required for some IIC slaves which cannot service the bus
;;  immediately on a request.
;;  NOTE: if enabled, you have to add a 1k pull-up resistor to the SCL line (Pin #22 of the PIC)
;;  IN:	  WREG = 0x00: clock stretching disabled
;;        WREG = 0x01: clock stretching enabled
;;  C_IN: <ctrl> = 0x00: clock stretching disabled
;;        <ctrl> = 0x01: clock stretching enabled
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;	;; enable clock stretching
;;	movlw   0x01
;;	call    MIOS_IIC_CtrlSet
;;  C_EXAMPLE:
;;	// enable clock stretching
;;	MIOS_IIC_CtrlSet(0x01);
;; --------------------------------------------------------------------------
MIOS_IIC_CtrlSet
	bcf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_IIC_STRETCH_CLK
	btfsc	WREG, 0
	bsf	MIOS_BOX_CFG1, MIOS_BOX_CFG1_IIC_STRETCH_CLK
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_IIC_CtrlGet
;;  C_DECLARATION: unsigned char MIOS_IIC_CtrlGet(void)
;;  DESCRIPTION: returns the IIC control status
;;  IN:  -
;;  C_IN:  -
;;  OUT:  WREG[0]: clock stretching enabled
;;  C_OUT:  bit 0 if return value: clock stretching enabled
;;  USES: -
;; --------------------------------------------------------------------------
MIOS_IIC_CtrlGet
	movlw	0x00
	btfsc	MIOS_BOX_CFG1, MIOS_BOX_CFG1_IIC_STRETCH_CLK
	movlw	0x01
	andlw	0xff		; fix STATUS
	return
