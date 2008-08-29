; $Id$
;
; MBSEQ Menu "Song"
;
; Activate this #define to measure the performance with a scope
; (connect the probe to RC.5)
#define SONG_MENU_MEASURE_PERFORMANCE 0
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
CS_M_SONG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 IF SEQ_SBANK_BS_CS_MODE == 4			; constant defined via EQU, therefore no #if, but IF
   CS_MENU_PARAMETER_ENTRY -1,					0x1f,	0x40+ 1, 3,	CS_M_SONG_ValueChanged
 ELSE
   CS_MENU_PARAMETER_ENTRY -1,					0x0f,	0x40+ 1, 3,	CS_M_SONG_ValueChanged
 ENDIF
 CS_MENU_PARAMETER_ENTRY -1,					0x7f,	0x40+ 6, 3,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x13,	0x40+10, 5,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+16, 4,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+21, 4,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+26, 4,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+31, 4,	CS_M_SONG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x01,	0x40+36, 3,	CS_M_SONG_SelChanged


;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_SONG_Select
	;; ignore if no BankStick for songmode
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

	;; check if current value has to be saved in BankStick before incrementing cursor
	rcall	CS_M_SONG_WriteValueChk

	;; increment cursor
	goto	CS_MENU_IncCursorPos


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
CS_M_SONG_GP_Callback
	;; ignore if no BankStick for songmode
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; button or encoder?
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_SONG_GP_Callback_Btn
CS_M_SONG_GP_Callback_Enc
	;; ignore if button number >= 8
	movlw	8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto CS_M_SONG_GP_Callback_Enc_Ignore
CS_M_SONG_GP_Callback_Enc_Ok
	;; do nothing if cursor position already set
	movf	MIOS_PARAMETER1, W
	xorwf	CS_MENU_CURSOR_POS, W
	skpnz
	return
	
	;; check if current value has to be saved in BankStick before incrementing cursor
	rcall	CS_M_SONG_WriteValueChk

	clrf	CS_MENU_ENC_INCREMENTER	; ignore first turn (CS_MENU_PARAMETER_L and max value not up-to-date yet)
	;; set new cursor position and exit
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos

CS_M_SONG_GP_Callback_Enc_Ignore
	clrf	CS_MENU_ENC_INCREMENTER
	return


CS_M_SONG_GP_Callback_Btn
	SET_BSR	SEQ_BASE
	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_GP_Callback_Song
	addlw	-7
	bz	CS_M_SONG_GP_Callback_Sel
	addlw	+7-1
	bz	CS_M_SONG_GP_Callback_Pos
	addlw	-1
	bz	CS_M_SONG_GP_Callback_Loop
	;; else call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel
	;; save value if one of the right buttons has been pressed
	RCALL_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_SONG_ValueChanged
	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set song number
CS_M_SONG_GP_Callback_Song
	movlw	0xf0		; only modify bit 3:0, so that we stay in the 1-16/17-32 group
	andwf	CS_MENU_PARAMETER_L, F
	movf	MIOS_PARAMETER1, W
	iorwf	CS_MENU_PARAMETER_L, F
	rcall	CS_M_SONG_ValueChanged
	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set song pos (button * 8)
CS_M_SONG_GP_Callback_Pos
	movf	MIOS_PARAMETER1, W
	mullw	8
	movff	PRODL, CS_MENU_PARAMETER_L
	rcall	CS_M_SONG_ValueChanged

#if 0
	;; if sequencer not running, start it now:
	CALL_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, SEQ_CORE_Start
#endif

	;; request pattern change
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos

	rgoto	CS_M_SONG_GP_Callback_Cont

	;; set loop counter
CS_M_SONG_GP_Callback_Loop
	movlw	0x10
	cpfsgt	SEQ_EDIT_SONG_ACTN, BANKED
	rgoto CS_M_SONG_GP_Callback_LoopX
CS_M_SONG_GP_Callback_LoopJ
	btfss	MIOS_PARAMETER1, 3; do nothing if button 1-8 is pressed
	return	
	movf	CS_MENU_PARAMETER_L, W
	addlw	-0x11
	andlw	0xf8
	iorwf	MIOS_PARAMETER1, W
	addlw	0x11
	rgoto	CS_M_SONG_GP_Callback_LoopCont

CS_M_SONG_GP_Callback_LoopX
	incf	MIOS_PARAMETER1, W
CS_M_SONG_GP_Callback_LoopCont
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_SONG_ValueChanged
	rgoto	CS_M_SONG_GP_Callback_Cont

	;; toggle pat/bnk sel
CS_M_SONG_GP_Callback_Sel
	;; 	btg	CS_STAT, CS_STAT_SONG_EDIT_BNK
	;; disable - somehow makes no sense here (UI not consistent - time to ask the community, which approach they would prefer)
	;; 	rgoto	CS_M_SONG_GP_Callback_Cont

