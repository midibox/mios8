; $Id$
;
; MB64 LCD functions, mainly used by cs_m_display.inc
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
MB64_LCD_SetDefaultOffsets
	movlw	DEFAULT_YOFFSET_LINE0
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_YOFFSET_LINE1
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_YOFFSET_LINE2
	movwf	MIOS_PARAMETER3
	movlw	DEFAULT_YOFFSET_LINE3
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
;;  This functions sets the default Y offsets
;;  IN:  -
;;  OUT: -
;; --------------------------------------------------------------------------
MB64_LCD_SetCSMDOffsets
	movlw	CSMD_YOFFSET_LINE0
	movwf	MIOS_PARAMETER1
	movlw	CSMD_YOFFSET_LINE1
	movwf	MIOS_PARAMETER2
	movlw	CSMD_YOFFSET_LINE2
	movwf	MIOS_PARAMETER3
	movlw	CSMD_YOFFSET_LINE3
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
;;  This help function prints the bank information
;;  IN:  -
;;  OUT: 8 characters
;; --------------------------------------------------------------------------
MB64_LCD_PrintBank
	movlw	'['
	call	MIOS_LCD_PrintChar

#if DEFAULT_ENABLE_PAGE_HANDLING == 0
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_LCD_PrintBank_Int
MB64_LCD_PrintBank_Ext
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD3
	rgoto	MB64_LCD_PrintBank_Cont

MB64_LCD_PrintBank_Int
	movlw	'I'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	't'
	call	MIOS_LCD_PrintChar

MB64_LCD_PrintBank_Cont
	movlw	']'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar

#else

	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_LCD_PrintBank_Int	
MB64_LCD_PrintBank_Ext
	incf	MB_BANK, W
	call	MIOS_LCD_PrintBCD2		;;[ 1.4]N
	movlw	'.'
	call	MIOS_LCD_PrintChar
	
	movff   MB64_PAGE, WREG
	addlw	0x1
	call	MIOS_LCD_PrintBCD1	
	rgoto	MB64_LCD_PrintBank_Cont
	
MB64_LCD_PrintBank_Int
	movlw	'I'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar	
	
	movlw	't'
	call	MIOS_LCD_PrintChar		;;[I.4] N
	
	movff   MB64_PAGE, WREG
	addlw	0x1
	call	MIOS_LCD_PrintBCD1	
	
MB64_LCD_PrintBank_Cont
	movlw	']'
	call	MIOS_LCD_PrintChar
#endif


	SET_BSR	MB64_BASE
	movlw	'N'
	btfsc	MB64_CFG0, MB64_CFG0_SNAP, BANKED
	movlw 'S'
	btfsc	MB64_CFG0, MB64_CFG0_RELATIVE, BANKED
	movlw 'R'
	btfsc	MB64_CFG0, MB64_CFG0_PARALLAX, BANKED
	movlw 'P'
	call	MIOS_LCD_PrintChar
	movlw	' '
	goto	MIOS_LCD_PrintChar
	

;; --------------------------------------------------------------------------
;;  This help function prints 8 bars of the current pot row
;;  IN:  first pot entry in WREG
;;       current pot in MB64_LAST_POT0
;;  OUT: 8 characters
;;  USES: MB64_CURRENT_POT, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_8PotBars
	;; store selected pot row in MB64_CURRENT_POT
	SET_BSR	MB64_BASE
	andlw 	0x78
	movwf	MB64_CURRENT_POT, BANKED
	
	;; store current pot number in TMP1
	movf	MB64_LAST_POT0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last pot unequal to selected row
	andlw 	0x70
	xorwf	MB64_CURRENT_POT, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64_CURRENT_POT, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MB64_LCD_xPotBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current pot row
;;  IN:  first pot entry in WREG
;;       current pot in MB64_LAST_POT0
;;  OUT: 16 characters
;;  USES: MB64_CURRENT_POT, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_16PotBars
	;; store selected pot row in MB64_CURRENT_POT
	SET_BSR	MB64_BASE
	andlw 	0x70
	movwf	MB64_CURRENT_POT, BANKED
	
	;; store current pot number in TMP1
	movf	MB64_LAST_POT0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last pot unequal to selected row
	andlw 	0x70
	xorwf	MB64_CURRENT_POT, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64_CURRENT_POT, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MB64_LCD_xPotBars
	

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current pot row
;;  IN:  first displayed pot in MB64_CURRENT_POT
;;       current pot (which should flash) in TMP1
;;          (if TMP1[7] set, no pot entry will be flashed)
;;       last displayed pot in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MB64_CURRENT_POT, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_xPotBars
MB64_LCD_xPotBars_Loop
	;; if printed pot number matches with current pot number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MB64_LCD_xPotBars_Loop_NoFlash
	movf	TMP1, W
	cpfseq	MB64_CURRENT_POT, BANKED
	rgoto MB64_LCD_xPotBars_Loop_NoFlash
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MB64_LCD_xPotBars_Loop_Next
MB64_LCD_xPotBars_Loop_NoFlash
	rcall	MB64_LCD_PrnVBar
