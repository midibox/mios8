; $Id$
;
; MBFM Control Surface Menu Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MBFM_Init during startup to reset the
;;  control surface
;; --------------------------------------------------------------------------
CS_MENU_Reset
	;; control surface: begin with main page
	movlw	CS_MENU_MAIN
	movwf	CS_MENU

	;; set view mode
#if CS_MENU_DEFAULT_VIEW_MODE
	bsf	CS_MENU_MODE, CS_MENU_MODE_VIEW
#else
	bcf	CS_MENU_MODE, CS_MENU_MODE_VIEW
#endif

	;; only if character LCD connected: init special characters
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, CS_MENU_Reset_NoCLCD
#if USE_CUSTOM_CHAR_HANDLER
	movlw	2
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
#else
	TABLE_ADDR CS_MENU_SPECIAL_CHARS	; (defined in special_characters.inc)
	call	MIOS_CLCD_SpecialCharsInit
#endif	
CS_MENU_Reset_NoCLCD

	;; set merger mode
	bcf	CS_MENU_MODE, CS_MENU_MODE_MERGER
	call	CS_MENU_UpdateMergerMode

	;; start with 1st instrument, InsOP button 1-4 depressed
	movlw	0xf1
	movwf	CS_MENU_SELECTED_INS_FLAGS
	movwf	CS_MENU_SELECTED_OP_FLAGS

	;; set speed for the CS encoders (1-5)
	;; set encoder speed to fast, divider value defined in main.asm
	movlw	0x01
	movwf	TMP1
CS_MENU_Reset_Loop
	movlw	CS_MENU_ENC_SPEED_VALUE	; speed divider 1
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_FAST	; fast mode
	movwf	MIOS_PARAMETER1
	movf	TMP1, W			; encoder number
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	5
	cpfseq	TMP1, ACCESS
	rgoto CS_MENU_Reset_Loop

	return

;; --------------------------------------------------------------------------
;;  entry function for a menu page update
;; --------------------------------------------------------------------------
CS_MENU_Handler
	;; call EEPROM write handler
	rcall	CS_MENU_Write_EEPROM

	;; check if new row/column has been selected in matrix
	call	CS_MENU_MATRIX_Update

	;; check if display initialization has been requested (CS_STAT.CS_STAT_DISPLAY_INIT_REQ set)
	BRA_IFCLR CS_STAT, CS_STAT_DISPLAY_INIT_REQ, ACCESS, CS_MENU_Handler_NoInit

CS_MENU_Handler_Init
	;; clear request flag
	bcf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; force an display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH

	;; if not in CFG page
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_CFG, ACCESS, CS_MENU_Handler_Init_CFGSkip
CS_MENU_Handler_Init_NotCFGSkip
	;; init menu table
	rcall	CS_MENU_Page_Init
	;; set encoder speed
	rcall	CS_MENU_EncSpeedSet
	;; set parameter
	rcall	CS_MENU_EncParameterSet
	;; set max position of cursor (or patch)
	rcall	CS_MENU_EncMaxSet
	;; clear item counter - used for optimized refresh handling
	clrf	CS_MENU_REFRESH_ITEM_CTR

CS_MENU_Handler_Init_CFGSkip
	;; clear screen
	call	MIOS_LCD_Clear

CS_MENU_Handler_Init_End

CS_MENU_Handler_NoInit
	;; don't continue if CS_SELECT_CTR > 0 (used by "CS messages")
	movf	CS_SELECT_CTR, W
	skpz
	return

	;; for the optimized display handling: refresh items step by step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_MENU_Handler_CheckMain

	;; check if a display update has been requested (CS_STAT.CS_STAT_DISPLAY_UPDATE_REQ set)
	;; do nothing if flag is zero
	btfss	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return
	;; clear request flag
	bcf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; update LEDs
	call	CS_MENU_LED_Update

CS_MENU_Handler_CheckMain
	;; branch if CFG page is displayed
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_CFG, ACCESS, CS_MENU_Page_Cfg

	;; branch if mainpage is displayed
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_Page_Main

	;; handle menu page
CS_MENU_Handler_MenuPage
	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd	

	;; feed the watchdog
	clrwdt

	;; branch depending on CS_MENU
	;; execute handler function
	;; calc pointer to function: CS_MENU_TABLES + (CS_MENU*CS_MENU_T_ENTRY_LEN) + CS_MENU_ENTRY_HANDLER_OFFSET
	TABLE_ADDR CS_MENU_TABLES+CS_MENU_T_ENTRY_HANDLER_OFFSET
	movf	CS_MENU, W
	mullw	CS_MENU_T_ENTRY_LEN
	movf	PRODL, W
	TABLE_ADD_W
	goto	MIOS_HLP_IndirectJump

;; --------------------------------------------------------------------------
;;  initialize the page
;; --------------------------------------------------------------------------
CS_MENU_Page_Init
	;; exit if in main page
	btfsc	CS_MENU, 7
	return

	;; get pointer to menu table depending on CS_MENU (CS_MENU_TABLES + CS_MENU*4)
	TABLE_ADDR CS_MENU_TABLES
	movf	CS_MENU, W
	mullw	CS_MENU_T_ENTRY_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; get address from table, store it in TBLPTR[LH]
	call	MIOS_HLP_AddressFromTable

	;; read number of menu entries and save it in CS_MENU_ENTRIES
	tblrd*+
	movff	TABLAT, CS_MENU_ENTRIES
	tblrd*+			; read dummy byte

	;; store pointer in CS_MENU_TABLE_0_[LH], so that other functions can reference
	;; to the entries
	movff	TBLPTRL, CS_MENU_TABLE_0_L
	movff	TBLPTRH, CS_MENU_TABLE_0_H

	;; clear cursor and page offset
	clrf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_PAGE_OFFSET

	return

	
;; --------------------------------------------------------------------------
;;  print the CFG page
;; --------------------------------------------------------------------------
CS_MENU_Page_Cfg
	;; print lower/upper line
	movf	CS_MENU_REFRESH_ITEM_CTR, W	
	bnz	CS_MENU_Page_Cfg_Step1
CS_MENU_Page_Cfg_Step0
	call	CS_MENU_CFG_PrintUL
	rgoto	CS_MENU_Page_Cfg_End
CS_MENU_Page_Cfg_Step1
	call	CS_MENU_CFG_PrintLL
	;; 	rgoto	CS_MENU_Page_Cfg_End

CS_MENU_Page_Cfg_End
	;; increment counter until last step reached
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	2-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	return
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return

;; --------------------------------------------------------------------------
;;  print the main page
;; --------------------------------------------------------------------------
CS_MENU_Page_Main
#if USE_CUSTOM_CHAR_HANDLER
	movlw	2
	call	MIOS_CUSTOM_CHAR_HANDLER_LoadCharset
#endif
	;; branch depending on view mode
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_VIEW, ACCESS, CS_MENU_Page_Main_Inst
CS_MENU_Page_Main_Full
	;; prepare line offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10

	;; step 0-3: print bank/patch numbers and channels
	movlw	4
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_MENU_Page_Main_Full_NoUL

CS_MENU_Page_Main_Full_UL
	;; set cursor
	movf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; set current instrument and get pointer to Ix record
	movff	CS_MENU_REFRESH_ITEM_CTR, MBFM_CURRENT_INSTRUMENT

	;; print bank and patch number
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_BANK
	movff	PLUSW1, MBFM_BANK
	movlw	MBFM_Ix_PATCH
	movff	PLUSW1, MBFM_PATCH
	movlw	MBFM_Ix_MIDI_CHANNEL
	movff	PLUSW1, TMP1
	rcall	CS_MENU_PrintBnkAndPatch

	;; print marker if channel selected
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_INS_FLAGS, W
	andlw	0x0f
	movlw	' '
	skpz
	movlw	'*'
	call	MIOS_LCD_PrintChar

	;; print MIDI channel
	movlw	'C'
	call	MIOS_LCD_PrintChar
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_MIDI_CHANNEL
	incf	PLUSW1, W
	call	MIOS_LCD_PrintBCD2

	;; if not the last line: print divider
	movlw	3
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_MENU_Page_Main_Full_End
	movlw	'|'
	call	MIOS_LCD_PrintChar

	rgoto	CS_MENU_Page_Main_Full_End


	;; step 4-7: print patch names
