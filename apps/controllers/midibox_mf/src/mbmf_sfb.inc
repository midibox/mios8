; $Id$
;
; MIDIbox MF
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
;;  This function is called by MBMF_button.inc when a special function
;;  has been assigned to a button
;;  It's also used by MBMF_sysex.inc to trigger a SFB event directly via SysEx
;;  (Remote Control feature)
;;  IN: 
;;        o special function number in MIDI_EVNT0
;;        o special function parameter value in MIDI_EVNT1
;;        o button value in MIDI_EVNT_VALUE (0=pressed, 1=depressed)
;; --------------------------------------------------------------------------
MBMF_SFB_Handler
	;; stop current message if button has been released
	CALL_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_MSG_Stop

	;; branch depending on selected SF
	movf	MIDI_EVNT0, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MBMF_SFB_Handler_00
	rgoto	MBMF_SFB_Handler_01
	rgoto	MBMF_SFB_Handler_02
	rgoto	MBMF_SFB_Handler_03
	rgoto	MBMF_SFB_Handler_04
	rgoto	MBMF_SFB_Handler_05
	rgoto	MBMF_SFB_Handler_06
	rgoto	MBMF_SFB_Handler_07
	rgoto	MBMF_SFB_Handler_08
	rgoto	MBMF_SFB_Handler_09
	rgoto	MBMF_SFB_Handler_0A
	rgoto	MBMF_SFB_Handler_0B
	rgoto	MBMF_SFB_Handler_0C
	rgoto	MBMF_SFB_Handler_0D
	rgoto	MBMF_SFB_Handler_0E
	rgoto	MBMF_SFB_Handler_0F

;; --------------------------------------------------------------------------
;; SF 00: Change Menu
MBMF_SFB_Handler_00
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_End; do nothing if button is depressed
	;; call the change menu function of the control surface
	movf	MIDI_EVNT1, W
	call	CS_MENU_ChangePage
	rgoto	MBMF_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 01: Switch group
MBMF_SFB_Handler_01
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_End; do nothing if button is depressed

	;; set base address
	SET_BSR	MBMF_BASE
	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MBMF_SFB_Handler_01_1x
MBMF_SFB_Handler_01_00 ; SF 02 0n: switch to Group
	movf	MIDI_EVNT1, W
	andlw	0x0f
	mullw	DEFAULT_FADER_GROUP_WIDTH
	movf	PRODL, W
	andlw	0x7f
	movwf	MBMF_GROUP_OFFSET, BANKED
	rgoto	MBMF_SFB_Handler_01_End

MBMF_SFB_Handler_01_1x ; SF 02 1x: inc/dec bank
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MBMF_SFB_Handler_01_12or13
MBMF_SFB_Handler_01_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_01_11

MBMF_SFB_Handler_01_10		; decrement without overflow
	movf	MBMF_GROUP_OFFSET, W, BANKED
	addlw	-DEFAULT_FADER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 0x00
	movwf	MBMF_GROUP_OFFSET, BANKED
	rgoto	MBMF_SFB_Handler_01_End

MBMF_SFB_Handler_01_11		; decrement with overflow
	movf	MBMF_GROUP_OFFSET, W, BANKED
	addlw	-DEFAULT_FADER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 128-DEFAULT_FADER_GROUP_WIDTH
	movwf	MBMF_GROUP_OFFSET, BANKED
	rgoto	MBMF_SFB_Handler_01_End

MBMF_SFB_Handler_01_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_01_13
	
MBMF_SFB_Handler_01_12		; increment without overflow
	movf	MBMF_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_FADER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 128-DEFAULT_FADER_GROUP_WIDTH
	movwf	MBMF_GROUP_OFFSET, BANKED
	rgoto	MBMF_SFB_Handler_01_End

MBMF_SFB_Handler_01_13		; increment with overflow
	movf	MBMF_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_FADER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 0x00
	movwf	MBMF_GROUP_OFFSET, BANKED
	;; 	rgoto	MBMF_SFB_Handler_01_End

