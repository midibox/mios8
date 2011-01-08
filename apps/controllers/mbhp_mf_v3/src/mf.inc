; $Id$
;
; Modified version of MIOS Motorfader Handler
; Adapted to MBHP_MF_V3 project
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; motorfader modes
#define MF_MODE_FLAG_ENABLED		0
#define MF_MODE_FLAG_MOTOR_INVERTED	1

;; trace states
#define MF_TRACE_STATE_FLAG_ARMED	0
#define MF_TRACE_STATE_FLAG_STARTED	1
#define MF_TRACE_STATE_FLAG_FINISHED	2
#define MF_TRACE_STATE_FLAG_SENT	3

#define MF_USE_DYNAMIC_DEADBAND         1
#define MF_SCALE_PWM_BETWEEN_MIN_MAX    1
#define MF_REDUCE_SPEED_AT_DIFF		127
#define MF_TIMEOUT_RELOAD_VALUE 	0xff
#define MF_REPEAT_CTR_VALUE     	0x1f
#define MF_MANUAL_CTR_VALUE		0xff

;; --------------------------------------------------------------------------
;;  FUNCTION: MF_FaderMove
;;  C_DECLARATION: void MF_FaderMove(unsigned char fader, unsigned int pos)
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
;;	call	MF_FaderMove
;;  C_EXAMPLE:
;;	// move fader #7 to highest position (0x3ff)
;;	MF_FaderMove(0x07, 0x3ff);
;; --------------------------------------------------------------------------
MF_FaderMove
	SET_BSR	AIN_BASE
	btfss	AIN_CTRL, AIN_CTRL_MF, BANKED
	return

	IRQ_DISABLE

	;; ignore new value if fader is currently manually moved (feedback killer)
	lfsr	FSR1, MF_MANUAL_CTR_0
	andlw	0x07
	addwf	FSR1L, F
	movf	INDF1, W
	bnz	MF_FaderMove_Skip

	movlw	(MF_TARGET_POS_L_0-MF_MANUAL_CTR_0) & 0xff
	addwf	FSR1L, F
	movff	MIOS_PARAMETER1, INDF1
	movlw	(MF_TARGET_POS_H_0-MF_TARGET_POS_L_0) & 0xff
	addwf	FSR1L, F
	movff	MIOS_PARAMETER2, INDF1

	movlw	(MF_REPEAT_CTR_0-MF_TARGET_POS_H_0) & 0xff
	addwf	FSR1L, F
	movf	INDF1, W
	movlw	MF_REPEAT_CTR_VALUE
	movwf	INDF1

	movlw	(MF_TIMEOUT_CTR_0-MF_REPEAT_CTR_0) & 0xff
	addwf	FSR1L, F
	movlw	MF_TIMEOUT_RELOAD_VALUE
	movwf	INDF1

	;; clear trace buffer if armed and fader matches with trace fader
	SET_BSR	MF_BASE
	BRA_IFCLR MF_TRACE_STATE, MF_TRACE_STATE_FLAG_ARMED, BANKED, MF_FaderMove_NoTrace
	BRA_IFSET MF_TRACE_STATE, MF_TRACE_STATE_FLAG_STARTED, BANKED, MF_FaderMove_NoTrace
	movf	FSR1L, W
	andlw	0x07
	xorwf	MF_TRACE_FADER, W, BANKED
	bnz	MF_FaderMove_NoTrace
MF_FaderMove_Trace
	bsf	MF_TRACE_STATE, MF_TRACE_STATE_FLAG_STARTED, BANKED
	rcall	MF_TRACE_Init
MF_FaderMove_NoTrace

MF_FaderMove_Skip
	IRQ_ENABLE
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MF_Enable
;;  C_DECLARATION: void MF_Enable(void)
;;  DESCRIPTION: enables the MF module - in this mode, multiplexers
;;  are disabled. Up to 8 motorfaders can be controlled over the MUX port.
;;  This function can only be used when the motordriver has been enabled.
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MF_Enable
	SET_BSR	AIN_BASE

	;; ensure that max. 8 faders are active
	movlw	0x08
	cpfsgt	AIN_NUMBER, BANKED
	rgoto MF_Enable_NumberOk
	movwf	AIN_NUMBER, BANKED