CS_M_SONG_GP_Callback_Cont
	;; update LEDs
	
CS_M_SONG_UpdateGPLEDs
	SET_BSR	SEQ_BASE

	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; branch depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_UpdateGPLEDs_Song
	addlw	-7
	bz	CS_M_SONG_UpdateGPLEDs_Sel
	addlw	+7-1
	bz	CS_M_SONG_UpdateGPLEDs_Pos
	addlw	-1
	bz	CS_M_SONG_UpdateGPLEDs_Loop
	;; else branch to LED function of CS_M_PATTERN (re-use)
	goto	CS_M_PATTERN_UpdateGPLEDs

CS_M_SONG_UpdateGPLEDs_Song
	movf	SEQ_SONG, W, BANKED
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Pos
	rrf	SEQ_EDIT_SONG_POS, W, BANKED
	rrf	WREG, W
	rrf	WREG, W
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Sel
	movlw	b'10000000'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	return
	
CS_M_SONG_UpdateGPLEDs_Loop
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	skpnz
	return
	movlw	0x10
	cpfsgt	SEQ_EDIT_SONG_ACTN, BANKED
	rgoto CS_M_SONG_UpdateGPLEDs_LoopX
CS_M_SONG_UpdateGPLEDs_LoopJ
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	addlw	-0x11
	andlw	0x07
	iorlw	0x08
	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_LoopX
	decf	SEQ_EDIT_SONG_ACTN, W, BANKED
	;; 	rgoto	CS_M_SONG_UpdateGPLEDs_Cont

CS_M_SONG_UpdateGPLEDs_Cont
	movwf	TMP1
	call	MIOS_HLP_GetBitORMask
	btfss	TMP1, 3
	movwf CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	TMP1, 3
	movwf CS_MENU_GP_OVERLAY_DOUT_SR1
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_SONG_Init
#if SONG_MENU_MEASURE_PERFORMANCE
	bsf	LATC, 5
#endif
	;; print static string if no bankstick for songmode
	BRA_IFSET MB_STAT, MB_STAT_USE_S_BANKSTICK, ACCESS, CS_M_SONG_Init_BA
CS_M_SONG_Init_BNA
	call	SEQ_LCD_Clear

	call	SEQ_LCD_PrintStringAfterCall
	STRING 16, 0x00, "* No BankStick *"
	call	SEQ_LCD_PrintStringAfterCall
	STRING 16, 0x40, "* for song :-/ *"
	return
CS_M_SONG_Init_BA


	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; read record of current selected song/position
	rcall	CS_M_SONG_ReadSongRecord

	;; install parameter table
	TABLE_ADDR CS_M_SONG_PARAMETERS	; pointer to table
	movlw	8			; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy selected register into menu parameter
	lfsr	FSR0, SEQ_SONG
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_Init_Song
	addlw	-7
	bz	CS_M_SONG_Init_Sel
	addlw	+7-1
	bz	CS_M_SONG_Init_Pos
	addlw	-1
	bnz	CS_M_SONG_Init_NotP
CS_M_SONG_Init_Song
CS_M_SONG_Init_Play
CS_M_SONG_Init_Pos
	movf	CS_MENU_CURSOR_POS, W
	movf	PLUSW0, W
	andlw	0x7f
	rgoto	CS_M_SONG_Init_P_Cont

CS_M_SONG_Init_Sel
	movlw	0x00
	btfsc	CS_STAT, CS_STAT_SONG_EDIT_BNK
	movlw	0x01
	rgoto	CS_M_SONG_Init_P_Cont

CS_M_SONG_Init_NotP
	SET_BSR	SEQ_BASE	; if loop action not selected: always change G0 parameter
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	addlw	-SEQ_SONG_ACTN_MODE_LOOPS
	bz	CS_M_SONG_Init_NotPLoops

	;; MAX value = 16/32 if songs edited (not 0x0f, CS_M_SONG_ValueChanged decrements -1)
	addlw	-SEQ_SONG_ACTN_MODE_JMP_SONG+SEQ_SONG_ACTN_MODE_LOOPS
	bnz	CS_M_SONG_Init_NotP_NotJS
 IF SEQ_SBANK_BS_CS_MODE == 4			; constant defined via EQU, therefore no #if, but IF
	movlw	32
 ELSE
	movlw	16
 ENDIF
	movwf	CS_MENU_PARAMETER_MAX_L
CS_M_SONG_Init_NotP_NotJS
	movlw	3
	rgoto	CS_M_SONG_Init_NotPLoopsC
CS_M_SONG_Init_NotPLoops

	;; MAX value = 8 if banks edited (not 0x07, CS_M_SONG_ValueChanged decrements -1)
	movlw	8
	btfsc	CS_STAT, CS_STAT_SONG_EDIT_BNK
	movwf	CS_MENU_PARAMETER_MAX_L

	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	movf	CS_MENU_CURSOR_POS, W
	btfsc	CS_STAT, CS_STAT_SONG_EDIT_BNK; (bank should be edited)
	addlw 4	
