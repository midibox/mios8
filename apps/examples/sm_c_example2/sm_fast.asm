;
; Scan Matrix Driver, fast version
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
;
;    Following function is located in main.c:
;    o SM_NotifyToggle: called from SM_ButtonHandler on button changes
;
;
; pins of SRIO shift registers
#define SM_SRIO_LAT_SCLK	LATD	; Pin D.3
#define SM_SRIO_PIN_SCLK	3
#define SM_SRIO_LAT_RCLK	LATD	; Pin D.2
#define SM_SRIO_PIN_RCLK	2
#define SM_SRIO_PORT_DIN	PORTD	; Pin D.1
#define SM_SRIO_PIN_DIN		1
#define SM_SRIO_LAT_DOUT	LATD	; Pin D.0
#define SM_SRIO_PIN_DOUT	0
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
	global	_SM_DebounceTimer
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

sm_dout_value		res	1
sm_button_column	res	1

sm_row_values		res	8
sm_debounce_ctrs	res	32	; each button uses a 4bit field, up to 64 buttons are supported

; ==========================================================================
SM_FAST code
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

	;; clear the debounce counters
	lfsr	FSR1, sm_debounce_ctrs
	movlw	0x20
	movwf	PRODL
SM_Init_Loop2
	clrf	POSTINC1
	decfsz	PRODL, F
	rgoto	SM_Init_Loop2

	;; deselect all columns
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK
	bsf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT
	movlw	16		; 16*8 shifts for up to 16 shift registers
	movwf	PRODL
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
	decfsz	PRODL, F
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

	movf	INDF1, W
	bz	SM_DEBOUNCE_CTR_DEC_End
	andlw	0x0f
	bz	SM_DEBOUNCE_CTR_DEC_1
SM_DEBOUNCE_CTR_DEC_0
	movlw	(-1 & 0x0f)
	addwf	INDF1, F
SM_DEBOUNCE_CTR_DEC_1
	movf	INDF1, W
	andlw	0xf0
	bz	SM_DEBOUNCE_CTR_DEC_End
	movlw	(-1 & 0xf0)
	addwf	INDF1, F

SM_DEBOUNCE_CTR_DEC_End
	;; increment pointer
	movf	POSTINC1, W
	ENDM
;; --------------------------------------------------------------------------
_SM_DebounceTimer ; (for C)
SM_DebounceTimer
#if 0
	;; for debugging: toggle debugging pin at J14
	bsf	PORTD, 4
#endif
	;; measured time: ca. 15 uS for 64 4-bit counters

	;; decrement all debounce counters until they are zero
	;; each counter uses a 4bit field!
	lfsr	FSR1, sm_debounce_ctrs
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
_SM_ButtonHandler ; (for C)
;; --------------------------------------------------------------------------
;; this defines a macro for a single shift step
SM_BUTTON_HANDLER_SHIFT MACRO bit_number
	LOCAL	SM_BUTTON_HANDLER_SHIFT_0
	LOCAL	SM_BUTTON_HANDLER_SHIFT_1
	LOCAL	SM_BUTTON_HANDLER_SHIFT_Cont

	;; set DOUT value
	bsf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT
	btfss	sm_dout_value, 7-bit_number
	bcf	SM_SRIO_LAT_DOUT, SM_SRIO_PIN_DOUT

	;; toggle shift clock to 0
        bcf     SM_SRIO_LAT_SCLK, SM_SRIO_PIN_SCLK

	;; branch depending on current DIN value
	btfsc	SM_SRIO_PORT_DIN, SM_SRIO_PIN_DIN
	rgoto	SM_BUTTON_HANDLER_SHIFT_1
SM_BUTTON_HANDLER_SHIFT_0
	;; do nothing if DIN pin already 0
	btfss	INDF2, bit_number
	rgoto	SM_BUTTON_HANDLER_SHIFT_Cont
	;; do nothing if debounce counter is not zero
	lfsr	FSR1, sm_debounce_ctrs
	rlf	_sm_button_column, W, BANKED
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
	bcf	INDF2, bit_number
	;; and call the toggle function
	movlw	bit_number
	rcall	SM_ButtonHandler_PinToggled
	rgoto	SM_BUTTON_HANDLER_SHIFT_Cont
SM_BUTTON_HANDLER_SHIFT_1
	;; do nothing if DIN pin already 1
	btfsc	INDF2, bit_number
	rgoto	SM_BUTTON_HANDLER_SHIFT_Cont
	;; do nothing if debounce counter is not zero
	lfsr	FSR1, sm_debounce_ctrs
	rlf	_sm_button_column, W, BANKED
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
	bsf	INDF2, bit_number
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
	;; store FSR2 on stack
	movff	FSR2L, POSTDEC0
	movff	FSR2H, POSTDEC0

	;; clear column counter
	SET_BSR	sm_button_column
	clrf	sm_button_column, BANKED

SM_ButtonHandler_InnerLoop
	;; latch DIN values
        bcf	SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK

	;; get pointer to last captured ROW values, it will be updated by the shift macro
	lfsr	FSR2, sm_row_values
	movf	sm_button_column, W, BANKED
	addwf	FSR2L, F

	;; release latch
        bsf     SM_SRIO_LAT_RCLK, SM_SRIO_PIN_RCLK

	;; determine DOUT value for *next* iteration step
	;; this value gets active with the next latch pulse
	incf	sm_button_column, W, BANKED
	call	MIOS_HLP_GetBitANDMask	; (inverted 1 of 8 code)
	movwf	sm_dout_value, BANKED

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
	incf	sm_button_column, F, BANKED
	movlw	8-1
	cpfsgt	sm_button_column, BANKED
	rgoto	SM_ButtonHandler_InnerLoop

	;; restore FSR2 from stack
	movff	PREINC0, FSR2H
	movff	PREINC0, FSR2L

	return


	;; ---

SM_ButtonHandler_PinToggled
	movwf	_sm_button_row, BANKED

	;; store pin value in SM_BUTTON_VALUE
	lfsr	FSR2, sm_row_values
	movf	sm_button_column, W, BANKED
	addwf	FSR2L, F
	movf	_sm_button_row, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	INDF2, W
	skpz
	movlw	0x01
	movwf	_sm_button_value

	;; for compatibility with USER_DIN_NotifyToggle:
	;; save button value in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; save button number (0x00-0xff) in MIOS_PARAMETER1
	;; if more than 256 buttons are connected, use SM_BUTTON_COLUMN and SM_BUTTON_ROW to determine the number
#if SM_SWAP_CR
	movf	_sm_button_row, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	sm_button_column, W, BANKED	
#else
	movf	sm_button_column, W, BANKED
	mullw	0x08
	movf	PRODL, W
	addwf	_sm_button_row, W, BANKED
#endif
	;; restore FSR2 from stack
	movff	PREINC0, FSR2H
	movff	PREINC0, FSR2L

	;; latch column in global variable (will be valid even after the handler has been exit)
	movff	sm_button_column, _sm_button_column

	;; call C hook
	movwf	MIOS_PARAMETER1			; button number still in WREG
	movff	_sm_button_value, POSTDEC0	; pin value
	call	_SM_NotifyToggle
	incf	FSR0L, F			; restore stack pointer

	;; store FSR2 on stack
	movff	FSR2L, POSTDEC0
	movff	FSR2H, POSTDEC0

	lfsr	FSR2, sm_row_values	; fix FSR2 if it has been overwritten
	SET_BSR	sm_button_column
	movf	sm_button_column, W, BANKED
	addwf	FSR2L, F
	return

; ==========================================================================
	END
