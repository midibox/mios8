; $Id$
;
; MIDIbox SID SysEx Parser
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

SID_SYSEX_STATE_MYSYSEX		EQU	7
SID_SYSEX_STATE_ACTION		EQU	6

SID_SYSEX_STATE_PATCH_RECEIVED	EQU	5	; used by Action PATCH_[Read/Write]
SID_SYSEX_STATE_WAIT_CHECKSUM	EQU	4	; used by Action PATCH_Write

SID_SYSEX_STATE_AH_RECEIVED	EQU	5	; used by Action PAR_[Read/Write]
SID_SYSEX_STATE_AL_RECEIVED	EQU	4	; used by Action PAR_[Read/Write]
SID_SYSEX_STATE_D_RECEIVED	EQU	3	; used by Action PAR_[Write]

SID_SYSEX_STATE_A_RECEIVED	EQU	5	; used by Action CFG_Write
SID_SYSEX_STATE_DH_RECEIVED	EQU	4	; used by Action CFG_Write
SID_SYSEX_STATE_DL_RECEIVED	EQU	3	; used by Action CFG_Write

;; --------------------------------------------------------------------------
;;  This sysex parser waits for the SID Header 
;; --------------------------------------------------------------------------
SID_SYSEX_SysExCheck
	;; store received byte in SID_SYSEX_IN
	movwf	SID_SYSEX_IN

	;; ignore realtime messages
	movlw	0xf8
	cpfslt	SID_SYSEX_IN, ACCESS
	return

	;; check sysex state
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_MYSYSEX, ACCESS, SID_SYSEX_Handler

	movf	SID_SYSEX_STATE, W
	rcall	SID_SYSEX_SysExHeaderGet
	cpfseq	SID_SYSEX_IN, ACCESS
	rgoto SID_SYSEX_SysExCheckFailed
	incf	SID_SYSEX_STATE, F
	movf	SID_SYSEX_STATE, W
	andlw	0x07
	xorlw	0x06	; wait for 6 bytes (f0 00 00 7E 46 <device-id>)
	bnz	SID_SYSEX_SysExCheckOk

	;; SysEx ID received, lets go
	movlw	(1 << SID_SYSEX_STATE_MYSYSEX)
	movwf	SID_SYSEX_STATE
	call	MIOS_MPROC_MergerDisable
	rgoto	SID_SYSEX_SysExCheckOk

SID_SYSEX_SysExCheckFailed
	;; reset the sysex counter and action ID
	rcall	SID_SYSEX_ActionInvalid
SID_SYSEX_SysExCheckOk

SID_SYSEX_SysExCheck_End
	return

; ==========================================================================

;; --------------------------------------------------------------------------
;;  Returns expected MIDI bytes from SysEx header
;; --------------------------------------------------------------------------
SID_SYSEX_SysExHeaderGet
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	retlw	0xf0
	retlw	0x00		; Vendor ID
	retlw	0x00
	retlw	0x7e
	retlw	0x46		; MIDIbox SID ID (46 - the ultimative number + 4)
	movf	SID_MIDI_DEVICE, W
	return

;; --------------------------------------------------------------------------
;;  Action Invalid will be invoked when receiving an invalid sequence
;; --------------------------------------------------------------------------
SID_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  Action finished will be invoked when midi action is done
;; --------------------------------------------------------------------------
SID_SYSEX_ActionFinished
	clrf	SID_SYSEX_STATE
	clrf	SID_SYSEX_ACTION

	;; reinit patch if engine has been disabled during upload
	CALL_IFSET SID_STAT, SID_STAT_ENGINE_DISABLE, ACCESS, SID_PATCH_Init
	
	call	MIOS_MPROC_MergerEnable
	rgoto	SID_SYSEX_SysExCheck_End


