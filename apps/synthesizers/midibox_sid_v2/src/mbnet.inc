; $Id$
;
;  MIDIbox specific Driver for ECAN module of PIC18F4685
;
;  The MBNet protocol is described under http://www.uCApps.de/midibox_network.html
;
;  See also cs_menu_mbnet.inc and sid_mbnet.inc
;
; ==========================================================================
;
;  Copyright 2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

; MBNET TOS definitions
MBNET_TOS_SPECIAL	EQU	0
MBNET_TOS_RAM_READ	EQU	1
MBNET_TOS_RAM_WRITE	EQU	2
MBNET_TOS_PING		EQU	3

MBNET_ACK_TOS_OK	EQU	0
MBNET_ACK_TOS_READ	EQU	1
MBNET_ACK_TOS_RETRY	EQU	2
MBNET_ACK_TOS_ERROR	EQU	3


; MBNET State Flags
MBNET_STATE_LOCK_MS_0		EQU	0	; Master which has locked the receiver (bit 0-2)
MBNET_STATE_LOCK_MS_1		EQU	1
MBNET_STATE_LOCK_MS_2		EQU	2
MBNET_STATE_LOCK_RECEIVER	EQU	3	; Lock flag, set by master via Special Command
MBNET_STATE_PANIC		EQU	4	; too many bus errors, recovery required
MBNET_STATE_PERMANENT_OFF	EQU	5	; no recovery possible (transmit errors) - reset required
MBNET_STATE_PING_ACK		EQU	6	; Expecting Ping acknowledge (payload transfered into MBNET_NODE_INFO)


; ==========================================================================

	;; allows to measure the slave response time with a scope
	;; the used port has to be specified here - comment out for no measuring
;; #define MBNET_SLAVE_ACK_MEASURING_PORT	LATC, 3


;; --------------------------------------------------------------------------
;;  FUNCTION: MBNET_Init
;;  DESCRIPTION: should be called after core reset to initialize the 
;;  MBNET interface
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MBNET_Init
	SET_BSR	MBNET_BASE

	;; clear MBNET state
	clrf	MBNET_STATE, BANKED

	;; start with setting where at least the master node is available (no CAN required)
	movlw	0x01
	movwf	MBNET_NODE_AVAIL, BANKED
	clrf	MBNET_NODE_INCOMPATIBLE, BANKED

	;; now check if application is running on a PIC18F468x
	IRQ_DISABLE			; interrupts must be disabled, as TBLPTRU is changed
	TABLE_ADDR_FULL 0x3fffff	; PIC part number is located here
	tblrd*+
	movf	TABLAT, W		; read DEVID2
	xorlw	0x27			; for PIC18F2682/2685/4682/4685
	bz	MBNET_Init_PIC_Ok
MBNET_Init_PIC_NotOk
	;; no ECAN peripheral - disable MBNet permanently
	bsf	MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED
MBNET_Init_PIC_Ok
	clrf	TBLPTRU			; clear TBLPTRU
	IRQ_ENABLE			; enable interrupts again

	;; exit if MBNet has been disabled
	btfsc	MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED
	return


	;; store MIOS Device ID in MBNET_MY_ID for fast access
	call	MIOS_MIDI_DeviceIDGet
	SET_BSR	MBNET_BASE
	movwf	MBNET_MY_ID, BANKED

	;; init MBNET node info (only required for master, node=0)
	bnz	MBNET_Init_NoMaster
MBNET_Init_Master
	lfsr	FSR1, MBNET_NODE_INFO_BEGIN + 0
	movlw	MBNET_CONST_PROTOCOL_V
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C0
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C1
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C2
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C3
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_VERSION
	movwf	POSTINC1
	movlw	LOW(MBNET_CONST_TYPE_SUBVERSION)
	movwf	POSTINC1
	movlw	HIGH(MBNET_CONST_TYPE_SUBVERSION)
	movwf	POSTINC1
MBNET_Init_NoMaster

	;; by default we assume that all nodes are available, and that they are compatible
	;; (prepared for up to 8 CAN nodes)
	setf	MBNET_NODE_AVAIL, BANKED
	clrf	MBNET_NODE_INCOMPATIBLE, BANKED

	;; reset retry counters (will be set once a slave is not available)
	clrf	MBNET_RETRY_WAIT_CTR, BANKED
	lfsr	FSR1, MBNET_RETRY_NODE_CTR_BEGIN
	clrf	MBNET_RETRY_NODE, BANKED
MBNET_Init_RtrResetLoop
	clrf	POSTINC1
	incf	MBNET_RETRY_NODE, F, BANKED
	BRA_IFCLR MBNET_RETRY_NODE, 3, BANKED, MBNET_Init_RtrResetLoop
	clrf	MBNET_RETRY_NODE, BANKED


	;; ReInit used for bus recovery (called from MBNET_BusErrorCheck function)
