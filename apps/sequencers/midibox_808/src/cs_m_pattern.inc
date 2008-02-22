; $Id$
;
; MBSEQ Menu "Patterns"
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
CS_M_PATTERN_GP_Callback
	;; if shift button pressed: always clear state of rightmost buttons
	SET_BSR	SEQ_BASE
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	clrf	CS_PATTERN_GP_STATE_R
	
	;; branch depending on button status
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_PATTERN_GP_Callback_Pressed
CS_M_PATTERN_GP_Callback_Depr
	;; exit immediately if leftmost GP buttons
	btfss	MIOS_PARAMETER1, 3
	return

	;; exit if shift button pressed (chain mode)
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	return

	;; clear button flag
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_PATTERN_GP_STATE_R, F

	;; if only one button is pressed anymore: change back to corresponding pattern
	movf	CS_PATTERN_GP_STATE_R, W
	xorlw	0x01
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x02 ^ 0x01
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x04 ^ 0x02
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x08 ^ 0x04
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x10 ^ 0x08
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x20 ^ 0x10
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x40 ^ 0x20
	bz	CS_M_PATTERN_GP_Callback_Depr_C
	xorlw	0x80 ^ 0x40
	bz	CS_M_PATTERN_GP_Callback_Depr_C

	rgoto	CS_M_PATTERN_UpdateGPLEDs


CS_M_PATTERN_GP_Callback_Depr_C
	;; determine button number which corresponds to pattern
	movlw	0x00
	btfsc	CS_PATTERN_GP_STATE_R, 1
	movlw	0x01
	btfsc	CS_PATTERN_GP_STATE_R, 2
	movlw	0x02
	btfsc	CS_PATTERN_GP_STATE_R, 3
	movlw	0x03
	btfsc	CS_PATTERN_GP_STATE_R, 4
	movlw	0x04
	btfsc	CS_PATTERN_GP_STATE_R, 5
	movlw	0x05
	btfsc	CS_PATTERN_GP_STATE_R, 6
	movlw	0x06
	btfsc	CS_PATTERN_GP_STATE_R, 7
	movlw	0x07
	iorlw	0x08
	movwf	MIOS_PARAMETER1

	;; no button pressed anymore: clear all flags
	clrf	CS_PATTERN_GP_STATE_R

	;; change to corresponding pattern
	;; 	rgoto	CS_M_PATTERN_GP_Callback_Pressed


CS_M_PATTERN_GP_Callback_Pressed
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, CS_M_PATTERN_GP_Callback_PrShift
	;; if not in chain mode, and rightmost GP buttons: set appr. state flag
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	btfsc	MIOS_PARAMETER1, 3
	iorwf	CS_PATTERN_GP_STATE_R, F
CS_M_PATTERN_GP_Callback_PrShift

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel

	;; change pattern if one of the right GP buttons has been pressed
	BRA_IFCLR MIOS_PARAMETER1, 3, ACCESS, CS_M_PATTERN_GP_Callback_Cont
CS_M_PATTERN_GP_Callback_PChng

	;; change to a new pattern
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

	movff	TMP1, SEQ_PATTERN
	movff	SEQ_NEXT_PATTERN_BANK0, SEQ_PATTERN_BANK

	;; if shift button pressed: add pattern to chain
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, CS_M_PATTERN_GP_Callback_Chain
CS_M_PATTERN_GP_Callback_P
	;; else clear chain
	call	SEQ_CORE_ChainClr
	;; and change to pattern immediately
	call	SEQ_CORE_ChangePatternSynched
	rgoto	CS_M_PATTERN_GP_Callback_Cont

CS_M_PATTERN_GP_Callback_Chain
	;; add pattern to chain
	call	SEQ_CORE_ChainAdd

#if 0
	;; print message
	TABLE_ADDR CS_M_PATTERN_GP_Callback_ChainM0
	call	MIOS_LCD_PrintMessage
	movff	SEQ_CHAIN_TOP, WREG
	call	MIOS_LCD_PrintBCD3
	movlw	7
	call	SEQ_LCD_PrintSpaces

	TABLE_ADDR CS_M_PATTERN_GP_Callback_ChainM1
	call	MIOS_LCD_PrintMessage
	call	CS_M_PATTERN_PrnP	; (5 characters)
	movlw	4
	call	SEQ_LCD_PrintSpaces
