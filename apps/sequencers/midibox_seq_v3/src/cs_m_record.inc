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
	SET_BSR	SEQ_BASE
	clrf	SEQ_RECORD_POLY_CTR, BANKED
	call	SEQ_RECORD_StepUpdate

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

