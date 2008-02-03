; $Id$
;
; MIDIbox MF
; Button handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  the DIN map (values defined in main.asm header)
;; --------------------------------------------------------------------------
MBMF_BUTTON_HANDLER_DIN_MAP
	db	(DEFAULT_DIN_SR_PIN_01_08 - 1) & 0xff, (DEFAULT_DIN_SR_PIN_09_16 - 1) & 0xff
	db	(DEFAULT_DIN_SR_PIN_17_24 - 1) & 0xff, (DEFAULT_DIN_SR_PIN_25_32 - 1) & 0xff
	db	(DEFAULT_DIN_SR_PIN_33_40 - 1) & 0xff, (DEFAULT_DIN_SR_PIN_41_48 - 1) & 0xff
	db	(DEFAULT_DIN_SR_PIN_49_56 - 1) & 0xff, (DEFAULT_DIN_SR_PIN_57_64 - 1) & 0xff

;; --------------------------------------------------------------------------
;; This function maps the physical pins to the button entry of the
;; SysEx structure depending on the DEFAULT_DIN_SR_PIN* settings
;; IN: physical button number in MIOS_PARAMETER1
;; OUT: MBMF button number in MIOS_PARAMETER1
;;      zero flag set if pin is mapped to a physical button
;; USES: TMP[12] and TBLPTR[LH]/TABLAT
;; --------------------------------------------------------------------------
MBMF_BUTTON_Map
	;; search for the DIN register in the DIN map
	;; extract the current shift register from MIOS_PARAMETER1[6..3]
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	movwf	TMP1

	clrf	TMP2		; used as counter
	TABLE_ADDR MBMF_BUTTON_HANDLER_DIN_MAP
MBMF_BUTTON_Map_Loop
	tblrd*+
	movf	TMP1, W
	xorwf	TABLAT, W
	bz	MBMF_BUTTON_Map_SR_Found
	incf	TMP2, F		; loop 8 times
	BRA_IFCLR TMP2, 3, ACCESS, MBMF_BUTTON_Map_Loop
	;; no SR found -> exit with zero flag cleared
	iorlw	0xff		; (clears the zero flag)
	return

MBMF_BUTTON_Map_SR_Found
	;; replace SR number
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	rlf	TMP2, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x78
	iorwf	MIOS_PARAMETER1, F

	;; exit with zero flag cleared
	andlw	0x00		; (sets the zero flag)
	return

;; --------------------------------------------------------------------------
;;  the button handler of MBMF
;;  Input:
;;     o Button number in MIOS_PARAMETER1 (AND NOT IN WREG!)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
MBMF_BUTTON_Handler

	;; check for control surface buttons - the CS handler will jump
	;; back if the button has not been assigned to a CS function
	goto	CS_MENU_BUTTON_Handler
CS_MENU_BUTTON_Handler_Return

	;; map physical button to MBMF button number
	call	MBMF_BUTTON_Map
	bz	MBMF_BUTTON_GP	; branch to GP handler if zero flag set (button found)
	return			; else exit


;; --------------------------------------------------------------------------
;;  MBMF specific snapshot button implementation
;; --------------------------------------------------------------------------
MBMF_BUTTON_Snapshot
	;; send snapshot when button has been depressed
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, MBMF_BUTTON_Snapshot_Depressed

MBMF_BUTTON_Snapshot_Pressed
	;; install delayed callback hook, used to trigger the save function after 2 seconds
	TABLE_ADDR MBMF_BUTTON_Snapshot_Save
	movlw	0x08
	call	CS_MENU_SetDelayedCallbackHook

	;; set HOLD flag
	bsf	MB_STAT, MB_STAT_SNAPSHOT_HOLD

	;; print message
	call	MIOS_LCD_Clear
	TABLE_ADDR MBMF_BUTTON_SNPSHOT_HOLD_STR_0
	call	MIOS_LCD_PrintMessage
	goto	MIOS_LCD_PrintMessage
MBMF_BUTTON_SNPSHOT_HOLD_STR_0	STRING 16, 0x00, " Hold 2 seconds "
MBMF_BUTTON_SNPSHOT_HOLD_STR_1	STRING 16, 0x40, " to save values "

MBMF_BUTTON_Snapshot_Depressed
	;; exit if hold flag already cleared (-> snapshot has been saved)
	btfss	MB_STAT, MB_STAT_SNAPSHOT_HOLD
	return
	;; else:
	;; clear HOLD flag
	bcf	MB_STAT, MB_STAT_SNAPSHOT_HOLD
	;; deinstall hook
	movlw	0x00
	call	CS_MENU_SetDelayedCallbackHook
	;; send snapshot
	goto	MBMF_PATCH_Send


	;; callback function
MBMF_BUTTON_Snapshot_Save
	;; clear hold flag
	bcf	MB_STAT, MB_STAT_SNAPSHOT_HOLD
	;; branch to save function
	goto	MBMF_PATCH_Save

;; --------------------------------------------------------------------------
;;  general purpose button function
;;  Input:
;;     o Button number in MIOS_PARAMETER1 (AND NOT IN WREG!)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
MBMF_BUTTON_GP
	;; save current button number in MBMF_CURRENT_BUTTON
	SET_BSR MBMF_BASE
	movf	MIOS_PARAMETER1, W
	movwf	MBMF_CURRENT_BUTTON, BANKED

	;; branch to SFB handler if button has been assigned to 0xff
	call	MBMF_FSR_ButtonEvnt0
	incf	INDF0, W
	bnz	MBMF_BUTTON_GP_NoSFB
