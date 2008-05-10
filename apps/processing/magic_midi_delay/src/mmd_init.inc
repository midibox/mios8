; $Id$
;
; MIOS Application init routine
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the AIN driver

	;; use 14 pots
#if USE_POTS
	movlw	14
#else
	movlw	0
#endif
	call	MIOS_AIN_NumberSet

	;; use the AIN 4051 multiplexers
	call	MIOS_AIN_Muxed

	;; define the deadband (with 7-bit values, a deadband value of 8 is adequate;
	;; calculated with the following formula: (2^(10 bit - 7 bit)) - 1 = (2 ^ 3) - 1 = 7

	;; set value to 0 or 1 if you want to see the 10-bit value at the LCD
	movlw	3
	call	MIOS_AIN_DeadbandSet

	;; initalize ENC driver: set all 8 encoders and the jog-wheel to "slow speed"
	clrf	TMP1
	movlw	MIOS_ENC_SPEED_SLOW
	movwf	MIOS_PARAMETER1
MMD_Init_EncLoop
	movf	TMP1, W
	call	MIOS_ENC_SpeedSet
	incf	TMP1, F
	movlw	0x09
	cpfsgt	TMP1, ACCESS
	rgoto MMD_Init_EncLoop

	;; define number of shift registers: for 128 buttons and LEDs, we need
	;; 16 registers. Btw.: thats the maximum number of supported DIN/DOUTs
	movlw	16
	call	MIOS_SRIO_NumberSet

	;; define the update frequency (latency) of DIN/DOUTs in mS
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; Enable Autorepeat for the first 32 buttons
	clrf	TMP1
MMD_Init_AutorepeatLoop
	movf	TMP1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	incf	TMP1, F
	movlw	0x1f
	cpfsgt	TMP1, ACCESS
	rgoto MMD_Init_AutorepeatLoop

	;; copy default values into registers
	movlw	BPM_VALUE
	call	MMD_Timer_BPMSet

	movlw	MIDI_IN_CFG_CHN_VALUE
	movwf	MMD_MIDI_IN_CFG_CHN
	movlw	MIDI_IN_CHN_VALUE
	movwf	MMD_MIDI_IN_CHN
	
	clrf	MMD_QUEUE0_HEAD
	clrf	MMD_QUEUE0_TAIL
	movlw	QUEUE0_CHN_VALUE
	movwf	MMD_QUEUE0_CHN
	movlw	QUEUE0_DLY_VALUE
	movwf	MMD_QUEUE0_DLY
	clrf	MMD_QUEUE0_OVERALL_DLY
	movlw	QUEUE0_TRANSP_VALUE
	movwf	MMD_QUEUE0_TRANSP

	clrf	MMD_QUEUE1_HEAD
	clrf	MMD_QUEUE1_TAIL
	movlw	QUEUE1_CHN_VALUE
	movwf	MMD_QUEUE1_CHN
	movlw	QUEUE1_DLY_VALUE
	movwf	MMD_QUEUE1_DLY
	clrf	MMD_QUEUE1_OVERALL_DLY
	movlw	QUEUE1_TRANSP_VALUE
	movwf	MMD_QUEUE1_TRANSP

	clrf	MMD_QUEUE2_HEAD
	clrf	MMD_QUEUE2_TAIL
	movlw	QUEUE2_CHN_VALUE
	movwf	MMD_QUEUE2_CHN
	movlw	QUEUE2_DLY_VALUE
	movwf	MMD_QUEUE2_DLY
	clrf	MMD_QUEUE2_OVERALL_DLY
	movlw	QUEUE2_TRANSP_VALUE
	movwf	MMD_QUEUE2_TRANSP

	clrf	MMD_QUEUE3_HEAD
	clrf	MMD_QUEUE3_TAIL
	movlw	QUEUE3_CHN_VALUE
	movwf	MMD_QUEUE3_CHN
	movlw	QUEUE3_DLY_VALUE
	movwf	MMD_QUEUE3_DLY
	clrf	MMD_QUEUE3_OVERALL_DLY
	movlw	QUEUE3_TRANSP_VALUE
	movwf	MMD_QUEUE3_TRANSP

	return
