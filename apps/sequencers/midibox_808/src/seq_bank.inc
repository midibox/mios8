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

;; BankStick allocation table - each BankStick has an own entry; values:
;     0: if no BankStick connected which listens to the CS address
;     1: if a 32k BankStick (24LC256) is connected and should be used as Pattern Storage
;     2: if a 64k BankStick (24LC512) is connected and should be used as Pattern Storage
;     3: if a 32k or 64k BankStick is connected and should be used as Song Storage
;        only the first 32k are used, so there is no advantage on using a 24LC512 here.

SEQ_BANK_TABLE
; the #define's are located in main.asm and setup_*.asm --- please don't touch the definitions here!
	db	DEFAULT_BANKSTICK_MODE_CS0, DEFAULT_BANKSTICK_MODE_CS1
	db	DEFAULT_BANKSTICK_MODE_CS2, DEFAULT_BANKSTICK_MODE_CS3
	db	DEFAULT_BANKSTICK_MODE_CS4, DEFAULT_BANKSTICK_MODE_CS5
	db	DEFAULT_BANKSTICK_MODE_CS6, DEFAULT_BANKSTICK_MODE_CS7

SEQ_BANK_TABLE_TYPE	; for the formatting menu to print the type (2 characters)
	;;        0     1     2     3     4     5
	db	"--", "Pt", "Pt", "Sg", "Sg", "Mx"
SEQ_BANK_TABLE_SIZE	; for the formatting menu to print the BS size
	db	  0,   32,   64,   32,   64,   32

;; --------------------------------------------------------------------------
;;  calculates the bankstick address:
;;     (SEQ_PATTERN << 9) + (EEADRH << 8) + EEADR 
;;  IN: SEQ_PATTERN, SEQ_PATTERN_BANK, EEADR[H]
;;  OUT: bankstick address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SEQ_BANK_CalcAddress
	SET_BSR	SEQ_BASE

	;; select BankStick for patterns
	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_BANKSTICK_CtrlSet

	;; select address
	SET_BSR	SEQ_BASE
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	rlf	SEQ_PATTERN, W			; calculate high-byte
	andlw	0xfe
	movwf	MIOS_PARAMETER2
	btfsc	EEADRH, 0
	bsf	MIOS_PARAMETER2, 0
	return

;; --------------------------------------------------------------------------
;;  This function returns the bankstick available status for the current bank
;;  IN: BankStick number in SEQ_PATTERN_BANK
;;  OUT: WREG != 0 when bankstick available
;;  SEQ_PATTERN set to <64 when 32k BankStick connected
;; --------------------------------------------------------------------------
SEQ_BANK_GetBankStickAvailable
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_SIZE, W, BANKED
	skpnz
	bcf	SEQ_PATTERN, 6, BANKED

	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	return

;; --------------------------------------------------------------------------
;;  Writes data to EEPROM or BankStick
;;  IN: Value in WREG, address offset in EEADR[H]
;;  OUT: increments address
;; --------------------------------------------------------------------------
SEQ_BANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; branch to internal bank if bankstick is not available or pattern is 0
	rcall	SEQ_BANK_GetBankStickAvailable
	bz	SEQ_BANK_WriteInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bz	SEQ_BANK_WriteInternal

SEQ_BANK_WriteBankStick				; BankStick write:
	rcall	SEQ_BANK_CalcAddress		; determine address
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_BANKSTICK_Write		; write content
	bz	SEQ_BANK_Write_BS_End		; goto end if write successfull
	;; here we could do something of the bankstick was not available

SEQ_BANK_Write_BS_End
	incf	EEADR, F
	skpnz
	incf	EEADRH, F
	rgoto	SEQ_BANK_Write_End

SEQ_BANK_WriteInternal
	movf	MIOS_PARAMETER3, W		; get byte from temp. register
	call	MIOS_EEPROM_Write		; write to EEPROM
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SEQ_BANK_Write_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to EEPROM or BankStick
;;  IN: Value in FSR1 buffer, address offset in EEADR[H]
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_BANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal bank if bankstick is not available or pattern is 0
	rcall	SEQ_BANK_GetBankStickAvailable
	bz	SEQ_BANK_WritePageInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bz	SEQ_BANK_WritePageInternal

