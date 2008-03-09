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
;;     ((SEQ_SONG+1) << 8) | EEADR
;;  IN: SEQ_SONG, EEADR
;;  OUT: bankstick address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SEQ_SBANK_CalcAddress
	;; select BankStick for songs
	movlw	SEQ_SBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	

	SET_BSR	SEQ_BASE
	movff	EEADR, MIOS_PARAMETER1		; copy address to low-byte
	movf	SEQ_SONG, W, BANKED		; calculate high address
	andlw	0x3f
	addlw	1
	movwf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------
;;  Writes data to BankStick
;;  IN: Value in WREG, song in SEQ_SONG, address offset in EEADR
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

	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
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

	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return

;; --------------------------------------------------------------------------
;;  Similar to SEQ_SBANK_WritePage, but starts non-blocking write operation
;;  IN: -
;;  OUT: -
;; --------------------------------------------------------------------------
SEQ_SBANK_WriteSong_NB
	;; always start at address 0x00, and select song base address
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	clrf	EEADR

	;; store current song in SEQ_SAVE_*
	movff	SEQ_SONG, SEQ_SAVE_SONG

	;; clear store counter
	SET_BSR	SEQ_BASE
	clrf	SEQ_BANKSTICK_SAVE_SONG_CTR, BANKED

	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_WriteSong_NB_End

	;; determine address
	rcall	SEQ_SBANK_CalcAddress

	;; wait until BankStick available (ensures, that an ongoing write operation won't be terminated)
SEQ_BANK_WriteSong_NB_Wait
	call	MIOS_BANKSTICK_CheckAvail
	bz	SEQ_BANK_WriteSong_NB_Wait

	;; increment store counter - first page will be programmed
	SET_BSR	SEQ_BASE
	incf	SEQ_BANKSTICK_SAVE_SONG_CTR, F, BANKED

	;; start write operation
 	call	MIOS_BANKSTICK_WritePage_NB

SEQ_SBANK_WriteSong_NB_End
	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
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

	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
	return


;; --------------------------------------------------------------------------
;;  Reads 64 bytes from BankStick
;;  IN: SEQ_SONG, EEADR
;;  OUT: result in FSR1 buffer, increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_SBANK_ReadPage
	;; align address
	movlw	0xc0
	andwf	EEADR, F

	;; exit if bankstick not available
	movlw	0x00
	BRA_IFCLR MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, SEQ_SBANK_ReadPage_End

	;; determine address
	rcall	SEQ_SBANK_CalcAddress

	;; read page from BankStick
	call	MIOS_BANKSTICK_ReadPage

SEQ_SBANK_ReadPage_End
	movlw	0x40
	addwf	EEADR, F

	SET_BSR	SEQ_BASE			; fix BSR after a MIOS routine has been called
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

	;; check if this is a MBSEQ Mixer BankStick
	;; in such a case, disable the stick, don't reformat it!
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MIXER_MAGIC0
	bnz	SEQ_SBANK_CheckStick_NoMixerBS
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MIXER_MAGIC1
	bnz	SEQ_SBANK_CheckStick_NoMixerBS
	;; disable Mixer BankStick
	SET_BSR	SEQ_BASE
	movf	SEQ_BANKSTICK_CHK_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_BANKSTICK_DISABLED, F, BANKED
	;; print message
	TABLE_ADDR TEXT_MIXBANK_0
	call	MIOS_LCD_PrintMessage
	movff	SEQ_BANKSTICK_CHK_CTR, WREG
	call	MIOS_LCD_PrintBCD1
	call	MIOS_LCD_PrintMessage
	rgoto	SEQ_SBANK_CheckStick_End
	
SEQ_SBANK_CheckStick_NoMixerBS

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
;;  Used for non-blocking store operations
;; --------------------------------------------------------------------------
SEQ_SBANK_StoreStick
	;; select BankStick for songs
	movlw	SEQ_SBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet

	;; check if BankStick available again
	call	MIOS_BANKSTICK_CheckAvail
	bz	SEQ_SBANK_StoreStick_End
SEQ_SBANK_StoreStick_Avail
	;; increment save counter
	;; once it reaches 0x05, all pages have been saved
	SET_BSR	SEQ_BASE
	incf	SEQ_BANKSTICK_SAVE_SONG_CTR, F, BANKED
	movf	SEQ_BANKSTICK_SAVE_SONG_CTR, W, BANKED
	xorlw	0x05
	bnz	SEQ_SBANK_StoreStick_Avail_NextP
SEQ_SBANK_StoreStick_Avail_LastP
	;; stop store operation
	clrf	SEQ_BANKSTICK_SAVE_SONG_CTR, BANKED
	rgoto	SEQ_SBANK_StoreStick_End

SEQ_SBANK_StoreStick_Avail_NextP
	;; select pattern and bank
	movff	SEQ_SAVE_SONG, SEQ_SONG

	;; select current address - we need to store 4 pages (256 bytes)
	decf	SEQ_BANKSTICK_SAVE_SONG_CTR, W, BANKED
	mullw	0x40
	movff	PRODL, EEADR

	;; select buffer address
	lfsr	FSR1, SEQ_PATTERN_CHAIN
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F

	;; select BankStick and set address
	rcall	SEQ_SBANK_CalcAddress
 	call	MIOS_BANKSTICK_WritePage_NB	; start write operation
SEQ_SBANK_StoreStick_End
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

	;; clear buffer (write all-zero)
	;; write pages until address 0x1000 reached
	lfsr	FSR1, SEQ_DATA_BUFFER
	clrf	PRODL
SEQ_SBANK_FormatStick_PrepLoop
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 6, ACCESS, SEQ_SBANK_FormatStick_PrepLoop

	;; clear song A1..H8
	SET_BSR	SEQ_BASE
	clrf	SEQ_SONG, BANKED
SEQ_SBANK_FormatStickCopyLoop
	;; feed watchdog
	clrwdt

	TABLE_ADDR TEXT_FORMATSBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print song number
	movlw	0x49
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	movf	SEQ_SONG, W, BANKED
	call	SEQ_LCD_PrnPattern	; (same naming style)

	;; write 4 pages (=256 bytes)
	call	SEQ_SBANK_WritePage
	call	SEQ_SBANK_WritePage
	call	SEQ_SBANK_WritePage
	call	SEQ_SBANK_WritePage

	;; increment song number until last is reached
	incf	SEQ_SONG, F, BANKED
	BRA_IFCLR SEQ_SONG, 6, BANKED, SEQ_SBANK_FormatStickCopyLoop

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
TEXT_FORMATSBANK_1 STRING 16, 0x40, "*   Song xx     "
