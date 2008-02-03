; $Id$
;
; MBMF LCD functions, mainly used by cs_m_display.inc
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
MBMF_LCD_SetDefaultOffsets
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
MBMF_LCD_PrintBank
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MBMF_LCD_PrintBank_Ext
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_LCD_PrintBank_Int
MBMF_LCD_PrintBank_Ext
	movf	MB_BANK, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MBMF_SELECTED_BANK, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD3
	rgoto	MBMF_LCD_PrintBank_Cont

MBMF_LCD_PrintBank_Int
	movlw	'I'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	't'
	call	MIOS_LCD_PrintChar
MBMF_LCD_PrintBank_Cont

	movlw	'.'
	call	MIOS_LCD_PrintChar
	
	call	MBMF_LCD_PrintGroupNumber
	
	movlw	' '
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This help function prints the group number
;;  IN:  -
;;  OUT: 3 characters
;; --------------------------------------------------------------------------
MBMF_LCD_PrintGroupNumber
	SET_BSR	MBMF_BASE

#if DEFAULT_FADER_GROUP_WIDTH == 1
	;; print MBMF_GROUP_OFFSET / 1 (3 digits)
	SET_BSR	MBMF_BASE
	incf	MBMF_GROUP_OFFSET, W, BANKED
	call	MIOS_LCD_PrintBCD3
#else
#if DEFAULT_FADER_GROUP_WIDTH == 2
	;; print MBMF_GROUP_OFFSET / 2 (2 digits)
	rrf	MBMF_GROUP_OFFSET, W, BANKED
	andlw	0x3f
	addlw	1
	call	MIOS_LCD_PrintBCD2
#endif
#if DEFAULT_FADER_GROUP_WIDTH == 4
	;; print MBMF_GROUP_OFFSET / 4 (2 digits)
	rrf	MBMF_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	andlw	0x1f
	addlw	1
	call	MIOS_LCD_PrintBCD2
#endif
#if DEFAULT_FADER_GROUP_WIDTH == 8
	;; print MBMF_GROUP_OFFSET / 8 (2 digits)
	rrf	MBMF_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_FADER_GROUP_WIDTH == 16
	;; print MBMF_GROUP_OFFSET / 16 (1 digit)
	swapf	MBMF_GROUP_OFFSET, W, BANKED
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_FADER_GROUP_WIDTH == 32
	;; print MBMF_GROUP_OFFSET / 32 (1 digit)
	swapf	MBMF_GROUP_OFFSET, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
#if DEFAULT_FADER_GROUP_WIDTH == 64
	;; print MBMF_GROUP_OFFSET / 64 (1 digit)
	swapf	MBMF_GROUP_OFFSET, W, BANKED
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
;;  This help function prints 8 bars of the current fader row
;;  IN:  first fader entry in WREG
;;       current fader in MBMF_LAST_FADER0
;;  OUT: 8 characters
;;  USES: MBMF_CURRENT_FADER, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_8FaderBars
	;; store selected fader row in MBMF_CURRENT_FADER
	SET_BSR	MBMF_BASE
	andlw 	0xf8
	movwf	MBMF_CURRENT_FADER, BANKED
	
	;; store current fader number in TMP1
	movf	MBMF_LAST_FADER0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last fader unequal to selected row
	andlw 	0xf0
	xorwf	MBMF_CURRENT_FADER, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MBMF_CURRENT_FADER, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MBMF_LCD_xFaderBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current fader row
