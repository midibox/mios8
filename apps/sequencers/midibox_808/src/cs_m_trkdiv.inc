; $Id$
;
; MBSEQ Menu "Timebase"
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
CS_M_TRKDIV_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; if button number == 15 -> toggle "Synch To Measure" flag
	movf	MIOS_PARAMETER1, W
	xorlw	15
	bz	CS_M_TRKDIV_GP_Callback_STB

CS_M_TRKDIV_GP_Callback_TimeBase
	;; else select timebase depending on button number
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	rcall	CS_M_TRKDIV_QuickSelTableGet
	movwf	TMP1			; store for triplet change

	;; set/clear triplet flag (TMP1.7)
	movlw	0x00
	btfsc	TMP1, 7
	movlw	0x01
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKDIV_TRPChanged

	;; change divider value
	movf	TMP1, W
	andlw	0x3f
	movwf	CS_MENU_PARAMETER_L	; set parameter
	rcall	CS_M_TRKDIV_DivChanged

	;; cursor pos -> selected timebase (for LED output)
	movff	CS_MENU_CURSOR_POS, CS_TRKDIV_TIMEBASE

	rgoto	CS_M_TRKDIV_GP_Callback_Cont


CS_M_TRKDIV_GP_Callback_STB
	;; toggle "Synch To Bar" flag
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Get
	xorlw	(1 << 0)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Set

	;; 	rgoto	CS_M_TRKDIV_GP_Callback_Cont


CS_M_TRKDIV_GP_Callback_Cont
	;; update LEDs
	rcall	CS_M_TRKDIV_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKDIV_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-15: selected timebase
	BRA_IFSET CS_TRKDIV_TIMEBASE, 7, ACCESS, CS_M_TRKDIV_UpdateGPLEDs_NoQuick
CS_M_TRKDIV_UpdateGPLEDs_Quick
	movf	CS_TRKDIV_TIMEBASE, W
	call	MIOS_HLP_GetBitORMask	; set appr. LED
	btfss	CS_TRKDIV_TIMEBASE, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	CS_TRKDIV_TIMEBASE, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
CS_M_TRKDIV_UpdateGPLEDs_NoQuick

	;; LED 16: ResetAtTrkEnd (Synch To Measure)
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 0)
	skpz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR1, 7
	
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKDIV_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; determine timebase setting -> CS_TRKDIV_TIMEBASE (for LED output)
	;; get current value -> TMP1
	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; set flag 7 if triplet mode
	call	CS_M_HLP_TrkCC_Get
	btfsc	WREG, 1
	bsf	TMP1, 7	

	;; search position in table (0xff if not found)
	movf	TMP1, W
	rcall	CS_M_TRKDIV_QuickSelTableSearch
	movwf	CS_TRKDIV_TIMEBASE

	;; update LEDs
	rgoto	CS_M_TRKDIV_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    TIMEBASE      Selection: 16T                                              
	;; 00000001111111111111222222222233333333334444444444555555555566666666667777777777
	;;   1    1T   2    2T   4    4T   8    8T  16   16T  32   32T  64   Odd  EOdd STB 
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    TIMEBASE  
	;; 0000000111111111
	;; Selection: 16T  
	;; 2222222222333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKDIV_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKDIV_Tick_0
	rgoto	CS_M_TRKDIV_Tick_1
	rgoto	CS_M_TRKDIV_Tick_2
	rgoto	CS_M_TRKDIV_Tick_3
	rgoto	CS_M_TRKDIV_Tick_4
	rgoto	CS_M_TRKDIV_Tick_5
	rgoto	CS_M_TRKDIV_Tick_6
	rgoto	CS_M_TRKDIV_Tick_7

CS_M_TRKDIV_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_1_STR	STRING 14, 0x00 + 6, "TIMEBASE      "
CS_M_TRKDIV_Tick_1
	TABLE_ADDR CS_M_TRKDIV_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIV_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKDIV_Tick_2_TB_STR  STRING 10, 0x00 + 20, "Selection:"
CS_M_TRKDIV_Tick_2_STB_STR STRING 13, 0x00 + 20, "Sync to Bar: "
#else
CS_M_TRKDIV_Tick_2_TB_STR  STRING 10, 0x40 + 0, "Selection:"
CS_M_TRKDIV_Tick_2_STB_STR STRING 13, 0x40 + 0, "Sync to Bar: "
#endif
CS_M_TRKDIV_Tick_2
	;; if last button has been pressed: print "Sync to Bar: " instead
	movf	CS_MENU_CURSOR_POS, W
	xorlw	0x0f
	bz	CS_M_TRKDIV_Tick_2_STB
