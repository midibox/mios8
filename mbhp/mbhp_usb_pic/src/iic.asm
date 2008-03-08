; $Id$
;
;  IIC (Master) Driver
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=18f4550
	radix	dec
#include <p18f4550.inc>
#include "macros.inc"

; ==========================================================================

	;; export lables
	global	IIC_Start
	global	IIC_Stop
	global	IIC_NakSend
	global	IIC_AckSend
	global	IIC_ByteSend
	global	IIC_ByteReceive

	global	IIC_STAT

; ==========================================================================

;; optional switch to stretch the SCL
#define STRETCH_SCL	1

;; if set, two additional NOPs will be inserted into the delay functions
#define RUNNING_AT_48MHz 1

;; if RA4 is not an open drain driver, set this to 0 (PIC18F4550)
#define RA4_IS_OPEN_DRAIN 0

; ==========================================================================

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

; ==========================================================================

accessram		udata_acs
IIC_STAT		res	1
IIC_CTR			res	1
IIC_BUFFER		res	1

; ==========================================================================
IIC	code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Delay Routines
;; --------------------------------------------------------------------------
IIC_Delay_600ns_Stretch
	;; 2 cycles for the call
#if STRETCH_SCL == 0
	nop	; 2 cycles
	nop
#if RUNNING_AT_48MHz
	nop
	nop
#endif
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

IIC_Delay_600ns
	;; 2 cycles for the call
	nop		; 1 cycle for the nop
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	;; 2 cycles for the return
	;; one for the following instruction
	return

IIC_Delay_1300ns
	;; 2 cycles for the call
	nop
	nop
	nop
	nop
	nop
	nop
	nop
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	;; 2 cycles for the return
	;; one for the following instruction
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Start
;;  DESCRIPTION: sends the IIC start condition (SCL=1, SDA 1->0)
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see IIC_ByteSend and IIC_ByteReceive
;; --------------------------------------------------------------------------
IIC_Start
	bsf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE
#if RA4_IS_OPEN_DRAIN == 0
	bcf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = output
#endif
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
	rcall	IIC_Delay_600ns
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_Stop
;;  DESCRIPTION: sends the IIC stop condition (SCL=0->1, SDA 0->1)
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see IIC_ByteSend and IIC_ByteReceive
;; --------------------------------------------------------------------------
IIC_Stop
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
	rcall	IIC_Delay_600ns
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
	return

;; --------------------------------------------------------------------------
;;  Check Ack
;; --------------------------------------------------------------------------
IIC_CheckAck
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA = input
#if RA4_IS_OPEN_DRAIN == 0
	bsf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = input
#endif
	nop
	nop
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch

	btfsc	IIC_PORT_SDA, IIC_PIN_SDA
	bcf	IIC_STAT, IIC_STAT_SLAVE_AVAILABLE ; notify that slave is not available

	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
	rcall	IIC_Delay_1300ns
#if RA4_IS_OPEN_DRAIN == 0
	bcf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = output
#endif
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_NakSend
;;  DESCRIPTION: sends a NAK (not acknowledge) to the slave(s)
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see IIC_ByteSend and IIC_ByteReceive
;; --------------------------------------------------------------------------
IIC_NakSend
	;; currently SCL = 0 and SDA=1
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 1
	rgoto	IIC_AckSend_Cont

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_AckSend
;;  DESCRIPTION: sends a ACK (acknowledge) to the slave(s)
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;      see IIC_ByteSend and IIC_ByteReceive
;; --------------------------------------------------------------------------
IIC_AckSend
	;; currently SCL = 0 and SDA=1
	bcf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA -> 0
IIC_AckSend_Cont
#if RA4_IS_OPEN_DRAIN == 0
	bcf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = output
#endif
	nop					; wait > 100 ns
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
	rgoto	IIC_Delay_1300ns
	
