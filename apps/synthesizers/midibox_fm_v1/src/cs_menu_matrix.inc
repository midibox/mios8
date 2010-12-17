; $Id$
;
; Selection Matrix Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called from CS_MENU_BUTTON_Handler on any button change
;; 
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_BHandler
	;; init table pointer
	TABLE_ADDR DIO_M_TABLE

	;; check if button is in table
	;; if pressed, change COLUMN number in CS_MENU_MATRIX_SELECTED[6..4]
	clrf	TMP1
CS_MENU_MATRIX_BHandler_Loop
	tblrd*+
	movf	TABLAT, W
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto CS_MENU_MATRIX_BHandler_LoopNext
	BRA_IFSET MIOS_PARAMETER2, 0, ACCESS, CS_MENU_MATRIX_BHandler_LoopNext
	movlw	0x0f
	andwf	CS_MENU_MATRIX_SELECTED, F
	swapf	TMP1, W
	iorlw	0x80		; set "changed" flag
	iorwf	CS_MENU_MATRIX_SELECTED, F
CS_MENU_MATRIX_BHandler_LoopNext
	incf	TMP1, F
	movlw	6-1
	cpfsgt	TMP1, ACCESS
	rgoto CS_MENU_MATRIX_BHandler_Loop

	return

;; --------------------------------------------------------------------------
;;  This function is called from USER_SR_Service_Finish, it handles the
;;  buttons/LEDs of the selection matrix
;;
;;  The pins are defined in cs_menu_io_tables.inc
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Handler

#ifdef CS_SAMMICH_FM
	;; increment row counter, wrap at 2
	;; this is repurposing the variable used in the full
	;; LED/button matrix, which is unused. Thus this
	;; would need a refactor if it is required.
	incf	CS_MENU_MATRIX_CTR, W
	andlw	0x01
	movwf	CS_MENU_MATRIX_CTR
 	bnz	CS_MENU_MATRIX_Handler_LED_Row_Inst
CS_MENU_MATRIX_Handler_LED_Row_Op
	movlw	0x40	; Op LED column is pin D1
	btfsc	CS_MENU_MODE, CS_MENU_MODE_INS_OP
	iorlw	0x10	; Op LED
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 0
	iorlw	0x08	; Op 1 LED
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 1	
	iorlw	0x04	; Op 2 LED
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 2	
	iorlw	0x02	; Op 3 LED
	btfsc	CS_MENU_SELECTED_OP_FLAGS, 3	
	iorlw	0x01	; Op 4 LED
	movwf	MIOS_PARAMETER1
	rgoto CS_MENU_MATRIX_Handler_LED_Row_Cont
CS_MENU_MATRIX_Handler_LED_Row_Inst
	movlw	0x20	; Inst LED column is pin D2
	btfss	CS_MENU_MODE, CS_MENU_MODE_INS_OP	
	iorlw	0x10	; Inst LED
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 0
	iorlw	0x08	; Inst 1 LED
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 1	
	iorlw	0x04	; Inst 2 LED
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 2	
	iorlw	0x02	; Inst 3 LED
	btfsc	CS_MENU_SELECTED_INS_FLAGS, 3	
	iorlw	0x01	; Inst 4 LED
	movwf	MIOS_PARAMETER1
	;;rgoto CS_MENU_MATRIX_Handler_LED_Row_Cont
CS_MENU_MATRIX_Handler_LED_Row_Cont
	movlw	0x00	; 1st SR
	call MIOS_DOUT_SRSet
	return
#endif

	;; init table pointer, start with the DOUT pins for the columns
	TABLE_ADDR DIO_M_TABLE + 6

	;; process the 6 LED row lines
	clrf	IRQ_TMP1
CS_MENU_MATRIX_Handler_CLoop
	tblrd*+
	;; check if LED of current row has to be switched on or off
	decf	CS_MENU_MATRIX_SELECTED, W	; (dec because we have to prepare the next cycle where CTR is incremented)
	andlw	0x03
	cpfseq	CS_MENU_MATRIX_CTR, ACCESS
	rgoto CS_MENU_MATRIX_Handler_CLoop_0
