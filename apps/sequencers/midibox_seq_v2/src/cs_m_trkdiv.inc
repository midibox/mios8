; $Id$
;
; MBSEQ Menu "Clock Divider"
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
CS_M_TRKDIV_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
#if DEFAULT_BPM_DIVIDER_BEHAVIOUR == 0
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_CLK_DIVIDER,		0x06,	0x46, 2,	CS_M_TRKDIV_ValueChanged
#else
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_CLK_DIVIDER,		0x0f,	0x46, 2,	CS_M_TRKDIV_ValueChanged
#endif
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x0f, 1,	CS_M_HLP_TrkChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKDIV_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_TRKDIV_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRKDIV_STR0 STRING 14, 0x00, "Divider Track:"
CS_M_TRKDIV_STR1 STRING  5, 0x40, "BPM /"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKDIV_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print divider value depending on selected track at second line
	movlw	0x46
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	call	CS_M_HLP_TrkCC_Get
#if DEFAULT_BPM_DIVIDER_BEHAVIOUR == 0
	call	MIOS_HLP_GetBitORMask	; returns: 0->1, 1->2, 2->4, 3->8, 4->16, etc
#else
	addlw	1
#endif
	call	MIOS_LCD_PrintBCD2

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKDIV_ValueChanged
	movlw	SEQ_CC_OFFSET_CLK_DIVIDER
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================

