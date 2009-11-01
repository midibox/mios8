; $Id$
;
;  USB Class Driver
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	radix	dec
#include <p18f4550.inc>
#include "macros.inc"
#include "usb_defs.inc"

; ==========================================================================

	;; export lables
	global	USBCLS_CheckUSBMIDIRequest
	global	USBCLS_USBMIDIInitEP
	global	USBCLS_USBMIDITick

	;; import lables
	extern	EP0_SETUP
	extern	USB_DEVICE_STATE
	extern	USBDRV_DTSHandshake

	extern	IIC_MIDI_Receive
	extern	IIC_MIDI_Transmit
	extern	IIC_MIDI_SLAVE
	extern	_iic_midi_rx_package
	extern	_iic_midi_tx_package

	extern	INT_MIDI_Receive
	extern	INT_MIDI_Transmit
	extern	_int_midi_rx_package
	extern	_int_midi_tx_package

; ==========================================================================

#define EP2_BUFFER_SIZE		0x40
#define EP2_IN_BUFFER_NUM	4	; (2 minimum)
#define EP2_OUT_BUFFER_NUM	2	; (2 minimum)

usb5_ep2		udata	0x580
EP2_IN_BUFFER		res	EP2_IN_BUFFER_NUM  * EP2_BUFFER_SIZE
EP2_OUT_BUFFER		res	EP2_OUT_BUFFER_NUM * EP2_BUFFER_SIZE

			udata
IN_BUFFER_CNTx		res	EP2_IN_BUFFER_NUM
OUT_BUFFER_CNTx		res	EP2_OUT_BUFFER_NUM

accessram		udata_acs
BUFFER_STAT		res	1
MIDI_PORT		res	1
OUT_BUFFER_OFFSET	res	1
IN_BUFFER_OFFSET	res	1
EP2_OUT_BUFFER_HEAD	res	1
EP2_OUT_BUFFER_TAIL	res	1
EP2_IN_BUFFER_HEAD	res	1
EP2_IN_BUFFER_TAIL	res	1
EP2_IN_BUFFER_TAIL_NEW	res	1
TX_RETRY_CTR		res	1
TX_BUFFER_CNT		res	1

; ==========================================================================
USBCLS	code
; ==========================================================================

USBCLS_CheckUSBMIDIRequest
	;; todo: interface ID must match interface numbers associated with USBMIDI class, else return

	;; if request recipient is not an interface then return
	SET_BSR	EP0_SETUP
	movf	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, W, BANKED
	andlw	0x1f		; bit 4:0
	xorlw	RCPT_INTF
	bnz	USBCLS_CheckUSBMIDIRequest_End

	;; if request type is not class-specific then return
	swapf	EP0_SETUP + EP0_SETUP_REQUEST_TYPE, W, BANKED	; request type is bit [6:5]
	rrf	WREG, W
	andlw	0x03
	xorlw	CLASS
	bnz	USBCLS_CheckUSBMIDIRequest_End

	;; nothing to do for USB MIDI

USBCLS_CheckUSBMIDIRequest_End
	return

; --------------------------------------------------------------------------
USBCLS_USBMIDIInitEP
	;; init EP2, use IN and OUT pipes
	movlw	(1 << EPHSHK) | (1 << EPCONDIS) | (1 << EPOUTEN) | (1 << EPINEN)
	movwf	UEP2, ACCESS

	SET_BSR	EP2BO
	movlw	EP2_BUFFER_SIZE
	movwf	EP2BO + BDn_CNT, BANKED
	movlw	LOW(EP2_OUT_BUFFER)
	movwf	EP2BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP2_OUT_BUFFER)
	movwf	EP2BO + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_UOWN) | (1 << BDn_STAT_DTSEN)
	movwf	EP2BO + BDn_STAT, BANKED

	clrf	EP2_OUT_BUFFER_HEAD
	clrf	EP2_OUT_BUFFER_TAIL

	SET_BSR	EP2BI
	movlw	LOW(EP2_IN_BUFFER)
	movwf	EP2BI + BDn_ADDRL, BANKED
	movlw	HIGH(EP2_IN_BUFFER)
	movwf	EP2BI + BDn_ADDRH, BANKED
	movlw	(1 << BDn_STAT_DTS)
	movwf	EP2BI + BDn_STAT, BANKED

	clrf	EP2_IN_BUFFER_HEAD
	clrf	EP2_IN_BUFFER_TAIL

	return

; --------------------------------------------------------------------------
USBCLS_EP2_Handler