CS_MENU_Page_Main_Full_NoUL
	;; set cursor
	movf	PRODL, W
	addlw	0x40
	call	MIOS_LCD_CursorSet

	;; set current instrument
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4
	movff	WREG, MBFM_CURRENT_INSTRUMENT

	;; print patch (9 characters only!)
	movlw	9
	call	MBFM_LCD_PrintPatch
	
	;; if not the last line: print divider
	movlw	7
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_MENU_Page_Main_Full_End	
	movlw	'|'
	call	MIOS_LCD_PrintChar

CS_MENU_Page_Main_Full_End
	;; increment counter until last step reached
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	8
	skpnz
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return

;; --------------------------------------------------------------------------
;; instrument view of main page
;; 0123456789012345678901234567890123456789
;; Ens.|I|Ptch|Name            | Chn.|Audio
;; A 1 |1|A  1|xxxxxxxxxxxxxxxx| 12  |1234

CS_MENU_PAGE_MAIN_INSTR_STR
	STRING 40, 0x00, "Ens.|I|Ptch|Name            | Chn.|Audio"

#if CS_SAMMICH_FM
CS_MENU_Page_Main_Inst
	;; set instrument number
	call	CS_MENU_MS_GetInstrNumber

	;; branch depending on step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	CS_MENU_Page_Main_Inst_UL
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Ens
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Inst
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Name
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Chn
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Audio
	;; last step reached
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return

CS_MENU_Page_Main_Inst_UL
	;; print static content
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Ens
	;; set cursor position
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	;; print inverted E
	movlw	5
	call MIOS_LCD_PrintChar
	;; print ensemble bank number
	movf	MBFM_EBANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	;; and the ensemble (+1)
	movlw	' '
	call 	MIOS_LCD_PrintChar
	incf	MBFM_ENSEMBLE, W
	call	MIOS_LCD_PrintBCD2
	;; print space and divider
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Inst
	;; set cursor position
	movlw	0x40 + 6
	call	MIOS_LCD_CursorSet
	;; print inverted I
	movlw	6
	call	MIOS_LCD_PrintChar
	;; print instrument number
	SET_BSR	MBFM_BASE
	incf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	call	MIOS_LCD_PrintBCD1
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; print inverted P
	movlw	7
	call 	MIOS_LCD_PrintChar
	;; get MBFM_BANK/MBFM_PATCH
	call	CS_MENU_MS_GetInstrPatch
	;; print patch bank number
	movf	MBFM_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	;; and the patch number (+1)
	incf	MBFM_PATCH, W
	call	MIOS_LCD_PrintBCD3
	;; print divider
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Name
	;; set cursor position
	movlw	0x00
	call	MIOS_LCD_CursorSet
	;; print patch name (16 characters)
	movlw	16
	call	MBFM_LCD_PrintPatch
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Chn
	;; set cursor position
	movlw	0x00 + 17
	call	MIOS_LCD_CursorSet
	;; print inverted [C]
	movlw 	4
	call 	MIOS_LCD_PrintChar
	;; get the midi channel
	call	MBFM_INSTR_GetBase
	;; print MIDI channel of instrument
	movlw	MBFM_Ix_MIDI_CHANNEL
	incf	PLUSW1, W
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Audio
	;; set cursor position
	movlw	0x40 + 15
	call	MIOS_LCD_CursorSet
	;; print inverted A
	movlw	3
	call	MIOS_LCD_PrintChar
	call	MBFM_INSTR_GetBase
	;; print combined (OP12 | OP34) audio channels
	movlw	MBFM_Ix_ACHN_OP12
	movf	PLUSW1, W
	movwf	TMP1
	movlw	MBFM_Ix_ACHN_OP34
	movf	PLUSW1, W
	iorwf	TMP1, W
	call	CS_MENU_PRINT_1234
	;; 	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_End
	;; increment counter
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	return
	
;; end sammichFM
#else
	
CS_MENU_Page_Main_Inst
	;; set instrument number
	call	CS_MENU_MS_GetInstrNumber

	;; branch depending on step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	CS_MENU_Page_Main_Inst_UL
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Ens
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Inst
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Name
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Chn
	addlw	-1
	bz	CS_MENU_Page_Main_Inst_LL_Audio
	;; last step reached
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return

CS_MENU_Page_Main_Inst_UL
	;; print upper line
	TABLE_ADDR CS_MENU_PAGE_MAIN_INSTR_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_MENU_Page_Main_Inst_End


CS_MENU_Page_Main_Inst_LL_Ens
	;; set cursor position
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	;; print ensemble bank number
	movf	MBFM_EBANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	;; and the ensemble (+1)
	incf	MBFM_ENSEMBLE, W
	call	MIOS_LCD_PrintBCD2
	;; print space and divider
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces
	movlw	'|'
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Inst
	;; set cursor position
	movlw	0x40 + 5
	call	MIOS_LCD_CursorSet
	;; print instrument number
	SET_BSR	MBFM_BASE
	incf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	call	MIOS_LCD_PrintBCD1
	movlw	'|'
	call	MIOS_LCD_PrintChar
	;; get MBFM_BANK/MBFM_PATCH
	call	CS_MENU_MS_GetInstrPatch
	;; print patch bank number
	movf	MBFM_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	;; and the patch number (+1)
	incf	MBFM_PATCH, W
	call	MIOS_LCD_PrintBCD3
	;; print divider
	movlw	'|'
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Name
	;; set cursor position
	movlw	0x40 + 12
	call	MIOS_LCD_CursorSet
	;; print patch name (16 characters)
	movlw	16
	call	MBFM_LCD_PrintPatch
	;; print divider
	movlw	'|'	
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Chn
	;; set cursor position
	movlw	0x40 + 30
	call	MIOS_LCD_CursorSet
	call	MBFM_INSTR_GetBase
	;; print MIDI channel of instrument
	movlw	MBFM_Ix_MIDI_CHANNEL
	incf	PLUSW1, W
	call	MIOS_LCD_PrintBCD2
	;; print spaces and divider
	movlw	2
	rcall	CS_MENU_Hlp_PrintSpaces
	movlw	'|'
	call	MIOS_LCD_PrintChar
	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_LL_Audio
	;; set cursor position
	movlw	0x40 + 35
	call	MIOS_LCD_CursorSet
	call	MBFM_INSTR_GetBase
	;; print combined (OP12 | OP34) audio channels
	movlw	MBFM_Ix_ACHN_OP12
	movf	PLUSW1, W
	movwf	TMP1
	movlw	MBFM_Ix_ACHN_OP34
	movf	PLUSW1, W
	iorwf	TMP1, W
	call	CS_MENU_PRINT_1234
	;; 	rgoto	CS_MENU_Page_Main_Inst_End

CS_MENU_Page_Main_Inst_End
	;; increment counter
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	return

#endif ;; main page draw routines

;; --------------------------------------------------------------------------
;;  print the root page
;; --------------------------------------------------------------------------
CS_MENU_Page_Root
	;; print upper line on first step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_MENU_Page_Root_Refresh_NoUL

CS_MENU_Page_Root_Refresh_UL
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank number
	call	CS_MENU_MS_GetInstrPatch

	movf	MBFM_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; and the patch number (+1)
	incf	MBFM_PATCH, W
	call	MIOS_LCD_PrintBCD3

	;; one space
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces

	;; print full patch name of current instrument
	call	CS_MENU_MS_GetInstrNumber
	movlw	16
	call	MBFM_LCD_PrintPatch

