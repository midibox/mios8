; $Id$
;
; MB64E Menu Morphing Handler
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
	;; exit morph mode
	bcf	MB_STAT2, MB_STAT2_MORPH_MODE

	;; update speed to presets and exit
	movlw	0xff
	goto	MB64E_ENC_UpdateSpeed

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
	SET_BSR MB64E_BASE
	movlw	0xf0
	andwf	MB64E_MORPH_BANK, F, BANKED
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	MB64E_MORPH_BANK, F, BANKED
	;; install delayed callback hook, used to overtake the current enc values after 2 seconds
	TABLE_ADDR CS_M_MORPH_CopyEncValues
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
	SET_BSR	MB64E_BASE
	movf	MB64E_MORPH_BANK, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1

	btfsc	MB64E_MORPH_BANK, 3, BANKED
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	movf	TMP1, W
	btfss	MB64E_MORPH_BANK, 3, BANKED
	movlw 0x00
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_MORPH_Snapshot
	;; do nothing
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MORPH_Init
	;; enter morph mode
	bsf	MB_STAT2, MB_STAT2_MORPH_MODE

	;; set normal speed
	movlw	0x00
	call	MB64E_ENC_UpdateSpeed

	;; initialize V-Bars
	call	MB64E_LCD_InitVBars

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_MORPH_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_MORPH_UpdateGPLEDs

	;; print group header
	TABLE_ADDR CS_M_MORPH_STR_0
	goto	MIOS_LCD_PrintString
CS_M_MORPH_STR_0 STRING 5, 0x00, "Group:"
CS_M_MORPH_STR_1_S STRING 8, 0x40, "Single  "
CS_M_MORPH_STR_1_G STRING 8, 0x40, "Global  "

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MORPH_Tick
	;; print selected morph groups at first line
	movlw	0x08
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintMorphGroups

	;; print group number or "global" at second line
	SET_BSR	MB64E_BASE
	incf	MB64E_MORPH_GROUP_SELECT, W, BANKED
	bz	CS_M_MORPH_Tick_PrintGlobal

CS_M_MORPH_Tick_PrintSingle
	TABLE_ADDR CS_M_MORPH_STR_1_S
	rgoto	CS_M_MORPH_Tick_Cont
CS_M_MORPH_Tick_PrintGlobal
	TABLE_ADDR CS_M_MORPH_STR_1_G
CS_M_MORPH_Tick_Cont
	call	MIOS_LCD_PrintString

	;; print 8 V-Bars
	goto	MB64E_LCD_PrintMorphBars
	

;; ==========================================================================

	;; this function transfers the current enc values to the ENC_VALUE_xx
	;; registers to overtake the morphed enc positions
CS_M_MORPH_CopyEncValues
	SET_BSR MB64E_BASE
	clrf	MB64E_CURRENT_ENTRY, BANKED
CS_M_MORPH_CopyEncValuesLoop
	call	MB64E_FSR_EncLastValue
	movff	INDF0, TMP1
	call	MB64E_FSR_EncValue
	movff	TMP1, INDF0
	bsf	INDF0, 7	; (set snap bit)
	incf	MB64E_CURRENT_ENTRY, F, BANKED
	movlw	DEFAULT_NUMBER_ENCS-1
	cpfsgt	MB64E_CURRENT_ENTRY, BANKED
	rgoto CS_M_MORPH_CopyEncValuesLoop
	return
