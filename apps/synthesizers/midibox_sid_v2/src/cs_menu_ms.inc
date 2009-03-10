; $Id$
;
; SID Control Surface Master/Slave SID Functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Determines the current SID number depending on the selected SIDs
;;  Result in WREG and CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_GetSIDNumber
	;; set CS_MENU_SID depending on first selected SID
	movlw	0x03
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 2
	movlw	0x02
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 1
	movlw	0x01
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 0
	movlw	0x00
	movwf	CS_MENU_SID
	andlw	0xff		; (fix ZERO flag)
	return


;; --------------------------------------------------------------------------
;;  This function updates the SID_EDIT_BUFFER if it is owned by the master
;;  (used after SysEx patch writes)
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateEditBufferMS
	movf	CS_MENU_EDIT_BUFFER_SID, W
	bz	CS_MENU_MS_UpdateEditBufferNow
	return

;; --------------------------------------------------------------------------
;;  This function updates the SID_EDIT_BUFFER if the selected SID has
;;  changed - it is called whenever SID1/SID2/SID3/SID4 button is pressed
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateEditBuffer
	call	CS_MENU_MS_GetSIDNumber
	xorwf	CS_MENU_EDIT_BUFFER_SID, W
	skpnz
	return

	;; store new SID number
	movff	CS_MENU_SID, CS_MENU_EDIT_BUFFER_SID

CS_MENU_MS_UpdateEditBufferNow
	movff	CS_MENU_EDIT_BUFFER_SID, CS_MENU_SID

	;; transfer patch into buffer
	;; master: copy SID_PATCH_BUFFER to SID_EDIT_BUFFER
	;; slave: request and transfer patch content via CAN
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_UpdateEditBuffer_S
CS_MENU_MS_UpdateEditBuffer_M
	;; get base address of edit and patch buffer
	lfsr	FSR0, SID_EDIT_BUFFER
	lfsr	FSR1, SID_PATCH_BUFFER
	;; copy 0x200 bytes (prepared for MBSID V2)
	clrf	PRODL
	clrf	PRODH
CS_MENU_MS_UpdateEditBuffer_ML
	movff	POSTINC1, POSTINC0
	incf	PRODL, F
	skpnz
	incf	PRODH, F
	BRA_IFCLR PRODH, 1, ACCESS, CS_MENU_MS_UpdateEditBuffer_ML

	;; transfer local ensemble setup into buffer
	call	SID_ENS_LocalSetupIntoBuffer

	rgoto	CS_MENU_MS_UpdateEditBufferEnd

CS_MENU_MS_UpdateEditBuffer_S
  	call	CS_MENU_MBNET_Tx_GetPatch
 	call	CS_MENU_MBNET_Tx_GetEns

CS_MENU_MS_UpdateEditBufferEnd
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; if in main page: force complete re-initialization
	btfsc	CS_MENU, 7
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


;; --------------------------------------------------------------------------
;;  This function updates the SID_ENS_BUFFER depending on CS_MENU_EDIT_BUFFER_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateEnsBuffer
	movf	CS_MENU_EDIT_BUFFER_SID, W
	movwf	CS_MENU_SID
	bnz	CS_MENU_MS_UpdateEnsBuffer_S

CS_MENU_MS_UpdateEnsBuffer_M
	;; transfer local ensemble setup into buffer
	goto	SID_ENS_LocalSetupIntoBuffer

CS_MENU_MS_UpdateEnsBuffer_S
	goto	CS_MENU_MBNET_Tx_GetEns


;; --------------------------------------------------------------------------
;;  A parameter should be sent
;;  value in INDF0, pointer to parameter is FSR0-SID_EDIT_BUFFER
;; --------------------------------------------------------------------------
CS_MENU_MS_SendParameter
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_SendParameter_Slave

CS_MENU_MS_SendParameter_Master

	;; calc pointer to patch buffer
	movff	FSR0L, FSR1L
	movf	FSR0H, W
	addlw	HIGH(SID_PATCH_BUFFER)-HIGH(SID_EDIT_BUFFER)
	movwf	FSR1H

	;; copy value to patch buffer
	movff	INDF0, INDF1

	;; copy to shadowed patch buffer
	movlw	HIGH(SID_PATCH_BUFFER_SHADOW)-HIGH(SID_PATCH_BUFFER)
	addwf	FSR1H, F
	movff	INDF0, INDF1