;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_ByteSend
;;  DESCRIPTION: sends a byte over the IIC bus and checks for acknowledge.
;;  If the slave didn't send an acknowledge, the IIC_STAT_SLAVE_AVAILABLE
;;  flag in MIOS_BOX_STAT will be cleared.
;;  IN:	  byte which should be sent in WREG
;;  OUT:  IIC_STAT.IIC_STAT_SLAVE_AVAILABLE
;;  USES: -
;;  EXAMPLE:
;;
;;	;; send 0x34, 0x56, 0x78 to the IIC slave with ID 0x12
;;
;;	call	IIC_Start		; start IIC
;;	movlw	0x12			; send device address
;;	call	IIC_ByteSend	; bit #0 cleared to notify a write!!!
;;	movlw	0x34			; send first data byte
;;	call	IIC_ByteSend
;;	movlw	0x56			; send second data byte
;;	call	IIC_ByteSend
;;	movlw	0x78			; send third data byte
;;	call	IIC_ByteSend
;;	call	IIC_Stop		; stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;; --------------------------------------------------------------------------
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
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
							; (wait > 1.3 us)
	rcall	IIC_Delay_600ns
	rlf	IIC_BUFFER, F			; shift left value
	decfsz	IIC_CTR, F			; loop
	rgoto	IIC_ByteSendLoop

	rgoto	IIC_CheckAck		; check the acknowledge

;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_ByteReceive
;;  DESCRIPTION: receives a byte from a IIC slave.
;;  IN:	  -
;;  OUT:  received byte in WREG
;;  USES: -
;;  EXAMPLE:
;;
;;	;; receive three bytes from the IIC slave with ID 0x12
;;
;;	rcall	IIC_Start		; start IIC
;;	movlw	0x12 | 1		; send device address - 
;;	call	IIC_ByteSend	;    set bit #0 to notify a read!!!
;;      ;; don't continue if IIC device not available 
;;      IFCLR   IIC_STAT, IIC_STAT_SLAVE_AVAILABLE, rgoto ReadFailed
;;
;;	call	IIC_ByteReceive	; read first byte
;;      movwf   TMP1			; save it in TMP1
;;	call	IIC_AckSend	; send acknowledge
;;	call	IIC_ByteReceive	; read second byte
;;      movwf   TMP2			; save it in TMP2
;;	call	IIC_AckSend	; send acknowledge
;;	call	IIC_ByteReceive	; read third byte
;;      movwf   TMP3			; save it in TMP3
;;	call	IIC_NakSend	; send disacknowledge!!!
;;	call	IIC_Stop		; stop IIC
;;
;;  For more details about the IIC protocol (officially called I2C), see
;;     http://www.semiconductors.philips.com/buses/i2c/
;; --------------------------------------------------------------------------
IIC_ByteReceive
	clrf	IIC_BUFFER
	bsf	IIC_LAT_SDA, IIC_PIN_SDA	; SDA = input
#if RA4_IS_OPEN_DRAIN == 0
	bsf	IIC_TRIS_SDA, IIC_PIN_SDA	; SDA = input
#endif
	
	movlw	0x08				; loop 8 times
	movwf	IIC_CTR
IIC_ByteReceiveLoop
	nop					; relax
	nop
#if RUNNING_AT_48MHz
	nop
	nop
#endif
	bsf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 1
	rcall	IIC_Delay_600ns_Stretch

	rlf	IIC_BUFFER, F			; shift left buffer
	bcf	IIC_BUFFER, 0			; copy status of SDA into rightmost bit
	btfsc	IIC_PORT_SDA, IIC_PIN_SDA
	bsf	IIC_BUFFER, 0
	
	bcf	IIC_LAT_SCL, IIC_PIN_SCL	; SCL -> 0
						; (wait > 1.3 us)
	rcall	IIC_Delay_600ns
	decfsz	IIC_CTR, F			; loop
	rgoto	IIC_ByteReceiveLoop

	movf	IIC_BUFFER, W			; now copy the received value into W
	
	return

; ==========================================================================
	END
