; $Id$
;
; MBSEQ Menu "Utilities"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; used "In-Menu" messages
CS_M_UTILS_MSG_DEFAULT	EQU	0x00
CS_M_UTILS_MSG_COPY	EQU	0x81
CS_M_UTILS_MSG_PASTE	EQU	0x82
CS_M_UTILS_MSG_CLEAR	EQU	0x83
CS_M_UTILS_MSG_MOVE	EQU	0x84
CS_M_UTILS_MSG_SCROLL	EQU	0x85
CS_M_UTILS_MSG_UNDO	EQU	0x86

CS_M_UTILS_TICK_IN_MENU_MSGS
	db	">COPIED<"	; #1
	db	">PASTED<"	; #2
	db	"CLEARED!"	; #3
	db	">>MOVE<<"	; #4
	db	">SCROLL<"	; #5
	db	">>UNDO<<"	; #6


;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_UTILS_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0, 4,	CS_M_UTILS_GxTyChanged

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
CS_M_UTILS_GP_Callback
	SET_BSR	SEQ_BASE

	;; always select track
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; special handling for several IN_MENU messages
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_UTILS_GP_Callback_Normal
	movf	CS_IN_MENU_MSG, W
	xorlw	CS_M_UTILS_MSG_MOVE
	bz	CS_M_UTILS_GP_Callback_Move
	xorlw	CS_M_UTILS_MSG_COPY ^ CS_M_UTILS_MSG_MOVE
	bz	CS_M_UTILS_GP_Callback_Copy
	xorlw	CS_M_UTILS_MSG_PASTE ^ CS_M_UTILS_MSG_COPY
	bz	CS_M_UTILS_GP_Callback_Paste
	xorlw	CS_M_UTILS_MSG_SCROLL ^ CS_M_UTILS_MSG_PASTE
	bz	CS_M_UTILS_GP_Callback_Scroll
	rgoto	CS_M_UTILS_GP_Callback_Normal


CS_M_UTILS_GP_Callback_Move
	;; call move handler
	rcall	CS_M_UTILS_Move
	;; clear incrementer (no additional encoder action)
	clrf	CS_MENU_ENC_INCREMENTER
	return


CS_M_UTILS_GP_Callback_Copy
	;; change copy offset and copy marker to begin/end range depending on encoder which has been moved
	movf	MIOS_PARAMETER1, W
	bz	CS_M_UTILS_GP_Callback_CopyB
	xorlw	15		; check for rightmost encoder (counted from zero): always modifies end position
	bz	CS_M_UTILS_GP_Callback_CopyE
	movf	MIOS_PARAMETER1, W ; back to original variable
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw 16
	movwf	MIOS_PARAMETER1
	movf	CS_COPYPASTE_END, W
	cpfslt	MIOS_PARAMETER1, ACCESS
	rgoto CS_M_UTILS_GP_Callback_CopyE
CS_M_UTILS_GP_Callback_CopyB
	movff	CS_COPYPASTE_BEGIN, SEQ_CURRENT_STEP
	rcall	CS_M_UTILS_StepIncDec
	incf	CS_COPYPASTE_END, W
	cpfslt	SEQ_CURRENT_STEP, BANKED
	rgoto CS_M_UTILS_GP_Callback_CopyBCont
	movff	SEQ_CURRENT_STEP, CS_COPYPASTE_BEGIN
CS_M_UTILS_GP_Callback_CopyBCont
	movff	CS_COPYPASTE_BEGIN, SEQ_CURRENT_STEP; (copy back final position)
	rgoto	CS_M_UTILS_GP_Callback_CopyCont

CS_M_UTILS_GP_Callback_CopyE
	movff	CS_COPYPASTE_END, SEQ_CURRENT_STEP
	rcall	CS_M_UTILS_StepIncDec
	movf	CS_COPYPASTE_BEGIN, W
	bz	CS_M_UTILS_GP_Callback_CopyEOk
	decf	CS_COPYPASTE_BEGIN, W
	cpfsgt	SEQ_CURRENT_STEP, BANKED
	rgoto CS_M_UTILS_GP_Callback_CopyECont
CS_M_UTILS_GP_Callback_CopyEOk
	movff	SEQ_CURRENT_STEP, CS_COPYPASTE_END
CS_M_UTILS_GP_Callback_CopyECont
	movff	CS_COPYPASTE_END, SEQ_CURRENT_STEP; (copy back final position)
	;; 	rgoto	CS_M_UTILS_GP_Callback_CopyCont

CS_M_UTILS_GP_Callback_CopyCont
	;; update display depending on selected step position
	rcall	CS_M_UTILS_StepUpdate
	;; clear incrementer (no additional encoder action)
	clrf	CS_MENU_ENC_INCREMENTER
	return


CS_M_UTILS_GP_Callback_Paste
	;; change paste offset
	rcall	CS_M_UTILS_StepIncDec
	;; clear incrementer (no additional encoder action)
	clrf	CS_MENU_ENC_INCREMENTER
	return


CS_M_UTILS_GP_Callback_Scroll
	;; if upper steps page is visible, increment 0x10 to starting point
	movf	MIOS_PARAMETER1, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	addlw	0x10
	movwf	SEQ_CURRENT_STEP, BANKED
	;; update display depending on selected step position
	rcall	CS_M_UTILS_StepUpdate
	;; call scroll handler
	rcall	CS_M_UTILS_Scroll
	;; clear incrementer (no additional encoder action)
	clrf	CS_MENU_ENC_INCREMENTER
	return


CS_M_UTILS_GP_Callback_Normal

	;; if button number != 0, ignore all encoder movements
	movf	MIOS_PARAMETER1, W
	bz	CS_M_UTILS_GP_Callback_EncOk
	movf	CS_MENU_ENC_INCREMENTER, W
	bz	CS_M_UTILS_GP_Callback_ButtonOk
