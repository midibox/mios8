; $Id$
;
;; MBNet Clone Code -- must be located at the begin of the user flash range
;; reserved region: 0x3300-0x3500
;
; NEW: common MBNet routines are running with ECAN mode #2 (FIFO) now, but
; this variant is coded for ECAN Mode #1
;
; In order to ensure that users are still able to update their Slave SIDs, this
; code hasn't been changed (because it won't be updated during cloning).
; Instead we switch to ECAN Mode #1 *before* MBNET_CLONE is called to achieve
; the same preconditions as before.
;
; ==========================================================================
;
;  Copyright 2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	org	0x03300

;; --------------------------------------------------------------------------
;;  FUNCTION: MBNET_CLONE
;;  DESCRIPTION: called from MBNET_RxHandler when ETOS=15 command has been
;;  received
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
MBNET_CLONE
	SET_BSR	MBNET_BASE

	;; break if this is a master node
	movf	MBNET_MY_ID, W, BANKED
	andlw	0x0f
	skpnz
	return

	;; disable all user tasks (to prevent that code will be executed from user flash area)
	call	MIOS_UserSuspend

	;; send acknowledge to master -- use a goto here, the AckOk function will branch back to MBNET_CLONE_RxHandler
	rgoto	MBNET_CLONE_AckOk

	;; start special receive loop
MBNET_CLONE_RxHandler
	;; feed watchdog
	clrwdt
	
	;; on bus error - give up and reset device (shit happened!)
	rcall	MBNET_CLONE_BusErrorCheck
	bnz	MBNET_CLONE_Reset_Error

	;; check if something has been received
	SET_BSR	MBNET_BASE
	lfsr	FSR1, RXB0CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, RXB1CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, B0CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, B1CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, B2CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, B3CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	lfsr	FSR1, B4CON
	BRA_IFSET INDF1, RXFUL, ACCESS, MBNET_CLONE_RxHandler_Go
	;; B5CON used as dedicated acknowledge buffer

	;; loop until slave will be reset by master
	rgoto	MBNET_CLONE_RxHandler

MBNET_CLONE_RxHandler_Go
	;; store pointer to receive buffer
	movff	FSR1L, MBNET_RX_BUFFER_PTR_L
	movff	FSR1H, MBNET_RX_BUFFER_PTR_H

	;; always copy number of received bytes to loop counter
	movlw	RXB0DLC & 0xf
	movf	PLUSW1, W
	andlw	0x0f
	SET_BSR	MBNET_BASE
	movwf	MBNET_LOOP_CTR, BANKED

	;; extract master ID and copy it to MBNET_MASTER_ID
	movlw	RXB0SIDL & 0xf		; located in EIDH[17:16]
	rrf	PLUSW1, W
	andlw	0x70
	movwf	MBNET_MASTER_ID, BANKED

	;; if receiver has been locked by another master: send retry acknowledge
	swapf	MBNET_MASTER_ID, W, BANKED
	xorwf	MBNET_STATE, W, BANKED
	andlw	0x07
	bnz	MBNET_CLONE_AckRetry

	;; check TOS (type of service) --- special instruction set in clone mode!
	movlw	RXB0SIDL & 0xf		; located in EIDH[17:16]
	movf	PLUSW1, W
	andlw	0x03
	bz	MBNET_CLONE_Special	; TOS=0: Special Service
	addlw	-2			; skip TOS=1 (no read)
	bz	MBNET_CLONE_RAMWr	; TOS=2: writes into RAM
	rgoto	MBNET_CLONE_AckError	; no TOS=3 (no ping in clone mode)

	;; all of these functions branch back to the acknowledge response

;; ---
MBNET_CLONE_AckOk
	movlw	MBNET_ACK_TOS_OK
	rgoto	MBNET_CLONE_Ack
MBNET_CLONE_AckRetry
	movlw	MBNET_ACK_TOS_RETRY
	rgoto	MBNET_CLONE_Ack
MBNET_CLONE_AckError
	movlw	MBNET_ACK_TOS_ERROR
	rgoto	MBNET_CLONE_Ack
MBNET_CLONE_NoAck
	movlw	-1
	;; 	rgoto	MBNET_CLONE_Ack