MF_Enable_NumberOk

	bsf	AIN_CTRL, AIN_CTRL_MF, BANKED

	SET_BSR	MF_BASE
	clrf	MF_SUSPEND, BANKED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MF_Disable
;;  C_DECLARATION: void MF_Disable(void)
;;  DESCRIPTION: disables the MF module
;;  IN:	  -
;;  C_IN:  -
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MF_Disable
	SET_BSR	AIN_BASE
	bcf	AIN_CTRL, AIN_CTRL_MF, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MF_SuspendEnable
;;  C_DECLARATION: void MF_SuspendEnable(unsigned char fader)
;;  DESCRIPTION: suspends the motor<BR>
;;  (function used by touchsensor detection)<BR>
;;  function can only be used when motordriver has been enabled.
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MF_SuspendEnable
	SET_BSR	AIN_BASE
	btfss	AIN_CTRL, AIN_CTRL_MF, BANKED
	return
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MF_BASE
	iorwf	MF_SUSPEND, F, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MF_SuspendDisable
;;  C_DECLARATION: void MF_SuspendDisable(unsigned char fader)
;;  DESCRIPTION: deactivate suspend mode of motor<BR>
;;  (function used by touchsensor detection)<BR>
;;  function can only be used when motordriver has been enabled.
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR, FSR1
;; --------------------------------------------------------------------------
MF_SuspendDisable
	SET_BSR	AIN_BASE
	btfss	AIN_CTRL, AIN_CTRL_MF, BANKED
	return
	SET_BSR	MF_BASE
	movwf	MF_TMP1, BANKED	
	lfsr	FSR1, MF_MANUAL_CTR_0
	andlw	0x07
	addwf	FSR1L, F
	movlw	MF_MANUAL_CTR_VALUE
	movwf	INDF1

	movf	MF_TMP1, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	SET_BSR	MF_BASE
	andwf	MF_SUSPEND, F, BANKED
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MF_SuspendGet
;;  C_DECLARATION: unsigned char MF_SuspendGet(unsigned char fader)
;;  DESCRIPTION: return suspend state of motor
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  WREG = 1 if motor is suspended, else 0
;;  C_OUT:  1 if motor is suspended, else 0
;;  USES: BSR
;; --------------------------------------------------------------------------
MF_SuspendGet
	call	MIOS_HLP_GetBitORMask
	SET_BSR	MF_BASE
	andwf	MF_SUSPEND, W, BANKED
	movlw	0x00
	skpz
	movlw	0x01
	andlw	0xff		; to update STATUS register
	return
		
;; --------------------------------------------------------------------------
;;  FUNCTION: MF_TouchDetectionReset
;;  C_DECLARATION: void MF_TouchDetectionReset(unsigned char fader)
;;  DESCRIPTION: this function resets the software implemented touch detection
;;  so that the fader is repositioned regardless if it is currently
;;  manually moved or not
;;  IN:	  number of motor in WREG (0-7)
;;  C_IN:  number of motor in <fader> (0-7)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;; --------------------------------------------------------------------------
MF_TouchDetectionReset
	SET_BSR	AIN_BASE
	btfss	AIN_CTRL, AIN_CTRL_MF, BANKED
	return

	lfsr	FSR1, MF_MANUAL_CTR_0
	andlw	0x07
	clrf	PLUSW1
	return


;; ------------------------------------------------------------------------------------------------
;;  MF Motor Tick: motor control routine
;; ------------------------------------------------------------------------------------------------
MF_Tick
	;; ----------------------------------------------------------------------------------------
	;; prepare variables
	;; ----------------------------------------------------------------------------------------

	;; --- now we are working in the MF page
	SET_BSR	MF_BASE

	;; --- copy the AIN input counter to MF_NUMBER
	movff	AIN_INPUT_CTR, MF_NUMBER

	;; --- ensure that NUMBER is never greather than 7
	movf	MF_NUMBER, W, BANKED
	andlw	0xf8
	skpz
