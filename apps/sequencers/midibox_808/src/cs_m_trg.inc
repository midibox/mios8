; $Id$
;
; MBSEQ Menu "Manual Trigger"
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
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_TRG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; determine step depending on current view
	SET_BSR	SEQ_BASE
	swapf	SEQ_SELECTED_STEP_VIEW, W, BANKED	; (switches between upper/lower and A/B steps)
	andlw	0x30
	addwf	MIOS_PARAMETER1, W
	movwf	CS_MENU_PARAMETER_L

	;; forward manual trigger to SEQ_CORE
	SET_BSR	SEQ_BASE
	movwf	SEQ_CURRENT_STEP, BANKED
	goto	SEQ_CORE_ManualTrigger


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRG_Init
	;; no parameter table!

	;; turn off the LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; install callback function for general purpose buttons, used to trigger the steps here
	TABLE_ADDR CS_M_TRG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    MANUALSTEP                                                                  
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    MANUALSTEP
	;; 0000000111111111
	;;                 
	;; 2222222233333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRG_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRG_Tick_0
	rgoto	CS_M_TRG_Tick_1
	rgoto	CS_M_TRG_Tick_2
	rgoto	CS_M_TRG_Tick_3
	rgoto	CS_M_TRG_Tick_4
	rgoto	CS_M_TRG_Tick_5
	rgoto	CS_M_TRG_Tick_6
	rgoto	CS_M_TRG_Tick_7

CS_M_TRG_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x00 + 6, "MANUALSTEP    "
	rgoto	CS_M_TRG_Tick_Cont


CS_M_TRG_Tick_2
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 20
#else
	movlw	0x40 + 0
#endif
	call	MIOS_LCD_CursorSet

	movlw	10
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRG_Tick_Cont


CS_M_TRG_Tick_3
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 30
#else
	movlw	0x40 + 10
#endif
	call	MIOS_LCD_CursorSet

	movlw	10
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRG_Tick_Cont


CS_M_TRG_Tick_4
CS_M_TRG_Tick_5
CS_M_TRG_Tick_6
CS_M_TRG_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRG_Tick_Cont

CS_M_TRG_Tick_89ABCDEF
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
	;;  	rgoto	CS_M_TRG_Tick_Cont


CS_M_TRG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRG_Tick_End
	return

;; ==========================================================================
