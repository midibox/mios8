; $Id$
;
;  MBNET communication routines for MBSID Control Surface
;
;  See also mbnet.inc and sid_mbnet.inc
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


					;;    <------------------>
TEXT_CS_MENU_MBNET_NOT_COMP  STRING 20, 0x00, "SIDx not compatible "
TEXT_CS_MENU_MBNET_NOT_AVAIL STRING 20, 0x00, "SIDx not available  "
					;;   12341234123412341234

TEXT_CS_MENU_MBNET_NO_RESP   STRING 20, 0x40, "(No MBNet Response) "
TEXT_CS_MENU_MBNET_BUSERR    STRING 20, 0x40, "(CAN Bus Errors)    "
TEXT_CS_MENU_MBNET_DISABLED  STRING 20, 0x40, "(CAN disabled!)     "


;; --------------------------------------------------------------------------
;;  This function is called from CS_MENU_Handler_Init to check, if a CAN node
;;  is available - if not, it disables the SID and prints an error message
;;  on LCD
;;  OUT: clears CS_STAT, CS_STAT_SID_AVAILABLE if SID not available
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Check
	;; note that we also check the availability of the master here - this seems to be perverse, but it is just a debugging help for myself ;-)
	call	CS_MENU_MS_GetSIDNumber
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_AVAIL, W, BANKED
	bnz	CS_MENU_MBNET_Check_NodeOk

	;; notify that SID not available
	bcf	CS_STAT, CS_STAT_SID_AVAILABLE

	;; branch if MBNet has been permanently disabled
	BRA_IFSET MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED, CS_MENU_MBNET_Check_NotAvail

	;; branch if we have determined that the node is not compatible
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_INCOMPATIBLE, W, BANKED
	bz	CS_MENU_MBNET_Check_NotAvail

CS_MENU_MBNET_Check_Incompatible
	;; print message
	TABLE_ADDR TEXT_CS_MENU_MBNET_NOT_COMP
	call	MIOS_LCD_PrintString
	;; print node info at the bottom
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movf	CS_MENU_SID, W
	rcall	CS_MENU_MBNET_PrnInfo
	rgoto	CS_MENU_MBNET_Check_Cont

CS_MENU_MBNET_Check_NotAvail
	;; print message
	TABLE_ADDR TEXT_CS_MENU_MBNET_NOT_AVAIL
	call	MIOS_LCD_PrintString

	;; branch if MBNet has been permanently disabled
	SET_BSR	MBNET_BASE
	BRA_IFSET MBNET_STATE, MBNET_STATE_PERMANENT_OFF, BANKED, CS_MENU_MBNET_Check_Disabled
	rgoto	CS_MENU_MBNET_Check_NotAvailP
CS_MENU_MBNET_Check_Disabled
	TABLE_ADDR TEXT_CS_MENU_MBNET_DISABLED
	rgoto	CS_MENU_MBNET_Check_DontPanic

CS_MENU_MBNET_Check_NotAvailP
	BRA_IFCLR MBNET_STATE, MBNET_STATE_PANIC, BANKED, CS_MENU_MBNET_Check_DontPanic
CS_MENU_MBNET_Check_Panic
	TABLE_ADDR TEXT_CS_MENU_MBNET_BUSERR
CS_MENU_MBNET_Check_DontPanic
	call	MIOS_LCD_PrintString

CS_MENU_MBNET_Check_Cont
	;; print SID number
	movlw	0x00 + 3
	call	MIOS_LCD_CursorSet
	incf	CS_MENU_SID, W
	call	MIOS_LCD_PrintBCD1

	return

CS_MENU_MBNET_Check_NodeOk
	return


;; --------------------------------------------------------------------------
;;  This function is called when a SID button is pressed
;;  It re-enables a CAN node if it has vanished from the network
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Retry
	;; check current avail status
	call	CS_MENU_MS_GetSIDNumber	; now in WREG and CS_MENU_SID
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MBNET_BASE
	andwf	MBNET_NODE_AVAIL, W, BANKED
	bnz	CS_MENU_MBNET_Retry_NodeOk

CS_MENU_MBNET_Retry_NodeVanished
	;; request a re-synchronisation
	movf	CS_MENU_SID, W
	call	MBNET_RequestSync

