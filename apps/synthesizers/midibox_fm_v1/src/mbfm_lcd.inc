; $Id$
;
; MIOS Application Display Driver
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  help function to print the patch name
;;  if CS enabled: pointer to instrument in MBFM_CURRENT_INSTRUMENT
;;  IN: number of characters which should be print in WREG
;;  USES: TMP[12]
;; --------------------------------------------------------------------------
#if CS_ENABLED == 0
MBFM_LCD_PrintPatch
	rgoto	MBFM_LCD_PrintPatchEE
#else
MBFM_LCD_PrintPatch
	movwf	TMP2		; save number of characters in TMP2
	call	CS_MENU_MS_GetCurInstrBase
	clrf	TMP1
	movf	INDF0, W
	bz	MBFM_LCD_PrintPatchEmpty

MBFM_LCD_PrintPatchLoop1
	movf	POSTINC0, W
	andlw	0x7f
	skpnz
	movlw	' '
	call	MIOS_LCD_PrintChar
	incf	TMP1, F
	decf	TMP2, W
	cpfsgt	TMP1, ACCESS
	rgoto MBFM_LCD_PrintPatchLoop1
	return
#endif
MBFM_LCD_PrintPatchEmpty
	TABLE_ADDR TEXT_EMPTY_PATCH_0
	movf	TMP2, W
	goto	MIOS_LCD_PrintPreconfString

TEXT_EMPTY_PATCH_0 da "<empty>         "


;; --------------------------------------------------------------------------
;;  help function to print the patch name directly from EEPROM (used by SAV menu)
;;  IN: number of characters which should be print in WREG
;;  USES: TMP[123]
;; --------------------------------------------------------------------------
MBFM_LCD_PrintPatchEE
	movwf	TMP2		; save number of characters in TMP2
#if CS_ENABLED == 1
	call	CS_MENU_MS_GetCurInstrBase
#else
	clrf	MBFM_CURRENT_INSTRUMENT
#endif
	clrf	EEADR		; patch name starts at offset 0x00

	clrf	TMP1
	call	MBFM_BANK_ReadPByte
	andlw	0x7f
	movwf	TMP3
	bz	MBFM_LCD_PrintPatchEmpty

MBFM_LCD_PrintPatchEEL
	movf	TMP3, W
	call	MIOS_LCD_PrintChar
	call	MBFM_BANK_ReadPByte
	andlw	0x7f
	skpnz
	movlw	' '
	movwf	TMP3
	incf	TMP1, F
	decf	TMP2, W
	cpfsgt	TMP1, ACCESS
	rgoto MBFM_LCD_PrintPatchEEL
	return
