; $Id$
;
; MBSEQ Menu "Morphing"
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
CS_M_MORPH_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x03,	0x40+ 1, 2,	CS_M_MORPH_GrpChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+ 9, 2,	CS_M_MORPH_PtnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+12, 3,	CS_M_MORPH_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+17, 4,	CS_M_MORPH_ModeT1Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+22, 4,	CS_M_MORPH_ModeT2Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+27, 4,	CS_M_MORPH_ModeT3Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+32, 4,	CS_M_MORPH_ModeT4Changed
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0xc0+28, 3,	CS_M_MORPH_CCChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x10,	0xc0+36, 2,	CS_M_MORPH_ChnChanged

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
CS_M_MORPH_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch if pattern is selected and no encoder (item #1)
	movf	CS_MENU_ENC_INCREMENTER, W
	bnz	CS_M_MORPH_GP_Callback_Enc
	decf	CS_MENU_CURSOR_POS, W
	bz	CS_M_MORPH_GP_Callback_Ptn
CS_M_MORPH_GP_Callback_Enc

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MORPH_GP_Callback_1
	rgoto	CS_M_MORPH_GP_Callback_2
	rgoto	CS_M_MORPH_GP_Callback_3
	rgoto	CS_M_MORPH_GP_Callback_4
	rgoto	CS_M_MORPH_GP_Callback_5
	rgoto	CS_M_MORPH_GP_Callback_6
	rgoto	CS_M_MORPH_GP_Callback_7
	rgoto	CS_M_MORPH_GP_Callback_8
	rgoto	CS_M_MORPH_GP_Callback_9
	rgoto	CS_M_MORPH_GP_Callback_10
	rgoto	CS_M_MORPH_GP_Callback_11
	rgoto	CS_M_MORPH_GP_Callback_12
	rgoto	CS_M_MORPH_GP_Callback_13
	rgoto	CS_M_MORPH_GP_Callback_14
	rgoto	CS_M_MORPH_GP_Callback_15
	rgoto	CS_M_MORPH_GP_Callback_16

CS_M_MORPH_GP_Callback_1
	movlw	0
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_2
	movlw	1
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_3
	movlw	2
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_4
	movlw	3
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_5
	movlw	4
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_6
	movlw	5
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_7
	movlw	6
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_8
	rgoto	CS_M_MORPH_GP_Callback_Ignore
CS_M_MORPH_GP_Callback_9
	rgoto	CS_M_MORPH_GP_Callback_Ignore
CS_M_MORPH_GP_Callback_10
	rgoto	CS_M_MORPH_GP_Callback_Ignore
CS_M_MORPH_GP_Callback_11
	rgoto	CS_M_MORPH_GP_Callback_Ignore
CS_M_MORPH_GP_Callback_12
	rgoto	CS_M_MORPH_GP_Callback_Ignore
CS_M_MORPH_GP_Callback_13
CS_M_MORPH_GP_Callback_14
	movlw	7
	rgoto	CS_M_MORPH_GP_Callback_Cont
CS_M_MORPH_GP_Callback_15
CS_M_MORPH_GP_Callback_16
	movlw	8
	;; 	rgoto	CS_M_MORPH_GP_Callback_Cont

CS_M_MORPH_GP_Callback_Cont
	;; update cursor if it has been changed
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_MORPH_GP_Callback_NewCur
	return
CS_M_MORPH_GP_Callback_NewCur
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_ENC_INCREMENTER	; ignore first turn (CS_MENU_PARAMETER_L and max value not up-to-date yet)
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

CS_M_MORPH_GP_Callback_Ignore
	;; (no increments)
	clrf	CS_MENU_ENC_INCREMENTER, W
	return


;; --------------------------------------------------------------------------
CS_M_MORPH_GP_Callback_Ptn
	SET_BSR	SEQ_BASE

	;; left 8 buttons or right 8 buttons pressed?
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_MORPH_GP_Callback_PtnR
CS_M_MORPH_GP_Callback_PtnL
	;; left side selects pattern group A-H/a-h
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

	;; change value (will also update LEDs)
	rgoto	CS_M_MORPH_PtnChanged

CS_M_MORPH_GP_Callback_PtnR
	;; pattern selected with 1-8 buttons --- both sides should not flash
	movlw	0xf8
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	iorwf	CS_MENU_PARAMETER_L, F

	;; change value (will also update LEDs)
	rgoto	CS_M_MORPH_PtnChanged

;; --------------------------------------------------------------------------
CS_M_MORPH_UpdateGPLEDs
	;; always flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; if cursor is on morph pattern: display pattern combination
	;; else lit LED which is below the item
	decf	CS_MENU_CURSOR_POS, W
	bz	CS_M_MORPH_UpdateGPLEDs_Ptn

CS_M_MORPH_UpdateGPLEDs_Normal
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	movf	CS_MENU_CURSOR_POS, W
	addlw	-7
	bz	CS_M_MORPH_UpdateGPLEDs_Normal7
	addlw	-1
	bz	CS_M_MORPH_UpdateGPLEDs_Normal8
CS_M_MORPH_UpdateGPLEDs_Normal06
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	return

CS_M_MORPH_UpdateGPLEDs_Normal7
	movlw	b'00110000'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

CS_M_MORPH_UpdateGPLEDs_Normal8
	movlw	b'11000000'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

CS_M_MORPH_UpdateGPLEDs_Ptn
	;; first 8 leds: CS_MENU_PARAMETER_L[5..3]
	rlf	CS_MENU_PARAMETER_L, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: CS_MENU_PARAMETER_L[2..0]
	movf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MORPH_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install parameter table
	TABLE_ADDR CS_M_MORPH_PARAMETERS	; pointer to table
	movlw	9				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; init menu parameter depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	JUMPTABLE_2BYTES 9 ; items
	rgoto	CS_M_MORPH_Init_0
	rgoto	CS_M_MORPH_Init_1
	rgoto	CS_M_MORPH_Init_2
	rgoto	CS_M_MORPH_Init_3
	rgoto	CS_M_MORPH_Init_4
	rgoto	CS_M_MORPH_Init_5
	rgoto	CS_M_MORPH_Init_6
	rgoto	CS_M_MORPH_Init_7
	rgoto	CS_M_MORPH_Init_8

CS_M_MORPH_Init_0
	movf	SEQ_SELECTED_GROUP, W, BANKED
	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_1
	rcall	CS_M_MORPH_GetMorphPtr	; stored in FSR1
	movf	INDF1, W
	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_2
	movff	SEQ_MORPH_VALUE, WREG
	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_3
CS_M_MORPH_Init_4
CS_M_MORPH_Init_5
CS_M_MORPH_Init_6
	movf	CS_MENU_PARAMETER_L, W	; (morph mode already initialized via CC)
 	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_7
	movff	SEQ_MORPH_CC, WREG
	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_8
	movff	SEQ_MORPH_CHANNEL, WREG
	;; 	rgoto	CS_M_MORPH_Init_Cont
CS_M_MORPH_Init_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_MORPH_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_MORPH_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MORPH_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Grp. Target Val. Trk1 Trk2 Trk3 Trk4      Morphing controlled by CC#  1 Chn. 1
	;; 1111111111111111-2222222222222222222------33333333-444444444444444445555555555--
	;;  G1  A1<>A2 127  Smth off  Smth Smth       <||||||||||||||||||||||||||||||||>
	;; -88--999999-AAA--BBBB-CCCC-DDDD-EEEE-------...........10-17..................---

	;; if fast display update, and counter < 0x18, set it to 0x18
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_MORPH_Tick_NotFast
CS_M_MORPH_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_MORPH_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_MORPH_Tick_NotCtrlFix
CS_M_MORPH_Tick_NotFast

	;; print segments 0-23
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_MORPH_Tick_1x

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MORPH_Tick_00
	rgoto	CS_M_MORPH_Tick_01
	rgoto	CS_M_MORPH_Tick_02
	rgoto	CS_M_MORPH_Tick_03
	rgoto	CS_M_MORPH_Tick_04
	rgoto	CS_M_MORPH_Tick_05
	rgoto	CS_M_MORPH_Tick_06
	rgoto	CS_M_MORPH_Tick_07
	rgoto	CS_M_MORPH_Tick_08
	rgoto	CS_M_MORPH_Tick_09
	rgoto	CS_M_MORPH_Tick_0A
	rgoto	CS_M_MORPH_Tick_0B
	rgoto	CS_M_MORPH_Tick_0C
	rgoto	CS_M_MORPH_Tick_0D
	rgoto	CS_M_MORPH_Tick_0E
	rgoto	CS_M_MORPH_Tick_0F

CS_M_MORPH_Tick_00
	;; init horizontal bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_HBARS
	call	CS_SPECIAL_CHARS_Init
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_01_STR	STRING 16, 0x00 + 0, "Grp. Target Val."
CS_M_MORPH_Tick_01
	TABLE_ADDR CS_M_MORPH_Tick_01_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_02_STR	STRING 19, 0x00 + 17, "Trk1 Trk2 Trk3 Trk4"
CS_M_MORPH_Tick_02
	TABLE_ADDR CS_M_MORPH_Tick_02_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_03_STR	STRING 8, 0x80 + 2, "Morphing"
CS_M_MORPH_Tick_03
	TABLE_ADDR CS_M_MORPH_Tick_03_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_04_STR	STRING 17, 0x80 + 11, "controlled by CC#"
CS_M_MORPH_Tick_04
	TABLE_ADDR CS_M_MORPH_Tick_04_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_05
	movlw	0x80 + 28
	call	MIOS_LCD_CursorSet
	movlw	7		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MORPH_Tick_05_Cont

	SET_BSR	SEQ_BASE
	movf	SEQ_MORPH_CC, W, BANKED
	call	MIOS_LCD_PrintBCD3

CS_M_MORPH_Tick_05_Cont
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'h'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	'.'
	call	MIOS_LCD_PrintChar

	movlw	0x80 + 36
	call	MIOS_LCD_CursorSet
	movlw	8		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_MORPH_Tick_Cont

	SET_BSR	SEQ_BASE
	movf	SEQ_MORPH_CHANNEL, W, BANKED
	call	SEQ_LCD_PrnChn
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_06
CS_M_MORPH_Tick_07
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_08
	movlw	0x40 + 1
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MORPH_Tick_Cont

	movlw	'G'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	addlw	'1'
	call	MIOS_LCD_PrintBCD1	
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_09
	movlw	0x40 + 5
	call	MIOS_LCD_CursorSet

	lfsr	FSR1, SEQ_PATTERN0
	SET_BSR	SEQ_BASE
	movf	SEQ_SELECTED_GROUP, W, BANKED
	movf	PLUSW1, W
	call	SEQ_LCD_PrnPattern

	movlw	'<'
	call	MIOS_LCD_PrintChar
	movlw	'>'
	call	MIOS_LCD_PrintChar

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MORPH_Tick_Cont

	rcall	CS_M_MORPH_GetMorphPtr
	movf	INDF1, W
	call	SEQ_LCD_PrnPattern
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_0A
CS_M_MORPH_Tick_1x_ValUpdate
	movlw	0x40 + 12
	call	MIOS_LCD_CursorSet
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MORPH_Tick_Cont
	movff	SEQ_MORPH_VALUE, WREG
	call	MIOS_LCD_PrintBCD3
	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_0B_STRTAB
	db	"off "
	db	"Smth"
	db	"Scld"

CS_M_MORPH_Tick_0B
CS_M_MORPH_Tick_0C
CS_M_MORPH_Tick_0D
CS_M_MORPH_Tick_0E
	;; determine track number -> TMP1
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-0xb
	movwf	TMP1

	;; set cursor
	mullw	5
	movlw	0x40 + 17
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet

	movf	TMP1, W		; item number
	addlw	3
	call	CS_MENU_CursorHandler2
	bz	CS_M_MORPH_Tick_Cont

	;; print mode of track
	call	CS_M_HLP_GetSelectedGroupOffset
	addwf	TMP1, W
	movwf	MIOS_PARAMETER2
	movlw	SEQ_CC_OFFSET_MORPH_MODE
	call	SEQ_CCOUT_Get

	;; print string of the mode
	mullw	4
	movlw	LOW(CS_M_MORPH_Tick_0B_STRTAB)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_MORPH_Tick_0B_STRTAB)
	addwfc	PRODH, W
	movwf	TBLPTRH
	movlw	4
	call	MIOS_LCD_PrintPreconfString

	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_0F
 	rgoto	CS_M_MORPH_Tick_Cont

