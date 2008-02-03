; $Id$
;
; MBMF Menu Display Handler
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
	call	MBMF_LCD_SetDefaultOffsets
	;; save page number and exit
	goto	MBMF_DUMP_StoreGlobal

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Snapshot
	goto	MBMF_BUTTON_Snapshot

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; set max number of display pages
	movlw	2
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H
	;; get pointer to MBMF_DISPLAY_PAGE and copy value
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
	goto	MBMF_LCD_InitVBars


;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_ValueChanged
	;; copy menu value to MBMF_DISPLAY_PAGE
	rcall	CS_M_DISPLAY_Hlp_GetFSR
	movff	CS_MENU_PARAMETER_L, INDF0

	;; set default offsets for 4 LCD lines
	call	MBMF_LCD_SetDefaultOffsets

#if 0
	call	MIOS_LCD_Clear
#endif
	return


;; ==========================================================================
	;; returns pointer to MBMF_DISPLAY_PAGE in FSR0
CS_M_DISPLAY_Hlp_GetFSR
	lfsr	FSR0, MBMF_DISPLAY_PAGE
	return
	
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick
	;; branch depending on selected page
	SET_BSR	MBMF_BASE
	movf	MBMF_DISPLAY_PAGE, W, BANKED
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

	;; print bank information and fader mode (8 characters)
	call	MBMF_LCD_PrintBank

	;; print 8 bars of the current fader or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn1
CS_M_DISPLAY_Page0_Fader1
	movff	MBMF_LAST_FADER0, WREG
	call	MBMF_FADER_Map	; special MBMF feature: map fader to parameter entry
	call	MBMF_LCD_8FaderBars
	rgoto	CS_M_DISPLAY_Page0_2ndline
CS_M_DISPLAY_Page0_Btn1
	movff	MBMF_LAST_BUTTON0, WREG
	call	MBMF_LCD_8ButtonBars
	;; 	rgoto	CS_M_DISPLAY_Page0_2ndline

CS_M_DISPLAY_Page0_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of current fader or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page0_Btn2
CS_M_DISPLAY_Page0_Fader2
	movff	MBMF_LAST_FADER0, WREG
	call	MBMF_FADER_Map	; special MBMF feature: map fader to parameter entry
	goto	MBMF_LCD_PrintLabel
CS_M_DISPLAY_Page0_Btn2
	movff	MBMF_LAST_BUTTON0, WREG
	goto	MBMF_LCD_PrintEvntStd_Btn

;; --------------------------------------------------------------------------
;;  --> Display Page 1
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page1
	;; print fader status at first line
CS_M_DISPLAY_Page1_1stline
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movff	MBMF_LAST_FADER0, WREG
	call	MBMF_LCD_16FaderBars

	;; print button status at second line
CS_M_DISPLAY_Page1_2ndline
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movff	MBMF_LAST_BUTTON0, WREG
	goto	MBMF_LCD_16ButtonBars

;; --------------------------------------------------------------------------
;;  --> Display Page 2
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Page2
CS_M_DISPLAY_Page2_1stline
	;; set cursor to first line, first column
	movlw	0x00
	call	MIOS_LCD_CursorSet

	;; print bank information and fader mode (8 characters)
	call	MBMF_LCD_PrintBank

	;; print short information about the last sent MIDI event (8 characters)
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn1
CS_M_DISPLAY_Page2_Fader1
	movff	MBMF_LAST_FADER0, WREG
	call	MBMF_FADER_Map	; special MBMF feature: map fader to parameter entry
	call	MBMF_LCD_PrintShortEvntStd_Fader
	rgoto	CS_M_DISPLAY_Page2_2ndline
CS_M_DISPLAY_Page2_Btn1
	movff	MBMF_LAST_BUTTON0, WREG
	call	MBMF_LCD_PrintShortEvntStd_Btn

CS_M_DISPLAY_Page2_2ndline
	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print status of fader or button row
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_DISPLAY_Page2_Btn2
CS_M_DISPLAY_Page2_Fader2
	movff	MBMF_LAST_FADER0, WREG
	goto	MBMF_LCD_16FaderBars
CS_M_DISPLAY_Page2_Btn2
	movff	MBMF_LAST_BUTTON0, WREG
	goto	MBMF_LCD_16ButtonBars


