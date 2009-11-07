; $Id: sm_simple.inc 326 2008-05-03 21:20:32Z tk $
; Scan Matrix Driver, simple version
; Alias sm_simple_c64keyb.inc - driver for the C64 Keyboard
; 
; This driver scans a matrix of up to 8x8 buttons/keys
; by using one DOUT and one DIN shift register
; 
; This include file provides following functions:
;
;    o SM_Init:         initializes the scan matrix
;                       should be called from USER_Init
;
;    o SM_PrepareCol:   prepares the DOUT register to drive a column
;                       should be called from USER_SR_Service_Prepare
;
;    o SM_GetRow:       gets the DIN values of the selected column
;                       should be called from USER_SR_Service_Finish
;
;    o SM_ButtonHandler: to be called from USER_Tick
;
;    o SM_NotifyToggle: called from SM_ButtonHandler on button changes
;
;
; Define the shift register which drives the column here
; we are counting from zero! The first DOUT register is 0x00, the second 0x01, etc...
#define SM_COLUMN_DOUT	0x00
;
; Define the shift register which gets the row values here
; we are counting from zero! The first DIN register is 0x00, the second 0x01, etc...
#define SM_ROW_DIN	0x00
;
; set this constant to 1 if the colums/rows are swapped (means: the cathodes and
; not the anodes of the diodes are connected together)
#define SM_SWAP_CR	0
;
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: SM_Init
;;  DESCRIPTION: This function initializes the scan matrix.
;;  It should be called from USER_Init
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
SM_Init
	;; set button value to initial value (1) of all 64 buttons
	lfsr	FSR0, SM_ROW0_VALUE
	movlw	0x08
	movwf	TMP1
SM_Init_Loop1
	setf	POSTINC0
	decfsz	TMP1, F
	rgoto	SM_Init_Loop1

	;; clear "changed" status of all 64 buttons
	lfsr	FSR0, SM_ROW0_CHANGED
	movlw	0x08
	movwf	TMP1
SM_Init_Loop2
	clrf	POSTINC0
	decfsz	TMP1, F
	rgoto	SM_Init_Loop2
	
	;; select first column
	movlw	0x00
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	MIOS_PARAMETER1
	movlw	SM_COLUMN_DOUT
	call	MIOS_DOUT_SRSet

	;; remember that this column has been selected
	SET_BSR	SM_SELECTED_COLUMN
	clrf	SM_SELECTED_COLUMN, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SM_PrepareCol
;;  DESCRIPTION: This function prepares the DOUT register to drive a column
;;  It should be called from USER_SR_Service_Prepare
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
SM_PrepareCol
	;; select next DOUT register

	;; (current column + 1) & 0x07
	SET_BSR	SM_SELECTED_COLUMN
	incf	SM_SELECTED_COLUMN, W, BANKED	; (* see note below)
	andlw	0x07
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	MIOS_PARAMETER1
	movlw	SM_COLUMN_DOUT
	call	MIOS_DOUT_SRSet

	;; (*) note: the incremented "selected column" value won't be saved here, but
	;; after SM_GetRow has been executed. 
	;; Why?
	;; because the new DOUT value will be "visible" after the DIN chain has
	;; been captured by the SRIO driver, therefore the column which has been
	;; selected here will be visible during the upper next scan
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SM_GetRow
;;  DESCRIPTION: This function gets the DIN values of the selected column
;;  It should be called from USER_SR_Service_Finish
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
SM_GetRow
	;; determine changes and save new value in SW_ROWx_VALUE depending on selected column
	movlw	SM_ROW_DIN
	call	MIOS_DIN_SRGet
	lfsr	FSR2, SM_ROW0_VALUE
	SET_BSR	SM_SELECTED_COLUMN	
	movf	SM_SELECTED_COLUMN, W, BANKED
	movff	PLUSW2, IRQ_TMP1
	movff	MIOS_PARAMETER1, PLUSW2

	;; old value in IRQ_TMP1
	;; new value in MIOS_PARAMETER1
	;; select SM_ROWx_CHANGED register
	lfsr	FSR2, SM_ROW0_CHANGED
	movf	SM_SELECTED_COLUMN, W, BANKED
	addwf	FSR2L, F

	;; changes are: IRQ_TMP1 xor MIOS_PARAMETER1
	movf	IRQ_TMP1, W
	xorwf	MIOS_PARAMETER1, W

	;; OR the result to the already notified changes (if they has not been zeroed by the ButtonHandler)
	iorwf	INDF2, F

	;; increment column, wrap at 8
	incf	SM_SELECTED_COLUMN, W, BANKED
	andlw	0x07
	movwf	SM_SELECTED_COLUMN, BANKED
	
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: SM_ButtonHandler
;;  DESCRIPTION: This function should be called from USER_Tick (in the
;;  mainprogram) to ask for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified via the SM_NotifyToggle hook
;;  which behaves similar to the USER_DIN_NotifyToggle hook provided by
;;  MIOS itself
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
SM_ButtonHandler
	SET_BSR	SM_BUTTON_COLUMN_CTR
	clrf	SM_BUTTON_COLUMN_CTR, BANKED
