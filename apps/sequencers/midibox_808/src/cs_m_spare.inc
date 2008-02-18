; $Id: cs_m_midicfg.inc 73 2008-02-03 00:00:52Z tk $
;
; Spare Menu (GP LEDs are flashing here to inform the user - thats no bug!)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_SPARE_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS


	;; update LEDs
	rcall	CS_M_SPARE_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_SPARE_UpdateGPLEDs
	;; *set* all LEDs to inform the user (no bug!)
	setf	CS_MENU_GP_OVERLAY_DOUT_SR0
	setf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SPARE_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_SPARE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_SPARE_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SPARE_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    SPAREPAGE                                                                 
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;                                                                                 
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_SPARE_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SPARE_Tick_0
	rgoto	CS_M_SPARE_Tick_1
	rgoto	CS_M_SPARE_Tick_2
	rgoto	CS_M_SPARE_Tick_3
	rgoto	CS_M_SPARE_Tick_4
	rgoto	CS_M_SPARE_Tick_5
	rgoto	CS_M_SPARE_Tick_6
	rgoto	CS_M_SPARE_Tick_7

CS_M_SPARE_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_SPARE_Tick_Cont

CS_M_SPARE_Tick_1_STR	STRING 14, 0x00 + 6, "SPAREPAGE     "
CS_M_SPARE_Tick_1
	TABLE_ADDR CS_M_SPARE_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SPARE_Tick_Cont

CS_M_SPARE_Tick_2
CS_M_SPARE_Tick_3
CS_M_SPARE_Tick_4
CS_M_SPARE_Tick_5
CS_M_SPARE_Tick_6
CS_M_SPARE_Tick_7
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
	rgoto	CS_M_SPARE_Tick_Cont

CS_M_SPARE_Tick_89ABCDEF
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
	;;  	rgoto	CS_M_SPARE_Tick_Cont


CS_M_SPARE_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SPARE_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_SPARE_Tick_End
	return

;; ==========================================================================