CS_M_UTILS_GP_Callback_EncIgnore
	clrf	CS_MENU_ENC_INCREMENTER
	return
CS_M_UTILS_GP_Callback_ButtonOk
CS_M_UTILS_GP_Callback_EncOk


	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_UTILS_GP_Callback_1
	rgoto	CS_M_UTILS_GP_Callback_2
	rgoto	CS_M_UTILS_GP_Callback_3
	rgoto	CS_M_UTILS_GP_Callback_4
	rgoto	CS_M_UTILS_GP_Callback_5
	rgoto	CS_M_UTILS_GP_Callback_6
	rgoto	CS_M_UTILS_GP_Callback_7
	rgoto	CS_M_UTILS_GP_Callback_8
	rgoto	CS_M_UTILS_GP_Callback_9
	rgoto	CS_M_UTILS_GP_Callback_10
	rgoto	CS_M_UTILS_GP_Callback_11
	rgoto	CS_M_UTILS_GP_Callback_12
	rgoto	CS_M_UTILS_GP_Callback_13
	rgoto	CS_M_UTILS_GP_Callback_14
	rgoto	CS_M_UTILS_GP_Callback_15
	rgoto	CS_M_UTILS_GP_Callback_16

;; ---[ Trk ]--------------------------------------------------
CS_M_UTILS_GP_Callback_1
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0		; cursor pos
	rgoto	CS_M_UTILS_GP_Callback_Cont

;; ---[ Copy ]--------------------------------------------------
CS_M_UTILS_GP_Callback_2
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_2_1
CS_M_UTILS_GP_Callback_2_0
	bcf	CS_IN_MENU_MSG, 7
	;; copy track
	rcall	CS_M_UTILS_Copy
	;; stop message
	goto	SEQ_MSG_Stop

CS_M_UTILS_GP_Callback_2_1
	;; print in-menu message
	movlw	CS_M_UTILS_MSG_COPY
	movwf	CS_IN_MENU_MSG

	;; always select full range
	SET_BSR	SEQ_BASE
	clrf	SEQ_CURRENT_STEP, BANKED
	rcall	CS_M_UTILS_StepUpdate
	clrf	CS_COPYPASTE_BEGIN
	call	SEQ_FSR_TrkLen
	movff	INDF1, CS_COPYPASTE_END

	;; start and hold message and exit
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	return


;; ---[ Paste ]--------------------------------------------------
CS_M_UTILS_GP_Callback_3
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_3_1
CS_M_UTILS_GP_Callback_3_0
	bcf	CS_IN_MENU_MSG, 7
	;; paste track
	rcall	CS_M_UTILS_Paste
	;; stop message
	goto	SEQ_MSG_Stop

CS_M_UTILS_GP_Callback_3_1
	;; update undo buffer
	rcall	CS_M_UTILS_UndoUpdate
	;; print in-menu message
	movlw	CS_M_UTILS_MSG_PASTE
	movwf	CS_IN_MENU_MSG

	;; reset step marker
	SET_BSR	SEQ_BASE
	clrf	SEQ_CURRENT_STEP, BANKED
	rcall	CS_M_UTILS_StepUpdate

	;; start and hold message and exit
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	return


;; ---[ Clr ]--------------------------------------------------
CS_M_UTILS_GP_Callback_4
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_4_1
CS_M_UTILS_GP_Callback_4_0
	bcf	CS_IN_MENU_MSG, 7
	return

CS_M_UTILS_GP_Callback_4_1
	;; call clear button handler
	call	CS_M_UTILS_ClearViaButton
	;; print in-menu message
	movlw	CS_M_UTILS_MSG_CLEAR
	movwf	CS_IN_MENU_MSG
	return


;; ---[ Move ]--------------------------------------------------
CS_M_UTILS_GP_Callback_5
	;; disable move encoder
	movlw	-1
	movff	WREG, CS_MOVE_ENCODER

	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and store moved value
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_5_1
CS_M_UTILS_GP_Callback_5_0
	bcf	CS_IN_MENU_MSG, 7

	;; stop message
	goto	SEQ_MSG_Stop

CS_M_UTILS_GP_Callback_5_1
	;; update undo buffer
	rcall	CS_M_UTILS_UndoUpdate

	;; print in-menu message
	movlw	CS_M_UTILS_MSG_MOVE
	movwf	CS_IN_MENU_MSG

	;; start and hold message and exit
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	return


;; ---[ Scroll ]--------------------------------------------------
CS_M_UTILS_GP_Callback_6
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and store moved value
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_6_1
CS_M_UTILS_GP_Callback_6_0
	bcf	CS_IN_MENU_MSG, 7

	;; stop message
	goto	SEQ_MSG_Stop

CS_M_UTILS_GP_Callback_6_1
	;; update undo buffer
	rcall	CS_M_UTILS_UndoUpdate

	;; start with first step
	clrf	SEQ_CURRENT_STEP, BANKED
	rcall	CS_M_UTILS_StepUpdate

	;; print in-menu message
	movlw	CS_M_UTILS_MSG_SCROLL
	movwf	CS_IN_MENU_MSG

	;; start and hold message and exit
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	return


;; ---[ Random ]--------------------------------------------------
CS_M_UTILS_GP_Callback_7
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to Random Generator menu
	movlw	CS_MENU_PAGE_RANDOM
	goto	CS_M_HLP_ChangePage


;; ---[ Undo ]--------------------------------------------------
CS_M_UTILS_GP_Callback_8
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_GP_Callback_8_1
CS_M_UTILS_GP_Callback_8_0
	bcf	CS_IN_MENU_MSG, 7
	return

