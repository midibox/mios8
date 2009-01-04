; $Id$
;
; MBSEQ Menu "Random Generator"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; used "In-Menu" messages
CS_M_TRKRND_MSG_DEFAULT	EQU	0x00
CS_M_TRKRND_MSG_RANDOM	EQU	0x81
CS_M_TRKRND_MSG_CLEAR	EQU	0x82
CS_M_TRKRND_MSG_UNDO	EQU	0x83

CS_M_TRKRND_TICK_IN_MENU_MSGS
	db	">RANDOM<"	; #1
	db	"CLEARED!"	; #2
	db	">>UNDO<<"	; #3

;; --------------------------------------------------------------------------
;;  This table contains the edit values
;; --------------------------------------------------------------------------
CS_M_TRKRND_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x0f,   0x40+ 0,  4,	CS_M_TRKRND_GxTyChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x3f,   0xc0+ 1,  2,	CS_M_TRKRND_LayAChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x3f,   0xc0+ 6,  2,	CS_M_TRKRND_LayBChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x3f,   0xc0+11,  2,	CS_M_TRKRND_LayCChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x0f,   0xc0+16,  2,	CS_M_TRKRND_TrgAChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x0f,   0xc0+21,  2,	CS_M_TRKRND_TrgBChanged
 CS_MENU_PARAMETER_ENTRY -1,                                    0x0f,   0xc0+26,  2,	CS_M_TRKRND_TrgCChanged

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
CS_M_TRKRND_GP_Callback
	SET_BSR	SEQ_BASE

	;; always select track
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKRND_GP_Callback_1
	rgoto	CS_M_TRKRND_GP_Callback_2
	rgoto	CS_M_TRKRND_GP_Callback_3
	rgoto	CS_M_TRKRND_GP_Callback_4
	rgoto	CS_M_TRKRND_GP_Callback_5
	rgoto	CS_M_TRKRND_GP_Callback_6
	rgoto	CS_M_TRKRND_GP_Callback_7
	rgoto	CS_M_TRKRND_GP_Callback_8
	rgoto	CS_M_TRKRND_GP_Callback_9
	rgoto	CS_M_TRKRND_GP_Callback_10
	rgoto	CS_M_TRKRND_GP_Callback_11
	rgoto	CS_M_TRKRND_GP_Callback_12
	rgoto	CS_M_TRKRND_GP_Callback_13
	rgoto	CS_M_TRKRND_GP_Callback_14
	rgoto	CS_M_TRKRND_GP_Callback_15
	rgoto	CS_M_TRKRND_GP_Callback_16

CS_M_TRKRND_GP_Callback_1
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_2
CS_M_TRKRND_GP_Callback_3
	movf	CS_MENU_ENC_INCREMENTER, W ; ignore encoder
	bnz	CS_M_TRKRND_GP_Callback_EncIgn

	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and store moved value
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_TRKRND_GP_Callback_2_1
CS_M_TRKRND_GP_Callback_2_0
	bcf	CS_IN_MENU_MSG, 7

	;; stop message
	goto	SEQ_MSG_Stop

CS_M_TRKRND_GP_Callback_2_1
	;; request update of all values
	movlw	0x3f
	movwf	CS_RANDOM_GEN_REQ
	rcall	CS_M_TRKRND_Generate

	;; print in-menu message
	movlw	CS_M_TRKRND_MSG_RANDOM
	movwf	CS_IN_MENU_MSG

	;; start and hold message and exit
	call	SEQ_MSG_Start
 	bsf	CS_STAT, CS_STAT_MESSAGE_HOLD
	return

CS_M_TRKRND_GP_Callback_4
CS_M_TRKRND_GP_Callback_5
	;; do nothing
	rgoto	CS_M_TRKRND_GP_Callback_EncIgn

CS_M_TRKRND_GP_Callback_6
	movf	CS_MENU_ENC_INCREMENTER, W ; ignore encoder
	bnz	CS_M_TRKRND_GP_Callback_EncIgn

	;; Clr
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_TRKRND_GP_Callback_6_1
CS_M_TRKRND_GP_Callback_6_0
	bcf	CS_IN_MENU_MSG, 7
	return

CS_M_TRKRND_GP_Callback_6_1
	;; update undo buffer
	call	CS_M_UTILS_UndoUpdate
	;; clear track
	call	CS_M_UTILS_Clear
	;; print in-menu message
	movlw	CS_M_TRKRND_MSG_CLEAR
	movwf	CS_IN_MENU_MSG
	return