SEQ_BANK_WritePageBankStick			; BankStick write:
	rcall	SEQ_BANK_CalcAddress		; determine address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage	; write page
	bz	SEQ_BANK_Write_BSP_End		; goto end if write successfull
	;; here we could do something of the bankstick was not available

SEQ_BANK_Write_BSP_End
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	movlw	0
	addwfc	EEADRH, F
	rgoto	SEQ_BANK_WritePage_End

SEQ_BANK_WritePageInternal
	;; write 64 bytes
	call	MIOS_EEPROM_WritePage		; write to EEPROM
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F
	;; here we could add an error exception handler

SEQ_BANK_WritePage_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Read data from EEPROM or BankStick
;;  IN: address offset in EEADR[H]
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
SEQ_BANK_Read
	;; branch to internal bank if bankstick is not available or pattern is 0
	rcall	SEQ_BANK_GetBankStickAvailable
	bz	SEQ_BANK_ReadInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bz	SEQ_BANK_ReadInternal

SEQ_BANK_ReadBankStick				; BankStick read:
	rcall	SEQ_BANK_CalcAddress		; determine address
	call	MIOS_BANKSTICK_Read		; read content
	incf	EEADR, F			; increment EEPROM address
	skpnz
	incf	EEADRH, F
	rgoto	SEQ_BANK_Read_End
	
SEQ_BANK_ReadInternal
	call	MIOS_EEPROM_Read
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SEQ_BANK_Read_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Reads 64 bytes from EEPROM or BankStick
;;  IN: pointer to 64 bytes buffer in FSR1, address in EEADR[H]
;;  OUT: result in FSR1 buffer, increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_BANK_ReadPage
	;; align address
	movlw	0xc0
	andwf	EEADR, F

	;; branch to internal bank if bankstick is not available or pattern is 0
	rcall	SEQ_BANK_GetBankStickAvailable
	bz	SEQ_BANK_ReadPageInternal
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN, W, BANKED
	andlw	0x7f
	bz	SEQ_BANK_ReadPageInternal

SEQ_BANK_ReadPageBankStick			; BankStick read:
	rcall	SEQ_BANK_CalcAddress		; determine address
	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_ReadPage		; read page
	movlw	0x40				; increment EEPROM address by 0x40
	addwf	EEADR, F
	movlw	0
	addwfc	EEADRH, F
	rgoto	SEQ_BANK_ReadPage_End

SEQ_BANK_ReadPageInternal
	;; read 64 bytes
	call	MIOS_EEPROM_ReadPage
	movf	EEADR, F			; MIOS_EEPROM_* functions don't auto-increment the high byte
	skpnz
	incf	EEADRH, F

SEQ_BANK_ReadPage_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear the appr. flag in
;;  SEQ_BANKSTICK_STATUS if BankStick not available
;; --------------------------------------------------------------------------
SEQ_BANK_CheckStick
	SET_BSR	SEQ_BASE

	;; increment check counter, wrap at 8
	incf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	andlw	0x07
	movwf	SEQ_BANKSTICK_CHK_CTR, BANKED

	;; load table entry
	rcall	SEQ_BANK_GetType

	;; exit if value is neither 1 (32k BankStick) nor 2 (64k BankStick) nor 3 (Song BankStick) is selected
	addlw	-1
	bz	SEQ_BANK_CheckStick_32kOk
	addlw	-1
	bz	SEQ_BANK_CheckStick_64kOk
	addlw	-1
	skpnz
	goto	SEQ_SBANK_CheckStick ; Song 32k
	addlw	-1
	skpnz
	goto	SEQ_SBANK_CheckStick ; Song 64k
	addlw	-1
	skpnz
	goto	SEQ_MBANK_CheckStick
	rgoto	SEQ_BANK_CheckStick_End

SEQ_BANK_CheckStick_32kOk
	;; if BankStick not already found...
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_BANK_CheckStick_Cont
	;; set size = 0 (32k)
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	SEQ_BANKSTICK_SIZE, F, BANKED
	rgoto	SEQ_BANK_CheckStick_Cont
SEQ_BANK_CheckStick_64kOk
	;; if BankStick not already found...
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_BANK_CheckStick_Cont
	;; set size = 1 (64k)
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_BANKSTICK_SIZE, F, BANKED
	;; 	rgoto	SEQ_BANK_CheckStick_Cont