CS_M_SONG_Init_NotPLoopsC
	BRA_IFSET PLUSW0, 7, ACCESS, CS_M_SONG_Init_PMuted
CS_M_SONG_Init_PNotMuted
	incf	PLUSW0, W
	rgoto	CS_M_SONG_Init_P_Cont
CS_M_SONG_Init_PMuted
	movlw	0x00
CS_M_SONG_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as bank select buttons here
	TABLE_ADDR CS_M_SONG_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_SONG_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_SONG_Tick
	;; ignore if no BankStick for songmode
	btfss	MB_STAT, MB_STAT_USE_S_BANKSTICK
	return

#if SONG_MENU_MEASURE_PERFORMANCE
	bsf	LATC, 5
#endif

	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Song  Pos  Actn  G1   G2   G3   G4  Sel. Song Mode   S#xx  Pos xx.xx  Loop xx/xx
	;; 00000000001111111111222222222233333333334444444444455555555556666666666777777777
	;;   1    A1  Stop 1:A1 1:C1 1:E1 1:G1 Pat  1:A1 ____ 1:C1 ____ 1:E1 ____ 1:G1 ____
	;; -888---99-AAAAA-BBBB-CCCC-DDDD-EEEEEEEEE22222AAAA-33333BBBB-44444CCCC-55555DDDD-

	;; Song Phrase      G1   G2   G3   G4  Sel.Phrase Mode  S#xx  Pos xx.xxx
	;; 00000000001111111111222222222233333333334444444444455555555556666666666777777777

	;; if fast display update, and counter < 0x18, set it to 0x18
	BRA_IFCLR CS_STAT2, CS_STAT2_FAST_DISPLAY_UPDATE, ACCESS, CS_M_SONG_Tick_NotFast
CS_M_SONG_Tick_Fast
	movlw	0x18
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_SONG_Tick_NotCtrlFix
	movwf	CS_MENU_REFRESH_ITEM_CTR
CS_M_SONG_Tick_NotCtrlFix
CS_M_SONG_Tick_NotFast

	;; print segments 0-7? (string)
	movlw	7
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_SONG_Tick_Upper
	
	;; print segments 8-31
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x1f
	addlw	-8
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_SONG_Tick_08
	rgoto	CS_M_SONG_Tick_09
	rgoto	CS_M_SONG_Tick_0A
	rgoto	CS_M_SONG_Tick_0B
	rgoto	CS_M_SONG_Tick_0C
	rgoto	CS_M_SONG_Tick_0D
	rgoto	CS_M_SONG_Tick_0E
	rgoto	CS_M_SONG_Tick_0F
	rgoto	CS_M_SONG_Tick_10
	rgoto	CS_M_SONG_Tick_11
	rgoto	CS_M_SONG_Tick_12
	rgoto	CS_M_SONG_Tick_13
	rgoto	CS_M_SONG_Tick_14
	rgoto	CS_M_SONG_Tick_15
	rgoto	CS_M_SONG_Tick_16
	rgoto	CS_M_SONG_Tick_17
	rgoto	CS_M_SONG_Tick_18
	rgoto	CS_M_SONG_Tick_19
	rgoto	CS_M_SONG_Tick_1A
	rgoto	CS_M_SONG_Tick_1B
	rgoto	CS_M_SONG_Tick_1C
	rgoto	CS_M_SONG_Tick_1D
	rgoto	CS_M_SONG_Tick_1E
	rgoto	CS_M_SONG_Tick_1F

CS_M_SONG_Tick_Upper_STR_S_L0	; Stop
	db	"Song  Pos  Actn                         "
CS_M_SONG_Tick_Upper_STR_S_L1	; Loop
	db	"Song  Pos  Actn  G1   G2   G3   G4  Sel."
CS_M_SONG_Tick_Upper_STR_S_L2	; Jump Position
	db	"Song  Pos  Actn  ->  Pos.               "
CS_M_SONG_Tick_Upper_STR_S_L3	; Jump Song   
	db	"Song  Pos  Actn  ->  Song               "
CS_M_SONG_Tick_Upper_STR_S_L4	; Mixer Map   
	db	"Song  Pos  Actn  ->  Map                "

CS_M_SONG_Tick_Upper_STR_L_R
	db	"Phrase Mode  S#    Pos   .              "

CS_M_SONG_Tick_Upper_STR_S_R
	db	" Song Mode   S#    Pos   .    Loop   /  "

CS_M_SONG_Tick_Upper
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x80
	call	MIOS_LCD_CursorSet

	;; determine string offset - use different one if in phrase mode!
	SET_BSR	SEQ_BASE
	movf	CS_MENU_REFRESH_ITEM_CTR, W

	BRA_IFSET WREG, 2, ACCESS, CS_M_SONG_Tick_Upper_R
