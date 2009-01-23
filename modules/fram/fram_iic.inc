; FRAM Module IIC driver (@1MHZ)
;
; ==========================================================================
;
;  Copyright 2009 Matthias Mächler (maechler@mm-computing.ch / thismaechler@gmx.ch)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Note that all sustain/hold values are calculated in respect to a 10MIPS
; CPU operation speed. If the communicaion is unstable, increase
; FRAM_IIC_SLOWDOWN's defined value. This increases CPU cycle count with nop's
; at every time critical position in the communication protocol.
;
; Serial Clock Port/Pin
#ifndef FRAM_IIC_PORT_SCL
#define FRAM_IIC_PORT_SCL PORTC
#define FRAM_IIC_TRIS_SCL TRISC
#define FRAM_IIC_PIN_SCL 4
#endif

; Serial Data/Address Port/Pin
#ifndef FRAM_IIC_PORT_SDA
#define FRAM_IIC_PORT_SDA PORTC
#define FRAM_IIC_TRIS_SDA TRISC
#define FRAM_IIC_PIN_SDA 5
#endif

; Save mode inserts more nop's, this will slow down the communication, but
; make it saver against failure (long cables etc.). Inserts nop's between
; SDA set/SCL high, on sustains and hold's and to stretch clock hihg/low
; times. Inserts as much nop's at each stretch point as FRAM_IIC_SLOWDOWN
; is set to. Increase this value to prevent errors with slow slew rates.
; If this value will be 0, the communication will be performed at maximum 
; speed according to the Ramtron FM24C512 specs and the PIC's limitations.
#ifndef FRAM_IIC_SLOWDOWN
#define FRAM_IIC_SLOWDOWN 0
#endif

FRAM_IIC_SlowdownDelay MACRO
	FILL (nop), FRAM_IIC_SLOWDOWN ; inserts as many nop's as savemode is set to!
	ENDM 
	

; FRAM_IIC_TimeExtend extends the code with nop's to reach a given delay
; target DELAY in nS, EXCL_MIPS: the count of operations already used
; for productive code

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_Start 
;;  DESCRIPTION: performs IIC start condition
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
FRAM_IIC_Start
	bcf FRAM_IIC_TRIS_SCL,FRAM_IIC_PIN_SCL ;SCL -> Output
	bcf FRAM_IIC_TRIS_SDA,FRAM_IIC_PIN_SDA ;SDA -> Output
	bsf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 1
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	nop;sustain start min. 0.25 uS
	nop
	FRAM_IIC_SlowdownDelay
	bcf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 0
	nop;hold start min 0.25 uS
	nop
	FRAM_IIC_SlowdownDelay
	bcf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 0
	;clock must stay low for at least 0.6uS now.
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_Stop 
;;  DESCRIPTION: performs IIC stop condition
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
FRAM_IIC_Stop
	bcf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 0
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	nop;sustain stop min. 0.25 uS
	nop
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 1
	;bus free before next transmission > 0.5uS
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_AckSend 
;;  DESCRIPTION: sends a ACK to the IIC slave
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
FRAM_IIC_AckSend
	bcf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 0, ACK
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	BRA FRAM_IIC_NackEnd

;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_NakSend 
;;  DESCRIPTION: sends a NAK to the IIC slave
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
FRAM_IIC_NakSend
	bsf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA ;SDA -> 1, NAK
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	BRA FRAM_IIC_NackEnd
	
FRAM_IIC_NackEnd
	nop;clock high min 04uS
	FRAM_IIC_SlowdownDelay
	bcf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 0	
	;clock must stay low now for min. 0.6uS
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_ByteReceive 
;;  DESCRIPTION: reads 1 byte from IIC port
;; 
;;  IN:   -
;;  OUT:  received byte in WREG
;;  USES: - FRAM_IIC_BUF
;; --------------------------------------------------------------------------
FRAM_IIC_ByteReceive
	SET_BSR	FRAM_IIC_BUF
	clrf FRAM_IIC_BUF, BANKED;init buffer with 0x00
	movlw 0x08;receive 8 bits
	; bsf FRAM_IIC_PORT_SDA, FRAM_IIC_PIN_SDA ;SDA -> Input (for open drain pins)
	bsf FRAM_IIC_TRIS_SDA,FRAM_IIC_PIN_SDA ;SDA -> Input
FRAM_IIC_ReadBit
	nop
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	rlncf FRAM_IIC_BUF,F,BANKED ;left shift no carry
	FRAM_IIC_SlowdownDelay
	btfsc FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA;set bit 0 in buffer if SDA == 1
	bsf FRAM_IIC_BUF,0,BANKED
	bcf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 0
	nop
	decfsz WREG,W ; dec WREG and read next bit if not zero
	BRA FRAM_IIC_ReadBit
	;end readbit loop	
	bcf FRAM_IIC_TRIS_SDA,FRAM_IIC_PIN_SDA ;SDA -> Output
	movf FRAM_IIC_BUF,W,BANKED ; copy received byte to WREG and return
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: FRAM_IIC_ByteReceive 
;;  DESCRIPTION: reads 1 byte from IIC port
;; 
;;  IN:  Byte to send in WREG
;;  OUT: 0x00 on NAK received, 0xFF on ACK received (ZERO flag set on NAK)
;;  USES: - FRAM_IIC_BUF
;; --------------------------------------------------------------------------
FRAM_IIC_ByteSend
	SET_BSR FRAM_IIC_BUF
	movwf FRAM_IIC_BUF,BANKED;move WREG to buffer
	movlw 0x08; send 8 bits
FRAM_IIC_WriteBit
	bcf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA; SDA -> 0	
	btfsc FRAM_IIC_BUF,7,BANKED ;if bit set... 
	bsf FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA;...SDA -> 1
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	rlncf FRAM_IIC_BUF,F,BANKED;rotate left bits in buffer
	nop;clock high min 0.4uS
	nop
	FRAM_IIC_SlowdownDelay
	bcf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 0	
	decfsz WREG,W ; dec WREG and write next bit if not zero
	BRA FRAM_IIC_WriteBit	
	;end writebit loop
	nop
	nop
FRAM_IIC_CheckAck
	; bsf FRAM_IIC_PORT_SDA, FRAM_IIC_PIN_SDA ;SDA -> Input (for open drain pins)
	bsf FRAM_IIC_TRIS_SDA,FRAM_IIC_PIN_SDA ;SDA -> Input
	FRAM_IIC_SlowdownDelay
	bsf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 1
	FRAM_IIC_SlowdownDelay
	iorlw 0xFF
	btfsc FRAM_IIC_PORT_SDA,FRAM_IIC_PIN_SDA
	andlw 0x00
	bcf FRAM_IIC_PORT_SCL,FRAM_IIC_PIN_SCL ;SCL -> 0
	bcf FRAM_IIC_TRIS_SDA,FRAM_IIC_PIN_SDA ;SDA -> Output
	return

