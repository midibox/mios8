; $Id$
;
; MIDIboxSEQ
; Mixer Map Load/Store Routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; each mixer map allocates 256 bytes
;; 
;; only 128 bytes are really used, the upper half is reserved for future extensions
;; the parameter structure is defined with SEQ_MIXER_* (see app_defines.inc)
;; 
;; the first mixer map location (0x0000-0x00ff) is not used, it contains the magic number
;; and could contain some additional configuration settings in future
;; 


;; determine which BankStick is assigned for Mixer Maps (some spaghetti code!!! too bad that MPASM doesn't provide "#elif"!)
;; the first definition wins, if no BankStick is assigned to "mode 5", -1 will be set which disables song mode
#if DEFAULT_BANKSTICK_MODE_CS0 == 5
SEQ_MBANK_BS_CS		EQU	0x00
#else
#if DEFAULT_BANKSTICK_MODE_CS1 == 5
SEQ_MBANK_BS_CS		EQU	0x01
#else
#if DEFAULT_BANKSTICK_MODE_CS2 == 5
SEQ_MBANK_BS_CS		EQU	0x02
#else
#if DEFAULT_BANKSTICK_MODE_CS3 == 5
SEQ_MBANK_BS_CS		EQU	0x03
#else
#if DEFAULT_BANKSTICK_MODE_CS4 == 5
SEQ_MBANK_BS_CS		EQU	0x04
#else
#if DEFAULT_BANKSTICK_MODE_CS5 == 5
SEQ_MBANK_BS_CS		EQU	0x05
#else
#if DEFAULT_BANKSTICK_MODE_CS6 == 5
SEQ_MBANK_BS_CS		EQU	0x06
#else
#if DEFAULT_BANKSTICK_MODE_CS7 == 5
SEQ_MBANK_BS_CS		EQU	0x07
#else
SEQ_MBANK_BS_CS		EQU	-1
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
;;     ((SEQ_MIXER_MAP+1) << 8) + EEADR
;;  IN: SEQ_MIXER_MAP, EEADR
;;  OUT: bankstick address in MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
SEQ_MBANK_CalcAddress
	;; select BankStick for songs
	movlw	SEQ_MBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	

	SET_BSR	SEQ_BASE
	movff	EEADR, MIOS_PARAMETER1
	incf	SEQ_MIXER_MAP, W, BANKED
	movwf	MIOS_PARAMETER2
	return

;; --------------------------------------------------------------------------
;;  Writes a page of 64 bytes to BankStick
;;  IN: pointer to source buffer in FSR1, address offset in EEADR
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_MBANK_WritePage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_M_BANKSTICK, ACCESS, SEQ_MBANK_WritePage_End

	;; determine address
	rcall	SEQ_MBANK_CalcAddress

	;; never write to page 0 (contains the magic numbers)
	movf	MIOS_PARAMETER2, W
	bz	SEQ_MBANK_WritePage_End

	;; buffer already prepared in FSR1
 	call	MIOS_BANKSTICK_WritePage

	;; goto end if write successfull
	bz	SEQ_MBANK_WritePage_End

	;; else disable bankstick
	bcf	MB_STAT, MB_STAT_USE_M_BANKSTICK
	bcf	MB_STAT, MB_STAT_M_BANKSTICK_AVAILABLE

SEQ_MBANK_WritePage_End
	movlw	0x40
	addwf	EEADR, F			; increment EEPROM address

	;; fix BSR after a MIOS routine has been called
	SET_BSR	SEQ_BASE
	return

;; --------------------------------------------------------------------------
;;  Read a page of 64 bytes from BankStick
;;  IN: pointer to target buffer in FSR1, address offset in EEADR
;;  OUT: increments address by 0x40
;; --------------------------------------------------------------------------
SEQ_MBANK_ReadPage
	;; align EEADR if necessary
	movlw	0xc0
	andwf	EEADR, F

	;; exit if bankstick not available
	BRA_IFCLR MB_STAT, MB_STAT_USE_M_BANKSTICK, ACCESS, SEQ_MBANK_ReadPage_End

	;; determine address
	rcall	SEQ_MBANK_CalcAddress

	;; buffer already prepared in FSR1
	call	MIOS_BANKSTICK_ReadPage

SEQ_MBANK_ReadPage_End
	movlw	0x40
	addwf	EEADR, F			; increment EEPROM address

	;; fix BSR after a MIOS routine has been called
	SET_BSR	SEQ_BASE
	return

;; --------------------------------------------------------------------------
;;  Check Stick: try to read from BankStick, clear MB_STAT.MB_STAT_USE_M_BANKSTICK
;;  if BankStick not available
;;  Note: this function is called from 
;; --------------------------------------------------------------------------
SEQ_MBANK_CheckStick
	;; ignore song bankstick if SEQ_MBANK_BS_CS == -1
	movlw	SEQ_MBANK_BS_CS
	addlw	1		; (add 1 to update zero flag, movlw doesn't do this (so far I remember))
	bz	SEQ_MBANK_CheckStick_End

	;; select BankStick for mixer maps
	movlw	SEQ_MBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	
	
	;; save BankStick status for later in TMP1
	movff	MB_STAT, TMP1

	;; try to read from Mixer BankStick (address 0x0000)
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read

	;; save current status of USE_BANKSTICK again - we use it to determine changes
	movff	MB_STAT, TMP2

	;; set the USE_BANKSTICK and BANKSTICK_AVAILABLE flag if EEPROM responded
	bcf	MB_STAT, MB_STAT_USE_M_BANKSTICK
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_USE_M_BANKSTICK

	bcf	MB_STAT, MB_STAT_M_BANKSTICK_AVAILABLE
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_BS_AVAILABLE
	bsf	MB_STAT, MB_STAT_M_BANKSTICK_AVAILABLE

	;; check if flag has been changed
	movf	MB_STAT, W			; get new status
	xorwf	TMP2, W				; compare with status last saved in TMP2
	bz	SEQ_MBANK_CheckStick_End	; no change

	;; flag changed!
	GOTO_IFSET MB_STAT, MB_STAT_USE_M_BANKSTICK, ACCESS, SEQ_MBANK_CheckStick_Ext
	
	;; --> Disable Mixer Map
SEQ_MBANK_CheckStick_Int
SEQ_MBANK_CheckStick_IntReload
	TABLE_ADDR TEXT_INTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	
	rgoto	SEQ_MBANK_CheckStick_End


	;; --> Enable Mixer Map
SEQ_MBANK_CheckStick_Ext
	;; don't print a message if bankstick already was active before checking (status saved in TMP1)
	BRA_IFSET TMP1, MB_STAT_M_BANKSTICK_AVAILABLE, ACCESS, SEQ_MBANK_CheckStrick_Ext_NoMsg
	TABLE_ADDR TEXT_EXTBANK_0		; print message
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; call the control surface function which loads the map referenced by CS_MIXER_MAP
	call	CS_M_MIXER_LoadMap

	movlw	255				; wait some ms to get a stable status
	call	MIOS_Delay			
SEQ_MBANK_CheckStrick_Ext_NoMsg

	;; now check if the magic numbers exist in bankstick bank - if not, format bank automatically
	clrf	MIOS_PARAMETER1			; read first byte from BankStick
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_Read
	xorlw	BANKSTICK_MIXER_MAGIC0		; branch to unformatted message if number not equal
	bnz	SEQ_MBANK_CheckStick_Unformatted
	call	MIOS_BANKSTICK_Read		; read second byte from BankStick
	xorlw	BANKSTICK_MIXER_MAGIC1		; branch to unformatted message if number not equal
	bz	SEQ_MBANK_CheckStick_Reload
SEQ_MBANK_CheckStick_Unformatted

	;; format BankStick
	rgoto	SEQ_MBANK_FormatStickNow

SEQ_MBANK_CheckStick_Reload
SEQ_MBANK_FormatStick_Reload
	;; request re-init of page so that new values will be displayed
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; change message delay counter
	movlw	0x20
	call	MIOS_LCD_MessageStart

SEQ_MBANK_CheckStick_End
	return


;; --------------------------------------------------------------------------
;;  Format Stick: initialize the songs
;; --------------------------------------------------------------------------
SEQ_MBANK_FormatStick
	btfss	MB_STAT, MB_STAT_USE_M_BANKSTICK
	return

SEQ_MBANK_FormatStickNow
	;; select BankStick for mixer maps
	movlw	SEQ_MBANK_BS_CS
	call	MIOS_BANKSTICK_CtrlSet	
	
	;; get initial mixer map setting by using the appr. control surface function
	call	CS_M_MIXER_ClearMap
	
	;; the upper block will be completely zeroed, using this temporary buffer:
	clrf	PRODL
	lfsr	FSR1, SEQ_DATA_BUFFER
SEQ_MBANK_FormatStickClrLoop
	clrf	POSTINC1
	incf	PRODL, F
	movlw	0x40-1
	cpfsgt	PRODL, ACCESS
	rgoto SEQ_MBANK_FormatStickClrLoop

	;; clear map 1-127
	SET_BSR	SEQ_BASE
	clrf	SEQ_MIXER_MAP, BANKED
SEQ_MBANK_FormatStickCopyLoop
	clrwdt			; feed watchdog

	TABLE_ADDR TEXT_FORMATMBANK_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; print mixer map number
	movlw	0x40 + 13
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_MIXER_MAP, W, BANKED
	call	MIOS_LCD_PrintBCD3

	;; write 256 bytes (4 x 64 byte blocks)
	clrf	EEADR
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x00
	rcall	SEQ_MBANK_WritePage
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x40
	rcall	SEQ_MBANK_WritePage

	lfsr	FSR1, SEQ_DATA_BUFFER	; (empty)
	rcall	SEQ_MBANK_WritePage
	lfsr	FSR1, SEQ_DATA_BUFFER	; (empty)
	rcall	SEQ_MBANK_WritePage

	;; increment mixer map number until we reached the last one
	incf	SEQ_MIXER_MAP, F, BANKED
	movlw	127-1		; (no error, first page contains configuration data)
	cpfsgt	SEQ_MIXER_MAP, BANKED
	rgoto SEQ_MBANK_FormatStickCopyLoop

	;; clear page 0 completely
	lfsr	FSR1, SEQ_DATA_BUFFER
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	call	MIOS_BANKSTICK_WritePage ; 0x0000-0x003f
	call	MIOS_BANKSTICK_WritePage ; 0x0040-0x007f
	call	MIOS_BANKSTICK_WritePage ; 0x0080-0x00bf
	call	MIOS_BANKSTICK_WritePage ; 0x00c0-0x00ff

	;; write magic bytes to address 0x0000
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	BANKSTICK_MIXER_MAGIC0
	call	MIOS_BANKSTICK_Write
	movlw	BANKSTICK_MIXER_MAGIC1
	call	MIOS_BANKSTICK_Write

	;; switch back to first mixer map
	SET_BSR	SEQ_BASE
	clrf	SEQ_MIXER_MAP, BANKED

	return


;; --------------------------------------------------------------------------
TEXT_FORMATMBANK_0 STRING 16, 0x00, "* Formatting    "
TEXT_FORMATMBANK_1 STRING 16, 0x40, "* Mixer Map #xxx"
