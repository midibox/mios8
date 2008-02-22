; $Id$
;
; SID Control Surface Menu Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by SID_Init during startup to reset the
;;  control surface
;; --------------------------------------------------------------------------
CS_MENU_Reset
	;; control surface: begin with main page
	movlw	CS_MENU_MAIN
	movwf	CS_MENU

	;; set reinit counter - CS configuration will be restored after one second
	movlw	10
	movwf	CS_MENU_REINIT_CFG_CTR

	;; only if character LCD connected: init special characters
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, CS_MENU_Reset_NoCLCD
	TABLE_ADDR CS_MENU_SPECIAL_CHARS	; (defined in special_characters.inc)
	call	MIOS_CLCD_SpecialCharsInit
CS_MENU_Reset_NoCLCD

	;; start with master SID, SID button 1-4 depressed
	movlw	0xf1
	movwf	CS_MENU_SELECTED_SID_FLAGS

	;; set link mode
#if CS_MENU_DEFAULT_LINK == 1
	bsf	CS_MENU_MODE, CS_MENU_MODE_LINK
#else
	bcf	CS_MENU_MODE, CS_MENU_MODE_LINK
#endif
	rcall	CS_MENU_UpdateLinkMode

	;; update OSC flags
	rcall	CS_MENU_Hlp_UpdateOSCFlags

	;; set mode of modulation matrix
#if DEFAULT_LEDMATRIX_MODE == 0
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
#else
	bsf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
#endif
	
	;; set speed for the CS encoders (1-14)
	;; set encoder speed to fast, divider value defined in main.asm
	movlw	0x01
	movwf	TMP1
CS_MENU_Reset_Loop
	movlw	CS_MENU_ENC_SPEED_VALUE	; speed divider 3
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_FAST	; fast mode
	movwf	MIOS_PARAMETER1
	movf	TMP1, W			; encoder number
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	15
	cpfseq	TMP1, ACCESS
	rgoto CS_MENU_Reset_Loop

	return

;; --------------------------------------------------------------------------
;;  entry function for a menu page update
;; --------------------------------------------------------------------------
CS_MENU_Handler
	;; check if configuration has to be restored from BankStick
	call	CS_MENU_BANK_RestoreCfg_Tst
	;; so long as re-init ctr is != zero, exit
	movf	CS_MENU_REINIT_CFG_CTR, W
	skpz
	return

	;; check if tx transfer to slave has been requested and can be granted
	call	CS_MENU_MS_Send_SysExDump_TxTst
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber
	;; call EEPROM write handler
	rcall	CS_MENU_Write_EEPROM

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
	;; init menu table
	rcall	CS_MENU_Page_Init
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber
	;; set encoder speed
	rcall	CS_MENU_EncSpeedSet
	;; set parameter
	rcall	CS_MENU_EncParameterSet
	;; set max position of cursor (or patch)
	rcall	CS_MENU_EncMaxSet
	;; clear item counter - used for optimized refresh handling
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; clear screen
	call	MIOS_LCD_Clear

	;; in main page: print static display elements
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_Handler_Init_NotMainPage
CS_MENU_Handler_Init_MainPage
	;; Chn for Channel
	movlw	0x05
	call	MIOS_LCD_CursorSet
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'h'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar

	;; print patch name at lower line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	call	SID_LCD_PrintPatch
CS_MENU_Handler_Init_NotMainPage

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
	;; if update of CC parameter has been requested, do it now
	RCALL_IFSET CS_STAT, CS_STAT_UPDATE_PARAMETERS, ACCESS, CS_MENU_UpdateCCPara
	bcf	CS_STAT, CS_STAT_UPDATE_PARAMETERS

	;; branch if menu page is displayed
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_Handler_MenuPage

	;; ------------------------------------------------------------------
	;; else handle mainpage
