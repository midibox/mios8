; $Id$
;
; MBSEQ Menu "Song"
;
; Activate this #define to measure the performance with a scope
; (connect the probe to RC.5)
#define SONG_MENU_MEASURE_PERFORMANCE 0
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
CS_M_SONG_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; if shift button pressed: change to song 17-32, else song 1-16
	SET_BSR	SEQ_BASE
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	addlw	0x10
	movwf	SEQ_SONG, BANKED
	;; (this also clears the "song disabled" bit #7)

	;; update LEDs
	rcall	CS_M_SONG_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_SONG_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; check if song within displayed range
	SET_BSR	SEQ_BASE
	movlw	0x00
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	movlw	0x10
	xorwf	SEQ_SONG, W, BANKED
	andlw	0x10
	bnz	CS_M_SONG_UpdateGPLEDs_End

	;; display song number
	movf	SEQ_SONG, W, BANKED
	call	MIOS_HLP_GetBitORMask
	btfss	SEQ_SONG, 3, BANKED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	SEQ_SONG, 3, BANKED
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; if song enabled: don't flash LEDs!
	BRA_IFSET SEQ_SONG, 7, BANKED, CS_M_SONG_UpdateGPLEDs_End
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
CS_M_SONG_UpdateGPLEDs_End
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SONG_Init
	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_SONG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; update LEDs
	rgoto	CS_M_SONG_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SONG_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; S:xx/xxxxx  1:A1    ________________
	;; 00000000001111111111222222222222222233334444444444555555555566666666667777777777
	;;   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16  
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; S:xx/xxxxx  1:A1
	;; 0000000000111111
	;; ________________
	;; 2222222222222222

	;; if fast display update, and counter < 0x10, set it to 0x10
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_SONG_Tick_NotFast
CS_M_SONG_Tick_Fast
	movlw	0x10
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_SONG_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_SONG_Tick_NotCtrlFix
CS_M_SONG_Tick_NotFast

	;; print meter segments 16-31?
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SONG_Tick_Meters

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_SONG_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SONG_Tick_0
	rgoto	CS_M_SONG_Tick_1
	rgoto	CS_M_SONG_Tick_2
	rgoto	CS_M_SONG_Tick_3
	rgoto	CS_M_SONG_Tick_4
	rgoto	CS_M_SONG_Tick_5
	rgoto	CS_M_SONG_Tick_6
	rgoto	CS_M_SONG_Tick_7

CS_M_SONG_Tick_0		; re-used from CS_M_MUTE_Init !!!
	;; print song number and position
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	movlw	'S'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, CS_M_SONG_Tick_0_S_Dis
CS_M_SONG_Tick_0_S_NotDis
	movf	SEQ_SONG, W
	andlw	0x7f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_SONG_Tick_0_S_Cont
CS_M_SONG_Tick_0_S_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
CS_M_SONG_Tick_0_S_Cont

	movlw	'/'
	call	MIOS_LCD_PrintChar

	;; song pointer padded with zeros
	clrf	MIOS_PARAMETER1
	movlw	0x00
	call	MIOS_HLP_Dec2BCD
	movf	MIOS_PARAMETER3, W
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER2, W
	call	MIOS_LCD_PrintHex2
	movf	MIOS_PARAMETER1, W
	call	MIOS_LCD_PrintHex2

	rgoto	CS_M_SONG_Tick_Cont


CS_M_SONG_Tick_1
	;; print currently played pattern
	movlw	0x00 + 10
	call	MIOS_LCD_CursorSet

	;; 2 spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces

	;; pattern name (5 characters)
	SET_BSR	SEQ_BASE
	incf	SEQ_NEXT_PATTERN0, W, BANKED
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_PATTERN_PrnP

	;; 3 additional spaces
	movlw	3
	call	SEQ_LCD_PrintSpaces

	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_2
	;; nothing to do... (normaly filled by meters)

	;; time to init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_3
	;; print remaining 4 spaces
#if DEFAULT_LCD_WIDTH == 80
	movlw	0x00 + 36
#else
	movlw	0x40 + 16
#endif
	call	MIOS_LCD_CursorSet
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_4
CS_M_SONG_Tick_5
CS_M_SONG_Tick_6
CS_M_SONG_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; set cursor depending on current segment
	call	CS_M_HLP_SetSegmentCursor

	;; print two song numbers (depends on shift button)
	SET_BSR	SEQ_BASE
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-8
	rlf	WREG, W
	andlw	0x0e
	btfsc	SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED
	addlw	0x10
	movwf	TMP1

	movlw	1
	call	SEQ_LCD_PrintSpaces
	incf	TMP1, W
	call	MIOS_LCD_PrintBCD2
	movlw	2
	call	SEQ_LCD_PrintSpaces
	
	movlw	1
	call	SEQ_LCD_PrintSpaces
	incf	TMP1, W
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	2
	call	SEQ_LCD_PrintSpaces	
#endif


CS_M_SONG_Tick_Meters
	;; set cursor
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
#if DEFAULT_LCD_WIDTH == 80
	addlw	0x00 + 20
#else
	addlw	0x40 + 0
#endif
	call	MIOS_LCD_CursorSet

	;; print meter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	call	CS_M_PATTERN_PrnM

	;;  	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_Cont
	;; Tick function will be called 32 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_SONG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_SONG_Tick_End

CS_M_SONG_Tick_End
	return

;; ==========================================================================