;;  IN:  first fader entry in WREG
;;       current fader in MBMF_LAST_FADER0
;;  OUT: 16 characters
;;  USES: MBMF_CURRENT_FADER, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_16FaderBars
	;; store selected pot row in MB64_CURRENT_FADER
	SET_BSR	MBMF_BASE
	andlw 	0xf0
	movwf	MBMF_CURRENT_FADER, BANKED
	
	;; store current pot number in TMP1
	movf	MBMF_LAST_FADER0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last pot unequal to selected row
	andlw 	0xf0
	xorwf	MBMF_CURRENT_FADER, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MBMF_CURRENT_FADER, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MBMF_LCD_xFaderBars

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current pot row
;;  IN:  first displayed pot in MBMF_CURRENT_FADER
;;       current pot (which should flash) in TMP1
;;          (if TMP1[7] set, no pot entry will be flashed)
;;       last displayed pot in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MBMF_CURRENT_FADER, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_xFaderBars
MBMF_LCD_xFaderBars_Loop
	;; if printed pot number matches with current pot number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MBMF_LCD_xFaderBars_Loop_NoFlash
	movf	TMP1, W
	cpfseq	MBMF_CURRENT_FADER, BANKED
	rgoto MBMF_LCD_xFaderBars_Loop_NoFlash
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MBMF_LCD_xFaderBars_Loop_Next
MBMF_LCD_xFaderBars_Loop_NoFlash
	call	MBMF_FADER_Map	; special MBMF feature: map fader to parameter entry
	rcall	MBMF_LCD_PrnVBar
MBMF_LCD_xFaderBars_Loop_Next
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_FADER, F, BANKED
	movf	TMP2, W
	cpfsgt	MBMF_CURRENT_FADER, BANKED
	rgoto MBMF_LCD_xFaderBars_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints 8 bars of the current button row
;;  IN:  current button number in WREG
;;  OUT: 8 characters
;;  USES: MBMF_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_8ButtonBars
	;; save current button number in TMP1
	SET_BSR	MBMF_BASE
	movwf	TMP1

	;; begin with 0, 8, 16, 24, ...
	movwf	MBMF_CURRENT_BUTTON, BANKED
	movlw	0xf8
	andwf	MBMF_CURRENT_BUTTON, F, BANKED

	;; last displayed button in TMP2
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MBMF_LCD_xButtonBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current button row
;;  IN:  current button number in WREG
;;  OUT: 16 characters
;;  USES: MBMF_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_16ButtonBars
	;; save current button number in TMP1
	SET_BSR	MBMF_BASE
	movwf	TMP1

	;; begin with 0, 16, 32, ...
	movwf	MBMF_CURRENT_BUTTON, BANKED
	movlw	0xf0
	andwf	MBMF_CURRENT_BUTTON, F, BANKED

	;; last displayed button in TMP2
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MBMF_LCD_xButtonBars

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current button row
;;  IN:  first displayed button in MBMF_CURRENT_BUTTON
;;       current button (which should flash) in TMP1
;;       last displayed button in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MBMF_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_xButtonBars
MBMF_LCD_xButtonBars_Loop
	;; if printed button number matches with current button number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MBMF_LCD_xButtonBars_Loop_NoFl
	movf	TMP1, W
	cpfseq	MBMF_CURRENT_BUTTON, BANKED
	rgoto MBMF_LCD_xButtonBars_Loop_NoFl
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MBMF_LCD_xButtonBars_Loop_Next
MBMF_LCD_xButtonBars_Loop_NoFl
	rcall	MBMF_LCD_PrnBtnStatus
MBMF_LCD_xButtonBars_Loop_Next
	SET_BSR	MBMF_BASE
	incf	MBMF_CURRENT_BUTTON, F, BANKED
	movf	TMP2, W
	cpfsgt	MBMF_CURRENT_BUTTON, BANKED
	rgoto MBMF_LCD_xButtonBars_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the fader label and value
;;  IN:  fader number in WREG
;;  OUT: 16 characters
;;  USES: MBMF_CURRENT_FADER, TMP[12]
;; --------------------------------------------------------------------------
MBMF_LCD_PrintLabel
	movff	WREG, MBMF_CURRENT_FADER
	call	MBMF_ADDR_LabelEntry	; get address to label
	call	MBMF_BANK_Read	; read mode byte
	movwf	TMP1		; save mode in TMP1
	andlw	0xff		; fix STATUS
	bnz	MBMF_LCD_PrintLabel_Ok
	;; no label defined, print standard strin
MBMF_LCD_PrintLabel_Std
	movff	MBMF_CURRENT_FADER, WREG
	rgoto	MBMF_LCD_PrintEvntStd_Fader
MBMF_LCD_PrintLabel_Ok
	;; print string
	movlw	13
	btfsc	TMP1, 5
	movlw 12
	movwf	TMP2
