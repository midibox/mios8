; $Id$
;
; SID Control Surface Menu Execute Functions
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


; ==========================================================================
; ==========================================================================
;  refered by cs_menu_tables.inc
; ==========================================================================
; ==========================================================================

; ==========================================================================
;  All supported EXEC_* indices
;; (max allowed number: 0x0f!)
; ==========================================================================
EXEC_NOP	EQU	0x00
EXEC_MENU	EQU	0x01
EXEC_MENU_WT	EQU	0x02
EXEC_MENU_303	EQU	0x03
EXEC_MENU_SAVE	EQU	0x04
EXEC_SELPAR	EQU	0x05
EXEC_TOGPAR	EQU	0x06
EXEC_EDITNAME	EQU	0x07
EXEC_SAVE_IT	EQU	0x08


; ==========================================================================
;  The EXEC handler
;; IN: EXEC_xxx ID in WREG
; ==========================================================================
CS_MENU_EXEC_Handler
	JUMPTABLE_2BYTES (0x08+1)	; entries
	rgoto	CS_MENU_EXEC_NOP
	rgoto	CS_MENU_EXEC_Menu
	rgoto	CS_MENU_EXEC_Menu_WT
	rgoto	CS_MENU_EXEC_Menu_303
	rgoto	CS_MENU_EXEC_Menu_Save
	rgoto	CS_MENU_EXEC_SelPar
	rgoto	CS_MENU_EXEC_TogPar
	rgoto	CS_MENU_EXEC_EditName
	rgoto	CS_MENU_EXEC_SaveIt


; ==========================================================================
;  Dummy function, can be called if nothing has to be done
; ==========================================================================
CS_MENU_EXEC_NOP
	return

; ==========================================================================
;  Use this function to change to a menu
; ==========================================================================
CS_MENU_EXEC_Menu
	movf	FSR0L, W
	andlw	0x7f
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Dummy function, can be called if nothing has to be done
; ==========================================================================
CS_MENU_EXEC_Nothing
	return

; ==========================================================================
;  Use this function to change to the WT page
; ==========================================================================
CS_MENU_EXEC_Menu_WT
	movf	FSR0L, W
	andlw	0x7f
	movwf	CS_MENU_PARAMETER_L	; used as tmp. storage

	;; update WT buffer
	clrf	CS_MENU_WT_POS
	call	CS_MENU_MS_Update_WT_Buffer

	movf	CS_MENU_PARAMETER_L, W	; restore menu number from CS_MENU_PARAMETER_L
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  Use this function to change to the 303 page
; ==========================================================================
CS_MENU_EXEC_Menu_303
	;; print message instead of changing if 303 mode not enabled
	SET_BSR	SID_BASE
	BRA_IFSET SID_SE_OPTION, SE_OPTION_TB303, BANKED, CS_MENU_EXEC_Menu_WT

	TABLE_ADDR CS_MENU_EXEC_MENU_303DIS_MSG
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage

CS_MENU_EXEC_MENU_303DIS_MSG
	STRING 20, 0x00, "TB303 Mode disabled "
	STRING 20, 0x40, "go to SEO menu page "
	

; ==========================================================================
;  Use this function to change to the save patch menu
; ==========================================================================
CS_MENU_EXEC_Menu_Save
	;; copy current patch/bank number to SAVE_PATCH and SAVE_BANK and change menu
	movff	SID_PATCH, CS_MENU_SAVE_PATCH
	movff	SID_BANK, CS_MENU_SAVE_BANK
	movlw	CS_MENU_SAV
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu


; ==========================================================================
;  Use this function to select/deselect a parameter
;  (depending on CS_STAT.CS_STAT_MODIFY_PARAMETER)
; ==========================================================================
CS_MENU_EXEC_SelPar
	;; don't select if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return

	;; set the "modify parameter" flag
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; request a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ	; (see cs_menu.inc)

	;; copy new max value to CS_MENU_PARAMETER_MAX_[LH]
	rcall	CS_MENU_Hlp_GetCursorPosEntry
	movlw	CS_MENU_ENTRY_MAX_OFFSET
	rcall	CS_MENU_Hlp_GetPointer

	;; read and copy
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	;; new max value: set encoder speed
	rcall	CS_MENU_EncSpeedSet

	;; update parameter value
	goto	CS_MENU_EncParameterSet