MBMF_BUTTON_GP_SFB
	;; toggle LED which is assigned to the SFB
	rcall	MBMF_LED_Set	; expects MIOS_PARAMETER[12] as parameter
	;; request display re-initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; save value in MIDI_EVNT_VALUE
	;; (do it here, since MIOS_PARAMETER2 will be overwritten by MBMF_BANK_Read)
	movff	MIOS_PARAMETER2, MIDI_EVNT_VALUE
	;; save SFB event number in MIDI_EVNT0
	call	MBMF_FSR_ButtonEvnt1
	movff	INDF0, MIDI_EVNT0
	;; save special function parameter value in MIDI_EVNT1
	;; calc address to button mode entry:
	call	MBMF_ADDR_ButtonEntry
	;; select the 3th byte
	movlw	3-1
	addwf	MB_ADDRL, F
	;; save it in MIDI_EVNT1
	call	MBMF_BANK_Read
	movwf	MIDI_EVNT1
	;; branch to SFB handler
	goto	MBMF_SFB_Handler
MBMF_BUTTON_GP_NoSFB

	;; save it also in MBMF_LAST_BUTTON0
	;; if LAST_BUTTON0 != CURRENT_BUTTON, shift value to LAST_BUTTON1
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	xorwf	MBMF_LAST_BUTTON0, W, BANKED
	skpz
	movff	MBMF_LAST_BUTTON0, MBMF_LAST_BUTTON1
	movff	MBMF_CURRENT_BUTTON, MBMF_LAST_BUTTON0
	;; notify that last controller move was a button
	bsf	MB_STAT, MB_STAT_LAST_WAS_BUTTON

	;; for MIDI learn function
	rcall	MBMF_BUTTON_Learn

	;; exit here if we are in learn mode (don't send button value)
	btfsc	MB_STAT, MB_STAT_LEARN_MODE
	return

	;; store current button value in TMP1
	movf	MIOS_PARAMETER2, W
	movwf	TMP1

	;; calc address to button mode entry:
	call	MBMF_ADDR_ButtonEntry
	;; select the 4th byte
	movlw	4-1
	addwf	MB_ADDRL, F

	;; get fourth byte - which selects the button mode - and branch depending on value
	call	MBMF_BANK_Read
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MBMF_BUTTON_OnOff
	rgoto	MBMF_BUTTON_OnOnly
	rgoto	MBMF_BUTTON_Toggle
	rgoto	MBMF_BUTTON_OnOff

	;; (button value stored in TMP1)
MBMF_BUTTON_OnOff
	;; save status of button
	rcall	MBMF_BUTTON_Hlp_SaveStat
	rgoto	MBMF_BUTTON_Send

	;; (button value stored in TMP1)
MBMF_BUTTON_OnOnly
	;; save status of button
	rcall	MBMF_BUTTON_Hlp_SaveStat

	;; when on: send button value defined in dump
	;; when off: send nothing
	BRA_IFSET TMP1, 0, ACCESS, MBMF_BUTTON_NotifyChangeEnd
	rgoto	MBMF_BUTTON_Send

	;; (button value stored in TMP1)
MBMF_BUTTON_Toggle
	;; when on: toggle between 0x00 and the button value defined in dump
	;; when off: send nothing
	BRA_IFSET TMP1, 0, ACCESS, MBMF_BUTTON_NotifyChangeEnd

	;; toggle appr. bit in BUTTON_VALUE register
	call	MBMF_FSR_ButtonValue
	SET_BSR	MBMF_BASE
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	call	MIOS_HLP_GetBitORMask
	xorwf	INDF0, F

	;; 	rgoto	MBMF_BUTTON_Send


MBMF_BUTTON_Send
	call	MBMF_MIDI_SendButtonEvent

	;; feedback, required if several entries are assigned to the same event
	call	MBMF_MIDI_Scan_Feedback 

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	;; reset the cursor of the CS
	call	CS_MENU_ResetCursor

MBMF_BUTTON_NotifyChangeEnd
	return


;; --------------------------------------------------------------------------
	;; this help function saves the button status in the MBMF_BUTTON register
MBMF_BUTTON_Hlp_SaveStat
	call	MBMF_FSR_ButtonValue
	movf	MBMF_CURRENT_BUTTON, W, BANKED
	andlw	0x07

	BRA_IFSET TMP1, 0, ACCESS, MBMF_BUTTON_Hlp_SaveStat1
MBMF_BUTTON_Hlp_SaveStat0
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF0, F
	return
MBMF_BUTTON_Hlp_SaveStat1
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF0, F
	return


;; --------------------------------------------------------------------------
;;  learn mode handler for buttons
;;  IN: button number in MIOS_PARAMETER1, button value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
MBMF_BUTTON_Learn
	;; exit if not in learn mode
	btfss	MB_STAT, MB_STAT_LEARN_MODE
	return

	;; exit if button has not been pressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; trigger learn function
	movf	MIOS_PARAMETER1, W
	goto	CS_M_LEARN_NotifyButtonEvent
