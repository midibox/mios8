; $Id$
;
; MIOS AIN Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ADC MUX Pins
MIOS_AIN_LAT_MUX_0       EQU     LATC   ; Pin C.0
MIOS_AIN_PIN_MUX_0       EQU     0
MIOS_AIN_LAT_MUX_1       EQU     LATC   ; Pin C.1
MIOS_AIN_PIN_MUX_1       EQU     1
MIOS_AIN_LAT_MUX_2       EQU     LATC   ; Pin C.3
MIOS_AIN_PIN_MUX_2       EQU     3

;; AIN flags
MIOS_AIN_CTRL_DYNPRIO_DISABLE EQU 0
MIOS_AIN_CTRL_DYNPRIO_ACTIVE  EQU 1
MIOS_AIN_CTRL_DYNPRIO_POT     EQU 2

MIOS_AIN_CTRL_STATE	EQU	3
MIOS_AIN_CTRL_MF	EQU	6
MIOS_AIN_CTRL_MUXED	EQU	7


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_AIN_NotifyChange
;;  C_DECLARATION: void AIN_NotifyChange(unsigned char pin, unsigned int pin_value)
;;  DESCRIPTION: This function is called by MIOS when a pot has been moved
;;  IN: Pot number in WREG and MIOS_PARAMETER1
;;      LSB value in MIOS_PARAMETER2
;;      MSB value in MIOS_PARAMETER3
;;  C_IN: Pot number in <pin>
;;      10bit value in <pin_value>
;;  OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_Muxed
;;  C_DECLARATION: void MIOS_AIN_Muxed(void)
;;  DESCRIPTION: enables the MUX mode (up to 64 pots can be connected via
;;  AIN multiplexers
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_Muxed
	SET_BSR	MIOS_AIN_CTRL
	bsf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MUXED, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_UnMuxed
;;  C_DECLARATION: void MIOS_AIN_UnMuxed(void)
;;  DESCRIPTION: disables the MUX mode (8 pots can be connected directly
;;  to the analog input pins of the PIC
;;  IN:	  -
;;  C_IN: -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_UnMuxed
	SET_BSR	MIOS_AIN_CTRL
	bcf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MUXED, BANKED
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_Pin7bitGet
;;  C_DECLARATION: unsigned char MIOS_AIN_Pin7bitGet(unsigned char pin)
;;  DESCRIPTION: returns 7-bit value of AIN input
;;  IN:	  number of analog input pin in WREG
;;  C_IN: number of analog input pin in <pin>
;;  OUT:  7-bit value in MIOS_PARAMETER1 and WREG
;;  C_OUT: 7-bit value
;;  USES: BSR, FSR1, MIOS_PARAMETER[12]
;; --------------------------------------------------------------------------
MIOS_AIN_Pin7bitGet
	andlw	0x3f
	lfsr	FSR1, MIOS_AIN_RESULT_L_00
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
;;  FUNCTION: MIOS_AIN_PinMSBGet
;;  C_DECLARATION: unsigned char MIOS_AIN_PinMSBGet(unsigned char pin)
;;  DESCRIPTION: returns MSB value of AIN input
;;  IN:	  pin number in WREG
;;  C_IN: pin number in <pin>
;;  OUT:  MSB value in MIOS_PARAMETER1 and WREG
;;  C_OUT: MSB value
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_AIN_PinMSBGet
	andlw	0x3f
	lfsr	FSR1, MIOS_AIN_RESULT_H_00
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x03
	movwf	MIOS_PARAMETER1
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_PinLSBGet
;;  C_DECLARATION: unsigned char MIOS_AIN_PinLSBGet(unsigned char pin)
;;  DESCRIPTION: returns LSB value of AIN input
;;  IN:	  pin number in WREG
;;  C_IN: pin number in <pin>
;;  OUT:  LSB value in MIOS_PARAMETER1 and WREG
;;  C_OUT: LSB value
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_AIN_PinLSBGet
	andlw	0x3f
	lfsr	FSR1, MIOS_AIN_RESULT_L_00
	addwf	FSR1L, F
	movf	INDF1, W
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_PinGet
;;  C_DECLARATION: unsigned int MIOS_AIN_PinGet(unsigned char pin)
;;  DESCRIPTION: returns value of AIN input
;;  IN:	  pin number in WREG
;;  C_IN: pin number in <pin>
;;  OUT:  MSB in MIOS_PARAMETER2, LSB in MIOS_PARAMETER1 and WREG
;;  C_OUT: 10bit value
;;  USES: FSR1
;; --------------------------------------------------------------------------
MIOS_AIN_PinGet
	andlw	0x3f
	lfsr	FSR1, MIOS_AIN_RESULT_H_00
	addwf	FSR1L, F
	movf	INDF1, W
	andlw	0x03
	movwf	MIOS_PARAMETER2
	bcf	FSR1L, 6
	movf	INDF1, W
	movwf	MIOS_PARAMETER1
	return
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_NumberSet
;;  C_DECLARATION: void MIOS_AIN_NumberSet(unsigned char pots)
;;  DESCRIPTION: sets number of available AIN pins<BR>
;;  If number > 64, value will be forced to 64
;;  IN:	  number of analog pins in WREG
;;  C_IN: number of analog pins in <pots>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_NumberSet
	;; clear input counter
	SET_BSR	MIOS_AIN_INPUT_CTR
	clrf	MIOS_AIN_INPUT_CTR, BANKED

	;; write number of pots into MIOS_AIN_NUMBER register. Ensure that not more than 64 pots are active
	movwf	MIOS_AIN_NUMBER, BANKED
	andlw	0xc0
	skpnz
	rgoto	MIOS_AIN_NumberSet_Cont
	movlw	0x40
	movwf	MIOS_AIN_NUMBER, BANKED
MIOS_AIN_NumberSet_Cont

	;; if MF module enabled, ensure that max. 8 faders are active
	BRA_IFCLR MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED, MIOS_AIN_NumberSet_NoMF
MIOS_AIN_NumberSet_MF
	movlw	0x08
	cpfsgt	MIOS_AIN_NUMBER, BANKED
	rgoto MIOS_AIN_NumberSet_MFOk
	movwf	MIOS_AIN_NUMBER, BANKED
MIOS_AIN_NumberSet_MFOk
MIOS_AIN_NumberSet_NoMF

	;; enable timer0 and IRQ. If number of pots is zero, timer will disable itself
	bsf	INTCON, T0IE
	bsf	T0CON, TMR0ON
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_NumberGet
;;  C_DECLARATION: unsigned char MIOS_AIN_NumberGet(void)
;;  DESCRIPTION: returns number of available analog pins
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  number of analog pins in MIOS_PARAMETER1 and WREG
;;  C_OUT:  number of analog pins
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_NumberGet
	SET_BSR	MIOS_AIN_NUMBER
	movf	MIOS_AIN_NUMBER, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_DeadbandSet
;;  C_DECLARATION: void MIOS_AIN_DeadbandSet(unsigned char deadband)
;;  DESCRIPTION: sets the difference between last and current pot value
;;  which has to be achieved to trigger the "NotifyChange" function
;;  IN:	  deadband value in WREG
;;  C_IN: deadband value in <deadband>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_DeadbandSet
	SET_BSR	MIOS_AIN_DEADBAND
	movwf	MIOS_AIN_DEADBAND, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_DeadbandGet
;;  C_DECLARATION: unsigned char MIOS_AIN_DeadbandGet(void)
;;  DESCRIPTION: returns the difference between last and current pot
;;  value which has to be achieved to trigger the "NotifyChange" function
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  diff value in MIOS_PARAMETER1 and WREG
;;  C_OUT:  diff value
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_DeadbandGet
	SET_BSR	MIOS_AIN_DEADBAND
	movf	MIOS_AIN_DEADBAND, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
	
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_DynamicPrioSet
;;  C_DECLARATION: void MIOS_AIN_DynamicPrioSet(unsigned char enable)
;;  DESCRIPTION: enables or disables the dynamic priority sampling feature.
;;  The flag will only be taken into account in multiplexed mode (more than 8
;;  pots connected to the core module via AINX4). If active, the sampling 
;;  frequency of the two last turned pots will be dynamically increased 
;;  for a better accuracy.
;;  IN:	  WREG = 0x00: dynamic priority sampling disabled
;;        WREG = 0x01: dynamic priority sampling enabled (default)
;;  C_IN: 0x00: dynamic priority sampling disabled
;;        0x01: dynamic priority sampling enabled (default)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_DynamicPrioSet
	SET_BSR	MIOS_AIN_CTRL
	bsf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_DISABLE, BANKED
	btfsc	WREG, 0
	bcf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_DISABLE, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_DynamicPrioGet
;;  C_DECLARATION: unsigned char MIOS_AIN_DynamicPrioGet(void)
;;  DESCRIPTION: returns the status of the dynamic priority sampling feature.
;;  The flag will only be taken into account in multiplexed mode (more than 8
;;  pots connected to the core module via AINX4). If active, the sampling 
;;  frequency of the two last turned pots will be dynamically increased 
;;  for a better accuracy.
;;  IN:   -
;;  C_IN:  -
;;  OUT:  WREG = 0x00: dynamic priority sampling disabled or non-multiplexed mode active
;;        WREG = 0x01: dynamic priority sampling enabled (default)
;;  C_OUT:  0x00: dynamic priority sampling disabled or non-multiplexed mode active
;;        0x01: dynamic priority sampling enabled (default)
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_DynamicPrioGet
	SET_BSR	MIOS_AIN_CTRL
	movlw	0x00
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_DISABLE, BANKED
	movlw	0x01
	andlw	0xff		; fix STATUS
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_AIN_LastPinsGet
;;  C_DECLARATION: unsigned char MIOS_AIN_LastPinsGet(void)
;;  DESCRIPTION: returns the index of the two pins which have been
;;  sampled with a different value at last.<BR>
;;  In less abstract words: returns the number of the last two turned pots.
;;  IN:   -
;;  C_IN:   -
;;  OUT:  WREG and MIOS_PARAMETER1: last pin
;;        MIOS_PARAMETER2: last but one pin
;;  C_OUT:  returned value and MIOS_PARAMETER1: last pin
;;        MIOS_PARAMETER2: last but one pin
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_AIN_LastPinsGet
	SET_BSR	MIOS_AIN_LAST_POT0
	movff	MIOS_AIN_LAST_POT1, MIOS_PARAMETER2
	movf	MIOS_AIN_LAST_POT0, W, BANKED
	movwf	MIOS_PARAMETER1
	return

;; --------------------------------------------------------------------------
;;  MIOS AIN Handler
;;  check changed AIN pins, call USER_AIN_NotifyChange when conversion result
;;  of AIN pin has been changed
;; --------------------------------------------------------------------------
MIOS_AIN_Handler
	SET_BSR	MIOS_TMP_CTR
	clrf	MIOS_TMP_CTR, BANKED

	movf	MIOS_AIN_NUMBER, W, BANKED
	skpnz
	return

	lfsr	FSR0, MIOS_AIN_RESULT_H_00
	movf	MIOS_TMP_CTR, W, BANKED
MIOS_AIN_Handler_Loop
	BRA_IFCLR PLUSW0, 7, ACCESS, MIOS_AIN_Handler_Loop_Next
	bcf	PLUSW0, 7

	movff	PLUSW0, MIOS_PARAMETER3
	bcf	FSR0L, 6
	movff	PLUSW0, MIOS_PARAMETER2
	;; call hook
	movwf	MIOS_PARAMETER1
	CALL_IFCLR MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER, ACCESS, USER_AIN_NotifyChange

	lfsr	FSR0, MIOS_AIN_RESULT_H_00
	SET_BSR	MIOS_TMP_CTR
MIOS_AIN_Handler_Loop_Next
	incf	MIOS_TMP_CTR, F, BANKED
	movf	MIOS_TMP_CTR, W, BANKED
	cpfseq	MIOS_AIN_NUMBER, BANKED
	rgoto MIOS_AIN_Handler_Loop
	return


;; --------------------------------------------------------------------------
;;  MIOS AIN Tick
;;  called by timer0 interrupt every 100 us
;; --------------------------------------------------------------------------
MIOS_AIN_Tick
	SET_BSR	MIOS_AIN_CTRL			; branch depending on state
	BRA_IFSET MIOS_AIN_CTRL, MIOS_AIN_CTRL_STATE, BANKED, MIOS_AIN_Tick_State1

	;; STATE0: start conversion
MIOS_AIN_Tick_State0
	bsf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_STATE, BANKED	; set next state
	bsf	ADCON0, GO					; do a dummy conversion
	rgoto	MIOS_AIN_Tick_End

	;; STATE1: get result and set multiplexers for next input
MIOS_AIN_Tick_State1
	bcf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_STATE, BANKED	; set next state

	lfsr	FSR2, MIOS_AIN_RESULT_L_00
	movf	MIOS_AIN_INPUT_CTR, W, BANKED
	BRA_IFCLR MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_ACTIVE, BANKED, MIOS_AIN_Tick_State1_NoDP
	movf	MIOS_AIN_LAST_POT0, W, BANKED
	btfsc	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_POT, BANKED
	movf	MIOS_AIN_LAST_POT1, W, BANKED
MIOS_AIN_Tick_State1_NoDP
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
	movwf	MIOS_AIN_POTSPEED, BANKED	; this value is also used by the MF driver

	;; save new result if absolute difference > MIOS_AIN_DEADBAND
	movf	MIOS_AIN_DEADBAND, W, BANKED
	cpfsgt	MIOS_AIN_POTSPEED, BANKED
	rgoto MIOS_AIN_Tick_State1_NoNew

MIOS_AIN_Tick_State1_New
	;; save new result
	bcf	FSR2L, 6
	movff	ADRESL, INDF2
	bsf	FSR2L, 6
	movff	ADRESH, INDF2
	bsf	INDF2, 7	; set "changed" bit
	bcf	FSR2L, 6

	;; if result < 5, then clear it (lowest position reached)
	movf	ADRESH, W
	bnz	MIOS_AIN_Tick_State1_New_No0
	movlw	0x05
	cpfslt	ADRESL, ACCESS
	rgoto MIOS_AIN_Tick_State1_New_No0
	clrf	INDF2
MIOS_AIN_Tick_State1_New_No0

	;; save pot number in MIOS_AIN_POT_LAST[01]
	movf	FSR2L, W
	andlw	0x3f
	cpfseq	MIOS_AIN_LAST_POT0, BANKED
	rgoto MIOS_AIN_Tick_State1_NewLast0
	rgoto	MIOS_AIN_Tick_State1_NoNew
MIOS_AIN_Tick_State1_NewLast0
	movff	MIOS_AIN_LAST_POT0, MIOS_AIN_LAST_POT1
	movwf	MIOS_AIN_LAST_POT0, BANKED
MIOS_AIN_Tick_State1_NoNew

	;; if motorfader mode: branch to MF tick, else use standard result handler
	BRA_IFSET MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED, MIOS_MF_Tick
MIOS_MF_Tick_Return
	SET_BSR	MIOS_AIN_INPUT_CTR
MIOS_AIN_Tick_State1_NoMF

	;; dynamic priority feature (only used in muxed mode)
MIOS_AIN_Tick_State1_DynPrio
	BRA_IFCLR MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_ACTIVE, BANKED, MIOS_AIN_Tick_State1_DynPrioChk
	bcf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_ACTIVE, BANKED
	rgoto	MIOS_AIN_Tick_State1_NoDynPrio
MIOS_AIN_Tick_State1_DynPrioChk
	BRA_IFCLR MIOS_AIN_CTRL, MIOS_AIN_CTRL_MUXED, BANKED, MIOS_AIN_Tick_State1_NoDynPrio
	BRA_IFSET MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_DISABLE, BANKED, MIOS_AIN_Tick_State1_NoDynPrio
	;; every 4th conversion...
	movf	MIOS_AIN_INPUT_CTR, W, BANKED
	andlw	0x03
	bnz	MIOS_AIN_Tick_State1_NoDynPrio
	;; ...convert one of the last two turned pots if dynamic priority function is enabled
	bsf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_ACTIVE, BANKED
	btg	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_POT, BANKED
	rgoto	MIOS_AIN_Tick_State1_DynPrioNow; (don't increment input counter)
MIOS_AIN_Tick_State1_NoDynPrio

	;; continue with next input pin
	incf	MIOS_AIN_INPUT_CTR, F, BANKED		; increment input counter
	movf	MIOS_AIN_NUMBER, W, BANKED
	bz	MIOS_AIN_Tick_Stop
	xorwf	MIOS_AIN_INPUT_CTR, W, BANKED		; reset if max value reached
	skpnz
	clrf	MIOS_AIN_INPUT_CTR, BANKED
MIOS_AIN_Tick_State1_DynPrioNow; (don't increment input counter)

	BRA_IFSET MIOS_AIN_CTRL, MIOS_AIN_CTRL_MUXED, BANKED, MIOS_AIN_Tick_State1_Muxed
MIOS_AIN_Tick_State1_UnMuxed
	swapf	MIOS_AIN_INPUT_CTR, W, BANKED ; calc ADCON0 value (0x81 | (pot number << 3)
	movwf	IRQ_TMP1
	rrf	IRQ_TMP1, W
	rgoto	MIOS_AIN_Tick_State1_UnMuxedCnt

MIOS_AIN_Tick_State1_Muxed
	movf	MIOS_AIN_INPUT_CTR, W, BANKED
	BRA_IFCLR MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_ACTIVE, BANKED, MIOS_AIN_Tick_State1_Muxed_NoDP
	movf	MIOS_AIN_LAST_POT0, W, BANKED
	btfsc	MIOS_AIN_CTRL, MIOS_AIN_CTRL_DYNPRIO_POT, BANKED
	movf	MIOS_AIN_LAST_POT1, W, BANKED
MIOS_AIN_Tick_State1_Muxed_NoDP

	BRA_IFSET WREG, 2, ACCESS, MIOS_AIN_Tick_State1_Muxed4567
MIOS_AIN_Tick_State1_Muxed0123
	BRA_IFSET WREG, 1, ACCESS, MIOS_AIN_Tick_State1_Muxed23
MIOS_AIN_Tick_State1_Muxed01
	bsf	MIOS_AIN_LAT_MUX_0, MIOS_AIN_PIN_MUX_0
	bsf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	BRA_IFSET WREG, 0, ACCESS, MIOS_AIN_Tick_State1_Muxed1
	
MIOS_AIN_Tick_State1_Muxed0
	bcf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	rgoto	MIOS_AIN_Tick_State1_MuxedCont
MIOS_AIN_Tick_State1_Muxed1
	bsf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	rgoto	MIOS_AIN_Tick_State1_MuxedCont

MIOS_AIN_Tick_State1_Muxed23
	bsf	MIOS_AIN_LAT_MUX_0, MIOS_AIN_PIN_MUX_0
	bcf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	BRA_IFSET WREG, 0, ACCESS, MIOS_AIN_Tick_State1_Muxed3

MIOS_AIN_Tick_State1_Muxed2
	bsf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	rgoto	MIOS_AIN_Tick_State1_MuxedCont
MIOS_AIN_Tick_State1_Muxed3
	bcf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	rgoto	MIOS_AIN_Tick_State1_MuxedCont

MIOS_AIN_Tick_State1_Muxed4567
	BRA_IFSET WREG, 1, ACCESS, MIOS_AIN_Tick_State1_Muxed67
MIOS_AIN_Tick_State1_Muxed45
	bcf	MIOS_AIN_LAT_MUX_0, MIOS_AIN_PIN_MUX_0
	BRA_IFSET WREG, 0, ACCESS, MIOS_AIN_Tick_State1_Muxed5

MIOS_AIN_Tick_State1_Muxed4
	bsf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	bcf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	rgoto	MIOS_AIN_Tick_State1_MuxedCont
MIOS_AIN_Tick_State1_Muxed5
	bcf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	bsf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	rgoto	MIOS_AIN_Tick_State1_MuxedCont

MIOS_AIN_Tick_State1_Muxed67
	bcf	MIOS_AIN_LAT_MUX_0, MIOS_AIN_PIN_MUX_0
	BRA_IFSET WREG, 0, ACCESS, MIOS_AIN_Tick_State1_Muxed7

MIOS_AIN_Tick_State1_Muxed6
	bcf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	bcf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	rgoto	MIOS_AIN_Tick_State1_MuxedCont
MIOS_AIN_Tick_State1_Muxed7
	bsf	MIOS_AIN_LAT_MUX_1, MIOS_AIN_PIN_MUX_1
	bsf	MIOS_AIN_LAT_MUX_2, MIOS_AIN_PIN_MUX_2
	;; 	rgoto	MIOS_AIN_Tick_State1_MuxedCont

MIOS_AIN_Tick_State1_MuxedCont
	;; 	movf	MIOS_AIN_INPUT_CTR, W, BANKED
	;; already in WREG
	;; can be different when dynamic priority active
MIOS_AIN_Tick_State1_UnMuxedCnt
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

MIOS_AIN_Tick_End
	return
	

	;; called when MIOS_AIN_NUMBER is zero: stop this timer
	;; timer can be enabled again with the MIOS_AIN_NumberSet
MIOS_AIN_Tick_Stop
	bcf	INTCON, T0IE
	bcf	T0CON, TMR0ON
	return

