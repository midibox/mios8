; $Id$
;
; MBSEQ Menu "Record Steps"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_RECORD_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,	0x46, 2,	CS_M_RECORD_ValueChanged

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_RECORD_GP_Callback
	;; start message if GP button has been released
	GOTO_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_MSG_Start

	;; select new record step
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L

	;; call the value change function
	call	CS_M_RECORD_ValueChanged

	;; and hold message
	goto	SEQ_MSG_Hold
	
;; --------------------------------------------------------------------------
;;  update LEDs
;; --------------------------------------------------------------------------
CS_M_RECORD_UpdateGPLEDs
	;; LEDs display the mute status like in edit mode
	call	SEQ_GP_Mode0_LED
	movff	SEQ_GP_LED0, CS_MENU_GP_OVERLAY_DOUT_SR0
	movff	SEQ_GP_LED1, CS_MENU_GP_OVERLAY_DOUT_SR1
	;; (flashing LED handled by seq_leds.inc, interrupt handler)
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_RECORD_Init
	;; install parameter table
	TABLE_ADDR CS_M_RECORD_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy step number to menu parameter
	movff	SEQ_RECORD_STEP, CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_RECORD_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; don't flash LEDs (record position will be flashed by IRQ service routine)
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; print static string
	TABLE_ADDR CS_M_RECORD_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_RECORD_STR0 STRING 13, 0x00, "Record Track:"
CS_M_RECORD_STR1 STRING  5, 0x40, "Step:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_RECORD_Tick
	rcall	CS_M_RECORD_UpdateGPLEDs
	
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print step number
	movlw	0x46
	call	MIOS_LCD_CursorSet
	SET_BSR	SEQ_BASE
	incf	SEQ_RECORD_STEP, W, BANKED
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_RECORD_ValueChanged
	;; copy menu parameter to step number
	movff	CS_MENU_PARAMETER_L, SEQ_RECORD_STEP
	;; update leds and exit
	rgoto	CS_M_RECORD_UpdateGPLEDs

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by SEQ_MIDI_NotifyReceivedEvent when a complete 
;;  MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CS_M_RECORD_NotifyEvent
	SET_BSR	SEQ_BASE
	;; copy MIOS_PARAMETER[123] to SEQ_EVNT[123], the values are modified
	movff	MIOS_PARAMETER1, SEQ_EVNT0
	movff	MIOS_PARAMETER2, SEQ_EVNT1
	movff	MIOS_PARAMETER3, SEQ_EVNT2
	;; the length is 0x10 by default
	movlw	0x10
	movwf	SEQ_EVNTL, BANKED
	;; select track and step
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED
	movff	SEQ_RECORD_STEP, SEQ_EVNTS	

	;; only continue of MIDI channel matches
	SET_BSR	SEQ_BASE
	movf	SEQ_EVNT0, W, BANKED
	andlw	0x0f
	xorwf	SEQ_MIDI_CHANNEL, W, BANKED
	bnz	CS_M_RECORD_NotifyEvent_End

	;; if we've received a note off event, convert it to 0x9x with velocity 0
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x80
	bnz	CS_M_RECORD_NotifyEvent_NoOff
	bsf	SEQ_EVNT0, 4, BANKED; 0x8x -> 0x9x
	clrf	SEQ_EVNT2, BANKED	
CS_M_RECORD_NotifyEvent_NoOff	

	;; only continue if received event type matches with track type
	call	SEQ_FSR_TrkEvnt0
	movf	INDF1, W
	xorwf	SEQ_EVNT0, W, BANKED
	andlw	0x70
	bnz	CS_M_RECORD_NotifyEvent_End

	;; if note event: ignore velocity == 0 (Note Off)
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x90
	bnz	CS_M_RECORD_NotifyEvent_NoNote
CS_M_RECORD_NotifyEvent_Note
	movf	SEQ_EVNT2, W, BANKED
	bz	CS_M_RECORD_NotifyEvent_Skip
CS_M_RECORD_NotifyEvent_NoNote

	;; save event
	call	SEQ_EVNT_Save

	;; unmute step
	call	SEQ_FSR_TrkMuted0
	btfsc	SEQ_EVNTS, 3, BANKED
	movf	POSTINC1, W	; (increment if step >= 8)

	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F

	;; increment step
	incf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	call	CS_M_RECORD_ValueChanged

CS_M_RECORD_NotifyEvent_Skip
	;; send event
	call	SEQ_FSR_TrkEvnt0
	bsf	INDF1, 7
	movff	INDF1, MIDI_EVNT0
	movff	SEQ_EVNT1, MIDI_EVNT1
	movff	SEQ_EVNT2, MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send

	;; we have to ensure that MIOS_PARAMETER[123] won't be overwritten!
	;; therefore save it in SEQ_DATA_BUFFER (dirty!!!) - sometimes I whished I would have good stack method...
	movff	MIOS_PARAMETER1, SEQ_DATA_BUFFER+0
	movff	MIOS_PARAMETER2, SEQ_DATA_BUFFER+1
	movff	MIOS_PARAMETER3, SEQ_DATA_BUFFER+2

	;; print message
	call	SEQ_MSG_Start

	;; update LEDs
	rcall	CS_M_RECORD_UpdateGPLEDs

	;; restore MIOS_PARAMETER[123]
	movff	SEQ_DATA_BUFFER+0, MIOS_PARAMETER1
	movff	SEQ_DATA_BUFFER+1, MIOS_PARAMETER2
	movff	SEQ_DATA_BUFFER+2, MIOS_PARAMETER3
CS_M_RECORD_NotifyEvent_End
	return

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_RECORD_Select
	;; start message if button depressed
	GOTO_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_MSG_Start

	;; decrement step
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	call	CS_M_RECORD_ValueChanged

	;; delete event
	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED
	movff	SEQ_RECORD_STEP, SEQ_EVNTS
	movlw	0x3c
	movwf	SEQ_EVNT1, BANKED
	movlw	0x7f
	movwf	SEQ_EVNT2, BANKED
	movlw	0x40
	movwf	SEQ_EVNTL, BANKED
	call	SEQ_EVNT_Save

	;; mute step
	call	SEQ_FSR_TrkMuted0
	btfsc	SEQ_EVNTS, 3, BANKED
	movf	POSTINC1, W	; (increment if step >= 8)

	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F

	;; update LEDs
	rcall	CS_M_RECORD_UpdateGPLEDs

	;; hold message and exit
	goto	SEQ_MSG_Hold