CS_M_TRKRND_GP_Callback_7
	movf	CS_MENU_ENC_INCREMENTER, W ; ignore encoder
	bnz	CS_M_TRKRND_GP_Callback_EncIgn

	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch to Utility menu
	movlw	CS_MENU_PAGE_UTILS
	goto	CS_M_HLP_ChangePage


CS_M_TRKRND_GP_Callback_8
	movf	CS_MENU_ENC_INCREMENTER, W ; ignore encoder
	bnz	CS_M_TRKRND_GP_Callback_EncIgn

	;; Undo
	;; once button has been depressed: clear CS_IN_MENU_MSG[7] and exit
	BRA_IFCLR MIOS_PARAMETER2, 0, ACCESS, CS_M_TRKRND_GP_Callback_8_1
CS_M_TRKRND_GP_Callback_8_0
	bcf	CS_IN_MENU_MSG, 7
	return

CS_M_TRKRND_GP_Callback_8_1
	;; Undo last change
	call	CS_M_UTILS_Undo
	;; print in-menu message
	movlw	CS_M_TRKRND_MSG_UNDO
	movwf	CS_IN_MENU_MSG
	return


CS_M_TRKRND_GP_Callback_9
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 0		; request randomise of LayA
	movlw	1		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_10
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 1		; request randomise of LayB
	movlw	2		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_11
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 2		; request randomise of LayC
	movlw	3		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_12
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 3		; request randomise of TrgA
	movlw	4		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_13
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 4		; request randomise of TrgB
	movlw	5		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont

CS_M_TRKRND_GP_Callback_14
	btfss	MIOS_PARAMETER2, 0		; request random generation if GP button has been pressed
	bsf	CS_RANDOM_GEN_REQ, 5		; request randomise of TrgC
	movlw	6		; cursor pos
	rgoto	CS_M_TRKRND_GP_Callback_Cont


CS_M_TRKRND_GP_Callback_15
CS_M_TRKRND_GP_Callback_16
	;; do nothing
	rgoto	CS_M_TRKRND_GP_Callback_EncIgn


CS_M_TRKRND_GP_Callback_Cont	; toggle function
	;; do nothing if GP button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	cpfseq	CS_MENU_CURSOR_POS, ACCESS
	rgoto CS_M_TRKRND_GP_Callback_Cont_NP
	return			; pos already selected, increment allowed

CS_M_TRKRND_GP_Callback_Cont_NP
	clrf	CS_MENU_ENC_INCREMENTER
	movwf	CS_MENU_CURSOR_POS
	goto	CS_MENU_NewCursorPos	; notify new cursor position and exit

CS_M_TRKRND_GP_Callback_EncIgn
	clrf	CS_MENU_ENC_INCREMENTER
	return

