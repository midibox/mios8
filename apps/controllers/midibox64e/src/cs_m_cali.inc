; $Id$
;
; MB64E Calibration Menu Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_CALI_Exit
	;; exit calibration mode
	rcall	CS_M_CALI_ExitMode

	;; and save settings in EEPROM
	goto	MB64E_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_CALI_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor position, wrap at 3
	incf	CS_MENU_CURSOR_POS, F
	movlw	0x02+1
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	clrf CS_MENU_CURSOR_POS

	;; update menu parameter
	rgoto	CS_M_CALI_Update_MenuParam

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_CALI_Init
	;; enter calibration mode
	rcall	CS_M_CALI_EnterMode

	;; clear menu position
	clrf	CS_MENU_CURSOR_POS

	;; update menu parameter + max value depending on cursor pos
	rcall	CS_M_CALI_Update_MenuParam
	
	TABLE_ADDR CS_M_CALI_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; print parameter headers
	TABLE_ADDR CS_M_CALI_STR_0_0
	goto	MIOS_LCD_PrintString
CS_M_CALI_STR_0_0 STRING 14, 0x00, "Val DD DU Pd  "

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_CALI_ValueChanged
	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	JUMPTABLE_2BYTES 3
	rgoto	CS_M_CALI_ValueChanged_CD
	rgoto	CS_M_CALI_ValueChanged_CU
	rgoto	CS_M_CALI_ValueChanged_Sp

CS_M_CALI_ValueChanged_CD
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MF_PWM_DutyCycleDownSet

CS_M_CALI_ValueChanged_CU
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MF_PWM_DutyCycleUpSet

CS_M_CALI_ValueChanged_Sp
	movf	CS_MENU_PARAMETER_L, W
	goto	MIOS_MF_PWM_PeriodSet

;; --------------------------------------------------------------------------
;;  transfer selected register value to CS_MENU_PARAMETER_L
;; --------------------------------------------------------------------------
CS_M_CALI_Update_MenuParam
	;; updates the menu parameter depending on menu position
	clrf	CS_MENU_PARAMETER_H
	clrf	CS_MENU_PARAMETER_MAX_H

	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	JUMPTABLE_2BYTES 3
	rgoto	CS_M_CALI_Update_MenuParam_CD
	rgoto	CS_M_CALI_Update_MenuParam_CU
	rgoto	CS_M_CALI_Update_MenuParam_Sp

CS_M_CALI_Update_MenuParam_CD
	movlw	0x1f
	movwf	CS_MENU_PARAMETER_MAX_L
	call	MIOS_MF_PWM_DutyCycleDownGet
	movwf	CS_MENU_PARAMETER_L
	return

CS_M_CALI_Update_MenuParam_CU
	movlw	0x1f
	movwf	CS_MENU_PARAMETER_MAX_L
	call	MIOS_MF_PWM_DutyCycleUpGet
	movwf	CS_MENU_PARAMETER_L
	return

CS_M_CALI_Update_MenuParam_Sp
	movlw	0x1f
	movwf	CS_MENU_PARAMETER_MAX_L
	call	MIOS_MF_PWM_PeriodGet
	movwf	CS_MENU_PARAMETER_L
	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_CALI_Tick
	;; print parameters at second line

	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print position of motorfader #1
	movlw	0x00
	call	MIOS_AIN_PinGet
	movf	MIOS_PARAMETER2, W	; MSB in MIOS_PARAMETER2, we only need the first digit
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W	; LSB in MIOS_PARAMETER1
	call	MIOS_LCD_PrintHex2

	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print "calibration down" value
	call	MIOS_MF_PWM_DutyCycleDownGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print "calibration up" value
	call	MIOS_MF_PWM_DutyCycleUpGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print "period" value
	call	MIOS_MF_PWM_PeriodGet
	call	MIOS_LCD_PrintHex2
	

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movf	CS_MENU_CURSOR_POS, W
	mullw	3
	movf	PRODL, W
	addlw	0x40 + 4
	movwf	TMP1
	movlw	0x02
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function enters the calibration mode
;; --------------------------------------------------------------------------
CS_M_CALI_EnterMode
	;; set flag
	bsf	MB_STAT3, MB_STAT3_CALI_MODE

	;; move all faders to the same position like fader #0
	SET_BSR	MB64E_BASE
	movlw	0x01
	movwf	TMP3
CS_M_CALI_EnterLoop
	movf	TMP3, W
	call	MIOS_MF_TouchDetectionReset
	
	movlw	0x00
	call	MIOS_AIN_PinGet

	movf	TMP3, W
	call	MIOS_MF_FaderMove

	incf	TMP3, F
	BRA_IFCLR TMP3, 3, ACCESS, CS_M_CALI_EnterLoop

	return

;; --------------------------------------------------------------------------
;;  This function exits the calibration mode
;; --------------------------------------------------------------------------
CS_M_CALI_ExitMode
	;; clear flag
	bcf	MB_STAT3, MB_STAT3_CALI_MODE

	;; update fader positions
	movlw	0xff
	goto	MB64E_FADER_UpdatePos


;; --------------------------------------------------------------------------
;;  This function is called by MB64E_FADER_Handler when a fader has been
;;  moved in calibration mode. Input:
;;     o Fader number in WREG and MIOS_PARAMETER1
;;     o 7-bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
CS_M_CALI_Handler
	;; only continue if fader #1 has been moved
	movlw	0x00		; (index of fader #1)
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	;; get 10-bit value of fader position
	movf	MIOS_PARAMETER1, W
	call	MIOS_AIN_PinGet
	
	;; save fader LSB value in TMP1
	movff	MIOS_PARAMETER1, TMP1
	;; save fader MSB value in TMP2
	movff	MIOS_PARAMETER2, TMP2

	;; move the other 7 faders
	;; use TMP3 as loop counter
	movlw	0x01		; start with fader #1
	movwf	TMP3
CS_M_CALI_HandlerLoop
	movff	TMP1, MIOS_PARAMETER1	; LSB in MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2	; MSB in MIOS_PARAMETER1
	movf	TMP3, W			; fader index in WREG
	call	MIOS_MF_FaderMove	; move fader

	incf	TMP3, F
	BRA_IFCLR TMP3, 3, ACCESS, CS_M_CALI_HandlerLoop

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return
