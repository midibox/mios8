; $Id$
;
; MBSEQ Menu "Mixer"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; used "In-Menu" messages
CS_M_MIXER_MSG_DEFAULT	EQU	0x00
CS_M_MIXER_MSG_COPY	EQU	0x91
CS_M_MIXER_MSG_PASTE	EQU	0x92
CS_M_MIXER_MSG_CLEAR	EQU	0x93
CS_M_MIXER_MSG_LOAD	EQU	0x94
CS_M_MIXER_MSG_SAVE	EQU	0x95
CS_M_MIXER_MSG_DUMP	EQU	0x96

CS_M_MIXER_TICK_IN_MENU_MSGS
	db	">COPIED<"	; #1
	db	">PASTED<"	; #2
	db	"CLEARED!"	; #3
	db	">>LOAD<<"	; #4
	db	">>SAVE<<"	; #5
	db	">>DUMP<<"	; #6

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_MIXER_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x06,   0x40+ 0,  4,	CS_M_MIXER_PageChanged

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
CS_M_MIXER_GP_Callback
	;; branch depending on CS_MIXER_PAGE[7] (utility page)
	BRA_IFCLR CS_MIXER_PAGE, 7, ACCESS, CS_M_MIXER_GP_Callback_N

	;; ------------------------------------------------------------------
	;; utility page
CS_M_MIXER_GP_Callback_U

	;; if button number != 0, ignore all encoder movements
	movf	MIOS_PARAMETER1, W
	bz	CS_M_MIXER_GP_Callback_U_EncOk
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_MIXER_GP_Callback_U_ButtnOk
CS_M_MIXER_GP_Callback_U_EncIgn
	rgoto	CS_M_MIXER_GP_Callback_End
CS_M_MIXER_GP_Callback_U_ButtnOk
CS_M_MIXER_GP_Callback_U_EncOk

	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_MIXER_GP_Callback_U_BSet
CS_M_MIXER_GP_Callback_U_BClr
	bcf	CS_IN_MENU_MSG, 7
	rgoto	CS_M_MIXER_GP_Callback_End

CS_M_MIXER_GP_Callback_U_BSet
	;; branch depending on button 1-8
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_MIXER_GP_Callback_1
	rgoto	CS_M_MIXER_GP_Callback_2
	rgoto	CS_M_MIXER_GP_Callback_3
	rgoto	CS_M_MIXER_GP_Callback_4
	rgoto	CS_M_MIXER_GP_Callback_5
	rgoto	CS_M_MIXER_GP_Callback_6
	rgoto	CS_M_MIXER_GP_Callback_7
	rgoto	CS_M_MIXER_GP_Callback_8
	rgoto	CS_M_MIXER_GP_Callback_9
	rgoto	CS_M_MIXER_GP_Callback_10
	rgoto	CS_M_MIXER_GP_Callback_11
	rgoto	CS_M_MIXER_GP_Callback_12
	rgoto	CS_M_MIXER_GP_Callback_13
	rgoto	CS_M_MIXER_GP_Callback_14
	rgoto	CS_M_MIXER_GP_Callback_15
	rgoto	CS_M_MIXER_GP_Callback_16

;; ---[ Map ]--------------------------------------------------
CS_M_MIXER_GP_Callback_1
	;; ignore if button
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_MIXER_GP_Callback_End
	;; store CS_MIXER_MAP in TMP[12]
	movff	CS_MIXER_MAP, TMP1
	clrf	TMP2
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; max value
	movlw	0x7e
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; move incrementer to WREG
	movf	CS_MENU_ENC_INCREMENTER, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, CS_M_MIXER_GP_Callback_End

	;; else copy TMP1 to mixer map, and load the new one
	movff	TMP1, CS_MIXER_MAP
	rcall	CS_M_MIXER_LoadMap
	rgoto	CS_M_MIXER_GP_Callback_End


;; ---[ Copy ]--------------------------------------------------
CS_M_MIXER_GP_Callback_2
	;; copy map
	rcall	CS_M_MIXER_CopyMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_COPY
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ Paste ]--------------------------------------------------
CS_M_MIXER_GP_Callback_3
	;; paste map
	rcall	CS_M_MIXER_PasteMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_PASTE
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ Clear ]--------------------------------------------------
CS_M_MIXER_GP_Callback_4
	;; clear map
	rcall	CS_M_MIXER_ClearMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_CLEAR
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_5
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ Load ]--------------------------------------------------
CS_M_MIXER_GP_Callback_6
	;; load map
	rcall	CS_M_MIXER_LoadMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_LOAD
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ Save ]--------------------------------------------------
CS_M_MIXER_GP_Callback_7
	;; save map
	rcall	CS_M_MIXER_SaveMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_SAVE
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ Dump ]--------------------------------------------------
CS_M_MIXER_GP_Callback_8
	;; dump map
	rcall	CS_M_MIXER_DumpMap
	;; print in-menu message
	movlw	CS_M_MIXER_MSG_DUMP
	movwf	CS_IN_MENU_MSG
	;; and exit
	rgoto	CS_M_MIXER_GP_Callback_End


