; $Id$
;
; MIDIbox64E
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
;;  This function is called by MB64E_button.inc when a special function
;;  has been assigned to a button
;;  It's also used by MB64E_sysex.inc to trigger a SFB event directly via SysEx
;;  (Remote Control feature)
;;  IN: 
;;        o special function number in MIDI_EVNT0
;;        o special function parameter value in MIDI_EVNT1
;;        o button value in MIDI_EVNT_VALUE (0=pressed, 1=depressed)
;; --------------------------------------------------------------------------
MB64E_SFB_Handler
	;; stop current message if button has been released
	CALL_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_MSG_Stop

	;; branch depending on selected SF
	movf	MIDI_EVNT0, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	MB64E_SFB_Handler_00
	rgoto	MB64E_SFB_Handler_01
	rgoto	MB64E_SFB_Handler_02
	rgoto	MB64E_SFB_Handler_03
	rgoto	MB64E_SFB_Handler_04
	rgoto	MB64E_SFB_Handler_05
	rgoto	MB64E_SFB_Handler_06
	rgoto	MB64E_SFB_Handler_07
	rgoto	MB64E_SFB_Handler_08
	rgoto	MB64E_SFB_Handler_09
	rgoto	MB64E_SFB_Handler_0A
	rgoto	MB64E_SFB_Handler_0B
	rgoto	MB64E_SFB_Handler_0C
	rgoto	MB64E_SFB_Handler_0D
	rgoto	MB64E_SFB_Handler_0E
	rgoto	MB64E_SFB_Handler_0F

;; --------------------------------------------------------------------------
;; SF 00: Change Menu
MB64E_SFB_Handler_00
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End; do nothing if button is depressed
	;; call the change menu function of the control surface
	movf	MIDI_EVNT1, W
	call	CS_MENU_ChangePage
	rgoto	MB64E_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 01: Switch group
MB64E_SFB_Handler_01
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End; do nothing if button is depressed

	;; set base address
	SET_BSR	MB64E_BASE
	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64E_SFB_Handler_01_1x
MB64E_SFB_Handler_01_00 ; SF 02 0n: switch to Group
	movf	MIDI_EVNT1, W
	andlw	0x0f
	mullw	DEFAULT_ENCODER_GROUP_WIDTH
	movf	PRODL, W
	andlw	0x7f
	movwf	MB64E_GROUP_OFFSET, BANKED
	rgoto	MB64E_SFB_Handler_01_End

MB64E_SFB_Handler_01_1x ; SF 02 1x: inc/dec bank
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64E_SFB_Handler_01_12or13
MB64E_SFB_Handler_01_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_01_11

MB64E_SFB_Handler_01_10		; decrement without overflow
	movf	MB64E_GROUP_OFFSET, W, BANKED
	addlw	-DEFAULT_ENCODER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 0x00
	movwf	MB64E_GROUP_OFFSET, BANKED
	rgoto	MB64E_SFB_Handler_01_End

MB64E_SFB_Handler_01_11		; decrement with overflow
	movf	MB64E_GROUP_OFFSET, W, BANKED
	addlw	-DEFAULT_ENCODER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 128-DEFAULT_ENCODER_GROUP_WIDTH
	movwf	MB64E_GROUP_OFFSET, BANKED
	rgoto	MB64E_SFB_Handler_01_End

MB64E_SFB_Handler_01_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_01_13
	
MB64E_SFB_Handler_01_12		; increment without overflow
	movf	MB64E_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_ENCODER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 128-DEFAULT_ENCODER_GROUP_WIDTH
	movwf	MB64E_GROUP_OFFSET, BANKED
	rgoto	MB64E_SFB_Handler_01_End

MB64E_SFB_Handler_01_13		; increment with overflow
	movf	MB64E_GROUP_OFFSET, W, BANKED
	addlw	DEFAULT_ENCODER_GROUP_WIDTH
	btfsc	WREG, 7
	movlw 0x00
	movwf	MB64E_GROUP_OFFSET, BANKED
	;; 	rgoto	MB64E_SFB_Handler_01_End

MB64E_SFB_Handler_01_End
	;; request fader position update
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	;; update encoder parameter map
	call	MB64E_ENC_UpdateMap

	;; print message
	movlw	MB64E_MSG_ID_BANK
	movff	MB_BANK, MB64E_SELECTED_BANK
	call	MB64E_MSG_Hold
	;; and exit
	rgoto	MB64E_SFB_Handler_End


;; --------------------------------------------------------------------------
;; SF 02: Switch BankStick bank
MB64E_SFB_Handler_02
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_SFB_Handler_02_Msg

	;; bankstick available
	;; extract first 3 bit of bank and save in in TMP1
	movf	MB_BANK, W
	andlw	0x07
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64E_SFB_Handler_02_1x
MB64E_SFB_Handler_02_00 ; SF 02 0n: switch to BankStick bank
	movf	MIDI_EVNT1, W
	andlw	0x07
	movwf	TMP1
	rgoto	MB64E_SFB_Handler_02_End