CS_MENU_Handler_MainPage
	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; print current bank and patch number
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	CS_MENU_MS_GetSIDBank
	movwf	PRODL
	call	CS_MENU_MS_GetSIDPatch
	movwf	PRODH
	call	SID_LCD_PrintPatchNumber

	;; print MIDI channel
	movlw	0x08
	call	MIOS_LCD_CursorSet
	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	incf	INDF1, W
	andlw	0x7f		; mask out "sent" flag
	call	MIOS_LCD_PrintBCD2

	;; print selected SIDs
	movlw	0x0c
	call	MIOS_LCD_CursorSet
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID1; inverted '-'
	movlw 0x06 
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, CS_MENU_Handler_MainPage_SID1
	movlw	'1'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID1; inverted 1
	movlw 0x02 
CS_MENU_Handler_MainPage_SID1
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID2; inverted '-'
	movlw 0x06 
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 1, ACCESS, CS_MENU_Handler_MainPage_SID2
	movlw	'2'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID2; inverted 2
	movlw 0x03 
CS_MENU_Handler_MainPage_SID2
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID3; inverted '-'
	movlw 0x06 
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 2, ACCESS, CS_MENU_Handler_MainPage_SID3
	movlw	'3'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID3; inverted 3
	movlw 0x04 
CS_MENU_Handler_MainPage_SID3
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID4; inverted '-'
	movlw 0x06 
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 3, ACCESS, CS_MENU_Handler_MainPage_SID4
	movlw	'4'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID4; inverted 4
	movlw 0x05 
CS_MENU_Handler_MainPage_SID4
	call	MIOS_LCD_PrintChar

	;; ...and exit
	return

	;; ------------------------------------------------------------------
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

	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; print patch name
	call	SID_LCD_PrintPatch

	;; print bank number and patch
	call	CS_MENU_MS_GetSIDBank
	movwf	PRODL
	call	CS_MENU_MS_GetSIDPatch
	movwf	PRODH
	call	SID_LCD_PrintPatchNumber

CS_MENU_Page_Root_Refresh_NoUL
	;; print menu item name
	movlw	0x40
	call	CS_MENU_PrintILine

CS_MENU_Page_Root_Refresh_End
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

	;; set cursor to: 0x40 + 4*(CS_MENU_CURSOR_POS-CS_MENU_PAGE_OFFSET) + 2
	movf	CS_MENU_PAGE_OFFSET, W
	subwf	CS_MENU_CURSOR_POS, W
	mullw	4
	movf	PRODL, W
	addlw	0x40 + 2
	call	MIOS_LCD_CursorSet

	;; enable blinking CLCD cursor and exit
	movlw	0x0e
	goto	MIOS_LCD_Cmd

;; --------------------------------------------------------------------------
;;  print name editing screen
;; --------------------------------------------------------------------------
CS_MENU_Page_EditName
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print patch name
	call	SID_LCD_PrintPatch

	;; fill rest with spaces
	movlw	40-16
	rcall	CS_MENU_Hlp_PrintSpaces

	;; flashing cursor: set space
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_Page_EditName_NoSpc
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet
	movlw	' '
	call	MIOS_LCD_PrintChar
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

					;;  <------------------>
TEXT_CS_MENU_EDIT_OPTIONS STRING 20, 0x40, " <   >  Del Ins Clr "
					;;  12341234123412341234

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
	;; print 4 spaces
	movlw	4
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
	;; print 4 spaces
	movlw	4
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintILine_End
CS_MENU_PrintILine_COn
#endif
	;; print short label (function switches to next table entry)
	rcall	CS_MENU_PrintSEntry
	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

CS_MENU_PrintILine_End
	return