CS_MENU_Page_Root_Refresh_NoUL
	;; print menu item name
	movlw	0x40
	call	CS_MENU_PrintILine

CS_MENU_Page_Root_Refresh_End
CS_MENU_Page_Drum_Refresh_End
	;; increment counter until last step reached
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	CS_MENU_DISPLAYED_ITEMS
	skpz
	return

	;; last step: print also the arrows and clear item counter
	movlw	0x40
	rcall	CS_MENU_PrintILine_Arw
	clrf	CS_MENU_REFRESH_ITEM_CTR
	return


;; --------------------------------------------------------------------------
;;  print the drum page
;; --------------------------------------------------------------------------
CS_MENU_Page_Drum
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_MENU_Page_Drum_Refresh_NoUL

CS_MENU_Page_Drum_Refresh_UL
	;; print "Drumset "
	TABLE_ADDR CS_MENU_PAGE_DRUM_STR
	call	MIOS_LCD_PrintString

	;; print bank and drumset number
	rcall	CS_MENU_PrintBnkAndDrumset
	
	;; TODO: what should be printed here?

CS_MENU_Page_Drum_Refresh_NoUL
	;; print menu item name
	movlw	0x40
	call	CS_MENU_PrintILine

	rgoto	CS_MENU_Page_Drum_Refresh_End

CS_MENU_PAGE_DRUM_STR
	STRING	8, 0x00, "Drumset "

;; --------------------------------------------------------------------------
;;  print a parameters page
;; --------------------------------------------------------------------------
CS_MENU_Page_Parameters
	;; if in MODIFY_NAME mode, print name editing screen
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_NAME, ACCESS, CS_MENU_Page_EditName

	;; print menu item name
	movlw	0x00
	call	CS_MENU_PrintILine

	;; print parameter value
	movlw	0x40
	call	CS_MENU_PrintPLine

	;; increment counter until last step reached
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	CS_MENU_DISPLAYED_ITEMS
	skpz
	return

	;; last step: print arrow, clear item counter and enable the cursor
	movlw	0x00
	rcall	CS_MENU_PrintILine_Arw
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; if not GLCD and in "modify parameter" mode, set cursor
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	return
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	return

	;; don't set cursor if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; set cursor to: 0x40 + 5*(CS_MENU_CURSOR_POS-CS_MENU_PAGE_OFFSET) + 2
	movf	CS_MENU_PAGE_OFFSET, W
	subwf	CS_MENU_CURSOR_POS, W
	mullw	5
	movf	PRODL, W
	addlw	0x40 + 2
	call	MIOS_LCD_CursorSet

	return


;; --------------------------------------------------------------------------
;;  print name editing screen
;; --------------------------------------------------------------------------
CS_MENU_Page_EditName
	;; get current instrument number
	call	CS_MENU_MS_GetInstrNumber

	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print full patch name
	movlw	16
	call	MBFM_LCD_PrintPatch

	;; fill rest with spaces
	movlw	40-16
	rcall	CS_MENU_Hlp_PrintSpaces

	;; flashing cursor: set space
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_Page_EditName_NoSpc
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces
CS_MENU_Page_EditName_NoSpc

	;; print button options @ position 0x40 (second line)
	TABLE_ADDR TEXT_CS_MENU_EDIT_OPTIONS
	call	MIOS_LCD_PrintString
	;; print 20 spaces (for 2x40 displays)
#if CS_MENU_DISPLAYED_ITEMS > 5
	movlw	20
	rcall	CS_MENU_Hlp_PrintSpaces
#endif

	;; if not GLCD, set cursor
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	return

	;; set cursor to: 0x00 + CS_MENU_NAME_POS
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet

	;; enable blinking CLCD cursor and exit
	movlw	0x0e
	goto	MIOS_LCD_Cmd

#if CS_SAMMICH_FM
									    ;;  <------------------>
TEXT_CS_MENU_EDIT_OPTIONS STRING 20, 0x40, " <    >   Del  Ins  "
										;;  1234 1234 1234 12345
#else
										;;  <------------------>
TEXT_CS_MENU_EDIT_OPTIONS STRING 20, 0x40, " <   >  Del Ins Clr "
										;;  12341234123412341234
#endif

;; --------------------------------------------------------------------------
;;  print the menu items line
;;  IN: pointer to menu entries in TBLPTR[LH]
;;      offset to first entry in CS_MENU_PAGE_OFFSET
;;      cursor offset (normaly 0x40 or 0x00) in WREG
;; --------------------------------------------------------------------------
CS_MENU_PrintILine
	;; determine cursor position and set cursor
	movwf	TMP1

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	CS_MENU_ENTRY_SLABEL_LEN+1
	movf	PRODL, W
	iorwf	TMP1, W
	call	MIOS_LCD_CursorSet

	;; move pointer to first visible entry to CS_MENU_ENTRY_TBLPTR[LH]
	rcall	CS_MENU_Hlp_GetFirstVisibleEntry

	;; add offset to selected item
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	CS_MENU_ENTRY_LEN
	movf	PRODL, W
	addwf	CS_MENU_ENTRY_TBLPTRL, F
	movf	PRODH, W
	addwfc	CS_MENU_ENTRY_TBLPTRH, F


	;; if  CS_MENU_PAGE_OFFSET + CS_MENU_LOOP_CTR - 1 >= CS_MENU_ENTRIES
	;;    -> print spaces
	movf	CS_MENU_PAGE_OFFSET, W
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	1
	cpfslt	CS_MENU_ENTRIES, ACCESS
	rgoto CS_MENU_PrintILine_Ok
	;; print 5 spaces
	movlw	5
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintILine_End
CS_MENU_PrintILine_Ok
	;; if CS_MENU_CURSOR_POS == CS_MENU_PAGE_OFFSET + CS_MENU_LOOP_CTR,
	;;    AND CS_STAT.CS_STAT_MODIFY_PARAMETER cleared
	;;    -> print spaces of cursor flash flag cleared,
	;;    -> else short label
#if CS_MENU_OLD_STYLE
	movf	CS_MENU_PAGE_OFFSET, W
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_MENU_PrintILine_COn
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_PrintILine_COn
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_PrintILine_COn
	;; print 5 spaces
	movlw	5
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintILine_End
CS_MENU_PrintILine_COn
#endif
	;; print short label (function switches to next table entry)
	rcall	CS_MENU_PrintSEntry
	;; print space
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces

CS_MENU_PrintILine_End
	return

;; --------------------------------------------------------------------------
;;  print the arrows at the end of an ILine
;;  IN: pointer to menu entries in TBLPTR[LH]
;;      offset to first entry in CS_MENU_PAGE_OFFSET
;; --------------------------------------------------------------------------
CS_MENU_PrintILine_Arw
	;; set cursor
	addlw	CS_MENU_DISPLAYED_ITEMS*5 - 1
	call	MIOS_LCD_CursorSet

	;; don't print arrow if in "modify parameter" mode
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_PrintILine_Arw_NL
CS_MENU_PrintILine_Arw_L
	;; print space, left or right arrow, depending on cursor position
	;; if CS_MENU_ENTRIES < CS_MENU_DISPLAYED_ITEMS, print space
	movlw	CS_MENU_DISPLAYED_ITEMS+1
	cpfslt	CS_MENU_ENTRIES, ACCESS
	rgoto CS_MENU_PrintILine_Arw_LNS
CS_MENU_PrintILine_Arw_LSp
	movlw	' '	
	rgoto	CS_MENU_PrintILine_Arw_C
CS_MENU_PrintILine_Arw_LNS
	;; if CS_MENU_PAGE_OFFSET == 0, print '>'
	movf	CS_MENU_PAGE_OFFSET, W
	bz	CS_MENU_PrintILine_Arw_LR
	;; if CS_MENU_PAGE_OFFSET != CS_MENU_ENTRIES-CS_MENU_DISPLAY_ITEMS, print '<>'
	movf	CS_MENU_ENTRIES, W
	addlw	-CS_MENU_DISPLAYED_ITEMS
	cpfseq	CS_MENU_PAGE_OFFSET, ACCESS
	rgoto CS_MENU_PrintILine_Arw_LLR
