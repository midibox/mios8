; $Id$
;
; MBSEQ Menu "Track Assigns"
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
CS_M_TRKASG_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x42, 1,	CS_M_HLP_TrkChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x02,	0x45, 1,	CS_M_TRKASG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x02,	0x48, 1,	CS_M_TRKASG_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x02,	0x4b, 1,	CS_M_TRKASG_ValueChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKASG_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKASG_PARAMETERS	; pointer to table
	movlw	4				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; when cursor pos > 0, copy assign value to CS_MENU_PARAMETER_L
	movf	CS_MENU_CURSOR_POS, W
	bz	CS_M_TRKASG_Init_NoAsgVal
CS_M_TRKASG_Init_AsgVal
	rcall	CS_M_TRKASG_HLP_GetVal
	movwf	CS_MENU_PARAMETER_L
	movf	CS_MENU_CURSOR_POS, W
	xorlw	2
	bnz	CS_M_TRKASG_Init_AsgValNoE1
CS_M_TRKASG_Init_AsgValE1
	rrf	CS_MENU_PARAMETER_L, F
	rrf	CS_MENU_PARAMETER_L, F
CS_M_TRKASG_Init_AsgValNoE1
	movlw	0x03
	andwf	CS_MENU_PARAMETER_L, F
CS_M_TRKASG_Init_NoAsgVal

	;; print static chars
	TABLE_ADDR CS_M_TRKASG_STR0
	goto	MIOS_LCD_PrintString

CS_M_TRKASG_STR0 STRING 13, 0x00, "Trk V1 V2 Len"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKASG_Tick
	;; print track number
	movlw	0x42
	call	MIOS_LCD_CursorSet
	call	CS_M_HLP_PrnTrk

	;; print assignments
	movlw	SEQ_CC_OFFSET_ASSIGNS_E01
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1
	movlw	SEQ_CC_OFFSET_ASSIGNS_L
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP2

	movlw	0x45
	call	MIOS_LCD_CursorSet
	movf	TMP1, W
	andlw	0x03
	addlw	'A'
	call	MIOS_LCD_PrintChar

	movlw	0x48
	call	MIOS_LCD_CursorSet
	rrf	TMP1, W
	rrf	WREG, W
	andlw	0x03
	addlw	'A'
	call	MIOS_LCD_PrintChar

	movlw	0x4b
	call	MIOS_LCD_CursorSet
	movf	TMP2, W
	andlw	0x03
	addlw	'A'
	call	MIOS_LCD_PrintChar

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKASG_ValueChanged
	rcall	CS_M_TRKASG_HLP_GetVal
	movwf	TMP1

	movff	CS_MENU_PARAMETER_L, TMP2

	movf	CS_MENU_CURSOR_POS, W
	andlw	0x03
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_M_TRKASG_ValueChanged_End
	rgoto	CS_M_TRKASG_ValueChanged_V1
	rgoto	CS_M_TRKASG_ValueChanged_V2
	rgoto	CS_M_TRKASG_ValueChanged_Len

CS_M_TRKASG_ValueChanged_V1
CS_M_TRKASG_ValueChanged_Len
	movlw	0x0c
	andwf	TMP1, F
	movf	TMP2, W
	iorwf	TMP1, W
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKASG_HLP_SetVal
	rgoto	CS_M_TRKASG_ValueChanged_End

CS_M_TRKASG_ValueChanged_V2
	movlw	0x03
	andwf	TMP1, F
	rlf	TMP2, W
	rlf	WREG, W
	andlw	0x0c
	iorwf	TMP1, W
	movwf	CS_MENU_PARAMETER_L
	rcall	CS_M_TRKASG_HLP_SetVal
	rgoto	CS_M_TRKASG_ValueChanged_End

CS_M_TRKASG_ValueChanged_End
	movff	TMP2, CS_MENU_PARAMETER_L
	return

;; ==========================================================================

;; help function to get the assign value (SEQ_CC_OFFSET_ASSIGNS_E01 or SEQ_CC_OFFSET_ASSIGNS_L)
CS_M_TRKASG_HLP_GetVal
	movf	CS_MENU_CURSOR_POS, W
	xorlw	0x03
	movlw	SEQ_CC_OFFSET_ASSIGNS_E01
	skpnz
	movlw	SEQ_CC_OFFSET_ASSIGNS_L
	goto	CS_M_HLP_TrkCC_Get

;; help function to set the assign value (SEQ_CC_OFFSET_ASSIGNS_E01 or SEQ_CC_OFFSET_ASSIGNS_L)
CS_M_TRKASG_HLP_SetVal
	movf	CS_MENU_CURSOR_POS, W
	xorlw	0x03
	movlw	SEQ_CC_OFFSET_ASSIGNS_E01
	skpnz
	movlw	SEQ_CC_OFFSET_ASSIGNS_L
	goto	CS_M_HLP_TrkCC_Set