;; ---[ CC1 Assignment ]--------------------------------------------------
CS_M_MIXER_GP_Callback_9
	;; just switch mixer page
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	movwf	CS_MIXER_PAGE
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ CC2 Assignment ]--------------------------------------------------
CS_M_MIXER_GP_Callback_10
	;; just switch mixer page
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+3
	movwf	CS_MIXER_PAGE
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_11
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_12
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_13
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_14
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_15
	rgoto	CS_M_MIXER_GP_Callback_End

;; ---[ reserved ]--------------------------------------------------
CS_M_MIXER_GP_Callback_16
	rgoto	CS_M_MIXER_GP_Callback_End


	;; ------------------------------------------------------------------
	;; normal page
CS_M_MIXER_GP_Callback_N
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; select new track
	movff	MIOS_PARAMETER1, CS_MIXER_SELECTED_TRK

	;; if encoder has been moved: increment mixer track value
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_MIXER_GP_Callback_End
CS_M_MIXER_GP_Callback_N_Enc
	rcall	CS_M_MIXER_ValueInc
CS_M_MIXER_GP_Callback_End
	;; clear incrementer (no additional encoder action)
	clrf	CS_MENU_ENC_INCREMENTER, W
	;; 	rgoto	CS_M_MIXER_UpdateGPLEDs

;; --------------------------------------------------------------------------
CS_M_MIXER_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; exit if utility page selected
	btfsc	CS_MIXER_PAGE, 7
	return

	;; set LED depending on selected track
	movf	CS_MIXER_SELECTED_TRK, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	btfss	CS_MIXER_SELECTED_TRK, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	CS_MIXER_SELECTED_TRK, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_MIXER_Select
	;; select/deselect CS_MIXER_PAGE[7] (utility page) depending on button
	btfsc	MIOS_PARAMETER2, 0
	bcf	CS_MIXER_PAGE, 7
	btfss	MIOS_PARAMETER2, 0
	bsf	CS_MIXER_PAGE, 7

	;; update LEDs
	rcall	CS_M_MIXER_UpdateGPLEDs

	;; clear refresh counter, so that we restart - if required
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpnz
	return

	clrf	CS_MENU_REFRESH_ITEM_CTR
	rgoto	CS_M_MIXER_Tick


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_MIXER_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_MIXER_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; ignore if utility page selected
	BRA_IFSET CS_MIXER_PAGE, 7, ACCESS, CS_M_MIXER_InitSkipPage
CS_M_MIXER_InitPage
	;; switch back to page 0 if CC assignment page selected (I feel that this behaviour is more logical when switching between EDIT and MIXER)
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	cpfslt	CS_MIXER_PAGE, ACCESS
	clrf CS_MIXER_PAGE
	;; copy menu parameter
	movff	CS_MIXER_PAGE, CS_MENU_PARAMETER_L
	;; call "PageChanged" so that the encoder speed will be updated
	rcall	CS_M_MIXER_PageChanged
CS_M_MIXER_InitSkipPage

	;; we are using the same memory like the UNDO buffer - therefore invalidate it:
	;; and load page if UNDO buffer already full
	BRA_IFCLR CS_STAT2, CS_STAT2_UNDO_FILLED, ACCESS, CS_M_MIXER_InitSkipLoad
CS_M_MIXER_InitLoad
	rcall	CS_M_MIXER_LoadMap
CS_M_MIXER_InitSkipLoad

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_MIXER_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_MIXER_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_MIXER_Tick
	;; branch depending on CS_MIXER_PAGE[7] (utility page)
	BRA_IFCLR CS_MIXER_PAGE, 7, ACCESS, CS_M_MIXER_Tick_Normal

	;; ------------------------------------------------------------------
CS_M_MIXER_Tick_Util
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Map#   Mixer Utility Functions          CC Assignments                          
	;; 00000000001111111122222222222233333333334444444444555555555566666666667777777777
	;; 127  Copy Paste Clr      Load Save Dump  CC1  CC2                               
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_MIXER_Tick_Cont
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_MIXER_Tick_Util_Upper
	rgoto	CS_M_MIXER_Tick_Util_Lower

