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
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0, 4,	CS_M_RECORD_GxTyChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+ 5,11,	CS_M_RECORD_ModeChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+19, 3,	CS_M_RECORD_AutoStartChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x1f,	0x40+27, 2,	CS_M_RECORD_StepChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+ 4, 8,	CS_M_RECORD_LayerAChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+18, 8,	CS_M_RECORD_LayerBChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0xc0+27, 8,	CS_M_RECORD_LayerCChanged

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
CS_M_RECORD_GP_Callback
	SET_BSR	SEQ_BASE

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_RECORD_GP_Callback_1
	rgoto	CS_M_RECORD_GP_Callback_2
	rgoto	CS_M_RECORD_GP_Callback_3
	rgoto	CS_M_RECORD_GP_Callback_4
	rgoto	CS_M_RECORD_GP_Callback_5
	rgoto	CS_M_RECORD_GP_Callback_6
	rgoto	CS_M_RECORD_GP_Callback_7
	rgoto	CS_M_RECORD_GP_Callback_8
	rgoto	CS_M_RECORD_GP_Callback_9
	rgoto	CS_M_RECORD_GP_Callback_10
	rgoto	CS_M_RECORD_GP_Callback_11
	rgoto	CS_M_RECORD_GP_Callback_12
	rgoto	CS_M_RECORD_GP_Callback_13
	rgoto	CS_M_RECORD_GP_Callback_14
	rgoto	CS_M_RECORD_GP_Callback_15
	rgoto	CS_M_RECORD_GP_Callback_16

CS_M_RECORD_GP_Callback_1
	movlw	0		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont

CS_M_RECORD_GP_Callback_2
CS_M_RECORD_GP_Callback_3
	;; toggle record mode flag
	btg	SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED

	movlw	1		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont_NP

CS_M_RECORD_GP_Callback_4
CS_M_RECORD_GP_Callback_5
	;; toggle autostart flag
	btg	SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED

	movlw	2		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont_NP

CS_M_RECORD_GP_Callback_6
	movlw	3		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont

CS_M_RECORD_GP_Callback_7
CS_M_RECORD_GP_Callback_8
	;; if encoder has been turned: modify layer value
	;; otherwise toggle gate
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_RECORD_GP_Callback_78_Tgl

CS_M_RECORD_GP_Callback_78_Enc
#if 0
	movf	MIOS_PARAMETER1, W
	movff	CS_MENU_ENC_INCREMENTER, MIOS_PARAMETER2
	call	SEQ_ENC_Handler_EditPage
	goto	SEQ_MSG_Start
#endif

CS_M_RECORD_GP_Callback_78_Tgl
	;; "toggle gate" of current step: set/clear function
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS
	call	SEQ_TRG_GateGet
	xorlw	1
	call	SEQ_TRG_GateSet
	;; print sequence on screen
	call	SEQ_MSG_Start
	rgoto	CS_M_RECORD_GP_Callback_NoEnc

CS_M_RECORD_GP_Callback_9
CS_M_RECORD_GP_Callback_10
CS_M_RECORD_GP_Callback_11
	;; toggle layer A flag
	btg	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED

	;; select Layer A
	movlw	0x01
	movwf	SEQ_SELECTED_LAYERS, BANKED

	movlw	4		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont_NP

CS_M_RECORD_GP_Callback_12
CS_M_RECORD_GP_Callback_13
	;; toggle layer B flag
	btg	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED

	;; select Layer B
	movlw	0x02
	movwf	SEQ_SELECTED_LAYERS, BANKED

	movlw	5		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont_NP

CS_M_RECORD_GP_Callback_14
CS_M_RECORD_GP_Callback_15
CS_M_RECORD_GP_Callback_16
	;; toggle layer C flag
	btg	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED

	;; select Layer C
	movlw	0x04
	movwf	SEQ_SELECTED_LAYERS, BANKED

	movlw	6		; cursor pos
	rgoto	CS_M_RECORD_GP_Callback_Cont_NP

CS_M_RECORD_GP_Callback_Cont	; toggle function
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_RECORD_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_RECORD_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

