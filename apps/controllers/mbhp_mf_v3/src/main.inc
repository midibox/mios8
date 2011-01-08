;$Id$
;
;  Source Code of MBHP_MF_V3 Firmware
;
; ==========================================================================
;
;  Copyright 1998-2010 Thorsten Klose (tk@midibox.org)
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
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>


;; ---[ Custom AIN Driver ]---
#include <ain.inc>

;; ---[ Custom MF Driver ]---
#include <mf.inc>

;; ---[ SysEx/Bank Handling ]---
#include <mf_bank.inc>
#include <mf_dump.inc>
#include <mf_sysex.inc>

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; ------------------------------------------------------------------
	;; no LCD is connected to MBHP_MF_V3 module
	;; -> enable "dummy" LCD driver to avoid J15 port accesses
	;; ------------------------------------------------------------------
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2
	movlw	0x07		; select dummy driver in modules/app_lcd/dummy
	call	MIOS_LCD_TypeSet

	;; ------------------------------------------------------------------
	;; configure IOs (a bit different compared to common MIOS applications)
	;; ------------------------------------------------------------------
	bsf	INTCON2, NOT_RBPU	; Disable Pull-Ups of PortB

	clrf	PORTA			; Reset PortA
	clrf	PORTB			; Reset PortB
	movlw	0x40			; Reset PortC (TX Pin = 1)
	movwf	PORTC
	clrf	PORTD			; Reset PortD
	clrf	PORTE			; Reset PortE

	movlw	b'11101111'		; all PORTA pins beside if RA4 are tristate
	movwf	TRISA
	clrf	TRISB			; PORTB driver enabled by default (display data port)
	movlw	b'10000000'		; Rx Pin tristate, all other outputs
	movwf	TRISC
	movlw	b'00000010'		; DIN pin tristate, all other outputs
	movwf	TRISD
	movlw	b'00000111'		; all PORTE pins are tristate
	movwf	TRISE

	;; ------------------------------------------------------------------
	;; configure SRIO (only a single DIN SR is connected for touchsensors)
	;; ------------------------------------------------------------------
	movlw	0x01			; use 1 shift register
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy to 1/1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	10			; 10 mS debouncing
	call	MIOS_SRIO_DebounceSet

	;; ------------------------------------------------------------------
	;; Restore Dump from EEPROM
	;; this will also configure and enable AIN/MF driver!
	;; ------------------------------------------------------------------
	call	MF_DUMP_RestoreAll

	;; ------------------------------------------------------------------
	;; initialize the user timer (used for generating PWM)
	;; timer should be called every 50 uS == 500 ticks
	;; ------------------------------------------------------------------
	movlw	500 & 0xff		; low-byte
	movwf	MIOS_PARAMETER1
	movlw	500 >> 8		; high-byte
	movwf	MIOS_PARAMETER2
	movlw	0x00			; no prescaler (1:1)
	call	MIOS_TIMER_Init

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; check if trace should be sent
	SET_BSR	MF_BASE
	BRA_IFCLR MF_TRACE_STATE, MF_TRACE_STATE_FLAG_FINISHED, BANKED, USER_Tick_NoTraceDump
	BRA_IFSET MF_TRACE_STATE, MF_TRACE_STATE_FLAG_SENT, BANKED, USER_Tick_NoTraceDump
USER_Tick_TraceDump
	bsf	MF_TRACE_STATE, MF_TRACE_STATE_FLAG_SENT, BANKED
	call	MF_SYSEX_SendTraceDump
USER_Tick_NoTraceDump

	;; AIN handler will call USER_AIN_NotifyChange on pot changes
	call	AIN_Handler
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; this timer is called each 50 uS

	;; call period MF tick each mS
	incf	PERIODIC_MS_CTR, F
	movf	PERIODIC_MS_CTR, W
	xorlw	20		; 1 mS = 20*50uS
	bnz	USER_Timer_Periodic_mS_Skip
USER_Timer_Periodic_mS
	clrf	PERIODIC_MS_CTR
	call	MF_Periodic_mS_Tick
