; $Id$
;
; MBMF Menu for editing the MIDI events directly
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
CS_M_EVENT_Exit
	;; store current event and exit
	rgoto	CS_M_EVENT_StoreBuffer

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_EVENT_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment cursor position, wrap at 3 or 4, depending on button/fader
	SET_BSR	MBMF_BASE
	incf	CS_MENU_CURSOR_POS, F
	movlw	0x03
	btfsc	MBMF_CURRENT_ELEMENT, 7, BANKED
	movlw 0x05
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	clrf CS_MENU_CURSOR_POS

	;; update menu parameter
	rgoto	CS_M_EVENT_Update_MenuParam

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_EVENT_Init
	;; set cursor to first position
	clrf	CS_MENU_CURSOR_POS

	;; invalidate current element, so that buffer will be updated with next tick
	SET_BSR	MBMF_BASE
	setf	MBMF_CURRENT_ELEMENT
	;; (will also set the CS_MENU_PARAMETER_[LH] and CS_MENU_PARAMETER_MAX_[LH])

	;; install "changed" hook
	TABLE_ADDR CS_M_EVENT_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; init special characters for vertical bars
	goto	MBMF_LCD_InitVBars

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_EVENT_ValueChanged
	;; copy menu value to MBMF_LEARN_EVNT[012] or MBMF_LEARN_BMODE
	SET_BSR	MBMF_BASE
	movf	MBMF_CURRENT_ELEMENT, W, BANKED
	andlw	0x7f
	movwf	MBMF_CURRENT_FADER, BANKED
	movwf	MBMF_CURRENT_BUTTON, BANKED

	movf	CS_MENU_CURSOR_POS, W
	JUMPTABLE_2BYTES 5	; entries
	rgoto	CS_M_EVENT_ValueChanged_Stat
	rgoto	CS_M_EVENT_ValueChanged_Chn
	rgoto	CS_M_EVENT_ValueChanged_2nd
	rgoto	CS_M_EVENT_ValueChanged_3rd
	rgoto	CS_M_EVENT_ValueChanged_BMode

CS_M_EVENT_ValueChanged_Stat
	movlw	0x0f
	andwf	MBMF_LEARN_EVNT0, F, BANKED
	swapf	CS_MENU_PARAMETER_L, W
	andlw	0x70
	iorlw	0x80
	rgoto	CS_M_EVENT_ValueChanged_StatC

CS_M_EVENT_ValueChanged_Chn
	movlw	0xf0
	andwf	MBMF_LEARN_EVNT0, F, BANKED
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x0f

CS_M_EVENT_ValueChanged_StatC
	iorwf	MBMF_LEARN_EVNT0, F, BANKED
	CALL_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MBMF_FSR_ButtonEvnt0
	CALL_IFCLR MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MBMF_FSR_FaderEvnt0
	movff	MBMF_LEARN_EVNT0, INDF0
	return

CS_M_EVENT_ValueChanged_2nd
	movff	CS_MENU_PARAMETER_L, MBMF_LEARN_EVNT1
	CALL_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MBMF_FSR_ButtonEvnt1
	CALL_IFCLR MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, MBMF_FSR_FaderEvnt1
	movff	MBMF_LEARN_EVNT1, INDF0
	return

CS_M_EVENT_ValueChanged_3rd
	movff	CS_MENU_PARAMETER_L, MBMF_LEARN_EVNT2
	return

CS_M_EVENT_ValueChanged_BMode
	movff	CS_MENU_PARAMETER_L, MBMF_LEARN_BMODE
	return


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_EVENT_STR_P_0 STRING 13, 0x00, "Edit Fader    #"
CS_M_EVENT_STR_B_0 STRING 13, 0x00, "Edit Button #"

CS_M_EVENT_Tick
	;; if CLCD: disable blinking CLCD cursor
	movlw	0x0c
	CALL_IFCLR CS_STAT, CS_STAT_DISPLAY_GLCD, ACCESS, MIOS_LCD_Cmd

	;; if fader or button has been changed, update buffer
	rcall	CS_M_EVENT_GetNewElement
	;; compare with selected element
	xorwf	MBMF_CURRENT_ELEMENT, W, BANKED
	bz	CS_M_EVENT_Tick_NoChange
	;; if not equal:
	rcall	CS_M_EVENT_StoreBuffer	; store current buffer
	rcall	CS_M_EVENT_GetNewElement
	movff	WREG, MBMF_CURRENT_ELEMENT
	rcall	CS_M_EVENT_UpdateBuffer	; copy new event into buffer
	clrf	CS_MENU_CURSOR_POS	; reset cursor

	;; print new header line
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_EVENT_Tick_ButtonPrint
CS_M_EVENT_Tick_FaderPrint
	TABLE_ADDR CS_M_EVENT_STR_P_0
	rgoto	CS_M_EVENT_Tick_PrintCont