MBMF_SFB_Handler_01_End
	;; request fader position update
	bsf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ

	;; don't print message if in "display" menu, page #0 and #2
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, MBMF_SFB_Handler_01_Msg
	movf	CS_MENU_POS, W
	bnz	MBMF_SFB_Handler_01_Msg
	SET_BSR	MBMF_BASE
	movf	MBMF_DISPLAY_PAGE, W, BANKED
	bz	MBMF_SFB_Handler_01_NoMsg
	movf	MBMF_DISPLAY_PAGE, W, BANKED
	xorlw	0x02
	bz	MBMF_SFB_Handler_01_NoMsg
MBMF_SFB_Handler_01_Msg
	;; print message
	movlw	MBMF_MSG_ID_BANK
	movff	MB_BANK, MBMF_SELECTED_BANK
	call	MBMF_MSG_Hold
MBMF_SFB_Handler_01_NoMsg
	;; and exit
	rgoto	MBMF_SFB_Handler_End


;; --------------------------------------------------------------------------
;; SF 02: Switch BankStick bank
MBMF_SFB_Handler_02
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_SFB_Handler_02_Msg

	;; bankstick available
	;; extract first 3 bit of bank and save in in TMP1
	movf	MB_BANK, W
	andlw	0x07
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MBMF_SFB_Handler_02_1x
MBMF_SFB_Handler_02_00 ; SF 02 0n: switch to BankStick bank
	movf	MIDI_EVNT1, W
	andlw	0x07
	movwf	TMP1
	rgoto	MBMF_SFB_Handler_02_End

MBMF_SFB_Handler_02_1x ; SF 02 1x: inc/dec bank
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MBMF_SFB_Handler_02_12or13
MBMF_SFB_Handler_02_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_02_11

MBMF_SFB_Handler_02_10		; decrement without overflow
	decf	TMP1, F
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MBMF_SFB_Handler_02_End

MBMF_SFB_Handler_02_11		; decrement with overflow
	decf	TMP1, F
	rgoto	MBMF_SFB_Handler_02_End

MBMF_SFB_Handler_02_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_02_13
	
MBMF_SFB_Handler_02_12		; increment without overflow
	incf	TMP1, F
	btfsc	TMP1, 3
	decf	TMP1, F
	rgoto	MBMF_SFB_Handler_02_End

MBMF_SFB_Handler_02_13		; increment with overflow
	incf	TMP1, F
	;; 	rgoto	MBMF_SFB_Handler_02_End

MBMF_SFB_Handler_02_End
	;; change to new bank
	movlw	0x07
	andwf	TMP1, F
	movf	MB_BANK, W
	andlw	0xf8
	iorwf	TMP1, W

	;; (reused by SF 03, switch bankstick)
MBMF_SFB_Handler_03_Cont
	call	MBMF_BANK_Change

MBMF_SFB_Handler_02_Msg
MBMF_SFB_Handler_03_Msg
	;; request fader position update
	bsf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ
	;; print message
	movlw	MBMF_MSG_ID_BANK
	movff	MB_BANK, MBMF_SELECTED_BANK
	call	MBMF_MSG_Hold
	;; and exit
	rgoto	MBMF_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 03: Switch BankStick
MBMF_SFB_Handler_03
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MBMF_SFB_Handler_03_Msg

MBMF_SFB_Handler_03_Ext
	;; extract bankstick number and save in in TMP1
	swapf	MB_BANK, W
	andlw	0x07
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MBMF_SFB_Handler_03_1x
MBMF_SFB_Handler_03_00 ; SF 02 0n: switch to BankStick
	movf	MIDI_EVNT1, W
	andlw	0x07
	movwf	TMP1
	rgoto	MBMF_SFB_Handler_03_End

MBMF_SFB_Handler_03_1x ; SF 02 1x: inc/dec bankstick
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MBMF_SFB_Handler_03_12or13
MBMF_SFB_Handler_03_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_03_11

MBMF_SFB_Handler_03_10
	decf	TMP1, F		; decrement without overflow
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MBMF_SFB_Handler_03_End

