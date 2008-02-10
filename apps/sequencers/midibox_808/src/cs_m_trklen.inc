; $Id$
;
; MBSEQ Menu "Track Length"
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
CS_M_TRKLEN_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKLEN_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_LENGTH,			0x1f,	0x40+ 8,  5,	CS_M_TRKLEN_LenChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_LOOP,			0x1f,	0x40+16,  2,	CS_M_TRKLEN_LoopChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x02,	0x40+22,  4,	CS_M_TRKLEN_ChainChanged

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
CS_M_TRKLEN_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on button
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_TRKLEN_GP_Callback_RLower
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKLEN_GP_Callback_1
	rgoto	CS_M_TRKLEN_GP_Callback_2
	rgoto	CS_M_TRKLEN_GP_Callback_3
	rgoto	CS_M_TRKLEN_GP_Callback_4
	rgoto	CS_M_TRKLEN_GP_Callback_5
	rgoto	CS_M_TRKLEN_GP_Callback_6
	rgoto	CS_M_TRKLEN_GP_Callback_7
	rgoto	CS_M_TRKLEN_GP_Callback_8

CS_M_TRKLEN_GP_Callback_1
	clrf	CS_MENU_CURSOR_POS	; select GxTy (Item #0)
	rgoto	CS_M_TRKLEN_GP_Callback_Cont

CS_M_TRKLEN_GP_Callback_2
CS_M_TRKLEN_GP_Callback_3
	movlw	1			; select Length (Item #1)
	movwf	CS_MENU_CURSOR_POS	
	rgoto	CS_M_TRKLEN_GP_Callback_Cont

CS_M_TRKLEN_GP_Callback_4
	movlw	2			; select Loop (Item #2)
	movwf	CS_MENU_CURSOR_POS	
	rgoto	CS_M_TRKLEN_GP_Callback_Cont

CS_M_TRKLEN_GP_Callback_5
	movlw	3			; select Chain (Item #3)
	movwf	CS_MENU_CURSOR_POS	
	rgoto	CS_M_TRKLEN_GP_Callback_Cont

CS_M_TRKLEN_GP_Callback_6
CS_M_TRKLEN_GP_Callback_7
CS_M_TRKLEN_GP_Callback_8
	;; ignore
	clrf	CS_MENU_ENC_INCREMENTER
	return

CS_M_TRKLEN_GP_Callback_RLower
	;; radio button function
	clrf	CS_MENU_ENC_INCREMENTER

	;; set cursor pos to #1 (length)
	movlw	0x01
	movwf	CS_MENU_CURSOR_POS

	;; set mapped length
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	rcall	CS_M_TRKLEN_QuickSelTableGet
	movwf	CS_MENU_PARAMETER_L	; set parameter
	rcall	CS_M_TRKLEN_LenChanged
	;; 	rgoto	CS_M_TRKLEN_GP_Callback_Cont


CS_M_TRKLEN_GP_Callback_Cont

	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_TRKLEN_UpdateGPLEDs
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

	;; LED 2-3: Length (item #1) selected
	movlw	1
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKLEN_UpdateGPLEDs_NoLen
CS_M_TRKLEN_UpdateGPLEDs_Len
	movlw	b'00000110'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKLEN_UpdateGPLEDs_NoLen

	;; LED 4: Loop (item #2) selected
	movlw	2
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKLEN_UpdateGPLEDs_NoLoop
CS_M_TRKLEN_UpdateGPLEDs_Loop
	movlw	b'00001000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKLEN_UpdateGPLEDs_NoLoop

	;; LED 5: Chain (item #3) selected
	movlw	3
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKLEN_UpdateGPLEDs_NoChain
CS_M_TRKLEN_UpdateGPLEDs_Chain
	movlw	b'00010000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKLEN_UpdateGPLEDs_NoChain

	;; LED 9-16: mapped length value

	;; get current value -> TMP1
	movlw	SEQ_CC_OFFSET_LENGTH
	call	CS_M_HLP_TrkCC_Get
	andlw	0x1f

	;; search position in table (0xff if not found)
	rcall	CS_M_TRKLEN_QuickSelTableSearch
	BRA_IFSET WREG, 7, ACCESS, CS_M_TRKLEN_UpdateGPLEDs_NoQuick
CS_M_TRKLEN_UpdateGPLEDs_Quick
	call	MIOS_HLP_GetBitORMask	; set appr. LED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
CS_M_TRKLEN_UpdateGPLEDs_NoQuick
	
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKLEN_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_TRKLEN_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; special initialisation for chain parameter (item #3)
	movf	CS_MENU_CURSOR_POS, W
	xorlw	3
	bnz	CS_M_TRKLEN_Init_NoChain
CS_M_TRKLEN_Init_Chain
	rcall	CS_M_TRKLEN_GetChainValue
	movwf	CS_MENU_PARAMETER_L, W
CS_M_TRKLEN_Init_NoChain

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKLEN_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKLEN_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKLEN_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.   Length  Loop  Chain                      Quick Selection: Length
	;; 0000---111111--22222222222----------------------3333333333333333-444444---------
	;; G1T1    16/32    1    1->2                 2    4    6    8   12   16   24   32
	;; 5555----66666--77777777777----------------88---99---AA---BB---CC---DD---EE---FF-

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKLEN_Tick_RLower

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKLEN_Tick_0
	rgoto	CS_M_TRKLEN_Tick_1
	rgoto	CS_M_TRKLEN_Tick_2
	rgoto	CS_M_TRKLEN_Tick_3
	rgoto	CS_M_TRKLEN_Tick_4
	rgoto	CS_M_TRKLEN_Tick_5
	rgoto	CS_M_TRKLEN_Tick_6
	rgoto	CS_M_TRKLEN_Tick_7

CS_M_TRKLEN_Tick_0_STR	STRING 4, 0x00 + 0, "Trk."
CS_M_TRKLEN_Tick_0
	TABLE_ADDR CS_M_TRKLEN_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_1_STR	STRING 6, 0x00 + 7, "Length"
CS_M_TRKLEN_Tick_1
	TABLE_ADDR CS_M_TRKLEN_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_2_STR	STRING 11, 0x00 + 15, "Loop  Chain"
CS_M_TRKLEN_Tick_2
	TABLE_ADDR CS_M_TRKLEN_Tick_2_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_3_STR	STRING 16, 0x80 + 8, "Quick Selection:"
CS_M_TRKLEN_Tick_3
	TABLE_ADDR CS_M_TRKLEN_Tick_3_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_4_STR	STRING 6, 0x80 + 25, "Length"
CS_M_TRKLEN_Tick_4
	TABLE_ADDR CS_M_TRKLEN_Tick_4_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_5
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_6
	movlw	0x40 + 8
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKLEN_Tick_Cont
	movlw	SEQ_CC_OFFSET_LENGTH
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	'/'
	call	MIOS_LCD_PrintChar
	movlw	'3'
	call	MIOS_LCD_PrintChar
	movlw	'2'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_7
	movlw	0x40 + 16
	call	MIOS_LCD_CursorSet

CS_M_TRKLEN_Tick_7_Loop
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKLEN_Tick_7_LoopCont
	
	movlw	SEQ_CC_OFFSET_LOOP
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

CS_M_TRKLEN_Tick_7_LoopCont
	movlw	4
	call	SEQ_LCD_PrintSpaces

CS_M_TRKLEN_Tick_7_Chain

	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKLEN_Tick_Cont

	rcall	CS_M_TRKLEN_GetChainValue
	movwf	TMP1		; temporary store this in TMP1

	call	CS_M_HLP_GetSelectedTrk	; track offset (0-3) stored in TMP2
	andlw	0x03
	movwf	TMP2

	BRA_IFSET TMP1, 0, ACCESS, CS_M_TRKLEN_Tick_7_2
	BRA_IFSET TMP1, 1, ACCESS, CS_M_TRKLEN_Tick_7_4
CS_M_TRKLEN_Tick_7_0
	movlw	'-'
	movwf	PRODL
	movlw	4
	call	SEQ_LCD_PrintChars
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_7_2
	movlw	'1'
	btfsc	TMP2, 1
	movlw '3'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'>'
	call	MIOS_LCD_PrintChar
	movlw	'2'
	btfsc	TMP2, 1
	movlw '4'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_7_4
	movlw	'1'
	call	MIOS_LCD_PrintChar
	movlw	'2'
	call	MIOS_LCD_PrintChar
	movlw	'3'
	call	MIOS_LCD_PrintChar
	movlw	'4'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_TRKLEN_Tick_Cont

CS_M_TRKLEN_Tick_RLower
	;; get current value -> TMP1
	movlw	SEQ_CC_OFFSET_LENGTH
	call	CS_M_HLP_TrkCC_Get
	andlw	0x1f
	movwf	TMP1

	;; get current item number of the lower line -> TMP2
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	movwf	TMP2

	;; get quick selection value from table -> TMP4
	rcall	CS_M_TRKLEN_QuickSelTableGet
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
	addlw	0xc0
	call	MIOS_LCD_CursorSet

	;; print left marker
	movlw	'>'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; check if item should be flashed
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKLEN_Tick_RLower_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_TRKLEN_Tick_RLower_Spaces

CS_M_TRKLEN_Tick_RLower_NoF
	incf	TMP4, W
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_TRKLEN_Tick_RLower_Cont

CS_M_TRKLEN_Tick_RLower_Spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces
	;; 	rgoto	CS_M_TRKLEN_Tick_RLower_Cont

CS_M_TRKLEN_Tick_RLower_Cont
	;; print right marker
	movlw	'<'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; 	rgoto	CS_M_TRKLEN_Tick_Cont


CS_M_TRKLEN_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKLEN_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKLEN_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKLEN_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKLEN_UpdateGPLEDs

CS_M_TRKLEN_LenChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_LENGTH
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKLEN_LoopChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_LOOP
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKLEN_ChainChanged
	;; special transfer (register only exists once per pattern)
	SET_BSR	SEQ_BASE
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; pointer to TRKCHAINx -> FSR1
	call	SEQ_FSR_TrkChainx

	BRA_IFSET SEQ_EVNTT, 1, BANKED, CS_M_TRKLEN_ChainChanged_T23
CS_M_TRKLEN_ChainChanged_T01
	bcf	INDF1, SEQ_TRKCHAIN_12
	bcf	INDF1, SEQ_TRKCHAIN_1234
	
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF1, SEQ_TRKCHAIN_12
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	INDF1, SEQ_TRKCHAIN_1234
	rgoto	CS_M_TRKLEN_ChainChanged_Cont

CS_M_TRKLEN_ChainChanged_T23
	bcf	INDF1, SEQ_TRKCHAIN_34
	bcf	INDF1, SEQ_TRKCHAIN_1234
	
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	INDF1, SEQ_TRKCHAIN_34
	btfsc	CS_MENU_PARAMETER_L, 1
	bsf	INDF1, SEQ_TRKCHAIN_1234
	;; 	rgoto	CS_M_TRKLEN_ChainChanged_Cont

CS_M_TRKLEN_ChainChanged_Cont
#if 0
	;; re-init handshake flags
	goto	SEQ_CORE_ResetHndShk
#else
	;; (full reset, so that counters are reinitialized properly!)
	goto	SEQ_CORE_Reset
#endif

;; ==========================================================================

	;; returns chain settings depending on selected track
CS_M_TRKLEN_GetChainValue
	SET_BSR	SEQ_BASE
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; pointer to TRKCHAINx -> FSR1
	call	SEQ_FSR_TrkChainx

	BRA_IFSET SEQ_EVNTT, 1, BANKED, CS_M_TRKLEN_GetChainValue_T23
CS_M_TRKLEN_GetChainValue_T01
	movlw	0x00
	btfsc	INDF1, SEQ_TRKCHAIN_12
	iorlw 0x01
	btfsc	INDF1, SEQ_TRKCHAIN_1234
	iorlw 0x02
	return

CS_M_TRKLEN_GetChainValue_T23
	movlw	0x00
	btfsc	INDF1, SEQ_TRKCHAIN_34
	iorlw 0x01
	btfsc	INDF1, SEQ_TRKCHAIN_1234
	iorlw 0x02
	return
	

;; ==========================================================================

CS_M_TRKLEN_QuickSelTable
	db	1, 3, 5, 7, 11, 15, 23, 31	; ...+1

	;; returns mapped length for quick selection
	;; IN: item number (0-7) in WREG
	;; OUT: mapped value in WREG
CS_M_TRKLEN_QuickSelTableGet
	addlw	LOW(CS_M_TRKLEN_QuickSelTable)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_TRKLEN_QuickSelTable)
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	return

	;; searches in table for a given length
	;; IN: length value in WREG
	;; OUT: table position in WREG (0xff if not found)
CS_M_TRKLEN_QuickSelTableSearch
	movwf	PRODL		; stores the search value
	clrf	PRODH		; used as loop counter
	TABLE_ADDR CS_M_TRKLEN_QuickSelTable
CS_M_TRKLEN_QuickSelTableSearchL
	tblrd*+
	movf	TABLAT, W
	cpfseq	PRODL, ACCESS
	rgoto CS_M_TRKLEN_QuickSelTableSearchN
	movf	PRODH, W
	return
CS_M_TRKLEN_QuickSelTableSearchN
	incf	PRODH, F
	BRA_IFCLR PRODH, 3, ACCESS, CS_M_TRKLEN_QuickSelTableSearchL
	retlw	0xff

;; ==========================================================================
