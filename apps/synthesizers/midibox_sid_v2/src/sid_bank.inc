; $Id$
;
; MIDIbox SID
; BankStick Handler
;
; (Bank Load/Store routines located in sid_pbank.inc and sid_ebank.inc)
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear the appr. flag in
;;  SID_BANKSTICK_STATUS if BankStick not available
;; --------------------------------------------------------------------------
SID_BANK_CheckStick
	;; increment check counter, wrap at 8
	incf	SID_BANKSTICK_CHK_CTR, W
	andlw	0x07
	movwf	SID_BANKSTICK_CHK_CTR

	;; select "magic number" and try to read from BankStick
	rcall	SID_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	;; this sets the MIOS_BOX_STAT_BS_AVAILABLE flag

	;; save old BankStick status in TMP1
	movff	SID_BANKSTICK_STATUS, TMP1

	;; modify the available flag of current bankstick
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, SID_BANK_CheckStick_AccPassed
SID_BANK_CheckStick_AccFailed
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitANDMask
	andwf	SID_BANKSTICK_STATUS, F
	rgoto	SID_BANK_CheckStick_AccCont
SID_BANK_CheckStick_AccPassed
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_BANKSTICK_STATUS, F
SID_BANK_CheckStick_AccCont

	;; check if flag has been changed
	movf	SID_BANKSTICK_STATUS, W
	xorwf	TMP1, W
	bz	SID_BANK_CheckStick_End		; no change

	;; flag changed - branch depending on available status
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bnz	SID_BANK_CheckStick_Ext


	;; ------------------------------------------------------------------
	;; --> Internal Patch
SID_BANK_CheckStick_Int
SID_BANK_CheckStick_IntReload
	;; 	TABLE_ADDR TEXT_INTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage

	;; play tune
	movf	SID_BANKSTICK_CHK_CTR, W
	call	SID_TUNE_Play2

	;; branch depending on patch/ensemble BankStick
#if DEFAULT_ENS_BS_CS >= 0 && DEFAULT_ENS_BS_CS <= 7
	movlw	DEFAULT_ENS_BS_CS
	xorwf	SID_BANKSTICK_CHK_CTR, W
	bnz	SID_BANK_CheckStick_Int_P
SID_BANK_CheckStick_Int_E
	;; change to ensemble 0
	clrf	SID_ENSEMBLE
	;; set reinit counter - CS configuration will be restored after 1 second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR
	rgoto	SID_BANK_CheckStick_End
#endif

SID_BANK_CheckStick_Int_P
	;; change to patch 0
	movff	SID_MV1_BASE + SID_MVx_MIDI_CHANNEL, WREG
	andlw	0x0f
	iorlw	0xc0
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	SID_MIDI_ProgramChange
	rgoto	SID_BANK_CheckStick_End


	;; ------------------------------------------------------------------
	;; --> External Patch
SID_BANK_CheckStick_Ext
	;; 	TABLE_ADDR TEXT_EXTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage
	;; play tune
	movf	SID_BANKSTICK_CHK_CTR, W
	call	SID_TUNE_Play1

	;; new setup will be reloaded automatically
	;; set reinit counter - CS configuration will be restored after one second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	movlw	50				; wait some ms to get a stable status
	call	MIOS_Delay			

	;; magic numbers are different between Ensemble/Patch bankstick
#if DEFAULT_ENS_BS_CS >= 0 && DEFAULT_ENS_BS_CS <= 7
	movlw	DEFAULT_ENS_BS_CS
	xorwf	SID_BANKSTICK_CHK_CTR, W
	bnz	SID_BANK_CheckStick_Ext_P
SID_BANK_CheckStick_Ext_E
	rcall	SID_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_E_MAGIC0		; branch to unformatted message if number not equal
	bnz	SID_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_E_MAGIC1		; branch to unformatted message if number not equal
	bnz	SID_BANK_CheckStick_Unformatted

	rgoto	SID_BANK_CheckStick_Ext_E_Cont
#endif

SID_BANK_CheckStick_Ext_P
	rcall	SID_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	SID_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bnz	SID_BANK_CheckStick_Unformatted

SID_BANK_CheckStick_Ext_E_Cont
	;; set bankstick size depending on third byte (32=32k, 64=64k)
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitANDMask
	andwf	SID_BANKSTICK_SIZE, F
	call	MIOS_BANKSTICK_Read
	xorlw	64
	bnz	SID_BANK_CheckStrick_Ext_Not64k
SID_BANK_CheckStrick_Ext_64k
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_BANKSTICK_SIZE, F
SID_BANK_CheckStrick_Ext_Not64k

	;; fix patches if required (and enabled via ENABLE_PATCH_FIXING)
	movff	SID_BANKSTICK_CHK_CTR, SID_BANK
	rcall	SID_BANK_FixPatches
	;; ok
	rgoto	SID_BANK_CheckStick_End

SID_BANK_CheckStick_Unformatted

	movff	SID_BANKSTICK_CHK_CTR, SID_BANK
	rcall	SID_BANK_FormatStick		; --> format bankstick

