; $Id$
;
; SID Control Surface Menu Functions
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
	movlw	0x18
	movwf	CS_MENU_REINIT_CFG_CTR

	;; start with master SID, SID button 1-4 depressed
	movlw	0xf1
	movwf	CS_MENU_SELECTED_SID_FLAGS

	;; select left and right channel
	movlw	0x03
	movwf	CS_MENU_SELECTED_SID_LR

	;; set mode of modulation matrix
#if DEFAULT_LEDMATRIX_MODE == 0
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
#else
	bsf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
#endif

	;; initialize button matrix handler
	call	CS_MENU_MATRIX_Init

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

	;; get SID number
	call	CS_MENU_MS_GetSIDNumber

	;; check if edit buffer is filled - exit so long buffer not complete
	call	CS_MENU_MBNET_Tx_GetPatchChk
	skpz
	return

	;; check if edit buffer update has been requested
	BRA_IFCLR CS_STAT2, CS_STAT2_EDIT_UPDATE_REQ, ACCESS, CS_MENU_Handler_NoEditUpdate
CS_MENU_Handler_EditUpdate
	bcf	CS_STAT2, CS_STAT2_EDIT_UPDATE_REQ
	;; update edit buffer and exit
	goto	CS_MENU_MS_UpdateEditBuffer
CS_MENU_Handler_NoEditUpdate

	;; check if tx transfer to slave has been requested and can be granted
 	call	CS_MENU_MS_Send_SysExDump_TxTst

	;; check if meters should be displayed (will also be checked in cs_menu_matrix.inc)
	IRQ_DISABLE
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER
	bsf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	movf	CS_MENU_MATRIX_SELECTED_COL, W		; don't display if one of the column buttons is pressed
	skpz
	bcf	CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP
	IRQ_ENABLE

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
	;; disable page and "big" message
	bcf	CS_STAT, CS_STAT_PAGE_MSG
	bcf	CS_STAT, CS_STAT_BIG_MSG
	;; update OSC flags
	rcall	CS_MENU_Hlp_UpdateOSCFlags

	;; if not in SHIFT page
	BRA_IFSET CS_STAT3, CS_STAT3_SHIFT_PAGE, ACCESS, CS_MENU_Handler_Init_CFGSkip
CS_MENU_Handler_Init_NotCFGSkip
	;; init menu table
	rcall	CS_MENU_Page_Init
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber
	;; set encoder speed of menu encoder
	movlw	0
	rcall	CS_MENU_EncSpeedSet
	;; set parameter
	rcall	CS_MENU_EncParameterSet
	;; set max position of cursor (or patch)
	rcall	CS_MENU_EncMaxSet
	;; clear item counter - used for optimized refresh handling
	clrf	CS_MENU_REFRESH_ITEM_CTR

CS_MENU_Handler_Init_CFGSkip
	;; SID are not available if no connection to MBNET node
	bsf	CS_STAT, CS_STAT_SID_AVAILABLE
	call	CS_MENU_MBNET_Check
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return
	
	;; in main page: print static display elements
	BRA_IFSET CS_STAT3, CS_STAT3_SHIFT_PAGE, ACCESS, CS_MENU_Handler_Init_NotMainPage
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_Handler_Init_NotMainPage
CS_MENU_Handler_Init_MainPage
#if CS_MENU_DISPLAYED_ITEMS > 5
	;; only relevant for 2x40 LCD: clear left half
	movlw	0x00 + 20
	call	MIOS_LCD_CursorSet
	movlw	20
	call	SID_LCD_PrintSpaces

	movlw	0x40 + 20
	call	MIOS_LCD_CursorSet
	movlw	20
	call	SID_LCD_PrintSpaces
#endif

	;; print patch name at lower line
	movlw	0x40 + 5
	call	MIOS_LCD_CursorSet
	call	SID_LCD_PrintPatch

CS_MENU_Handler_Init_NotMainPage

CS_MENU_Handler_Init_End

CS_MENU_Handler_NoInit
	;; exit if SID not available (only update LEDs)
	GOTO_IFCLR CS_STAT, CS_STAT_SID_AVAILABLE, ACCESS, CS_MENU_LED_Update

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
	;; branch if SHIFT page is displayed
	BRA_IFSET CS_STAT3, CS_STAT3_SHIFT_PAGE, ACCESS, CS_MENU_Page_Cfg

	;; branch if menu page is displayed
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_Handler_MenuPage

	;; ------------------------------------------------------------------
	;; else handle mainpage
CS_MENU_Handler_MainPage
	;; skip upper line if page message is print
	BRA_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_Handler_MainPage_End

	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; branch depending on item counter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	CS_MENU_Handler_MainPage_0
	addlw	-1
	bz	CS_MENU_Handler_MainPage_1
	addlw	-1
	bz	CS_MENU_Handler_MainPage_2
	addlw	-1
	bz	CS_MENU_Handler_MainPage_3
	rgoto	CS_MENU_Handler_MainPage_4



	;; print current bank and patch number
	;; NOTE: it's intended that patch number is print before ensemble number, although the
	;; ensemble is the leftmost value!
	;; Printing the patch value first ensures, that value will be displayed immediately
	;; on fast encoder turns
CS_MENU_Handler_MainPage_0
	movlw	0x00 + 5
	call	MIOS_LCD_CursorSet
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
	call	SID_LCD_PrintPatchNumber

	;; print '*' if patch not valid
	call	SID_PBANK_CheckPatchValid
	movlw	' '
	skpz
	movlw	'*'
	call	MIOS_LCD_PrintChar

	;; increment item counter, so that on next call we continue at MainPage_1
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	rgoto	CS_MENU_Handler_MainPage_End


	;; print ensemble number
CS_MENU_Handler_MainPage_1
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet
	call	SID_LCD_PrintEnsembleNumber

	;; print divider
	movlw	'|'
	call	MIOS_LCD_PrintChar

	;; increment item counter, so that on next call we continue at MainPage_3
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	rgoto	CS_MENU_Handler_MainPage_End


	;; print engine name
CS_MENU_Handler_MainPage_2
	movlw	0x00 + 10
	call	MIOS_LCD_CursorSet
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	call	SID_LCD_PrintEngine
	movlw	2
	call	SID_LCD_PrintSpaces

	;; increment item counter, so that on next call we continue at MainPage_2
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	rgoto	CS_MENU_Handler_MainPage_End


	;; print MIDI channel
CS_MENU_Handler_MainPage_3
	;; Chn for Channel
	movlw	0x00 + 14
	call	MIOS_LCD_CursorSet
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	'h'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	'.'
	call	MIOS_LCD_PrintChar

	lfsr	FSR1, CS_MENU_SID_M_CHN
	movf	CS_MENU_SID, W
	andlw	0x03
	addwf	FSR1L, F
	incf	INDF1, W
	andlw	0x7f		; mask out "sent" flag
	call	MIOS_LCD_PrintBCD2

	;; increment item counter, so that on next call we continue at MainPage_2
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	rgoto	CS_MENU_Handler_MainPage_End


	;; print selected SIDs
