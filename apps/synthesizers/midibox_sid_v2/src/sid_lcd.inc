; $Id$
;
; MIDIbox SID LCD routines
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Shortcut to MIOS_LCD_PrintChar (can be called with rcall which consumes only 2 bytes)
;; --------------------------------------------------------------------------
SID_LCD_PrintChar
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This functions prints x chars
;;  IN:  character in PRODL, number of chars in WREG
;;  OUT: x chars
;;  USES: PRODH
;; --------------------------------------------------------------------------
SID_LCD_PrintChars
	movwf	PRODH
SID_LCD_PrintCharsLoop
	movf	PRODL, W
	rcall	SID_LCD_PrintChar
	decfsz	PRODH, F
	rgoto	SID_LCD_PrintCharsLoop
	return

;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: PROD[LH]
;; --------------------------------------------------------------------------
SID_LCD_PrintSpaces
	movwf	PRODH
	movlw	' '
	movwf	PRODL
	rgoto	SID_LCD_PrintCharsLoop

;; --------------------------------------------------------------------------
;;  help function to print an ensemble number
;;  IN: ensemble in SID_ENSEMBLE
;;  OUT: 4 characters
;; --------------------------------------------------------------------------

SID_LCD_PrintEnsembleNumber
	movff	SID_ENSEMBLE, PRODL

SID_LCD_PrintEnsembleNumberPRODL	; label for cs_menu_print.inc to store CS_MENU_SAVE_ENS in PRODL
	movlw	'E'
	call	SID_LCD_PrintChar

	;; print ensemble number, padded with 0
	incf	PRODL, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	goto	MIOS_LCD_PrintHex2

;; --------------------------------------------------------------------------
;;  help function to print a patch number
;;  + '*' if patch or BankStick not accessible
;;  IN: bank in SID_BANK, patch in SID_PATCH
;;  OUT: 4 characters
;; --------------------------------------------------------------------------
SID_LCD_PrintPatchNumber
	;; print bank character
	movf	SID_BANK, W
	addlw	'A'
	rcall	SID_LCD_PrintChar

	;; print patch number, padded with 0
	incf	SID_PATCH, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	goto	MIOS_LCD_PrintHex2


;; --------------------------------------------------------------------------
;;  help function to print the engine name
;;  IN: engine number (0-3) in WREG
;;  OUT: 2 characters
;; --------------------------------------------------------------------------
SID_LCD_PrintEngine
	andlw	0x03
	TABLE_ADDR_MUL_W SID_LCD_PrintEngine_Tab, 2
	movlw	2
	goto	MIOS_LCD_PrintPreconfString

SID_LCD_PrintEngine_Tab
	db	"Ld", "Bs", "Dr", "Ml"

;; --------------------------------------------------------------------------
;;  help function to print the patch name
;;  USES: TMP1
;; --------------------------------------------------------------------------
SID_LCD_PrintPatch
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH

	movf	SID_BANK, W
	bnz	SID_LCD_PrintPatch_BSCheck
	movf	SID_PATCH, W
	bz	SID_LCD_PrintPatch_NoBSCheck
SID_LCD_PrintPatch_BSCheck
	call	SID_PBANK_CheckPatchValid
	bz	SID_LCD_PrintPatch_BSCheck_Ok
	xorlw	2
	bz	SID_LCD_PrintPatchNoPtch
	rgoto	SID_LCD_PrintPatchNoBS

SID_LCD_PrintPatch_BSCheck_Ok
SID_LCD_PrintPatch_NoBSCheck
	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	SID_Ix_NAME_x
	addwf	FSR0L, F
	clrf	TMP1
	movf	POSTINC0, W
	bz	SID_LCD_PrintPatchEmpty

SID_LCD_PrintPatchLoop1
	andlw	0x7f
	skpnz
	movlw	' '
	rcall	SID_LCD_PrintChar
	movf	POSTINC0, W
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, SID_LCD_PrintPatchLoop1
	return

TEXT_EMPTY_PATCH_0 da "<empty>         "
SID_LCD_PrintPatchEmpty
	TABLE_ADDR TEXT_EMPTY_PATCH_0
