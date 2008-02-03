; $Id$
;
; MBSEQ Menu "Patterns"
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
CS_M_PATTERN_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+ 2, 2,	CS_M_PATTERN_PtnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+12, 2,	CS_M_PATTERN_PtnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+22, 2,	CS_M_PATTERN_PtnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x80,	0x40+32, 2,	CS_M_PATTERN_PtnChanged

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
CS_M_PATTERN_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; if incrementer != 0, encoder has been moved: modify pattern number directly
	SET_BSR	SEQ_BASE
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_PATTERN_GP_Callback_Button
CS_M_PATTERN_GP_Callback_Enc
	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_PATTERN_GP_Callback_1
	rgoto	CS_M_PATTERN_GP_Callback_2
	rgoto	CS_M_PATTERN_GP_Callback_3
	rgoto	CS_M_PATTERN_GP_Callback_4
	rgoto	CS_M_PATTERN_GP_Callback_5
	rgoto	CS_M_PATTERN_GP_Callback_6
	rgoto	CS_M_PATTERN_GP_Callback_7
	rgoto	CS_M_PATTERN_GP_Callback_8
	rgoto	CS_M_PATTERN_GP_Callback_9
	rgoto	CS_M_PATTERN_GP_Callback_10
	rgoto	CS_M_PATTERN_GP_Callback_11
	rgoto	CS_M_PATTERN_GP_Callback_12
	rgoto	CS_M_PATTERN_GP_Callback_13
	rgoto	CS_M_PATTERN_GP_Callback_14
	rgoto	CS_M_PATTERN_GP_Callback_15
	rgoto	CS_M_PATTERN_GP_Callback_16

CS_M_PATTERN_GP_Callback_1
CS_M_PATTERN_GP_Callback_2
	movlw	0
	movwf	SEQ_SELECTED_GROUP, BANKED
	movlw	0x00
	rgoto	CS_M_PATTERN_GP_Callback_EncCont

CS_M_PATTERN_GP_Callback_3
CS_M_PATTERN_GP_Callback_4
	movlw	1
	movwf	SEQ_SELECTED_GROUP, BANKED
	movlw	0x01
	rgoto	CS_M_PATTERN_GP_Callback_EncCont

CS_M_PATTERN_GP_Callback_5
CS_M_PATTERN_GP_Callback_6
	movlw	2
	movwf	SEQ_SELECTED_GROUP, BANKED
	movlw	0x02
	rgoto	CS_M_PATTERN_GP_Callback_EncCont

CS_M_PATTERN_GP_Callback_7
CS_M_PATTERN_GP_Callback_8
	movlw	3
	movwf	SEQ_SELECTED_GROUP, BANKED
	movlw	0x03
	rgoto	CS_M_PATTERN_GP_Callback_EncCont

CS_M_PATTERN_GP_Callback_EncCont
	movwf	CS_MENU_CURSOR_POS
	call	CS_MENU_NewCursorPos	; notify new cursor position

	;; if encoder with even number, change bank instead of pattern value
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, CS_M_PATTERN_GP_Callback_Cont
	;; increment bank

	;; inc/dec value by using the 16 bit add w/ saturation routine
	lfsr	FSR0, SEQ_NEXT_PATTERN_BANK0	; copy bank into TMP1
	rrf	MIOS_PARAMETER1, W
	andlw	0x03
	addwf	FSR0L, F
	movf	INDF0, W
	movwf	TMP1
	clrf	TMP2

	;; set pointer to TMP1 (is located to an even address, thats important when this function is used!)
	lfsr	FSR1, TMP1
	;; set max value
	movlw	0x07
	movwf	MIOS_PARAMETER1	; low-byte of max value
	clrf	MIOS_PARAMETER2	; high-byte of max value
	;; get incrementer
	movf	CS_MENU_ENC_INCREMENTER, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = TMP[12] contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	clrf	CS_MENU_ENC_INCREMENTER	; don't increment pattern number

	;; do nothing if update flag of previous operation not set
	BRA_IFCLR MIOS_PARAMETER1, 0, ACCESS, CS_M_PATTERN_GP_Callback_Cont

	;; change bank
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	movwf	SEQ_GROUP, BANKED
	lfsr	FSR0, SEQ_NEXT_PATTERN0
	movf	PLUSW0, W
	andlw	0x7f
	movwf	SEQ_PATTERN, BANKED

	movff	TMP1, SEQ_PATTERN_BANK
	call	SEQ_CORE_ChangePatternSynched
	rgoto	CS_M_PATTERN_GP_Callback_Cont	