CS_M_MIXER_Tick_Util_Upper_STR
	db	"Map#   Mixer Utility Functions          CC Assignments                          "
CS_M_MIXER_Tick_Util_Upper
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
	TABLE_ADDR_MUL_W CS_M_MIXER_Tick_Util_Upper_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; if last position reached, print in-menu message at right upper corner of first display
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	7
	bnz	CS_M_MIXER_Tick_Util_Upper_End
	movf	CS_IN_MENU_MSG, W
	andlw	0x7f
	bz	CS_M_MIXER_Tick_Util_Upper_End
CS_M_MIXER_Tick_Util_Upper_Msg
	movlw	0x00 + 32
	call	MIOS_LCD_CursorSet
	decf	CS_IN_MENU_MSG, W
	andlw	0x7f
	addlw	-0x10
	TABLE_ADDR_MUL_W CS_M_MIXER_TICK_IN_MENU_MSGS, 8
	movlw	8 ; characters
	call	MIOS_LCD_PrintPreconfString

	;; switch back to default message once button has been released (will be updated after ca. 1 second)
	BRA_IFSET CS_IN_MENU_MSG, 7, ACCESS, CS_M_MIXER_Tick_Util_Upper_End
	movlw	CS_M_MIXER_MSG_DEFAULT
	movwf	CS_IN_MENU_MSG
CS_M_MIXER_Tick_Util_Upper_End
	rgoto	CS_M_MIXER_Tick_Cont


CS_M_MIXER_Tick_Util_Lower_STR
	db	"127  Copy Paste Clr      Load Save Dump  CC1  CC2                               "
CS_M_MIXER_Tick_Util_Lower
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x80
	iorlw	0x40
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_MIXER_Tick_Util_Lower_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; if first position, print map number in first 4 characters
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	8
	bnz	CS_M_MIXER_Tick_Util_Lower_End
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	incf	CS_MIXER_MAP, W
	call	MIOS_LCD_PrintBCD3
CS_M_MIXER_Tick_Util_Lower_End
	rgoto	CS_M_MIXER_Tick_Cont


	;; ------------------------------------------------------------------

CS_M_MIXER_Tick_Normal
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Mixer Map #  1                          Page [ 4] Volume             IIC1 Chn#12
	;; 0000000000000011111111111111111111111111222222222222222222222222222223333-444444
	;;  127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_ 127_
	;; 00000111112222233333444445555566666777778888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF

	;; print lower segments
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_MIXER_Tick_Lower
	;; else print the two upper segments
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	bz	CS_M_MIXER_Tick_Upper0
	addlw	-1
	bz	CS_M_MIXER_Tick_Upper1
	addlw	-1
	bz	CS_M_MIXER_Tick_Upper2
	addlw	-1
	bz	CS_M_MIXER_Tick_Upper3
	rgoto	CS_M_MIXER_Tick_Upper4


;; ---
CS_M_MIXER_Tick_Upper0_STR STRING 11, 0x00 + 0, "Mixer Map #"
CS_M_MIXER_Tick_Upper0
	;; init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	TABLE_ADDR CS_M_MIXER_Tick_Upper0_STR
	call	MIOS_LCD_PrintString

	incf	CS_MIXER_MAP, W
	call	MIOS_LCD_PrintBCD3

	rgoto	CS_M_MIXER_Tick_Cont


CS_M_MIXER_Tick_Upper1
	movlw	0x00 + 14
	call	MIOS_LCD_CursorSet
	movlw	26
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_MIXER_Tick_Cont

CS_M_MIXER_Tick_Upper2_STR STRING 6, 0x80 + 0, "Page ["
CS_M_MIXER_Tick_Upper2
	TABLE_ADDR CS_M_MIXER_Tick_Upper2_STR
	call	MIOS_LCD_PrintString

	incf	CS_MIXER_PAGE, W
	call	MIOS_LCD_PrintBCD2
	movlw	']'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar

	movlw	CS_M_MIXER_PARAMETER_ENTRIES
	cpfslt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_Tick_Upper2_CC
CS_M_MIXER_Tick_Upper2_Tab
	movf	CS_MIXER_PAGE, W
	TABLE_ADDR_MUL_W CS_M_MIXER_PARAMETER_TABLE, CS_M_MIXER_PARAMETER_LEN
	movlw	CS_M_MIXER_PARAMETER_LEN
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_MIXER_Tick_Cont


