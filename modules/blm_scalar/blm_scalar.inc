; $Id$
;
; Button/Duo-LED Matrix Driver
; 
; ==========================================================================
;
;  Copyright 2010 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
; 
; Definitions can be optionally overruled from MAKEFILE.SPEC (therefore the #ifndef statements)
;
; define the shift registers to which the anodes of these LEDs are connected
#ifndef BLM_SCALAR_DOUT_GREEN1
#define BLM_SCALAR_DOUT_GREEN1	2
#endif
#ifndef BLM_SCALAR_DOUT_GREEN2
#define BLM_SCALAR_DOUT_GREEN2	5
#endif
#ifndef BLM_SCALAR_DOUT_GREEN3
#define BLM_SCALAR_DOUT_GREEN3	8
#endif
#ifndef BLM_SCALAR_DOUT_GREEN4
#define BLM_SCALAR_DOUT_GREEN4	11
#endif
#ifndef BLM_SCALAR_DOUT_GREEN5
#define BLM_SCALAR_DOUT_GREEN5	14
#endif
	
#ifndef BLM_SCALAR_DOUT_RED1
#define BLM_SCALAR_DOUT_RED1	3
#endif
#ifndef BLM_SCALAR_DOUT_RED2
#define BLM_SCALAR_DOUT_RED2	6
#endif
#ifndef BLM_SCALAR_DOUT_RED3
#define BLM_SCALAR_DOUT_RED3	9
#endif
#ifndef BLM_SCALAR_DOUT_RED4
#define BLM_SCALAR_DOUT_RED4	12
#endif
#ifndef BLM_SCALAR_DOUT_RED5
#define BLM_SCALAR_DOUT_RED5	15
#endif
;
; define the shift register to which the cathodes of these LEDs are connected
#ifndef BLM_SCALAR_DOUT_CATHODES1
#define BLM_SCALAR_DOUT_CATHODES1	1
#endif
#ifndef BLM_SCALAR_DOUT_CATHODES2
#define BLM_SCALAR_DOUT_CATHODES2	4
#endif
#ifndef BLM_SCALAR_DOUT_CATHODES3
#define BLM_SCALAR_DOUT_CATHODES3	7
#endif
#ifndef BLM_SCALAR_DOUT_CATHODES4
#define BLM_SCALAR_DOUT_CATHODES4	10
#endif
#ifndef BLM_SCALAR_DOUT_CATHODES5
#define BLM_SCALAR_DOUT_CATHODES5	13
#endif
;
; set an inversion mask for the DOUT shift registers if sink drivers (transistors)
; have been added to the cathode lines
; Settings: 0x00 - no sink drivers
;           0xff - sink drivers connected to D7..D0
#ifndef BLM_SCALAR_CATHODES_INV_MASK
#define BLM_SCALAR_CATHODES_INV_MASK	0xff
#endif
;
; define the DIN shift registers to which the button matrix is connected
#ifndef BLM_SCALAR_DIN1
#define BLM_SCALAR_DIN1		1
#endif
#ifndef BLM_SCALAR_DIN2
#define BLM_SCALAR_DIN2		2
#endif
#ifndef BLM_SCALAR_DIN3
#define BLM_SCALAR_DIN3		3
#endif
#ifndef BLM_SCALAR_DIN4
#define BLM_SCALAR_DIN4		4
#endif
#ifndef BLM_SCALAR_DIN5
#define BLM_SCALAR_DIN5		5
#endif
;
; set to 1 if buttons should not be debounced (debouncing allocates 256 extra bytes)
#ifndef BLM_SCALAR_NO_DEBOUNCING
#define BLM_SCALAR_NO_DEBOUNCING 1
#endif
;
;
;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_SCALAR_Init
;;  DESCRIPTION: This function initializes the scan matrix.
;;  It should be called from USER_Init
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
_BLM_SCALAR_Init ; (for C)
BLM_SCALAR_Init
	;; set button value to initial value (1) of all 256 buttons
	lfsr	FSR1, blm_scalar_button_row_values
	movlw	0x28
	movwf	PRODL
BLM_SCALAR_Init_Loop1
	setf	POSTINC1
	decfsz	PRODL, F
	rgoto	BLM_SCALAR_Init_Loop1

	;; clear "changed" status of all 320 buttons
	lfsr	FSR1, blm_scalar_button_row_changed
	movlw	0x28
	movwf	PRODL
BLM_SCALAR_Init_Loop2
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	BLM_SCALAR_Init_Loop2

#if BLM_SCALAR_NO_DEBOUNCING == 0
	;; initialise debounce counters of 320 buttons
	;; TODO: currently only prepared for 256
	lfsr	FSR1, blm_scalar_button_debounce_ctr
	clrf	PRODL
BLM_SCALAR_Init_Loop3
	setf	POSTINC1	; start with maximum delay
	decfsz	PRODL, F
	rgoto	BLM_SCALAR_Init_Loop3

	;; set initial debounce delay
	SET_BSR	_blm_scalar_button_debounce_delay
	movlw	0x2		; (-> 2 * 4 mS = 8 mS)
	movwf	_blm_scalar_button_debounce_delay, BANKED
#endif

	;; select first column
	movlw	0x00
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	xorlw	BLM_SCALAR_CATHODES_INV_MASK
	movwf	MIOS_PARAMETER1
#if BLM_SCALAR_DOUT_CATHODES1
	movlw	BLM_SCALAR_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES2
	movlw	BLM_SCALAR_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES3
	movlw	BLM_SCALAR_DOUT_CATHODES3 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES4
	movlw	BLM_SCALAR_DOUT_CATHODES4 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES5
	movlw	BLM_SCALAR_DOUT_CATHODES5 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; remember that this column has been selected
	SET_BSR	blm_scalar_selected_column
	clrf	blm_scalar_selected_column, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_SCALAR_PrepareCol
;;  DESCRIPTION: This function prepares the DOUT register to drive a column
;;  It should be called from USER_SR_Service_Prepare
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
_BLM_SCALAR_PrepareCol ; (for C)
BLM_SCALAR_PrepareCol
	;; increment column
	SET_BSR	blm_scalar_selected_column
	incf	blm_scalar_selected_column, F, BANKED
	bcf	blm_scalar_selected_column, 3, BANKED	; wrap at 8
	
	;; select next DOUT/DIN column
	movf	blm_scalar_selected_column, W, BANKED	; (* see note below)
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	MIOS_PARAMETER1

	;; finally apply inversion mask (required when sink drivers are connected to cathode lines)
	xorlw	BLM_SCALAR_CATHODES_INV_MASK
	movwf	MIOS_PARAMETER1

	;; output on CATHODES* registers
#if BLM_SCALAR_DOUT_CATHODES1
	movlw	BLM_SCALAR_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES2
	movlw	BLM_SCALAR_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES3
	movlw	BLM_SCALAR_DOUT_CATHODES3 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES4
	movlw	BLM_SCALAR_DOUT_CATHODES4 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_SCALAR_DOUT_CATHODES5
	movlw	BLM_SCALAR_DOUT_CATHODES5 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_GREEN1
	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_green + 0*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_GREEN1 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_RED1
	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_red + 0*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_RED1 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_GREEN2
	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_green + 1*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_GREEN2 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_RED2
	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_red + 1*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_RED2 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_GREEN3
	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_green + 2*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_GREEN3 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_RED3
	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_red + 2*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_RED3 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_GREEN4
	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_green + 3*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_GREEN4 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_RED4
	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_red + 3*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_RED4 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_GREEN5
	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_green + 4*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_GREEN5 - 1
	call	MIOS_DOUT_SRSet
#endif

#if BLM_SCALAR_DOUT_RED5
	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_scalar_row_red + 4*8
	movf	blm_scalar_selected_column, W
	movff	PLUSW2, MIOS_PARAMETER1
	movlw	BLM_SCALAR_DOUT_RED5 - 1
	call	MIOS_DOUT_SRSet
#endif

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_SCALAR_GetRow
;;  DESCRIPTION: This function gets the DIN values of the selected column
;;  It should be called from USER_SR_Service_Finish
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
_BLM_SCALAR_GetRow ; (for C)
BLM_SCALAR_GetRow
	;; since the row line of the buttons is identical to the row line of the LEDs,
	;; we can derive the button row offset from blm_scalar_selected_column:
	SET_BSR blm_scalar_selected_column
	decf	blm_scalar_selected_column, W, BANKED
	andlw	0x07
	movwf	IRQ_TMP1

#if BLM_SCALAR_DIN1
	movlw	BLM_SCALAR_DIN1 - 1
	rcall	BLM_SCALAR_GetRow_Sub
#endif

	movlw	8
	addwf	IRQ_TMP1, F
#if BLM_SCALAR_DIN2
	movlw	BLM_SCALAR_DIN2 - 1
	rcall	BLM_SCALAR_GetRow_Sub
#endif

	movlw	8
	addwf	IRQ_TMP1, F
#if BLM_SCALAR_DIN3
	movlw	BLM_SCALAR_DIN3 - 1
	rcall	BLM_SCALAR_GetRow_Sub
#endif

	movlw	8
	addwf	IRQ_TMP1, F
#if BLM_SCALAR_DIN4
	movlw	BLM_SCALAR_DIN4 - 1
	rcall	BLM_SCALAR_GetRow_Sub
#endif

	movlw	8
	addwf	IRQ_TMP1, F
#if BLM_SCALAR_DIN5
	movlw	BLM_SCALAR_DIN5 - 1
	rcall	BLM_SCALAR_GetRow_Sub
#endif

	return


	;; expecting: DIN SR number in WREG
	;;            row counter in IRQ_TMP1
BLM_SCALAR_GetRow_Sub
	;; get 8bit value of selected DIN register -> MIOS_PARAMETER1
	call	MIOS_DIN_SRGet

	;; debounce handling: each pin has to be handled individually by BLM_SCALAR_GetRow_PinSub

	;; determine initial offset to debounce counter array -> FSR2
#if BLM_SCALAR_NO_DEBOUNCING == 0
	lfsr	FSR2, blm_scalar_button_debounce_ctr
	rlf	IRQ_TMP1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0xf8
	addwf	FSR2L, F
#endif

	;; call pin handler 8 times
	movlw	(1 << 0)	; mask for pin #0
	rcall	BLM_SCALAR_GetRow_PinSub
	
	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 1)	; mask for pin #1
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 2)	; mask for pin #2
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 3)	; mask for pin #3
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 4)	; mask for pin #4
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 5)	; mask for pin #5
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 6)	; mask for pin #6
	rcall	BLM_SCALAR_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 7)	; mask for pin #7
	;; 	rgoto	BLM_SCALAR_GetRow_PinSub

