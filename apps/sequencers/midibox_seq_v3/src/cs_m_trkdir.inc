; $Id$
;
; MBSEQ Menu "Track Direction"
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
CS_M_TRKDIR_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKDIR_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_DIRECTION,		0x06,	     -1, -1,	CS_M_TRKDIR_ModeChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_DIRPAR_FORWARD,		0x07,	0xc0+11,  1,	CS_M_TRKDIR_FwdChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_DIRPAR_JMPBCK,		0x07,	0xc0+25,  1,	CS_M_TRKDIR_JmpBckChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_DIRPAR_REPLAY,		0x07,	0xc0+38,  1,	CS_M_TRKDIR_ReplayChanged

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
CS_M_TRKDIR_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKDIR_GP_Callback_1
	rgoto	CS_M_TRKDIR_GP_Callback_2
	rgoto	CS_M_TRKDIR_GP_Callback_3
	rgoto	CS_M_TRKDIR_GP_Callback_4
	rgoto	CS_M_TRKDIR_GP_Callback_5
	rgoto	CS_M_TRKDIR_GP_Callback_6
	rgoto	CS_M_TRKDIR_GP_Callback_7
	rgoto	CS_M_TRKDIR_GP_Callback_8
	rgoto	CS_M_TRKDIR_GP_Callback_9
	rgoto	CS_M_TRKDIR_GP_Callback_10
	rgoto	CS_M_TRKDIR_GP_Callback_11
	rgoto	CS_M_TRKDIR_GP_Callback_12
	rgoto	CS_M_TRKDIR_GP_Callback_13
	rgoto	CS_M_TRKDIR_GP_Callback_14
	rgoto	CS_M_TRKDIR_GP_Callback_15
	rgoto	CS_M_TRKDIR_GP_Callback_16