CS_M_UTILS_GP_Callback_8_1
	;; Undo last change
	rcall	CS_M_UTILS_Undo
	;; print in-menu message
	movlw	CS_M_UTILS_MSG_UNDO
	movwf	CS_IN_MENU_MSG
	return


;; ---[ Save Menu ]--------------------------------------------------
CS_M_UTILS_GP_Callback_9
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to save menu
	movlw	CS_MENU_PAGE_SAVE
	goto	CS_M_HLP_ChangePage

;; ---[ Record Menu ]------------------------------------------------
CS_M_UTILS_GP_Callback_10
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to save menu
	movlw	CS_MENU_PAGE_RECORD
	goto	CS_M_HLP_ChangePage

;; ---[ Options Menu ]------------------------------------------------
CS_M_UTILS_GP_Callback_11
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to mixer menu
	movlw	CS_MENU_PAGE_MIXER
	goto	CS_M_HLP_ChangePage

;; ---[ Options Menu ]------------------------------------------------
CS_M_UTILS_GP_Callback_12
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to options menu
	movlw	CS_MENU_PAGE_OPT
	goto	CS_M_HLP_ChangePage

;; ---[ Port Mute Menu ]------------------------------------------------
CS_M_UTILS_GP_Callback_13
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to Port Mute menu
	movlw	CS_MENU_PAGE_PMUTE
	goto	CS_M_HLP_ChangePage

;; ---[ free ]------------------------------------------------
CS_M_UTILS_GP_Callback_14
	return

;; ---[ Mute All ]--------------------------------------------------
CS_M_UTILS_GP_Callback_15
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; Mute all tracks
	rcall	CS_M_UTILS_MuteAll
	;; branch to mute menu
	movlw	CS_MENU_PAGE_MUTE
	goto	CS_M_HLP_ChangePage

;; ---[ UnMute All ]--------------------------------------------------
CS_M_UTILS_GP_Callback_16
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; UnMute all tracks
	rcall	CS_M_UTILS_UnMuteAll
	;; branch to mute menu
	movlw	CS_MENU_PAGE_MUTE
	goto	CS_M_HLP_ChangePage


CS_M_UTILS_GP_Callback_Cont	; toggle function
	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_UTILS_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_UTILS_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit


;; --------------------------------------------------------------------------
CS_M_UTILS_UpdateGPLEDs
	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; first item always active
	movlw	b'00000001'
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_UTILS_Init
	;; install parameter table
	TABLE_ADDR CS_M_UTILS_PARAMETERS	; pointer to table
	movlw	1				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; install callback function for general purpose buttons
	TABLE_ADDR CS_M_UTILS_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook

	;; init LEDs
	rgoto	CS_M_UTILS_UpdateGPLEDs


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_UTILS_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.        Utility Functions                       Quick Menu Change           
	;; 00000000001111111122222222222233333333334444444444555555555566666666667777777777
	;; G1T1 Copy Paste Clr Move Scrl Rand Undo Save Rec. Mix. Opt. PMte     Mute UnMute
	;; 88888888889999999999AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFF

	;; print segments 0-15
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_UTILS_Tick_Upper
	rgoto	CS_M_UTILS_Tick_Lower

CS_M_UTILS_Tick_Upper_STR
	db	"Trk.        Utility Functions                       Quick Menu Change           "
CS_M_UTILS_Tick_Upper
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
	TABLE_ADDR_MUL_W CS_M_UTILS_Tick_Upper_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; if last position reached, print in-menu message at right upper corner of first display
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	7
	bnz	CS_M_UTILS_Tick_Upper_End
	movf	CS_IN_MENU_MSG, W
	andlw	0x7f
	bz	CS_M_UTILS_Tick_Upper_End
CS_M_UTILS_Tick_Upper_Msg
	movlw	0x00 + 32
	call	MIOS_LCD_CursorSet
	decf	CS_IN_MENU_MSG, W
	andlw	0x7f
	TABLE_ADDR_MUL_W CS_M_UTILS_TICK_IN_MENU_MSGS, 8
	movlw	8 ; characters
	call	MIOS_LCD_PrintPreconfString

	;; switch back to default message once button has been released (will be updated after ca. 1 second)
	BRA_IFSET CS_IN_MENU_MSG, 7, ACCESS, CS_M_UTILS_Tick_Upper_End
	movlw	CS_M_UTILS_MSG_DEFAULT
	movwf	CS_IN_MENU_MSG
CS_M_UTILS_Tick_Upper_End
	rgoto	CS_M_UTILS_Tick_Cont


CS_M_UTILS_Tick_Lower_STR
	db	"G1T1 Copy Paste Clr Move Scrl Rand Undo Save Rec. Mix. Opt. PMte     Mute UnMute"
CS_M_UTILS_Tick_Lower
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw 0x80
	iorlw	0x40
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x07
	TABLE_ADDR_MUL_W CS_M_UTILS_Tick_Lower_STR, 10

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; if first position, print track number in first 4 characters
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	8
	bnz	CS_M_UTILS_Tick_Lower_End
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy
CS_M_UTILS_Tick_Lower_End
	;;  	rgoto	CS_M_UTILS_Tick_Cont

CS_M_UTILS_Tick_Cont
	;; Tick function will be called 16 times
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 4, ACCESS, CS_M_UTILS_Tick_End
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; 	rgoto	CS_M_PATTERN_Tick_End

CS_M_UTILS_Tick_End
	return


;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_UTILS_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_UTILS_UpdateGPLEDs

;; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function is called from the edit page when a 15-character message
;;  should be print at the right upper corner
;; --------------------------------------------------------------------------
CS_M_UTILS_PrnEditMsg
	;; print 15 spaces if flash flag cleared
	BRA_IFSET CS_STAT, CS_STAT_CURSOR_FLASH, ACCESS, CS_M_UTILS_PrnEditMsg_NoFlash
