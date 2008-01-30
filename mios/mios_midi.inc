; $Id$
;
; MIDI Service Routines of MIOS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

MIOS_MIDI_MBLINK_START     EQU 0xf9	; control byte for begin of stream
MIOS_MIDI_MBLINK_STOP      EQU 0xfe	; control byte for end of stream

MIOS_MIDI_INTERFACE_COMMON	EQU	0x00
MIOS_MIDI_INTERFACE_TO_HOST	EQU	0x01

MIOS_MIDI_MERGER_DISABLED	EQU	0x00
MIOS_MIDI_MERGER_ENABLED	EQU	0x01
MIOS_MIDI_MERGER_MBLINK_EP	EQU	0x02
MIOS_MIDI_MERGER_MBLINK_FP	EQU	0x03

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_Init
;;  C_DECLARATION: void MIOS_MIDI_Init(void)
;;  DESCRIPTION: initialises the MIDI interface: baudrate will be set
;;  depending on To-Host/MIDI option, Rx and Tx buffer will be emptied
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MIDI_Init
	;; enable transmitter and init baudrate
	movlw	(1 << TXEN) | (1 << BRGH)
	movwf	TXSTA
	rcall	MIOS_MIDI_Init_Baudrate

	; Configure Receive Status and Control Register
	movlw	(1 << SPEN) | (1 << CREN)
	movwf	RCSTA

	; initialize ringbuffers
	SET_BSR	MIOS_RX_BUFFER_TAIL
	movlw	MIOS_RX_BUFFER & 0xff
	movwf	MIOS_RX_BUFFER_TAIL, BANKED
	movwf	MIOS_RX_BUFFER_HEAD, BANKED

	movlw	MIOS_TX_BUFFER & 0xff
	movwf	MIOS_TX_BUFFER_TAIL, BANKED
	movwf	MIOS_TX_BUFFER_HEAD, BANKED

	;; annul "lastevent"
	setf	MIOS_MPROC_LASTEVENT0, BANKED
	setf	MIOS_MPROC_LASTEVENT1, BANKED
	setf	MIOS_MPROC_LASTEVENT2, BANKED

	return

;; -------
;; (function used by MIOS_MIDI_Init and MIOS_MIDI_InterfaceSet)
MIOS_MIDI_Init_Baudrate
	BRA_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST, ACCESS, MIOS_MIDI_Init_Baudrate_31250

MIOS_MIDI_Init_Baudrate_38400
  	movlw	2*0x21-1			; set baudrate to 38400 baud
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_20MHz
	movlw	0x21-1
	rgoto	MIOS_MIDI_Init_Baudrate_End

MIOS_MIDI_Init_Baudrate_31250
  	movlw	8*0x0a-1			; set baudrate to 31250 baud
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_20MHz
	movlw	4*0x0a-1
	;; 	rgoto	MIOS_MIDI_Init_Baudrate_End

MIOS_MIDI_Init_Baudrate_End
	;; don't overwrite SPBRG if value already initialized - otherwise the
	;; baudrate generator will be reset, this can lead to invalid MIDI bytes
	xorwf	SPBRG, W
	bz	MIOS_MIDI_Init_Baudrate_NoUpdate
MIOS_MIDI_Init_Baudrate_Update
	xorwf	SPBRG, W	; (XOR back to new value)
	movwf	SPBRG		; (write new value)
