; $Id$
;
; MBSEQ Menu "Display"
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Menu Exit routine is called when the exit button is pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Exit
	;; init default LCD offsets for a menu page
	goto	SEQ_LCD_SetMenuOffsets

;; --------------------------------------------------------------------------
;;  Select routine is called when the select button has been pressed
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Select
	;; exit if button depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; stop message
	call	SEQ_MSG_Stop

	;; clear screen
	call	SEQ_LCD_Clear

	;; increment page, wrap at 2
	SET_BSR	SEQ_BASE
	incf	SEQ_DISPLAY_PAGE, F, BANKED
	movlw	2
        cpfslt  SEQ_DISPLAY_PAGE, BANKED
	clrf	SEQ_DISPLAY_PAGE, BANKED

	;; request re-initialization of page
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; branch depending on selected page
	movff	SEQ_DISPLAY_PAGE, WREG
	JUMPTABLE_2BYTES 2 ; entries
	rgoto	CS_M_DISPLAY_Init0
	rgoto	CS_M_DISPLAY_Init1

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick
	;; branch depending on selected page
	movff	SEQ_DISPLAY_PAGE, WREG
	JUMPTABLE_2BYTES 2 ; entries
	rgoto	CS_M_DISPLAY_Tick0
	rgoto	CS_M_DISPLAY_Tick1


;; --------------------------------------------------------------------------
;;  This is the main page
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init0
	;; init Msg offsets
	call	SEQ_LCD_SetMsgOffsets
	;; force complete refresh of message
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; begin with the first step
	call	SEQ_MSG_Handler
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return


CS_M_DISPLAY_Tick0
	;; branch to message handler until refresh counter is zero again
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	SEQ_MSG_Handler
	return


;; --------------------------------------------------------------------------
;;  This page prints some sequencer info
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init1
	;; init common offsets
	call	SEQ_LCD_SetMenuOffsets

	goto	SEQ_LCD_PrintInfoStatic


CS_M_DISPLAY_Tick1
	goto	SEQ_LCD_PrintInfo

