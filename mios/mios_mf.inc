; $Id$
;
; MIOS Motorfader Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; driver can be optionally disabled
#if MIOS_ENABLE_AIN && MIOS_ENABLE_MF


;; for debugging ONLY: the ADC values during a move are logged at 0x080-0x37f
#define MIOS_MF_LOG_MOVES 0


;; Pins of MF Shift Registers
MIOS_MF_LAT_RCLK	EQU	LATC	; Pin C.0
MIOS_MF_PIN_RCLK	EQU	0
MIOS_MF_LAT_SCLK	EQU	LATC	; Pin C.1
MIOS_MF_PIN_SCLK	EQU	1
MIOS_MF_LAT_DOUT	EQU	LATC	; Pin C.2
MIOS_MF_PIN_DOUT	EQU	2

MIOS_MF_TIMEOUT_RELOAD_VALUE EQU 0xff
MIOS_MF_REPEAT_CTR_VALUE     EQU 0x1f
MIOS_MF_MANUAL_CTR_VALUE     EQU 0xff

;; ==========================================================================
;;  Motor Pins
;; ==========================================================================
MIOS_MF_PIN_Mx_UP       EQU     0
MIOS_MF_PIN_Mx_DOWN     EQU     1

MIOS_MF_REG_M1          EQU     MIOS_MF_SR0
MIOS_MF_PIN_M1_UP       EQU     6
MIOS_MF_PIN_M1_DOWN     EQU     7
MIOS_MF_REG_M2          EQU     MIOS_MF_SR0
MIOS_MF_PIN_M2_UP       EQU     4
MIOS_MF_PIN_M2_DOWN     EQU     5
MIOS_MF_REG_M3          EQU     MIOS_MF_SR0
MIOS_MF_PIN_M3_UP       EQU     2
MIOS_MF_PIN_M3_DOWN     EQU     3
MIOS_MF_REG_M4          EQU     MIOS_MF_SR0
MIOS_MF_PIN_M4_UP       EQU     0
MIOS_MF_PIN_M4_DOWN     EQU     1
MIOS_MF_REG_M5          EQU     MIOS_MF_SR1
MIOS_MF_PIN_M5_UP       EQU     6
MIOS_MF_PIN_M5_DOWN     EQU     7
MIOS_MF_REG_M6          EQU     MIOS_MF_SR1
MIOS_MF_PIN_M6_UP       EQU     4
MIOS_MF_PIN_M6_DOWN     EQU     5
MIOS_MF_REG_M7          EQU     MIOS_MF_SR1
MIOS_MF_PIN_M7_UP       EQU     2
MIOS_MF_PIN_M7_DOWN     EQU     3
MIOS_MF_REG_M8          EQU     MIOS_MF_SR1
MIOS_MF_PIN_M8_UP       EQU     0
MIOS_MF_PIN_M8_DOWN     EQU     1

#if MIOS_MF_LOG_MOVES
MIOS_MF_LOG_PTR_L	EQU	0x7c
MIOS_MF_LOG_PTR_H	EQU	0x7d

MIOS_MF_LOG_SAVED_FSR1L EQU	0x7e
MIOS_MF_LOG_SAVED_FSR1H	EQU	0x7f

;; clears log range from 0x080-0x37f
MIOS_MF_LOG_Init
	movff	FSR1L, MIOS_MF_LOG_SAVED_FSR1L
	movff	FSR1H, MIOS_MF_LOG_SAVED_FSR1H

	lfsr	FSR1, 0x080
MIOS_MF_LOG_Init_Loop
	clrf	POSTINC1

	movf	FSR1H, W
	xorlw	0x03
	bnz	MIOS_MF_LOG_Init_Loop
	movf	FSR1L, W
	xorlw	0x80
	bnz	MIOS_MF_LOG_Init_Loop

	lfsr	FSR1, 0x080
	movff	FSR1L, MIOS_MF_LOG_PTR_L
	movff	FSR1H, MIOS_MF_LOG_PTR_H

	movff	MIOS_MF_LOG_SAVED_FSR1L, FSR1L
	movff	MIOS_MF_LOG_SAVED_FSR1H, FSR1H
	return


MIOS_MF_LOG_Push
	movff	FSR1L, MIOS_MF_LOG_SAVED_FSR1L
	movff	FSR1H, MIOS_MF_LOG_SAVED_FSR1H

	movff	MIOS_MF_LOG_PTR_L, FSR1L
	movff	MIOS_MF_LOG_PTR_H, FSR1H

	movwf	POSTINC1

	movf	FSR1H, W
	xorlw	0x03
	bnz	MIOS_MF_LOG_Push_DontResetPtr
	movf	FSR1L, W
	xorlw	0x80
	bnz	MIOS_MF_LOG_Push_DontResetPtr