MBMF_SFB_Handler_03_11
	decf	TMP1, F		; decrement with overflow
	rgoto	MBMF_SFB_Handler_03_End

MBMF_SFB_Handler_03_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_03_13
	
MBMF_SFB_Handler_03_12
	incf	TMP1, F		; increment without overflow
	btfsc	TMP1, 3
	decf	TMP1, F
	rgoto	MBMF_SFB_Handler_03_End

MBMF_SFB_Handler_03_13
	incf	TMP1, F		; increment with overflow
	rgoto	MBMF_SFB_Handler_03_End

MBMF_SFB_Handler_03_End
	;; change to new BankStick
	swapf	TMP1, W
	andlw	0x70
	movwf	TMP1
	movf	MB_BANK, W
	andlw	0x0f
	iorwf	TMP1, W
	rgoto	MBMF_SFB_Handler_03_Cont


;; --------------------------------------------------------------------------
;; SF 04: Save Snapshot, Switch Bank, Restore Bank
MBMF_SFB_Handler_04
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_End; do nothing if button is depressed

	call	MBMF_PATCH_Save
	rgoto	MBMF_SFB_Handler_02


;; --------------------------------------------------------------------------
;; SF 08: Set Fader Mode
MBMF_SFB_Handler_08
	BRA_IFSET MIDI_EVNT1, 6, ACCESS, MBMF_SFB_Handler_08_4x
	rgoto	MBMF_SFB_Handler_End

MBMF_SFB_Handler_08_4x	; SF 08 4x: Select Fader Parameter or Group/Bank
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MBMF_SFB_Handler_08_41
	rgoto	MBMF_SFB_Handler_End

MBMF_SFB_Handler_08_41	; SF 08 41: Select Bank/Group
	BRA_IFCLR MIDI_EVNT_VALUE, 0, ACCESS, MBMF_SFB_Handler_08_41_1

MBMF_SFB_Handler_08_41_0	; button depressed
	;; exit "select bank" mode
	bcf	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	bcf	MB_STAT2, MB_STAT2_MUTE_FADERS

	;; change to new bank if MBMF_SELECTED_BANK different from MB_BANK
	movff	MBMF_SELECTED_BANK, WREG
	cpfseq	MB_BANK, ACCESS
	call MBMF_BANK_Change

#if DEFAULT_PATCHMODE != 0
	;; change to new patch if MBMF_SELECTED_PATCH different from MB_PATCH
	movff	MBMF_SELECTED_PATCH, WREG
	cpfseq	MB_PATCH, ACCESS
	call MBMF_PATCH_Change
#endif

	;; request fader position update
	bsf	MB_STAT2, MB_STAT2_FADER_UPDATE_REQ

	;; and exit
	rgoto	MBMF_SFB_Handler_End

MBMF_SFB_Handler_08_41_1	; button pressed
	;; transfer current bank to selected bank
	movff	MB_BANK, MBMF_SELECTED_BANK
	;; transfer current patch to selected patch
	movff	MB_PATCH, MBMF_SELECTED_PATCH

	;; update fader positions to bank/group positions
	movlw	0x00
	call	MBMF_FADER_UpdatePos

	;; print message
	movlw	MBMF_MSG_ID_BANK
	call	MBMF_MSG_Hold

	;; and set "bank and group" and "mute faders" flag
	bsf	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	bsf	MB_STAT2, MB_STAT2_MUTE_FADERS

	rgoto	MBMF_SFB_Handler_End


;; --------------------------------------------------------------------------
;; unsupported or unimplemented special functions
MBMF_SFB_Handler_05
MBMF_SFB_Handler_06
MBMF_SFB_Handler_07
MBMF_SFB_Handler_09
MBMF_SFB_Handler_0A
MBMF_SFB_Handler_0B
MBMF_SFB_Handler_0C
MBMF_SFB_Handler_0D
MBMF_SFB_Handler_0E
MBMF_SFB_Handler_0F
MBMF_SFB_Handler_End
	return



