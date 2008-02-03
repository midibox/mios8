; $Id$
;
; Control Surface Menu Functions
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Table of special characters
;; --------------------------------------------------------------------------
CS_MENU_SPECIAL_CHARS
	;; due to an imperfection in the MPASM we have
	;; to write two bytes in every line :-(
	;; char #1: left-arrow
	db b'00000011', b'00000111'; 1st and 2nd line of special char
	db b'00001111', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00001111'; 5th and 6th line of special char
	db b'00000111', b'00000011'; 7th and 8th line of special char
	;; char #2: right-arrow
	db b'00011000', b'00011100'; 1st and 2nd line of special char
	db b'00011110', b'00011111'; 3rd and 4th line of special char
	db b'00011111', b'00011110'; 5th and 6th line of special char
	db b'00011100', b'00011000'; 7th and 8th line of special char
	;; char #3: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #4: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #5: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #6: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #7: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char
	;; char #8: unused
	db b'00000000', b'00000000'; 1st and 2nd line of special char
	db b'00000000', b'00000000'; 3rd and 4th line of special char
	db b'00000000', b'00000000'; 5th and 6th line of special char
	db b'00000000', b'00000000'; 7th and 8th line of special char


;; --------------------------------------------------------------------------
;;  this function initializes the menu interface
;; --------------------------------------------------------------------------
CS_MENU_Init
	;; clear CS stat
	clrf	CS_STAT
	;; enable autorepeat for left and right button if datawheel not enabled
#if DEFAULT_ENC_DATAWHEEL == -1
	movlw	DEFAULT_DIN_MENU_LEFT
	call	MIOS_DIN_PinAutoRepeatEnable
	movlw	DEFAULT_DIN_MENU_RIGHT
	call	MIOS_DIN_PinAutoRepeatEnable
#endif
	;; change to DISPLAY page and exit
	movlw	0x00
	rgoto	CS_MENU_ChangePage
	

;; --------------------------------------------------------------------------
;;  entry function for a menu page update
;; --------------------------------------------------------------------------
CS_MENU_Handler
	;; check delayed callback counter if requested
	BRA_IFCLR CS_STAT, CS_STAT_DELAYED_CALLBACK, ACCESS, CS_MENU_Handler_NoCallback
	movf	CS_MENU_DEL_CALLBACK_CTR, W
	bnz	CS_MENU_Handler_NoCallback
	rcall	CS_MENU_ExecDelayedCallbackHook
	return			; and exit so that messages will not be overwritten!
CS_MENU_Handler_NoCallback

	;; when not in hold mode and message ctr == 1, clear counter and request init
	BRA_IFSET CS_STAT, CS_STAT_MESSAGE_HOLD, ACCESS, CS_MENU_Handler_NoCtrReset
	decf	CS_MENU_MESSAGE_CTR, W
	bnz	CS_MENU_Handler_NoCtrReset
	clrf	CS_MENU_MESSAGE_CTR
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
CS_MENU_Handler_NoCtrReset

	;; check if display initialization has been requested (CS_STAT.CS_STAT_DISPLAY_INIT_REQ set)
	BRA_IFCLR CS_STAT, CS_STAT_DISPLAY_INIT_REQ, ACCESS, CS_MENU_Handler_NoInit

CS_MENU_Handler_Init
	;; clear request flag
	bcf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; force an display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear message flags
	bcf	CS_STAT, CS_STAT_MESSAGE_HOLD
	bcf	CS_STAT, CS_STAT_MESSAGE_REQ

	;; set CS_STAT_DISPLAY_GLCD depending on LCD mode
	bcf	CS_STAT, CS_STAT_DISPLAY_GLCD
	call	MIOS_LCD_TypeGet
	skpz
	bsf	CS_STAT, CS_STAT_DISPLAY_GLCD

	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR CS_STAT, CS_STAT_DISPLAY_GLCD, ACCESS, MIOS_LCD_Cmd

	;; clear don't flash flags
	bcf	MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT
	bcf	MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT

	;; clear GP hook (has to be re-initialized by user init routine)
	clrf	TBLPTRL
	clrf	TBLPTRH
	rcall	CS_MENU_SetMenuGPCallbackHook

	;; init menu table and print first line
	rcall	CS_MENU_TABLE_Init

	;; branch depending on CS_STAT_WITHIN_PAGE flag
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Handler_Init_Menu
CS_MENU_Handler_Init_Page
	;; clear screen
	call	MIOS_LCD_Clear
	;; clear parameter entry hook
	clrf	CS_MENU_PARAMETER_HOOK_L
	clrf	CS_MENU_PARAMETER_HOOK_H
	;; branch to CS_M_*Init
	movlw	CS_MENU_ENTRY_INIT_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuPageCall
	rgoto	CS_MENU_Handler_Init_End

CS_MENU_Handler_Init_Menu
	;; clear counter so that cs_menu_timer.inc counts from zero
	clrf	CS_MENU_MESSAGE_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH

	;; only if character LCD connected: init special characters
	BRA_IFSET CS_STAT, CS_STAT_DISPLAY_GLCD, ACCESS, CS_MENU_Handler_Init_NoCLCD

	;; init special chars
	TABLE_ADDR CS_MENU_SPECIAL_CHARS
	call	MIOS_CLCD_SpecialCharsInit

CS_MENU_Handler_Init_NoCLCD
CS_MENU_Handler_Init_End
	
CS_MENU_Handler_NoInit

CS_MENU_Handler_Tick

	;; continue if display update has been requested, else exit
	btfss	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return
	;; clear request
	bcf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; update LEDs
	movlw	CS_MENU_HEADER_LED_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuHdrCall

	;; if GP hook has been installed, overlay 2 LED shiftregisters
	rcall	CS_MENU_LED_OverlayHandler

	;; if message has been requested: print it so long as counter > 0
	BRA_IFSET CS_STAT, CS_STAT_MESSAGE_REQ, ACCESS, CS_MENU_Handler_Tick_Msg
	;; so long as message counter > 0, print this message
	movf	CS_MENU_MESSAGE_CTR, W
	bnz	CS_MENU_Handler_Tick_Msg

	;; if CS_STAT_WITHIN_PAGE flag cleared, we are in the menu - nothing else to do
	btfss	CS_STAT, CS_STAT_WITHIN_PAGE
	return

	;; else branch to CS_M_*Tick
	movlw	CS_MENU_ENTRY_TICK_FUNC_OFFSET
	rgoto	CS_MENU_Hlp_IndirectMenuPageCall

	
	
CS_MENU_Handler_Tick_Msg
	;; if message has been requested
	BRA_IFCLR CS_STAT, CS_STAT_MESSAGE_REQ, ACCESS, CS_MENU_Handler_Tick_Msg_NoReq
	;; clear request
	bcf	CS_STAT, CS_STAT_MESSAGE_REQ
	;; and re-ini message counter (counted every 100 ms, so we take 20 for 2 seconds)
	movlw	20
	movwf	CS_MENU_MESSAGE_CTR
CS_MENU_Handler_Tick_Msg_NoReq
	movlw	CS_MENU_HEADER_MSG_FUNC_OFFSET
	rgoto	CS_MENU_Hlp_IndirectMenuHdrCall


;; --------------------------------------------------------------------------
;;  Install menu structure, set menu pos to 0
;; --------------------------------------------------------------------------
CS_MENU_TABLE_Install
	;; store pointer in CS_MENU_HEADER_TBLPTR[LH]
	movff	TBLPTRL, CS_MENU_HEADER_TBLPTRL
	movff	TBLPTRH, CS_MENU_HEADER_TBLPTRH
	clrf	CS_MENU_POS
	return

;; --------------------------------------------------------------------------
;;  initialize the menu, print menu page name
;; --------------------------------------------------------------------------
CS_MENU_TABLE_Init
	;; clear screen
	call	MIOS_LCD_Clear

	;; restore pointer from CS_MENU_HEADER_TBLPTR[LH]
	movff	CS_MENU_HEADER_TBLPTRL, TBLPTRL
	movff	CS_MENU_HEADER_TBLPTRH, TBLPTRH

	;; read number of menu entries and save it in CS_MENU_ENTRIES
	tblrd*+			; read number of entries
	movff	TABLAT, CS_MENU_ENTRIES
	tblrd*+			; read dummy byte

	;; copy number to CS_MENU_PARAMETER_MAX_[LH]
	decf	CS_MENU_ENTRIES, W
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; copy menu pos to CS_MENU_PARAMETER_[LH]
	movf	CS_MENU_POS, W
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; install hook to "value changed" function
	TABLE_ADDR CS_MENU_PosUpdate
	call	CS_MENU_SetMenuParameterHook

	;; finally goto this function for an initial write
	goto	CS_MENU_PosUpdate

;; --------------------------------------------------------------------------
;;  this function is called when the menu pos has been changed
;; --------------------------------------------------------------------------
CS_MENU_PosUpdate
	;; copy CS_MENU_PARAMETER_L to CS_MENU_POS
	movff	CS_MENU_PARAMETER_L, CS_MENU_POS

	;; print header string at first LCD line, position 0
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print menu name
	movlw	CS_MENU_HEADER_NAME_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuHdrCall

	;; calc pointer to entry depending on menu position
	;; restore pointer from CS_MENU_HEADER_TBLPTR[LH]
	movff	CS_MENU_HEADER_TBLPTRL, TBLPTRL
	movff	CS_MENU_HEADER_TBLPTRH, TBLPTRH

	;; add len
	movlw	CS_MENU_HEADER_LEN
	TABLE_ADD_W

	;; multiply entry len with menu pos and add to pointer
	movf	CS_MENU_POS, W
	mullw	CS_MENU_ENTRY_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	
	;; store pointer in CS_MENU_ENTRY_TBLPTR[LH], so that other functions can reference
	;; to the entries
	movff	TBLPTRL, CS_MENU_ENTRY_TBLPTRL
	movff	TBLPTRH, CS_MENU_ENTRY_TBLPTRH

	;; print second line (14 chars) at second LCD line, position 0
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	0x0e
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	rgoto	CS_MENU_PrintArrows

;; --------------------------------------------------------------------------
;;  Snapshot Button Handler (could also be called via MIDI for MBLink)
;;  if in menu: call snapshot function which has been specified in menu header
;;  if in menu page: call CS_M_*Snapshot
;; --------------------------------------------------------------------------
CS_MENU_Snapshot
	;; stop current message
	rcall	CS_MENU_StopMessage
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset cursor
	rcall	CS_MENU_ResetCursor

	;; branch if in menu
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Snapshot_Menu

	;; within menu page
CS_MENU_Snapshot_M
	;; branch to CS_M_*Snapshot
	movlw	CS_MENU_ENTRY_SNAP_FUNC_OFFSET
	rgoto	CS_MENU_Hlp_IndirectMenuPageCall

	;; within menu: branch to MB64 specific snapshot routine
CS_MENU_Snapshot_Menu
	movlw	CS_MENU_HEADER_SNAP_FUNC_OFFSET
	rgoto	CS_MENU_Hlp_IndirectMenuHdrCall


;; --------------------------------------------------------------------------
;;  Left Button Handler - decrement menu parameter
;; --------------------------------------------------------------------------
CS_MENU_Left
	movlw	-1
	rgoto	CS_MENU_AddMenuParameter

;; --------------------------------------------------------------------------
;;  Right Button Handler - increment menu parameter
;; --------------------------------------------------------------------------
CS_MENU_Right
	movlw	1
	rgoto	CS_MENU_AddMenuParameter

;; --------------------------------------------------------------------------
;;  Encoder Handler - increment/decrement the menu parameter
;;  expects incrementer in WREG
;; --------------------------------------------------------------------------
CS_MENU_Enc
	rgoto	CS_MENU_AddMenuParameter

;; --------------------------------------------------------------------------
;;  Exec Button Handler (could also be called via MIDI for MBLink)
;;  if in menu: enter menu page and call CS_M_*Init
;;  if in menu page: call CS_M_*Exit and leave menu page
;; --------------------------------------------------------------------------
CS_MENU_Exec
	;; stop current message
	rcall	CS_MENU_StopMessage
	;; request reinit of screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; reset cursor
	rcall	CS_MENU_ResetCursor

	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Exec_Menu

	;; within menu page
CS_MENU_Exec_M
	;; call CS_M_*Exit
	movlw	CS_MENU_ENTRY_EXIT_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuPageCall
	;; clear CS_STAT_WITHIN_PAGE flag
	bcf	CS_STAT, CS_STAT_WITHIN_PAGE
	rgoto	CS_MENU_Exec_End

	;; in menu
CS_MENU_Exec_Menu
	;; set CS_STAT_WITHIN_PAGE flag
	bsf	CS_STAT, CS_STAT_WITHIN_PAGE

CS_MENU_Exec_End
	return

;; --------------------------------------------------------------------------
;;  This function installs the menu parameter hook which is called after
;;  the parameter has been changed
;;  IN: pointer to hook function in TBLPTR[LH]
;; --------------------------------------------------------------------------
CS_MENU_SetMenuParameterHook
	movff	TBLPTRL, CS_MENU_PARAMETER_HOOK_L
	movff	TBLPTRH, CS_MENU_PARAMETER_HOOK_H
	return

;; --------------------------------------------------------------------------
;;  This function executes the menu parameter hook
;; --------------------------------------------------------------------------
CS_MENU_ExecMenuParameterHook
	;; exit if pointer is zero
	movf	CS_MENU_PARAMETER_HOOK_L, W
	iorwf	CS_MENU_PARAMETER_HOOK_H, W
	skpnz
	return

	IRQ_DISABLE
	push
	movf	CS_MENU_PARAMETER_HOOK_L, W
	movwf	TOSL
	movf	CS_MENU_PARAMETER_HOOK_H, W
	movwf	TOSH
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  This function installs the general purpose buttons hook which is called
;;  when one of the 16 GP buttons is pressed (can be used within menus
;;  for special select functions)
;;  IN: pointer to hook function in TBLPTR[LH]
;; --------------------------------------------------------------------------
CS_MENU_SetMenuGPCallbackHook
	movff	TBLPTRL, CS_MENU_GP_CALLBACK_HOOK_L
	movff	TBLPTRH, CS_MENU_GP_CALLBACK_HOOK_H
	return

;; --------------------------------------------------------------------------
;;  This function returns a value != 0 if the hook has been installed, 0x00 if not
;; --------------------------------------------------------------------------
CS_MENU_CheckMenuGPCallbackHook
	movf	CS_MENU_GP_CALLBACK_HOOK_L, W
	iorwf	CS_MENU_GP_CALLBACK_HOOK_H, W
	return

;; --------------------------------------------------------------------------
;;  This function executes the menu parameter hook
;; --------------------------------------------------------------------------
CS_MENU_ExecMenuGPCallbackHook
	rcall	CS_MENU_ExecMenuGPCallbackHook2
	rgoto	CS_MENU_ResetCursor

CS_MENU_ExecMenuGPCallbackHook2
	IRQ_DISABLE
	push
	movf	CS_MENU_GP_CALLBACK_HOOK_L, W
	movwf	TOSL
	movf	CS_MENU_GP_CALLBACK_HOOK_H, W
	movwf	TOSH
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  This function installs the delayed callback hook used e.g. by the 
;;  snapshot and bank format button
;;  IN: pointer to hook function in TBLPTR[LH]
;;      delay value in WREG, deinstall if zero
;;  USES: MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_SetDelayedCallbackHook
	andlw	0xff		; fix STATUS
	bz	CS_MENU_SetDelayedCallbackHook0

	;; install hook
	movwf	CS_MENU_DEL_CALLBACK_CTR
	bsf	CS_STAT, CS_STAT_DELAYED_CALLBACK

	movff	TBLPTRL, CS_MENU_DEL_CALLBACK_HOOK_L
	movff	TBLPTRH, CS_MENU_DEL_CALLBACK_HOOK_H

	return

	;; deinstall hook
CS_MENU_SetDelayedCallbackHook0
	clrf	CS_MENU_DEL_CALLBACK_HOOK_L
	clrf	CS_MENU_DEL_CALLBACK_HOOK_H
	bcf	CS_STAT, CS_STAT_DELAYED_CALLBACK
	return

;; --------------------------------------------------------------------------
;;  This function executes the delayed callback hook
;;  IN: pointer to hook function in TBLPTR[LH]
;; --------------------------------------------------------------------------
CS_MENU_ExecDelayedCallbackHook
	;; clear request
	bcf	CS_STAT, CS_STAT_DELAYED_CALLBACK

	;; execute function if pointer != 0
	movf	CS_MENU_DEL_CALLBACK_HOOK_L, W
	iorwf	CS_MENU_DEL_CALLBACK_HOOK_H, W
	skpnz
	return

	IRQ_DISABLE
	push
	movf	CS_MENU_DEL_CALLBACK_HOOK_L, W
	movwf	TOSL
	movf	CS_MENU_DEL_CALLBACK_HOOK_H, W
	movwf	TOSH
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  This function adds WREG to CS_MENU_PARAMETER_[LH] and saturates
;;  if the max value has been reached
;;  IN: add value in WREG
;;  OUT: result in CS_MENU_PARAMETER_L
;;       branches to menu parameter hook and requests screen update
;; --------------------------------------------------------------------------
CS_MENU_AddMenuParameter
	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, CS_MENU_PARAMETER_L
	;; set max value
	movff	CS_MENU_PARAMETER_MAX_L, MIOS_PARAMETER1	; low-byte of max value
	movff	CS_MENU_PARAMETER_MAX_H, MIOS_PARAMETER2	; high-byte of max value
	;; incrementer is already in WREG
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; do nothing if flag not set
	btfss	MIOS_PARAMETER1, 0
	return

	;; stop current message
	rcall	CS_MENU_StopMessage

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; reset cursor
	rcall	CS_MENU_ResetCursor

	;; branch to hook if value has been changed
	goto	CS_MENU_ExecMenuParameterHook


;; --------------------------------------------------------------------------
;;  This function prints two arrows depending on the current 
;;  CS_MENU_PARAMETER_[LH] value and the specified max value
;; --------------------------------------------------------------------------
CS_MENU_PrintArrows
	;; print left cursor marker depending on position
	movf	CS_MENU_PARAMETER_L, W
	bz	CS_MENU_PrintArrows_NL
CS_MENU_PrintArrows_L
	movlw	0x00		; special char 0: "<"
	btfsc	CS_STAT, CS_STAT_DISPLAY_GLCD
	addlw 8
	rgoto	CS_MENU_PrintArrows_LCont
CS_MENU_PrintArrows_NL
	movlw	' '
CS_MENU_PrintArrows_LCont
	call	MIOS_LCD_PrintChar

	;; print right cursor marker depending on position
	movf	CS_MENU_PARAMETER_MAX_L, W
	cpfslt	CS_MENU_PARAMETER_L, ACCESS
	rgoto CS_MENU_PrintArrows_NR
CS_MENU_PrintArrows_R
	movlw	0x01		; special char 1: ">"
	btfsc	CS_STAT, CS_STAT_DISPLAY_GLCD
	addlw 8
	rgoto	CS_MENU_PrintArrows_RCont
CS_MENU_PrintArrows_NR
	movlw	' '
CS_MENU_PrintArrows_RCont
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This help function jumps to the functiion refered by a menu header
;;  table entry in WREG.
;; --------------------------------------------------------------------------
CS_MENU_Hlp_IndirectMenuHdrCall
	;; calc pointer to offset
	addwf	CS_MENU_HEADER_TBLPTRL, W
	movwf	TBLPTRL
	movlw	0x00
	addwfc	CS_MENU_HEADER_TBLPTRH, W
	movwf	TBLPTRH

	;; call function
	goto	MIOS_HLP_IndirectJump

;; --------------------------------------------------------------------------
;;  This help function jumps to the function refered by a menu page 
;;  table entry offset in WREG.
;; --------------------------------------------------------------------------
CS_MENU_Hlp_IndirectMenuPageCall
	;; calc pointer to offset
	addwf	CS_MENU_ENTRY_TBLPTRL, W
	movwf	TBLPTRL
	movlw	0x00
	addwfc	CS_MENU_ENTRY_TBLPTRH, W
	movwf	TBLPTRH

	;; call function
	goto	MIOS_HLP_IndirectJump

;; --------------------------------------------------------------------------
;;  This help can be used to display a flashing cursor
;;  IN: cursor pos in TMP1
;;      number of spaces (cursor width) in TMP2
;; --------------------------------------------------------------------------
CS_MENU_CursorHandler
	;; flashing cursor: print space(s)
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_CursorHandler_NoSpc
	movf	TMP1, W
	call	MIOS_LCD_CursorSet
CS_MENU_CursorHandler_SpcLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP2, F
	rgoto	CS_MENU_CursorHandler_SpcLoop
CS_MENU_CursorHandler_NoSpc

	;; if CLCD: enable blinking CLCD cursor
	BRA_IFSET CS_STAT, CS_STAT_DISPLAY_GLCD, ACCESS, CS_MENU_CursorHandler_NoCLCD
	movf	TMP1, W
	call	MIOS_LCD_CursorSet
	movlw	0x0e
	call	MIOS_LCD_Cmd
CS_MENU_CursorHandler_NoCLCD

	;; exit
	return

;; --------------------------------------------------------------------------
;;  This function resets the cursor and updates the LED values
;; --------------------------------------------------------------------------
CS_MENU_ResetCursor
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_MENU_CURSOR_CTR
	;; set "CS_STAT_CURSOR_FLASH" bit (see cs_menu_timer.inc for the handling)
	bsf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; update GP leds (if enabled)
	rgoto	CS_MENU_LED_OverlayHandler

;; --------------------------------------------------------------------------
;;  This function overlays two DOUT shift registers if the GP hook has been installed
;; --------------------------------------------------------------------------
CS_MENU_LED_OverlayHandler
	rcall	CS_MENU_CheckMenuGPCallbackHook
	bz	CS_MENU_LED_OverlayHandler_End
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, MIOS_PARAMETER1
	BRA_IFSET MB_STAT2, MB_STAT2_DONT_FLASH_L_GP_DOUT, ACCESS, CS_MENU_LED_OverlayHandler_NoFlL	
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf MIOS_PARAMETER1
CS_MENU_LED_OverlayHandler_NoFlL
	movlw	DEFAULT_GP_DOUT_SR0-1
#if DEFAULT_GP_DOUT_SR0 > 0
	call	MIOS_DOUT_SRSet
#endif
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, MIOS_PARAMETER1
	BRA_IFSET MB_STAT2, MB_STAT2_DONT_FLASH_R_GP_DOUT, ACCESS, CS_MENU_LED_OverlayHandler_NoFlR
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf MIOS_PARAMETER1
CS_MENU_LED_OverlayHandler_NoFlR
	movlw	DEFAULT_GP_DOUT_SR1-1
#if DEFAULT_GP_DOUT_SR1 > 0
	call	MIOS_DOUT_SRSet
#endif
CS_MENU_LED_OverlayHandler_End
	return

;; --------------------------------------------------------------------------
;;  This function is used to change to another menu page directly
;;  IN:	 page number in WREG
;; --------------------------------------------------------------------------
CS_MENU_ChangePage
	;; save new page number
	movwf	CS_MENU_NEW_POS
	;; saturate to avoid invalid jumps
	decf	CS_MENU_ENTRIES, W
	cpfsgt	CS_MENU_NEW_POS, ACCESS
	rgoto CS_MENU_ChangePage_Ok
	movwf	CS_MENU_NEW_POS
CS_MENU_ChangePage_Ok
	;; if already in page, call exit function
	RCALL_IFSET CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Exec
	;; install menu structure
	TABLE_ADDR CS_MENU_TABLE_ROOT
	rcall	CS_MENU_TABLE_Install
	;; init CS_STAT: within page, request re-init)
	movlw	(1 << CS_STAT_DISPLAY_INIT_REQ) | (1 << CS_STAT_WITHIN_PAGE)
	movwf	CS_STAT
	;; init CS_MENU_POS
	movff	CS_MENU_NEW_POS, CS_MENU_POS
	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This function holds a message