;; --------------------------------------------------------------------------
;;  print the arrows at the end of an ILine
;;  IN: pointer to menu entries in TBLPTR[LH]
;;      offset to first entry in CS_MENU_PAGE_OFFSET
;; --------------------------------------------------------------------------
CS_MENU_PrintILine_Arw
	;; set cursor
	addlw	CS_MENU_DISPLAYED_ITEMS*4 - 1
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
#if CS_MENU_OLD_STYLE
	;; if CS_MENU_CURSOR_POS != CS_MENU_ENTRIES-1, print '>'
	decf	CS_MENU_ENTRIES, W
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_MENU_PrintILine_Arw_LR
#else
	;; if CS_MENU_PAGE_OFFSET == 0, print '>'
	movf	CS_MENU_PAGE_OFFSET, W
	bz	CS_MENU_PrintILine_Arw_LR
	;; if CS_MENU_PAGE_OFFSET != CS_MENU_ENTRIES-CS_MENU_DISPLAY_ITEMS, print '<>'
	movf	CS_MENU_ENTRIES, W
	addlw	-CS_MENU_DISPLAYED_ITEMS
	cpfseq	CS_MENU_PAGE_OFFSET, ACCESS
	rgoto CS_MENU_PrintILine_Arw_LLR
#endif
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
	btfss	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD; special char #7: '<>'
	movlw 0x07
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
	xorlw	CS_MENU_SAV
	bz	CS_MENU_PrintPLine_End
	;; print 4 spaces
	movlw	4
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
	;; print 4 spaces
	movlw	4
	rcall	CS_MENU_Hlp_PrintSpaces
	rgoto	CS_MENU_PrintPLine_End
CS_MENU_PrintPLine_COn
	rcall	CS_MENU_PrintPEntry
	movlw	' '
	call	MIOS_LCD_PrintChar

CS_MENU_PrintPLine_End

	return

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
;;  Set max value of currently selected parameter (or menu pos or SID patch)
;; --------------------------------------------------------------------------
CS_MENU_EncMaxSet
	;; if in parameter selection mode, max value has been set by CS_MENU_EXEC_SelPar
	btfsc	CS_STAT, CS_STAT_MODIFY_PARAMETER
	return

	;; set CS_MENU_PARAMETER depending on CS_MENU[7] (main or menu page)
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_ParameterMaxSet_Main
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
	movlw	0x7f
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H
	return

;; --------------------------------------------------------------------------
;;  Update parameter value of currently selected parameter (or menu pos or SID patch)
;; --------------------------------------------------------------------------
CS_MENU_EncParameterSet
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
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber
	;; copy selected patch * 4 to CS_MENU_PARAMETER_[LH]
	call	CS_MENU_MS_GetSIDPatch
	movwf	CS_MENU_PARAMETER_L
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
;;  IN: function which should be executed in WREG (CS_MENU_PAGE_OFFSET has to be substracted!)
;; --------------------------------------------------------------------------
CS_MENU_Select_NameFunc
	movwf	TMP1		; temporary save function in TMP1
	movf	CS_MENU_PAGE_OFFSET, W
	subwf	TMP1, F

	;; clear CS_SELECT_CTR (so that new message appears immediately)
	clrf	CS_SELECT_CTR
	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber

	;; branch depending on (new) cursor pos, set by the select button
	movf	TMP1, W
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
	call	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_NAME_x
	addwf	FSR0L, F

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
	call	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_NAME_x
	addwf	FSR0L, F

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
	call	CS_MENU_MS_GetSIDBase
	movlw	CS_SID_NAME_x
	addwf	FSR0L, F
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
	;; save current patch if it has been changed
	rcall	CS_MENU_PatchUpdate_SaveOld

	;; copy patch number to all selected SIDs
	clrf	CS_MENU_SID		; using CS_MENU_SID as counter
CS_MENU_PatchUpdate_Loop
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_SID_FLAGS, W
	bz	CS_MENU_PatchUpdate_Next
	;; copy changed CS_MENU_PARAMETER_L to saved patch number and init sound
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	movwf	INDF1
	;; request update of CS parameters
	call	CS_MENU_MS_Send_SysExDump_TxReq
	;; update patch name immediately
 	call	CS_MENU_MS_UpdatePatchName