CS_MENU_Handler_MainPage_4
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movff	MBNET_NODE_AVAIL, PRODL

	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID1
	movlw 'P'
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, CS_MENU_Handler_MainPage_SID1
	movlw	'1'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID1; inverted 1
	movlw 0x02 
CS_MENU_Handler_MainPage_SID1
	btfss	PRODL, 0
	movlw '*'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID2
	movlw 'P'
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 1, ACCESS, CS_MENU_Handler_MainPage_SID2
	movlw	'2'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID2; inverted 2
	movlw 0x03 
CS_MENU_Handler_MainPage_SID2
	btfss	PRODL, 1
	movlw '*'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID3
	movlw 'P'
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 2, ACCESS, CS_MENU_Handler_MainPage_SID3
	movlw	'3'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID3; inverted 3
	movlw 0x04 
CS_MENU_Handler_MainPage_SID3
	btfss	PRODL, 2
	movlw '*'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID4
	movlw 'P'
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 3, ACCESS, CS_MENU_Handler_MainPage_SID4
	movlw	'4'
	btfsc	CS_STAT2, CS_STAT2_PLAY_SID4; inverted 4
	movlw 0x05 
CS_MENU_Handler_MainPage_SID4
	btfss	PRODL, 3
	movlw '*'
	call	MIOS_LCD_PrintChar

	;; print divider
	movlw	'|'
	call	MIOS_LCD_PrintChar

	;; last item reached - clear item counter
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; rgoto	CS_MENU_Handler_MainPage_End


	;; ...and exit
CS_MENU_Handler_MainPage_End
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
	;; calc pointer to function: CS_MENU_TABLES_x + (CS_MENU*CS_MENU_T_ENTRY_LEN) + CS_MENU_ENTRY_HANDLER_OFFSET
	movlw	CS_MENU_T_ENTRY_HANDLER_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	goto	MIOS_HLP_IndirectJump

;; --------------------------------------------------------------------------
;;  initialize the page
;; --------------------------------------------------------------------------
CS_MENU_Page_Init
	;; exit if in main page
	btfsc	CS_MENU, 7
	return

	;; get pointer to menu table depending on CS_MENU (CS_MENU_TABLES + CS_MENU*4)
	movlw	0		; offset
	call	CS_MENU_GetMenuTablesPtr

	;; get address from table, store it in TBLPTR[LH]
	call	MIOS_HLP_AddressFromTable
	clrf	TBLPTRU		; TBLPTRU must be 0 (lower 64k page)

	;; read number of menu entries and save it in CS_MENU_ENTRIES
	tblrd*+
	movff	TABLAT, CS_MENU_ENTRIES
	tblrd*+			; read dummy byte

	;; store pointer in CS_MENU_TABLE_0_[LH], so that other functions can reference
	;; to the entries
	movff	TBLPTRL, CS_MENU_TABLE_0_L
	movff	TBLPTRH, CS_MENU_TABLE_0_H

	;; done in CS_MENU_EXEC_GoToRoot now, so that the cursor doesn't change when we are within a menu page and LCD initialisation is requested
#if 0
	;; clear cursor and page offset
	clrf	CS_MENU_CURSOR_POS
	clrf	CS_MENU_PAGE_OFFSET
#endif
	;; update max value and exit
	rgoto	CS_MENU_EncMaxSet

	
;; --------------------------------------------------------------------------
;;  print the CFG page
;; --------------------------------------------------------------------------
CS_MENU_Page_Cfg
	;; print lower/upper line
	movf	CS_MENU_REFRESH_ITEM_CTR, W	
	bnz	CS_MENU_Page_Cfg_Step1
CS_MENU_Page_Cfg_Step0
	call	CS_MENU_SHIFT_PrintUL
	rgoto	CS_MENU_Page_Cfg_End
CS_MENU_Page_Cfg_Step1
	call	CS_MENU_SHIFT_PrintLL
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
;;  print the root page
;; --------------------------------------------------------------------------
CS_MENU_PAGE_ROOT_ENS_STR STRING 10, 0x00, "Ensemble: "

CS_MENU_Page_Root
	;; print upper line on first step
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	CS_MENU_Page_Root_Refresh_NoUL

CS_MENU_Page_Root_Refresh_UL
	;; skip upper line if page message is print
	BRA_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_Page_Root_Refresh_NoUL

	;; in ensemble menu: print ensemble number, otherwise patch name
	BRA_IFCLR CS_MENU, 6, ACCESS, CS_MENU_Page_Root_Refresh_UL_P
CS_MENU_Page_Root_Refresh_UL_E
	TABLE_ADDR CS_MENU_PAGE_ROOT_ENS_STR
	call	MIOS_LCD_PrintString

	;; print ensemble number
	call	SID_LCD_PrintEnsembleNumber

	;; print '*' if ensemble not valid (no BankStick)
	call	SID_EBANK_CheckValid
	movlw	' '
	skpz
	movlw	'*'
	call	MIOS_LCD_PrintChar

	;; fill rest with spaces
	movlw	5
	call	SID_LCD_PrintSpaces

	rgoto	CS_MENU_Page_Root_Refresh_UL_C

CS_MENU_Page_Root_Refresh_UL_P
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; print patch name
	call	SID_LCD_PrintPatch

	;; print bank number and patch
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
	call	SID_LCD_PrintPatchNumber

CS_MENU_Page_Root_Refresh_UL_C
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

	;; if in MODIFY_SEQ mode, print drum editing screen
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ, ACCESS, CS_MENU_Page_EditSeq

	;; print menu item name
	;; skip if page or "big" message is print
	BRA_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_Page_Parameters_SkipUL
	BRA_IFSET CS_STAT, CS_STAT_BIG_MSG, ACCESS, CS_MENU_Page_Parameters_SkipUL
CS_MENU_Page_Parameters_UL
	movlw	0x00
	call	CS_MENU_PrintILine
CS_MENU_Page_Parameters_SkipUL

	;; print parameter value 
	;; skip if "big" big message is print
	BRA_IFSET CS_STAT, CS_STAT_BIG_MSG, ACCESS, CS_MENU_Page_Parameters_SkipLL
CS_MENU_Page_Parameters_LL
	movlw	0x40
	call	CS_MENU_PrintPLine
CS_MENU_Page_Parameters_SkipLL

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

	;; don't set cursor if "big message" is visible
	btfsc	CS_STAT, CS_STAT_BIG_MSG
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
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd
	return


;; --------------------------------------------------------------------------
;;  print parameter name on whole screen
;;  called when a parameter assignment is edited
;; --------------------------------------------------------------------------
				     ;;  <------------------>
