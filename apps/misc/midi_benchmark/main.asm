; $Id$
;
; MIDI Performance Measuring Application
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
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
#include <app_defines.h>

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---
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
	;; set initial min/max delay
	setf	DELAY_MIN_L
	setf	DELAY_MIN_H
	clrf	DELAY_MAX_L
	clrf	DELAY_MAX_H
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; do the performance measuring on every 16384th execution of this routine
	incf	WAIT_TIMER0, F
	skpnz
	incf	WAIT_TIMER1, F
	btfss	WAIT_TIMER1, 4
	return

	clrf	WAIT_TIMER0
	clrf	WAIT_TIMER1
	goto	MIDI_Measure_Start


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
	call	MIOS_LCD_Clear

	TABLE_ADDR TEXT_WELCOME_0
	call	MIOS_LCD_PrintString

	return

TEXT_WELCOME_0	STRING 16, 0x00, "Curr. MIN   MAX "

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; print new result if available
	goto	MIDI_Measure_Display


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; first check if received event matches with sent event
	movlw	0xb0
	cpfseq	MIOS_PARAMETER1, ACCESS
	return
	movlw	0x01
	cpfseq	MIOS_PARAMETER2, ACCESS
	return
	movf	RUNNING_CTR, W
	cpfseq	MIOS_PARAMETER3, ACCESS
	return

	;; event received: stop measuring
	goto	MIDI_Measure_Stop


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
	return



;; --------------------------------------------------------------------------
;;  This function measures the performance of a MIDI connection
;;  Call MIDI_Measure_Start to begin the measuring
;; --------------------------------------------------------------------------
MIDI_Measure_Start
	;; if timer still running, then we had an overrun
	BRA_IFCLR T3CON, TMR3ON, ACCESS, MIDI_Measure_Start_NoOverrun
MIDI_Measure_Start_Overrun
	;; stop timer and notify overrun on display
	rcall	MIDI_Measure_Stop
	bsf	BENCHMARK_UPDATE_REQ, 1
	rcall	MIDI_Measure_Display
MIDI_Measure_Start_NoOverrun

	;; using timer3 to measure the number of cycles
	;; this timer is also used by MIOS for the USER_Timer hook, so
	;; we first should disable the interrupt here
	bcf	PIE2, TMR3IE

	;; (Note: timings could also be measured with timer2, which
	;; isn't used by MIOS - but timer2 is 8 bit only (+8 bit period)
	;; so not accurate enough for long delays)

	;; reset the timer, set prescaler to 1:8, so that we can measure
	;; delays of up to 65536 * 8 = 524288 cycles = 52.429 mS
	movlw	0x30
	movwf	T3CON

	;; reset timer registers
	clrf	TMR3L
	clrf	TMR3H

	;; clear interrupt flag - we will use it as overflow indicator
	bcf	PIR2, TMR3IF	

	;; increment running counter, wrap at 0x80
	incf	RUNNING_CTR, F
	bcf	RUNNING_CTR, 7

	;; set debug pin at CORE:J14 to '1' (for measurements with the scope)
	bsf	LATD, 4
	;; start measuring
	bsf	T3CON, TMR3ON

	;; benchmark definition:
	;; send a MIDI event
	;; measure the time until the event will be notified by USER_MPROC_NotifyReceivedEvent
	movlw	0xb0
	call	MIOS_MIDI_TxBufferPut
	movlw	0x01
	call	MIOS_MIDI_TxBufferPut
	movf	RUNNING_CTR, W
	goto	MIOS_MIDI_TxBufferPut


;; --------------------------------------------------------------------------
;;  This function measures the performance of a MIDI connection
;;  Call MIDI_Measure_Stop to stop the measuring
;; --------------------------------------------------------------------------
MIDI_Measure_Stop
	;; stop measuring
	bcf	T3CON, TMR3ON
	;; set debug pin at CORE:J14 to '0'
	bcf	LATD, 4

	bsf	BENCHMARK_UPDATE_REQ, 0
	return

;; --------------------------------------------------------------------------
;;  This function displays the measuring result if a new value is available
;; --------------------------------------------------------------------------
MIDI_Measure_Display
	;; update display if new result is available
	btfss	BENCHMARK_UPDATE_REQ, 0
	return
	bcf	BENCHMARK_UPDATE_REQ, 0

	;; print result
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; branch on overflow
	BRA_IFSET PIR2, TMR3IF, ACCESS, MIDI_Measure_Overflow
	BRA_IFSET BENCHMARK_UPDATE_REQ, 1, ACCESS, MIDI_Measure_Overflow
MIDI_Measure_NoOverflow
	movff	TMR3H, MIOS_PARAMETER1
	movf	TMR3L, W
	call	MIOS_LCD_PrintBCD5
	rgoto	MIDI_Measure_MinMax

MIDI_Measure_Overflow
	bcf	BENCHMARK_UPDATE_REQ, 1

	;; re-init initial min/max delay
	setf	DELAY_MIN_L
	setf	DELAY_MIN_H
	clrf	DELAY_MAX_L
	clrf	DELAY_MAX_H

	TABLE_ADDR MIDI_MEASURE_OV_STR
	call	MIOS_LCD_PrintString
	rgoto	MIDI_Measure_End
MIDI_MEASURE_OV_STR	STRING 16, 0x40, "  oo    ?     ? "


MIDI_Measure_MinMax
	;; determine min delay
	movf	DELAY_MIN_L, W
	subwf	TMR3L, W
	movf	DELAY_MIN_H, W
	subwfb	TMR3H, W
	bc	MIDI_Measure_NoNewMin
MIDI_Measure_NewMin
	movff	TMR3L, DELAY_MIN_L
	movff	TMR3H, DELAY_MIN_H
MIDI_Measure_NoNewMin
	movff	DELAY_MIN_H, MIOS_PARAMETER1
	movf	DELAY_MIN_L, W
	call	MIOS_LCD_PrintBCD5

	movlw	'/'
	call	MIOS_LCD_PrintChar

	;; determine max delay
	movf	TMR3L, W
	subwf	DELAY_MAX_L, W
	movf	TMR3H, W
	subwfb	DELAY_MAX_H, W
	bc	MIDI_Measure_NoNewMax
MIDI_Measure_NewMax
	movff	TMR3L, DELAY_MAX_L
	movff	TMR3H, DELAY_MAX_H
MIDI_Measure_NoNewMax
	movff	DELAY_MAX_H, MIOS_PARAMETER1
	movf	DELAY_MAX_L, W
	call	MIOS_LCD_PrintBCD5

MIDI_Measure_End
	return

	END
