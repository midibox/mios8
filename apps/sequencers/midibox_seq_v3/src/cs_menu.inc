; $Id$
;
; Control Surface Menu Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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
	;; others are not used

;; --------------------------------------------------------------------------
;;  this function initializes the menu interface
;; --------------------------------------------------------------------------
CS_MENU_Init
	;; clear CS stat
	clrf	CS_STAT

	;; not for MIDIboxSEQ - button handler is different
#if 0
	;; enable autorepeat for left and right button if datawheel not enabled
#if DEFAULT_ENC_DATAWHEEL == -1
	movlw	DEFAULT_DIN_MENU_LEFT
	call	MIOS_DIN_PinAutoRepeatEnable
	movlw	DEFAULT_DIN_MENU_RIGHT
	call	MIOS_DIN_PinAutoRepeatEnable
#endif
#endif

	;; default random generator setting
	movlw	20
	movwf	CS_RANDOM_INTENSITY_LA
	movlw	15		; all
	movwf	CS_RANDOM_INTENSITY_TA

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
	;; clear screen(s)
	call	SEQ_LCD_Clear
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
CS_MENU_Handler_NoCtrReset

	;; check if display initialization has been requested (CS_STAT.CS_STAT_DISPLAY_INIT_REQ set)
	BRA_IFCLR CS_STAT, CS_STAT_DISPLAY_INIT_REQ, ACCESS, CS_MENU_Handler_NoInit

CS_MENU_Handler_Init
	;; clear request flag
	bcf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; force an display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear message flags and counter
	bcf	CS_STAT, CS_STAT_MESSAGE_HOLD
	bcf	CS_STAT, CS_STAT_MESSAGE_REQ
	clrf	CS_MENU_MESSAGE_CTR
	;; set menu offsets
	call	SEQ_LCD_SetMenuOffsets

	;; clear don't flash flags (MBSEQ variation)
#ifdef MB_STAT_DONT_FLASH_L_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT
#endif
#ifdef MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
#endif

	;; clear GP hook (has to be re-initialized by user init routine)
	clrf	TBLPTRL
	clrf	TBLPTRH
	rcall	CS_MENU_SetMenuGPCallbackHook

	;; clear item counter - used for optimized refresh handling
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; branch depending on CS_STAT_WITHIN_PAGE flag
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Handler_Init_Menu
CS_MENU_Handler_Init_Page
	;; clear parameter entry hook
	clrf	CS_MENU_PARAMETER_HOOK_L
	clrf	CS_MENU_PARAMETER_HOOK_H
	;; clear parameter table
	clrf	CS_MENU_PARAMETER_TABLE_L
	clrf	CS_MENU_PARAMETER_TABLE_H
	;; don't allow fast display updates by default
	bcf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE
	;; calc pointer to entry depending on menu position
	rcall	CS_MENU_CalcEntryPtr
	;; branch to CS_M_*Init
	movlw	CS_MENU_ENTRY_INIT_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuPageCall
	rgoto	CS_MENU_Handler_Init_End

CS_MENU_Handler_Init_Menu
	;; upload charset for menu
	movlw	CS_MENU_CHARSET_ARROWS
	call	CS_SPECIAL_CHARS_Init

	;; init menu table and print first line
	rcall	CS_MENU_TABLE_Init

	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
CS_MENU_Handler_Init_End
	
CS_MENU_Handler_NoInit

CS_MENU_Handler_Tick

	;; for the optimized display handling: refresh items step by step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_MENU_Handler_Tick_Refresh

	;; clear fast update flag (will be set on next request)
	bcf	CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE

	;; continue if display update has been requested
	BRA_IFSET CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ, ACCESS, CS_MENU_Handler_Tick_Req

	;; check if fast update has been requested for realtime informations, else exit
	btfss	CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE_REQ
	return
	;; clear request
	bcf	CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE_REQ
	;; exit if fast updates not allowed
	btfss	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE
	return

	;; call menu tick return with FAST_DISPLAY_UPDATE flag set
	bsf	CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE
	rgoto	CS_MENU_Handler_Tick_Refresh

CS_MENU_Handler_Tick_FastReq

CS_MENU_Handler_Tick_Req
	;; clear request
	bcf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; update LEDs
	movlw	CS_MENU_HEADER_LED_FUNC_OFFSET
	rcall	CS_MENU_Hlp_IndirectMenuHdrCall

	;; modification for MIDIboxSEQ - overlay registers are handled by seq_led.inc
#if 0
	;; if GP hook has been installed, overlay 2 LED shiftregisters
	rcall	CS_MENU_LED_OverlayHandler
#endif

CS_MENU_Handler_Tick_Refresh
	;; if message has been requested: print it so long as counter > 0
	BRA_IFSET CS_STAT, CS_STAT_MESSAGE_REQ, ACCESS, CS_MENU_Handler_Tick_Msg
	;; so long as message counter > 0, print this message
	movf	CS_MENU_MESSAGE_CTR, W
	bnz	CS_MENU_Handler_Tick_Msg

	;; if CS_STAT_WITHIN_PAGE flag cleared, we are in the menu - nothing else to do
	btfss	CS_STAT, CS_STAT_WITHIN_PAGE
	return

	;; else call CS_M_*Tick
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
	rcall	CS_MENU_CalcEntryPtr

	;; print second line (14 chars) at second LCD line, position 0
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	0x0e
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	rgoto	CS_MENU_PrintArrows