TEXT_CS_MENU_PARAMETER STRING 20, 0x00, "Parameter Assign.:  "
				     ;;  12341234123412341234
CS_MENU_Page_BigMsg_ParName
	;; notify that "big" message will be print (until item is deselected)
	bsf	CS_STAT, CS_STAT_BIG_MSG

	;; print uppper line
	TABLE_ADDR TEXT_CS_MENU_PARAMETER
	call	MIOS_LCD_PrintString

	;; print lower line
	movlw	0x40
	call	MIOS_LCD_CursorSet

	movf	CS_MENU_PARAMETER_L, W
	goto	SID_LCD_LongParName


;; --------------------------------------------------------------------------
;;  print patch and bank
;;  called when a parameter assignment is edited
;; --------------------------------------------------------------------------
				     ;;  <------------------>
TEXT_CS_MENU_PATCH     STRING  7, 0x00, "Patch: "
TEXT_CS_MENU_BANK      STRING  7, 0x00, "Bank:  "
				     ;;  12341234123412341234

CS_MENU_Page_BigMsg_Bank
	TABLE_ADDR TEXT_CS_MENU_BANK
	rgoto	CS_MENU_Page_BigMsg_Bank_Cont

CS_MENU_Page_BigMsg_Patch
	TABLE_ADDR TEXT_CS_MENU_PATCH
	rgoto	CS_MENU_Page_BigMsg_Bank_Cont

CS_MENU_Page_BigMsg_Bank_Cont
	;; notify that "big" message will be print (until item is deselected)
	bsf	CS_STAT, CS_STAT_BIG_MSG

	;; print uppper line
	call	MIOS_LCD_PrintString

	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
	call	SID_LCD_PrintPatchNumber

	movlw	9
	call	SID_LCD_PrintSpaces


	;; print lower line
	movlw	0x40
	call	MIOS_LCD_CursorSet

	call	SID_LCD_PrintPatchEE	; (new name not loaded into edit buffer yet!)

	movlw	4
	goto	SID_LCD_PrintSpaces


;; --------------------------------------------------------------------------
;;  name editing screen
;; --------------------------------------------------------------------------
CS_MENU_Page_EditName
	;; skip upper line if page message is print
	BRA_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_Page_EditName_SkipUL
CS_MENU_Page_EditName_UL
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print steps
	call	SID_LCD_PrintPatch

	;; fill rest with spaces
	movlw	40-16
	rcall	CS_MENU_Hlp_PrintSpaces

CS_MENU_Page_EditName_SkipUL
	;; flashing cursor: set space
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_Page_EditName_NoSpc
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet
	movlw	' '
	call	MIOS_LCD_PrintChar
CS_MENU_Page_EditName_NoSpc

	;; print button options @ position 0x40 (second line)
	TABLE_ADDR TEXT_CS_MENU_EDIT_NAME_OPTIONS
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
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd
	return

				  	     ;;  <------------------>
TEXT_CS_MENU_EDIT_NAME_OPTIONS STRING 20, 0x40, "SAVE  <   >  Del Clr"
					     ;;  12341234123412341234

;; --------------------------------------------------------------------------
;;  drum sequence editing screen
;; --------------------------------------------------------------------------
CS_MENU_Page_EditSeq
	;; skip upper line if page message is print
	BRA_IFSET CS_STAT, CS_STAT_PAGE_MSG, ACCESS, CS_MENU_Page_EditSeq_SkipUL
CS_MENU_Page_EditSeq_UL
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print track steps (16 characters)
	call	CS_MENU_PRINT_SeqSteps

	;; one space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; if rotate button pressed, print "Rot", otherwise sequencer and track number
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE, ACCESS, CS_MENU_Page_EditSeq_UL_Rot
CS_MENU_Page_EditSeq_UL_Norm
	;; print track
	incf	CS_MENU_WT_SEQ, W
	call	MIOS_LCD_PrintBCD1
	movlw	'_'
	call	MIOS_LCD_PrintChar
	movf	CS_MENU_SELECTED_TRK, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_MENU_Page_EditSeq_UL_Cont

CS_MENU_Page_EditSeq_UL_Rot
	movlw	'R'
	call	MIOS_LCD_PrintChar
	movlw	'o'
	call	MIOS_LCD_PrintChar
	movlw	't'
	call	MIOS_LCD_PrintChar
	;; 	rgoto	CS_MENU_Page_EditSeq_UL_Cont

CS_MENU_Page_EditSeq_UL_Cont
	;; fill rest with spaces
	movlw	40-20
	rcall	CS_MENU_Hlp_PrintSpaces

CS_MENU_Page_EditSeq_SkipUL
	;; not if rotate function active
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE, ACCESS, CS_MENU_Page_EditSeq_NoSpc

	;; flashing cursor: set space
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_MENU_Page_EditSeq_NoSpc
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet
	movlw	' '
	call	MIOS_LCD_PrintChar
CS_MENU_Page_EditSeq_NoSpc

	;; print button options @ position 0x40 (second line)
	TABLE_ADDR TEXT_CS_MENU_EDIT_SEQ_OPTIONS
	call	MIOS_LCD_PrintString

	;; print 20 spaces (for 2x40 displays)
#if CS_MENU_DISPLAYED_ITEMS > 5
	movlw	20
	rcall	CS_MENU_Hlp_PrintSpaces
#endif

	;; if not GLCD, set cursor
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	return

	;; not if rotate function active
	btfsc	CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE
	return

	;; set cursor to: 0x00 + CS_MENU_NAME_POS
	movf	CS_MENU_NAME_POS, W
	call	MIOS_LCD_CursorSet

	;; enable blinking CLCD cursor and exit
	movlw	0x0e
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd
	return


TEXT_CS_MENU_EDIT_SEQ_OPTIONS STRING 20, 0x40, "GAS Rot Rnd Clr DRM "
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
	call	CS_MENU_GetMenuID_SAV
	xorwf	CS_MENU, W
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

	;; quick hack: if first character is "$", print WTPos offset
	;; used in WT Editor
	tblrd*
	movlw	'$'
	cpfseq	TABLAT, ACCESS
	rgoto CS_MENU_PrintSEntry_Norm
CS_MENU_PrintSEntry_WT
	movlw	'$'
	CALL_IFCLR CS_STAT2, CS_STAT2_WT_DEC_VIEW, ACCESS, MIOS_LCD_PrintChar

	;; hack: CS_MENU_FSR0_WTPos expects WT number (L:0-3/M:0-5) in CS_MENU_PARAMETER_IX, and returns WT address in FSR0L
	;; it sets ZERO flag if address outside begin/end range
	decf	CS_MENU_PAGE_OFFSET, W
	addwf	CS_MENU_REFRESH_ITEM_CTR, W
	movwf	CS_MENU_PARAMETER_IX
	call	CS_MENU_FSR0_WTPos
	bnz	CS_MENU_PrintSEntry_WT_Ok
