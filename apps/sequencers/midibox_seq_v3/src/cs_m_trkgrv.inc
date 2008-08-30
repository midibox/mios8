; $Id$
;
; MBSEQ Menu "Groove Style"
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
CS_M_TRKGRV_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKGRV_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_GROOVE_STYLE, SEQ_GROOVE_NUMBER-1, 0x40+ 6, 12,	CS_M_TRKGRV_GrvStyleChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_GROOVE_VALUE,		0x0f,	0x40+23,  2,	CS_M_TRKGRV_GrvValueChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_HUMANIZE_VALUE,		0x0f,	0xc0+ 6,  2,	CS_M_TRKGRV_HumValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+15,  3,	CS_M_TRKGRV_HumModeNChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+23,  3,	CS_M_TRKGRV_HumModeVChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+33,  3,	CS_M_TRKGRV_HumModeLChanged

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
CS_M_TRKGRV_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKGRV_GP_Callback_1
	rgoto	CS_M_TRKGRV_GP_Callback_2
	rgoto	CS_M_TRKGRV_GP_Callback_3
	rgoto	CS_M_TRKGRV_GP_Callback_4
	rgoto	CS_M_TRKGRV_GP_Callback_5
	rgoto	CS_M_TRKGRV_GP_Callback_6
	rgoto	CS_M_TRKGRV_GP_Callback_7
	rgoto	CS_M_TRKGRV_GP_Callback_8
	rgoto	CS_M_TRKGRV_GP_Callback_9
	rgoto	CS_M_TRKGRV_GP_Callback_10
	rgoto	CS_M_TRKGRV_GP_Callback_11
	rgoto	CS_M_TRKGRV_GP_Callback_12
	rgoto	CS_M_TRKGRV_GP_Callback_13
	rgoto	CS_M_TRKGRV_GP_Callback_14
	rgoto	CS_M_TRKGRV_GP_Callback_15
	rgoto	CS_M_TRKGRV_GP_Callback_16

CS_M_TRKGRV_GP_Callback_1
	movlw	0		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont

CS_M_TRKGRV_GP_Callback_2
CS_M_TRKGRV_GP_Callback_3
CS_M_TRKGRV_GP_Callback_4
	movlw	1		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont

CS_M_TRKGRV_GP_Callback_5
CS_M_TRKGRV_GP_Callback_6
	movlw	2		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont

CS_M_TRKGRV_GP_Callback_7
CS_M_TRKGRV_GP_Callback_8
	;; ignored
	clrf	CS_MENU_ENC_INCREMENTER
	return

CS_M_TRKGRV_GP_Callback_9
CS_M_TRKGRV_GP_Callback_10
	movlw	3		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont

CS_M_TRKGRV_GP_Callback_11
CS_M_TRKGRV_GP_Callback_12
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE	; toggle Humanize Note Flag
	call	CS_M_HLP_TrkCC_Get
	xorlw	1 << 0
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set

	movlw	4		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont_NP

CS_M_TRKGRV_GP_Callback_13
CS_M_TRKGRV_GP_Callback_14
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE	; toggle Humanize Velocity Flag
	call	CS_M_HLP_TrkCC_Get
	xorlw	1 << 1
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set

	movlw	5		; cursor pos
	rgoto	CS_M_TRKGRV_GP_Callback_Cont_NP

CS_M_TRKGRV_GP_Callback_15
CS_M_TRKGRV_GP_Callback_16
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE	; toggle Humanize Length Flag
	call	CS_M_HLP_TrkCC_Get
	xorlw	1 << 2
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set

	movlw	6		; cursor pos
 	rgoto	CS_M_TRKGRV_GP_Callback_Cont_NP

CS_M_TRKGRV_GP_Callback_Cont	; toggle function
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKGRV_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_TRKGRV_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

