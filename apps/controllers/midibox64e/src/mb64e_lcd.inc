; $Id$
;
; MB64E LCD functions, mainly used by cs_m_display.inc
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This functions sets the default Y offsets
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MB64E_LCD_SetDefaultOffsets
	movlw	DEFAULT_YOFFSET_LINE0
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_YOFFSET_LINE1
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_YOFFSET_LINE2
	movwf	MIOS_PARAMETER3
	movlw	DEFAULT_YOFFSET_LINE3
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
;;  This help function prints the bank information
;;  IN:  -
;;  OUT: 8 characters
;; --------------------------------------------------------------------------
MB64E_LCD_PrintBank
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MB64E_LCD_PrintBank_Ext
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_LCD_PrintBank_Int
MB64E_LCD_PrintBank_Ext
	movf	MB_BANK, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MB64E_SELECTED_BANK, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD3
	rgoto	MB64E_LCD_PrintBank_Cont

MB64E_LCD_PrintBank_Int
	movlw	'I'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	't'
	call	MIOS_LCD_PrintChar
MB64E_LCD_PrintBank_Cont

	movlw	'.'
	call	MIOS_LCD_PrintChar
	
	call	MB64E_LCD_PrintGroupNumber
	
	movlw	' '
	btfsc	MB_STAT2, MB_STAT2_ENC_SLOW
	movlw 'N'
	btfsc	MB_STAT2, MB_STAT2_ENC_SLOW
	movlw 'S'
	btfsc	MB_STAT2, MB_STAT2_ENC_FAST
	movlw 'F'
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This help function prints the group number
;;  IN:  -
;;  OUT: 3 characters
;; --------------------------------------------------------------------------
MB64E_LCD_PrintGroupNumber
	SET_BSR	MB64E_BASE

#if DEFAULT_ENCODER_GROUP_WIDTH == 1
	;; print MB64E_GROUP_OFFSET / 1 (3 digits)
	SET_BSR	MB64E_BASE
	incf	MB64E_GROUP_OFFSET, W, BANKED
	call	MIOS_LCD_PrintBCD3
#else
#if DEFAULT_ENCODER_GROUP_WIDTH == 2
	;; print MB64E_GROUP_OFFSET / 2 (2 digits)
	rrf	MB64E_GROUP_OFFSET, W, BANKED
	andlw	0x3f
	addlw	1
	call	MIOS_LCD_PrintBCD2
#endif
#if DEFAULT_ENCODER_GROUP_WIDTH == 4
	;; print MB64E_GROUP_OFFSET / 4 (2 digits)
	rrf	MB64E_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	andlw	0x1f
	addlw	1
	call	MIOS_LCD_PrintBCD2
#endif
#if DEFAULT_ENCODER_GROUP_WIDTH == 8
	;; print MB64E_GROUP_OFFSET / 8 (2 digits)
	rrf	MB64E_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_ENCODER_GROUP_WIDTH == 16
	;; print MB64E_GROUP_OFFSET / 16 (1 digit)
	swapf	MB64E_GROUP_OFFSET, W, BANKED
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_ENCODER_GROUP_WIDTH == 32
	;; print MB64E_GROUP_OFFSET / 32 (1 digit)
	swapf	MB64E_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_ENCODER_GROUP_WIDTH == 64
	;; print MB64E_GROUP_OFFSET / 64 (1 digit)
	swapf	MB64E_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	andlw	0x01
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#endif
	return


;; --------------------------------------------------------------------------
;;  This help function prints 8 bars of the current encoder row
;;  IN:  first enc entry in WREG
;;       current enc in MB64E_LAST_ENTRY
;;  OUT: 8 characters
;;  USES: MB64E_CURRENT_ENTRY, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_8EncBars
	;; store selected enc row in MB64E_CURRENT_ENTRY
	SET_BSR	MB64E_BASE
	andlw 	0xf8
	movwf	MB64E_CURRENT_ENTRY, BANKED
	
	;; store current enc number in TMP1
	movf	MB64E_LAST_ENTRY, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last enc unequal to selected row
	andlw 	0xf0
	xorwf	MB64E_CURRENT_ENTRY, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MB64E_LCD_xEncBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current encoder row