CS_MENU_PrintSEntry_WT_Inv
	movlw	'<'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	CALL_IFSET CS_STAT2, CS_STAT2_WT_DEC_VIEW, ACCESS, MIOS_LCD_PrintChar
	movlw	'>'
	goto	MIOS_LCD_PrintChar

CS_MENU_PrintSEntry_WT_Ok	
	movf	FSR0L, W
	addlw	0-LOW(SID_Ix_WAVETABLE)
	GOTO_IFSET CS_STAT2, CS_STAT2_WT_DEC_VIEW, ACCESS, MIOS_LCD_PrintBCD3
	goto	MIOS_LCD_PrintHex2

CS_MENU_PrintSEntry_Norm	
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
;;  Encoder number in WREG
;;  If >0, function has to be called after CS_MENU_PARAMETER_MAX_[LH] has been updated!
;; --------------------------------------------------------------------------
CS_MENU_EncSpeedSet
	;; if encoder number > 0, always use ModVal speed
	movwf	PRODL		; store number in PRODL
	andlw	0xff		; fix ZERO flag
	bnz	CS_MENU_EncSpeedSet_ModVal

	;; following branch only used with menu encoder
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_EncSpeedSet_Main
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EncSpeedSet_ModVal

CS_MENU_EncSpeedSet_SelItem
	;; "select menu item" mode: set encoder speed to "normal"
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movf	PRODL, W	; encoder number
	goto	MIOS_ENC_SpeedSet

	;; in main page (where the patch number is selected with the encoder)
	;; - set to "fast mode" if SHIFT button is *not* pressed (fast selection)
	;; - set to "normal mode" if SHIFT button is pressed (slow selection)
CS_MENU_EncSpeedSet_Main
	movlw	2			; this speed value is taken when ENC_SPEED_FAST is selected
	movwf	MIOS_PARAMETER2
	movlw	MIOS_ENC_SPEED_FAST	; FAST or NORMAL mode depending on state of shift button
	btfsc	CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movf	PRODL, W		; encoder number
	goto	MIOS_ENC_SpeedSet

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
	;; "select menu item" mode: set encoder speed to "normal"
	movlw	0x00
	movwf	MIOS_PARAMETER3
	movlw	MIOS_ENC_SPEED_NORMAL
	movwf	MIOS_PARAMETER1
	movf	PRODL, W	; encoder number
	goto	MIOS_ENC_SpeedSet

CS_MENU_EncSpeedSet_ModVal_GT1F
	;; max value > 0x1f: set fast speed
	movf	CS_MENU_PARAMETER_MAX_H, W
	movwf	TMP2
	rlf	TMP1, F
	rlf	TMP2, F
	movlw	0x05		; saturate if value >= 5
	cpfsgt  TMP2, ACCESS	; (IFLEQ)
	movf	TMP2, W

	;; increase speed if shift button NOT pressed
	btfss	CS_MENU_MODE, CS_MENU_MODE_SHIFT_PRESSED
	addlw 2

	movwf	MIOS_PARAMETER2

	movlw	MIOS_ENC_SPEED_FAST
	movwf	MIOS_PARAMETER1
	movf	PRODL, W	; encoder number
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
	movf	CS_MENU_ENTRIES, W
	addlw	-CS_MENU_DISPLAYED_ITEMS
	skpc
	movlw	0x00
	movwf	CS_MENU_PARAMETER_MAX_L	; low-byte of max value
	clrf	CS_MENU_PARAMETER_MAX_H	; high-byte of max value
	return

CS_MENU_ParameterMaxSet_Ens
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
	goto	CS_MENU_R2P_Handler

CS_MENU_EncParameterSet_NoItem

	;; set CS_MENU_PARAMETER depending on CS_MENU[7] (main or menu page)
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_ParameterSet_Main
CS_MENU_ParameterSet_Root
	movf	CS_MENU_PAGE_OFFSET, W
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return

CS_MENU_ParameterSet_Main
	;; also called from CS_MENU_ENC_Handler, so that CS_MENU_PARAMETER_[LH] contain the value
	;; depending on menu button state
	BRA_IFCLR CS_MENU_MODE, CS_MENU_MODE_MENU_PRESSED, ACCESS, CS_MENU_ParameterSet_Main_Patch
CS_MENU_ParameterSet_Main_Ens
	movf	SID_ENSEMBLE, W
	rgoto	CS_MENU_ParameterSet_Main_End

CS_MENU_ParameterSet_Main_Patch
	;; get SID number
	call	CS_MENU_MS_GetSIDNumber
	;; copy selected patch * 4 to CS_MENU_PARAMETER_[LH]
	call	CS_MENU_MS_GetSIDPatch
	;; 	rgoto	CS_MENU_ParameterSet_Main_End
	
CS_MENU_ParameterSet_Main_End
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	return


;; --------------------------------------------------------------------------
;;  called by cs_menu_buttons.inc when a parameter has been selected
;;  IN: new cursor position in WREG
;; --------------------------------------------------------------------------
CS_MENU_Select
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

	;; ignore if "big" message is print
	btfsc	CS_STAT, CS_STAT_BIG_MSG
	return

	;; if in name editing mode, branch to CS_MENU_Select_NameFunc
	BRA_IFSET CS_STAT, CS_STAT_MODIFY_NAME, ACCESS, CS_MENU_Select_NameFunc

	;; if in drum sequence editing mode, branch to CS_MENU_Select_SeqFunc
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ, ACCESS, CS_MENU_Select_SeqFunc

	;; if in main page (CS_MENU[7] set), branch to root menu
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_Select_Menu
CS_MENU_Select_Main
	;; branch depending on button number
	;; first button branches to ENS menu, all others to normal root page
	andlw	0xff		; fix zero flag
	skpnz
	goto	CS_MENU_EXEC_GoToEnsRoot
	goto	CS_MENU_EXEC_GoToRoot

CS_MENU_Select_Menu
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
	goto	CS_MENU_EXEC_Handler

;; --------------------------------------------------------------------------
;;  called by cs_menu_buttons.inc when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_MENU_Exec
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

	;; clear CS_SELECT_CTR (so that new message appears immediately)
	clrf	CS_SELECT_CTR

	;; clear refresh item counter --- ensures that the whole display will be refreshed
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; exit if we are in main menu
	btfsc	CS_MENU, 7
	return

	;; execute function
	;; calc pointer to function: CS_MENU_TABLES + (CS_MENU*CS_MENU_T_ENTRY_LEN) + CS_MENU_ENTRY_EXEC_OFFSET
	movlw	CS_MENU_T_ENTRY_EXEC_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	goto	MIOS_HLP_IndirectJump