CS_MENU_PatchUpdate_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_PatchUpdate_Loop

	;; restore CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; request display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc when a single patch should be updated
;;  patch number in CS_MENU_PARAMETER_[LH]
;;  SID number in CS_MENU_SID
;; --------------------------------------------------------------------------
CS_MENU_PatchUpdateSingle
	;; save current patch if it has been changed
	rcall	CS_MENU_PatchUpdate_SaveOld

	;; copy changed CS_MENU_PARAMETER_L to saved patch number and init sound
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	movwf	INDF1
	;; request update of CS parameters and exit
	goto	CS_MENU_MS_Send_SysExDump_TxReq


;; --------------------------------------------------------------------------
;;  this function saves the current patch if it has been changed
;; --------------------------------------------------------------------------
CS_MENU_PatchUpdate_SaveOld
	;; in edit mode: save current patch if it has been changed
	btfss	CS_MENU_MODE, CS_MENU_MODE_EDIT
	return
	;; don't save if nothing has been changed
	btfss	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER
	return
	;; clear edit notifier
	bcf	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER
	;; save patch to CS_MENU_SAVE_PATCH
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movf	INDF1, W
	andlw	0x7f
	movwf	CS_MENU_SAVE_PATCH
	call	CS_MENU_MS_GetSIDBank; sets FSR1 to bank register
	movf	INDF1, W
	movwf	CS_MENU_SAVE_BANK
	rgoto	CS_MENU_SavePatch


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

	;; notify that a value has been changed
	bsf	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER

	;; call P2R function for all selected SIDs
	clrf	CS_MENU_SID		; using CS_MENU_SID as counter
CS_MENU_ParameterUpdate_Loop
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_SID_FLAGS, W
	bz	CS_MENU_ParameterUpdate_Next
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs_SIDPreSel
	;; R2PP2R ID in MIOS_PARAMETER3
	movf	MIOS_PARAMETER3, W
	rcall	CS_MENU_P2R_Handler
CS_MENU_ParameterUpdate_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_ParameterUpdate_Loop

	;; now request a display update so that we see the new parameter on screen
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see tc_display.inc)
	;; set counter to 5*2 so that the cs_menu_timer.inc function prints the value
	movlw	5*2
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
	call	CS_MENU_MS_GetSIDNumber

CS_MENU_GetHandlerIDs_SIDPreSel	; used by CS_MENU_ParameterUpdate which can call the function for all selected SIDs
	;; get base address to currently selected SID device
	call	CS_MENU_MS_GetSIDBase

	;; copy offset to register entry (must be the first entry!) to tablepointer
	movff	CS_MENU_ENTRY_TBLPTRL, TBLPTRL
	movff	CS_MENU_ENTRY_TBLPTRH, TBLPTRH

	;; get register address and add it to FSR0L
	tblrd*+
	movf	TABLAT, W
	addwf	FSR0L, F

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
;;  This help function updates the OSC select flags
;; --------------------------------------------------------------------------
CS_MENU_Hlp_UpdateOSCFlags
	movf	CS_MENU_SELECTED_OSC, W
	rcall	CS_MENU_Hlp_UpdateOSCFlags_Val
	movwf	CS_MENU_SELECTED_OSC_FLAGS
	return
CS_MENU_Hlp_UpdateOSCFlags_Val
	JUMPTABLE_2BYTES 7	; 7 entries
	retlw	b'00000001'	; OSC1
	retlw	b'00000010'	; OSC2
	retlw	b'00000100'	; OSC3
	retlw	b'00000011'	; OSC1+OSC2
	retlw	b'00000101'	; OSC1+OSC3
	retlw	b'00000110'	; OSC2+OSC3
	retlw	b'00000111'	; OSC1+OSC2+OSC3

