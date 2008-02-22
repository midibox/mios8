; $Id$
;
; Control Surface Encoder Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: incrementer of menu encoder in WREG
;; --------------------------------------------------------------------------
CS_MENU_ENC_Handler
	;; store incrementer in TMP3
	movwf	TMP3

	;; do nothing if in CFG mode
	btfsc	CS_MENU_MODE, CS_MENU_MODE_CFG
	return

	;; stop message
	call	MIOS_LCD_MessageStop

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, CS_MENU_PARAMETER_L
	;; set max value
	movff	CS_MENU_PARAMETER_MAX_L, MIOS_PARAMETER1	; low-byte of max value
	movff	CS_MENU_PARAMETER_MAX_H, MIOS_PARAMETER2	; high-byte of max value
	;; get incrementer (which has been stored in TMP3)
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; if main page (CS_MENU[7] set): update patch and init sound
	GOTO_IFSET CS_MENU, 7, ACCESS, CS_MENU_PatchUpdate
	;; if drum page (CS_MENU == CS_MENU_DRUM): update drumset and init sound
	movf	CS_MENU, W
	xorlw	CS_MENU_DRUM
	skpnz
	goto	CS_MENU_DrumUpdate

	;; if not in selection mode: update cursor pos
	GOTO_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_MenuPosUpdate

	;; else do a parameter update
	goto	CS_MENU_ParameterUpdate


;; --------------------------------------------------------------------------
;;  CS Encoder Handler
;;  IN: encoder number (-1) in MIOS_PARAMETER1
;;      incrementer in MIOS_PARAMETER2
;;  NOTE: the menu encoder will not be counted as encoder #0 here to avoid
;;        problems with the INCDEC_BUTTONS option
;; --------------------------------------------------------------------------
CS_MENU_ENC_CS_Handler
	;; branch to CS_MENU_MATRIX
	goto	CS_MENU_MATRIX_Enc