MF_Tick_EndlessLoop	; force a watchdog timer reset in this case
	rgoto	MF_Tick_EndlessLoop

	;; --- store PROD[LH]
	movff	PRODL, MF_STORED_PRODL
	movff	PRODH, MF_STORED_PRODH

	;; --- calc initial pointer to MF* page and add MF number
	lfsr	FSR2, MF_BASE & 0xf80
	movf	MF_NUMBER, W, BANKED
	addwf	FSR2L, F

	;; --- skip if motor not enabled
	movlw	MF_MODE_0 & 0x7f
	BRA_IFSET PLUSW2, MF_MODE_FLAG_ENABLED, ACCESS, MF_Tick_FaderEnabled
	rcall	MF_StandBy	; force standby fader
	rgoto	MF_Tick_End
MF_Tick_FaderEnabled

	;; --- store current fader value into MF_VALUE_[LH]
	movff	ADRESL, MF_VALUE_L		; take original value from ADC so that AIN deadband doesn't matter!
	movff	ADRESH, MF_VALUE_H
	
	;; --- store also in MF_TRACE_VALUE_[LH]?
	movf	MF_TRACE_FADER, W, BANKED
	xorwf	MF_NUMBER, W, BANKED
	bnz	MF_Tick_NotTraceFader
MF_Tick_TraceFader
	movff	ADRESL, MF_TRACE_VALUE_L	; take original value from ADC so that AIN deadband doesn't matter!
	movff	ADRESH, MF_TRACE_VALUE_H
MF_Tick_NotTraceFader

	;; -- clear "changed" flag
	lfsr	FSR1, AIN_RESULT_H_00
	movf	MF_NUMBER, W, BANKED
	movff	PLUSW1, MF_AIN_CHANGED_FLAG; save "changed" flag
	bcf	PLUSW1, 7	; clear "changed" flag - MF driver takes control over this semaphore

	;; --- store target position into MF_TARGET_POS_[LH]
	movlw	MF_TARGET_POS_L_0 & 0x7f
	movff	PLUSW2, MF_TARGET_POS_L
	movlw	MF_TARGET_POS_H_0 & 0x7f
	movff	PLUSW2, MF_TARGET_POS_H

	;; --- store current timeout counter MF_TIMEOUT_CTR
	movlw	MF_TIMEOUT_CTR_0 & 0x7f
	movff	PLUSW2, MF_TIMEOUT_CTR


	;; ----------------------------------------------------------------------------------------
	;; case: motor on target position (REPEAT_CTRx <= 1)
	;; ----------------------------------------------------------------------------------------
	movlw	MF_REPEAT_CTR_0 & 0x7f
	movf	PLUSW2, W
	sublw	1
	bnc	MF_Tick_NotSnapped
MF_Tick_Snapped
	;; --- motor should go into standby mode
	rcall	MF_StandBy

	;; if TIMEOUT_CTR == 0, go directly into idle mode
	movf	MF_TIMEOUT_CTR, W, BANKED
	bz	MF_SnappedIdle

	;; --- get REPEAT_CTRx again
	movlw	MF_REPEAT_CTR_0 & 0x7f
	movf	PLUSW2, W
	;; --- special case: REPEAT_CTRx == 1: in this case the motor is just stopping
	bz	MF_SnappedIdle

	;; --- motor is just stopping, clear CTR
	movlw	MF_REPEAT_CTR_0 & 0x7f
	clrf	PLUSW2

	;; ---> END
	rgoto	MF_Tick_End

	;; ----------------------------------------------------------------------------------------

MF_SnappedIdle
	;; --- copy current value into target position, TIMEOUT counter > 0 (reassurance phase)
	movf	MF_TIMEOUT_CTR, W, BANKED
	bz	MF_SnappedIdle2
	movlw	MF_TARGET_POS_L_0 & 0x7f
	movff	MF_VALUE_L, PLUSW2
	movlw	MF_TARGET_POS_H_0 & 0x7f
	movff	MF_VALUE_H, PLUSW2
	
	rgoto	MF_Tick_End
	
	;; ----------------------------------------------------------------------------------------