BLM_SCALAR_GetRow_PinSub
	movwf	IRQ_TMP2	; store mask in IRQ_TMP2

#if BLM_SCALAR_NO_DEBOUNCING == 0
	;; if debounce counter > 0: decrement it and exit (ignore bouncing values)
	movf	INDF2, W
	bz	BLM_SCALAR_GetRow_PinSub_Debounced
BLM_SCALAR_GetRow_PinSub_Ignore
	decf	INDF2, F
	return
#endif


BLM_SCALAR_GetRow_PinSub_Debounced
	;; transfer new value to blm_scalar_button_row_values[] array
	lfsr	FSR1, blm_scalar_button_row_values
	movf	IRQ_TMP1, W		; row index
	addwf	FSR1L, F

	;; temporary store old value in IRQ_TMP3
	movff	INDF1, IRQ_TMP3

	comf	IRQ_TMP2, W		; clear bit with inverted pin mask...
	andwf	INDF1, F
	movf	IRQ_TMP2, W		; ...and if DIN pin is set...
	andwf	MIOS_PARAMETER1, W
	skpz				; ...set bit with pin mask
	iorwf	INDF1, F

	;; exit if value hasn't been changed
	movf	IRQ_TMP3, W		; old value
	xorwf	MIOS_PARAMETER1, W	; XOR with new value
	andwf	IRQ_TMP2, W		; apply pin mask
	bz	BLM_SCALAR_GetRow_PinSub_End	; if 0 -> no change