SID_LCD_PrintPatchNoBS_Cont
	movlw	0x10
	goto	MIOS_LCD_PrintPreconfString


TEXT_EMPTY_PATCH_1 da "<No BankStick>  "
SID_LCD_PrintPatchNoBS
	TABLE_ADDR TEXT_EMPTY_PATCH_1
	rgoto	SID_LCD_PrintPatchNoBS_Cont

TEXT_EMPTY_PATCH_2 da "<only 32k BS>   "
SID_LCD_PrintPatchNoPtch
	TABLE_ADDR TEXT_EMPTY_PATCH_2
	rgoto	SID_LCD_PrintPatchNoBS_Cont

;; --------------------------------------------------------------------------
;;  help function to print the patch name directly from EEPROM (used by SAV menu)
;;  IN: patch in SID_PATCH, bank in SID_BANK
;;  USES: TMP1
;; --------------------------------------------------------------------------
SID_LCD_PrintPatchEE
	movf	SID_BANK, W	; always allow to store in bank #0
	bz	SID_LCD_PrintPatchEE_Ok

	;; notify if no bankstick (or invalid patch)
	call	SID_PBANK_CheckPatchValid
	bz	SID_LCD_PrintPatchEE_Ok
	xorlw	2
	bz	SID_LCD_PrintPatchNoPtch
	rgoto	SID_LCD_PrintPatchNoBS

SID_LCD_PrintPatchEE_Ok
	clrf	EEADR		; Patch Name is stored at offset 0
	clrf	EEADRH
	clrf	TMP1
	call	SID_PBANK_Read
	andlw	0x7f
	bz	SID_LCD_PrintPatchEmpty

SID_LCD_PrintPatchEELoop
	rcall	SID_LCD_PrintChar
	call	SID_PBANK_Read
	andlw	0x7f
	skpnz
	movlw	' '
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, SID_LCD_PrintPatchEELoop
	return


;; --------------------------------------------------------------------------
;;  This function prints the parameter name (20 characters)
;;  called by CS_MENU_ParameterUpdate when Knob/WT parameter assignments are changed
;;  to print a temporary message
;;  IN: parameter number in WREG
;;  USES: PROD[LH], TMP[1234]
;; --------------------------------------------------------------------------
SID_LCD_LongParName
	movwf	TMP1		; temporary store number in TMP1

	;; print number at left side
	call	MIOS_LCD_PrintBCD3
	movlw	'|'
	rcall	SID_LCD_PrintChar

	;; get left/right string part from parameter table (-> sid_par_table.inc)
	movf	TMP1, W
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, TABLAT
	call	SID_PAR_TABLE_GetPtr	; expecting engine in TABLAT[1:0]
	tblrd*+
	movff	TABLAT, TMP2		; left string -> TMP2
	tblrd*+
	movff	TABLAT, TMP3		; right string -> TMP3

	;; print parameter name (8 characters)
	movf	TMP2, W
	TABLE_ADDR_MUL_W SID_PAR_S_TABLE, SID_PAR_S_TABLE_WIDTH
	movlw	8
	call	MIOS_LCD_PrintPreconfString

	;; print space
	movlw	' '
	rcall	SID_LCD_PrintChar

	;; print target (7 characters)
	movf	TMP3, W
	JUMPTABLE_2BYTES 19 ; entries
	rgoto	SID_LCD_LongParName_NOP
	rgoto	SID_LCD_LongParName_OSC
	rgoto	SID_LCD_LongParName_FIL
	rgoto	SID_LCD_LongParName_KNOB
	rgoto	SID_LCD_LongParName_EXT
	rgoto	SID_LCD_LongParName_OSC123
	rgoto	SID_LCD_LongParName_MOD
	rgoto	SID_LCD_LongParName_LFO
	rgoto	SID_LCD_LongParName_ENV
	rgoto	SID_LCD_LongParName_WT
	rgoto	SID_LCD_LongParName_OSC_INS
	rgoto	SID_LCD_LongParName_LFO1_INS
	rgoto	SID_LCD_LongParName_LFO2_INS
	rgoto	SID_LCD_LongParName_ENV_INS
	rgoto	SID_LCD_LongParName_OSC_BL
	rgoto	SID_LCD_LongParName_LFO_BL
	rgoto	SID_LCD_LongParName_ENV_BL
	rgoto	SID_LCD_LongParName_DRM_CUR
	rgoto	SID_LCD_LongParName_DRM_INS