MF_SnappedIdle2
	;; do nothing if "changed" flag was not set by AIN driver
	BRA_IFCLR MF_AIN_CHANGED_FLAG, 7, BANKED, MF_Tick_End

	;; --- else: save new value
	movlw	MF_TARGET_POS_L_0 & 0x7f
	movff	MF_VALUE_L, PLUSW2
	movlw	MF_TARGET_POS_H_0 & 0x7f
	movff	MF_VALUE_H, PLUSW2

	;; --- set manual move counter, so that the motor is not moved during this time
	movlw	MF_MANUAL_CTR_VALUE
	movwf	IRQ_TMP1
	movlw	MF_MANUAL_CTR_0 & 0x7f
	movff	IRQ_TMP1, PLUSW2

	;; --- notify change in AIN_RESULT_H[7], so that the AIN handler can regognize it
	lfsr	FSR1, AIN_RESULT_H_00
	movf	MF_NUMBER, W, BANKED
	bsf	PLUSW1, 7

	;; ---> END
	rgoto	MF_Tick_End


	;; ----------------------------------------------------------------------------------------
MF_Tick_NotSnapped

	;; ----------------------------------------------------------------------------------------
	;; case: motor very slow or not moving
	;; ----------------------------------------------------------------------------------------
	movlw	0x07
	cpfslt	AIN_POTSPEED, BANKED
	rgoto	MF_Tick_Moving
MF_Tick_NotMoving
	;; --- continue if TIMEOUT > 0
	movf	MF_TIMEOUT_CTR, W, BANKED
	bnz	MF_Tick_MovingCont

	;; --- Dont Move Fader -> Standby
	rcall	MF_StandBy

	;; --- if TIMEOUT == 0, write 0x01 into REPEAT_CTRx for a proper shutdown
	movlw	MF_REPEAT_CTR_0 & 0x7f
	clrf	PLUSW2		; (PLUSW2 = 0x01)
	incf	PLUSW2, F
	rgoto	MF_Tick_End

	;; ----------------------------------------------------------------------------------------

MF_Tick_Moving
	;; --- reload TIMEOUT
	movlw	MF_TIMEOUT_RELOAD_VALUE
	movwf	IRQ_TMP1
	movlw	MF_TIMEOUT_CTR_0 & 0x7f
	movff	IRQ_TMP1, PLUSW2
MF_Tick_MovingCont

	;; ----------------------------------------------------------------------------------------
	;; Check Touch Detection. ShutDown Motor if active
	;; ----------------------------------------------------------------------------------------
	movf	MF_NUMBER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	andwf	MF_SUSPEND, W, BANKED
	bz	MF_Tick_NotSuspended
MF_Tick_Suspended
	;; --- write 0x01 into REPEAT_CTRx for a clean shutdown
	movlw	MF_REPEAT_CTR_0 & 0x7f
	clrf	PLUSW2		; (PLUSW2 = 0x01)
	incf	PLUSW2, F
	;; --- write 0x00 into timeout register
	movlw	MF_TIMEOUT_CTR_0 & 0x7f
	clrf	PLUSW2
	;; ---> End
	rgoto	MF_Tick_End

MF_Tick_NotSuspended

	;; ----------------------------------------------------------------------------------------
	;; case: determine in which direction the motor should be moved
	;; ----------------------------------------------------------------------------------------

	;; --- special case: if target and current position < MF_MIN_VALUE, stop motor
	movf	MF_TARGET_POS_H, W, BANKED
	bnz	MF_Tick_NotTargetMin
	movf	MF_VALUE_H, W, BANKED
	bnz	MF_Tick_NotTargetMin
	movlw	MF_MIN_VALUE_L_0 & 0x7f
	movf	PLUSW2, W
	cpfslt	MF_TARGET_POS_L, BANKED
	rgoto	MF_Tick_NotTargetMin
	cpfslt	MF_VALUE_L, BANKED
	rgoto	MF_Tick_NotTargetMin
	rgoto	MF_DontMove
MF_Tick_NotTargetMin

	;; --- special case: if target and current position > MF_MAX_VALUE, stop motor
	movf	MF_TARGET_POS_H, W, BANKED
	xorlw	0x03
	bnz	MF_Tick_NotTargetMax
	movf	MF_VALUE_H, W, BANKED
	xorlw	0x03
	bnz	MF_Tick_NotTargetMax
	movlw	MF_MAX_VALUE_L_0 & 0x7f
	movf	PLUSW2, W
	cpfsgt	MF_TARGET_POS_L, BANKED
	rgoto	MF_Tick_NotTargetMax
	cpfsgt	MF_VALUE_L, BANKED
	rgoto	MF_Tick_NotTargetMax
	rgoto	MF_DontMove
