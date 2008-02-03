; $Id$
;
; MB64E Menu MIDI Learn Handler
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
CS_M_LEARN_Exit
	;; exit learn mode
	bcf	MB_STAT, MB_STAT_LEARN_MODE

	;; reconfigure MIDI merger which has been switched off in Learn mode
	call	MB64E_DUMP_RestoreGlobal
	
	return

;; --------------------------------------------------------------------------
;;  Menu Snapshot routine is called when the snapshot button has been pressed
;; --------------------------------------------------------------------------
CS_M_LEARN_Snapshot
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment and display button mode
	SET_BSR	MB64E_BASE
	incf	MB64E_LEARN_BMODE, F, BANKED
	movlw	0x02
	cpfsgt	MB64E_LEARN_BMODE, BANKED
	rgoto CS_M_LEARN_Snapshot_NoReset
	clrf	MB64E_LEARN_BMODE, BANKED
CS_M_LEARN_Snapshot_NoReset
	;; print message depending on mode
	TABLE_ADDR CS_M_LEARN_BMODE_STR_0
	call	MIOS_LCD_PrintMessage

	;; second line
	movlw	0x40
	call	MIOS_LCD_CursorSet

	SET_BSR	MB64E_BASE
	movf	MB64E_LEARN_BMODE, W, BANKED
	mullw	CS_M_LEARN_BMODE_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_LEARN_BMODE_LEN		; finally print table entry
	goto	MIOS_LCD_PrintPreconfString

CS_M_LEARN_BMODE_STR_0 STRING 16, 0x00, "Learn Button:   "

CS_M_LEARN_BMODE_LEN		EQU	16	; 16 characters per entry
CS_M_LEARN_BMODE_ENTRIES	EQU	3	; 3 entries
CS_M_LEARN_BMODE_TABLE				; all entries are listed here
	db	"ON/OFF Value    "
	db	"ON Value only   "
	db	"Toggle Value    "

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_LEARN_Init
	movlw	2
	movwf	CS_MENU_PARAMETER_MAX_L
	clrf	CS_MENU_PARAMETER_MAX_H

	clrf	CS_MENU_PARAMETER_L
	clrf	CS_MENU_PARAMETER_H

	TABLE_ADDR CS_M_LEARN_ValueChanged
	call	CS_MENU_SetMenuParameterHook

	;; disable MIDI merger in LEARN mode
	movlw	MIOS_MIDI_MERGER_DISABLED
	call	MIOS_MIDI_MergerSet

	;; enter learn mode
	bsf	MB_STAT, MB_STAT_LEARN_MODE

	;; invalidate event (so that we have to receive a new event to continue)
	SET_BSR	MB64E_BASE
	clrf	MB64E_LEARN_EVNT0, BANKED

	return

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_LEARN_ValueChanged
	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_LEARN_Tick
	;; received value?
	SET_BSR	MB64E_BASE
	movf	MB64E_LEARN_EVNT0, BANKED
	bz	CS_M_LEARN_Tick_Waiting

CS_M_LEARN_Tick_PrintEvent
	;; print the event at first line
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	MB64E_LCD_PrintEvntStd_Learn

	;; print option at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	TABLE_ADDR CS_M_LEARN_NAME_TABLE	; initial pointer to table
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	mullw	CS_M_LEARN_NAME_LEN		; multiply with string len
	movf	PRODL, W			; add result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	CS_M_LEARN_NAME_LEN		; finally print table entry
	call	MIOS_LCD_PrintPreconfString

	;; print arrows and exit
	goto	CS_MENU_PrintArrows

	
CS_M_LEARN_WAITING_0	STRING 16, 0x00, "Learn: Waiting  "
CS_M_LEARN_WAITING_1	STRING 16, 0x40, "for a MIDI Event"
CS_M_LEARN_Tick_Waiting
	;; we are waiting for a MIDI event...
	TABLE_ADDR CS_M_LEARN_WAITING_0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString
	return


