; $Id$
;
; MIDIbox FM Control Surface Button Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This handler has to be called by USER_DIN_NotifyToggle when a button
;;  has been pressed/depressed
;;  It goes through the CS_MENU_DIN_TABLE in "cs_menu_io_tables.inc",
;;  searches for a matching DIN pin and calls the appr. function
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Handler
	;; stop message if button has been pressed
	CALL_IFCLR MIOS_PARAMETER2, 0, ACCESS, MIOS_LCD_MessageStop

	;; go through the DIN map
	TABLE_ADDR CS_MENU_DIN_TABLE
CS_MENU_BUTTON_Handler_Loop
	tblrd*+				; get low-byte of function pointer
	movf	TABLAT, W		; load it in WREG
	movwf	TMP1			; and store it in TMP1
	tblrd*+				; get high byte
	movff	TABLAT, TMP2		; store it in TMP2 without touching WREG
	iorwf	TABLAT, W		; or the byte with WREG --- if both bytes are zero, the result will be zero
	bz	CS_MENU_BUTTON_Handler_End; in this case jump to the end

	tblrd*+				; get pin-number
	movf	TABLAT, W
	tblrd*+				; and read dummy entry
	;; pin number still in WREG, compare it with MIOS_PARAMETER1
	xorwf	MIOS_PARAMETER1, W
	bnz	CS_MENU_BUTTON_Handler_Loop; next loop if not equal

	;; else: matching pin has been found
	;; jump indirectly to the function (pointer in TMP1/TMP2)
	IRQ_DISABLE		; (IRQs have to be disabled when TOSL/TOSH is modified)
	push
	movf	TMP1, W
	movwf	TOSL
	movf	TMP2, W
	movwf	TOSH
	IRQ_ENABLE
	return			; this return leads to a jump to the function stored in TOSL/TOSH

CS_MENU_BUTTON_Handler_End

	;; the selection matrix buttons are handled separately
	goto	CS_MENU_MATRIX_BHandler


;; --------------------------------------------------------------------------
;; the button functions are defined here
;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Dec
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; decrement 1
	movlw	-1
#if CS_SAMMICH_FM
	;; branch if mainpage is displayed
	GOTO_IFSET CS_MENU, 7, ACCESS, CS_MENU_BUTTON_Dec_End
	;; if we're editing a parameter...
	btfsc	CS_STAT, CS_STAT_MODIFY_PARAMETER
	;; ...stop doing that :)
	call	CS_MENU_Exec

	;; set SOURCE_BUTTON flag, so we can determine that it's a 
	;; button triggering this event
	bsf	SAMMICH_FLAGS, SOURCE_BUTTON

	;; decrement to the next value where x mod 4 = 0
	;; (not in drum menu)
	movf	CS_MENU, W
	xorlw	CS_MENU_DRUM
	bnz	CS_MENU_BUTTON_Dec_NotDrumPage
	movlw	-1
	rgoto	CS_MENU_BUTTON_Dec_End

CS_MENU_BUTTON_Dec_NotDrumPage
	decf 	CS_MENU_PAGE_OFFSET, W
	btfsc	W, 7
	movlw   0
	andlw	0x03
	sublw	0xff
CS_MENU_BUTTON_Dec_End
#endif
	;; branch to ENC handler
	rgoto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Inc
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; enable auto repeat (should normaly be done in the _init.inc file,
	;; but we want to handle it more flexible
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	;; increment 1
	movlw	0x01
#if CS_SAMMICH_FM
	;; branch if mainpage is displayed
	GOTO_IFSET CS_MENU, 7, ACCESS, CS_MENU_BUTTON_Inc_End

	;; if we're editing a parameter...
	btfsc	CS_STAT, CS_STAT_MODIFY_PARAMETER
	;; ...stop doing that :)
	call	CS_MENU_Exec
	
	;; set SOURCE_BUTTON flag, so we can determine that it's a 
	;; button triggering this event
	bsf	SAMMICH_FLAGS, SOURCE_BUTTON
	
	;; prepare increment and so on
	bsf	MIOS_PARAMETER1, 0

	;; set incrementer to next page mod 4
	;; (not in drum menu)
	movf	CS_MENU, W
	xorlw	CS_MENU_DRUM
	bnz	CS_MENU_BUTTON_Inc_NotDrumPage
	movlw	0x01
	rgoto	CS_MENU_BUTTON_Inc_End

