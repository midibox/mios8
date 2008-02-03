; $Id$
;
; MIDIboxSEQ
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
;;  calculates the bankstick address:
;;     (SEQ_PATTERN << 8) + EEADR
;;  IN: SEQ_SONG, SEQ_SBANK_SECTOR, EEADR
;;  OUT: bankstick address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SEQ_BANK_CalcAddress
#if DEFAULT_BANKSTICK_PATTERN_CS >= 0 && DEFAULT_BANKSTICK_PATTERN_CS <= 7
	;; select BankStick for patterns
	movlw	DEFAULT_BANKSTICK_PATTERN_CS
	call	MIOS_BANKSTICK_CtrlSet	
#endif
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movff	SEQ_PATTERN, MIOS_PARAMETER2	; copy bank to high-byte
	return


;; --------------------------------------------------------------------------
;;  Writes data to EEPROM or BankStick
;;  IN: Value in WREG, address offset in EEADR
;;  OUT: increments address
;; --------------------------------------------------------------------------
SEQ_BANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; branch to internal bank if bankstick is not available or pattern is 0
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_WriteInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	bz	SEQ_BANK_WriteInternal

SEQ_BANK_WriteBankStick				; BankStick write:
	rcall	SEQ_BANK_CalcAddress		; determine address
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_BANKSTICK_Write		; write content
	bz	SEQ_BANK_Write_BS_End		; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

SEQ_BANK_Write_BS_End
	incf	EEADR, F			; increment EEPROM address
	rgoto	SEQ_BANK_Write_End

SEQ_BANK_WriteInternal
	;; don't write if address in range between 0xfc and 0xff (used to save the global settings)
	movlw	0xfc-1
	cpfsgt	EEADR, ACCESS
	rgoto SEQ_BANK_WriteInternal_Ok
	incf	EEADR, F			; increment EEADR
	rgoto	SEQ_BANK_Write_End		; and goto end
SEQ_BANK_WriteInternal_Ok
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
SEQ_BANK_WriteInternal_NOk

SEQ_BANK_Write_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Value in FSR1 buffer, address offset in EEADR
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_BANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal bank if bankstick is not available or pattern is 0
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_WritePageInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	bz	SEQ_BANK_WritePageInternal

SEQ_BANK_WritePageBankStick			; BankStick write:
	rcall	SEQ_BANK_CalcAddress		; determine address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	bz	SEQ_BANK_Write_BSP_End		; goto end if write successfull
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK; else disable BankStick
	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

SEQ_BANK_Write_BSP_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	SEQ_BANK_WritePage_End

SEQ_BANK_WritePageInternal
	;; write 64 bytes
SEQ_BANK_WritePageInternalLoop
	;; don't write if address in range between 0xfc and 0xff (used to save the global settigs)
	movlw	0xfc-1
	cpfsgt	EEADR, ACCESS
	rgoto SEQ_BANK_WritePageInternal_Ok
	incf	EEADR, F			; increment EEADR
	rgoto	SEQ_BANK_WritePage_Next		; and goto next
SEQ_BANK_WritePageInternal_Ok
	movf	EEADR, W
	andlw	0x3f
	movf	PLUSW1, W			; get byte from buffer
	call	MIOS_EEPROM_Write		; write to EEPROM
	;; here we could add an error exception handler
SEQ_BANK_WritePage_Next
	movf	EEADR, W
	andlw	0x3f
	bnz	SEQ_BANK_WritePageInternalLoop

SEQ_BANK_WritePage_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
SEQ_BANK_Read
	;; branch to internal bank if bankstick is not available or pattern is 0
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_ReadInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	bz	SEQ_BANK_ReadInternal

SEQ_BANK_ReadBankStick				; BankStick read:
	rcall	SEQ_BANK_CalcAddress		; determine address
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	rgoto	SEQ_BANK_Read_End
	
SEQ_BANK_ReadInternal
	call	MIOS_EEPROM_Read