MB64_LCD_xPotBars_Loop_Next
	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_POT, F, BANKED
	movf	TMP2, W
	cpfsgt	MB64_CURRENT_POT, BANKED
	rgoto MB64_LCD_xPotBars_Loop

	return

;; --------------------------------------------------------------------------
;;  This help function prints 8 bars of the current button row
;;  IN:  first button entry in WREG
;;       current button in MB64_LAST_BUTTON0
;;  OUT: 8 characters
;;  USES: MB64_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_8ButtonBars
	;; store selected button row in MB64_CURRENT_BUTTON
	SET_BSR	MB64_BASE
	andlw 	0x78
	movwf	MB64_CURRENT_BUTTON, BANKED
	
	;; store current button number in TMP1
	movf	MB64_LAST_BUTTON0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last button unequal to selected row
	andlw 	0x70
	xorwf	MB64_CURRENT_BUTTON, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64_CURRENT_BUTTON, W, BANKED
	addlw	7		; +7
	movwf	TMP2

	rgoto	MB64_LCD_xButtonBars

;; --------------------------------------------------------------------------
;;  This help function prints 16 bars of the current button row
;;  IN:  first button entry in WREG
;;       current button in MB64_LAST_BUTTON0
;;  OUT: 16 characters
;;  USES: MB64_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_16ButtonBars
	;; store selected button row in MB64_CURRENT_BUTTON
	SET_BSR	MB64_BASE
	andlw 	0x70
	movwf	MB64_CURRENT_BUTTON, BANKED
	
	;; store current button number in TMP1
	movf	MB64_LAST_BUTTON0, W, BANKED
	movwf	TMP1

	;; don´t flash if row of last button unequal to selected row
	andlw 	0x70
	xorwf	MB64_CURRENT_BUTTON, W, BANKED
	skpz
	bsf	TMP1, 7

	movf	MB64_CURRENT_BUTTON, W, BANKED
	addlw	15		; +15
	movwf	TMP2

	rgoto	MB64_LCD_xButtonBars
	

;; --------------------------------------------------------------------------
;;  This help function prints x bars of the current button row
;;  IN:  first displayed button in MB64_CURRENT_BUTTON
;;       current button (which should flash) in TMP1
;;          (if TMP1[7] set, no pot entry will be flashed)
;;       last displayed button in TMP2
;;  OUT: TMP2-TMP1 characters
;;  USES: MB64_CURRENT_BUTTON, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_xButtonBars
MB64_LCD_xButtonBars_Loop
	;; if printed button number matches with current button number: flash entry
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, MB64_LCD_xButtonBars_Loop_NoFl
	movf	TMP1, W
	cpfseq	MB64_CURRENT_BUTTON, BANKED
	rgoto MB64_LCD_xButtonBars_Loop_NoFl
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	MB64_LCD_xButtonBars_Loop_Next
MB64_LCD_xButtonBars_Loop_NoFl
	rcall	MB64_LCD_PrnBtnStatus
MB64_LCD_xButtonBars_Loop_Next
	SET_BSR	MB64_BASE
	incf	MB64_CURRENT_BUTTON, F, BANKED
	movf	TMP2, W
	cpfsgt	MB64_CURRENT_BUTTON, BANKED
	rgoto MB64_LCD_xButtonBars_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the pot label and value
;;  IN:  pot number in WREG
;;  OUT: 16 characters
;;  USES: MB64_CURRENT_POT, TMP[12]
;; --------------------------------------------------------------------------
MB64_LCD_PrintLabel
	movff	WREG, MB64_CURRENT_POT
	call	MB64_ADDR_LabelEntry	; get address to label
	call	MB64_BANK_Read	; read mode byte
	movwf	TMP1		; save mode in TMP1
	andlw	0xff		; fix STATUS
	bnz	MB64_LCD_PrintLabel_Ok
	;; no label defined, print standard strin
