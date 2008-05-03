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
;    o SM_DebounceTimer: decrements the debounce counters until they are zero
;                        called periodically from USER_Timer
;
;    o SM_ButtonHandler: scans all buttons, to be called from USER_Tick
;
;    o SM_NotifyToggle: called from SM_ButtonHandler on button changes
;
;
; pins of SRIO shift registers
SM_SRIO_LAT_SCLK	EQU	LATD	; Pin D.3
SM_SRIO_PIN_SCLK	EQU	3
SM_SRIO_LAT_RCLK	EQU	LATD	; Pin D.2
SM_SRIO_PIN_RCLK	EQU	2
SM_SRIO_PORT_DIN	EQU	PORTD	; Pin D.1
SM_SRIO_PIN_DIN	EQU	1
SM_SRIO_LAT_DOUT	EQU	LATD	; Pin D.0
SM_SRIO_PIN_DOUT	EQU	0
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

	;; init values of all 64 buttons
	lfsr	FSR0, SM_ROW0_VALUE
	movlw	0x08
	movwf	TMP1
SM_Init_Loop2
	setf	POSTINC0
	decfsz	TMP1, F
	rgoto	SM_Init_Loop2

	;; deselect all columns
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
	bsf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT
	movlw	16		; 16*8 shifts for up to 16 shift registers
	movwf	TMP1
SM_Init_Loop3
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
	decfsz	TMP1, F
	rgoto	SM_Init_Loop3

	;; latch DOUT values
        bcf	SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK
	nop
	nop
	nop
        bsf     SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK
	
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: SM_DebounceTimer
;;  DESCRIPTION: This function decrements the debounce counters until they
;;  are zero. It should be called from USER_Timer
;;  IN:   -
;;  OUT:  -
;;  USES: BSR
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;; this defines a macro for a single counter operation
SM_DEBOUNCE_CTR_DEC MACRO
	LOCAL	SM_DEBOUNCE_CTR_DEC_0
	LOCAL	SM_DEBOUNCE_CTR_DEC_1
	LOCAL	SM_DEBOUNCE_CTR_DEC_End

	movf	INDF2, W
	bz	SM_DEBOUNCE_CTR_DEC_End
	andlw	0x0f
	bz	SM_DEBOUNCE_CTR_DEC_1
SM_DEBOUNCE_CTR_DEC_0
	movlw	(-1 & 0x0f)
	addwf	INDF2, F
SM_DEBOUNCE_CTR_DEC_1
	movf	INDF2, W
	andlw	0xf0
	bz	SM_DEBOUNCE_CTR_DEC_End
	movlw	(-1 & 0xf0)
	addwf	INDF2, F

SM_DEBOUNCE_CTR_DEC_End
	;; increment pointer
	movf	POSTINC2, W
	ENDM
;; --------------------------------------------------------------------------

SM_DebounceTimer
#if 0
	;; for debugging: toggle debugging pin at J14
	bsf	PORTD, 4
#endif
	;; measured time: ca. 15 uS for 64 4-bit counters

	;; decrement all debounce counters until they are zero
	;; each counter uses a 4bit field!
	lfsr	FSR2, SM_DEBOUNCE_CTR0_01
	movlw	8	; number of rows
	movwf	IRQ_TMP1
SM_DebounceTimer_Loop
	;; 4 registers for 8 counters
	SM_DEBOUNCE_CTR_DEC	; SM_DEBOUNCE_CTRx_01
	SM_DEBOUNCE_CTR_DEC	; SM_DEBOUNCE_CTRx_23
	SM_DEBOUNCE_CTR_DEC	; SM_DEBOUNCE_CTRx_45
	SM_DEBOUNCE_CTR_DEC	; SM_DEBOUNCE_CTRx_67

	decfsz	IRQ_TMP1, F
	rgoto	SM_DebounceTimer_Loop

#if 0
	;; for debugging: toggle debugging pin at J14
	bcf	PORTD, 4
#endif

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

;; --------------------------------------------------------------------------
;; this defines a macro for a single shift step
SM_BUTTON_HANDLER_SHIFT MACRO bit_number
	LOCAL	SM_BUTTON_HANDLER_SHIFT_0
	LOCAL	SM_BUTTON_HANDLER_SHIFT_1
	LOCAL	SM_BUTTON_HANDLER_SHIFT_Cont

	;; set DOUT value
	bsf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT
	btfss	SM_DOUT_VALUE, 7-bit_number
	bcf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT

	;; toggle shift clock to 0
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK

	;; branch depending on current DIN value
	BRA_IFSET SM_SRIO_PORT_DIN, SM_SRIO_PIN_DIN, ACCESS, SM_BUTTON_HANDLER_SHIFT_1
SM_BUTTON_HANDLER_SHIFT_0
	;; do nothing if DIN pin already 0
	BRA_IFCLR INDF0, bit_number, ACCESS, SM_BUTTON_HANDLER_SHIFT_Cont
	;; do nothing if debounce counter is not zero
	lfsr	FSR1, SM_DEBOUNCE_CTR0_01
	rlf	SM_BUTTON_COLUMN, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addwf	FSR1L, F
	movlw	bit_number >> 1
	addwf	FSR1L, F
   if bit_number & 1
	movlw	0xf0
   else
	movlw	0x0f
   endif
	andwf	INDF1, W
	bnz	SM_BUTTON_HANDLER_SHIFT_Cont

	;; else reload debounce counter
   if bit_number & 1
	movlw	0xf0
   else
	movlw	0x0f
   endif
	iorwf	INDF1, F
	;; save 0
	bcf	INDF0, bit_number
	;; and call the toggle function
	movlw	bit_number
	rcall	SM_ButtonHandler_PinToggled
	rgoto	SM_BUTTON_HANDLER_SHIFT_Cont