SID_LCD_LongParName_NOP
	;; print 7 spaces
	movlw	7
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_OSC
	;; print "OSC    "
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'S'
	rcall	SID_LCD_PrintChar
	movlw	'C'
	rcall	SID_LCD_PrintChar
	movlw	4
	rgoto	SID_LCD_PrintSpaces


SID_LCD_LongParName_FIL_STR
	db	"Filter "
SID_LCD_LongParName_FIL
	;; "Filter "
	TABLE_ADDR SID_LCD_LongParName_FIL_STR
	movlw	7
	goto	MIOS_LCD_PrintPreconfString
	
SID_LCD_LongParName_KNOB_TAB
	db	'1', '2', '3', '4', '5', 'V', 'P', 'A'
SID_LCD_LongParName_KNOB
	;; print "KNOB#. "
	movlw	'K'
	rcall	SID_LCD_PrintChar
	movlw	'N'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'B'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	TABLE_ADDR_MUL_W SID_LCD_LongParName_KNOB_TAB, 1
	tblrd*+
	movf	TABLAT, W
	rcall	SID_LCD_PrintChar
	movlw	' '
	rgoto	SID_LCD_PrintChar

SID_LCD_LongParName_EXT
	;; print "EXT#.  "
	movlw	'E'
	rcall	SID_LCD_PrintChar
	movlw	'X'
	rcall	SID_LCD_PrintChar
	movlw	'T'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	2
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_OSC123
	;; print "OSC#123" or "OSC#.  "
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'S'
	rcall	SID_LCD_PrintChar
	movlw	'C'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x03
	bz	SID_LCD_LongParName_OSC123_All
	call	MIOS_LCD_PrintBCD1
	movlw	2
	rgoto	SID_LCD_PrintSpaces
SID_LCD_LongParName_OSC123_All
	movlw	'1'
	rcall	SID_LCD_PrintChar
	movlw	'2'
	rcall	SID_LCD_PrintChar
	movlw	'3'
	rgoto	SID_LCD_PrintChar

SID_LCD_LongParName_MOD
	;; print "MOD#.  "
	movlw	'M'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'D'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	2
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_LFO
	;; print "LFO#.  "
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	'F'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	2
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_ENV
	;; print "ENV#.  "
	movlw	'E'
	rcall	SID_LCD_PrintChar
	movlw	'N'
	rcall	SID_LCD_PrintChar
	movlw	'V'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	swapf	TMP1, W
	andlw	0x01
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	2
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_WT
	;; print "WT#.   "
	movlw	'W'
	rcall	SID_LCD_PrintChar
	movlw	'T'
	rcall	SID_LCD_PrintChar
	movlw	'#'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	3
	rgoto	SID_LCD_PrintSpaces

SID_LCD_LongParName_OSC_INS
	;; print "OSC All", "OSC Cur" or "OSC  Ix" (x=num)
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'S'
	rcall	SID_LCD_PrintChar
	movlw	'C'
	rcall	SID_LCD_PrintChar
	movlw	' '
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_INS_Hlp

SID_LCD_LongParName_LFO1_INS
	;; print "LFO1All", "LFO1Cur" or "LFO1 Ix" (x=num)
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	'F'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'1'
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_INS_Hlp

SID_LCD_LongParName_LFO2_INS
	;; print "LFO2All", "LFO2Cur" or "LFO2 Ix" (x=num)
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	'F'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'2'
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_INS_Hlp

SID_LCD_LongParName_ENV_INS
	;; print "ENV All", "ENV Cur" or "ENV  Ix" (x=num)
	movlw	'E'
	rcall	SID_LCD_PrintChar
	movlw	'N'
	rcall	SID_LCD_PrintChar
	movlw	'V'
	rcall	SID_LCD_PrintChar
	movlw	' '
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_INS_Hlp