CS_M_MIXER_Tick_Upper2_CC
	movf	CS_MIXER_PAGE, W
	addlw	-CS_M_MIXER_PARAMETER_ENTRIES
	BRA_IFSET WREG, 1, ACCESS, CS_M_MIXER_Tick_Upper2_CC_Asg
CS_M_MIXER_Tick_Upper2_CC_Val
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movf	CS_MIXER_PAGE, W
	addlw	-CS_M_MIXER_PARAMETER_ENTRIES+1
	call	MIOS_LCD_PrintBCD1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'#'
	call	MIOS_LCD_PrintChar

	movf	CS_MIXER_PAGE, W			; (determine assignmend CC)
	addlw	-CS_M_MIXER_PARAMETER_ENTRIES
	mullw	0x10
	lfsr	FSR1, SEQ_MIXER_MAP_CC1ASG
	movf	PRODL, W
	addwf	CS_MIXER_SELECTED_TRK, W
	movf	PLUSW1, W
	call	MIOS_LCD_PrintBCD3
	movlw	CS_M_MIXER_PARAMETER_LEN-8
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_MIXER_Tick_Cont


CS_M_MIXER_Tick_Upper2_CC_AsgSTR
	db	"Assignment for CC"
CS_M_MIXER_Tick_Upper2_CC_Asg
	TABLE_ADDR CS_M_MIXER_Tick_Upper2_CC_AsgSTR
	movlw	17
	call	MIOS_LCD_PrintPreconfString
	movf	CS_MIXER_PAGE, W
	addlw	-CS_M_MIXER_PARAMETER_ENTRIES-2+1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_MIXER_Tick_Cont


CS_M_MIXER_Tick_Upper3
	movlw	0x80 + 29
	call	MIOS_LCD_CursorSet
	lfsr	FSR1, SEQ_MIXER_MAP_MIDI
	movf	CS_MIXER_SELECTED_TRK, W
	swapf	PLUSW1, W
	andlw	0x0f
	call	SEQ_LCD_PrnPort
	rgoto	CS_M_MIXER_Tick_Cont

CS_M_MIXER_Tick_Upper4_STR STRING 4, 0x80 + 34, "Chn#"
CS_M_MIXER_Tick_Upper4
	TABLE_ADDR CS_M_MIXER_Tick_Upper4_STR
	call	MIOS_LCD_PrintString

	lfsr	FSR1, SEQ_MIXER_MAP_MIDI
	movf	CS_MIXER_SELECTED_TRK, W
	movf	PLUSW1, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_MIXER_Tick_Cont


;; ---
CS_M_MIXER_Tick_Lower
	movf	CS_MENU_REFRESH_ITEM_CTR, W

	;; current value -> TMP1
	movlw	0x7f
	movwf	TMP1

	;; item selected -> TMP3.0
	setf	TMP3
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	cpfseq	CS_MIXER_SELECTED_TRK, ACCESS
	clrf TMP3

	;; set cursor
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	addlw	0x40
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 3
	addlw 0x80
	call	MIOS_LCD_CursorSet

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; check if item should be flashed
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_MIXER_Tick_Lower_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_MIXER_Tick_Lower_Spaces

CS_M_MIXER_Tick_Lower_NoF
	;; branch depending on selected parameter
	movf	CS_MIXER_PAGE, W
	bz	CS_M_MIXER_Tick_Lower_MPort
	addlw	-1
	bz	CS_M_MIXER_Tick_Lower_MChn
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	cpfslt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_Tick_Lower_Asg
	rgoto	CS_M_MIXER_Tick_Lower_Value
CS_M_MIXER_Tick_Lower_MPort
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rcall	CS_M_MIXER_ValueGet
	call	SEQ_LCD_PrnPort	; 4 characters
	rgoto	CS_M_MIXER_Tick_Lower_Cont


CS_M_MIXER_Tick_Lower_MChn
	movlw	'#'
	call	MIOS_LCD_PrintChar
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rcall	CS_M_MIXER_ValueGet
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_MIXER_Tick_Lower_Cont

CS_M_MIXER_Tick_Lower_Asg
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rcall	CS_M_MIXER_ValueGet
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_MIXER_Tick_Lower_Cont

CS_M_MIXER_Tick_Lower_Value
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rcall	CS_M_MIXER_ValueGet
	bz	CS_M_MIXER_Tick_Lower_Value_Dis
CS_M_MIXER_Tick_Lower_Value_End
	addlw	-1
	movwf	TMP5
	call	MIOS_LCD_PrintBCD3
	movf	TMP5, W
	call	SEQ_LCD_PrnVBar
	rgoto	CS_M_MIXER_Tick_Lower_Cont