MIOS_MF_LOG_Push_ResetPtr
	lfsr	FSR1, 0x080
MIOS_MF_LOG_Push_DontResetPtr

	movff	FSR1L, MIOS_MF_LOG_PTR_L
	movff	FSR1H, MIOS_MF_LOG_PTR_H
	
	movff	MIOS_MF_LOG_SAVED_FSR1L, FSR1L
	movff	MIOS_MF_LOG_SAVED_FSR1H, FSR1H
	return
#endif

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_FaderMove
;;  C_DECLARATION: void MIOS_MF_FaderMove(unsigned char fader, unsigned int pos)
;;  DESCRIPTION: set target position and move fader
;;  NOTE: function can only be used when motordriver has been enabled.
;;  IN:	  fader number in WREG, fader position in MIOS_PARAMETER[12]
;;  C_IN: fader number in <fader>, fader position in <pos>
;;  OUT:  -
;;  USES: FSR1, BSR
;;  EXAMPLE:
;;	;; move fader #7 to highest position (0x3ff)
;;	movlw	0x03
;;	movwf	MIOS_PARAMETER2
;;	movlw	0xff
;;	movwf	MIOS_PARAMETER1
;;	movlw	0x07
;;	call	MIOS_MF_FaderMove
;;  C_EXAMPLE:
;;	// move fader #7 to highest position (0x3ff)
;;	MIOS_MF_FaderMove(0x07, 0x3ff);
;; --------------------------------------------------------------------------
MIOS_MF_FaderMove
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return

	IRQ_DISABLE

	;; ignore new value if fader is currently manually moved (feedback killer)
	lfsr	FSR1, MIOS_MF_MANUAL_CTR0
	andlw	0x07
	addwf	FSR1L, F
	movf	INDF1, W
	bnz	MIOS_MF_FaderMove_Skip

	movlw	(MIOS_MF_TARGET_POS_L0-MIOS_MF_MANUAL_CTR0) & 0xff
	addwf	FSR1L, F
	movff	MIOS_PARAMETER1, INDF1
	bsf	FSR1L, 3
	movff	MIOS_PARAMETER2, INDF1

	movlw	(MIOS_MF_REPEAT_CTR0-MIOS_MF_TARGET_POS_H0) & 0xff
	addwf	FSR1L, F
	movf	INDF1, W
	movlw	MIOS_MF_REPEAT_CTR_VALUE
	movwf	INDF1

	movlw	(MIOS_MF_TIMEOUT_CTR0-MIOS_MF_REPEAT_CTR0) & 0xff
	addwf	FSR1L, F
	movlw	MIOS_MF_TIMEOUT_RELOAD_VALUE
	movwf	INDF1

#if MIOS_MF_LOG_MOVES
	;; clear log buffer if fader number == 0x00
	movf	FSR1L, W
	andlw	0x07
	skpnz
	rcall	MIOS_MF_LOG_Init
#endif

MIOS_MF_FaderMove_Skip
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_Enable
;;  C_DECLARATION: void MIOS_MF_Enable(void)
;;  DESCRIPTION: enables the MF module - in this mode, multiplexers
;;  are disabled. Up to 8 motorfaders can be controlled over the MUX port.
;;  This function can only be used when the motordriver has been enabled.
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;;
;;	;; initialize the MF driver for Alps RSAON11M9 faders
;;	call	MIOS_MF_Enable
;;	movlw	3
;;	call	MIOS_MF_DeadbandSet
;;
;;	movlw	1
;;	call	MIOS_MF_PWM_DutyCycleUpSet
;;	movlw	1
;;	call	MIOS_MF_PWM_DutyCycleDownSet
;;	movlw	3
;;	call	MIOS_MF_PWM_PeriodSet
;;
;;  C_EXAMPLE:
;;
;;	;; initialize the MF driver for Alps RSAON11M9 faders
;;	MIOS_MF_Enable();
;;	MIOS_MF_DeadbandSet(3);
;;	MIOS_MF_PWM_DutyCycleUpSet(1);
;;	MIOS_MF_PWM_DutyCycleDownSet(1);
;;	MIOS_MF_PWM_PeriodSet(3);
;;
;; --------------------------------------------------------------------------
MIOS_MF_Enable
	;; ensure that max. 8 faders are active
	SET_BSR MIOS_AIN_NUMBER
	movlw	0x08
	cpfsgt	MIOS_AIN_NUMBER, BANKED
	rgoto MIOS_MF_Enable_NumberOk
	movwf	MIOS_AIN_NUMBER, BANKED