CS_M_RECORD_GP_Callback_NoEnc
	clrf	CS_MENU_ENC_INCREMENTER
	return

;; --------------------------------------------------------------------------
CS_M_RECORD_UpdateGPLEDs_TAB
	db	b'00000000', b'00000001'
	db	b'00000000', b'00000110'
	db	b'00000000', b'00011000'
	db	b'00000000', b'00100000'
	db	b'00000111', b'00000000'
	db	b'00011000', b'00000000'
	db	b'11100000', b'00000000'

CS_M_RECORD_UpdateGPLEDs
	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; get pattern depending on cursor pos
	rlf	CS_MENU_CURSOR_POS, W
	andlw	0xfe
	addlw	LOW(CS_M_RECORD_UpdateGPLEDs_TAB)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_M_RECORD_UpdateGPLEDs_TAB)
	addwfc	TBLPTRH, F

	tblrd*+
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR1
	tblrd*+
	movff	TABLAT, CS_MENU_GP_OVERLAY_DOUT_SR0

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_RECORD_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; install parameter table
	TABLE_ADDR CS_M_RECORD_PARAMETERS	; pointer to table
	movlw	7				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy selected register into menu parameter
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_RECORD_Init_Trk
	addlw	-1
	bz	CS_M_RECORD_Init_Mode
	addlw	-1
	bz	CS_M_RECORD_Init_AutoStart
	addlw	-1
	bz	CS_M_RECORD_Init_Step
	addlw	-1
	bz	CS_M_RECORD_Init_LayerA
	addlw	-1
	bz	CS_M_RECORD_Init_LayerB
	rgoto	CS_M_RECORD_Init_LayerC
CS_M_RECORD_Init_Mode
	movlw	0x00			; mode (live/step) located in SEQ_RECORD_MODE[0]
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED
	movlw 0x01
	movf	SEQ_RECORD_MODE, W	
	andlw	0x01
	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_AutoStart
	movlw	0x00			; autostat located in SEQ_RECORD_MODE[3]
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED
	movlw 0x01
	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_Step
	movf	SEQ_RECORD_STEP, W, BANKED
	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_LayerA
	movlw	0x00			; layer A located in SEQ_RECORD_MODE[4]
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED
	movlw 0x01
	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_LayerB
	movlw	0x00			; layer B located in SEQ_RECORD_MODE[5]
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED
	movlw 0x01
	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_LayerC
	movlw	0x00			; layer C located in SEQ_RECORD_MODE[6]
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED
	movlw 0x01
	rgoto	CS_M_RECORD_Init_Cont
	;; 	rgoto	CS_M_RECORD_Init_Cont
CS_M_RECORD_Init_Cont
	movwf	CS_MENU_PARAMETER_L
CS_M_RECORD_Init_Trk		; (track already copied into CS_MENU_PARAMETER_L by cursor handler)

	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_RECORD_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; init LEDs
	rgoto	CS_M_RECORD_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_RECORD_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk. Record Mode  AStart  Step  TglGate Recorded Layers (Assignm. -> Event Menu)
	;; 00000000001111111122222222222233333333334444444444555555555566666666667777777777
	;; G1T1 Realtime      on      16               R A:Chrd    R B:Vel.   R C:Len.
	;; 8888-99999999999---AAA-----AA---------------CCCCCCCC----DDDDDDDD---EEEEEEEE-----

	;; print segments 0-15
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_RECORD_Tick_Upper
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_RECORD_Tick_8
	rgoto	CS_M_RECORD_Tick_9
	rgoto	CS_M_RECORD_Tick_A
	rgoto	CS_M_RECORD_Tick_B
	rgoto	CS_M_RECORD_Tick_C
	rgoto	CS_M_RECORD_Tick_D
	rgoto	CS_M_RECORD_Tick_E
	rgoto	CS_M_RECORD_Tick_F

CS_M_RECORD_Tick_Upper_STR
	db	"Trk. Record Mode  AStart  Step  TglGate Recorded Layers (Assignm. -> Event Menu)"