CS_M_MORPH_Tick_1x
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	bz	CS_M_MORPH_Tick_10
	mullw	4
	movf	PRODL, W
	addlw	0xc0 + 4
	call	MIOS_LCD_CursorSet
	rgoto	CS_M_MORPH_Tick_1x_Cont
	
CS_M_MORPH_Tick_10
	movlw	0xc0 + 3
	call	MIOS_LCD_CursorSet
	movlw	'<'
	call	MIOS_LCD_PrintChar
	;; 	rgoto	CS_M_MORPH_Tick_1x_Cont

CS_M_MORPH_Tick_1x_Cont
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	rcall	CS_M_MORPH_PrnLBar

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	0x17
	bnz	CS_M_MORPH_Tick_Cont
	movlw	'>'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_MORPH_Tick_1x_ValUpdate
	
CS_M_MORPH_Tick_Cont
	;; Tick function will be called 24 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	0x18-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_MORPH_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_0End

CS_M_MORPH_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Help function for printing the large horizontal bar in 8 segments
;;  IN: segment number (0-7) in WREG
;;  OUT: 4 characters
;; --------------------------------------------------------------------------
CS_M_MORPH_PrnLBar
	;; store segment number in WREG
	movwf	TMP1

	;; each segments prints 4 chars which represent a range of 4*4=16
	swapf	WREG, W
	andlw	0x70
	movwf	TMP2		; compare value in TMP2

	movff	SEQ_MORPH_VALUE, TMP3	; morph value in TMP3

	movlw	4		; loop counter
	movwf	TMP4
