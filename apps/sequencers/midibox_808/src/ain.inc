; $Id: ain.inc 44 2008-01-30 21:39:30Z tk $
;
; Using J5 as analog input
; Replaces the MIOS AIN driver for dedicated analog conversion routines
; Works only with PIC18F4685/PIC18F4620 (due to different ADC channel handling!)
; 
; This include replaces provides following functions:
;    o AIN_Init:    initializes the J5 port
;    o AIN_Tick:    triggers the sampling routine - should be called from a timer interrupt
;    o AIN_Handler: checks for changes of analog pins - should be called from USER_Tick
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_Init
;;  DESCRIPTION: This function initializes J5 as analog input
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
AIN_Init
	;; no configuration of ADC required, as MIOS is already doing this for us
	;; (depending on PIC derivative)

	;; use left-justified values, highest acquisition time and slowest clock (for best results)
	movlw	(0x7 << 3) | (0x6 << 0)
	movwf	ADCON2

	;; clear input counter
	SET_BSR	AIN_INPUT_CTR
	clrf	AIN_INPUT_CTR, BANKED

	;; clear CHANGED flags
	clrf	AIN_INPUT_CHANGED, BANKED

	;; clear AIN values
	clrf	PRODL
	lfsr	FSR1, AIN_VALUE_0
AIN_Init_Loop
	clrf	POSTINC1
	incf	PRODL, F
	BRA_IFCLR PRODL, 3, ACCESS, AIN_Init_Loop

	;; start the conversion of first pin
#if 0
	rlf	AIN_INPUT_CTR, W, BANKED
	rlf	WREG, W
	andlw	0x1c		; channel mask for PIC18F4685
	iorlw	(1 << ADON)	; ADC ON
#else
	movlw	(1 << ADON)	; the same for first channel in one instruction
#endif
	movwf	ADCON0
	bsf	ADCON0, GO	; start conversion
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_Tick
;;  DESCRIPTION: Should be called from a timer interrupt to trigger a conversion
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
AIN_Tick
	;; new 8bit result in ADRESH - we convert it to 7bit and expect a value change when abs(old_value-new_value) >= 2
	lfsr	FSR1, AIN_VALUE_0		; pointer to AIN_VALUE_x in FSR1
	SET_BSR	AIN_INPUT_CTR
	movf	AIN_INPUT_CTR, W, BANKED
	addwf	FSR1L, F

	;; calculate abs(old_value-new_value)
	movf	INDF1, W
	subwf	ADRESH, W
	bc	AIN_Tick_DiffPos
AIN_Tick_DiffNeg
	xorlw	0xff		; n-1
	addlw	1
AIN_Tick_DiffPos
	bz	AIN_Tick_NoChange ; check for difference 0..1
	addlw	-1
	bz	AIN_Tick_NoChange
AIN_Tick_Change
	;; store new value in array
	movff	ADRESH, INDF1

	;; notify that value has been changed
	movf	AIN_INPUT_CTR, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	AIN_INPUT_CHANGED, F, BANKED
AIN_Tick_NoChange

	;; increment AIN input counter, wrap at 7
	incf	AIN_INPUT_CTR, W, BANKED
	andlw	0x07
	movwf	AIN_INPUT_CTR, BANKED

	;; start next conversion and exit
	rlf	AIN_INPUT_CTR, W, BANKED
	rlf	WREG, W
	andlw	0x1c		; channel mask for PIC18F4685
 	iorlw	(1 << ADON)	; ADC ON
	movwf	ADCON0
	bsf	ADCON0, GO	; start conversion
	return



;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_Handler
;;  DESCRIPTION: Should be called from USER_Tick to check for new values
;;  IN:   -
;;  OUT:  calls AIN_NotifyChange
;;  USES: -
;; --------------------------------------------------------------------------
AIN_Handler
	;; exit immediately if no value has been changed
	SET_BSR	AIN_INPUT_CHANGED
	IRQ_DISABLE
	movf	AIN_INPUT_CHANGED, W, BANKED		; copy value to AIN_INPUT_CHANGED_TMP to avoid IRQ collision
	clrf	AIN_INPUT_CHANGED, BANKED
	movwf	AIN_INPUT_CHANGED_TMP, BANKED
	andlw	0xff	; fix ZERO flag (changed by clrf!)
	IRQ_ENABLE
	bz	AIN_Handler_End

	;; check for changed values and propagate to hook
	movlw	0
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 0, BANKED, AIN_Handler_CallHook
	movlw	1
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 1, BANKED, AIN_Handler_CallHook
	movlw	2
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 2, BANKED, AIN_Handler_CallHook
	movlw	3
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 3, BANKED, AIN_Handler_CallHook
	movlw	4
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 4, BANKED, AIN_Handler_CallHook
	movlw	5
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 5, BANKED, AIN_Handler_CallHook
	movlw	6
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 6, BANKED, AIN_Handler_CallHook
	movlw	7
	SET_BSR	AIN_INPUT_CHANGED_TMP
	RCALL_IFSET AIN_INPUT_CHANGED_TMP, 7, BANKED, AIN_Handler_CallHook
	rgoto	AIN_Handler_End

AIN_Handler_CallHook
	;; store input number in MIOS_PARAMETER1 (ensure that it stays in WREG!)
	movwf	MIOS_PARAMETER1
	;; calculate pointer to AIN_VALUE_x and copy it to MIOS_PARAMETER2
	lfsr	FSR1, AIN_VALUE_0
	movff	PLUSW1, MIOS_PARAMETER2
	;; call hook (pin number still in WREG)
	goto	AIN_NotifyChange

AIN_Handler_End
	return