CS_M_SONG_Tick_Upper_S_L
	andlw	0x03
	movwf	TMP1
	;; get action mode to select string
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xfc
	iorwf	TMP1, W
	TABLE_ADDR_MUL_W CS_M_SONG_Tick_Upper_STR_S_L0, 10
	rgoto	CS_M_SONG_Tick_Upper_P_Cont

CS_M_SONG_Tick_Upper_R
	andlw	0x03
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_Tick_Upper_R_S
CS_M_SONG_Tick_Upper_R_L
	TABLE_ADDR_MUL_W CS_M_SONG_Tick_Upper_STR_L_R, 10
	rgoto	CS_M_SONG_Tick_Upper_P_Cont
CS_M_SONG_Tick_Upper_R_S
	TABLE_ADDR_MUL_W CS_M_SONG_Tick_Upper_STR_S_R, 10
	rgoto	CS_M_SONG_Tick_Upper_P_Cont
CS_M_SONG_Tick_Upper_P_Cont

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	rgoto	CS_M_SONG_Tick_Cont



CS_M_SONG_Tick_08
	movlw	0x40 + 1
	call	MIOS_LCD_CursorSet
	movlw	0 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	;; print song number
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG, W, BANKED
	andlw	0x7f
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_09
	movlw	0x40 + 7
	call	MIOS_LCD_CursorSet
	movlw	1 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	;; print song position
#if 0
	SET_BSR	SEQ_BASE
	incf	SEQ_EDIT_SONG_POS, W, BANKED
	call	MIOS_LCD_PrintBCD3
#else
	;; new naming style
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_LCD_PrnSongPos
#endif
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0A
	movlw	0x40 + 10
	call	MIOS_LCD_CursorSet
	movlw	2 		; item number
	call	CS_MENU_CursorHandler2
	bnz	CS_M_SONG_Tick_0A_Cont
	rgoto	CS_M_SONG_Tick_Cont


CS_M_SONG_Tick_0A_STR
	db	" Stop "
	db	" x xx "	; will be handled extra (number of loops will be displayed)
	db	" Jump "
	db	" Jump "
	db	"Mixer "
CS_M_SONG_Tick_0A_Cont
	;; get action mode to select string
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	addlw	-SEQ_SONG_ACTN_MODE_LOOPS ; will be handled extra (number of loops will be displayed)
	bz	CS_M_SONG_Tick_0A_Loops
	addlw	+SEQ_SONG_ACTN_MODE_LOOPS
	TABLE_ADDR_MUL_W CS_M_SONG_Tick_0A_STR, 6
	movlw	5
	call	MIOS_LCD_PrintPreconfString
	rgoto	CS_M_SONG_Tick_0A_End

CS_M_SONG_Tick_0A_Loops
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'x'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	' '
	call	MIOS_LCD_PrintChar
CS_M_SONG_Tick_0A_End
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0B
CS_M_SONG_Tick_0C
CS_M_SONG_Tick_0D
CS_M_SONG_Tick_0E
	;; 4 items, calculate index from refresh counter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-0xb
	movwf	TMP5
	mullw	5

	;; set cursor pos
	movlw	0x40 + 16
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; branch depending on action mode
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	andlw	0xff		; fix ZERO flag
	bz	CS_M_SONG_Tick_0E_0
	addlw	-1
	bz	CS_M_SONG_Tick_0E_1
	addlw	-1
	bz	CS_M_SONG_Tick_0E_2
	addlw	-1
	bz	CS_M_SONG_Tick_0E_3
	addlw	-1
	bz	CS_M_SONG_Tick_0E_4

