; $Id$
;
; SID Control Surface Modulation Matrix Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Initialises the Button matrix handler (called from cs_menu.inc)
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Init

#if DEFAULT_SRM_BUTTONS_DIN
	lfsr	FSR0, BM_ROW0_VALUES	; will be set to 0xff
	lfsr	FSR1, BM_ROW0_CHANGED	; will be set to 0x00
	clrf	PRODL			; init loop counter
CS_MENU_MATRIX_Init_Loop
	setf	POSTINC0
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 3, ACCESS, CS_MENU_MATRIX_Init_Loop	; loop 8 times
#endif
	
	return


;; --------------------------------------------------------------------------
;;  This function drives the modulation matrix (8*7 LEDs)
;;  it's called periodically every SR update cycle
;;  by sid_srio.inc, USER_SR_Service_Prepare
;;
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Handler
	;; increment line counter, wrap at 8 (0, 1, 2, ... 7, 0, 1, 2, ...)
	incf	CS_MENU_MATRIX_CTR, W
	andlw	0x07
	movwf	CS_MENU_MATRIX_CTR

	;; on overrun, increment flash counter
	skpnz
	incf	CS_MENU_MATRIX_FLASH_CTR, F

	;; get shift register pattern for the common line drivers
	;; this selects the column (E1, E2, L1, ... L6)
	movf	CS_MENU_MATRIX_CTR, W
	call	MIOS_HLP_GetBitORMask
#if DEFAULT_MB6582_CS
#if DEFAULT_SRM_CATHODES
	movwf	MIOS_PARAMETER1
	movlw	(DEFAULT_SRM_CATHODES-1) & 0xf
	call	MIOS_DOUT_SRSet
#endif
#else
#if DEFAULT_SRM_MODMATRIX_DOUT
	movwf	MIOS_PARAMETER1
	movlw	(DEFAULT_SRM_MODMATRIX_DOUT-1) & 0xf
	call	MIOS_DOUT_SRSet
#endif
#endif

	;; now determine the pattern for the LED anodes
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE, ACCESS, CS_MENU_MATRIX_Handler_SP ; always used in SID player mode
	BRA_IFCLR CS_STAT2, CS_STAT2_LEVEL_METERS, ACCESS, CS_MENU_MATRIX_Handler_NoSP	; optionally used in normal mode
CS_MENU_MATRIX_Handler_SP
	;; show animated level meters, e.g. triggered by gates

	;; meter matrix array used as counter array
	lfsr	FSR1, METER_VALUES0
	movf	CS_MENU_MATRIX_CTR, W
#if DEFAULT_MB6582_CS
	sublw	0x07		; invert order of columns (0=right, 7=left)
#endif
	addwf	FSR1L, F

	movf	INDF1, W
	skpz
	decf	INDF1, F

	;; don't show pattern once counter is zero
	movf	INDF1, W	; (ensure that WREG is 0 on branch to SP_Dis)
	bz	CS_MENU_MATRIX_Handler_SP_Dis

	;; show pattern depending on counter value
	TABLE_ADDR CS_MENU_MATRIX_LEVEL_METER_TABLE
	rrf	INDF1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	TABLE_ADD_W
	tblrd*+
	movf	TABLAT, W
CS_MENU_MATRIX_Handler_SP_Dis
#if !DEFAULT_MB6582_CS
	xorlw	0xff
#endif
	rgoto	CS_MENU_MATRIX_Handler_SP_Cont
	
CS_MENU_MATRIX_Handler_NoSP
	;; this selects the row (O1 Pitch, O2 Pitch, ... Filter)
	BRA_IFSET CS_MENU_MODE, CS_MENU_MODE_MATRIX_METER_DISP, ACCESS, CS_MENU_MATRIX_Handler_Meters
CS_MENU_MATRIX_Handler_Nodes
	lfsr	FSR1, CS_MENU_MATRIX_BEGIN
	rgoto	CS_MENU_MATRIX_Handler_Cont
CS_MENU_MATRIX_Handler_Meters
	lfsr	FSR1, METER_VALUES0

CS_MENU_MATRIX_Handler_Cont

	movf	CS_MENU_MATRIX_CTR, W			; add row number to pointer
#if DEFAULT_MB6582_CS
	sublw	0x07					; invert order of columns (0=right, 7=left)
	movf	PLUSW1, W