CS_MENU_MATRIX_Handler_CLoop_1
	swapf	CS_MENU_MATRIX_SELECTED, W
	andlw	0x07
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CS_MENU_MATRIX_Handler_CLoop_0
	;; switch LED on
	movf	TABLAT, W	; contains the pin number of the DOUT
	call	MIOS_DOUT_PinSet1
	rgoto	CS_MENU_MATRIX_Handler_CLoopNext
	
CS_MENU_MATRIX_Handler_CLoop_0
	;; switch LED off
	movf	TABLAT, W	; contains the pin number of the DOUT
	call	MIOS_DOUT_PinSet0
	;; 	rgoto	CS_MENU_MATRIX_Handler_CLoopNext
CS_MENU_MATRIX_Handler_CLoopNext
	incf	IRQ_TMP1, F
	movlw	6-1
	cpfsgt	IRQ_TMP1, ACCESS
	rgoto CS_MENU_MATRIX_Handler_CLoop

	;; get number of multiplexed DIN
	tblrd*+
	movf	TABLAT, W

	;; get DIN value
	call	MIOS_DIN_PinGet

	;; set/clear flag in CS_MENU_MATRIX_RBUTTONS
	decf	CS_MENU_MATRIX_CTR, W
	andlw	0x03
	BRA_IFSET MIOS_PARAMETER1, 0, ACCESS, CS_MENU_MATRIX_Handler_Row1
CS_MENU_MATRIX_Handler_Row0
	call	MIOS_HLP_GetBitANDMask
	andwf	CS_MENU_MATRIX_RBUTTONS, F
	rgoto	CS_MENU_MATRIX_Handler_RowCont
CS_MENU_MATRIX_Handler_Row1
	call	MIOS_HLP_GetBitORMask
	iorwf	CS_MENU_MATRIX_RBUTTONS, F
	;; 	rgoto	CS_MENU_MATRIX_Handler_RowCont
CS_MENU_MATRIX_Handler_RowCont

	;; increment button counter, wrap at 4
	incf	CS_MENU_MATRIX_CTR, W
	andlw	0x03
	movwf	CS_MENU_MATRIX_CTR

	;; if counter is zero, and any row button selected, copy the new row button to CS_MENU_MATRIX_SELECTED[3..0]
 	bnz	CS_MENU_MATRIX_Handler_NoResR
