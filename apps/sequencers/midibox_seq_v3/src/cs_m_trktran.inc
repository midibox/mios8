; $Id$
;
; MBSEQ Menu "Track Transpose"
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
CS_M_TRKTRAN_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKTRAN_GxTyChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_TRANSPOSE_OCT | 0x80,	0x0f,	     -1, -1,	CS_M_TRKTRAN_OctChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_TRANSPOSE_SEMI | 0x80,	0x0f,	     -1, -1,	CS_M_TRKTRAN_SemiChanged

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
CS_M_TRKTRAN_GP_Callback
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; button 0 selects track
	movf	MIOS_PARAMETER1, W
	bnz	CS_M_TRKTRAN_GP_Callback_Trn
CS_M_TRKTRAN_GP_Callback_Trk
	movlw	0				; set (new) cursor pos
	movwf	CS_MENU_CURSOR_POS
	rgoto	CS_M_TRKTRAN_GP_Callback_End

CS_M_TRKTRAN_GP_Callback_Trn
	;; always radio button function!
	clrf	CS_MENU_ENC_INCREMENTER

	;; set octave/semitones depending on cursor pos
	;; branch depending on button
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKTRAN_GP_Callback_Oct	; GxTy pos
	addlw	-1
	bz	CS_M_TRKTRAN_GP_Callback_Oct	; Oct pos
	rgoto	CS_M_TRKTRAN_GP_Callback_Semi	; Semi pos

CS_M_TRKTRAN_GP_Callback_Oct
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L	; set parameter
	rcall	CS_M_TRKTRAN_OctChanged
	movlw	1				; set (new) cursor pos
	movwf	CS_MENU_CURSOR_POS
	rgoto	CS_M_TRKTRAN_GP_Callback_End

CS_M_TRKTRAN_GP_Callback_Semi
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_L	; set parameter
	rcall	CS_M_TRKTRAN_SemiChanged
	movlw	2				; set (new) cursor pos
	movwf	CS_MENU_CURSOR_POS
	;; 	rgoto	CS_M_TRKTRAN_GP_Callback_End

CS_M_TRKTRAN_GP_Callback_End	
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit
	;; (LEDs are updated during re-initialisation)

;; --------------------------------------------------------------------------
CS_M_TRKTRAN_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; set LED depending on transpose value
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKTRAN_UpdateGPLEDs_Oct	; GxTy pos
	addlw	-1
	bz	CS_M_TRKTRAN_UpdateGPLEDs_Oct	; Oct pos
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs_Semi	; Semi pos

CS_M_TRKTRAN_UpdateGPLEDs_Oct
	movlw	SEQ_CC_OFFSET_TRANSPOSE_OCT
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs_Cont

CS_M_TRKTRAN_UpdateGPLEDs_Semi
	movlw	SEQ_CC_OFFSET_TRANSPOSE_SEMI
	;; 	rgoto	CS_M_TRKTRAN_UpdateGPLEDs_Cont

CS_M_TRKTRAN_UpdateGPLEDs_Cont
	call	CS_M_HLP_TrkCC_Get
	addlw	-8
	movwf	TMP1
	andlw	0x07
	call	MIOS_HLP_GetBitORMask

	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1
	btfss	TMP1, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	btfsc	TMP1, 3
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; LED 1 active if track item selected
	movf	CS_MENU_CURSOR_POS, W
	skpnz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0
	return

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_TRKTRAN_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKTRAN_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons, used to select an item here
	TABLE_ADDR CS_M_TRKTRAN_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKTRAN_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.  Transpose: >Octave<  >Semitone< 
	;; 0000--1111111111-2222222222--33333333------------------------------------------
	;; lower line: print GxTy and -7...0...7 (0x04..0x13)

	;; print segments 0-3
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0xfc
	bnz	CS_M_TRKTRAN_Tick_Lower

;; ---
CS_M_TRKTRAN_Tick_Upper
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	bz	CS_M_TRKTRAN_Tick_0
	addlw	-1
	bz	CS_M_TRKTRAN_Tick_1
	addlw	-1
	bz	CS_M_TRKTRAN_Tick_2
	rgoto	CS_M_TRKTRAN_Tick_3