MIOS_MIDI_Init_Baudrate_NoUpdate
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_InterfaceSet
;;  C_DECLARATION: void MIOS_MIDI_InterfaceSet(unsigned char interface)
;;  DESCRIPTION: this function has two purposes:<BR>
;;  If the interface number is 0x00 or 0x01, the internal USART will be selected,
;;  and a baudrate of either 31250 or 38400 baud will be configured:<BR>
;;     0x00: MIOS_MIDI_INTERFACE_COMMON (common MIDI interface, 31250 baud)<BR>
;;     0x01: MIOS_MIDI_INTERFACE_TO_HOST (MIDIbox-to-COM interface, RS232, 38400 baud)<BR>
;;  <BR>
;;  With values >= 0x02 a MBHP_IIC_MIDI module will be selected for <B>outgoing</B>
;;  MIDI messages (which are sent with MIOS_MIDI_TxBufferPut)<BR>
;;  Incoming messages are still handled with the internal USART!<BR>
;;  If data should be received from a MBHP_IIC_MIDI module, please use the appr.
;;  functions of the software package which is provided together with the module.<BR>
;;  Note that the MIOS_MIDI_InterfaceAutoSet function allows you to derive the MIDI
;;  interface type and the IIC slave address from the PIC ID header!<BR>
;;  IN:	  interface type in WREG
;;  C_IN:  interface type in <interface>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; send MIDI clock over internal MIDI Out with normal baudrate
;;	movlw	MIOS_MIDI_INTERFACE_COMMON
;;	call	MIOS_MIDI_InterfaceSet
;; 
;;	movlw	0xf8  ; (MIDI Clock)
;;	call	MIOS_MIDI_TxBufferPut
;; 
;;	;; send MIDI clock over MBHP_IIC_MIDI with address 0x10
;;	movlw	0x10
;;	call	MIOS_MIDI_InterfaceSet
;; 
;;	movlw	0xf8  ; (MIDI Clock)
;;	call	MIOS_MIDI_TxBufferPut
;; 
;;	;; switch back to the default interface
;;	call	MIOS_MIDI_InterfaceAutoSet
;; 
;;  C_EXAMPLE:
;;
;;	// send MIDI clock over internal MIDI Out with normal baudrate
;;	MIOS_MIDI_InterfaceSet(MIOS_MIDI_INTERFACE_COMMON);
;;	MIOS_MIDI_TxBufferPut(0xf8);
;; 
;;	// send MIDI clock over MBHP_IIC_MIDI with address 0x10
;;	MIOS_MIDI_InterfaceSet(0x10);
;;	MIOS_MIDI_TxBufferPut(0xf8);
;; 
;;	// switch back to the default interface
;;	MIOS_MIDI_InterfaceAutoSet();
;;
;; --------------------------------------------------------------------------
MIOS_MIDI_InterfaceSet
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED

	andlw	0xfe
	bz	MIOS_MIDI_InterfaceSet_Int

