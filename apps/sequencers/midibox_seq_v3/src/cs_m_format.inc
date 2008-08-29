; $Id$
;
; MBSEQ Menu "Format"
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
CS_M_FORMAT_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+0*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+1*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+2*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+3*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+4*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+5*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+6*5+1, 3,	CS_M_FORMAT_ChangeCursor
 CS_MENU_PARAMETER_ENTRY -1,					0x07,	0x40+7*5+1, 3,	CS_M_FORMAT_ChangeCursor


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
CS_M_FORMAT_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; never use ENC incrementer
	clrf	CS_MENU_ENC_INCREMENTER

	;; ignore if button number 9-16
	btfsc	MIOS_PARAMETER1, 3
	return

	;; set new cursor pos
	movf	MIOS_PARAMETER1, W

CS_M_FORMAT_GP_Callback_Cont
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_FORMAT_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_FORMAT_GP_Callback_Cont_NP
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit


;; --------------------------------------------------------------------------
CS_M_FORMAT_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; pattern is simply the OR mask of cursor pos
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	return


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_FORMAT_Select
	;; clear callback if button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_FORMAT_Select_Depressed

CS_M_FORMAT_Select_Pressed
	;; install delayed callback hook, used to trigger the save function after 2 seconds
	TABLE_ADDR CS_M_FORMAT_Select_DoIt
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; clear screen(s)
	call	SEQ_LCD_Clear
	;; print message
	call	SEQ_LCD_PrintStringAfterCall
	STRING 25, 0x00, "**    Hold 2 seconds   **"
	call	SEQ_LCD_PrintStringAfterCall
	STRING 25, 0x40, "** to format BankStick **"
	movlw	0x80
	goto	MIOS_LCD_MessageStart

CS_M_FORMAT_Select_Depressed
	;; deinstall hook and stop message
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	goto	MIOS_LCD_MessageStop


	;; callback function
CS_M_FORMAT_Select_DoIt

	;; ignore if BankStick not available
	movf	CS_MENU_CURSOR_POS, W
	call	SEQ_BANK_GetTypeAvailable
	bz	CS_M_FORMAT_Select_DoIt_Failed

	;; branch depending on BS type
	movf	CS_MENU_CURSOR_POS, W
	call	SEQ_BANK_GetType
	bz	CS_M_FORMAT_Select_DoIt_Failed
	addlw	-1
	bz	CS_M_FORMAT_Select_DoIt_P	; Pattern, 32k
	addlw	-1
	bz	CS_M_FORMAT_Select_DoIt_P	; Pattern, 64k
	addlw	-1
	bz	CS_M_FORMAT_Select_DoIt_S	; Song, 32k
	addlw	-1
	bz	CS_M_FORMAT_Select_DoIt_S	; Song, 64k
	addlw	-1
	bz	CS_M_FORMAT_Select_DoIt_M	; Mixer
	rgoto	CS_M_FORMAT_Select_DoIt_Failed

CS_M_FORMAT_Select_DoIt_P
	;; select bankstick and branch to format function
	movff	CS_MENU_CURSOR_POS, SEQ_PATTERN_BANK
	goto	SEQ_BANK_FormatStick

CS_M_FORMAT_Select_DoIt_S
	;; ignore song bankstick if SEQ_SBANK_BS_CS == -1
	movlw	SEQ_SBANK_BS_CS
	addlw	1		; (add 1 to update zero flag, movlw doesn't do this (so far I remember))
	bz	CS_M_FORMAT_Select_DoIt_Failed

	;; format SongStick
	goto	SEQ_SBANK_FormatStick


CS_M_FORMAT_Select_DoIt_M
	;; ignore song bankstick if SEQ_MBANK_BS_CS == -1
	movlw	SEQ_MBANK_BS_CS
	addlw	1		; (add 1 to update zero flag, movlw doesn't do this (so far I remember))
	bz	CS_M_FORMAT_Select_DoIt_Failed

	;; format MixerStick
	goto	SEQ_MBANK_FormatStick


CS_M_FORMAT_Select_DoIt_Failed
	;; print message	
	call	SEQ_LCD_PrintStringAfterCall
	STRING 17, 0x00, "* BankStick #1  *"
	call	SEQ_LCD_PrintStringAfterCall
	STRING 17, 0x40, "* not available *"
	movlw	0x00 + 13
	call	MIOS_LCD_CursorSet
	incf	CS_MENU_CURSOR_POS, W
	call	MIOS_LCD_PrintBCD1
	movlw	0x80
	goto	MIOS_LCD_MessageStart

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_FORMAT_Init
	;; install parameter table
	TABLE_ADDR CS_M_FORMAT_PARAMETERS	; pointer to table
	movlw	8			; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_FORMAT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; copy current cursor pos into menu parameter
	;; (in this menu, the data wheel changes the cursor pos)
	movff	CS_MENU_CURSOR_POS, CS_MENU_PARAMETER_L

	;; init LEDs
	rgoto	CS_M_FORMAT_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_FORMAT_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; 1:Pt 2:Pt 3:Mx 4:Sg 5:Pt 6:Pt 7:Pt 8:Pt For formatting BankStick, select number 
	;; 00000111112222233333444445555566666777770000000000111111111122222222223333333333
	;;  64k  64k  32k  32k  64k  64k  n/a  n/a  and press&hold SELECT button 2 seconds 
	;; 8888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF4444444444555555555566666666667777777777

	;; print segments 0-23

	;; right side
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_FORMAT_Tick_R

	;; left side upper
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_FORMAT_Tick_LU
	;; left side lower
	rgoto	CS_M_FORMAT_Tick_LL

CS_M_FORMAT_Tick_R_STR
	db	"For formatting BankStick, select number  and press&hold SELECT button 2 seconds "
CS_M_FORMAT_Tick_R
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	addlw	0x80
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x40
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_FORMAT_Tick_R_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	rgoto	CS_M_FORMAT_Tick_Cont


CS_M_FORMAT_Tick_LU
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; print BS number and type
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1

	movlw	':'
	call	MIOS_LCD_PrintChar

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	call	SEQ_BANK_PrnType

	rgoto	CS_M_FORMAT_Tick_Cont

CS_M_FORMAT_Tick_LL
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	addlw	0x40 + 1
	call	MIOS_LCD_CursorSet

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_FORMAT_Tick_Cont

	;; print BS size (n/a for not available)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	call	SEQ_BANK_PrnSize
	;;  	rgoto	CS_M_FORMAT_Tick_Cont

CS_M_FORMAT_Tick_Cont
	;; Tick function will be called 24 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	24-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_FORMAT_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_FORMAT_Tick_End
	return


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_FORMAT_ChangeCursor
	movff	CS_MENU_PARAMETER_L, CS_MENU_CURSOR_POS

	;; update LEDs
	rgoto	CS_M_FORMAT_UpdateGPLEDs

