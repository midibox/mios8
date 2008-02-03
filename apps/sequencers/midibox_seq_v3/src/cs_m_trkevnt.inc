; $Id$
;
; MBSEQ Menu "Track Events"
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
CS_M_TRKEVNT_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0, 4,	CS_M_TRKEVNT_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_EVNT0,		SEQ_LAYER_NUMBER_EVNT_MODES-1, 0x40+ 2, 2, CS_M_TRKEVNT_ModeChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_EVNTCONST1,		0x7f,	0x40+26, 4,	CS_M_TRKEVNT_Val1Changed
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_EVNTCONST2,		0x7f,	0x40+31, 4,	CS_M_TRKEVNT_Val2Changed
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_EVNTCONST3,		0x7f,	0x40+36, 4,	CS_M_TRKEVNT_Val3Changed
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_CHANNEL,		0x0f,	0xc0+ 1, 2,	CS_M_TRKEVNT_ChnChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_MIDI_PORT,		0x07,	0xc0+ 5, 4,	CS_M_TRKEVNT_PortChanged


;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;     o encoder incrementer in CS_MENU_ENC_INCREMENTER if encoder has been used
;;  Output:
;;     should return the GP LED values in
;;     CS_MENU_GP_OVERLAY_DOUT_SR[01]
;;     should clear CS_MENU_ENC_INCREMENTER if radio-button function desired
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; button 15/16: copy preset
	movf	MIOS_PARAMETER1, W
	andlw	0x0e
	xorlw	0x0e
	bz	CS_M_TRKEVNT_CopyPreset

	;; map button number to new cursor pos
	TABLE_ADDR CS_M_TRKEVNT_GP_CURSOR_MAP
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	TABLE_ADD_W
	tblrd*+
	movff	TABLAT, CS_MENU_CURSOR_POS

	;; notify new cursor position and exit
	goto	CS_MENU_NewCursorPos
	;; (LEDs are updated during re-initialisation)

CS_M_TRKEVNT_GP_CURSOR_MAP
	     ;; 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
	db	0, 1, 1, 1, 1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6

CS_M_TRKEVNT_GP_LED_MAP
	db	b'00000001', b'00000000'	; cursor pos = 0
	db	b'00011110', b'00000000'	; cursor pos = 1
	db	b'00100000', b'00000000'	; cursor pos = 2
	db	b'01000000', b'00000000'	; cursor pos = 3
	db	b'10000000', b'00000000'	; cursor pos = 4
	db	b'00000000', b'00000001'	; cursor pos = 5
	db	b'00000000', b'00000010'	; cursor pos = 6


	;; ---
CS_M_TRKEVNT_CopyPreset_MSGOK
	STRING 16, 0x80 + 24, " *** Preset *** "
	STRING 16, 0xc0 + 24, " *** copied *** "

CS_M_TRKEVNT_CopyPreset_MSGENC
	STRING 27, 0x80 + 13, "      Encoder disabled     "
	STRING 27, 0xc0 + 13, "  Press GP button 15 or 16!"

	;; idea: we could demand that button 15 AND 16 are presset at the same time
	;; but since the GP button assignments and states are not saved anywhere (yet), the
	;; appr. check would be difficult to realize
CS_M_TRKEVNT_CopyPreset
	;; print message if encoder has been turned (should not trigger the preset mechanism!)
	TABLE_ADDR CS_M_TRKEVNT_CopyPreset_MSGENC
	movf	CS_MENU_ENC_INCREMENTER, W
	bnz	CS_M_TRKEVNT_CopyPreset_End

	;; this function initialises all selected tracks
	call	SEQ_LAYER_Init

	TABLE_ADDR CS_M_TRKEVNT_CopyPreset_MSGOK
CS_M_TRKEVNT_CopyPreset_End
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString
	movlw	0x40
	goto	MIOS_LCD_MessageStart

;; --------------------------------------------------------------------------
CS_M_TRKEVNT_UpdateGPLEDs
	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; determine pattern depending on cursor pos
	TABLE_ADDR CS_M_TRKEVNT_GP_LED_MAP
	rlf	CS_MENU_CURSOR_POS, W
	andlw	0xfe
	TABLE_ADD_W
	tblrd*+
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR0
	tblrd*+
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_TRKEVNT_PARAMETERS	; pointer to table
	movlw	7				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKEVNT_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKEVNT_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk. Layer Assignments:xx Val1 Val2 Val3Chn. Port                     >> COPY <<
	;; 0000-11111111111111111111-66666666666666AAAAAAAAA---------------------EEEEEEEEEE
	;; G1T1 A:Chrd B:Vel. C:Len.  127  127  127 16  IIC2 (not available)     > PRESET <
	;; 2222-333333-444444-555555-7777-8888-9999--BB-CCCC-DDDDDDDDDDDDDDD-----FFFFFFFFFF

	;; Strings for LayerA/B/C and Val1/2/3 -> see seq_layer.inc

	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKEVNT_Tick_0
	rgoto	CS_M_TRKEVNT_Tick_1
	rgoto	CS_M_TRKEVNT_Tick_2
	rgoto	CS_M_TRKEVNT_Tick_3
	rgoto	CS_M_TRKEVNT_Tick_4
	rgoto	CS_M_TRKEVNT_Tick_5
	rgoto	CS_M_TRKEVNT_Tick_6
	rgoto	CS_M_TRKEVNT_Tick_7
	rgoto	CS_M_TRKEVNT_Tick_8
	rgoto	CS_M_TRKEVNT_Tick_9
	rgoto	CS_M_TRKEVNT_Tick_A
	rgoto	CS_M_TRKEVNT_Tick_B
	rgoto	CS_M_TRKEVNT_Tick_C
	rgoto	CS_M_TRKEVNT_Tick_D
	rgoto	CS_M_TRKEVNT_Tick_E
	rgoto	CS_M_TRKEVNT_Tick_F

