; $Id$
;
; MIDIbox LC Calibration Mode Routines
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Updates the CALI message screen
;; --------------------------------------------------------------------------
LC_CALI_Msg_Update
	;; fill message buffer with spaces
	lfsr	FSR0, MSG_GPC_BEGIN
	movlw	0x80
	movwf	TMP1
	movlw	' '
LC_CALI_Msg_Update_Loop1
	movwf	POSTINC0
	decfsz	TMP1, F
	rgoto	LC_CALI_Msg_Update_Loop1

	;; copy string into message buffer, first line
	TABLE_ADDR LC_CALI_STR
	lfsr	FSR0, MSG_GPC_BEGIN
	tblrd*+
	movff	TABLAT, TMP1
LC_CALI_Msg_Update_Loop2
	tblrd*+
	movff	TABLAT, POSTINC0
	decfsz	TMP1, F
	rgoto	LC_CALI_Msg_Update_Loop2

	;; 	rgoto	LC_CALI_Msg_Update_Values

;; --------------------------------------------------------------------------
;;  Updates the values of the GPC message screen
;; --------------------------------------------------------------------------
LC_CALI_Msg_Update_Values
	;; write the values into message buffer, second line
	lfsr	FSR0, MSG_GPC_BEGIN+0x40
	call	MIOS_MF_PWM_DutyCycleDownGet
	rcall	LC_CALI_CopyHex2
	rcall	LC_CALI_CopySpace
	call	MIOS_MF_PWM_DutyCycleUpGet
	rcall	LC_CALI_CopyHex2
	rcall	LC_CALI_CopySpace
	call	MIOS_MF_PWM_PeriodGet
	rcall	LC_CALI_CopyHex2
	rcall	LC_CALI_CopySpace
	call	MIOS_AIN_DeadbandGet
	rcall	LC_CALI_CopyHex2
	rcall	LC_CALI_CopySpace
	call	MIOS_MF_DeadbandGet
	rgoto	LC_CALI_CopyHex2
		
LC_CALI_CopyHex2
	movwf	TMP5
	swapf	TMP5, W
	rcall	LC_CALI_CopyHex1
	movf	TMP5, W
	;; 	rgoto	LC_CALI_CopyHex1
LC_CALI_CopyHex1
	andlw	0x0f
	addlw	-0x0a
	skpnc
	addlw	0x07
	addlw	0x3a
	movwf	POSTINC0
	return

LC_CALI_CopySpace
	movlw	' '
	movwf	POSTINC0
	return

LC_CALI_STR db 14, "DD DU Pd AI MF"


;; --------------------------------------------------------------------------
;;  Button handler for calibration mode
;; --------------------------------------------------------------------------
LC_CALI_BUTTON_Handler
	;; save button number in TMP1
	movff	MIOS_PARAMETER1, TMP1
	;; save button value in TMP2
	movff	MIOS_PARAMETER2, TMP2

	;; if button index >= 16, jump back to normal handler
	movlw	0x10
	cpfslt	TMP1, ACCESS
	goto LC_CALI_BUTTON_Handler_Return

	;; request display message update
	bsf	LC_STAT, LC_STAT_MSG_UPDATE_REQ

	;; toggle the appr. DOUT LED
	movf	TMP1, W				; button number has been saved in TMP1. This is also the LED number
	CALL_IFCLR TMP2, 0, ACCESS, MIOS_DOUT_PinSet1	; if button has been pressed (value = 0V = CLR): LED on
	CALL_IFSET TMP2, 0, ACCESS, MIOS_DOUT_PinSet0	; if button has been depressed (value = 5V = SET): LED off

	;; don't continue of button has been depressed
	btfsc	TMP2, 0
	return

	;; button pressed: branch depending on button number
	movf	TMP1, W
	JUMPTABLE_2BYTES 16	; 16 entries
	rgoto	CALI_BUTTON_Dec_CD
	rgoto	CALI_BUTTON_Inc_CD
	rgoto	CALI_BUTTON_Dec_CU
	rgoto	CALI_BUTTON_Inc_CU
	rgoto	CALI_BUTTON_Dec_Sp
	rgoto	CALI_BUTTON_Inc_Sp
	rgoto	CALI_BUTTON_Dec_AI
	rgoto	CALI_BUTTON_Inc_AI
	rgoto	CALI_BUTTON_Dec_MF
	rgoto	CALI_BUTTON_Inc_MF
	rgoto	CALI_BUTTON_Exit
	rgoto	CALI_BUTTON_Exit
	rgoto	CALI_BUTTON_Exit
	rgoto	CALI_BUTTON_Exit
	rgoto	CALI_BUTTON_Exit
	rgoto	CALI_BUTTON_Exit

	;; thats all
	return


;; DIN Button Handlers:
CALI_BUTTON_Dec_CD
	;; decrement "calibration down value" if > 0
	call	MIOS_MF_PWM_DutyCycleDownGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_DutyCycleDownSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Inc_CD
	;; increment "calibration down value" if <= 0x1e
	call	MIOS_MF_PWM_DutyCycleDownGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_DutyCycleDownSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Dec_CU
	;; decrement "calibration up value" if > 0
	call	MIOS_MF_PWM_DutyCycleUpGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_DutyCycleUpGet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Inc_CU
	;; increment "calibration up value" if <= 0x1e
	call	MIOS_MF_PWM_DutyCycleUpGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_DutyCycleUpGet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Dec_Sp
	;; decrement "period" if > 0
	call	MIOS_MF_PWM_PeriodGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_PeriodSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Inc_Sp
	;; increment "startup speed value" if <= 0x1e
	call	MIOS_MF_PWM_PeriodGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_PeriodSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Dec_AI
	;; decrement "AIN deadband" if > 0
	call	MIOS_AIN_DeadbandGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_AIN_DeadbandSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Inc_AI
	;; increment "AIN deadband value" if <= 0x3e (more makes no sense)
	call	MIOS_AIN_DeadbandGet	; get value
	movlw	0x3f			; check if value >= 0x3f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_AIN_DeadbandSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Dec_MF
	;; decrement "MF deadband" if > 0
	call	MIOS_MF_DeadbandGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_DeadbandSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values
	
CALI_BUTTON_Inc_MF
	;; increment "MF deadband value" if <= 0x3e (more makes no sense)
	call	MIOS_MF_DeadbandGet	; get value
	movlw	0x3f			; check if value >= 0x3f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_DeadbandSet	; set new value
	rgoto	LC_CALI_Msg_Update_Values

CALI_BUTTON_Exit
	bcf	LC_STAT, LC_STAT_CALI_SEL; switch to emulation mode
	goto	_LC_SFB_CALI_Cont