USBCLS_EP2_Handler_Out
	SET_BSR	EP2BO				; check if something has been received from host
	btfsc	EP2BO + BDn_STAT, BDn_STAT_UOWN, BANKED
	rgoto	USBCLS_EP2_Handler_Out_End
	
	movff	EP2_OUT_BUFFER_TAIL, PRODL	; temporary store current tail pointer in PRODL

	incf	EP2_OUT_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	EP2_OUT_BUFFER_NUM
	skpz
	xorlw	EP2_OUT_BUFFER_NUM		; XOR back to old value
	cpfseq	EP2_OUT_BUFFER_HEAD, ACCESS	; check if we've reached the head
	rgoto	USBCLS_EP2_Handler_Out_New
	rgoto	USBCLS_EP2_Handler_Out_End	; we reached it: no free buffer!

USBCLS_EP2_Handler_Out_New
	movwf	EP2_OUT_BUFFER_TAIL		; save new tail pointer

	lfsr	FSR2, OUT_BUFFER_CNTx		; save byte counter in CNT array of prev. tail (stored in PRODL)
	movf	PRODL, W
	addwf	FSR2L, F
	movlw	0
	addwfc	FSR2H, F
	movff	EP2BO + BDn_CNT, INDF2

	lfsr	FSR0, EP2_OUT_BUFFER	; calculate buffer pointer, store in BDn_ADDR[LH]
	movf	EP2_OUT_BUFFER_TAIL, W
	mullw	EP2_BUFFER_SIZE
	movlw	LOW(EP2_OUT_BUFFER)
	addwf	PRODL, W
	movwf	EP2BO + BDn_ADDRL, BANKED
	movlw	HIGH(EP2_OUT_BUFFER)
	addwfc	PRODH, W
	movwf	EP2BO + BDn_ADDRH, BANKED

	;; this was missing in previous versions of MBHP_USB_PIC firmware:
	;; it's important to write back the available buffer size into BDn_CNT
	;; otherwise we won't receive a complete package anymore (relevant
	;; for SysEx transfers under MacOS)
	movlw	EP2_BUFFER_SIZE
	movwf	EP2BO + BDn_CNT, BANKED

	lfsr	FSR1, EP2BO + BDn_STAT	; send handshake to USB peripheral
	call	USBDRV_DTSHandshake
USBCLS_EP2_Handler_Out_End


USBCLS_EP2_Handler_In
	SET_BSR	EP2BI			; exit if USB transfer not finished yet
	btfsc	EP2BI + BDn_STAT, BDn_STAT_UOWN, BANKED
	rgoto	USBCLS_EP2_Handler_In_Nix

	movf	EP2_IN_BUFFER_HEAD, W	; exit if no new buffer available
	xorwf	EP2_IN_BUFFER_TAIL, W
	bz	USBCLS_EP2_Handler_In_End

	incf	EP2_IN_BUFFER_HEAD, F	; increment (and wrap) head pointer
	movf	EP2_IN_BUFFER_HEAD, W
	xorlw	EP2_IN_BUFFER_NUM
	skpnz
	clrf	EP2_IN_BUFFER_HEAD

	lfsr	FSR0, EP2_IN_BUFFER	; calculate buffer pointer, copy into BI address registers
	movf	EP2_IN_BUFFER_HEAD, W
	mullw	EP2_BUFFER_SIZE
	movlw	LOW(EP2_IN_BUFFER)
	addwf	PRODL, W
	movwf	EP2BI + BDn_ADDRL, BANKED
	movlw	HIGH(EP2_IN_BUFFER)
	addwfc	PRODH, W
	movwf	EP2BI + BDn_ADDRH, BANKED

	lfsr	FSR0, IN_BUFFER_CNTx	; set counter
	movf	EP2_IN_BUFFER_HEAD, W
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	movff	INDF0, EP2BI + BDn_CNT

	lfsr	FSR1, EP2BI + BDn_STAT	; send handshake to USB peripheral
	call	USBDRV_DTSHandshake

USBCLS_EP2_Handler_In_End
	return

USBCLS_EP2_Handler_In_Nix
	return

; --------------------------------------------------------------------------
USBCLS_USBMIDITick
	movlw	CONFIGURED_STATE - 1
	cpfsgt	USB_DEVICE_STATE, ACCESS
	rgoto	USBCLS_USBMIDITick_End
	btfsc	UCON, SUSPND, ACCESS
	rgoto	USBCLS_USBMIDITick_End

	;; check EP2 IN/OUT buffer
	call	USBCLS_EP2_Handler

	;; check received bytes
	call	USBCLS_MIDIReceiveCheck

	;; check if something should be transmitted
	lfsr	FSR2, OUT_BUFFER_CNTx	; store saved CNT value in TX_BUFFER_CNT
	movf	EP2_OUT_BUFFER_HEAD, W
	addwf	FSR2L, F
	movlw	0
	addwfc	FSR2H, F
	movf	INDF2, W
	clrf	INDF2			; clear counter (buffer free again)
	movwf	TX_BUFFER_CNT
	andlw	0xff
	bz	USBCLS_USBMIDITick_NoTx	; exit if CNT is zero (nothing received yet...)