;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc and CS_MENU_PageUpDown when the ensemble number should be updated
;;  ensemble number in SID_ENSEMBLE
;; --------------------------------------------------------------------------
CS_MENU_EnsUpdate
	;; init ensemble and exit
	goto	SID_ENS_Init


;; --------------------------------------------------------------------------
;;  called by cs_menu_enc.inc and CS_MENU_PageUpDown when the patch number should be updated
;;  patch number in CS_MENU_PARAMETER_[LH]
;; --------------------------------------------------------------------------
CS_MENU_PatchUpdate
	;; save current patch if it has been changed
	rcall	CS_MENU_PatchUpdate_SaveOld

	;; determine bank of currently selected SID -> SID_BANK
	call	CS_MENU_MS_GetSIDNumber
	call	CS_MENU_MS_GetSIDBank
	movwf	SID_BANK

	;; new patch -> SID_PATCH
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x7f
	movwf	SID_PATCH

	;; copy patch (and bank) number to all selected SIDs
	clrf	CS_MENU_SID		; using CS_MENU_SID as counter
CS_MENU_PatchUpdate_Loop
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_SID_FLAGS, W
	bz	CS_MENU_PatchUpdate_Next
	;; copy changed CS_MENU_PARAMETER_L to saved patch number and init sound
	call	CS_MENU_MS_GetSIDPatch; sets FSR1 to patch register
	movff	SID_PATCH, INDF1
	;; copy bank
	call	CS_MENU_MS_GetSIDBank; set FSR1 to bank register
	movff	SID_BANK, INDF1
	;; request update of CS parameters
	call	CS_MENU_MS_Send_SysExDump_TxReq
	;; store new patch and bank number in ensemble
	lfsr	FSR0, SID_ENS_BUFFER + SID_ENSx_PATCH
	movff	SID_PATCH, INDF0
	call	CS_MENU_MS_SendEnsParameter
	lfsr	FSR0, SID_ENS_BUFFER + SID_ENSx_BANK
	movff	SID_BANK, INDF0
	call	CS_MENU_MS_SendEnsParameter
CS_MENU_PatchUpdate_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_PatchUpdate_Loop

	;; restore CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

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

	;; set page offset directly
	movwf	CS_MENU_PAGE_OFFSET

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
	call	CS_MENU_P2R_Handler
CS_MENU_ParameterUpdate_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_ParameterUpdate_Loop

	;; temporary print long label on special menu items which are listed below
	;; get handler IDs
	rcall	CS_MENU_GetHandlerIDs
	;; PRINT ID in MIOS_PARAMETER1
	movf	MIOS_PARAMETER1, W
	xorlw	PRINT_KNB_PAR			; Knob Assignment
	bz	CS_MENU_ParameterUpdateParName
	xorlw	PRINT_WTx_PAR ^ PRINT_KNB_PAR	; WT Assignment
	bz	CS_MENU_ParameterUpdateParName
	xorlw	PRINT_M_D_Vx_ASG ^ PRINT_WTx_PAR; Velocity/Pitchbender Assignment
	bz	CS_MENU_ParameterUpdateParName
	xorlw	PRINT_PAT ^ PRINT_M_D_Vx_ASG	; Patch Name
	bz	CS_MENU_ParameterUpdatePatName
	xorlw	PRINT_BNK ^ PRINT_PAT		; Bank Name
	bz	CS_MENU_ParameterUpdatePatBank
	rgoto	CS_MENU_ParameterUpdateNoBigMsg

CS_MENU_ParameterUpdateParName
	rcall	CS_MENU_Page_BigMsg_ParName
	rgoto	CS_MENU_ParameterUpdateParName_C
CS_MENU_ParameterUpdatePatBank
	rcall	CS_MENU_Page_BigMsg_Bank
	rgoto	CS_MENU_ParameterUpdateParName_C
CS_MENU_ParameterUpdatePatName
	rcall	CS_MENU_Page_BigMsg_Patch
	;; 	rgoto	CS_MENU_ParameterUpdateParName_C