CS_M_EVENT_Tick_ButtonPrint
	TABLE_ADDR CS_M_EVENT_STR_B_0
	;; 	rgoto	CS_M_EVENT_Tick_PrintCont
CS_M_EVENT_Tick_PrintCont
	call	MIOS_LCD_PrintString
	movff	MBMF_CURRENT_ELEMENT, WREG
	andlw	0x7f
	addlw	1
	call	MIOS_LCD_PrintBCD3

CS_M_EVENT_Tick_NoChange

	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_EVENT_Tick_NoFaderValUpdate
	SET_BSR	MBMF_BASE
	movf	MBMF_LAST_FADER0, W, BANKED
	call	MBMF_FADER_Map
	movwf	MBMF_CURRENT_FADER, BANKED
	call	MBMF_FSR_FaderValue
	movf	INDF0, W
	andlw	0x7f
	movff	WREG, MBMF_LEARN_EVNT2
CS_M_EVENT_Tick_NoFaderValUpdate

	;; print learn event at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	call	MBMF_LCD_PrintEvntStd_Learn

	;; branch to cursor handler
	rgoto	CS_M_EVENT_CursorHandler


;; ==========================================================================
	;; initializes the event buffer (we are using MBMF_LEARN_EVNT[012] as temporary buffer)
CS_M_EVENT_UpdateBuffer
	SET_BSR	MBMF_BASE
	movf	MBMF_CURRENT_ELEMENT, W, BANKED
	andlw	0x7f
	BRA_IFSET MBMF_CURRENT_ELEMENT, 7, BANKED, CS_M_EVENT_UpdateBuffer_B
CS_M_EVENT_UpdateBuffer_P
	movwf	MBMF_CURRENT_FADER, BANKED
	;; get address to fader event
	call	MBMF_ADDR_FaderEntry
	;; save it in the learn buffer
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_EVNT0
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_EVNT1
#if 0
	;; fader value will also be updated later...
	call	MBMF_FSR_FaderValue
	movf	INDF0, W
	andlw	0x7f
	movff	WREG, MBMF_LEARN_EVNT2
#endif
	;; update menu parameter
	rgoto	CS_M_EVENT_Update_MenuParam

CS_M_EVENT_UpdateBuffer_B
	movwf	MBMF_CURRENT_BUTTON, BANKED
	;; get address to button event
	call	MBMF_ADDR_ButtonEntry
	;; save it in the learn buffer
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_EVNT0
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_EVNT1
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_EVNT2
	call	MBMF_BANK_Read
	movff	WREG, MBMF_LEARN_BMODE
	;; update menu parameter
	rgoto	CS_M_EVENT_Update_MenuParam


;; ==========================================================================
	;; stores the current buffer in Bank
CS_M_EVENT_StoreBuffer
	;; call the store routine
	;; parameters:
;;  IN: fader or button number in WREG, 
;;      event in MBMF_LEARN_EVNT[012]
;;      button mode (on/off, on only or toggle) in MBMF_LEARN_BMODE
;;      learn mode (single/row/add) in TMP5[1..0]
;;      TMP5[7] == 0: store fader event
;;      TMP5[7] == 1: store button event
	;; events already in registers
	;; button mode already in register
	;; learn mode: force to 0x00
	;; select button/fader:
	SET_BSR	MBMF_BASE
	clrf	TMP5
	btfsc	MBMF_CURRENT_ELEMENT, 7, BANKED
	bsf	TMP5, 7
	movf	MBMF_CURRENT_ELEMENT, W, BANKED
	call	MBMF_MEM_StoreEvent

	;; restore fader/button setup and exit
	goto	MBMF_DUMP_RestoreFadButtonEvnts
	
;; ==========================================================================

	;; updates the menu parameter depending on menu position
CS_M_EVENT_Update_MenuParam
	;; set max value
	SET_BSR	MBMF_BASE
	clrf	CS_MENU_PARAMETER_MAX_H
	SET_BSR	MBMF_BASE
	movf	CS_MENU_CURSOR_POS, W
	addlw	CS_M_EVENT_MAX_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	(CS_M_EVENT_MAX_TABLE >> 8) & 0xff
	addwfc	TBLPTRH, F
	tblrd*+
	movff	TABLAT, CS_MENU_PARAMETER_MAX_L

	;; copy MBMF_LEARN_EVNT[012] or MBMF_LEARN_BMODE to menu value
	clrf	CS_MENU_PARAMETER_H
	movf	CS_MENU_CURSOR_POS, W
	JUMPTABLE_2BYTES 5	; entries
	rgoto	CS_M_EVENT_Update_MP_Stat
	rgoto	CS_M_EVENT_Update_MP_Chn
	rgoto	CS_M_EVENT_Update_MP_2nd
	rgoto	CS_M_EVENT_Update_MP_3rd
	rgoto	CS_M_EVENT_Update_MP_BMode