MBNET_ReInit
	;; clear MBNET_STATE again, but don't touch the PANIC flag
	SET_BSR	MBNET_BASE
	movlw	1 << MBNET_STATE_PANIC
	andwf	MBNET_STATE, F, BANKED

	;; change to configuration mode
	movlw	0x80
	rcall	MBNET_ECAN_ChangeMode

	;; set ECAN mode 2 (enhanced FIFO mode)
	movlw	0x80
	movwf	ECANCON

	;; set bit rate values
	movlw	(0 << 6) | (0 << 0)	; SJW, BRP
	movwf	BRGCON1
	movlw	(1 << 7) | (1 << 6) | (0 << 3) | (5 << 0)	; PHSEG2 Mode, Bus Sample Mode, PHSEG1, PROPSEG
	movwf	BRGCON2
	movlw	(0 << 7) | (0 << 6) | (1 << 0)	; Wakeup Mode, Filter Mode, PHSEG2
	movwf	BRGCON3

	;; Set CANTX2, TXDRIVE and CAN Capture modes.
	movlw	(0 << 7) | (0 << 6) | (1 << 5) | (0 << 0)	; TX2 Source, TX2 Mode, TX Drive Mode, Capture Mode
	movwf	CIOCON


	;; Set RXB0 and RXB1 message buffer modes
	movlw	0x00
	SET_BSR	RXB0CON
	movwf	RXB0CON, BANKED	; all valid messages, use acceptance filter 0
	movwf	RXB1CON, BANKED	; all valid messages, use acceptance filter 0

	;; all additional message buffers are configured for receive mode
	SET_BSR	BSEL0
	clrf	BSEL0, BANKED

	;; clear receive/transmit mode of additional message buffers
	SET_BSR	B0CON
	clrf	B0CON, BANKED
	clrf	B1CON, BANKED
	clrf	B2CON, BANKED
	clrf	B3CON, BANKED
	clrf	B4CON, BANKED
	clrf	B5CON, BANKED

	;; filter #0 used for FIFO, all other filters disabled
	SET_BSR	RXFCON0
	movlw	0x01
	movwf	RXFCON0, BANKED
	clrf	RXFCON1, BANKED

	;; no data bytes filter
	SET_BSR	SDFLC
	clrf	SDFLC, BANKED

	;; assign filter #0 to all buffers
	SET_BSR	RXFBCON0
	clrf	RXFBCON0, BANKED
	clrf	RXFBCON1, BANKED
	clrf	RXFBCON2, BANKED
	clrf	RXFBCON3, BANKED
	clrf	RXFBCON4, BANKED
	clrf	RXFBCON5, BANKED
	clrf	RXFBCON6, BANKED
	clrf	RXFBCON7, BANKED

	;; assign mask #0 to all filters
	SET_BSR	MSEL0
	clrf	MSEL0, BANKED
	clrf	MSEL1, BANKED
	clrf	MSEL2, BANKED
	clrf	MSEL3, BANKED

	;; setup filter #0 - we check for REQUEST_NODE=MIOS Device ID and ACK=0
	movff	MBNET_MY_ID, MBNET_SLAVE_ID
	SET_BSR	MBNET_BASE
	clrf	MBNET_MASTER_ID, BANKED	; (ignored by mask)
	clrf	MBNET_EID_L, BANKED	; (ignored by mask)
	clrf	MBNET_EID_H, BANKED	; (ignored by mask)
	movlw	0x00
	lfsr	FSR1, RXF0SIDH
	rcall	MBNET_SetID

	;; acceptance mask for all filters: only REQUEST_NODE is checked
	;; always expect extended IDs
	SET_BSR	RXM0SIDH
	movlw	0x7f
	movwf	RXM0SIDH, BANKED
 	movlw	0x08	; extended frames are required
	movwf	RXM0SIDL, BANKED
	clrf	RXM0EIDH, BANKED
	clrf	RXM0EIDL, BANKED
	
	;; Set ECAN functional mode
	movlw	0x00
	rgoto	MBNET_ECAN_ChangeMode



;; --------------------------------------------------------------------------
MBNET_Handler
	call	MBNET_ConHandler
	goto	MBNET_RxHandler


;; --------------------------------------------------------------------------
;; connects to slaves
MBNET_ConHandler
	;; exit if this node is not a master (MY_ID != 0)
	SET_BSR	MBNET_BASE
	movf	MBNET_MY_ID, W, BANKED
	bnz	MBNET_ConHandler_End

	;; check all 64 loops
	decf	MBNET_RETRY_WAIT_CTR, W, BANKED
	andlw	0x3f
	movwf	MBNET_RETRY_WAIT_CTR, BANKED
	bnz	MBNET_ConHandler_End

	;; determine next node (master node 0 is skipped)
	incf	MBNET_RETRY_NODE, W, BANKED
	andlw	0x07
	skpnz
	addlw	1
	movwf	MBNET_RETRY_NODE, BANKED

	;; check if retry counter set
	lfsr	FSR1, MBNET_RETRY_NODE_CTR_BEGIN
	movf	MBNET_RETRY_NODE, W, BANKED
	movf	PLUSW1, F
	bz	MBNET_ConHandler_End

	;; decrement counter
	decf	PLUSW1, F

	;; try a recovery on bus errors
	rcall	MBNET_BusErrorCheck
	bnz	MBNET_ConHandler_End	; exit on error

	;; force slave available
	SET_BSR	MBNET_BASE
	movf	MBNET_RETRY_NODE, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	MBNET_NODE_AVAIL, F, BANKED

	;; send a ping, wait for a pong
	movff	MBNET_RETRY_NODE, MBNET_SLAVE_ID; prepare transmission
	clrf	MBNET_EID_L, BANKED		; EID[LH] always 0
	clrf	MBNET_EID_H, BANKED
	movlw	MBNET_TOS_PING			; TOS
	rcall	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	MBNET_ConHandler_End		; skip if node still not available

	movlw	0				; 0 bytes to send
	movwf	POSTINC1			; DLC

	bsf	MBNET_STATE, MBNET_STATE_PING_ACK, BANKED	; flag that we expect a ping acknowlegde
	call	MBNET_Tx_Perform				; performs the transfer and waits for an acknowledge from slave
	bcf	MBNET_STATE, MBNET_STATE_PING_ACK, BANKED	; clear flag

	;; clear retry counter if slave has answered
	bnz	MBNET_ConHandler_End

	lfsr	FSR1, MBNET_RETRY_NODE_CTR_BEGIN
	movf	MBNET_RETRY_NODE, W, BANKED
	clrf	PLUSW1

	;; notify CS that a new CAN node has been detected
	;; node number in WREG and MIOS_PARAMETER1
	movf	MBNET_RETRY_NODE, W, BANKED
	movwf	MIOS_PARAMETER1
	call	CS_MENU_MBNET_FoundNode

	;; return with ZERO flag cleared
	iorlw	0xff
	return

MBNET_ConHandler_End
	andlw	0x00		; ensure that ZERO flag set
	return


;; --------------------------------------------------------------------------
;; checks for incoming messages
MBNET_RxHandler
	;; feed watchdog
	clrwdt
	
	;; try a recovery on bus errors
	rcall	MBNET_BusErrorCheck
	skpz			; exit on error
	return

	;; check if something has been received
	;; map FIFO head to 0xf60 (access bank), so that it can be accessed via RXB0CON
	movf	CANCON, W
	andlw	0x07
	iorlw	0x90
	movwf	ECANCON
	BRA_IFSET RXB0CON, RXFUL, ACCESS, MBNET_RxHandler_Go

	;; loop so long receive handler is locked
	SET_BSR	MBNET_BASE
	BRA_IFSET MBNET_STATE, MBNET_STATE_LOCK_RECEIVER, BANKED, MBNET_RxHandler
	return			; else exit

MBNET_RxHandler_Go
	;; ignore message if it wasn't a REQ (TODO: or send a retry?)
	BRA_IFCLR RXB0SIDH, 7, ACCESS, MBNET_RxHandler_Go_Req
	bcf	RXB0CON, RXFUL, ACCESS
	rgoto	MBNET_RxHandler
