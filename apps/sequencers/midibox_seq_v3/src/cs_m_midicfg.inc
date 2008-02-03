; $Id$
;
; MBSEQ Menu for editing the MIDI settings
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
CS_M_MIDICFG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x10, 0x40+ 4, 3,	CS_M_MIDICFG_KeybChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01, 0x40+13, 3,	CS_M_MIDICFG_TA_SplitChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f, 0x40+23, 3,	CS_M_MIDICFG_MiddNoteChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01, 0x40+33, 3,	CS_M_MIDICFG_MergerChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01, 0xc0+0,  4,	CS_M_MIDICFG_RSourceChanged
 CS_MENU_PARAMETER_ENTRY -1,					SEQ_ROUTER_AVAILABLE_PORTS-1, 0xc0+ 5, 4, CS_M_MIDICFG_RTargetChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x11, 0xc0+13, 3,	CS_M_MIDICFG_RSrcChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x11, 0xc0+23, 3,	CS_M_MIDICFG_RDstChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01, 0xc0+33, 3,	CS_M_MIDICFG_RMClkChanged


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
CS_M_MIDICFG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MIDICFG_GP_Callback_1
	rgoto	CS_M_MIDICFG_GP_Callback_2
	rgoto	CS_M_MIDICFG_GP_Callback_3
	rgoto	CS_M_MIDICFG_GP_Callback_4
	rgoto	CS_M_MIDICFG_GP_Callback_5
	rgoto	CS_M_MIDICFG_GP_Callback_6
	rgoto	CS_M_MIDICFG_GP_Callback_7
	rgoto	CS_M_MIDICFG_GP_Callback_8
	rgoto	CS_M_MIDICFG_GP_Callback_9
	rgoto	CS_M_MIDICFG_GP_Callback_10
	rgoto	CS_M_MIDICFG_GP_Callback_11
	rgoto	CS_M_MIDICFG_GP_Callback_12
	rgoto	CS_M_MIDICFG_GP_Callback_13
	rgoto	CS_M_MIDICFG_GP_Callback_14
	rgoto	CS_M_MIDICFG_GP_Callback_15
	rgoto	CS_M_MIDICFG_GP_Callback_16

CS_M_MIDICFG_GP_Callback_1
CS_M_MIDICFG_GP_Callback_2
	movlw	0		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_3
CS_M_MIDICFG_GP_Callback_4
	btg	SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED	; toggle split
	movlw	1		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont_NP

CS_M_MIDICFG_GP_Callback_5
CS_M_MIDICFG_GP_Callback_6
	movlw	2		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_7
CS_M_MIDICFG_GP_Callback_8
	btg	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED	; toggle merger
	call	CS_M_MIDICFG_MergerChanged_Quer
	movlw	3		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont_NP

CS_M_MIDICFG_GP_Callback_9
	movlw	4		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_10
	movlw	5		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_11
CS_M_MIDICFG_GP_Callback_12
	movlw	6		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_13
CS_M_MIDICFG_GP_Callback_14
	movlw	7		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont

CS_M_MIDICFG_GP_Callback_15
CS_M_MIDICFG_GP_Callback_16
	movf	CS_ROUTER_TARGET, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	xorwf	SEQ_ROUTER_MCLK_ENABLED, F, BANKED
	movlw	8		; cursor pos
	rgoto	CS_M_MIDICFG_GP_Callback_Cont_NP

CS_M_MIDICFG_GP_Callback_Cont	; toggle function
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_MIDICFG_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_MIDICFG_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

;; --------------------------------------------------------------------------
CS_M_MIDICFG_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; get pattern depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	TABLE_ADDR_MUL_W CS_M_MIDICFG_UpdateGPLEDs_TABLE, 1
	tblrd*+

	movlw	4
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_MIDICFG_UpdateGPLEDs_R
CS_M_MIDICFG_UpdateGPLEDs_L
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR0
	return

CS_M_MIDICFG_UpdateGPLEDs_R
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR1
	return