;; --------------------------------------------------------------------------
;;  MIDI Check action: perform a sysex action
;; --------------------------------------------------------------------------
SID_SYSEX_Handler
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_ACTION, ACCESS, SID_SYSEX_Handler_DoAction

	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_ACTION

	movff	SID_SYSEX_IN, SID_SYSEX_ACTION	; store action ID
	rcall	SID_SYSEX_Handler_InitAction	; initialize the action
	rgoto	SID_SYSEX_SysExCheck_End	; branch to the end

	;; ---

SID_SYSEX_Handler_DoAction
	;; branch to end if status byte (i.e. F7)
	BRA_IFSET SID_SYSEX_IN, 7, ACCESS, SID_SYSEX_Handler_EndAction

	;; branch depending on current action ID
	movf	SID_SYSEX_ACTION, W
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Action_PATCH_Read
	rgoto	SID_SYSEX_Action_PATCH_Write
	rgoto	SID_SYSEX_Action_PTCHES_Read
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Action_PAR_Read
	rgoto	SID_SYSEX_Action_PAR_Write
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Action_BANK_Sel
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Action_CFG_Write
	rgoto	SID_SYSEX_Action_RequestCC
	rgoto	SID_SYSEX_Action_Ping


;; --------------------------------------------------------------------------
;;  Initialize an action depending on SID_SYSEX_ACTION
;; --------------------------------------------------------------------------
SID_SYSEX_Handler_InitAction
	movf	SID_SYSEX_ACTION, W
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Init_PATCH_Read
	rgoto	SID_SYSEX_Init_PATCH_Write
	rgoto	SID_SYSEX_Init_PTCHES_Read
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Init_PAR_Read
	rgoto	SID_SYSEX_Init_PAR_Write
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Init_BANK_Sel
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_Init_CFG_Write
	rgoto	SID_SYSEX_Init_RequestCC
	rgoto	SID_SYSEX_Init_Ping

	
;; --------------------------------------------------------------------------
;;  Finish an action depending on SID_SYSEX_ACTION
;; --------------------------------------------------------------------------
SID_SYSEX_Handler_EndAction
	;; if sysex footer (F7) has not been received here, action is invalid!
	movlw	0xf7
	xorwf	SID_SYSEX_IN, W
	bnz	SID_SYSEX_ActionInvalid

	;; else finish action
	movf	SID_SYSEX_ACTION, W
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_End_PATCH_Read
	rgoto	SID_SYSEX_End_PATCH_Write
	rgoto	SID_SYSEX_End_PTCHES_Read
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_End_PAR_Read
	rgoto	SID_SYSEX_End_PAR_Write
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_End_BANK_Sel
	rgoto	SID_SYSEX_ActionInvalid
	rgoto	SID_SYSEX_End_CFG_Write
	rgoto	SID_SYSEX_End_RequestCC
	rgoto	SID_SYSEX_End_Ping

	

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Read
;; --------------------------------------------------------------------------
SID_SYSEX_Init_PATCH_Read
	return

	;; receive <patch> F7
SID_SYSEX_Action_PATCH_Read
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_PATCH_RECEIVED, ACCESS, SID_SYSEX_Action_PATCH_ReadStall
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_PATCH_RECEIVED
	movff	SID_SYSEX_IN, SID_PATCH		; switch to new patch
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_Action_PATCH_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_PATCH_Read
	;; action invalid if patch number has not been received
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_PATCH_RECEIVED, ACCESS, SID_SYSEX_ActionInvalid

	;; send SysEx header
	rcall	SID_SYSEX_Send_SysExHeader

	;; send PATCH_Write ID
	movlw	0x02
	call	MIOS_MIDI_TxBufferPut

	;; send requested patch number
	movf	SID_PATCH, W
	call	MIOS_MIDI_TxBufferPut

	;; clear checksum
	clrf	SID_SYSEX_CHECKSUM

	;; send patch (256 bytes)
	rcall	SID_SYSEX_Hlp_SendPatch
	
	;; send checksum
	movf	SID_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SID_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Patch Write
;; --------------------------------------------------------------------------
SID_SYSEX_Init_PATCH_Write
	;; disable SID engine until end of transfer (will be requested by ActionFinished)
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

	;; receive <patch> <256 bytes> F7
