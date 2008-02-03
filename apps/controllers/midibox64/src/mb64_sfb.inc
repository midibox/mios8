; $Id$
;
; MIDIbox64
; Special Function Buttons Handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by mb64_button.inc when a special function
;;  has been assigned to a button
;;  It's also used by mb64_sysex.inc to trigger a SFB event directly via SysEx
;;  (Remote Control feature)
;;  IN: 
;;        o special function number in MIDI_EVNT0
;;        o special function parameter value in MIDI_EVNT1
;;        o button value in MIDI_EVNT_VALUE (0=pressed, 1=depressed)
;; --------------------------------------------------------------------------
MB64_SFB_Handler
	;; stop current message if button has been released
	CALL_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_MSG_Stop

	;; branch depending on selected SF
	movf	MIDI_EVNT0, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MB64_SFB_Handler_00
	rgoto	MB64_SFB_Handler_01
	rgoto	MB64_SFB_Handler_02
	rgoto	MB64_SFB_Handler_03
	rgoto	MB64_SFB_Handler_04
	rgoto	MB64_SFB_Handler_05
	rgoto	MB64_SFB_Handler_06
	rgoto	MB64_SFB_Handler_07
	rgoto	MB64_SFB_Handler_08
	rgoto	MB64_SFB_Handler_09
	rgoto	MB64_SFB_Handler_0A
	rgoto	MB64_SFB_Handler_0B
	rgoto	MB64_SFB_Handler_0C
	rgoto	MB64_SFB_Handler_0D
	rgoto	MB64_SFB_Handler_0E
	rgoto	MB64_SFB_Handler_0F

;; --------------------------------------------------------------------------
;; SF 00: Change Menu
MB64_SFB_Handler_00
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_SFB_Handler_End; do nothing if button is depressed
	;; call the change menu function of the control surface
	movf	MIDI_EVNT1, W
	call	CS_MENU_ChangePage
	rgoto	MB64_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 01: Switch page
;;        not supported by MB64
MB64_SFB_Handler_01
	rgoto	MB64_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 02: Switch BankStick bank
MB64_SFB_Handler_02
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_SFB_Handler_02_Msg

	;; bankstick available
	;; extract first 4 bit of bank and save in in TMP1
	movf	MB_BANK, W
	andlw	0x0f
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64_SFB_Handler_02_1x
MB64_SFB_Handler_02_0x ; SF 02 0n: switch to BankStick bank
	movf	MIDI_EVNT1, W
	andlw	0x0f
	movwf	TMP1
	rgoto	MB64_SFB_Handler_02_End

MB64_SFB_Handler_02_1x ; SF 02 1x: inc/dec bank
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64_SFB_Handler_02_12or13
MB64_SFB_Handler_02_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_02_11

MB64_SFB_Handler_02_10		; decrement without overflow
	decf	TMP1, F
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MB64_SFB_Handler_02_End

MB64_SFB_Handler_02_11		; decrement with overflow
	decf	TMP1, F
	rgoto	MB64_SFB_Handler_02_End

MB64_SFB_Handler_02_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_02_13
	
MB64_SFB_Handler_02_12		; increment without overflow
	incf	TMP1, F
	btfsc	TMP1, 4
	decf	TMP1, F
	rgoto	MB64_SFB_Handler_02_End

MB64_SFB_Handler_02_13		; increment with overflow
	incf	TMP1, F
	;; 	rgoto	MB64_SFB_Handler_02_End

MB64_SFB_Handler_02_End
	;; change to new bank
	movlw	0x0f
	andwf	TMP1, F
	movf	MB_BANK, W
	andlw	0xf0
	iorwf	TMP1, W

	;; (reused by SF 03, switch bankstick)
MB64_SFB_Handler_03_Cont
	call	MB64_BANK_Change

MB64_SFB_Handler_02_Msg
MB64_SFB_Handler_03_Msg
	;; print message
	movlw	MB64_MSG_ID_BANK
	call	MB64_MSG_Hold
	;; and exit
	rgoto	MB64_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 03: Switch BankStick
MB64_SFB_Handler_03
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64_SFB_Handler_03_Msg

MB64_SFB_Handler_03_Ext
	;; extract bankstick number and save in in TMP1
	swapf	MB_BANK, W
	andlw	0x07
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64_SFB_Handler_03_1x
MB64_SFB_Handler_03_0x ; SF 02 0n: switch to BankStick
	movf	MIDI_EVNT1, W
	andlw	0x07
	movwf	TMP1
	rgoto	MB64_SFB_Handler_03_End

