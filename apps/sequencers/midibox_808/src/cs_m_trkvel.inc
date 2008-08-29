; $Id$
;
; MBSEQ Menu for editing the MIDI Velocity Values (Accented/Unaccented)
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
CS_M_TRKVEL_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; map button number to velocity (button * 8 + 7)
	movf	MIOS_PARAMETER1, W
	mullw	8
	movf	PRODL, W
	addlw	7
	movwf	CS_MENU_PARAMETER_L

	;; set VEL/VELA value depending on shift button
	SET_BSR	SEQ_BASE
	movlw	SEQ_CC_OFFSET_MIDI_VEL
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	movlw	SEQ_CC_OFFSET_MIDI_VELA
	call	CS_M_HLP_TrkCC_Set

	;; update LEDs
	rcall	CS_M_TRKVEL_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKVEL_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-16: velocity >> 3
	SET_BSR	SEQ_BASE
	movlw	SEQ_CC_OFFSET_MIDI_VEL
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	movlw	SEQ_CC_OFFSET_MIDI_VELA
	call	CS_M_HLP_TrkCC_Get
	rrf	WREG, W
	rrf	WREG, W
	rrf	WREG, W
	movwf	PRODL
	call	MIOS_HLP_GetBitORMask
	btfss	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKVEL_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKVEL_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_TRKVEL_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKVEL_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    VELOCITY      Vel:127   VelA:127                                            
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   7   15   23   31   39   47   55   63   71   79   87   95   103  111  119  127 
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    VELOCITY  
	;; 0000000111111111
	;; Vel:127 VelA:127
	;; 2222222222333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKVEL_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKVEL_Tick_0
	rgoto	CS_M_TRKVEL_Tick_1
	rgoto	CS_M_TRKVEL_Tick_2
	rgoto	CS_M_TRKVEL_Tick_3
	rgoto	CS_M_TRKVEL_Tick_4
	rgoto	CS_M_TRKVEL_Tick_5
	rgoto	CS_M_TRKVEL_Tick_6
	rgoto	CS_M_TRKVEL_Tick_7

CS_M_TRKVEL_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKVEL_Tick_Cont

CS_M_TRKVEL_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x00 + 6, "VELOCITY      "
	rgoto	CS_M_TRKVEL_Tick_Cont

CS_M_TRKVEL_Tick_2
	;; print unaccented velocity
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	4, 0x00 + 20, "Vel:"
#else
	STRING	4, 0x40 + 0, "Vel:"
#endif
	movlw	SEQ_CC_OFFSET_MIDI_VEL
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD3
#if DEFAULT_LCD_WIDTH == 80
	movlw	3
#else
	movlw	1
#endif
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKVEL_Tick_Cont

CS_M_TRKVEL_Tick_3
	;; print accented velocity
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	5, 0x00 + 30, "VelA:"
#else
	STRING	5, 0x40 + 8, "VelA:"
#endif
	movlw	SEQ_CC_OFFSET_MIDI_VELA
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD3

	;; print 2 spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKVEL_Tick_Cont

CS_M_TRKVEL_Tick_4
CS_M_TRKVEL_Tick_5
CS_M_TRKVEL_Tick_6
CS_M_TRKVEL_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKVEL_Tick_Cont

CS_M_TRKVEL_Tick_89ABCDEF_STR
	db	"  7   15   23   31   39   47   55   63   71   79   87   95   103  111  119  127 "
CS_M_TRKVEL_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKVEL_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKVEL_Tick_Cont


CS_M_TRKVEL_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKVEL_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKVEL_Tick_End
	return

;; ==========================================================================