MBNET_RxHandler_Go_Req

	SET_BSR	MBNET_BASE

	;; extract master ID and copy it to MBNET_MASTER_ID
	rrf	RXB0SIDL, W, ACCESS	; located in EIDH[17:16]
	andlw	0x70
	movwf	MBNET_MASTER_ID, BANKED

	;; special case if receiver has been locked by another master: send retry acknowledge
	BRA_IFCLR MBNET_STATE, MBNET_STATE_LOCK_RECEIVER, BANKED, MBNET_RxHandler_Go_NoRetry
MBNET_RxHandler_Go_RetryChk
	swapf	MBNET_MASTER_ID, W, BANKED
	xorwf	MBNET_STATE, W, BANKED
	andlw	0x07
	bnz	MBNET_RxHandler_AckRetry
MBNET_RxHandler_Go_NoRetry

	;; check TOS (type of service)
	movf	RXB0SIDL, W, ACCESS	; located in EIDH[17:16]
	andlw	0x03
	bz	MBNET_RxHandler_Special	; TOS=0: Special Service
	addlw	-1
	bz	MBNET_RxHandler_RAMRd    ; TOS=1: reads from RAM
	addlw	-1
	bz	MBNET_RxHandler_RAMWr    ; TOS=2: writes into RAM
	rgoto	MBNET_RxHandler_Ping	; TOS=3: ping

	;; all of these functions branch back to the acknowledge response
MBNET_RxHandler_AckOk
	movlw	MBNET_ACK_TOS_OK
	rgoto	MBNET_RxHandler_Ack
MBNET_RxHandler_AckRetry
	movlw	MBNET_ACK_TOS_RETRY
	rgoto	MBNET_RxHandler_Ack
MBNET_RxHandler_AckError
	movlw	MBNET_ACK_TOS_ERROR
	rgoto	MBNET_RxHandler_Ack
MBNET_RxHandler_NoAck
	movlw	-1
	;; 	rgoto	MBNET_RxHandler_Ack

MBNET_RxHandler_Ack
	;; expecting ACK_TOS in WREG
	;; if -1, no acknowledge will be sent (already done before - RAM read case)

	;; release buffer
	bcf	RXB0CON, RXFUL, ACCESS

	;; send acknowledge, TOS in WREG
	addlw	1
	bz	MBNET_RxHandler_AckSkip
	addlw	-1
	rcall	MBNET_SendAck

MBNET_RxHandler_AckSkip
	;; loop until no new message
	rgoto	MBNET_RxHandler




;; TOS Handlers

MBNET_RxHandler_RAMRd
	;; wait until transmit buffer #1 free
MBNET_RxHandler_RAMRd_Loop
	rcall	MBNET_BusErrorCheck
	bnz	MBNET_RxHandler_NoAck
	SET_BSR	TXB1CON
	BRA_IFSET TXB1CON, TXREQ, BANKED, MBNET_RxHandler_RAMRd_Loop

	;; call hook in MBSID engine
	lfsr	FSR2, TXB1DLC
	call	SID_MBNET_RAM_Read

	;; prepare ID field for acknowledge message
	lfsr	FSR1, TXB1SIDH
	movlw	MBNET_ACK_TOS_READ	; (response on read request)
	rcall	MBNET_SetAckID

	;; start transfer
	SET_BSR	TXB1CON
	bsf	TXB1CON, TXREQ, BANKED

	;; skip "normal" acknowledge message
	rgoto	MBNET_RxHandler_NoAck


MBNET_RxHandler_RAMWr
	;; call hook in MBSID engine
	lfsr	FSR2, TXB1DLC
	call	SID_MBNET_RAM_Write
	;; WREG contains ACK TOS
	rgoto	MBNET_RxHandler_Ack

	
MBNET_RxHandler_Ping
	;; release buffer
	bcf	RXB0CON, RXFUL, ACCESS

	;; send acknowledge with ok status
	movlw	0
	rcall	MBNET_SendAckPong

	;; skip "normal" acknowledge message
	rgoto	MBNET_RxHandler_NoAck


MBNET_RxHandler_Special
	;; ignore number of received byte (ignored for future compatibility, service is coded in EID)

	;; branch depending on EIDL
	movf	RXB0EIDL, W, ACCESS
	andlw	0xf0
	bnz	MBNET_RxHandler_Special_App	; branch if Application Specific ETOS

	movf	RXB0EIDL, W, ACCESS		; HLP specific ETOS
	bz	MBNET_RxHandler_Special_LR	; ETOS=0 -> lock receiver
	addlw	-1
	bz	MBNET_RxHandler_Special_ULR	; ETOS=1 -> unlock receiver
	addlw	-1-1-1				; ETOS=2/3 skipped
	bz	MBNET_RxHandler_Special_M4	; ETOS=4 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M5	; ETOS=5 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M6	; ETOS=6 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M7	; ETOS=7 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M8	; ETOS=8 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M9	; ETOS=9 (MIOS hook)
	addlw	-1
	bz	MBNET_RxHandler_Special_M10	; ETOS=10 (MIOS hook)
	addlw	-5
	bz	MBNET_RxHandler_Special_M15	; ETOS=15 (MBNet Clone function)
	rgoto	MBNET_RxHandler_AckError		; unsupported ETOS

MBNET_RxHandler_Special_App
	;; call hook in MBSID engine
	movf	RXB0EIDL, W, ACCESS
	;; WREG contains received TOS
	call	SID_MBNET_RxSpecial
	;; WREG contains Ack TOS
	rgoto	MBNET_RxHandler_Ack

	;; lock receiver and store master ID in MBNET_STATE[2:0]
MBNET_RxHandler_Special_LR
	bsf	MBNET_STATE, MBNET_STATE_LOCK_RECEIVER, BANKED

	andlw	0xf8
	andwf	MBNET_STATE, F, BANKED
	swapf	MBNET_MASTER_ID, W, BANKED
	andlw	0x07
	iorwf	MBNET_STATE, F, BANKED
	rgoto	MBNET_RxHandler_AckOk

	;; unlock receiver
MBNET_RxHandler_Special_ULR
	bcf	MBNET_STATE, MBNET_STATE_LOCK_RECEIVER, BANKED
	rgoto	MBNET_RxHandler_AckOk

	;; call USER_Init
MBNET_RxHandler_Special_M4
	;; call hook
	call	USER_Init
	rgoto	MBNET_RxHandler_AckOk
	
	;; call USER_MPROC_DebugTrigger