CS_M_RECORD_Tick_Upper
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	TABLE_ADDR_MUL_W CS_M_RECORD_Tick_Upper_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString
CS_M_RECORD_Tick_Upper_End
	rgoto	CS_M_RECORD_Tick_Cont


CS_M_RECORD_Tick_8
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_9_TAB_LEN EQU 8
CS_M_RECORD_Tick_9_TAB
	db	"Realtime"
	db	"Step    "
CS_M_RECORD_Tick_9
	movlw	0x40 + 5
	call	MIOS_LCD_CursorSet
	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_Cont

	SET_BSR	SEQ_BASE
	movlw	0
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED
	movlw 1
	mullw	CS_M_RECORD_Tick_9_TAB_LEN
	movlw	LOW(CS_M_RECORD_Tick_9_TAB)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_RECORD_Tick_9_TAB)
	addwfc	PRODH, W
	movwf	TBLPTRH
	movlw	CS_M_RECORD_Tick_9_TAB_LEN
	call	MIOS_LCD_PrintPreconfString
	
	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_A
	movlw	0x40 + 19
	call	MIOS_LCD_CursorSet
	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_A_Cont

	SET_BSR	SEQ_BASE
	movlw	0
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED
	movlw 1
	call	SEQ_LCD_PrnOnOff

CS_M_RECORD_Tick_A_Cont
	movlw	0x40 + 27
	call	MIOS_LCD_CursorSet
	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_Cont

	movff	SEQ_RECORD_STEP, WREG
	addlw	1
	call	MIOS_LCD_PrintBCD2

	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_B
	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_C
	movlw	0xc0 + 4
	call	MIOS_LCD_CursorSet
	movlw	4		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_Cont

	SET_BSR	SEQ_BASE
	movlw	'-'
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED
	movlw 'R'
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	0		; print assignment of layer A (4 characters)
	call	SEQ_LAYER_PrnVType
	
	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_D
	movlw	0xc0 + 16
	call	MIOS_LCD_CursorSet
	movlw	5		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_Cont

	SET_BSR	SEQ_BASE
	movlw	'-'
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED
	movlw 'R'
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'B'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	1		; print assignment of layer B (4 characters)
	call	SEQ_LAYER_PrnVType

	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_E
	movlw	0xc0 + 27
	call	MIOS_LCD_CursorSet
	movlw	6		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_RECORD_Tick_Cont

	SET_BSR	SEQ_BASE
	movlw	'-'
	btfsc	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED
	movlw 'R'
	call	MIOS_LCD_PrintChar

	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'C'
	call	MIOS_LCD_PrintChar
	movlw	':'
	call	MIOS_LCD_PrintChar

	movlw	SEQ_CC_OFFSET_MIDI_EVNT0 ; store event mode in MIOS_PARAMETER1
	call	CS_M_HLP_TrkCC_Get
	movwf	MIOS_PARAMETER1
	movlw	2		; print assignment of layer C (4 characters)
	call	SEQ_LAYER_PrnVType
	
	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_F
	;; 	rgoto	CS_M_RECORD_Tick_Cont

CS_M_RECORD_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_RECORD_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_RECORD_Tick_End
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_RECORD_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_ModeChanged
	;; record mode (live/step) stored in SEQ_RECORD_MODE[0]
	SET_BSR	SEQ_BASE
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_StepChanged
	movff	CS_MENU_PARAMETER_L, SEQ_RECORD_STEP
	rcall	CS_M_RECORD_StepUpdate
	call	SEQ_MSG_Start
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_AutoStartChanged
	;; autostart (on/off) stored in SEQ_RECORD_MODE[3]
	SET_BSR	SEQ_BASE
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_LayerAChanged
	;; record layer A flag stored in SEQ_RECORD_MODE[4]
	SET_BSR	SEQ_BASE
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_A, BANKED
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_LayerBChanged
	;; record layer B flag stored in SEQ_RECORD_MODE[5]
	SET_BSR	SEQ_BASE
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_B, BANKED
	rgoto	CS_M_RECORD_UpdateGPLEDs

