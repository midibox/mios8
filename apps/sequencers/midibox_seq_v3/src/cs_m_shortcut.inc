; $Id$
;
; MBSEQ Menu "Shortcuts"
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
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_SHORTCUT_GP_Callback
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; invalidate old menu page (so that releasing the MENU button doesn't jump back to it)
	movlw	0xff
	movwf	CS_MENU_MEMO_PAGE

	;; clear screens
	call	SEQ_LCD_Clear

	;; determine shortcut menu page number
	movf	MIOS_PARAMETER1, W
	rcall	CS_M_SHORTCUT_GetGPMenu

	;; branch to the change menu function of the control surface
	goto	CS_MENU_ChangePage


;; --------------------------------------------------------------------------
CS_M_SHORTCUT_UpdateGPLEDs
	;; flash LED of selected menu
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	clrf	PRODL
	TABLE_ADDR CS_MENU_TABLE_GPMAP
CS_M_SHORTCUT_UpdateGPLEDs_Loop
	tblrd*+
	movf	TABLAT, W
	cpfseq	CS_MENU_MEMO_PAGE, ACCESS
	rgoto	CS_M_SHORTCUT_UpdateGPLEDs_LoopN
	movf	PRODL, W
	call	MIOS_HLP_GetBitORMask
	btfss	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
CS_M_SHORTCUT_UpdateGPLEDs_LoopN
	incf	PRODL, F
	BRA_IFCLR PRODL, 4, ACCESS, CS_M_SHORTCUT_UpdateGPLEDs_Loop
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SHORTCUT_Init
	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_SHORTCUT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_SHORTCUT_UpdateGPLEDs

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SHORTCUT_Tick
	;; print segments 0-15 (each menu page has an own tick

	;; if metronome button is pressed: print step number instead (we are using this
	;; button combination for playing a MIDI start event at a specific step)

	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	mullw	5
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 3
	iorlw	0x80
	movwf	TMP1
	call	MIOS_LCD_CursorSet

	;; upper line: print string in first 6 segments, otherwise spaces
	movlw	6-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto	CS_M_SHORTCUT_Tick_Upper_Label
CS_M_SHORTCUT_Tick_Upper_Spc
	movlw	5
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SHORTCUT_Tick_Lower

CS_M_SHORTCUT_Tick_Upper_Label
	SET_BSR	SEQ_BASE
	movlw	0
	btfsc	SEQ_MODE1, SEQ_MODE1_MIDISTART_HOLD, BANKED
	movlw	1
	TABLE_ADDR_MUL_W CS_M_SHORTCUT_LABEL, 30
	movf	TMP1, W		; re-use segment offset from cursor pos
	andlw	0x1f
	TABLE_ADD_W
	movlw	5
	call	MIOS_LCD_PrintPreconfString
	;; 	rgoto	CS_M_SHORTCUT_Tick_Lower
	

CS_M_SHORTCUT_Tick_Lower
	;; lower line: print page name
	;; flash if it was previously selected
	movf	TMP1, W
	iorlw	0x40
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_MODE1, SEQ_MODE1_MIDISTART_HOLD, BANKED, CS_M_SHORTCUT_TickNoExtMIDIStart
CS_M_SHORTCUT_Tick_ExtMIDIStart
	;; set TMP2,0 if current step is selected
	clrf	TMP2
	movf	SEQ_EXT_MIDISTART_STEP, W
	xorwf	CS_MENU_REFRESH_ITEM_CTR, W
	skpnz
	bsf	TMP2, 0

	movlw	'>'		; mark step if selected
	btfss	TMP2, 0
	movlw	' '
	call	MIOS_LCD_PrintChar

	incf	CS_MENU_REFRESH_ITEM_CTR, W	; print step number
	call	MIOS_LCD_PrintBCD2

	movlw	'<'		; mark step if selected
	btfss	TMP2, 0
	movlw	' '
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar

	rgoto	CS_M_SHORTCUT_Tick_Next


CS_M_SHORTCUT_TickNoExtMIDIStart
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_SHORTCUT_Tick_NoFlash
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	rcall	CS_M_SHORTCUT_GetGPMenu
	xorwf	CS_MENU_MEMO_PAGE, W
	bnz	CS_M_SHORTCUT_Tick_NoFlash
CS_M_SHORTCUT_Tick_Flash
	movlw	5
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SHORTCUT_Tick_Next
CS_M_SHORTCUT_Tick_NoFlash
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	TABLE_ADDR_MUL_W CS_MENU_TABLE_GPMAP_STR, 5
	movlw	5
	call	MIOS_LCD_PrintPreconfString
	;; 	rgoto	CS_M_SHORTCUT_Tick_Next

CS_M_SHORTCUT_Tick_Next
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_SHORTCUT_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_SHORTCUT_Tick_End

CS_M_SHORTCUT_Tick_End
	return


CS_M_SHORTCUT_LABEL
	;;       <----------------------------> 30 chars = 6 segments
	db	"Menu Shortcuts:               "
	db	"Play Ext. MIDI Start Event at:"

;; ==========================================================================

;; returns menu page number which is assigned to GP button number in WREG
CS_M_SHORTCUT_GetGPMenu
	addlw	CS_MENU_TABLE_GPMAP & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	CS_MENU_TABLE_GPMAP >> 8
	addwfc	TBLPTRH, F
	tblrd*+
	movf	TABLAT, W
	return