CS_M_PATTERN_GP_Callback_9
CS_M_PATTERN_GP_Callback_10
CS_M_PATTERN_GP_Callback_11
CS_M_PATTERN_GP_Callback_12
CS_M_PATTERN_GP_Callback_13
CS_M_PATTERN_GP_Callback_14
CS_M_PATTERN_GP_Callback_15
CS_M_PATTERN_GP_Callback_16
	clrf	CS_MENU_ENC_INCREMENTER	; (do nothing)
	rgoto	CS_M_PATTERN_GP_Callback_Cont


CS_M_PATTERN_GP_Callback_Button
	;; call general GP routine for pattern selection
	call	CS_M_HLP_GPPatternSel
	;; save value if one of the right buttons has been pressed
	RCALL_IFSET MIOS_PARAMETER1, 3, ACCESS, CS_M_PATTERN_PtnChanged

CS_M_PATTERN_GP_Callback_Cont
	;; update the LEDs and exit
	;; 	rgoto	CS_M_PATTERN_UpdateGPLEDs

CS_M_PATTERN_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1
	;; exit when menu parameter == 0
	movf	CS_MENU_PARAMETER_L, W
	skpnz
	return

	;; first 8 leds: (CS_MENU_PARAMETER_L-1)[5..3]
	decf	CS_MENU_PARAMETER_L, W
	rlf	WREG, W
	swapf	WREG, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	;; last 8 leds: (CS_MENU_PARAMETER_L-1)[2..0]
	decf	CS_MENU_PARAMETER_L, W
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
	return

;; --------------------------------------------------------------------------
;;  Called when select button is pressed
;; --------------------------------------------------------------------------
CS_M_PATTERN_Select
	;; do nothing if select button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; increment pattern group
	SET_BSR	SEQ_BASE
	incf	SEQ_SELECTED_GROUP, W, BANKED
	andlw	0x03
	movwf	SEQ_SELECTED_GROUP, BANKED
	goto	CS_MENU_NewCursorPos	; exception: just to update screen, cursor pos will be determined in the init routine

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_PATTERN_Init
	;; switch to full screen view
	call	SEQ_LCD_SetMsgOffsets

	;; allow fast display update
	bsf	CS_STAT2, CS_STAT2_ALLOW_FAST_DISP_UPDATE

	;; install parameter table
	TABLE_ADDR CS_M_PATTERN_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; set cursor position depending on selected group
	movff	SEQ_SELECTED_GROUP, CS_MENU_CURSOR_POS

	;; copy current pattern number into parameter
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	lfsr	FSR0, SEQ_NEXT_PATTERN0
	movf	CS_MENU_CURSOR_POS, W
	BRA_IFSET PLUSW0, 7, ACCESS, CS_M_PATTERN_Init_PMuted
CS_M_PATTERN_Init_PNotMuted
	incf	PLUSW0, W
	rgoto	CS_M_PATTERN_Init_P_Cont
CS_M_PATTERN_Init_PMuted
	movlw	0x00
CS_M_PATTERN_Init_P_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as pattern select buttons here
	TABLE_ADDR CS_M_PATTERN_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rcall	CS_M_PATTERN_UpdateGPLEDs

	;; don't flash LEDs by default, only during a pattern is changed via GP buttons
	bsf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bsf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_PATTERN_Tick
	;; print segments 0-15
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_PATTERN_Tick_0
	rgoto	CS_M_PATTERN_Tick_1
	rgoto	CS_M_PATTERN_Tick_2
	rgoto	CS_M_PATTERN_Tick_3
	rgoto	CS_M_PATTERN_Tick_4
	rgoto	CS_M_PATTERN_Tick_5
	rgoto	CS_M_PATTERN_Tick_6
	rgoto	CS_M_PATTERN_Tick_7
	rgoto	CS_M_PATTERN_Tick_8
	rgoto	CS_M_PATTERN_Tick_9
	rgoto	CS_M_PATTERN_Tick_A
	rgoto	CS_M_PATTERN_Tick_B
	rgoto	CS_M_PATTERN_Tick_C
	rgoto	CS_M_PATTERN_Tick_D
	rgoto	CS_M_PATTERN_Tick_E
	rgoto	CS_M_PATTERN_Tick_F

