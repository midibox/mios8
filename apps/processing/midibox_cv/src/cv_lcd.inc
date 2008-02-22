; $Id$
;
; CV LCD functions, mainly used by cs_m_display.inc
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ==========================================================================
;; print dec value -64..+63
;; USES: MIOS_PARAMETER1
;; ==========================================================================
CV_LCD_Prn_PMDec2
	movwf	MIOS_PARAMETER1
	movlw	'+'		; '+' or '-'?
	btfsc	MIOS_PARAMETER1, 7;
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	MIOS_PARAMETER1, W
	btfsc	WREG, 7			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintBCD2	; and print it

;; ==========================================================================
;; print dec value -8..+7
;; USES: MIOS_PARAMETER1
;; ==========================================================================
CV_LCD_Prn_PMDec1
	movwf	MIOS_PARAMETER1
	movlw	'+'		; '+' or '-'?
	btfsc	MIOS_PARAMETER1, 3;
	movlw '-'
	call	MIOS_LCD_PrintChar

	movf	MIOS_PARAMETER1, W
	btfsc	WREG, 3			; negate value if negative to get absolute value
	negf	WREG, ACCESS
	goto	MIOS_LCD_PrintHex1	; and print it


;; --------------------------------------------------------------------------
;;  help function to print a patch number (3 digits, padded by zeroes)
;;  IN: patch in CV_PATCH
;;  OUT: 3 characters
;; --------------------------------------------------------------------------
CV_LCD_PrintPatchNumber
	;; print patch number, padded with 0
	incf	CV_PATCH, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	goto	MIOS_LCD_PrintHex2


;; --------------------------------------------------------------------------
;;  help function to print the patch name
;;  IN: patch in CV_PATCH
;;  OUT: 16 characters
;; --------------------------------------------------------------------------
CV_LCD_PrintPatch
	movf	CV_PATCH, W
	bz	CV_LCD_PrintPatch_NoBSCheck
CV_LCD_PrintPatch_BSCheck
	call	CV_BANK_CheckPatchValid
	bnz	CV_LCD_PrintPatchNoBS

CV_LCD_PrintPatch_BSCheck_Ok
CV_LCD_PrintPatch_NoBSCheck
	lfsr	FSR0, CV_PATCH_NAME0
	clrf	PRODL
	movf	POSTINC0, W
	bz	CV_LCD_PrintPatchEmpty

CV_LCD_PrintPatchLoop1
	andlw	0x7f
	skpnz
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	POSTINC0, W
	incf	PRODL, F
	BRA_IFCLR PRODL, 4, ACCESS, CV_LCD_PrintPatchLoop1
	return

TEXT_EMPTY_PATCH_0 da "<No Name>       "
CV_LCD_PrintPatchEmpty
	TABLE_ADDR TEXT_EMPTY_PATCH_0
CV_LCD_PrintPatchNoBS_Cont
	movlw	0x10
	goto	MIOS_LCD_PrintPreconfString


TEXT_EMPTY_PATCH_1 da "<No BankStick>  "
CV_LCD_PrintPatchNoBS
	TABLE_ADDR TEXT_EMPTY_PATCH_1
	rgoto	CV_LCD_PrintPatchNoBS_Cont

;; --------------------------------------------------------------------------
;;  help function to print the patch name directly from EEPROM (used by SAV menu)
;;  IN: patch in CV_PATCH
;; --------------------------------------------------------------------------
CV_LCD_PrintPatchEE
	movf	CV_PATCH, W
	bz	CV_LCD_PrintPatchEE_Ok
CV_LCD_PrintPatchEE_BSCheck
	;; notify if no bankstick (or invalid patch)
	call	CV_BANK_CheckPatchValid
	bnz	CV_LCD_PrintPatchNoBS

CV_LCD_PrintPatchEE_Ok
	movlw	(CV_PRESETS_NAME0_7 & 0xff)	; Patch Name is stored at offset 0xf0
	movwf	EEADR
	clrf	PRODL
	call	CV_BANK_Read
	andlw	0x7f
	bz	CV_LCD_PrintPatchEmpty

CV_LCD_PrintPatchEELoop
	call	MIOS_LCD_PrintChar
	call	CV_BANK_Read
	andlw	0x7f
	skpnz
	movlw	' '
	incf	PRODL, F
	BRA_IFCLR PRODL, 4, ACCESS, CV_LCD_PrintPatchEELoop
	return


;; --------------------------------------------------------------------------
;;  This functions prints x spaces
;;  IN:  number of spaces in WREG
;;  OUT: x spaces
;;  USES: TMP5
;; --------------------------------------------------------------------------
CV_LCD_PrintSpaces
	movwf	TMP5
CV_LCD_PrintSpacesLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP5, F
	rgoto	CV_LCD_PrintSpacesLoop
	return

;; ==========================================================================
;; this function prints the menu header
;; it contains the application name and the song position
;; ==========================================================================
CV_LCD_VERSION_STR_0 MBCV_VERSION_STR	; (defined in main.asm)
CV_LCD_PrintHeader
	TABLE_ADDR CV_LCD_VERSION_STR_0
	goto	MIOS_LCD_PrintString
