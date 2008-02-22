; $Id$
;
; MIDIbox SID
; Bank Load/Store Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Writes data to EEPROM or BankStick
;;  IN: Value in WREG, address offset in EEADR
;;  OUT: increments address
;; --------------------------------------------------------------------------
SID_BANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_BANK_GetBankStickAvailable
	bz	SID_BANK_WriteInternal
	movf	SID_PATCH, W
	bz	SID_BANK_WriteInternal

SID_BANK_WriteBankStick				; BankStick write:
	rcall	SID_BANK_SetBankStickAddress
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
 	call	MIOS_BANKSTICK_Write		; write content
	bz	SID_BANK_Write_BS_End		; goto end if write successfull
	;; here we could do something of the bankstick was not available

SID_BANK_Write_BS_End
	incf	EEADR, F			; increment EEPROM address
	rgoto	SID_BANK_Write_End

SID_BANK_WriteInternal
	;; don't write if address in range between 0x7c and 0x7f (used to save channel and device number)
	BRA_IFSET EEADR, 7, ACCESS, SID_BANK_WriteInternal_Ok
	movlw	0x7c-1
	cpfsgt	EEADR, ACCESS
	rgoto SID_BANK_WriteInternal_Ok
	incf	EEADR, F			; increment EEADR
	rgoto	SID_BANK_Write_End		; and goto end
SID_BANK_WriteInternal_Ok
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler

SID_BANK_Write_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Value in FSR1 buffer, address offset in EEADR
;;  OUT: increments address
;; --------------------------------------------------------------------------
SID_BANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_BANK_GetBankStickAvailable
	bz	SID_BANK_WritePageInternal
	movf	SID_PATCH, W
	bz	SID_BANK_WritePageInternal

SID_BANK_WritePageBankStick			; BankStick write:
	rcall	SID_BANK_SetBankStickAddress
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	bz	SID_BANK_Write_BSP_End		; goto end if write successfull
	;; here we could do something of the bankstick was not available

SID_BANK_Write_BSP_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	SID_BANK_WritePage_End

SID_BANK_WritePageInternal
	;; write 64 bytes
SID_BANK_WritePageInternalLoop
	;; don't write if address in range between 0x7c and 0x7f (used to save channel and device number)
	BRA_IFSET EEADR, 7, ACCESS, SID_BANK_WritePageInternal_Ok
	movlw	0x7c-1
	cpfsgt	EEADR, ACCESS
	rgoto SID_BANK_WritePageInternal_Ok
	incf	EEADR, F			; increment EEADR
	rgoto	SID_BANK_WritePage_Next		; and goto next
SID_BANK_WritePageInternal_Ok
	movf	EEADR, W
	andlw	0x3f
	movf	PLUSW1, W			; get byte from buffer
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
SID_BANK_WritePage_Next
	movf	EEADR, W
	andlw	0x3f
	bnz	SID_BANK_WritePageInternalLoop

SID_BANK_WritePage_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR
;;  OUT: 7-bit result in WREG, increments address
;; --------------------------------------------------------------------------
SID_BANK_Read
	;; branch to internal patch if bankstick is not available or Patch is 0
	rcall	SID_BANK_GetBankStickAvailable
	bz	SID_BANK_ReadInternal
	movf	SID_PATCH, W
	bz	SID_BANK_ReadInternal

SID_BANK_ReadBankStick				; BankStick read:
	rcall	SID_BANK_SetBankStickAddress
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	rgoto	SID_BANK_Read_End
	
SID_BANK_ReadInternal
	call	MIOS_EEPROM_Read

SID_BANK_Read_End
	SET_BSR	SID_BASE			; fix BSR after a MIOS routine has been called
	return


	
;; --------------------------------------------------------------------------
;;  This function returns the bankstick available status for the current bank
;;  IN: BankStick number in SID_BANK
;;  OUT: WREG != 0 when bankstick available
;; --------------------------------------------------------------------------
SID_BANK_GetBankStickAvailable
	movf	SID_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	return

;; --------------------------------------------------------------------------
;;  This function sets a BankStick address
;;  IN: patch offset in EEADR
;;      patch number in SID_PATCH
;;      BankStick number in SID_BANK
;;  OUT: address in MIOS_PARAMETER[12]
;;       BankStick selected via MIOS_BANKSTICK_CtrlSet
;; --------------------------------------------------------------------------
SID_BANK_SetBankStickAddress
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movff	SID_PATCH, MIOS_PARAMETER2	; copy patch to high-byte
#if DEFAULT_BS_TYPE == 1	; (64k device)
	btfsc	SID_BANK, 0
	bsf	MIOS_PARAMETER2, 7		; select upper address range on odd bank number
	rrf	SID_BANK, W			; select BankStick 0-3
	andlw	0x03
#else				; (32k device)
	movf	SID_BANK, W			; select BankStick 0-7
	andlw	0x07
#endif
	iorlw	0x80	; (disable verify for faster write accesses)
	goto	MIOS_BANKSTICK_CtrlSet


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

	;; --> Internal Patch
SID_BANK_CheckStick_Int
SID_BANK_CheckStick_IntReload
	;; 	TABLE_ADDR TEXT_INTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage

	;; play tune and change to patch 0
	call	SID_TUNE_Play2
	movff	SID_V1_BASE + SID_Vx_MIDI_CHANNEL, WREG
	andlw	0x0f
	iorlw	0xc0
	movwf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	SID_MIDI_ProgramChange
	rgoto	SID_BANK_CheckStick_End

	;; --> External Patch