MBNET_CLONE_Ack
	;; expecting ACK_TOS in WREG
	;; if -1, no acknowledge will be sent (already done before - RAM read case)

	;; release buffer
	movff	MBNET_RX_BUFFER_PTR_L, FSR1L
	movff	MBNET_RX_BUFFER_PTR_H, FSR1H
	bcf	INDF1, RXFUL

	;; send acknowledge, TOS in WREG
	addlw	1
	bz	MBNET_CLONE_AckSkip
	addlw	-1
	call	MBNET_CLONE_SendAck

MBNET_CLONE_AckSkip
	rgoto	MBNET_CLONE_RxHandler


	;; reset on error or success
MBNET_CLONE_Reset_Error
MBNET_CLONE_Reset_Ok
	goto	MIOS_Reset


;; TOS Handlers

MBNET_CLONE_Special
	;; ignore number of received byte (ignored for future compatibility, service is coded in EID)

	;; branch depending on EIDL
	movlw	RXB0EIDL & 0xf
	movf	PLUSW1, W
	addlw	-1
	bz	MBNET_CLONE_Special_ULR	; ETOS=1 -> unlock receiver
	rgoto	MBNET_CLONE_AckError		; unsupported ETOS


	;; unlock receiver -- here we return an acknowledge and reset the device
MBNET_CLONE_Special_ULR
	movlw	MBNET_ACK_TOS_OK
	call	MBNET_CLONE_SendAck

	;; wait until transmit buffer #1 free
MBNET_CLONE_Special_ULR_WaitLoop
	;; check for error
	rcall	MBNET_CLONE_BusErrorCheck
	bnz	MBNET_CLONE_Reset_Error
	SET_BSR	TXB1CON
	BRA_IFSET TXB1CON, TXREQ, BANKED, MBNET_CLONE_Special_ULR_WaitLoop

	rgoto	MBNET_CLONE_Reset_Ok


;;  IN: pointer to receive buffer in FSR1
;;      number of received bytes in MBNET_LOOP_CTR
;;  OUT: must return the acknowledge TOS (MBNET_ACK_TOS_*)
MBNET_CLONE_RAMWr
	;; only accept loop ctr == 8
	movlw	8
	cpfseq	MBNET_LOOP_CTR, BANKED
	rgoto MBNET_CLONE_RAMWr_Error

	;; determine target address -> TBLPTR[LHU]
	movlw	RXB0EIDL & 0xf
	movff	PLUSW1, TBLPTRL
	movlw	RXB0EIDH & 0xf
	movff	PLUSW1, TBLPTRH
	clrf	TBLPTRU
	movlw	3	; multiply by 8
MBNET_CLONE_RAMWr_Mul8Loop
	clrc
	rlf	TBLPTRL, F
	rlf	TBLPTRH, F
	rlf	TBLPTRU, F
	addlw	-1
	bnz	MBNET_CLONE_RAMWr_Mul8Loop

	;; fill receive buffer
	movlw	RXB0D0 & 0xf
	addwf	FSR1L, F
	lfsr	FSR0, MBNET_CLONE_BUFFER
	movf	TBLPTRL, W	; add buffer offset
	andlw	0x38
	addwf	FSR0L, F
	movlw	8
MBNET_CLONE_RAMWr_CopyLoop
	movff	POSTINC1, POSTINC0
	addlw	-1
	bnz	MBNET_CLONE_RAMWr_CopyLoop

	;; once 0x40 window has been filled, program flash/EEPROM
	movf	TBLPTRL, W
	andlw	0x3f
	xorlw	0x38
	bnz	MBNET_CLONE_RAMWr_Ok

	;; ignore range 0x3300..0x34ff (location of mbnet_clone.inc)
	movf	TBLPTRU, W
	bnz	MBNET_CLONE_RAMWr_Program
	movf	TBLPTRH, W
	xorlw	0x33
	bz	MBNET_CLONE_RAMWr_Ok
	xorlw	0x33 ^ 0x34
	bz	MBNET_CLONE_RAMWr_Ok