;; --- G1 ---
CS_M_PATTERN_Tick_0
	;; init vertical bars (if required - function breaks if charset already uploaded)
	movlw	CS_MENU_CHARSET_VBARS
	call	CS_SPECIAL_CHARS_Init

	movlw	0
	rcall	CS_M_PATTERN_PrnP
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_1
	movlw	0
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_2
	movlw	1
	rcall	CS_M_PATTERN_PrnM
	movlw	2
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_3
	movlw	3
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

;; --- G2 ---
CS_M_PATTERN_Tick_4
	movlw	1
	rcall	CS_M_PATTERN_PrnP
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_5
	movlw	4
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_6
	movlw	5
	rcall	CS_M_PATTERN_PrnM
	movlw	6
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_7
	movlw	7
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

;; --- G3 ---
CS_M_PATTERN_Tick_8
	movlw	2
	rcall	CS_M_PATTERN_PrnP
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_9
	movlw	8
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_A
	movlw	9
	rcall	CS_M_PATTERN_PrnM
	movlw	10
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_B
	movlw	11
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont
	
;; --- G4 ---
CS_M_PATTERN_Tick_C
	movlw	3
	rcall	CS_M_PATTERN_PrnP
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_D
	movlw	12
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_E
	movlw	13
	rcall	CS_M_PATTERN_PrnM
	movlw	14
	rcall	CS_M_PATTERN_PrnM
	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_F
	movlw	15
	rcall	CS_M_PATTERN_PrnM
	;; 	rgoto	CS_M_PATTERN_Tick_Cont

CS_M_PATTERN_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_PATTERN_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_PATTERN_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This help function prints Px at the upper line, and the pattern number
;;  at the lower line. Pattern index 0-3 expected in WREG
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnP
	movwf	TMP1		; save index n in WREG

	mullw	10		; >Px print at 0x00 + 1 + 10*n
	movf	PRODL, W
	addlw	0x00 + 1
	call	MIOS_LCD_CursorSet

	SET_BSR	SEQ_BASE
	movf	TMP1, W
	xorwf	SEQ_SELECTED_GROUP, W, BANKED
	movlw	' '
	skpnz
	movlw	'>'
	call	MIOS_LCD_PrintChar

	movlw	'G'
	call	MIOS_LCD_PrintChar
	movf	TMP1, W
	addlw	'1'
	call	MIOS_LCD_PrintChar

	SET_BSR	SEQ_BASE
	movf	TMP1, W
	xorwf	SEQ_SELECTED_GROUP, W, BANKED
	movlw	' '
	skpnz
	movlw	'<'
	call	MIOS_LCD_PrintChar

	movf	PRODL, W	; pattern number print at 0x40 + 0 + 10*n
	addlw	0x40 + 0
	call	MIOS_LCD_CursorSet

	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movf	TMP1, W
	incf	PLUSW1, W
	call	MIOS_LCD_PrintBCD1
	movlw	':'
	call	MIOS_LCD_PrintChar

	;; saved pattern number -> TMP2
	lfsr	FSR1, SEQ_NEXT_PATTERN0
	movf	TMP1, W
	movff	PLUSW1, TMP2
	;; if TMP1 == CURSOR_POS, write converted CS_MENU_PARAMETER_L to TMP2
	movf	TMP1, W
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_PATTERN_PrnP_NMP
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP2
CS_M_PATTERN_PrnP_NMP


	movf	TMP1, W				; item number
	rcall	CS_M_PATTERN_PrnCursor		; prints spaces and sets ZERO flag if cursor flashed
	bz	CS_M_PATTERN_PrnP_Flashed