MB64_LCD_PrintLabel_Std
	movff	MB64_CURRENT_POT, WREG
	rgoto	MB64_LCD_PrintEvntStd_Pot
MB64_LCD_PrintLabel_Ok
	;; print string
	movlw	13
	btfsc	TMP1, 5
	movlw 12
	movwf	TMP2
MB64_LCD_PrintLabel_Loop
	call	MB64_BANK_Read
	call	MIOS_LCD_PrintChar
	decfsz	TMP2, F
	rgoto	MB64_LCD_PrintLabel_Loop
	
	;; get value
	call	MB64_FSR_PotValue
	;; continue depending on value mode
	movf	TMP1, W
	rcall	MB64_LCD_PrintValue
	CALL_IFSET TMP1, 5, ACCESS, MB64_LCD_PrnVBar
	return


;; --------------------------------------------------------------------------
;;  This help function prints a value in different number format
;;  IN:   value format in WREG, value in INDF0
;;  OUT:  2/3 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64_LCD_PrintValue
	andlw	0x03
	JUMPTABLE_2BYTES 4
	rgoto	MB64_LCD_PrintValue_Hex
	rgoto	MB64_LCD_PrintValue_PmHex
	rgoto	MB64_LCD_PrintValue_Dec
	rgoto	MB64_LCD_PrintValue_PmDec

	;; print hex value 0x00..0x7f
MB64_LCD_PrintValue_Hex
	movlw	' '		; fill with space
	call	MIOS_LCD_PrintChar
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintHex2

	;; print hex value -0x40..0x3f
MB64_LCD_PrintValue_PmHex
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
MB64_LCD_PrintValue_Dec
	movf	INDF0, W
	andlw	0x7f
	goto	MIOS_LCD_PrintBCD3

	;; print dec value -64..63
MB64_LCD_PrintValue_PmDec
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
;;  This help function prints the standard string for a pot value
;;  IN:   pot number in WREG
;;  OUT:  16 characters
;;  USES: MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_LCD_PrintEvntStd_Pot
	;; copy WREG to MB64_CURRENT_POT
	movff	WREG, MB64_CURRENT_POT
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64_MIDI_PreparePotEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print bar
	rgoto	MB64_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a pot value
;;  IN:   pot number in WREG
;;  OUT:  8 characters
;;  USES: MB64_CURRENT_POT
;; --------------------------------------------------------------------------
MB64_LCD_PrintShortEvntStd_Pot
	;; copy WREG to MB64_CURRENT_POT
	movff	WREG, MB64_CURRENT_POT
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64_MIDI_PreparePotEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print bar
	rgoto	MB64_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This help function prints the standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 16 characters
;;  USES: MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_LCD_PrintEvntStd_Btn
	;; copy WREG to MB64_CURRENT_BUTTON
	movff	WREG, MB64_CURRENT_BUTTON
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64_MIDI_PrepareButtonEvent
	;; print MIDI event
	call	MIDI_EVNT_Print
	;; print button status
	rgoto	MB64_LCD_PrnBtnStatus

;; --------------------------------------------------------------------------
;;  This help function prints the short standard string for a button value
;;  IN:  button number in WREG
;;  OUT: 8 characters
;;  USES: MB64_CURRENT_BUTTON
;; --------------------------------------------------------------------------
MB64_LCD_PrintShortEvntStd_Btn
	;; copy WREG to MB64_CURRENT_BUTTON
	movff	WREG, MB64_CURRENT_BUTTON
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; prepare MIDI_EVNT[01] and MIDI_EVNT_VALUE
	call	MB64_MIDI_PrepareButtonEvent
	;; print MIDI event in short form
	call	MIDI_EVNT_PrintShort
	;; print button status
	rgoto	MB64_LCD_PrnBtnStatus


;; --------------------------------------------------------------------------
;;  This help function prints the standard string for the learned event
;;  IN:  event in MB64_LEARN_EVNT[012],
;;       button/pot event selected by MB_STAT_LAST_WAS_BUTTON
;;  OUT: 16 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64_LCD_PrintEvntStd_Learn
	movff	MB64_LEARN_EVNT0, MIDI_EVNT0
	movff	MB64_LEARN_EVNT1, MIDI_EVNT1
	movff	MB64_LEARN_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Print
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MB64_LCD_PrintEvntStd_Learn_B
MB64_LCD_PrintEvntStd_Learn_P
	movlw	' '
MB64_LCD_PrintEvntStd_Learn_C
	goto	MIOS_LCD_PrintChar