MBNET_RxHandler_Special_M5
	;; error if != 4 byte received
	movlw	4
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer bytes into WREG and MIOS_PARAMETER[123]
	movf	RXB0D3, W, ACCESS
	movwf	MIOS_PARAMETER3
	movf	RXB0D2, W, ACCESS
	movwf	MIOS_PARAMETER2
	movf	RXB0D1, W, ACCESS
	movwf	MIOS_PARAMETER1
	movf	RXB0D0, W, ACCESS

	;; call hook
	call	USER_MPROC_DebugTrigger

	rgoto	MBNET_RxHandler_AckOk
	
	;; call USER_MPROC_NotifyReceivedEvent
MBNET_RxHandler_Special_M6
	;; error if != 3 byte received
	movlw	3
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer bytes into MIOS_PARAMETER[123]
	movf	RXB0D2, W, ACCESS
	movwf	MIOS_PARAMETER3
	movf	RXB0D1, W, ACCESS
	movwf	MIOS_PARAMETER2
	movf	RXB0D0, W, ACCESS
	movwf	MIOS_PARAMETER1

	;; call hook
	call	USER_MPROC_NotifyReceivedEvent

	rgoto	MBNET_RxHandler_AckOk
	
	;; call USER_MPROC_NotifyReceivedByte
MBNET_RxHandler_Special_M7
	;; error if != 1 byte received
	movlw	1
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer byte into WREG and MIOS_PARAMETER[1]
	movf	RXB0D0, W, ACCESS
	movwf	MIOS_PARAMETER1

	;; call hook
	call	USER_MPROC_NotifyReceivedByte

	rgoto	MBNET_RxHandler_AckOk
	
	;; call USER_DIN_NotifyToggle
MBNET_RxHandler_Special_M8
	;; error if != 2 bytes received
	movlw	2
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer bytes into WREG and MIOS_PARAMETER[12]
	movf	RXB0D1, W, ACCESS
	movwf	MIOS_PARAMETER2
	movf	RXB0D0, W, ACCESS
	movwf	MIOS_PARAMETER1

	;; call hook
	call	USER_DIN_NotifyToggle

	rgoto	MBNET_RxHandler_AckOk
	
	;; call USER_ENC_NotifyChange
MBNET_RxHandler_Special_M9
	;; error if != 2 bytes received
	movlw	2
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer bytes into WREG and MIOS_PARAMETER[12]
	movf	RXB0D2, W, ACCESS
	movwf	MIOS_PARAMETER2
	movf	RXB0D1, W, ACCESS
	movwf	MIOS_PARAMETER1

	;; call hook
	call	USER_ENC_NotifyChange

	rgoto	MBNET_RxHandler_AckOk
	
	
	;; call USER_AIN_NotifyChange
MBNET_RxHandler_Special_M10
	;; error if != 3 bytes received
	movlw	3
	cpfseq	RXB0DLC, ACCESS
	rgoto	MBNET_RxHandler_AckError

	;; transfer bytes into WREG and MIOS_PARAMETER[12]
	movf	RXB0D1, W, ACCESS
	movwf	MIOS_PARAMETER2
	movf	RXB0D0, W, ACCESS
	movwf	MIOS_PARAMETER1

	;; call hook
	call	USER_AIN_NotifyChange

	rgoto	MBNET_RxHandler_AckOk


MBNET_RxHandler_Special_M15

	;; NEW: switch to ECAN mode #1 for legacy reasons (see comments in mbnet_clone.inc)
	movlw	0x80		; switch to configuration mode
	rcall	MBNET_ECAN_ChangeMode
	movlw	0x40		; set ECAN mode 1
	movwf	ECANCON
	movlw	0x00		; switch to functional mode
	rcall	MBNET_ECAN_ChangeMode

	movlw	0x50		; map RXB0CON
	movwf	ECANCON

	lfsr	FSR1, RXB0CON	; ...the old MBNET_CLONE implementation needs this
	movff	FSR1L, MBNET_RX_BUFFER_PTR_L
	movff	FSR1H, MBNET_RX_BUFFER_PTR_H

	;; branch to cloning routine (will acknowledge by itself)
	call	MBNET_CLONE

	;; NEW: switch back to ECAN mode #2
	rcall	MBNET_ReInit

	;; this function will reset the device at the end
	;; if it returns, an error has happened
	rgoto	MBNET_RxHandler_AckError



;; --------------------------------------------------------------------------
MBNET_SendAck
	SET_BSR	MBNET_BASE	; temporary store TOS in MBNET_TOS
	movwf	MBNET_TOS, BANKED

	;; wait until transmit buffer #1 free
MBNET_SendAckLoop
	;; check for error
	rcall	MBNET_BusErrorCheck
	bnz	MBNET_SendAck_Fail
	SET_BSR	TXB1CON
	BRA_IFSET TXB1CON, TXREQ, BANKED, MBNET_SendAckLoop

	;; prepare ID field for acknowledge message
	lfsr	FSR1, TXB1SIDH
	SET_BSR	MBNET_BASE
	movf	MBNET_TOS, W, BANKED
	rcall	MBNET_SetAckID

	;; 0 byte
	SET_BSR	TXB1DLC
	clrf	TXB1DLC, BANKED

	;; start transfer
	bsf	TXB1CON, TXREQ, BANKED

	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

MBNET_SendAck_Fail
	;; nothing else to do - the master will disable the slave,
	;; connection has to be established again via "ping"
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

;; --------------------------------------------------------------------------
MBNET_SendAckPong
	;; wait until transmit buffer #1 free
MBNET_SendAckPongLoop
	rcall	MBNET_BusErrorCheck
	bnz	MBNET_SendAckPong_Fail
	SET_BSR	TXB1CON
	BRA_IFSET TXB1CON, TXREQ, BANKED, MBNET_SendAckPongLoop

	;; prepare ID field for acknowledge message
	lfsr	FSR1, TXB1SIDH
	movlw	MBNET_ACK_TOS_OK
	rcall	MBNET_SetAckID

	;; 8 byte
	lfsr	FSR1, TXB1DLC
	movlw	8
	movwf	POSTINC1

	movlw	MBNET_CONST_PROTOCOL_V	; D0
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C0	; D1
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C1	; D2
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C2	; D3
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_C3	; D4
	movwf	POSTINC1
	movlw	MBNET_CONST_TYPE_VERSION	; D5
	movwf	POSTINC1
	movlw	LOW(MBNET_CONST_TYPE_SUBVERSION)	; D6
	movwf	POSTINC1
	movlw	HIGH(MBNET_CONST_TYPE_SUBVERSION) ; D7
	movwf	POSTINC1

	;; start transfer
	SET_BSR	TXB1CON
	bsf	TXB1CON, TXREQ, BANKED

	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