;;  IN:  first enc entry in WREG
;;       current enc in MB64E_LAST_ENTRY
;;  OUT: 16 characters
;;  USES: MB64E_CURRENT_ENTRY, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_16EncBars
	;; store selected pot row in MB64_CURRENT_ENTRY
	SET_BSR	MB64E_BASE
	andlw 	0xf0
	movwf	MB64E_CURRENT_ENTRY, BANKED
	
	;; store current pot number in TMP1
	movf	MB64E_LAST_ENTRY, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last pot unequal to selected row
	andlw 	0xf0
	xorwf	MB64E_CURRENT_ENTRY, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64E_CURRENT_ENTRY, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MB64E_LCD_xEncBars

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current pot row
;;  IN:  first displayed pot in MB64E_CURRENT_ENTRY
;;       current pot (which should flash) in TMP1
;;          (if TMP1[7] set, no pot entry will be flashed)
;;       last displayed pot in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MB64E_CURRENT_ENTRY, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_xEncBars
MB64E_LCD_xEncBars_Loop
	;; if printed pot number matches with current pot number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MB64E_LCD_xEncBars_Loop_NoFlash
	movf	TMP1, W
	cpfseq	MB64E_CURRENT_ENTRY, BANKED
	rgoto MB64E_LCD_xEncBars_Loop_NoFlash
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MB64E_LCD_xEncBars_Loop_Next
MB64E_LCD_xEncBars_Loop_NoFlash
	call	MB64E_ENC_Map	; special MB64E feature: map encoder to parameter entry
	rcall	MB64E_LCD_PrnVBar
MB64E_LCD_xEncBars_Loop_Next
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movf	TMP2, W
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto MB64E_LCD_xEncBars_Loop

	return

;; --------------------------------------------------------------------------
;;  This help function prints 8 bars of the current button row
;;  IN:  current button number in WREG
;;  OUT: 8 characters
;;  USES: MB64E_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_8ButtonBars
	;; save current button number in TMP1
	SET_BSR	MB64E_BASE
	movwf	TMP1

	;; begin with 0, 8, 16, 24, ...
	movwf	MB64E_CURRENT_BUTTON, BANKED
	movlw	0xf8
	andwf	MB64E_CURRENT_BUTTON, F, BANKED

	;; last displayed button in TMP2
	movf	MB64E_CURRENT_BUTTON, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MB64E_LCD_xButtonBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current button row
;;  IN:  current button number in WREG
;;  OUT: 16 characters
;;  USES: MB64E_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_16ButtonBars
	;; save current button number in TMP1
	SET_BSR	MB64E_BASE
	movwf	TMP1

	;; begin with 0, 16, 32, ...
	movwf	MB64E_CURRENT_BUTTON, BANKED
	movlw	0xf0
	andwf	MB64E_CURRENT_BUTTON, F, BANKED

	;; last displayed button in TMP2
	movf	MB64E_CURRENT_BUTTON, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MB64E_LCD_xButtonBars

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current button row
;;  IN:  first displayed button in MB64E_CURRENT_BUTTON
;;       current button (which should flash) in TMP1
;;       last displayed button in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MB64E_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_xButtonBars
MB64E_LCD_xButtonBars_Loop
	;; if printed button number matches with current button number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MB64E_LCD_xButtonBars_Loop_NoFl
	movf	TMP1, W
	cpfseq	MB64E_CURRENT_BUTTON, BANKED
	rgoto MB64E_LCD_xButtonBars_Loop_NoFl
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MB64E_LCD_xButtonBars_Loop_Next
MB64E_LCD_xButtonBars_Loop_NoFl
	rcall	MB64E_LCD_PrnBtnStatus
