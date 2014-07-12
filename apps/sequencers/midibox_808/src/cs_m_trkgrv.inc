; $Id$
;
; MBSEQ Menu "Groove Style"
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
CS_M_TRKGRV_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; button 7 (Shf6?) currently ignored - no assignment
	movf	MIOS_PARAMETER1, W
	xorlw	0x07
	skpnz
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; left side: groove style, right side: groove value (*2)
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L

	;; right side: leftshift value by 1 (*2)
	clrc
	btfsc	MIOS_PARAMETER1, 3
	rlf	CS_MENU_PARAMETER_L, F

	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	btfsc	MIOS_PARAMETER1, 3
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	call	CS_M_HLP_TrkCC_Set

	;; update LEDs
	rcall	CS_M_TRKGRV_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKGRV_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0_A
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1_A

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-8: groove style
	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0_A

	;; LED 9-16: intensity/2 (4bit value rightshifted)
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	call	CS_M_HLP_TrkCC_Get
	clrc
	rrf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1_A

	;; copy A into B LED layer (no dimming used)
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0_A, CS_MENU_GP_OVERLAY_DOUT_SR0_B
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1_A, CS_MENU_GP_OVERLAY_DOUT_SR1_B

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKGRV_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_TRKGRV_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    GROOVE          Shuffle     # 0                                              
	;; 00000001111111111111222222222222333333334444444444555555555566666666667777777777
	;;  Off Shfl IShf Sfh2 Shf3 Shf4 Shf5        0    2    4    6    8   10   12   14  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    GROOVE    
	;; 0000000111111111
	;; Shuffle     # 0 
	;; 2222222222223333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKGRV_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKGRV_Tick_0
	rgoto	CS_M_TRKGRV_Tick_1
	rgoto	CS_M_TRKGRV_Tick_2
	rgoto	CS_M_TRKGRV_Tick_3
	rgoto	CS_M_TRKGRV_Tick_4
	rgoto	CS_M_TRKGRV_Tick_5
	rgoto	CS_M_TRKGRV_Tick_6
	rgoto	CS_M_TRKGRV_Tick_7

CS_M_TRKGRV_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKGRV_Tick_Cont

CS_M_TRKGRV_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x00 + 6, "GROOVE        "
	rgoto	CS_M_TRKGRV_Tick_Cont

CS_M_TRKGRV_Tick_2
	;; print groove style
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 20
#else
	movlw	0x40 + 00
#endif
	call	MIOS_LCD_CursorSet

	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_GROOVE_GetName	; 12 characters
	rgoto	CS_M_TRKGRV_Tick_Cont

CS_M_TRKGRV_Tick_3
	;; print intensity
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 32
#else
	movlw	0x40 + 12
#endif
	call	MIOS_LCD_CursorSet

#if DEFAULT_LCD_WIDTH == 80
	;; 2 spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces
#endif

	;; 3 chars
	movlw	'#'
	call	MIOS_LCD_PrintChar
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD2

	;; 3 spaces
	movlw	3
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKGRV_Tick_Cont

CS_M_TRKGRV_Tick_4
CS_M_TRKGRV_Tick_5
CS_M_TRKGRV_Tick_6
CS_M_TRKGRV_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKGRV_Tick_Cont

CS_M_TRKGRV_Tick_89ABCDEF_STR
	db	" Off Shfl IShf Sfh2 Shf3 Shf4 Shf5        0    2    4    6    8   10   12   14  "
CS_M_TRKGRV_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKGRV_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKGRV_Tick_Cont


CS_M_TRKGRV_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKGRV_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKGRV_Tick_End
	return

;; ==========================================================================