CS_M_EVENT_Update_MP_Stat
	swapf	MBMF_LEARN_EVNT0, W, BANKED
	andlw	0x07
	movwf	CS_MENU_PARAMETER_L
	return

CS_M_EVENT_Update_MP_Chn
	movf	MBMF_LEARN_EVNT0, W, BANKED
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	return

CS_M_EVENT_Update_MP_2nd
	movff	MBMF_LEARN_EVNT1, CS_MENU_PARAMETER_L
	return

CS_M_EVENT_Update_MP_3rd
	movff	MBMF_LEARN_EVNT2, CS_MENU_PARAMETER_L
	return

CS_M_EVENT_Update_MP_BMode
	movff	MBMF_LEARN_BMODE, CS_MENU_PARAMETER_L
	return


	;; contains the max values depending on cursor position
CS_M_EVENT_MAX_TABLE
	db	0x07, 0x0f, 0x7f, 0x7f, 0x02

;; ==========================================================================
	;; returns the new element depending on MB_STAT_LAST_WAS_BUTTON and MBMF_LAST_*0
CS_M_EVENT_GetNewElement
	;;    if last event was triggered by fader: fader number
	;;    if last event was triggered by button: 0x80 | button number
	SET_BSR	MBMF_BASE
	BRA_IFSET MB_STAT, MB_STAT_LAST_WAS_BUTTON, ACCESS, CS_M_EVENT_GetNewElement_Button
CS_M_EVENT_GetNewElement_Fader
	movf	MBMF_LAST_FADER0, W, BANKED
	goto	MBMF_FADER_Map

CS_M_EVENT_GetNewElement_Button
	movf	MBMF_LAST_BUTTON0, W, BANKED
	iorlw	0x80
	return

;; ==========================================================================
	;; set cursor depending on position and MIDI event
CS_M_EVENT_CursorHandler
	SET_BSR	MBMF_BASE
	swapf	MBMF_LEARN_EVNT0, W, BANKED
	andlw	0x07
	mullw	10
	rlf	CS_MENU_CURSOR_POS, W
	andlw	0xfe
	addwf	PRODL, W
	addlw	CS_M_EVENT_CURSOR_TABLE & 0xff
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	(CS_M_EVENT_CURSOR_TABLE >> 8) & 0xff
	addwfc	TBLPTRH, F

	;; read cursor pos and save it in TMP1
	tblrd*+
	movff	TABLAT, TMP1
	;; read number of spaces and save it in TMP2
	tblrd*+
	movff	TABLAT, TMP2

	;; expects:
	;;  IN: cursor pos in TMP1
	;;      number of spaces (cursor width) in TMP2
	goto	CS_MENU_CursorHandler

	;; contains the display positions and the cursor length for every MIDI event
CS_M_EVENT_CURSOR_TABLE
	;; Note Off Event
	;;      POS   LEN
	db	0x43, 0x04	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x48, 0x03	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Note On Event
	;;      POS   LEN
	db	0x43, 0x04	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x48, 0x03	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Aftertouch Event
	;;      POS   LEN
	db	0x43, 0x04	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x48, 0x03	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Controller Event
	;;      POS   LEN
	db	0x43, 0x04	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x47, 0x03	; CS_MENU_CURSOR_POS == 2
	db	0x4b, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Program Change Event
	;;      POS   LEN
	db	0x43, 0x09	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x50, 0x01	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Channel Pressure Event
	;;      POS   LEN
	db	0x43, 0x09	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x50, 0x01	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Pitch Bender Event
	;;      POS   LEN
	db	0x43, 0x09	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x50, 0x01	; CS_MENU_CURSOR_POS == 2
	db	0x4c, 0x03	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
	;; Meta Event
	;;      POS   LEN
	db	0x43, 0x08	; CS_MENU_CURSOR_POS == 0
	db	0x40, 0x02	; CS_MENU_CURSOR_POS == 1
	db	0x4b, 0x02	; CS_MENU_CURSOR_POS == 2
	db	0x4d, 0x02	; CS_MENU_CURSOR_POS == 3
	db	0x4f, 0x01	; CS_MENU_CURSOR_POS == 4
	
;; ==========================================================================