SID_SYSEX_Action_PATCH_Write
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_PATCH_RECEIVED, ACCESS, SID_SYSEX_Action_PATCH_WriteC
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_PATCH_RECEIVED
	movff	SID_SYSEX_IN, SID_PATCH		; switch to new patch
#if CS_ENABLED == 1
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W		; skip if device ID != 0x00
	bnz	SID_SYSEX_Action_PATCH_WriteNoCS
#endif
	movff	SID_SYSEX_IN, CS_MENU_SID_M_PATCH; and to the master patch
	call	CS_MENU_MS_Send_SysExDump_TxReq	; request CS update
	movlw	0x8f				; update after ca. one second
	movwf	INDF1
SID_SYSEX_Action_PATCH_WriteNoCS
#endif
	clrf	SID_SYSEX_CHECKSUM		; clear checksum
	clrf	SID_SYSEX_ADDRESS		; clear address
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_Action_PATCH_WriteC
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_WAIT_CHECKSUM, ACCESS, SID_SYSEX_Action_PATCH_WriteChk

	lfsr	FSR0, SID_BASE			; init pointer to upload buffer
	movff	SID_SYSEX_ADDRESS, FSR0L

	;; store received byte in upload buffer
	movf	SID_SYSEX_IN, W
	movwf	INDF0

	;; add to checksum
	andlw	0x7f
	addwf	SID_SYSEX_CHECKSUM, F
	
	;; increment address
	incf	SID_SYSEX_ADDRESS, F

	;; if FSR0L is zero, go into WAIT_CHECKSUM state
	skpnz
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_WAIT_CHECKSUM

	;; wait for next byte
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_Action_PATCH_WriteChk
	;; store received byte in checksum (using SID_SYSEX_ADDRESS as store register)
	movff	SID_SYSEX_IN, SID_SYSEX_ADDRESS

	;; wait for next byte
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_PATCH_Write
	;; action invalid if checksum has not been received
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_WAIT_CHECKSUM, ACCESS, SID_SYSEX_ActionInvalid

	;; calc final checksum
	movf	SID_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f

	;; compare with received checksum
	xorwf	SID_SYSEX_ADDRESS, W

	;; if not equal jump to ActionInvalid
	skpz
	rgoto	SID_SYSEX_ActionInvalid

	;; write buffer to EEPROM
	lfsr	FSR1, SID_BASE	; init pointer to upload buffer
	clrf	EEADR		; use EEADR as loop counter
SID_SYSEX_Action_PatchWriteLoop
	clrwdt			; feed watchdog
	call	SID_BANK_WritePage ; write a 64 bytes page to EEPROM
	;; increment FSR1 by 0x40
	movlw	0x40
	addwf	FSR1L, F
	movf	EEADR, W	; until EEADR == 0
	bnz	SID_SYSEX_Action_PatchWriteLoop

#if CS_ENABLED == 1
	;; print message if CS enabled
	TABLE_ADDR SID_SYSEX_Action_PatchWrite_STR
	call	MIOS_LCD_PrintMessage
	
	movlw	0x00 + 6
	call	MIOS_LCD_CursorSet
	movff	SID_BANK, PRODL
	movff	SID_PATCH, PRODH
	call	SID_LCD_PrintPatchNumber

	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	lfsr	FSR0, SID_BASE	; init pointer to upload buffer (name located between 0x00-0x0f)
SID_SYSEX_PATCH_WritePrnLoop
	movf	POSTINC0, W
	call	MIOS_LCD_PrintChar
	movf	FSR0L, W
	andlw	0x0f
	bnz	SID_SYSEX_PATCH_WritePrnLoop
#endif

	;; send acknowledge
	rcall	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

SID_SYSEX_Action_PatchWrite_STR
	STRING	20, 0x00, "Patch A  1 uploaded "

