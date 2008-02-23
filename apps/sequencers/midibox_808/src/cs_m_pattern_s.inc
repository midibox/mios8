; $Id$
;
; MBSEQ Menu "Pattern Save"
; Partly uses code from cs_m_pattern.inc
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
CS_M_PATTERN_S_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel

	;; store pattern if one of the right buttons has been pressed
	BRA_IFCLR MIOS_PARAMETER1, 3, ACCESS, CS_M_PATTERN_S_GP_Callback_NoSave
CS_M_PATTERN_S_GP_Callback_Save
	;; change to a new pattern
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

	movff	TMP1, SEQ_PATTERN
	movff	SEQ_NEXT_PATTERN_BANK0, SEQ_PATTERN_BANK

	;; store pattern
	movff	SEQ_PATTERN, SEQ_SAVE_PATTERN
	movff	SEQ_PATTERN_BANK, SEQ_SAVE_PATTERN_BANK
	call	SEQ_DUMP_StoreTrks

	;; switch to pattern immediately
	movff	SEQ_SAVE_PATTERN_BANK, SEQ_NEXT_PATTERN_BANK0
	movff	SEQ_SAVE_PATTERN_BANK, SEQ_PATTERN_BANK0
	movff	SEQ_SAVE_PATTERN, SEQ_NEXT_PATTERN0
	movff	SEQ_SAVE_PATTERN, SEQ_PATTERN0

	;; select normal pattern page
	;; this is to ensure, thats somebody overwrites multiple patterns
	;; if he doesn't remember, that he is still in save page
	movlw	CS_MENU_PAGE_PATTERN
#if 0
	call	CS_M_HLP_ChangePage

	;; print message
	TABLE_ADDR CS_M_PATTERN_S_GP_Callback_SaveM0
	call	MIOS_LCD_PrintMessage
	call	CS_M_PATTERN_PrnP	; (5 characters)
	movlw	4
	call	SEQ_LCD_PrintSpaces

	TABLE_ADDR CS_M_PATTERN_S_GP_Callback_SaveM1
	call	MIOS_LCD_PrintMessage

	;; DIRTY!
	;; ensure that GP LEDs updated immediately
	rcall	CS_M_PATTERN_S_UpdateGPLEDs
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, SEQ_GP_LED_L
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, SEQ_GP_LED_R
	return
#else
	goto	CS_M_HLP_ChangePage
#endif


CS_M_PATTERN_S_GP_Callback_NoSave
	;; update LEDs
	rcall	CS_M_PATTERN_S_UpdateGPLEDs

	;; DIRTY!
	;; ensure that GP LEDs updated immediately
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, SEQ_GP_LED_L
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, SEQ_GP_LED_R

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


CS_M_PATTERN_S_GP_Callback_SaveM0
	STRING 11, 0x00 + 0, "** Pattern "
CS_M_PATTERN_S_GP_Callback_SaveM1
	STRING 20, 0x40 + 0, "** stored           "	

;; --------------------------------------------------------------------------
CS_M_PATTERN_S_UpdateGPLEDs
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
CS_M_PATTERN_S_Init
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_PATTERN_S_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; copy current pattern number into parameter
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_NEXT_PATTERN0, 7, BANKED, CS_M_PATTERN_S_Init_PMuted
CS_M_PATTERN_S_Init_PNotMuted
	incf	SEQ_NEXT_PATTERN0, W, BANKED
	rgoto	CS_M_PATTERN_S_Init_P_Cont
CS_M_PATTERN_S_Init_PMuted
	movlw	0x00
CS_M_PATTERN_S_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; update LEDs
	rcall	CS_M_PATTERN_S_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATTERN_S_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    SAVE P. 1:A1* ________________                                              
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;  A/a  B/b  C/c  D/d  E/e  F/f  G/g  H/h   1    2    3    4    5    6    7    8  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    SAVE 1:A1*
	;; 0000000111111111
	;; ________________
	;; 2222222222333333

	;; if fast display update, and counter < 0x10, set it to 0x10
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_PATTERN_S_Tick_NotFast
CS_M_PATTERN_S_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_PATTERN_S_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_PATTERN_S_Tick_NotCtrlFix
CS_M_PATTERN_S_Tick_NotFast

	;; print meter segments 16-31?
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_PATTERN_S_Tick_Meters

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_PATTERN_S_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_PATTERN_S_Tick_0
	rgoto	CS_M_PATTERN_S_Tick_1
	rgoto	CS_M_PATTERN_S_Tick_2
	rgoto	CS_M_PATTERN_S_Tick_3
	rgoto	CS_M_PATTERN_S_Tick_4
	rgoto	CS_M_PATTERN_S_Tick_5
	rgoto	CS_M_PATTERN_S_Tick_6
	rgoto	CS_M_PATTERN_S_Tick_7

CS_M_PATTERN_S_Tick_0		; re-used from CS_M_MUTE_Init !!!
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_PATTERN_S_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_PATTERN_S_Tick_1_STR	STRING 8, 0x00 + 6, "SAVE P. "
#else
CS_M_PATTERN_S_Tick_1_STR	STRING 5, 0x00 + 6, "SAVE "
#endif
CS_M_PATTERN_S_Tick_1
	TABLE_ADDR CS_M_PATTERN_S_Tick_1_STR
	call	MIOS_LCD_PrintString

	;; pattern name (5 characters)
	call	CS_M_PATTERN_PrnP

	;; additional space
	movlw	' '
	call	MIOS_LCD_PrintChar

	rgoto	CS_M_PATTERN_S_Tick_Cont

CS_M_PATTERN_S_Tick_2
	;; nothing to do... (normaly filled by meters)

	;; time to init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	rgoto	CS_M_PATTERN_S_Tick_Cont

CS_M_PATTERN_S_Tick_3
	;; print remaining 4 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 36
#else
	movlw	0x40 + 16
#endif
	call	MIOS_LCD_CursorSet
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_PATTERN_S_Tick_Cont

CS_M_PATTERN_S_Tick_4
CS_M_PATTERN_S_Tick_5
CS_M_PATTERN_S_Tick_6
CS_M_PATTERN_S_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_PATTERN_S_Tick_Cont

CS_M_PATTERN_S_Tick_89ABCDEF_STR
	db	" A/a  B/b  C/c  D/d  E/e  F/f  G/g  H/h   1    2    3    4    5    6    7    8  "
CS_M_PATTERN_S_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07

	TABLE_ADDR_MUL_W CS_M_PATTERN_S_Tick_89ABCDEF_STR, 10
	call	CS_M_HLP_Print10Chars
	rgoto	CS_M_PATTERN_S_Tick_Cont
#endif


CS_M_PATTERN_S_Tick_Meters
	;; set cursor
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
#if DEFAULT_LCD_WIDTH == 80
	addlw	0x00 + 20
#else
	addlw	0x40 + 0
#endif
	call	MIOS_LCD_CursorSet

	;; print meter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	call	CS_M_PATTERN_PrnM

	;;  	rgoto	CS_M_PATTERN_S_Tick_Cont

CS_M_PATTERN_S_Tick_Cont
	;; Tick function will be called 32 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_PATTERN_S_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_S_Tick_End

CS_M_PATTERN_S_Tick_End
	return

;; ==========================================================================