MB64_SFB_Handler_03_1x ; SF 02 1x: inc/dec bankstick
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64_SFB_Handler_03_12or13
MB64_SFB_Handler_03_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_03_11

MB64_SFB_Handler_03_10
	decf	TMP1, F		; decrement without overflow
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MB64_SFB_Handler_03_End

MB64_SFB_Handler_03_11
	decf	TMP1, F		; decrement with overflow
	rgoto	MB64_SFB_Handler_03_End

MB64_SFB_Handler_03_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_03_13
	
MB64_SFB_Handler_03_12
	incf	TMP1, F		; increment without overflow
	btfsc	TMP1, 3
	decf	TMP1, F
	rgoto	MB64_SFB_Handler_03_End

MB64_SFB_Handler_03_13
	incf	TMP1, F		; increment with overflow
	rgoto	MB64_SFB_Handler_03_End

MB64_SFB_Handler_03_End
	;; change to new BankStick
	swapf	TMP1, W
	andlw	0x70
	movwf	TMP1
	movf	MB_BANK, W
	andlw	0x0f
	iorwf	TMP1, W
	rgoto	MB64_SFB_Handler_03_Cont

;; --------------------------------------------------------------------------
;; SF 04: Save Snapshot, Switch Bank, Restore Bank
MB64_SFB_Handler_04
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_SFB_Handler_End; do nothing if button is depressed

	call	MB64_PATCH_Save
	rgoto	MB64_SFB_Handler_02


;; --------------------------------------------------------------------------
;; SF 05: Switch Page
MB64_SFB_Handler_05
#if DEFAULT_ENABLE_PAGE_HANDLING

	SET_BSR MB64_BASE

	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64_SFB_Handler_End; do nothing if button is depressed

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64_SFB_Handler_05_1x
MB64_SFB_Handler_05_0x ; SF 05 0n: switch to page directly
	movf	MIDI_EVNT1, W
	rgoto	MB64_SFB_Handler_05_Cont

MB64_SFB_Handler_05_1x
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64_SFB_Handler_05_12or13

MB64_SFB_Handler_05_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_05_11

; FF 05 10
MB64_SFB_Handler_05_10
	movf	MB64_PAGE, W, BANKED
	skpz
	decf	MB64_PAGE, W, BANKED
	rgoto	MB64_SFB_Handler_05_Cont

; FF 05 11
MB64_SFB_Handler_05_11
	decf	MB64_PAGE, W, BANKED
	rgoto	MB64_SFB_Handler_05_Cont

MB64_SFB_Handler_05_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64_SFB_Handler_05_13

; FF 05 12	
MB64_SFB_Handler_05_12
	incf	MB64_PAGE, W, BANKED
	btfsc	WREG, 2
	movf	MB64_PAGE, W, BANKED
	rgoto	MB64_SFB_Handler_05_Cont

; FF 05 13
MB64_SFB_Handler_05_13
	incf	MB64_PAGE, W, BANKED
	rgoto	MB64_SFB_Handler_05_Cont

MB64_SFB_Handler_05_Cont
	;; change to new BankStick
	andlw	0x03
	movwf	TMP1

	;; branch to the end if new page is equal to current page
	xorwf	MB64_PAGE, W, BANKED
	bz	MB64_SFB_Handler_05_End

	;; switch to new page
	movf	TMP1, W
	movwf	MB64_PAGE, BANKED

	;; clear soft-takeover flag of all 64 pot entries
	clrf	MB64_CURRENT_POT, BANKED
MB64_SFB_Handler_05_CSLoop
	call	MB64_FSR_PotValue
	bcf	INDF0, 7
	incf	MB64_CURRENT_POT, F, BANKED
	BRA_IFCLR MB64_CURRENT_POT, 6, BANKED, MB64_SFB_Handler_05_CSLoop

	;; change LAST_POT, so that values of new page will be visible immediately
	swapf	MB64_PAGE, W, BANKED
	andlw	0x30
	movwf	MB64_LAST_POT0, BANKED

MB64_SFB_Handler_05_End
	rgoto	MB64_SFB_Handler_End	
#else
	return
#endif

;; --------------------------------------------------------------------------
;; unsupported or unimplemented special functions
MB64_SFB_Handler_06
MB64_SFB_Handler_07
MB64_SFB_Handler_08
MB64_SFB_Handler_09
MB64_SFB_Handler_0A
MB64_SFB_Handler_0B
MB64_SFB_Handler_0C
MB64_SFB_Handler_0D
MB64_SFB_Handler_0E
MB64_SFB_Handler_0F
MB64_SFB_Handler_End
	return
