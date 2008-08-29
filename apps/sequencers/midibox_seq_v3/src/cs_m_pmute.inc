; $Id$
;
; MBSEQ Menu "Port Mute"
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
CS_M_PMUTE_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*0+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*1+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*2+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*3+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*4+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*5+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*6+2, 2,	CS_M_PMUTE_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,   0x40+ 5*7+2, 2,	CS_M_PMUTE_ValueChanged

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
CS_M_PMUTE_GP_Callback
	SET_BSR	SEQ_BASE

	;; exit if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; ignore buttons 8-15
	BRA_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_PMUTE_GP_Callback_Ignore
CS_M_PMUTE_GP_Callback_Toggle
	;; set new cursor position
	movf	MIOS_PARAMETER1, W
	movwf	CS_MENU_CURSOR_POS

	;; toggle flag
	SET_BSR	SEQ_BASE
	call	MIOS_HLP_GetBitORMask
	xorwf	SEQ_PORTS_MUTED, F, BANKED
	
	clrf	CS_MENU_ENC_INCREMENTER	; (no encoder action)
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit


CS_M_PMUTE_GP_Callback_Ignore
	clrf	CS_MENU_ENC_INCREMENTER	; (no encoder action)
	return

;; --------------------------------------------------------------------------
CS_M_PMUTE_UpdateGPLEDs
	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; set LED for each muted port
	movff	SEQ_PORTS_MUTED, CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_PMUTE_Init
	;; install parameter table
	TABLE_ADDR CS_M_PMUTE_PARAMETERS	; pointer to table
	movlw	8				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_PMUTE_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; determine content of CS_MENU_PARAMETER_L
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PORTS_MUTED, W, BANKED
	clrf	CS_MENU_PARAMETER_L
	skpz
	bsf	CS_MENU_PARAMETER_L, 0

	;; init LEDs
	rgoto	CS_M_PMUTE_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PMUTE_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Def. Int. IIC0 IIC1 IIC2 IIC3 AOUT Lbk  <-- Muted Ports
	;; 0000011111222223333344444555556666677777000000000000000-------------------------
	;;   o    o    o    o    o    o    o    o  <-- *=muted, o=unmuted
	;; 8888899999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF1111111111111111111111------------------

	;; print segments 0-17
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-0x10
	bz	CS_M_PMUTE_Tick_16
	addlw	-1
	bz	CS_M_PMUTE_Tick_17

	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_PMUTE_Tick_Upper
	rgoto	CS_M_PMUTE_Tick_Lower

CS_M_PMUTE_Tick_Upper
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; print port name
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	call	SEQ_LCD_PrnPort
	rgoto	CS_M_PMUTE_Tick_Cont


CS_M_PMUTE_Tick_Lower
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	addlw	0x40 + 2
	call	MIOS_LCD_CursorSet

	movf	CS_MENU_REFRESH_ITEM_CTR, W	; item number
	andlw	0x07
	call	CS_MENU_CursorHandler2
	bz	CS_M_PMUTE_Tick_Cont

	;; print 'o'/'*' depending on mute status
	SET_BSR	SEQ_BASE
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PORTS_MUTED, W, BANKED
	movlw	'o'
	skpz
	movlw	'*'
	call	MIOS_LCD_PrintChar

  	rgoto	CS_M_PMUTE_Tick_Cont

CS_M_PMUTE_Tick_16
	call	SEQ_LCD_PrintStringAfterCall
	STRING	15, 0x80+0, "<-- Muted Ports"
  	rgoto	CS_M_PMUTE_Tick_Cont

CS_M_PMUTE_Tick_17
	call	SEQ_LCD_PrintStringAfterCall
	STRING	22, 0xc0+0, "<-- *=muted, o=unmuted"
	;;   	rgoto	CS_M_PMUTE_Tick_Cont

CS_M_PMUTE_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	16+2-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_PMUTE_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_PMUTE_Tick_End
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_PMUTE_ValueChanged
	SET_BSR	SEQ_BASE

	;; RMW
	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitANDMask
	andwf	SEQ_PORTS_MUTED, F, BANKED

	movf	CS_MENU_CURSOR_POS, W
	call	MIOS_HLP_GetBitORMask
	btfsc	CS_MENU_PARAMETER_L, 0
	iorwf	SEQ_PORTS_MUTED, F, BANKED

	rgoto	CS_M_PMUTE_UpdateGPLEDs

;; ==========================================================================