SID_LCD_LongParName_OSC_BL
	;; print "BL   LR", "BL  Cur", "BL   L-" or "BL   -R"
	movlw	'B'
	rcall	SID_LCD_PrintChar
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	' '
	rcall	SID_LCD_PrintChar
	movlw	' '
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_BL_Hlp

SID_LCD_LongParName_LFO_BL
	;; print "LFOx LR", "LFOxCur", "LFOx L-" or "LFOx -R"
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	'F'
	rcall	SID_LCD_PrintChar
	movlw	'O'
	rcall	SID_LCD_PrintChar
	movlw	'1'
	btfsc	TMP1, 2
	movlw '2'
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_BL_Hlp

SID_LCD_LongParName_ENV_BL
	;; print "ENV  LR", "ENV Cur", "ENV  L-" or "ENV  -R"
	movlw	'E'
	rcall	SID_LCD_PrintChar
	movlw	'N'
	rcall	SID_LCD_PrintChar
	movlw	'V'
	rcall	SID_LCD_PrintChar
	movlw	' '
	rcall	SID_LCD_PrintChar
	rgoto	SID_LCD_LongParName_BL_Hlp


SID_LCD_LongParName_DRM_CUR
	;; print "Drum Cur"
	rcall	SID_LCD_LongParName_DRM_Hlp_Drum
	rgoto	SID_LCD_LongParName_DRM_Hlp_C

SID_LCD_LongParName_DRM_INS
	;; print "Drum xx" (x=num)
	rcall	SID_LCD_LongParName_DRM_Hlp_Drum
	movlw	' '
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x0f
	addlw	1
	goto	MIOS_LCD_PrintBCD2



;; ---
;; ---
;; ---
SID_LCD_LongParName_INS_Hlp
	movf	TMP1, W
	andlw	0x07
	bz	SID_LCD_LongParName_INS_Hlp_A
	addlw	-1
	bz	SID_LCD_LongParName_INS_Hlp_C
SID_LCD_LongParName_INS_Hlp_N
	movlw	' '
	rcall	SID_LCD_PrintChar
	movlw	'I'
	rcall	SID_LCD_PrintChar
	movf	TMP1, W
	andlw	0x07
	addlw	-1
	goto	MIOS_LCD_PrintBCD1

SID_LCD_LongParName_INS_Hlp_A
	movlw	'A'
	rcall	SID_LCD_PrintChar
	movlw	'l'
	rcall	SID_LCD_PrintChar
	movlw	'l'
	rgoto	SID_LCD_PrintChar


SID_LCD_LongParName_BL_Hlp
	movf	TMP1, W
	andlw	0x03
	bz	SID_LCD_LongParName_BL_Hlp_A
	addlw	-1
	bz	SID_LCD_LongParName_BL_Hlp_C
SID_LCD_LongParName_BL_Hlp_LR
	movlw	' '
	rcall	SID_LCD_PrintChar
	movlw	'-'
	btfss	TMP1, 0
	movlw 'L'
	rcall	SID_LCD_PrintChar
	movlw	'-'
	btfsc	TMP1, 0
	movlw 'R'
	rgoto	SID_LCD_PrintChar

SID_LCD_LongParName_BL_Hlp_A
	movlw	' '
	rcall	SID_LCD_PrintChar
	movlw	'L'
	rcall	SID_LCD_PrintChar
	movlw	'R'
	rgoto	SID_LCD_PrintChar

SID_LCD_LongParName_INS_Hlp_C
SID_LCD_LongParName_BL_Hlp_C
SID_LCD_LongParName_DRM_Hlp_C
	movlw	'C'
	rcall	SID_LCD_PrintChar
	movlw	'u'
	rcall	SID_LCD_PrintChar
	movlw	'r'
	rgoto	SID_LCD_PrintChar


SID_LCD_LongParName_DRM_Hlp_Drum
	movlw	'D'
	rcall	SID_LCD_PrintChar
	movlw	'r'
	rcall	SID_LCD_PrintChar
	movlw	'u'
	rcall	SID_LCD_PrintChar
	movlw	'm'
	rgoto	SID_LCD_PrintChar