;; --------------------------------------------------------------------------
;;  This function calculates the pointer to the entry table
;; --------------------------------------------------------------------------
CS_MENU_CalcEntryPtr
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

	return

;; --------------------------------------------------------------------------
;;  Select Button Handler (could also be called via MIDI for MBLink)
;;  if in menu: call select function which has been specified in menu header
;;  if in menu page: call CS_M_*Select
;; --------------------------------------------------------------------------
CS_MENU_Select
	;; stop current message
	rcall	CS_MENU_StopMessage

	;; branch if in menu
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, CS_MENU_Select_Menu

	;; within menu page
CS_MENU_Select_M
	;; branch to CS_M_*Select
	movlw	CS_MENU_ENTRY_SEL_FUNC_OFFSET
	rgoto	CS_MENU_Hlp_IndirectMenuPageCall

	;; within menu: branch to MB specific select routine
CS_MENU_Select_Menu
	movlw	CS_MENU_HEADER_SEL_FUNC_OFFSET
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
	;; reset cursor counter
	rcall	CS_MENU_ResetCursorCtr

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
;;  This function installs the menu parameter table which is taken for the
;;  cursor control functions. It contains the:
;;     o edit value (CC or -1: track number - if 7th bit set, then biased value)
;;     o max value
;;     o cursor position
;;     o parameter length
;;     o pointer to function which updates the parameter
;;  IN: pointer to table in TBLPTR[LH]
;;      number of parameters in WREG
;;  OUT: edit value in CS_MENU_PARAMETER_L, _H cleared
;;       max value in CS_MENU_PARAMETER_MAX_L, _H cleared
;;       LCD cursor pos in CS_MENU_CURSOR_LCD_POS
;;       LCD cursor length in CS_MENU_CURSOR_LCD_WIDTH
;;       Pointer to menu parameter hook CS_MENU_PARAMETER_HOOK_[LH]
;;  USES: TBLPTR[LH], TMP1
;; --------------------------------------------------------------------------
	;; this function defines the parameter entry
CS_MENU_PARAMETER_ENTRY MACRO value, max, cursor, width, hook
	db	value & 0xff, max & 0xff
	db	cursor & 0xff, width & 0xff
	dw	hook & 0xffff
	ENDM

CS_MENU_SetMenuParameterTable
	;; store number of parameters (-1)
	addlw	-1
	movwf	CS_MENU_CURSOR_POS_MAX

	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	movwf CS_MENU_CURSOR_POS

	;; multiply TBLPTR[LH] with CURSOR_POS * 6 (table width)
	movf	CS_MENU_CURSOR_POS, W
	mullw	6
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; store table pointer
	movff	TBLPTRL, CS_MENU_PARAMETER_TABLE_L
	movff	TBLPTRH, CS_MENU_PARAMETER_TABLE_H

	;; read value
	tblrd*+
	incf	TABLAT, W
	bnz	CS_MENU_ReadCursorParameter_CC
CS_MENU_ReadCursorParameter_Trk
	call	CS_M_HLP_GetSelectedTrk
	rgoto	CS_MENU_ReadCursorParameter_C
CS_MENU_ReadCursorParameter_CC
	;; save TABLAT in TMP1 for later
	movf	TABLAT, W
	movwf	TMP1
	;; get CC
	call	CS_M_HLP_TrkCC_Get
	;; if TMP1[7] set, then biased value
	BRA_IFCLR TMP1, 7, ACCESS, CS_MENU_ReadCursorParameter_C
	addlw	8
	andlw	0x0f
CS_MENU_ReadCursorParameter_C
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	;; (CC function has overwritten TBLPTR[LH]!)
	movff	CS_MENU_PARAMETER_TABLE_L, TBLPTRL
	movff	CS_MENU_PARAMETER_TABLE_H, TBLPTRH
	tblrd*+			; (dummy read)

	;; read max value
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; read cursor pos and len
	tblrd*+
	movff	TABLAT, CS_MENU_CURSOR_LCD_POS
	tblrd*+
	movff	TABLAT, CS_MENU_CURSOR_LCD_WIDTH

	;; install the menu parameter hook
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_HOOK_L
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_HOOK_H

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
	rgoto	CS_MENU_ResetCursorCtr

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
;;  select and bank format button
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

#if 0
	;; clears LCD screen! Only required if a modified menu parameter has nothing to do with the EDIT screen content

	;; stop current message
	rcall	CS_MENU_StopMessage
#endif

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; reset cursor counter
	rcall	CS_MENU_ResetCursorCtr

	;; branch to hook if value has been changed
	goto	CS_MENU_ExecMenuParameterHook