MIOS_MF_Enable_NumberOk

	SET_BSR	MIOS_AIN_CTRL
	bsf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED

	SET_BSR	MIOS_MF_SUSPEND
	clrf	MIOS_MF_SUSPEND, BANKED

	;; set initial PWM values
	movlw	0x03
	rcall	MIOS_MF_PWM_PeriodSet
	movlw	0x01
	rcall	MIOS_MF_PWM_DutyCycleDownSet
	movlw	0x01
	rcall	MIOS_MF_PWM_DutyCycleUpSet

	;; set initial deadband
	movlw	0x01
	rcall	MIOS_MF_DeadbandSet

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_Disable
;;  C_DECLARATION: void MIOS_MF_Disable(void)
;;  DESCRIPTION: disables the MF module
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_Disable
	SET_BSR	MIOS_AIN_CTRL
	bcf	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_SuspendEnable
;;  C_DECLARATION: void MIOS_MF_SuspendEnable(unsigned char fader)
;;  DESCRIPTION: suspends the motor<BR>
;;  (function used by touchsensor detection)<BR>
;;  function can only be used when motordriver has been enabled.
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_SuspendEnable
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MIOS_MF_SUSPEND
	iorwf	MIOS_MF_SUSPEND, F, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_SuspendDisable
;;  C_DECLARATION: void MIOS_MF_SuspendDisable(unsigned char fader)
;;  DESCRIPTION: deactivate suspend mode of motor<BR>
;;  (function used by touchsensor detection)<BR>
;;  function can only be used when motordriver has been enabled.
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
MIOS_MF_SuspendDisable
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	SET_BSR	MIOS_TMP1
	movwf	MIOS_TMP1, BANKED	
	lfsr	FSR1, MIOS_MF_MANUAL_CTR0
	andlw	0x07
	addwf	FSR1L, F
	movlw	MIOS_MF_MANUAL_CTR_VALUE
	movwf	INDF1

	movf	MIOS_TMP1, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	SET_BSR	MIOS_MF_SUSPEND
	andwf	MIOS_MF_SUSPEND, F, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_SuspendGet
;;  C_DECLARATION: unsigned char MIOS_MF_SuspendGet(unsigned char fader)
;;  DESCRIPTION: return suspend state of motor
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  WREG = 1 if motor is suspended, else 0
;;  C_OUT:  1 if motor is suspended, else 0
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_SuspendGet
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MIOS_MF_SUSPEND
	andwf	MIOS_MF_SUSPEND, W, BANKED
	movlw	0x00
	skpz
	movlw	0x01
	andlw	0xff		; to update STATUS register
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_TouchDetectionReset
;;  C_DECLARATION: void MIOS_MF_TouchDetectionReset(unsigned char fader)
;;  DESCRIPTION: this function resets the software implemented touch detection
;;  so that the fader is repositioned regardless if it is currently
;;  manually moved or not
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_TouchDetectionReset
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return

	lfsr	FSR1, MIOS_MF_MANUAL_CTR0
	andlw	0x07
	clrf	PLUSW1
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_DeadbandSet
;;  C_DECLARATION: void MIOS_MF_DeadbandSet(unsigned char deadband)
;;  DESCRIPTION: sets the deadband value for MF driver
;;  NOTE: function can only be used when motordriver has been enabled.
;;  IN:	  deadband value in WREG
;;  C_IN:  deadband value in <deadband>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_DeadbandSet
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	SET_BSR	MIOS_MF_DEADBAND
	movwf	MIOS_MF_DEADBAND, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_DeadbandGet
;;  C_DECLARATION: unsigned char MIOS_MF_DeadbandGet(void)
;;  DESCRIPTION: returns the deadband value for MF driver
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  deadband value in MIOS_PARAMETER1 and WREG
;;  C_OUT:  deadband value
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_DeadbandGet
	SET_BSR	MIOS_MF_DEADBAND
	movf	MIOS_MF_DEADBAND, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_PeriodSet
;;  C_DECLARATION: void MIOS_MF_PWM_PeriodSet(unsigned char speed)
;;  DESCRIPTION: Sets the PWM period - see http://www.ucapps.de/mbhp_mf.html
;;  for detailed informations about this value
;;  IN:	  PWM period in WREG
;;  C_IN:  PWM period in <speed>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_PeriodSet
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	SET_BSR	MIOS_MF_PWM_PERIOD
	movwf	MIOS_MF_PWM_PERIOD, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_PeriodGet
;;  C_DECLARATION: unsigned char MIOS_MF_PWM_PeriodGet(void)
;;  DESCRIPTION: Returns the PWM period - see http://www.ucapps.de/mbhp_mf.html
;;  for detailed informations about this value
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  PWM period in WREG
;;  C_OUT:  PWM period
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_PeriodGet
	SET_BSR	MIOS_MF_PWM_PERIOD
	movf	MIOS_MF_PWM_PERIOD, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_DutyCycleDownSet