CS_M_RECORD_LayerCChanged
	;; record layer C flag stored in SEQ_RECORD_MODE[6]
	SET_BSR	SEQ_BASE
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_LAYER_C, BANKED
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

	;; select track and step
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; determine pointer to SEQ_TRKVAR -> FSR2
	mullw	SEQ_TRKVARRECORD_LENGTH
	lfsr	FSR2, SEQ_TRKVAR0
	movf	PRODL, W
	addwf	FSR2L, F
	movf	PRODH, W
	addwfc	FSR2H, F
	
	;; copy MIOS_PARAMETER[123] to SEQ_EVNT[123], the values are modified
	movff	MIOS_PARAMETER1, SEQ_EVNT0
	movff	MIOS_PARAMETER2, SEQ_EVNT1
	movff	MIOS_PARAMETER3, SEQ_EVNT2

	;; the length is 0x11 (75%) by default
	movlw	0x11
	movwf	SEQ_EVNTL, BANKED

	;; only continue of MIDI channel matches
	movf	SEQ_MIDI_CHANNEL, W, BANKED
	skpnz
	rgoto	CS_M_RECORD_NotifyEvent_End
	addlw	-1
	xorwf	SEQ_EVNT0, W, BANKED
	andlw	0x0f
	skpz
	rgoto	CS_M_RECORD_NotifyEvent_End

	;; branch depending on Note/CC - ignore all other events
	movf	SEQ_EVNT0, W
	andlw	0xf0
	xorlw	0xb0
	bz	CS_M_RECORD_NotifyEventChk_CC
	movf	SEQ_EVNT0, W
	andlw	0xe0
	xorlw	0x80
	bz	CS_M_RECORD_NotifyEventChk_Note
	rgoto	CS_M_RECORD_NotifyEvent_End

CS_M_RECORD_NotifyEventChk_Note
	bcf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED

	;; if we've received a note off event, convert it to 0x9x with velocity 0
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0x80
	bnz	CS_M_RECORD_NotifyEventChk_NoOff
	bsf	SEQ_EVNT0, 4, BANKED; 0x8x -> 0x9x
	clrf	SEQ_EVNT2, BANKED
CS_M_RECORD_NotifyEventChk_NoOff

	;; don't record event if velocity == 0 (Note Off)
	movf	SEQ_EVNT2, W, BANKED
	bnz	CS_M_RECORD_NotifyEventChk_Cont

CS_M_RECORD_NotifyEventChk_NOff
	;; if note number matches with last recorded EVNT1, release NOTE_ACTIVE flag
	movf	SEQ_EVNT1, W, BANKED
	cpfseq	SEQ_RECORD_CURRENT_EVNT1, BANKED
	rgoto CS_M_RECORD_NotifyEventChk_NOffS
	call	SEQ_FSR_TrkVarState2
	bcf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE
	bsf	INDF1, SEQ_TRKVARSTATE2_REC_MUTE_NXTSTP

CS_M_RECORD_NotifyEventChk_NOffS
	;; play off events and skip recording
	call	SEQ_CORE_Hlp_PlayTrkOffEvnts
	rgoto	CS_M_RECORD_NotifyEvent_Skip


CS_M_RECORD_NotifyEventChk_CC
	bsf	SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED
	;; 	rgoto	CS_M_RECORD_NotifyEventChk_Cont
CS_M_RECORD_NotifyEventChk_Cont

	;; determine step number if not in step mode
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, CS_M_RECORD_NotifyEvent_NoNewStp
	;; new step if sequencer is running
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, CS_M_RECORD_NotifyEvent_NewStep
	;; clear step number if autostart enabled
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, CS_M_RECORD_NotifyEvent_NoNewStp
CS_M_RECORD_NotifyEvent_ClrStep
	clrf	SEQ_RECORD_STEP, BANKED
	rgoto	CS_M_RECORD_NotifyEvent_ClrStpC
CS_M_RECORD_NotifyEvent_NewStep
	call	SEQ_FSR_TrkVarStep
	movff	INDF1, SEQ_RECORD_STEP