CS_MENU_PrintILine_Arw_LL
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; special char #0: '<'
	movlw 0x00
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; undocumented feature: GLCD special char #8: '<'	
	movlw 0x08
	rgoto	CS_MENU_PrintILine_Arw_C
CS_MENU_PrintILine_Arw_LR
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; special char #1: '>'
	movlw 0x01
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; undocumented feature: GLCD special char #9: '>'	
	movlw 0x09
	rgoto	CS_MENU_PrintILine_Arw_C
CS_MENU_PrintILine_Arw_LLR
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; special char #2: '<>'
	movlw 0x02
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; undocumented feature: GLCD special char #9: '>'	
	movlw 0x09
	rgoto	CS_MENU_PrintILine_Arw_C

CS_MENU_PrintILine_Arw_NL
	;; no: print space
	movlw	' '
	;; 	rgoto	CS_MENU_PrintILine_Arw_C
CS_MENU_PrintILine_Arw_C
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  print the menu parameters line
;;  IN: pointer to menu entries in TBLPTR[LH]
;;      offset to first entry in CS_MENU_PAGE_OFFSET
;;      cursor offset (normaly 0x40 or 0x00) in WREG
;; --------------------------------------------------------------------------
CS_MENU_PrintPLine
	;; determine cursor position and set cursor
	movwf	TMP1

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	CS_MENU_ENTRY_SLABEL_LEN+1
	movf	PRODL, W
	iorwf	TMP1, W
	call	MIOS_LCD_CursorSet

	;; move pointer to first visible entry to CS_MENU_ENTRY_TBLPTR[LH]
	rcall	CS_MENU_Hlp_GetFirstVisibleEntry

	;; add offset to selected item
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	CS_MENU_ENTRY_LEN
	movf	PRODL, W
	addwf	CS_MENU_ENTRY_TBLPTRL, F
	movf	PRODH, W
	addwfc	CS_MENU_ENTRY_TBLPTRH, F

	;; if  CS_MENU_PAGE_OFFSET + CS_MENU_REFRESH_ITEM_CTR - 1 >= CS_MENU_ENTRIES
	;;    -> print spaces
	movf	CS_MENU_PAGE_OFFSET, W
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	1
	cpfslt	CS_MENU_ENTRIES, ACCESS
	rgoto CS_MENU_PrintPLine_Ok
	;; EXTRA: not in "save" menu due to the long parameter name
	movf	CS_MENU, W
	xorlw	CS_MENU_SAVE
	bz	CS_MENU_PrintPLine_End
	;; print 5 spaces
	movlw	5
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintPLine_End
CS_MENU_PrintPLine_Ok
	;; if CS_MENU_CURSOR_POS == CS_MENU_PAGE_OFFSET + CS_MENU_LOOP_CTR
	;;    AND CS_STAT.CS_STAT_MODIFY_PARAMETER set
	;;    -> print spaces of cursor flash flag cleared,
	;;    -> else parameter
	movf	CS_MENU_PAGE_OFFSET, W
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_MENU_PrintPLine_COn
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_PrintPLine_COn
	BRA_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_PrintPLine_COn
	;; print 5 spaces
	movlw	4
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintPLine_CEnd
CS_MENU_PrintPLine_COn
	rcall	CS_MENU_PrintPEntry
	movlw	1
	rcall	CS_MENU_Hlp_PrintSpaces

CS_MENU_PrintPLine_CEnd
	;; (prevent space on last item)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	0x07
	skpnz
CS_MENU_PrintPLine_End
	return
CS_MENU_PrintPLine_End2	
	movlw	1
	rgoto	CS_MENU_Hlp_PrintSpaces

;; --------------------------------------------------------------------------
;;  print the short label of a menu item
;;  IN: pointer to menu entry in CS_MENU_ENTRY_TBLPTR[LH]
;;  USES: TMP1
;;  OUT: pointer to next entry    
;; --------------------------------------------------------------------------
CS_MENU_PrintSEntry
	;; calc offset to short entry and copy result to tablepointer
	movlw	CS_MENU_ENTRY_SLABEL_OFFSET
	rcall	CS_MENU_Hlp_GetPointer
	
	;; print short label
	movlw	CS_MENU_ENTRY_SLABEL_LEN
	movwf	TMP1
CS_MENU_PrintSEntry_Loop
	tblrd*+
	movf	TABLAT, W
	call	MIOS_LCD_PrintChar
	decfsz	TMP1, F
	rgoto	CS_MENU_PrintSEntry_Loop
	return


;; --------------------------------------------------------------------------
;;  print the parameter of a menu entry
;;  IN: pointer to menu entry in CS_MENU_ENTRY_TBLPTR[LH]
;; --------------------------------------------------------------------------
CS_MENU_PrintPEntry
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs
	;; PRINT ID in MIOS_PARAMETER1
	movf	MIOS_PARAMETER1, W
	goto	CS_MENU_PRINT_Handler


;; --------------------------------------------------------------------------
;;  prints the bank and patch number of the selected instrument
;;  IN: instrument number in MBFM_CURRENT_INSTRUMENT
;; --------------------------------------------------------------------------
CS_MENU_PrintBnkAndPatch
	movf	MBFM_BANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; and the patch number (+1)
	incf	MBFM_PATCH, W
	call	MIOS_LCD_PrintBCD3

	;; print "*" if bank doesn't exist
	movf	MBFM_BANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	movlw	' '
	skpnz
	movlw	'*'
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  prints the bank and drumset number
;; --------------------------------------------------------------------------
CS_MENU_PrintBnkAndDrumset
	;; print bank number
	movf	MBFM_DBANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	
	;; print drumset number
	incf	MBFM_DRUMSET, W
	call	MIOS_LCD_PrintBCD2

	;; print "*" if bank doesn't exist
	movf	MBFM_DBANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	movlw	' '
	skpnz
	movlw	'*'
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  prints the bank and ensemble number
;; --------------------------------------------------------------------------
CS_MENU_PrintBnkAndEnsemble
	;; print bank number
	movf	MBFM_EBANK, W
	addlw	'A'
	call	MIOS_LCD_PrintChar
	
	;; print ensemble number
	incf	MBFM_ENSEMBLE, W
	call	MIOS_LCD_PrintBCD2

	;; print "*" if bank doesn't exist
	movf	MBFM_EBANK, W
	call	MIOS_HLP_GetBitORMask
	andwf	MBFM_BANKSTICK_STATUS, W
	movlw	' '
	skpnz
	movlw	'*'
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;;  Update the encoder speed setting
;; --------------------------------------------------------------------------
CS_MENU_EncSpeedSet
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_EncSpeedSet_Main
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EncSpeedSet_ModVal
	
CS_MENU_EncSpeedSet_SelItem
#if CS_MENU_USE_DETENTED_ENCODER == 1
	;; "select menu item" mode: set encoder speed to "normal"
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#else
	;; "select menu item" mode: set encoder speed to slow, pre-divider 4
	movlw	3
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#endif

	;; in main page (where the patch number is selected with the encoder) set encoder speed to "normal"
CS_MENU_EncSpeedSet_Main
#if CS_MENU_USE_DETENTED_ENCODER == 1
	;; "select patch" mode: set encoder speed to "normal"
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#else
	;; "select patch" mode: set encoder speed to slow, pre-divider 4
	movlw	3
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#endif

CS_MENU_EncSpeedSet_ModVal
	;; "modify parameter" mode: set encoder speed to "fast"
	;; divider value depends directly on max value
	
	movf	CS_MENU_PARAMETER_MAX_L, W
	movwf	TMP1
	andlw	0xe0
	iorwf	CS_MENU_PARAMETER_MAX_H, W
	bnz	CS_MENU_EncSpeedSet_ModVal_GT1F
	;; max value <= 0x1f: set slow/normal speed