CS_M_MIXER_Tick_Lower_Value_Dis
	movlw	'-'
	movwf	PRODL
	movlw	3
	call	SEQ_LCD_PrintChars
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_MIXER_Tick_Lower_Cont


CS_M_MIXER_Tick_Lower_Spaces
	movlw	4
	call	SEQ_LCD_PrintSpaces
	;; 	rgoto	CS_M_MIXER_Tick_Lower_Cont

CS_M_MIXER_Tick_Lower_Cont

CS_M_MIXER_Tick_Cont
	;; Tick function will be called 21 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	16+5-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_MIXER_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_MIXER_Tick_End

CS_M_MIXER_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_MIXER_PageChanged
	;; ignore if utility page selected
	btfsc	CS_MIXER_PAGE, 7
	return

	;; select new parameter
	movff	CS_MENU_PARAMETER_L, CS_MIXER_PAGE

	;; set encoder speed depending on max value - if >= 0x80, set fast speed
	rcall	CS_M_MIXER_MaxValueGet

	SET_BSR	SEQ_BASE
	bcf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED
	btfsc	WREG, 7
	bsf	SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS, BANKED

	;; set encoder speed depending on SEQ_MODE0, SEQ_MODE0_FAST_ENCODERS
	goto	SEQ_ENC_ConfigSpeed


;; ==========================================================================
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function returns the maximum value depending on the selected
;;  parameter in CS_MIXER_PAGE
;; --------------------------------------------------------------------------
CS_M_MIXER_MaxValueGet
	movf	CS_MIXER_PAGE, W
	bz	CS_M_MIXER_MaxValueGetPort
	addlw	-1
	bz	CS_M_MIXER_MaxValueGetChn
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	cpfslt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_MaxValueGetAsg
	
	retlw	0x80		; 0-127 + disabled

CS_M_MIXER_MaxValueGetPort
	retlw	0x07		; 8 ports

CS_M_MIXER_MaxValueGetChn
	retlw	0x0f		; 16 MIDI channels

CS_M_MIXER_MaxValueGetAsg
	retlw	0x7f		; CC assignments: 0-127


;; --------------------------------------------------------------------------
;;  This function returns a value from the mixer map depending on
;;  the track number in WREG (0-15) and the selected parameter CS_MIXER_PAGE
;; --------------------------------------------------------------------------
CS_M_MIXER_ValueGet
	lfsr	FSR1, SEQ_MIXER_MAP_BASE
	addwf	FSR1L, F

	movf	CS_MIXER_PAGE, W
	bz	CS_M_MIXER_ValueGetPort
	addlw	-1
	bz	CS_M_MIXER_ValueGetChn

	;; (WREG=1..6)
	mullw	0x10
	movf	PRODL, W
	addwf	FSR1L, F
	movf	INDF1, W
	return

CS_M_MIXER_ValueGetPort
	swapf	INDF1, W
	andlw	0x0f
	return

CS_M_MIXER_ValueGetChn
	movf	INDF1, W
	andlw	0x0f
	return

;; --------------------------------------------------------------------------
;;  This function sets a value of the mixer map depending on
;;  the track number in WREG (0-15) and the selected parameter CS_MIXER_PAGE
;;  Expect value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_M_MIXER_ValueSet
	lfsr	FSR1, SEQ_MIXER_MAP_BASE
	addwf	FSR1L, F

	movf	CS_MIXER_PAGE, W
	bz	CS_M_MIXER_ValueSetPort
	addlw	-1
	bz	CS_M_MIXER_ValueSetChn

	;; (WREG=1..6)
	mullw	0x10
	movf	PRODL, W
	addwf	FSR1L, F
	movff	MIOS_PARAMETER1, INDF1
	return

CS_M_MIXER_ValueSetPort
	movlw	0x0f
	andwf	INDF1, F
	swapf	MIOS_PARAMETER1, W
	andlw	0xf0
	iorwf	INDF1, F
	return

CS_M_MIXER_ValueSetChn
	movlw	0xf0
	andwf	INDF1, F
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorwf	INDF1, F
	return