CS_M_SONG_Tick_0E_0		; Stop - print 4 spaces
	movlw	4
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0E_2		; Jump Pos
	;; (only on second position)
	decf	TMP5, W
	bnz	CS_M_SONG_Tick_0E_0

	movlw	3 + 1 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	movlw	' '
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_G0, W, BANKED
	andlw	0x7f
	call	SEQ_LCD_PrnSongPos
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0E_3		; Jump Song
	;; (only on second position)
	decf	TMP5, W
	bnz	CS_M_SONG_Tick_0E_0

	movlw	3 + 1 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_G0, W, BANKED
 IF SEQ_SBANK_BS_CS_MODE == 4			; constant defined via EQU, therefore no #if, but IF
	andlw	0x1f
 ELSE
	andlw	0x0f
 ENDIF
	addlw	1
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0E_4		; Select Mixer Map
	;; (only on second position)
	decf	TMP5, W
	bnz	CS_M_SONG_Tick_0E_0

	movlw	3 + 1 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_G0, W, BANKED
	andlw	0x7f
	addlw	1
	call	MIOS_LCD_PrintBCD3
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0E_1		; Loop - print banks and patterns
	movlw	3 		; item number
	addwf	TMP5, W
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	;; saved bank number -> TMP3
	lfsr	FSR0, SEQ_EDIT_SONG_BANK_G0
	movf	TMP5, W
	movf	PLUSW0, W
	andlw	0x07
	movwf	TMP3

	;; saved pattern number -> TMP4
	lfsr	FSR0, SEQ_EDIT_SONG_G0
	movf	TMP5, W
	movff	PLUSW0, TMP4

	;; if TMP5 == CURSOR_POS, and no bank edited, write converted CS_MENU_PARAMETER_L to TMP4
	BRA_IFSET CS_STAT, CS_STAT_SONG_EDIT_BNK, ACCESS, CS_M_SONG_Tick_0B_NMP
	movf	TMP5, W
	addlw	3
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_Tick_0B_NMP
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP4
CS_M_SONG_Tick_0B_NMP

	;; print bank
	incf	TMP3, W
	call	MIOS_LCD_PrintBCD1
	movlw	':'
	call	MIOS_LCD_PrintChar

	;; print pattern
	BRA_IFCLR TMP4, 7, ACCESS, CS_M_SONG_Tick_0B_NM
CS_M_SONG_Tick_0B_M
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_0B_C
CS_M_SONG_Tick_0B_NM
	movf	TMP4, W
	call	SEQ_LCD_PrnPattern
CS_M_SONG_Tick_0B_C
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_0F
	;; if cursor line is disabled: check if value selected has to be saved
	RCALL_IFCLR CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_SONG_WriteValueChk
 	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_10
	movlw	0x80 + 15
	call	MIOS_LCD_CursorSet
	
	;; song number
	SET_BSR	SEQ_BASE
	incf	SEQ_SONG, W, BANKED
	andlw	0x7f
	call	MIOS_LCD_PrintBCD2

	rgoto	CS_M_SONG_Tick_Cont


CS_M_SONG_Tick_11
	;; init vertical bars (if required - function breaks if charset already uploaded)
	call	CS_SPECIAL_CHARS_Init
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_12
CS_M_SONG_Tick_13
CS_M_SONG_Tick_14
CS_M_SONG_Tick_15
	;; 4 items, calculate index from refresh counter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-0x12
	movwf	TMP5
	mullw	10

	;; set cursor pos
	movlw	0xc0 + 0
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet

	movlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	':'
	call	MIOS_LCD_PrintChar

	;; saved pattern number -> TMP1
	lfsr	FSR1, SEQ_PATTERN0
	movf	TMP5, W
	movf	PLUSW1, W
	movwf	TMP1

	;; print pattern
	call	SEQ_LCD_PrnPattern

	;; print "*" if pattern synched to measure
	BRA_IFSET TMP1, 7, ACCESS, CS_M_SONG_Tick_12_NoS
CS_M_SONG_Tick_12_S
	SET_BSR	SEQ_BASE
	movf	TMP5, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, W, BANKED
	bz	CS_M_SONG_Tick_12_NoS
	movlw	'*'
	rgoto	CS_M_SONG_Tick_12_Cont
CS_M_SONG_Tick_12_NoS
	movlw	' '
CS_M_SONG_Tick_12_Cont
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_16
	movlw	0x40 + 36
	call	MIOS_LCD_CursorSet
	movlw	7 		; item number
	call	CS_MENU_CursorHandler2
	skpnz
	rgoto	CS_M_SONG_Tick_Cont

	;; print spaces if SEQ_SONG_ACTN_MODE_LOOPS not selected
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	addlw	-SEQ_SONG_ACTN_MODE_LOOPS
	bz	CS_M_SONG_Tick_16_Loops
CS_M_SONG_Tick_16_NoLoops
	movlw	3
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_16_Loops
	BRA_IFSET CS_STAT, CS_STAT_SONG_EDIT_BNK, ACCESS, CS_M_SONG_Tick_16_Bnk
CS_M_SONG_Tick_16_Pat
	movlw	'P'
	call	MIOS_LCD_PrintChar
	movlw	'a'
	call	MIOS_LCD_PrintChar
	movlw	't'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont
CS_M_SONG_Tick_16_Bnk
	movlw	'B'
	call	MIOS_LCD_PrintChar
	movlw	'n'
	call	MIOS_LCD_PrintChar
	movlw	'k'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_17
	;; nothing to do here
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_18
	movlw	0x80 + 23
	call	MIOS_LCD_CursorSet
	
	;; song position
	SET_BSR	SEQ_BASE
	movf	SEQ_SONG_POS, W, BANKED
	call	SEQ_LCD_PrnSongPos

	movlw	'.'
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, CS_M_SONG_Tick_18_SongNR
CS_M_SONG_Tick_18_SongR
	movf	SEQ_CLK_STEP_CTR, W, BANKED
	call	SEQ_LCD_PrnDecP1
	rgoto	CS_M_SONG_Tick_18_SongC
