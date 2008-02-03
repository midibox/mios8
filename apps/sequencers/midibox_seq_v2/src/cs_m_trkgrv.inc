; $Id$
;
; MBSEQ Menu "Groove Style"
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
CS_M_TRKGRV_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_GROOVE_VALUE,		0x0f,	0x41, 3,	CS_M_TRKGRV_GrvValueChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_GROOVE_STYLE,		0x01,	0x40, 1,	CS_M_TRKGRV_GrvStyleChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_HUMANIZE_VALUE,		0x0f,	0x48, 3,	CS_M_TRKGRV_HumValueChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_HUMANIZE_MODE,		0x07,	0x45, 3,	CS_M_TRKGRV_HumModeChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x0f, 1,	CS_M_HLP_TrkChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKGRV_PARAMETERS	; pointer to table
	movlw	5				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_TRKGRV_STR0
	goto	MIOS_LCD_PrintString

CS_M_TRKGRV_STR0 STRING 15, 0x00, "Grv. Hum.  Trk:"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKGRV_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print groove style depending on selected track and event at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1
	movlw	'S'			; shuffle
	btfsc	TMP1, 0; inverse shuffle
	movlw 'I'	
	call	MIOS_LCD_PrintChar

	;; print groove value
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD3

	;; print humanize mode depending on selected track and event at second line
	movlw	0x45
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	call	CS_M_HLP_TrkCC_Get
	movwf	TMP1
	movlw	'-'
	btfsc	TMP1, 0
	movlw '2'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	TMP1, 1
	movlw '3'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	btfsc	TMP1, 2
	movlw 'L'
	call	MIOS_LCD_PrintChar

	;; print humanize value
	movlw	SEQ_CC_OFFSET_HUMANIZE_VALUE
	call	CS_M_HLP_TrkCC_Get
	call	MIOS_LCD_PrintBCD3


	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKGRV_GrvValueChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_GROOVE_VALUE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_GrvStyleChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_GROOVE_STYLE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_HumValueChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_HUMANIZE_VALUE
	goto	CS_M_HLP_TrkCC_Set

CS_M_TRKGRV_HumModeChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_HUMANIZE_MODE
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================
