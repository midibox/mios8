; $Id$
;
; MBSEQ Menu "Track Direction"
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
CS_M_TRKDIR_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; exit if button number >= 7
	movlw	7
	cpfslt	MIOS_PARAMETER1, ACCESS
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; set direction
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Set

	;; update LEDs
	rcall	CS_M_TRKDIR_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKDIR_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-7: direction
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKDIR_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKDIR_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; determine cursor pos
	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Get
	movwf	CS_MENU_CURSOR_POS

	;; update LEDs
	rgoto	CS_M_TRKDIR_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKDIR_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    DIRECTION     Forward                                                     
	;; 00000001111111111111222222222233333333334444444444555555555566666666667777777777
	;; Fwd. Bck. PP.  Pnd. R.D. R.S. RD+S                                              
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    DIRECTION 
	;; 0000000111111111
	;; Forward         
	;; 2222222222222222


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKDIR_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKDIR_Tick_0
	rgoto	CS_M_TRKDIR_Tick_1
	rgoto	CS_M_TRKDIR_Tick_2
	rgoto	CS_M_TRKDIR_Tick_3
	rgoto	CS_M_TRKDIR_Tick_4
	rgoto	CS_M_TRKDIR_Tick_5
	rgoto	CS_M_TRKDIR_Tick_6
	rgoto	CS_M_TRKDIR_Tick_7

CS_M_TRKDIR_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_1_STR	STRING 14, 0x00 + 6, "DIRECTION     "
CS_M_TRKDIR_Tick_1
	TABLE_ADDR CS_M_TRKDIR_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_2
	;; print direction name
	;; get pointer to item depending on direction mode
	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Get
	TABLE_ADDR_MUL_W CS_M_TRKDIR_NAME_TABLE, 10

#if DEFAULT_LCD_WIDTH == 80
	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#else
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	10
	call	MIOS_LCD_PrintPreconfString
	movlw	6
	call	SEQ_LCD_PrintSpaces
#endif

	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_3
CS_M_TRKDIR_Tick_4
CS_M_TRKDIR_Tick_5
CS_M_TRKDIR_Tick_6
CS_M_TRKDIR_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_89AB_STR
	db	"Fwd. Bck. PP.  Pnd. R.D. R.S. RD+S                                    "
CS_M_TRKDIR_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	TABLE_ADDR_MUL_W CS_M_TRKDIR_Tick_89AB_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKDIR_Tick_Cont


CS_M_TRKDIR_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKDIR_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKDIR_Tick_End
	return

;; ==========================================================================

CS_M_TRKDIR_NAME_TABLE
	db	"Forward   "
	db	"Backward  "
	db	"PingPong  "
	db	"Pendulum  "
	db	"Rand.Dir  "
	db	"Rand.Step "
	db	"Rand.D+S  "
