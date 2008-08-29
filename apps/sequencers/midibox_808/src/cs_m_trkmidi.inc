; $Id$
;
; MBSEQ Menu for editing the MIDI Note/Octave/Channel
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This is the menu specific GP callback function called by cs_menu_button.inc 
;;  when the GP hook has been installed
;;  Input:
;;     o GP Button number in MIOS_PARAMETER1 (0x00-0x0f)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;  Output:
;;     should return the GP LED values in CS_MENU_GP_OVERLAY_DOUT_SR[01]
;; --------------------------------------------------------------------------
CS_M_TRKMIDI_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; store button number in cursor pos
	movff	MIOS_PARAMETER1, CS_MENU_CURSOR_POS

	;; if shift button pressed: change MIDI channel
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, CS_M_TRKMIDI_GP_Callback_Chn
CS_M_TRKMIDI_GP_Callback_NoteOct
	;; button 1-12: note
	;; button 13-16: octave (+2)
	movlw	12
	cpfslt	MIOS_PARAMETER1
	rgoto	CS_M_TRKMIDI_GP_Callback_Oct
CS_M_TRKMIDI_GP_Callback_Note
	movf	MIOS_PARAMETER1, W	; set new note
	movwf	CS_TRKMIDI_NOTE
	rgoto	CS_M_TRKMIDI_GP_Callback_NoteCnt
CS_M_TRKMIDI_GP_Callback_Oct
	movf	MIOS_PARAMETER1, W
	addlw	-12+3			; set new octave
	movwf	CS_TRKMIDI_OCTAVE

CS_M_TRKMIDI_GP_Callback_NoteCnt
	;; forward new note/octave to track parameter
	rcall	CS_M_TRKMIDI_SetNoteOct

	rgoto	CS_M_TRKMIDI_GP_Callback_Cont

CS_M_TRKMIDI_GP_Callback_Chn
	;; change MIDI channel
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	call	CS_M_HLP_TrkCC_Set
	;; 	rgoto	CS_M_TRKMIDI_GP_Callback_Cont
CS_M_TRKMIDI_GP_Callback_Cont

	;; update LEDs
	rcall	CS_M_TRKMIDI_UpdateGPLEDs

	;; request display update and exit
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
CS_M_TRKMIDI_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; if shift button pressed: display MIDI channel
	SET_BSR	SEQ_BASE
	BRA_IFSET SEQ_MODE1, SEQ_MODE1_SHIFT, BANKED, CS_M_TRKMIDI_UpdateGPLEDs_Chn

CS_M_TRKMIDI_UpdateGPLEDs_NoteOct
	;; LED 1-12: note
	movf	CS_TRKMIDI_NOTE, W
	call	MIOS_HLP_GetBitORMask
	btfss	CS_TRKMIDI_NOTE, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	CS_TRKMIDI_NOTE, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; LED 13-16: octave (if >= 3 and <= 6)

	;; exit if octave <= 2
	movlw	2
	cpfsgt	CS_TRKMIDI_OCTAVE
	return

	;; exit if octave >= 7
	movlw	7
	cpfslt	CS_TRKMIDI_OCTAVE
	return

	movf	CS_TRKMIDI_OCTAVE, W
	addlw	1		; (shift to LED position 13..16)
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_GP_OVERLAY_DOUT_SR1, F

	return

CS_M_TRKMIDI_UpdateGPLEDs_Chn
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	call	CS_M_HLP_TrkCC_Get
	movwf	PRODL
	call	MIOS_HLP_GetBitORMask

	btfss	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	PRODL, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKMIDI_Init
	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKMIDI_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; determine note and octave of current track
	rcall	CS_M_TRKMIDI_GetNoteOct

	;; update LEDs
	rgoto	CS_M_TRKMIDI_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKMIDI_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; BD    MIDI CONFIG   Note: C-3  Chn #10                                          
	;; 00000001111111111111222222223333333333334444444444555555555566666666667777777777
	;;   C   C#   D    D#   E    F    F#   G     G#   A   A#    B   Oct2 Oct3 Oct4 Oct5
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; for 2x16 LCD:
	;; 0000000000111111
	;; 0123456789012345
	;; <-------------->
	;; BD    MIDI CONF.
	;; 0000000111111111
	;; Note: C-3 Chn.10
	;; 2222222222333333


	;; print segments 0-15
	BRA_IFSET CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKMIDI_Tick_89ABCDEF

	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKMIDI_Tick_0
	rgoto	CS_M_TRKMIDI_Tick_1
	rgoto	CS_M_TRKMIDI_Tick_2
	rgoto	CS_M_TRKMIDI_Tick_3
	rgoto	CS_M_TRKMIDI_Tick_4
	rgoto	CS_M_TRKMIDI_Tick_5
	rgoto	CS_M_TRKMIDI_Tick_6
	rgoto	CS_M_TRKMIDI_Tick_7

