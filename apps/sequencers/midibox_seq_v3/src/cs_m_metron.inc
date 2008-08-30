; $Id$
;
; MBSEQ Menu for Metronome Settings
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
CS_M_METRON_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x10, 0x40+ 3, 3,	CS_M_METRON_MetroChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x10, 0x40+14, 3,	CS_M_METRON_MetroChnChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f, 0x40+23, 3,	CS_M_METRON_MetroMNoteChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x7f, 0x40+33, 3,	CS_M_METRON_MetroBNoteChanged


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
CS_M_METRON_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; exit if GP button 9-16 is pressed (not used in this menu)
	movlw	8
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto CS_M_METRON_GP_Callback_Ignore

	SET_BSR	SEQ_BASE

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_METRON_GP_Callback_1
	rgoto	CS_M_METRON_GP_Callback_2
	rgoto	CS_M_METRON_GP_Callback_3
	rgoto	CS_M_METRON_GP_Callback_4
	rgoto	CS_M_METRON_GP_Callback_5
	rgoto	CS_M_METRON_GP_Callback_6
	rgoto	CS_M_METRON_GP_Callback_7
	rgoto	CS_M_METRON_GP_Callback_8

CS_M_METRON_GP_Callback_1
CS_M_METRON_GP_Callback_2
	btg	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED	; toggle metronome on/off
	movlw	0		; cursor pos
	rgoto	CS_M_METRON_GP_Callback_Cont_NP

CS_M_METRON_GP_Callback_3
CS_M_METRON_GP_Callback_4
	movlw	1		; cursor pos
	rgoto	CS_M_METRON_GP_Callback_Cont

CS_M_METRON_GP_Callback_5
CS_M_METRON_GP_Callback_6
	movlw	2		; cursor pos
	rgoto	CS_M_METRON_GP_Callback_Cont

CS_M_METRON_GP_Callback_7
CS_M_METRON_GP_Callback_8
	movlw	3		; cursor pos
	;; 	rgoto	CS_M_METRON_GP_Callback_Cont

CS_M_METRON_GP_Callback_Cont	; toggle function
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_METRON_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_METRON_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

CS_M_METRON_GP_Callback_Ignore
	clrf	CS_MENU_ENC_INCREMENTER
	return

;; --------------------------------------------------------------------------
CS_M_METRON_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; determine pattern with a short algorithm
	;; 0 -> 00000011
	;; 1 -> 00001100
	;; 2 -> 00110000
	;; 3 -> 11000000
	;; left half only
	movf	CS_MENU_CURSOR_POS, W
	clrc
	rlf	WREG, W
	movwf	TMP1
	call	MIOS_HLP_GetBitORMask
	movwf	TMP2

	movf	TMP1, W
	addlw	1
	call	MIOS_HLP_GetBitORMask
	iorwf	TMP2, W

	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_METRON_Init
	;; install parameter table
	TABLE_ADDR CS_M_METRON_PARAMETERS	; pointer to table
	movlw	8				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy value into menu parameter depending on cursor pos
	SET_BSR	SEQ_BASE
	movf	CS_MENU_CURSOR_POS, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_METRON_Init_Cursor0
	rgoto	CS_M_METRON_Init_Cursor1
	rgoto	CS_M_METRON_Init_Cursor2
	rgoto	CS_M_METRON_Init_Cursor3

CS_M_METRON_Init_Cursor0	; metronome on/off
	movlw	0
	btfsc	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
	movlw 1
 	rgoto	CS_M_METRON_Init_Cont

CS_M_METRON_Init_Cursor1	; metronome channel
	movf	SEQ_METRONOME_CHANNEL, W, BANKED
 	rgoto	CS_M_METRON_Init_Cont

CS_M_METRON_Init_Cursor2	; metronome measure note
	movf	SEQ_METRONOME_NOTE_MEASURE, W, BANKED
 	rgoto	CS_M_METRON_Init_Cont

CS_M_METRON_Init_Cursor3	; metronome beat note
	movf	SEQ_METRONOME_NOTE_BEAT, W, BANKED
	;;  	rgoto	CS_M_METRON_Init_Cont

CS_M_METRON_Init_Cont
	movwf	CS_MENU_PARAMETER_L

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_METRON_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_METRON_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_METRON_Tick_Upper_STR
	db	"Metronome   Chn.    Meas.Note  BeatNote                                         "
CS_M_METRON_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Metronome   Chn.    Meas.Note  BeatNote 
	;; 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	;;    off       #10       C#1       C#1            
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	CS_M_HLP_LCD_Tick_Resume ; (will branch to next CS_M_METRON_Tick_x)

CS_M_METRON_Tick_Upper
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
	mullw	10
	movlw	LOW(CS_M_METRON_Tick_Upper_STR)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_METRON_Tick_Upper_STR)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString
	;; repeat 8 times
	rcall	CS_M_METRON_Tick_Yield
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_METRON_Tick_Upper

        ;; ------------------------------------------------------------------
CS_M_METRON_Tick_8
	movlw	0x40 + 3
	call	MIOS_LCD_CursorSet

	movlw	0		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_METRON_Tick_8_End

	SET_BSR SEQ_BASE
	movlw	0
	btfsc	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
	movlw 1
	call	SEQ_LCD_PrnOnOff

CS_M_METRON_Tick_8_End
	rcall	CS_M_METRON_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_METRON_Tick_9
	movlw	0x40 + 13
	call	MIOS_LCD_CursorSet
	movlw	'#'
	call	MIOS_LCD_PrintChar

	movlw	1		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_METRON_Tick_9_End

	movff	SEQ_METRONOME_CHANNEL, WREG
	call	SEQ_LCD_PrnChn

CS_M_METRON_Tick_9_End
	rcall	CS_M_METRON_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_METRON_Tick_A
	movlw	0x40 + 23
	call	MIOS_LCD_CursorSet

	movlw	2		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_METRON_Tick_A_End

	movff	SEQ_METRONOME_NOTE_MEASURE, WREG
	call	SEQ_LCD_PrnNote

CS_M_METRON_Tick_A_End
	rcall	CS_M_METRON_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_METRON_Tick_B
	movlw	0x40 + 33
	call	MIOS_LCD_CursorSet

	movlw	3		; item number
	call	CS_MENU_CursorHandler2
	bz	CS_M_METRON_Tick_B_End

	movff	SEQ_METRONOME_NOTE_BEAT, WREG

CS_M_METRON_Tick_B_End
	call	SEQ_LCD_PrnNote

        ;; ------------------------------------------------------------------
CS_M_METRON_Tick_End
	goto	CS_M_HLP_LCD_Tick_End

CS_M_METRON_Tick_Yield
	goto	CS_M_HLP_LCD_Tick_Yield


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_METRON_MetroChanged
	SET_BSR	SEQ_BASE
	bcf	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
	btfsc	CS_MENU_PARAMETER_L, 0
	bsf	SEQ_MODE1, SEQ_MODE1_METRONOME, BANKED
	return

CS_M_METRON_MetroChnChanged
	movff	CS_MENU_PARAMETER_L, SEQ_METRONOME_CHANNEL
	return

CS_M_METRON_MetroMNoteChanged
	movff	CS_MENU_PARAMETER_L, SEQ_METRONOME_NOTE_MEASURE
	return

CS_M_METRON_MetroBNoteChanged
	movff	CS_MENU_PARAMETER_L, SEQ_METRONOME_NOTE_BEAT
	return