CS_MENU_MBNET_Retry_NodeOk
	return


;; --------------------------------------------------------------------------
;;  This function is called from MBNET_ConHandler when a new CAN node has
;;  been detected
;;  IN: CAN Node number (1-7) in WREG and MIOS_PARAMETER1
;;      CAN Node ping response in MBNET_NODE_INFO
;; --------------------------------------------------------------------------
CS_MENU_MBNET_FoundNode
	;; transfer node (=slave) number in CS_MENU_SID
	movwf	CS_MENU_SID

	;; request patch update
	andlw	0x03
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	bcf	PLUSW1, 7
	call	CS_MENU_MS_SendPatch_TxReq

	;; send ensemble settings
	movf	CS_MENU_SID, W
	call	SID_ENS_LoadSIDSetup
	call	CS_MENU_MBNET_Tx_SendEns

	;; re-init ensemble buffer based on selected SID
	call	CS_MENU_MS_UpdateEnsBuffer

	;; re-init display
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  Print the information of a given SID node
;;  IN: node number in WREG
;;  OUT: 20 characters: "Pxxx xxxx Vxxx.xxxxx"
;;  USES: FSR0, PRODL
;; --------------------------------------------------------------------------
CS_MENU_MBNET_PrnInfo
	;; calculate pointer to info array
	lfsr	FSR0, MBNET_NODE_INFO_BEGIN
	rlf	WREG, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x38
	addwf	FSR0L, F

	movlw	'P'
	call	MIOS_LCD_PrintChar
	movf	POSTINC0, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintHex2

	movlw	' '
	call	MIOS_LCD_PrintChar
	
	movlw	4
	movwf	PRODL
CS_MENU_MBNET_PrnInfoLoop
	movf	POSTINC0, W
	skpnz
	movlw	'?'
	call	MIOS_LCD_PrintChar
	decfsz	PRODL, F
	rgoto	CS_MENU_MBNET_PrnInfoLoop

	movlw	' '
	call	MIOS_LCD_PrintChar
	
	movlw	'V'
	call	MIOS_LCD_PrintChar
	movf	POSTINC0, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintHex2

	movlw	'.'
	call	MIOS_LCD_PrintChar

	movf	POSTINC0, W
	movff	POSTINC0, MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER3, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex2
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintHex2
	return


;; --------------------------------------------------------------------------
;;  sends a single event to USER_MPROC_NotifyReceivedEvent of a slave
;;  IN: slave target in CS_MENU_SID
;;      MIDI event in MIOS_PARAMETER[123]
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendEvent
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	6				; ETOS=6
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare			; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendEvent_End	; skip if slave not available

	movlw	3				; 3 additional byte to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0
	movff	MIOS_PARAMETER2, POSTINC1	; D1
	movff	MIOS_PARAMETER3, POSTINC1	; D2

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SendEvent_End
	return