SM_ButtonHandler_Loop
	lfsr	FSR0, SM_ROW0_CHANGED
	movf	SM_BUTTON_COLUMN_CTR, W, BANKED
	addwf	FSR0L, F
	movf	INDF0, W
	bz	SM_ButtonHandler_Loop_Next
	RCALL_IFSET INDF0, 0, ACCESS, SM_NotifyToggle_Pin0	; fastest solution...
	RCALL_IFSET INDF0, 1, ACCESS, SM_NotifyToggle_Pin1
	RCALL_IFSET INDF0, 2, ACCESS, SM_NotifyToggle_Pin2
	RCALL_IFSET INDF0, 3, ACCESS, SM_NotifyToggle_Pin3
	RCALL_IFSET INDF0, 4, ACCESS, SM_NotifyToggle_Pin4
	RCALL_IFSET INDF0, 5, ACCESS, SM_NotifyToggle_Pin5
	RCALL_IFSET INDF0, 6, ACCESS, SM_NotifyToggle_Pin6
	RCALL_IFSET INDF0, 7, ACCESS, SM_NotifyToggle_Pin7
SM_ButtonHandler_Loop_Next
	incf	SM_BUTTON_COLUMN_CTR, F, BANKED
	movlw	0x08-1		; (8 columns)
	cpfsgt	SM_BUTTON_COLUMN_CTR, BANKED
	rgoto SM_ButtonHandler_Loop
	return

	;; ---

SM_NotifyToggle_Pin0
	bcf	INDF0, 0			; clear "changed" flag
	movlw	0x00
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin1
	bcf	INDF0, 1			; clear "changed" flag
	movlw	0x01
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin2
	bcf	INDF0, 2			; clear "changed" flag
	movlw	0x02
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin3
	bcf	INDF0, 3			; clear "changed" flag
	movlw	0x03
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin4
	bcf	INDF0, 4			; clear "changed" flag
	movlw	0x04
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin5
	bcf	INDF0, 5			; clear "changed" flag
	movlw	0x05
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin6
	bcf	INDF0, 6			; clear "changed" flag
	movlw	0x06
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin7
	bcf	INDF0, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin_Cont
	movwf	SM_BUTTON_ROW, BANKED

	;; store pin value in SM_BUTTON_VALUE
	lfsr	FSR0, SM_ROW0_VALUE
	movf	SM_BUTTON_COLUMN_CTR, W, BANKED
	addwf	FSR0L, F
	movf	SM_BUTTON_ROW, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF0, W
	skpz
	movlw	0x01
	movwf	SM_BUTTON_VALUE

	;; for compatibility with USER_DIN_NotifyToggle:
	;; save button value in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; save button number (0x00-0xff) in MIOS_PARAMETER1
	;; if more than 256 buttons are connected, use SM_BUTTON_COLUMN and SM_BUTTON_ROW to determine the number
#if SM_SWAP_CR
	movf	SM_BUTTON_COLUMN_CTR, W, BANKED
	movwf	SM_BUTTON_COLUMN, BANKED
	movf	SM_BUTTON_ROW, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	SM_BUTTON_COLUMN, W, BANKED	
#else
	movf	SM_BUTTON_COLUMN_CTR, W, BANKED
	movwf	SM_BUTTON_COLUMN, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	SM_BUTTON_ROW, W, BANKED
#endif

	movwf	MIOS_PARAMETER1
	;; call hook
	call	SM_NotifyToggle

	lfsr	FSR0, SM_ROW0_CHANGED	; fix FSR0 if it has been overwritten
	SET_BSR	SM_BUTTON_COLUMN_CTR
	movf	SM_BUTTON_COLUMN_CTR, W, BANKED
	addwf	FSR0L, F
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SM_NotifyToggle
;;  DESCRIPTION: This function is NOT called by MIOS, but by the scan matrix
;;  handler when a pin of the scan matrix has been toggled
;;  IN: Button number in WREG and MIOS_PARAMETER1
;;      Column in SM_BUTTON_COLUMN
;;      Row in SM_BUTTON_ROW
;;      Button value MIOS_PARAMETER2:
;;        - 1 if button has been released (=5V)
;;        - 0 if button has been pressed (=0V)
;;  OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------
SM_NotifyToggle
	
	;; MEMO by TK: here you could also branch directly to USER_DIN_NotifyToggle in order
	;; to emulate buttons which are connected on the common way to the shift registers
	;;
	;; 
	;; MEMO: A BUTTON HAS BEEN PRESSED 
	;; We branch directly to SM_C64_KEY_NotifyToggle
	;;
	;; Button number (0x00-0xff) in MIOS_PARAMETER1
	;; Button value  (0x00-0x01) in MIOS_PARAMETER2
	;; 1) WRITE BUTTON NUMBER to SM_DRIVER_KEYBOARD_KEY
	;; 2) WRITE Button value
	;; => 0 = BUTTON IS PRESSED
	;; => 1 = BUTTON IS DEPRESSED (!)
	;; TO AY_KEY_STATE
	;; BOTH USED @ sub_menu_functions.inc etc.
	movff 	MIOS_PARAMETER1, SM_DRIVER_KEYBOARD_KEY
	movff 	MIOS_PARAMETER2, SM_KEYB_KEY_STATE
		
		;; => main.asm
		goto SM_C64_KEY_NotifyToggle

		

