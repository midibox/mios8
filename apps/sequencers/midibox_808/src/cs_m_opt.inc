; $Id$
;
; MBSEQ Menu "Options"
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
CS_M_OPT_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_OPT_GP_Callback_0
	rgoto	CS_M_OPT_GP_Callback_1
	rgoto	CS_M_OPT_GP_Callback_2
	rgoto	CS_M_OPT_GP_Callback_3
	rgoto	CS_M_OPT_GP_Callback_4
	rgoto	CS_M_OPT_GP_Callback_5
	rgoto	CS_M_OPT_GP_Callback_6
	rgoto	CS_M_OPT_GP_Callback_7
	rgoto	CS_M_OPT_GP_Callback_8
	rgoto	CS_M_OPT_GP_Callback_9
	rgoto	CS_M_OPT_GP_Callback_A
	rgoto	CS_M_OPT_GP_Callback_B
	rgoto	CS_M_OPT_GP_Callback_C
	rgoto	CS_M_OPT_GP_Callback_D
	rgoto	CS_M_OPT_GP_Callback_E
	rgoto	CS_M_OPT_GP_Callback_F

CS_M_OPT_GP_Callback_0		; Synch To Measure
	;; toggle flag
	SET_BSR	SEQ_BASE
	btg	SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_1		; Follow Pattern/Song
	SET_BSR	SEQ_BASE
	btg	SEQ_CFG0, SEQ_CFG0_FOLLOW_SONG, BANKED
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_2		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_3		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_4		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_5		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_6		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_7		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_8		; Send Pattern Changes via Program Change
	SET_BSR	SEQ_BASE
	btg	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_PTN, BANKED
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_9		; Send Song Changes via MIDI CC
	SET_BSR	SEQ_BASE
	btg	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_SONG, BANKED
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_A		; Send Mute Changes via MIDI CC
	SET_BSR	SEQ_BASE
	btg	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_MUTE, BANKED
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_B		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_C		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_D		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_E		; reserved
	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_F		; reserved
	;; 	rgoto	CS_M_OPT_GP_Callback_Cont

CS_M_OPT_GP_Callback_Cont
	;; update LEDs
	rcall	CS_M_OPT_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_OPT_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; copy flags to LED positions
	SET_BSR	SEQ_BASE
	btfsc	SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0
	btfsc	SEQ_CFG0, SEQ_CFG0_FOLLOW_SONG, BANKED
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 1
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_PTN, BANKED
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR1, 0
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_SONG, BANKED
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR1, 1
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_MUTE, BANKED
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR1, 2

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_OPT_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_OPT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_OPT_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_OPT_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    OPTIONS       SynchToMeas: on                                                
	;; 00000001111111111111222222222222233333334444444444555555555566666666667777777777
	;;  STM Follow                              Ptn  Sng  Mute (MIDI Change)           
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD     OPTIONS  
	;; 0000000111111111
	;; SynchToMeas: on 
	;; 2222222222333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_OPT_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_OPT_Tick_0
	rgoto	CS_M_OPT_Tick_1
	rgoto	CS_M_OPT_Tick_2
	rgoto	CS_M_OPT_Tick_3
	rgoto	CS_M_OPT_Tick_4
	rgoto	CS_M_OPT_Tick_5
	rgoto	CS_M_OPT_Tick_6
	rgoto	CS_M_OPT_Tick_7

CS_M_OPT_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	SEQ_TRK_PrnName	; (6 characters)

	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_1_STR	STRING 14, 0x00 + 6, "OPTIONS       "
CS_M_OPT_Tick_1
	TABLE_ADDR CS_M_OPT_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_2_TABLE
	db	"SynchToMeas:"
	db	"Follow Song:"
	db	"            "
	db	"            "
	db	"            "
	db	"            "
	db	"            "
	db	"            "
	db	"SendPtnMIDI:"
	db	"SendSngMIDI:"
	db	"SendMteMIDI:"
	db	"            "
	db	"            "
	db	"            "
	db	"            "
	db	"            "

CS_M_OPT_Tick_2
	;; print last option
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 20
#else
	movlw	0x40 + 0
#endif
	call	MIOS_LCD_CursorSet

	movf	CS_MENU_CURSOR_POS, W
	TABLE_ADDR_MUL_W CS_M_OPT_Tick_2_TABLE, 12
	movlw	12
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_3
	;; print on/off depending on flag
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 32
#else
	movlw	0x40 + 12
#endif
	call	MIOS_LCD_CursorSet

	movlw	' '
	call	MIOS_LCD_PrintChar

	;; prepare BSR
	SET_BSR	SEQ_BASE

	;; branch depending on cursor position
	movf	CS_MENU_CURSOR_POS, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_OPT_Tick_3_0
	rgoto	CS_M_OPT_Tick_3_1
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_8
	rgoto	CS_M_OPT_Tick_3_9
	rgoto	CS_M_OPT_Tick_3_A
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv
	rgoto	CS_M_OPT_Tick_3_Inv

CS_M_OPT_Tick_3_0
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_PATTERN_SYNCH, BANKED
	movlw	0x01
	rgoto	CS_M_OPT_Tick_3_Cont

CS_M_OPT_Tick_3_1
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_FOLLOW_SONG, BANKED
	movlw	0x01
	rgoto	CS_M_OPT_Tick_3_Cont

CS_M_OPT_Tick_3_8
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_PTN, BANKED
	movlw	0x01
	rgoto	CS_M_OPT_Tick_3_Cont

CS_M_OPT_Tick_3_9
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_SONG, BANKED
	movlw	0x01
	rgoto	CS_M_OPT_Tick_3_Cont

CS_M_OPT_Tick_3_A
	movlw	0x00
	btfsc	SEQ_CFG0, SEQ_CFG0_SEND_MIDI_MUTE, BANKED
	movlw	0x01
	rgoto	CS_M_OPT_Tick_3_Cont

CS_M_OPT_Tick_3_Cont
	;; print on/off
	call	SEQ_LCD_PrnOnOff
	;; print 4 spaces
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_3_Inv
	;; invalid selection - print 7 spaces
	movlw	7
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_4
CS_M_OPT_Tick_5
CS_M_OPT_Tick_6
CS_M_OPT_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_OPT_Tick_Cont

CS_M_OPT_Tick_89ABCDEF_STR
	db	" STM Follow                              Ptn  Sng  Mute (MIDI Change)           "
CS_M_OPT_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_OPT_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_OPT_Tick_Cont


CS_M_OPT_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_OPT_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_OPT_Tick_End
	return

;; ==========================================================================