CS_M_SONG_Tick_18_SongNR
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
CS_M_SONG_Tick_18_SongC
	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_19
	movlw	0x80 + 35
	call	MIOS_LCD_CursorSet

	;; don't print in phrase mode
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_SONG, 7, BANKED, CS_M_SONG_Tick_Cont

	;; loop counter
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_STATE, SEQ_STATE_RUN, BANKED, CS_M_SONG_Tick_19_SongNR
CS_M_SONG_Tick_19_SongR
	movf	SEQ_SONG_LOOP, W, BANKED
	subwf	SEQ_SONG_LOOP_MAX, W, BANKED
	addlw	1
	call	MIOS_LCD_PrintBCD2
	rgoto	CS_M_SONG_Tick_19_SongC
CS_M_SONG_Tick_19_SongNR
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
CS_M_SONG_Tick_19_SongC


	movlw	'/'
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	movf	SEQ_SONG_LOOP_MAX, W, BANKED
	addlw	1
	call	MIOS_LCD_PrintBCD2

	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_1A
CS_M_SONG_Tick_1B
CS_M_SONG_Tick_1C
CS_M_SONG_Tick_1D
	;; 4 items, calculate index from refresh counter
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-0x1a
	movwf	TMP5
	mullw	10

	;; set cursor pos
	movlw	0xc0 + 5
	addwf	PRODL, W
	call	MIOS_LCD_CursorSet

	;; loop 4 times
	movlw	4		; loop counter -> TMP1
	movwf	TMP1
	
	rlf	TMP5, W		; track offset -> TMP2
	rlf	WREG, W
	andlw	0x0c
	movwf	TMP2
CS_M_SONG_Tick_1A_Loop
	;; print "M" if track is muted, else vertical meter for track status
	SET_BSR	SEQ_BASE
	movf	SEQ_TRKS_MUTED0, W, BANKED
	btfsc	TMP2, 3
	movf	SEQ_TRKS_MUTED1, W, BANKED
	movwf	TMP3
	movf	TMP2, W
	call	MIOS_HLP_GetBitORMask
	andwf	TMP3, W
	bz	CS_M_SONG_Tick_1A_NotMuted
CS_M_SONG_Tick_1A_Muted
	movlw	'M'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_SONG_Tick_1A_LoopNext

CS_M_SONG_Tick_1A_NotMuted
	lfsr	FSR1, CS_MENU_METER_CTR_BEGIN
	movf	TMP2, W
	movf	PLUSW1, W
	call	SEQ_LCD_PrnVBar
CS_M_SONG_Tick_1A_LoopNext
	incf	TMP2, F
	decfsz	TMP1, F
	rgoto	CS_M_SONG_Tick_1A_Loop

	rgoto	CS_M_SONG_Tick_Cont

	
CS_M_SONG_Tick_1E
CS_M_SONG_Tick_1F
	;; 	rgoto	CS_M_SONG_Tick_Cont

CS_M_SONG_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 5, ACCESS, CS_M_SONG_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_0End

CS_M_SONG_Tick_End
#if SONG_MENU_MEASURE_PERFORMANCE
	bcf	LATC, 5
#endif
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_SONG_ValueChanged
	;; copy menu parameter to SEQ_EDIT_SONG*
	movf	CS_MENU_CURSOR_POS, W
	movwf	TMP2		; TMP2 holds the parameter index which should be modified
	bz	CS_M_SONG_ValueChanged_Song
	decf	CS_MENU_CURSOR_POS, W
	bz	CS_M_SONG_ValueChanged_Pos
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NotP
CS_M_SONG_ValueChanged_Pos
	movff	CS_MENU_PARAMETER_L, TMP1
	rgoto	CS_M_SONG_ValueChanged_Cont
CS_M_SONG_ValueChanged_Song
	movff	CS_MENU_PARAMETER_L, TMP1
	SET_BSR	SEQ_BASE
	btfsc	SEQ_SONG, 7, BANKED	; copy "song disabled" bit
	bsf	TMP1, 7
	rgoto	CS_M_SONG_ValueChanged_Cont

CS_M_SONG_ValueChanged_NotP
	SET_BSR	SEQ_BASE	; if loop action not selected: always change G0 parameter!
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	addlw	-SEQ_SONG_ACTN_MODE_LOOPS
	bz	CS_M_SONG_ValueChanged_NotPLoops
	movlw	3
	movwf	TMP2		; force parameter index to 3
	rgoto	CS_M_SONG_ValueChanged_NotP_Cont
	
CS_M_SONG_ValueChanged_NotPLoops
	;; if bank selected, add offset of 4
	movf	TMP2, W
	addlw	4
	btfsc	CS_STAT, CS_STAT_SONG_EDIT_BNK; (bank should be edited)
	movwf TMP2	