SEQ_BANK_CheckStick_Cont

	;; select BankStick for patterns
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_BANKSTICK_CtrlSet

	;; save old BankStick status in TMP1
	movff	SEQ_BANKSTICK_STATUS, TMP1

	;; try to read from BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	;; this sets the MIOS_BOX_STAT_BS_AVAILABLE flag

	;; modify the available flag of current bankstick
	SET_BSR	SEQ_BASE
	BRA_IFSET MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE, ACCESS, SEQ_BANK_CheckStick_AccPassed
SEQ_BANK_CheckStick_AccFailed
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	SEQ_BANKSTICK_STATUS, F, BANKED
	rgoto	SEQ_BANK_CheckStick_AccCont
SEQ_BANK_CheckStick_AccPassed
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_BANKSTICK_STATUS, F, BANKED
SEQ_BANK_CheckStick_AccCont

	;; check if flag has been changed
	movf	SEQ_BANKSTICK_STATUS, W, BANKED
	xorwf	TMP1, W
	bz	SEQ_BANK_CheckStick_End		; no change

	;; flag changed - branch depending on available status
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_BANK_CheckStick_Ext

	;; --> Internal Pattern
SEQ_BANK_CheckStick_Int
SEQ_BANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	rgoto	SEQ_BANK_CheckStick_Reload		; branch to reload routine

	;; --> External Pattern
SEQ_BANK_CheckStick_Ext
	TABLE_ADDR TEXT_EXTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print BankStick number and Device memory
	movlw	0x00 + 21
	call	MIOS_LCD_CursorSet
	movff	SEQ_BANKSTICK_CHK_CTR, WREG
	call	MIOS_LCD_PrintBCD1

	movlw	0x40 + 13
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_SIZE, W, BANKED
	bz	SEQ_BANK_CheckStrick_Ext_32k
SEQ_BANK_CheckStrick_Ext_64k
	movlw	64
	rgoto	SEQ_BANK_CheckStrick_Ext_Cont
SEQ_BANK_CheckStrick_Ext_32k
	movlw	32
	;; 	rgoto	SEQ_BANK_CheckStrick_Ext_Cont
SEQ_BANK_CheckStrick_Ext_Cont
	call	MIOS_LCD_PrintBCD2

	movlw	50				; wait some ms to get a stable status
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

	;; format BankStick
	movff	SEQ_BANKSTICK_CHK_CTR, SEQ_PATTERN_BANK
	rgoto	SEQ_BANK_FormatStick

SEQ_BANK_CheckStick_Reload
SEQ_BANK_FormatStick_Reload
	;; re-init sequencer
	call	SEQ_CORE_Init

	;; change message delay counter
	movlw	100
	call	MIOS_LCD_MessageStart

SEQ_BANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;;  Format Stick: copy the internal bank into the currently selected BankStick Bank
;;  expecting BankStick address in SEQ_PATTERN_BANK
;; --------------------------------------------------------------------------
SEQ_BANK_FormatStick
	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_BANKSTICK_CtrlSet	

	;; clear pattern 1-63 (64k BankStick: 1-127)
	movlw	0x01
	movff	WREG, SEQ_PATTERN
SEQ_BANK_FormatStickCopyLoop

	TABLE_ADDR TEXT_FORMATBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print bank and pattern number
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movff	SEQ_PATTERN_BANK, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	':'
	call	MIOS_LCD_PrintChar
	movff	SEQ_PATTERN, WREG
	call	SEQ_LCD_PrnPattern

	;; copy internal pattern to external pattern
	movff	SEQ_PATTERN, WREG
	rcall	SEQ_BANK_CopyFromInternal

	;; increment pattern number until last is reached
	SET_BSR	SEQ_BASE
	incf	SEQ_PATTERN, F, BANKED

	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_SIZE, W, BANKED
	bz	SEQ_BANK_FormatStickCopyLoopN32k
SEQ_BANK_FormatStickCopyLoopN64k
	BRA_IFCLR SEQ_PATTERN, 7, BANKED, SEQ_BANK_FormatStickCopyLoop
	rgoto	SEQ_BANK_FormatStickCopyLoopEnd
SEQ_BANK_FormatStickCopyLoopN32k
	BRA_IFCLR SEQ_PATTERN, 6, BANKED, SEQ_BANK_FormatStickCopyLoop