CS_M_UTILS_PrnEditMsg_Flash
	movlw	15
	goto	SEQ_LCD_PrintSpaces
CS_M_UTILS_PrnEditMsg_NoFlash

	movf	CS_IN_MENU_MSG, W
	xorlw	CS_M_UTILS_MSG_COPY
	bz	CS_M_UTILS_PrnEditMsg_Copy
	xorlw	CS_M_UTILS_MSG_PASTE ^ CS_M_UTILS_MSG_COPY
	bz	CS_M_UTILS_PrnEditMsg_Paste
	xorlw	CS_M_UTILS_MSG_MOVE ^ CS_M_UTILS_MSG_PASTE
	bz	CS_M_UTILS_PrnEditMsg_Move
	xorlw	CS_M_UTILS_MSG_SCROLL ^ CS_M_UTILS_MSG_MOVE
	bz	CS_M_UTILS_PrnEditMsg_Scroll
	return

CS_M_UTILS_PrnEditMsg_Copy_STR
	db	"COPY S "
CS_M_UTILS_PrnEditMsg_Copy
	TABLE_ADDR CS_M_UTILS_PrnEditMsg_Copy_STR
	movlw	7
	call	MIOS_LCD_PrintPreconfString
	incf	CS_COPYPASTE_BEGIN, W
	call	MIOS_LCD_PrintBCD2
	movlw	'-'
	call	MIOS_LCD_PrintChar
	incf	CS_COPYPASTE_END, W
	call	MIOS_LCD_PrintBCD2
	movlw	3
	goto	SEQ_LCD_PrintSpaces


CS_M_UTILS_PrnEditMsg_Paste_STR
	db	"PASTE OFFS. "
CS_M_UTILS_PrnEditMsg_Paste
	TABLE_ADDR CS_M_UTILS_PrnEditMsg_Paste_STR
	movlw	12
	call	MIOS_LCD_PrintPreconfString
	SET_BSR	SEQ_BASE
	incf	SEQ_CURRENT_STEP, W, BANKED
	call	MIOS_LCD_PrintBCD2
	movlw	1
	goto	SEQ_LCD_PrintSpaces


CS_M_UTILS_PrnEditMsg_Move_STR
	db	"MOVE STEPS     "
CS_M_UTILS_PrnEditMsg_Move
	TABLE_ADDR CS_M_UTILS_PrnEditMsg_Move_STR
	movlw	15
	goto	MIOS_LCD_PrintPreconfString
	
CS_M_UTILS_PrnEditMsg_Scroll_STR
	db	"SCROLL TRACK   "
CS_M_UTILS_PrnEditMsg_Scroll
	TABLE_ADDR CS_M_UTILS_PrnEditMsg_Scroll_STR
	movlw	15
	goto	MIOS_LCD_PrintPreconfString

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function increments the record step
;; --------------------------------------------------------------------------
CS_M_UTILS_StepInc
	SET_BSR	SEQ_BASE

	;; increment step number
	incf	SEQ_CURRENT_STEP, F, BANKED
	;; stay on this step on overrun
	call	SEQ_FSR_TrkLen
	movf	INDF1, W
	cpfsgt	SEQ_CURRENT_STEP, BANKED
	rgoto CS_M_UTILS_StepInc_NoOv
	movwf	SEQ_CURRENT_STEP, BANKED
CS_M_UTILS_StepInc_NoOv
	rgoto	CS_M_UTILS_StepUpdate

;; --------------------------------------------------------------------------
;;  This function decrements the record step
;; --------------------------------------------------------------------------
CS_M_UTILS_StepDec
	SET_BSR	SEQ_BASE

	;; decrement step number
	decf	SEQ_CURRENT_STEP, F, BANKED
	;; stay on first step on underrun
	BRA_IFCLR SEQ_CURRENT_STEP, 7, BANKED, CS_M_UTILS_StepDec_NoUn
	clrf	SEQ_CURRENT_STEP, BANKED
CS_M_UTILS_StepDec_NoUn
	rgoto	CS_M_UTILS_StepUpdate

;; --------------------------------------------------------------------------
;;  This function updates the active step value + the 1-16/17-32 screen
;; --------------------------------------------------------------------------
CS_M_UTILS_StepUpdate
	;; select step 1-16/17-32
	SET_BSR	SEQ_BASE
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	btfsc	SEQ_CURRENT_STEP, 4, BANKED
	bsf	SEQ_SELECTED_STEPS, 0, BANKED

	return

;; --------------------------------------------------------------------------
;;  This function increments/decrements the step number
;;  IN:	 step number in SEQ_CURRENT_STEP
;;       Inc/Dec value in CS_MENU_ENC_INCREMENTER
;;  OUT: new step number in SEQ_CURRENT_STEP
;; --------------------------------------------------------------------------
CS_M_UTILS_StepIncDec
	BRA_IFSET CS_MENU_ENC_INCREMENTER, 7, ACCESS, CS_M_UTILS_StepDec
	rgoto	CS_M_UTILS_StepInc


;; --------------------------------------------------------------------------
;;  Copy Track
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_Copy
	SET_BSR	SEQ_BASE
	
	;; use SEQ_EVNTS as loop counter
	clrf	SEQ_EVNTS, BANKED
CS_M_UTILS_CopyLoop
	movlw	0		; Layer A
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_COPYPASTE_LAYER_A
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	movlw	1		; Layer B
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_COPYPASTE_LAYER_B
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	movlw	2		; Layer C
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_COPYPASTE_LAYER_C
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	call	SEQ_TRG_AllGet	; Trigger Layer
	movwf	TABLAT
	lfsr	FSR0, SEQ_COPYPASTE_TRG_LAYERS
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

