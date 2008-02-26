; $Id$
;
; MBSEQ Menu "Track Loop points"
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
CS_M_TRKLOOP_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; determine step depending on current view
	SET_BSR	SEQ_BASE
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED	; (switches between upper/lower and A/B steps)
	andlw	0x30
	addwf	MIOS_PARAMETER1, W
	movwf	CS_MENU_PARAMETER_L

	;; set LAST/LOOP value depending on shift button
	;; for more induitive usage: if selected step < LOOP, change LOOP instead of LAST independent of SHIFT button
	movlw	SEQ_CC_OFFSET_LOOP
	call	CS_M_HLP_TrkCC_Get
	cpfslt	CS_MENU_PARAMETER_L
	rgoto	CS_M_TRKLOOP_GP_Callback_Shft
CS_M_TRKLOOP_GP_Callback_Loop
	movlw	SEQ_CC_OFFSET_LOOP
	rgoto	CS_M_TRKLOOP_GP_Callback_Cont
CS_M_TRKLOOP_GP_Callback_Shft

	;; if button number < LOOP
	movlw	SEQ_CC_OFFSET_LAST
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	movlw	SEQ_CC_OFFSET_LOOP
	;; 	rgoto	CS_M_TRKLOOP_GP_Callback_Cont
CS_M_TRKLOOP_GP_Callback_Cont
	call	CS_M_HLP_TrkCC_Set

	;; update LEDs
	rcall	CS_M_TRKLOOP_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKLOOP_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; don't flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; determine first/last/loop pointer depending on displayed viewview
	;; -> first step copied to TMP1
	;; -> loop point copied to TMP2
	;; -> last step copied to TMP3
	SET_BSR	SEQ_BASE
	movlw	0x00
	movwf	TMP1
	movlw	SEQ_CC_OFFSET_LOOP
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP2
	movlw	SEQ_CC_OFFSET_LAST
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP3

	;; start with selected step view
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED
	andlw	0x30
	movwf	PRODL
CS_M_TRKLOOP_UpdateGPLEDs_Loop
	;; set LED if:
	;; step counter >= loop point (TMP2)
	;; step counter <= last step (TMP3)
	movf	TMP2, W
	bz	CS_M_TRKLOOP_UpdateGPLEDs_Loop0Ok
	addlw	-1
	cpfsgt	PRODL
	rgoto	CS_M_TRKLOOP_UpdateGPLEDs_LoopNx
CS_M_TRKLOOP_UpdateGPLEDs_Loop0Ok

	incf	TMP3, W
	cpfslt	PRODL
	rgoto	CS_M_TRKLOOP_UpdateGPLEDs_LoopNx

	movf	PRODL, W
	call	MIOS_HLP_GetBitORMask
	btfss	PRODL, 3
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
	btfsc	PRODL, 3
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F
CS_M_TRKLOOP_UpdateGPLEDs_LoopNx
	;; loop 16 times
	incf	PRODL, F
	movf	PRODL, W
	andlw	0x0f
	bnz	CS_M_TRKLOOP_UpdateGPLEDs_Loop
	
CS_M_TRKLOOP_UpdateGPLEDs_End
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKLOOP_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKLOOP_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_TRKLOOP_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKLOOP_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    LOOPPOINTS    Loop:xx   End:xx                                              
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    LOOPPOINTS
	;; 0000000111111111
	;; Loop:xx   End:xx
	;; 2222222233333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKLOOP_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKLOOP_Tick_0
	rgoto	CS_M_TRKLOOP_Tick_1
	rgoto	CS_M_TRKLOOP_Tick_2
	rgoto	CS_M_TRKLOOP_Tick_3
	rgoto	CS_M_TRKLOOP_Tick_4
	rgoto	CS_M_TRKLOOP_Tick_5
	rgoto	CS_M_TRKLOOP_Tick_6
	rgoto	CS_M_TRKLOOP_Tick_7

CS_M_TRKLOOP_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKLOOP_Tick_Cont

CS_M_TRKLOOP_Tick_1_STR	STRING 14, 0x00 + 6, "LOOPPOINTS    "
CS_M_TRKLOOP_Tick_1
	TABLE_ADDR CS_M_TRKLOOP_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLOOP_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKLOOP_Tick_2_STR	STRING 5, 0x00 + 20, "Loop:"
#else
CS_M_TRKLOOP_Tick_2_STR	STRING 5, 0x40 + 00, "Loop:"
#endif
CS_M_TRKLOOP_Tick_2
	TABLE_ADDR CS_M_TRKLOOP_Tick_2_STR
	call	MIOS_LCD_PrintString

	movlw	SEQ_CC_OFFSET_LOOP
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

	movlw	3
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKLOOP_Tick_Cont


#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKLOOP_Tick_3_STR	STRING 4, 0x00 + 30, "End:"
#else
CS_M_TRKLOOP_Tick_3_STR	STRING 4, 0x40 + 10, "End:"
#endif
CS_M_TRKLOOP_Tick_3
	TABLE_ADDR CS_M_TRKLOOP_Tick_3_STR
	call	MIOS_LCD_PrintString

	movlw	SEQ_CC_OFFSET_LAST
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKLOOP_Tick_Cont

CS_M_TRKLOOP_Tick_4
CS_M_TRKLOOP_Tick_5
CS_M_TRKLOOP_Tick_6
CS_M_TRKLOOP_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKLOOP_Tick_Cont

CS_M_TRKLOOP_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; set cursor depending on current segment
	call	CS_M_HLP_SetSegmentCursor

	;; print two step numbers (depends on step view)
	SET_BSR	SEQ_BASE
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED	; (switches between upper/lower and A/B steps)
	andlw	0x30
	movwf	TMP1
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	rlf	WREG, W
	andlw	0x0e
	iorwf	TMP1, F

	movlw	1
	call	SEQ_LCD_PrintSpaces
	incf	TMP1, W
	call	MIOS_LCD_PrintBCD2
	movlw	2
	call	SEQ_LCD_PrintSpaces
	
	movlw	1
	call	SEQ_LCD_PrintSpaces
	incf	TMP1, W
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	2
	call	SEQ_LCD_PrintSpaces	
#endif
	;;  	rgoto	CS_M_TRKLOOP_Tick_Cont


CS_M_TRKLOOP_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKLOOP_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKLOOP_Tick_End
	return

;; ==========================================================================
