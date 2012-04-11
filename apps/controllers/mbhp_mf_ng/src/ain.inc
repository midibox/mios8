; $Id$
;
; Downstripped version of MIOS AIN Handler
; Adapted to MBHP_MF_NG project
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


AIN_CTRL_STATE	EQU	3
AIN_CTRL_MF	EQU	6


;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_Pin7bitGet
;;  C_DECLARATION: unsigned char AIN_Pin7bitGet(unsigned char pin)
;;  DESCRIPTION: returns 7-bit value of AIN input
;;  IN:	  number of analog input pin in WREG
;;  C_IN: number of analog input pin in <pin>
;;  OUT:  7-bit value in MIOS_PARAMETER1 and WREG
;;  C_OUT: 7-bit value
;;  USES: BSR, FSR1, MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
AIN_Pin7bitGet
	andlw	0x3f
	lfsr	FSR1, AIN_RESULT_L_00
	addwf	FSR1L, F
	movff	INDF1, MIOS_PARAMETER1
	bsf	FSR1L, 6
	movff	INDF1, MIOS_PARAMETER2

	;; note: a multiplication would work faster, but would overwrite PRODL/PRODH
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER1, F
	rrf	MIOS_PARAMETER1, W
	andlw	0x7f
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_PinGet
;;  C_DECLARATION: unsigned int AIN_PinGet(unsigned char pin)
;;  DESCRIPTION: returns value of AIN input
;;  IN:	  pin number in WREG
;;  C_IN: pin number in <pin>
;;  OUT:  MSB in MIOS_PARAMETER2, LSB in MIOS_PARAMETER1 and WREG
;;  C_OUT: 10bit value
;;  USES: FSR1
;; --------------------------------------------------------------------------
AIN_PinGet
	andlw	0x3f
	lfsr	FSR1, AIN_RESULT_H_00
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x03
	movwf	MIOS_PARAMETER2
	bcf	FSR1L, 6
	movf	INDF1, W
	movwf	MIOS_PARAMETER1
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_NumberSet
;;  C_DECLARATION: void AIN_NumberSet(unsigned char pots)
;;  DESCRIPTION: sets number of available AIN pins<BR>
;;  Only for up to 8 pins
;;  IN:	  number of analog pins in WREG
;;  C_IN: number of analog pins in <pots>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
AIN_NumberSet
	;; clear input counter
	SET_BSR	AIN_BASE
	clrf	AIN_INPUT_CTR, BANKED

	;; write number of pots into AIN_NUMBER register. Ensure that not more than 64 pots are active
	movwf	AIN_NUMBER, BANKED
	andlw	0xc0
	skpnz
	rgoto	AIN_NumberSet_Cont
	movlw	0x40
	movwf	AIN_NUMBER, BANKED
AIN_NumberSet_Cont

	;; ensure that max. 8 faders are active
	BRA_IFCLR AIN_CTRL, AIN_CTRL_MF, BANKED, AIN_NumberSet_NoMF
AIN_NumberSet_MF
	movlw	0x08
	cpfsgt	AIN_NUMBER, BANKED
	rgoto	AIN_NumberSet_MFOk
	movwf	AIN_NUMBER, BANKED
AIN_NumberSet_MFOk
AIN_NumberSet_NoMF

	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: AIN_NumberGet
;;  C_DECLARATION: unsigned char AIN_NumberGet(void)
;;  DESCRIPTION: returns number of available analog pins
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  number of analog pins in MIOS_PARAMETER1 and WREG
;;  C_OUT:  number of analog pins
;;  USES: BSR
;; --------------------------------------------------------------------------
AIN_NumberGet
	SET_BSR	AIN_BASE
	movf	AIN_NUMBER, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  MIOS AIN Handler
;;  check changed AIN pins, call USER_AIN_NotifyChange when conversion result
;;  of AIN pin has been changed
;; --------------------------------------------------------------------------
AIN_Handler
	SET_BSR	AIN_BASE
	clrf	AIN_POT_CTR, BANKED

	movf	AIN_NUMBER, W, BANKED
	skpnz
	return

	lfsr	FSR0, AIN_RESULT_H_00
	movf	AIN_POT_CTR, W, BANKED