SID_BANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;; selects address of magic number depending on SID_BANKSTICK_CHK_CTR (0x0000)
;; 0x0000
;; --------------------------------------------------------------------------
SID_BANK_CheckStick_SelectMagic
	;; select BankStick
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_BANKSTICK_CtrlSet

	;; select address of magic number
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------
;;  Format Stick: copy the lead default patch into the currently selected BankStick Bank
;;  IN: bank number in SID_BANK
;; --------------------------------------------------------------------------
TEXT_FORMATBANK_0 STRING 20, 0x00, "* Formatting xxk *  "
TEXT_FORMATBANK_1 STRING 20, 0x40, "* Patch x  0     *  "
TEXT_FORMATBANK_E STRING 20, 0x40, "* Ensemble 000   *  "

SID_BANK_FormatStick
	;; select BankStick depending on SID_BANK
	movf	SID_BANK, W
	call	MIOS_BANKSTICK_CtrlSet

	;; determine if this is a 64k BankStick:
	;; read from address 0x8000, store value in PRODL
	clrf	MIOS_PARAMETER1
	movlw	0x80
	movwf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	movwf	PRODL

	;; add 0x42 and write number to 0x0000
	addlw	0x42
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Write

	;; read again number from 0x8000, check if we still see the old value
	clrf	MIOS_PARAMETER1
	movlw	0x80
	movwf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorwf	PRODL, W
	bz	SID_BANK_FormatStick_64k
SID_BANK_FormatStick_32k
	;; clear flag in SIZE register
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitANDMask
	andwf	SID_BANKSTICK_SIZE, F
	;; write 32 to address 0x0002
	movlw	32
	rgoto	SID_BANK_FormatStick_SizeIDCont
SID_BANK_FormatStick_64k
	;; set flag in SIZE register
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_HLP_GetBitORMask
	iorwf	SID_BANKSTICK_SIZE, F
	;; write 64 to address 0x0002
	movlw	64
	;; 	rgoto	SID_BANK_FormatStick_SizeIDCont
SID_BANK_FormatStick_SizeIDCont
	movwf	PRODL
	movlw	0x02
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movf	PRODL, W
	call	MIOS_BANKSTICK_Write

	;; print format message
	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print bankstick size
	movlw	0x00 + 13
	call	MIOS_LCD_CursorSet
	movf	PRODL, W
	call	MIOS_LCD_PrintBCD2



	;; now branch depending on Ensemble/Patch
#if DEFAULT_ENS_BS_CS >= 0 && DEFAULT_ENS_BS_CS <= 7
	movlw	DEFAULT_ENS_BS_CS
	xorwf	SID_BANKSTICK_CHK_CTR, W
	bnz	SID_BANK_FormatStick_P
SID_BANK_FormatStick_E
	;; print ensemble message (TBLPTR[LHU] already correctly initialised from previous PrintMessage)
	call	MIOS_LCD_PrintMessage

	;; using SID_ENSEMBLE as loop counter, start with E1
	movlw	1
	movwf	SID_ENSEMBLE
SID_BANK_FormatStick_E_OuterLoop
	;; print ensemble number
	movlw	0x40 + 11
	call	MIOS_LCD_CursorSet
	incf	SID_ENSEMBLE, W
	call	MIOS_LCD_PrintBCD3

	;; copy 256 bytes from EEPROM to BankStick
	clrf	EEADR
	movlw	HIGH(EEPROM_ENSEMBLE)
	movwf	EEADRH		; (only relevant for internal EEPROM)
SID_BANK_FormatStick_E_InnerLoop
	clrwdt			; feed watchdog

	;; select ENS buffer (64 bytes)
	lfsr	FSR1, SID_ENS_BUFFER

	;; load page of internal EEPROM into this buffer
	call	MIOS_EEPROM_ReadPage

	;; switch back to old address
	movlw	-0x40
	addwf	EEADR, F

	;; write a 64byte page into EEPROM
	call	SID_EBANK_WritePage
	;; (will increment EEADR by 0x40)

	movf	EEADR, W	; until address 0x100 is reached
	bnz	SID_BANK_FormatStick_E_InnerLoop

	;; increment ensemble until last one (128) reached
	incf	SID_ENSEMBLE, F
	BRA_IFCLR SID_ENSEMBLE, 7, ACCESS, SID_BANK_FormatStick_E_OuterLoop


	;; write magic bytes to confirm valid content
	movff	SID_BANK, SID_BANKSTICK_CHK_CTR
	rcall	SID_BANK_CheckStick_SelectMagic
	movlw	BANKSTICK_E_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_E_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; select first external ensemble
	movlw	1
	movwf	SID_ENSEMBLE

	;; store number in EEPROM
	call	SID_ENS_StoreDefaultNum

	;; and branch to end
	rgoto	SID_BANK_FormatStickEnd

#endif
SID_BANK_FormatStick_P

	;; print bank number
	movlw	0x40 + 8
	call	MIOS_LCD_CursorSet
	movf	SID_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; load preset patch into edit buffer
	call	SID_PATCH_LoadPresetIntoEdit
	
	;; clear name
	lfsr	FSR0, SID_EDIT_BUFFER
