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
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_TRANSPOSE_OCT | 0x80,	0x0f,	0x40, 3,	CS_M_TRKTRAN_OctChanged
 CS_MENU_PARAMETER_ENTRY SEQ_CC_OFFSET_TRANSPOSE_SEMI | 0x80,	0x0f,	0x47, 3,	CS_M_TRKTRAN_NoteChanged
 CS_MENU_PARAMETER_ENTRY -1,					0x03,   0x0f, 1,	CS_M_HLP_TrkChanged

;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------	
CS_M_TRKTRAN_Init
	;; install parameter table
	TABLE_ADDR CS_M_TRKTRAN_PARAMETERS	; pointer to table
	movlw	3				; number of parameters
	call	CS_MENU_SetMenuParameterTable

	;; print static chars
	TABLE_ADDR CS_M_TRKTRAN_STR0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString
	goto	MIOS_LCD_PrintString

CS_M_TRKTRAN_STR0  STRING 14, 0x00, "Transpose Trk:"
CS_M_TRKTRAN_STR1  STRING  3, 0x43, "Oct"
CS_M_TRKTRAN_STR2  STRING  3, 0x4a, "Sem."


;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_TRKTRAN_Tick
	;; print track number
	call	CS_M_HLP_PrnTrkAt0F

	;; print octave transpose value
	movlw	0x40
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_TRANSPOSE_OCT
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_LCD_Prn_PMDec1

	;; print octave transpose value
	movlw	0x47
	call	MIOS_LCD_CursorSet
	movlw	SEQ_CC_OFFSET_TRANSPOSE_SEMI
	call	CS_M_HLP_TrkCC_Get
	call	SEQ_LCD_Prn_PMDec1

	;; branch to the cursor handler
	goto	CS_MENU_CursorHandler

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This hook is called when the menu parameter value has been changed
;; --------------------------------------------------------------------------
CS_M_TRKTRAN_OctChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	SEQ_CC_OFFSET_TRANSPOSE_OCT
	call	CS_M_HLP_TrkCC_Set
	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	return

CS_M_TRKTRAN_NoteChanged
	movlw	-8
	addwf	CS_MENU_PARAMETER_L, F
	movlw	SEQ_CC_OFFSET_TRANSPOSE_SEMI
	call	CS_M_HLP_TrkCC_Set
	movlw	+8
	addwf	CS_MENU_PARAMETER_L, F
	return

;; ==========================================================================

