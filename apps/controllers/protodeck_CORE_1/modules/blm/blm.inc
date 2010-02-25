; $Id: blm.inc 379 2008-07-06 23:17:18Z tk $
;
; Button/Duo-LED Matrix Driver
; 
; ==========================================================================
;
;  Copyright 2008 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
; 
; This driver handles a 4x16 Button/Duo-LED scan matrix of up to 64 buttons and 64 Duo-LEDs
; by using the circuit documented under:
;   http://www.ucapps.de/mbhp/button_duoled_matrix.pdf
; 
; The hardware was originally used for MIDIbox SEQ V3, but could also be useful for
; other projects.
; 
; Note that the buttons/LEDs can be easily arranged to a 8x8 matrix w/o software changes.
; 
; Please note also, that the shift register assignments below (DEFAULT_*) need to be adapted
; to your hardware. The setup used in this example is prepared for connecting the 
; DIN/DOUT chain directly to the MBHP_CORE module (there are no additional DINX4 or DOUTX4
; modules between the BLM and the core like on the MBSEQ V3 hardware).
; 
; This include file provides following functions:
;
;    o BLM_Init:        initializes the scan matrix
;                       should be called from USER_Init
;
;    o BLM_PrepareCol:  prepares the DOUT register to drive a column
;                       should be called from USER_SR_Service_Prepare
;
;    o BLM_GetRow:      gets the DIN values of the selected column
;                       should be called from USER_SR_Service_Finish
;
;    o BLM_ButtonHandler: to be called from USER_Tick
;
;    Following function is located in main.c:
;    o BLM_NotifyToggle: called from SM_ButtonHandler on button changes
;
; Buttons can be debounced with a definable delay (-> blm_button_debounce_delay)
; The resulting delay is calculated as <value> * 4 mS
; BLM_Init() sets the value to 2 (-> 2*4 mS = 8 mS) by default, it can be changed
; outside this file (variable is declared as global)
;
; Define the shift register which gets the row values here
; we are counting from zero! The first DIN register is 0x00, the second 0x01, etc...
; Note:	for following settings (and in difference to the sm_*.asm examples!) we are
; not counting shift registers from zero anymore, but from 1
; The first DIN/DOUT register is 1, the second 2, etc...
; A 0 disables the DIN/DOUT assignment (not recommented here)
;
; Definitions can be optionally overruled from MAKEFILE.SPEC (therefore the #ifndef statements)
;
; define the shift registers to which the anodes of these LEDs are connected
; Note: they can be equal to DEFAULT_GP_DOUT_SR_[LH], this saves two shift registers, but doesn't allow a separate view of UI selections
#ifndef BLM_DOUT_L1
#define BLM_DOUT_L1	2
#endif
#ifndef BLM_DOUT_R1
#define BLM_DOUT_R1	5
#endif
;
; define the shift register to which the cathodes of these LEDs are connected
; Note that the whole shift register (8 pins) will be allocated! The 4 select lines are duplicated (4 for LED matrix, 4 for button matrix)
; The second DOUT_CATHODES2 selection is optional if LEDs with high power consumption are used - set this to 0 if not used
#ifndef BLM_DOUT_CATHODES1
#define BLM_DOUT_CATHODES1	1
#endif
#ifndef BLM_DOUT_CATHODES2
#define BLM_DOUT_CATHODES2	4
#endif
;
; set an inversion mask for the DOUT shift registers if sink drivers (transistors)
; have been added to the cathode lines
; Settings: 0x00 - no sink drivers
;           0xf0 - sink drivers connected to D0..D3
;           0x0f - sink drivers connected to D7..D4
#ifndef BLM_CATHODES_INV_MASK
#define BLM_CATHODES_INV_MASK	0x00
#endif
;
; define the shift registers to which the anodes of the "second colour" (red) LEDs are connected
#ifndef BLM_DOUT_L2
#define BLM_DOUT_L2	3
#endif
#ifndef BLM_DOUT_R2
#define BLM_DOUT_R2	6
#endif
;
; define the DIN shift registers to which the button matrix is connected
#ifndef BLM_DIN_L
#define BLM_DIN_L	1
#endif
#ifndef BLM_DIN_R
#define BLM_DIN_R	2
#endif
;
; set to 1 if buttons should not be debounced (debouncing allocates 64 extra bytes)
#ifndef BLM_NO_DEBOUNCING
#define BLM_NO_DEBOUNCING 0
#endif
;
;
;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_Init
;;  DESCRIPTION: This function initializes the scan matrix.
;;  It should be called from USER_Init
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
_BLM_Init ; (for C)
BLM_Init
	;; set button value to initial value (1) of all 64 buttons
	lfsr	FSR1, blm_button_row_values
	movlw	0x08
	movwf	PRODL
BLM_Init_Loop1
	setf	POSTINC1
	decfsz	PRODL, F
	rgoto	BLM_Init_Loop1

	;; clear "changed" status of all 64 buttons
	lfsr	FSR1, blm_button_row_changed
	movlw	0x08
	movwf	PRODL
BLM_Init_Loop2
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	BLM_Init_Loop2

#if BLM_NO_DEBOUNCING == 0
	;; initialise debounce counters of 64 buttons
	lfsr	FSR1, blm_button_debounce_ctr
	movlw	0x40
	movwf	PRODL
BLM_Init_Loop3
	setf	POSTINC1	; start with maximum delay
	decfsz	PRODL, F
	rgoto	BLM_Init_Loop3

	;; set initial debounce delay
	SET_BSR	_blm_button_debounce_delay
	movlw	0x2		; (-> 2 * 4 mS = 8 mS)
	movwf	_blm_button_debounce_delay, BANKED
#endif

	;; select first column
	movlw	0x00
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	MIOS_PARAMETER1
#if BLM_DOUT_CATHODES1
	movlw	BLM_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_DOUT_CATHODES2
	movlw	BLM_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; remember that this column has been selected
	SET_BSR	blm_selected_column
	clrf	blm_selected_column, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_PrepareCol
;;  DESCRIPTION: This function prepares the DOUT register to drive a column
;;  It should be called from USER_SR_Service_Prepare
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
_BLM_PrepareCol ; (for C)
BLM_PrepareCol
	;; increment column
	SET_BSR	blm_selected_column
	incf	blm_selected_column, F, BANKED
	bcf	blm_selected_column, 2, BANKED	; wrap at 4
	
	;; select next DOUT/DIN column
	movf	blm_selected_column, W, BANKED	; (* see note below)
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	andlw	0x0f
	movwf	MIOS_PARAMETER1

	;; duplicate the 4 selection lines for button matrix
	swapf	WREG, W
 	iorwf	MIOS_PARAMETER1, W

	;; finally apply inversion mask (required when sink drivers are connected to cathode lines)
	xorlw	BLM_CATHODES_INV_MASK
	movwf	MIOS_PARAMETER1

	;; output on CATHODES* registers
#if BLM_DOUT_CATHODES1
	movlw	BLM_DOUT_CATHODES1 - 1
	call	MIOS_DOUT_SRSet
#endif
#if BLM_DOUT_CATHODES2
	movlw	BLM_DOUT_CATHODES2 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; store row*2 offset in IRQ_TMP1 to simplify the addressing of blm_row_green and blm_row_red array
	SET_BSR	blm_selected_column
	movf	blm_selected_column, W, BANKED
	rlf	WREG, W
	andlw	0x06
	movwf	IRQ_TMP1

	;; output value of "green LED row" depending on current column
	lfsr	FSR2, _blm_row_green

	;; left side
	movf	IRQ_TMP1, W
	movff	PLUSW2, MIOS_PARAMETER1
#if BLM_DOUT_L1
	movlw	BLM_DOUT_L1 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; right side
	incf	IRQ_TMP1, W
	movff	PLUSW2, MIOS_PARAMETER1
#if BLM_DOUT_R1
	movlw	BLM_DOUT_R1 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; output value of "red LED row" depending on current column
	lfsr	FSR2, _blm_row_red

	;; left side
	movf	IRQ_TMP1, W
	movff	PLUSW2, MIOS_PARAMETER1
#if BLM_DOUT_L2
	movlw	BLM_DOUT_L2 - 1
	call	MIOS_DOUT_SRSet
#endif

	;; right side
	incf	IRQ_TMP1, W
	movff	PLUSW2, MIOS_PARAMETER1
#if BLM_DOUT_R2
	movlw	BLM_DOUT_R2 - 1
	call	MIOS_DOUT_SRSet
#endif

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_GetRow
;;  DESCRIPTION: This function gets the DIN values of the selected column
;;  It should be called from USER_SR_Service_Finish
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
_BLM_GetRow ; (for C)
BLM_GetRow
	;; since the row line of the buttons is identical to the row line of the LEDs,
	;; we can derive the button row offset from blm_selected_column

	;; store row*2 offset in IRQ_TMP1
	SET_BSR	blm_selected_column
	decf	blm_selected_column, W, BANKED
	rlf	WREG, W
	andlw	0x06
	movwf	IRQ_TMP1

	;; check the left half
#if BLM_DIN_L
	movlw	BLM_DIN_L - 1
	rcall	BLM_GetRow_Sub
#endif

	;; check the right half
	incf	IRQ_TMP1, F	; row*2+1
#if BLM_DIN_R
	movlw	BLM_DIN_R - 1
	rcall	BLM_GetRow_Sub
#endif
	
	return


	;; expecting: DIN SR number in WREG
	;;            row counter in IRQ_TMP1
BLM_GetRow_Sub
	;; get 8bit value of selected DIN register -> MIOS_PARAMETER1
	call	MIOS_DIN_SRGet

	;; debounce handling: each pin has to be handled individually by BLM_GetRow_PinSub

	;; determine initial offset to debounce counter array -> FSR2
#if BLM_NO_DEBOUNCING == 0
	lfsr	FSR2, blm_button_debounce_ctr
	rlf	IRQ_TMP1, W
	rlf	WREG, W
	rlf	WREG, W
	andlw	0x78
	addwf	FSR2L, F
#endif

	;; call pin handler 8 times
	movlw	(1 << 0)	; mask for pin #0
	rcall	BLM_GetRow_PinSub
	
	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 1)	; mask for pin #1
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 2)	; mask for pin #2
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 3)	; mask for pin #3
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 4)	; mask for pin #4
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 5)	; mask for pin #5
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 6)	; mask for pin #6
	rcall	BLM_GetRow_PinSub

	incf	FSR2L, F	; increment offset (not relevant if debouncing disabled)
	movlw	(1 << 7)	; mask for pin #7
	;; 	rgoto	BLM_GetRow_PinSub