CS_MENU_ParameterUpdateParName_C
CS_MENU_ParameterUpdateNoBigMsg

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
	clrf	TBLPTRU		; TBLPTRU must be 0 (lower 64k page)
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
	;; base address of edit buffer -> FSR0
	lfsr	FSR0, SID_EDIT_BUFFER

	;; copy offset to register entry (must be the first entry!) to tablepointer
	movff	CS_MENU_ENTRY_TBLPTRL, TBLPTRL
	movff	CS_MENU_ENTRY_TBLPTRH, TBLPTRH

	;; get register address, add it to FSR0L and store it also in CS_MENU_PARAMETER_IX
	tblrd*+
	movf	TABLAT, W
	movwf	CS_MENU_PARAMETER_IX
	addwf	FSR0L, F

	;; get pointer to PRINT_IX offset
	movlw	CS_MENU_ENTRY_PRINT_IX_OFFSET
	rcall	CS_MENU_Hlp_GetPointer

	;; PRINT ID in MIOS_PARAMETER1
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER1

	;; EXEC ID in MIOS_PARAMETER2
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER2

	;; R2PP2R ID in MIOS_PARAMETER3
	tblrd*+
	movff	TABLAT, MIOS_PARAMETER3

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
;;  if ensemble should be saved
;;  (target ensemble in CS_MENU_SAVE_ENS
;; --------------------------------------------------------------------------
CS_MENU_SaveEns
	call	SID_EBANK_CheckValid
	bnz	CS_MENU_SaveEns_End

	;; switch to new ensemble
	movff	CS_MENU_SAVE_ENS, SID_ENSEMBLE

	;; loop through all SIDs
	clrf	CS_MENU_SID
CS_MENU_SaveEns_Loop
	;; load setup into ENS buffer
	movf	CS_MENU_SID, W
	bnz	CS_MENU_SaveEns_Loop_S
CS_MENU_SaveEns_Loop_M
	call	SID_ENS_LocalSetupIntoBuffer
	rgoto	CS_MENU_SaveEns_Loop_Cont
CS_MENU_SaveEns_Loop_S
	call	CS_MENU_MBNET_Tx_GetEns
	;; skip if slave not available
	movff	MBNET_NODE_AVAIL, PRODL
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	PRODL, W
	bz	CS_MENU_SaveEns_Loop_Next
CS_MENU_SaveEns_Loop_Cont
	;; store into BankStick
	movf	CS_MENU_SID, W
	call	SID_ENS_StoreSIDSetup

CS_MENU_SaveEns_Loop_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, CS_MENU_SaveEns_Loop

	;; store new ensemble number as default ensemble, so that it will be selected again after power-on
	call	SID_ENS_StoreDefaultNum

	;; restore SID_ENS_BUFFER
	call	CS_MENU_MS_UpdateEnsBuffer

CS_MENU_SaveEns_End
	return


;; --------------------------------------------------------------------------
;;  if patch should be saved
;;  (target patch in CS_MENU_SAVE_PATCH/CS_MENU_SAVE_BANK)
;; --------------------------------------------------------------------------
CS_MENU_SavePatch
	;; get current CS_MENU_SID
	call	CS_MENU_MS_GetSIDNumber

	;; switch patch/bank to CS_MENU_SAVE_PATCH/BANK
	movff	CS_MENU_SAVE_PATCH, SID_PATCH
	movff	CS_MENU_SAVE_BANK, SID_BANK

	;; check if target location available
	call	SID_PBANK_CheckPatchValid
	bnz	CS_MENU_SavePatch_End
	
	;; store edit buffer into BankStick (8 * 64byte pages)
	clrf	EEADR
	clrf	EEADRH
CS_MENU_SavePatch_Loop
	clrwdt	; feed watchdog
	lfsr	FSR1, SID_EDIT_BUFFER
	movf	EEADR, W
	addwf	FSR1L, F
	movf	EEADRH, W
	addwfc	FSR1H, F
	call	SID_PBANK_WritePage
	movf	EEADR, W
	bnz	CS_MENU_SavePatch_Loop
	movlw	2-1
	cpfsgt	EEADRH, ACCESS
	rgoto CS_MENU_SavePatch_Loop

	;; switch to new patch/bank
	call	CS_MENU_MS_GetSIDPatch	; get pointer to patch number
	movff	SID_PATCH, INDF1
	call	CS_MENU_MS_GetSIDBank	; get pointer to bank number
	movff	SID_BANK, INDF1

	;; store new patch also in ensemble
	lfsr	FSR0, SID_ENS_BUFFER + SID_ENSx_PATCH
	movff	SID_PATCH, INDF0
	call	CS_MENU_MS_SendEnsParameter

	lfsr	FSR0, SID_ENS_BUFFER + SID_ENSx_BANK
	movff	SID_BANK, INDF0
	call	CS_MENU_MS_SendEnsParameter

CS_MENU_SavePatch_End
	return

;; --------------------------------------------------------------------------
;;  Updates a CS register on SysEx changes in EDIT mode
;;  IN: Syx number in WREG
;;      value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_UpdateSyxPara
#if 0
	btfsc	WREG, 7; exit if value stored in EEPROM
	return		
	movwf	TABLAT

	;; get SID base
	clrf	CS_MENU_SID
	lfsr	FSR0, SID_EDIT_BUFFER

	;; transfer to CS_SID register
	movf	TABLAT, W
	movff	MIOS_PARAMETER1, PLUSW0
#endif
	;; TODO

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
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

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
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

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
;;     used encoder in CS_MENU_USED_ENCODER (for new speed setting)
;;  used by cs_menu_enc.inc
;; --------------------------------------------------------------------------
CS_MENU_ENC_CSInc
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

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

	;; update encoder speed according to new max setting
	;; will overwrite MIOS_PARAMETER[123] !!!
	movf	CS_MENU_USED_ENCODER, W
	rcall	CS_MENU_EncSpeedSet

	;; deselect parameter
	call	CS_MENU_EXEC_Hlp_Deselect

	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return


;; --------------------------------------------------------------------------
;;  This function increments/decrements the OSC/LFO/ENV/MOD/TRG/KNB/Patch value
;;  depending on the menu context
;;  Note: if the _NoWrap variation is called, the number won't be wrapped
;;  IN: incrementer (1 or -1) in WREG
;; --------------------------------------------------------------------------
CS_MENU_PageUpDown_NoWrap
	bcf	MIOS_PARAMETER1, 0	; store wrap flag in MIOS_PARAMETER1.0
	rgoto	CS_MENU_PageUpDown_NoWrap_Cont

CS_MENU_PageUpDown
	bsf	MIOS_PARAMETER1, 0	; store wrap flag in MIOS_PARAMETER1.0

CS_MENU_PageUpDown_NoWrap_Cont
	;; store incrementer in TMP1
	;; ensure that it is either 1 or -1
	clrf	TMP1
	incf	TMP1, F
	btfsc	WREG, 7
	setf TMP1

	;; increment patch/ensemble if in main menu
	BRA_IFCLR CS_MENU, 7, ACCESS, CS_MENU_PageUpDown_NotMain
CS_MENU_PageUpDown_Main
	movlw	SID_PATCH
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MENU_PRESSED
	movlw SID_ENSEMBLE
	rgoto	CS_MENU_PageUpDown_Main_Cont
CS_MENU_PageUpDown_NotMain
	;; get variable which should be modified - if 0, do nothing
	movlw	CS_MENU_T_ENTRY_NUMVAR_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	tblrd*+
	movf	TABLAT, W
	skpnz
	rgoto	CS_MENU_PageUpDown_Inv
CS_MENU_PageUpDown_Main_Cont
	movwf	FSR0L		; store pointer to variable in FSR0
	clrf	FSR0H

	;; if variable is SID_PATCH, get current patch number and store it into SID_PATCH
	xorlw	SID_PATCH
	bnz	CS_MENU_PageUpDown_DontGetPatch
CS_MENU_PageUpDown_GetPatch
	call	CS_MENU_MS_GetSIDPatch
	movwf	SID_PATCH
CS_MENU_PageUpDown_DontGetPatch

	;; get max value -> TMP2
	tblrd*+
	movf	TABLAT, W
	btfsc	CS_MENU, 7; (patch/ensemble)
	movlw 0x7f	
	movwf	TMP2

	;; wrap variable if allowed, otherwise saturate at boundaries
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, CS_MENU_PageUpDown_Wrap
CS_MENU_PageUpDown_Sat
	movf	TMP1, W
	addwf	INDF0, F
	BRA_IFSET TMP1, 7, ACCESS, CS_MENU_PageUpDown_SatDown
CS_MENU_PageUpDown_SatUp
	movf	TMP2, W
	cpfslt	INDF0, ACCESS
	movwf INDF0
	rgoto	CS_MENU_PageUpDown_SatCont
CS_MENU_PageUpDown_SatDown
	movf	TMP2, W
	btfsc	INDF0, 7
	clrf INDF0
	rgoto	CS_MENU_PageUpDown_SatCont


CS_MENU_PageUpDown_Wrap
	movf	TMP1, W
	addwf	INDF0, F
	BRA_IFSET TMP1, 7, ACCESS, CS_MENU_PageUpDown_WrapDown
CS_MENU_PageUpDown_WrapUp
	movf	TMP2, W
	addlw	1
	cpfslt	INDF0, ACCESS
	clrf INDF0
	rgoto CS_MENU_PageUpDown_WrapCont
CS_MENU_PageUpDown_WrapDown
	movf	TMP2, W
	btfsc	INDF0, 7
	movwf INDF0
	;; 	rgoto CS_MENU_PageUpDown_WrapCont
CS_MENU_PageUpDown_WrapCont
CS_MENU_PageUpDown_SatCont

	;; if PRINT_VAR_123: call the OSC flags update function and set FSR0 to CS_MENU_SELECTED_OSC_FLAGS
	;; if PRINT_LR: if zero, increment by one, so that only L, R or LR selectable
	BRA_IFSET CS_MENU, 7, ACCESS, CS_MENU_PageUpDown_NoSpecialFunc
	movlw	CS_MENU_T_ENTRY_PRINT_IX_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	tblrd*+
	movf	TABLAT, W
	xorlw	PRINT_VAR_123
	bz	CS_MENU_PageUpDown_OSC
	xorlw	PRINT_VAR_LR ^ PRINT_VAR_123
	bz	CS_MENU_PageUpDown_LR
	rgoto	CS_MENU_PageUpDown_SF_Cont
CS_MENU_PageUpDown_OSC	
	call	CS_MENU_Hlp_UpdateOSCFlags
	lfsr	FSR0, CS_MENU_SELECTED_OSC_FLAGS	; (for correct PRINT_VAR_123)
	rgoto	CS_MENU_PageUpDown_SF_Cont

CS_MENU_PageUpDown_LR
	BRA_IFSET TMP1, 7, ACCESS, CS_MENU_PageUpDown_LR_Down
CS_MENU_PageUpDown_LR_Up
	movf	INDF0, W
	skpnz
	incf	INDF0, F
 	rgoto	CS_MENU_PageUpDown_SF_Cont

CS_MENU_PageUpDown_LR_Down
	movf	INDF0, W
	skpnz
	decf	INDF0, W
	andlw	0x03
	movwf	INDF0
	;; 	rgoto	CS_MENU_PageUpDown_SF_Cont

CS_MENU_PageUpDown_SF_Cont
CS_MENU_PageUpDown_NoSpecialFunc

#if CS_MENU_DISPLAYED_ITEMS >= 5
	;; don't print message if in editor mode (track is displayed at right side)
	;; will only work if LCD display size >= 20
	BRA_IFSET CS_STAT3, CS_STAT3_MODIFY_SEQ, ACCESS, CS_MENU_PageUpDown_End_NoMsg
#endif

	;; special handling for patch/ensemble (only used in menu page, no special display mode required)
	movf	FSR0L, W
	xorlw	SID_PATCH
	bz	CS_MENU_PageUpDown_Patch
	xorlw	SID_ENSEMBLE ^ SID_PATCH
	bz	CS_MENU_PageUpDown_Ens
	rgoto	CS_MENU_PageUpDown_NoPatchOrEns
CS_MENU_PageUpDown_Patch
	;; select new patch
	movff	SID_PATCH, CS_MENU_PARAMETER_L
	rcall	CS_MENU_PatchUpdate
	rgoto	CS_MENU_PageUpDown_End_NoMsg
CS_MENU_PageUpDown_Ens
	;; select new ensemble (ensemble number already in SID_ENSEMBLE)
	rcall	CS_MENU_EnsUpdate
	rgoto	CS_MENU_PageUpDown_End_NoMsg
CS_MENU_PageUpDown_NoPatchOrEns

	;; set cursor
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print new variable content
	movlw	'['
	call	MIOS_LCD_PrintChar
	movlw	CS_MENU_T_ENTRY_PGNAME_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	movlw	3
	call	MIOS_LCD_PrintPreconfString

	movlw	CS_MENU_T_ENTRY_PRINT_IX_OFFSET
	call	CS_MENU_GetMenuTablesPtr
	tblrd*+
	movf	TABLAT, W
	xorlw	PRINT_VAR_DECP1
	bnz	CS_MENU_PageUpDown_NotDecP1
CS_MENU_PageUpDown_DecP1
	;; special measure for DECP1 - print number without spaces

	;; branch if value >= 10
	movlw	8
	cpfsgt	INDF0, ACCESS
	rgoto CS_MENU_PageUpDown_DecP1_LT9
CS_MENU_PageUpDown_DecP1_GE9
	incf	INDF0, W
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_MENU_PageUpDown_DecP1_Cont
CS_MENU_PageUpDown_DecP1_LT9
	incf	INDF0, W
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_MENU_PageUpDown_DecP1_Cont
CS_MENU_PageUpDown_NotDecP1
	movlw	' '
	call	MIOS_LCD_PrintChar
	movf	TABLAT, W
	call	CS_MENU_PRINT_Handler
CS_MENU_PageUpDown_DecP1_Cont

	movlw	']'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, MIOS_LCD_Cmd	

	rgoto	CS_MENU_PageUpDown_End


CS_MENU_PageUpDown_Inv_STR STRING 12, 0x00, "[Not here!] "
CS_MENU_PageUpDown_Inv
	TABLE_ADDR CS_MENU_PageUpDown_Inv_STR
	call	MIOS_LCD_PrintString
	;; 	rgoto	CS_MENU_PageUpDown_End

CS_MENU_PageUpDown_End
	;; enable temporary page message (LCD won't be updated until next DISPLAY_INIT_REQ
	bsf	CS_STAT, CS_STAT_PAGE_MSG

CS_MENU_PageUpDown_End_NoMsg
	;; request display update (will only update LEDs, the rest is blocked by the CS_STAT_PAGE_MSG flag)
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; if parameter was selected before: select it again for proper CS_MENU_PARAMETER_* initialisation
	btfss	CS_STAT, CS_STAT_MODIFY_PARAMETER
	return
	goto	CS_MENU_EXEC_SelPar


;; --------------------------------------------------------------------------
;;  This function returns the CS_MENU_x_xxx ID depending on selected engine
;; --------------------------------------------------------------------------
CS_MENU_GetMenuID_OSC
	movlw	0*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_FIL
	movlw	1*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_LFO
	movlw	2*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_ENV
	movlw	3*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_MOD
	movlw	4*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_KNB
	movlw	5*4
	rgoto	CS_MENU_GetMenuID_Cont
CS_MENU_GetMenuID_SAV
	movlw	6*4
	;; 	rgoto	CS_MENU_GetMenuID_Cont

CS_MENU_GetMenuID_Cont
	movwf	PRODL
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	addwf	PRODL, W
	addlw	LOW(CS_MENU_TABLES_IDS)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_MENU_TABLES_IDS)
	addwfc	TBLPTRH, F
	clrf	TBLPTRU
	movlw	UPPER(CS_MENU_TABLES_IDS)
	addwfc	TBLPTRU, F
	tblrd*+
	movf	TABLAT, W
	return

;; --------------------------------------------------------------------------
;;  This function returns the pointer to a CS_MENU_TABLES_x entry 
;;  depending on selected engine
;;  IN: table offset in WREG
;;      menu page in CS_MENU
;;  USES: PROD[LH] and FSR1L (!!!)
;; --------------------------------------------------------------------------
CS_MENU_GetMenuTablesPtr
	movwf	FSR1L		; temporary store offset in FSR1L
	;; FSR1L is a very bad choice (unexpected temporary register) --- try to find a better one!

	;; get pointer to CS_MENU_TABLES_x depending on selected engine
	movff	SID_EDIT_BUFFER + SID_Ix_ENGINE, WREG
	andlw	0x03
	btfsc	CS_MENU, 6; extra: select ensemble menu structure if flag 6 is set)
	movlw 0x04	
	TABLE_ADDR_MUL_W CS_MENU_TABLES_ENGINE, 2

	;; get address from table (we have a nice MIOS help function for this)
	call	MIOS_HLP_AddressFromTable
	clrf	TBLPTRU		; TBLPTRU must be 0 (lower 64k page)

	;; select page entry
	movf	CS_MENU, W
	andlw	0x3f
	mullw	CS_MENU_T_ENTRY_LEN
	movf	PRODL, W
	addwf	FSR1L, W	; add offset
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	return