MBMF_LCD_PrintLabel_Loop
	call	MBMF_BANK_Read
	call	MIOS_LCD_PrintChar
	decfsz	TMP2, F
	rgoto	MBMF_LCD_PrintLabel_Loop
	
	;; get value
	call	MBMF_FSR_FaderValue
	;; continue depending on value mode
	movf	TMP1, W
	rcall	MBMF_LCD_PrintValue
	CALL_IFSET TMP1, 5, ACCESS, MBMF_LCD_PrnVBar
	return


;; --------------------------------------------------------------------------
;;  This help function prints a value in different number format
;;  IN:   value format in WREG, value in INDF0
;;  OUT:  2/3 characters
;;  USES: -
;; --------------------------------------------------------------------------
MBMF_LCD_PrintValue
	andlw	0x03
	JUMPTABLE_2BYTES 4
	rgoto	MBMF_LCD_PrintValue_Hex
	rgoto	MBMF_LCD_PrintValue_PmHex
	rgoto	MBMF_LCD_PrintValue_Dec
	rgoto	MBMF_LCD_PrintValue_PmDec

	;; print hex value 0x00..0x7f
MBMF_LCD_PrintValue_Hex
	movlw	' '		; fill with space
	call	MIOS_LCD_PrintChar
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintHex2

	;; print hex value -0x40..0x3f
MBMF_LCD_PrintValue_PmHex
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
MBMF_LCD_PrintValue_Dec
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintBCD3

	;; print dec value -64..63
MBMF_LCD_PrintValue_PmDec
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
;;  This help function prints the standard string for a fader value
;;  IN:   fader number in WREG
;;  OUT:  16 characters
;;  USES: MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_LCD_PrintEvntStd_Fader
	;; copy WREG to MBMF_CURRENT_FADER
	movff	WREG, MBMF_CURRENT_FADER
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MBMF_MIDI_PrepareFaderEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print bar
	rgoto	MBMF_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a fader value
;;  IN:   fader number in WREG
;;  OUT:  8 characters
;;  USES: MBMF_CURRENT_FADER
;; --------------------------------------------------------------------------
MBMF_LCD_PrintShortEvntStd_Fader
	;; copy WREG to MBMF_CURRENT_FADER
	movff	WREG, MBMF_CURRENT_FADER
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MBMF_MIDI_PrepareFaderEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print bar
	rgoto	MBMF_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 16 characters
;;  USES: MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_LCD_PrintEvntStd_Btn
	;; copy WREG to MBMF_CURRENT_BUTTON
	movff	WREG, MBMF_CURRENT_BUTTON
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MBMF_MIDI_PrepareButtonEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print button status
	rgoto	MBMF_LCD_PrnBtnStatus

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 8 characters
;;  USES: MBMF_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MBMF_LCD_PrintShortEvntStd_Btn
	;; copy WREG to MBMF_CURRENT_BUTTON
	movff	WREG, MBMF_CURRENT_BUTTON
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MBMF_MIDI_PrepareButtonEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print button status
	rgoto	MBMF_LCD_PrnBtnStatus


;; --------------------------------------------------------------------------
;;  This help function prints the standard string for the learned event
;;  IN:  event in MBMF_LEARN_EVNT[012],
;;       button/fader event selected by MB_STAT_LAST_WAS_BUTTON
;;  OUT: 16 characters
;;  USES: -
;; --------------------------------------------------------------------------
MBMF_LCD_PrintEvntStd_Learn
	movff	MBMF_LEARN_EVNT0, MIDI_EVNT0
	movff	MBMF_LEARN_EVNT1, MIDI_EVNT1
	movff	MBMF_LEARN_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Print
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MBMF_LCD_PrintEvntStd_Learn_B
MBMF_LCD_PrintEvntStd_Learn_P
	movlw	' '
MBMF_LCD_PrintEvntStd_Learn_C
	goto	MIOS_LCD_PrintChar
MBMF_LCD_PrintEvntStd_Learn_B
	movff	MBMF_LEARN_BMODE, WREG
	BRA_IFSET WREG, 1, ACCESS, MBMF_LCD_PrintEvntStd_Learn_B2
	BRA_IFSET WREG, 0, ACCESS, MBMF_LCD_PrintEvntStd_Learn_B1