;; --------------------------------------------------------------------------
CS_MENU_HoldMessage
	;; this is a hack which ensures that values within a menu will be saved
	;; by the CS_M_*Exit function, before the CS_M_*Init function will be
	;; called
	movf	CS_MENU_POS, W
	rcall	CS_MENU_ChangePage
	;; hold message
	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	rgoto	CS_MENU_StartMessage_Cont

;; --------------------------------------------------------------------------
;;  This function starts a message
;; --------------------------------------------------------------------------
CS_MENU_StartMessage
	;; this is a hack which ensures that values within a menu will be saved
	;; by the CS_M_*Exit function, before the CS_M_*Init function will be
	;; called
	movf	CS_MENU_POS, W
	rcall	CS_MENU_ChangePage
	;; don't hold message
	bcf	CS_STAT, CS_STAT_MESSAGE_HOLD

CS_MENU_StartMessage_Cont
	;; remove request of display init
	bcf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; request message
	bsf	CS_STAT, CS_STAT_MESSAGE_REQ
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  This function stops a message
;; --------------------------------------------------------------------------
CS_MENU_StopMessage
	;; stop MIOS message
	call	MIOS_LCD_MessageStop

	;; don't re-init screen if counter already zero
	movf	CS_MENU_MESSAGE_CTR, W
	skpnz
	return

	;; clear request
	bcf	CS_STAT, CS_STAT_MESSAGE_REQ
	;; clear message counter
	clrf	CS_MENU_MESSAGE_CTR
	;; request initialization of screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return