MBNET_SendAckPong_Fail
	;; nothing else to do - the master will disable the slave,
	;; connection has to be established again via "ping"
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return


;; --------------------------------------------------------------------------
;; pointer to SIDH register in FSR1
;; EID[17:16] (TOS) in WREG
;; SID[10] will always be set to 0 (command)
;; SID[9:3] (MIOS Device ID of Slave) in MBNET_SLAVE_ID
;; SID[2:0] (MIOS Device ID[6:4] of Master) in MBNET_MASTER_ID
;; EID[15:0] (address or other TOS specific information) in MBNET_EID_[LH]
MBNET_SetID
	movff	MBNET_SLAVE_ID, INDF1	; MBNET_SLAVE_ID -> SID[9:3]
	bcf	POSTINC1, 7		; SID[10] = 0 (Command)

	andlw	0x03			; TOS[1:0] -> EID[17:16]
	iorlw	0x08			; always extended ID
	movwf	INDF1

	movff	MBNET_MASTER_ID, WREG	; master ID[6:4] -> SID[2:0]
	rlf	WREG, W
	andlw	0xe0
	iorwf	POSTINC1, F

	;; transfer EID[15:0]
	movff	MBNET_EID_H, POSTINC1
	movff	MBNET_EID_L, POSTINC1

	return


;; --------------------------------------------------------------------------
;; pointer to SIDH register in FSR1
;; EID[17:16] (TOS) in WREG
;; SID[10] will always be set to 1 (ACK)
;; SID[9:3] (MIOS Device ID of Master) in MBNET_MASTER_ID
;; SID[2:0] will always be set to 0
;; EID[15:8] will always be set to 0
;; EID[7:0] (MIOS Device ID of responding slave) in MBNET_MY_ID
MBNET_SetAckID
	movff	MBNET_MASTER_ID, INDF1	; MBNET_MASTER_ID -> SID[9:3]
	bsf	POSTINC1, 7		; SID[10] = 1 (Ack)

	andlw	0x03			; TOS[1:0] -> EID[17:16]
	iorlw	0x08			; always extended ID
	movwf	POSTINC1

	;; EIDH always 0
	clrf	POSTINC1

	;; EIDL contains responding slave ID (for comparison at master side)
	movff	MBNET_MY_ID, POSTINC1

	return

;; --------------------------------------------------------------------------
MBNET_ECAN_ChangeMode
	movwf	PRODL

	movf	CANCON, W
	andlw	0x1f
	iorwf	PRODL, W
	movwf	CANCON

	;; wait until requested mode has changed
MBNET_ECAN_ChangeModeLoop
	movf	CANSTAT, W
	andlw	0xe0
	xorwf	PRODL, W
	bnz	MBNET_ECAN_ChangeModeLoop
	return


;; --------------------------------------------------------------------------
;; returns pointer to RX buffer depending on CANCON.FP in FSR1
MBNET_GetRxPtr
	BRA_IFSET CANCON, 2, ACCESS, MBNET_GetRxPtr_4567
MBNET_GetRxPtr_0123
	BRA_IFSET CANCON, 1, ACCESS, MBNET_GetRxPtr_23
MBNET_GetRxPtr_01
	lfsr	FSR1, RXB0CON
	btfsc	CANCON, 0
	lfsr	FSR1, RXB1CON
	return

MBNET_GetRxPtr_23
	lfsr	FSR1, B0CON
	btfsc	CANCON, 0
	lfsr	FSR1, B1CON
	return

MBNET_GetRxPtr_4567
	BRA_IFSET CANCON, 1, ACCESS, MBNET_GetRxPtr_67
MBNET_GetRxPtr_45
	lfsr	FSR1, B2CON
	btfsc	CANCON, 0
	lfsr	FSR1, B3CON
	return

MBNET_GetRxPtr_67
	lfsr	FSR1, B4CON
	btfsc	CANCON, 0
	lfsr	FSR1, B5CON
	return

;; --------------------------------------------------------------------------
;; prepare transfer
;; IN: TOS[3:0] in WREG
;;     MIOS Device ID in MBNET_SLAVE_ID
;;     EID[15:0] (address or other TOS specific information) in MBNET_EID_[LH]
;; OUT: pointer to DLC of transmit buffer in FSR1
;;      ZERO flag set when slave available
MBNET_Tx_Prepare
	;; store byte in MBNET_TOS
	SET_BSR	MBNET_BASE
	movwf	MBNET_TOS, BANKED

	;; exit with error if slave not available
	movf	MBNET_SLAVE_ID, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MBNET_NODE_AVAIL, W, BANKED
	bz	MBNET_Tx_Prepare_Fail

	;; wait until transmit buffer free
	;; abort on bus errors and start recovery
MBNET_Tx_Prepare_WaitLoop
	rcall	MBNET_BusErrorCheck
	bnz	MBNET_Tx_Prepare_Fail
	SET_BSR	TXB0CON
	BRA_IFSET TXB0CON, TXREQ, BANKED, MBNET_Tx_Prepare_WaitLoop

	;; set new ID (TOS in MBNET_TOS, slave id in MBNET_SLAVE_ID)
	SET_BSR	MBNET_BASE
	movf	MBNET_TOS, W, BANKED
	movff	MBNET_MY_ID, MBNET_MASTER_ID
	lfsr	FSR1, TXB0SIDH
	rcall	MBNET_SetID

	;; return pointer to TXB0DLC in FSR1
	lfsr	FSR1, TXB0DLC

	;; and exit
	andlw	0x00
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

MBNET_Tx_Prepare_Fail
	iorlw	0xff
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

	
;; --------------------------------------------------------------------------
;; starts a transfer and waits for acknowledge from slave
;;      ZERO flag set when slave responded with acknowledge
;; NOTE: this routine is splitted into two parts. It is possible to transfer
;; the package, to do something else and to wait for the acknowledge
;; later (MBNET_Tx_Perform_Start, MBNET_Tx_Perform_Finish)
;; IN: expected slave ID in MBNET_SLAVE_ID
MBNET_Tx_Perform_Start
MBNET_Tx_Perform_Start_RxClear
	;; discard evtl. received ACK message (fault tolerant)
	movf	CANCON, W
	andlw	0x07
	iorlw	0x90
	movwf	ECANCON
	BRA_IFCLR RXB0CON, RXFUL, ACCESS, MBNET_Tx_Perform_Start_RxCleared
	BRA_IFCLR RXB0SIDH, 7, ACCESS, MBNET_Tx_Perform_Start_RxCleared
	bcf	RXB0CON, RXFUL, ACCESS
	movlw	0xd0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	rgoto	MBNET_Tx_Perform_Start_RxClear