CS_M_MIDICFG_UpdateGPLEDs_TABLE
	db	b'00000011', b'00001100'	; left half
	db	b'00110000', b'11000000'
	db	b'00000001', b'00000010'	; right half
	db	b'00001100', b'00110000'
	db	b'11000000'

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_MIDICFG_PARAMETERS	; pointer to table
	movlw	9				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy value into menu parameter depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_MIDICFG_Init_Cursor0
	addlw	-1
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MIDICFG_Init_Cursor1
	rgoto	CS_M_MIDICFG_Init_Cursor2
	rgoto	CS_M_MIDICFG_Init_Cursor3
	rgoto	CS_M_MIDICFG_Init_Cursor4
	rgoto	CS_M_MIDICFG_Init_Cursor5
	rgoto	CS_M_MIDICFG_Init_Cursor6
	rgoto	CS_M_MIDICFG_Init_Cursor7
	rgoto	CS_M_MIDICFG_Init_Cursor8

CS_M_MIDICFG_Init_Cursor0	; Keyboard Channel
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	rgoto	CS_M_MIDICFG_Init_Cont
	
CS_M_MIDICFG_Init_Cursor1	; T/A Split
	movlw	0
	btfsc	SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED
	movlw 1
	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor2	; middle note
	movf	SEQ_MIDI_SPLIT_NOTE, W, BANKED
	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor3	; merger
	movlw	0
	btfsc	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED
	movlw 1
 	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor4	; router source
	movf	CS_ROUTER_SOURCE, W
 	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor5	; router target
	movf	CS_ROUTER_TARGET, W
 	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor6	; router source channel
	lfsr	FSR1, ROUTER_IN0_SRC0_CHN
	btfsc	CS_ROUTER_SOURCE, 0
	lfsr	FSR1, ROUTER_IN1_SRC0_CHN

	movf	CS_ROUTER_TARGET, W
	movf	PLUSW1, W
 	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor7	; router destination channel
	lfsr	FSR1, ROUTER_IN0_DST0_CHN
	btfsc	CS_ROUTER_SOURCE, 0
	lfsr	FSR1, ROUTER_IN1_DST0_CHN

	movf	CS_ROUTER_TARGET, W
	movf	PLUSW1, W
 	rgoto	CS_M_MIDICFG_Init_Cont

CS_M_MIDICFG_Init_Cursor8	; MIDI clock forwarding on/off
	movf	CS_ROUTER_TARGET, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_ROUTER_MCLK_ENABLED, W, BANKED
	movlw	0
	skpz
	movlw	1

CS_M_MIDICFG_Init_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_MIDICFG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_MIDICFG_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MIDICFG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Keyb.Chn. T/A Split Midd.Note   Merger  IN   OUT   Src.Chn.->Dst.Chn. MIDI Clock
	;; 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	;;    #16       off       C-3       off    Int. IIC1    All       All       On
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; print segments 0-15
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_MIDICFG_Tick_Upper
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MIDICFG_Tick_8
	rgoto	CS_M_MIDICFG_Tick_9
	rgoto	CS_M_MIDICFG_Tick_A
	rgoto	CS_M_MIDICFG_Tick_B
	rgoto	CS_M_MIDICFG_Tick_C
	rgoto	CS_M_MIDICFG_Tick_D
	rgoto	CS_M_MIDICFG_Tick_E
	rgoto	CS_M_MIDICFG_Tick_F

CS_M_MIDICFG_Tick_Upper_STR
	db	"Keyb.Chn. T/A Split Midd.Note   Merger  IN   OUT   Src.Chn.->Dst.Chn. MIDI Clock"
CS_M_MIDICFG_Tick_Upper
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
	movlw	LOW(CS_M_MIDICFG_Tick_Upper_STR)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_MIDICFG_Tick_Upper_STR)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_8
	movlw	0x40 + 3
	call	MIOS_LCD_CursorSet
	movlw	'#'
	call	MIOS_LCD_PrintChar

	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	movff	SEQ_MIDI_CHANNEL, WREG
	call	SEQ_LCD_PrnChn
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_9
	movlw	0x40 + 13
	call	MIOS_LCD_CursorSet

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	SET_BSR SEQ_BASE
	movlw	0
	btfsc	SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED
	movlw 1
	call	SEQ_LCD_PrnOnOff
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_A
	movlw	0x40 + 23
	call	MIOS_LCD_CursorSet

	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	movff	SEQ_MIDI_SPLIT_NOTE, WREG
	call	SEQ_LCD_PrnNote
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_B
	movlw	0x40 + 33
	call	MIOS_LCD_CursorSet

	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	SET_BSR SEQ_BASE
	movlw	0
	btfsc	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED
	movlw 1
	call	SEQ_LCD_PrnOnOff
	rgoto	CS_M_MIDICFG_Tick_Cont