MIOS_MIDI_InterfaceSet_IIC
	;; (we've already ensured that bit #0 is not set with the "andlw 0xfe"!)
	movwf	MIOS_IIC_MIDI_ADDR, BANKED
	rgoto	MIOS_MIDI_InterfaceSet_End

MIOS_MIDI_InterfaceSet_Int
	clrf	MIOS_IIC_MIDI_ADDR, BANKED

	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST
	btfsc	MIOS_TMP1, 0, BANKED
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST
	rcall	MIOS_MIDI_Init_Baudrate	; set selected baudrate
	;; 	rgoto	MIOS_MIDI_InterfaceSet_End

MIOS_MIDI_InterfaceSet_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_InterfaceAutoSet
;;  C_DECLARATION: void MIOS_MIDI_InterfaceAutoSet(void)
;;  DESCRIPTION: derives the MIDI interface type and the MBHP_IIC_MIDI module address
;;  from the PIC ID header.<BR>
;;  To-Host flag: ID Byte 6, Bit 0<BR>
;;  MBHP_IIC_MIDI module address: ID Byte 5<BR>
;;  If the slave address is != 0x00 and != 0xff, it will be taken as default
;;  MIDI Out by MIOS
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, TBLPTR
;;  EXAMPLE:
;;
;;	;; set MIDI interface to the specified type in the PIC ID header
;;	call	MIOS_MIDI_InterfaceAutoSet
;; 
;;  C_EXAMPLE:
;;
;;	// set MIDI interface to the specified type in the PIC ID header
;;	MIOS_MIDI_InterfaceAutoSet();
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_InterfaceAutoSet
	IRQ_DISABLE			; we change TBLPTRU - IRQs have to be disabled!
	TABLE_ADDR_FULL _IDLOC6		; derive interface type from ID sector

	tblrd*-				; get to-host flag
	movf	TABLAT, W
	andlw	0x01
	rcall	MIOS_MIDI_InterfaceSet
	;; note: clears also MIOS_IIC_MIDI_ADDR

	tblrd*-				; get MBHP_IIC_MIDI module address
	movf	TABLAT, W		; don't use IIC if ID entry is 0x00 or 0xff
	bz	MIOS_MIDI_InterfaceAutoSet_NoIIC
	addlw	1
	bz	MIOS_MIDI_InterfaceAutoSet_NoIIC
	movf	TABLAT, W
	rcall	MIOS_MIDI_InterfaceSet
MIOS_MIDI_InterfaceAutoSet_NoIIC

	clrf	TBLPTRU			; clear upper table pointer so
	IRQ_ENABLE			; that the IRQs can be enabled again
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_InterfaceGet
;;  C_DECLARATION: unsigned char MIOS_MIDI_InterfaceGet(void)
;;  DESCRIPTION: returns the current MIDI interface type
;;  following Interface types are provided:<BR>
;;     0x00: MIOS_MIDI_INTERFACE_COMMON (common MIDI interface, 31250 baud)<BR>
;;     0x01: MIOS_MIDI_INTERFACE_TO_HOST (MIDIbox-to-COM interface, RS232, 38400 baud)<BR>
;;     >= 0x02: MBHP_IIC_MIDI module address
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  interface type in WREG and MIOS_PARAMETER1
;;  C_OUT:  interface type
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; branch if MBHP_IIC_MIDI module is selected
;;	call	MIOS_MIDI_InterfaceGet
;;	andlw   0xfe
;;	bnz     HandleExternalMIDI
;; 
;;  C_EXAMPLE:
;;
;;	// get current interface type and branch depending on setting
;;	switch( MIOS_MIDI_InterfaceGet() ) {
;;	  case MIOS_MIDI_INTERFACE_COMMON:
;;	    // do this...
;;	    break;
;;	  case MIOS_MIDI_INTERFACE_TO_HOST:
;;	    // do that...
;;	    break;
;;	  default:
;;	    // IIC slave selected...
;;	}
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_InterfaceGet
	SET_BSR	MIOS_IIC_MIDI_ADDR
	movf	MIOS_IIC_MIDI_ADDR, W, BANKED
	bz	MIOS_MIDI_InterfaceGet_Int

MIOS_MIDI_InterfaceGet_Ext
	;; (address already in WREG)
	rgoto	MIOS_MIDI_InterfaceGet_End

MIOS_MIDI_InterfaceGet_Int
	movlw	0x00
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_TO_HOST
	iorlw	0x01
	;; 	rgoto	MIOS_MIDI_InterfaceGet_End

MIOS_MIDI_InterfaceGet_End
	movwf	MIOS_PARAMETER1
	andlw	0xff		; (update STATUS)
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_DeviceIDSet
;;  C_DECLARATION: void MIOS_MIDI_DeviceIDSet(unsigned char device_id)
;;  DESCRIPTION: sets the MIDI device ID. Should be a unique number in a MIDI
;;  chain. Normaly this ID is derived from the PIC ID header with the
;;  MIOS_MIDI_DeviceIDAutoSet function
;;  IN:	  device number in WREG (0x00-0x7f)
;;  C_IN:  device number in <device_id>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; set device ID to 0x03
;;	movlw	0x03
;;	call	MIOS_MIDI_DeviceIDSet
;; 
;;  C_EXAMPLE:
;;
;;	;; change device ID
;;	MIOS_MIDI_DeviceIDSet(0x03);
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_DeviceIDSet
	andlw	0x7f
	SET_BSR	MIOS_MIDI_DEVICE_ID
	movwf	MIOS_MIDI_DEVICE_ID, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_DeviceIDAutoSet
;;  C_DECLARATION: void MIOS_MIDI_DeviceIDAutoSet(void)
;;  DESCRIPTION: derives the MIDI device ID from the PIC ID header
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, TBLPTR
;;  EXAMPLE:
;;
;;	;; set device ID to the specified value in the PIC ID header
;;	call	MIOS_MIDI_DeviceIDAutoSet
;; 
;;  C_EXAMPLE:
;;
;;	// set device ID to the specified value in the PIC ID header
;;	MIOS_MIDI_DeviceIDAutoSet();
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_DeviceIDAutoSet
	IRQ_DISABLE			; we change TBLPTRU - IRQs have to be disabled!
	TABLE_ADDR_FULL _IDLOC7		; derive device ID from ID sector
	tblrd*				; get first byte
	movf	TABLAT, W		; copy 7-bit value to DEVICE_ID
	rcall	MIOS_MIDI_DeviceIDSet
	clrf	TBLPTRU			; clear upper table pointer so
	IRQ_ENABLE			; that the IRQs can be enabled again
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_DeviceIDGet
;;  C_DECLARATION: unsigned char MIOS_MIDI_DeviceIDGet(void)
;;  DESCRIPTION: returns the MIDI device ID
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  device number in WREG and MIOS_PARAMETER1 (0x00-0x7f)
;;  C_OUT:  device number
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; print device ID on display
;;	call	MIOS_MIDI_DeviceIDGet
;;	call	MIOS_LCD_PrintHex2
;; 
;;  C_EXAMPLE:
;;
;;	// print device ID on display
;;	MIOS_LCD_PrintHex2(MIOS_MIDI_DeviceIDGet());
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_DeviceIDGet
	SET_BSR	MIOS_MIDI_DEVICE_ID
	movf	MIOS_MIDI_DEVICE_ID, W, BANKED
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_MergerSet
;;  C_DECLARATION: void MIOS_MIDI_MergerSet(unsigned char mode)
;;  DESCRIPTION: configures the integrated MIDI merger
;;  following Merger IDs are provided:<BR>
;;     0x00: MIOS_MIDI_MERGER_DISABLED  (merger disabled)<BR>
;;     0x01: MIOS_MIDI_MERGER_ENABLED   (merger enabled)<BR>
;;     0x02: MIOS_MIDI_MERGER_MBLINK_EP (MIDIbox Link End Point)<BR>
;;     0x03: MIOS_MIDI_MERGER_MBLINK_FP (MIDIbox Link Forwarding Point)
;;  IN:	  merger function ID in WREG
;;  C_IN:  merger function ID in <mode>
;;  OUT:  -
;;  USES: -
;;  EXAMPLE:
;;
;;	;; configure the MIDIbox as MIDIbox Link End Point
;;	;; (means: merger enabled only for events which has 
;;	;; been generated by another MIDIbox which is configured 
;;	;; as MIDIbox Link Forwarding Point)
;;	movlw	MIOS_MIDI_MERGER_MBLINK_EP
;;	call	MIOS_MIDI_MergerSet
;; 
;;  C_EXAMPLE:
;;
;;	// configure the MIDIbox as MIDIbox Link End Point
;;	// (means: merger enabled only for events which has 
;;	// been generated by another MIDIbox which is configured 
;;	// as MIDIbox Link Forwarding Point)
;;	MIOS_MIDI_MergerSet(MIOS_MIDI_MERGER_MBLINK_EP);
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_MergerSet
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER
	bcf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK

	btfsc	WREG, 0
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER
	btfsc	WREG, 1
	bsf	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_MergerGet
;;  C_DECLARATION: unsigned char MIOS_MIDI_MergerGet(void)
;;  DESCRIPTION: returns configuration of integrated MIDI merger:
;;     0x00: MIOS_MIDI_MERGER_DISABLED  (merger disabled)<BR>
;;     0x01: MIOS_MIDI_MERGER_ENABLED   (merger enabled)<BR>
;;     0x02: MIOS_MIDI_MERGER_MBLINK_EP (MIDIbox Link End Point)<BR>
;;     0x03: MIOS_MIDI_MERGER_MBLINK_FP (MIDIbox Link Forwarding Point)
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  merger function ID in WREG and MIOS_PARAMETER1
;;  C_OUT:  merger function ID
;;  USES: -
;;  EXAMPLE:
;;
;;	;; get current merger configuration and branch depending on setting
;;	call	MIOS_MIDI_MergerGet
;;	BRA_IFSET WREG, 1, ACCESS, MBLinkEnabled
;;	rgoto	MBLinkNotEnabled
;; 
;;  C_EXAMPLE:
;;
;;	// get current merger configuration and branch depending on setting
;;	switch( MIOS_MIDI_MergerGet() ) {
;;	  case MIOS_MIDI_MERGER_ENABLED:
;;	    // do this...
;;	    break;
;;	  case MIOS_MIDI_MERGER_MBLINK_EP:
;;	    // do this...
;;	    break;
;;	  case MIOS_MIDI_MERGER_MBLINK_FP:
;;	    // do this...
;;	    break;
;;	  default: // MIOS_MIDI_MERGER_DISABLED:
;;	    // do that...
;;	}
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_MergerGet
	movlw	0x00
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER
	iorlw	0x01
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK
	iorlw	0x02
	movwf	MIOS_PARAMETER1
	andlw	0xff		; (update STATUS)
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_BeginStream
;;  C_DECLARATION: void MIOS_MIDI_BeginStream(void)
;;  DESCRIPTION: use this function before a new MIDI event or stream should
;;  be sent. Necessary for a correct function of the MIDIbox Link feature!
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;
;;	;; send the MIDI event B0 07 7F
;;	call	MIOS_MIDI_BeginStream
;;	movlw	0xb0
;;	call	MIOS_MIDI_TxBufferPut
;;	movlw	0x07
;;	call	MIOS_MIDI_TxBufferPut
;;	movlw	0x7f
;;	call	MIOS_MIDI_TxBufferPut
;;	call	MIOS_MIDI_EndStream
;; 
;;  C_EXAMPLE:
;;
;;	// send the MIDI event B0 07 7F
;;	MIOS_MIDI_BeginStream();
;;	MIOS_MIDI_TxBufferPut(0xb0);
;;	MIOS_MIDI_TxBufferPut(0x07);
;;	MIOS_MIDI_TxBufferPut(0x7f);
;;	MIOS_MIDI_EndStream();
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_BeginStream
	movlw	MIOS_MIDI_MBLINK_START
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK
	return
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER
	return
	rgoto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_EndStream
;;  C_DECLARATION: void MIOS_MIDI_EndStream(void)
;;  DESCRIPTION: use this function after a MIDI event or stream has been
;;  sent. Necessary for a correct function of the MIDIbox Link feature!
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: -
;;  EXAMPLE:
;;
;;	;; send the MIDI event 90 36 7F
;;	call	MIOS_MIDI_BeginStream
;;	movlw	0x90
;;	call	MIOS_MIDI_TxBufferPut
;;	movlw	0x36
;;	call	MIOS_MIDI_TxBufferPut
;;	movlw	0x7f
;;	call	MIOS_MIDI_TxBufferPut
;;	call	MIOS_MIDI_EndStream
;; 
;;  C_EXAMPLE:
;;
;;	// send the MIDI event 90 36 7F
;;	MIOS_MIDI_BeginStream();
;;	MIOS_MIDI_TxBufferPut(0x90);
;;	MIOS_MIDI_TxBufferPut(0x36);
;;	MIOS_MIDI_TxBufferPut(0x7f);
;;	MIOS_MIDI_EndStream();
;; 
;; --------------------------------------------------------------------------
MIOS_MIDI_EndStream
	movlw	MIOS_MIDI_MBLINK_STOP
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MBLINK
	return
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_MERGER
	return
	rgoto	MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_RxBufferUsed
;;  C_DECLARATION: unsigned char MIOS_MIDI_RxBufferUsed(void)
;;  DESCRIPTION: returns number of used bytes in receive buffer
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of used bytes in WREG
;;  C_OUT:  number of used bytes
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; branch if something has been received via MIDI
;;	call	MIOS_MIDI_RxBufferUsed
;;	bz	ReceivedSomething
;; 
;;  C_EXAMPLE:
;;
;;	// branch if something has been received via MIDI
;;	if( MIOS_MIDI_RxBufferUsed() ) {
;;	  // do something...
;;	}
;; 
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  Received bytes will be notified by the MPROC hooks!</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_RxBufferUsed
	SET_BSR	MIOS_RX_BUFFER_HEAD
	movf	MIOS_RX_BUFFER_HEAD, W, BANKED
	cpfslt	MIOS_RX_BUFFER_TAIL, BANKED
	rgoto MIOS_MIDI_RxBufferUsed_Geq
	sublw	(MIOS_RX_BUFFER_END+1-MIOS_RX_BUFFER)
	addwf	MIOS_RX_BUFFER_TAIL, W, BANKED
	return
MIOS_MIDI_RxBufferUsed_Geq
	subwf	MIOS_RX_BUFFER_TAIL, W, BANKED
MIOS_MIDI_RxBufferUsed_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_RxBufferFree
;;  C_DECLARATION: unsigned char MIOS_MIDI_RxBufferFree(void)
;;  DESCRIPTION: returns number of free bytes in receive buffer
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of free bytes in WREG
;;  C_OUT:  number of free bytes
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Rx Buffer if it isn't full
;;	call	MIOS_MIDI_RxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIOS_MIDI_RxBufferPut
;;
;;  C_EXAMPLE:
;;
;;	// put a MIDI byte into the Rx Buffer if it isn't full
;;	if( MIOS_MIDI_RxBufferFree() ) {
;;	  MIOS_MIDI_RxBufferPut(0xf8); // MIDI Clock
;;	}
;;
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  Received bytes will be notified by the MPROC hooks!</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_RxBufferFree
	rcall	MIOS_MIDI_RxBufferUsed
	sublw	(MIOS_RX_BUFFER_END-MIOS_RX_BUFFER)
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_RxBufferPut
;;  C_DECLARATION: void MIOS_MIDI_RxBufferPut(unsigned char b)
;;  DESCRIPTION: puts a byte onto the receive buffer
;;  IN:	  byte in WREG
;;  C_IN:  byte in <b>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, FSR1
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Rx Buffer if it isn't full
;;	call	MIOS_MIDI_RxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIOS_MIDI_RxBufferPut
;;
;;  C_EXAMPLE:
;;
;;	// put a MIDI byte into the Rx Buffer if it isn't full
;;	if( MIOS_MIDI_RxBufferFree() ) {
;;	  MIOS_MIDI_RxBufferPut(0xf8); // MIDI Clock
;;	}
;;
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  Received bytes will be notified by the MPROC hooks!</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_RxBufferPut
	SET_BSR	MIOS_RX_BUFFER_TMP
	movwf	MIOS_RX_BUFFER_TMP, BANKED		; save byte which should be sent

	;; call user hook
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MIDI_NotifyRx

	SET_BSR	MIOS_RX_BUFFER_TMP
	incf	MIOS_RX_BUFFER_TAIL, W, BANKED	; increment ringcounter, but don't save yet
	xorlw	(MIOS_RX_BUFFER_END&0xff)+1
	skpnz
	movlw	(MIOS_RX_BUFFER&0xff) ^ ((MIOS_RX_BUFFER_END&0xff)+1)
	xorlw	(MIOS_RX_BUFFER_END&0xff)+1
	cpfseq	MIOS_RX_BUFFER_HEAD, BANKED
	rgoto MIOS_MIDI_RxBufferPut_Ok
	return					; prevent overrun
MIOS_MIDI_RxBufferPut_Ok
	movwf	MIOS_RX_BUFFER_TAIL, BANKED		; save counter
	lfsr	FSR1, MIOS_RX_BUFFER			; prepare FSR
	movwf	FSR1L
	movf	MIOS_RX_BUFFER_TMP, W, BANKED	; push byte onto buffer
	movwf	INDF1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_RxBufferGet
;;  C_DECLARATION: unsigned char MIOS_MIDI_RxBufferGet(void)
;;  DESCRIPTION: gets a byte from the receive buffer
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  received byte in WREG
;;  C_OUT:  received byte
;;  USES: BSR, FSR1
;;  EXAMPLE:
;;
;;	;; get a MIDI byte from the Rx Buffer if something has been received
;;	call	MIOS_MIDI_RxBufferUsed
;;	bz	NothingReceived
;;	call	MIOS_MIDI_RxBufferGet
;;
;;  C_EXAMPLE:
;;	
;;	unsigned char b;
;;
;;	// get a MIDI byte from the Rx Buffer if something has been received
;;	if( MIOS_MIDI_RxBufferUsed() ) {
;;	  b = MIOS_MIDI_RxBufferGet();
;;	}
;;
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  Received bytes will be notified by the MPROC hooks!</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_RxBufferGet
	SET_BSR	MIOS_RX_BUFFER_HEAD
	incf	MIOS_RX_BUFFER_HEAD, F, BANKED
	movlw	(MIOS_RX_BUFFER_END&0xff)+1
	cpfseq	MIOS_RX_BUFFER_HEAD, BANKED
	rgoto MIOS_MIDI_RxBufferGet_NoReset
	movlw	MIOS_RX_BUFFER & 0xff
	movwf	MIOS_RX_BUFFER_HEAD, BANKED
MIOS_MIDI_RxBufferGet_NoReset
	lfsr	FSR1, MIOS_RX_BUFFER
	movff	MIOS_RX_BUFFER_HEAD, FSR1L
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_TxBufferUsed
;;  C_DECLARATION: unsigned char MIOS_MIDI_TxBufferUsed(void)
;;  DESCRIPTION: returns number of used bytes in buffer
;;  NOTE: always deliver the result of the internal UART buffer, and not of a MBHP_IIC_MIDI module
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of used bytes in WREG
;;  C_OUT:  number of used bytes
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; branch if something has been put into the Tx Buffer
;;	call	MIOS_MIDI_TxBufferUsed
;;	bz	GotSomething
;; 
;;  C_EXAMPLE:
;;
;;	// branch if something has been put into the Tx Buffer
;;	if( MIOS_MIDI_TxBufferUsed() ) {
;;	  // got something
;;	}
;; 
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  The MIOS_MIDI_TxBufferPut() function will wait if the buffer is full.</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_TxBufferUsed
	SET_BSR	MIOS_TX_BUFFER_HEAD
	movf	MIOS_TX_BUFFER_HEAD, W, BANKED
	cpfslt	MIOS_TX_BUFFER_TAIL, BANKED
	rgoto MIOS_MIDI_TxBufferUsed_Geq
	sublw	(MIOS_TX_BUFFER_END+1-MIOS_TX_BUFFER)
	addwf	MIOS_TX_BUFFER_TAIL, W, BANKED
	return
MIOS_MIDI_TxBufferUsed_Geq
	subwf	MIOS_TX_BUFFER_TAIL, W, BANKED
MIOS_MIDI_TxBufferUsed_End
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_TxBufferFree
;;  C_DECLARATION: unsigned char MIOS_MIDI_TxBufferFree(void)
;;  DESCRIPTION: returns number of free bytes in transmit buffer
;;  NOTE: always deliver the result of the internal UART buffer, and not of a MBHP_IIC_MIDI module
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  number of free bytes in WREG
;;  C_OUT:  number of free bytes
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Tx Buffer if it isn't full
;;	call	MIOS_MIDI_TxBufferFree
;;	bz	BufferIsFull
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIOS_MIDI_TxBufferPut
;;
;;  C_EXAMPLE:
;;
;;	// put a MIDI byte into the Tx Buffer if it isn't full
;;	if( MIOS_MIDI_TxBufferFree() ) {
;;	  MIOS_MIDI_TxBufferPut(0xf8);
;;	}
;;
;;  NOTE: <I>this function is for internal use within MIOS only!<BR>
;;  The MIOS_MIDI_TxBufferPut() function will wait if the buffer is full.</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_TxBufferFree
	rcall	MIOS_MIDI_TxBufferUsed
	sublw	(MIOS_TX_BUFFER_END-MIOS_TX_BUFFER)
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_TxBufferPut
;;  C_DECLARATION: void MIOS_MIDI_TxBufferPut(unsigned char b)
;;  DESCRIPTION: puts a byte onto the transmit buffer.<BR>
;;  If Tx buffer is full, the function will be suspended until one byte has been
;;  transmitted via MIDI.<BR>
;;  This function redirects outgoing bytes to the MBHP_IIC_MIDI module if it has
;;  been selected with the MIOS_MIDI_InterfaceSet or MIOS_MIDI_InterfaceAutoSet function
;;  IN:	  byte in WREG
;;  C_IN: byte to be transmitted in <b>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, FSR1
;;  EXAMPLE:
;;
;;	;; put a MIDI byte into the Tx Buffer if it isn't full
;;	movlw	0xf8	; (MIDI Clock)
;;	call	MIOS_MIDI_TxBufferPut
;;
;;  C_EXAMPLE:
;;
;;	// put a MIDI byte into the Tx Buffer
;;	MIOS_MIDI_TxBufferPut(0xf8); // MIDI Clock
;;
;; --------------------------------------------------------------------------
MIOS_MIDI_TxBufferPut
	SET_BSR	MIOS_TX_BUFFER_TMP
	movwf	MIOS_TX_BUFFER_TMP, BANKED		; save byte which should be sent

	;; branch to IIC send function if IIC slave enabled
	SET_BSR	MIOS_IIC_MIDI_ADDR
	movf	MIOS_IIC_MIDI_ADDR, F, BANKED
	bnz	MIOS_MIDI_IIC_Send

	incf	MIOS_TX_BUFFER_TAIL, W, BANKED	; increment ringcounter, but don't save yet
	xorlw	(MIOS_TX_BUFFER_END&0xff)+1
	skpnz
	movlw	(MIOS_TX_BUFFER&0xff) ^ ((MIOS_TX_BUFFER_END&0xff)+1)
	xorlw	(MIOS_TX_BUFFER_END&0xff)+1
	cpfseq	MIOS_TX_BUFFER_HEAD, BANKED
	rgoto MIOS_MIDI_TxBufferPut_Ok
#if 0
	return					; prevent overrun
#else
	movf	MIOS_TX_BUFFER_TMP, W, BANKED
	rgoto	MIOS_MIDI_TxBufferPut		; repeat until buffer is free again
#endif
MIOS_MIDI_TxBufferPut_Ok
	IRQ_DISABLE				; disable interrupts to avoid a malfunction when a Tx IRQ happens here
	movwf	MIOS_TX_BUFFER_TAIL, BANKED	; save counter
	lfsr	FSR1, MIOS_TX_BUFFER		; prepare FSR
	movwf	FSR1L
	movf	MIOS_TX_BUFFER_TMP, W, BANKED	; push byte onto buffer
	movwf	INDF1

	bsf	PIE1, TXIE			; (re-)enable transmit IRQ
	IRQ_ENABLE				; enable interrupts again
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_TxBufferGet
;;  C_DECLARATION: unsigned char MIOS_MIDI_TxBufferGet(void)
;;  DESCRIPTION: gets a byte from the transmit buffer
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  byte to be transmitted in WREG
;;  C_OUT:  byte to be transmitted
;;  USES: BSR, FSR1
;;  EXAMPLE:
;;
;;	;; get a MIDI byte from the Tx Buffer if new byte is available
;;	call	MIOS_MIDI_TxBufferUsed
;;	bz	NoByteAvailable
;;	call	MIOS_MIDI_TxBufferGet
;;
;;  C_EXAMPLE:
;;
;;	unsigned char b;
;;	
;;	// get a MIDI byte from the Tx Buffer if new byte is available
;;	if( MIOS_MIDI_TxBufferUsed() ) {
;;	  b = MIOS_MIDI_TxBufferGet(); 
;;	}
;;
;;  NOTE: <I>this function is for internal use within MIOS only!</I>
;; --------------------------------------------------------------------------
MIOS_MIDI_TxBufferGet
	SET_BSR	MIOS_TX_BUFFER_HEAD
	incf	MIOS_TX_BUFFER_HEAD, F, BANKED
	movlw	(MIOS_TX_BUFFER_END&0xff)+1
	cpfseq	MIOS_TX_BUFFER_HEAD, BANKED
	rgoto MIOS_MIDI_TxBufferGet_NoReset
	movlw	MIOS_TX_BUFFER & 0xff
	movwf	MIOS_TX_BUFFER_HEAD, BANKED
MIOS_MIDI_TxBufferGet_NoReset
	lfsr	FSR1, MIOS_TX_BUFFER
	movff	MIOS_TX_BUFFER_HEAD, FSR1L
	movf	INDF1, W

	;; call user hook
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_MIDI_NotifyTx

	lfsr	FSR1, MIOS_TX_BUFFER
	movff	MIOS_TX_BUFFER_HEAD, FSR1L
	movf	INDF1, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MIDI_TxBufferFlush
;;  C_DECLARATION: void MIOS_MIDI_TxBufferFlush(void)
;;  DESCRIPTION: waits until all MIDI bytes in the Tx buffer have been transmitted
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, FSR1
;;  EXAMPLE:
;;
;;	;; wait until buffer is empty, thereafter send a bunch of new bytes
;;	call	MIOS_MIDI_TxBufferFlush
;;	movlw	0xf0
;;	call	MIOS_MIDI_TxBufferPut
;;	;;  ...
;;
;;  C_EXAMPLE:
;;
;;	// wait until buffer is empty, thereafter send a bunch of new bytes
;;	MIOS_MIDI_TxBufferFlush();
;;	MIOS_MIDI_TxBufferPut(0xf0);
;;	//  ...
;;
;; --------------------------------------------------------------------------
MIOS_MIDI_TxBufferFlush
	clrwdt
MIOS_MIDI_TxBufferFlush_WaitTx
	rcall	MIOS_MIDI_TxBufferUsed
	bnz	MIOS_MIDI_TxBufferFlush_WaitTx
	;; wait until byte has been sent
MIOS_MIDI_TxBufferFlush_Sync
	GOTO_IFCLR TXSTA, TRMT, ACCESS, MIOS_MIDI_TxBufferFlush_Sync	
	return

;; --------------------------------------------------------------------------
;; internal function to send a byte via MBHP_IIC_MIDI
;; note that this module comes with a seperate package for optimized transfers
;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------
;;  IIC MIDI Send function
;;  In:	 byte to send in W
;;  Out: sent byte
;; --------------------------------------------------------------------------
MIOS_MIDI_IIC_Send
	;; temporary store value
	movwf	MIOS_MIDI_IIC_TMP, BANKED

	;; check if 0xff (control byte) should be sent - if so, we have to
	;; send it twice (one for entering command mode, one for exiting
	;; command mode + sending a 0xff at MIDI Out)
	xorlw	0xff
	bnz	MIOS_MIDI_IIC_Send_NotFF
MIOS_MIDI_IIC_Send_FF
	call	MIOS_MIDI_IIC_Send_FFOne ; first time
	;; second time...

MIOS_MIDI_IIC_Send_FFOne
MIOS_MIDI_IIC_Send_NotFF

	;; clear timeout counter (256 tries - more than enough!)
	clrf	MIOS_MIDI_IIC_RETRY_CTR, BANKED

MIOS_MIDI_IIC_SendRestart
	rcall	MIOS_MIDI_IIC_TransmitStart	; start transfer
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_MIDI_IIC_SendTry
MIOS_MIDI_IIC_SendRetry
	call	MIOS_IIC_Stop		; stop transfer
	decfsz	MIOS_MIDI_IIC_RETRY_CTR, F, BANKED	; restart until counter reaches zero
	rgoto	MIOS_MIDI_IIC_SendRestart

	;; give up!
	bcf	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE ; notify that slave is not available
	rgoto	MIOS_MIDI_IIC_Send_End

MIOS_MIDI_IIC_SendTry
	;; try to send byte
	movf	MIOS_MIDI_IIC_TMP, W, BANKED
	call	MIOS_IIC_ByteSend

	;; retry if no acknowledge received
	BRA_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, MIOS_MIDI_IIC_SendRetry

	;; stop transfer
	call	MIOS_IIC_Stop

MIOS_MIDI_IIC_Send_End
	return

;; --------------------------------------------------------------------------

MIOS_MIDI_IIC_TransmitStart
	call	MIOS_IIC_Start		; start IIC
	movf	MIOS_IIC_MIDI_ADDR, W, BANKED	; send address (write transfer)
	andlw	0xfe			; ensure that read flag not set
	goto	MIOS_IIC_ByteSend
	;; (check MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE after this function call!)


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MIDI_NotifyTx
;;  C_DECLARATION: not available in C - code has to be added to mios_wrapper
;;  DESCRIPTION: This function is called by MIOS before the transfer of 
;;  a MIDI byte. It can be used to monitor the Tx activity or to do any
;;  other actions (e.g. to switch a pin for multiplexed MIDI Outs) before
;;  the byte will be sent.
;;
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  IN:	  transmitted byte in WREG
;;  OUT:  -
;;  ISR: yes
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_MIDI_NotifyRx
;;  C_DECLARATION: not available in C - code has to be added to mios_wrapper
;;  DESCRIPTION: This function is called by MIOS when a MIDI byte has been
;;  received. It can be used to monitor the Rx activity or to do any
;;  action - e.g. to react on realtime events like MIDI clock (0xf8) with
;;  a minimum latency.
;;
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  IN:	  received byte in WREG
;;  OUT:  -
;;  ISR: yes
;; --------------------------------------------------------------------------