SEQ_BANK_FormatStickCopyLoopEnd

	;; write magic bytes to address 0x0000
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	BANKSTICK_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MAGIC1
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
;;  Modified version: changes MIDI channels and mutes depending on SEQ_PATTERN
;;  to improve default setup of a freshly formatted bankstick
;;  IN: source pattern in SEQ_PATTERN/SEQ_PATTERN_BANK
;;      target pattern in WREG
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
	;; store current pattern in TMP1
	movff	SEQ_PATTERN, TMP1
	;; store target pattern in TMP2
	movwf	TMP2

	;; check if selected BankStick available
	SET_BSR	SEQ_BASE
	movf	SEQ_PATTERN_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	SEQ_BANK_CopyNow

	TABLE_ADDR TEXT_COPYNOT_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage

SEQ_BANK_CopyNow
	;; copy 8*0x40 bytes
	clrf	TMP5		; used as loop counter
SEQ_BANK_CopyLoop
	clrwdt			; feed watchdog

	;; calculate block address
	movf	TMP5, W
	mullw	0x40
	movff	PRODL, EEADR
	movff	PRODH, EEADRH

	;; read page from source pattern
	SET_BSR	SEQ_BASE
	movf	TMP1, W
	btfss	TMP3, 0; internal pattern selected
	movlw 0x00 
	movwf	SEQ_PATTERN, BANKED
	lfsr	FSR1, SEQ_DATA_BUFFER
	rcall	SEQ_BANK_ReadPage

	;; get back last address
	movf	TMP5, W
	mullw	0x40
	movff	PRODL, EEADR
	movff	PRODH, EEADRH

#if 1
	;; modification: change MIDI channels and mutes depending on target pattern number (stored in TMP2)
	;; modification done in range 0x180-0x1ff
	BRA_IFCLR EEADRH, 0, ACCESS, SEQ_BANK_Copy_G_Skip
	BRA_IFCLR EEADR, 7, ACCESS, SEQ_BANK_Copy_G_Skip

	BRA_IFSET TMP2, 5, ACCESS, SEQ_BANK_Copy_G23
SEQ_BANK_Copy_G01
	BRA_IFSET TMP2, 4, ACCESS, SEQ_BANK_Copy_G1
SEQ_BANK_Copy_G0
	;; no modification
	rgoto	SEQ_BANK_Copy_G_Skip

SEQ_BANK_Copy_G1
	;; copy channel #5-#8 into data buffer offset SEQ_TRKCHNx
	SET_BSR SEQ_DATA_BUFFER
	movlw	4
	btfsc	EEADR, 6
	movlw 6
	movwf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED
	movlw	5
	btfsc	EEADR, 6
	movlw 7
	movwf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED

	rgoto	SEQ_BANK_Copy_G_Cont

SEQ_BANK_Copy_G23
	BRA_IFSET TMP2, 4, ACCESS, SEQ_BANK_Copy_G3
SEQ_BANK_Copy_G2
	;; copy channel #9-#12 into data buffer offset SEQ_TRKCHNx
	SET_BSR SEQ_DATA_BUFFER
	movlw	8
	btfsc	EEADR, 6
	movlw 10
	movwf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED
	movlw	9
	btfsc	EEADR, 6
	movlw 11
	movwf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED

	rgoto	SEQ_BANK_Copy_G_Cont

SEQ_BANK_Copy_G3
	;; copy channel #13-#16 into data buffer offset SEQ_TRKCHNx
	SET_BSR SEQ_DATA_BUFFER
	movlw	12
	btfsc	EEADR, 6
	movlw 14
	movwf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED
	movlw	13
	btfsc	EEADR, 6
	movlw 15
	movwf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKCHNx, BANKED

	rgoto	SEQ_BANK_Copy_G_Cont

SEQ_BANK_Copy_G_Cont
	;; mute all steps
	clrf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_0x, BANKED
	clrf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_1x, BANKED
	clrf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_2x, BANKED
	clrf	SEQ_DATA_BUFFER + 0 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_3x, BANKED

	clrf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_0x, BANKED
	clrf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_1x, BANKED
	clrf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_2x, BANKED
	clrf	SEQ_DATA_BUFFER + 1 * SEQ_TRKRECORD_LENGTH + SEQ_TRKTRGA_3x, BANKED

SEQ_BANK_Copy_G_Skip
	SET_BSR	SEQ_BASE