; ==========================================================================
;  Use this function to select/deselect a parameter and to toggle it
;  (usefull for on/off switches)
; ==========================================================================
CS_MENU_EXEC_TogPar
	;; don't toggle if no menu item
	movf	CS_MENU_ENTRIES, W
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	return
	
	;; select parameter
	rcall	CS_MENU_EXEC_SelPar

	;; toggle it
	btg	CS_MENU_PARAMETER_L, 0

	;; transfer to register
	rgoto	CS_MENU_ParameterUpdate

; ==========================================================================
;  Enter the "edit name" function
; ==========================================================================
CS_MENU_EXEC_EditName
	;; clear name position
	clrf	CS_MENU_NAME_POS
	;; enable "edit name" mode
	bsf	CS_STAT, CS_STAT_MODIFY_NAME
	;; enter parameter mode
	goto	CS_MENU_EXEC_SelPar

; ==========================================================================
;  Use this function to save the current patch to CS_MENU_SAVE_PATCH
; ==========================================================================
CS_MENU_EXEC_SaveIt
	;; print "save patch" at the second line
	TABLE_ADDR TEXT_SAVE_PATCH
	call	MIOS_LCD_PrintString
	;; fill with spaces
	movlw	40-16
	rcall	CS_MENU_Hlp_PrintSpaces
	;; save patch
	rcall	CS_MENU_SavePatch
	;; reset menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	rgoto	CS_MENU_EXEC_Menu_Save
	
TEXT_SAVE_PATCH STRING 16, 0x40, "-- Save Patch --"

; ==========================================================================
;  This help function is used to change to another menu if
;  "parameter modify" mode not active
;  if currently a parameter is modified, change to parameter select
;  mode instead
;  IN: new menu ID in WREG
; ==========================================================================
CS_MENU_EXEC_Hlp_ChangeMenu
	BRA_IFCLR CS_STAT, CS_STAT_MODIFY_PARAMETER, ACCESS, CS_MENU_EXEC_Hlp_ChangeMenu_Now

	;; we are in "modify parameter" mode, clear flag and update screen
	;; the user has to press the exec button once more to leave the menu
	bcf	CS_STAT, CS_STAT_MODIFY_PARAMETER
	bcf	CS_STAT, CS_STAT_MODIFY_NAME
	;; request update of display
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; clear counter so that cs_menu_timer.inc counts from zero and the menu entry is marked for a short time
	clrf	CS_CURSOR_CTR
	;; clear "CS_STAT_CURSOR_FLASH" bit (see cs_menu.inc for the handling)
	bcf	CS_STAT, CS_STAT_CURSOR_FLASH
	;; set encoder speed
	rcall	CS_MENU_EncSpeedSet
	;; update parameter
	rcall	CS_MENU_EncParameterSet
	;; update parameter max value and exit
	rgoto	CS_MENU_EncMaxSet

CS_MENU_EXEC_Hlp_ChangeMenu_Now
	movwf	CS_MENU
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; set encoder speed and exit
	rgoto	CS_MENU_EncSpeedSet


; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the main page
; ==========================================================================
CS_MENU_EXEC_GoToMain
	movlw	CS_MENU_MAIN
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  directly called from cs_menu_tables.inc
;  Use this function to change to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRoot
	movlw	CS_MENU_ROOT
	rgoto	CS_MENU_EXEC_Hlp_ChangeMenu

; ==========================================================================
;  directly called from cs_menu_tables.inc
;  This function first writes to EEPROM before it changes to the root menu
; ==========================================================================
CS_MENU_EXEC_GoToRootWT
	;; force write to EEPROM
	call	CS_MENU_MS_Update_WT_EEPROM
	rgoto	CS_MENU_EXEC_GoToRoot

