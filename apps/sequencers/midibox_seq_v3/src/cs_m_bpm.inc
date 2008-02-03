; $Id$
;
; MBSEQ Menu "BPM"
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
CS_M_BPM_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					  0x02, 0x40+ 8, 6,	CS_M_BPM_ModeChanged
 CS_MENU_PARAMETER_ENTRY -1,					255-48, 0x40+28, 3,	CS_M_BPM_BPMChanged
 CS_MENU_PARAMETER_ENTRY -1,					  0x03,      -1, 1,	CS_M_BPM_IntDivChanged
 CS_MENU_PARAMETER_ENTRY -1,					  0x03,      -1, 1,	CS_M_BPM_ExtDivChanged


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
CS_M_BPM_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	SET_BSR	SEQ_BASE

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_BPM_GP_Callback_1
	rgoto	CS_M_BPM_GP_Callback_2
	rgoto	CS_M_BPM_GP_Callback_3
	rgoto	CS_M_BPM_GP_Callback_4
	rgoto	CS_M_BPM_GP_Callback_5
	rgoto	CS_M_BPM_GP_Callback_6
	rgoto	CS_M_BPM_GP_Callback_7
	rgoto	CS_M_BPM_GP_Callback_8
	rgoto	CS_M_BPM_GP_Callback_9
	rgoto	CS_M_BPM_GP_Callback_10
	rgoto	CS_M_BPM_GP_Callback_11
	rgoto	CS_M_BPM_GP_Callback_12
	rgoto	CS_M_BPM_GP_Callback_13
	rgoto	CS_M_BPM_GP_Callback_14
	rgoto	CS_M_BPM_GP_Callback_15
	rgoto	CS_M_BPM_GP_Callback_16