;; --------------------------------------------------------------------------
CS_M_TRKGRV_UpdateGPLEDs
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

	;; LED 2-4: Groove Mode (item #1) selected
	movlw	1
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKGRV_UpdateGPLEDs_NoMod
CS_M_TRKGRV_UpdateGPLEDs_Mod
	movlw	b'00001110'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKGRV_UpdateGPLEDs_NoMod

	;; LED 5-5: Intensity (item #2) selected
	movlw	2
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKGRV_UpdateGPLEDs_NoInt
CS_M_TRKGRV_UpdateGPLEDs_Int
	movlw	b'00110000'
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR0, F
CS_M_TRKGRV_UpdateGPLEDs_NoInt

	;; right half?
	movlw	3-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; right half: determine pattern with a short algorithm
	;; 0 -> 00000011
	;; 1 -> 00001100
	;; 2 -> 00110000
	;; 3 -> 11000000
	movf	CS_MENU_CURSOR_POS, W
	addlw	-3
	clrc
	rlf	WREG, W
	movwf	TMP1
	call	MIOS_HLP_GetBitORMask
	movwf	TMP2

	movf	TMP1, W
	addlw	1
	call	MIOS_HLP_GetBitORMask
	iorwf	TMP2, W
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKGRV_PARAMETERS	; pointer to table
	movlw	7				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy value into menu parameter depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKGRV_Init_Cursor0
	rgoto	CS_M_TRKGRV_Init_Cursor1
	rgoto	CS_M_TRKGRV_Init_Cursor2
	rgoto	CS_M_TRKGRV_Init_Cursor3
	rgoto	CS_M_TRKGRV_Init_Cursor4
	rgoto	CS_M_TRKGRV_Init_Cursor5
	rgoto	CS_M_TRKGRV_Init_Cursor6
	rgoto	CS_M_TRKGRV_Init_Cursor7

CS_M_TRKGRV_Init_Cursor0	; Track Number
	;; already prepared
	rgoto	CS_M_TRKGRV_Init_Cont
	
CS_M_TRKGRV_Init_Cursor1	; groove mode
	;; already prepared
	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor2	; groove intensinty
	;; already prepared
	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor3	; humanize value
	;; already prepared
	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor4	; humanize mode: note
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	0x01
	movwf	CS_MENU_PARAMETER_L
 	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor5	; humanize mode: velocity
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	0x02
	skpz
	movlw	0x01
	movwf	CS_MENU_PARAMETER_L
 	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor6	; humanize mode: length
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	0x04
	skpz
	movlw	0x01
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cursor7	; dummy
	;;  	rgoto	CS_M_TRKGRV_Init_Cont

CS_M_TRKGRV_Init_Cont

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_TRKGRV_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKGRV_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Tick_Upper_STR
	db	"Trk.  Groove Style  Intensity           RndIntensity  Note   Vel./CC    Length  "
CS_M_TRKGRV_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.  Groove Style  Intensity           RndIntensity  Note   Vel./CC    Length  
	;; 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	;; G1T1  Inv. Shuffle     15                     0        on      off       off            
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	CS_M_HLP_LCD_Tick_Resume ; (will branch to next CS_M_TRKGRV_Tick_x)

CS_M_TRKGRV_Tick_Upper
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	10
	movlw	LOW(CS_M_TRKGRV_Tick_Upper_STR)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_TRKGRV_Tick_Upper_STR)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; repeat 8 times
	rcall	CS_M_TRKGRV_Tick_Yield
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKGRV_Tick_Upper

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_8
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy

	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_9
	movlw	0x40 + 6
	call	MIOS_LCD_CursorSet

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_9_End

	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_GROOVE_GetName

CS_M_TRKGRV_Tick_9_End
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_A
	movlw	0x40 + 23
	call	MIOS_LCD_CursorSet

	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_A_End

	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD2

CS_M_TRKGRV_Tick_A_End
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_B
	;; not used
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_C
	movlw	0xc0 + 6
	call	MIOS_LCD_CursorSet

	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_C_End

	movlw	SEQ_CC_OFFSET_HUMANIZE_VALUE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD2

CS_M_TRKGRV_Tick_C_End
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_D
	movlw	0xc0 + 15
	call	MIOS_LCD_CursorSet

	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_D_End

	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 0)
	call	SEQ_LCD_PrnOnOff

CS_M_TRKGRV_Tick_D_End
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_E
	movlw	0xc0 + 23
	call	MIOS_LCD_CursorSet

	movlw	5		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_E_End

	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 1)
	call	SEQ_LCD_PrnOnOff

CS_M_TRKGRV_Tick_E_End
	rcall	CS_M_TRKGRV_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_F
	movlw	0xc0 + 33
	call	MIOS_LCD_CursorSet

	movlw	6		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKGRV_Tick_F_End

	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	(1 << 2)
	call	SEQ_LCD_PrnOnOff

CS_M_TRKGRV_Tick_F_End

        ;; ------------------------------------------------------------------
CS_M_TRKGRV_Tick_End
	goto	CS_M_HLP_LCD_Tick_End

CS_M_TRKGRV_Tick_Yield
	goto	CS_M_HLP_LCD_Tick_Yield


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKGRV_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKGRV_UpdateGPLEDs

CS_M_TRKGRV_GrvValueChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_GrvStyleChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_HumValueChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_HUMANIZE_VALUE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_HumModeNChanged
	;; modify bit #0
	movff	CS_MENU_PARAMETER_L, TMP1
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	~(1 << 0)
	btfsc	CS_MENU_PARAMETER_L, 0
	iorlw	(1 << 0)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set
	movff	TMP1, CS_MENU_PARAMETER_L
	return

CS_M_TRKGRV_HumModeVChanged
	;; modify bit #1
	movff	CS_MENU_PARAMETER_L, TMP1
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	~(1 << 1)
	btfsc	CS_MENU_PARAMETER_L, 0
	iorlw	(1 << 1)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set
	movff	TMP1, CS_MENU_PARAMETER_L
	return

CS_M_TRKGRV_HumModeLChanged
	;; modify bit #2
	movff	CS_MENU_PARAMETER_L, TMP1
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	andlw	~(1 << 2)
	btfsc	CS_MENU_PARAMETER_L, 0
	iorlw	(1 << 2)
	movwf	CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Set
	movff	TMP1, CS_MENU_PARAMETER_L
	return


;; ==========================================================================