USBCLS_USBMIDITick_Tx
	clrf	OUT_BUFFER_OFFSET
USBCLS_USBMIDITick_Loop
	clrf	TX_RETRY_CTR

	;; exit once all bytes within buffer have been processed
	movf	TX_BUFFER_CNT, W
	cpfslt	OUT_BUFFER_OFFSET, ACCESS
	rgoto	USBCLS_USBMIDITick_Loop_Exit
	rgoto	USBCLS_USBMIDITick_Loop_Try

USBCLS_USBMIDITick_Loop_Retry

	;; exit on more than 256 retries (IIC slave disconnected?)
	decfsz	TX_RETRY_CTR, F
	rgoto	USBCLS_USBMIDITick_Loop_Try
 	rgoto	USBCLS_USBMIDITick_Loop_Next

USBCLS_USBMIDITick_Loop_Try

	;; check EP2 IN/OUT buffer
	call	USBCLS_EP2_Handler

	;; check received bytes
	call	USBCLS_MIDIReceiveCheck

	;; send packet
	lfsr	FSR2, EP2_OUT_BUFFER	; calculate buffer pointer -> FSR2
	movf	EP2_OUT_BUFFER_HEAD, W
	mullw	EP2_BUFFER_SIZE
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F

	movf	OUT_BUFFER_OFFSET, W
	addwf	FSR2L, F
	movlw	0
	addwfc	FSR2H, F

	call	USBCLS_MIDISend
 	bnz	USBCLS_USBMIDITick_Loop_Next
	rgoto	USBCLS_USBMIDITick_Loop_Retry

	;; switch to next packet
USBCLS_USBMIDITick_Loop_Next
	movlw	4			; switch to next packet
	addwf	OUT_BUFFER_OFFSET, F
	rgoto	USBCLS_USBMIDITick_Loop
USBCLS_USBMIDITick_Loop_Exit

	incf	EP2_OUT_BUFFER_HEAD, F	; increment (and wrap) head pointer
	movf	EP2_OUT_BUFFER_HEAD, W
	xorlw	EP2_OUT_BUFFER_NUM
	skpnz
	clrf	EP2_OUT_BUFFER_HEAD

USBCLS_USBMIDITick_NoTx

USBCLS_USBMIDITick_End
	return

; --------------------------------------------------------------------------
USBCLS_MIDIReceiveCheck
	;; check if any buffer portion is free
	incf	EP2_IN_BUFFER_TAIL, W		; increment tail ptr, wrap if end of buffer reached
	xorlw	EP2_IN_BUFFER_NUM
	skpz
	xorlw	EP2_IN_BUFFER_NUM	; XOR back to old value
	cpfseq	EP2_IN_BUFFER_HEAD, ACCESS	; check if we've reached the head
	rgoto	USBCLS_MIDIReceiveCheck_Start
	rgoto	USBCLS_MIDIReceiveCheck_End	; we reached it: no free buffer!

USBCLS_MIDIReceiveCheck_Start
	movwf	EP2_IN_BUFFER_TAIL_NEW	; save new tail pointer in temporary variable

	lfsr	FSR0, EP2_IN_BUFFER	; calculate buffer pointer
	mullw	EP2_BUFFER_SIZE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F

	clrf	IN_BUFFER_OFFSET	; clear offset within Tx buffer

	clrf	MIDI_PORT		; Port 0 (internal UART)
	call	USBCLS_MIDIReceive

	incf	MIDI_PORT, F		; Port 1 (IIC Slave 1)
	call	USBCLS_MIDIReceive

	incf	MIDI_PORT, F		; Port 2 (IIC Slave 2)
	call	USBCLS_MIDIReceive

	incf	MIDI_PORT, F		; Port 3 (IIC Slave 3)
	call	USBCLS_MIDIReceive

	incf	MIDI_PORT, F		; Port 4 (IIC Slave 4)
	call	USBCLS_MIDIReceive

	movf	IN_BUFFER_OFFSET, W	; check if something has been put into buffer
	bz	USBCLS_MIDIReceiveCheck_End

	movf	EP2_IN_BUFFER_TAIL_NEW, W	; take new tail pointer and save counter
	movwf	EP2_IN_BUFFER_TAIL
	lfsr	FSR0, IN_BUFFER_CNTx
	addwf	FSR0L, F
	movlw	0
	addwfc	FSR0H, F
	movff	IN_BUFFER_OFFSET, INDF0