#else
	comf	PLUSW1, W
#endif
CS_MENU_MATRIX_Handler_SP_Cont
	movwf	MIOS_PARAMETER1	

	;; flash the currently selected row/column
	movf	CS_MENU_MATRIX_FLASH_CTR, W; every 8th cycle: invert column
	andlw	0x38
	bnz	CS_MENU_MATRIX_Handler_NoFlash
CS_MENU_MATRIX_Handler_Flash
	;; note: row/column need to be swapped when sinkdrivers are used
	movf	CS_MENU_MATRIX_SELECTED_COL, W
	xorwf	MIOS_PARAMETER1, F

	movf	CS_MENU_MATRIX_CTR, W
#if DEFAULT_MB6582_CS
	sublw	0x07					; invert order of columns (0=right, 7=left)
#endif
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_MATRIX_SELECTED_ROW, W
	skpz
	comf	MIOS_PARAMETER1, F
CS_MENU_MATRIX_Handler_NoFlash

#if DEFAULT_MB6582_CS
#if DEFAULT_SRM_MODMATRIX_DOUT
	movlw	(DEFAULT_SRM_MODMATRIX_DOUT-1) & 0xf
	call	MIOS_DOUT_SRSet
#endif
#else
#if DEFAULT_SRM_CATHODES
	movlw	(DEFAULT_SRM_CATHODES-1) & 0xf
	call	MIOS_DOUT_SRSet
#endif
#endif

#if DEFAULT_SRM_LEDS_DOUT
	;; optional LED matrix
	lfsr	FSR2, LM_ROW0_VALUES
	movf	CS_MENU_MATRIX_CTR, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	(DEFAULT_SRM_LEDS_DOUT-1) & 0xf
	call	MIOS_DOUT_SRSet
#endif

	;; thats all!
	return



#if DEFAULT_SRM_LEDS_DOUT
;; --------------------------------------------------------------------------
;;  LED Matrix: set a pin - parameter interface like for MIOS_DOUT_PinSet
;;     - WREG contains the LED number (0-63, 64-127 will be mirrored!)
;;     - MIOS_PARAMETER1 contains the LED value (0 or 1)
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_PinSet
	;; branch depending on LED value
	lfsr	FSR1, LM_ROW0_VALUES
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, CS_MENU_MATRIX_PinSet1
	rgoto	CS_MENU_MATRIX_PinSet0

CS_MENU_MATRIX_PinSet1
	movwf	MIOS_PARAMETER1		; temporary store value in MIOS_PARAMETER1
	rlf	WREG, F
	swapf	WREG, W
	andlw	0x07			; 8 SRs maximum, >=8 will be mirrored
	addwf	FSR1L, F

	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	iorwf	INDF1, F
	return

CS_MENU_MATRIX_PinSet0
	movwf	MIOS_PARAMETER1		; temporary store value in MIOS_PARAMETER1
	rlf	WREG, F
	swapf	WREG, W
	andlw	0x07			; 8 SRs maximum, >=8 will be mirrored
	addwf	FSR1L, F

	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitANDMask
	andwf	INDF1, F
	return
#endif