MBNET_CLONE_RAMWr_Program
	;; program flash
	movlw	0xc0				; ensure that TBLPTR[LHU] is aligned to 64 byte
	andwf	TBLPTRL, F
	lfsr	FSR1, MBNET_CLONE_BUFFER	; receiver buffer in FSR1
	call	MIOS_FLASH_Write		; call MIOS routine
	xorlw	0x01
	bz	MBNET_CLONE_RAMWr_Error		; branch to error if return value == 1
	;; ignore 0x02 error (memory access error), so that a PIC18F4685 can program a PIC18F4682

MBNET_CLONE_RAMWr_Ok
	rgoto	MBNET_CLONE_AckOk

MBNET_CLONE_RAMWr_Error
	rgoto	MBNET_CLONE_AckError

	

;; --------------------------------------------------------------------------
MBNET_CLONE_SendAck
	SET_BSR	MBNET_BASE	; temporary store TOS in MBNET_TOS
	movwf	MBNET_TOS, BANKED

	;; wait until transmit buffer #1 free
MBNET_CLONE_SendAckLoop
	;; check for error
	rcall	MBNET_CLONE_BusErrorCheck
	bnz	MBNET_CLONE_SendAck_Fail
	SET_BSR	TXB1CON
	BRA_IFSET TXB1CON, TXREQ, BANKED, MBNET_CLONE_SendAckLoop

	;; prepare ID field for acknowledge message
	lfsr	FSR1, TXB1SIDH
	SET_BSR	MBNET_BASE
	movf	MBNET_TOS, W, BANKED
	rcall	MBNET_CLONE_SetAckID

	;; 0 byte
	SET_BSR	TXB1DLC
	clrf	TXB1DLC, BANKED

	;; start transfer
	bsf	TXB1CON, TXREQ, BANKED

	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return

MBNET_CLONE_SendAck_Fail
	;; nothing else to do - the master will disable the slave,
	;; connection has to be established again via "ping"
	SET_BSR	MBNET_BASE	; set BSR to MBNET_BASE (easier handling)
	return


;; --------------------------------------------------------------------------
;; pointer to SIDH register in FSR1
;; EID[17:16] (TOS) in WREG
;; SID[10] will always be set to 1 (ACK)
;; SID[9:3] (MIOS Device ID of Master) in MBNET_MASTER_ID
;; SID[2:0] will always be set to 0
;; EID[15:8] will always be set to 0
;; EID[7:0] (MIOS Device ID of Slave) in MBNET_SLAVE_ID
MBNET_CLONE_SetAckID
	movff	MBNET_MASTER_ID, INDF1	; MBNET_MASTER_ID -> SID[9:3]
	bsf	POSTINC1, 7		; SID[10] = 1 (Ack)

	andlw	0x03			; TOS[1:0] -> EID[17:16]
	iorlw	0x08			; always extended ID
	movwf	POSTINC1

	;; EIDH always 0
	clrf	POSTINC1

	;; EIDL contains slave ID (for comparison at master side)
	movff	MBNET_SLAVE_ID, POSTINC1

	return

;; --------------------------------------------------------------------------
;; used during transmit or receive polling to determine if a bus error has occured
;; (e.g. receiver passive or no nodes connected to bus)
;; In this case, all pending transmissions will be aborted
;; No recovery possible in clone mode (since ECAN initialisation code has been overwritten?)!!!
;; ZERO flag set when no error
MBNET_CLONE_BusErrorCheck
	;; skip if bus permanently off
	SET_BSR	MBNET_BASE
	BRA_IFSET MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED, MBNET_CLONE_BusErrorCheck_Panic

	movf	COMSTAT, W
	andlw	(1 << TXBO) | (1 << TXBP) | (1 << RXBP)
	bz	MBNET_CLONE_BusErrorCheck_NoPanc

	;; abort all pending transmissions
	bsf	CANCON, ABAT

	;; change to disable mode
	movf	CANCON, W
	andlw	0x1f
	iorlw	0x20
	movwf	CANCON

MBNET_CLONE_BusErrorCheck_Panic
	iorlw	0xff		; notify error
	return

MBNET_CLONE_BusErrorCheck_NoPanc
	andlw	0x00		; no error
	return


;; --------------------------------------------------------------------------
;; --------------------------------------------------------------------------

	;; normal user code starts from 0x3500
	org	0x03500