MBNET_Tx_Perform_Start_RxCleared
	SET_BSR	TXB0CON
	bsf	TXB0CON, TXREQ, BANKED	; start the transfer

	;; wait until it is finished
MBNET_Tx_Perform_StartLoop
	BRA_IFCLR TXB0CON, TXREQ, BANKED, MBNET_Tx_Perform_StartLoop_End
	;; skip on Tx error (CAN bus not connected?) --- don't use MBNet_BusErrorCheck here to avoid register collisions
	movf	COMSTAT, W
	andlw	(1 << TXBO) | (1 << TXBP)
	bz	MBNET_Tx_Perform_StartLoop
MBNET_Tx_Perform_StartLoop_End

#ifdef MBNET_SLAVE_ACK_MEASURING_PORT
	bsf	MBNET_SLAVE_ACK_MEASURING_PORT
#endif
	return


MBNET_Tx_Perform
	rcall	MBNET_Tx_Perform_Start
	rgoto	MBNET_Tx_Perform_Finish


	;; perform function w/o timeout - should only be used for PIC cloning!
MBNET_Tx_Perform_NoTO
	rcall	MBNET_Tx_Perform_Start
MBNET_Tx_Perform_NoTO_Loop
	movf	CANCON, W
	andlw	0x07
	iorlw	0x90
	movwf	ECANCON
	BRA_IFCLR RXB0CON, RXFUL, ACCESS, MBNET_Tx_Perform_NoTO_Loop
	rgoto	MBNET_Tx_Perform_Finish_Cont


MBNET_Tx_Perform_Finish
	;; waiting for acknowledge

	;; init timeout counter
	SET_BSR	MBNET_BASE
	clrf	MBNET_TIMEOUT_CTR_L, BANKED
	clrf	MBNET_TIMEOUT_CTR_H, BANKED

	;; wait until something has been received
	;; timeout after 8192 loops (ca. 10 mS when interrupts are running in background)
	;; UPDATE: increased timeout value since SysEx transfers via editor caused dropouts!
MBNET_Tx_PerformLoop
	SET_BSR	MBNET_BASE
	incf	MBNET_TIMEOUT_CTR_L, F, BANKED
	skpnz
	incf	MBNET_TIMEOUT_CTR_H, F, BANKED
#if 0
	BRA_IFSET MBNET_TIMEOUT_CTR_H, 5, BANKED, MBNET_Tx_Perform_Failed
#else
	;; to avoid dropout during SysEx transfers... :-/
	BRA_IFSET MBNET_TIMEOUT_CTR_H, 7, BANKED, MBNET_Tx_Perform_Failed
#endif
	movf	CANCON, W
	andlw	0x07
	iorlw	0x90
	movwf	ECANCON
	BRA_IFCLR RXB0CON, RXFUL, ACCESS, MBNET_Tx_PerformLoop

	;; ignore message if it wasn't an ACK (TODO: or send a retry?)
	BRA_IFSET RXB0SIDH, 7, ACCESS, MBNET_Tx_Perform_Finish_Cont
	movlw	0xd1
	call	MIOS_MIDI_TxBufferPut
	movf	RXB0CON, W, ACCESS
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	bcf	RXB0CON, RXFUL, ACCESS
	rgoto	MBNET_Tx_PerformLoop
	
MBNET_Tx_Perform_Finish_Cont
	;; acknowledge message received
	;; clear panic state (seems that the bus is available again)
	SET_BSR	MBNET_BASE
	bcf	MBNET_STATE, MBNET_STATE_PANIC, BANKED

	;; check that EIDL contains the expected slave ID
	movf	RXB0EIDL, W, ACCESS
	cpfseq	MBNET_SLAVE_ID, BANKED
	rgoto	MBNET_Tx_Perform_Failed

	;; branch depending on acknowledge TOS
	movf	RXB0SIDL, W, ACCESS
	andlw	0x03
	bz	MBNET_Tx_Perform_AckOk	; TOS=0 (Ok)
	addlw	-1
	bz	MBNET_Tx_Perform_AckOk	; TOS=1 (Read Response) handled like TOS=0, payload taken in cs_menu_mbnet.inc
	addlw	-1
	bz	MBNET_Tx_Perform_Retry	; TOS=2 (Retry)
	rgoto	MBNET_Tx_Perform_Failed	; TOS=3 (Error)

MBNET_Tx_Perform_AckOk
	;; if this was the response of a ping (MBNET_STATE_PING_ACK flag set):
	;; check that we received 8 bytes
	;; transfer the payload to MBNET_NODE_INFO
	BRA_IFCLR MBNET_STATE, MBNET_STATE_PING_ACK, BANKED, MBNET_Tx_Perform_Cont
MBNET_Tx_Perform_AckOk_Pong
	;; determine pointer to INFO array
	lfsr	FSR1, MBNET_NODE_INFO_BEGIN
	rlf	MBNET_SLAVE_ID, W, BANKED
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x38
	addwf	FSR1L, F

	;; transfer playload
	movff	RXB0D0, POSTINC1
	movff	RXB0D1, POSTINC1
	movff	RXB0D2, POSTINC1
	movff	RXB0D3, POSTINC1
	movff	RXB0D4, POSTINC1
	movff	RXB0D5, POSTINC1
	movff	RXB0D6, POSTINC1
	movff	RXB0D7, POSTINC1

	;; error if we haven't received 8 bytes - set incompatible flag in this case and clear info array
	movf	RXB0DLC, W, ACCESS
	xorlw	8
	bnz	MBNET_Tx_Perform_AckOk_PongNotOk

	;; error if protocol version doesn't match
	movlw	MBNET_CONST_PROTOCOL_V
	cpfseq	RXB0D0, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk

	;; error if type characters don't match
	movlw	MBNET_CONST_TYPE_C0
	cpfseq	RXB0D1, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk
	movlw	MBNET_CONST_TYPE_C1
	cpfseq	RXB0D2, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk
	movlw	MBNET_CONST_TYPE_C2
	cpfseq	RXB0D3, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk
	movlw	MBNET_CONST_TYPE_C3
	cpfseq	RXB0D4, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk

	;; error if type version number doesn't match
	movlw	MBNET_CONST_TYPE_VERSION
	cpfseq	RXB0D5, ACCESS
	rgoto	MBNET_Tx_Perform_AckOk_PongNotOk

	;; subversions are ignored
	rgoto	MBNET_Tx_Perform_Cont

