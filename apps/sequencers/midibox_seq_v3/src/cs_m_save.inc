; $Id$
;
; MBSEQ Menu "Save Pattern"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_SAVE_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x03,	0x40+ 0, 4,	CS_M_SAVE_GroupChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x06,	0x40+25, 1,	CS_M_SAVE_TargetBnkChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+26, 2,	CS_M_SAVE_TargetPtnChanged

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;     o encoder incrementer in CS_MENU_ENC_INCREMENTER if encoder has been used
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;;     should clear CS_MENU_ENC_INCREMENTER if radio-button function desired
;; --------------------------------------------------------------------------
CS_M_SAVE_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; branch depending on cursor position
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SAVE_GP_Callback_Cur0
	addlw	-1
	bz	CS_M_SAVE_GP_Callback_Cur1
	addlw	-1
	bz	CS_M_SAVE_GP_Callback_Cur2
	rgoto	CS_M_SAVE_GP_Callback_Cur3

CS_M_SAVE_GP_Callback_Cur0
CS_M_SAVE_GP_Callback_Cur1
CS_M_SAVE_GP_Callback_Cur2_Enc
CS_M_SAVE_GP_Callback_Cur3
	;; ignore button/encoder 9-16
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SAVE_GP_Callback_Ignore

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SAVE_GP_Callback_1
	rgoto	CS_M_SAVE_GP_Callback_2
	rgoto	CS_M_SAVE_GP_Callback_3
	rgoto	CS_M_SAVE_GP_Callback_4
	rgoto	CS_M_SAVE_GP_Callback_5
	rgoto	CS_M_SAVE_GP_Callback_6
	rgoto	CS_M_SAVE_GP_Callback_7
	rgoto	CS_M_SAVE_GP_Callback_8

CS_M_SAVE_GP_Callback_1
CS_M_SAVE_GP_Callback_2
CS_M_SAVE_GP_Callback_3
CS_M_SAVE_GP_Callback_4
	movlw	0x00
	rgoto	CS_M_SAVE_GP_Callback_Cont

CS_M_SAVE_GP_Callback_5
	movlw	0x01
	rgoto	CS_M_SAVE_GP_Callback_Cont
CS_M_SAVE_GP_Callback_6
	movlw	0x02
	rgoto	CS_M_SAVE_GP_Callback_Cont

CS_M_SAVE_GP_Callback_7
CS_M_SAVE_GP_Callback_8
	rgoto	CS_M_SAVE_GP_Callback_Ignore


CS_M_SAVE_GP_Callback_Cont
	movwf	CS_MENU_CURSOR_POS
	call	CS_MENU_NewCursorPos	; notify new cursor position
	;; (LEDs are updated during re-initialisation)
	return

CS_M_SAVE_GP_Callback_Ignore
	;; update cursor if it has been changed
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SAVE_GP_Callback_NewCur
	return
CS_M_SAVE_GP_Callback_NewCur
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_ENC_INCREMENTER	; ignore first turn (CS_MENU_PARAMETER_L and max value not up-to-date yet)
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

;; --------------------------------------------------------------------------

