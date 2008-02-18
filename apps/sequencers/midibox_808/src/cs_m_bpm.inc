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
CS_M_BPM_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; prepare BSR
	SET_BSR	SEQ_BASE

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_BPM_GP_Callback_0
	rgoto	CS_M_BPM_GP_Callback_1
	rgoto	CS_M_BPM_GP_Callback_2
	rgoto	CS_M_BPM_GP_Callback_3
	rgoto	CS_M_BPM_GP_Callback_4
	rgoto	CS_M_BPM_GP_Callback_5
	rgoto	CS_M_BPM_GP_Callback_6
	rgoto	CS_M_BPM_GP_Callback_7
	rgoto	CS_M_BPM_GP_Callback_8
	rgoto	CS_M_BPM_GP_Callback_9
	rgoto	CS_M_BPM_GP_Callback_A
	rgoto	CS_M_BPM_GP_Callback_B
	rgoto	CS_M_BPM_GP_Callback_C
	rgoto	CS_M_BPM_GP_Callback_D
	rgoto	CS_M_BPM_GP_Callback_E
	rgoto	CS_M_BPM_GP_Callback_F

CS_M_BPM_GP_Callback_0		; Master Mode
	movlw	0x00
	call	SEQ_BPM_ModeSet
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_1		; Slave Mode
	movlw	0x01
	call	SEQ_BPM_ModeSet
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_2		; Auto Mode
	movlw	0x02
	call	SEQ_BPM_ModeSet
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_3		; reserved
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_4		; reserved
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_5		; reserved
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_6		; reserved
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_7		; reserved
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_8		; Internal Divider
CS_M_BPM_GP_Callback_9
CS_M_BPM_GP_Callback_A
CS_M_BPM_GP_Callback_B
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_CORE_INT_CLK_DIVIDER_PRELOAD, BANKED
	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_C		; External Divider
CS_M_BPM_GP_Callback_D		; reserved
CS_M_BPM_GP_Callback_E		; reserved
CS_M_BPM_GP_Callback_F		; reserved
	movf	MIOS_PARAMETER1, W
	andlw	0x03
	movwf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, BANKED
	;; 	rgoto	CS_M_BPM_GP_Callback_Cont

CS_M_BPM_GP_Callback_Cont
	;; update LEDs
	rcall	CS_M_BPM_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_BPM_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; copy flags to LED positions
	SET_BSR	SEQ_BASE
	movlw	0x01
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED
	movlw	0x02
	btfsc	SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED
	movlw	0x04
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

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
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_BPM_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_BPM_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_BPM_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    BPM  >120<    ClkSync: Master        Global Clock Dividers (Int./Ext.)    
	;; 00000001111111111111222222222222222233334444444444555555555566666666667777777777
	;;  Mst  Slv  Auto                           1    2    4    8    2    4    8   16  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    BPM  >120<
	;; 0000000111111111
	;; ClkSync: Master
	;; 2222222222222222


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_BPM_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_BPM_Tick_0
	rgoto	CS_M_BPM_Tick_1
	rgoto	CS_M_BPM_Tick_2
	rgoto	CS_M_BPM_Tick_3
	rgoto	CS_M_BPM_Tick_4
	rgoto	CS_M_BPM_Tick_5
	rgoto	CS_M_BPM_Tick_6
	rgoto	CS_M_BPM_Tick_7

CS_M_BPM_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_1_STR	STRING 14, 0x00 + 6, "BPM  >xxx<    "
CS_M_BPM_Tick_1
	TABLE_ADDR CS_M_BPM_Tick_1_STR
	call	MIOS_LCD_PrintString

	;; print BPM
	movlw	0x00 + 12
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_AUTO, BANKED, CS_M_BPM_Tick_1_Master
	BRA_IFSET SEQ_CFG0, SEQ_CFG0_BPM_CLK_SLAVE, BANKED, CS_M_BPM_Tick_1_Slave
CS_M_BPM_Tick_1_Master
	call	SEQ_BPM_Get
	call	MIOS_LCD_PrintBCD3
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_1_Slave
	movlw	'-'
	movwf	PRODL
	movlw	3
	call	SEQ_LCD_PrintChars
	rgoto	CS_M_BPM_Tick_Cont


CS_M_BPM_Tick_2_TABLE
	db	"ClkSync: Master "
	db	"ClkSync: Slave  "
	db	"ClkSync: Auto   "
	db	"                "
	db	"                "
	db	"                "
	db	"                "
	db	"                "
	db	"Internal Clk 1:1"
	db	"Internal Clk 1:2"
	db	"Internal Clk 1:4"
	db	"Internal Clk 1:8"
	db	"External Clk 2:1"
	db	"External Clk 4:1"
	db	"External Clk 8:1"
	db	"External Clk16:1"

CS_M_BPM_Tick_2
	;; print last option
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 20
#else
	movlw	0x40 + 0
#endif
	call	MIOS_LCD_CursorSet

	movf	CS_MENU_CURSOR_POS, W
	TABLE_ADDR_MUL_W CS_M_BPM_Tick_2_TABLE, 16
	movlw	16
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_BPM_Tick_Cont

CS_M_BPM_Tick_3
#if DEFAULT_LCD_WIDTH == 80
	;; print 4 spaces
	movlw	0x00 + 36
	call	MIOS_LCD_CursorSet

	movlw	4
	call	SEQ_LCD_PrintSpaces
#endif
	rgoto	CS_M_BPM_Tick_Cont


CS_M_BPM_Tick_4567_STR
	db	"   Global Clock Dividers (Int./Ext.)    "
CS_M_BPM_Tick_4
CS_M_BPM_Tick_5
CS_M_BPM_Tick_6
CS_M_BPM_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	TABLE_ADDR_MUL_W CS_M_BPM_Tick_4567_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	rgoto	CS_M_BPM_Tick_Cont


CS_M_BPM_Tick_89ABCDEF_STR
	db	" Mst  Slv  Auto                           1    2    4    8    2    4    8   16  "
CS_M_BPM_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_BPM_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_BPM_Tick_Cont


CS_M_BPM_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_BPM_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_BPM_Tick_End
	return

;; ==========================================================================