CS_MENU_MS_SendParameter_MasterE
	return
	

CS_MENU_MS_SendParameter_Slave
	;; send parameter to slave via CAN
	movff	FSR0L, MIOS_PARAMETER1	; low byte
	movf	FSR0H, W		; high byte
	addlw	0-HIGH(SID_EDIT_BUFFER)
	movwf	MIOS_PARAMETER2
	movf	INDF0, W		; value in WREG
	call	CS_MENU_MBNET_Tx_SendPar	; send parameter to slave
	return


;; --------------------------------------------------------------------------
;;  An ensemble parameter should be sent
;;  IN: value in INDF0
;;      SID in CS_MENU_SID
;;      pointer to edit parameter (SID_ENS_BUFFER + SID_ENSx_*) in FSR0
;; --------------------------------------------------------------------------
CS_MENU_MS_SendEnsParameter
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_SendEnsParameter_S

CS_MENU_MS_SendEnsParameter_M
	;; just update all parameters (it's fast enough)
	goto	SID_ENS_Init_Local	

CS_MENU_MS_SendEnsParameter_S
	;; send parameter to slave via CAN
	movlw	0-LOW(SID_ENS_BUFFER)
	addwf	FSR0L, W
	movwf	MIOS_PARAMETER1		; address
	movf	INDF0, W		; value in WREG
	goto	CS_MENU_MBNET_Tx_SendEPar ; send parameter to slave


;; --------------------------------------------------------------------------
;;  An ensemble parameter should be sent to all SIDs
;;  IN: value in INDF0
;;      CS_MENU_SID *must* be 0 (master selected)
;;      pointer to edit parameter (SID_ENS_BUFFER + SID_ENSx_*) in FSR0
;; --------------------------------------------------------------------------
CS_MENU_MS_SendEnsParameterToAll
	;; only allowed for master (reason: we change this register, and have to restore it at the end)
	movf	CS_MENU_SID, W
	skpz
	return

	call	SID_ENS_Init_Local		; SID1
	incf	CS_MENU_SID, F
	rcall	CS_MENU_MS_SendEnsParameter_S	; SID2
	incf	CS_MENU_SID, F
	rcall	CS_MENU_MS_SendEnsParameter_S	; SID3
	incf	CS_MENU_SID, F
	rcall	CS_MENU_MS_SendEnsParameter_S	; SID4

	;; restore CS_MENU_SID
	clrf	CS_MENU_SID
	return


;; --------------------------------------------------------------------------
;;  A knob value should be sent
;;  IN: value in INDF0
;;      knob number in CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_MS_SendKnob
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_SendKnob_Slave

CS_MENU_MS_SendKnob_Master
	;; forward to knob handler
	movff	INDF0, MIOS_PARAMETER1
	movf	CS_MENU_PARAMETER_IX, W
	call	SID_KNOB_SetValue
	;; continue with CC sending routine
	rgoto	CS_MENU_MS_SendKnob_CC

CS_MENU_MS_SendKnob_Slave
	;; send knob value to slave via CAN
	movff	CS_MENU_PARAMETER_IX, MIOS_PARAMETER1	; knob number
	movf	INDF0, W	; value in WREG
	call	CS_MENU_MBNET_Tx_SendKnob
	;; 	rgoto	CS_MENU_MS_SendKnob_CC


CS_MENU_MS_SendKnob_CC
	;; skip if CC mode not activated
	btfss	CS_MENU_MODE, CS_MENU_MODE_CC
	return

	;; select MIDI channel -> FSR1
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	addwf	FSR1L, F

	;; branch depending on knob number
	movf	CS_MENU_PARAMETER_IX, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_MS_SendKnob_CC_1
	rgoto	CS_MENU_MS_SendKnob_CC_2
	rgoto	CS_MENU_MS_SendKnob_CC_3
	rgoto	CS_MENU_MS_SendKnob_CC_4
	rgoto	CS_MENU_MS_SendKnob_CC_5
	rgoto	CS_MENU_MS_SendKnob_CC_V
	rgoto	CS_MENU_MS_SendKnob_CC_P
	rgoto	CS_MENU_MS_SendKnob_CC_A

CS_MENU_MS_SendKnob_CC_1
	movlw	0x01		; send ModWheel
CS_MENU_MS_SendKnob_CC_Cont
	movwf	PRODL		; store CC number in PRODL

	movf	INDF1, W	; status byte (pointer to channel in FSR1)
	andlw	0x0f
	iorlw	0xb0
	call	MIOS_MIDI_TxBufferPut

	movf	PRODL, W	; CC number
	call	MIOS_MIDI_TxBufferPut

	rrf	INDF0, W	; 7bit value
	andlw	0x7f
	goto	MIOS_MIDI_TxBufferPut
	

CS_MENU_MS_SendKnob_CC_2
	movlw	0x10		; send CC#16
	rgoto	CS_MENU_MS_SendKnob_CC_Cont
CS_MENU_MS_SendKnob_CC_3
	movlw	0x11		; send CC#17
	rgoto	CS_MENU_MS_SendKnob_CC_Cont
CS_MENU_MS_SendKnob_CC_4
	movlw	0x12		; send CC#18
	rgoto	CS_MENU_MS_SendKnob_CC_Cont
CS_MENU_MS_SendKnob_CC_5
	movlw	0x13		; send CC#19
	rgoto	CS_MENU_MS_SendKnob_CC_Cont
CS_MENU_MS_SendKnob_CC_V
	return			; ignore velocity
CS_MENU_MS_SendKnob_CC_P
	movf	INDF1, W	; status byte (pointer to channel in FSR1)
	andlw	0x0f
	iorlw	0xe0
	call	MIOS_MIDI_TxBufferPut

	movlw	0x00		; LSB
	btfsc	INDF0, 0
	movlw 0x40
	call	MIOS_MIDI_TxBufferPut

	rrf	INDF0, W	; MSBs
	andlw	0x7f
	goto	MIOS_MIDI_TxBufferPut

CS_MENU_MS_SendKnob_CC_A
	return			; ignore aftertouch


;; --------------------------------------------------------------------------
;;  A knob value should be updated (e.g. based on new Min/Max boundary)
;;  IN: knob number in WREG
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateKnob
	;; store knob number in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1

	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_UpdateKnob_Slave

CS_MENU_MS_UpdateKnob_Master
	;; just get&set value, this will calculate the new modulation result
	movf	CS_MENU_SELECTED_KNOB, W
	goto	SID_KNOB_UpdateValue

CS_MENU_MS_UpdateKnob_Slave
	;; request update via CAN
	;; (expects knob number in MIOS_PARAMETER1)
	goto	CS_MENU_MBNET_Tx_UpdateKnob


;; --------------------------------------------------------------------------
;;  Updates the shadow buffer - should be used when WT/Knob assignments are
;;  changed
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateShadow
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_UpdateShadow_Slave

CS_MENU_MS_UpdateShadow_Master
	;; use local update routine
	goto	SID_PATCH_UpdateShadowBuffer

CS_MENU_MS_UpdateShadow_Slave
	;; request update via CAN
	goto	CS_MENU_MBNET_Tx_UpdateShadow


;; --------------------------------------------------------------------------
;;  If update of all parameters has been requested
;;  expecting SID number in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_UpdateAllParameters
	;; send dump
	rcall	CS_MENU_MS_SendPatch

	;; force a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; if in main page: force complete re-initialization
	btfsc	CS_MENU, 7
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	return


;; --------------------------------------------------------------------------
;;  some util routines for the SID_PATCH and SID_BANK handling
;; --------------------------------------------------------------------------
	;; set SID_PATCH/BANK to patch/bank number of current selected SID
	;; doesn't overwrite WREG!
CS_MENU_MS_SetSIDPatch_C
	;; branch depending on selected SID
	BRA_IFSET CS_MENU_SID, 1, ACCESS, CS_MENU_MS_SetSIDPatch_C_23

CS_MENU_MS_SetSIDPatch_C_01
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_SetSIDPatch_C_1
CS_MENU_MS_SetSIDPatch_C_0
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End
CS_MENU_MS_SetSIDPatch_C_1
	movff	CS_MENU_SID_S1_PATCH, SID_PATCH
	movff	CS_MENU_SID_S1_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End

CS_MENU_MS_SetSIDPatch_C_23
	BRA_IFSET CS_MENU_SID, 0, ACCESS, CS_MENU_MS_SetSIDPatch_C_3
CS_MENU_MS_SetSIDPatch_C_2
	movff	CS_MENU_SID_S2_PATCH, SID_PATCH
	movff	CS_MENU_SID_S2_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End
CS_MENU_MS_SetSIDPatch_C_3
	movff	CS_MENU_SID_S3_PATCH, SID_PATCH
	movff	CS_MENU_SID_S3_BANK, SID_BANK
	rgoto	CS_MENU_MS_SetSIDPatch_C_End

CS_MENU_MS_SetSIDPatch_C_End
	;; mask out 'sent' flag
	bcf	SID_PATCH, 7
	return


	;; set SID_BANK to patch number of master SID
	;; doesn't overwrite WREG!
CS_MENU_MS_SetSIDPatch_M
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK
	;; mask out 'sent' flag
	bcf	SID_PATCH, 7
	return

	;; get patch number of current selected SID
	;; uses FSR1!
CS_MENU_MS_GetSIDPatch
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x7f
	return

	;; get bank number of current selected SID
	;; uses FSR1!
CS_MENU_MS_GetSIDBank
	lfsr	FSR1, CS_MENU_SID_M_BANK
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x07
	return

;; --------------------------------------------------------------------------
;;  This function is called to forward a Program Change event to the control surface
;;  Input:
;;     o midi channel in SID_CURRENT_CHANNEL
;;     o patch number in SID_MIDI_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_MS_NotifyProgramChange
	;; request patch send to SIDs with matching channel number
	SET_BSR	SID_BASE
	clrf	PRODL			; PRODL used as counter
CS_MENU_MS_NotifyProgramChangeL
	;; calc pointer to channel entry
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	PRODL, W
	addwf	FSR0L, F

	;; check if channel number matches
	movf	INDF0, W
	xorwf	SID_CURRENT_CHANNEL, W, BANKED
	andlw	0x0f
	bnz	CS_MENU_MS_NotifyProgramChangeLN
	;; check if patch number matches (in this case don't change the patch)
	movlw	(CS_MENU_SID_M_PATCH-CS_MENU_SID_M_CHN) & 0xff
	addwf	FSR0L, F
	movf	SID_MIDI_PARAMETER1, W
	xorwf	INDF0, W
	andlw	0x7f
	bz	CS_MENU_MS_NotifyProgramChangeLN
	;; store patch number
	movff	SID_MIDI_PARAMETER1, INDF0
	;; skip if patch change already requested
	movlw	(CS_MENU_TX_M_CTR-CS_MENU_SID_M_PATCH) & 0xff
	addwf	FSR0L, F
	BRA_IFSET INDF0, 7, ACCESS, CS_MENU_MS_NotifyProgramChangeLN
	;; request patch change
	movlw	0x81
	movwf	INDF0
CS_MENU_MS_NotifyProgramChangeLN
	incf	PRODL, F
	BRA_IFCLR PRODL, 2, ACCESS, CS_MENU_MS_NotifyProgramChangeL

	return
	

;; --------------------------------------------------------------------------
;;  This function is called to forward a Bank Change event to the control surface
;;  Input:
;;     o midi channel in SID_CURRENT_CHANNEL
;;     o bank number in SID_MIDI_PARAMETER2
;; --------------------------------------------------------------------------
CS_MENU_MS_NotifyBankChange
	;; request patch send to SIDs with matching channel number
	SET_BSR	SID_BASE
	clrf	PRODL			; PRODL used as counter
CS_MENU_MS_NotifyBankChangeL
	;; calc pointer to channel entry
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	PRODL, W
	addwf	FSR0L, F

	;; check if channel number matches
	movf	INDF0, W
	xorwf	SID_CURRENT_CHANNEL, W, BANKED
	andlw	0x0f
	bnz	CS_MENU_MS_NotifyBankChangeLN
	;; check if bank number matches (in this case don't change the patch)
	movlw	(CS_MENU_SID_M_BANK-CS_MENU_SID_M_CHN) & 0xff
	addwf	FSR0L, F
	movf	SID_MIDI_PARAMETER2, W
	xorwf	INDF0, W
	andlw	0x07
	bz	CS_MENU_MS_NotifyBankChangeLN
	;; store bank number
	movf	SID_MIDI_PARAMETER2, W
	andlw	0x07
	movwf	INDF0

	movlw	(CS_MENU_SID_M_PATCH-CS_MENU_SID_M_BANK) & 0xff
	addwf	FSR0L, F	; clear 'sent' flag of patch
	bcf	INDF0, 7
	;; skip if patch change already requested
	movlw	(CS_MENU_TX_M_CTR-CS_MENU_SID_M_PATCH) & 0xff
	addwf	FSR0L, F
	BRA_IFSET INDF0, 7, ACCESS, CS_MENU_MS_NotifyBankChangeLN
	;; request bank/patch change
	movlw	0x81
	movwf	INDF0
CS_MENU_MS_NotifyBankChangeLN
	incf	PRODL, F
	BRA_IFCLR PRODL, 2, ACCESS, CS_MENU_MS_NotifyBankChangeL

	return


;; --------------------------------------------------------------------------
;;  this function sends the edit buffer to the SID selected by CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_EditBuffer
	;; store selected engine code
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	call	CS_MENU_MS_StoreEngineCode

	;; branch depending on master/slave SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_Send_EditBuffer_S

CS_MENU_MS_Send_EditBuffer_M
	;; local SID: copy edit buffer into SID_PATCH_BUFFER and shadow buffer
	lfsr	FSR0, SID_EDIT_BUFFER
	lfsr	FSR1, SID_PATCH_BUFFER
	lfsr	FSR2, SID_PATCH_BUFFER_SHADOW
	movlw	2
	movwf	PRODL
CS_MENU_MS_Send_EditBuffer_MLoop
	movff	INDF0, POSTINC1
	movff	POSTINC0, POSTINC2
	movf	FSR0L, W
	bnz	CS_MENU_MS_Send_EditBuffer_MLoop
	decfsz	PRODL, F
	rgoto	CS_MENU_MS_Send_EditBuffer_MLoop

	;; init patch and exit
	goto	SID_PATCH_Init


CS_MENU_MS_Send_EditBuffer_S
	;; send edit buffer to slave
	goto	CS_MENU_MBNET_Tx_SendEdit


;; --------------------------------------------------------------------------
;;  this function sends the edit buffer to all selected SIDs
;; --------------------------------------------------------------------------
CS_MENU_MS_Send_EditBufferAll
	clrf	CS_MENU_SID
CS_MENU_MS_Send_EditBufferAllLp
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_SID_FLAGS, W
	bz	CS_MENU_MS_Send_EditBufferAllLpN
	rcall	CS_MENU_MS_Send_EditBuffer
CS_MENU_MS_Send_EditBufferAllLpN
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_MS_Send_EditBufferAllLp

	;; get back old CS_MENU_SID and exit
	rgoto	CS_MENU_MS_GetSIDNumber

	
;; --------------------------------------------------------------------------
;;  this function sends the whole patch to the SID which is
;;  selected by CS_MENU_SID
;;  Doesn't overwrite the edit buffer
;; --------------------------------------------------------------------------
CS_MENU_MS_SendPatch
	;; check TX counter, if > 6, delay the sending
	;; this register is decremented by cs_menu_timer.inc every 100 ms
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR1L, F
	
	;; if counter != 0, request delayed TX of dump and exit
	movf	INDF1, W
	skpz
	rgoto	CS_MENU_MS_SendPatch_TxReq

	;; branch depending on master/slave SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_SendPatch_S

CS_MENU_MS_SendPatch_M
	;; reload TX counter
	movlw	0x01
	movwf	INDF1

	;; skip if 'sent' flag set
	SET_BSR	CS_MENU_SID_M_PATCH
	BRA_IFSET CS_MENU_SID_M_PATCH, 7, BANKED, CS_MENU_MS_SendPatch_End

	;; store patch/bank number of master in SID_PATCH/SID_BANK
	movff	CS_MENU_SID_M_PATCH, SID_PATCH
	movff	CS_MENU_SID_M_BANK, SID_BANK

	bsf	CS_MENU_SID_M_PATCH, 7, BANKED	; set 'sent' flag

	;; transfer EEPROM content into SID patch buffer and init patch
	call	SID_PATCH_LoadPatchBuffer

	;; store selected engine code
	movff	SID_PATCH_BUFFER + SID_Ix_ENGINE, WREG
	call	CS_MENU_MS_StoreEngineCode

	;; restore CS_MENU_SID
	rgoto	CS_MENU_MS_SendPatch_End

CS_MENU_MS_SendPatch_S
	;; else send dump of current patch to slave SID

	;; reload TX counter
	movlw	0x06
	movwf	INDF1

	;; skip send patch if 'sent' flag set
	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	BRA_IFSET PLUSW1, 7, ACCESS, CS_MENU_MS_SendPatch_End
	bsf	PLUSW1, 7	; set 'sent' flag

	;; disable SID engine of slave
	call	CS_MENU_MBNET_Tx_DisableEngine

	;; send patch to slave
	call	CS_MENU_MS_SetSIDPatch_C	; prepares SID_PATCH and SID_BANK
	call	CS_MENU_MBNET_Tx_SendPatch	; sends the patch
	
CS_MENU_MS_SendPatch_End

#if DEFAULT_LCD_LINES >= 3
	;; request label update (important for ENS main page)
	call	CS_MENU_UpdateLabel
#endif

	return


;; --------------------------------------------------------------------------
;; request delayed TX of dump and exit
CS_MENU_MS_SendPatch_TxReq
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR1L, F	; no delay
	movlw	0x80
	movwf	INDF1
	return

;; --------------------------------------------------------------------------
;; check if transfer can be granted
CS_MENU_MS_SendPatch_TxTst

	clrf	CS_MENU_SID		; handle all 4 SIDs
CS_MENU_MS_SendPatch_TxTstL
	rcall	CS_MENU_MS_SendPatch_ChkRequest
	bnz	CS_MENU_MS_SendPatch_TxTstN

	;; clear request
	bcf	INDF1, 7

	;; and update parameters
	rcall	CS_MENU_MS_UpdateAllParameters

#if 1
	;; set request flags again if slave and in superpoly mode
	;; see also code after this loop
	movff	SID_LOCAL_ENS + SID_ENSx_SUPERPOLY_CTRL, WREG
	andlw	0x07
	bz	CS_MENU_MS_SendPatch_TxTstN
	;; this is really dirty now, but required: ensure that all slaves will be updated at once
	;; counter could be different for the slaves, as it's decremented in CS_MENU_Timer interrupt routine
	movlw	0x80
	movwf	CS_MENU_TX_S1_CTR
	movwf	CS_MENU_TX_S2_CTR
	movwf	CS_MENU_TX_S3_CTR
#endif

CS_MENU_MS_SendPatch_TxTstN
	incf	CS_MENU_SID, F		; loop 4 times
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_MS_SendPatch_TxTstL

CS_MENU_MS_SendPatch_TxTst_End
	return




;; help routine which checks, if a patch should be updated
;; ZERO flag set if request flag set and counter reached 0
;; outputs pointer to CS_MENU_TX_x_CTR register in INDF1
CS_MENU_MS_SendPatch_ChkRequest
	lfsr	FSR1, CS_MENU_TX_M_CTR
	movf	CS_MENU_SID, W
	addwf	FSR1L, F

	;; skip if CTR[7] not set (no sync requested)
	BRA_IFCLR INDF1, 7, ACCESS, CS_MENU_MS_SendPatch_ChkRequestN

	;; skip if CTR[6..0] is != 0
	movf	INDF1, W
	andlw	0x7f
	bnz	CS_MENU_MS_SendPatch_ChkRequestN

	;; request valid - ZERO flag set
	return

CS_MENU_MS_SendPatch_ChkRequestN
	iorlw	0xff		; no valid request - ZERO flag cleared
	return


;; --------------------------------------------------------------------------
;;  this routine is used when a note should be played on a given SID
;;  Note On/Off status will be toggled each time this function is executed
;;  IN: SID number in WREG (will be stored in CS_MENU_SID)
;; --------------------------------------------------------------------------
CS_MENU_MS_PlayNote
	movwf	CS_MENU_SID

	;; if master is running in superpoly mode: play chord on all SIDs
	movff	SID_LOCAL_ENS + SID_ENSx_SUPERPOLY_CTRL, WREG
	andlw	0x07
	bz	CS_MENU_MS_PlayNote_NoSuperPoly
CS_MENU_MS_PlayNote_SuperPoly
	;; toggle play state of all SIDs
	BRA_IFSET CS_STAT2, CS_STAT2_PLAY_SID1, ACCESS, CS_MENU_MS_PlayNote_SuperPoly1
CS_MENU_MS_PlayNote_SuperPoly0
	movlw	0xf0
	iorwf	CS_STAT2, F
	goto	SID_PATCH_NotesOn ; will handle all SIDs

CS_MENU_MS_PlayNote_SuperPoly1
	movlw	0x0f
	andwf	CS_STAT2, F
	goto	SID_PATCH_NotesOff ; will handle all SIDs

	
CS_MENU_MS_PlayNote_NoSuperPoly
	;; toggle play state which is stored in CS_STAT2
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andlw	0xf0
	movwf	TMP2

	IRQ_DISABLE		; must be atomic since the IRQ handler modifies this register
	movf	CS_STAT2, W
	xorwf	TMP2, W
	movwf	CS_STAT2
	IRQ_ENABLE

	;; branch to Note On/Off function
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	andwf	CS_STAT2, W
	bz	CS_MENU_MS_PlayNoteOff
CS_MENU_MS_PlayNoteOn
	rgoto	CS_MENU_MS_NotesOn
CS_MENU_MS_PlayNoteOff	
	rgoto	CS_MENU_MS_NotesOff


;; --------------------------------------------------------------------------
;;  All Notes On command
;; --------------------------------------------------------------------------
CS_MENU_MS_NotesOn
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_NotesOn_Slave

CS_MENU_MS_NotesOn_Master
	;; use local routine
	goto	SID_PATCH_NotesOn

CS_MENU_MS_NotesOn_Slave
	;; request notes off via CAN
	goto	CS_MENU_MBNET_Tx_NotesOn


;; --------------------------------------------------------------------------
;;  All Notes Off command
;; --------------------------------------------------------------------------
CS_MENU_MS_NotesOff
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_NotesOff_Slave

CS_MENU_MS_NotesOff_Master
	;; use local routine
	goto	SID_PATCH_NotesOff

CS_MENU_MS_NotesOff_Slave
	;; request notes off via CAN
	goto	CS_MENU_MBNET_Tx_NotesOff


;; --------------------------------------------------------------------------
;;  Voice Reinit command
;; --------------------------------------------------------------------------
CS_MENU_MS_VoiceReInit
	;; branch depending on selected SID
	movf	CS_MENU_SID, W
	bnz	CS_MENU_MS_VoiceReInit_Slave

CS_MENU_MS_VoiceReInit_Master
	;; use local routine
	goto	SID_VOICE_ReInit

CS_MENU_MS_VoiceReInit_Slave
	;; request notes off via CAN
	goto	CS_MENU_MBNET_Tx_VoiceReInit


;; --------------------------------------------------------------------------
;;  Store Engine code of the selected SID
;;  IN: engine code (2bit) in WREG
;;      selected SID in CS_MENU_SID
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
CS_MENU_MS_StoreEngineCode
	;; using a single variable to store engine codes - this saves RAM, but costs a bit more flash memory...

	;; store engine code at the correct position in PRODL, and mask in PRODH
	andlw	0x03
	movwf	PRODL
	movlw	0x03
	movwf	PRODH

	btfsc	CS_MENU_SID, 1
	swapf	PRODL, F
	btfsc	CS_MENU_SID, 1
	swapf	PRODH, F

	BRA_IFCLR CS_MENU_SID, 0, ACCESS, CS_MENU_MS_StoreEngineCode_NoShf
CS_MENU_MS_StoreEngineCode_Shift
	clrc
	rlf	PRODL, F
	rlf	PRODL, F
	rlf	PRODH, F
	rlf	PRODH, F
CS_MENU_MS_StoreEngineCode_NoShf

	comf	PRODH, W
	andwf	CS_MENU_ENGINES, F
	movf	PRODL, W
	iorwf	CS_MENU_ENGINES, F
	return


;; --------------------------------------------------------------------------
;;  Returns the engine code of the selected SID
;;  IN: selected SID in CS_MENU_SID
;;  OUT: engine code (2bit) in WREG
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
CS_MENU_MS_GetEngineCode
	movf	CS_MENU_ENGINES, W
	btfsc	CS_MENU_SID, 1
	swapf	WREG, W

	BRA_IFCLR CS_MENU_SID, 0, ACCESS, CS_MENU_MS_GetEngineCode_NoShf
CS_MENU_MS_GetEngineCode_Shift
	clrc
	rrf	WREG, W
	clrc
	rrf	WREG, W
CS_MENU_MS_GetEngineCode_NoShf
	andlw	0x03
	return