;;  C_DECLARATION: void MIOS_MF_PWM_DutyCycleDownSet(unsigned char cali_down)
;;  DESCRIPTION: Sets the Duty Cycle for downward moves - 
;;  see http://www.ucapps.de/mbhp_mf.html for detailed informations about this value
;;  IN:	  Duty Cycle in WREG
;;  C_IN:  Duty Cycle in <cali_down>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_DutyCycleDownSet
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	SET_BSR	MIOS_MF_PWM_DUTY_CYCLE_DOWN
	movwf	MIOS_MF_PWM_DUTY_CYCLE_DOWN, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_DutyCycleDownGet
;;  C_DECLARATION: unsigned char MIOS_MF_PWM_DutyCycleDownGet(void)
;;  DESCRIPTION: Returns the Duty Cycle for downward moves - 
;;  see http://www.ucapps.de/mbhp_mf.html for detailed informations about this value
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  Duty Cycle in WREG
;;  C_OUT:  Duty Cycle
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_DutyCycleDownGet
	SET_BSR	MIOS_MF_PWM_DUTY_CYCLE_DOWN
	movf	MIOS_MF_PWM_DUTY_CYCLE_DOWN, W, BANKED
	movwf	MIOS_PARAMETER1
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_DutyCycleUpSet
;;  C_DECLARATION: void MIOS_MF_PWM_DutyCycleUpSet(unsigned char cali_up)
;;  DESCRIPTION: Sets the Duty Cycle for upward moves - 
;;  see http://www.ucapps.de/mbhp_mf.html for detailed informations about this value
;;  IN:	  Duty Cycle in WREG
;;  C_IN:  Duty Cycle in <cali_up>
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_DutyCycleUpSet
	SET_BSR	MIOS_AIN_CTRL
	btfss	MIOS_AIN_CTRL, MIOS_AIN_CTRL_MF, BANKED
	return
	SET_BSR	MIOS_MF_PWM_DUTY_CYCLE_UP
	movwf	MIOS_MF_PWM_DUTY_CYCLE_UP, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_MF_PWM_DutyCycleUpGet
;;  C_DECLARATION: unsigned char MIOS_MF_PWM_DutyCycleUpGet(void)
;;  DESCRIPTION: Returns the Duty Cycle for upward moves - 
;;  see http://www.ucapps.de/mbhp_mf.html for detailed informations about this value
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  Duty Cycle in WREG
;;  C_OUT:  Duty Cycle
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_MF_PWM_DutyCycleUpGet
	SET_BSR	MIOS_MF_PWM_DUTY_CYCLE_UP
	movf	MIOS_MF_PWM_DUTY_CYCLE_UP, W, BANKED
	movwf	MIOS_PARAMETER1
	return


;; --------------------------------------------------------------------------
;;  MIOS SRM Tick (called every 100 uS by the AIN handler)
;; --------------------------------------------------------------------------
MIOS_MF_SRM_Tick
	SET_BSR	MIOS_MF_NUMBER

	;; load MF shift register
	movff	MIOS_MF_SR1, IRQ_TMP2
	movff	MIOS_MF_SR0, IRQ_TMP1
	movlw	0x10
	movwf	MIOS_MF_NUMBER, BANKED
MIOS_MF_SRM_Tick_Loop
	btfsc	IRQ_TMP2, 7
	bsf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT
	btfss	IRQ_TMP2, 7
	bcf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT

	rlf	IRQ_TMP1, F
        bsf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK	; toggle clock
	rlf	IRQ_TMP2, F
        bcf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
	decfsz	MIOS_MF_NUMBER, F, BANKED		; loop 16 times
	rgoto	MIOS_MF_SRM_Tick_Loop

        bsf     MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK	; trigger latch
        bcf     MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK


	;; increment/decrement the MF counters each 800 uS
	incf	MIOS_MF_CTR_UPDATE_CTR, F, BANKED	; the "counter update counter"
	BRA_IFCLR MIOS_MF_CTR_UPDATE_CTR, 3, BANKED, MIOS_MF_SRM_Tick_NoCtrUpdate
MIOS_MF_SRM_Tick_CtrUpdate
	clrf	MIOS_MF_CTR_UPDATE_CTR, BANKED
	clrf	MIOS_MF_NUMBER, BANKED
