; $Id$
;
; MF Calibration Test
;
; ==========================================================================
;
;  Copyright 2004 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ---[ MIOS header file ]---
#include <mios.h>

;; ---[ useful macros ]---
#include <macros.h>

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---

;; Events assigned to the DINs/DOUTs and Pots are defined here
#include <mios_mt_table.inc>
#include <mios_enc_table.inc>

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the AIN driver

	;; use 8 faders
	movlw	8
	call	MIOS_AIN_NumberSet

	;; Multiplexers are not supported by MF driver
	call	MIOS_AIN_UnMuxed

	;; define the deadband (with 7-bit values, a deadband value of 8 is adequate;
	;; calculated with the following formula: (2^(10 bit - 7 bit)) - 1 = (2 ^ 3) - 1 = 7

	;; set value to 0 or 1 if you want to see the 10-bit value at the LCD
	movlw	3		; value can be changed with button 7/8
	call	MIOS_AIN_DeadbandSet


	;; initialize the MF driver
	call	MIOS_MF_Enable
	movlw	0x03		; value can be changed with button 9/10
	call	MIOS_MF_DeadbandSet

	movlw	0x03		; value can be changed with button 4/5
	call	MIOS_MF_PWM_PeriodSet
	movlw	0x01		; value can be changed with button 2/3
	call	MIOS_MF_PWM_DutyCycleUpSet
	movlw	0x01		; value can be changed with button 1/2
	call	MIOS_MF_PWM_DutyCycleDownSet

	;; define number of shift registers (it doesn't hurt to define just the max value)
	movlw	16
	call	MIOS_SRIO_NumberSet

	;; define the update frequency (latency) of DIN/DOUTs in mS
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; Enable Autorepeat for the first 10 buttons
	clrf	TMP1
Init_Autorepeat_Loop
	movf	TMP1, W
	call	MIOS_DIN_PinAutoRepeatEnable
	incf	TMP1, F
	movlw	0x09
	cpfsgt	TMP1, ACCESS
	rgoto Init_Autorepeat_Loop

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a debug command has been received
;;  via SysEx
;;  Input:
;;     o WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3 like
;;       specified in the debug command
;;  Output:
;;     o return values WREG, MIOS_PARAMETER1, MIOS_PARAMETER2, MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_DebugTrigger
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	;; clear screen
	call	MIOS_LCD_Clear

	;; request status update message
	bsf	DISPLAY_UPDATE_REQ, 0

	return

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; check if update has been requested, else leave routine
	;; (variable DISPLAY_UPDATE_REQ is defined in app_defines.h)
	btfss	DISPLAY_UPDATE_REQ, 0
	return
	bcf	DISPLAY_UPDATE_REQ, 0	; clear request

	;; print status message
	TABLE_ADDR TEXT_STATUS_0		; init pointer to string
	call	MIOS_LCD_PrintString		; print line 1

	;; set cursor to second line, first column
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print "calibration down" value
	call	MIOS_MF_PWM_DutyCycleDownGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print "calibration up" value
	call	MIOS_MF_PWM_DutyCycleUpGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print "period" value
	call	MIOS_MF_PWM_PeriodGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print AIN deadband value
	call	MIOS_AIN_DeadbandGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print MF deadband value
	call	MIOS_MF_DeadbandGet
	call	MIOS_LCD_PrintHex2

	;; print space
	movlw	' '
	call	MIOS_LCD_PrintChar

	;; print position of motorfader #1
	movlw	0x00
	call	MIOS_AIN_PinGet
	movf	MIOS_PARAMETER2, W	; MSB in MIOS_PARAMETER2, we only need the first digit
	call	MIOS_LCD_PrintHex1
	movf	MIOS_PARAMETER1, W	; LSB in MIOS_PARAMETER1
	call	MIOS_LCD_PrintHex2

	return

TEXT_STATUS_0	STRING 19, 0x00, "DD DU Pd AI MF Val."


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the CONFIG_MIDI_IN table
;;  Input:
;;     o number of entry in WREG
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyFoundEvent
	;; here we could insert code which switches the LEDs on/off via
	;; MIDI
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the transfer of a MIDI byte. 
;;  It can be used to monitor the Tx activity or to do any other actions
;;  (e.g. to switch a pin for multiplexed MIDI Outs) before the byte will 
;;  be sent.
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o transmitted byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyTx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received.
;;  It can be used to monitor the Rx activity or to do any action - e.g.
;;  to react on realtime events like MIDI clock (0xf8) with a minimum latency
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;;  Input:
;;     o received byte in WREG
;; --------------------------------------------------------------------------
USER_MIDI_NotifyRx
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	;; a button has been pressed or depressed. 
	;; Send value which has been defined in MIOS_MPROC_EVENT_TABLE

	;; save button number in TMP1
	movff	MIOS_PARAMETER1, TMP1
	;; save button value in TMP2
	movff	MIOS_PARAMETER2, TMP2

	;; if button index >= 10, leave routine
	movlw	0x0a
	cpfslt	TMP1, ACCESS
	return


	;; toggle the appr. DOUT LED
	movf	TMP1, W				; button number has been saved in TMP1. This is also the LED number
	CALL_IFCLR TMP2, 0, ACCESS, MIOS_DOUT_PinSet1	; if button has been pressed (value = 0V = CLR): LED on
	CALL_IFSET TMP2, 0, ACCESS, MIOS_DOUT_PinSet0	; if button has been depressed (value = 5V = SET): LED off

	;; don't continue of button has been depressed
	btfsc	TMP2, 0
	return

	;; button pressed: branch depending on button number
	movf	TMP1, W
	JUMPTABLE_2BYTES 10	; 10 entries
	rgoto	DIN_Button_Dec_DD
	rgoto	DIN_Button_Inc_DD
	rgoto	DIN_Button_Dec_DU
	rgoto	DIN_Button_Inc_DU
	rgoto	DIN_Button_Dec_Pd
	rgoto	DIN_Button_Inc_Pd
	rgoto	DIN_Button_Dec_AI
	rgoto	DIN_Button_Inc_AI
	rgoto	DIN_Button_Dec_MF
	rgoto	DIN_Button_Inc_MF

	;; thats all
	return


;; DIN Button Handlers:
DIN_Button_Dec_DD
	;; decrement "calibration down value" if > 0
	call	MIOS_MF_PWM_DutyCycleDownGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_DutyCycleDownSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Inc_DD
	;; increment "calibration down value" if <= 0x1e
	call	MIOS_MF_PWM_DutyCycleDownGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_DutyCycleDownSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Dec_DU
	;; decrement "calibration up value" if > 0
	call	MIOS_MF_PWM_DutyCycleUpGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_DutyCycleUpSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Inc_DU
	;; increment "calibration up value" if <= 0x1e
	call	MIOS_MF_PWM_DutyCycleUpGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_DutyCycleUpSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Dec_Pd
	;; decrement "period value" if > 0
	call	MIOS_MF_PWM_PeriodGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_PWM_PeriodSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Inc_Pd
	;; increment "period value" if <= 0x1e
	call	MIOS_MF_PWM_PeriodGet	; get value
	movlw	0x1f			; check if value >= 0x1f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_PWM_PeriodSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Dec_AI
	;; decrement "AIN deadband" if > 0
	call	MIOS_AIN_DeadbandGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_AIN_DeadbandSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Inc_AI
	;; increment "AIN deadband value" if <= 0x3e (more makes no sense)
	call	MIOS_AIN_DeadbandGet	; get value
	movlw	0x3f			; check if value >= 0x3f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_AIN_DeadbandSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Dec_MF
	;; decrement "MF deadband" if > 0
	call	MIOS_MF_DeadbandGet	; get value
	skpnz				; skip next instruction if zero
	return				; return if zero
	addlw	-1			; add -1 (-> subtract 1)
	call	MIOS_MF_DeadbandSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return
	
DIN_Button_Inc_MF
	;; increment "MF deadband value" if <= 0x3e (more makes no sense)
	call	MIOS_MF_DeadbandGet	; get value
	movlw	0x3f			; check if value >= 0x3f
	cpfslt	MIOS_PARAMETER1, ACCESS; return if true
	return	
	incf	MIOS_PARAMETER1, W	; increment value and store result in WREG
	call	MIOS_MF_DeadbandSet	; set new value
	bsf	DISPLAY_UPDATE_REQ, 0	; request status screen update
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	;; only continue if fader #1 has been moved
	movlw	0x00		; (index of fader #1)
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	;; save fader LSB value in TMP1
	movff	MIOS_PARAMETER2, TMP1
	;; save fader MSB value in TMP2
	movff	MIOS_PARAMETER3, TMP2

	;; move the other 7 faders
	;; use TMP3 as loop counter
	movlw	0x01		; start with fader #1
	movwf	TMP3
AIN_MoveFaderLoop
	movff	TMP1, MIOS_PARAMETER1	; LSB in MIOS_PARAMETER1
	movff	TMP2, MIOS_PARAMETER2	; MSB in MIOS_PARAMETER1
	movf	TMP3, W			; fader index in WREG
	call	MIOS_MF_FaderMove	; move fader

	incf	TMP3, F			; increment fader index
	movlw	0x08			; next loop until index is 8
	cpfseq	TMP3, ACCESS
	rgoto AIN_MoveFaderLoop

	bsf	DISPLAY_UPDATE_REQ, 0	; request update of status screen

	return

	END