#endif

	;; write to target pattern
	movff	TMP2, SEQ_PATTERN
	lfsr	FSR1, SEQ_DATA_BUFFER
	rcall	SEQ_BANK_WritePage

	;; loop until last block reached
	incf	TMP5, F
	movlw	8-1
	cpfsgt	TMP5, ACCESS
	rgoto SEQ_BANK_CopyLoop
		

	;; change to old pattern
	movff	TMP1, SEQ_PATTERN

	;; and exit
	return


;; --------------------------------------------------------------------------
;;  Returns the BankStick Type
;;  IN: BS number
;;  OUT: BS type, coded like specified in setup_*.asm
;; --------------------------------------------------------------------------
SEQ_BANK_GetType
	addlw	LOW(SEQ_BANK_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(SEQ_BANK_TABLE)
	addwfc	TBLPTRH, F
	tblrd*
	movf	TABLAT, W
	return


;; --------------------------------------------------------------------------
;;  Returns !=0 if the BankStick type with selected BS number is available
;;  works for patterns, song and mixer stick
;;  IN: BS number
;;  OUT: != 0 if BankStick type of selected position is available
;; --------------------------------------------------------------------------
SEQ_BANK_GetTypeAvailable
	movwf	PRODL		; temporary store BS number in PRODL
	rcall	SEQ_BANK_GetType
	bz	SEQ_BANK_GetTypeAvailable_Failed
	addlw	-1
	bz	SEQ_BANK_GetTypeAvailable_P	; Pattern 32k
	addlw	-1
	bz	SEQ_BANK_GetTypeAvailable_P	; Pattern 64k
	addlw	-1
	bz	SEQ_BANK_GetTypeAvailable_S	; Song 32k
	addlw	-1
	bz	SEQ_BANK_GetTypeAvailable_S	; Song 64k
	addlw	-1
	bz	SEQ_BANK_GetTypeAvailable_M	; Mixer

SEQ_BANK_GetTypeAvailable_Failed
	andlw	0x00
	return

SEQ_BANK_GetTypeAvailable_P
	movf	PRODL, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	return

SEQ_BANK_GetTypeAvailable_S
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_BANK_GetTypeAvailable_Failed
	iorlw	0x01
	return

SEQ_BANK_GetTypeAvailable_M
	BRA_IFCLR MB_STAT, MB_STAT_USE_M_BANKSTICK, ACCESS, SEQ_BANK_GetTypeAvailable_Failed
	iorlw	0x01
	return


;; --------------------------------------------------------------------------
;;  Prints the specified BankStick type
;;  IN: BS number
;;  OUT: 2 characters
;; --------------------------------------------------------------------------
SEQ_BANK_PrnType
	rcall	SEQ_BANK_GetType
	TABLE_ADDR_MUL_W SEQ_BANK_TABLE_TYPE, 2

	movlw	2 ; characters
	goto	MIOS_LCD_PrintPreconfString


;; --------------------------------------------------------------------------
;;  Prints the specified BankStick Size (and n/a if not available)
;;  IN: BS number
;;  OUT: 3 characters
;; --------------------------------------------------------------------------
SEQ_BANK_PrnSize
	movwf	PRODL		; temporary store BS number in PRODL

	;; check if available
	rcall	SEQ_BANK_GetTypeAvailable
	bz	SEQ_BANK_PrnSize_NA

	;; get size from table
	movf	PRODL, W
	rcall	SEQ_BANK_GetType
	TABLE_ADDR_MUL_W SEQ_BANK_TABLE_SIZE, 1
	tblrd*
	movf	TABLAT, W
	call	MIOS_LCD_PrintBCD2
	movlw	'k'
	goto	MIOS_LCD_PrintChar

SEQ_BANK_PrnSize_NA
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	'/'
	call	MIOS_LCD_PrintChar
	movlw	'a'
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------

TEXT_EXTBANK_0   STRING 26, 0x00, "* Found BankStick CS#   * "
TEXT_EXTBANK_1   STRING 26, 0x40, "* Defined as 32k Device * "

TEXT_INTBANK_0   STRING 16, 0x00, "* Loading Setup "
TEXT_INTBANK_1   STRING 16, 0x40, "* from Internal "
	
TEXT_FORMATBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATBANK_1 STRING 16, 0x40, "* Pattern x:xx  "

TEXT_COPYNOT_0  STRING 18, 0x00, "* Target BankStick"
TEXT_COPYNOT_1  STRING 18, 0x40, "* not available!  "