MBMF_LCD_PrintEvntStd_Learn_B0
	movlw	'*'
	rgoto	MBMF_LCD_PrintEvntStd_Learn_C
MBMF_LCD_PrintEvntStd_Learn_B1
	movlw	'O'
	rgoto	MBMF_LCD_PrintEvntStd_Learn_C
MBMF_LCD_PrintEvntStd_Learn_B2
	movlw	'T'
	rgoto	MBMF_LCD_PrintEvntStd_Learn_C
	
;; ==========================================================================
;; print vertical bar depending on fader status (value & soft-takeover flag)
;; use MBMF_LCD_PrnVBar_NoArw to skip the soft-takeover indication arrows
;; IN:   fader number in MBMF_CURRENT_FADER
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MBMF_LCD_PrnVBar_NoArw
MBMF_LCD_PrnVBar
MBMF_LCD_PrnVBar_Normal
	;; get pointer to fader value and soft-takeover status
	call	MBMF_FSR_FaderValue
	swapf	INDF0, W	; saved value / 16
	andlw	0x07
MBMF_LCD_PrnVBar_Now
	;; finally print character and exit
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the button status (* if pressed, o if depressed)
;; IN:   button number in MBMF_CURRENT_BUTTON
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MBMF_LCD_PrnBtnStatus
	;; get button state
	call	MBMF_FSR_ButtonValue
	movff	MBMF_CURRENT_BUTTON, WREG
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
MBMF_LCD_PrnPatch
	BRA_IFSET WREG, 7, ACCESS, MBMF_LCD_PrnPatch_Dis
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

MBMF_LCD_PrnPatch_Dis
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

MBMF_LCD_PrintBNameRAM
#if DEFAULT_PATCHMODE == 0
MBMF_LCD_PrintBankName
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MBMF_LCD_PrintBnkNameEmpty
#else
MBMF_LCD_PrintPatchName
	lfsr	FSR0, MBMF_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MBMF_LCD_PrintPatchNameEmpty
#endif


	clrf	TMP1		; used as loop counter
MBMF_LCD_PrintBNameRAMLoop
	movff	POSTINC0, TMP2
	movlw	0x20
	cpfslt	TMP2, ACCESS
	rgoto MBMF_LCD_PrintBNameRAMLoopOk
	movlw	' '
	movwf	TMP2
MBMF_LCD_PrintBNameRAMLoopOk
	movf	TMP2, W
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_LCD_PrintBNameRAMLoop
	return

;; ---

MBMF_LCD_PrintBNameEEP
#if DEFAULT_PATCHMODE == 0
MBMF_LCD_PrintPatchName
	; if not in PATCHMODE, we don't have Patch names in EEPROM
	rgoto	MBMF_LCD_PrintPatchNameEmpty
#else
MBMF_LCD_PrintBankName
	call	MBMF_ADDR_Bankname
	call	MBMF_BANK_Read
	bz	MBMF_LCD_PrintBnkNameEmpty
#endif

	call	MIOS_LCD_PrintChar

	clrf	TMP1		; used as loop counter
	incf	TMP1, F
MBMF_LCD_PrintBNameEEPLoop
	call	MBMF_BANK_Read
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MBMF_LCD_PrintBNameEEPLoop
	return

;; ---
MBMF_LCD_PrintBnkNameEmpty
	TABLE_ADDR MBMF_LCD_BNAME_EMPTY_STR
	movf	MB_BANK, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MBMF_SELECTED_BANK, WREG
	rgoto	MBMF_LCD_PrintBPNameEmptyC

MBMF_LCD_PrintPatchNameEmpty
	TABLE_ADDR MBMF_LCD_PNAME_EMPTY_STR
	movf	MB_PATCH, W
	btfsc	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	movff	MBMF_SELECTED_PATCH, WREG
	iorlw	0x80
	;; 	rgoto	MBMF_LCD_PrintBPNameEmptyC

MBMF_LCD_PrintBPNameEmptyC
	movwf	TMP1
	tblrd*+
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintPreconfString
	BRA_IFSET TMP1, 7, ACCESS, MBMF_LCD_PrintBPNameEmptyCP