BLM_SCALAR_GetRow_PinSub_Change
	;; notify pin change in blm_scalar_button_row_changed array
	lfsr	FSR1, blm_scalar_button_row_changed
	movf	IRQ_TMP1, W		; row index
	addwf	FSR1L, F

	movf	IRQ_TMP2, W		; pin mask
	iorwf	INDF1, F		; OR with other bits

#if BLM_SCALAR_NO_DEBOUNCING == 0
	;; re-load debounce delay
	SET_BSR	_blm_scalar_button_debounce_delay
	movf	_blm_scalar_button_debounce_delay, W, BANKED
	movwf	INDF2
#endif
BLM_SCALAR_GetRow_PinSub_End

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_SCALAR_ButtonHandler
;;  DESCRIPTION: This function should be called from USER_Tick (in the
;;  mainprogram) to ask for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified via the BLM_SCALAR_NotifyToggle hook
;;  which behaves similar to the USER_DIN_NotifyToggle hook provided by
;;  MIOS itself
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
_BLM_SCALAR_ButtonHandler ; (for C)
BLM_SCALAR_ButtonHandler
	SET_BSR	blm_scalar_button_column_ctr
	clrf	blm_scalar_button_column_ctr, BANKED
BLM_SCALAR_ButtonHandler_Loop
	lfsr	FSR1, blm_scalar_button_row_changed
	movf	blm_scalar_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	INDF1, W
	bz	BLM_SCALAR_ButtonHandler_Loop_Next
	btfsc	INDF1, 0		; fastest solution...
	rcall	BLM_SCALAR_NotifyToggle_Pin0
	btfsc	INDF1, 1
	rcall	BLM_SCALAR_NotifyToggle_Pin1
	btfsc	INDF1, 2
	rcall	BLM_SCALAR_NotifyToggle_Pin2
	btfsc	INDF1, 3
	rcall	BLM_SCALAR_NotifyToggle_Pin3
	btfsc	INDF1, 4
	rcall	BLM_SCALAR_NotifyToggle_Pin4
	btfsc	INDF1, 5
	rcall	BLM_SCALAR_NotifyToggle_Pin5
	btfsc	INDF1, 6
	rcall	BLM_SCALAR_NotifyToggle_Pin6
	btfsc	INDF1, 7
	rcall	BLM_SCALAR_NotifyToggle_Pin7
