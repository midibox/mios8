; $Id$
;
; MBSEQ Menu for SysEx dumps
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
CS_M_SYSEX_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; set new device ID
	movff	MIOS_PARAMETER1, MIDI_DEVICE_ID

	;; dump target pattern/bank always current one
	movff	SEQ_PATTERN0, SEQ_PATTERN
	movff	SEQ_PATTERN_BANK0, SEQ_PATTERN_BANK

	;; RAM dump
	SET_BSR	SEQ_BASE
	movlw	0x80
	movwf	SEQ_PATTERN, BANKED

	;; send dump
	call	SEQ_SYSEX_SendDump

	;; update LEDs
	rcall	CS_M_SYSEX_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_SYSEX_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0_A
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1_A

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1-16: MIDI device ID (masked by 0x0f)
	movf	MIDI_DEVICE_ID, W
	call	MIOS_HLP_GetBitORMask
	btfss	MIDI_DEVICE_ID, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0_A
	btfsc	MIDI_DEVICE_ID, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1_A

	;; copy A into B LED layer (no dimming used)
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0_A, CS_MENU_GP_OVERLAY_DOUT_SR0_B
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1_A, CS_MENU_GP_OVERLAY_DOUT_SR1_B

	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SYSEX_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_SYSEX_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_SYSEX_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SYSEX_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    SYSEX         Device ID: 00                                               
	;; 00000001111111111111222222222222233333334444444444555555555566666666667777777777
	;;   00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F 
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    SYSEX     
	;; 0000000111111111
	;; Device ID: 00   
	;; 2222222222222333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_SYSEX_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SYSEX_Tick_0
	rgoto	CS_M_SYSEX_Tick_1
	rgoto	CS_M_SYSEX_Tick_2
	rgoto	CS_M_SYSEX_Tick_3
	rgoto	CS_M_SYSEX_Tick_4
	rgoto	CS_M_SYSEX_Tick_5
	rgoto	CS_M_SYSEX_Tick_6
	rgoto	CS_M_SYSEX_Tick_7

CS_M_SYSEX_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x00 + 6, "SYSEX         "
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_2
	;; print device ID in hex-format
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	11, 0x00 + 20, "Device ID: "
#else
	STRING	11, 0x40 + 00, "Device ID: "
#endif

	movf	MIDI_DEVICE_ID, W
	call	MIOS_LCD_PrintHex2
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_3
	;; print 7 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 33
#else
	movlw	0x40 + 13
#endif
	call	MIOS_LCD_CursorSet
	movlw	7
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_4
CS_M_SYSEX_Tick_5
CS_M_SYSEX_Tick_6
CS_M_SYSEX_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_SYSEX_Tick_Cont

CS_M_SYSEX_Tick_89ABCDEF_STR
	db	"  00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F "
CS_M_SYSEX_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_SYSEX_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_SYSEX_Tick_Cont


CS_M_SYSEX_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SYSEX_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_SYSEX_Tick_End
	return

;; ==========================================================================