;; --------------------------------------------------------------------------
;;  This help function adds the voice offset to FSR0 depending on the
;;  currently selected oscillator
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddVoiceOffset
	movlw	CS_SID_VOICE3_OFFSET
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 1
	movlw CS_SID_VOICE2_OFFSET
	btfsc	CS_MENU_SELECTED_OSC_FLAGS, 0
	movlw CS_SID_VOICE1_OFFSET
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the voice 1 offset to FSR0
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddVoice1Offset
	movlw	CS_SID_VOICE1_OFFSET
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the LFO offset to FSR0 depending on the
;;  currently selected LFO
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddLFOOffset
	movf	CS_MENU_SELECTED_LFO, W
	mullw	CS_SID_LFO2_OFFSET-CS_SID_LFO1_OFFSET
	movf	PRODL, W
	addlw	CS_SID_LFO1_OFFSET
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the ENV offset to FSR0 depending on the
;;  currently selected envelope
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddENVOffset
	movf	CS_MENU_SELECTED_ENV, W
	mullw	CS_SID_ENV2_OFFSET-CS_SID_ENV1_OFFSET
	movf	PRODL, W
	addlw	CS_SID_ENV1_OFFSET
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function adds the ENV1 offset to FSR0
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddENV1Offset
	movlw	CS_SID_ENV1_OFFSET
	addwf	FSR0L, F
	return

;; --------------------------------------------------------------------------
;;  This help function returns the pointer to the LFO modulation register 
;;  of the currently selected modulation item
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddModLFOOffset
	rcall	CS_MENU_Hlp_AddModLFOOffset_Tab
	addwf	FSR0L, F
	return
CS_MENU_Hlp_AddModLFOOffset_Tab
	movf	CS_MENU_SELECTED_MOD, W
	JUMPTABLE_2BYTES 7
	retlw	CS_SID_VOICE1_OFFSET + CS_SID_VOICEx_LFO_PITCH
	retlw	CS_SID_VOICE2_OFFSET + CS_SID_VOICEx_LFO_PITCH
	retlw	CS_SID_VOICE3_OFFSET + CS_SID_VOICEx_LFO_PITCH
	retlw	CS_SID_VOICE1_OFFSET + CS_SID_VOICEx_LFO_PW
	retlw	CS_SID_VOICE2_OFFSET + CS_SID_VOICEx_LFO_PW
	retlw	CS_SID_VOICE3_OFFSET + CS_SID_VOICEx_LFO_PW
	retlw	CS_SID_FILTER_LFO

;; --------------------------------------------------------------------------
;;  This help function returns the pointer to the ENV modulation register 
;;  of the currently selected modulation item
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddModENVOffset
	rcall	CS_MENU_Hlp_AddModENVOffset_Tab
	addwf	FSR0L, F
	return
CS_MENU_Hlp_AddModENVOffset_Tab
	movf	CS_MENU_SELECTED_MOD, W
	JUMPTABLE_2BYTES 7
	retlw	CS_SID_VOICE1_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_VOICE2_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_VOICE3_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_VOICE1_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_VOICE2_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_VOICE3_OFFSET + CS_SID_VOICEx_ENV
	retlw	CS_SID_FILTER_ENV

;; --------------------------------------------------------------------------
;;  This help function returns the bit offset within the ENV modulation register 
;;  of the currently selected modulation item
;; --------------------------------------------------------------------------
CS_MENU_Hlp_AddModENVBitOffset
	rcall	CS_MENU_Hlp_AddModENVBitOffset_T
	andlw	0xff		; (fix STATUS)
	return

CS_MENU_Hlp_AddModENVBitOffset_T
	movf	CS_MENU_SELECTED_MOD, W
	JUMPTABLE_2BYTES 7
	retlw	0
	retlw	0
	retlw	0
	retlw	4
	retlw	4
	retlw	4
	retlw	0

