; $Id$
;
; MBSEQ Menu "Patterns"
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
CS_M_PATTERN_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40, 2,	CS_M_PATTERN_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x43, 2,	CS_M_PATTERN_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x46, 2,	CS_M_PATTERN_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x49, 2,	CS_M_PATTERN_ValueChanged

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_PATTERN_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel
	;; save value if one of the right buttons has been pressed
	RCALL_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_PATTERN_ValueChanged

	;; update the LEDs and exit
	;; 	rgoto	CS_M_PATTERN_UpdateGPLEDs

CS_M_PATTERN_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1
	;; exit when menu parameter == 0
	movf	CS_MENU_PARAMETER_L, W
	skpnz
	return

	;; first 8 leds: (CS_MENU_PARAMETER_L-1)[5..3]
	decf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: (CS_MENU_PARAMETER_L-1)[2..0]
	decf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_PATTERN_Init
	;; install parameter table
	TABLE_ADDR CS_M_PATTERN_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy current pattern number into parameter
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	lfsr	FSR0, SEQ_PATTERN0
	movf	CS_MENU_CURSOR_POS, W
	BRA_IFSET PLUSW0, 7, ACCESS, CS_M_PATTERN_Init_PMuted
CS_M_PATTERN_Init_PNotMuted
	incf	PLUSW0, W
	rgoto	CS_M_PATTERN_Init_P_Cont
CS_M_PATTERN_Init_PMuted
	movlw	0x00
CS_M_PATTERN_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as pattern select buttons here
	TABLE_ADDR CS_M_PATTERN_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_PATTERN_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static string
	TABLE_ADDR CS_M_PATTERN_STR_BA_0
	BRA_IFSET MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, CS_M_PATTERN_Init_BA
CS_M_PATTERN_Init_BNA
	TABLE_ADDR CS_M_PATTERN_STR_BNA_0
	call	MIOS_LCD_PrintString
CS_M_PATTERN_Init_BA
	goto	MIOS_LCD_PrintString

CS_M_PATTERN_STR_BA_0	STRING 11, 0x00, "P1 P2 P3 P4"

CS_M_PATTERN_STR_BNA_0	STRING 16, 0x00, "* No BankStick *"
CS_M_PATTERN_STR_BNA_1	STRING 16, 0x40, "*  found  :-(  *"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATTERN_Tick
	;; exit if bankstick not available
	btfss	MB_STAT, MB_STAT_USE_BANKSTICK
	return

	;; print pattern number of group 0-3
	;; print "--" if pattern disabled (bit #7 set)
	movlw	0x40
	call	MIOS_LCD_CursorSet
	clrf	TMP5
CS_M_PATTERN_Tick_Loop
	;; saved pattern number -> TMP4
	lfsr	FSR0, SEQ_PATTERN0
	movf	TMP5, W
	movff	PLUSW0, TMP4
	;; if TMP5 == CURSOR_POS, write converted CS_MENU_PARAMETER_L to TMP4
	movf	TMP5, W
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_PATTERN_Tick_Loop_NMP
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP4
CS_M_PATTERN_Tick_Loop_NMP

	movf	TMP4, W
	call	SEQ_LCD_PrnPattern

	;; print "*" if pattern synched to measure
	BRA_IFSET TMP4, 7, ACCESS, CS_M_PATTERN_Tick_Loop_NoS
CS_M_PATTERN_Tick_Loop_S
	lfsr	FSR0, SEQ_NEXT_PATTERN0
	movf	TMP5, W
	BRA_IFSET PLUSW0, 7, ACCESS, CS_M_PATTERN_Tick_Loop_NoS
	movlw	'*'
	rgoto	CS_M_PATTERN_Tick_Loop_SPr
CS_M_PATTERN_Tick_Loop_NoS
	movlw	' '
CS_M_PATTERN_Tick_Loop_SPr
	call	MIOS_LCD_PrintChar

	incf	TMP5, F
	BRA_IFCLR TMP5, 2, ACCESS, CS_M_PATTERN_Tick_Loop

	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_PATTERN_Tick_FlashAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_PATTERN_Tick_FlashR
CS_M_PATTERN_Tick_FlashAll
	;; flash both digits
	goto	CS_MENU_CursorHandler

CS_M_PATTERN_Tick_FlashR
	;; flash only right digit
	incf	CS_MENU_CURSOR_LCD_POS, F
	decf	CS_MENU_CURSOR_LCD_WIDTH, F
	call	CS_MENU_CursorHandler
	decf	CS_MENU_CURSOR_LCD_POS, F
	incf	CS_MENU_CURSOR_LCD_WIDTH, F
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_PATTERN_ValueChanged
	;; change to a new pattern
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

	lfsr	FSR0, SEQ_PATTERN0
	movf	CS_MENU_CURSOR_POS, W
	movff	TMP1, PLUSW0
	call	SEQ_CORE_ChangePatternSynched

	;; update LEDs
	rgoto	CS_M_PATTERN_UpdateGPLEDs

;; ==========================================================================