MIOS_MF_SRM_Tick_CtrUpdate_Loop

	;; --- decrement MANUAL CTR if != 0
	lfsr	FSR2, MIOS_MF_MANUAL_CTR0
	movf	MIOS_MF_NUMBER, W, BANKED
	movf	PLUSW2, F
	skpz
	decf	PLUSW2, F

	;; --- decrement TIMEOUT if != 0
	lfsr	FSR2, MIOS_MF_TIMEOUT_CTR0
	movf	MIOS_MF_NUMBER, W, BANKED
	movf	PLUSW2, F
	skpz
	decf	PLUSW2, F

	incf	MIOS_MF_NUMBER, F, BANKED
	BRA_IFCLR MIOS_MF_NUMBER, 3, BANKED, MIOS_MF_SRM_Tick_CtrUpdate_Loop

MIOS_MF_SRM_Tick_NoCtrUpdate

	return


;; ------------------------------------------------------------------------------------------------
;;  MIOS MF Motor Tick: motor control routine
;; ------------------------------------------------------------------------------------------------
MIOS_MF_Tick
	;; ----------------------------------------------------------------------------------------
	;; prepare variables
	;; ----------------------------------------------------------------------------------------

	;; --- store FSR1 in temp. register
	movff	FSR1L, MIOS_IRQ_FSR1L
	movff	FSR1H, MIOS_IRQ_FSR1H

	;; --- now we are working in the MIOS_MF page
	SET_BSR	MIOS_MF_NUMBER

	;; --- copy the AIN input counter to MIOS_MF_NUMBER
	movff	MIOS_AIN_INPUT_CTR, MIOS_MF_NUMBER

	;; --- ensure that NUMBER is never greather than 7
	movf	MIOS_MF_NUMBER, W, BANKED
	andlw	0xf8
	skpz
MIOS_MF_Tick_EndlessLoop	; force a watchdog timer reset in this case
	rgoto	MIOS_MF_Tick_EndlessLoop

	;; --- calc initial pointer to MIOS_MF* page and add MF number
	lfsr	FSR2, MIOS_MF_NUMBER & 0xf80
	movf	MIOS_MF_NUMBER, W, BANKED
	addwf	FSR2L, F

	;; --- store current fader value into MIOS_MF_VALUE_[LH]
	movff	ADRESL, MIOS_MF_VALUE_L		; take original value from ADC so that AIN deadband doesn't matter!
	movff	ADRESH, MIOS_MF_VALUE_H

	;; -- clear "changed" flag
	movlw	MIOS_AIN_RESULT_H_00 & 0x7f
	movff	PLUSW2, MIOS_MF_AIN_CHANGED_FLAG; save "changed" flag
	bcf	PLUSW2, 7	; clear "changed" flag - MF driver takes control over this semaphore

	;; --- store target position into MIOS_MF_TARGET_POS_[LH]
	movlw	MIOS_MF_TARGET_POS_L0 & 0x7f
	movff	PLUSW2, MIOS_MF_TARGET_POS_L
	movlw	MIOS_MF_TARGET_POS_H0 & 0x7f
	movff	PLUSW2, MIOS_MF_TARGET_POS_H

	;; --- store current timeout counter MIOS_MF_TIMEOUT_CTR
	movlw	MIOS_MF_TIMEOUT_CTR0 & 0x7f
	movff	PLUSW2, MIOS_MF_TIMEOUT_CTR


	;; ----------------------------------------------------------------------------------------
	;; case: motor on target position (REPEAT_CTRx <= 1)
	;; ----------------------------------------------------------------------------------------
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	movf	PLUSW2, W
	sublw	1
	bnc	MIOS_MF_Tick_NotSnapped
MIOS_MF_Tick_Snapped
	;; --- motor should go into standby mode
	rcall	MIOS_MF_StandBy

	;; if TIMEOUT_CTR == 0, go directly into idle mode
	movf	MIOS_MF_TIMEOUT_CTR, W, BANKED
	bz	MIOS_MF_SnappedIdle

	;; --- get REPEAT_CTRx again
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	movf	PLUSW2, W
	;; --- special case: REPEAT_CTRx == 1: in this case the motor is just stopping
	bz	MIOS_MF_SnappedIdle

	;; --- motor is just stopping, clear CTR
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	clrf	PLUSW2

#if MIOS_MF_LOG_MOVES
	;; log that the target has been reached
	movf	MIOS_MF_NUMBER, W, BANKED
	bnz	MIOS_MF_Tick_DontLogTarget
	movlw	0xff
	rcall	MIOS_MF_LOG_Push
	movlw	0xff
	rcall	MIOS_MF_LOG_Push
MIOS_MF_Tick_DontLogTarget
#endif

	;; ---> END
	rgoto	MIOS_MF_Tick_End

	;; ----------------------------------------------------------------------------------------