SID_BANK_FormatStick_P_ClearLoop
	clrf	POSTINC0
	BRA_IFCLR FSR0L, 4, ACCESS, SID_BANK_FormatStick_P_ClearLoop

	;; now format patch 1 to 127 (64k) or 63 (32k)
	movlw	0x01
	movwf	SID_PATCH	; starting patch
SID_BANK_FormatStick_P_OuterLoop
	movlw	0x49
	call	MIOS_LCD_CursorSet
	incf	SID_PATCH, W
	call	MIOS_LCD_PrintBCD3

	movf	SID_PATCH, W
	call	SID_TUNE_Play3_Note

	;; copy 0x200 words
	lfsr	FSR1, SID_EDIT_BUFFER
	clrf	EEADR
	clrf	EEADRH
SID_BANK_FormatStick_P_InnerLoop
	clrwdt			; feed watchdog

	;; write a 64byte page into EEPROM
	call	SID_PBANK_WritePage

	;; increment FSR1 by 0x40
	movlw	0x40
	addwf	FSR1L, F
	movlw	0x00
	addwfc	FSR1H, F

	movf	EEADR, W	; until address 0x200 is reached
	bnz	SID_BANK_FormatStick_P_InnerLoop
	movlw	0x02-1
	cpfsgt	EEADRH, ACCESS
	rgoto SID_BANK_FormatStick_P_InnerLoop

	;; continue until last patch is reached
	incf	SID_PATCH, F

	movf	SID_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_SIZE, W
	movlw	128-1
	skpnz
	movlw	64-1
	cpfsgt	SID_PATCH, W
	rgoto SID_BANK_FormatStick_P_OuterLoop

	;; clear address 0x0003-0x01ff
	movlw	0x03
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
SID_BANK_FormatStick_P_CfgLoop
	clrwdt			; feed watchdog
	movlw	0x00
	call	MIOS_BANKSTICK_Write
	movf	MIOS_PARAMETER1, W
	bnz	SID_BANK_FormatStick_P_CfgLoop
	movlw	0x02-1
	cpfsgt	MIOS_PARAMETER2, ACCESS
	rgoto SID_BANK_FormatStick_P_CfgLoop

	;; write magic bytes to confirm valid content
	movff	SID_BANK, SID_BANKSTICK_CHK_CTR
	rcall	SID_BANK_CheckStick_SelectMagic
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; switch to internal patch
	clrf	SID_PATCH
	
	;; play tune, new setup will be reloaded automatically
	movf	SID_BANK, W
	call	SID_TUNE_Play1

SID_BANK_FormatStickEnd
	;; set reinit counter - CS configuration will be restored after 1 second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR
	return

;; --------------------------------------------------------------------------


;; --------------------------------------------------------------------------
;;  Fix Patches - temporary routine to modify all patches stored in BankStick
;;  IN: bank number in SID_BANK
;; --------------------------------------------------------------------------
TEXT_FIXBANK_0 STRING 20, 0x00, "* Fixing         *  "
TEXT_FIXBANK_1 STRING 20, 0x40, "* Patch x  0     *  "

SID_BANK_FixPatches
#if ENABLE_PATCH_FIXING
	;; select BankStick depending on SID_BANK
	movf	SID_BANK, W
	call	MIOS_BANKSTICK_CtrlSet

	;; print message
	TABLE_ADDR TEXT_FIXBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print bank number
	movlw	0x40 + 8
	call	MIOS_LCD_CursorSet
	movf	SID_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; now fixing patch 1 to 127 (64k) or 63 (32k)
	movlw	0x01
	movwf	SID_PATCH	; starting patch
SID_BANK_FixPatches_OuterLoop
	movlw	0x49
	call	MIOS_LCD_CursorSet
	incf	SID_PATCH, W
	call	MIOS_LCD_PrintBCD3

	;; fix pitchbender assignment if K#P not already assigned to other parameter
	movlw	SID_Ix_P_KP_BASE+SID_Ix_Px_ASSIGN1
	movwf	EEADR
	clrf	EEADRH
	call	SID_BANK_SetBankStickAddress
	call	MIOS_BANKSTICK_Read
	bnz	SID_BANK_FixPatches_NoPbFix
SID_BANK_FixPatches_PbFix
	decf	MIOS_PARAMETER1, F
	movlw	0x50
	call	MIOS_BANKSTICK_Write
SID_BANK_FixPatches_NoPbFix

#if 0
	movf	SID_PATCH, W
	call	SID_TUNE_Play3_Note
#endif

	;; continue until last patch is reached
	incf	SID_PATCH, F

	movf	SID_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_SIZE, W
	movlw	128-1
	skpnz
	movlw	64-1
	cpfsgt	SID_PATCH, W
	rgoto SID_BANK_FixPatches_OuterLoop

	;; switch to internal patch
	clrf	SID_PATCH

SID_BANK_FixPatchesEnd
#endif
	return