USER_Timer_Periodic_mS_Skip

	;; call AIN interrupt handler each 100 uS
	;; it starts conversions, stores the result of previous conversions, and calls the MF_Tick handler
	btfsc	PERIODIC_MS_CTR, 0
	call	AIN_Tick

	;; call PWM handler
	;; it generates PWM on all motor control lines
	call	MF_PWM_Tick

	;; ------------------------------------------------------------------
	;; Status LED sweep effect
	;; ------------------------------------------------------------------
USER_Timer_LedSweep
	SET_BSR	MF_BASE
	incf	MF_PWM_STATUS_LED_SWEEP_CTR, F, BANKED
	bnz	USER_Timer_LedSweepEnd
USER_Timer_LedSweepNext
	movf	MF_PWM_STATUS_LED_SWEEP_DIR, W, BANKED
	bnz	USER_Timer_LedSweepNext_Down
USER_Timer_LedSweepNext_Up
	incf	MF_PWM_DUTY_STATUS_LED, F, BANKED
	movlw	0x40		; sweep between 0x00..0x40
	cpfsgt	MF_PWM_DUTY_STATUS_LED, BANKED
	rgoto	USER_Timer_LedSweepEnd
	setf	MF_PWM_STATUS_LED_SWEEP_DIR, BANKED
	rgoto	USER_Timer_LedSweepEnd

USER_Timer_LedSweepNext_Down
	decf	MF_PWM_DUTY_STATUS_LED, F, BANKED
	skpnz
	clrf	MF_PWM_STATUS_LED_SWEEP_DIR, BANKED
	;;  rgoto USER_Timer_LedSweepEnd

USER_Timer_LedSweepEnd
	
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
	;; NO DISPLAY CONNECTED!!!
	return

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; NO DISPLAY CONNECTED!!!
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; branch to USER_MPROC_Received_Ex if pitch bender event (for motorfaders) has been received
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0xe0
	bz	USER_MPROC_Received_Ex

	;; nothing else to parse
	return


USER_MPROC_Received_Ex
	;; mask out MIDI channel (== fader number)
	movlw	0x0f
	andwf	MIOS_PARAMETER1, F

	;; do nothing if fader number >= 8
	movlw	0x08
	cpfslt	MIOS_PARAMETER1, ACCESS
	return

	;; save fader number in LC_FADER_NUMBER
	movf	MIOS_PARAMETER1, W
	movwf	LC_FADER_NUMBER

	;; reset touch detection if touch sensor mode >= 1
	SET_BSR	MF_BASE
	movf	MF_TOUCHSENSOR_MODE, W, BANKED
	bnz	USER_MPROC_Received_Ex_NoTs
USER_MPROC_Received_Ex_Ts
	movf	LC_FADER_NUMBER, W
	call	MF_TouchDetectionReset
USER_MPROC_Received_Ex_NoTs

	;; derive 10-bit value from received 14-bit value. Store result in LC_FADER_POS_[LH]
	movff	MIOS_PARAMETER2, LC_FADER_POS_L ; MIDI LSB -> LC_FADER_POS_L
	movff	MIOS_PARAMETER3, LC_FADER_POS_H ; MIDI MSB -> LC_FADER_POS_H

	;; LSB = MSB[0] | MIDI LSB
	btfsc	LC_FADER_POS_H, 0
	bsf	LC_FADER_POS_L, 7
	;; MSB = MIDI MSB >> 1
	clrc
	rrf	LC_FADER_POS_H, F

	;; 10-bit value = 14-bit value >> 4
	clrc
	rrf	LC_FADER_POS_H, F	; 1
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 2
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 3
	rrf	LC_FADER_POS_L, F
	clrc
	rrf	LC_FADER_POS_H, F	; 4
	rrf	LC_FADER_POS_L, F

	;; finally move fader
	movff	LC_FADER_POS_L, MIOS_PARAMETER1
	movff	LC_FADER_POS_H, MIOS_PARAMETER2
	movf	LC_FADER_NUMBER, W
	goto	MF_FaderMove


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
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	;; -> jump to "ActionInvalid" for a proper reset of the sysex parser
	goto	MF_SYSEX_ActionInvalid


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> branch to SysEx parser
	goto	MF_SYSEX_Parser

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
	;; exit if touch sensors have been disabled
	SET_BSR	MF_BASE
	movf	MF_TOUCHSENSOR_MODE, W, BANKED
	skpnz
	return

	;; only button 0..7 should toggle - exit function if button >= 8
	andlw	0xf8
	skpz
	return

	;; TOUCH_SENSOR_MODE >= 2
	SET_BSR	MF_BASE
	movlw	2-1
	cpfsgt	MF_TOUCHSENSOR_MODE, BANKED
	rgoto	USER_DIN_NotifyToggle_NoSuspend

	;; in this mode, motorfaders will be suspended if touch sensors active