;; --------------------------------------------------------------------------
;;  MIDI Action: All Patch Read
;; --------------------------------------------------------------------------
SID_SYSEX_Init_PTCHES_Read
	return

SID_SYSEX_Action_PTCHES_Read
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_PTCHES_Read
	;; send SysEx header
	rcall	SID_SYSEX_Send_SysExHeader

	;; send PTCHES_Write ID
	movlw	0x04
	call	MIOS_MIDI_TxBufferPut

	;; save current patch in TMP1
	movff	SID_PATCH, TMP1

	;; clear checksum
	clrf	SID_SYSEX_CHECKSUM

	;; 128 patches to send, SID_PATCH used as counter
	clrf	SID_PATCH
SID_SYSEX_Action_PTCHES_Read_OL	; outer loop
	clrwdt				; feed the watchdog
	rcall	SID_SYSEX_Hlp_SendPatch	; send 256 bytes

	incf	SID_PATCH, F	; loop 128 times
	BRA_IFCLR SID_PATCH, 7, ACCESS, SID_SYSEX_Action_PTCHES_Read_OL

	;; send checksum
	movf	SID_SYSEX_CHECKSUM, W
	sublw	0x80
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; restore patch number which has been saved in TMP1
	movff	TMP1, SID_PATCH

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SID_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Read
;; --------------------------------------------------------------------------
SID_SYSEX_Init_PAR_Read
	return

SID_SYSEX_Action_PAR_Read
	;; receive <AH> <AL> F7
SID_SYSEX_Action_PAR_ReadAH
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_AH_RECEIVED, ACCESS, SID_SYSEX_Action_PAR_ReadAL
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_AH_RECEIVED
	clrf	SID_SYSEX_ADDRESS		; clear register and set SID_SYSEX_ADDRESS[7] if IN[0] is set
	btfsc	SID_SYSEX_IN, 0
	bsf	SID_SYSEX_ADDRESS, 7
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_Action_PAR_ReadAL
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_AL_RECEIVED, ACCESS, SID_SYSEX_Action_PAR_ReadStall
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_AL_RECEIVED
	movf	SID_SYSEX_IN, W			; OR SID_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	SID_SYSEX_ADDRESS, F
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_Action_PAR_ReadStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_PAR_Read
	;; action invalid if low-byte of address has not been received
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_AL_RECEIVED, ACCESS, SID_SYSEX_ActionInvalid

	;; send SysEx header
	rcall	SID_SYSEX_Send_SysExHeader

	;; send PAR_Write ID
	movlw	0x06
	call	MIOS_MIDI_TxBufferPut

	;; send AH
	movlw	0x00
	btfsc	SID_SYSEX_ADDRESS, 7
	movlw 0x01
	call	MIOS_MIDI_TxBufferPut

	;; send AL
	movf	SID_SYSEX_ADDRESS, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; get corresponding CC parameter from address
	movf	SID_SYSEX_ADDRESS, W
	call	SID_SYSEX_TABLE_Get

	;; branch if EEPROM value should be sent
	BRA_IFSET WREG, 7, ACCESS, SID_SYSEX_Action_PAR_Read_EE

SID_SYSEX_Action_PAR_Read_RAM
	;; get CC value from RAM
	call	SID_CCOUT_Get
	rgoto	SID_SYSEX_Action_PAR_Read_Cont
SID_SYSEX_Action_PAR_Read_EE
	;; read value from EEPROM
	movff	SID_SYSEX_ADDRESS, EEADR
	call	SID_BANK_Read
SID_SYSEX_Action_PAR_Read_Cont
	;; send value
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut
	
	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SID_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Parameter Write
;; --------------------------------------------------------------------------
SID_SYSEX_Init_PAR_Write
	return

SID_SYSEX_Action_PAR_Write
	;; receive <AH> <AL> <value> F7