BLM_GetRow_PinSub
	movwf	IRQ_TMP2	; store mask in IRQ_TMP2

#if BLM_NO_DEBOUNCING == 0
	;; if debounce counter > 0: decrement it and exit (ignore bouncing values)
	movf	INDF2, W
	bz	BLM_GetRow_PinSub_Debounced
BLM_GetRow_PinSub_Ignore
	decf	INDF2, F
	return
#endif


BLM_GetRow_PinSub_Debounced
	;; transfer new value to blm_button_row_values[] array
	lfsr	FSR1, blm_button_row_values
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
	bz	BLM_GetRow_PinSub_End	; if 0 -> no change

BLM_GetRow_PinSub_Change
	;; notify pin change in blm_button_row_changed array
	lfsr	FSR1, blm_button_row_changed
	movf	IRQ_TMP1, W		; row index
	addwf	FSR1L, F

	movf	IRQ_TMP2, W		; pin mask
	iorwf	INDF1, F		; OR with other bits

#if BLM_NO_DEBOUNCING == 0
	;; re-load debounce delay
	SET_BSR	_blm_button_debounce_delay
	movf	_blm_button_debounce_delay, W, BANKED
	movwf	INDF2
#endif
BLM_GetRow_PinSub_End

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: BLM_ButtonHandler
;;  DESCRIPTION: This function should be called from USER_Tick (in the
;;  mainprogram) to ask for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified via the BLM_NotifyToggle hook
;;  which behaves similar to the USER_DIN_NotifyToggle hook provided by
;;  MIOS itself
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
_BLM_ButtonHandler ; (for C)
BLM_ButtonHandler
	SET_BSR	blm_button_column_ctr
	clrf	blm_button_column_ctr, BANKED