MB64_LCD_PrintEvntStd_Learn_B
	movff	MB64_LEARN_BMODE, WREG
	BRA_IFSET WREG, 1, ACCESS, MB64_LCD_PrintEvntStd_Learn_B2
	BRA_IFSET WREG, 0, ACCESS, MB64_LCD_PrintEvntStd_Learn_B1
MB64_LCD_PrintEvntStd_Learn_B0
	movlw	'*'
	rgoto	MB64_LCD_PrintEvntStd_Learn_C
MB64_LCD_PrintEvntStd_Learn_B1
	movlw	'O'
	rgoto	MB64_LCD_PrintEvntStd_Learn_C
MB64_LCD_PrintEvntStd_Learn_B2
	movlw	'T'
	rgoto	MB64_LCD_PrintEvntStd_Learn_C
	
;; ==========================================================================
;; print vertical bar depending on pot status (value & soft-takeover flag)
;; use MB64_LCD_PrnVBar_NoArw to skip the soft-takeover indication arrows
;; IN:   pot number in MB64_CURRENT_POT
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MB64_LCD_PrnVBar_NoArw
	;; print bar w/o overtake status
	rgoto	MB64_LCD_PrnVBar_Normal

MB64_LCD_PrnVBar
	;; get pointer to pot value and soft-takeover status
	call	MB64_FSR_PotValue
	;; if softovertake-flag set, print bar
	BRA_IFSET INDF0, 7, ACCESS, MB64_LCD_PrnVBar_Normal
	;; if not in snap mode, print bar
	SET_BSR	MB64_BASE
	BRA_IFCLR MB64_CFG0, MB64_CFG0_SNAP, BANKED, MB64_LCD_PrnVBar_Normal
	;; else print arrows depending on direction
MB64_LCD_PrnVBar_Arrows
	;; get current pot value (7-bit) and save it in TMP1
	SET_BSR	MB64_BASE
	movf	MB64_CURRENT_POT, W, BANKED
	call	MIOS_AIN_Pin7bitGet
	subwf	INDF0, W	; INDF0 contains the saved value, overtake-flag is cleared here
	bz	MB64_LCD_PrnVBar_Normal
	movlw	0x7f		; '>'
	skpnc
	movlw	0x7e		; '<'
	rgoto	MB64_LCD_PrnVBar_Now

MB64_LCD_PrnVBar_Normal
	;; get pointer to pot value and soft-takeover status
	call	MB64_FSR_PotValue
	swapf	INDF0, W	; saved value / 16
	andlw	0x07
MB64_LCD_PrnVBar_Now
	;; finally print character and exit
	goto	MIOS_LCD_PrintChar


;; ==========================================================================
;; print the button status (* if pressed, o if depressed)
;; IN:   button number in MB64_CURRENT_BUTTON
;; OUT:  1 character
;; USES: TMP1
;; ==========================================================================
MB64_LCD_PrnBtnStatus
	;; get button state
	call	MB64_FSR_ButtonValue
	movff	MB64_CURRENT_BUTTON, WREG
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W

	;; print character
	movlw	'*'		; button pressed
	skpz
	movlw	'o'		; button depressed
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  This functions prints the MIDI clock which is traced by midiclk.inc
;;  IN:  -
;;  OUT: 16 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64_LCD_PrintClk
	movf	LEDDIGITS_7, W
	call	MB64_LCD_PrintClk_Hlp
	movf	LEDDIGITS_6, W
	call	MB64_LCD_PrintClk_Hlp
	movf	LEDDIGITS_5, W
	call	MB64_LCD_PrintClk_Hlp
	movf	LEDDIGITS_4, W
	call	MB64_LCD_PrintClk_Hlp_BCD
	movlw	'.'
	call	MB64_LCD_PrintClk_Hlp_Chr
	movf	LEDDIGITS_3, W
	call	MB64_LCD_PrintClk_Hlp
	movf	LEDDIGITS_2, W
	call	MB64_LCD_PrintClk_Hlp_BCD
	movlw	'.'
	call	MB64_LCD_PrintClk_Hlp_Chr
	movf	LEDDIGITS_1, W
	call	MB64_LCD_PrintClk_Hlp
	movf	LEDDIGITS_0, W
	call	MB64_LCD_PrintClk_Hlp_BCD

	movlw	3
	rcall	MB64_LCD_PrintSpaces

	call	BPM_Get
	goto	MIOS_LCD_PrintBCD3