SID_SYSEX_Action_PAR_WriteAH
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_AH_RECEIVED, ACCESS, SID_SYSEX_Action_PAR_WriteAL
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_AH_RECEIVED
	clrf	SID_SYSEX_ADDRESS		; clear register and set SID_SYSEX_ADDRESS[7] if IN[0] is set
	btfsc	SID_SYSEX_IN, 0
	bsf	SID_SYSEX_ADDRESS, 7
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_Action_PAR_WriteAL
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_AL_RECEIVED, ACCESS, SID_SYSEX_Action_PAR_WriteD
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_AL_RECEIVED
	movf	SID_SYSEX_IN, W			; OR SID_SYSEX_ADDRESS with low-byte
	andlw	0x7f
	iorwf	SID_SYSEX_ADDRESS, F
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_Action_PAR_WriteD
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_D_RECEIVED, ACCESS, SID_SYSEX_Action_PAR_WriteStall
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_D_RECEIVED	
	movff	SID_SYSEX_IN, SID_SYSEX_CHECKSUM; store byte in SID_SYSEX_CHECKSUM (used as write buffer here)
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_Action_PAR_WriteStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_PAR_Write
	;; action invalid if data has not been received
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_D_RECEIVED, ACCESS, SID_SYSEX_ActionInvalid

	;; update parameter in CS
#if CS_ENABLED
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	SID_SYSEX_End_PAR_Write_NoCS
#endif
	clrf	CS_MENU_SID
	movff	SID_SYSEX_CHECKSUM, MIOS_PARAMETER1 ; byte has been stored in _CHECKSUM
	movf	SID_SYSEX_ADDRESS, W
	call	CS_MENU_UpdateSyxPara
SID_SYSEX_End_PAR_Write_NoCS
#endif

	;; get corresponding CC parameter from address
	movf	SID_SYSEX_ADDRESS, W
	call	SID_SYSEX_TABLE_Get
	movwf	TMP1

	;; branch if value should be saved in EEPROM
	BRA_IFSET TMP1, 7, ACCESS, SID_SYSEX_Action_PAR_Write_EE

SID_SYSEX_Action_PAR_Write_RAM
	;; write CC value to RAM
	movff	SID_SYSEX_CHECKSUM, MIOS_PARAMETER1 ; byte has been stored in _CHECKSUM
	call	SID_CCIN_Set
	rgoto	SID_SYSEX_Action_PAR_Write_Cont
SID_SYSEX_Action_PAR_Write_EE
	;; if split parameter, redirect to SID_Vx_SPLIT_xxx
	movf	TMP1, W
	andlw	0xf0
	xorlw	0x90
	bnz	SID_SYSEX_Action_PAR_Write_EEC
SID_SYSEX_Action_PAR_Write_EES
	movf	TMP1, W
	call	SID_PATCH_GetSplitPointer
	movff	SID_SYSEX_CHECKSUM, PLUSW1
	rgoto	SID_SYSEX_Action_PAR_Write_Cont

SID_SYSEX_Action_PAR_Write_EEC
	;; write value to EEPROM
	movff	SID_SYSEX_ADDRESS, EEADR
	movf	SID_SYSEX_CHECKSUM, W; byte has been stored in _CHECKSUM
	call	SID_BANK_Write
SID_SYSEX_Action_PAR_Write_Cont

	;; send acknowledge
	call	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: Switch Bank
;; --------------------------------------------------------------------------
SID_SYSEX_Init_BANK_Sel
	clrf	SID_SYSEX_ADDRESS
	return

SID_SYSEX_Action_BANK_Sel
	;; store bank in SID_SYSEX_ADDRESS
	movff	SID_SYSEX_IN, SID_SYSEX_ADDRESS
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_End_BANK_Sel
	;; check if BankStick available
	movlw	0x08
	cpfslt	SID_SYSEX_ADDRESS, ACCESS
	rgoto SID_SYSEX_End_BANK_SelInv
	movf	SID_SYSEX_ADDRESS, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bz	SID_SYSEX_End_BANK_SelInv

	;; change to bank immediately
	movff	SID_SYSEX_ADDRESS, SID_BANK
	rcall	SID_SYSEX_ChangeMPatch

	;; 	rgoto	SID_SYSEX_End_BANK_Cont

	;; don't select new bankstick