;; --------------------------------------------------------------------------
;;  special functions of name editing mode
;;  IN: function which should be executed in WREG (CS_MENU_PAGE_OFFSET has to be substracted!)
;; --------------------------------------------------------------------------
CS_MENU_Select_NameFunc
	;; exit if SID not available
	btfss	CS_STAT, CS_STAT_SID_AVAILABLE
	return

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
	JUMPTABLE_2BYTES 6	; 6 entries
	rgoto	CS_MENU_Select_NameFunc_Save
	rgoto	CS_MENU_Select_NameFunc_Dec
	rgoto	CS_MENU_Select_NameFunc_Inc
	rgoto	CS_MENU_Select_NameFunc_Del
	rgoto	CS_MENU_Select_NameFunc_Clr
 	rgoto	CS_MENU_Select_NameFunc_Ins	; (function if 6th button - if available)

CS_MENU_Select_NameFunc_Save
	;; exit page and store patch
	call	CS_MENU_EXEC_Hlp_Deselect
	goto	CS_MENU_EXEC_SavePatch_Final

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
	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	SID_Ix_NAME_x
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
	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	SID_Ix_NAME_x
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
	lfsr	FSR0, SID_EDIT_BUFFER
	movlw	SID_Ix_NAME_x
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
;;  special functions of sequence editing mode
;;  IN: function which should be executed in WREG (CS_MENU_PAGE_OFFSET has to be substracted!)
;; --------------------------------------------------------------------------
CS_MENU_Select_SeqFunc
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
	rgoto	CS_MENU_Select_SeqFunc_GAS
	rgoto	CS_MENU_Select_SeqFunc_Rot
	rgoto	CS_MENU_Select_SeqFunc_Rnd
	rgoto	CS_MENU_Select_SeqFunc_Clr
	rgoto	CS_MENU_Select_SeqFunc_DRM