CS_M_MIDICFG_Tick_C
CS_M_MIDICFG_Tick_C_S
	movlw	0xc0 + 0
	call	MIOS_LCD_CursorSet
	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_C_T

	;; print source name
	movf	CS_ROUTER_SOURCE, W
	call	SEQ_ROUTER_PrnIn

CS_M_MIDICFG_Tick_C_T
	movlw	0xc0 + 5
	call	MIOS_LCD_CursorSet
	movlw	5		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	;; print target name
	movf	CS_ROUTER_TARGET, W
	call	SEQ_ROUTER_PrnOut
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_D
	movlw	0xc0 + 13
	call	MIOS_LCD_CursorSet
	movlw	6		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	;; print source channel
	swapf	CS_ROUTER_SOURCE, W
	iorwf	CS_ROUTER_TARGET, W
	call	SEQ_ROUTER_PrnSrcChn
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_E
	movlw	0xc0 + 23
	call	MIOS_LCD_CursorSet
	movlw	7		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	;; print destination channel
	swapf	CS_ROUTER_SOURCE, W
	iorwf	CS_ROUTER_TARGET, W
	call	SEQ_ROUTER_PrnDstChn
	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_F
	movlw	0xc0 + 33
	call	MIOS_LCD_CursorSet
	movlw	8		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_MIDICFG_Tick_Cont

	;; print midi clock forwarding on/off flag
	movf	CS_ROUTER_TARGET, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	andwf	SEQ_ROUTER_MCLK_ENABLED, W, BANKED
	movlw	0
	skpz
	movlw	1
	call	SEQ_LCD_PrnOnOff
	;; 	rgoto	CS_M_MIDICFG_Tick_Cont

CS_M_MIDICFG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_MIDICFG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_MIDICFG_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MIDICFG_KeybChnChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MIDI_CHANNEL
	return

CS_M_MIDICFG_TA_SplitChanged
	SET_BSR	SEQ_BASE
	bcf	SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_CFG0, SEQ_CFG0_T_A_SPLIT, BANKED
	return

CS_M_MIDICFG_MiddNoteChanged
	movff	CS_MENU_PARAMETER_L, SEQ_MIDI_SPLIT_NOTE
	return

CS_M_MIDICFG_MergerChanged
	SET_BSR	SEQ_BASE
	bcf	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED

CS_M_MIDICFG_MergerChanged_Quer
	;; set merger function and exit
	SET_BSR	SEQ_BASE
	movlw	MIOS_MIDI_MERGER_DISABLED
	btfsc	SEQ_CFG0, SEQ_CFG0_MERGER, BANKED
	movlw MIOS_MIDI_MERGER_ENABLED
	goto	MIOS_MIDI_MergerSet

CS_M_MIDICFG_RSourceChanged
	movff	CS_MENU_PARAMETER_L, CS_ROUTER_SOURCE
	return

CS_M_MIDICFG_RTargetChanged
	movff	CS_MENU_PARAMETER_L, CS_ROUTER_TARGET
	return

CS_M_MIDICFG_RSrcChnChanged
	lfsr	FSR1, ROUTER_IN0_SRC0_CHN
	btfsc	CS_ROUTER_SOURCE, 0
	lfsr	FSR1, ROUTER_IN1_SRC0_CHN

	movf	CS_ROUTER_TARGET, W
	movff	CS_MENU_PARAMETER_L, PLUSW1
	return

CS_M_MIDICFG_RDstChnChanged
	lfsr	FSR1, ROUTER_IN0_DST0_CHN
	btfsc	CS_ROUTER_SOURCE, 0
	lfsr	FSR1, ROUTER_IN1_DST0_CHN

	movf	CS_ROUTER_TARGET, W
	movff	CS_MENU_PARAMETER_L, PLUSW1
	return

CS_M_MIDICFG_RMClkChanged
	SET_BSR	SEQ_BASE
	movf	CS_ROUTER_TARGET, W
	BRA_IFSET CS_MENU_PARAMETER_L, 0, ACCESS, CS_M_MIDICFG_RMClkChanged_1
CS_M_MIDICFG_RMClkChanged_0
	call	MIOS_HLP_GetBitANDMask
	andwf	SEQ_ROUTER_MCLK_ENABLED, F, BANKED
	return
CS_M_MIDICFG_RMClkChanged_1
	call	MIOS_HLP_GetBitORMask
	iorwf	SEQ_ROUTER_MCLK_ENABLED, F, BANKED
	return