SEQ_BANK_Read_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Reads 64 bytes from EEPROM or BankStick
;;  IN: pointer to 64 bytes buffer in FSR1, address in EEADR
;;  OUT: result in FSR1 buffer, increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_BANK_ReadPage
	;; align address
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal bank if bankstick is not available or pattern is 0
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_ReadPageInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	bz	SEQ_BANK_ReadPageInternal

SEQ_BANK_ReadPageBankStick			; BankStick read:
	rcall	SEQ_BANK_CalcAddress		; determine address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_ReadPage		; read page
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	rgoto	SEQ_BANK_ReadPage_End

SEQ_BANK_ReadPageInternal
	;; read 64 bytes
	call	MIOS_EEPROM_ReadPage

SEQ_BANK_ReadPage_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear MB_STAT.MB_STAT_USE_BANKSTICK
;;  if BankStick not available
;; --------------------------------------------------------------------------
SEQ_BANK_CheckStick
#if DEFAULT_BANKSTICK_PATTERN_CS >= 0 && DEFAULT_BANKSTICK_PATTERN_CS <= 7
	;; select BankStick for patterns
	movlw	DEFAULT_BANKSTICK_PATTERN_CS
	call	MIOS_BANKSTICK_CtrlSet	
#else
	;; exit if no valid BankStick address has been specified
	return
#endif
	
	;; save BankStick status for later in TMP1
	movff	MB_STAT, TMP1

	;; try to read from BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read

	;; save current status of USE_BANKSTICK again - we use it to determine changes
	movff	MB_STAT, TMP2

	;; set the USE_BANKSTICK and BANKSTICK_AVAILABLE flag if EEPROM responded
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK

	bcf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_BANKSTICK_AVAILABLE

	;; check if flag has been changed
	movf	MB_STAT, W			; get new status
	xorwf	TMP2, W				; compare with status last saved in TMP2
	bz	SEQ_BANK_CheckStick_End		; no change

	;; flag changed!
	GOTO_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_CheckStick_Ext
	
	;; --> Internal Pattern
SEQ_BANK_CheckStick_Int
SEQ_BANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	rgoto	SEQ_BANK_CheckStick_Reload		; branch to reload routine

	;; --> External Pattern
SEQ_BANK_CheckStick_Ext
	;; don't print a message if bankstick already was active before checking (status saved in TMP1)
	BRA_IFSET TMP1, MB_STAT_BANKSTICK_AVAILABLE, ACCESS, SEQ_BANK_CheckStrick_Ext_NoMsg
	TABLE_ADDR TEXT_EXTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	movlw	255				; wait some ms to get a stable status
	call	MIOS_Delay			
SEQ_BANK_CheckStrick_Ext_NoMsg

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	clrf	MIOS_PARAMETER1			; read first byte from BankStick
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MAGIC0		; branch to unformatted message if number not equal
	bnz	SEQ_BANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MAGIC1		; branch to unformatted message if number not equal
	bz	SEQ_BANK_CheckStick_Reload
SEQ_BANK_CheckStick_Unformatted

	;; format BankStick immediately
	rgoto	SEQ_BANK_FormatStickNow

SEQ_BANK_CheckStick_Reload
SEQ_BANK_FormatStick_Reload
	;; restore dump
	call	SEQ_DUMP_RestoreAll
	;; request re-init of page so that the new bankname will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

SEQ_BANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;;  Format Stick: copy the internal bank into the currently selected BankStick Bank
;; --------------------------------------------------------------------------
SEQ_BANK_FormatStick
#if DEFAULT_BANKSTICK_PATTERN_CS >= 0 && DEFAULT_BANKSTICK_PATTERN_CS <= 7
	;; select BankStick for patterns
	movlw	DEFAULT_BANKSTICK_PATTERN_CS
	call	MIOS_BANKSTICK_CtrlSet	
#endif

	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_FormatStickNow

	TABLE_ADDR TEXT_FORMATNOT_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage

SEQ_BANK_FormatStickNow
	;; clear pattern 1-128
	movlw	0x01
	movff	WREG, SEQ_PATTERN