CS_MENU_BUTTON_Inc_NotDrumPage
	movf 	CS_MENU_PAGE_OFFSET, W
	andlw	0x03
	sublw	4
CS_MENU_BUTTON_Inc_End
#endif
	;; branch to ENC handler
	rgoto	CS_MENU_ENC_Handler

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Esc
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	goto	CS_MENU_Exec

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Sel8
	;; select button #8, set cursor to 8th position
	movlw	0x07
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel7
	;; select button #7, set cursor to 7th position
	movlw	0x06
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel6
	;; select button #6, set cursor to 6th position
	movlw	0x05
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel5
	;; select button #5, set cursor to 5th position
	movlw	0x04
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel4
	;; select button #4, set cursor to 4th position
	movlw	0x03
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel3
	;; select button #3, set cursor to 3rd position
	movlw	0x02
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel2
	;; select button #2, set cursor to 2nd position
	movlw	0x01
	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Sel1
	;; select button #1, set cursor to 1st position
	movlw	0x00
	;; 	rgoto	CS_MENU_BUTTON_Select_Cont

CS_MENU_BUTTON_Select_Cont
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch if in CFG mode
	GOTO_IFSET CS_MENU_MODE, CS_MENU_MODE_CFG, ACCESS, CS_MENU_CFG_Button

	;; set cursor to: CS_MENU_PAGE_OFFSET + number in WREG
	addwf	CS_MENU_PAGE_OFFSET, W

	;; branch to the CS_MENU_Select function
	goto	CS_MENU_Select

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Cfg
	;; ignore the CFG button while in name editing mode
	;; it leads to odd behaviour
	btfsc CS_STAT, CS_STAT_MODIFY_NAME
	return

	;; switch on/off the CFG page
	bcf	CS_MENU_MODE, CS_MENU_MODE_CFG
	btfss	MIOS_PARAMETER2, 0
	bsf	CS_MENU_MODE, CS_MENU_MODE_CFG

	;; re-init screen
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_InsOP1
	movlw	0
	rgoto	CS_MENU_BUTTON_InsOP_Cont
	
CS_MENU_BUTTON_InsOP2
	movlw	1
	rgoto	CS_MENU_BUTTON_InsOP_Cont
	
CS_MENU_BUTTON_InsOP3
	movlw	2
	rgoto	CS_MENU_BUTTON_InsOP_Cont
	
CS_MENU_BUTTON_InsOP4
	movlw	3
	;; 	rgoto	CS_MENU_BUTTON_InsOP_Cont

CS_MENU_BUTTON_InsOP_Cont
	;; if CFG button pressed: play/unplay Instrument
	GOTO_IFSET CS_MENU_MODE, CS_MENU_MODE_CFG, ACCESS, CS_MENU_BUTTON_Ins_Play

	;; store number in TMP1
	movwf	TMP1

	;; pointer to _FLAGS register which should be modified in FSR0
	lfsr	FSR0, CS_MENU_SELECTED_INS_FLAGS
	btfsc	CS_MENU_MODE, CS_MENU_MODE_INS_OP
	lfsr	FSR0, CS_MENU_SELECTED_OP_FLAGS

	;; only save button state if button has been depressed and exit
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_InsOP_Hlp_Sav

	;; else branch depending on state of all 4 Instrument/OP buttons:
	;; if only one OP button pressed: radio-button function (1 of 4)
	;; if more than one OP button pressed: toggle function (4 of 4)
	movf	INDF0, W	; CS_MENU_SELECTED_x_FLAGS
	andlw	0xf0
	xorlw	0xf0
	bz	CS_MENU_BUTTON_InsOP_Radio
CS_MENU_BUTTON_InsOP_Select
	;; toggle InsOP flag
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	xorwf	INDF0, F	; CS_MENU_SELECTED_x_FLAGS
 	rgoto	CS_MENU_BUTTON_InsOP_End