CS_M_UTILS_CopyLoopNext
	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_UTILS_CopyLoop

	lfsr	FSR1, SEQ_TRK0
	call	SEQ_FSR_TrkXXX	; determines FSR1 depending on SEQ_EVNTT
	movff	FSR1L, FSR0L
	movff	FSR1H, FSR0H
	lfsr	FSR1, SEQ_COPYPASTE_CFG
	;; IN: source pointer in FSR0, target pointer in FSR1
	rcall	CS_M_UTILS_Hlp_CopyCfg

	;; notify that copy&paste buffer is filled
	bsf	CS_STAT2, CS_STAT2_COPYPASTE_FILLED_P	; assign to pattern
	bcf	CS_STAT2, CS_STAT2_COPYPASTE_FILLED_M	; clear for mixer

	return


;; --------------------------------------------------------------------------
;;  Paste Track
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_Paste
	SET_BSR	SEQ_BASE

	;; branch to clear function if copy&paste buffer not filled
	BRA_IFCLR CS_STAT2, CS_STAT2_COPYPASTE_FILLED_P, ACCESS, CS_M_UTILS_Clear

	BRA_IFCLR SEQ_CFG0, SEQ_CFG0_PASTE_CLR_ALL, BANKED, CS_M_UTILS_Paste_NoCfg