MB64E_SFB_Handler_02_1x ; SF 02 1x: inc/dec bank
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64E_SFB_Handler_02_12or13
MB64E_SFB_Handler_02_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_02_11

MB64E_SFB_Handler_02_10		; decrement without overflow
	decf	TMP1, F
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MB64E_SFB_Handler_02_End

MB64E_SFB_Handler_02_11		; decrement with overflow
	decf	TMP1, F
	rgoto	MB64E_SFB_Handler_02_End

MB64E_SFB_Handler_02_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_02_13
	
MB64E_SFB_Handler_02_12		; increment without overflow
	incf	TMP1, F
	btfsc	TMP1, 3
	decf	TMP1, F
	rgoto	MB64E_SFB_Handler_02_End

MB64E_SFB_Handler_02_13		; increment with overflow
	incf	TMP1, F
	;; 	rgoto	MB64E_SFB_Handler_02_End

MB64E_SFB_Handler_02_End
	;; change to new bank
	movlw	0x07
	andwf	TMP1, F
	movf	MB_BANK, W
	andlw	0xf8
	iorwf	TMP1, W

	;; (reused by SF 03, switch bankstick)
MB64E_SFB_Handler_03_Cont
	call	MB64E_BANK_Change

MB64E_SFB_Handler_02_Msg
MB64E_SFB_Handler_03_Msg
	;; request fader position update
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ
	;; print message
	movlw	MB64E_MSG_ID_BANK
	movff	MB_BANK, MB64E_SELECTED_BANK
	call	MB64E_MSG_Hold
	;; and exit
	rgoto	MB64E_SFB_Handler_End

;; --------------------------------------------------------------------------
;; SF 03: Switch BankStick
MB64E_SFB_Handler_03
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End; do nothing if button is depressed

	;; branch to the end if no bankstick available
	BRA_IFCLR MB_STAT, MB_STAT_USE_BANKSTICK, ACCESS, MB64E_SFB_Handler_03_Msg

MB64E_SFB_Handler_03_Ext
	;; extract bankstick number and save in in TMP1
	swapf	MB_BANK, W
	andlw	0x07
	movwf	TMP1

	;; branch depending on mode
	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64E_SFB_Handler_03_1x
MB64E_SFB_Handler_03_00 ; SF 02 0n: switch to BankStick
	movf	MIDI_EVNT1, W
	andlw	0x07
	movwf	TMP1
	rgoto	MB64E_SFB_Handler_03_End

MB64E_SFB_Handler_03_1x ; SF 02 1x: inc/dec bankstick
	BRA_IFSET MIDI_EVNT1, 1, ACCESS, MB64E_SFB_Handler_03_12or13
MB64E_SFB_Handler_03_10or11
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_03_11

MB64E_SFB_Handler_03_10
	decf	TMP1, F		; decrement without overflow
	btfsc	TMP1, 7
	incf	TMP1, F
	rgoto	MB64E_SFB_Handler_03_End

MB64E_SFB_Handler_03_11
	decf	TMP1, F		; decrement with overflow
	rgoto	MB64E_SFB_Handler_03_End

MB64E_SFB_Handler_03_12or13
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_03_13
	
MB64E_SFB_Handler_03_12
	incf	TMP1, F		; increment without overflow
	btfsc	TMP1, 3
	decf	TMP1, F
	rgoto	MB64E_SFB_Handler_03_End

MB64E_SFB_Handler_03_13
	incf	TMP1, F		; increment with overflow
	rgoto	MB64E_SFB_Handler_03_End

MB64E_SFB_Handler_03_End
	;; change to new BankStick
	swapf	TMP1, W
	andlw	0x70
	movwf	TMP1
	movf	MB_BANK, W
	andlw	0x0f
	iorwf	TMP1, W
	rgoto	MB64E_SFB_Handler_03_Cont


;; --------------------------------------------------------------------------
;; SF 04: Save Snapshot, Switch Bank, Restore Bank
MB64E_SFB_Handler_04
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End; do nothing if button is depressed

	call	MB64E_PATCH_Save
	rgoto	MB64E_SFB_Handler_02


;; --------------------------------------------------------------------------
;; SF 08: Set Encoder Mode
MB64E_SFB_Handler_08
	BRA_IFSET MIDI_EVNT1, 6, ACCESS, MB64E_SFB_Handler_08_4x

	;; clear all speed modifier flags
	movlw	~((1 << MB_STAT2_ENC_NORMAL) | (1 << MB_STAT2_ENC_SLOW) | (1 << MB_STAT2_ENC_FAST))
	andwf	MB_STAT2, F

	BRA_IFSET MIDI_EVNT1, 4, ACCESS, MB64E_SFB_Handler_08_10
	BRA_IFSET MIDI_EVNT1, 5, ACCESS, MB64E_SFB_Handler_08_20

