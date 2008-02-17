; $Id$
;
; MIDIboxSEQ
; Song Bank Load/Store Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; every song bank consists of 8 sectors, each sector consists of 256 bytes
;; the purpose of the sectors is defined here:
SEQ_SBANK_SECTOR_CFG		EQU	0x00
SEQ_SBANK_SECTOR_ACTN		EQU	0x01
SEQ_SBANK_SECTOR_G0		EQU	0x02
SEQ_SBANK_SECTOR_G1		EQU	0x03
SEQ_SBANK_SECTOR_G2		EQU	0x04
SEQ_SBANK_SECTOR_G3		EQU	0x05
SEQ_SBANK_SECTOR_BNK_G01	EQU	0x06
SEQ_SBANK_SECTOR_BNK_G23	EQU	0x07

;; determine which BankStick is assigned for Song Mode (some spaghetti code!!! too bad that MPASM doesn't provide "#elif"!)
;; the first definition wins, if no BankStick is assigned to "mode 3" or "mode 4", -1 will be set which disables song mode
#if DEFAULT_BANKSTICK_MODE_CS0 == 3 || DEFAULT_BANKSTICK_MODE_CS0 == 4
SEQ_SBANK_BS_CS		EQU	0x00
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS0
#else
#if DEFAULT_BANKSTICK_MODE_CS1 == 3 || DEFAULT_BANKSTICK_MODE_CS1 == 4
SEQ_SBANK_BS_CS		EQU	0x01
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS1
#else
#if DEFAULT_BANKSTICK_MODE_CS2 == 3 || DEFAULT_BANKSTICK_MODE_CS2 == 4
SEQ_SBANK_BS_CS		EQU	0x02
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS2
#else
#if DEFAULT_BANKSTICK_MODE_CS3 == 3 || DEFAULT_BANKSTICK_MODE_CS3 == 4
SEQ_SBANK_BS_CS		EQU	0x03
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS3
#else
#if DEFAULT_BANKSTICK_MODE_CS4 == 3 || DEFAULT_BANKSTICK_MODE_CS4 == 4
SEQ_SBANK_BS_CS		EQU	0x04
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS4
#else
#if DEFAULT_BANKSTICK_MODE_CS5 == 3 || DEFAULT_BANKSTICK_MODE_CS5 == 4
SEQ_SBANK_BS_CS		EQU	0x05
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS5
#else
#if DEFAULT_BANKSTICK_MODE_CS6 == 3 || DEFAULT_BANKSTICK_MODE_CS6 == 4
SEQ_SBANK_BS_CS		EQU	0x06
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS6
#else
#if DEFAULT_BANKSTICK_MODE_CS7 == 3 || DEFAULT_BANKSTICK_MODE_CS7 == 4
SEQ_SBANK_BS_CS		EQU	0x07
SEQ_SBANK_BS_CS_MODE	EQU	DEFAULT_BANKSTICK_MODE_CS7
#else
SEQ_SBANK_BS_CS		EQU	-1
SEQ_SBANK_BS_CS_MODE	EQU	0
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

;; --------------------------------------------------------------------------
;;  calculates the bankstick address:
;;     (SEQ_SONG << 11) + EEADR[H]
;;  IN: SEQ_SONG, EEADR
;;  OUT: bankstick address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SEQ_SBANK_CalcAddress
	;; select BankStick for songs
	movlw	SEQ_SBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	

	SET_BSR	SEQ_BASE
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	rlf	SEQ_SONG, W, BANKED		; calculate high address
	rlf	WREG, W
	rlf	WREG, W
 IF SEQ_SBANK_BS_CS_MODE == 4			; constant defined via EQU, therefore no #if, but IF
	andlw	0xf8
 ELSE
	andlw	0x78
 ENDIF
	iorwf	EEADRH, W
	movwf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------
;;  Writes data to BankStick
;;  IN: Value in WREG, song in SEQ_SONG, address offset in EEADR[H]
;;  OUT: increments address
;; --------------------------------------------------------------------------
SEQ_SBANK_Write
	movwf	MIOS_PARAMETER3			; temporary save word in MIOS_PARAMETER3

	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_Write_End

	;; determine address
	rcall	SEQ_SBANK_CalcAddress

	;; write content
	movf	MIOS_PARAMETER3, W
	call	MIOS_BANKSTICK_Write

	;; goto end if write successfull
	bz	SEQ_SBANK_Write_End

	;; else disable bankstick
	bcf	MB_STAT, MB_STAT_USE_S_BANKSTICK
	bcf	MB_STAT, MB_STAT_S_BANKSTICK_AVAILABLE

SEQ_SBANK_Write_End
	incf	EEADR, F			; increment EEPROM address
	skpnz
	incf	EEADRH, F

	;; fix BSR after a MIOS routine has been called
	SET_BSR	SEQ_BASE
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to BankStick
;;  IN: Value in FSR1 buffer, song in SEQ_SONG, address offset in EEADR
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_SBANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_WritePage_End

	;; determine address
	rcall	SEQ_SBANK_CalcAddress

	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage

	;; goto end if write successfull
	bz	SEQ_SBANK_WritePage_End

	;; else disable bankstick
	bcf	MB_STAT, MB_STAT_USE_S_BANKSTICK
	bcf	MB_STAT, MB_STAT_S_BANKSTICK_AVAILABLE

SEQ_SBANK_WritePage_End
	movlw	0x40
	addwf	EEADR, F			; increment EEPROM address
	movlw	0
	addwfc	EEADRH, F

	;; fix BSR after a MIOS routine has been called
	SET_BSR	SEQ_BASE
	return

;; --------------------------------------------------------------------------
;;  Read data from BankStick
;;  IN: SEQ_SONG, EEADR
;;  OUT: result in WREG, increments address
;; --------------------------------------------------------------------------
SEQ_SBANK_Read
	;; return 0x00 if bankstick not available
	movlw	0x00
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_Read_End

	;; determine address
	rcall	SEQ_SBANK_CalcAddress

	;; read content
	call	MIOS_BANKSTICK_Read

SEQ_SBANK_Read_End
	incf	EEADR, F
	skpnz
	incf	EEADRH, F

	;; fix BSR after a MIOS routine has been called
	SET_BSR	SEQ_BASE
	return


;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear MB_STAT.MB_STAT_USE_S_BANKSTICK
;;  if BankStick not available
;;  Note: this function is called from 
;; --------------------------------------------------------------------------
SEQ_SBANK_CheckStick
	;; ignore song bankstick if SEQ_SBANK_BS_CS == -1
	movlw	SEQ_SBANK_BS_CS
	addlw	1		; (add 1 to update zero flag, movlw doesn't do this (so far I remember))
	bz	SEQ_SBANK_CheckStick_End

	;; select BankStick for songs
	movlw	SEQ_SBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	
	
	;; save BankStick status for later in TMP1
	movff	MB_STAT, TMP1

	;; try to read from Song BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read

	;; save current status of USE_BANKSTICK again - we use it to determine changes
	movff	MB_STAT, TMP2

	;; set the USE_BANKSTICK and BANKSTICK_AVAILABLE flag if EEPROM responded
	bcf	MB_STAT, MB_STAT_USE_S_BANKSTICK
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_USE_S_BANKSTICK

	bcf	MB_STAT, MB_STAT_S_BANKSTICK_AVAILABLE
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_S_BANKSTICK_AVAILABLE

	;; check if flag has been changed
	movf	MB_STAT, W			; get new status
	xorwf	TMP2, W				; compare with status last saved in TMP2
	bz	SEQ_SBANK_CheckStick_End	; no change

	;; flag changed!
	GOTO_IFSET MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_CheckStick_Ext
	
	;; --> Disable Songmode
SEQ_SBANK_CheckStick_Int
SEQ_SBANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	
	SET_BSR	SEQ_BASE			; disable song
	movlw	0x80
	movwf	SEQ_SONG
	call	SEQ_SONG_ExitSongMode
	rgoto	SEQ_SBANK_CheckStick_End


	;; --> Enable Songmode
SEQ_SBANK_CheckStick_Ext
	;; don't print a message if bankstick already was active before checking (status saved in TMP1)
	BRA_IFSET TMP1, MB_STAT_S_BANKSTICK_AVAILABLE, ACCESS, SEQ_SBANK_CheckStrick_Ext_NoMsg
	TABLE_ADDR TEXT_EXTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	movlw	255				; wait some ms to get a stable status
	call	MIOS_Delay			
SEQ_SBANK_CheckStrick_Ext_NoMsg

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	clrf	MIOS_PARAMETER1			; read first byte from BankStick
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_SONG_MAGIC0		; branch to unformatted message if number not equal
	bnz	SEQ_SBANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_SONG_MAGIC1		; branch to unformatted message if number not equal
	bz	SEQ_SBANK_CheckStick_Reload
SEQ_SBANK_CheckStick_Unformatted

	;; format BankStick
	rgoto	SEQ_SBANK_FormatStickNow

SEQ_SBANK_CheckStick_Reload
	;; check if this is really 
SEQ_SBANK_FormatStick_Reload
	;; request re-init of page so that new values will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; change message delay counter
	movlw	0x20
	call	MIOS_LCD_MessageStart

SEQ_SBANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;;  Format Stick: initialize the songs
;; --------------------------------------------------------------------------
SEQ_SBANK_FormatStick
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

SEQ_SBANK_FormatStickNow
	;; select BankStick for songs
	movlw	SEQ_SBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	

	;; clear song 1-16 (in mode 4: 1-32)
	SET_BSR	SEQ_BASE
	clrf	SEQ_SONG, BANKED
SEQ_SBANK_FormatStickCopyLoop
	TABLE_ADDR TEXT_FORMATSBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print song number
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG, W, BANKED
	call	MIOS_LCD_PrintBCD2

	;; set song address to 0x000
	clrf	EEADR
	clrf	EEADRH

	;; initialize first page with 0
	lfsr	FSR1, SEQ_DATA_BUFFER
	clrf	PRODL
SEQ_SBANK_FormatStickCopyLoopL1
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 6, ACCESS, SEQ_SBANK_FormatStickCopyLoopL1

	;; write first page
	call	SEQ_SBANK_WritePage

	;; remaining pages completely initialized with 0x80 (no pattern) 0x10 (16 steps delay)
	lfsr	FSR1, SEQ_DATA_BUFFER
	clrf	PRODL
SEQ_SBANK_FormatStickCopyLoopL2
	movlw	0x80
	movwf	POSTINC1
	movlw	0x10
	movwf	POSTINC1
	movlw	2
	addwf	PRODL, F
	BRA_IFCLR PRODL, 6, ACCESS, SEQ_SBANK_FormatStickCopyLoopL2

	;; write pages until address 0x800 reached
SEQ_SBANK_FormatStickCopyLoopL3
	clrwdt			; feed watchdog
	call	SEQ_SBANK_WritePage
	BRA_IFCLR EEADRH, 3, ACCESS, SEQ_SBANK_FormatStickCopyLoopL3

	;; increment song number until last is reached
	incf	SEQ_SONG, F, BANKED
 IF SEQ_SBANK_BS_CS_MODE == 4			; constant defined via EQU, therefore no #if, but IF
	BRA_IFCLR SEQ_SONG, 5, BANKED, SEQ_SBANK_FormatStickCopyLoop
 ELSE
	BRA_IFCLR SEQ_SONG, 4, BANKED, SEQ_SBANK_FormatStickCopyLoop
 ENDIF

	;; write magic bytes to address 0x0000
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	BANKSTICK_SONG_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_SONG_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; deselect song
	SET_BSR	SEQ_BASE
	movlw	0x80
	movwf	SEQ_SONG, BANKED

	return


;; --------------------------------------------------------------------------
TEXT_FORMATSBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATSBANK_1 STRING 16, 0x40, "*   Song  xx    "