SM_BUTTON_HANDLER_SHIFT_1
	;; do nothing if DIN pin already 1
	BRA_IFSET INDF0, bit_number, ACCESS, SM_BUTTON_HANDLER_SHIFT_Cont
	;; do nothing if debounce counter is not zero
	lfsr	FSR1, SM_DEBOUNCE_CTR0_01
	rlf	SM_BUTTON_COLUMN, W, BANKED
	rlf	WREG, W
	andlw	0xfc
	addwf	FSR1L, F
	movlw	bit_number >> 1
	addwf	FSR1L, F
   if bit_number & 1
	movlw	0xf0
   else
	movlw	0x0f
   endif
	andwf	INDF1, W
	bnz	SM_BUTTON_HANDLER_SHIFT_Cont

	;; else reload debounce counter
   if bit_number & 1
	movlw	0xf0
   else
	movlw	0x0f
   endif
	iorwf	INDF1, F
	;; save 1
	bsf	INDF0, bit_number
	;; and call the toggle function
	movlw	bit_number
	rcall	SM_ButtonHandler_PinToggled
	;; 	rgoto	SM_BUTTON_HANDLER_SHIFT_Cont
SM_BUTTON_HANDLER_SHIFT_Cont
	;; toggle shift clock to 1
        bsf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
	ENDM
;; --------------------------------------------------------------------------

;; the button handler
SM_ButtonHandler
	;; clear column counter
	SET_BSR	SM_BUTTON_COLUMN
	clrf	SM_BUTTON_COLUMN, BANKED

SM_ButtonHandler_InnerLoop
	;; latch DIN values
        bcf	SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK

	;; get pointer to last captured ROW values, it will be updated by the shift macro
	lfsr	FSR0, SM_ROW0_VALUE
	movf	SM_BUTTON_COLUMN, W, BANKED
	addwf	FSR0L, F

	;; release latch
        bsf     SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK

	;; determine DOUT value for *next* iteration step
	;; this value gets active with the next latch pulse
	incf	SM_BUTTON_COLUMN, W, BANKED
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	SM_DOUT_VALUE, BANKED

	;; shift 8 times
	SM_BUTTON_HANDLER_SHIFT	7
	SM_BUTTON_HANDLER_SHIFT	6
	SM_BUTTON_HANDLER_SHIFT	5
	SM_BUTTON_HANDLER_SHIFT	4
	SM_BUTTON_HANDLER_SHIFT	3
	SM_BUTTON_HANDLER_SHIFT	2
	SM_BUTTON_HANDLER_SHIFT	1
	SM_BUTTON_HANDLER_SHIFT	0

	;; latch DOUT values
        bcf     SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK

	;; toggle shift clock to 0
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
	;; set DOUT pin to a defined value (1)
	bsf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT

	;; release latch
        bsf     SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK


	;; loop 8 times
	incf	SM_BUTTON_COLUMN, F, BANKED
	movlw	8-1
	cpfsgt	SM_BUTTON_COLUMN, BANKED
	rgoto SM_ButtonHandler_InnerLoop

	return


	;; ---

SM_ButtonHandler_PinToggled
	movwf	SM_BUTTON_ROW, BANKED

	;; store pin value in SM_BUTTON_VALUE
	lfsr	FSR0, SM_ROW0_VALUE
	movf	SM_BUTTON_COLUMN, W, BANKED
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
	movf	SM_BUTTON_ROW, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	SM_BUTTON_COLUMN, W, BANKED	
#else
	movf	SM_BUTTON_COLUMN, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	SM_BUTTON_ROW, W, BANKED
#endif

	movwf	MIOS_PARAMETER1
	;; call hook
	call	SM_NotifyToggle

	lfsr	FSR0, SM_ROW0_VALUE	; fix FSR0 if it has been overwritten
	SET_BSR	SM_BUTTON_COLUMN
	movf	SM_BUTTON_COLUMN, W, BANKED
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
	;; MEMO: here you could also branch directly to USER_DIN_NotifyToggle in order
	;; to emulate buttons which are connected on the common way to the shift registers
	;; ->
	;;	goto	USER_DIN_NotifyToggle

	;; but in this example we just send a note on/off event depending on button status
	movlw	0x90
	movwf	MIDI_EVNT0
	movf	MIOS_PARAMETER1, W
	addlw	0x18		; lowest note: C-1
	andlw	0x7f
	movwf	MIDI_EVNT1
	movlw	0x00
	btfss	MIOS_PARAMETER2, 0
	movlw 0x7f
	movwf	MIDI_EVNT_VALUE
	call	MIDI_EVNT_Send

	;; only for debugging: request display update to show SM_BUTTON_COLUMN/ROW/VALUE on LCD
	bsf	DISPLAY_UPDATE_REQ, 0
	return