BLM_ButtonHandler_Loop
	lfsr	FSR1, blm_button_row_changed
	movf	blm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	INDF1, W
	bz	BLM_ButtonHandler_Loop_Next
	btfsc	INDF1, 0		; fastest solution...
	rcall	BLM_NotifyToggle_Pin0
	btfsc	INDF1, 1
	rcall	BLM_NotifyToggle_Pin1
	btfsc	INDF1, 2
	rcall	BLM_NotifyToggle_Pin2
	btfsc	INDF1, 3
	rcall	BLM_NotifyToggle_Pin3
	btfsc	INDF1, 4
	rcall	BLM_NotifyToggle_Pin4
	btfsc	INDF1, 5
	rcall	BLM_NotifyToggle_Pin5
	btfsc	INDF1, 6
	rcall	BLM_NotifyToggle_Pin6
	btfsc	INDF1, 7
	rcall	BLM_NotifyToggle_Pin7
BLM_ButtonHandler_Loop_Next
	incf	blm_button_column_ctr, F, BANKED
	movlw	0x08-1		; (8 columns)
	cpfsgt	blm_button_column_ctr, BANKED
	rgoto	BLM_ButtonHandler_Loop
	return

	;; ---

BLM_NotifyToggle_Pin0
	bcf	INDF1, 0			; clear "changed" flag
	movlw	0x00
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin1
	bcf	INDF1, 1			; clear "changed" flag
	movlw	0x01
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin2
	bcf	INDF1, 2			; clear "changed" flag
	movlw	0x02
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin3
	bcf	INDF1, 3			; clear "changed" flag
	movlw	0x03
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin4
	bcf	INDF1, 4			; clear "changed" flag
	movlw	0x04
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin5
	bcf	INDF1, 5			; clear "changed" flag
	movlw	0x05
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin6
	bcf	INDF1, 6			; clear "changed" flag
	movlw	0x06
	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin7
	bcf	INDF1, 7			; clear "changed" flag
	movlw	0x07
	;; 	rgoto	BLM_NotifyToggle_Pin_Cont
BLM_NotifyToggle_Pin_Cont
	movwf	_blm_button_column, BANKED

	;; store pin value in BLM_BUTTON_VALUE
	lfsr	FSR1, blm_button_row_values
	movf	blm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	movf	_blm_button_column, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF1, W
	skpz
	movlw	0x01
	movwf	_blm_button_value

	;; for compatibility with USER_DIN_NotifyToggle:
	;; save button value in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; save button number (0x00-0xff) in MIOS_PARAMETER1
	;; if more than 256 buttons are connected, use BLM_BUTTON_COLUMN and BLM_BUTTON_ROW to determine the number
	movf	blm_button_column_ctr, W, BANKED
	movwf	_blm_button_row, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	_blm_button_column, W, BANKED

	;; call C hook
	movwf	MIOS_PARAMETER1			; button number still in WREG
	movff	_blm_button_value, POSTDEC0	; pin value
	call	_BLM_NotifyToggle
	incf	FSR0L, F			; restore stack pointer

	lfsr	FSR1, blm_button_row_changed	; fix FSR1 if it has been overwritten
	SET_BSR	blm_button_column_ctr
	movf	blm_button_column_ctr, W, BANKED
	addwf	FSR1L, F
	return

; ==========================================================================
	END