MF_Tick_NotTargetMax

	;; --- check distance between current and target position
	;; --- if fader is in between "deadband", don't move it
	;; calc MF_TARGET_DIFF_[LH] = MF_VALUE_[LH] - MF_TARGET_POS_[LH]
	movf	MF_TARGET_POS_L, W, BANKED
	subwf	MF_VALUE_L, W, BANKED
	movwf	MF_TARGET_DIFF, BANKED
	movf	MF_TARGET_POS_H, W, BANKED
	subwfb	MF_VALUE_H, W, BANKED
	movwf	MF_TARGET_DIFF_H, BANKED
	;; if result negative, invert values
	bcf	MF_TARGET_MOTOR_DIR, 0, BANKED
	BRA_IFCLR MF_TARGET_DIFF_H, 7, BANKED, MF_SVNotNeg
	comf	MF_TARGET_DIFF, F, BANKED
	comf	MF_TARGET_DIFF_H, F, BANKED
	incf	MF_TARGET_DIFF, F, BANKED
	skpnz
	incf	MF_TARGET_DIFF_H, F, BANKED
	bsf	MF_TARGET_MOTOR_DIR, 0, BANKED		; set marker for "motor moved up"
MF_SVNotNeg

	;; saturate difference at 0xff (therefore the appr. variable has no _L postfix)
	movf	MF_TARGET_DIFF, W, BANKED
	movf	MF_TARGET_DIFF_H, F, BANKED
	skpz
	movlw	0xff
	movwf	MF_TARGET_DIFF, BANKED

	;; dynamic deadband - depends on repeat counter
	movlw	MF_REPEAT_CTR_0 & 0x7f
	movff	PLUSW2, IRQ_TMP4
	movf	MF_DEADBAND, W, BANKED
#if MF_USE_DYNAMIC_DEADBAND
	btfsc	IRQ_TMP4, 2
	movlw	0x04
	btfsc	IRQ_TMP4, 3
	movlw	0x08
	btfsc	IRQ_TMP4, 4
	movlw	0x10
#endif
	cpfsgt	MF_TARGET_DIFF, BANKED
	rgoto	MF_DontMove
	rgoto	MF_Move

MF_DontMove
	;; --- Dont Move Fader -> Standby
	rcall	MF_StandBy

	;; --- Decrement Repeat CTR
	movlw	MF_REPEAT_CTR_0 & 0x7f
	decf	PLUSW2, F

	;; ---> END
	rgoto	MF_Tick_End

MF_Move
	;; ----------------------------------------------------------------------------------------
	;; motor speed in MF_TARGET_DIFF (saturated to 0xff)
	;; motor direction in MF_TARGET_MOTOR_DIR, 0 (0=down, 1=up)
	;; ----------------------------------------------------------------------------------------

	;; ----------------------------------------------------------------------------------------
	;; set motor speed
	;; slow down motor if distance between current and target position < 0x60
	;; ----------------------------------------------------------------------------------------
	;; --- check if fader should be moved up or down
	BRA_IFSET MF_TARGET_MOTOR_DIR, 0, BANKED, MF_MoveUp

MF_MoveDown
	;; max by default
	movlw	MF_MAX_DUTY_DOWN_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5

	;; slow down to min PWM if speed > MF_REDUCE_SPEED_AT_DIFF/1024
	movlw	MF_REDUCE_SPEED_AT_DIFF
	cpfsgt	AIN_POTSPEED, BANKED
	rgoto	MF_MoveDown_DontReduceSpeed
MF_MoveDown_ReduceSpeed
	movlw	MF_MIN_DUTY_DOWN_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5
	clrf	IRQ_TMP5
	rgoto	MF_MoveDown_NoPWM
MF_MoveDown_DontReduceSpeed

	;; slow down if distance to target position < 0x80
	movlw	0x80
	cpfslt	MF_TARGET_DIFF, BANKED
	rgoto	MF_MoveDown_NoPWM