CS_M_TRKEVNT_Tick_0_STR	STRING 4, 0x00 + 0, "Trk."
CS_M_TRKEVNT_Tick_0
	TABLE_ADDR CS_M_TRKEVNT_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_1_STR	STRING 18, 0x00 + 5, "Layer Assignments:"
CS_M_TRKEVNT_Tick_1
	TABLE_ADDR CS_M_TRKEVNT_Tick_1_STR
	call	MIOS_LCD_PrintString

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2

	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_2
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_3
	movlw	0x40 + 5
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	0		; print assignment of layer A (4 characters)
	call	SEQ_LAYER_PrnVType
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_4
	movlw	0x40 + 12
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	'B'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	1		; print assignment of layer B (4 characters)
	call	SEQ_LAYER_PrnVType
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_5
	movlw	0x40 + 19
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	2		; print assignment of layer C (4 characters)
	call	SEQ_LAYER_PrnVType
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_6
	movlw	0x00 + 26
	call	MIOS_LCD_CursorSet

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1

	movlw	0		; print name of constant #1 (4 characters)
	call	SEQ_LAYER_PrnCType
	movlw	' '		; print space
	call	MIOS_LCD_PrintChar
	movlw	1		; print name of constant #2 (4 characters)
	call	SEQ_LAYER_PrnCType
	movlw	' '		; print space
	call	MIOS_LCD_PrintChar
	movlw	2		; print name of constant #3 (4 characters)
	call	SEQ_LAYER_PrnCType
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_7
	movlw	0x40 + 26
	call	MIOS_LCD_CursorSet
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST1	; store event value in MIOS_PARAMETER2
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER2

	movlw	0		; print constant value #1
	call	SEQ_LAYER_PrnCVal
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_8
	movlw	0x40 + 31
	call	MIOS_LCD_CursorSet
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST2	; store event value in MIOS_PARAMETER2
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER2

	movlw	1		; print constant value #2
	call	SEQ_LAYER_PrnCVal
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_9
	movlw	0x40 + 36
	call	MIOS_LCD_CursorSet
	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST3	; store event value in MIOS_PARAMETER2
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER2

	movlw	2		; print constant value #3
	call	SEQ_LAYER_PrnCVal
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_A_STR	STRING 9, 0x80 + 0, "Chn. Port"
CS_M_TRKEVNT_Tick_A
	TABLE_ADDR CS_M_TRKEVNT_Tick_A_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_B
	movlw	0xc0 + 2
	call	MIOS_LCD_CursorSet
	movlw	5		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	call	CS_M_HLP_TrkCC_Get
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_C
	movlw	0xc0 + 5
	call	MIOS_LCD_CursorSet
	movlw	6		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_TRKEVNT_Tick_Cont

	movlw	SEQ_CC_OFFSET_MIDI_PORT
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_LCD_PrnPort
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_D_STR_TAB	db "(not available) "	; 16 characters expected!
				db "(available)     "	; 16 characters expected!
				db "(enabled)       "	; 16 characters expected!
CS_M_TRKEVNT_Tick_D
	movlw	0xc0 + 10	; set cursor
	call	MIOS_LCD_CursorSet

	;; check if selected interface is available
	movlw	SEQ_CC_OFFSET_MIDI_PORT
	call	CS_M_HLP_TrkCC_Get
	call	MIDI_EVNT_CheckPort
	mullw	16		; add character offset
	movlw	LOW(CS_M_TRKEVNT_Tick_D_STR_TAB)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_TRKEVNT_Tick_D_STR_TAB)
	addwfc	PRODH, W
	movwf	TBLPTRH

	movlw	16
	call	MIOS_LCD_PrintPreconfString
 	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_E_STR	STRING 10, 0x80 + 30, ">> COPY <<"
CS_M_TRKEVNT_Tick_E
	TABLE_ADDR CS_M_TRKEVNT_Tick_E_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_F_STR	STRING 10, 0xc0 + 30, "> PRESET <"
CS_M_TRKEVNT_Tick_F
	TABLE_ADDR CS_M_TRKEVNT_Tick_F_STR
	call	MIOS_LCD_PrintString
	;; 	rgoto	CS_M_TRKEVNT_Tick_Cont

CS_M_TRKEVNT_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKEVNT_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKEVNT_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKEVNT_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKEVNT_UpdateGPLEDs

CS_M_TRKEVNT_ModeChanged
	movlw	SEQ_CC_OFFSET_MIDI_EVNT0
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKEVNT_Val1Changed
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST1
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKEVNT_Val2Changed
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST2
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKEVNT_Val3Changed
	movlw	SEQ_CC_OFFSET_MIDI_EVNTCONST3
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKEVNT_ChnChanged
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKEVNT_PortChanged
	movlw	SEQ_CC_OFFSET_MIDI_PORT
	goto	CS_M_HLP_TrkCC_Set


;; ==========================================================================