;; this help function prints a space if bit 4 set, else the BCD
MB64_LCD_PrintClk_Hlp
	BRA_IFSET WREG, 4, ACCESS, MB64_LCD_PrintClk_Hlp_Spc
MB64_LCD_PrintClk_Hlp_BCD
	goto	MIOS_LCD_PrintBCD1
MB64_LCD_PrintClk_Hlp_Spc
	movlw	' '
MB64_LCD_PrintClk_Hlp_Chr
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This functions prints the MTC which is traced by mtc.inc
;;  IN:  -
;;  OUT: 16 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64_LCD_PrintMTC_STR
	db	"MTC: "
MB64_LCD_PrintMTC
	TABLE_ADDR MB64_LCD_PrintMTC_STR
	movlw	5
	call	MIOS_LCD_PrintPreconfString

	movf	LEDDIGITS_7, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_6, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MB64_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_5, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_4, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MB64_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_3, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_2, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movlw	':'
	rcall	MB64_LCD_PrintMTC_Hlp_Chr
	movf	LEDDIGITS_1, W
	rcall	MB64_LCD_PrintMTC_Hlp_BCD
	movf	LEDDIGITS_0, W
	rgoto	MB64_LCD_PrintMTC_Hlp_BCD

;; this help function prints a space if bit 4 set, else the BCD
MB64_LCD_PrintMTC_Hlp
	BRA_IFSET WREG, 4, ACCESS, MB64_LCD_PrintClk_Hlp_Spc
MB64_LCD_PrintMTC_Hlp_BCD
	goto	MIOS_LCD_PrintBCD1
MB64_LCD_PrintMTC_Hlp_Spc
	movlw	' '
MB64_LCD_PrintMTC_Hlp_Chr
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  This functions prints either the MTC or the MIDI clock, depending on last event
;;  IN:  -
;;  OUT: 20 characters
;;  USES: -
;; --------------------------------------------------------------------------
MB64_LCD_PrintClkOrMTC
	;; if MIDI clock should be displayed, do so
	BRA_IFSET MB_STAT, MB_STAT_SHOW_MIDICLK, ACCESS, MB64_LCD_PrintClk
	;; if MTC should be displayed, do so
	BRA_IFSET MB_STAT, MB_STAT_SHOW_MTC, ACCESS, MB64_LCD_PrintMTC
	;; default: print MTC
	rgoto	MB64_LCD_PrintMTC

;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: TMP5
;; --------------------------------------------------------------------------
MB64_LCD_PrintSpaces
	movwf	TMP5
MB64_LCD_PrintSpacesLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP5, F
	rgoto	MB64_LCD_PrintSpacesLoop
	return

;; ==========================================================================
;; print patch ID (uses TMP1)
;; ==========================================================================
MB64_LCD_PrnPatch
	BRA_IFSET WREG, 7, ACCESS, MB64_LCD_PrnPatch_Dis
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

MB64_LCD_PrnPatch_Dis
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

MB64_LCD_PrintBNameRAM
#if DEFAULT_PATCHMODE == 0
MB64_LCD_PrintBankName
	lfsr	FSR0, MB64_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MB64_LCD_PrintBnkNameEmpty
#else
MB64_LCD_PrintPatchName
	lfsr	FSR0, MB64_BANKNAME_BEGIN
	movf	INDF0, W
	bz	MB64_LCD_PrintPatchNameEmpty
#endif


	clrf	TMP1		; used as loop counter
MB64_LCD_PrintBNameRAMLoop
	movff	POSTINC0, TMP2
	movlw	0x20
	cpfslt	TMP2, ACCESS
	rgoto MB64_LCD_PrintBNameRAMLoopOk
	movlw	' '
	movwf	TMP2
MB64_LCD_PrintBNameRAMLoopOk
	movf	TMP2, W
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_LCD_PrintBNameRAMLoop
	return

;; ---

MB64_LCD_PrintBNameEEP
#if DEFAULT_PATCHMODE == 0
MB64_LCD_PrintPatchName
	; if not in PATCHMODE, we don't have Patch names in EEPROM
	rgoto	MB64_LCD_PrintPatchNameEmpty
#else
MB64_LCD_PrintBankName
	call	MB64_ADDR_Bankname
	call	MB64_BANK_Read
	bz	MB64_LCD_PrintBnkNameEmpty
#endif

	call	MIOS_LCD_PrintChar

	clrf	TMP1		; used as loop counter
	incf	TMP1, F
MB64_LCD_PrintBNameEEPLoop
	call	MB64_BANK_Read
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, MB64_LCD_PrintBNameEEPLoop
	return