CS_MENU_MATRIX_Handler_ResR
	;; check if a button has been changed
	swapf	CS_MENU_MATRIX_RBUTTONS, W
	xorwf	CS_MENU_MATRIX_RBUTTONS, W
	andlw	0x0f
	bz	CS_MENU_MATRIX_Handler_NoResR
	movwf	IRQ_TMP1	; changed mask in IRQ_TMP1

	;; copy current button state to last button state ([3..0] -> [7..4]
	swapf	CS_MENU_MATRIX_RBUTTONS, W
	andlw	0xf0
	movwf	IRQ_TMP2
	movlw	0x0f
	andwf	CS_MENU_MATRIX_RBUTTONS, F
	movf	IRQ_TMP2, W
	iorwf	CS_MENU_MATRIX_RBUTTONS, F

	;; button selected?
	movlw	0x00
	BRA_IFCLR IRQ_TMP1, 0, ACCESS, CS_MENU_MATRIX_Handler_ResR0Skip
CS_MENU_MATRIX_Handler_ResR0
	btfss	CS_MENU_MATRIX_RBUTTONS, 0
	movlw 0x80
CS_MENU_MATRIX_Handler_ResR0Skip

	BRA_IFCLR IRQ_TMP1, 1, ACCESS, CS_MENU_MATRIX_Handler_ResR1Skip
CS_MENU_MATRIX_Handler_ResR1
	btfss	CS_MENU_MATRIX_RBUTTONS, 1
	movlw 0x81
CS_MENU_MATRIX_Handler_ResR1Skip

	BRA_IFCLR IRQ_TMP1, 2, ACCESS, CS_MENU_MATRIX_Handler_ResR2Skip
CS_MENU_MATRIX_Handler_ResR2
	btfss	CS_MENU_MATRIX_RBUTTONS, 2
	movlw 0x82
CS_MENU_MATRIX_Handler_ResR2Skip

	BRA_IFCLR IRQ_TMP1, 3, ACCESS, CS_MENU_MATRIX_Handler_ResR3Skip
CS_MENU_MATRIX_Handler_ResR3
	btfss	CS_MENU_MATRIX_RBUTTONS, 3
	movlw 0x83
CS_MENU_MATRIX_Handler_ResR3Skip

	;; select new row if bit 7 of WREG set
	BRA_IFCLR WREG, 7, ACCESS, CS_MENU_MATRIX_Handler_NoResR
	movwf	IRQ_TMP1
	movlw	0xf0
	andwf	CS_MENU_MATRIX_SELECTED, F
	movf	IRQ_TMP1, W
	iorwf	CS_MENU_MATRIX_SELECTED, F
CS_MENU_MATRIX_Handler_NoResR

	;; go through the rest of the table, set all pins to 0 except for the
	;; pin which matches with the CTR number
	clrf	IRQ_TMP1
CS_MENU_MATRIX_Handler_RLoop
	tblrd*+
	movf	TABLAT, W
	call	MIOS_DOUT_PinSet1
	movf	CS_MENU_MATRIX_CTR, W
	cpfseq	IRQ_TMP1, ACCESS
	rgoto CS_MENU_MATRIX_Handler_RLoopNext
	movf	TABLAT, W
	call	MIOS_DOUT_PinSet0
CS_MENU_MATRIX_Handler_RLoopNext
	incf	IRQ_TMP1, F
	BRA_IFCLR IRQ_TMP1, 2, ACCESS, CS_MENU_MATRIX_Handler_RLoop

	return



;; --------------------------------------------------------------------------
;;  This function is called from CS_MENU_Handler and checks if a new row/column
;;  has been selected
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Update
	;; exit if nothing has been changed
	btfss	CS_MENU_MATRIX_SELECTED, 7
	return

	;; clear "changed" flag
	bcf	CS_MENU_MATRIX_SELECTED, 7

	;; get pointer to table entry depending on selected row/column
	rcall	CS_MENU_MATRIX_TABLE_GetPtr

	;; force a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; get mode
	tblrd*+
	;; (skip if mode 1: OP should not be changed via button!)
	decf	TABLAT, W
	bz	CS_MENU_MATRIX_Update_NoOPChng
	movf	TABLAT, W
	rcall	CS_MENU_MATRIX_Hlp_Mode
CS_MENU_MATRIX_Update_NoOPChng

	;; stop here if MJMP_N flag not active
	btfsc	CS_MENU_MODE, CS_MENU_MODE_MJMP_N
	return

	;; get menu
	tblrd*+
	movff	TABLAT, CS_MENU

	;; get cursor pos/offset of first entry
	tblrd*+
	movf	TABLAT, W
	andlw	0x0f
	movwf	SAVED_CS_MENU_CURSOR_POS
	swapf	TABLAT, W
	andlw	0x0f
	movwf	SAVED_CS_MENU_PAGE_OFFSET

	;; change to menu
	call	CS_MENU_Page_Init

	;; set cursor/page offset
	movff	SAVED_CS_MENU_CURSOR_POS, CS_MENU_CURSOR_POS
	movff	SAVED_CS_MENU_PAGE_OFFSET, CS_MENU_PAGE_OFFSET

	;; deselect parameter
	bsf	CS_STAT, CS_STAT_MODIFY_PARAMETER

	;; quickfix: if wavetable, ensure that parameters will be updated
	movlw	CS_MENU_WT
	cpfseq	CS_MENU, ACCESS
	rgoto CS_MENU_MATRIX_Update_NoWT
CS_MENU_MATRIX_Update_WT
	;; update WT buffer
	clrf	CS_MENU_WT_STEP
	call	CS_MENU_MS_Update_WT_Buffer
CS_MENU_MATRIX_Update_NoWT

	;; change to menu
	movf	CS_MENU, W
	goto	CS_MENU_EXEC_Hlp_ChangeMenu


;; --------------------------------------------------------------------------
;;  This function is called from CS_MENU_ENC_CS_Handler when an encoder
;;  has been moved
;;  IN: encoder number (-1) in MIOS_PARAMETER1
;;      incrementer in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Enc
	;; ensure that the encoder number is within 0-3
	movlw	0x03
	andwf	MIOS_PARAMETER1, F

	;; copy incrementer to MIOS_PARAMETER3
	movff	MIOS_PARAMETER2, MIOS_PARAMETER3

	;; get pointer to table entry depending on selected row/column
	rcall	CS_MENU_MATRIX_TABLE_GetPtr

	;; force a display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ

	;; get mode and do anything with it (see cs_menu_matrix_table.inc)
	tblrd*+
	movf	TABLAT, W
	rcall	CS_MENU_MATRIX_Hlp_Mode

	;; get menu
	tblrd*+
	movff	TABLAT, TMP1	; (-> CS_MENU)

	;; add offset to encoder entry
	movf	MIOS_PARAMETER1, W
	TABLE_ADD_W

	;; get cursor position and offset
	tblrd*+
	movf	TABLAT, W
	andlw	0x0f
	movwf	MIOS_PARAMETER1
	swapf	TABLAT, W
	andlw	0x0f
	movwf	MIOS_PARAMETER2

	;; inc/dec parameter
	movf	TMP1, W
	goto	CS_MENU_ENC_CSInc



;; --------------------------------------------------------------------------
;;  This function is doing special actions depending on the matrix mode
;; --------------------------------------------------------------------------
CS_MENU_MATRIX_Hlp_Mode
	andlw	0x07
	JUMPTABLE_2BYTES_UNSECURE
	rgoto	CS_MENU_MATRIX_Hlp_Mode0
	rgoto	CS_MENU_MATRIX_Hlp_Mode1
	rgoto	CS_MENU_MATRIX_Hlp_Mode2
	rgoto	CS_MENU_MATRIX_Hlp_Mode3
	rgoto	CS_MENU_MATRIX_Hlp_Mode4
	rgoto	CS_MENU_MATRIX_Hlp_Mode5
	rgoto	CS_MENU_MATRIX_Hlp_Mode6
	rgoto	CS_MENU_MATRIX_Hlp_Mode7

	;; mode 0: no special action
CS_MENU_MATRIX_Hlp_Mode0
	return

	;; mode 1: select the operator depending on the moved encoder
CS_MENU_MATRIX_Hlp_Mode1
	movf	MIOS_PARAMETER1, W
	movwf	CS_MENU_SELECTED_OP
	call	MIOS_HLP_GetBitORMask
	movwf	CS_MENU_SELECTED_OP_FLAGS
	swapf	CS_MENU_SELECTED_OP_FLAGS, W	; ([7..4] are the button state)
	xorlw	0xf0
	iorwf	CS_MENU_SELECTED_OP_FLAGS, F
	return

	;; mode 2: select LFO1
CS_MENU_MATRIX_Hlp_Mode2
	clrf	CS_MENU_SELECTED_LFO
	return

	;; mode 3: select LFO2
CS_MENU_MATRIX_Hlp_Mode3
	movlw	0x01
	movwf	CS_MENU_SELECTED_LFO
	return

	;; mode 4: select BD Modulator
CS_MENU_MATRIX_Hlp_Mode4
	bcf	CS_MENU_SELECTED_BD_HH, 0
	return

	;; mode 5: select BD Carrier
CS_MENU_MATRIX_Hlp_Mode5
	bsf	CS_MENU_SELECTED_BD_HH, 0
	return

	;; mode 6: select HH Open
CS_MENU_MATRIX_Hlp_Mode6
	bcf	CS_MENU_SELECTED_BD_HH, 1
	return

	;; mode 7: select HH Close
CS_MENU_MATRIX_Hlp_Mode7
	bsf	CS_MENU_SELECTED_BD_HH, 1
	return