;; --------------------------------------------------------------------------
;;  if patch should be saved
;;  (target patch in CS_MENU_SAVE_PATCH/CS_MENU_SAVE_BANK)
;; --------------------------------------------------------------------------
CS_MENU_SavePatch
	;; check if BankStick target available (only required for bank > 0)
	movf	CS_MENU_SAVE_BANK, W
	bz	CS_MENU_SavePatch_NoBnkChk
	call	MIOS_HLP_GetBitORMask
	andwf	SID_BANKSTICK_STATUS, W
	bz	CS_MENU_SavePatch_End
CS_MENU_SavePatch_NoBnkChk
	
	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; save current patch and bank in TMP[45] (for WT copy)
	call	CS_MENU_MS_GetSIDPatch
	movwf	TMP4
	call	CS_MENU_MS_GetSIDBank
	movwf	TMP5

	;; switch patch/bank to CS_MENU_SAVE_PATCH/BANK
	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK

	;; save 256 parameters
	call	CS_MENU_MS_GetSIDBase
	clrf	TMP1
CS_MENU_SavePatch_Loop
	;; feed watchdog
	clrwdt

	;; copy loop counter to address
	movff	TMP1, EEADR

	;; get CC number
	movf	TMP1, W
	call	SID_SYSEX_TABLE_Get

	;; temporary store number in TMP2
	movwf	TMP2

	;; copy if EEPROM value
	BRA_IFSET TMP2, 7, ACCESS, CS_MENU_SavePatch_CopyEE
CS_MENU_SavePatch_CopyCC
CS_MENU_SavePatch_CopyEE_Name
CS_MENU_SavePatch_CopyEE_Split
	;; get value which has been saved in the CS_SID_ array
	movf	TMP1, W
	movf	PLUSW0, W

	;; write to eeprom
	call	SID_BANK_Write

	;; next entry
	rgoto	CS_MENU_SavePatch_Loop_Next

CS_MENU_SavePatch_CopyEE

	;; if name is selected, write content of CS_SID_ array
	movf	TMP1, W
	andlw	0xf0
	bz	CS_MENU_SavePatch_CopyEE_Name

	;; if split is selected, write content of CS_SID_ array
	movf	TMP2, W
	andlw	0xf0
	xorlw	0x90
	bnz	CS_MENU_SavePatch_CopyEE_NoSplit
	rgoto	CS_MENU_SavePatch_CopyEE_Split
	
CS_MENU_SavePatch_CopyEE_NoName
CS_MENU_SavePatch_CopyEE_NoSplit
	;; get content of EEPROM, old patch/bank number
	movff	TMP4, SID_PATCH
	movff	TMP5, SID_BANK
	call	SID_BANK_Read

	;; copy loop counter to address
	movff	TMP1, EEADR

	;; write byte to new EEPROM location
	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK
	call	SID_BANK_Write

CS_MENU_SavePatch_Loop_Next
	;; loop 256 times
	incf	TMP1, F
	bnz	CS_MENU_SavePatch_Loop

	;; save MIDI device ID
	movlw	0x7f
	movwf	EEADR
	movff	SID_MIDI_DEVICE, WREG
	call	MIOS_EEPROM_Write

	;; switch to new patch/bank
	call	CS_MENU_MS_GetSIDPatch	; get pointer to patch number
	movff	SID_PATCH, INDF1
	movff	SID_PATCH, SID_WT_PATCH
	call	CS_MENU_MS_GetSIDBank	; get pointer to bank number
	movff	SID_BANK, INDF1

CS_MENU_SavePatch_End
	return