CS_MENU_Select_SeqFunc_GAS
	;; toggle between four states (No Note, Gate, Accent, Secondary)
	call	CS_MENU_FSR0_WTPos
	movlw	2
	btfsc	CS_MENU_NAME_POS, 3
	addwf	FSR0L, F

	movf	CS_MENU_NAME_POS, W
	call	MIOS_HLP_GetBitORMask
	movwf	TMP1		; bit mask for step

	;; coding:
	;; Gate  Accent  Result
	;;    0       0  Don't play note
	;;    1       0  Play Note w/o accent
	;;    0       1  Play Note w/ accent
	;;    1       1  Play Secondary Note
	clrf	TMP2		; using TMP2 as temporary accumulator
	movf	POSTINC0, W	; extract G/A from track structure
	andwf	TMP1, W
	skpz
	bsf	TMP2, 0
	movf	POSTDEC0, W
	andwf	TMP1, W
	skpz
	bsf	TMP2, 1

	incf	TMP2, W			; increment G/A
	andlw	0x03
	movwf	TMP2


	comf	TMP1, W			; copy back to track structure
	andwf	INDF0, F
	BRA_IFCLR TMP2, 0, ACCESS, CS_MENU_Select_SeqFunc_GAS_NoG
CS_MENU_Select_SeqFunc_GAS_G
	movf	TMP1, W
	iorwf	INDF0, F
CS_MENU_Select_SeqFunc_GAS_NoG
	movf	INDF0, W
 	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)

	comf	TMP1, W
	andwf	INDF0, F
	BRA_IFCLR TMP2, 1, ACCESS, CS_MENU_Select_SeqFunc_GAS_NoA
CS_MENU_Select_SeqFunc_GAS_A
	movf	TMP1, W
	iorwf	INDF0, F
CS_MENU_Select_SeqFunc_GAS_NoA
	movf	INDF0, W
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)

	rgoto	CS_MENU_Select_SeqFunc_End


CS_MENU_Select_SeqFunc_Rot	; activate/deactivate rotate function
	btg	CS_STAT3, CS_STAT3_MODIFY_SEQ_ROTATE
	;; always reset cursor position for defined behaviour of rotary encoder
	clrf	CS_MENU_NAME_POS
	clrf	CS_MENU_PARAMETER_L
 	rgoto	CS_MENU_Select_SeqFunc_End

CS_MENU_Select_SeqFunc_Rnd	; random sequence
	call	CS_MENU_FSR0_WTPos

	;; gate flags
	call	SID_RND_GenRandomNumber
	movff	SID_RANDOM_SEED_L, WREG
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
	movff	SID_RANDOM_SEED_H, WREG
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)

	;; accent flags
	call	SID_RND_GenRandomNumber
	movff	SID_RANDOM_SEED_L, WREG
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
	movff	SID_RANDOM_SEED_H, WREG
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)

 	rgoto	CS_MENU_Select_SeqFunc_End

CS_MENU_Select_SeqFunc_Clr	; clear sequence
	call	CS_MENU_FSR0_WTPos
	movlw	0x00
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
	movlw	0x00
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
	movlw	0x00
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
	movlw	0x00
	rcall	CS_MENU_Select_SeqFunc_Hlp_Send	; (increments FSR0)
 	rgoto	CS_MENU_Select_SeqFunc_End

CS_MENU_Select_SeqFunc_DRM	; branch to drum page
	;; disable track editor
	bcf	CS_STAT3, CS_STAT3_MODIFY_SEQ
	;; select drum instrument depending on current track
	clrc
	rlf	CS_MENU_SELECTED_TRK, W
	movwf	CS_MENU_SELECTED_INS
	;; go to DRM page
	movlw	CS_MENU_D_DRM
	goto	CS_MENU_BUTTON_Hlp_MenuChange

CS_MENU_Select_SeqFunc_End
	return


	;; help function to send a parameter, and to increment FSR0 pointer
CS_MENU_Select_SeqFunc_Hlp_Send
	movwf	INDF0
	call	CS_MENU_MS_SendParameter
	movf	POSTINC0, W
	return
