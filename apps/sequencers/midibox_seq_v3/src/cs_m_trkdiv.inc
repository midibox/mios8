; $Id$
;
; MBSEQ Menu "Clock Divider"
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
CS_M_TRKDIV_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKDIV_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_CLK_DIVIDER,		0x3f,	0x40+ 7,  2,	CS_M_TRKDIV_DivChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_CLKDIV_FLAGS,		0x01,	0x40+10,  9,	CS_M_TRKDIV_TRPChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_CLKDIV_FLAGS,		0x01,	0x40+27,  3,	CS_M_TRKDIV_STMChanged

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
CS_M_TRKDIV_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on button
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_TRKDIV_GP_Callback_RLower
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKDIV_GP_Callback_1
	rgoto	CS_M_TRKDIV_GP_Callback_2
	rgoto	CS_M_TRKDIV_GP_Callback_3
	rgoto	CS_M_TRKDIV_GP_Callback_4
	rgoto	CS_M_TRKDIV_GP_Callback_5
	rgoto	CS_M_TRKDIV_GP_Callback_6
	rgoto	CS_M_TRKDIV_GP_Callback_7
	rgoto	CS_M_TRKDIV_GP_Callback_8

CS_M_TRKDIV_GP_Callback_1
	clrf	CS_MENU_CURSOR_POS	; select GxTy (Item #0)
	rgoto	CS_M_TRKDIV_GP_Callback_Cont

CS_M_TRKDIV_GP_Callback_2
	movlw	1			; select Divider (Item #1)
	movwf	CS_MENU_CURSOR_POS	
	rgoto	CS_M_TRKDIV_GP_Callback_Cont

CS_M_TRKDIV_GP_Callback_3
CS_M_TRKDIV_GP_Callback_4
	movlw	2			; select and Toggle "Triplets" (Item #2)
	movwf	CS_MENU_CURSOR_POS

	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Get
	xorlw	(1 << 1)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Set

	rgoto	CS_M_TRKDIV_GP_Callback_Cont

CS_M_TRKDIV_GP_Callback_5
CS_M_TRKDIV_GP_Callback_6
CS_M_TRKDIV_GP_Callback_7
	movlw	3			; select and Toggle "Synch to Measure" (Item #3)
	movwf	CS_MENU_CURSOR_POS

	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Get
	xorlw	(1 << 0)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Set

	rgoto	CS_M_TRKDIV_GP_Callback_Cont

CS_M_TRKDIV_GP_Callback_8
	;; ignore
	clrf	CS_MENU_ENC_INCREMENTER
	return

CS_M_TRKDIV_GP_Callback_RLower
	;; radio button function
	clrf	CS_MENU_ENC_INCREMENTER

	;; set cursor pos to #1 (divider)
	movlw	0x01
	movwf	CS_MENU_CURSOR_POS

	;; set mapped divider
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	rcall	CS_M_TRKDIV_QuickSelTableGet
	movwf	TMP1			; store for triplet change

	;; set/clear triplet flag (TMP1.7)
	movlw	0x00
	btfsc	TMP1, 7
	movlw 0x01
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKDIV_TRPChanged

	;; change divider value
	movf	TMP1, W
	andlw	0x3f
	movwf	CS_MENU_PARAMETER_L	; set parameter
	rcall	CS_M_TRKDIV_DivChanged
	;; 	rgoto	CS_M_TRKDIV_GP_Callback_Cont


CS_M_TRKDIV_GP_Callback_Cont

	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_TRKDIV_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1: GxTy (item #0) selected
	movf	CS_MENU_CURSOR_POS, W
	skpnz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0

	;; LED 2: Divider (item #1) selected
	movlw	1
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIV_UpdateGPLEDs_NoDiv
CS_M_TRKDIV_UpdateGPLEDs_Div
	movlw	b'00000010'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKDIV_UpdateGPLEDs_NoDiv

	;; LED 3-4: Triplet (item #2) selected
	movlw	2
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIV_UpdateGPLEDs_NoTrp
CS_M_TRKDIV_UpdateGPLEDs_Trp
	movlw	b'00001100'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKDIV_UpdateGPLEDs_NoTrp

	;; LED 4: ResetAtTrkEnd (item #2) selected
	movlw	3
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIV_UpdateGPLEDs_NoSynch
CS_M_TRKDIV_UpdateGPLEDs_Synch
	movlw	b'01110000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKDIV_UpdateGPLEDs_NoSynch

	;; LED 9-16: mapped length value

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
	BRA_IFSET WREG, 7, ACCESS, CS_M_TRKDIV_UpdateGPLEDs_NoQuick
CS_M_TRKDIV_UpdateGPLEDs_Quick
	call	MIOS_HLP_GetBitORMask	; set appr. LED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
CS_M_TRKDIV_UpdateGPLEDs_NoQuick
	
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKDIV_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; init cs menu parameter if item #1 or #2 selected
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKDIV_Init_Cont
	addlw	-1
	bz	CS_M_TRKDIV_Init_1
	addlw	-1
	bz	CS_M_TRKDIV_Init_2
	rgoto	CS_M_TRKDIV_Init_3

CS_M_TRKDIV_Init_1
	;; normal CC value - already in CS_MENU_PARAMETER_L
	rgoto	CS_M_TRKDIV_Init_Cont

CS_M_TRKDIV_Init_2
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; select bit 1
	call	CS_M_HLP_TrkCC_Get
	clrf	CS_MENU_PARAMETER_L
	btfsc	WREG, 1
	bsf	CS_MENU_PARAMETER_L, 0
 	rgoto	CS_M_TRKDIV_Init_Cont

CS_M_TRKDIV_Init_3
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; select bit 0
	call	CS_M_HLP_TrkCC_Get
	clrf	CS_MENU_PARAMETER_L
	btfsc	WREG, 0
	bsf	CS_MENU_PARAMETER_L, 0
	;; 	rgoto	CS_M_TRKDIV_Init_Cont

CS_M_TRKDIV_Init_Cont

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKDIV_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKDIV_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.  Clock Divider  Synch to Measure          Quick Selection: Timebase
	;; 0000000000000000000--1111111111111111----------2222222222222222-33333333--------
	;; G1T1    4 (normal)         yes           1/4  3/4  1/8 3/32 1/16 3/64 1/32 1/64
	;; 4444---55-666666666--------777----------8888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	CS_M_HLP_LCD_Tick_Resume ; (will branch to next CS_M_TRKDIV_Tick_x)

CS_M_TRKDIV_Tick_0
	call	SEQ_LCD_PrintStringAfterCall
	STRING	19, 0x00 + 0, "Trk.  Clock Divider"
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	16, 0x00 + 21, "Synch to Measure"
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_2
	call	SEQ_LCD_PrintStringAfterCall
	STRING	16, 0x80 + 7, "Quick Selection:"
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_3
	call	SEQ_LCD_PrintStringAfterCall
	STRING	8, 0x80 + 24, "Timebase"
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_4
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_5
	movlw	0x40 + 7
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIV_Tick_5_End

	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

CS_M_TRKDIV_Tick_5_End
	rcall	CS_M_TRKDIV_Tick_Yield
	rgoto	CS_M_TRKDIV_Tick_6

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_6_TAB
	db	"(normal)  "
	db	"(triplet) "
CS_M_TRKDIV_Tick_6
	movlw	0x40 + 10
	call	MIOS_LCD_CursorSet
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIV_Tick_6_End

	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; select bit 1
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 1)
	skpz
	movlw	1
	TABLE_ADDR_MUL_W CS_M_TRKDIV_Tick_6_TAB, 10
	movlw	10
	call	MIOS_LCD_PrintPreconfString

CS_M_TRKDIV_Tick_6_End
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_7
	movlw	0x40 + 27
	call	MIOS_LCD_CursorSet
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIV_Tick_7_End

	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; select bit 0
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 0)
	bnz	CS_M_TRKDIV_Tick_7_1
CS_M_TRKDIV_Tick_7_0
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	'o'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_TRKDIV_Tick_7_End

CS_M_TRKDIV_Tick_7_1
	movlw	'y'
	call	MIOS_LCD_PrintChar
	movlw	'e'
	call	MIOS_LCD_PrintChar
	movlw	's'
	call	MIOS_LCD_PrintChar
	;; 	rgoto	CS_M_TRKDIV_Tick_7_End

CS_M_TRKDIV_Tick_7_End
	rcall	CS_M_TRKDIV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_RLower
	;; get current value -> TMP1
	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; set flag 7 if triplet mode
	call	CS_M_HLP_TrkCC_Get
	btfsc	WREG, 1
	bsf	TMP1, 7

	;; get current item number of the lower line -> TMP2
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	movwf	TMP2

	;; get quick selection value from table -> TMP4
	rcall	CS_M_TRKDIV_QuickSelTableGet
	movwf	TMP4

	;; determine if item is selected (TMP1 == TMP4) (-> TMP3.0)
	setf	TMP3
	cpfseq	TMP1, ACCESS
	clrf TMP3

	;; determine cursor position
	movf	TMP2, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	addlw	0xc0 + 1
	call	MIOS_LCD_CursorSet

	;; check if item should be flashed
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKDIV_Tick_RLower_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_TRKDIV_Tick_RLower_Spaces

CS_M_TRKDIV_Tick_RLower_NoF
	movf	TMP2, W
	rcall	CS_M_TRKDIV_QuickSelStrPrint
	rgoto	CS_M_TRKDIV_Tick_RLower_Cont

CS_M_TRKDIV_Tick_RLower_Spaces
	movlw	4
	call	SEQ_LCD_PrintSpaces
	;; 	rgoto	CS_M_TRKDIV_Tick_RLower_Cont

CS_M_TRKDIV_Tick_RLower_Cont
        ;; ------------------------------------------------------------------
        ;; repeat 8 times
	rcall	CS_M_TRKDIV_Tick_Yield
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKDIV_Tick_End
	rgoto	CS_M_TRKDIV_Tick_RLower

        ;; ------------------------------------------------------------------
CS_M_TRKDIV_Tick_End
	goto	CS_M_HLP_LCD_Tick_End

CS_M_TRKDIV_Tick_Yield
	goto	CS_M_HLP_LCD_Tick_Yield

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKDIV_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKDIV_UpdateGPLEDs

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

CS_M_TRKDIV_STMChanged
	movff	CS_MENU_PARAMETER_L, TMP5	; RMW operation - store menu parameter
	
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS	; modify bit 0
	call	CS_M_HLP_TrkCC_Get
	andlw	~(1 << 0)
	btfsc	CS_MENU_PARAMETER_L, 0
	iorlw	(1 << 0)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_CLKDIV_FLAGS
	call	CS_M_HLP_TrkCC_Set

	movff	TMP5, CS_MENU_PARAMETER_L
	return

;; ==========================================================================

CS_M_TRKDIV_QuickSelTable
	db	63, 31, 15, 7, 7 | 0x80, 3, 3 | 0x80, 1	; ...+1, bit 7 selects triplets

CS_M_TRKDIV_QuickSelStrTable
	db	" 1  "
	db	" 2  "
	db	" 4  "
	db	" 8  "
	db	" 8T "
	db	" 16 "
	db	" 16T"
	db	" 32 "

	;; prints 4 characters
	;; IN: item number (0-7) in WREG
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
	BRA_IFCLR PRODH, 3, ACCESS, CS_M_TRKDIV_QuickSelTableSearchL
	retlw	0xff

;; ==========================================================================