CS_M_TRKDIV_Tick_2_TB
	TABLE_ADDR CS_M_TRKDIV_Tick_2_TB_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_2_STB
	TABLE_ADDR CS_M_TRKDIV_Tick_2_STB_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_3
	;; if last button has been pressed: print 4 spaces instead
	movf	CS_MENU_CURSOR_POS, W
	xorlw	0x0f
	bz	CS_M_TRKDIV_Tick_3_STB

CS_M_TRKDIV_Tick_3_TB
	;; print current selection
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 30
#else
	movlw	0x40 + 11
#endif
	call	MIOS_LCD_CursorSet

	;; stored in CS_TRKDIV_TIMEBASE
	movf	CS_TRKDIV_TIMEBASE, W
	TABLE_ADDR_MUL_W CS_M_TRKDIV_QuickSelStrTable, 4
	movlw	4
	call	MIOS_LCD_PrintPreconfString

	movlw	6
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_3_STB
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 33
#else
	movlw	0x40 + 13
#endif
	call	MIOS_LCD_CursorSet

	;; print on/off depending on STB flag
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 0)
	call	SEQ_LCD_PrnOnOff

	;; print 4 spaces
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_4
CS_M_TRKDIV_Tick_5
CS_M_TRKDIV_Tick_6
CS_M_TRKDIV_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKDIV_Tick_Cont

CS_M_TRKDIV_Tick_89ABCDEF_STR
	db	"  1    1T   2    2T   4    4T   8    8T  16   16T  32   32T  64   Odd  EOdd STB "
CS_M_TRKDIV_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKDIV_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKDIV_Tick_Cont


CS_M_TRKDIV_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKDIV_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKDIV_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Help Functions to change CC settings
;; --------------------------------------------------------------------------
CS_M_TRKDIV_DivChanged
	movff	CS_MENU_PARAMETER_L, MIOS_PARAMETER1
	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	call	CS_M_HLP_TrkCC_Set
	return

CS_M_TRKDIV_TRPChanged
	movff	CS_MENU_PARAMETER_L, TMP5	; RMW operation - store menu parameter
	
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; modify bit 1
	call	CS_M_HLP_TrkCC_Get
	andlw	~(1 << 1)
	btfsc	CS_MENU_PARAMETER_L, 0
	iorlw	(1 << 1)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Set

	movff	TMP5, CS_MENU_PARAMETER_L
	return

;; ==========================================================================

CS_M_TRKDIV_QuickSelTable
	db	63, 63 | 0x80	; ...+1, bit 7 selects triplets
	db	31, 31 | 0x80
	db	15, 15 | 0x80
	db	 7,  7 | 0x80
	db	 3,  3 | 0x80
	db	 1,  1 | 0x80
	db	 0,  5
	db	 9 | 0x80, 0xff

CS_M_TRKDIV_QuickSelStrTable
	db	" 1  "
	db	" 1T "
	db	" 2  "
	db	" 2T "
	db	" 4  "
	db	" 4T "
	db	" 8  "
	db	" 8T "
	db	" 16 "
	db	" 16T"
	db	" 32 "
	db	" 32T"
	db	" 64 "
	db	"Odd "
	db	"EOdd"

	;; prints 4 characters
	;; IN: item number (0-14) in WREG
CS_M_TRKDIV_QuickSelStrPrint
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x1c
	addlw	LOW(CS_M_TRKDIV_QuickSelStrTable)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_TRKDIV_QuickSelStrTable)
	addwfc	TBLPTRH, F
	movlw	4
	goto	MIOS_LCD_PrintPreconfString

	;; returns mapped divider for quick selection
	;; IN: item number (0-7) in WREG
	;; OUT: mapped value in WREG (if bit 7 set: activate triplet flag)
CS_M_TRKDIV_QuickSelTableGet
	addlw	LOW(CS_M_TRKDIV_QuickSelTable)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_TRKDIV_QuickSelTable)
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	return

	;; searches in table for a given divider
	;; IN: divider value in WREG (bit 7 corresponds to triplet flag)
	;; OUT: table position in WREG (0xff if not found)
CS_M_TRKDIV_QuickSelTableSearch
	movwf	PRODL		; stores the search value
	clrf	PRODH		; used as loop counter
	TABLE_ADDR CS_M_TRKDIV_QuickSelTable
CS_M_TRKDIV_QuickSelTableSearchL
	tblrd*+
	movf	TABLAT, W
	cpfseq	PRODL, ACCESS
	rgoto CS_M_TRKDIV_QuickSelTableSearchN
	movf	PRODH, W
	return
CS_M_TRKDIV_QuickSelTableSearchN
	incf	PRODH, F
	BRA_IFCLR PRODH, 4, ACCESS, CS_M_TRKDIV_QuickSelTableSearchL
	retlw	0xff

;; ==========================================================================