#if DEFAULT_SRM_BUTTONS_DIN
;; --------------------------------------------------------------------------
;;  Button Matrix: this function gets the DIN values of the selected column
;;  It should be called from USER_SR_Service_Finish
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_GetRowIRQ
	;; since the row line of the buttons is identical to the row line of the LEDs,
	;; we can derive the button row offset from CS_MENU_MATRIX_CTR

	;; store row offset in IRQ_TMP1
	decf	CS_MENU_MATRIX_CTR, W
	andlw	0x07
	movwf	IRQ_TMP1

	;; determine changes and save new value in ROWx_VALUE depending on selected column
	movlw	(DEFAULT_SRM_BUTTONS_DIN-1) & 0xf
	call	MIOS_DIN_SRGet	; result in MIOS_PARAMETER1
	
	;; ---[ BEGIN DIRTY CODE ]---
	movlw	0x10		; clear the MIOS_SR_DIN_CHANGED_x register to avoid, that USER_DIN_NotifyToggle
	addwf	FSR1L, F	; will be called - we have our own toggle handler
	clrf	INDF1		; Direct accesses to MIOS registers are normaly not allowed, I do this here since
				; I know that the register address offset of the DIN_CHANGED_x register will
				; never be changed in future
				; REMINDER for later MIOS versions: provide a function to access the DIN_CHANGED_x register
	;; ---[ END DIRTY CODE ]---

	;; backup old values in IRQ_TMP2
	;; store new values in BM_ROW0_VALUES + IRQ_TMP1
	lfsr	FSR1, BM_ROW0_VALUES
	movf	IRQ_TMP1, W
	movff	PLUSW1, IRQ_TMP2
	movff	MIOS_PARAMETER1, PLUSW1

	;; old value in IRQ_TMP2
	;; new value in MIOS_PARAMETER1
	;; select ROWx_CHANGED register
	lfsr	FSR1, BM_ROW0_CHANGED
	movf	IRQ_TMP1, W
	addwf	FSR1L, F

	;; changes are: IRQ_TMP2 xor MIOS_PARAMETER1
	movf	IRQ_TMP2, W
	xorwf	MIOS_PARAMETER1, W

	;; OR the result to the already notified changes (if they have not been zeroed by CS_MENU_MATRIX_MHandler)
	iorwf	INDF1, F

	return


;; --------------------------------------------------------------------------
;;  Button Matrix: this function should be called from USER_Tick (in the
;;  main program) to determine for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified to the CS_MENU_BUTTON_Handler function
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_BHandler
	SET_BSR	SRM_COL_CTR
	clrf	SRM_COL_CTR, BANKED
CS_MENU_MATRIX_BHandler_Loop
	;; get address to ROWx[LR]_CHANGED register
	lfsr	FSR1, BM_ROW0_CHANGED
	movf	SRM_COL_CTR, W, BANKED
	addwf	FSR1L, F

	;; skip individual checks if register is zero
	movf	INDF1, W
	bz	CS_MENU_MATRIX_BHandler_LoopNext

	;; check which bits are set
	btfsc	INDF1, 0		; fastest solution...
	rcall	CS_MENU_MATRIX_BNotify_Pin0
	btfsc	INDF1, 1
	rcall	CS_MENU_MATRIX_BNotify_Pin1
	btfsc	INDF1, 2
	rcall	CS_MENU_MATRIX_BNotify_Pin2
	btfsc	INDF1, 3
	rcall	CS_MENU_MATRIX_BNotify_Pin3
	btfsc	INDF1, 4
	rcall	CS_MENU_MATRIX_BNotify_Pin4
	btfsc	INDF1, 5
	rcall	CS_MENU_MATRIX_BNotify_Pin5
	btfsc	INDF1, 6
	rcall	CS_MENU_MATRIX_BNotify_Pin6
	btfsc	INDF1, 7
	rcall	CS_MENU_MATRIX_BNotify_Pin7
	
CS_MENU_MATRIX_BHandler_LoopNext
	;; increment until last column has been reached
	incf	SRM_COL_CTR, F, BANKED
	movlw	0x08-1		; (8 columns)
	cpfsgt	SRM_COL_CTR, BANKED
	rgoto CS_MENU_MATRIX_BHandler_Loop
	return

	;; ---

CS_MENU_MATRIX_BNotify_Pin0
	bcf	INDF1, 0			; clear "changed" flag
	movlw	0x00
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin1
	bcf	INDF1, 1			; clear "changed" flag
	movlw	0x01
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin2
	bcf	INDF1, 2			; clear "changed" flag
	movlw	0x02
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin3
	bcf	INDF1, 3			; clear "changed" flag
	movlw	0x03
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin4
	bcf	INDF1, 4			; clear "changed" flag
	movlw	0x04
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin5
	bcf	INDF1, 5			; clear "changed" flag
	movlw	0x05
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin6
	bcf	INDF1, 6			; clear "changed" flag
	movlw	0x06
	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin7
	bcf	INDF1, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	CS_MENU_MATRIX_BNotify_Pin_Cont