CS_MENU_EncSpeedSet_ModVal_LE1F
#if CS_MENU_USE_DETENTED_ENCODER == 1
	;; "select menu item" mode: set encoder speed to "normal"
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#else
	;; "select menu item" mode: set encoder speed to slow, pre-divider 4
	movlw	3
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet
#endif

CS_MENU_EncSpeedSet_ModVal_GT1F
	;; max value > 0x1f: set fast speed
	movf	CS_MENU_PARAMETER_MAX_H, W
	movwf	TMP2
	rlf	TMP1, F
	rlf	TMP2, F
	rlf	TMP1, F
	rlf	TMP2, F
	movf	TMP2, W
	andlw	0xfc		; saturate if value >= 4
	movlw	0x04
	skpz
	movwf	TMP2
	movf	TMP2, W

	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_FAST
	movwf	MIOS_PARAMETER1
	movlw	0x00
	goto	MIOS_ENC_SpeedSet


;; --------------------------------------------------------------------------
;;  Set max value of currently selected parameter (or menu pos or MBFM patch)
;; --------------------------------------------------------------------------
CS_MENU_EncMaxSet
	;; if in parameter selection mode, max value has been set by CS_MENU_EXEC_SelPar
	btfsc	CS_STAT, CS_STAT_MODIFY_PARAMETER
	return

	;; set CS_MENU_PARAMETER depending on CS_MENU (main/drum or menu page)
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_ParameterMaxSet_Main
;; if there's less than 8 menu items display, ie with a 2x20, the drum page needs
;; to scroll like any other page (using the inc/dec buttons)
#if CS_MENU_DISPLAYED_ITEMS > 7
	movf	CS_MENU, W
	xorlw	CS_MENU_DRUM
	bz		CS_MENU_ParameterMaxSet_Drum
#endif 

CS_MENU_ParameterMaxSet_Root
	decf	CS_MENU_ENTRIES, W
#if CS_MENU_OLD_STYLE
	decf	CS_MENU_ENTRIES, W
#else
	movf	CS_MENU_ENTRIES, W
	addlw	-CS_MENU_DISPLAYED_ITEMS
	skpc
	movlw	0x00
#endif
	movwf	CS_MENU_PARAMETER_MAX_L	; low-byte of max value
	clrf	CS_MENU_PARAMETER_MAX_H	; high-byte of max value
	return

CS_MENU_ParameterMaxSet_Main
CS_MENU_ParameterMaxSet_Drum
	movlw	0x7f
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H
	return

;; --------------------------------------------------------------------------
;;  Update parameter value of currently selected parameter (or menu pos or patch)
;; --------------------------------------------------------------------------
CS_MENU_EncParameterSet
	;; clear high byte by default (mostly not used!)
	clrf	CS_MENU_PARAMETER_H

	BRA_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EncParameterSet_NoItem
CS_MENU_EncParameterSet_Item
	;; menu item: execute R2P function and exit

	;; get pointer to entry which has been selected
	rcall	CS_MENU_Hlp_GetCursorPosEntry
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs
	;; R2PP2R ID in MIOS_PARAMETER3
	movf	MIOS_PARAMETER3, W
	rgoto	CS_MENU_R2P_Handler

CS_MENU_EncParameterSet_NoItem

	;; set CS_MENU_PARAMETER depending on CS_MENU[7] (main or menu page)
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_ParameterSet_Main
CS_MENU_ParameterSet_Root
#if CS_MENU_OLD_STYLE
	movf	CS_MENU_CURSOR_POS, W
#else
	movf	CS_MENU_PAGE_OFFSET, W
#endif
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

CS_MENU_ParameterSet_Main
	;; copy patch to CS_MENU_PARAMETER_[LH]
	call	CS_MENU_MS_GetInstrBase
	movlw	MBFM_Ix_PATCH
	movff	PLUSW0, CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

;; --------------------------------------------------------------------------
;;  called by cs_menu_buttons.inc when a parameter has been selected
;;  IN: new cursor position in WREG
;; --------------------------------------------------------------------------
CS_MENU_Select
	;; if in name editing mode, branch to CS_MENU_Select_NameFunc
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_NAME, ACCESS, CS_MENU_Select_NameFunc

	;; if in main page (CS_MENU[7] set), branch to root menu
	GOTO_IFSET CS_MENU, 7, ACCESS, CS_MENU_EXEC_GoToRoot

	;; store new position in CS_MENU_CURSOR_POS
	movwf	CS_MENU_CURSOR_POS

	;; store cursor pos in CS_MENU_PARAMETER_L
	movwf	CS_MENU_PARAMETER_L

	;; clear CS_SELECT_CTR (so that new message appears immediately)
	clrf	CS_SELECT_CTR

	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH

	;; leave function if cursor pos >= max entries
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; branch to EXEC handler

	;; get pointer to entry which has been selected
	rcall	CS_MENU_Hlp_GetCursorPosEntry

	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs

	;; EXEC ID in MIOS_PARAMETER2
	movf	MIOS_PARAMETER2, W
	rgoto	CS_MENU_EXEC_Handler

;; --------------------------------------------------------------------------
;;  special functions of name editing mode
;;  IN: function which should be executed in WREG
;; --------------------------------------------------------------------------
CS_MENU_Select_NameFunc
	movwf	TMP1		; temporary save function in TMP1

	;; clear CS_SELECT_CTR (so that new message appears immediately)
	clrf	CS_SELECT_CTR
	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH

	;; branch depending on (new) cursor pos, set by the select button
#if CS_SAMMICH_FM
	movf	CS_MENU_PAGE_OFFSET, W
	subwf	TMP1, W
#else
	movf	TMP1, W
#endif	
	JUMPTABLE_2BYTES 5	; 5 entries
	rgoto	CS_MENU_Select_NameFunc_Dec
	rgoto	CS_MENU_Select_NameFunc_Inc
	rgoto	CS_MENU_Select_NameFunc_Del
	rgoto	CS_MENU_Select_NameFunc_Ins
	rgoto	CS_MENU_Select_NameFunc_Clr

CS_MENU_Select_NameFunc_Dec	; decrement name pos
	decf	CS_MENU_NAME_POS, W
	andlw	0x0f
	movwf	CS_MENU_NAME_POS
 	rgoto	CS_MENU_Select_NameFunc_End

CS_MENU_Select_NameFunc_Inc	; increment name pos (16 positions max)
	incf	CS_MENU_NAME_POS, W
	andlw	0x0f
	movwf	CS_MENU_NAME_POS
 	rgoto	CS_MENU_Select_NameFunc_End

CS_MENU_Select_NameFunc_Del	; delete character
	call	CS_MENU_MS_GetInstrBase
	movf	CS_MENU_NAME_POS, W
	movwf	TMP1
	movlw	0x0f
	cpfslt	TMP1, ACCESS
	rgoto CS_MENU_Select_NameFunc_Del_End
	;; shift left name from current cursor position
CS_MENU_Select_NameFunc_Del_Loop
	incf	TMP1, W
	movff	PLUSW0, TMP2
	movf	TMP1, W
	movff	TMP2, PLUSW0
	incf	TMP1, F
	movlw	0x0e
	cpfsgt	TMP1, ACCESS
	rgoto CS_MENU_Select_NameFunc_Del_Loop
	
CS_MENU_Select_NameFunc_Del_End
	movlw	0x0f		; set last character to 0x00
	clrf	PLUSW0
 	rgoto	CS_MENU_Select_NameFunc_End

CS_MENU_Select_NameFunc_Ins	; insert space
	call	CS_MENU_MS_GetInstrBase
	movlw	0x0e
	movwf	TMP1
	;; shift right name from current cursor position
