; $Id$
;
; MIDIbox MF
; Motorfader Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by USER_DIN_NotifyToggle when a touch sensor
;;  has been toggled
;;  Input:
;;     o DIN number in WREG and MIOS_PARAMETER1 (TS ID = button number & 0x07)
;;     o TS status in MIOS_PARAMETER2:
;;       - 1 if touch sensor has been depressed (=5V)
;;       - 0 if touch sensor has been pressed (=0V)
;; 
;;  IMPORTANT: this handler has to left MIOS_PARAMETER[12] untouched!!!
;; --------------------------------------------------------------------------
MBMF_TS_Handler
	;; store MIOS_PARAMETER[12] in TMP[12]
	movff	MIOS_PARAMETER1, TMP1
	movff	MIOS_PARAMETER2, TMP2

	SET_BSR	MBMF_BASE

	;; store TS number in MBMF_CURRENT_FADER
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	movwf	MBMF_CURRENT_FADER, BANKED

	;; branch depending on TS status
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, MBMF_TS_Handler_Pressed

MBMF_TS_Handler_Depressed
	;; clear flag in TS_STATUS
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	MBMF_TS_STATUS, F, BANKED

	;; disable suspend status of the appr. motorfader
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_MF_SuspendDisable

	;; if TS mode > 0, move fader to last received position if snap bit set
	SET_BSR	MBMF_BASE
	BRA_IFSET MBMF_CFG0, MBMF_CFG0_TSMODE1, BANKED, MBMF_TS_Handler_Depressed_M1
	BRA_IFCLR MBMF_CFG0, MBMF_CFG0_TSMODE0, BANKED, MBMF_TS_Handler_Depressed_NotM1
MBMF_TS_Handler_Depressed_M1

	;; don't do this in following modes:
	BRA_IFSET MB_STAT, MB_STAT_SEL_GROUP_AND_BANK, ACCESS, MBMF_TS_Handler_Depressed_NotM1
	BRA_IFSET MB_STAT2, MB_STAT2_MORPH_MODE, ACCESS, MBMF_TS_Handler_Depressed_NotM1
	BRA_IFSET MB_STAT2, MB_STAT2_CALI_MODE, ACCESS, MBMF_TS_Handler_Depressed_NotM1
	BRA_IFSET MB_STAT, MB_STAT_LEARN_MODE, ACCESS, MBMF_TS_Handler_Depressed_NotM1

	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_MF_TouchDetectionReset
	SET_BSR	MBMF_BASE
	movf	MBMF_CURRENT_FADER, W, BANKED
	rcall	MBMF_FADER_Map
	lfsr	FSR0, MBMF_FADER_VALUES_00
	movf	PLUSW0, W
	BRA_IFCLR WREG, 7, ACCESS, MBMF_TS_Handler_Depressed_NotM1
	andlw	0x7f
	rcall	MBMF_FADER_Move
MBMF_TS_Handler_Depressed_NotM1

	rgoto	MBMF_TS_Handler_End

MBMF_TS_Handler_Pressed
	;; set flag in TS_STATUS
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	MBMF_TS_STATUS, F, BANKED

	;; if TS mode > 0 suspend the appr. motorfader
	BRA_IFSET MBMF_CFG0, MBMF_CFG0_TSMODE1, BANKED, MBMF_TS_Handler_Pressed_M1
	BRA_IFCLR MBMF_CFG0, MBMF_CFG0_TSMODE0, BANKED, MBMF_TS_Handler_Pressed_NotM1
MBMF_TS_Handler_Pressed_M1
	movf	MBMF_CURRENT_FADER, W, BANKED
	call	MIOS_MF_SuspendEnable
MBMF_TS_Handler_Pressed_NotM1
	;; 	rgoto	MBMF_TS_Handler_End

MBMF_TS_Handler_End

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; restore MIOS_PARAMETER[12]
	movff	TMP1, MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2

	return