MIOS_MF_SnappedIdle
	;; --- copy current value into target position, TIMEOUT counter > 0 (reassurance phase)
	movf	MIOS_MF_TIMEOUT_CTR, W, BANKED
	bz	MIOS_MF_SnappedIdle2
	movlw	MIOS_MF_TARGET_POS_L0 & 0x7f
	movff	MIOS_MF_VALUE_L, PLUSW2
	movlw	MIOS_MF_TARGET_POS_H0 & 0x7f
	movff	MIOS_MF_VALUE_H, PLUSW2
	
	rgoto	MIOS_MF_Tick_End
	
	;; ----------------------------------------------------------------------------------------

MIOS_MF_SnappedIdle2
	;; do nothing if "changed" flag was not set by AIN driver
	BRA_IFCLR MIOS_MF_AIN_CHANGED_FLAG, 7, BANKED, MIOS_MF_Tick_End

	;; --- else: save new value
	movlw	MIOS_MF_TARGET_POS_L0 & 0x7f
	movff	MIOS_MF_VALUE_L, PLUSW2
	movlw	MIOS_MF_TARGET_POS_H0 & 0x7f
	movff	MIOS_MF_VALUE_H, PLUSW2

	;; --- set manual move counter, so that the motor is not moved during this time
	movlw	MIOS_MF_MANUAL_CTR_VALUE
	movwf	IRQ_TMP1
	movlw	MIOS_MF_MANUAL_CTR0 & 0x7f
	movff	IRQ_TMP1, PLUSW2

	;; --- notify change in MIOS_AIN_RESULT_H[7], so that the AIN handler can regognize it
	movlw	MIOS_AIN_RESULT_H_00 & 0x7f
	bsf	PLUSW2, 7

	;; ---> END
	rgoto	MIOS_MF_Tick_End


	;; ----------------------------------------------------------------------------------------
MIOS_MF_Tick_NotSnapped

	;; ----------------------------------------------------------------------------------------
	;; case: motor very slow or not moving
	;; ----------------------------------------------------------------------------------------
	movff	MIOS_AIN_POTSPEED, IRQ_TMP4
	movlw	0x01
	cpfslt	IRQ_TMP4, ACCESS
	rgoto MIOS_MF_Tick_Moving
MIOS_MF_Tick_NotMoving
	;; --- continue if TIMEOUT > 0
	movf	MIOS_MF_TIMEOUT_CTR, W, BANKED
	bnz	MIOS_MF_Tick_MovingCont

	;; --- if TIMEOUT == 0, write 0x01 into REPEAT_CTRx for a proper shutdown
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	clrf	PLUSW2		; (PLUSW2 = 0x01)
	incf	PLUSW2, F
	rgoto	MIOS_MF_Tick_End

	;; ----------------------------------------------------------------------------------------

MIOS_MF_Tick_Moving
	;; --- reload TIMEOUT
	movlw	MIOS_MF_TIMEOUT_RELOAD_VALUE
	movwf	IRQ_TMP1
	movlw	MIOS_MF_TIMEOUT_CTR0 & 0x7f
	movff	IRQ_TMP1, PLUSW2
MIOS_MF_Tick_MovingCont

	;; ----------------------------------------------------------------------------------------
	;; Check Touch Detection. ShutDown Motor if active
	;; ----------------------------------------------------------------------------------------
	movf	MIOS_MF_NUMBER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MIOS_MF_SUSPEND, W, BANKED
	bz	MIOS_MF_Tick_NotSuspended
MIOS_MF_Tick_Suspended
	;; --- write 0x01 into REPEAT_CTRx for a clean shutdown
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	clrf	PLUSW2		; (PLUSW2 = 0x01)
	incf	PLUSW2, F
	;; --- write 0x00 into timeout register
	movlw	MIOS_MF_TIMEOUT_CTR0 & 0x7f
	clrf	PLUSW2
	;; ---> End
	rgoto	MIOS_MF_Tick_End

MIOS_MF_Tick_NotSuspended

	;; ----------------------------------------------------------------------------------------
	;; Don't move motor if speed > 5/1024
	;; ----------------------------------------------------------------------------------------
	movff	MIOS_AIN_POTSPEED, IRQ_TMP4
	movlw	10
	cpfsgt	IRQ_TMP4, ACCESS
	rgoto MIOS_MF_Tick_DontReduceSpeed
MIOS_MF_Tick_ReduceSpeed
	;; --- Dont Move Fader -> Standby
	rcall	MIOS_MF_StandBy
	;; ---> END
	rgoto	MIOS_MF_Tick_End
MIOS_MF_Tick_DontReduceSpeed

	;; ----------------------------------------------------------------------------------------
	;; case: determine in which direction the motor should be moved
	;; ----------------------------------------------------------------------------------------

#if MIOS_MF_LOG_MOVES
	;; log move of fader 0
	movf	MIOS_MF_NUMBER, W, BANKED
	bnz	MIOS_MF_Tick_DontLogValue
	movf	MIOS_MF_VALUE_L, W, BANKED
	rcall	MIOS_MF_LOG_Push
	movf	MIOS_MF_VALUE_H, W, BANKED
	rcall	MIOS_MF_LOG_Push