MB64E_SFB_Handler_08_00	; SF 08 00: Set Normal Encoder Mode
	btfss	MIDI_EVNT_VALUE, 0
	bsf	MB_STAT2, MB_STAT2_ENC_NORMAL
	movlw	0x00
	rgoto	MB64E_SFB_Handler_08_SetSpeed
MB64E_SFB_Handler_08_10	; SF 08 10: Set Fast Encoder Mode
	btfss	MIDI_EVNT_VALUE, 0
	bsf	MB_STAT2, MB_STAT2_ENC_FAST
	movlw	0x02
	rgoto	MB64E_SFB_Handler_08_SetSpeed
MB64E_SFB_Handler_08_20	; SF 08 20: Set Slow Encoder Mode
	btfss	MIDI_EVNT_VALUE, 0
	bsf	MB_STAT2, MB_STAT2_ENC_SLOW
	movlw	0x01
	;; 	rgoto	MB64E_SFB_Handler_08_SetSpeed
MB64E_SFB_Handler_08_SetSpeed
	;; don't change the encoders are muted to prevent interferences with MB64E_LEDRING_PATTERNS and MB64E_ENC_MODE_BUFFER
	BRA_IFSET MB_STAT3, MB_STAT3_MUTE_ENCODERS, ACCESS, MB64E_SFB_Handler_End

	;; restore predefined speed settings if button released
	btfsc	MIDI_EVNT_VALUE, 0
	movlw 0xff
	;; call update speed function
	call	MB64E_ENC_UpdateSpeed
	rgoto	MB64E_SFB_Handler_End

MB64E_SFB_Handler_08_4x	; SF 08 4x: Select Encoder Parameter or Group/Bank
	BRA_IFSET MIDI_EVNT1, 0, ACCESS, MB64E_SFB_Handler_08_41

MB64E_SFB_Handler_08_40	; SF 08 4x: Select Encoder Parameter
	;; switch to "select parameter" mode depending on button status
	bcf	MB_STAT, MB_STAT_SEL_PARAMETER

	;; encoder speed: switch to normal if button pressed, else to preset
	movlw	0x00
	btfsc	MIDI_EVNT_VALUE, 0
	movlw 0xff
	call	MB64E_ENC_UpdateSpeed

	;; exit if button depressed
	BRA_IFSET MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_End

	;; print message
	movlw	MB64E_MSG_ID_MAP
	call	MB64E_MSG_Hold
	;; and set "select parameter" flag
	bsf	MB_STAT, MB_STAT_SEL_PARAMETER
	rgoto	MB64E_SFB_Handler_End


MB64E_SFB_Handler_08_41	; SF 08 41: Select Bank/Group
	BRA_IFCLR MIDI_EVNT_VALUE, 0, ACCESS, MB64E_SFB_Handler_08_41_1

MB64E_SFB_Handler_08_41_0	; button depressed
	;; exit "select bank" mode
	bcf	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK

	;; change to new bank if MB64E_SELECTED_BANK different from MB_BANK
	movff	MB64E_SELECTED_BANK, WREG
	cpfseq	MB_BANK, ACCESS
	call MB64E_BANK_Change

#if DEFAULT_PATCHMODE != 0
	;; change to new patch if MB64E_SELECTED_PATCH different from MB_PATCH
	movff	MB64E_SELECTED_PATCH, WREG
	cpfseq	MB_PATCH, ACCESS
	call MB64E_PATCH_Change
#endif

	;; request fader position update
	bsf	MB_STAT3, MB_STAT3_FADER_UPDATE_REQ

	;; update group map (will also update the encoder speed)
	call	MB64E_ENC_UpdateMap

	;; and exit
	rgoto	MB64E_SFB_Handler_End

MB64E_SFB_Handler_08_41_1	; button pressed
	;; transfer current bank to selected bank
	movff	MB_BANK, MB64E_SELECTED_BANK
	;; transfer current patch to selected patch
	movff	MB_PATCH, MB64E_SELECTED_PATCH

	;; encoder speed: switch to slow mode
	movlw	0x01
	call	MB64E_ENC_UpdateSpeed

	;; print message
	movlw	MB64E_MSG_ID_BANK
	call	MB64E_MSG_Hold
	;; and set "bank and group" flag
	bsf	MB_STAT, MB_STAT_SEL_GROUP_AND_BANK
	rgoto	MB64E_SFB_Handler_End


;; --------------------------------------------------------------------------
;; unsupported or unimplemented special functions
MB64E_SFB_Handler_05
MB64E_SFB_Handler_06
MB64E_SFB_Handler_07
MB64E_SFB_Handler_09
MB64E_SFB_Handler_0A
MB64E_SFB_Handler_0B
MB64E_SFB_Handler_0C
MB64E_SFB_Handler_0D
MB64E_SFB_Handler_0E
MB64E_SFB_Handler_0F
MB64E_SFB_Handler_End
	return