CS_M_MORPH_PrnLBar_Loop
	movf	TMP2, W
	cpfsgt	TMP3, ACCESS
	rgoto CS_M_MORPH_PrnLBar_Loop_LEQ
	addlw	4
	cpfslt	TMP3, ACCESS
	rgoto CS_M_MORPH_PrnLBar_Loop_GEQ

	;; within range - print char 0..3
	movf	TMP3, W
	andlw	0x03
	rgoto	CS_M_MORPH_PrnLBar_Loop_Cont

CS_M_MORPH_PrnLBar_Loop_LEQ
	movlw	0x00
	rgoto	CS_M_MORPH_PrnLBar_Loop_Cont
	
CS_M_MORPH_PrnLBar_Loop_GEQ
	movlw	0x03
	;; 	rgoto	CS_M_MORPH_PrnLBar_Loop_Cont

CS_M_MORPH_PrnLBar_Loop_Cont
	call	MIOS_LCD_PrintChar

	movlw	4		; increment compare value by 4
	addwf	TMP2, F
	decfsz	TMP4, F		; 4 times
	rgoto	CS_M_MORPH_PrnLBar_Loop

	return

;; ==========================================================================

	;; help function which returns pointer to morph pattern in FSR1
CS_M_MORPH_GetMorphPtr
	SET_BSR	SEQ_BASE
	lfsr	FSR1, SEQ_TRK1 + SEQ_TRKMORPH_PATTERNx	; stored in second track, first byte
	movf	SEQ_SELECTED_GROUP, W, BANKED
	mullw	4*SEQ_TRKRECORD_LENGTH
	movf	PRODL, W
	addwf	FSR1L, F
	movf	PRODH, W
	addwfc	FSR1H, F
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MORPH_GrpChanged
	SET_BSR	SEQ_BASE
	movff	CS_MENU_PARAMETER_L, SEQ_SELECTED_GROUP
	return

