; $Id$
;
; MIDIbox SID LCD routines
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  help function to print a patch number
;;  IN: bank in PRODL, patch in PRODH
;;  OUT: 4 characters
;;  USES: TMP1
;; --------------------------------------------------------------------------
SID_LCD_PrintPatchNumber
	;; print bank character
	movf	PRODL, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; print patch number, padded with 0
	incf	PRODH, W
	clrf	MIOS_PARAMETER1
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W
	goto	MIOS_LCD_PrintHex2


;; --------------------------------------------------------------------------
;;  help function to print the patch name
;;  USES: TMP1
;; --------------------------------------------------------------------------
#if CS_ENABLED == 0
SID_LCD_PrintPatch
	rgoto	SID_LCD_PrintPatchEE
#else
SID_LCD_PrintPatch
	call	CS_MENU_MS_GetSIDBank
	bnz	SID_LCD_PrintPatch_BSCheck
	call	CS_MENU_MS_GetSIDPatch
	bz	SID_LCD_PrintPatch_NoBSCheck
SID_LCD_PrintPatch_BSCheck
	call	CS_MENU_MS_GetSIDBank
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bz	SID_LCD_PrintPatchNoBS

SID_LCD_PrintPatch_NoBSCheck
	call	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_NAME_x
	addwf	FSR0L, F
	clrf	TMP1
	movf	POSTINC0, W
	bz	SID_LCD_PrintPatchEmpty

SID_LCD_PrintPatchLoop1
	andlw	0x7f
	skpnz
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	POSTINC0, W
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, SID_LCD_PrintPatchLoop1
	return
#endif
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

;; --------------------------------------------------------------------------
;;  help function to print the patch name directly from EEPROM (used by SAV menu)
;;  USES: TMP1
;; --------------------------------------------------------------------------
SID_LCD_PrintPatchEE
	movlw	EEPROM_PATCHNAME & 0xff
	movwf	EEADR
	clrf	TMP1
	call	SID_BANK_Read
	andlw	0x7f
	bz	SID_LCD_PrintPatchEmpty

SID_LCD_PrintPatchEEL
	call	MIOS_LCD_PrintChar
	call	SID_BANK_Read
	andlw	0x7f
	skpnz
	movlw	' '
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, SID_LCD_PrintPatchEEL
	return