MB64E_LCD_xButtonBars_Loop_Next
	SET_BSR	MB64E_BASE
	incf	MB64E_CURRENT_BUTTON, F, BANKED
	movf	TMP2, W
	cpfsgt	MB64E_CURRENT_BUTTON, BANKED
	rgoto MB64E_LCD_xButtonBars_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the enc label and value
;;  IN:  enc number in WREG
;;  OUT: 16 characters
;;  USES: MB64E_CURRENT_ENTRY, TMP[12]
;; --------------------------------------------------------------------------
MB64E_LCD_PrintLabel
	movff	WREG, MB64E_CURRENT_ENTRY
	call	MB64E_ADDR_LabelEntry	; get address to label
	call	MB64E_BANK_Read	; read mode byte
	movwf	TMP1		; save mode in TMP1
	andlw	0xff		; fix STATUS
	bnz	MB64E_LCD_PrintLabel_Ok
	;; no label defined, print standard strin
MB64E_LCD_PrintLabel_Std
	movff	MB64E_CURRENT_ENTRY, WREG
	rgoto	MB64E_LCD_PrintEvntStd_Enc
MB64E_LCD_PrintLabel_Ok
	;; print string
	movlw	13
	btfsc	TMP1, 5
	movlw 12
	movwf	TMP2
MB64E_LCD_PrintLabel_Loop
	call	MB64E_BANK_Read
	call	MIOS_LCD_PrintChar
	decfsz	TMP2, F
	rgoto	MB64E_LCD_PrintLabel_Loop
	
	;; get value
	call	MB64E_FSR_EncValue
	;; continue depending on value mode
	movf	TMP1, W
	rcall	MB64E_LCD_PrintValue
	CALL_IFSET TMP1, 5, ACCESS, MB64E_LCD_PrnVBar
	return


;; --------------------------------------------------------------------------
;;  This help function prints a value in different number format
;;  IN:   value format in WREG, value in INDF0
;;  OUT:  2/3 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64E_LCD_PrintValue
	andlw	0x03
	JUMPTABLE_2BYTES 4
	rgoto	MB64E_LCD_PrintValue_Hex
	rgoto	MB64E_LCD_PrintValue_PmHex
	rgoto	MB64E_LCD_PrintValue_Dec
	rgoto	MB64E_LCD_PrintValue_PmDec

	;; print hex value 0x00..0x7f
MB64E_LCD_PrintValue_Hex
	movlw	' '		; fill with space
	call	MIOS_LCD_PrintChar
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintHex2

	;; print hex value -0x40..0x3f
MB64E_LCD_PrintValue_PmHex
	movlw	' '		; space or "-"?
	btfss	INDF0, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	INDF0, W		; calc: 0x40-value
	andlw	0x7f
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintHex2	; and print it

	;; print dec value 1..128
MB64E_LCD_PrintValue_Dec
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintBCD3

	;; print dec value -64..63