CS_M_MORPH_PtnChanged
	rcall	CS_M_MORPH_GetMorphPtr
	movff	CS_MENU_PARAMETER_L, INDF1
	rgoto	CS_M_MORPH_UpdateGPLEDs

CS_M_MORPH_ValueChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MORPH_VALUE
	return

CS_M_MORPH_ModeT1Changed
	movlw	0
	rgoto	CS_M_MORPH_ModeTxChanged
CS_M_MORPH_ModeT2Changed
	movlw	1
	rgoto	CS_M_MORPH_ModeTxChanged
CS_M_MORPH_ModeT3Changed
	movlw	2
	rgoto	CS_M_MORPH_ModeTxChanged
CS_M_MORPH_ModeT4Changed
	movlw	3
	;; 	rgoto	CS_M_MORPH_ModeTxChanged
CS_M_MORPH_ModeTxChanged
	movwf	MIOS_PARAMETER2
	call	CS_M_HLP_GetSelectedGroupOffset
	addwf	MIOS_PARAMETER2, F
	movff	CS_MENU_PARAMETER_L, MIOS_PARAMETER1
	movlw	SEQ_CC_OFFSET_MORPH_MODE
	goto	SEQ_CCIN_Set

CS_M_MORPH_CCChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MORPH_CC
	return

CS_M_MORPH_ChnChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MORPH_CHANNEL
	return

