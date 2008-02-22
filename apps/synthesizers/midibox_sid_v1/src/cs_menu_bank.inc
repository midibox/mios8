; $Id$
;
; SID Control Surface Bank Handling
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  In difference to the SID_BANK_Read function CS_BANK_Read reads from
;;  the patch of the current selected SID
;; --------------------------------------------------------------------------
CS_MENU_BANK_Read
	rcall	CS_MENU_MS_SetSIDPatch_C
	call	SID_BANK_Read
	rcall	CS_MENU_MS_SetSIDPatch_M
	return

;; --------------------------------------------------------------------------
;;  In difference to the SID_BANK_Write function CS_BANK_Write writes to
;;  the patch of the current selected SID
;; --------------------------------------------------------------------------
CS_MENU_BANK_Write
	rcall	CS_MENU_MS_SetSIDPatch_C
	call	SID_BANK_Write
	rgoto	CS_MENU_MS_SetSIDPatch_M


;; --------------------------------------------------------------------------
;;  Store CS SID channel in BankStick
;; --------------------------------------------------------------------------
CS_MENU_BANK_SaveCfgChannel
	btfss	SID_BANKSTICK_STATUS, 0
	return

	;; select first BankStick, verify off
	movlw	0x80
	call	MIOS_BANKSTICK_CtrlSet

	;; MIDI channels stored at address 0x0010 + 0x10*SID number
	swapf	CS_MENU_SID, W
	addlw	0x10
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	movf	PLUSW1, W
	andlw	0x7f
	goto	MIOS_BANKSTICK_Write

;; --------------------------------------------------------------------------
;;  Store CS SID Patch and bank number in BankStick
;; --------------------------------------------------------------------------
CS_MENU_BANK_SaveCfgPatchNumber
	btfss	SID_BANKSTICK_STATUS, 0
	return

	;; select first BankStick, verify off
	movlw	0x80
	call	MIOS_BANKSTICK_CtrlSet

	;; MIDI patch number stored at address 0x0014 + 0x10*SID number
	swapf	CS_MENU_SID, W
	addlw	0x14
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	lfsr	FSR1, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	movf	PLUSW1, W
	andlw	0x7f
	call	MIOS_BANKSTICK_Write

	;; MIDI patch number stored at address 0x0018 + 0x10*SID number
	movlw	0x04-1
	addwf	MIOS_PARAMETER1, F

	lfsr	FSR1, CS_MENU_SID_M_BANK
	movf	CS_MENU_SID, W
	movf	PLUSW1, W
	andlw	0x07
	goto	MIOS_BANKSTICK_Write
	

;; --------------------------------------------------------------------------
;;  Restore channel and patch numbers from BankStick
;;  (called after startup and BankStick exchange)
;; --------------------------------------------------------------------------
CS_MENU_BANK_RestoreCfg
	clrf	CS_MENU_SID
CS_MENU_BANK_RestoreCfg_Loop
	BRA_IFCLR SID_BANKSTICK_STATUS, 0, ACCESS, CS_MENU_BANK_RestoreCfg_LoopNoBS

	;; select first BankStick, verify off
	movlw	0x80
	call	MIOS_BANKSTICK_CtrlSet

	;; always access lower page
	clrf	MIOS_PARAMETER2

	;; MIDI channels stored at address 0x0010 + 0x10*SID number
	lfsr	FSR0, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	addwf	FSR0L, F

	swapf	CS_MENU_SID, W
	addlw	0x10
	movwf	MIOS_PARAMETER1
	call	MIOS_BANKSTICK_Read
	andlw	0x0f
	movwf	INDF0

	;; patch numbers stored at address 0x0014 + 0x10*SID number
	lfsr	FSR0, CS_MENU_SID_M_PATCH
	movf	CS_MENU_SID, W
	addwf	FSR0L, F

	swapf	WREG, W
	addlw	0x14
	movwf	MIOS_PARAMETER1
	call	MIOS_BANKSTICK_Read
	andlw	0x7f
	movwf	INDF0

	;; bank numbers stored at address 0x0018 + 0x10*SID number
	lfsr	FSR0, CS_MENU_SID_M_BANK
	movf	CS_MENU_SID, W
	addwf	FSR0L, F

	swapf	WREG, W
	addlw	0x18
	movwf	MIOS_PARAMETER1
	call	MIOS_BANKSTICK_Read
	andlw	0x07
	movwf	INDF0

CS_MENU_BANK_RestoreCfg_LoopNoBS
	;; request update of CS parameters
	call	CS_MENU_MS_Send_SysExDump_TxReq

	incf	CS_MENU_SID, F		; loop 4 times
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_BANK_RestoreCfg_Loop

	return

	

;; --------------------------------------------------------------------------
;;  Check restore of configuration
;; --------------------------------------------------------------------------
CS_MENU_BANK_RestoreCfg_Tst
	decf	CS_MENU_REINIT_CFG_CTR, W
	skpz
	return

	clrf	CS_MENU_REINIT_CFG_CTR
	goto	CS_MENU_BANK_RestoreCfg
