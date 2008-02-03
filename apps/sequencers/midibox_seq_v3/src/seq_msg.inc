; $Id$
;
; MBSEQ Message Handler
;
; Activate this #define to measure the performance with a scope
; (connect the probe to RC.5)
#define SEQ_MSG_MEASURE_PERFORMANCE 0
;
; MEMO performance measurings show:
;   o initialization of a complete 2x80 screen: ca. 10 ms
;     (happens when the message was not visible before or *ForceInit has been called)
;   o value update of a single item: ca. 200 uS
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Use this function to start the message
;; --------------------------------------------------------------------------
SEQ_MSG_Start
	;; we want to refresh the whole display
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; don't start a message if we are in menu display page #0
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_MSG_Start_Ok
	SET_BSR	SEQ_BASE
	movf	CS_MENU_POS, W
	iorwf	SEQ_DISPLAY_PAGE, W, BANKED
	bz	SEQ_MSG_Handler	; (initial step)
SEQ_MSG_Start_Ok

	;; begin with CS_MENU message
	goto	CS_MENU_StartMessage


;; --------------------------------------------------------------------------
;;  Use this function to hold the message
;;  IN: message ID in WREG
;; --------------------------------------------------------------------------
SEQ_MSG_Hold
	;; we want to refresh the whole display
	clrf	CS_MENU_REFRESH_ITEM_CTR

	;; don't start a message if we are in menu page #0
	BRA_IFCLR CS_STAT, CS_STAT_WITHIN_PAGE, ACCESS, SEQ_MSG_Hold_Ok
	SET_BSR	SEQ_BASE
	movf	CS_MENU_POS, W
	iorwf	SEQ_DISPLAY_PAGE, W, BANKED
	bz	SEQ_MSG_Handler	; (initial step)
SEQ_MSG_Hold_Ok

	;; hold a CS_MENU message
	goto	CS_MENU_HoldMessage


;; --------------------------------------------------------------------------
;;  Use this function to stop a message
;;  IN: -
;; --------------------------------------------------------------------------
SEQ_MSG_Stop
	;; init menu offsets
	call	SEQ_LCD_SetMenuOffsets
	;; stop a CS_MENU message
	goto	CS_MENU_StopMessage


;; --------------------------------------------------------------------------
;;  This function is called by the CS when a message update has been requested
;;  (see cs_menu_tables.inc)
;; --------------------------------------------------------------------------
SEQ_MSG_Handler
#if SEQ_MSG_MEASURE_PERFORMANCE
	bsf	LATC, 5
#endif

	;; ignore first step (for relaxed timings)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	SEQ_MSG_Handler_End
	

	;; copy track number to SEQ_EVNTT
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; second step?
	decf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	SEQ_MSG_Handler_NotFirstStep
SEQ_MSG_Handler_FirstStep
	;; init Msg offsets
	call	SEQ_LCD_SetMsgOffsets

	;; init ptype display
	call	SEQ_LCD_InitPType
SEQ_MSG_Handler_NotFirstStep

	;; else build screen step by step
	SET_BSR	SEQ_BASE
	decf	CS_MENU_REFRESH_ITEM_CTR, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED; selects between step 1-16/17-32
	iorlw 0x10	
	movwf	SEQ_EVNTS, BANKED

	call	SEQ_LCD_PrnMsgItemU

	SET_BSR	SEQ_BASE
	decf	CS_MENU_REFRESH_ITEM_CTR, W
	btfsc	SEQ_SELECTED_STEPS, 0, BANKED; selects between step 1-16/17-32
	iorlw 0x10	
	movwf	SEQ_EVNTS, BANKED

	call	SEQ_LCD_PrnMsgItemL

	;; ...until last step reached
SEQ_MSG_Handler_End
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	16 + 1		; (default)
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	clrf CS_MENU_REFRESH_ITEM_CTR
	
	;; update finished

#if SEQ_MSG_MEASURE_PERFORMANCE
	bcf	LATC, 5
#endif
	return