MB64E_LCD_PrintValue_PmDec
	movlw	' '		; space or "-"?
	btfss	INDF0, 6; (if value <= 0x3f (6th bit cleared), print "-"
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	INDF0, W		; calc: 0x40-value
	andlw	0x7f
	sublw	0x40
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it

;; --------------------------------------------------------------------------
;;  This help function prints the standard string for a enc value
;;  IN:   enc number in WREG
;;  OUT:  16 characters
;;  USES: MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_LCD_PrintEvntStd_Enc
	;; copy WREG to MB64E_CURRENT_ENTRY
	movff	WREG, MB64E_CURRENT_ENTRY
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64E_MIDI_PrepareEncEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print bar
	rgoto	MB64E_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a enc value
;;  IN:   enc number in WREG
;;  OUT:  8 characters
;;  USES: MB64E_CURRENT_ENTRY
;; --------------------------------------------------------------------------
MB64E_LCD_PrintShortEvntStd_Enc
	;; copy WREG to MB64E_CURRENT_ENTRY
	movff	WREG, MB64E_CURRENT_ENTRY
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64E_MIDI_PrepareEncEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print bar
	rgoto	MB64E_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 16 characters
;;  USES: MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_LCD_PrintEvntStd_Btn
	;; copy WREG to MB64E_CURRENT_BUTTON
	movff	WREG, MB64E_CURRENT_BUTTON
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64E_MIDI_PrepareButtonEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print button status
	rgoto	MB64E_LCD_PrnBtnStatus

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 8 characters
;;  USES: MB64E_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64E_LCD_PrintShortEvntStd_Btn
	;; copy WREG to MB64E_CURRENT_BUTTON
	movff	WREG, MB64E_CURRENT_BUTTON
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64E_MIDI_PrepareButtonEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print button status
	rgoto	MB64E_LCD_PrnBtnStatus


;; --------------------------------------------------------------------------
;;  This help function prints the standard string for the learned event
;;  IN:  event in MB64E_LEARN_EVNT[012],
;;       button/enc event selected by MB_STAT_LAST_WAS_BUTTON
;;  OUT: 16 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64E_LCD_PrintEvntStd_Learn
	movff	MB64E_LEARN_EVNT0, MIDI_EVNT0
	movff	MB64E_LEARN_EVNT1, MIDI_EVNT1
	movff	MB64E_LEARN_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Print
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MB64E_LCD_PrintEvntStd_Learn_B
MB64E_LCD_PrintEvntStd_Learn_P
	movlw	' '
MB64E_LCD_PrintEvntStd_Learn_C
	goto	MIOS_LCD_PrintChar
MB64E_LCD_PrintEvntStd_Learn_B
	movff	MB64E_LEARN_BMODE, WREG
	BRA_IFSET WREG, 1, ACCESS, MB64E_LCD_PrintEvntStd_Learn_B2
	BRA_IFSET WREG, 0, ACCESS, MB64E_LCD_PrintEvntStd_Learn_B1
MB64E_LCD_PrintEvntStd_Learn_B0
	movlw	'*'
	rgoto	MB64E_LCD_PrintEvntStd_Learn_C
MB64E_LCD_PrintEvntStd_Learn_B1
	movlw	'O'
	rgoto	MB64E_LCD_PrintEvntStd_Learn_C
MB64E_LCD_PrintEvntStd_Learn_B2
	movlw	'T'
	rgoto	MB64E_LCD_PrintEvntStd_Learn_C
	
;; ==========================================================================
;; print vertical bar depending on enc status (value & soft-takeover flag)
;; use MB64E_LCD_PrnVBar_NoArw to skip the soft-takeover indication arrows
;; IN:   enc number in MB64E_CURRENT_ENTRY
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MB64E_LCD_PrnVBar_NoArw
MB64E_LCD_PrnVBar
MB64E_LCD_PrnVBar_Normal
	;; get pointer to enc value and soft-takeover status
	call	MB64E_FSR_EncValue
	swapf	INDF0, W	; saved value / 16
	andlw	0x07
MB64E_LCD_PrnVBar_Now
	;; finally print character and exit
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the button status (* if pressed, o if depressed)
;; IN:   button number in MB64E_CURRENT_BUTTON
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MB64E_LCD_PrnBtnStatus
	;; get button state
	call	MB64E_FSR_ButtonValue
	movff	MB64E_CURRENT_BUTTON, WREG
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W

	;; print character
	movlw	'*'		; button pressed
	skpz
	movlw	'o'		; button depressed
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print patch ID (uses TMP1)
;; ==========================================================================
MB64E_LCD_PrnPatch
	BRA_IFSET WREG, 7, ACCESS, MB64E_LCD_PrnPatch_Dis
	movwf	TMP1
	rlf	WREG, W
	swapf	WREG, W
	andlw	0x07
	btfss	TMP1, 6
	addlw 'A'
	btfsc	TMP1, 6
	addlw 'a'
	call	MIOS_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	'1'
	goto	MIOS_LCD_PrintChar

MB64E_LCD_PrnPatch_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the bank/patch name
;; IN:   -
;; OUT:  16 characters
;; USES: TMP1
;; ==========================================================================

MB64E_LCD_PrintBNameRAM
#if DEFAULT_PATCHMODE == 0
MB64E_LCD_PrintBankName
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MB64E_LCD_PrintBnkNameEmpty
#else
MB64E_LCD_PrintPatchName
	lfsr	FSR0, MB64E_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MB64E_LCD_PrintPatchNameEmpty
#endif


	clrf	TMP1		; used as loop counter
MB64E_LCD_PrintBNameRAMLoop
	movff	POSTINC0, TMP2
	movlw	0x20
	cpfslt	TMP2, ACCESS
	rgoto MB64E_LCD_PrintBNameRAMLoopOk
	movlw	' '
	movwf	TMP2
MB64E_LCD_PrintBNameRAMLoopOk
	movf	TMP2, W
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_LCD_PrintBNameRAMLoop
	return

;; ---

MB64E_LCD_PrintBNameEEP
#if DEFAULT_PATCHMODE == 0
MB64E_LCD_PrintPatchName
	; if not in PATCHMODE, we don't have Patch names in EEPROM
	rgoto	MB64E_LCD_PrintPatchNameEmpty
#else
MB64E_LCD_PrintBankName
	call	MB64E_ADDR_Bankname
	call	MB64E_BANK_Read
	bz	MB64E_LCD_PrintBnkNameEmpty
#endif

	call	MIOS_LCD_PrintChar

	clrf	TMP1		; used as loop counter
	incf	TMP1, F
MB64E_LCD_PrintBNameEEPLoop
	call	MB64E_BANK_Read
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64E_LCD_PrintBNameEEPLoop
	return

;; ---
MB64E_LCD_PrintBnkNameEmpty
	TABLE_ADDR MB64E_LCD_BNAME_EMPTY_STR
	movf	MB_BANK, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MB64E_SELECTED_BANK, WREG
	rgoto	MB64E_LCD_PrintBPNameEmptyC

MB64E_LCD_PrintPatchNameEmpty
	TABLE_ADDR MB64E_LCD_PNAME_EMPTY_STR
	movf	MB_PATCH, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MB64E_SELECTED_PATCH, WREG
	iorlw	0x80
	;; 	rgoto	MB64E_LCD_PrintBPNameEmptyC

MB64E_LCD_PrintBPNameEmptyC
	movwf	TMP1
	tblrd*+
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintPreconfString
	BRA_IFSET TMP1, 7, ACCESS, MB64E_LCD_PrintBPNameEmptyCP
MB64E_LCD_PrintBPNameEmptyCB
	incf	TMP1, W
	call	MIOS_LCD_PrintBCD2
	rgoto	MB64E_LCD_PrintBPNameEmptyCC
MB64E_LCD_PrintBPNameEmptyCP
	movf	TMP1, W
	andlw	0x7f
	rcall	MB64E_LCD_PrnPatch
	;; 	rgoto	MB64E_LCD_PrintBPNameEmptyCC
MB64E_LCD_PrintBPNameEmptyCC
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'>'
	goto	MIOS_LCD_PrintChar

MB64E_LCD_BNAME_EMPTY_STR
	STRING	12, 0x00, "< new bank  "
MB64E_LCD_PNAME_EMPTY_STR
	STRING	12, 0x00, "< new patch "

;; --------------------------------------------------------------------------
;;  This help function prints a horizontal bar
;;  IN: value (0x00-0x7f) in WREG
;;  OUT: 16 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64E_LCD_PrintHBar
	;; save value in TMP1
	;; length of bar is: value / 8
	rrf	WREG, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	movwf	TMP1

	;; save position of the zero-marker ('|') in TMP3
	movlw	0x08
	movwf	TMP2

	;; start loop, 16 chars to print, example: #####-----------
	clrf	TMP3
MB64E_LCD_PrintHBar_Loop
	movf	TMP3, W
	cpfseq	TMP2, ACCESS
	rgoto MB64E_LCD_PrintHBar_Loop_M12
MB64E_LCD_PrintHBar_Loop_Mark
	incf	TMP1, F		; only valid for panorama: increment "#" pos
	movlw	'|'		; print vertical bar
	rgoto	MB64E_LCD_PrintHBar_Loop_Cont

MB64E_LCD_PrintHBar_Loop_M12
	bnz	MB64E_LCD_PrintHBar_Loop_M12C
	movlw	'>'
	rgoto	MB64E_LCD_PrintHBar_Loop_Cont
MB64E_LCD_PrintHBar_Loop_M12C
	movf	TMP1, W
	cpfsgt	TMP3, ACCESS
	rgoto MB64E_LCD_PrintHBar_Loop_M2
MB64E_LCD_PrintHBar_Loop_M1
	movlw	'-'
	rgoto	MB64E_LCD_PrintHBar_Loop_Cont
MB64E_LCD_PrintHBar_Loop_M2
	movlw	'#'
	;; 	rgoto	MB64E_LCD_PrintHBar_Loop_Cont
MB64E_LCD_PrintHBar_Loop_Cont
	call	MIOS_LCD_PrintChar

	incf	TMP3, F		; increment
	movlw	16
	cpfseq	TMP3, ACCESS
	rgoto MB64E_LCD_PrintHBar_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the selected morph groups
;;  IN: MB64E_MORPH_GROUP_SELECT
;;  OUT: 8 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64E_LCD_PrintMorphGroups
	movff	MB64E_MORPH_GROUP_SELECT, TMP1	; TMP1 saves the group flags, it will be shifted with every iteration
	clrf	TMP2				; TMP2 used as loop counter
MB64E_LCD_PrintMorphGroupsLoop
	movlw	'-'
	BRA_IFCLR TMP1, 0, ACCESS, MB64E_LCD_PrintMorphGroupsLoopC
	movf	TMP2, W
	addlw	'1'
MB64E_LCD_PrintMorphGroupsLoopC
	call	MIOS_LCD_PrintChar

	rrf	TMP1, F
	incf	TMP2, F
	BRA_IFCLR TMP2, 3, ACCESS, MB64E_LCD_PrintMorphGroupsLoop
	return


;; --------------------------------------------------------------------------
;;  This help function prints the values of all morph groups as V bars
;;  IN:  -
;;  OUT: 8 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64E_LCD_PrintMorphBars
	clrf	TMP1				; using TMP1 as loop counter
	lfsr	FSR0, MB64E_MORPH_VALUES_G0
MB64E_LCD_PrintMorphBarsLoop
	swapf	POSTINC0, W
	andlw	0x07
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MB64E_LCD_PrintMorphBarsLoop
	return


;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
MB64E_LCD_InitVBars
	;; exit if GLCD connected
	btfsc	CS_STAT, CS_STAT_DISPLAY_GLCD
	return

	;; init special chars and exit
	TABLE_ADDR MB64E_LCD_V_BARS
	goto	MIOS_CLCD_SpecialCharsInit

	;; for vertical bars
MB64E_LCD_V_BARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: bar1
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00011111'; 7th and 8th line of special char
	;; char #2: bar2
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #3: bar3
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #4: bar4
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #5: bar5
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #6: bar6
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #7: bar7
	db b'00000000', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
	;; char #8: bar8
	db b'00011111', b'00011111'; 1st and 2nd line of special char
	db b'00011111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011111'; 5th and 6th line of special char
	db b'00011111', b'00011111'; 7th and 8th line of special char