CS_M_BPM_GP_Callback_1
CS_M_BPM_GP_Callback_2
CS_M_BPM_GP_Callback_3
CS_M_BPM_GP_Callback_4
	clrf	CS_MENU_CURSOR_POS	; select Master/Slave (Item #0)
	clrf	CS_MENU_ENC_INCREMENTER	; (toggle)
	movlw	1
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE
	movlw 2
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO
	movlw 0
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_BPM_ModeChanged
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_5
CS_M_BPM_GP_Callback_6
CS_M_BPM_GP_Callback_7
CS_M_BPM_GP_Callback_8
	movlw	1	; select BPM (item #1)
	movwf	CS_MENU_CURSOR_POS
	call	SEQ_BPM_Get
	addlw	-48
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_9
CS_M_BPM_GP_Callback_10
CS_M_BPM_GP_Callback_11
CS_M_BPM_GP_Callback_12
	movlw	2			; select internal divider (item #2)
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_ENC_INCREMENTER	; (toggle)
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, BANKED
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_13
CS_M_BPM_GP_Callback_14
CS_M_BPM_GP_Callback_15
CS_M_BPM_GP_Callback_16
	movlw	3			; select external divider (item #3)
	movwf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_ENC_INCREMENTER	; (toggle)
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, BANKED
	;; 	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_Cont

	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_BPM_UpdateGPLEDs
	SET_BSR	SEQ_BASE

	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; left side: first 4 LEDs active if BPM clock mode selected (cursor pos 0)
	;;            second 4 LEDs active if BPM selected (cursor pos 1)
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_BPM_UpdateGPLEDs_0
	addlw	-1
	bz	CS_M_BPM_UpdateGPLEDs_1
	rgoto	CS_M_BPM_UpdateGPLEDs_23

CS_M_BPM_UpdateGPLEDs_0
	movlw	0x0f
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
 	rgoto	CS_M_BPM_UpdateGPLEDs_Cont

CS_M_BPM_UpdateGPLEDs_1
	movlw	0xf0
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
 	rgoto	CS_M_BPM_UpdateGPLEDs_Cont

CS_M_BPM_UpdateGPLEDs_23
	;; 	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT	; flash LEDs at right side
	;; (not consistent, all two items will flash)
	;; 	rgoto	CS_M_BPM_UpdateGPLEDs_Cont

CS_M_BPM_UpdateGPLEDs_Cont

	;; right side: display the Int/Ext divider status
	movf	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	movf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, W, BANKED
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_BPM_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_BPM_PARAMETERS		; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy value into menu parameter depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_BPM_Init_Cursor0
	addlw	-1
	bz	CS_M_BPM_Init_Cursor1
	addlw	-1
	bz	CS_M_BPM_Init_Cursor2
	rgoto	CS_M_BPM_Init_Cursor3

CS_M_BPM_Init_Cursor0		; clock mode
	SET_BSR	SEQ_BASE
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	movlw 0x01
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED
	movlw 0x02
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_BPM_Init_Cont
	
CS_M_BPM_Init_Cursor1		; BPM
	call	SEQ_BPM_Get
	addlw	-48
	movwf	CS_MENU_PARAMETER_L
	rgoto	CS_M_BPM_Init_Cont

CS_M_BPM_Init_Cursor2		; int. divider
	movff	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, CS_MENU_PARAMETER_L
	rgoto	CS_M_BPM_Init_Cont

CS_M_BPM_Init_Cursor3		; ext. divider
	movff	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, CS_MENU_PARAMETER_L
	;; 	rgoto	CS_M_BPM_Init_Cont

CS_M_BPM_Init_Cont

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_BPM_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_BPM_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_BPM_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;;     BPM Clock Mode    Beats per Minute     Global Clock Dividers (Int./Ext.)
	;; ----00000000000000----1111111111111111-----222222222222222222222-33333333333----
	;;         Master              140           1    2    4    8    2    4    8   16
	;; --------444444--------------555---------8888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_BPM_Tick_0
	rgoto	CS_M_BPM_Tick_1
	rgoto	CS_M_BPM_Tick_2
	rgoto	CS_M_BPM_Tick_3
	rgoto	CS_M_BPM_Tick_4
	rgoto	CS_M_BPM_Tick_5
	rgoto	CS_M_BPM_Tick_6
	rgoto	CS_M_BPM_Tick_7
	rgoto	CS_M_BPM_Tick_8
	rgoto	CS_M_BPM_Tick_9
	rgoto	CS_M_BPM_Tick_A
	rgoto	CS_M_BPM_Tick_B
	rgoto	CS_M_BPM_Tick_C
	rgoto	CS_M_BPM_Tick_D
	rgoto	CS_M_BPM_Tick_E
	rgoto	CS_M_BPM_Tick_F

CS_M_BPM_Tick_0_STR	STRING 14, 0x00 + 4, "BPM Clock Mode"
CS_M_BPM_Tick_0
	TABLE_ADDR CS_M_BPM_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_1_STR	STRING 16, 0x00 + 22, "Beats per Minute"
CS_M_BPM_Tick_1
	TABLE_ADDR CS_M_BPM_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_2_STR	STRING 21, 0x80 + 3, "Global Clock Dividers"
CS_M_BPM_Tick_2
	TABLE_ADDR CS_M_BPM_Tick_2_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_3_STR	STRING 11, 0x80 + 25, "(Int./Ext.)"
CS_M_BPM_Tick_3
	TABLE_ADDR CS_M_BPM_Tick_3_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_4_TXT0	db	"Master"
CS_M_BPM_Tick_4_TXT1	db	"Slave "
CS_M_BPM_Tick_4_TXT2	db	"Auto  "

CS_M_BPM_Tick_4
	movlw	0x40 + 8
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_BPM_Tick_Cont

	SET_BSR	SEQ_BASE
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	movlw 1*0x06
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED
	movlw 2*0x06
	addlw	LOW(CS_M_BPM_Tick_4_TXT0)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_BPM_Tick_4_TXT0)
	addwfc	TBLPTRH, F

	movlw	6	; characters
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_5
	movlw	0x40 + 28
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_BPM_Tick_Cont

	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, CS_M_BPM_Tick_5_Master
	BRA_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, CS_M_BPM_Tick_5_Slave
CS_M_BPM_Tick_5_Master
	call	SEQ_BPM_Get
	call	MIOS_LCD_PrintBCD3
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_5_Slave
	movlw	'-'
	movwf	PRODL
	movlw	3
	call	SEQ_LCD_PrintChars
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_6
CS_M_BPM_Tick_7
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_8
CS_M_BPM_Tick_9
CS_M_BPM_Tick_A
CS_M_BPM_Tick_B
	;; current value -> TMP1
	movff	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, TMP1

	;; menu cursor pos -> TMP2
	movlw	2
	movwf	TMP2

	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	5
	movf	PRODL, W
	addlw	0xc0 + 1
	rgoto	CS_M_BPM_Tick_Div_Print

CS_M_BPM_Tick_C
CS_M_BPM_Tick_D
CS_M_BPM_Tick_E
CS_M_BPM_Tick_F
	;; current value -> TMP1
	movff	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, TMP1

	;; menu cursor pos -> TMP2
	movlw	3
	movwf	TMP2

	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	5
	movf	PRODL, W
	addlw	0xc0 + 20 + 1
	;; 	rgoto	CS_M_BPM_Tick_Div_Print

CS_M_BPM_Tick_Div_Print
	call	MIOS_LCD_CursorSet

	;; item selected -> TMP3.0
	setf	TMP3
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	cpfseq	TMP1, ACCESS
	clrf TMP3

	;; print left marker
	movlw	'>'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; check if item should be flashed
	movf	CS_MENU_CURSOR_POS, W
	cpfseq	TMP2, ACCESS
	rgoto CS_M_BPM_Tick_Div_NoF
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_BPM_Tick_Div_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_BPM_Tick_Div_Spaces

CS_M_BPM_Tick_Div_NoF
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 2, ACCESS, CS_M_BPM_Tick_Div_NoF_Ext
CS_M_BPM_Tick_Div_NoF_Int
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	call	MIOS_HLP_GetBitORMask
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_BPM_Tick_Div_Cont

CS_M_BPM_Tick_Div_NoF_Ext
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	addlw	1
	call	MIOS_HLP_GetBitORMask
	BRA_IFSET WREG, 4, ACCESS, CS_M_BPM_Tick_Div_NoF_Ext_BCD2

CS_M_BPM_Tick_Div_NoF_Ext_BCD1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_BPM_Tick_Div_Cont

CS_M_BPM_Tick_Div_NoF_Ext_BCD2
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_BPM_Tick_Div_Cont

CS_M_BPM_Tick_Div_Spaces
	movlw	1
	call	SEQ_LCD_PrintSpaces
	;; 	rgoto	CS_M_BPM_Tick_Div_Cont

CS_M_BPM_Tick_Div_Cont
	;; print right marker
	movlw	'<'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

CS_M_BPM_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_BPM_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_BPM_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_BPM_ModeChanged
	;; set new clock mode
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	goto	SEQ_BPM_ModeSet

CS_M_BPM_BPMChanged
	;; set new BPM
	movf	CS_MENU_PARAMETER_L, W
	addlw	48
	goto	SEQ_BPM_Set

CS_M_BPM_IntDivChanged
	movff	CS_MENU_PARAMETER_L, SEQ_CORE_INT_CLK_DIVIDER_PRELOAD
	rgoto	CS_M_BPM_UpdateGPLEDs

CS_M_BPM_ExtDivChanged
	movff	CS_MENU_PARAMETER_L, SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD
	rgoto	CS_M_BPM_UpdateGPLEDs


;; ==========================================================================