MF_MoveDown_PWM
#if MF_SCALE_PWM_BETWEEN_MIN_MAX
	;; scale between MF_MIN_DUTY and MF_PWM_STEPS depending on distance
	movlw	MF_MAX_DUTY_DOWN_0 & 0x7f
	movff	PLUSW2, PRODL
	movlw	MF_MIN_DUTY_DOWN_0 & 0x7f
	movf	PLUSW2, W
	subwf	PRODL, W
	mulwf	MF_TARGET_DIFF, BANKED
	clrc
	rlf	PRODL, F
	rlf	PRODH, F

	movlw	MF_MIN_DUTY_DOWN_0 & 0x7f
	movf	PLUSW2, W
	addwf	PRODH, W
	movwf	IRQ_TMP5
#else
	;; for comparison w/o scaling (use calibration curve feature of MbhpMfTool to see the difference!)
	movlw	MF_MIN_DUTY_DOWN_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5
#endif
MF_MoveDown_NoPWM
	movlw	MF_PWM_DUTY_0 & 0x7f
	movff	IRQ_TMP5, PLUSW2

	;; ---> Move Fader Down
	movlw	MF_MODE_0 & 0x7f
	btfsc	PLUSW2, MF_MODE_FLAG_MOTOR_INVERTED
	rgoto	MF_MoveDown_Inverted
MF_MoveUp_Inverted
	movf	MF_NUMBER, W, BANKED
	call	MIOS_HLP_GetBitANDMask
	andwf	MF_MOTOR_DIR, F, BANKED
	;; ---> END
	rgoto	MF_Tick_End



MF_MoveUp
	;; max by default
	movlw	MF_MAX_DUTY_UP_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5

	;; slow down to min PWM if speed > MF_REDUCE_SPEED_AT_DIFF/1024
	movlw	MF_REDUCE_SPEED_AT_DIFF
	cpfsgt	AIN_POTSPEED, BANKED
	rgoto	MF_MoveUp_DontReduceSpeed
MF_MoveUp_ReduceSpeed
	movlw	MF_MIN_DUTY_UP_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5
	clrf	IRQ_TMP5
	rgoto	MF_MoveUp_NoPWM
MF_MoveUp_DontReduceSpeed

	;; slow down if distance to target position < 0x80
	movlw	0x80
	cpfslt	MF_TARGET_DIFF, BANKED
	rgoto	MF_MoveUp_NoPWM
MF_MoveUp_PWM
#if MF_SCALE_PWM_BETWEEN_MIN_MAX
	;; scale between MF_MIN_DUTY and MF_PWM_STEPS depending on distance
	movlw	MF_MAX_DUTY_UP_0 & 0x7f
	movff	PLUSW2, PRODL
	movlw	MF_MIN_DUTY_UP_0 & 0x7f
	movf	PLUSW2, W
	subwf	PRODL, W
	mulwf	MF_TARGET_DIFF, BANKED
	clrc
	rlf	PRODL, F
	rlf	PRODH, F

	movlw	MF_MIN_DUTY_UP_0 & 0x7f
	movf	PLUSW2, W
	addwf	PRODH, W
	movwf	IRQ_TMP5
#else
	;; for comparison w/o scaling (use calibration curve feature of MbhpMfTool to see the difference!)
	movlw	MF_MIN_DUTY_UP_0 & 0x7f
	movff	PLUSW2, IRQ_TMP5
#endif
MF_MoveUp_NoPWM
	movlw	MF_PWM_DUTY_0 & 0x7f
	movff	IRQ_TMP5, PLUSW2

	;; ---> Move Fader Up
	movlw	MF_MODE_0 & 0x7f
	btfsc	PLUSW2, MF_MODE_FLAG_MOTOR_INVERTED
	rgoto	MF_MoveUp_Inverted
MF_MoveDown_Inverted
	movf	MF_NUMBER, W, BANKED
	call	MIOS_HLP_GetBitORMask
	iorwf	MF_MOTOR_DIR, F, BANKED
	;; ---> END
	;; 	goto	MF_Tick_End

MF_Tick_End
	;; --- restore PROD[LH]
	movff	MF_STORED_PRODL, PRODL
	movff	MF_STORED_PRODH, PRODH

	return


;; --------------------------------------------------------------------------
MF_StandBy
	movlw	MF_PWM_DUTY_0 & 0x7f
	clrf	PLUSW2
	return



