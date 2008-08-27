; $Id$
;
; MBSEQ Menu "Trigger Assignments"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_TRKASG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,	0x40+ 1, 4,	CS_M_TRKASG_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_GATE,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_GateChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_SKIP,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_SkipChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_ACCENT,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_AccentChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_GLIDE,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_GlideChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_ROLL,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_RollChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_RANDOM_GATE,		0x03,	0x40+ 9, 1,	CS_M_TRKASG_RGateChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_ASG_RANDOM_VALUE,	0x03,	0x40+ 9, 1,	CS_M_TRKASG_RValueChanged

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;     o encoder incrementer in CS_MENU_ENC_INCREMENTER if encoder has been used
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;;     should clear CS_MENU_ENC_INCREMENTER if radio-button function desired
;; --------------------------------------------------------------------------
CS_M_TRKASG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; ignore if button/encoder number <= 7
	movlw	7
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKASG_GP_Callback_Ok
CS_M_TRKASG_GP_Callback_Ignore
	clrf	CS_MENU_ENC_INCREMENTER
	return

CS_M_TRKASG_GP_Callback_Ok
	;; set cursor depending on button number
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit


;; --------------------------------------------------------------------------
CS_M_TRKASG_UpdateGPLEDs
	;; always flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; set LEDs depending on cursor position
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; LEDs of right half always deactivated
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKASG_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKASG_PARAMETERS	; pointer to table
	movlw	8				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_TRKASG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKASG_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKASG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk. Gate Skip Acc. Glide Roll R.G  R.V                                            
	;; 0000011111222223333344444555556666677777----------------------------------------
	;; G1T1   A    -    B    -     C   -    -                                             
	;; 8888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF----------------------------------------

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKASG_Tick_0
	rgoto	CS_M_TRKASG_Tick_1
	rgoto	CS_M_TRKASG_Tick_2
	rgoto	CS_M_TRKASG_Tick_3
	rgoto	CS_M_TRKASG_Tick_4
	rgoto	CS_M_TRKASG_Tick_5
	rgoto	CS_M_TRKASG_Tick_6
	rgoto	CS_M_TRKASG_Tick_7
	rgoto	CS_M_TRKASG_Tick_8
	rgoto	CS_M_TRKASG_Tick_9
	rgoto	CS_M_TRKASG_Tick_A
	rgoto	CS_M_TRKASG_Tick_B
	rgoto	CS_M_TRKASG_Tick_C
	rgoto	CS_M_TRKASG_Tick_D
	rgoto	CS_M_TRKASG_Tick_E
	rgoto	CS_M_TRKASG_Tick_F

CS_M_TRKASG_Tick_0_STR	STRING 4, 0x00 + 0, "Trk."
CS_M_TRKASG_Tick_0
	TABLE_ADDR CS_M_TRKASG_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKASG_Tick_Cont

CS_M_TRKASG_Tick_1
CS_M_TRKASG_Tick_2
CS_M_TRKASG_Tick_3
CS_M_TRKASG_Tick_4
CS_M_TRKASG_Tick_5
CS_M_TRKASG_Tick_6
CS_M_TRKASG_Tick_7
	;; set cursor depending on refresh counter value
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	5
	movf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; print type (5 characters)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-1
	call	SEQ_TRG_PrnType
	rgoto	CS_M_TRKASG_Tick_Cont

CS_M_TRKASG_Tick_8
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKASG_Tick_Cont

CS_M_TRKASG_Tick_9
CS_M_TRKASG_Tick_A
CS_M_TRKASG_Tick_B
CS_M_TRKASG_Tick_C
CS_M_TRKASG_Tick_D
CS_M_TRKASG_Tick_E
CS_M_TRKASG_Tick_F
	;; determine CC offset (Skip=0...) -> TMP1
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-9
	movwf	TMP1

	;; set cursor depending on offset
	mullw	5
	movf	PRODL, W
	addlw	0x40 + 7
	call	MIOS_LCD_CursorSet

	movf	TMP1, W		; item number
	addlw	1
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKASG_Tick_Cont

	;; get CC depending on offset and print layer assignment
	movf	TMP1, W
	addlw	SEQ_CC_OFFSET_ASG_GATE
	call	CS_M_HLP_TrkCC_Get
	bz	CS_M_TRKASG_Tick_9_Off
CS_M_TRKASG_Tick_9_L
	addlw	'A'-1
	rgoto	CS_M_TRKASG_Tick_9_Cont
CS_M_TRKASG_Tick_9_Off
	movlw	'-'
	;; 	rgoto	CS_M_TRKASG_Tick_9_Cont
CS_M_TRKASG_Tick_9_Cont
	call	MIOS_LCD_PrintChar
	;; 	rgoto	CS_M_TRKASG_Tick_Cont

CS_M_TRKASG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	0x10-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_TRKASG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKASG_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKASG_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKASG_UpdateGPLEDs

CS_M_TRKASG_GateChanged
	movlw	SEQ_CC_OFFSET_ASG_GATE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_SkipChanged
	movlw	SEQ_CC_OFFSET_ASG_SKIP
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_AccentChanged
	movlw	SEQ_CC_OFFSET_ASG_ACCENT
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_GlideChanged
	movlw	SEQ_CC_OFFSET_ASG_GLIDE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_RollChanged
	movlw	SEQ_CC_OFFSET_ASG_ROLL
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_RGateChanged
	movlw	SEQ_CC_OFFSET_ASG_RANDOM_GATE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKASG_RValueChanged
	movlw	SEQ_CC_OFFSET_ASG_RANDOM_VALUE
	goto	CS_M_HLP_TrkCC_Set