;; --------------------------------------------------------------------------
;;  This function increments the cursor position and wraps at CURSOR_POS_MAX
;;  IN:  button status in MIOS_PARAMETER2
;;  OUT: result in CS_MENU_CURSOR_POS
;;       requests screen re-initialization
;; --------------------------------------------------------------------------
CS_MENU_IncCursorPos
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor pos, wrap at max value
	incf	CS_MENU_CURSOR_POS, F
	;; 	rgoto	CS_MENU_NewCursorPos

;; --------------------------------------------------------------------------
;;  This function sets the cursor to a new position
;;  IN:  button status in MIOS_PARAMETER2
;;  OUT: result in CS_MENU_CURSOR_POS
;;       requests screen re-initialization
;; --------------------------------------------------------------------------
CS_MENU_NewCursorPos
	movf	CS_MENU_CURSOR_POS_MAX, W
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_MENU_IncCursorPos_NoReset
	clrf	CS_MENU_CURSOR_POS
CS_MENU_IncCursorPos_NoReset
	;; force an display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; and exit
	return


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
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
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
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
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
;;  IN: cursor pos in CS_MENU_CURSOR_LCD_POS
;;      number of spaces (cursor width) in CS_MENU_CURSOR_LCD_WIDTH
;;  USES: PRODL
;; --------------------------------------------------------------------------
CS_MENU_CursorHandler
	;; flashing cursor: print space(s)
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_CursorHandler_NoSpc
	movf	CS_MENU_CURSOR_LCD_POS, W
	call	MIOS_LCD_CursorSet
	movff	CS_MENU_CURSOR_LCD_WIDTH, PRODL
CS_MENU_CursorHandler_SpcLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	PRODL, F
	rgoto	CS_MENU_CursorHandler_SpcLoop
CS_MENU_CursorHandler_NoSpc

	;; print arrows and exit
	movlw	0x4e
	call	MIOS_LCD_CursorSet
	rgoto	CS_MENU_PrintArrows

;; --------------------------------------------------------------------------
;;  Similar to CS_MENU_CursorHandler, but with new behaviour optimized
;;  for large screen layout:
;;  This function prints spaces if the item (specified in WREG) is
;;  selected by cursor, and CS_STAT, CS_STAT_CURSOR_FLASH is cleared
;;  ZERO flag set if cursor flashed
;;  IN: Selected Item in WREG
;;      number of spaces (cursor width) in CS_MENU_CURSOR_LCD_WIDTH
;;  OUT: ZERO flag set if cursor flashed
;;  USES: PRODL
;; --------------------------------------------------------------------------
CS_MENU_CursorHandler2
	xorwf	CS_MENU_CURSOR_POS, W
	bnz	CS_MENU_CursorHandler2_NoSpc

	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_CursorHandler2_NoSpc

	movff	CS_MENU_CURSOR_LCD_WIDTH, PRODL
CS_MENU_CursorHandler2_SpcLoop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	PRODL, F
	rgoto	CS_MENU_CursorHandler2_SpcLoop
	andlw	0x00		; set ZERO flag
	return

CS_MENU_CursorHandler2_NoSpc
	iorlw	0xff		; clear ZERO flag
	return


;; --------------------------------------------------------------------------
;;  This function resets the cursor counter and updates the LED values
;; --------------------------------------------------------------------------
CS_MENU_ResetCursorCtr
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
#if 1
	;; modification for MIDIboxSEQ: overlay is handled by seq_led.inc
	return
#else
	rcall	CS_MENU_CheckMenuGPCallbackHook
	bz	CS_MENU_LED_OverlayHandler_End
	movff	CS_MENU_GP_OVERLAY_DOUT_SR0, MIOS_PARAMETER1
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_MENU_LED_OverlayHandler_NoFlL	
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf MIOS_PARAMETER1
CS_MENU_LED_OverlayHandler_NoFlL
	movlw	DEFAULT_GP_DOUT_SR0
	call	MIOS_DOUT_SRSet
	movff	CS_MENU_GP_OVERLAY_DOUT_SR1, MIOS_PARAMETER1
	BRA_IFSET MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_MENU_LED_OverlayHandler_NoFlR	
	btfss	CS_STAT, CS_STAT_CURSOR_FLASH
	clrf MIOS_PARAMETER1
CS_MENU_LED_OverlayHandler_NoFlR
	movlw	DEFAULT_GP_DOUT_SR1
	call	MIOS_DOUT_SRSet
CS_MENU_LED_OverlayHandler_End
	return
#endif

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
	;; clear cursor pos
	clrf	CS_MENU_CURSOR_POS
	;; init CS_MENU_POS
	movff	CS_MENU_NEW_POS, CS_MENU_POS
	;; clear in-menu message
	clrf	CS_IN_MENU_MSG
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

	;; clear screen(s)
 	call	SEQ_LCD_Clear
	;; clear request
	bcf	CS_STAT, CS_STAT_MESSAGE_REQ
	;; clear message counter
	clrf	CS_MENU_MESSAGE_CTR
	;; request initialization of screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return
