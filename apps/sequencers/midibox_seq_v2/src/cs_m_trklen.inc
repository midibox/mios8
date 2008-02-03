; $Id$
;
; MBSEQ Menu "Track Length"
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
CS_M_TRKLEN_PARAMETERS
 ;;			 CC/value				max	cursor/len	MenuParameterHook
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_LENGTH,			0x0f,	0x40, 2,	CS_M_TRKLEN_ValueChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x0f, 1,	CS_M_HLP_TrkChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_TRKLEN_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKLEN_PARAMETERS	; pointer to table
	movlw	2				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_TRKLEN_STR0
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRKLEN_STR0 STRING 13, 0x00, "Length Track:"
CS_M_TRKLEN_STR1 STRING  3, 0x42, "/16"

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKLEN_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print track length value depending on selected track at second line
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_LENGTH
	call	CS_M_HLP_TrkCC_Get
	addlw	1				; add 1
	call	MIOS_LCD_PrintBCD2
	
	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKLEN_ValueChanged
	;; transfer handled by CC routine
	movlw	SEQ_CC_OFFSET_LENGTH
	goto	CS_M_HLP_TrkCC_Set

;; ==========================================================================