SEQ_BANK_FormatStickCopyLoop
	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print pattern number
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movff	SEQ_PATTERN, WREG
	call	SEQ_LCD_PrnPattern

	;; copy internal pattern to external pattern
	movff	SEQ_PATTERN, WREG
	rcall	SEQ_BANK_CopyFromInternal

	;; increment pattern number until last is reached
	SET_BSR	SEQ_BASE
	incf	SEQ_PATTERN, F, BANKED
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_BANK_FormatStickCopyLoop

	;; write magic bytes to address 0x0000
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
	call	MIOS_BANKSTICK_Write
	movlw	32		; 32k indicator
	call	MIOS_BANKSTICK_Write

	;; clear addresses 0x0002-0x00ff of bankstick
	;; (this area is overlayed by internal bank)
SEQ_BANK_FormatStickEraseLoop
	movlw	0x00
	call	MIOS_BANKSTICK_Write
	movf	MIOS_PARAMETER2, W
	bnz	SEQ_BANK_FormatStickEraseLoop

	;; switch to internal bank
	SET_BSR	SEQ_BASE
	clrf	SEQ_PATTERN, BANKED

	;; reload bank
	rgoto	SEQ_BANK_FormatStick_Reload


;; --------------------------------------------------------------------------
;;  Copy function
;;  IN: source bank in SEQ_PATTERN
;;      target bank in WREG
;;      TMP3[0] == 0: copy from internal (-> use SEQ_BANK_CopyFromInternal)
;;      TMP3[0] == 1: copy from external (-> use SEQ_BANK_CopyFromExternal)
;;  USES: TMP1
;; --------------------------------------------------------------------------
SEQ_BANK_CopyFromInternal
	bcf	TMP3, 0		; copy from internal: TMP3[0] == cleared
	rgoto	SEQ_BANK_Copy

SEQ_BANK_CopyFromExternal
	bsf	TMP3, 0		; copy from external: TMP3[0] == set
	;; 	rgoto	SEQ_BANK_Copy

SEQ_BANK_Copy
	;; store current bank in TMP1
	movff	SEQ_PATTERN, TMP1
	;; store target bank in TMP2
	movwf	TMP2

	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, SEQ_BANK_CopyNow

	TABLE_ADDR TEXT_COPYNOT_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage

SEQ_BANK_CopyNow
	;; copy 4*0x40 bytes
	clrf	EEADR
SEQ_BANK_CopyLoop
	clrwdt			; feed watchdog

	;; read page from source bank
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK
	btfss	TMP3, 0
	bcf	MB_STAT, MB_STAT_USE_BANKSTICK
	movff	TMP1, SEQ_PATTERN
	lfsr	FSR1, SEQ_DATA_BUFFER
	rcall	SEQ_BANK_ReadPage

	;; fixing address
	movlw	-0x40
	addwf	EEADR, F

	;; write to target bank
	bsf	MB_STAT, MB_STAT_USE_BANKSTICK
	movff	TMP2, SEQ_PATTERN
	lfsr	FSR1, SEQ_DATA_BUFFER
	rcall	SEQ_BANK_WritePage

	;; loop until last byte reached
	movf	EEADR, W
	bnz	SEQ_BANK_CopyLoop

	;; change to old bank
	movff	TMP1, SEQ_PATTERN

	;; and exit
	return


;; --------------------------------------------------------------------------

TEXT_EXTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_EXTBANK_1   STRING 16, 0x40, "* from BankStick"

TEXT_INTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_INTBANK_1   STRING 16, 0x40, "* from Internal "
	
TEXT_FORMATBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATBANK_1 STRING 16, 0x40, "* Pattern xx    "

TEXT_FORMATNOT_0  STRING 16, 0x00, "* Internal Bank "
TEXT_FORMATNOT_1  STRING 16, 0x40, "* not formatable"

TEXT_COPYNOT_0  STRING 16, 0x00, "* Only internal "
TEXT_COPYNOT_1  STRING 16, 0x40, "* bank availabl."