MBNET_Tx_Perform_AckOk_PongNotOk
	SET_BSR	MBNET_BASE
	movf	MBNET_SLAVE_ID, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	MBNET_NODE_INCOMPATIBLE, F, BANKED

	rgoto	MBNET_Tx_Perform_Failed


MBNET_Tx_Perform_Retry
	;; retry has been received - send transmit buffer again
	rgoto	MBNET_Tx_Perform


MBNET_Tx_Perform_Cont
	;; release message buffer
	bcf	RXB0CON, RXFUL, ACCESS

#ifdef MBNET_SLAVE_ACK_MEASURING_PORT
	bcf	MBNET_SLAVE_ACK_MEASURING_PORT
#endif

	;; ZERO bit set
	andlw	0x00
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

MBNET_Tx_Perform_Failed
	;; release message buffer
	bcf	RXB0CON, RXFUL, ACCESS

	;; slave didn't respond, disable it
	;; Note: a connection to the slave can be retried by pressing the appr. SID button on the Control Surface
	SET_BSR	MBNET_BASE
	movf	MBNET_SLAVE_ID, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	MBNET_NODE_AVAIL, F, BANKED

#if 0
	movf	MBNET_SLAVE_ID, W, BANKED
	andlw	0x0f
	iorlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movff	MBNET_TOS, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	movff	MBNET_EID_L, WREG
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
#endif

#ifdef MBNET_SLAVE_ACK_MEASURING_PORT
	bcf	MBNET_SLAVE_ACK_MEASURING_PORT
#endif

	;; ZERO bit must be cleared (however, it already is due to the last operation, but this is just more secure)
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;; used during transmit or receive polling to determine if a bus error has occured
;; (e.g. receiver passive or no nodes connected to bus)
;; In this case, all pending transmissions will be aborted
;; The MBNET_STATE_PANIC flag is set to report to the Control Surface, that the
;; bus is not ready for transactions. This flag will be cleared by WaitAck once we
;; got back a message from any slave
;; ZERO flag set when no error
MBNET_BusErrorCheck
	;; skip if bus permanently off
	SET_BSR	MBNET_BASE
	BRA_IFSET MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED, MBNET_BusErrorCheck_Panic

	movf	COMSTAT, W
	andlw	(1 << TXBO) | (1 << TXBP) | (1 << RXBP)
	bz	MBNET_BusErrorCheck_DontPanic

MBNET_BusError_Recover
	;; abort all pending transmissions
	bsf	CANCON, ABAT

	;; notify that an error has happened
	SET_BSR	MBNET_BASE
	bsf	MBNET_STATE, MBNET_STATE_PANIC, BANKED

	;; if TXBO flag set: don't access bus anymore (transmit failed), turn off ECAN
	BRA_IFCLR COMSTAT, TXBO, ACCESS, MBNET_BusError_RecoverPossible
MBNET_BusError_NoRecovery
	bsf	MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED

	;; disable all slave nodes
	movlw	0x01
	andwf	MBNET_NODE_AVAIL, F, BANKED

	;; change to disable mode
	movlw	0x20
	rcall	MBNET_ECAN_ChangeMode

#if 0
	movff	MBNET_MY_ID, WREG
	iorlw	0xd0
	call	MIOS_MIDI_TxBufferPut
	movf	COMSTAT, W
	call	MIOS_MIDI_TxBufferPut
#endif

	rgoto	MBNET_BusErrorCheck_Panic
	
MBNET_BusError_RecoverPossible
	;; re-initialise CAN if bus not permanently off
	rcall	MBNET_ReInit
	
MBNET_BusErrorCheck_Panic
	iorlw	0xff		; notify error
	return

MBNET_BusErrorCheck_DontPanic
	andlw	0x00		; no error
	return


;; --------------------------------------------------------------------------
;;  Requests a Master->Slave Synchronisation
;;  IN: Node which should be retried in WREG
;; --------------------------------------------------------------------------
MBNET_RequestSync
	movwf	PRODL
	lfsr	FSR1, MBNET_RETRY_NODE_CTR_BEGIN
	addwf	FSR1L, F
	movlw	64		; retry 8 times
	movwf	INDF1

	movf	PRODL, W
	call	MIOS_HLP_GetBitANDMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_AVAIL, F, BANKED

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBNET_Clone_Me
;;  DESCRIPTION: transfers the flash content of the master to all slaves
;;  MBNET interface
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MBNET_Clone_STR_START
	STRING	14, 0x00, "Cloning Slaves"	; ("s" will be erased)

MBNET_Clone_STR_ADDRESS
	STRING	10, 0x40, "Address: 0x"

MBNET_Clone_STR_WAIT_RESET
	STRING	17, 0x40, "Waiting for Reset"

MBNET_Clone_STR_FAILED
	STRING	17, 0x40, "!!!! FAILED !!!! "

MBNET_Clone_Me
	;; skip if not master
	SET_BSR	MBNET_BASE
	movf	MBNET_MY_ID, W, BANKED
	andlw	0x70
	skpz
	return

	;; print message
	call	MIOS_LCD_Clear
	TABLE_ADDR MBNET_Clone_STR_START
	call	MIOS_LCD_PrintString

	;; wait two seconds to give slaves a chance to boot properly
	movlw	8
	movwf	PRODL
MBNET_Clone_Me_WaitLoop
	movlw	255
	call	MIOS_Delay
	decfsz	PRODL, F
	rgoto	MBNET_Clone_Me_WaitLoop

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut

	;; start with slave ID #01
	movlw	0x01
	SET_BSR	MBNET_BASE
	movwf	MBNET_SLAVE_ID, BANKED
MBNET_Clone_Me_Loop
	;; print slave number
	movlw	0x00 + 13
	call	MIOS_LCD_CursorSet
	movlw	' '			; erase "s" of "slaves"
	call	MIOS_LCD_PrintChar
	movff	MBNET_SLAVE_ID, WREG	; slave number
	call	MIOS_LCD_PrintHex2
	TABLE_ADDR MBNET_Clone_STR_ADDRESS	; address line
	call	MIOS_LCD_PrintString
	SET_BSR	MBNET_BASE

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

	;; store slave ID in TMP1
	SET_BSR	MBNET_BASE
	movf	MBNET_SLAVE_ID, W, BANKED
	movwf	TMP1
	rcall	MBNET_RequestSync

	;; search for slave until all retry counter is zero