USER_DIN_NotifyToggle_Suspend
	movf	MIOS_PARAMETER2, W
	bz	USER_DIN_NotifyToggle_Suspend1
USER_DIN_NotifyToggle_Suspend0
	movf	MIOS_PARAMETER1, W
	call	MF_SuspendDisable
	rgoto	USER_DIN_NotifyToggle_Cont
USER_DIN_NotifyToggle_Suspend1
	movf	MIOS_PARAMETER1, W
	call	MF_SuspendEnable
	;; 	rgoto	USER_DIN_NotifyToggle_Cont
USER_DIN_NotifyToggle_Cont
USER_DIN_NotifyToggle_NoSuspend

	;; generate MIDI event

	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; Note On/Off 0x68..0x6f complies to Mackie Control Spec
	movlw	0x90			; Note Event Channel #1
	call	MIOS_MIDI_TxBufferPut
	
	movlw	0x68			; 0x68..0x6f depending on button number
	addwf	MIOS_PARAMETER1, W
	call	MIOS_MIDI_TxBufferPut

	movlw	0x00			; velocity 0 if button released, otherwise 0x7f
	movf	MIOS_PARAMETER2, F
	skpnz
	movlw	0x7f
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream

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

	;; save the fader number in LC_FADER_NUMBER
	movwf	LC_FADER_NUMBER

	;; TOUCH_SENSOR_MODE >= 3
	SET_BSR	MF_BASE
	movlw	3-1
	cpfsgt	MF_TOUCHSENSOR_MODE, BANKED
	rgoto	USER_AIN_NotifyChange_SendAlways

	;; in this mode, no value will be sent if touch sensor not active (fader not suspended)
USER_AIN_NotifyChange_SendIfAct
	movf	LC_FADER_NUMBER, W
	call	MF_SuspendGet
	bz	LC_MF_FaderEvent_End

USER_AIN_NotifyChange_SendAlways
	;; store 10-bit LSB value in LC_FADER_POS_L
	movff	MIOS_PARAMETER2, LC_FADER_POS_L

	;; store 10-bit MSB value in LC_FADER_POS_H
	movff	MIOS_PARAMETER3, LC_FADER_POS_H

	;; shift the values to the right position like descriped in
	;; the Logic Control MIDI implementation, chapter 12

	;; 10 bit -> 14 bit (LC_FADER_POS_[LH] << 4)
	clrc
	rlf	LC_FADER_POS_L, F	; 1
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 2
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 3
	rlf	LC_FADER_POS_H, F
	rlf	LC_FADER_POS_L, F	; 4
	rlf	LC_FADER_POS_H, F

	;; MIDI MSB: (LC_FADER_POS_H << 1) | LC_FADER_POS_L[7]
	clrc
	rlf	LC_FADER_POS_H, W
	btfsc	LC_FADER_POS_L, 7
	iorlw 0x01
	movwf	LC_FADER_POS_H

	;; MIDI LSB: LC_FADER_POS_L & 0x7f
	movlw	0x7f
	andwf	LC_FADER_POS_L, F

	;; for MIDIbox Link: notify begin of stream
	call	MIOS_MIDI_BeginStream

	;; finally send value: E<fader> LSB MSB
	movf	LC_FADER_NUMBER, W
	andlw	0x0f
	iorlw	0xe0
	call	MIOS_MIDI_TxBufferPut

	movf	LC_FADER_POS_L, W
	call	MIOS_MIDI_TxBufferPut

	movf	LC_FADER_POS_H, W
	call	MIOS_MIDI_TxBufferPut

	;; for MIDIbox Link: notify end of stream
	call	MIOS_MIDI_EndStream
	
LC_MF_FaderEvent_End
	;; thats all
	return


;; ---[ EEPROM content ]---
#include "mf_presets.inc"

	END