CS_MENU_Select_NameFunc_Ins_Loop
	movf	TMP1, W
	movff	PLUSW0, TMP2
	incf	TMP1, W
	movff	TMP2, PLUSW0
	decf	TMP1, F
	incf	TMP1, W
	bz	CS_MENU_Select_NameFunc_Ins_End
	movf	CS_MENU_NAME_POS, W
	cpfslt	TMP1, ACCESS
	rgoto CS_MENU_Select_NameFunc_Ins_Loop
	
CS_MENU_Select_NameFunc_Ins_End
	movf	CS_MENU_NAME_POS, W; set current character to 0x20 (space)
	clrf	PLUSW0
	bsf	PLUSW0, 5	
 	rgoto	CS_MENU_Select_NameFunc_End

CS_MENU_Select_NameFunc_Clr	; clear name
	;; print patch name
	call	CS_MENU_MS_GetInstrBase
	clrf	TMP1			; TMP1 used as loop counter
CS_MENU_Select_NameFunc_Clr_Loop
	clrf	POSTINC0
	incf	TMP1, F
	BRA_IFCLR TMP1, 4, ACCESS, CS_MENU_Select_NameFunc_Clr_Loop; loop 16 times
	;; 	rgoto	CS_MENU_Select_NameFunc_End

CS_MENU_Select_NameFunc_End
	;; execute R2P function and exit
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs
	;; R2PP2R ID in MIOS_PARAMETER3
	movf	MIOS_PARAMETER3, W
	rgoto	CS_MENU_R2P_Handler

;; --------------------------------------------------------------------------
;;  called by cs_menu_buttons.inc when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_MENU_Exec
	;; clear CS_SELECT_CTR (so that new message appears immediately)
	clrf	CS_SELECT_CTR

	;; clear refresh item counter --- ensures that the whole display will be refreshed
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; exit if we are in main menu
	btfsc	CS_MENU, 7
	return

	;; execute function
	;; calc pointer to function: CS_MENU_TABLES + (CS_MENU*CS_MENU_T_ENTRY_LEN) + CS_MENU_ENTRY_EXEC_OFFSET
	TABLE_ADDR CS_MENU_TABLES+CS_MENU_T_ENTRY_EXEC_OFFSET
	movf	CS_MENU, W
	mullw	CS_MENU_T_ENTRY_LEN
	movf	PRODL, W
	TABLE_ADD_W
	goto	MIOS_HLP_IndirectJump


;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc when the patch number should be updated
;;  patch number in CS_MENU_PARAMETER_[LH]
;; --------------------------------------------------------------------------
CS_MENU_PatchUpdate
	;; set and init patches for all selected instruments:
	SET_BSR	MBFM_BASE
	clrf	MBFM_CURRENT_INSTRUMENT, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 0, ACCESS, CS_MENU_PatchUpdate_Hlp_Load
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 1, ACCESS, CS_MENU_PatchUpdate_Hlp_Load
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 2, ACCESS, CS_MENU_PatchUpdate_Hlp_Load
	incf	MBFM_CURRENT_INSTRUMENT, F, BANKED
	RCALL_IFSET CS_MENU_SELECTED_INS_FLAGS, 3, ACCESS, CS_MENU_PatchUpdate_Hlp_Load

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


CS_MENU_PatchUpdate_Hlp_Load
	;; save patch number in Ix_PATCH
	call	MBFM_INSTR_GetBase
	movlw	MBFM_Ix_PATCH
	movff	CS_MENU_PARAMETER_L, PLUSW1

	;; initialize instrument
	goto	MBFM_INSTR_Init


;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc when the drumset number should be updated
;;  drumset number in CS_MENU_PARAMETER_[LH]
;; --------------------------------------------------------------------------
CS_MENU_DrumUpdate
	;; save new number
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	MBFM_DRUMSET
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x07
	movwf	MBFM_DBANK

	;; load drumset
	call	MBFM_DRUM_Init

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc when the menu pos should be updated
;;  item in CS_MENU_PARAMETER_L
;; --------------------------------------------------------------------------
CS_MENU_MenuPosUpdate
	;; copy result to cursor pos and request display update
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x3f
	movwf	CS_MENU_CURSOR_POS

#if CS_MENU_OLD_STYLE
	;; switch page offset if necessary
	;; depending on increment/descrement
	BRA_IFSET TMP3, 7, ACCESS, CS_MENU_MenuPosUpdate_Dec
CS_MENU_MenuPosUpdate_Inc
	;; if CS_MENU_CURSOR_POS >= CS_MENU_PAGE_OFFSET+CS_MENU_DISPLAYED_ITEMS
	;;    -> set CS_MENU_PAGE_OFFSET to CS_MENU_CURSOR_POS-CS_MENU_DISPLAYED_ITEMS
	movf	CS_MENU_PAGE_OFFSET, W
	addlw	CS_MENU_DISPLAYED_ITEMS-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_MENU_MenuPosUpdate_Cont
	movf	CS_MENU_CURSOR_POS, W
	addlw	-(CS_MENU_DISPLAYED_ITEMS-1)
	movwf	CS_MENU_PAGE_OFFSET
	rgoto	CS_MENU_MenuPosUpdate_Cont

CS_MENU_MenuPosUpdate_Dec
	;; if CS_MENU_CURSOR_POS < CS_MENU_PAGE_OFFSET
	;;    -> set CS_MENU_PAGE_OFFSET to CS_MENU_CURSOR_POS
	movf	CS_MENU_CURSOR_POS, W
	cpfsgt	CS_MENU_PAGE_OFFSET, ACCESS
	rgoto CS_MENU_MenuPosUpdate_Cont
	movwf	CS_MENU_PAGE_OFFSET
CS_MENU_MenuPosUpdate_Cont
#else
	;; new style handling:
	;; set page offset directly
	movwf	CS_MENU_PAGE_OFFSET
#endif

	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu item is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; thats all
	return

;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc and cs_menu_exec.inc when a parameter 
;;  should be updated
;; --------------------------------------------------------------------------
CS_MENU_ParameterUpdate
	;; prepare pointers to currently selected entry
	rcall	CS_MENU_Hlp_GetCursorPosEntry

	;; call P2R function
	
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs

	;; R2PP2R ID in MIOS_PARAMETER3
	movf	MIOS_PARAMETER3, W
	rcall	CS_MENU_P2R_Handler

	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see tc_display.inc)
	;; set counter to 2*2 so that the cs_menu_timer.inc function prints the value
	movlw	2*2
	movwf	CS_CURSOR_CTR
	;; set the "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bsf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; thats all
	return


;; --------------------------------------------------------------------------
;;  This help function adds WREG to CS_MENU_ENTRY_TBLPTR[LH] and copies
;;  the result to TBLPTR[LH]
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPointer
	addwf	CS_MENU_ENTRY_TBLPTRL, W
	movwf	TBLPTRL
	movlw	0x00
	addwfc	CS_MENU_ENTRY_TBLPTRH, W
	movwf	TBLPTRH
	return

;; --------------------------------------------------------------------------
;;  This function returns the handler IDs of the selected parameter in the menu
;;  Prepares also MBFM_SELECTED_INS and CS_MENU_PARAMETER_IX depending on
;;  the menu table entry
;;  OUT: ID for CS_MENU_PRINT in MIOS_PARAMETER1
;;       ID for CS_MENU_EXEC in MIOS_PARAMETER2
;;       ID for CS_MENU_R2PP2R in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CS_MENU_GetHandlerIDs
	;; select instrument
	call	CS_MENU_MS_GetInstrNumber

	;; save parameter index (must be the first entry!) in CS_MENU_PARAMETER_IX
	movff	CS_MENU_ENTRY_TBLPTRL, TBLPTRL
	movff	CS_MENU_ENTRY_TBLPTRH, TBLPTRH
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_IX

	;; get pointer to PRINT_IX offset
	movlw	CS_MENU_ENTRY_PRINT_IX_OFFSET
	rcall	CS_MENU_Hlp_GetPointer

	;; PRINT ID in MIOS_PARAMETER1
	tblrd*+
	movf	TABLAT, W
	andlw	0x3f
	movwf	MIOS_PARAMETER1

	;; EXEC ID in MIOS_PARAMETER2
	swapf	TABLAT, W	; bit 1..0
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x03
	movwf	MIOS_PARAMETER2

	tblrd*+			; bit 3..2
	swapf	TABLAT, W
	andlw	0x0c
	iorwf	MIOS_PARAMETER2, F

	;; R2PP2R ID in MIOS_PARAMETER3
	movf	TABLAT, W
	andlw	0x3f
	movwf	MIOS_PARAMETER3

	return