;; ------------------------------------------------------------------------------------------------
;;  Trace functions
;; ------------------------------------------------------------------------------------------------
MF_TRACE_Init
	lfsr	FSR1, TRACE_MEM_START
	movff	FSR1L, MF_TRACE_PTR_L
	movff	FSR1H, MF_TRACE_PTR_H
#if 0
	;; not required - trace will be sent after dump complete!
MF_TRACE_Init_Loop
	setf	POSTINC1	; invalidate

	movf	FSR1L, W
	xorlw	LOW(TRACE_MEM_END+1)
	bnz	MF_TRACE_Init_Loop
	movf	FSR1H, W
	xorlw	HIGH(TRACE_MEM_END+1)
	bnz	MF_TRACE_Init_Loop
#endif

	movlw	0x00		; don't touch BSR
	movff	WREG, MF_TRACE_SCALE_CTR

	return


	;; traces MF_TRACE_VALUE_[LH]
	;; we expect MF_BASE in BSR
MF_TRACE_Push
	movff	MF_TRACE_PTR_L, FSR1L
	movff	MF_TRACE_PTR_H, FSR1H

	movff	MF_TRACE_VALUE_L, POSTINC1
	movff	MF_TRACE_VALUE_H, POSTINC1

	;; end reached?
	movf	FSR1L, W
	xorlw	LOW(TRACE_MEM_END+1)
	bnz	MF_TRACE_Push_EndNotReached
	movf	FSR1H, W
	xorlw	HIGH(TRACE_MEM_END+1)
	bnz	MF_TRACE_Push_EndNotReached
MF_TRACE_Push_EndReached
	bsf	MF_TRACE_STATE, MF_TRACE_STATE_FLAG_FINISHED, BANKED
MF_TRACE_Push_EndNotReached
	movff	FSR1L, MF_TRACE_PTR_L
	movff	FSR1H, MF_TRACE_PTR_H
	return


;; ------------------------------------------------------------------------------------------------
;;  Should be called each mS
;; ------------------------------------------------------------------------------------------------
MF_Periodic_mS_Tick
	SET_BSR	MF_BASE

	clrf	MF_NUMBER, BANKED
MF_Periodic_mS_Tick_Loop

	;; ------------------------------------------------------------------
	;; --- decrement MANUAL CTR if != 0
	;; ------------------------------------------------------------------
	lfsr	FSR2, MF_MANUAL_CTR_0
	movf	MF_NUMBER, W, BANKED
	movf	PLUSW2, F
	skpz
	decf	PLUSW2, F

	;; ------------------------------------------------------------------
	;; --- decrement TIMEOUT if != 0
	;; ------------------------------------------------------------------
	lfsr	FSR2, MF_TIMEOUT_CTR_0
	movf	MF_NUMBER, W, BANKED
	movf	PLUSW2, F
	skpz
	decf	PLUSW2, F

	incf	MF_NUMBER, F, BANKED
	BRA_IFCLR MF_NUMBER, 3, BANKED, MF_Periodic_mS_Tick_Loop

	;; log move of fader
	BRA_IFSET MF_TRACE_STATE, MF_TRACE_STATE_FLAG_FINISHED, BANKED, MF_Periodic_mS_Tick_NoTrace
	BRA_IFCLR MF_TRACE_STATE, MF_TRACE_STATE_FLAG_STARTED, BANKED, MF_Periodic_mS_Tick_NoTrace
	incf	MF_TRACE_SCALE_CTR, F, BANKED
	movf	MF_TRACE_SCALE, W, BANKED
	cpfslt	MF_TRACE_SCALE_CTR, BANKED
	clrf	MF_TRACE_SCALE_CTR, BANKED

	movf	MF_TRACE_SCALE_CTR, W, BANKED
	bnz	MF_Periodic_mS_Tick_NoTrace
MF_Periodic_mS_Tick_Trace
	rcall	MF_TRACE_Push
MF_Periodic_mS_Tick_NoTrace

	return