;; --------------------------------------------------------------------------
;;  This function sends a value over the selected port depending on
;;  the track number in WREG (0-15) and the selected parameter CS_MIXER_PAGE
;;  Expect value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_M_MIXER_ValueSend
	movwf	PRODL		; store selected track in PRODL

	;; don't send when MIDI port, channel or assignment is selected
	movf	CS_MIXER_PAGE, W
	bz	CS_M_MIXER_ValueSend_End
	addlw	-1
	bz	CS_M_MIXER_ValueSend_End
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	cpfslt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_ValueSend_End

	;; determine MIDI_EVNT[01], value and port
	lfsr	FSR1, SEQ_MIXER_MAP_MIDI
	movf	PRODL, W
	addwf	FSR1L, F
	swapf	INDF1, W
	andlw	0x0f
	movwf	MIDI_EVNT_PORT
	movf	INDF1, W
	iorlw	0xb0
	movwf	MIDI_EVNT0

	;; which event?
	movf	CS_MIXER_PAGE, W
	addlw	-2
	bz	CS_M_MIXER_ValueSend_Prg
	addlw	-1
	bz	CS_M_MIXER_ValueSend_Vol
	addlw	-1
	bz	CS_M_MIXER_ValueSend_Pan
	addlw	-1
	bz	CS_M_MIXER_ValueSend_CC1
	rgoto	CS_M_MIXER_ValueSend_CC2

CS_M_MIXER_ValueSend_Prg
	movlw	0x10		; 0xbx -> 0xcx
	addwf	MIDI_EVNT0, F
	lfsr	FSR1, SEQ_MIXER_MAP_PRG
	movf	PRODL, W
	movf	PLUSW1, W
	bz	CS_M_MIXER_ValueSend_End
	addlw	-1
	movwf	MIDI_EVNT1
	movwf	MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send	
	rgoto	CS_M_MIXER_ValueSend_End

CS_M_MIXER_ValueSend_Vol
	movlw	0x07
	movwf	MIDI_EVNT1
	lfsr	FSR1, SEQ_MIXER_MAP_VOL
CS_M_MIXER_ValueSend_Pan_Cont
CS_M_MIXER_ValueSend_CC1_Cont
CS_M_MIXER_ValueSend_CC2_Cont
	movf	PRODL, W
	movf	PLUSW1, W
	bz	CS_M_MIXER_ValueSend_End
	addlw	-1
	movwf	MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send	
	rgoto	CS_M_MIXER_ValueSend_End

CS_M_MIXER_ValueSend_Pan
	movlw	0x0a
	movwf	MIDI_EVNT1
	lfsr	FSR1, SEQ_MIXER_MAP_PAN
	rgoto	CS_M_MIXER_ValueSend_Pan_Cont
	
CS_M_MIXER_ValueSend_CC1
	lfsr	FSR1, SEQ_MIXER_MAP_CC1ASG
	movf	PRODL, W
	movff	PLUSW1, MIDI_EVNT1
	lfsr	FSR1, SEQ_MIXER_MAP_CC1
	rgoto	CS_M_MIXER_ValueSend_CC1_Cont
	
CS_M_MIXER_ValueSend_CC2
	lfsr	FSR1, SEQ_MIXER_MAP_CC2ASG
	movf	PRODL, W
	movff	PLUSW1, MIDI_EVNT1
	lfsr	FSR1, SEQ_MIXER_MAP_CC2
	rgoto	CS_M_MIXER_ValueSend_CC2_Cont

CS_M_MIXER_ValueSend_End
	return


;; --------------------------------------------------------------------------
;;  This function increments the selected mixer value(s) depending on:
;;     - CS_MENU_ENC_INCREMENTER (the incrementer)
;;     - CS_MIXER_PAGE (the selected page)
;;     - CS_MIXER_SELECTED_TRK (the selected value)
;;     - SEQ_MODE0.SEQ_MODE0_CHANGE_ALL_STEPS and CS_STAT2.CS_STAT2_ALL_SAME_VALUE
;; --------------------------------------------------------------------------
CS_M_MIXER_ValueInc
	;; branch depending on the two "All" Modes
	SET_BSR	SEQ_BASE
	BRA_IFSET CS_STAT2, CS_STAT2_ALL_SAME_VALUE, ACCESS, CS_M_MIXER_ValueIncAllSame
	BRA_IFSET SEQ_MODE0, SEQ_MODE0_CHANGE_ALL_STEPS, BANKED, CS_M_MIXER_ValueIncAll
	rgoto	CS_M_MIXER_ValueIncSingle

CS_M_MIXER_ValueIncAll
	;; increment all values - using TMP5 as loop counter
	clrf	TMP5
CS_M_MIXER_ValueIncAllLoop
	;; if page >= 2 and <= 7: check if parameter enabled (!= 0)
	movlw	2-1
	cpfsgt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_ValueIncAllLoopCont
	movlw	CS_M_MIXER_PARAMETER_ENTRIES+2
	cpfslt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_ValueIncAllLoopCont
	movf	TMP5, W
	rcall	CS_M_MIXER_ValueGet
	bz	CS_M_MIXER_ValueIncAllLoopNext