CS_MENU_BUTTON_InsOP_Radio
	;; select 1 of 4
	movlw	0xf0
	andwf	INDF0, F	; CS_MENU_SELECTED_x_FLAGS
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF0, F	; CS_MENU_SELECTED_x_FLAGS
	;; 	rgoto	CS_MENU_BUTTON_InsOP_End

CS_MENU_BUTTON_InsOP_End
	;; save button state
	rcall	CS_MENU_BUTTON_InsOP_Hlp_Sav
	;; request new display-initialization in mainpage (required to display the new patchname)
	btfsc	CS_MENU, 7
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; update parameter
	call	CS_MENU_EncParameterSet
	;; and request display update
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq


CS_MENU_BUTTON_InsOP_Hlp_Sav
	;; mask out old button status
	movf	TMP1, W
	call	MIOS_HLP_GetBitANDMask
	swapf	WREG, W
	andwf	INDF0, F	; CS_MENU_SELECTED_x_FLAGS

	;; store new button status (pressed/depressed) in CS_MENU_SELECTED_OP_FLAGS[7..4]
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	swapf	WREG, W
	btfsc	MIOS_PARAMETER2, 0
	iorwf	INDF0, F	; CS_MENU_SELECTED_x_FLAGS
	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Ins_Play
	SET_BSR	MBFM_BASE
	movwf	MBFM_CURRENT_INSTRUMENT, BANKED

	;; get base pointer to instrument in FSR1 and copy to FSR0
	call	MBFM_INSTR_GetBase
	movff	FSR1L, FSR0L
	movff	FSR1H, FSR0H

	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_MENU_BUTTON_Ins_Play_1
CS_MENU_BUTTON_Ins_Play_0
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_PLAY_INS_FLAGS, F

	IRQ_DISABLE
	;; pop note from stack (pointer to stack in FSR2)
	movf	FSR0L, W
	addlw	MBFM_Ix_NOTE_STACK_0
	movwf	FSR2L
	movf	FSR0H, W
	skpnc
	incf	FSR0H, W
	movwf	FSR2H
	movff	INDF2, TMP3	; save current #0 entry in TMP3 for later use
	call	MBFM_MIDI_Hlp_PopNote
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Ins_Play_0_Skip
	movf	TMP3, W		; restore note
	call	MBFM_MIDI_Hlp_NoteOff
CS_MENU_BUTTON_Ins_Play_0_Skip
	IRQ_ENABLE

	return

CS_MENU_BUTTON_Ins_Play_1
	movf	MBFM_CURRENT_INSTRUMENT, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_PLAY_INS_FLAGS, F

	movlw	0x3c
	movwf	MBFM_MIDI_PARAMETER1, BANKED
	movlw	100
	movwf	MBFM_MIDI_PARAMETER2, BANKED

	IRQ_DISABLE
	;; push note to stack (pointer to stack in FSR2)
	movf	FSR0L, W
	addlw	MBFM_Ix_NOTE_STACK_0
	movwf	FSR2L
	movf	FSR0H, W
	skpnc
	incf	FSR0H, W
	movwf	FSR2H
	call	MBFM_MIDI_Hlp_PushNote
	BRA_IFSET WREG, 0, ACCESS, CS_MENU_BUTTON_Ins_Play_1_Skip ; exit if note already in stack
	movlw	MBFM_Ix_CTRL1				; switch off gate if not in legato mode
	CALL_IFCLR PLUSW0, MBFM_CTRL1_LEGATO, ACCESS, MBFM_MIDI_Hlp_GateOff
	call	MBFM_MIDI_Hlp_NoteOn			; call note-on handler
CS_MENU_BUTTON_Ins_Play_1_Skip
	IRQ_ENABLE

	return

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_InsOPSel
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; toggle InsOP LED
	btg	CS_MENU_MODE, CS_MENU_MODE_INS_OP
	;; and request display update
	rgoto	CS_MENU_BUTTON_DisplayUpdateReq

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_DisplayUpdateReq
	;; force a display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return
