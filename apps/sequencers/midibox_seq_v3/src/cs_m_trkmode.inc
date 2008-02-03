; $Id$
;
; MBSEQ Menu "Track Mode"
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
CS_M_TRKMODE_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0, 4,	CS_M_TRKMODE_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE,			0x03,	     -1, -1,	CS_M_TRKMODE_ModeChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE_FLAGS,		0x01,	0xc0+ 1,  3,	CS_M_TRKMODE_HoldChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE_FLAGS,		0x01,	0xc0+ 6,  3,	CS_M_TRKMODE_SortChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE_FLAGS,		0x01,	0xc0+13,  3,	CS_M_TRKMODE_RestartChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE_FLAGS,		0x01,	0xc0+23,  3,	CS_M_TRKMODE_ForceScaleChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MODE_FLAGS,		0x01,	0xc0+34,  3,	CS_M_TRKMODE_SustainChanged

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
CS_M_TRKMODE_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKMODE_GP_Callback_1
	rgoto	CS_M_TRKMODE_GP_Callback_2
	rgoto	CS_M_TRKMODE_GP_Callback_3
	rgoto	CS_M_TRKMODE_GP_Callback_4
	rgoto	CS_M_TRKMODE_GP_Callback_5
	rgoto	CS_M_TRKMODE_GP_Callback_6
	rgoto	CS_M_TRKMODE_GP_Callback_7
	rgoto	CS_M_TRKMODE_GP_Callback_8
	rgoto	CS_M_TRKMODE_GP_Callback_9
	rgoto	CS_M_TRKMODE_GP_Callback_10
	rgoto	CS_M_TRKMODE_GP_Callback_11
	rgoto	CS_M_TRKMODE_GP_Callback_12
	rgoto	CS_M_TRKMODE_GP_Callback_13
	rgoto	CS_M_TRKMODE_GP_Callback_14
	rgoto	CS_M_TRKMODE_GP_Callback_15
	rgoto	CS_M_TRKMODE_GP_Callback_16

