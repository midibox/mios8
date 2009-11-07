;; dout_calls.inc


;;; LED control

LED_ToneA_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_ToneA
	call MIOS_DOUT_PinSet1
		return


LED_ToneA_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_ToneA
	call MIOS_DOUT_PinSet0
		return


LED_ToneB_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_ToneB
	call MIOS_DOUT_PinSet1
		return


LED_ToneB_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_ToneB
	call MIOS_DOUT_PinSet0
		return

LED_ToneC_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_ToneC
	call MIOS_DOUT_PinSet1
		return


LED_ToneC_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_ToneC
	call MIOS_DOUT_PinSet0
		return


;; ---------


LED_NoiseA_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_NoiseA
	call MIOS_DOUT_PinSet1
		return


LED_NoiseA_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_NoiseA
	call MIOS_DOUT_PinSet0
		return


LED_NoiseB_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_NoiseB
	call MIOS_DOUT_PinSet1
		return


LED_NoiseB_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_NoiseB
	call MIOS_DOUT_PinSet0
		return

LED_NoiseC_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_NoiseC
	call MIOS_DOUT_PinSet1
		return


LED_NoiseC_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_NoiseC
	call MIOS_DOUT_PinSet0
		return

;; ------


LED_EnvelopeModeA_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeA
	call MIOS_DOUT_PinSet1
		return


LED_EnvelopeModeA_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeA
	call MIOS_DOUT_PinSet0
		return


LED_EnvelopeModeB_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeB
	call MIOS_DOUT_PinSet1
		return


LED_EnvelopeModeB_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeB
	call MIOS_DOUT_PinSet0
		return

LED_EnvelopeModeC_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeC
	call MIOS_DOUT_PinSet1
		return


LED_EnvelopeModeC_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_EnvelopeModeC
	call MIOS_DOUT_PinSet0
		return

;; -------------

LED_Fx_A_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_Fx_A
	call MIOS_DOUT_PinSet1
		return


LED_Fx_A_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Fx_A
	call MIOS_DOUT_PinSet0
		return


LED_Fx_B_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_Fx_B
	call MIOS_DOUT_PinSet1
		return


LED_Fx_B_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Fx_B
	call MIOS_DOUT_PinSet0
		return

LED_Fx_C_on
	;; MIOS_DOUT_PinSet1
	;; DESCRIPTION 	set pin to 5V
	;; IN 	Pin number in WREG
	movlw LED_Fx_C
	call MIOS_DOUT_PinSet1
		return


LED_Fx_C_off
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Fx_C
	call MIOS_DOUT_PinSet0
		return


;; ----------------

LED_Envelope_Shape_1_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_1
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_2_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_2
	call MIOS_DOUT_PinSet1
		return


LED_Envelope_Shape_3_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_3
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_4_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_4
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_5_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_5
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_6_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_6
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_7_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_7
	call MIOS_DOUT_PinSet1
		return

LED_Envelope_Shape_8_on
	;; MIOS_DOUT_PinSet0
	;; DESCRIPTION 	set pin to 0V
	;; IN 	Pin number in WREG
	movlw LED_Envelope_Shape_8
	call MIOS_DOUT_PinSet1
		return

		
LED_Envelope_Shape_all_off
	;; DESCRIPTION 	set all *ENV LED's* pins to 0V
	;; Pin number in WREG
	movlw LED_Envelope_Shape_1
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_2
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_3
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_4
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_5
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_6
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_7
	call MIOS_DOUT_PinSet0
	movlw LED_Envelope_Shape_8
	call MIOS_DOUT_PinSet0
		return