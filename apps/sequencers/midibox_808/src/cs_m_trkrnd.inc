; $Id$
;
; MBSEQ Menu "Random Generator"
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
CS_M_TRKRND_GP_Callback
	;; all LEDs off when GP button has been depressed
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_TRKRND_GP_Callback_On
CS_M_TRKRND_GP_Callback_Off
	setf	CS_MENU_CURSOR_POS	; (invalidate)
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_On
	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; randomize all selected tracks
	;; button number controls the "intensity"
	SET_BSR	SEQ_BASE
	clrf	SEQ_EVNTT, BANKED
CS_M_TRKRND_GP_Callback_On_Loop
	;; skip if track not selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	CS_M_TRKRND_GP_Callback_On_LoopN

	incf	CS_MENU_CURSOR_POS, W	; should be at least 1
	call	CS_M_TRKRND_Gen_T
CS_M_TRKRND_GP_Callback_On_LoopN
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_TRKRND_GP_Callback_On_Loop

	;; display triggers instead of random indicator LED so long button pressed!
	goto	CS_M_EDIT_UpdateGPLEDs

CS_M_TRKRND_GP_Callback_Cont
	;; update LEDs
	rcall	CS_M_TRKRND_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKRND_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; *don't* flash LEDs
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; don't lit LED if cursor pos == 0xff (no option triggered)
	incf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKRND_UpdateGPLEDs_End
	
	;; set LED 1-16 depending on last selected option
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	btfss	CS_MENU_CURSOR_POS, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	CS_MENU_CURSOR_POS, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

CS_M_TRKRND_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKRND_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKRND_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; set cursor pos to invalid value (0xff) -> no LED will lit
	setf	CS_MENU_CURSOR_POS

	;; update LEDs
	rgoto	CS_M_TRKRND_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    RANDOM        Randomize # 1                                                  
	;; 00000001111111111111222222222233333333334444444444555555555566666666667777777777
	;;   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    RANDOM    
	;; 0000000111111111
	;; Randomize # 1   
	;; 2222222222333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKRND_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKRND_Tick_0
	rgoto	CS_M_TRKRND_Tick_1
	rgoto	CS_M_TRKRND_Tick_2
	rgoto	CS_M_TRKRND_Tick_3
	rgoto	CS_M_TRKRND_Tick_4
	rgoto	CS_M_TRKRND_Tick_5
	rgoto	CS_M_TRKRND_Tick_6
	rgoto	CS_M_TRKRND_Tick_7

CS_M_TRKRND_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKRND_Tick_Cont

CS_M_TRKRND_Tick_1_STR	STRING 14, 0x00 + 6, "RANDOM        "
CS_M_TRKRND_Tick_1
	TABLE_ADDR CS_M_TRKRND_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKRND_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKRND_Tick_2_STR	STRING 10, 0x00 + 20, "Randomize "
#else
CS_M_TRKRND_Tick_2_STR	STRING 10, 0x40 + 0, "Randomize "
#endif
CS_M_TRKRND_Tick_2
	;; only print so long cursor pos valid (== button pressed)
	incf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKRND_Tick_2_NoBtn
CS_M_TRKRND_Tick_2_Btn
	TABLE_ADDR CS_M_TRKRND_Tick_2_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKRND_Tick_Cont

CS_M_TRKRND_Tick_2_NoBtn
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
	rgoto	CS_M_TRKRND_Tick_Cont

CS_M_TRKRND_Tick_3
	;; only print so long cursor pos valid (== button pressed)
	incf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKRND_Tick_3_NoBtn
CS_M_TRKRND_Tick_3_Btn
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 30
#else
	movlw	0x40 + 10
#endif
	call	MIOS_LCD_CursorSet

	;; print button number
	movlw	'#'
	call	MIOS_LCD_PrintChar
	incf	CS_MENU_CURSOR_POS, W
	call	MIOS_LCD_PrintBCD2
	;; and fill with spaces
	movlw	7
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKRND_Tick_Cont

CS_M_TRKRND_Tick_3_NoBtn
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKRND_Tick_Cont


CS_M_TRKRND_Tick_4
CS_M_TRKRND_Tick_5
CS_M_TRKRND_Tick_6
CS_M_TRKRND_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKRND_Tick_Cont

CS_M_TRKRND_Tick_89ABCDEF_STR
	db	"  1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16  "
CS_M_TRKRND_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKRND_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKRND_Tick_Cont


CS_M_TRKRND_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKRND_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKRND_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;; randomisation of selected trigger layer (Gate or Aux)
;; Expecting "intensity" in WREG, and track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_TRKRND_Gen_T
	movwf	TMP1		; store intensity in TMP1

	;; don't touch if intensity is 0
	movf	TMP1, W
	bz	CS_M_TRKRND_Gen_T_End

	SET_BSR	SEQ_BASE
	
	clrf	SEQ_EVNTS, BANKED
CS_M_TRKRND_Gen_T_Loop
	movlw	0x01		; set by default
	movwf	MIOS_PARAMETER1
	;; always set if intensity == 15
	movf	TMP1, W
	xorlw	0x0f
	bz	CS_M_TRKRND_Gen_T_Loop_Set
CS_M_TRKRND_Gen_T_Loop_Chk	
	call	SEQ_CORE_GenRandomNumber
	movf	SEQ_RANDOM_SEED_L, W, BANKED
	xorwf	SEQ_RANDOM_SEED_H, W, BANKED
	andlw	0x0f
	cpfslt	TMP1, ACCESS
	rgoto	CS_M_TRKRND_Gen_T_Loop_Set
	clrf	MIOS_PARAMETER1	; clear trigger
CS_M_TRKRND_Gen_T_Loop_Set
	;; gate or aux?
	call	CS_M_HLP_GetSelectedTrgLayer
	bnz	CS_M_TRKRND_Gen_T_Loop_Set_Aux
CS_M_TRKRND_Gen_T_Loop_Set_Gate
	movf	MIOS_PARAMETER1, W
	call	SEQ_TRG_GateSet
	rgoto	CS_M_TRKRND_Gen_T_Loop_Next
CS_M_TRKRND_Gen_T_Loop_Set_Aux
	movf	MIOS_PARAMETER1, W
	call	SEQ_TRG_AuxSet
	;; 	rgoto	CS_M_TRKRND_Gen_T_Loop_Next

CS_M_TRKRND_Gen_T_Loop_Next
	incf	SEQ_EVNTS, F, BANKED
	movlw	64 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto	CS_M_TRKRND_Gen_T_Loop

CS_M_TRKRND_Gen_T_End
	return