;; ------------------------------------------------------------------------------------------------
;;  PWM Handler
;; ------------------------------------------------------------------------------------------------
MF_PWM_Tick
	SET_BSR	MF_BASE

	;; ------------------------------------------------------------------
	;; increment global PWM period counter
	;; reset at PWM_STEPS, this gives us a duty cycle of 0..PWM_STEPS
	;; ------------------------------------------------------------------
	incf	MF_PWM_STEPS_COUNTER, F, BANKED
	movf	MF_PWM_STEPS, W, BANKED
	cpfslt	MF_PWM_STEPS_COUNTER, BANKED
	clrf	MF_PWM_STEPS_COUNTER, BANKED


	;; ------------------------------------------------------------------
	;; following macro simplifies the duty cycle handling
	;; 0x00: signal is always 0
	;; 0x01..0x0f: duty cycle (
	;; 0x10: signal is always 1
	;; ------------------------------------------------------------------
PWM_HANDLER MACRO port, pin, dutyReg
	LOCAL	PWM_HANDLER_0
	LOCAL	PWM_HANDLER_1
	LOCAL	PWM_HANDLER_End

	movf	MF_PWM_STEPS_COUNTER, W, BANKED
	cpfsgt	dutyReg, BANKED
	rgoto	PWM_HANDLER_0

PWM_HANDLER_1
	bsf	port, pin
	rgoto	PWM_HANDLER_End
PWM_HANDLER_0
	bcf	port, pin
	;; rgoto PWM_HANDLER_End
PWM_HANDLER_End
	ENDM
	;; ------------------------------------------------------------------

	;; ------------------------------------------------------------------
	;; following macro handles the two motor pins
	;; ------------------------------------------------------------------
PWM_HANDLER_MOTOR MACRO portUp, pinUp, portDown, pinDown, dirReg, dirBit, dutyReg
	LOCAL	PWM_HANDLER_MOTOR_Up
	LOCAL	PWM_HANDLER_MOTOR_Down
	LOCAL	PWM_HANDLER_MOTOR_End

	BRA_IFCLR dirReg, dirBit, BANKED, PWM_HANDLER_MOTOR_Down
PWM_HANDLER_MOTOR_Up
	bcf	portDown, pinDown
	PWM_HANDLER portUp, pinUp, dutyReg
	rgoto	PWM_HANDLER_MOTOR_End

PWM_HANDLER_MOTOR_Down
	bcf	portUp, pinUp
	PWM_HANDLER portDown, pinDown, dutyReg
	;; rgoto PWM_HANDLER_MOTOR_End

PWM_HANDLER_MOTOR_End
	ENDM
	;; ------------------------------------------------------------------

	;; handle PWMs
 	PWM_HANDLER STATUS_LED_PORT, STATUS_LED_PIN, MF_PWM_DUTY_STATUS_LED

	PWM_HANDLER_MOTOR M1_UP_PORT, M1_UP_PIN, M1_DOWN_PORT, M1_DOWN_PIN, MF_MOTOR_DIR, 0, MF_PWM_DUTY_0+0
	PWM_HANDLER_MOTOR M2_UP_PORT, M2_UP_PIN, M2_DOWN_PORT, M2_DOWN_PIN, MF_MOTOR_DIR, 1, MF_PWM_DUTY_0+1
	PWM_HANDLER_MOTOR M3_UP_PORT, M3_UP_PIN, M3_DOWN_PORT, M3_DOWN_PIN, MF_MOTOR_DIR, 2, MF_PWM_DUTY_0+2
	PWM_HANDLER_MOTOR M4_UP_PORT, M4_UP_PIN, M4_DOWN_PORT, M4_DOWN_PIN, MF_MOTOR_DIR, 3, MF_PWM_DUTY_0+3
	PWM_HANDLER_MOTOR M5_UP_PORT, M5_UP_PIN, M5_DOWN_PORT, M5_DOWN_PIN, MF_MOTOR_DIR, 4, MF_PWM_DUTY_0+4
	PWM_HANDLER_MOTOR M6_UP_PORT, M6_UP_PIN, M6_DOWN_PORT, M6_DOWN_PIN, MF_MOTOR_DIR, 5, MF_PWM_DUTY_0+5
	PWM_HANDLER_MOTOR M7_UP_PORT, M7_UP_PIN, M7_DOWN_PORT, M7_DOWN_PIN, MF_MOTOR_DIR, 6, MF_PWM_DUTY_0+6
	PWM_HANDLER_MOTOR M8_UP_PORT, M8_UP_PIN, M8_DOWN_PORT, M8_DOWN_PIN, MF_MOTOR_DIR, 7, MF_PWM_DUTY_0+7

	return
