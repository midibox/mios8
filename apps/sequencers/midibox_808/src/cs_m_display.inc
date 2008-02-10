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

	;; increment current step number, wrap at 32
	SET_BSR	SEQ_BASE
	incf	SEQ_CURRENT_STEP, F, BANKED
	bcf	SEQ_CURRENT_STEP, 5, BANKED

	;; fix "SEQ_SELECTED_STEPS" flag (show step 1-16 or 17-32)
	bcf	SEQ_SELECTED_STEPS, 0, BANKED
	btfsc	SEQ_CURRENT_STEP, 4, BANKED
	bsf	SEQ_SELECTED_STEPS, 0, BANKED

	;; request re-initialization of page
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	return


;; --------------------------------------------------------------------------
;;  Menu Init routine is called when menu page should be initialized
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Init
	;; init Msg offsets
	call	SEQ_LCD_SetMsgOffsets
	;; force complete refresh of message
	clrf	CS_MENU_REFRESH_ITEM_CTR
	;; begin with the first step
	call	SEQ_MSG_Handler
	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  Menu Tick routine is called on every refresh cycle
;; --------------------------------------------------------------------------
CS_M_DISPLAY_Tick
	;; branch to message handler until refresh counter is zero again
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	skpz
	call	SEQ_MSG_Handler
	return