CS_M_TRKMODE_GP_Callback_1
	clrf	CS_MENU_CURSOR_POS	; select GxTy (Item #0)
	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_2
CS_M_TRKMODE_GP_Callback_3
CS_M_TRKMODE_GP_Callback_4
CS_M_TRKMODE_GP_Callback_5
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (radio button function)
	movlw	1			; select track mode (item #1)
	movwf	CS_MENU_CURSOR_POS
	decf	MIOS_PARAMETER1, W
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKMODE_ModeChanged
	rgoto	CS_M_TRKMODE_GP_Callback_Cont


CS_M_TRKMODE_GP_Callback_6		; ignore
CS_M_TRKMODE_GP_Callback_7
CS_M_TRKMODE_GP_Callback_8
	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_9
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (toggle function)
	movlw	2			; select and toggle Hold (item #2)
	movwf	CS_MENU_CURSOR_POS

	movlw	(1 << 0)		; CC value position of HOLD flag
	rcall	CS_M_TRKMODE_Hlp_ModeFlagToggle

	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_10
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (toggle function)
	movlw	3			; select and toggle Sort (item #3)
	movwf	CS_MENU_CURSOR_POS

	movlw	(1 << 4)		; CC value position of UNSORTED flag
	rcall	CS_M_TRKMODE_Hlp_ModeFlagToggle

	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_11
CS_M_TRKMODE_GP_Callback_12
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (toggle function)
	movlw	4			; select and toggle Restart (item #4)
	movwf	CS_MENU_CURSOR_POS

	movlw	(1 << 1)		; CC value position of RESTART flag
	rcall	CS_M_TRKMODE_Hlp_ModeFlagToggle

	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_13
CS_M_TRKMODE_GP_Callback_14
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (toggle function)
	movlw	5			; select and toggle ForceScale (item #5)
	movwf	CS_MENU_CURSOR_POS

	movlw	(1 << 2)		; CC value position of FORCE_SCALE flag
	rcall	CS_M_TRKMODE_Hlp_ModeFlagToggle

	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_15
CS_M_TRKMODE_GP_Callback_16
	clrf	CS_MENU_ENC_INCREMENTER	; clear incrementer (toggle function)
	movlw	6			; select and toggle Sustain flag (item #6)
	movwf	CS_MENU_CURSOR_POS

	movlw	(1 << 3)		; CC value position of SUSTAIN flag
	rcall	CS_M_TRKMODE_Hlp_ModeFlagToggle

	;; 	rgoto	CS_M_TRKMODE_GP_Callback_Cont

CS_M_TRKMODE_GP_Callback_Cont

	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_TRKMODE_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 2-7: track mode
	movlw	1
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKMODE_UpdateGPLEDs_NoMod
CS_M_TRKMODE_UpdateGPLEDs_Mod
	movlw	SEQ_CC_OFFSET_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	clrc
	rlf	WREG, W
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
CS_M_TRKMODE_UpdateGPLEDs_NoMod

	;; LED 1: GxTy (item #0) selected
	movf	CS_MENU_CURSOR_POS, W
	skpnz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0


	;; if cursor >= 2, get pattern from table
	movlw	2-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKMODE_UpdateGPLEDs_End

	movf	CS_MENU_CURSOR_POS, W
	addlw	-2
	TABLE_ADDR_MUL_W CS_M_TRKMODE_UpdateGPLEDs_TABLE, 1
	tblrd*+
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR1

CS_M_TRKMODE_UpdateGPLEDs_End
	return

CS_M_TRKMODE_UpdateGPLEDs_TABLE
	db	b'00000001', b'00000010'	; right half
	db	b'00001100', b'00110000'
	db	b'11000000'


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKMODE_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_TRKMODE_PARAMETERS	; pointer to table
	movlw	7				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKMODE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKMODE_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKMODE_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk. off     Transpose                  Hold Sort  Restart  ForceScale  Sustain 
	;; 444400000---22222222222-----------------8888888888AAAAAAAAAACCCCCCCCCCEEEEEEEEEE
	;; G1T1   >Normal<  Arpeggiator             on   on     on        on         on     
	;; 5555---11111111-3333333333333-----------9999999999BBBBBBBBBBDDDDDDDDDDFFFFFFFFFF

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKMODE_Tick_0
	rgoto	CS_M_TRKMODE_Tick_1
	rgoto	CS_M_TRKMODE_Tick_2
	rgoto	CS_M_TRKMODE_Tick_3
	rgoto	CS_M_TRKMODE_Tick_4
	rgoto	CS_M_TRKMODE_Tick_5
	rgoto	CS_M_TRKMODE_Tick_6
	rgoto	CS_M_TRKMODE_Tick_7
	rgoto	CS_M_TRKMODE_Tick_8
	rgoto	CS_M_TRKMODE_Tick_9
	rgoto	CS_M_TRKMODE_Tick_A
	rgoto	CS_M_TRKMODE_Tick_B
	rgoto	CS_M_TRKMODE_Tick_C
	rgoto	CS_M_TRKMODE_Tick_D
	rgoto	CS_M_TRKMODE_Tick_E
	rgoto	CS_M_TRKMODE_Tick_F

CS_M_TRKMODE_Tick_0
CS_M_TRKMODE_Tick_1
CS_M_TRKMODE_Tick_2
CS_M_TRKMODE_Tick_3
	movf	CS_MENU_REFRESH_ITEM_CTR, W	; derive track mode item number
	rcall	CS_M_TRKMODE_PrnItem		; print item
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_4_STR	STRING 4, 0x00 + 0, "Trk."
CS_M_TRKMODE_Tick_4
	TABLE_ADDR CS_M_TRKMODE_Tick_4_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_5
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_6				; ignore
CS_M_TRKMODE_Tick_7
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_8_STR	STRING 9, 0x80 + 0, "Hold Sort"
CS_M_TRKMODE_Tick_8
	TABLE_ADDR CS_M_TRKMODE_Tick_8_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_9
	movlw	0xc0 + 1
	call	MIOS_LCD_CursorSet
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKMODE_Tick_9_Cont

	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 0)		; CC value position of HOLD flag
	call	SEQ_LCD_PrnOnOff

CS_M_TRKMODE_Tick_9_Cont
	movlw	0xc0 + 6
	call	MIOS_LCD_CursorSet
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKMODE_Tick_Cont

	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 4)		; CC value position of UNSORTED flag
	xorlw	(1 << 4)		; (invert flag: unsorted -> sorted)
	call	SEQ_LCD_PrnOnOff
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_A_STR	STRING 7, 0x80 + 11, "Restart"
CS_M_TRKMODE_Tick_A
	TABLE_ADDR CS_M_TRKMODE_Tick_A_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_B
	movlw	0xc0 + 13
	call	MIOS_LCD_CursorSet
	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKMODE_Tick_Cont

	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 1)		; CC value position of RESTART flag
	call	SEQ_LCD_PrnOnOff
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_C_STR	STRING 10, 0x80 + 20, "ForceScale"
CS_M_TRKMODE_Tick_C
	TABLE_ADDR CS_M_TRKMODE_Tick_C_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_D
	movlw	0xc0 + 23
	call	MIOS_LCD_CursorSet
	movlw	5		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKMODE_Tick_Cont

	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 2)		; CC value position of FORCE_SCALE flag
	call	SEQ_LCD_PrnOnOff
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_E_STR	STRING 7, 0x80 + 32, "Sustain"
CS_M_TRKMODE_Tick_E
	TABLE_ADDR CS_M_TRKMODE_Tick_E_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_F
	movlw	0xc0 + 34
	call	MIOS_LCD_CursorSet
	movlw	6		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKMODE_Tick_Cont

	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 3)		; CC value position of SUSTAIN flag
	call	SEQ_LCD_PrnOnOff
	;; 	rgoto	CS_M_TRKMODE_Tick_Cont

CS_M_TRKMODE_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKMODE_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKMODE_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKMODE_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKMODE_UpdateGPLEDs

CS_M_TRKMODE_ModeChanged	; modify bit 2..0
	movff	CS_MENU_PARAMETER_L, TMP1
	movlw	SEQ_CC_OFFSET_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	b'11111000'
	iorwf	CS_MENU_PARAMETER_L, F
	movlw	SEQ_CC_OFFSET_MODE
	call	CS_M_HLP_TrkCC_Set
	movff	TMP1, CS_MENU_PARAMETER_L
	return

CS_M_TRKMODE_SortChanged
	movlw	(1 << 4)	; CC value position of UNSORTED flag
	rgoto	CS_M_TRKMODE_Hlp_ModeFlagMod

CS_M_TRKMODE_HoldChanged
	movlw	(1 << 0)	; CC value position of HOLD flag
	rgoto	CS_M_TRKMODE_Hlp_ModeFlagMod

CS_M_TRKMODE_RestartChanged
	movlw	(1 << 1)	; CC value position of RESTART flag
	rgoto	CS_M_TRKMODE_Hlp_ModeFlagMod

CS_M_TRKMODE_ForceScaleChanged
	movlw	(1 << 2)	; CC value position of FORCE_SCALE flag
	rgoto	CS_M_TRKMODE_Hlp_ModeFlagMod

CS_M_TRKMODE_SustainChanged
	movlw	(1 << 3)	; CC value position of SUSTAIN flag
	rgoto	CS_M_TRKMODE_Hlp_ModeFlagMod


;; ==========================================================================
;; Help functions for Track Mode Flags
;; ==========================================================================

	;; used by CS_M_TRKMODE_GP_Callback
CS_M_TRKMODE_Hlp_ModeFlagToggle
	movwf	TMP1
	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	xorwf	TMP1, W
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	goto	CS_M_HLP_TrkCC_Set


	;; used by CS_M_TRKMODE_XXXChanged
CS_M_TRKMODE_Hlp_ModeFlagMod
	movwf	TMP1		; OR mask
	xorlw	0xff
	movwf	TMP2		; AND mask
	movff	CS_MENU_PARAMETER_L, TMP3	; will be temporary overwritten
	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Get
	andwf	TMP2, W
	btfsc	CS_MENU_PARAMETER_L, 0
	iorwf	TMP1, W
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_MODE_FLAGS
	call	CS_M_HLP_TrkCC_Set
	movff	TMP3, CS_MENU_PARAMETER_L	; restore CS_MENU_PARAMETER_L
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function prints the selected track mode item
;; --------------------------------------------------------------------------
CS_M_TRKMODE_PrnItem
	movwf	TMP1

	movlw	SEQ_CC_OFFSET_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	0x07
	movwf	TMP2

	;; calculate pointer to name entry
	TABLE_ADDR CS_M_TRKMODE_NAME_TABLE
	movf	TMP1, W
	mullw	CS_M_TRKMODE_NAME_LEN
	movf	PRODL, W
	TABLE_ADD_W

	;; determine cursor pos
	movf	TMP1, W
	mullw	5
	movf	PRODL, W
	btfss	TMP1, 0
	addlw 0x00 + 2
	btfsc	TMP1, 0
	addlw 0x40 + 1
	movwf	TMP5
	call	MIOS_LCD_CursorSet
	movlw	0x3f		; store X position
	andwf	TMP5, F

	;; print unmodified name if item selected
	;; replace '>' and '<' by space if item not selected
	;; flash item (print only '>'/'<' and spaces) if cursor position == 1 and flash flag set by timer
	movlw	CS_M_TRKMODE_NAME_LEN
	movwf	TMP3

	clrf	TMP4		; if bit 0 set: flash item
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKMODE_PrnItem_NoFlash
	movf	TMP1, W		; compare item number with CC value
	cpfseq	TMP2, ACCESS
	rgoto CS_M_TRKMODE_PrnItem_NoFlash
	movlw	1		; compare cursor pos
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKMODE_PrnItem_NoFlash
	bsf	TMP4, 0
CS_M_TRKMODE_PrnItem_NoFlash

CS_M_TRKMODE_PrnItemLoop
	tblrd*+
	movf	TABLAT, W
	xorlw	'>'
	bz	CS_M_TRKMODE_PrnItemLoop_ModChk
	movf	TABLAT, W
	xorlw	'<'
	bnz	CS_M_TRKMODE_PrnItemLoop_NoMod
CS_M_TRKMODE_PrnItemLoop_ModChk
	movf	TABLAT, W	; (flash flag set)
	BRA_IFSET TMP4, 0, ACCESS, CS_M_TRKMODE_PrnItemLoop_PrnChr 
	movf	TMP1, W		; compare item number with CC value
	xorwf	TMP2, W
	bz	CS_M_TRKMODE_PrnItemLoop_NoMod
	movlw	' '
	rgoto	CS_M_TRKMODE_PrnItemLoop_PrnChr	
CS_M_TRKMODE_PrnItemLoop_NoMod
	movf	TABLAT, W
	btfsc	TMP4, 0; (flash flag set)
	movlw ' '	
CS_M_TRKMODE_PrnItemLoop_PrnChr
	call	MIOS_LCD_PrintChar
	BRA_IFSET TMP4, 1, ACCESS, CS_M_TRKMODE_PrnItemLoop_Break	; break loop if '<' reached
	incf	TMP5, F			; increment X
	movlw	40			; break loop if X >= 40
	cpfslt	TMP5, ACCESS
	rgoto CS_M_TRKMODE_PrnItemLoop_Break
CS_M_TRKMODE_PrnItemLoop_Next
	decfsz	TMP3, F
	rgoto	CS_M_TRKMODE_PrnItemLoop
CS_M_TRKMODE_PrnItemLoop_Break

CS_M_TRKMODE_PrnItemLoop_End
	return

;; ==========================================================================

CS_M_TRKMODE_NAME_LEN		EQU	14	; 14 characters per entry
CS_M_TRKMODE_NAME_ENTRIES	EQU	4	; 4 entries
CS_M_TRKMODE_NAME_TABLE				; all entries are listed here
	db	"  >off<       "
	db	" >Normal<     "
	db	">Transpose<   "
	db	">Arpeggiator< "
	