;; --------------------------------------------------------------------------
CS_M_TRKRND_UpdateGPLEDs
	;; clear all LEDs
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR0
	clrf	CS_MENU_GP_OVERLAY_DOUT_SR1

	;; flash LEDs
	bcf	MB_STAT, MB_STAT_DONT_FLASH_R_GP_DOUT
	bcf	MB_STAT, MB_STAT_DONT_FLASH_L_GP_DOUT

	;; LED 1: GxTy (item #0) selected
	movf	CS_MENU_CURSOR_POS, W
	skpnz
	bsf	CS_MENU_GP_OVERLAY_DOUT_SR0, 0

	;; LED 9-14: LayA..TrgC selected
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKRND_UpdateGPLEDs_NoLT
CS_M_TRKRND_UpdateGPLEDs_LT
	addlw	-1
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_GP_OVERLAY_DOUT_SR1
CS_M_TRKRND_UpdateGPLEDs_NoLT

	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKRND_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKRND_PARAMETERS	; pointer to table
	movlw	7				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; copy value into menu parameter depending on cursor pos
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKRND_Init_Cont	; (pos 0: track number, already prepared)
CS_M_TRKRND_Init_Pos
	addlw	-1
	lfsr	FSR1, CS_RANDOM_INTENSITY_LA
	movff	PLUSW1, CS_MENU_PARAMETER_L
CS_M_TRKRND_Init_Cont

	;; install callback function for general purpose buttons, used as ext clock divider buttons here
	TABLE_ADDR CS_M_TRKRND_GP_Callback
	call	CS_MENU_SetMenuGPCallbackHook
	rgoto	CS_M_TRKRND_UpdateGPLEDs

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick_Upper_STR
	db	"Trk.          Random Generator          LayA LayB LayC TrgA TrgB TrgC           "

CS_M_TRKRND_Tick
	;; layout:
	;; 00000000001111111111222222222233333333330000000000111111111122222222223333333333
	;; 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	;; <--------------------------------------><-------------------------------------->
	;; Trk.          Random Generator          LayA LayB LayC TrgA TrgB TrgC           
	;; 00000000001111111111222222222233333333334444444444555555555566666666667777777777
	;; G1T1  Generate           Clr. Util Undo  64   --   --   All  --   --            
	;; 88888888888888-----------999999999999999AAAAABBBBBCCCCCDDDDDEEEEEFFFFF----------

	;; handle random generator requests
	rcall	CS_M_TRKRND_Generate

	;; branch depending on selected LCD segment
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	CS_M_HLP_LCD_Tick_Resume ; (will branch to next CS_M_TRKRND_Tick_x)

CS_M_TRKRND_Tick_Upper
	;; determine cursor pos
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	andlw	0x03
	mullw	10
	movf	PRODL, W
	btfsc	CS_MENU_REFRESH_ITEM_CTR, 2
	iorlw	0x80
	call	MIOS_LCD_CursorSet

	;; determine string offset
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	mullw	10
	movlw	LOW(CS_M_TRKRND_Tick_Upper_STR)
	addwf	PRODL, W
	movwf	TBLPTRL
	movlw	HIGH(CS_M_TRKRND_Tick_Upper_STR)
	addwfc	PRODH, W
	movwf	TBLPTRH

	;; print string part (10 characters)
	movlw	10
	call	MIOS_LCD_PrintPreconfString

	;; if last position reached, print in-menu message at right upper corner of first display
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	7
	bnz	CS_M_TRKRND_Tick_Upper_End
	movf	CS_IN_MENU_MSG, W
	andlw	0x7f
	bz	CS_M_TRKRND_Tick_Upper_End
CS_M_TRKRND_Tick_Upper_Msg
	movlw	0x00 + 32
	call	MIOS_LCD_CursorSet
	decf	CS_IN_MENU_MSG, W
	andlw	0x7f
	TABLE_ADDR_MUL_W CS_M_TRKRND_TICK_IN_MENU_MSGS, 8
	movlw	8 ; characters
	call	MIOS_LCD_PrintPreconfString

	;; switch back to default message once button has been released (will be updated after ca. 1 second)
	BRA_IFSET CS_IN_MENU_MSG, 7, ACCESS, CS_M_TRKRND_Tick_Upper_End
	movlw	CS_M_TRKRND_MSG_DEFAULT
	movwf	CS_IN_MENU_MSG
CS_M_TRKRND_Tick_Upper_End

	;; repeat 8 times
	rcall	CS_M_TRKRND_Tick_Yield
	BRA_IFCLR CS_MENU_REFRESH_ITEM_CTR, 3, ACCESS, CS_M_TRKRND_Tick_Upper

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_8
	movlw	0x40 + 0
	call	MIOS_LCD_CursorSet
	
	movlw	0		; item number
	call	CS_M_HLP_PrnGxTy

	call	SEQ_LCD_PrintStringAfterCall
	STRING	8, 0x40 + 6, "Generate"

	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_9
	call	SEQ_LCD_PrintStringAfterCall
	STRING	14, 0x40 + 25, "Clr. Util Undo"
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_A
	;; initial cursor pos at right lower half
	movlw	0xc0
	call	MIOS_LCD_CursorSet

	movlw	0		; first parameter layer
	rcall	CS_M_TRKRND_Tick_PrnPLay
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_B
	movlw	1		; second parameter layer
	rcall	CS_M_TRKRND_Tick_PrnPLay
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_C
	movlw	2		; third parameter layer
	rcall	CS_M_TRKRND_Tick_PrnPLay
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_D
	movlw	0		; first trigger layer
	rcall	CS_M_TRKRND_Tick_PrnTLay
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_E
	movlw	1		; second trigger layer
	rcall	CS_M_TRKRND_Tick_PrnTLay
	rcall	CS_M_TRKRND_Tick_Yield

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_F
	movlw	2		; third trigger layer
	rcall	CS_M_TRKRND_Tick_PrnTLay

        ;; ------------------------------------------------------------------
CS_M_TRKRND_Tick_End
	goto	CS_M_HLP_LCD_Tick_End

CS_M_TRKRND_Tick_Yield
	goto	CS_M_HLP_LCD_Tick_Yield

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  Print Parameter Layer Probability Value
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick_PrnPLay
	movwf	TMP1		; store layer number
	
	movlw	1		; print 1 space
	call	SEQ_LCD_PrintSpaces

	lfsr	FSR1, CS_RANDOM_INTENSITY_LA ; print probability
	movf	TMP1, W
	movf	PLUSW1, W
	bz	CS_M_TRKRND_Tick_PrnxLay_Dis
	
	call	MIOS_LCD_PrintBCD2 ; print 2 digits
	movlw	2		; print 2 spaces
	goto	SEQ_LCD_PrintSpaces

;; --------------------------------------------------------------------------
;;  Print Triger Layer Probability Value
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick_PrnTLay
	movwf	TMP1		; store layer number
	
	movlw	1		; print 1 space
	call	SEQ_LCD_PrintSpaces

	lfsr	FSR1, CS_RANDOM_INTENSITY_TA ; print probability
	movf	TMP1, W
	movf	PLUSW1, W
	bz	CS_M_TRKRND_Tick_PrnxLay_Dis
	xorlw	0x0f
	bz	CS_M_TRKRND_Tick_PrnxLay_All

	xorlw	0x0f		; print 2 digits
	call	MIOS_LCD_PrintBCD2
	movlw	2		; print 2 spaces
	goto	SEQ_LCD_PrintSpaces


;; --------------------------------------------------------------------------
;;  Print "All "
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick_PrnxLay_All
	movlw	'A'
	call	MIOS_LCD_PrintChar
	movlw	'l'
	call	MIOS_LCD_PrintChar
	movlw	'l'
	call	MIOS_LCD_PrintChar
	movlw	1
	goto	SEQ_LCD_PrintSpaces

;; --------------------------------------------------------------------------
;;  Print "--  "
;; --------------------------------------------------------------------------
CS_M_TRKRND_Tick_PrnxLay_Dis
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	2
	goto	SEQ_LCD_PrintSpaces

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKRND_GxTyChanged
	call	CS_M_HLP_GxTyChanged
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_LayAChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_LA
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_LayBChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_LB
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_LayCChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_LC
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_TrgAChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_TA
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_TrgBChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_TB
	rgoto	CS_M_TRKRND_UpdateGPLEDs

CS_M_TRKRND_TrgCChanged
	movff	CS_MENU_PARAMETER_L, CS_RANDOM_INTENSITY_TC
	rgoto	CS_M_TRKRND_UpdateGPLEDs

;; ==========================================================================

CS_M_TRKRND_Generate
	;; check for random generator request
	movf	CS_RANDOM_GEN_REQ, W
	bz	CS_M_TRKRND_Tick_NoGenReq
CS_M_TRKRND_Tick_GenReq
	;; select track
	call	CS_M_HLP_GetSelectedTrk
	movff	WREG, SEQ_EVNTT

	;; update undo buffer
	call	CS_M_UTILS_UndoUpdate

	;; generate new values
	RCALL_IFSET CS_RANDOM_GEN_REQ, 0, ACCESS, CS_M_TRKRND_Gen_LA
	RCALL_IFSET CS_RANDOM_GEN_REQ, 1, ACCESS, CS_M_TRKRND_Gen_LB
	RCALL_IFSET CS_RANDOM_GEN_REQ, 2, ACCESS, CS_M_TRKRND_Gen_LC
	RCALL_IFSET CS_RANDOM_GEN_REQ, 3, ACCESS, CS_M_TRKRND_Gen_TA
	RCALL_IFSET CS_RANDOM_GEN_REQ, 4, ACCESS, CS_M_TRKRND_Gen_TB
	RCALL_IFSET CS_RANDOM_GEN_REQ, 5, ACCESS, CS_M_TRKRND_Gen_TC

	;; clear all requests
	clrf	CS_RANDOM_GEN_REQ
CS_M_TRKRND_Tick_NoGenReq
	return


	;; returns random number depending on CS_RANDOM_INTENSITY
	;; mean value in MIOS_PARAMETER1
	;; range in MIOS_PARAMETER2
CS_M_TRKRND_Gen_GetNumber
	;; 0x40 +/- RAND*CS_RANDOM_INTENSITY*0x20
	call	SEQ_CORE_GenRandomNumber
	movf	MIOS_PARAMETER2, W
	mulwf	SEQ_RANDOM_SEED_L, BANKED

	;; add/subtract depending on SEQ_RANDOM_SEED_H[0]
	BRA_IFCLR SEQ_RANDOM_SEED_H, 0, BANKED, CS_M_TRKRND_Gen_GetNumber_Add
CS_M_TRKRND_Gen_GetNumber_Sub
	movf	PRODH, W
	subwf	MIOS_PARAMETER1, W
	;; saturate on overflow
	btfsc	WREG, 7
	movlw 0x00
	movwf	PRODL

	;; value should never be 0x00
	movf	PRODL, W
	skpnz
	addlw	1
	return

CS_M_TRKRND_Gen_GetNumber_Add
	movf	PRODH, W
	addwf	MIOS_PARAMETER1, W
	;; saturate on overflow
	btfsc	WREG, 7
	movlw 0x7f
	return


;; --------------------------------------------------------------------------
;;  Randomizes the current track depending on CS_RANDOM_INTENSITY_xx
;;  IN: selected track in SEQ_EVNTT
;; --------------------------------------------------------------------------
CS_M_TRKRND_Gen_LA
	movlw	0		; Layer A
	rgoto	CS_M_TRKRND_Gen_L

CS_M_TRKRND_Gen_LB
	movlw	1		; Layer B
	rgoto	CS_M_TRKRND_Gen_L

CS_M_TRKRND_Gen_LC
	movlw	2		; Layer C
	rgoto	CS_M_TRKRND_Gen_L

CS_M_TRKRND_Gen_TA
	movlw	0		; Layer A
	rgoto	CS_M_TRKRND_Gen_T

CS_M_TRKRND_Gen_TB
	movlw	1		; Layer B
	rgoto	CS_M_TRKRND_Gen_T

CS_M_TRKRND_Gen_TC
	movlw	2		; Layer C
	rgoto	CS_M_TRKRND_Gen_T

;; --------------------------------------------------------------------------
CS_M_TRKRND_Gen_L
	movwf	TMP1		; store layer in TMP1
	
	lfsr	FSR1, CS_RANDOM_INTENSITY_LA
	movff	PLUSW1, TMP2	; store intensity in TMP2

	;; don't touch if intensity is 0
	movf	TMP2, W
	bz	CS_M_TRKRND_Gen_L_End

	SET_BSR	SEQ_BASE
	
	clrf	SEQ_EVNTS, BANKED
CS_M_TRKRND_Gen_L_Loop
	;; length?
	call	SEQ_FSR_TrkEvnt
	movff	INDF1, MIOS_PARAMETER1
	movf	TMP1, W		; Layer number
	call	SEQ_LAYER_GetVType
	xorlw	SEQ_LAYER_V_LEN
	bz	CS_M_TRKRND_Gen_L_Loop_Len
CS_M_TRKRND_Gen_L_Loop_NoLen
	movlw	0x40			; 0x40
	movwf	MIOS_PARAMETER1
	movf	TMP2, W			; +/- 63
	movwf	MIOS_PARAMETER2
	rgoto	CS_M_TRKRND_Gen_L_Loop_Cont
CS_M_TRKRND_Gen_L_Loop_Len
	movlw	0x10			; 0x10
	movwf	MIOS_PARAMETER1
	rrf	TMP2, W			; +/- 15
	rrf	WREG, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2
CS_M_TRKRND_Gen_L_Loop_Cont
	rcall	CS_M_TRKRND_Gen_GetNumber
	movwf	MIOS_PARAMETER1
	movf	TMP1, W		; Layer
	call	SEQ_LAYER_SetVValue

	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_TRKRND_Gen_L_Loop

CS_M_TRKRND_Gen_L_End
	return

;; --------------------------------------------------------------------------
CS_M_TRKRND_Gen_T
	movwf	TMP1		; store layer in TMP1
	
	lfsr	FSR1, CS_RANDOM_INTENSITY_TA
	movff	PLUSW1, TMP2	; store intensity in TMP2

	;; don't touch if intensity is 0
	movf	TMP2, W
	bz	CS_M_TRKRND_Gen_T_End

	SET_BSR	SEQ_BASE
	
	clrf	SEQ_EVNTS, BANKED
CS_M_TRKRND_Gen_T_Loop
	movlw	0x01		; set by default
	movwf	MIOS_PARAMETER1
	;; always set if intensity == 15
	movf	TMP2, W
	xorlw	0x0f
	bz	CS_M_TRKRND_Gen_T_Loop_Set
CS_M_TRKRND_Gen_T_Loop_Chk	
	call	SEQ_CORE_GenRandomNumber
	movf	SEQ_RANDOM_SEED_L, W, BANKED
	xorwf	SEQ_RANDOM_SEED_H, W, BANKED
	andlw	0x0f
	cpfslt	TMP2, ACCESS
	rgoto CS_M_TRKRND_Gen_T_Loop_Set
	clrf	MIOS_PARAMETER1	; clear trigger
CS_M_TRKRND_Gen_T_Loop_Set
	movf	TMP1, W		; layer A/B/C
	call	SEQ_TRG_SetLayer

	incf	SEQ_EVNTS, F, BANKED
	movlw	32 - 1
	cpfsgt	SEQ_EVNTS, BANKED
	rgoto CS_M_TRKRND_Gen_T_Loop

CS_M_TRKRND_Gen_T_End
	return