CS_M_UTILS_Paste_Cfg
	;; first paste configuration data from buffer
	;; (trigger layer will be overwritten again inside loop
	lfsr	FSR0, SEQ_COPYPASTE_CFG
	lfsr	FSR1, SEQ_TRK0
	call	SEQ_FSR_TrkXXX	; determines FSR1 depending on SEQ_EVNTT
	;; IN: source pointer in FSR0, target pointer in FSR1
	rcall	CS_M_UTILS_Hlp_CopyCfg
CS_M_UTILS_Paste_NoCfg

	;; paste layers
	movf	CS_COPYPASTE_BEGIN, W	; (copy buffer offset)
	movwf	TMP1
	movf	SEQ_CURRENT_STEP, W, BANKED
	movwf	SEQ_EVNTS, BANKED	; (paste target offset)
CS_M_UTILS_PasteLoop
	lfsr	FSR0, SEQ_COPYPASTE_LAYER_A
	movf	TMP1, W
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	0		; Layer A
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_COPYPASTE_LAYER_B
	movf	TMP1, W
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	1		; Layer B
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_COPYPASTE_LAYER_C
	movf	TMP1, W
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	2		; Layer C
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_COPYPASTE_TRG_LAYERS
	movf	TMP1, W
	movf	PLUSW0, W
	call	SEQ_TRG_AllSet

CS_M_UTILS_PasteLoopNext
	incf	TMP1, F		; increment offsets
	incf	CS_COPYPASTE_END, W	; break if end of copy range has been reached
	cpfslt	TMP1, ACCESS
	rgoto CS_M_UTILS_PasteLoopBreak
	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_UTILS_PasteLoop
CS_M_UTILS_PasteLoopBreak

	return


;; --------------------------------------------------------------------------
;;  Clear Track
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_Clear
	;; call clear layer function
	call	SEQ_LAYER_InitLayer

	;; init configuration (if enabled)
	SET_BSR	SEQ_BASE
	CALL_IFSET SEQ_CFG0, SEQ_CFG0_PASTE_CLR_ALL, BANKED, SEQ_LAYER_InitCfg

	;; clear all triggers
	call	SEQ_FSR_TrkTrgA_0
	movlw	3*4
	movwf	TMP1		; used as loop counter
CS_M_UTILS_ClearLoop
	clrf	POSTINC1
	decfsz	TMP1, F
	rgoto	CS_M_UTILS_ClearLoop

	return


;; --------------------------------------------------------------------------
;;  Undo
;; --------------------------------------------------------------------------
CS_M_UTILS_Undo
	SET_BSR	SEQ_BASE

	;; exit if undo buffer not filled
	btfss	CS_STAT2, CS_STAT2_UNDO_FILLED
	return

	;; select track
	movff	CS_UNDO_TRK, SEQ_EVNTT

	;; copy 32 steps
	clrf	SEQ_EVNTS, BANKED
CS_M_UTILS_UndoLoop
	lfsr	FSR0, SEQ_UNDO_LAYER_A
	movf	SEQ_EVNTS, W, BANKED
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	0		; Layer A
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_UNDO_LAYER_B
	movf	SEQ_EVNTS, W, BANKED
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	1		; Layer B
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_UNDO_LAYER_C
	movf	SEQ_EVNTS, W, BANKED
	movff	PLUSW0, MIOS_PARAMETER1
	movlw	2		; Layer C
	call	SEQ_LAYER_SetVValue

	lfsr	FSR0, SEQ_UNDO_TRG_LAYERS
	movf	SEQ_EVNTS, W, BANKED
	movf	PLUSW0, W
	call	SEQ_TRG_AllSet

CS_M_UTILS_UndoLoopNext
	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_UTILS_UndoLoop

	return


;; --------------------------------------------------------------------------
;;  Update Undo Buffer
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_UndoUpdate
	SET_BSR	SEQ_BASE

	;; store track in special variable, so that we restore the right one later
	movff	SEQ_EVNTT, CS_UNDO_TRK

	;; use SEQ_EVNTS as loop counter
	clrf	SEQ_EVNTS, BANKED
CS_M_UTILS_UndoUpdateLoop
	movlw	0		; Layer A
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_UNDO_LAYER_A
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	movlw	1		; Layer B
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_UNDO_LAYER_B
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	movlw	2		; Layer C
	call	SEQ_LAYER_GetVValue
	movwf	TABLAT
	lfsr	FSR0, SEQ_UNDO_LAYER_C
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

	call	SEQ_TRG_AllGet	; Trigger Layer
	movwf	TABLAT
	lfsr	FSR0, SEQ_UNDO_TRG_LAYERS
	movf	SEQ_EVNTS, W, BANKED
	movff	TABLAT, PLUSW0

CS_M_UTILS_UndoUpdateLoopNext
	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_UTILS_UndoUpdateLoop

	;; notify that undo buffer is filled
	bsf	CS_STAT2, CS_STAT2_UNDO_FILLED

	return

;; --------------------------------------------------------------------------
;;  Move Step Handler
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_Move
	;; if encoder number is different from MOVE_ENC, get step, otherwise move step

	;; store step number in TMP1
	movff	MIOS_PARAMETER1, TMP1

	SET_BSR	SEQ_BASE
	;; check if first selection
	movf	CS_MOVE_ENCODER, W
	xorlw	-1
	bnz	CS_M_UTILS_Move_NotFirstSel

CS_M_UTILS_Move_FirstSel
	;; select new step
	movf	TMP1, W
	movwf	CS_MOVE_ENCODER
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED
	iorlw 0x10
	movwf	SEQ_CURRENT_STEP, BANKED

	;; store current step value in SEQ_STPBUF_MOVED_VALUE
	rcall	CS_M_UTILS_StoreMoved

	;; store it also in OLD record and disable current value (clear all triggers)
	rcall	CS_M_UTILS_StoreOld
	movlw	0
	movff	WREG, SEQ_STPBUF_OLD_TRG

	rgoto	CS_M_UTILS_Move_End


CS_M_UTILS_Move_NotFirstSel
	;; check if encoder step matches with moved step
	movf	CS_MOVE_ENCODER, W
	xorwf	TMP1, W
	bz	CS_M_UTILS_Move_It

	;; restore moved value
	rcall	CS_M_UTILS_RestoreMoved

	;; select new one
	rgoto	CS_M_UTILS_Move_FirstSel


CS_M_UTILS_Move_It
	;; restore old value
	rcall	CS_M_UTILS_RestoreOld

	;; set new step
	rcall	CS_M_UTILS_StepIncDec

	;; store "new" old value
	rcall	CS_M_UTILS_StoreOld

	;; restore moved value
	rcall	CS_M_UTILS_RestoreMoved

CS_M_UTILS_Move_End
	;; update step
	rcall	CS_M_UTILS_StepUpdate

	;; start & hold message
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD	; dirty!
	return

;; --------------------------------------------------------------------------
;;  This function stores the layer/trigger values of the selected step
;;  into SEQ_STPBUF_xxx_*
;;  IN:	 step number in SEQ_CURRENT_STEP
;; --------------------------------------------------------------------------
CS_M_UTILS_StoreOld
	lfsr	FSR0, SEQ_STPBUF_OLD_LAYERA
	rgoto	CS_M_UTILS_Store
CS_M_UTILS_StoreMoved
	lfsr	FSR0, SEQ_STPBUF_MOVED_LAYERA
	;; 	rgoto	CS_M_UTILS_Store


CS_M_UTILS_Store
	;; copy layer and trigger values
	SET_BSR	SEQ_BASE
	
	;; select Step
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS

	movlw	0		; Layer A
	call	SEQ_LAYER_GetVValue
	movwf	POSTINC0
	movlw	1		; Layer B
	call	SEQ_LAYER_GetVValue
	movwf	POSTINC0
	movlw	2		; Layer C
	call	SEQ_LAYER_GetVValue
	movwf	POSTINC0
	call	SEQ_TRG_AllGet	; Triggers
	movwf	POSTINC0
	return


;; --------------------------------------------------------------------------
;;  This function restores the layer/trigger values of the selected step
;;  from SEQ_STPBUF_xxx_*
;;  IN:	 step number in SEQ_CURRENT_STEP
;; --------------------------------------------------------------------------
CS_M_UTILS_RestoreOld
	lfsr	FSR0, SEQ_STPBUF_OLD_LAYERA
	rgoto	CS_M_UTILS_Restore
CS_M_UTILS_RestoreMoved
	lfsr	FSR0, SEQ_STPBUF_MOVED_LAYERA
	;; 	rgoto	CS_M_UTILS_Restore


CS_M_UTILS_Restore
	;; copy layer and trigger values
	SET_BSR	SEQ_BASE
	
	;; select track
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; select Step
	movff	SEQ_CURRENT_STEP, SEQ_EVNTS

	movff	POSTINC0, MIOS_PARAMETER1
	movlw	0		; Layer A
	call	SEQ_LAYER_SetVValue

	movff	POSTINC0, MIOS_PARAMETER1
	movlw	1		; Layer B
	call	SEQ_LAYER_SetVValue

	movff	POSTINC0, MIOS_PARAMETER1
	movlw	2		; Layer C
	call	SEQ_LAYER_SetVValue

	movf	POSTINC0, W
	goto	SEQ_TRG_AllSet	; Triggers



;; --------------------------------------------------------------------------
;;  Mute All Tracks
;; --------------------------------------------------------------------------
CS_M_UTILS_MuteAll
	;; mute all tracks
	SET_BSR	SEQ_BASE
	setf	SEQ_TRKS_MUTED0, BANKED
	setf	SEQ_TRKS_MUTED1, BANKED
	return


;; --------------------------------------------------------------------------
;;  UnMute All Tracks
;; --------------------------------------------------------------------------
CS_M_UTILS_UnMuteAll
	;; unmute all tracks
	SET_BSR	SEQ_BASE
	clrf	SEQ_TRKS_MUTED0, BANKED
	clrf	SEQ_TRKS_MUTED1, BANKED
	return


;; --------------------------------------------------------------------------
;;  Scroll Track
;;  Expecting starting point in SEQ_CURRENT_STEP and direction in CS_MENU_ENC_INCREMENTER
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_UTILS_Scroll
	SET_BSR	SEQ_BASE

#if 0
	SET_BSR	SEQ_BASE
	clrf	SEQ_EVNTT, BANKED
	clrf	TMP5		; using TMP5 as loop counter
CS_M_UTILS_ScrollLoop
	;; check if track selected
	call	CS_M_HLP_ChkTrkSelected
	bnz	CS_M_UTILS_ScrollLNext
#else
	;; (scroll only one track, so that this function behaves like the others, and so that UNDO is working properly!)
	;; the track number is already in SEQ_EVNTT
#endif

	;; shift layers
	lfsr	FSR0, SEQ_LAYER_VALUES_TRK0_A
	rcall	CS_M_UTILS_Scroll_Hlp
	lfsr	FSR0, SEQ_LAYER_VALUES_TRK0_B
	rcall	CS_M_UTILS_Scroll_Hlp
	lfsr	FSR0, SEQ_LAYER_VALUES_TRK0_C
	rcall	CS_M_UTILS_Scroll_Hlp

	;; shift step triggers as well
	movlw	0
	rcall	CS_M_UTILS_Scroll_Hlp2
	movlw	1
	rcall	CS_M_UTILS_Scroll_Hlp2
	movlw	2
	rcall	CS_M_UTILS_Scroll_Hlp2

#if 0
CS_M_UTILS_ScrollLNext
	incf	SEQ_EVNTT, F, BANKED
	BRA_IFCLR SEQ_EVNTT, 4, BANKED, CS_M_UTILS_ScrollLoop
#endif

	return


	;; ---
CS_M_UTILS_Scroll_Hlp
	;; determine offset to layer values -> FSR0
	movf	SEQ_EVNTT, W, BANKED
	mullw	0x20
	movf	PRODL, W
	addwf	FSR0L, F
	movf	PRODH, W
	addwf	FSR0H, F

	;; number of steps -> TMP1
	;; exit if only one step has to be rotated
	rcall	CS_M_UTILS_Scroll_HlpNumberSteps
	movwf	TMP1
	xorlw	1
	skpnz
	return

	;; determine how many times we have to rotate -> TMP2
	BRA_IFCLR CS_MENU_ENC_INCREMENTER, 7, ACCESS, CS_M_UTILS_Scroll_HlpR
CS_M_UTILS_Scroll_HlpL
	;; rotate left: shift "number of steps" - 1 times
	decf	TMP1, W
	movwf	TMP2
	rgoto	CS_M_UTILS_Scroll_HlpCnt
CS_M_UTILS_Scroll_HlpR
	;; rotate right: shift 1 time
	movlw	1
	movwf	TMP2
CS_M_UTILS_Scroll_HlpCnt

	;; copy FSR0->FSR2
	movff	FSR0L, FSR2L
	movff	FSR0H, FSR2H

CS_M_UTILS_Scroll_Hlp_OL
	;; copy FSR2->FSR0
	movff	FSR2L, FSR0L
	movff	FSR2H, FSR0H

	;; determine pointer to last step -> FSR0
	call	SEQ_FSR_TrkLen
	movf	INDF1, W
	addwf	FSR0L, F

	; save last step value in TMP3 and decrement pointer
	movff	POSTDEC0, TMP3

	;; preload counter (number of steps)
	decf	TMP1, W
	movwf	TMP4
CS_M_UTILS_Scroll_Hlp_IL
	movff	POSTINC0, POSTDEC0	; save n-1 in n and decrement pointer
	movf	POSTDEC0, W		; decrement pointer again
	decfsz	TMP4, F			; loop x times (x = loop length)
	rgoto	CS_M_UTILS_Scroll_Hlp_IL

	movff	TMP3, PREINC0		; save stored last step in first step

	;; loop until TMP2 is zero
	decfsz	TMP2, F
	rgoto	CS_M_UTILS_Scroll_Hlp_OL
	return


	;; ---
CS_M_UTILS_Scroll_Hlp2
	movwf	TMP5		; save trigger layer number in TMP5

	SET_BSR	SEQ_BASE

	;; number of steps -> TMP1
	;; exit if only one step has to be rotated
	rcall	CS_M_UTILS_Scroll_HlpNumberSteps
	movwf	TMP1
	xorlw	1
	skpnz
	return

	;; determine how many times we have to rotate -> TMP2
	BRA_IFCLR CS_MENU_ENC_INCREMENTER, 7, ACCESS, CS_M_UTILS_Scroll_Hlp2R
CS_M_UTILS_Scroll_Hlp2L
	;; rotate left: shift "number of steps" - 1 times
	decf	TMP1, W
	movwf	TMP2
	rgoto	CS_M_UTILS_Scroll_Hlp2C
CS_M_UTILS_Scroll_Hlp2R
	;; rotate right: shift 1 time
	movlw	1
	movwf	TMP2
CS_M_UTILS_Scroll_Hlp2C

	movff	SEQ_CURRENT_STEP, TMP3	; starting point -> TMP3

CS_M_UTILS_Scroll_Hlp2OL
	call	SEQ_FSR_TrkLen
	movf	INDF1, W		; set first step
	movwf	SEQ_EVNTS, BANKED

	movf	TMP5, W
	call	SEQ_TRG_Get_Lx		; save last step value in TMP4 and decrement pointer
	movwf	TMP4
	decf	SEQ_EVNTS, F, BANKED
CS_M_UTILS_Scroll_Hlp2IL
	movf	TMP5, W
	call	SEQ_TRG_Get_Lx		; save n-1 in n and decrement pointer
	movwf	TABLAT
	incf	SEQ_EVNTS, F, BANKED
	movf	TMP5, W
	call	SEQ_TRG_Set_Lx
	decf	SEQ_EVNTS, F, BANKED
	decf	SEQ_EVNTS, F, BANKED
	;; loop until loop point has passed
	decf	TMP3, W
	cpfseq	SEQ_EVNTS, BANKED
	rgoto CS_M_UTILS_Scroll_Hlp2IL

	incf	SEQ_EVNTS, F, BANKED	; save stored last step in first step
	movff	TMP4, TABLAT
	movf	TMP5, W
	call	SEQ_TRG_Set_Lx

	;; loop until TMP2 zero
	decfsz	TMP2, F
	rgoto	CS_M_UTILS_Scroll_Hlp2OL
	return


;;  returns the number of steps which will be rotated depending on the track length and start position
CS_M_UTILS_Scroll_HlpNumberSteps
	call	SEQ_FSR_TrkLen		; track length -> TABLAT
	movf	INDF1, W
	andlw	0x1f
	movwf	TABLAT

	movf	SEQ_CURRENT_STEP, W, BANKED	; starting point -> WREG

	;; calc: track length - starting point
	subwf	TABLAT, W

	;; return track length on overrun (loop point > track length)
	btfsc	WREG, 7
	movf	TABLAT, W

	addlw	1

	return


;; --------------------------------------------------------------------------
;;  Help Routine which copies Configuration Data from FSR0 -> FSR1
;;  IN: source pointer in FSR0, target pointer in FSR1
;; --------------------------------------------------------------------------
CS_M_UTILS_Hlp_CopyCfg
	clrf	PRODL		; using PRODL as loop counter
CS_M_UTILS_Hlp_CopyCfgLoop
	;; skip if MIDI channel
	movlw	SEQ_TRKCHNx
	xorwf	PRODL, W
	bz	CS_M_UTILS_Hlp_CopyCfgLoopNext
	movff	INDF0, INDF1
CS_M_UTILS_Hlp_CopyCfgLoopNext
	;; increment pointers
	movf	POSTINC1, W
	movf	POSTINC0, W
	incf	PRODL, F
	movlw	SEQ_TRKRECORD_LENGTH - 3*4 - 1	; (dirty!!! don't overwrite triggers - the are located at the end of the record)
	cpfsgt	PRODL, ACCESS
	rgoto CS_M_UTILS_Hlp_CopyCfgLoop

	return


;; --------------------------------------------------------------------------
;;  Clear button function
;;  Used by CLR function of this page, and by the dedicated button function 
;;  in seq_button.inc
;; --------------------------------------------------------------------------
CS_M_UTILS_ClearViaButton
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; request display re-initialisation
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	call	CS_M_HLP_GetSelectedTrk
	SET_BSR	SEQ_BASE
	movwf	SEQ_EVNTT, BANKED
	
	;; update undo buffer
	rcall	CS_M_UTILS_UndoUpdate

	;; clear track
	rgoto	CS_M_UTILS_Clear


;; --------------------------------------------------------------------------
;;  Copy/Paste button function
;;  Only used by dedicated button function in seq_button.inc
;; --------------------------------------------------------------------------
CS_M_UTILS_CopyViaButton
	;; branch depending on button status
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_CopyViaButton_Depr
CS_M_UTILS_CopyViaButton_Pressed
	;; prepare CopyPaste button operation
	rcall	CS_M_UTILS_CopyPasteViaButtonPrep

CS_M_UTILS_CopyViaButton_DeprSub
	;; (a bit dirty - requires adaptions if menu handling is changed)
	;; execute button function #0x01 (2nd menu item of UTILITY page)
	clrf	CS_MENU_ENC_INCREMENTER
	movlw	0x01
	movwf	MIOS_PARAMETER1
	rgoto	CS_M_UTILS_GP_Callback

CS_M_UTILS_CopyViaButton_Depr
	;; execute copy button handler of UTILITY page
	rcall	CS_M_UTILS_CopyViaButton_DeprSub

	;; change back to original page
	rgoto	CS_M_UTILS_CopyPasteViaButtonDepr


;; --------------------------------------------------------------------------
CS_M_UTILS_PasteViaButton
	;; branch depending on button status
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_M_UTILS_PasteViaButton_Depr
CS_M_UTILS_PasteViaButton_Pressed
	;; prepare CopyPaste button operation
	rcall	CS_M_UTILS_CopyPasteViaButtonPrep

CS_M_UTILS_PasteViaButton_DeprSub
	;; (a bit dirty - requires adaptions if menu handling is changed)
	;; execute button function #0x02 (3rd menu item of UTILITY page)
	clrf	CS_MENU_ENC_INCREMENTER
	movlw	0x02
	movwf	MIOS_PARAMETER1
	rgoto	CS_M_UTILS_GP_Callback

CS_M_UTILS_PasteViaButton_Depr
	;; execute paste button handler of UTILITY page
	rcall	CS_M_UTILS_PasteViaButton_DeprSub

	;; change back to original page
	rgoto	CS_M_UTILS_CopyPasteViaButtonDepr


;; --------------------------------------------------------------------------
;;  Help Function
;;  Prepares Copy/Paste via dedicated button
;; --------------------------------------------------------------------------
CS_M_UTILS_CopyPasteViaButtonPrep
	;; remember current page
	movff	CS_MENU_POS, CS_MENU_COPYPASTE_MEMO_PAGE

	;; enter and initialize utility page
	movlw	CS_MENU_PAGE_UTILS
	call	CS_M_HLP_ChangePage
	goto	CS_M_UTILS_Init

;; --------------------------------------------------------------------------
;;  Help Function
;;  Called when dedicated Copy/Paste button has been depressed to switch to the
;;  original page (used by SEQ_BUTTON_Copy and SEQ_BUTTON_Paste in seq_button.inc)
;; --------------------------------------------------------------------------
CS_M_UTILS_CopyPasteViaButtonDepr
	movf	CS_MENU_COPYPASTE_MEMO_PAGE, W
	goto	CS_M_HLP_ChangePage