;; --------------------------------------------------------------------------
;;  EEPROM write handler
;;  expecting correct SID number in CS_MENU_SID!
;; --------------------------------------------------------------------------
CS_MENU_Write_EEPROM
	IRQ_DISABLE
	;; do nothing if write to WT has not been granted
	BRA_IFCLR CS_STAT2, CS_STAT2_EEPROM_WR_WT_GNT, ACCESS, CS_MENU_Write_EEPROM_End
	;; do nothing if request not set anymore
	BRA_IFCLR CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ, ACCESS, CS_MENU_Write_EEPROM_End
	;; clear grant
	bcf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_GNT
	;; clear request
	bcf	CS_STAT2, CS_STAT2_EEPROM_WR_WT_REQ
	IRQ_ENABLE
	;; write WT buffer to EEPROM
	call	CS_MENU_MS_Update_WT_EEPROM
CS_MENU_Write_EEPROM_End
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  Updates the link mode (enables/disables merger, sends a program change
;;  to all slaves)
;; --------------------------------------------------------------------------
CS_MENU_UpdateLinkMode
	;; enable/disable the merger
	movlw	MIOS_MIDI_MERGER_ENABLED
	btfss	CS_MENU_MODE, CS_MENU_MODE_LINK
	movlw	MIOS_MIDI_MERGER_DISABLED
	call	MIOS_MIDI_MergerSet
	;; send program change event to all slaves
	;; this ensures that the internal patch will be used and
	;; switches off all notes (to avoid note hangups when merger is disabled)
	movf	CS_MENU_SID_S1_CHN, W
	rcall	CS_MENU_UpdateLinkMode_Hlp
	movf	CS_MENU_SID_S2_CHN, W
	rcall	CS_MENU_UpdateLinkMode_Hlp
	movf	CS_MENU_SID_S3_CHN, W
	rgoto	CS_MENU_UpdateLinkMode_Hlp
	
CS_MENU_UpdateLinkMode_Hlp
	andlw	0x0f
	iorlw	0xc0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  Updates all CS registers of the master SID on incoming CCs
;; --------------------------------------------------------------------------
CS_MENU_UpdateCCPara
	clrf	CS_MENU_SID
	call	CS_MENU_MS_GetSIDBase

	clrf	TMP1
CS_MENU_UpdateCCPara_Loop
	;; get CC number
	movf	TMP1, W
	call	SID_SYSEX_TABLE_Get

	;; temporary store number in TMP2
	movwf	TMP2

	;; ignore if EEPROM value
	BRA_IFSET TMP2, 7, ACCESS, CS_MENU_UpdateCCPara_Next

	;; get CC value
	call	SID_CCOUT_Get

	;; temporary store value in TMP3
	movwf	TMP3

	;; transfer to CS_SID register
	movf	TMP1, W
	movff	TMP3, PLUSW0

CS_MENU_UpdateCCPara_Next
	incf	TMP1, F
	movlw	0x7c-1
	cpfsgt	TMP1, ACCESS
	rgoto CS_MENU_UpdateCCPara_Loop

	;; notify that a value has been changed
	bsf	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER

	;; force an display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return

;; --------------------------------------------------------------------------
;;  Updates a CS register on SysEx changes in EDIT mode
;;  IN: Syx number in WREG
;;      value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_UpdateSyxPara
	btfsc	WREG, 7; exit if value stored in EEPROM
	return		
	movwf	TABLAT

	;; get SID base
	clrf	CS_MENU_SID
	call	CS_MENU_MS_GetSIDBase

	;; transfer to CS_SID register
	movf	TABLAT, W
	movff	MIOS_PARAMETER1, PLUSW0

	;; notify that a value has been changed
	bsf	CS_MENU_MODE, CS_MENU_MODE_EDIT_NOTIFIER

	;; force an display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return