;; ---
MB64_LCD_PrintBnkNameEmpty
	TABLE_ADDR MB64_LCD_BNAME_EMPTY_STR
	movf	MB_BANK, W
	rgoto	MB64_LCD_PrintBPNameEmptyC

MB64_LCD_PrintPatchNameEmpty
	TABLE_ADDR MB64_LCD_PNAME_EMPTY_STR
	movf	MB_PATCH, W
	iorlw	0x80
	;; 	rgoto	MB64_LCD_PrintBPNameEmptyC

MB64_LCD_PrintBPNameEmptyC
	movwf	TMP1
	tblrd*+
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintPreconfString
	BRA_IFSET TMP1, 7, ACCESS, MB64_LCD_PrintBPNameEmptyCP
MB64_LCD_PrintBPNameEmptyCB
	incf	TMP1, W
	call	MIOS_LCD_PrintBCD2
	rgoto	MB64_LCD_PrintBPNameEmptyCC
MB64_LCD_PrintBPNameEmptyCP
	movf	TMP1, W
	andlw	0x7f
	rcall	MB64_LCD_PrnPatch
	;; 	rgoto	MB64_LCD_PrintBPNameEmptyCC
MB64_LCD_PrintBPNameEmptyCC
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'>'
	goto	MIOS_LCD_PrintChar

MB64_LCD_BNAME_EMPTY_STR
	STRING	12, 0x00, "< new bank  "
MB64_LCD_PNAME_EMPTY_STR
	STRING	12, 0x00, "< new patch "



;; --------------------------------------------------------------------------
;;  This help function prints a horizontal bar
;;  IN: value (0x00-0x7f) in WREG
;;  OUT: 16 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64_LCD_PrintHBar
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
MB64_LCD_PrintHBar_Loop
	movf	TMP3, W
	cpfseq	TMP2, ACCESS
	rgoto MB64_LCD_PrintHBar_Loop_M12
MB64_LCD_PrintHBar_Loop_Mark
	incf	TMP1, F		; only valid for panorama: increment "#" pos
	movlw	'|'		; print vertical bar
	rgoto	MB64_LCD_PrintHBar_Loop_Cont

MB64_LCD_PrintHBar_Loop_M12
	bnz	MB64_LCD_PrintHBar_Loop_M12C
	movlw	'>'
	rgoto	MB64_LCD_PrintHBar_Loop_Cont
MB64_LCD_PrintHBar_Loop_M12C
	movf	TMP1, W
	cpfsgt	TMP3, ACCESS
	rgoto MB64_LCD_PrintHBar_Loop_M2
MB64_LCD_PrintHBar_Loop_M1
	movlw	'-'
	rgoto	MB64_LCD_PrintHBar_Loop_Cont
MB64_LCD_PrintHBar_Loop_M2
	movlw	'#'
	;; 	rgoto	MB64_LCD_PrintHBar_Loop_Cont
MB64_LCD_PrintHBar_Loop_Cont
	call	MIOS_LCD_PrintChar

	incf	TMP3, F		; increment
	movlw	16
	cpfseq	TMP3, ACCESS
	rgoto MB64_LCD_PrintHBar_Loop

	return


;; --------------------------------------------------------------------------
;;  This help function prints the selected morph groups
;;  IN: MB64_MORPH_GROUP_SELECT
;;  OUT: 4 characters
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MB64_LCD_PrintMorphGroups
	movff	MB64_MORPH_GROUP_SELECT, TMP1	; TMP1 saves the group flags, it will be shifted with every iteration
	clrf	TMP2				; TMP2 used as loop counter
MB64_LCD_PrintMorphGroupsLoop
	movlw	'-'
	BRA_IFCLR TMP1, 0, ACCESS, MB64_LCD_PrintMorphGroupsLoopC
	movf	TMP2, W
	addlw	'1'
MB64_LCD_PrintMorphGroupsLoopC
	call	MIOS_LCD_PrintChar

	rrf	TMP1, F
	incf	TMP2, F
	BRA_IFCLR TMP2, 2, ACCESS, MB64_LCD_PrintMorphGroupsLoop
	return


;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
MB64_LCD_InitVBars	
	;; exit if GLCD connected
	btfsc	CS_STAT, CS_STAT_DISPLAY_GLCD
	return

	;; init special chars and exit
	TABLE_ADDR MB64_LCD_V_BARS
	goto	MIOS_CLCD_SpecialCharsInit

	;; for vertical bars
MB64_LCD_V_BARS
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