CS_M_RECORD_NotifyEvent_NoNewStp
CS_M_RECORD_NotifyEvent_ClrStpC

	;; transfer step number to SEQ_EVNTS
	movff	SEQ_RECORD_STEP, SEQ_EVNTS

	;; save current note/CC number and clear length counter
	movff	SEQ_EVNT1, SEQ_RECORD_CURRENT_EVNT1
	clrf	SEQ_RECORD_LENGTH_CTR, BANKED

	;; notify that note/CC is held
	call	SEQ_FSR_TrkVarState2
	bsf	INDF1, SEQ_TRKVARSTATE2_REC_EVNT_ACTIVE

	;; save the event in current step, loop 4 times (for event mode which store multiple events)
	clrf	SEQ_EVNT_NUMBER, BANKED
CS_M_RECORD_NotifyEvent_SaveLoop
	call	SEQ_LAYER_SaveEvnt
	incf	SEQ_EVNT_NUMBER, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER, BANKED
	rgoto CS_M_RECORD_NotifyEvent_SaveLoop

	;; step mode: increment step number
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_STEP, BANKED, CS_M_RECORD_NotifyEvent_NotStep
CS_M_RECORD_NotifyEvent_IncStep
	rcall	CS_M_RECORD_StepInc
CS_M_RECORD_NotifyEvent_NotStep

	;; if autostart enabled and sequencer not running: request start
	BRA_IFSET SEQ_STATE, SEQ_STATE_RUN, BANKED, CS_M_RECORD_NotifyEvent_NoStart
	BRA_IFCLR SEQ_RECORD_MODE, SEQ_RECORD_MODE_AUTOSTART, BANKED, CS_M_RECORD_NotifyEvent_NoStart
	bsf	SEQ_REQ, SEQ_REQ_START, BANKED
CS_M_RECORD_NotifyEvent_NoStart

	;; send stored events
	;; start event loop (each track can play up to 4 MIDI events)
	clrf	SEQ_EVNT_NUMBER_CTR, BANKED
CS_M_RECORD_NotifyEvent_SLoop
	;; get event order and store in in SEQ_EVNT_NUMBER
	movf	SEQ_EVNT_NUMBER_CTR, W, BANKED
	call	SEQ_LAYER_GetEvntPrio
	movwf	SEQ_EVNT_NUMBER, BANKED

	;; play off event (if there is still one in the queue)
	call	SEQ_CORE_Hlp_PlayOffEvnt

	;; don't play if return value is 15
	xorlw	15
	skpnz
	rgoto	CS_M_RECORD_NotifyEvent_SLoopNxt

	;; get MIDI event depending on SEQ_EVNT_NUMBER
	call	SEQ_LAYER_GetEvnt
	;; events now in SEQ_EVNT[012L]

	movf	SEQ_EVNT0, W, BANKED
	bz	CS_M_RECORD_NotifyEvent_SLoopNxt
	movwf	MIDI_EVNT0

	movf	SEQ_EVNT1, W, BANKED
	bz	CS_M_RECORD_NotifyEvent_SLoopNxt
	movwf	MIDI_EVNT1

	movff	SEQ_EVNT2, MIDI_EVNT_VALUE

	call	SEQ_FSR_TrkChn
	swapf	INDF1, W
	andlw	0x07
	movwf	MIDI_EVNT_PORT

	;; only send if received MIDI event matches with send event
	BRA_IFSET SEQ_RECORD_MODE, SEQ_RECORD_MODE_REC_CC, BANKED, CS_M_RECORD_NotifyEvent_SLoop_CC
CS_M_RECORD_NotifyEvent_SLoop_N
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xe0
	xorlw	0x80
	bnz	CS_M_RECORD_NotifyEvent_SLoopNxt	
	rgoto	CS_M_RECORD_NotifyEvent_SLoopSnd

CS_M_RECORD_NotifyEvent_SLoop_CC
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bnz	CS_M_RECORD_NotifyEvent_SLoopNxt	
	;; 	rgoto	CS_M_RECORD_NotifyEvent_SLoopSnd