SID_SYSEX_End_BANK_SelInv

SID_SYSEX_End_BANK_Cont
	;; send acknowledge with bank number
	rcall	SID_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	movf	SID_BANK, W
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rcall	SID_SYSEX_Send_SysExFooter

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Configuration Write
;; --------------------------------------------------------------------------
SID_SYSEX_Init_CFG_Write
	;; disable SID engine until end of transfer (will be requested by ActionFinished)
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE
	return

SID_SYSEX_Action_CFG_Write
	;; receive <A> <DH> <DL> F7
SID_SYSEX_Action_CFG_WriteA
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_A_RECEIVED, ACCESS, SID_SYSEX_Action_CFG_WriteDH
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_A_RECEIVED
	movff	SID_SYSEX_IN, SID_SYSEX_ADDRESS	; store address in SID_SYSEX_ADDRESS
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte
	
SID_SYSEX_Action_CFG_WriteDH
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_DH_RECEIVED, ACCESS, SID_SYSEX_Action_CFG_WriteDL
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_DH_RECEIVED
	swapf	SID_SYSEX_IN, W			; store high-nibble in SID_SYSEX_CHECKSUM (used as data buffer here)
	andlw	0xf0
	movwf	SID_SYSEX_CHECKSUM
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_Action_CFG_WriteDL
	BRA_IFSET SID_SYSEX_STATE, SID_SYSEX_STATE_DL_RECEIVED, ACCESS, SID_SYSEX_Action_CFG_WriteStall
	bsf	SID_SYSEX_STATE, SID_SYSEX_STATE_DL_RECEIVED
	movf	SID_SYSEX_IN, W			; store low-nibble in SID_SYSEX_CHECKSUM (used as data buffer here)
	iorwf	SID_SYSEX_CHECKSUM, F
	rgoto	SID_SYSEX_SysExCheck_End	; wait for next byte

SID_SYSEX_Action_CFG_WriteStall
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_CFG_Write
	;; action invalid if data has not been received
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_DL_RECEIVED, ACCESS, SID_SYSEX_ActionInvalid

	;; calc resulting EEADR
	movf	SID_SYSEX_ADDRESS, W
	andlw	0x03
	addlw	EEPROM_CFG_BASE & 0xff
	movwf	EEADR

	;; write value to EEPROM
	movf	SID_SYSEX_CHECKSUM, W	; (value has been stored in SID_SYSEX_CHECKSUM)
	call	MIOS_EEPROM_Write
	
	;; send acknowledge
	call	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished

;; --------------------------------------------------------------------------
;;  MIDI Action: CC Request
;; --------------------------------------------------------------------------
SID_SYSEX_Init_RequestCC
	return

SID_SYSEX_Action_RequestCC
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_RequestCC
	;; request CC dump (handled by sid_ccout.inc)
	bsf	SID_STAT, SID_STAT_CC_DUMP_REQ

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished


;; --------------------------------------------------------------------------
;;  MIDI Action: Ping
;; --------------------------------------------------------------------------
SID_SYSEX_Init_Ping
	return

SID_SYSEX_Action_Ping
	;; do nothing until sysex footer (F7) has been received
	rgoto	SID_SYSEX_SysExCheck_End

SID_SYSEX_End_Ping
	;; send Acknowledge
	rcall	SID_SYSEX_Send_Acknowledge

	;; finish Action
	rgoto	SID_SYSEX_ActionFinished



