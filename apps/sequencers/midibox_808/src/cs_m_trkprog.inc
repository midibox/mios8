; $Id: cs_m_trkdir.inc 73 2008-02-03 00:00:52Z tk $
;
; MBSEQ Menu "Step Progression"
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
CS_M_TRKPROG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; left side: forward, right side: jump back value
	;; range: always 0..7
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L

	movlw	SEQ_CC_OFFSET_DIRPAR_FORWARD
	btfsc	MIOS_PARAMETER1, 3
	movlw	SEQ_CC_OFFSET_DIRPAR_JMPBCK
	call	CS_M_HLP_TrkCC_Set

	;; update LEDs
	rcall	CS_M_TRKPROG_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKPROG_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-8: forward value
	movlw	SEQ_CC_OFFSET_DIRPAR_FORWARD
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; LED 9-16: jump back value
	movlw	SEQ_CC_OFFSET_DIRPAR_JMPBCK
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKPROG_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKPROG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_TRKPROG_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKPROG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    PROGRESSION   Fwd: 1  JBck: 0                                                
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;; Fwd1 Fwd2 Fwd3 Fwd4 Fwd5 Fwd6 Fwd7 Fwd8 Bck0 Bck1 Bck2 Bck3 Bck4 Bck5 Bck6 Bck7 
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    PROGRESSION
	;; 0000000111111111
	;; Fwd: 1  JBck: 0
	;; 2222222233333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKPROG_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKPROG_Tick_0
	rgoto	CS_M_TRKPROG_Tick_1
	rgoto	CS_M_TRKPROG_Tick_2
	rgoto	CS_M_TRKPROG_Tick_3
	rgoto	CS_M_TRKPROG_Tick_4
	rgoto	CS_M_TRKPROG_Tick_5
	rgoto	CS_M_TRKPROG_Tick_6
	rgoto	CS_M_TRKPROG_Tick_7

CS_M_TRKPROG_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	SEQ_TRK_PrnName	; (6 characters)

	rgoto	CS_M_TRKPROG_Tick_Cont

CS_M_TRKPROG_Tick_1_STR	STRING 14, 0x00 + 6, "PROGRESSION   "
CS_M_TRKPROG_Tick_1
	TABLE_ADDR CS_M_TRKPROG_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKPROG_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKPROG_Tick_2_STR	STRING 5, 0x00 + 20, "Fwd: "
#else
CS_M_TRKPROG_Tick_2_STR	STRING 5, 0x40 + 0, "Fwd: "
#endif
CS_M_TRKPROG_Tick_2
	;; print "forward" value
	TABLE_ADDR CS_M_TRKPROG_Tick_2_STR
	call	MIOS_LCD_PrintString
	movlw	SEQ_CC_OFFSET_DIRPAR_FORWARD
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	2
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKPROG_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_TRKPROG_Tick_3_STR	STRING 6, 0x00 + 28, "JBck: "
#else
CS_M_TRKPROG_Tick_3_STR	STRING 6, 0x40 + 8, "JBck: "
#endif
CS_M_TRKPROG_Tick_3
	;; print "jump back" value
	TABLE_ADDR CS_M_TRKPROG_Tick_3_STR
	call	MIOS_LCD_PrintString
	movlw	SEQ_CC_OFFSET_DIRPAR_JMPBCK
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD1
	movlw	5
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKPROG_Tick_Cont

CS_M_TRKPROG_Tick_4
CS_M_TRKPROG_Tick_5
CS_M_TRKPROG_Tick_6
CS_M_TRKPROG_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKPROG_Tick_Cont

CS_M_TRKPROG_Tick_89ABCDEF_STR
	db	"Fwd1 Fwd2 Fwd3 Fwd4 Fwd5 Fwd6 Fwd7 Fwd8 Bck0 Bck1 Bck2 Bck3 Bck4 Bck5 Bck6 Bck7 "
CS_M_TRKPROG_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKPROG_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKPROG_Tick_Cont


CS_M_TRKPROG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKPROG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKPROG_Tick_End
	return

;; ==========================================================================