CS_M_RECORD_NotifyEvent_SLoopSnd
	call	MIDI_EVNT_Send
	SET_BSR	SEQ_BASE

	;; if note (no CC): store OFF event
	movf	SEQ_EVNT0, W, BANKED
	andlw	0xf0
	xorlw	0xb0
	bz	CS_M_RECORD_NotifyEvent_SLoopNoO
CS_M_RECORD_NotifyEvent_SLoopOff
	call	SEQ_CORE_Hlp_GetTRKQUEUEP_Ptr
	movff	MIDI_EVNT_PORT, INDF1
	call	SEQ_CORE_Hlp_GetTRKQUEUE0_Ptr
	movff	SEQ_EVNT0, INDF1
	call	SEQ_CORE_Hlp_GetTRKQUEUE1_Ptr
	movff	SEQ_EVNT1, INDF1

	;; set gatelength to maximum (we are waiting for Note Off)
	movlw	SEQ_TRKVARQUEUEL_Lx
	setf	PLUSW2
	movlw	SEQ_TRKVARQUEUEL_Hx
	setf	PLUSW2
CS_M_RECORD_NotifyEvent_SLoopNoO

CS_M_RECORD_NotifyEvent_SLoopNxt
	;; loop 4 times to play all 4 MIDI events of a track
	SET_BSR	SEQ_BASE
	incf	SEQ_EVNT_NUMBER_CTR, F, BANKED
	movlw	4-1
	cpfsgt	SEQ_EVNT_NUMBER_CTR, BANKED
	rgoto CS_M_RECORD_NotifyEvent_SLoop


CS_M_RECORD_NotifyEvent_Skip
	;; update current step and select screen half
	rcall	CS_M_RECORD_StepUpdate

	;; print message
	call	SEQ_MSG_Start

CS_M_RECORD_NotifyEvent_End
	return

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;;  CURRENTLY DEAD CODE! (never called)
;; --------------------------------------------------------------------------
CS_M_RECORD_Select
	;; start message if button depressed
	GOTO_IFSET MIOS_PARAMETER2, 0, ACCESS, SEQ_MSG_Start

	;; decrement step
	decf	CS_MENU_PARAMETER_L, W
	andlw	0x0f
	movwf	CS_MENU_PARAMETER_L
	call	CS_M_RECORD_StepChanged

	;; delete event
	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED
	movff	SEQ_RECORD_STEP, SEQ_EVNTS
	movlw	0x3c
	movwf	SEQ_EVNT1, BANKED
	movlw	0x7f
	movwf	SEQ_EVNT2, BANKED
	movlw	0x11
	movwf	SEQ_EVNTL, BANKED
	call	SEQ_LAYER_SaveEvnt

	;; mute step
	call	SEQ_FSR_TrkTrgA_0
	rlf	SEQ_EVNTS, W, BANKED	; add step offset
	swapf	WREG, W
	andlw	0x03
	addwf	FSR1L, F

	SET_BSR	SEQ_BASE
	movf	SEQ_EVNTS, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F

	;; hold message and exit
	goto	SEQ_MSG_Hold


;; --------------------------------------------------------------------------
;;  This function increments the record step
;; --------------------------------------------------------------------------
CS_M_RECORD_StepInc
	SET_BSR	SEQ_BASE

	;; increment step number
	incf	SEQ_RECORD_STEP, F, BANKED
	;; wrap on overrun
	call	SEQ_FSR_TrkLen
	movf	INDF1, W
	cpfsgt	SEQ_RECORD_STEP, BANKED
	rgoto CS_M_RECORD_StepInc_NoOv
	clrf	SEQ_RECORD_STEP, BANKED
CS_M_RECORD_StepInc_NoOv
	rgoto	CS_M_RECORD_StepUpdate

;; --------------------------------------------------------------------------
;;  This function updates the active step value + the 1-16/17-32 screen
;; --------------------------------------------------------------------------
CS_M_RECORD_StepUpdate
	;; set new current step
	movff	SEQ_RECORD_STEP, SEQ_CURRENT_STEP
	
	;; select step 1-16/17-32
	SET_BSR	SEQ_BASE
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	btfsc	SEQ_CURRENT_STEP, 4, BANKED
	bsf	SEQ_SELECTED_STEPS, 0, BANKED

	return