;; ==========================================================================

CS_M_LEARN_STR_0 STRING 16, 0x00, "Enc Behaviour:  "

CS_M_LEARN_NAME_LEN		EQU	14	; 14 characters per entry
CS_M_LEARN_NAME_ENTRIES		EQU	3	; 3 entries
CS_M_LEARN_NAME_TABLE				; all entries are listed here
	db	"Learn Single  "
	db	"Learn Row Chn "
	db	"Learn Row Add "


;; ==========================================================================
;;  called by mb64e_encs.inc and mb64e_fader.inc
;;  IN: enc number in WREG
;; ==========================================================================
CS_M_LEARN_NotifyEncEvent
CS_M_LEARN_NotifyFaderEvent

	;; do nothing if no MIDI event received
	SET_BSR	MB64E_BASE
	movf	MB64E_LEARN_EVNT0, F, BANKED
	skpnz
	return

	clrf	TMP5		; TMP5[7] used to indicate a enc or button event
	rgoto	CS_M_LEARN_StoreEvent


;; ==========================================================================
;;  called by MB64E_buttons.inc
;;  IN: button number in WREG
;; ==========================================================================
CS_M_LEARN_NotifyButtonEvent

	;; do nothing if no MIDI event received
	SET_BSR	MB64E_BASE
	movf	MB64E_LEARN_EVNT0, F, BANKED
	skpnz
	return

	clrf	TMP5		; TMP5[7] used to indicate a enc or button event
	bsf	TMP5, 7
	rgoto	CS_M_LEARN_StoreEvent

;; --------------------------------------------------------------------------
;;  This function is called by MB64E_MIDI_NotifyReceivedEvent when a complete
;;  MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
CS_M_LEARN_NotifyReceivedEvent
	;; ignore SysEx events
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	movwf	TMP1

	xorlw	0xf0
	skpnz
	return

	;; ignore Note Off Events
	movf	TMP1, W
	xorlw	0x80
	skpnz
	return

	;; ignore Note On events with velocity == 0 (-> Note Off)
	movf	TMP1, W
	xorlw	0x90
	skpnz
	movf	MIOS_PARAMETER3, W
	skpnz
	return

	;; if Program Change: copy MIOS_PARAMETER2 to MIOS_PARAMETER3
	movf	TMP1, W
	xorlw	0xc0
	skpnz
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3

	;; if Channel Pressure: copy MIOS_PARAMETER2 to MIOS_PARAMETER3
	movf	TMP1, W
	xorlw	0xd0
	skpnz
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3

	;; copy events to learn registers
	movff	MIOS_PARAMETER1, MB64E_LEARN_EVNT0
	movff	MIOS_PARAMETER2, MB64E_LEARN_EVNT1
	movff	MIOS_PARAMETER3, MB64E_LEARN_EVNT2

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	return

	
;; ==========================================================================
;;  store an event in bank
;;  IN: enc or button number in WREG, 
;;      event in MB64E_LEARN_EVNT[012]
;;      button mode (on/off, on only or toggle) in MB64E_LEARN_BMODE
;;      learn mode (single/row/add) in CS_MENU_PARAMETER_L[1..0]
;;      TMP5[7] == 0: store enc event
;;      TMP5[7] == 1: store button event
;; ==========================================================================
CS_M_LEARN_StoreEvent
	movwf	TMP1
	;; call the general store routine (which uses the same parameters)
	movf	CS_MENU_PARAMETER_L, W
	andlw	0x03
	iorwf	TMP5, F
	movf	TMP1, W
	call	MB64E_MEM_StoreEvent
	
	;; restore enc/button setup
	call	MB64E_DUMP_RestoreEncButtonEvnts

	;; invalidate event (so that we have to receive a new event to continue)
	SET_BSR	MB64E_BASE
	clrf	MB64E_LEARN_EVNT0, BANKED
	
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; stop message and exit
	goto	MIOS_LCD_MessageStop