SID_BANK_CheckStick_Ext
	;; 	TABLE_ADDR TEXT_EXTBANK_0		; print message
	;; 	call	MIOS_LCD_PrintMessage
	;; 	call	MIOS_LCD_PrintMessage
	;; play tune, new setup will be reloaded automatically
	call	SID_TUNE_Play1
#if CS_ENABLED == 1
	;; set reinit counter - CS configuration will be restored after one second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR
#endif

SID_BANK_CheckStrick_Ext_NoMsg
	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	movlw	50				; wait some ms to get a stable status
	call	MIOS_Delay			

	rcall	SID_BANK_CheckStick_SelectMagic
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	SID_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bz	SID_BANK_CheckStick_End
SID_BANK_CheckStick_Unformatted

	movff	SID_BANKSTICK_CHK_CTR, SID_BANK
	rcall	SID_BANK_FormatStick		; --> format bankstick

SID_BANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;; selects address of magic number depending on SID_BANKSTICK_CHK_CTR
;; 0x0000 on 32k EEPROM
;; 0x0000 or 0x8000 on 64k EEPROM, SID_BANKSTICK_CHK_CTR.0 selects the lower/upper half
;; --------------------------------------------------------------------------
SID_BANK_CheckStick_SelectMagic
#if DEFAULT_BS_TYPE == 1	; (64k device)
	;; select BankStick
	rrf	SID_BANKSTICK_CHK_CTR, W	; (we've only up to 4 EEPROMs connected)
	andlw	0x03
	call	MIOS_BANKSTICK_CtrlSet

	;; try to read from BankStick (address 0x0000 or 0x8000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	btfsc	SID_BANKSTICK_CHK_CTR, 0
	bsf	MIOS_PARAMETER2, 7

#else				; (32k device)
	;; select BankStick
	movf	SID_BANKSTICK_CHK_CTR, W
	call	MIOS_BANKSTICK_CtrlSet

	;; try to read from BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
#endif
	return

;; --------------------------------------------------------------------------
;;  Format Stick: copy the internal patch into the currently selected BankStick Bank
;; --------------------------------------------------------------------------
SID_BANK_FormatStick
	;; print format message
	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print bank number
	movlw	0x48
	call	MIOS_LCD_CursorSet
	movf	SID_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; copy Patch 0 (internal patch) to upload buffer
	clrf	SID_PATCH
	clrf	EEADR
	lfsr	FSR0, BANKSTICK_FORMAT_BEGIN
SID_BANK_FormatStick_CIntLoop
	rcall	SID_BANK_Read
	movwf	POSTINC0
	movf	EEADR, W	; EEADR incremented by SID_BANK_Read
	bnz	SID_BANK_FormatStick_CIntLoop

	;; clear name
	lfsr	FSR0, BANKSTICK_FORMAT_BEGIN
SID_BANK_FormatStick_ClearNLoop
	clrf	POSTINC0
	BRA_IFCLR FSR0L, 4, ACCESS, SID_BANK_FormatStick_ClearNLoop

	;; now format patch 1 to 127
	movlw	0x01
	movwf	SID_PATCH
SID_BANK_FormatStick_OuterLoop
	movlw	0x49
	call	MIOS_LCD_CursorSet
	incf	SID_PATCH, W
	call	MIOS_LCD_PrintBCD3

	movf	SID_PATCH, W
	call	SID_TUNE_Play3_Note

	;; copy 0x100 words
	lfsr	FSR1, BANKSTICK_FORMAT_BEGIN	; init pointer to upload buffer
	clrf	EEADR
SID_BANK_FormatStick_InnerLoop
	clrwdt			; feed watchdog

	call	SID_BANK_WritePage ; write a 64 bytes page to EEPROM
	;; increment FSR1 by 0x40
	movlw	0x40
	addwf	FSR1L, F

	movf	EEADR, W	; until EEADR == 0
	bnz	SID_BANK_FormatStick_InnerLoop

	incf	SID_PATCH, F
	GOTO_IFCLR SID_PATCH, 7, ACCESS, SID_BANK_FormatStick_OuterLoop

	;; clear address 0x0000-0x00ff (or 0x8000-0x80ff)
	clrf	EEADR
SID_BANK_FormatStick_CfgLoop
	rcall	SID_BANK_CheckStick_SelectMagic	; get base address (0x0000 or 0x8000)
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movlw	0x00
	call	MIOS_BANKSTICK_Write		; write 0
	incf	EEADR, F
	bnz	SID_BANK_FormatStick_CfgLoop

	;; write magic bytes to confirm valid content
	rcall	SID_BANK_CheckStick_SelectMagic
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; switch to internal patch
	clrf	SID_PATCH
	
	;; play tune, new setup will be reloaded automatically
	call	SID_TUNE_Play1

SID_BANK_FormatStickEnd
#if CS_ENABLED == 1
	;; set reinit counter - CS configuration will be restored after 1 second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR
#endif
	return

;; --------------------------------------------------------------------------

TEXT_FORMATBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATBANK_1 STRING 16, 0x40, "* Patch x  0    "