MIOS_MF_Tick_DontLogValue
#endif

	;; --- special case: if target and current position <= 0x07, stop motor
	;; (workaround for ALPS faders which never reach the 0x00 value)
	movf	MIOS_MF_TARGET_POS_H, W, BANKED
	bnz	MIOS_MF_Tick_NotTargetZero
	movf	MIOS_MF_VALUE_H, W, BANKED
	bnz	MIOS_MF_Tick_NotTargetZero
	movf	MIOS_MF_TARGET_POS_L, W, BANKED
	iorwf	MIOS_MF_VALUE_L, W, BANKED
	andlw	0xf8
	bnz	MIOS_MF_Tick_NotTargetZero
	rgoto	MIOS_MF_DontMove
MIOS_MF_Tick_NotTargetZero

	;; --- check distance between current and target position
	;; --- if fader is in between "deadband", don't move it
	;; calc IRQ_TMP_[12] = MIOS_MF_VALUE_[LH] - MIOS_MF_TARGET_POS_[LH]
	movf	MIOS_MF_TARGET_POS_L, W, BANKED
	subwf	MIOS_MF_VALUE_L, W, BANKED
	movwf	IRQ_TMP1
	movf	MIOS_MF_TARGET_POS_H, W, BANKED
	subwfb	MIOS_MF_VALUE_H, W, BANKED
	movwf	IRQ_TMP2
	;; if result negative, invert values
	bcf	IRQ_TMP3, 0
	BRA_IFCLR IRQ_TMP2, 7, ACCESS, MIOS_MF_SVNotNeg
	comf	IRQ_TMP1, F
	comf	IRQ_TMP2, F
	incf	IRQ_TMP1, F
	skpnz
	incf	IRQ_TMP2, F
	bsf	IRQ_TMP3, 0		; set marker for "motor moved up"
MIOS_MF_SVNotNeg

	movf	IRQ_TMP1, W
	movf	IRQ_TMP2, F
	skpz
	movlw	0xff
	movwf	IRQ_TMP1

	;; dynamic deadband - depends on repeat counter
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	movff	PLUSW2, IRQ_TMP4
	movf	MIOS_MF_DEADBAND, W, BANKED
	btfsc	IRQ_TMP4, 2
	movlw 0x04
	btfsc	IRQ_TMP4, 3
	movlw 0x08
	btfsc	IRQ_TMP4, 4
	movlw 0x20
	cpfsgt	IRQ_TMP1, ACCESS
	rgoto MIOS_MF_DontMove
	rgoto	MIOS_MF_Move

MIOS_MF_DontMove
	;; --- Dont Move Fader -> Standby
	rcall	MIOS_MF_StandBy

	;; --- Decrement Repeat CTR
	movlw	MIOS_MF_REPEAT_CTR0 & 0x7f
	decf	PLUSW2, F

	;; ---> END
	rgoto	MIOS_MF_Tick_End

MIOS_MF_Move

	;; ----------------------------------------------------------------------------------------
	;; slow down motor if distance between current and target position < 0x60
	;; ----------------------------------------------------------------------------------------
	movlw	0x60
	cpfslt	IRQ_TMP1, ACCESS
	rgoto MIOS_MF_Tick_NoPWM

	movlw	MIOS_MF_PWM_CTR0 & 0x7f
	incf	PLUSW2, F
	movff	PLUSW2, IRQ_TMP4
	movf	MIOS_MF_PWM_PERIOD, W, BANKED
	bz	MIOS_MF_Tick_NoPWM
	cpfsgt	IRQ_TMP4, ACCESS
	rgoto MIOS_MF_Tick_NoPWMReset
MIOS_MF_Tick_PWMReset
	clrf	IRQ_TMP4
	movlw	MIOS_MF_PWM_CTR0 & 0x7f
	movff	IRQ_TMP4, PLUSW2
MIOS_MF_Tick_NoPWMReset
	movf	MIOS_MF_PWM_DUTY_CYCLE_DOWN, W, BANKED
	btfsc	IRQ_TMP3, 0
	movf	MIOS_MF_PWM_DUTY_CYCLE_UP, W, BANKED
	cpfsgt	IRQ_TMP4, ACCESS
	rgoto MIOS_MF_Tick_NoPWM
	;; --- Dont Move Fader -> Standby
	rcall	MIOS_MF_StandBy
	;; ---> END
	rgoto	MIOS_MF_Tick_End
MIOS_MF_Tick_NoPWM

	;; --- check if fader should be moved up or down
	BRA_IFSET IRQ_TMP3, 0, ACCESS, MIOS_MF_MoveUp	; check marker for "motor moved up"