;; --------------------------------------------------------------------------
;;  This function increments a given parameter which is indexed like shown
;;  here:
;;     ID of CS_MENU in WREG (example: CS_MENU_OSC)
;;     table (cursor) position in MIOS_PARAMETER1 (example: 0x01 for waveform)
;;     page offset in MIOS_PARAMETER2 (example: 0x00 begins at first line)
;;  used by cs_menu_buttons.inc
;; --------------------------------------------------------------------------
CS_MENU_ButtonInc
	;; save WREG
	movwf	TMP5
	;; save menu settings so that we can jump back to the current menu later
	movff	CS_MENU, SAVED_CS_MENU
	movff	CS_MENU_CURSOR_POS, SAVED_CS_MENU_CURSOR_POS
	movff	CS_MENU_PAGE_OFFSET, SAVED_CS_MENU_PAGE_OFFSET
	;; change to menu
	movwf	CS_MENU
	rcall	CS_MENU_Page_Init
	;; set new page offset and cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS
	movff	MIOS_PARAMETER2, CS_MENU_PAGE_OFFSET
	;; select parameter
	call	CS_MENU_EXEC_SelPar
	;; increment the parameter value, if max value reached, reset value
	movf	CS_MENU_PARAMETER_L, W
	cpfsgt	CS_MENU_PARAMETER_MAX_L, ACCESS
	movlw 0xff
	addlw	1
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_MENU_ParameterUpdate		; update parameter
	call	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter
	;; change back to old parameter if we were already in the menu before
	movf	SAVED_CS_MENU, W
	xorwf	TMP5, W
	bnz	CS_MENU_ButtonInc_End
	movff	SAVED_CS_MENU, CS_MENU
	rcall	CS_MENU_Page_Init
	movff	SAVED_CS_MENU_PAGE_OFFSET, CS_MENU_PAGE_OFFSET
	movff	SAVED_CS_MENU_CURSOR_POS, CS_MENU_CURSOR_POS
CS_MENU_ButtonInc_End
	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return

;; --------------------------------------------------------------------------
;;  This function toggles a given parameter which is indexed like shown
;;  here:
;;     ID of CS_MENU in WREG (example: CS_MENU_OSC)
;;     table (cursor) position in MIOS_PARAMETER1 (example: 0x01 for waveform)
;;     page offset in MIOS_PARAMETER2 (example: 0x00 begins at first line)
;;  used by cs_menu_buttons.inc
;; --------------------------------------------------------------------------
CS_MENU_ButtonToggle
	;; save WREG
	movwf	TMP5
	;; save menu settings so that we can jump back to the current menu later
	movff	CS_MENU, SAVED_CS_MENU
	movff	CS_MENU_CURSOR_POS, SAVED_CS_MENU_CURSOR_POS
	movff	CS_MENU_PAGE_OFFSET, SAVED_CS_MENU_PAGE_OFFSET
	;; change to menu
	movwf	CS_MENU
	rcall	CS_MENU_Page_Init
	;; set new page offset and cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS
	movff	MIOS_PARAMETER2, CS_MENU_PAGE_OFFSET
	;; select parameter
	call	CS_MENU_EXEC_SelPar
	;; toggle the parameter value
	btg	CS_MENU_PARAMETER_L, 0
	rcall	CS_MENU_ParameterUpdate		; update parameter
	call	CS_MENU_EXEC_Hlp_ChangeMenu	; deselect parameter

	;; change back to old menu and cursor pos
	movff	SAVED_CS_MENU, CS_MENU
	rcall	CS_MENU_Page_Init
	movff	SAVED_CS_MENU_PAGE_OFFSET, CS_MENU_PAGE_OFFSET
	movff	SAVED_CS_MENU_CURSOR_POS, CS_MENU_CURSOR_POS

	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function increments a given parameter which is indexed like shown
;;  here:
;;     ID of CS_MENU in WREG (example: CS_MENU_OSC)
;;     table (cursor) position in MIOS_PARAMETER1 (example: 0x01 for waveform)
;;     page offset in MIOS_PARAMETER2 (example: 0x00 begins at first line)
;;     increment value in MIOS_PARAMETER3
;;  used by cs_menu_enc.inc
;; --------------------------------------------------------------------------
CS_MENU_ENC_CSInc
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

	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return