CS_M_SONG_ValueChanged_NotP_Cont
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

CS_M_SONG_ValueChanged_Cont
	SET_BSR	SEQ_BASE
	lfsr	FSR0, SEQ_SONG
	movf	TMP2, W		; holds the parameter index (normaly CS_MENU_CURSOR_POS)
	movff	TMP1, PLUSW0

	;; if CURSOR_POS == 0 or 1 and not song mode, change pattern immediately
	bz	CS_M_SONG_ValueChanged_ChkSng1
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NoChkSng1
CS_M_SONG_ValueChanged_ChkSng1
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_ValueChanged_ChkSngEnd
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos
	SET_BSR	SEQ_BASE
	rgoto	CS_M_SONG_ValueChanged_ChkSngEnd

CS_M_SONG_ValueChanged_NoChkSng1

	;; ignore CURSOR_POS == 2 and not song mode
	addlw	-1
	bnz	CS_M_SONG_ValueChanged_NoChkSng2
CS_M_SONG_ValueChanged_ChkSng2
	btfsc	SEQ_SONG, 7, BANKED
	return
CS_M_SONG_ValueChanged_NoChkSng2
CS_M_SONG_ValueChanged_ChkSngEnd

	;; if CURSOR_POS >=3 and not in play mode, activate it
	movlw	0x03-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_SONG_ValueChanged_NoChkPly
CS_M_SONG_ValueChanged_ChkPly
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	bnz	CS_M_SONG_ValueChanged_NoChkPly
	movlw	0x01
	movwf	SEQ_EDIT_SONG_ACTN, BANKED

	;; save value in EEPROM
	movlw	SEQ_SBANK_SECTOR_ACTN
	movwf	SEQ_SBANK_SECTOR, BANKED
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SBANK_Write
CS_M_SONG_ValueChanged_NoChkPly

	;; if CURSOR_POS <= 1: read record of current selected song/position
	movlw	0x01
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rcall CS_M_SONG_ReadSongRecord

	;; update LEDs
	rgoto	CS_M_SONG_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Toggles the Pattern/Bank Selection
;; --------------------------------------------------------------------------
CS_M_SONG_SelChanged
	bcf	CS_STAT, CS_STAT_SONG_EDIT_BNK
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	CS_STAT, CS_STAT_SONG_EDIT_BNK
	return


;; ==========================================================================

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  function to read a song record
;; --------------------------------------------------------------------------
CS_M_SONG_ReadSongRecord
	SET_BSR	SEQ_BASE
	movlw	SEQ_SBANK_SECTOR_ACTN
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_ACTN, BANKED

	movlw	SEQ_SBANK_SECTOR_G0
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_G0, BANKED

	movlw	SEQ_SBANK_SECTOR_G1
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_G1, BANKED

	movlw	SEQ_SBANK_SECTOR_G2
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_G2, BANKED

	movlw	SEQ_SBANK_SECTOR_G3
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_EDIT_SONG_G3, BANKED

	movlw	SEQ_SBANK_SECTOR_BNK_G01
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	PRODL
	andlw	0x0f
	movwf	SEQ_EDIT_SONG_BANK_G0, BANKED
	swapf	PRODL, W
	andlw	0x0f
	movwf	SEQ_EDIT_SONG_BANK_G1, BANKED

	movlw	SEQ_SBANK_SECTOR_BNK_G23
	rcall	CS_M_SONG_ReadSongRecord_Hlp
	movwf	PRODL
	andlw	0x0f
	movwf	SEQ_EDIT_SONG_BANK_G2, BANKED
	swapf	PRODL, W
	andlw	0x0f
	movwf	SEQ_EDIT_SONG_BANK_G3, BANKED
	return


CS_M_SONG_ReadSongRecord_Hlp
	movwf	SEQ_SBANK_SECTOR, BANKED
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	goto	SEQ_SBANK_Read


;; --------------------------------------------------------------------------
;;  this function checks if the currently selected value has to be saved in BankStick
;;  ATTENTION: this function is also called by SEQ_BUTTON* when a new menu position is selected directly by 1/2/3/4/A/B/C button
;; --------------------------------------------------------------------------
CS_M_SONG_WriteValueChk
	;; we have to save MIOS_PARAMETER[12] here since it is used by the button routines
	movff	MIOS_PARAMETER1, TMP1
	movff	MIOS_PARAMETER2, TMP2

	;; don't save if cursor pos <= 1
	movlw	0x01
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto	CS_M_SONG_WriteValueChk_End

	;; don't save if cursor pos >= 7
	movlw	0x06
	cpfslt	CS_MENU_CURSOR_POS, ACCESS
	rgoto	CS_M_SONG_WriteValueChk_End

	;; parameter index in TMP3
	movff	CS_MENU_CURSOR_POS, TMP3

	;; determine position (different if loop action not selected: always change G0 parameter!)
	movlw	0x03-1
	cpfsgt	CS_MENU_CURSOR_POS, ACCESS
	rgoto	CS_M_SONG_WriteValueChk_NotP