MIOS_MF_MoveDown
	;; ---> save direction
	movlw	MIOS_MF_STATE0 & 0x7f
	bcf	PLUSW2, 0
	;; ---> Move Fader Down
	rcall	MIOS_MF_Down
	;; ---> END
	rgoto	MIOS_MF_Tick_End

MIOS_MF_MoveUp
	;; ---> save direction
	movlw	MIOS_MF_STATE0 & 0x7f
	bsf	PLUSW2, 0
	;; ---> Move Fader Up
	rcall	MIOS_MF_Up
	;; ---> END
	;; 	goto	MIOS_MF_Tick_End

MIOS_MF_Tick_End
	;; restore FSR1 from temp. register
	movff	MIOS_IRQ_FSR1L, FSR1L
	movff	MIOS_IRQ_FSR1H, FSR1H

	rgoto	MIOS_MF_Tick_Return



;; --------------------------------------------------------------------------
;; this is a result of a code optimisation
;; requirements:
;;    - can be used by MIOS_MF_StandBy/MIOS_MF_Down and MIOS_MF_Up
;;    - only FSR1 pointer access allowed
;;    - beside of WREG no other register used

;; expecting UP/DOWN bit in WREG[1:0]
MIOS_MF_UpDownStandby_Mod
	lfsr	FSR1, MIOS_MF_SR0
	btfsc	MIOS_MF_NUMBER, 2, BANKED
	lfsr	FSR1, MIOS_MF_SR1

	BRA_IFSET MIOS_MF_NUMBER, 1, BANKED, MIOS_MF_UpDownStandby_Mod_3478
MIOS_MF_UpDownStandby_Mod_1256
	BRA_IFSET MIOS_MF_NUMBER, 0, BANKED, MIOS_MF_UpDownStandby_Mod_26
MIOS_MF_UpDownStandby_Mod_15
	bcf	INDF1, MIOS_MF_PIN_M1_UP
	bcf	INDF1, MIOS_MF_PIN_M1_DOWN
	btfsc	WREG, MIOS_MF_PIN_Mx_UP
	bsf	INDF1, MIOS_MF_PIN_M1_UP
	btfsc	WREG, MIOS_MF_PIN_Mx_DOWN
	bsf	INDF1, MIOS_MF_PIN_M1_DOWN
	return

MIOS_MF_UpDownStandby_Mod_26
	bcf	INDF1, MIOS_MF_PIN_M2_UP
	bcf	INDF1, MIOS_MF_PIN_M2_DOWN
	btfsc	WREG, MIOS_MF_PIN_Mx_UP
	bsf	INDF1, MIOS_MF_PIN_M2_UP
	btfsc	WREG, MIOS_MF_PIN_Mx_DOWN
	bsf	INDF1, MIOS_MF_PIN_M2_DOWN
	return

MIOS_MF_UpDownStandby_Mod_3478
	BRA_IFSET MIOS_MF_NUMBER, 0, BANKED, MIOS_MF_UpDownStandby_Mod_48
MIOS_MF_UpDownStandby_Mod_37
	bcf	INDF1, MIOS_MF_PIN_M3_UP
	bcf	INDF1, MIOS_MF_PIN_M3_DOWN
	btfsc	WREG, MIOS_MF_PIN_Mx_UP
	bsf	INDF1, MIOS_MF_PIN_M3_UP
	btfsc	WREG, MIOS_MF_PIN_Mx_DOWN
	bsf	INDF1, MIOS_MF_PIN_M3_DOWN
	return

MIOS_MF_UpDownStandby_Mod_48
	bcf	INDF1, MIOS_MF_PIN_M4_UP
	bcf	INDF1, MIOS_MF_PIN_M4_DOWN
	btfsc	WREG, MIOS_MF_PIN_Mx_UP
	bsf	INDF1, MIOS_MF_PIN_M4_UP
	btfsc	WREG, MIOS_MF_PIN_Mx_DOWN
	bsf	INDF1, MIOS_MF_PIN_M4_DOWN
	return


;; --------------------------------------------------------------------------
MIOS_MF_Up
	movlw	(1 << MIOS_MF_PIN_Mx_UP)
	rgoto	MIOS_MF_UpDownStandby_Mod

;; --------------------------------------------------------------------------
MIOS_MF_Down
	movlw	(1 << MIOS_MF_PIN_Mx_DOWN)
	rgoto	MIOS_MF_UpDownStandby_Mod

;; --------------------------------------------------------------------------
MIOS_MF_StandBy
	movlw	0
	rgoto	MIOS_MF_UpDownStandby_Mod


#endif	; MIOS_ENABLE_AIN && MIOS_ENABLE_MF