;; --------------------------------------------------------------------------
;;  returns first visible entry
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetFirstVisibleEntry
	;; copy pointer to first entry to CS_MENU_ENTRY_TBLPTR[LH]
	movff	CS_MENU_TABLE_0_L, CS_MENU_ENTRY_TBLPTRL
	movff	CS_MENU_TABLE_0_H, CS_MENU_ENTRY_TBLPTRH

	;; calc offset to first visible entry:
	movf	CS_MENU_PAGE_OFFSET, W
	mullw	CS_MENU_ENTRY_LEN
	movf	PRODL, W
	addwf	CS_MENU_ENTRY_TBLPTRL, F
	movlw	0x00
	addwfc	CS_MENU_ENTRY_TBLPTRH, F
	return

;; --------------------------------------------------------------------------
;;  returns entry, indexed by cursor_pos
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetCursorPosEntry
	;; copy pointer to first entry to CS_MENU_ENTRY_TBLPTR[LH]
	movff	CS_MENU_TABLE_0_L, CS_MENU_ENTRY_TBLPTRL
	movff	CS_MENU_TABLE_0_H, CS_MENU_ENTRY_TBLPTRH

	;; calc offset to cursor pos entry:
	movf	CS_MENU_CURSOR_POS, W
	mullw	CS_MENU_ENTRY_LEN
	movf	PRODL, W
	addwf	CS_MENU_ENTRY_TBLPTRL, F
	movlw	0x00
	addwfc	CS_MENU_ENTRY_TBLPTRH, F
	return

;; --------------------------------------------------------------------------
;;  This help function prints n spaces
;;  IN: number of spaces in WREG
;; --------------------------------------------------------------------------
CS_MENU_Hlp_PrintSpaces
	movwf	TMP1		; using TMP1 as loop counter
CS_MENU_Hlp_PrintSpaces_Loop
	movlw	' '
	call	MIOS_LCD_PrintChar
	decfsz	TMP1, F		; decrement TMP1, skip if zero
	rgoto	CS_MENU_Hlp_PrintSpaces_Loop

	return
	
;; --------------------------------------------------------------------------
;;  prints "save ***" on screen
;; --------------------------------------------------------------------------
TEXT_SAVE_DATA_0	db "(i) Patch saved     "
TEXT_SAVE_DATA_1	db "(i) Drumset saved   "
TEXT_SAVE_DATA_2	db "(i) Ensemble saved  "
TEXT_SAVE_DATA_3	db "(i) Everything saved"

CS_MENU_PrintSaveData
	;; print "Save Patch/Drumset/Ensemble/Everything saved"
	mullw	20
	movlw	TEXT_SAVE_DATA_0 & 0xff
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	TEXT_SAVE_DATA_0 >> 8
	addwfc	PRODH, W
	movwf	TBLPTRH

	movlw	0x40
	call	MIOS_LCD_CursorSet

	movlw	20
	call	MIOS_LCD_PrintPreconfString

	;; fill with spaces
	movlw	20
	rcall	CS_MENU_Hlp_PrintSpaces

	;; start message
	goto	MIOS_LCD_MessageStart


;; --------------------------------------------------------------------------
;;  if patch should be saved
;;  (target patch in CS_MENU_SAVE_PATCH/CS_MENU_SAVE_BANK)
;; --------------------------------------------------------------------------
CS_MENU_SavePatch
	;; switch to target MBFM_PATCH/BANK
	movff	CS_MENU_SAVE_PATCH, MBFM_PATCH
	movff	CS_MENU_SAVE_BANK, MBFM_BANK

CS_MENU_SaveCurPatch
	;; print "save patch"
	movlw	0x00
	rcall	CS_MENU_PrintSaveData

	;; get instrument number
	call	CS_MENU_MS_GetInstrNumber

	;; save parameters into save patch
	call	MBFM_BANK_StoreP

	;; ensure that WT buffer won't be flushed
	bcf	CS_EEPROM_WR, CS_EEPROM_WR_WT_REQ
	bcf	CS_EEPROM_WR, CS_EEPROM_WR_WT_GNT
	
	;; copy WT EEPROM content to save patch
	clrwdt
	clrf	TMP1
CS_MENU_SavePatch_CopyWTLoop
	movff	TMP1, EEADR
	call	CS_MENU_MS_GetInstrPatch
	call	MBFM_BANK_ReadWByte
	movff	TMP1, EEADR
	movff	CS_MENU_SAVE_PATCH, MBFM_PATCH
	movff	CS_MENU_SAVE_BANK, MBFM_BANK
	call	MBFM_BANK_WriteWByte
	incf	TMP1, F
	BRA_IFCLR TMP1, 7, ACCESS, CS_MENU_SavePatch_CopyWTLoop

	;; switch to new patch
	call	CS_MENU_MS_GetInstrBase
	movlw	MBFM_Ix_PATCH
	movff	MBFM_PATCH, PLUSW0
	movlw	MBFM_Ix_BANK
	movff	MBFM_BANK, PLUSW0
	goto	MBFM_INSTR_InitAll

;; --------------------------------------------------------------------------
;;  if drumset should be saved
;;  (target drumset in CS_MENU_SAVE_DRUMSET/CS_MENU_SAVE_DBANK)
;; --------------------------------------------------------------------------
CS_MENU_SaveDrumset
	movff	CS_MENU_SAVE_DRUMSET, MBFM_DRUMSET
	movff	CS_MENU_SAVE_DBANK, MBFM_DBANK
CS_MENU_SaveCurDrumset
	movlw	0x01
	rcall	CS_MENU_PrintSaveData
	call	MBFM_BANK_StoreD
	goto	MBFM_DRUM_Init

;; --------------------------------------------------------------------------
;;  if drumset should be saved
;;  (target drumset in CS_MENU_SAVE_DRUMSET/CS_MENU_SAVE_DBANK)
;; --------------------------------------------------------------------------
CS_MENU_SaveEnsemble
	movff	CS_MENU_SAVE_ENSEMBLE, MBFM_ENSEMBLE
	movff	CS_MENU_SAVE_EBANK, MBFM_EBANK
CS_MENU_SaveCurEnsemble
	movlw	0x02
	rcall	CS_MENU_PrintSaveData
	call	MBFM_BANK_StoreE
	goto	MBFM_ENS_Init


;; --------------------------------------------------------------------------
;;  EEPROM write handler
;; --------------------------------------------------------------------------
CS_MENU_Write_EEPROM
	IRQ_DISABLE
	;; do nothing if write to WT has not been granted
	BRA_IFCLR CS_EEPROM_WR, CS_EEPROM_WR_WT_GNT, ACCESS, CS_MENU_Write_EEPROM_End
	;; do nothing if request not set anymore
	BRA_IFCLR CS_EEPROM_WR, CS_EEPROM_WR_WT_REQ, ACCESS, CS_MENU_Write_EEPROM_End
	;; clear grant
	bcf	CS_EEPROM_WR, CS_EEPROM_WR_WT_GNT
	;; clear request
	bcf	CS_EEPROM_WR, CS_EEPROM_WR_WT_REQ
	IRQ_ENABLE
	;; write WT buffer to EEPROM
	call	CS_MENU_MS_Update_WT_EEPROM