CS_M_MIXER_ValueIncAllLoopCont	
	movf	TMP5, W
	rcall	CS_M_MIXER_ValueInc_Hlp
CS_M_MIXER_ValueIncAllLoopNext
	incf	TMP5, F
	BRA_IFCLR TMP5, 4, ACCESS, CS_M_MIXER_ValueIncAllLoop
	return
	
	
CS_M_MIXER_ValueIncAllSame
	;; first change selected parameter
	movf	CS_MIXER_SELECTED_TRK, W
	rcall	CS_M_MIXER_ValueInc_Hlp
	;; now set all parameters to the same value
	clrf	TMP5
CS_M_MIXER_ValueIncAllSameLoop
	;; skip the parameter which has already been changed
	movf	TMP5, W
	xorwf	CS_MIXER_SELECTED_TRK, W
	bz	CS_M_MIXER_ValueIncAllSameLoopNx

	;; store and send value
	movf	CS_MIXER_SELECTED_TRK, W
	rcall	CS_M_MIXER_ValueGet
	movwf	MIOS_PARAMETER1
	movf	TMP5, W
	rcall	CS_M_MIXER_ValueSet
	movf	TMP5, W
	rcall	CS_M_MIXER_ValueSend
CS_M_MIXER_ValueIncAllSameLoopNx
	incf	TMP5, F
	BRA_IFCLR TMP5, 4, ACCESS, CS_M_MIXER_ValueIncAllSameLoop
	return


CS_M_MIXER_ValueIncSingle
	;; change single selected value
	movf	CS_MIXER_SELECTED_TRK, W
	rgoto	CS_M_MIXER_ValueInc_Hlp


	;; help function used by CS_M_MIXER_ValueInc only!
CS_M_MIXER_ValueInc_Hlp
	movwf	TMP3		; temporary store parameter number in TMP3
	rcall	CS_M_MIXER_ValueGet
	;; store it in TMP[12]
	movwf	TMP1
	clrf	TMP2
	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; get max value
	rcall	CS_M_MIXER_MaxValueGet
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; move incrementer to WREG
	movf	CS_MENU_ENC_INCREMENTER, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, CS_M_MIXER_ValueInc_Hlp_End

	;; store new result
	movff	TMP1, MIOS_PARAMETER1
	movf	TMP3, W
	rcall	CS_M_MIXER_ValueSet

	;; send value
	movff	TMP1, MIOS_PARAMETER1
	movf	TMP3, W
	rcall	CS_M_MIXER_ValueSend
CS_M_MIXER_ValueInc_Hlp_End
	return

;; ==========================================================================
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function clears a Mixer map
;; --------------------------------------------------------------------------
CS_M_MIXER_ClearMap
	;; notify that UNDO buffer is not filled anymore (it overlays SEQ_MIXER_MAP_BASE)
	bcf	CS_STAT2, CS_STAT2_UNDO_FILLED

	;; MIDI Port + Channel + Program
	lfsr	FSR0, SEQ_MIXER_MAP_MIDI
	lfsr	FSR1, SEQ_MIXER_MAP_PRG
	lfsr	FSR2, SEQ_MIXER_MAP_VOL
	clrf	TMP1
CS_M_MIXER_ClearMapLoop1
	movff	TMP1, POSTINC0	; MIDI channel increments, port always 0
	clrf	POSTINC1	; PRG = deselected
	clrf	POSTINC2	; VOL = deselected
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_M_MIXER_ClearMapLoop1

	;; PAN and MIDI CC's
	lfsr	FSR0, SEQ_MIXER_MAP_PAN
	lfsr	FSR1, SEQ_MIXER_MAP_CC1
	lfsr	FSR2, SEQ_MIXER_MAP_CC2
	clrf	TMP1
CS_M_MIXER_ClearMapLoop2
	clrf	POSTINC0	; PAN = deselected
	clrf	POSTINC1	; CC1 = deselected
	clrf	POSTINC2	; CC2 = deselected
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_M_MIXER_ClearMapLoop2

	;; MIDI CC Assignments
	lfsr	FSR0, SEQ_MIXER_MAP_CC1ASG
	lfsr	FSR1, SEQ_MIXER_MAP_CC2ASG
	clrf	TMP1
CS_M_MIXER_ClearMapLoop3
	movlw	91
	movwf	POSTINC0	; CC1 = Reverb
	movlw	93
	movwf	POSTINC1	; CC2 = Chorus
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_M_MIXER_ClearMapLoop3

	return


