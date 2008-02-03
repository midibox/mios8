; $Id$
;
; MBSEQ Message Handler
;
; Activate this #define to measure the performance with a scope
; (connect the probe to J14)
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
	bsf	PORTD, 4
#endif

	;; ignore first step (for relaxed timings)
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	bz	SEQ_MSG_Handler_End
	

	;; copy track number to SEQ_EVNTT
	call	CS_M_HLP_GetSelectedTrk
	movwf	SEQ_EVNTT, BANKED

	;; copy drum layer number to SEQ_EVNTD (only used in drum mode)
	call	CS_M_HLP_GetSelectedLayer
	movwf	SEQ_EVNTD, BANKED

	;; get parameter type --- will be stored in SEQ_PTYPE
	call	SEQ_PTYPE_Get

	;; second step?
	decf	CS_MENU_REFRESH_ITEM_CTR, W
	bnz	SEQ_MSG_Handler_NotFirstStep
SEQ_MSG_Handler_FirstStep
	;; init Msg offsets
	call	SEQ_LCD_SetMsgOffsets

	;; init V or H bars depending on SEQ_PTYPE for both displays
	;; select first display
	movlw	0x00
	call	MIOS_LCD_CursorSet
	call	SEQ_LCD_InitPType
#if DEFAULT_LCD_SIZE == 3
	;; reset charset number, so that the same one will also be used for the second display
	clrf	CS_MENU_CHARSET
	;; select second display
	movlw	0x80
	call	MIOS_LCD_CursorSet
	call	SEQ_LCD_InitPType
#endif
SEQ_MSG_Handler_NotFirstStep

	;; else build screen step by step
	SET_BSR	SEQ_BASE
	decf	CS_MENU_REFRESH_ITEM_CTR, W
	movwf	SEQ_EVNTS, BANKED

	;; determine offset depending on display size
	;; for small displays: print beat number at the right side so that the user know which page is displayed
#if DEFAULT_LCD_SIZE == 0 || DEFAULT_LCD_SIZE == 1
	;; ---[ 2x16, 2x20 LCD ]-----------------------------------------------------
	movf	SEQ_CURRENT_STEP, W, BANKED
	andlw	0x0c
	iorwf	SEQ_EVNTS, F, BANKED
	
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	4
	bnz	SEQ_MSG_Handler_NotLastStep
SEQ_MSG_Handler_LastStep
#if DEFAULT_LCD_SIZE == 0
	movlw	0x0c
#else
	movlw	0x0f
#endif
	call	MIOS_LCD_CursorSet
#if DEFAULT_LCD_SIZE == 1
	movlw	' '
	call	MIOS_LCD_PrintChar
#endif
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'['
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	rrf	SEQ_EVNTS, W, BANKED
	rrf	WREG, W
	andlw	0x03
	addlw	1
	call	MIOS_LCD_PrintBCD1
	movlw	']'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	rgoto	SEQ_MSG_Handler_LastStepCont
SEQ_MSG_Handler_NotLastStep
	call	SEQ_LCD_PrnMsgItemU
SEQ_MSG_Handler_LastStepCont
#endif
#if DEFAULT_LCD_SIZE == 2
	;; ---[ 2x40 LCD ]-----------------------------------------------------------
	movf	SEQ_CURRENT_STEP, W, BANKED
	andlw	0x08
	iorwf	SEQ_EVNTS, F, BANKED
	
	movf	CS_MENU_REFRESH_ITEM_CTR, W
	xorlw	8
	bnz	SEQ_MSG_Handler_NotLastStep
SEQ_MSG_Handler_LastStep
	movlw	40-5
	call	MIOS_LCD_CursorSet
	movlw	'['
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movlw	'1'
	btfsc	SEQ_EVNTS, 3, BANKED
	movlw '3'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	movlw	'2'
	btfsc	SEQ_EVNTS, 3, BANKED
	movlw '4'
	call	MIOS_LCD_PrintChar
	movlw	']'
	call	MIOS_LCD_PrintChar
	SET_BSR	SEQ_BASE
	rgoto	SEQ_MSG_Handler_LastStepCont
SEQ_MSG_Handler_NotLastStep
	call	SEQ_LCD_PrnMsgItemU
SEQ_MSG_Handler_LastStepCont
#endif
#if DEFAULT_LCD_SIZE == 3
	;; ---[ two 2x40 LCD ]-------------------------------------------------------
	call	SEQ_LCD_PrnMsgItemU
#endif
	call	SEQ_LCD_PrnMsgItemL

	;; ...until last step reached
SEQ_MSG_Handler_End
	incf	CS_MENU_REFRESH_ITEM_CTR, F
	movlw	16 + 1		; (default)
	;; determine number of items depending on display size
#if DEFAULT_LCD_SIZE == 0 || DEFAULT_LCD_SIZE == 1
	movlw	4 + 1
#endif
#if DEFAULT_LCD_SIZE == 2
	movlw	8 + 1
#endif
#if DEFAULT_LCD_SIZE == 3
	;; 	movlw	16 + 1 ; (default)
#endif
	cpfslt	CS_MENU_REFRESH_ITEM_CTR, ACCESS
	clrf CS_MENU_REFRESH_ITEM_CTR
	
	;; update finished

#if SEQ_MSG_MEASURE_PERFORMANCE
	bcf	PORTD, 4
#endif
	return
