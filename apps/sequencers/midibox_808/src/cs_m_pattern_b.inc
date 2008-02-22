; $Id$
;
; MBSEQ Menu "Pattern Bank"
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
CS_M_PATTERN_B_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; ignore button >= 8
	btfsc	MIOS_PARAMETER1, 3
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; change bank
	movff	MIOS_PARAMETER1, SEQ_PATTERN_BANK
	movff	SEQ_NEXT_PATTERN0, SEQ_PATTERN
	call	SEQ_CORE_ChangePatternSynched

	;; update LEDs
	rcall	CS_M_PATTERN_B_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_PATTERN_B_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	SET_BSR	SEQ_BASE
	movf	SEQ_NEXT_PATTERN_BANK0, W, BANKED
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_PATTERN_B_Init
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_PATTERN_B_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_PATTERN_B_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATTERN_B_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    P.BANK  1:A1* ________________                                              
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   1    2    3    4    5    6    7    8                                          
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    BANK 1:A1*
	;; 0000000111111111
	;; ________________
	;; 2222222222333333

	;; if fast display update, and counter < 0x10, set it to 0x10
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_PATTERN_B_Tick_NotFast
CS_M_PATTERN_B_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_PATTERN_B_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_PATTERN_B_Tick_NotCtrlFix
CS_M_PATTERN_B_Tick_NotFast

	;; print meter segments 16-31?
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_PATTERN_B_Tick_Meters

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_PATTERN_B_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_PATTERN_B_Tick_0
	rgoto	CS_M_PATTERN_B_Tick_1
	rgoto	CS_M_PATTERN_B_Tick_2
	rgoto	CS_M_PATTERN_B_Tick_3
	rgoto	CS_M_PATTERN_B_Tick_4
	rgoto	CS_M_PATTERN_B_Tick_5
	rgoto	CS_M_PATTERN_B_Tick_6
	rgoto	CS_M_PATTERN_B_Tick_7

CS_M_PATTERN_B_Tick_0		; re-used from CS_M_MUTE_Init !!!
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_PATTERN_B_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_PATTERN_B_Tick_1_STR	STRING 8, 0x00 + 6, "P.BANK  "
#else
CS_M_PATTERN_B_Tick_1_STR	STRING 5, 0x00 + 6, "BANK "
#endif
CS_M_PATTERN_B_Tick_1
	TABLE_ADDR CS_M_PATTERN_B_Tick_1_STR
	call	MIOS_LCD_PrintString

	;; pattern name (5 characters)
	call	CS_M_PATTERN_PrnP

	;; additional space
	movlw	' '
	call	MIOS_LCD_PrintChar

	rgoto	CS_M_PATTERN_B_Tick_Cont

CS_M_PATTERN_B_Tick_2
	;; nothing to do... (normaly filled by meters)

	;; time to init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	rgoto	CS_M_PATTERN_B_Tick_Cont

CS_M_PATTERN_B_Tick_3
	;; print remaining 4 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 36
#else
	movlw	0x40 + 16
#endif
	call	MIOS_LCD_CursorSet
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_PATTERN_B_Tick_Cont

CS_M_PATTERN_B_Tick_4
CS_M_PATTERN_B_Tick_5
CS_M_PATTERN_B_Tick_6
CS_M_PATTERN_B_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_PATTERN_B_Tick_Cont

CS_M_PATTERN_B_Tick_89ABCDEF_STR
	db	"  1    2    3    4    5    6    7    8                                          "
CS_M_PATTERN_B_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07

	TABLE_ADDR_MUL_W CS_M_PATTERN_B_Tick_89ABCDEF_STR, 10
	call	CS_M_HLP_Print10Chars
	rgoto	CS_M_PATTERN_B_Tick_Cont
#endif


CS_M_PATTERN_B_Tick_Meters
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

	;;  	rgoto	CS_M_PATTERN_B_Tick_Cont

CS_M_PATTERN_B_Tick_Cont
	;; Tick function will be called 32 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_PATTERN_B_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_B_Tick_End

CS_M_PATTERN_B_Tick_End
	return

;; ==========================================================================