CS_M_TRKMIDI_Tick_0
	;; print name of selected track
	movlw	0x00 + 0
	call	MIOS_LCD_CursorSet

	call	CS_M_HLP_GetSelectedTrk
	call	CS_M_HLP_PrnTrkNameOrRec	; (6 characters)

	rgoto	CS_M_TRKMIDI_Tick_Cont

CS_M_TRKMIDI_Tick_1
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	14, 0x00 + 6, "MIDI CONFIG   "
#else
	STRING	14, 0x00 + 6, "MIDI CONF.    "
#endif
	rgoto	CS_M_TRKMIDI_Tick_Cont

CS_M_TRKMIDI_Tick_2
	;; print note
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	6, 0x00 + 20, "Note: "
#else
	STRING	6, 0x40 + 0, "Note: "
#endif
	movlw	SEQ_CC_OFFSET_MIDI_NOTE
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_LCD_PrnNote
	movlw	' '
	call	MIOS_LCD_PrintChar
	rgoto	CS_M_TRKMIDI_Tick_Cont

CS_M_TRKMIDI_Tick_3
	;; print MIDI channel
	call	SEQ_LCD_PrintStringAfterCall
#if DEFAULT_LCD_WIDTH == 80
	STRING	6, 0x00 + 30, " Chn #"
#else
	STRING	4, 0x40 + 10, "Chn."
#endif
	movlw	SEQ_CC_OFFSET_MIDI_CHANNEL
	call	CS_M_HLP_TrkCC_Get
	addlw	1
	call	MIOS_LCD_PrintBCD2
	movlw	2
	call	SEQ_LCD_PrintSpaces
	rgoto	CS_M_TRKMIDI_Tick_Cont

CS_M_TRKMIDI_Tick_4
CS_M_TRKMIDI_Tick_5
CS_M_TRKMIDI_Tick_6
CS_M_TRKMIDI_Tick_7
#if DEFAULT_LCD_WIDTH == 80
	;; fill with spaces
	call	CS_M_HLP_Print10Spaces
#endif
	rgoto	CS_M_TRKMIDI_Tick_Cont

CS_M_TRKMIDI_Tick_89ABCDEF_STR
	db	"  C   C#   D    D#   E    F    F#   G     G#   A   A#    B   Oct2 Oct3 Oct4 Oct5"
CS_M_TRKMIDI_Tick_89ABCDEF
#if DEFAULT_LCD_WIDTH == 80
	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_TRKMIDI_Tick_89ABCDEF_STR, 10

	;; print 10 characters
	call	CS_M_HLP_Print10Chars
#endif
	;;  	rgoto	CS_M_TRKMIDI_Tick_Cont


CS_M_TRKMIDI_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_TRKMIDI_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_TRKMIDI_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Determines Note and Octave of current track
;;  Note stored in CS_TRKMIDI_NOTE, octave in CS_TRKMIDI_OCTAVE
;; --------------------------------------------------------------------------
CS_M_TRKMIDI_GetNoteOct
	movlw	SEQ_CC_OFFSET_MIDI_NOTE
	call	CS_M_HLP_TrkCC_Get

	;; determine octave (PRODH) and note number within octave (PRODL)
	movwf	CS_TRKMIDI_NOTE		; note number
	clrf	CS_TRKMIDI_OCTAVE	; octave

CS_M_TRKMIDI_GetNoteOct_Loop
	movlw	12 - 1		; octave found?
	cpfsgt	CS_TRKMIDI_NOTE
	rgoto	CS_M_TRKMIDI_GetNoteOct_Break
	movlw	-12		; if not, decrement 12 from note number and increment octave
	addwf	CS_TRKMIDI_NOTE, F
	incf	CS_TRKMIDI_OCTAVE, F
	rgoto	CS_M_TRKMIDI_GetNoteOct_Loop
CS_M_TRKMIDI_GetNoteOct_Break

	return


;; --------------------------------------------------------------------------
;;  Sets Note value depending on CS_TRKMIDI_NOTE and CS_TRKMIDI_OCTAVE
;; --------------------------------------------------------------------------
CS_M_TRKMIDI_SetNoteOct
	movf	CS_TRKMIDI_OCTAVE, W
	mullw	12
	movf	PRODL, W
	addwf	CS_TRKMIDI_NOTE, W
	movwf	CS_MENU_PARAMETER_L

	movlw	SEQ_CC_OFFSET_MIDI_NOTE
	goto	CS_M_HLP_TrkCC_Set