CS_MENU_MATRIX_BNotify_Pin_Cont
	;; store pin number in MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
	swapf	SRM_COL_CTR, W, BANKED
	rrf	WREG, W
	andlw	0x38
	iorlw	0x80		; add 128, so that the button handler "knows" that this is a button of the matrix
	iorwf	MIOS_PARAMETER1, F

	;; store pin value in MIOS_PARAMETER2
	lfsr	FSR1, BM_ROW0_VALUES
	movf	SRM_COL_CTR, W, BANKED
	addwf	FSR1L, F
	movf	MIOS_PARAMETER1, W
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER2

	;; call "normal" menu button handler
	call	CS_MENU_BUTTON_Handler

	;; fix BSR and FSR1, they propably have been overwritten by the GP_BUTTON handler
	lfsr	FSR1, BM_ROW0_CHANGED
	SET_BSR	SRM_COL_CTR
	movf	SRM_COL_CTR, W, BANKED
	addwf	FSR1L, F
	return
#endif


;; --------------------------------------------------------------------------
;;  This function sets a meter value (directly called from SID sound engine)
;;  IN: meter number (0-7) in WREG
;;      signed meter value in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_MeterSet
	;; calc pointer to meter
	lfsr	FSR1, METER_VALUES0
	addwf	FSR1L, F	; (index in WREG)

	;; all LEDs off if value around 0
	movlw	0x80
	addwf	MIOS_PARAMETER1, F
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x80
	bnz	CS_MENU_MATRIX_MeterSet_Not0
CS_MENU_MATRIX_MeterSet_0
	clrf	INDF1
	return

CS_MENU_MATRIX_MeterSet_Not0
	swapf	MIOS_PARAMETER1, W
#if 0
	rrf	WREG, W
#else
	;; display range 0x4..0xb, saturate if abs. value is greater
	BRA_IFCLR WREG, 3, ACCESS, CS_MENU_MATRIX_MeterSet_Neg
CS_MENU_MATRIX_MeterSet_Pos
	btfsc	WREG, 2
	movlw 0x07
	iorlw	0x04
	rgoto	CS_MENU_MATRIX_MeterSet_Cont
CS_MENU_MATRIX_MeterSet_Neg
	btfss	WREG, 2
	movlw 0x00
	andlw	0x03
	;; 	rgoto	CS_MENU_MATRIX_MeterSet_Cont
CS_MENU_MATRIX_MeterSet_Cont
#endif
	andlw	0x07
	addlw	LOW(CS_MENU_MATRIX_PATTERN_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_MENU_MATRIX_PATTERN_TABLE)
	addwfc	TBLPTRH, F
	clrf	TBLPTRU
	movlw	UPPER(CS_MENU_MATRIX_PATTERN_TABLE)
	addwfc	TBLPTRU, F
	tblrd*+
	movff	TABLAT, INDF1
	return

;; --------------------------------------------------------------------------
;;  Special variation to display a linear value (used in bassline mode)
;;  IN: meter number (0-7) in WREG
;;      high-byte of linear frequency (SID_Vx_LINEAR_FRQ_H) in MIOS_PARAMETER1
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_MeterLinearSet
	;; calc pointer to meter
	lfsr	FSR1, METER_VALUES0
	addwf	FSR1L, F	; (index in WREG)
	swapf	MIOS_PARAMETER1, W
	rrf	WREG, W
	andlw	0x07
	addlw	LOW(CS_MENU_MATRIX_LIN_PATTERN_TABLE)
	movwf	TBLPTRL
	clrf	TBLPTRH
	movlw	HIGH(CS_MENU_MATRIX_LIN_PATTERN_TABLE)
	addwfc	TBLPTRH, F
	clrf	TBLPTRU
	movlw	UPPER(CS_MENU_MATRIX_LIN_PATTERN_TABLE)
	addwfc	TBLPTRU, F
	tblrd*+
	movff	TABLAT, INDF1
	return


CS_MENU_MATRIX_PATTERN_TABLE	; (used to display meter values)
	db	b'01111000', b'00111000'
	db	b'00011000', b'00001000'
	db	b'00001000', b'00001100'
	db	b'00001110', b'00001111'

CS_MENU_MATRIX_LIN_PATTERN_TABLE ; (used to display meter values)
	db	b'10000000', b'01000000'
	db	b'00100000', b'00010000'
	db	b'00001000', b'00000100'
	db	b'00000010', b'00000001'

CS_MENU_MATRIX_LEVEL_METER_TABLE ; (used in meter level mode)
	db	b'10000000', b'11000000'
	db	b'11100000', b'11110000'
	db	b'11111000', b'11111100'
	db	b'11111110', b'11111111'

