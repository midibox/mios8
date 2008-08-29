; $Id$
;
; MBSEQ Menu "Song"
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
CS_M_SONG_GP_Callback
	;; ignore if no BankStick for songmode
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; call general GP routine for pattern (and song) selection
	clrf	CS_PATTERN_NEW_BANK	; ensure that no new bank is selected
	call	CS_M_HLP_GPPatternSel

	;; change song if one of the right GP buttons has been pressed
	BRA_IFCLR MIOS_PARAMETER1, 3, ACCESS, CS_M_SONG_GP_Callback_Cont
CS_M_SONG_GP_Callback_PChng

	;; change to a new song
	;; 7th bit cleared: CS_MENU_PARAMETER_L = song number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

	movff	TMP1, SEQ_SONG

	;; restore song
	call	SEQ_DUMP_RestoreSong


CS_M_SONG_GP_Callback_Cont
	;; DIRTY!
	;; ensure that GP LEDs updated immediately
	rcall	CS_M_SONG_UpdateGPLEDs
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, SEQ_GP_LED_L
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, SEQ_GP_LED_R

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_SONG_UpdateGPLEDs
	;; if no BankStick for songmode: flash all LEDs
	BRA_IFSET MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, CS_M_SONG_UpdateGPLEDs_BSOk
CS_M_SONG_UpdateGPLEDs_NoBS
	;; flash all LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	setf	CS_MENU_GP_OVERLAY_DOUT_SR0
	setf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

CS_M_SONG_UpdateGPLEDs_BSOk
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
	;; + state of GP buttons if SHIFT button not pressed (no chain mode)
	decf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	iorwf	CS_PATTERN_GP_STATE_R, W
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SONG_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_SONG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rcall	CS_M_SONG_UpdateGPLEDs

	;; if no BankStick for songmode: print special message
	BRA_IFSET MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, CS_M_SONG_Init_BSOk
CS_M_SONG_Init_NoBS
	;; print message
	call	SEQ_LCD_Clear
	call	SEQ_LCD_PrintStringAfterCall
	STRING	16, 0x00, "* No BankStick *"
	call	SEQ_LCD_PrintStringAfterCall
	STRING	16, 0x40, "* for song :-/ *"
	return

CS_M_SONG_Init_BSOk
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; copy current song number into parameter
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, CS_M_SONG_Init_PMuted
CS_M_SONG_Init_PNotMuted
	incf	SEQ_SONG, W, BANKED
	rgoto	CS_M_SONG_Init_P_Cont
CS_M_SONG_Init_PMuted
	movlw	0x00
CS_M_SONG_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SONG_Tick
	;; update LEDs
	rcall	CS_M_SONG_UpdateGPLEDs

	;; ignore if no BankStick for songmode
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

	;; always update pattern LEDs (important if mutes have been changed via MIDI)
	rcall	CS_M_SONG_UpdateGPLEDs

	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    SONG      A1* ________________                                              
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   A    B    C    D    E    F    G    H    1    2    3    4    5    6    7    8  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    SONG   A1*
	;; 0000000111111111
	;; ________________
	;; 2222222222333333

	;; if fast display update, and counter < 0x10, set it to 0x10
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_SONG_Tick_NotFast
CS_M_SONG_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_SONG_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_SONG_Tick_NotCtrlFix
CS_M_SONG_Tick_NotFast

	;; print meter segments 16-31?
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SONG_Tick_Meters

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_SONG_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SONG_Tick_0
	rgoto	CS_M_SONG_Tick_1
	rgoto	CS_M_SONG_Tick_2
	rgoto	CS_M_SONG_Tick_3
	rgoto	CS_M_SONG_Tick_4
	rgoto	CS_M_SONG_Tick_5
	rgoto	CS_M_SONG_Tick_6
	rgoto	CS_M_SONG_Tick_7

CS_M_SONG_Tick_0
	;; print chain info (6 chars)
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet
	call	CS_M_PATTERN_PrnChain
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	7, 0x00 + 6, "SONG   "

	;; pattern name (3 characters)
	rcall	CS_M_SONG_PrnS

	;; additional space
	movlw	' '
	call	MIOS_LCD_PrintChar

	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_2
	;; nothing to do... (normaly filled by meters)

	;; time to init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_3
	;; print remaining 4 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 36
#else
	movlw	0x40 + 16
#endif
	call	MIOS_LCD_CursorSet
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_4
CS_M_SONG_Tick_5
CS_M_SONG_Tick_6
CS_M_SONG_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_89ABCDEF_STR
	db	"  A    B    C    D    E    F    G    H    1    2    3    4    5    6    7    8  "
CS_M_SONG_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07

	TABLE_ADDR_MUL_W CS_M_SONG_Tick_89ABCDEF_STR, 10
	call	CS_M_HLP_Print10Chars
	rgoto	CS_M_SONG_Tick_Cont
#endif


CS_M_SONG_Tick_Meters
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

	;;  	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_Cont
	;; Tick function will be called 32 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_SONG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_SONG_Tick_End

CS_M_SONG_Tick_End
	return
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This help function prints the song number (3 characters)
;; --------------------------------------------------------------------------
CS_M_SONG_PrnS
	SET_BSR	SEQ_BASE

	;; write converted CS_MENU_PARAMETER_L to TMP2
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP2

	;; if new section selection and cursor flashes: print two spaces instead of pattern number
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_SONG_PrnS_Ptn_NoFlsh
CS_M_SONG_PrnS_Ptn_FlshChk
	SET_BSR	SEQ_BASE
	movf	TMP2, W
	xorwf	SEQ_SONG, W, BANKED
	bz	CS_M_SONG_PrnS_Ptn_NoFlsh
CS_M_SONG_PrnS_Ptn_Flsh
	movlw	2
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SONG_PrnS_Ptn_Cont
CS_M_SONG_PrnS_Ptn_NoFlsh
	;; print pattern
	movf	TMP2, W
	call	SEQ_LCD_PrnPattern
CS_M_SONG_PrnS_Ptn_Cont

	;; print '*' if song bankstick not available
	movlw	' '
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	movlw	'*'
	goto	MIOS_LCD_PrintChar