CS_M_PATTERN_PrnP_NotFlashed
	;; if BankStick not available - print "!!" instead of pattern number
	lfsr	FSR1, SEQ_NEXT_PATTERN_BANK0
	movf	TMP1, W
	movf	PLUSW1, W
	call	MIOS_HLP_GetBitORMask
	SET_BSR	SEQ_BASE
	andwf	SEQ_BANKSTICK_STATUS, W, BANKED
	bnz	CS_M_PATTERN_PrnP_NotFlashedAv
CS_M_PATTERN_PrnP_NotFlashedNA
	movlw	'!'
	call	MIOS_LCD_PrintChar
	movlw	'!'
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_PATTERN_PrnP_Flashed
CS_M_PATTERN_PrnP_NotFlashedAv
	movf	TMP2, W
	call	SEQ_LCD_PrnPattern
CS_M_PATTERN_PrnP_Flashed



	;; print "*" if pattern synched to measure
	BRA_IFSET TMP2, 7, ACCESS, CS_M_PATTERN_PrnP_NoS
CS_M_PATTERN_PrnP_S
	SET_BSR	SEQ_BASE
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	SEQ_PATTERN_SYNC_TRANSFER_REQ, W, BANKED
	bz	CS_M_PATTERN_PrnP_NoS
	movlw	'*'
	rgoto	CS_M_PATTERN_PrnP_SPr
CS_M_PATTERN_PrnP_NoS
	movlw	' '
CS_M_PATTERN_PrnP_SPr
	goto	MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
;;  This help function prints a vertical meter at the lower line
;;  track number 0-15 in WREG
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnM
	movwf	TMP1		; save index n in WREG

	;; cursor offset: 10*TMP1[3:2] + TMP[1:0] + 5
	rrf	TMP1, W
	rrf	WREG, W
	andlw	0x03
	mullw	10

	movf	TMP1, W
	andlw	0x03
	addwf	PRODL, W
	addlw	5
	movwf	TMP3

	addlw	0x40		; lower line

	call	MIOS_LCD_CursorSet

	;; print "M" if track is muted, else vertical meter for track status
	SET_BSR	SEQ_BASE
	movf	SEQ_TRKS_MUTED0, W, BANKED
	btfsc	TMP1, 3
	movf	SEQ_TRKS_MUTED1, W, BANKED
	movwf	TMP2
	movf	TMP1, W
	call	MIOS_HLP_GetBitORMask
	andwf	TMP2, W
	bz	CS_M_PATTERN_PrnM_NotMuted
CS_M_PATTERN_PrnM_Muted
	movlw	'M'
	goto	MIOS_LCD_PrintChar

CS_M_PATTERN_PrnM_NotMuted
	lfsr	FSR1, CS_MENU_METER_CTR_BEGIN
	movf	TMP1, W
	movf	PLUSW1, W
	goto	SEQ_LCD_PrnVBar

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_PATTERN_PtnChanged
	;; change to a new pattern
	;; 7th bit cleared: CS_MENU_PARAMETER_L = pattern number + 1
	;; 7th bit set:     CS_MENU_PARAMETER_L = 0
	decf	CS_MENU_PARAMETER_L, W
	movf	CS_MENU_PARAMETER_L, F
	skpnz
	movlw	0x80
	movwf	TMP1

	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	movwf	SEQ_GROUP, BANKED
	lfsr	FSR0, SEQ_NEXT_PATTERN_BANK0
	movff	PLUSW0, SEQ_PATTERN_BANK
	movff	TMP1, SEQ_PATTERN

	call	SEQ_CORE_ChangePatternSynched

	;; update LEDs
	rgoto	CS_M_PATTERN_UpdateGPLEDs

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function prints spaces if the item (specified in WREG) is
;;  selected by cursor, and CS_STAT, CS_STAT_CURSOR_FLASH is cleared
;;  ZERO flag set if cursor flashed
;; --------------------------------------------------------------------------
CS_M_PATTERN_PrnCursor
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT, ACCESS, CS_M_PATTERN_PrnCursorAll
	BRA_IFCLR MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT, ACCESS, CS_M_PATTERN_PrnCursorR
CS_M_PATTERN_PrnCursorAll
	;; flash both digits
	goto	CS_MENU_CursorHandler2

CS_M_PATTERN_PrnCursorR
	;; flash only right digit
	;; TODO
	goto	CS_MENU_CursorHandler2