MBNET_Clone_Me_WaitSlave
	movlw	1
	call	MIOS_Delay
	clrwdt
	rcall	MBNET_ConHandler

	lfsr	FSR1, MBNET_RETRY_NODE_CTR_BEGIN
	movf	TMP1, W
	movf	PLUSW1, W
	bnz	MBNET_Clone_Me_WaitSlave

	;; restore slave ID
	SET_BSR	MBNET_BASE
	movf	TMP1, W
	movwf	MBNET_SLAVE_ID, BANKED

	;; break if slave not available
	;; 	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBNET_NODE_AVAIL, W, BANKED
	skpnz
	rgoto	MBNET_Clone_Me_Failed


	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x02
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

	movlw	0				; ETOS=0 -> lock receiver
	rcall	MBNET_Clone_Me_SendCmd
	skpz
	rgoto	MBNET_Clone_Me_Failed

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x03
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

	movlw	15				; ETOS=15 -> clone
	rcall	MBNET_Clone_Me_SendCmd
	skpz
	rgoto	MBNET_Clone_Me_Failed

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x04
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

	;; slave now in special receive state (code located in mbnet_clone.inc)
	;; start clone loop - start from 0x3000
	TABLE_ADDR_FULL 0x3000
MBNET_Clone_Me_TxLoop_O
	;; print new address
	movf	TBLPTRL, W	; speed up: output only each 256 bytes
	bnz	MBNET_Clone_Me_TxLoop_O_NoLCD
	movlw	0x40 + 10
	call	MIOS_LCD_CursorSet
	movf	TBLPTRU, W
	call	MIOS_LCD_PrintHex2
	movf	TBLPTRH, W
	call	MIOS_LCD_PrintHex2
	movf	TBLPTRL, W
	call	MIOS_LCD_PrintHex2
	movlw	' '		; (clear artefact - the 't' - of "waiting for reset" message)
	call	MIOS_LCD_PrintChar
MBNET_Clone_Me_TxLoop_O_NoLCD

	clrwdt					; feed watchdog
	movff	TBLPTRL, MBNET_EID_L		; EIDL contains low-byte of address
	movff	TBLPTRH, MBNET_EID_H		; EIDH contains high-byte of address
	rrf	TBLPTRU, W			; address must be right-shifted by 3 to cover the whole flash range
	rrf	MBNET_EID_H, F, BANKED
	rrf	MBNET_EID_L, F, BANKED
	clrc
	rrf	MBNET_EID_H, F, BANKED
	rrf	MBNET_EID_L, F, BANKED
	clrc
	rrf	MBNET_EID_H, F, BANKED
	rrf	MBNET_EID_L, F, BANKED
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM (here: FLASH/EEPROM)
	rcall	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	MBNET_Clone_Me_Failed		; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from flash and write them into data buffer
MBNET_Clone_Me_TxLoop_I
	tblrd*+
	movff	TABLAT, POSTINC1
	movf	TBLPTRL, W
	andlw	0x07
	bnz	MBNET_Clone_Me_TxLoop_I

	rcall	MBNET_Tx_Perform_NoTO		; performs the transfer and waits for an acknowledge from slave (without timeout)
	bnz	MBNET_Clone_Me_Failed

	;; loop until last byte has been sent
	movf	TBLPTRU, W
	bz	MBNET_Clone_Me_TxLoop_O
	movf	TBLPTRL, W
	bnz	MBNET_Clone_Me_TxLoop_O
	movf	TBLPTRH, W
	xorlw	0x80
	bnz	MBNET_Clone_Me_TxLoop_O

	;; the end

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x05
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

MBNET_Clone_Me_Ok
	movlw	1				; ETOS=1 -> unlock receiver (will reset the slave device!)
	rcall	MBNET_Clone_Me_SendCmd

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x06
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

	;; print message "waiting for reset"
	TABLE_ADDR MBNET_Clone_STR_WAIT_RESET
	call	MIOS_LCD_PrintMessage

	;; store slave ID in TMP1
	SET_BSR	MBNET_BASE
	movf	MBNET_SLAVE_ID, W, BANKED
	movwf	TMP1


MBNET_Clone_Me_Ok_WaitLoop
	;; feed watchdog
	clrwdt
	
	;; request re-sync (node number in TMP1)
	movf	TMP1, W
	rcall	MBNET_RequestSync

	;; call CON handler, wait for response from slave
	rcall	MBNET_ConHandler

	;; restore slave ID
	SET_BSR	MBNET_BASE
	movf	TMP1, W
	movwf	MBNET_SLAVE_ID, BANKED

	;; loop until slave is available again
	;; 	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_AVAIL, W, BANKED
	bz	MBNET_Clone_Me_Ok_WaitLoop

	;; for debugging and remote diagnosis
	movff	MBNET_SLAVE_ID, WREG
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x07
	call	MIOS_MIDI_TxBufferPut
	SET_BSR	MBNET_BASE

MBNET_Clone_Me_Loop_Next
	;; clone all 3 slaves
	SET_BSR	MBNET_BASE
	incf	MBNET_SLAVE_ID, F, BANKED
	movlw	4-1
	cpfsgt	MBNET_SLAVE_ID, BANKED
	rgoto MBNET_Clone_Me_Loop

	;; for debugging and remote diagnosis
	movlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	return


MBNET_Clone_Me_Failed
	movlw	1				; ETOS=1 -> unlock receiver (will reset the slave device!)
	rcall	MBNET_Clone_Me_SendCmd

	;; print message and exit
	TABLE_ADDR MBNET_Clone_STR_FAILED
	call	MIOS_LCD_PrintMessage
	rgoto	MBNET_Clone_Me_Loop_Next

;; ----
MBNET_Clone_Me_SendCmd
	;; expecting ETOS in WREG
	;; slave number already in MBNET_SLAVE_ID
	SET_BSR	MBNET_BASE
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED		; EIDH always 0
	movlw	0				; TOS
	rcall	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	MBNET_Clone_Me_SendCmd_End	; skip if slave not available

	movlw	0				; no byte to send
	movwf	POSTINC1			; DLC

	rcall	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
MBNET_Clone_Me_SendCmd_End
	;; ZERO flag != 0 if transfer failed
	return