;; --------------------------------------------------------------------------
;;  MIDI Send Acknowledge (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_Acknowledge
	rcall	SID_SYSEX_Send_SysExHeader

	movlw	0x0f		; (acknowledge ID)
	call	MIOS_MIDI_TxBufferPut

	;; send of SysEx footer
	movlw	0x01		; (independend from merger state)
	rgoto	SID_SYSEX_Send_SysExFooter

;; --------------------------------------------------------------------------
;;  Send SID SysEx Header (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_SysExHeader
	;; if SID_SYSEX_SYXSTATE > 0, check merger flag to allow propper sysex merging
	BRA_IFCLR SID_SYSEX_STATE, SID_SYSEX_STATE_MYSYSEX, ACCESS, SID_SYSEX_Send_SysExHeader_Skp
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	bz	SID_SYSEX_Send_SysExHeader_Skp
	movlw	0x05		; send only DEVICE_ID
	movwf	TMP1
	rgoto	SID_SYSEX_Send_SysExHeaderLoop
SID_SYSEX_Send_SysExHeader_Skp

	clrf	TMP1
SID_SYSEX_Send_SysExHeaderLoop
	movf	TMP1, W
	rcall	SID_SYSEX_SysExHeaderGet
	call	MIOS_MIDI_TxBufferPut
	incf	TMP1, F
	movlw	0x06
	cpfseq	TMP1, ACCESS
	rgoto SID_SYSEX_Send_SysExHeaderLoop
	return

;; --------------------------------------------------------------------------
;;  MIDI Send SysEx Footer (Util function)
;; --------------------------------------------------------------------------
SID_SYSEX_Send_SysExFooter
	;; if WREG[0]=1: send F7 regardless of the merger state
	BRA_IFSET WREG, 0, ACCESS, SID_SYSEX_Send_SysExFooter_Force
	;; send footer only if merger has been enabled
	;; to ensure a proper MIDI protocol
	call	MIOS_MIDI_MergerGet
	andlw	0x01
	skpnz
	return

SID_SYSEX_Send_SysExFooter_Force
	movlw	0xf7
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Changes the SID Master patch
;;  IN: patch number in SID_PATCH
;;      bank number in SID_BANK
;; --------------------------------------------------------------------------
SID_SYSEX_ChangeMPatch
#if CS_ENABLED == 1
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W		; skip if device ID != 0x00
	bnz	SID_SYSEX_ChangeMPatch_End
#endif
	movff	SID_PATCH, CS_MENU_SID_M_PATCH
	bsf	CS_MENU_SID_M_PATCH, 7
	movff	SID_BANK, CS_MENU_SID_M_BANK
	;; update all patch parameters
	call	SID_PATCH_Init
	;; request complete CS initialization
	clrf	CS_MENU_SID
	call	CS_MENU_MS_UpdateAllParameters
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
SID_SYSEX_ChangeMPatch_End
#endif
	return


;; --------------------------------------------------------------------------
;;  Sends a patch
;;  IN: patch number in SID_PATCH
;;      bank number in SID_BANK
;;      SID_SYSEX_CHECKSUM should be cleared if required
;;  OUT: 256 bytes will be sent
;;       checksum in SID_SYSEX_CHECKSUM
;; --------------------------------------------------------------------------
SID_SYSEX_Hlp_SendPatch
	clrf	EEADR			; 0x100 bytes to send, use EEADR as counter
SID_SYSEX_Hlp_SendPatchLoop
	call	SID_BANK_Read	; read patch content (EEADR will be incremented)
	movwf	TABLAT		; store data in TABLAT

	movlw	0x10+1		; ensure that patch name doesn't contain characters < 0x20
	cpfslt	EEADR, ACCESS
	rgoto SID_SYSEX_Hlp_SendPatchLoop_NoN
SID_SYSEX_Hlp_SendPatchLoop_N
	movlw	0x20
	cpfslt	TABLAT, ACCESS
	rgoto SID_SYSEX_Hlp_SendPatchLoop_NoN
	movwf	TABLAT
SID_SYSEX_Hlp_SendPatchLoop_NoN
	movf	TABLAT, W
	andlw	0x7f		; add to checksum
	addwf	SID_SYSEX_CHECKSUM, F

	call	MIOS_MIDI_TxBufferPut; send byte

	movf	EEADR, W
	bnz	SID_SYSEX_Hlp_SendPatchLoop	; loop 256 times

	return