CS_M_TRKTRAN_Tick_0_STR	STRING 4, 0x00 + 0, "Trk."
CS_M_TRKTRAN_Tick_0
	TABLE_ADDR CS_M_TRKTRAN_Tick_0_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKTRAN_Tick_Cont

CS_M_TRKTRAN_Tick_1_STR	STRING 10, 0x00 + 6, "Transpose:"
CS_M_TRKTRAN_Tick_1
	TABLE_ADDR CS_M_TRKTRAN_Tick_1_STR
	call	MIOS_LCD_PrintString
	rgoto	CS_M_TRKTRAN_Tick_Cont

CS_M_TRKTRAN_Tick_2
	movlw	1				; cursor pos #1
	rcall	CS_M_TRKTRAN_PrnItem		; print item
	rgoto	CS_M_TRKTRAN_Tick_Cont

CS_M_TRKTRAN_Tick_3
	movlw	2				; cursor pos #2
	rcall	CS_M_TRKTRAN_PrnItem		; print item
	rgoto	CS_M_TRKTRAN_Tick_Cont

;; ---
CS_M_TRKTRAN_Tick_Lower
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4
	bnz	CS_M_TRKTRAN_Tick_Lower_Trn

CS_M_TRKTRAN_Tick_4
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
	rgoto	CS_M_TRKTRAN_Tick_Cont

CS_M_TRKTRAN_Tick_Lower_Trn
	;; current value -> TMP1
	movlw	SEQ_CC_OFFSET_TRANSPOSE_OCT
	btfsc	CS_MENU_CURSOR_POS, 1
	movlw SEQ_CC_OFFSET_TRANSPOSE_SEMI
	call	CS_M_HLP_TrkCC_Get
	addlw	-8
	andlw	0x0f
	movwf	TMP1

	;; item selected -> TMP3.0
	setf	TMP3
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4
	cpfseq	TMP1, ACCESS
	clrf TMP3

	movlw	8+4
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_TRKTRAN_Tick_Lower_R
CS_M_TRKTRAN_Tick_Lower_L	; left side (negative values)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4
	mullw	5
	movf	PRODL, W
	addlw	0x40
	call	MIOS_LCD_CursorSet

	;; print left marker
	movlw	'>'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; check if item should be flashed
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKTRAN_Tick_Lower_L_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_TRKTRAN_Tick_Lower_Spaces

CS_M_TRKTRAN_Tick_Lower_L_NoF
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	sublw	8+4
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_TRKTRAN_Tick_Lower_Cont

CS_M_TRKTRAN_Tick_Lower_R	; right side (positive values)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4-8
	mullw	5
	movf	PRODL, W
	addlw	0xc0
	call	MIOS_LCD_CursorSet

	;; print left marker
	movlw	'>'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar

	;; check if item should be flashed
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKTRAN_Tick_Lower_R_NoF
	BRA_IFSET TMP3, 0, ACCESS, CS_M_TRKTRAN_Tick_Lower_Spaces

CS_M_TRKTRAN_Tick_Lower_R_NoF
	movlw	'+'
	call	MIOS_LCD_PrintChar
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	addlw	-4-8
	call	MIOS_LCD_PrintBCD1
	rgoto	CS_M_TRKTRAN_Tick_Lower_Cont

CS_M_TRKTRAN_Tick_Lower_Spaces
	movlw	2
	call	SEQ_LCD_PrintSpaces
	;; 	rgoto	CS_M_TRKTRAN_Tick_Lower_Cont

CS_M_TRKTRAN_Tick_Lower_Cont
	;; print right marker
	movlw	'<'
	btfss	TMP3, 0
	movlw ' '
	call	MIOS_LCD_PrintChar


CS_M_TRKTRAN_Tick_Cont
	;; Tick function will be called 20 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	20-1
	cpfsgt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	rgoto CS_M_TRKTRAN_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_TRKTRAN_Tick_End

CS_M_TRKTRAN_Tick_End
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKTRAN_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs

CS_M_TRKTRAN_OctChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	SEQ_CC_OFFSET_TRANSPOSE_OCT
	call	CS_M_HLP_TrkCC_Set
	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs

CS_M_TRKTRAN_SemiChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	SEQ_CC_OFFSET_TRANSPOSE_SEMI
	call	CS_M_HLP_TrkCC_Set
	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	rgoto	CS_M_TRKTRAN_UpdateGPLEDs

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function "highlights" either octave or semitones depending on cursor
;; --------------------------------------------------------------------------
CS_M_TRKTRAN_PrnItem
	addlw	-1
	movwf	TMP1

	decf	CS_MENU_CURSOR_POS, W
	movwf	TMP2

	;; calculate pointer to name entry
	TABLE_ADDR CS_M_TRKTRAN_NAME_TABLE
	movf	TMP1, W
	mullw	CS_M_TRKTRAN_NAME_LEN
	movf	PRODL, W
	TABLE_ADD_W

	;; determine cursor pos
	movf	TMP1, W
	mullw	10
	movf	PRODL, W
	addlw	17
	movwf	TMP5
	call	MIOS_LCD_CursorSet
	movlw	0x3f		; store X position
	andwf	TMP5, F

	;; print unmodified name if item selected
	;; replace '>' and '<' by space if item not selected
	;; flash item (print only '>'/'<' and spaces) if cursor position == 1 and flash flag set by timer
	movlw	CS_M_TRKTRAN_NAME_LEN
	movwf	TMP3

	clrf	TMP4		; if bit 0 set: flash item
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_TRKTRAN_PrnItem_NoFlash
	movf	TMP1, W		; compare item number with CC value
	cpfseq	TMP2, ACCESS
	rgoto CS_M_TRKTRAN_PrnItem_NoFlash
#if 0
	movlw	1		; compare cursor pos
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKTRAN_PrnItem_NoFlash
#endif
	bsf	TMP4, 0
CS_M_TRKTRAN_PrnItem_NoFlash

CS_M_TRKTRAN_PrnItemLoop
	tblrd*+
	movf	TABLAT, W
	xorlw	'>'
	bz	CS_M_TRKTRAN_PrnItemLoop_ModChk
	movf	TABLAT, W
	xorlw	'<'
	bnz	CS_M_TRKTRAN_PrnItemLoop_NoMod
CS_M_TRKTRAN_PrnItemLoop_ModChk
	movf	TABLAT, W	; (flash flag set)
	BRA_IFSET TMP4, 0, ACCESS, CS_M_TRKTRAN_PrnItemLoop_PrnChr 
	movf	TMP1, W		; compare item number with CC value
	xorwf	TMP2, W
	bz	CS_M_TRKTRAN_PrnItemLoop_NoMod
	movlw	' '
	rgoto	CS_M_TRKTRAN_PrnItemLoop_PrnChr	
CS_M_TRKTRAN_PrnItemLoop_NoMod
	movf	TABLAT, W
	btfsc	TMP4, 0; (flash flag set)
	movlw ' '	
CS_M_TRKTRAN_PrnItemLoop_PrnChr
	call	MIOS_LCD_PrintChar
	BRA_IFSET TMP4, 1, ACCESS, CS_M_TRKTRAN_PrnItemLoop_Break	; break loop if '<' reached
	incf	TMP5, F			; increment X
	movlw	40			; break loop if X >= 40
	cpfslt	TMP5, ACCESS
	rgoto CS_M_TRKTRAN_PrnItemLoop_Break
CS_M_TRKTRAN_PrnItemLoop_Next
	decfsz	TMP3, F
	rgoto	CS_M_TRKTRAN_PrnItemLoop
CS_M_TRKTRAN_PrnItemLoop_Break

CS_M_TRKTRAN_PrnItemLoop_End
	return

;; ==========================================================================

CS_M_TRKTRAN_NAME_LEN		EQU	10	; 10 characters per entry
CS_M_TRKTRAN_NAME_ENTRIES	EQU	2	; 2 entries
CS_M_TRKTRAN_NAME_TABLE				; all entries are listed here
	db	">Octave<  "
	db	">Semitone<"