#endif

	;; DIRTY!
	;; ensure that GP LEDs updated immediately
	rcall	CS_M_PATTERN_UpdateGPLEDs
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, SEQ_GP_LED_L
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, SEQ_GP_LED_R

	;; 	rgoto	CS_M_PATTERN_GP_Callback_Cont

CS_M_PATTERN_GP_Callback_Cont
	;; update LEDs
	rcall	CS_M_PATTERN_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

CS_M_PATTERN_GP_Callback_ChainM0
	STRING 10, 0x00 + 0, "** Chain #"
CS_M_PATTERN_GP_Callback_ChainM1
	STRING 11, 0x40 + 0, "** Pattern "


;; --------------------------------------------------------------------------
CS_M_PATTERN_UpdateGPLEDs
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
CS_M_PATTERN_Init
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_PATTERN_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; copy current pattern number into parameter
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_NEXT_PATTERN0, 7, BANKED, CS_M_PATTERN_Init_PMuted
CS_M_PATTERN_Init_PNotMuted
	incf	SEQ_NEXT_PATTERN0, W, BANKED
	rgoto	CS_M_PATTERN_Init_P_Cont
CS_M_PATTERN_Init_PMuted
	movlw	0x00
CS_M_PATTERN_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; update LEDs
	rcall	CS_M_PATTERN_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATTERN_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    PATTERN 1:A1* ________________                                              
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;  A/a  B/b  C/c  D/d  E/e  F/f  G/g  H/h   1    2    3    4    5    6    7    8  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    PTN  1:A1*
	;; 0000000111111111
	;; ________________
	;; 2222222222333333

	;; if fast display update, and counter < 0x10, set it to 0x10
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_PATTERN_Tick_NotFast
CS_M_PATTERN_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_PATTERN_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_PATTERN_Tick_NotCtrlFix
CS_M_PATTERN_Tick_NotFast

	;; print meter segments 16-31?
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_PATTERN_Tick_Meters

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_PATTERN_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_PATTERN_Tick_0
	rgoto	CS_M_PATTERN_Tick_1
	rgoto	CS_M_PATTERN_Tick_2
	rgoto	CS_M_PATTERN_Tick_3
	rgoto	CS_M_PATTERN_Tick_4
	rgoto	CS_M_PATTERN_Tick_5
	rgoto	CS_M_PATTERN_Tick_6
	rgoto	CS_M_PATTERN_Tick_7

CS_M_PATTERN_Tick_0		; re-used from CS_M_MUTE_Init !!!
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	;; extra: print chain info if SHIFT button pressed or chain mode active
	SET_BSR	SEQ_BASE
	movf	SEQ_CHAIN_TOP, W, BANKED
	bnz	CS_M_PATTERN_Tick_0_Chain
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, CS_M_PATTERN_Tick_0_Chain
CS_M_PATTERN_Tick_0_NoChain
	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_0_Chain
	;; if top >= 100, special view
	movlw	100
	cpfslt	SEQ_CHAIN_TOP, BANKED
	bz	CS_M_PATTERN_Tick_0_Chain_33
CS_M_PATTERN_Tick_0_Chain_22
	movf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	skpnz
	movf	SEQ_CHAIN_TOP, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	'/'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_CHAIN_TOP, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_0_Chain_33
	movf	SEQ_CHAIN_PLAY_PTR, W, BANKED
	skpnz
	movf	SEQ_CHAIN_TOP, W, BANKED
	call	MIOS_LCD_PrintBCD3
	;; no slash... :-/
	SET_BSR	SEQ_BASE
	movf	SEQ_CHAIN_TOP, W, BANKED
	call	MIOS_LCD_PrintBCD3
	rgoto	CS_M_PATTERN_Tick_Cont

#if DEFAULT_LCD_WIDTH == 80
CS_M_PATTERN_Tick_1_STR	STRING 8, 0x00 + 6, "PATTERN "
#else
CS_M_PATTERN_Tick_1_STR	STRING 5, 0x00 + 6, "PTN  "
#endif
CS_M_PATTERN_Tick_1
	TABLE_ADDR CS_M_PATTERN_Tick_1_STR
	call	MIOS_LCD_PrintString

	;; pattern name (5 characters)
	rcall	CS_M_PATTERN_PrnP

	;; additional space
	movlw	' '
	call	MIOS_LCD_PrintChar

	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_2
	;; nothing to do... (normaly filled by meters)

	;; time to init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_3
	;; print remaining 4 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 36