;; --------------------------------------------------------------------------
;;  This function copies a Mixer map into the paste buffer
;; --------------------------------------------------------------------------
CS_M_MIXER_CopyMap
	;; use PRODL as loop counter
	clrf	PRODL
	lfsr	FSR0, SEQ_MIXER_MAP_BASE
	lfsr	FSR1, SEQ_COPYPASTE_MIXER
CS_M_MIXER_CopyMapLoop
	movff	POSTINC0, POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, CS_M_MIXER_CopyMapLoop

	;; notify that copy&paste buffer is filled
	bcf	CS_STAT2, CS_STAT2_COPYPASTE_FILLED_P	; clear for pattern
	bsf	CS_STAT2, CS_STAT2_COPYPASTE_FILLED_M	; assign to mixer

	return

;; --------------------------------------------------------------------------
;;  This function copies a Mixer map from the paste buffer
;; --------------------------------------------------------------------------
CS_M_MIXER_PasteMap
	;; branch to clear function if copy&paste buffer not filled
	BRA_IFCLR CS_STAT2, CS_STAT2_COPYPASTE_FILLED_M, ACCESS, CS_M_MIXER_ClearMap

	;; copy 128 bytes from SEQ_COPYPASTE_MIXER to SEQ_MIXER_MAP_BASE
	;; use PRODL as loop counter
	clrf	PRODL
	lfsr	FSR0, SEQ_COPYPASTE_MIXER
	lfsr	FSR1, SEQ_MIXER_MAP_BASE
CS_M_MIXER_PasteMapLoop
	movff	POSTINC0, POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 7, ACCESS, CS_M_MIXER_PasteMapLoop

	return

;; --------------------------------------------------------------------------
;;  This function load a Mixer map from BankStick
;;  IN:	Mixer Map in CS_MIXER_MAP
;; --------------------------------------------------------------------------
CS_M_MIXER_LoadMap
	;; notify that UNDO buffer is not filled anymore (it overlays SEQ_MIXER_MAP_BASE)
	bcf	CS_STAT2, CS_STAT2_UNDO_FILLED

	movff	CS_MIXER_MAP, SEQ_MIXER_MAP
	clrf	EEADR
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x00
	call	SEQ_MBANK_ReadPage
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x40
	goto	SEQ_MBANK_ReadPage

;; --------------------------------------------------------------------------
;;  This function saves a Mixer map into BankStick
;;  IN:	Mixer Map in CS_MIXER_MAP
;; --------------------------------------------------------------------------
CS_M_MIXER_SaveMap
	movff	CS_MIXER_MAP, SEQ_MIXER_MAP
	clrf	EEADR
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x00
	call	SEQ_MBANK_WritePage
	lfsr	FSR1, SEQ_MIXER_MAP_BASE + 0x40
	goto	SEQ_MBANK_WritePage

;; --------------------------------------------------------------------------
;;  This function dumps a Mixer map over the MIDI ports
;; --------------------------------------------------------------------------
CS_M_MIXER_DumpMap
	;; dump the MIDI events of all mixer tracks
	movff	CS_MIXER_PAGE, TMP2	; temporary store mixer page in TMP2
	clrf	TMP1			; TMP1 contains the mixer track number
CS_M_MIXER_DumpMapLoopO
	clrwdt				; feed watchdog

	;; dump Prg..CC2 (2..6)
	movlw	2
	movwf	CS_MIXER_PAGE
CS_M_MIXER_DumpMapLoopI
	movf	TMP1, W			; get value
	rcall	CS_M_MIXER_ValueGet
	movwf	MIOS_PARAMETER1
	movf	TMP1, W			; send value
	rcall	CS_M_MIXER_ValueSend
	
	incf	CS_MIXER_PAGE, F
	movlw	6
	cpfsgt	CS_MIXER_PAGE, ACCESS
	rgoto CS_M_MIXER_DumpMapLoopI

	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_M_MIXER_DumpMapLoopO

	movff	TMP2, CS_MIXER_PAGE	; restore mixer page from TMP2

	return


;; ==========================================================================
;; ==========================================================================

CS_M_MIXER_PARAMETER_LEN		EQU	18	; 18 characters per entry
CS_M_MIXER_PARAMETER_ENTRIES		EQU	5	; 5 entries
CS_M_MIXER_PARAMETER_TABLE				; all entries are listed here
	db	"MIDI Port         "
	db	"MIDI Channel      "
	db	"Prog.Change       "
	db	"Volume            "
	db	"Panorama          "
	;; 5 and 6: CC#<number> will be displayed separately
	;; 7 and 8: CC# Assignment will be displayed separately