#if 0
	;; only for testing purposes: insert number of free receive buffer bytes into package
	movf	EP2_IN_BUFFER_TAIL_NEW, W

	lfsr	FSR0, EP2_IN_BUFFER	; calculate buffer pointer
	mullw	EP2_BUFFER_SIZE
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwfc	FSR0H, F
	extern	MIDI_RxBufferFree
	call	MIDI_RxBufferFree
	btfsc	WREG, 7
	movlw	0x7f
	movwf	PRODL
	movlw	2
	movff	PRODL, PLUSW0
#endif

USBCLS_MIDIReceiveCheck_End
	return


; --------------------------------------------------------------------------
; IN:  pointer to MIDI_INx record in FSR2, port number in MIDI_PORT, pointer to EP2_IN_BUFFER in FSR0
; OUT: -
USBCLS_MIDIReceive
	movf	IN_BUFFER_OFFSET, W		; stop if buffer already full
	xorlw	EP2_BUFFER_SIZE
	bz	USBCLS_MIDIReceive_End

	movf	MIDI_PORT, W			; branch depending on port number
	bnz	USBCLS_MIDIReceive_IIC

USBCLS_MIDIReceive_Int
	;; receiving from internal EUSART
	call	INT_MIDI_Receive		; (returns _int_midi_rx_package+0)
	bz	USBCLS_MIDIReceive_End		; no new package if 0 is returned
	movff	_int_midi_rx_package+0, POSTINC0	; else copy new package into EP buffer
	movff	_int_midi_rx_package+1, POSTINC0
	movff	_int_midi_rx_package+2, POSTINC0
	movff	_int_midi_rx_package+3, POSTINC0
	movlw	4
	addwf	IN_BUFFER_OFFSET, F
	rgoto	USBCLS_MIDIReceive_End

USBCLS_MIDIReceive_IIC
	;; receiving from IIC slave
	addlw	-1
	movwf	IIC_MIDI_SLAVE
	call	IIC_MIDI_Receive		; (returns _iic_midi_rx_package+0)
	bz	USBCLS_MIDIReceive_End		; no new package if 0 is returned
	swapf	MIDI_PORT, W			; set cable number (== MIDI_PORT)
	andlw	0xf0
	iorwf	_iic_midi_rx_package+0, W
	movwf	POSTINC0
	movff	_iic_midi_rx_package+1, POSTINC0
	movff	_iic_midi_rx_package+2, POSTINC0
	movff	_iic_midi_rx_package+3, POSTINC0
	movlw	4
	addwf	IN_BUFFER_OFFSET, F
	;; 	rgoto	USBCLS_MIDIReceive_End

USBCLS_MIDIReceive_End
	return


; --------------------------------------------------------------------------
; IN:  expecting pointer to Tx packet in FSR2
; OUT: request retry if ZERO flag set!
USBCLS_MIDISend
	movf	INDF2, W		; ignore zero packages
	andlw	0x0f
	bnz	USBCLS_MIDISend_Ok
	iorlw	0xff			; ensure that ZERO flag not set!
	rgoto	USBCLS_MIDISend_End

USBCLS_MIDISend_Ok
	swapf	INDF2, W		; branch depending on cable number
	andlw	0x0f
	bnz	USBCLS_MIDISend_IIC
USBCLS_MIDISend_Int
	;; sending via internal EUSART
	movff	POSTINC2, _int_midi_tx_package+0
	movff	POSTINC2, _int_midi_tx_package+1
	movff	POSTINC2, _int_midi_tx_package+2
	movff	POSTINC2, _int_midi_tx_package+3
	call	INT_MIDI_Transmit	; returns 0 if buffer full
	rgoto	USBCLS_MIDISend_End
	
USBCLS_MIDISend_IIC
	;; sending via IIC slave
	addlw	-1
	movwf	IIC_MIDI_SLAVE

	movff	POSTINC2, _iic_midi_tx_package+0
	movff	POSTINC2, _iic_midi_tx_package+1
	movff	POSTINC2, _iic_midi_tx_package+2
	movff	POSTINC2, _iic_midi_tx_package+3
	call	IIC_MIDI_Transmit	; returns 0 if slave not available or buffer full
	;; 	rgoto	USBCLS_MIDISend_End

USBCLS_MIDISend_End
	return

; ==========================================================================
	END
