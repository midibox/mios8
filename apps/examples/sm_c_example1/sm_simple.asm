; $Id$
;
; Scan Matrix Driver, simple version
; 
; This driver demonstrates how to scan a matrix of up to 8x8 buttons/keys
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
;
;    Following function is located in main.c:
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
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	list	p=18f452
	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	_SM_Init		; (compiler removes the "_", see sm_simple.h)
	global	_SM_PrepareCol
	global	_SM_GetRow
	global	_SM_ButtonHandler

	global	_sm_button_column
	global	_sm_button_row
	global	_sm_button_value

	;; import lables
	extern	_SM_NotifyToggle

; ==========================================================================

accessram		udata		; (no access ram required, these variables can be located anywhere)

_sm_button_column	res	1	; exported to C, therefore an "_" has been added
_sm_button_row		res	1
_sm_button_value	res	1

sm_selected_column	res	1
sm_button_column_ctr	res	1

sm_row_values		res	8
sm_row_changed		res	8

; ==========================================================================
SM_SIMPLE code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: SM_Init
;;  DESCRIPTION: This function initializes the scan matrix.
;;  It should be called from USER_Init
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
_SM_Init ; (for C)
SM_Init
	;; set button value to initial value (1) of all 64 buttons
	lfsr	FSR1, sm_row_values
	movlw	0x08
	movwf	PRODL
SM_Init_Loop1
	setf	POSTINC1
	decfsz	PRODL, F
	rgoto	SM_Init_Loop1

	;; clear "changed" status of all 64 buttons
	lfsr	FSR1, sm_row_changed
	movlw	0x08
	movwf	PRODL
SM_Init_Loop2
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	SM_Init_Loop2

	;; select first column
	movlw	0x00
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	MIOS_PARAMETER1
	movlw	SM_COLUMN_DOUT
	call	MIOS_DOUT_SRSet

	;; remember that this column has been selected
	SET_BSR	sm_selected_column
	clrf	sm_selected_column, BANKED

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
_SM_PrepareCol ; (for C)
SM_PrepareCol
	;; select next DOUT register

	;; (current column + 1) & 0x07
	SET_BSR	sm_selected_column
	incf	sm_selected_column, W, BANKED	; (* see note below)
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
_SM_GetRow ; (for C)
SM_GetRow
	;; determine changes and save new value in SW_ROWx_VALUE depending on selected column
	movlw	SM_ROW_DIN
	call	MIOS_DIN_SRGet
	lfsr	FSR1, sm_row_values
	SET_BSR	sm_selected_column
	movf	sm_selected_column, W, BANKED
	movff	PLUSW1, IRQ_TMP1
	movff	MIOS_PARAMETER1, PLUSW1

	;; old value in IRQ_TMP1
	;; new value in MIOS_PARAMETER1
	;; select SM_ROWx_CHANGED register
	lfsr	FSR1, sm_row_changed
	movf	sm_selected_column, W, BANKED
	addwf	FSR1L, F

	;; changes are: IRQ_TMP1 xor MIOS_PARAMETER1
	movf	IRQ_TMP1, W
	xorwf	MIOS_PARAMETER1, W

	;; OR the result to the already notified changes (if they has not been zeroed by the ButtonHandler)
	iorwf	INDF1, F

	;; increment column, wrap at 8
	incf	sm_selected_column, W, BANKED
	andlw	0x07
	movwf	sm_selected_column, BANKED
	
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
_SM_ButtonHandler ; (for C)
SM_ButtonHandler
	SET_BSR	sm_button_column_ctr
	clrf	sm_button_column_ctr, BANKED
SM_ButtonHandler_Loop
	lfsr	FSR1, sm_row_changed
	movf	sm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	INDF1, W
	bz	SM_ButtonHandler_Loop_Next
	btfsc	INDF1, 0		; fastest solution...
	rcall	SM_NotifyToggle_Pin0
	btfsc	INDF1, 1
	rcall	SM_NotifyToggle_Pin1
	btfsc	INDF1, 2
	rcall	SM_NotifyToggle_Pin2
	btfsc	INDF1, 3
	rcall	SM_NotifyToggle_Pin3
	btfsc	INDF1, 4
	rcall	SM_NotifyToggle_Pin4
	btfsc	INDF1, 5
	rcall	SM_NotifyToggle_Pin5
	btfsc	INDF1, 6
	rcall	SM_NotifyToggle_Pin6
	btfsc	INDF1, 7
	rcall	SM_NotifyToggle_Pin7
SM_ButtonHandler_Loop_Next
	incf	sm_button_column_ctr, F, BANKED
	movlw	0x08-1		; (8 columns)
	cpfsgt	sm_button_column_ctr, BANKED
	rgoto	SM_ButtonHandler_Loop
	return

	;; ---

SM_NotifyToggle_Pin0
	bcf	INDF1, 0			; clear "changed" flag
	movlw	0x00
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin1
	bcf	INDF1, 1			; clear "changed" flag
	movlw	0x01
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin2
	bcf	INDF1, 2			; clear "changed" flag
	movlw	0x02
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin3
	bcf	INDF1, 3			; clear "changed" flag
	movlw	0x03
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin4
	bcf	INDF1, 4			; clear "changed" flag
	movlw	0x04
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin5
	bcf	INDF1, 5			; clear "changed" flag
	movlw	0x05
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin6
	bcf	INDF1, 6			; clear "changed" flag
	movlw	0x06
	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin7
	bcf	INDF1, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	SM_NotifyToggle_Pin_Cont
SM_NotifyToggle_Pin_Cont
	movwf	_sm_button_row, BANKED

	;; store pin value in SM_BUTTON_VALUE
	lfsr	FSR1, sm_row_values
	movf	sm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	_sm_button_row, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	movwf	_sm_button_value

	;; for compatibility with USER_DIN_NotifyToggle:
	;; save button value in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; save button number (0x00-0xff) in MIOS_PARAMETER1
	;; if more than 256 buttons are connected, use SM_BUTTON_COLUMN and SM_BUTTON_ROW to determine the number
#if SM_SWAP_CR
	movf	sm_button_column_ctr, W, BANKED
	movwf	_sm_button_column, BANKED
	movf	_sm_button_row, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	_sm_button_column, W, BANKED	
#else
	movf	sm_button_column_ctr, W, BANKED
	movwf	_sm_button_column, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	_sm_button_row, W, BANKED
#endif

	;; call C hook
	movwf	MIOS_PARAMETER1			; button number still in WREG
	movff	_sm_button_value, POSTDEC0	; pin value
	call	_SM_NotifyToggle
	incf	FSR0L, F			; restore stack pointer

	lfsr	FSR1, sm_row_changed	; fix FSR1 if it has been overwritten
	SET_BSR	sm_button_column_ctr
	movf	sm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	return

; ==========================================================================
	END