CS_M_TRKDIR_GP_Callback_1
	clrf	CS_MENU_CURSOR_POS	; select GxTy (Item #0)
	rgoto	CS_M_TRKDIR_GP_Callback_Cont

CS_M_TRKDIR_GP_Callback_2
CS_M_TRKDIR_GP_Callback_3
CS_M_TRKDIR_GP_Callback_4
CS_M_TRKDIR_GP_Callback_5
CS_M_TRKDIR_GP_Callback_6
CS_M_TRKDIR_GP_Callback_7
CS_M_TRKDIR_GP_Callback_8
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (radio button function)
	movlw	1			; select DIR mode (item #1)
	movwf	CS_MENU_CURSOR_POS
	decf	MIOS_PARAMETER1, W
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKDIR_ModeChanged
	rgoto	CS_M_TRKDIR_GP_Callback_Cont


CS_M_TRKDIR_GP_Callback_9
CS_M_TRKDIR_GP_Callback_10
CS_M_TRKDIR_GP_Callback_11
	movlw	2			; select Steps Fwd (item #2)
	movwf	CS_MENU_CURSOR_POS
	rgoto	CS_M_TRKDIR_GP_Callback_Cont

CS_M_TRKDIR_GP_Callback_12
CS_M_TRKDIR_GP_Callback_13
	movlw	3			; select Jump Back (item #3)
	movwf	CS_MENU_CURSOR_POS
	rgoto	CS_M_TRKDIR_GP_Callback_Cont

CS_M_TRKDIR_GP_Callback_14
CS_M_TRKDIR_GP_Callback_15
CS_M_TRKDIR_GP_Callback_16
	movlw	4			; select Replay (item #4)
	movwf	CS_MENU_CURSOR_POS
	;; 	rgoto	CS_M_TRKDIR_GP_Callback_Cont

CS_M_TRKDIR_GP_Callback_Cont

	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_TRKDIR_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 2-7: DIR mode
	movlw	1
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIR_UpdateGPLEDs_NoDir
CS_M_TRKDIR_UpdateGPLEDs_Dir
	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_HLP_GetBitORMask
	clrc
	rlf	WREG, W
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
CS_M_TRKDIR_UpdateGPLEDs_NoDir

	;; LED 1: GxTy (item #0) selected
	movf	CS_MENU_CURSOR_POS, W
	skpnz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0

	;; LED 9/10/11: Forward (item #2) selected
	movlw	2
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIR_UpdateGPLEDs_NoFwd
CS_M_TRKDIR_UpdateGPLEDs_Fwd
	movlw	b'00000111'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F
CS_M_TRKDIR_UpdateGPLEDs_NoFwd

	;; LED 12/13: Jump Back (item #3) selected
	movlw	3
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIR_UpdateGPLEDs_NoJBck
CS_M_TRKDIR_UpdateGPLEDs_JBck
	movlw	b'00011000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F
CS_M_TRKDIR_UpdateGPLEDs_NoJBck

	;; LED 14/15/16: Replay (item #4) selected
	movlw	4
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIR_UpdateGPLEDs_NoRpl
CS_M_TRKDIR_UpdateGPLEDs_Rpl
	movlw	b'11100000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F
CS_M_TRKDIR_UpdateGPLEDs_NoRpl
	
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKDIR_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_TRKDIR_PARAMETERS	; pointer to table
	movlw	5				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKDIR_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKDIR_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKDIR_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; >Forward<  PingPong  Rand.Dir  Rand.D+S  Experimental "Progression" Parameters
	;; 1111111111333333333355555555557777777777-888888888888-AAAAAAAAAAAAA-CCCCCCCCCC--
	;; G1T1   Backward  Pendulum  Rand.Step    Steps Fwd: 1  Jump Back: 0  Replay: x 1
	;; 0000--2222222222444444444466666666666---999999999999--BBBBBBBBBBBB--DDDDDDDDDDD-

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKDIR_Tick_0
	rgoto	CS_M_TRKDIR_Tick_1
	rgoto	CS_M_TRKDIR_Tick_2
	rgoto	CS_M_TRKDIR_Tick_3
	rgoto	CS_M_TRKDIR_Tick_4
	rgoto	CS_M_TRKDIR_Tick_5
	rgoto	CS_M_TRKDIR_Tick_6
	rgoto	CS_M_TRKDIR_Tick_7
	rgoto	CS_M_TRKDIR_Tick_8
	rgoto	CS_M_TRKDIR_Tick_9
	rgoto	CS_M_TRKDIR_Tick_A
	rgoto	CS_M_TRKDIR_Tick_B
	rgoto	CS_M_TRKDIR_Tick_C
	rgoto	CS_M_TRKDIR_Tick_D
	rgoto	CS_M_TRKDIR_Tick_E
	rgoto	CS_M_TRKDIR_Tick_F

CS_M_TRKDIR_Tick_0
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_1
CS_M_TRKDIR_Tick_2
CS_M_TRKDIR_Tick_3
CS_M_TRKDIR_Tick_4
CS_M_TRKDIR_Tick_5
CS_M_TRKDIR_Tick_6
CS_M_TRKDIR_Tick_7
	decf	CS_MENU_REFRESH_ITEM_CTR, W	; derive direction mode item number
	rcall	CS_M_TRKDIR_PrnItem		; print item
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_8_STR	STRING 12, 0x80 + 1, "Experimental"
CS_M_TRKDIR_Tick_8
	TABLE_ADDR CS_M_TRKDIR_Tick_8_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_9_STR	STRING 11, 0xc0 + 0, "Steps Fwd: "
CS_M_TRKDIR_Tick_9
	TABLE_ADDR CS_M_TRKDIR_Tick_9_STR
	call	MIOS_LCD_PrintString
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIR_Tick_Cont
	movlw	SEQ_CC_OFFSET_DIRPAR_FORWARD
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_A_STR	STRING 13, 0x80 + 14, "\"Progression\""
CS_M_TRKDIR_Tick_A
	TABLE_ADDR CS_M_TRKDIR_Tick_A_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_B_STR	STRING 11, 0xc0 + 14, "Jump Back: "
CS_M_TRKDIR_Tick_B
	TABLE_ADDR CS_M_TRKDIR_Tick_B_STR
	call	MIOS_LCD_PrintString
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIR_Tick_Cont
	movlw	SEQ_CC_OFFSET_DIRPAR_JMPBCK
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_C_STR	STRING 10, 0x80 + 28, "Parameters"
CS_M_TRKDIR_Tick_C
	TABLE_ADDR CS_M_TRKDIR_Tick_C_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_D_STR	STRING 10, 0xc0 + 28, "Replay: x "
CS_M_TRKDIR_Tick_D
	TABLE_ADDR CS_M_TRKDIR_Tick_D_STR
	call	MIOS_LCD_PrintString
	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKDIR_Tick_Cont
	movlw	SEQ_CC_OFFSET_DIRPAR_REPLAY
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_TRKDIR_Tick_Cont

CS_M_TRKDIR_Tick_E
CS_M_TRKDIR_Tick_F

CS_M_TRKDIR_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKDIR_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKDIR_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  These hooks are called when a menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKDIR_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKDIR_UpdateGPLEDs

CS_M_TRKDIR_ModeChanged
	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Set
	rgoto	CS_M_TRKDIR_UpdateGPLEDs

CS_M_TRKDIR_FwdChanged
	movlw	SEQ_CC_OFFSET_DIRPAR_FORWARD
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKDIR_JmpBckChanged
	movlw	SEQ_CC_OFFSET_DIRPAR_JMPBCK
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKDIR_ReplayChanged
	movlw	SEQ_CC_OFFSET_DIRPAR_REPLAY
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function prints the selected direction mode item
;; --------------------------------------------------------------------------
CS_M_TRKDIR_PrnItem
	movwf	TMP1

	movlw	SEQ_CC_OFFSET_DIRECTION
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP2

	;; calculate pointer to name entry
	TABLE_ADDR CS_M_TRKDIR_NAME_TABLE
	movf	TMP1, W
	mullw	CS_M_TRKDIR_NAME_LEN
	movf	PRODL, W
	TABLE_ADD_W

	;; determine cursor pos
	movf	TMP1, W
	mullw	5
	movf	PRODL, W
	btfss	TMP1, 0
	addlw 0x00
	btfsc	TMP1, 0
	addlw 0x40 + 1
	movwf	TMP5
	call	MIOS_LCD_CursorSet
	movlw	0x3f		; store X position
	andwf	TMP5, F

	;; print unmodified name if item selected
	;; replace '>' and '<' by space if item not selected
	;; flash item (print only '>'/'<' and spaces) if cursor position == 1 and flash flag set by timer
	movlw	CS_M_TRKDIR_NAME_LEN
	movwf	TMP3

	clrf	TMP4		; if bit 0 set: flash item
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKDIR_PrnItem_NoFlash
	movf	TMP1, W		; compare item number with CC value
	cpfseq	TMP2, ACCESS
	rgoto CS_M_TRKDIR_PrnItem_NoFlash
	movlw	1		; compare cursor pos
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKDIR_PrnItem_NoFlash
	bsf	TMP4, 0
CS_M_TRKDIR_PrnItem_NoFlash

CS_M_TRKDIR_PrnItemLoop
	tblrd*+
	movf	TABLAT, W
	xorlw	'>'
	bz	CS_M_TRKDIR_PrnItemLoop_ModChk
	movf	TABLAT, W
	xorlw	'<'
	bnz	CS_M_TRKDIR_PrnItemLoop_NoMod
CS_M_TRKDIR_PrnItemLoop_ModChk
	movf	TABLAT, W	; (flash flag set)
	BRA_IFSET TMP4, 0, ACCESS, CS_M_TRKDIR_PrnItemLoop_PrnChr 
	movf	TMP1, W		; compare item number with CC value
	xorwf	TMP2, W
	bz	CS_M_TRKDIR_PrnItemLoop_NoMod
	movlw	' '
	rgoto	CS_M_TRKDIR_PrnItemLoop_PrnChr	
CS_M_TRKDIR_PrnItemLoop_NoMod
	movf	TABLAT, W
	btfsc	TMP4, 0; (flash flag set)
	movlw ' '	
CS_M_TRKDIR_PrnItemLoop_PrnChr
	call	MIOS_LCD_PrintChar
	BRA_IFSET TMP4, 1, ACCESS, CS_M_TRKDIR_PrnItemLoop_Break	; break loop if '<' reached
	incf	TMP5, F			; increment X
	movlw	40			; break loop if X >= 40
	cpfslt	TMP5, ACCESS
	rgoto CS_M_TRKDIR_PrnItemLoop_Break
CS_M_TRKDIR_PrnItemLoop_Next
	decfsz	TMP3, F
	rgoto	CS_M_TRKDIR_PrnItemLoop
CS_M_TRKDIR_PrnItemLoop_Break

CS_M_TRKDIR_PrnItemLoop_End
	return

;; ==========================================================================

CS_M_TRKDIR_NAME_LEN		EQU	12	; 12 characters per entry
CS_M_TRKDIR_NAME_ENTRIES	EQU	7	; 7 entries
CS_M_TRKDIR_NAME_TABLE				; all entries are listed here
	db	">Forward<   "
	db	">Backward<  "
	db	">PingPong<  "
	db	">Pendulum<  "
	db	">Rand.Dir<  "
	db	">Rand.Step< "
	db	">Rand.D+S<  "