#else
	movlw	0x40 + 16
#endif
	call	MIOS_LCD_CursorSet
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_4
CS_M_PATTERN_Tick_5
CS_M_PATTERN_Tick_6
CS_M_PATTERN_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_89ABCDEF_STR
	db	" A/a  B/b  C/c  D/d  E/e  F/f  G/g  H/h   1    2    3    4    5    6    7    8  "
CS_M_PATTERN_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07

	TABLE_ADDR_MUL_W CS_M_PATTERN_Tick_89ABCDEF_STR, 10
	call	CS_M_HLP_Print10Chars
	rgoto	CS_M_PATTERN_Tick_Cont
#endif


CS_M_PATTERN_Tick_Meters
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
	rcall	CS_M_PATTERN_PrnM

	;;  	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_Cont
	;; Tick function will be called 32 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_PATTERN_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_PATTERN_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This help function prints the bank and pattern number
;;  (5 characters)
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnP
	SET_BSR	SEQ_BASE
	incf	SEQ_NEXT_PATTERN_BANK0, W, BANKED
	call	MIOS_LCD_PrintBCD1

	movlw	':'
	call	MIOS_LCD_PrintChar

	;; write converted CS_MENU_PARAMETER_L to TMP2
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP2

	movf	TMP1, W				; item number
	rcall	CS_M_PATTERN_PrnCursor		; prints spaces and sets ZERO flag if cursor flashed
	bz	CS_M_PATTERN_PrnP_Flashed
CS_M_PATTERN_PrnP_NotFlashed
	;; if BankStick not available - print "!!" instead of pattern number
	SET_BSR	SEQ_BASE
	movf	SEQ_NEXT_PATTERN_BANK0, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	CS_M_PATTERN_PrnP_NotFlashedAv
CS_M_PATTERN_PrnP_NotFlashedNA
	movlw	'!'
	call	MIOS_LCD_PrintChar
	movlw	'!'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_PATTERN_PrnP_Flashed
CS_M_PATTERN_PrnP_NotFlashedAv
	movf	TMP2, W
	call	SEQ_LCD_PrnPattern
CS_M_PATTERN_PrnP_Flashed



	;; print "*" if pattern synched to measure
	BRA_IFSET TMP2, 7, ACCESS, CS_M_PATTERN_PrnP_NoS
CS_M_PATTERN_PrnP_S
	SET_BSR	SEQ_BASE
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, W, BANKED
	bz	CS_M_PATTERN_PrnP_NoS
	movlw	'*'
	rgoto	CS_M_PATTERN_PrnP_SPr
CS_M_PATTERN_PrnP_NoS
	movlw	' '
CS_M_PATTERN_PrnP_SPr
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  This help function prints a vertical meter at the lower line
;;  track number 0-15 in WREG
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnM
	movwf	TMP1		; save index n in WREG

	;; print "M" if track is muted, else vertical meter for track status
	SET_BSR	SEQ_BASE
	movf	SEQ_TRKS_MUTED0, W, BANKED
	btfsc	TMP1, 3
	movf	SEQ_TRKS_MUTED1, W, BANKED
	movwf	TMP2
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	TMP2, W
	bz	CS_M_PATTERN_PrnM_NotMuted
CS_M_PATTERN_PrnM_Muted
	movlw	'M'
	goto	MIOS_LCD_PrintChar

CS_M_PATTERN_PrnM_NotMuted
	lfsr	FSR1, CS_MENU_METER_CTR_BEGIN
	movf	TMP1, W
	movf	PLUSW1, W
	goto	SEQ_LCD_PrnVBar


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function prints spaces if the item (specified in WREG) is
;;  selected by cursor, and CS_STAT, CS_STAT_CURSOR_FLASH is cleared
;;  ZERO flag set if cursor flashed
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnCursor
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_PATTERN_PrnCursorAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_PATTERN_PrnCursorR
CS_M_PATTERN_PrnCursorAll
	;; flash both digits
	goto	CS_MENU_CursorHandler2

CS_M_PATTERN_PrnCursorR
	;; flash only right digit
	;; TODO
	goto	CS_MENU_CursorHandler2