CS_M_SAVE_GP_Callback_Cur2
	;; if incrementer != 0, skip set function (GP encoder #6 and #7 increments pattern, all others select other items)
	movf	CS_MENU_ENC_INCREMENTER, W
	bnz	CS_M_SAVE_GP_Callback_Cur2_Enc

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SAVE_GP_Callback_Cur2_R
CS_M_SAVE_GP_Callback_Cur2_L
	;; left side selects pattern group A-H/a-h
	;; select pattern group A-H/a-h depending on pressed button --- don't change bank until 1-8 button has been pressed!

	;; mask out first 3 bits (save current pattern number in TMP1)
	movf	CS_MENU_PARAMETER_L, W
	movwf	TMP1
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L

	;; add button number [2..0] * 8 to pattern number
	movf	MIOS_PARAMETER1, W
	mullw	8
	movf	PRODL, W
	andlw	0x38
	iorwf	CS_MENU_PARAMETER_L, F

	;; if new pattern number equal to old button number, toggle 6th bit
	movf	TMP1, W
	xorwf	CS_MENU_PARAMETER_L, W
	skpnz
	btg	CS_MENU_PARAMETER_L, 6

	rgoto	CS_M_SAVE_GP_Callback_Cur2_Cont

CS_M_SAVE_GP_Callback_Cur2_R
	;; pattern selected with 1-8 buttons --- both sides should not flash
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

	;; 	rgoto	CS_M_SAVE_GP_Callback_Cur2_Cont

CS_M_SAVE_GP_Callback_Cur2_Cont
	;; change value (will also update LEDs)
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_PATTERN
	rgoto	CS_M_SAVE_TargetPtnChanged

;; --------------------------------------------------------------------------

CS_M_SAVE_UpdateGPLEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; branch depending on cursor position
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SAVE_UpdateGPLEDs_0
	addlw	-1
	bz	CS_M_SAVE_UpdateGPLEDs_1
	addlw	-1
	bz	CS_M_SAVE_UpdateGPLEDs_2
	rgoto	CS_M_SAVE_UpdateGPLEDs_3

CS_M_SAVE_UpdateGPLEDs_0
	;; item #0 selected: LED 1-4 active
	movlw	b'00001111'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	rgoto	CS_M_SAVE_UpdateGPLEDs_End

CS_M_SAVE_UpdateGPLEDs_1
	;; item #1 selected: LED 5 active
	movlw	b'00010000'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	rgoto	CS_M_SAVE_UpdateGPLEDs_End

CS_M_SAVE_UpdateGPLEDs_2
	;; item #2 selected: print pattern selection on LED 1-16
	;; first 8 leds: CS_MENU_PARAMETER_L[5..3]
	rlf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: CS_MENU_PARAMETER_L[2..0]
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	rgoto	CS_M_SAVE_UpdateGPLEDs_End

CS_M_SAVE_UpdateGPLEDs_3
	;; item #3 selected: LED 7 and 8 active
	movlw	b'11000000'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	;; 	rgoto	CS_M_SAVE_UpdateGPLEDs_End

CS_M_SAVE_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SAVE_Select
	;; increment cursor
	call	CS_MENU_IncCursorPos

	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_SAVE_Select_Depressed

CS_M_SAVE_Select_Pressed
	;; install delayed callback hook, used to trigger the save function after 2 seconds
	TABLE_ADDR CS_M_SAVE_Select_Copy
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; print message
	TABLE_ADDR CS_M_SAVE_HOLD_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_SAVE_HOLD_STR_0	STRING 16, 0x00, " Hold 2 seconds "
CS_M_SAVE_HOLD_STR_1	STRING 16, 0x40, "to save pattern "

CS_M_SAVE_Select_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop


	;; callback function
CS_M_SAVE_Select_Copy
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movwf	SEQ_GROUP, BANKED
	call	SEQ_DUMP_StoreAll

	;; disabled - only required for debugging!
#if 0
	;; re-load new pattern immediately
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	SEQ_GROUP, W, BANKED
	movff	SEQ_SAVE_PATTERN_BANK, PLUSW1
	lfsr	FSR1, SEQ_PATTERN0
	movf	SEQ_GROUP, W, BANKED
	movff	SEQ_SAVE_PATTERN, PLUSW1
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movff	SEQ_SAVE_PATTERN, PLUSW1
	bsf	PLUSW1, 7	; (ensure that pattern won't be loaded again by sequencer)
	call	SEQ_DUMP_RestoreTrk
	call	SEQ_DUMP_RestorePots
#else
	;; change to new pattern
	movf	SEQ_GROUP, W, BANKED

	lfsr	FSR1, SEQ_PATTERN_BANK0
	movff	SEQ_SAVE_PATTERN_BANK, PLUSW1
	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movff	SEQ_SAVE_PATTERN_BANK, PLUSW1
	
	lfsr	FSR1, SEQ_PATTERN0
	movff	SEQ_SAVE_PATTERN, PLUSW1
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movff	SEQ_SAVE_PATTERN, PLUSW1
#endif

	;; print message	
	TABLE_ADDR CS_M_SAVE_FINISHED_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
CS_M_SAVE_FINISHED_STR_0 STRING 16, 0x00, "*** Pattern  ***"
CS_M_SAVE_FINISHED_STR_1 STRING 16, 0x40, "***  saved   ***"

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SAVE_Init
	;; install parameter table
	TABLE_ADDR CS_M_SAVE_PARAMETERS		; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; copy selected value into CS menu parameter
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SAVE_Init_0
	addlw	-1
	bz	CS_M_SAVE_Init_1
	rgoto	CS_M_SAVE_Init_2
CS_M_SAVE_Init_0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	lfsr	FSR1, SEQ_PATTERN0	; select the right save pattern whenever item #1 is selected
	movff	PLUSW1, SEQ_SAVE_PATTERN
	bcf	PLUSW1, 7
	lfsr	FSR1, SEQ_PATTERN_BANK0; select the right save bank as well
	movff	PLUSW1, SEQ_SAVE_PATTERN_BANK
	rgoto	CS_M_SAVE_Init_Cont
CS_M_SAVE_Init_1
	movf	SEQ_SAVE_PATTERN_BANK, W, BANKED
	rgoto	CS_M_SAVE_Init_Cont
CS_M_SAVE_Init_2
	movf	SEQ_SAVE_PATTERN, W, BANKED
	;; 	rgoto	CS_M_SAVE_Init_Cont
CS_M_SAVE_Init_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_SAVE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_SAVE_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SAVE_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Grp. Save Pattern    to Target              Hold Select Button for 2 seconds
	;; 00000000000000000----111111111--------------44444444444-5555555555-666666666----
	;; G1 (Track 11-11)/1:A1 -> 1:A1                       to save pattern              
	;; 88-99999999999999AAAAAA--BBBB-----------------------DDDDDDDDDDDDDDD-------------

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SAVE_Tick_0
	rgoto	CS_M_SAVE_Tick_1
	rgoto	CS_M_SAVE_Tick_2
	rgoto	CS_M_SAVE_Tick_3
	rgoto	CS_M_SAVE_Tick_4
	rgoto	CS_M_SAVE_Tick_5
	rgoto	CS_M_SAVE_Tick_6
	rgoto	CS_M_SAVE_Tick_7
	rgoto	CS_M_SAVE_Tick_8
	rgoto	CS_M_SAVE_Tick_9
	rgoto	CS_M_SAVE_Tick_A
	rgoto	CS_M_SAVE_Tick_B
	rgoto	CS_M_SAVE_Tick_C
	rgoto	CS_M_SAVE_Tick_D
	rgoto	CS_M_SAVE_Tick_E
	rgoto	CS_M_SAVE_Tick_F

CS_M_SAVE_Tick_0_STR	STRING 17, 0x00 + 0, "Grp. Save Pattern"
CS_M_SAVE_Tick_0
	TABLE_ADDR CS_M_SAVE_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_1_STR	STRING 9, 0x00 + 21, "to Target"
CS_M_SAVE_Tick_1
	TABLE_ADDR CS_M_SAVE_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_2
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_3
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_4_STR	STRING 11, 0x80 + 4, "Hold Select"
CS_M_SAVE_Tick_4
	TABLE_ADDR CS_M_SAVE_Tick_4_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_5_STR	STRING 10, 0x80 + 16, "Button for"
CS_M_SAVE_Tick_5
	TABLE_ADDR CS_M_SAVE_Tick_5_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_6_STR	STRING 9, 0x80 + 27, "2 seconds"
CS_M_SAVE_Tick_6
	TABLE_ADDR CS_M_SAVE_Tick_6_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_7
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_8
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SAVE_Tick_Cont

	movlw	'G'
	call	MIOS_LCD_PrintChar
	call	CS_M_HLP_PrnG
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_9_TAB	db	"(Track  1- 4)/"
			db	"(Track  5- 8)/"
			db	"(Track  9-12)/"
			db	"(Track 13-16)/"
CS_M_SAVE_Tick_9
	movlw	0x40 + 3
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	mullw	14
	movlw	LOW(CS_M_SAVE_Tick_9_TAB)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_SAVE_Tick_9_TAB)
	addwfc	PRODH, W
	movwf	TBLPTRH
	movlw	14
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_A
	movlw	0x40 + 22
	call	MIOS_LCD_CursorSet
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'>'
	call	MIOS_LCD_PrintChar

	movlw	0x40 + 17
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SAVE_Tick_Cont

	lfsr	FSR1, SEQ_PATTERN_BANK0
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	incf	PLUSW1, W
	call	MIOS_LCD_PrintBCD1

	movlw	':'
	call	MIOS_LCD_PrintChar

	lfsr	FSR1, SEQ_PATTERN0
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	call	SEQ_LCD_PrnPattern

	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_B
	movlw	0x40 + 25
	call	MIOS_LCD_CursorSet

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SAVE_Tick_B_Cont
	SET_BSR	SEQ_BASE
	incf	SEQ_SAVE_PATTERN_BANK, W, BANKED
	call	MIOS_LCD_PrintBCD1
CS_M_SAVE_Tick_B_Cont
	movlw	0x40 + 26
	call	MIOS_LCD_CursorSet
	movlw	':'
	call	MIOS_LCD_PrintChar
	
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_SAVE_Tick_Cont
	SET_BSR	SEQ_BASE
	movf	SEQ_SAVE_PATTERN, W, BANKED
	call	SEQ_LCD_PrnPattern
	rgoto	CS_M_SAVE_Tick_Cont
	
CS_M_SAVE_Tick_C
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_D_STR	STRING 15, 0xc0 + 12, "to save pattern"
CS_M_SAVE_Tick_D
	TABLE_ADDR CS_M_SAVE_Tick_D_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_E
CS_M_SAVE_Tick_F
	;; 	rgoto	CS_M_SAVE_Tick_Cont

CS_M_SAVE_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SAVE_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_SAVE_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SAVE_GroupChanged
	SET_BSR	SEQ_BASE
	movff	CS_MENU_PARAMETER_L, SEQ_SELECTED_GROUP

	lfsr	FSR1, SEQ_PATTERN_BANK0
	movf	CS_MENU_PARAMETER_L, W
	movff	PLUSW1, SEQ_SAVE_PATTERN_BANK
	lfsr	FSR1, SEQ_PATTERN0
	movf	CS_MENU_PARAMETER_L, W
	movff	PLUSW1, SEQ_SAVE_PATTERN
	rgoto	CS_M_SAVE_UpdateGPLEDs

CS_M_SAVE_TargetBnkChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_PATTERN_BANK
	rgoto	CS_M_SAVE_UpdateGPLEDs

CS_M_SAVE_TargetPtnChanged
	movff	CS_MENU_PARAMETER_L, SEQ_SAVE_PATTERN
	rgoto	CS_M_SAVE_UpdateGPLEDs