MBMF_LCD_PrintBPNameEmptyCB
	incf	TMP1, W
	call	MIOS_LCD_PrintBCD2
	rgoto	MBMF_LCD_PrintBPNameEmptyCC
MBMF_LCD_PrintBPNameEmptyCP
	movf	TMP1, W
	andlw	0x7f
	rcall	MBMF_LCD_PrnPatch
	;; 	rgoto	MBMF_LCD_PrintBPNameEmptyCC
MBMF_LCD_PrintBPNameEmptyCC
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'>'
	goto	MIOS_LCD_PrintChar

MBMF_LCD_BNAME_EMPTY_STR
	STRING	12, 0x00, "< new bank  "
MBMF_LCD_PNAME_EMPTY_STR
	STRING	12, 0x00, "< new patch "



;; --------------------------------------------------------------------------
;;  This help function prints a horizontal bar
;;  IN: value (0x00-0x7f) in WREG
;;  OUT: 16 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MBMF_LCD_PrintHBar
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
MBMF_LCD_PrintHBar_Loop
	movf	TMP3, W
	cpfseq	TMP2, ACCESS
	rgoto MBMF_LCD_PrintHBar_Loop_M12
MBMF_LCD_PrintHBar_Loop_Mark
	incf	TMP1, F		; only valid for panorama: increment "#" pos
	movlw	'|'		; print vertical bar
	rgoto	MBMF_LCD_PrintHBar_Loop_Cont

MBMF_LCD_PrintHBar_Loop_M12
	bnz	MBMF_LCD_PrintHBar_Loop_M12C
	movlw	'>'
	rgoto	MBMF_LCD_PrintHBar_Loop_Cont
MBMF_LCD_PrintHBar_Loop_M12C
	movf	TMP1, W
	cpfsgt	TMP3, ACCESS
	rgoto MBMF_LCD_PrintHBar_Loop_M2
MBMF_LCD_PrintHBar_Loop_M1
	movlw	'-'
	rgoto	MBMF_LCD_PrintHBar_Loop_Cont
MBMF_LCD_PrintHBar_Loop_M2
	movlw	'#'
	;; 	rgoto	MBMF_LCD_PrintHBar_Loop_Cont
MBMF_LCD_PrintHBar_Loop_Cont
	call	MIOS_LCD_PrintChar

	incf	TMP3, F		; increment
	movlw	16
	cpfseq	TMP3, ACCESS
	rgoto MBMF_LCD_PrintHBar_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the selected morph groups
;;  IN: MBMF_MORPH_GROUP_SELECT
;;  OUT: 8 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MBMF_LCD_PrintMorphGroups
	movff	MBMF_MORPH_GROUP_SELECT, TMP1	; TMP1 saves the group flags, it will be shifted with every iteration
	clrf	TMP2				; TMP2 used as loop counter
MBMF_LCD_PrintMorphGroupsLoop
	movlw	'-'
	BRA_IFCLR TMP1, 0, ACCESS, MBMF_LCD_PrintMorphGroupsLoopC
	movf	TMP2, W
	addlw	'1'
MBMF_LCD_PrintMorphGroupsLoopC
	call	MIOS_LCD_PrintChar

	rrf	TMP1, F
	incf	TMP2, F
	BRA_IFCLR TMP2, 3, ACCESS, MBMF_LCD_PrintMorphGroupsLoop
	return


;; --------------------------------------------------------------------------
;;  This help function prints the values of all morph groups as V bars
;;  IN:  -
;;  OUT: 8 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MBMF_LCD_PrintMorphBars
	clrf	TMP1				; using TMP1 as loop counter
	lfsr	FSR0, MBMF_MORPH_VALUES_G0
MBMF_LCD_PrintMorphBarsLoop
	swapf	POSTINC0, W
	andlw	0x07
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 3, ACCESS, MBMF_LCD_PrintMorphBarsLoop
	return


;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
MBMF_LCD_InitVBars
	;; exit if GLCD connected
	btfsc	CS_STAT, CS_STAT_DISPLAY_GLCD
	return

	;; init special chars and exit
	TABLE_ADDR MBMF_LCD_V_BARS
	goto	MIOS_CLCD_SpecialCharsInit

	;; for vertical bars
MBMF_LCD_V_BARS
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