CS_M_SONG_WriteValueChk_P
	SET_BSR	SEQ_BASE
	movf	SEQ_EDIT_SONG_ACTN, W, BANKED
	call	SEQ_SONG_GetActnMode
	addlw	-SEQ_SONG_ACTN_MODE_LOOPS
	bz	CS_M_SONG_WriteValueChk_PLoops
	movlw	3
	movwf	TMP3		; force parameter index to 3
	rgoto	CS_M_SONG_WriteValueChk_Cont

CS_M_SONG_WriteValueChk_PLoops
	;; special treatment when EDIT_BNK is active
	BRA_IFCLR CS_STAT, CS_STAT_SONG_EDIT_BNK, ACCESS, CS_M_SONG_WriteValueChk_NotP

CS_M_SONG_WriteValueChk_PBnk
	;; select sector depending on cursor pos
	SET_BSR	SEQ_BASE
	movlw	SEQ_SBANK_SECTOR_BNK_G01
	movwf	SEQ_SBANK_SECTOR, BANKED
	movf	TMP3, W		; (holds CS_MENU_CURSOR_POS)
	addlw	-3
	btfsc	WREG, 1
	incf	SEQ_SBANK_SECTOR, F, BANKED	; (SEQ_SBANK_SECTOR_BNK_G23)

	;; read value from BankStick depending on song position
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	call	SEQ_SBANK_Read
	decf	EEADR, F	; (fix address for upcoming write)
	movwf	TMP5		; save value in TMP5

	;; copy evantually new bank into TMP4
	lfsr	FSR0, SEQ_SONG
	movf	TMP3, W		; (holds CS_MENU_CURSOR_POS)
	addlw	4		; (banks)
	movf	PLUSW0, W
	andlw	0x07
	movwf	TMP4

	;; combine eventually new bank w/ TMP5[7:4] or TMP5[3:0] depending on cursor pos
	movf	TMP3, W
	addlw	-3
	BRA_IFSET WREG, 0, ACCESS, CS_M_SONG_WriteValueChk_PBnkU
CS_M_SONG_WriteValueChk_PBnkL
	movf	TMP5, W
	andlw	0xf0
	iorwf	TMP4, F
	rgoto	CS_M_SONG_WriteValueChk_PBnkCont

CS_M_SONG_WriteValueChk_PBnkU
	swapf	TMP5, W
	andlw	0xf0
	iorwf	TMP4, F
	swapf	TMP4, F
	;; 	rgoto	CS_M_SONG_WriteValueChk_PBnkCont

CS_M_SONG_WriteValueChk_PBnkCont

	;; check with current edit value
	movf	TMP4, W
	xorwf	TMP5, W
	bz	CS_M_SONG_WriteValueChk_End

	;; store new value
	rgoto	CS_M_SONG_WriteValueChk_Store

CS_M_SONG_WriteValueChk_NotP
CS_M_SONG_WriteValueChk_Cont

	;; select sector depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	TMP3, W		; (holds CS_MENU_CURSOR_POS)
	addlw	-1
	movwf	SEQ_SBANK_SECTOR, BANKED

	;; read value from BankStick depending on song position
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	andlw	0x7f
	movwf	EEADR
	call	SEQ_SBANK_Read
	decf	EEADR, F	; (fix address for upcoming write)
	movwf	TMP5		; save value in TMP5

	;; check with current edit value
	lfsr	FSR0, SEQ_SONG
	movf	TMP3, W		; (holds CS_MENU_CURSOR_POS)
	movf	PLUSW0, W
	movwf	TMP4		; store value to be written in TMP4
	xorwf	TMP5, W
	bz	CS_M_SONG_WriteValueChk_End

CS_M_SONG_WriteValueChk_Store
	;; value not equal: save it in BankStick
#if 0
	;; (debugging: output a CC#0 event with cursor position whenever the value will be saved in BankStick
	movlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x00
	call	MIOS_MIDI_TxBufferPut
	movf	TMP3, W
	call	MIOS_MIDI_TxBufferPut
#endif

	movf	TMP4, W
	call	SEQ_SBANK_Write

	;; if not in song mode, change pattern
	SET_BSR	SEQ_BASE
	BRA_IFCLR SEQ_SONG, 7, BANKED, CS_M_SONG_WriteValueChk_NoRfrsh
	movf	SEQ_EDIT_SONG_POS, W, BANKED
	call	SEQ_SONG_SetPos
	SET_BSR	SEQ_BASE
CS_M_SONG_WriteValueChk_NoRfrsh

CS_M_SONG_WriteValueChk_End
	;; restore MIOS_PARAMETER[12]
	movff	TMP1, MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2
	return