AIN_Handler_Loop
	BRA_IFCLR PLUSW0, 7, ACCESS, AIN_Handler_Loop_Next
	bcf	PLUSW0, 7

	movff	PLUSW0, MIOS_PARAMETER3
	bcf	FSR0L, 6
	movff	PLUSW0, MIOS_PARAMETER2
	;; call hook
	movwf	MIOS_PARAMETER1
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_AIN_NotifyChange

	lfsr	FSR0, AIN_RESULT_H_00
	SET_BSR	AIN_BASE
AIN_Handler_Loop_Next
	incf	AIN_POT_CTR, F, BANKED
	movf	AIN_POT_CTR, W, BANKED
	cpfseq	AIN_NUMBER, BANKED
	rgoto AIN_Handler_Loop
	return


;; --------------------------------------------------------------------------
;;  MIOS AIN Tick
;;  called by timer0 interrupt every 50 us
;; --------------------------------------------------------------------------
AIN_Tick
	SET_BSR	AIN_BASE			; branch depending on state
	BRA_IFSET AIN_CTRL, AIN_CTRL_STATE, BANKED, AIN_Tick_State1

	;; STATE0: start conversion
AIN_Tick_State0
	bsf	AIN_CTRL, AIN_CTRL_STATE, BANKED	; set next state
	bsf	ADCON0, GO					; do a dummy conversion
	rgoto	AIN_Tick_End

	;; STATE1: get result
AIN_Tick_State1
	bcf	AIN_CTRL, AIN_CTRL_STATE, BANKED	; set next state

	lfsr	FSR2, AIN_RESULT_L_00
	movf	AIN_INPUT_CTR, W, BANKED
	addwf	FSR2L, F

	;; calc IRQ_TMP_[12] = ADRES_[LH] - INDF2_[LH]
	movf	INDF2, W
	subwf	ADRESL, W
	movwf	IRQ_TMP1
	bsf	FSR2L, 6
	movf	INDF2, W
	andlw	0x7f		; (clear "changed" bit)
	subwfb	ADRESH, W
	movwf	IRQ_TMP2
	;; if result negative, invert values
	btfsc	IRQ_TMP2, 7
	comf	IRQ_TMP1, F
	btfsc	IRQ_TMP2, 7
	comf	IRQ_TMP2, F

	movf	IRQ_TMP1, W
	movf	IRQ_TMP2, F
	skpz
	movlw	0xff
	movwf	AIN_POTSPEED, BANKED	; this value is also used by the MF driver

	;; save new result if absolute difference > AIN_DEADBAND
	movf	AIN_DEADBAND, W, BANKED
	cpfsgt	AIN_POTSPEED, BANKED
	rgoto AIN_Tick_State1_NoNew

AIN_Tick_State1_New
	;; save new result
	bcf	FSR2L, 6
	movff	ADRESL, INDF2
	bsf	FSR2L, 6
	movff	ADRESH, INDF2
	bsf	INDF2, 7	; set "changed" bit
	bcf	FSR2L, 6

	;; if result < 5, then clear it (lowest position reached)
	movf	ADRESH, W
	bnz	AIN_Tick_State1_New_No0
	movlw	0x05
	cpfslt	ADRESL, ACCESS
	rgoto AIN_Tick_State1_New_No0
	clrf	INDF2
AIN_Tick_State1_New_No0
AIN_Tick_State1_NoNew

	;; if motorfader mode: branch to MF tick, else use standard result handler
	btfsc	AIN_CTRL, AIN_CTRL_MF, BANKED
	call	MF_Tick
	SET_BSR	AIN_BASE

	;; continue with next input pin
	incf	AIN_INPUT_CTR, F, BANKED		; increment input counter
	movf	AIN_NUMBER, W, BANKED
	bz	AIN_Tick_End
	xorwf	AIN_INPUT_CTR, W, BANKED		; reset if max value reached
	skpnz
	clrf	AIN_INPUT_CTR, BANKED

	swapf	AIN_INPUT_CTR, W, BANKED ; calc ADCON0 value (0x81 | (pot number << 3)
	movwf	IRQ_TMP1
	rrf	IRQ_TMP1, W

#if PIC_DERIVATIVE_NEW_ADC	; PIC18F4620, PIC18F4520, ...
	rrf	WREG, W
	andlw	0x1c
	iorlw	0x01
#else
	andlw	0x38
	iorlw	0x81
#endif
	movwf	ADCON0
	bsf	ADCON0, GO	; do a dummy conversion

AIN_Tick_End
	return
