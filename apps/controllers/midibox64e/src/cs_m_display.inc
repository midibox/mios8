; $Id$
;
; MB64E Menu Display Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exec button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Exit
	;; set the default LCD offsets
	call	MB64E_LCD_SetDefaultOffsets
	;; save page number and exit
	goto	MB64E_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Snapshot
	goto	MB64E_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; set max number of display pages
	movlw	2
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H
	;; get pointer to MB64E_DISPLAY_PAGE and copy value
	;; to CS_MENU_PARAMETER_[LH]
	rcall	CS_M_DISPLAY_Hlp_GetFSR
	movf	INDF0, W
	andlw	0x03
	movwf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H
	;; install hook which notifies value changes
	TABLE_ADDR CS_M_DISPLAY_ValueChanged
	call	CS_MENU_SetMenuParameterHook
	;; exception: since this function does some additional display
	;; initialization, we call it also from here to ensure that the
	;; screen has been initialized properly
	call	CS_M_DISPLAY_ValueChanged
	;; init special characters for vertical bars
	goto	MB64E_LCD_InitVBars


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_ValueChanged
	;; copy menu value to MB64E_DISPLAY_PAGE
	rcall	CS_M_DISPLAY_Hlp_GetFSR
	movff	CS_MENU_PARAMETER_L, INDF0

	;; set default offsets for 4 LCD lines
	call	MB64E_LCD_SetDefaultOffsets

	call	MIOS_LCD_Clear

	return


;; ==========================================================================
	;; returns pointer to MB64E_DISPLAY_PAGE in FSR0
CS_M_DISPLAY_Hlp_GetFSR
	lfsr	FSR0, MB64E_DISPLAY_PAGE
	return
	
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick
	;; branch depending on selected page
	SET_BSR	MB64E_BASE
	movf	MB64E_DISPLAY_PAGE, W, BANKED
	JUMPTABLE_2BYTES 3	; 3 entries
	rgoto	CS_M_DISPLAY_Page0
	rgoto	CS_M_DISPLAY_Page1
	rgoto	CS_M_DISPLAY_Page2


;; --------------------------------------------------------------------------
;;  --> Display Page 0
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page0
CS_M_DISPLAY_Page0_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank information and enc mode (8 characters)
	call	MB64E_LCD_PrintBank

	;; print 8 bars of the current enc or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn1
CS_M_DISPLAY_Page0_Enc1
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	call	MB64E_LCD_8EncBars
	rgoto	CS_M_DISPLAY_Page0_2ndline
CS_M_DISPLAY_Page0_Btn1
	movff	MB64E_LAST_BUTTON, WREG
	call	MB64E_LCD_8ButtonBars
	;; 	rgoto	CS_M_DISPLAY_Page0_2ndline

CS_M_DISPLAY_Page0_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of current enc or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn2
CS_M_DISPLAY_Page0_Enc2
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	goto	MB64E_LCD_PrintLabel
CS_M_DISPLAY_Page0_Btn2
	movff	MB64E_LAST_BUTTON, WREG
	goto	MB64E_LCD_PrintEvntStd_Btn

;; --------------------------------------------------------------------------
;;  --> Display Page 1
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page1
	;; print enc status at first line
CS_M_DISPLAY_Page1_1stline
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	call	MB64E_LCD_16EncBars

	;; print button status at second line
CS_M_DISPLAY_Page1_2ndline
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	MB64E_LAST_BUTTON, WREG
	goto	MB64E_LCD_16ButtonBars

;; --------------------------------------------------------------------------
;;  --> Display Page 2
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page2
CS_M_DISPLAY_Page2_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank information and enc mode (8 characters)
	call	MB64E_LCD_PrintBank

	;; print short information about the last sent MIDI event (8 characters)
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn1
CS_M_DISPLAY_Page2_Enc1
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	call	MB64E_LCD_PrintShortEvntStd_Enc
	rgoto	CS_M_DISPLAY_Page2_2ndline
CS_M_DISPLAY_Page2_Btn1
	movff	MB64E_LAST_BUTTON, WREG
	call	MB64E_LCD_PrintShortEvntStd_Btn

CS_M_DISPLAY_Page2_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of enc or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn2
CS_M_DISPLAY_Page2_Enc2
	movff	MB64E_LAST_ENTRY_MAPPED, WREG
	goto	MB64E_LCD_16EncBars
CS_M_DISPLAY_Page2_Btn2
	movff	MB64E_LAST_BUTTON, WREG
	goto	MB64E_LCD_16ButtonBars