BLM_SCALAR_ButtonHandler_Loop_Next
	incf	blm_scalar_button_column_ctr, F, BANKED
	movlw	0x28-1		; (8 columns)
	cpfsgt	blm_scalar_button_column_ctr, BANKED
	rgoto	BLM_SCALAR_ButtonHandler_Loop
	return

	;; ---

BLM_SCALAR_NotifyToggle_Pin0
	bcf	INDF1, 0			; clear "changed" flag
	movlw	0x00
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin1
	bcf	INDF1, 1			; clear "changed" flag
	movlw	0x01
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin2
	bcf	INDF1, 2			; clear "changed" flag
	movlw	0x02
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin3
	bcf	INDF1, 3			; clear "changed" flag
	movlw	0x03
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin4
	bcf	INDF1, 4			; clear "changed" flag
	movlw	0x04
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin5
	bcf	INDF1, 5			; clear "changed" flag
	movlw	0x05
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin6
	bcf	INDF1, 6			; clear "changed" flag
	movlw	0x06
	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin7
	bcf	INDF1, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	BLM_SCALAR_NotifyToggle_Pin_Cont
BLM_SCALAR_NotifyToggle_Pin_Cont
	movwf	_blm_scalar_button_column, BANKED

	;; store pin value in BLM_SCALAR_BUTTON_VALUE
	lfsr	FSR1, blm_scalar_button_row_values
	movf	blm_scalar_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	_blm_scalar_button_column, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	movwf	_blm_scalar_button_value

	;; for compatibility with USER_DIN_NotifyToggle:
	;; save button value in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; save button number (0x00-0xff) in MIOS_PARAMETER1
	;; if more than 256 buttons are connected, use BLM_SCALAR_BUTTON_COLUMN and BLM_SCALAR_BUTTON_ROW to determine the number
	movf	blm_scalar_button_column_ctr, W, BANKED
	movwf	_blm_scalar_button_row, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	_blm_scalar_button_column, W, BANKED

	;; call C hook
	movwf	MIOS_PARAMETER1			; button number still in WREG
	movff	_blm_scalar_button_value, POSTDEC0	; pin value
	call	_BLM_SCALAR_NotifyToggle
	incf	FSR0L, F			; restore stack pointer

	lfsr	FSR1, blm_scalar_button_row_changed	; fix FSR1 if it has been overwritten
	SET_BSR	blm_scalar_button_column_ctr
	movf	blm_scalar_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	return

; ==========================================================================
	END