CS_MENU_Write_EEPROM_End
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  Updates the merger mode
;; --------------------------------------------------------------------------
CS_MENU_UpdateMergerMode
	;; enable/disable the merger
	movlw	MIOS_MIDI_MERGER_ENABLED
	btfss	CS_MENU_MODE, CS_MENU_MODE_MERGER
	movlw	MIOS_MIDI_MERGER_DISABLED
	goto	MIOS_MIDI_MergerSet

;; --------------------------------------------------------------------------
;;  This function increments a given parameter which is indexed like shown
;;  here:
;;     ID of CS_MENU in WREG (example: CS_MENU_OP)
;;     table (cursor) position in MIOS_PARAMETER1 (example: 0x01 for waveform)
;;     page offset in MIOS_PARAMETER2 (example: 0x00 begins at first line)
;;     increment value in MIOS_PARAMETER3
;;  used by cs_menu_enc.inc
;; --------------------------------------------------------------------------
CS_MENU_ENC_CSInc
	;; save menu settings so that we can jump back to the current menu later
	movff	CS_MENU, SAVED_CS_MENU
	movff	CS_MENU_CURSOR_POS, SAVED_CS_MENU_CURSOR_POS
	movff	CS_MENU_PAGE_OFFSET, SAVED_CS_MENU_PAGE_OFFSET

	;; save incrementer in TMP5
	movff	MIOS_PARAMETER3, TMP5

	;; change to menu
	movwf	CS_MENU
	rcall	CS_MENU_Page_Init

	;; set new page offset and cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS
	movff	MIOS_PARAMETER2, CS_MENU_PAGE_OFFSET

	;; select parameter
	call	CS_MENU_EXEC_SelPar

	;; inc/dec value by using the 16 bit add w/ saturation routine
	;; set pointer to CS_MENU_PARAMETER_L (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, CS_MENU_PARAMETER_L
	;; set max value
	movff	CS_MENU_PARAMETER_MAX_L, MIOS_PARAMETER1	; low-byte of max value
	movff	CS_MENU_PARAMETER_MAX_H, MIOS_PARAMETER2	; high-byte of max value
	;; get incrementer (which has been stored in TMP5)
	movf	TMP5, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = CS_MENU_PARAMETER_[LH] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed
	RCALL_IFSET MIOS_PARAMETER1, 0, ACCESS, CS_MENU_ParameterUpdate		; update parameter
	call	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter

	;; if MJMP flag activated
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_MJMP_N, ACCESS, CS_MENU_ENC_CSInc_NoJmp
CS_MENU_ENC_CSInc_Jmp
	;; change back to old menu and cursor pos
	movff	SAVED_CS_MENU, CS_MENU
	rcall	CS_MENU_Page_Init
	movff	SAVED_CS_MENU_PAGE_OFFSET, CS_MENU_PAGE_OFFSET
	movff	SAVED_CS_MENU_CURSOR_POS, CS_MENU_CURSOR_POS
CS_MENU_ENC_CSInc_NoJmp

	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This help function updates the INS select flags
;; --------------------------------------------------------------------------
CS_MENU_Hlp_UpdateINSFlags
	movlw	0xf0
	andwf	CS_MENU_SELECTED_INS_FLAGS, F
	movf	CS_MENU_SELECTED_INS, W
	rcall	CS_MENU_Hlp_UpdateINSFlags_Val
	iorwf	CS_MENU_SELECTED_INS_FLAGS, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the INS offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected operator
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddINSOffset
	;; replace high-byte to HIGH(MBFM_BASE)!
	movlw	(MBFM_BASE >> 8) & 0xff
	movwf	FSR0H
	
	movlw	0x03
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 2
	movlw 0x02
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 1
	movlw 0x01
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 0
	movlw 0x00
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function updates the OP select flags
;; --------------------------------------------------------------------------
CS_MENU_Hlp_UpdateOPFlags
	movlw	0xf0
	andwf	CS_MENU_SELECTED_OP_FLAGS, F
	movf	CS_MENU_SELECTED_OP, W
	rcall	CS_MENU_Hlp_UpdateOPFlags_Val
	iorwf	CS_MENU_SELECTED_OP_FLAGS, F
	return
CS_MENU_Hlp_UpdateOPFlags_Val
CS_MENU_Hlp_UpdateINSFlags_Val	; (also used for instruments)
	JUMPTABLE_2BYTES 7	; 7 entries
	retlw	b'00000001'	; OP1
	retlw	b'00000010'	; OP2
	retlw	b'00000100'	; OP3
	retlw	b'00001000'	; OP4
	retlw	b'00000011'	; OP1+OP2
	retlw	b'00001100'	; OP3+OP4
	retlw	b'00001111'	; OP1+OP2+OP3+OP4

;; --------------------------------------------------------------------------
;;  This help function adds the OP offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected operator
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddOPOffset
	movlw	0x03
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 2
	movlw 0x02
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 1
	movlw 0x01
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 0
	movlw 0x00
	addwf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the LFO offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected LFO
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddLFOOffset
	movlw	0x00
	btfsc	CS_MENU_SELECTED_LFO, 0
	movlw 0x10
	addwf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the EG offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected EG
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddEGOffset
	movlw	0x00		; only one EG
	addwf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the BD_OP offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected Bassdrum operator (Modulator/Carrier)
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddBDOPOffset
	btfsc	CS_MENU_SELECTED_BD_HH, 0
	incf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the HH_OC offset to CS_MENU_IX_PARAMETER depending on the
;;  currently selected HiHat (Open/Close)
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddHHOCOffset
	btfsc	CS_MENU_SELECTED_BD_HH, 1
	incf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the AOUT offset to CS_MENU_IX_PARAMETER depending
;;  currently selected AOUT
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddAOUTOffset
	movf	CS_MENU_SELECTED_AOUT, W
	mullw	MBFM_AOUT_RECORD_LEN
	movf	PRODL, W
	addwf	CS_MENU_PARAMETER_IX, F
	return

;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the OP offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxOxValue
	rcall	CS_MENU_Hlp_AddOPOffset
CS_MENU_Hlp_GetPxValue
	movf	CS_MENU_PARAMETER_IX, W
	goto	MBFM_PAROUT_Get

;; --------------------------------------------------------------------------
;;  This help function returns the pointer to a wavetable parameter in RAM
;;  IN:	CS_MENU_PARAMETER_IX contains the parameter number (0-3)
;;  OUT: pointer in FSR0
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetWTPxPtr
	lfsr	FSR0, CS_MENU_WT_MODE
	movf	CS_MENU_PARAMETER_IX, W
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the LFO offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxLFOxValue
	rcall	CS_MENU_Hlp_AddLFOOffset
	rgoto	CS_MENU_Hlp_GetPxValue

;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the EG offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxEGxValue
	rcall	CS_MENU_Hlp_AddEGOffset
	rgoto	CS_MENU_Hlp_GetPxValue

;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the BD_OP offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxBDxValue
	rcall	CS_MENU_Hlp_AddBDOPOffset
	rgoto	CS_MENU_Hlp_GetPxValue

;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the HH_OC offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxHHxValue
	rcall	CS_MENU_Hlp_AddHHOCOffset
	rgoto	CS_MENU_Hlp_GetPxValue


;; --------------------------------------------------------------------------
;;  This help function returns the parameter value which is selected with
;;  CS_MENU_PARAMETER_IX
;;  it adds optionally the AOUT offset to CS_MENU_PARAMETER_IX
;; --------------------------------------------------------------------------
CS_MENU_Hlp_GetPxAOUTxValue
	rcall	CS_MENU_Hlp_AddAOUTOffset
	movf	CS_MENU_PARAMETER_IX, W
	goto	MBFM_AOUT_ParGet