;; --------------------------------------------------------------------------
;;  sends a sound parameter
;;  IN: slave target in CS_MENU_SID
;;      parameter address in MIOS_PARAMETER[12]
;;      parameter value in WREG (will be copied to MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendPar
	movwf	MIOS_PARAMETER3
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	18				; ETOS=18
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare			; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendPar_End	; skip if slave not available

	movlw	4				; 4 bytes to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0
	movff	MIOS_PARAMETER2, POSTINC1	; D1
	movff	MIOS_PARAMETER3, POSTINC1	; D3
	clrf	POSTINC1			; D4 (high-byte of parameter, always 0)

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SendPar_End
	return


;; --------------------------------------------------------------------------
;;  sends an ensemble parameter
;;  IN: slave target in CS_MENU_SID
;;      parameter address in MIOS_PARAMETER1
;;      parameter value in WREG (will be copied to MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendEPar
	movwf	MIOS_PARAMETER3
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	17				; ETOS=17
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendEPar_End	; skip if slave not available

	movlw	4				; 4 bytes to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0
	clrf	POSTINC1			; D1 (high-byte of address, always 0)
	movff	MIOS_PARAMETER3, POSTINC1	; D3
	clrf	POSTINC1			; D4 (high-byte of parameter, always 0)

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SendEPar_End
	return


;; --------------------------------------------------------------------------
;;  temporary routine which allows to write into bank 0 if the slave
;;  (currently used to change the MIDI channels)
;;  IN: slave target in CS_MENU_SID
;;      bank 0 address in MIOS_PARAMETER1
;;      value in WREG (will be copied to MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendBank0
	movwf	MIOS_PARAMETER3
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	28				; ETOS=28 (tmp)
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendBank0_End	; skip if slave not available

	movlw	4				; 4 bytes to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0
	clrf	POSTINC1			; D1 (high-byte of address, always 0)
	movff	MIOS_PARAMETER3, POSTINC1	; D3
	clrf	POSTINC1			; D4 (high-byte of parameter, always 0)

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SendBank0_End
	return


;; --------------------------------------------------------------------------
;;  sends a knob value
;;  IN: slave target in CS_MENU_SID
;;      knob number in MIOS_PARAMETER1
;;      knob value in WREG (will be copied to MIOS_PARAMETER3)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendKnob
	movwf	MIOS_PARAMETER3
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	19				; ETOS=19
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare			; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendKnob_End	; skip if slave not available

	movlw	4				; 4 bytes to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0
	clrf	POSTINC1			; D1 (high-byte of address, always 0)
	movff	MIOS_PARAMETER3, POSTINC1	; D3
	clrf	POSTINC1			; D4 (high-byte of parameter, always 0)

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SendKnob_End
	return


;; --------------------------------------------------------------------------
;;  updates a knob value
;;  IN: slave target in CS_MENU_SID
;;      knob number in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_UpdateKnob
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	20				; ETOS=20
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare			; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_UpdateKnob_End	; skip if slave not available

	movlw	1				; 1 byte to send
	movwf	POSTINC1			; DLC
	movff	MIOS_PARAMETER1, POSTINC1	; D0

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_UpdateKnob_End
	return


;; --------------------------------------------------------------------------
;;  Special Command
;;  IN: slave target in CS_MENU_SID
;;      ETOS in WREG
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SpecialCmd
	SET_BSR	MBNET_BASE			; command located in EIDL
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED		; EIDH always 0
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	movlw	0				; TOS
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SpecialCmd_End	; skip if slave not available

	movlw	0				; no byte to send
	movwf	POSTINC1			; DLC

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_SpecialCmd_End
	return

;; --------------------------------------------------------------------------
;;  ETOS=0: lock slave receiver
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_LockSlaveRx
	movlw	0
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=1: unlock slave receiver
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_UnlockSlaveRx
	movlw	1
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=16: init patch
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_InitPatch
	movlw	16
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=23: init ensemble
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_InitEns
	movlw	23
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd


;; --------------------------------------------------------------------------
;;  fills the remote buffer with a patch from a slave --> stepwise <--
;;  fetches 32 byte blocks; immediate address stored in CS_MENU_GET_PATCH_ADDR_[LH]
;;  The CS_MENU_MBNET_Tx_GetPatchChk function has to be called periodically
;;  to check if the patch is complete
;;  IN: slave target in CS_MENU_SID
;;  OUT: ZERO flag set if patch is complete (no block has to be fetched)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_GetPatchChk
	;; corner case: abort if master has been selected meanwhile
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MBNET_Tx_GetPatchChk_Slv
CS_MENU_MBNET_Tx_GetPatchChk_Mst
	;; zero patch address so that no additional block will be requested
	clrf	CS_MENU_GET_PATCH_ADDR_L
	clrf	CS_MENU_GET_PATCH_ADDR_H
	andlw	0x00
	return

CS_MENU_MBNET_Tx_GetPatchChk_Slv
	;; check if the patch is complete (address is zero)
	movf	CS_MENU_GET_PATCH_ADDR_L, W
	iorwf	CS_MENU_GET_PATCH_ADDR_H, W
	bnz	CS_MENU_MBNET_Tx_GetPatch_Cont	; continue fetching

	;; set ZERO flag to notify, that patch is complete
	andlw	0x00
	return

CS_MENU_MBNET_Tx_GetPatch
	clrf	CS_MENU_GET_PATCH_ADDR_L
	clrf	CS_MENU_GET_PATCH_ADDR_H

CS_MENU_MBNET_Tx_GetPatch_Cont
	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_GetPatch_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; we are doing a continuous read, so address only needs to be set at the beginning
	;; optimized Slave Patch Buffer->CAN->Master Edit Buffer transfer
CS_MENU_MBNET_Tx_GetPatch_LoopO
	movff	CS_MENU_GET_PATCH_ADDR_L, MBNET_EID_L	; EIDL contains low-byte of address
	movff	CS_MENU_GET_PATCH_ADDR_H, MBNET_EID_H	; EIDH contains high-byte of address
	movlw	MBNET_TOS_RAM_READ		; TOS for reading RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_GetPatch_End	; skip if slave not available

	movlw	0				; DLC always 0
	movwf	POSTINC1

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; check that we received 8 bytes
	lfsr	FSR1, B5DLC
	movf	POSTINC1, W
	xorlw	8
	skpz
	rgoto	CS_MENU_MBNET_Error

	;; copy bytes into edit buffer
	movff	CS_MENU_GET_PATCH_ADDR_L, FSR0L
	movf	CS_MENU_GET_PATCH_ADDR_H, W
	andlw	0x01
	addlw	HIGH(SID_EDIT_BUFFER)
	movwf	FSR0H

	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0

	movlw	8
	addwf	CS_MENU_GET_PATCH_ADDR_L, F
	skpnz
	incf	CS_MENU_GET_PATCH_ADDR_H, F

	;; loop until a 32 byte block has been received
	movf	CS_MENU_GET_PATCH_ADDR_L, W
	andlw	0x1f
	bnz	CS_MENU_MBNET_Tx_GetPatch_LoopO

	;; check if last block has been received
	movf	CS_MENU_GET_PATCH_ADDR_L, W
	bnz	CS_MENU_MBNET_Tx_GetPatch_Yield
	movf	CS_MENU_GET_PATCH_ADDR_H, W
	andlw	0xfe
	bz	CS_MENU_MBNET_Tx_GetPatch_Yield

CS_MENU_MBNET_Tx_GetPatch_Abort
	;; zero patch address so that no additional block will be requested
	clrf	CS_MENU_GET_PATCH_ADDR_L
	clrf	CS_MENU_GET_PATCH_ADDR_H

CS_MENU_MBNET_Tx_GetPatch_Yield
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler

CS_MENU_MBNET_Tx_GetPatch_End
	;; clear ZERO flag to notify, that patch is not complete
	iorlw	0xff
	return


;; --------------------------------------------------------------------------
;;  fills the remote buffer for ensembles with data from slave
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_GetEns
	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_GetEns_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; using EEADR as loop counter
	clrf	EEADR
	;; optimized BankStick->CAN->Slave Patch Buffer transfer
CS_MENU_MBNET_Tx_GetEns_LoopO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movlw	0xff				; EIDH contains high-byte of address, 0xff for ensembles!
	movff	WREG, MBNET_EID_H
	movlw	MBNET_TOS_RAM_READ		; TOS for reading RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_GetEns_End	; skip if slave not available

	movlw	0				; DLC always 0
	movwf	POSTINC1

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; check that we received 8 bytes
	lfsr	FSR1, B5DLC
	movf	POSTINC1, W
	xorlw	8
	skpz
	rgoto	CS_MENU_MBNET_Error

	;; copy bytes into ensemble edit buffer
	lfsr	FSR0, SID_ENS_BUFFER
	movf	EEADR, W
	addwf	FSR0L, F

	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0
	movff	POSTINC1, POSTINC0

	movlw	8
	addwf	EEADR, F

	;; loop until 64 byte have been sent (2 iterations)
	BRA_IFCLR EEADR, 6, ACCESS, CS_MENU_MBNET_Tx_GetEns_LoopO

CS_MENU_MBNET_Tx_GetEns_Abort
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler

CS_MENU_MBNET_Tx_GetEns_End
	return


;; --------------------------------------------------------------------------
;;  ETOS=21: update shadow buffer
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_UpdateShadow
	movlw	21
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=22: disable SID engine
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_DisableEngine
	movlw	22
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd


;; --------------------------------------------------------------------------
;;  ETOS=24: All Notes On command
;;  IN: slave target in CS_MENU_SID
;;      current instrument in CS_MENU_SELECTED_INS
;;      current L/R selection in CS_MENU_SELECTED_SID_LR
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_NotesOn
	movff	CS_MENU_SID, MBNET_SLAVE_ID	; prepare transmission
	SET_BSR	MBNET_BASE			; EID[LH] always 0
	movlw	24				; ETOS=24
	movwf	MBNET_EID_L, BANKED
	clrf	MBNET_EID_H, BANKED
	movlw	0				; TOS
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_NotesOn_End	; skip if slave not available

	movlw	2				; 2 bytes to send
	movwf	POSTINC1			; DLC
	movff	CS_MENU_SELECTED_INS, POSTINC1	; D0
	movff	CS_MENU_SELECTED_SID_LR, POSTINC1 ; D1

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

CS_MENU_MBNET_Tx_NotesOn_End
	return

;; --------------------------------------------------------------------------
;;  ETOS=25: All Notes Off command
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_NotesOff
	movlw	25
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=26: Voice ReInit command
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_VoiceReInit
	movlw	26
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  ETOS=27: Superpoly ReInit command
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SP_ReInit
	movlw	27
	rgoto	CS_MENU_MBNET_Tx_SpecialCmd

;; --------------------------------------------------------------------------
;;  sends a patch to a given slave
;;  IN: slave target in CS_MENU_SID
;;      BankStick patch and bank in SID_PATCH/SID_BANK
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendPatch
	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_SendPatch_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; branch to special sending routine if patch not valid, or Patch #0 selected
	call	SID_PBANK_CheckPatchValid
	bnz	CS_MENU_MBNET_Tx_SendPatch_Int
	movf	SID_PATCH, W
	bz	CS_MENU_MBNET_Tx_SendPatch_Int

CS_MENU_MBNET_Tx_SendPatch_Ext
	;; select BankStick address
	;; we are doing a continuous read, so address only needs to be set at the beginning
	clrf	EEADR
	clrf	EEADRH
	call	SID_PBANK_SetBankStickAddress

	call	MIOS_IIC_Start			; start IIC

	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa0				; set address
	call	MIOS_IIC_ByteSend
	movf	MIOS_PARAMETER2, W		; send high address
	call	MIOS_IIC_ByteSend
	movf	MIOS_PARAMETER1, W		; send low address
	call	MIOS_IIC_ByteSend

	call	MIOS_IIC_Start			; start IIC again
	rlf	MIOS_BOX_CFG1, W
	andlw	0x0e
	iorlw	0xa1
	call	MIOS_IIC_ByteSend

	;; optimized BankStick->CAN->Slave Patch Buffer transfer
	lfsr	FSR0, SID_EDIT_BUFFER		; written if slave is selected on control surface
CS_MENU_MBNET_Tx_SendPatchLoopO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movff	EEADRH, MBNET_EID_H		; EIDH contains high-byte of address
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendPatch_End	; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from BankStick and write them into data buffer
CS_MENU_MBNET_Tx_SendPatchLoopI
	;; read byte and copy into transmit buffer
 	call	MIOS_IIC_ByteReceive
	movwf	POSTINC1
	movwf	PRODL

	;; copy into edit buffer if SID is selected
	movf	CS_MENU_EDIT_BUFFER_SID, W
	xorwf	CS_MENU_SID, W
	skpnz
	movff	PRODL, POSTINC0

	;; increment address
 	incf	EEADR, F
	skpnz
	incf	EEADRH, F

	;; send acknowledge
	;; on the last address, a Nak *must* be sent, otherwise the next EEPROM write will fail
	movf	EEADR, W
	bnz	CS_MENU_MBNET_Tx_SendPatchLoopIL
	movf	EEADRH, W
	andlw	0xfe
	bz	CS_MENU_MBNET_Tx_SendPatchLoopIL
	call	MIOS_IIC_NakSend
	rgoto	CS_MENU_MBNET_Tx_SendPatchLoopIN
CS_MENU_MBNET_Tx_SendPatchLoopIL
	call	MIOS_IIC_AckSend
	;; 	rgoto	CS_MENU_MBNET_Tx_SendPatchLoopIN
CS_MENU_MBNET_Tx_SendPatchLoopIN
	movf	EEADR, W
	andlw	0x07
	bnz	CS_MENU_MBNET_Tx_SendPatchLoopI

	;; optimized "interlaced" transfer - send package while fetching next part from BankStick
	movf	EEADRH, W
	bnz	CS_MENU_MBNET_Tx_SendPatchLoopI1
	movf	EEADR, W
	xorlw	0x08
	bz	CS_MENU_MBNET_Tx_SendPatchLoopI0
CS_MENU_MBNET_Tx_SendPatchLoopI1
	call	MBNET_Tx_Perform_Finish		; waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error
CS_MENU_MBNET_Tx_SendPatchLoopI0
	call	MBNET_Tx_Perform_Start		; performs the transfer

	;; loop until last byte has been sent
	movf	EEADR, W
	bnz	CS_MENU_MBNET_Tx_SendPatchLoopO
	movf	EEADRH, W
	andlw	0xfe
	bz	CS_MENU_MBNET_Tx_SendPatchLoopO

	;; wait for the last block
	call	MBNET_Tx_Perform_Finish		; waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	call	MIOS_IIC_Stop			; stop IIC

	call	CS_MENU_MBNET_Tx_InitPatch	; init patch

CS_MENU_MBNET_Tx_SendPatch_Abort
CS_MENU_MBNET_Tx_SendPatch_Int_C
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler

CS_MENU_MBNET_Tx_SendPatch_End
	return



	;; special sending routine for internal patch
CS_MENU_MBNET_Tx_SendPatch_Int
	clrf	EEADR
	clrf	EEADRH

	;; optimized EEPROM->CAN->Slave Patch Buffer transfer
	lfsr	FSR0, SID_EDIT_BUFFER		; written if slave is selected on control surface
CS_MENU_MBNET_Tx_SendPatch_IntLO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movff	EEADRH, MBNET_EID_H		; EIDH contains high-byte of address
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM
	call	MBNET_Tx_Prepare			; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendPatch_End	; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from EEPROM and write them into data buffer
CS_MENU_MBNET_Tx_SendPatch_IntLI
	;; read byte and copy into transmit buffer
	call	MIOS_EEPROM_Read
	movwf	POSTINC1
	movwf	PRODL

	;; increment EEADRH if EEADR is zero (not done in MIOS_EEPROM_Read due to compatibility reasons)
	movf	EEADR, W
	skpnz
	incf	EEADRH, F

	;; copy into edit buffer if SID is selected
	movf	CS_MENU_EDIT_BUFFER_SID, W
	xorwf	CS_MENU_SID, W
	skpnz
	movff	PRODL, POSTINC0

	movf	EEADR, W
	andlw	0x07
	bnz	CS_MENU_MBNET_Tx_SendPatch_IntLI

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; loop until last byte has been sent
	movf	EEADR, W
	bnz	CS_MENU_MBNET_Tx_SendPatch_IntLO
	movf	EEADRH, W
	andlw	0xfe
	bz	CS_MENU_MBNET_Tx_SendPatch_IntLO

	call	CS_MENU_MBNET_Tx_InitPatch	; init patch

	;; continue above
	rgoto	CS_MENU_MBNET_Tx_SendPatch_Int_C


;; --------------------------------------------------------------------------
;;  sends the patch buffer to a given slave
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendPBuffer
	lfsr	FSR0, SID_PATCH_BUFFER		; source location
	rgoto	CS_MENU_MBNET_Tx_SendPBufferCont


;; --------------------------------------------------------------------------
;;  sends the edit buffer to a given slave
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendEdit
	lfsr	FSR0, SID_EDIT_BUFFER		; source location
CS_MENU_MBNET_Tx_SendPBufferCont

	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_SendEdit_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; optimized Edit buffer->CAN->Slave Patch Buffer transfer
	clrf	EEADR				; used as loop counter
	clrf	EEADRH
CS_MENU_MBNET_Tx_SendEdit_IntLO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movff	EEADRH, MBNET_EID_H		; EIDH contains high-byte of address
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendEdit_End	; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from RAM buffer and write them into data buffer
CS_MENU_MBNET_Tx_SendEdit_IntLI
	;; read byte and copy into transmit buffer
	movff	POSTINC0, POSTINC1
	incf	EEADR, F
	skpnz
	incf	EEADRH, F

	movf	EEADR, W
	andlw	0x07
	bnz	CS_MENU_MBNET_Tx_SendEdit_IntLI

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; loop until last byte has been sent
	movf	EEADR, W
	bnz	CS_MENU_MBNET_Tx_SendEdit_IntLO
	movf	EEADRH, W
	andlw	0xfe
	bz	CS_MENU_MBNET_Tx_SendEdit_IntLO

	call	CS_MENU_MBNET_Tx_InitPatch	; init patch

CS_MENU_MBNET_Tx_SendEdit_Abort
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler
CS_MENU_MBNET_Tx_SendEdit_End
	return


;; --------------------------------------------------------------------------
;;  sends only the name from edit buffer to a given slave
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendName
	lfsr	FSR0, SID_EDIT_BUFFER		; source location

	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_SendName_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; optimized Edit buffer->CAN->Slave Patch Buffer transfer
	clrf	EEADR				; used as loop counter
	clrf	EEADRH
CS_MENU_MBNET_Tx_SendName_IntLO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movff	EEADRH, MBNET_EID_H		; EIDH contains high-byte of address
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendName_End	; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from RAM buffer and write them into data buffer
CS_MENU_MBNET_Tx_SendName_IntLI
	;; read byte and copy into transmit buffer
	movff	POSTINC0, POSTINC1
	incf	EEADR, F
	skpnz
	incf	EEADRH, F

	movf	EEADR, W
	andlw	0x07
	bnz	CS_MENU_MBNET_Tx_SendName_IntLI

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; loop two times
	movf	EEADR, W
	xorlw	0x10
	bnz	CS_MENU_MBNET_Tx_SendName_IntLO

CS_MENU_MBNET_Tx_SendName_Abort
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler
CS_MENU_MBNET_Tx_SendName_End
	return


;; --------------------------------------------------------------------------
;;  sends the ensemble buffer to a given slave
;;  IN: slave target in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Tx_SendEns
	;; lock receive handler
	call	CS_MENU_MBNET_Tx_LockSlaveRx
	bnz	CS_MENU_MBNET_Tx_SendEns_Abort	; abort if slave not available (branch to UnlockRx to give slave a chance to recover)

	;; optimized Edit buffer->CAN->Slave Ensemble Buffer transfer
	lfsr	FSR0, SID_ENS_BUFFER		; source location
	clrf	EEADR				; used as loop counter
CS_MENU_MBNET_Tx_SendEns_IntLO
	movff	EEADR, MBNET_EID_L		; EIDL contains low-byte of address
	movlw	0xff				; EIDH contains high-byte of address, 0xff for ensembles!
	movff	WREG, MBNET_EID_H
	movlw	MBNET_TOS_RAM_WRITE		; TOS for writing into RAM
	call	MBNET_Tx_Prepare		; returns pointer to DLC of Tx buffer in FSR1
	bnz	CS_MENU_MBNET_Tx_SendEns_End	; skip if slave not available

	movlw	8				; DLC always 8
	movwf	POSTINC1

	;; read 8 bytes from EEPROM and write them into data buffer
CS_MENU_MBNET_Tx_SendEns_IntLI
	;; read byte and copy into transmit buffer
	movff	POSTINC0, POSTINC1
	incf	EEADR, F

	movf	EEADR, W
	andlw	0x07
	bnz	CS_MENU_MBNET_Tx_SendEns_IntLI

	call	MBNET_Tx_Perform		; performs the transfer and waits for an acknowledge from slave
	skpz					; re-init display if transfer failed
	rgoto	CS_MENU_MBNET_Error

	;; loop until last byte has been sent
	movf	EEADR, W
	andlw	0x3f
 	bnz	CS_MENU_MBNET_Tx_SendEns_IntLO

 	call	CS_MENU_MBNET_Tx_InitEns	; init ensemble

CS_MENU_MBNET_Tx_SendEns_Abort
	call	CS_MENU_MBNET_Tx_UnlockSlaveRx	; unlock receive handler
CS_MENU_MBNET_Tx_SendEns_End
	return


;; --------------------------------------------------------------------------
;;  An error has happened during a CAN transfer: re-init display
;;  (MBNET_AVAIL flag will be cleared by driver in mbnet.inc)
;; --------------------------------------------------------------------------
CS_MENU_MBNET_Error
	;; re-init display
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; *must* return value != 0 (ZERO flag not set!)
	iorlw	0xff
	return
