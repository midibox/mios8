; $Id$
;
; MB64 Menu Morphing Handler
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
CS_M_MORPH_Exit
	goto	MB64_DUMP_StoreGlobal; (save MB64_MORHP_*)

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
CS_M_MORPH_GP_Callback
	;; clear callback if GP button has been depressed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_MORPH_GP_Callback_On
CS_M_MORPH_GP_Callback_Off
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop

CS_M_MORHP_OVERTAKE_STR_0 STRING 16, 0x00, " Hold 2 sec. to "
CS_M_MORHP_OVERTAKE_STR_1 STRING 16, 0x40, " overtake values"

CS_M_MORPH_GP_Callback_On
	;; set new morph bank
	SET_BSR MB64_BASE
	movlw	0xf0
	andwf	MB64_MORPH_BANK, F, BANKED
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	MB64_MORPH_BANK, F, BANKED
	;; install delayed callback hook, used to overtake the current pot values after 2 seconds
	TABLE_ADDR CS_M_MORPH_CopyPotValues
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook
	;; print message
	TABLE_ADDR CS_M_MORHP_OVERTAKE_STR_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage
	
	;; update the LEDs and exit
	;; 	rgoto	CS_M_MORPH_UpdateGPLEDs

CS_M_MORPH_UpdateGPLEDs
	;; set "1-of-16" code
	SET_BSR	MB64_BASE
	movf	MB64_MORPH_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	btfsc	MB64_MORPH_BANK, 3, BANKED
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	movf	TMP1, W
	btfss	MB64_MORPH_BANK, 3, BANKED
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_MORPH_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor position, wrap at 5
	incf	CS_MENU_CURSOR_POS, F
	movlw	0x04+1
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	clrf CS_MENU_CURSOR_POS

	;; update menu parameter
	rgoto	CS_M_MORPH_Update_MenuParam

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MORPH_Init
	;; clear menu position
	clrf	CS_MENU_CURSOR_POS

	;; update menu parameter + max value depending on cursor pos
	rcall	CS_M_MORPH_Update_MenuParam
	
	TABLE_ADDR CS_M_MORPH_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_MORPH_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_MORPH_UpdateGPLEDs

	;; print parameter headers
	TABLE_ADDR CS_M_MORPH_STR_0_0
	goto	MIOS_LCD_PrintString
CS_M_MORPH_STR_0_0 STRING 14, 0x00, "Gl G1 G2 G3 G4"

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MORPH_ValueChanged
	lfsr	FSR0, MB64_MORPH_POT_GLOBAL
	movf	CS_MENU_CURSOR_POS, W
	movff	CS_MENU_PARAMETER_L, PLUSW0
	return

;; --------------------------------------------------------------------------
;;  transfer selected register value to CS_MENU_PARAMETER_L
;; --------------------------------------------------------------------------
CS_M_MORPH_Update_MenuParam
	;; updates the menu parameter depending on menu position
	clrf	CS_MENU_PARAMETER_H
	clrf	CS_MENU_PARAMETER_MAX_H

	movlw	64
	movwf	CS_MENU_PARAMETER_MAX_L

	lfsr	FSR0, MB64_MORPH_POT_GLOBAL
	movf	CS_MENU_CURSOR_POS, W
	movff	PLUSW0, CS_MENU_PARAMETER_L
	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MORPH_Tick
	;; print pot numbers

	lfsr	FSR0, MB64_MORPH_POT_GLOBAL
	clrf	TMP1
CS_M_MORPH_Tick_Loop
	movf	TMP1, W
	mullw	3
	movf	PRODL, W
	iorlw	0x40
	call	MIOS_LCD_CursorSet

	movf	POSTINC0, W
	bnz	CS_M_MORPH_Tick_Loop_PotOk
CS_M_MORPH_Tick_Loop_PotDis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_MORPH_Tick_Loop_Next
CS_M_MORPH_Tick_Loop_PotOk
	;; 	addlw	1 ; NO! - 0x00 == disabled, 0x01-0x40: pot number - 1
	call	MIOS_LCD_PrintBCD2
CS_M_MORPH_Tick_Loop_Next
	incf	TMP1, F
	movlw	0x04
	cpfsgt	TMP1, ACCESS
	rgoto CS_M_MORPH_Tick_Loop

#if 0
	;; print bank number
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	
	SET_BSR	MB64_BASE
	incf	MB64_MORPH_BANK, W, BANKED
	call	MIOS_LCD_PrintBCD2
#endif

	;; cursor handler expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	movf	CS_MENU_CURSOR_POS, W
	mullw	3
	movf	PRODL, W
	addlw	0x40
	movwf	TMP1
	movlw	0x02
	movwf	TMP2
	call	CS_MENU_CursorHandler

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	goto	CS_MENU_PrintArrows


;; ==========================================================================

	;; this function transfers the current pot values to the POT_VALUE_xx
	;; registers to overtake the morphed pot positions
CS_M_MORPH_CopyPotValues
	SET_BSR MB64_BASE
	clrf	MB64_CURRENT_POT, BANKED
CS_M_MORPH_CopyPotValuesLoop
	call	MB64_FSR_PotLastValue
	movff	INDF0, TMP1
	call	MB64_FSR_PotValue
	movff	TMP1, INDF0
	bsf	INDF0, 7	; (set snap bit)
	incf	MB64_CURRENT_POT, F, BANKED
	movlw	DEFAULT_NUMBER_POTS-1
	cpfsgt	MB64_CURRENT_POT, BANKED
	rgoto CS_M_MORPH_CopyPotValuesLoop
	return
