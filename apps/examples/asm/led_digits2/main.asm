; $Id$
;
; LED Digits Example 2
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
#include <mios_mt_table.inc>
#include <mios_enc_table.inc>

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  The LED digits handler
;; ==========================================================================
#include "leddigits.inc"

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; define number of shift registers: for 128 IOs, we need
	;; 16 registers. Btw.: thats the maximum number of supported DIN/DOUTs
	movlw	16
	call	MIOS_SRIO_NumberSet

	;; define the update frequency (latency) of DIN/DOUTs in mS
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; initialize the LED digits
	call	LEDDIGITS_Init

	;; initialize the countdown register, starting with 9999
	movlw	((9999) >> 0) & 0xff
	movwf	COUNTDOWN_L
	movlw	((9999) >> 8) & 0xff
	movwf	COUNTDOWN_H

	;; initialize the countup register, starting with 0000
	movlw	((0) >> 0) & 0xff
	movwf	COUNTUP_L
	movlw	((0) >> 8) & 0xff
	movwf	COUNTUP_H

	;; timer handles the countdown
	;; we want to setup the timer with a frequency of 25 Hz == 40 mS
	;; prescaler 1:8 should be used
	;; calculate the required number of clocks for this period:
	;; clocks = (period / 100 nS)/8 = (40 mS / 100 nS)/8 = 50000
	;; calculate low and high byte:
	;;    low byte  = period & 0xff
	;;    high byte = period >> 8
	;; therefore:
	movlw	50000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	50000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x03		; (1:8)
	call	MIOS_TIMER_Init

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	IRQ_DISABLE			; temporary disable interrupts so that we get a consistent counter value
	
	;; move current countdown value to the leftmost digits
	;; move first digit to LEDDIGITS_4
	movf	COUNTDOWN_L, W
	andlw	0x0f
	iorlw	0x80		; set the "dot" here!
	movwf	LEDDIGITS_4

	;; move second digit to LEDDIGITS_5
	swapf	COUNTDOWN_L, W
	andlw	0x0f
	movwf	LEDDIGITS_5
	
	;; move third digit to LEDDIGITS_6
	movf	COUNTDOWN_H, W
	andlw	0x0f
	movwf	LEDDIGITS_6
	
	;; move fourth digit to LEDDIGITS_7
	swapf	COUNTDOWN_H, W
	andlw	0x0f
	movwf	LEDDIGITS_7
	
	;; convert 16-bit value to BCD
	movf	COUNTDOWN_H, W		; move high-byte to MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
	movf	COUNTDOWN_L, W		; move low-byte to WREG
	IRQ_ENABLE			; enable the IRQs again

	call	MIOS_HLP_Dec2BCD	; convert to BCD code
	;; now:
	;;    MIOS_PARAMETER3 contains the leftmost digits (n5*10^5, b4*10^4)
	;;    MIOS_PARAMETER2 contains the middle digits (n3*10^3, n2*10^2)
	;;    MIOS_PARAMETER1 contains the rightmost digits (n1*10^1, n0*10^0)

	;; move first digit to LEDDIGITS_0
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_0

	;; move second digit to LEDDIGITS_1
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_1

	;; move third digit to LEDDIGITS_2
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_2

	;; move fourth digit to LEDDIGITS_3
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_3




	IRQ_DISABLE			; temporary disable interrupts so that we get a consistent counter value
	
	;; move current countup value to the leftmost digits
	;; move first digit to LEDDIGITS_12
	movf	COUNTUP_L, W
	andlw	0x0f
	iorlw	0x80		; set the "dot" here!
	movwf	LEDDIGITS_12

	;; move second digit to LEDDIGITS_13
	swapf	COUNTUP_L, W
	andlw	0x0f
	movwf	LEDDIGITS_13
	
	;; move third digit to LEDDIGITS_14
	movf	COUNTUP_H, W
	andlw	0x0f
	movwf	LEDDIGITS_14
	
	;; move fourth digit to LEDDIGITS_15
	swapf	COUNTUP_H, W
	andlw	0x0f
	movwf	LEDDIGITS_15
	
	;; convert 16-bit value to BCD
	movf	COUNTUP_H, W		; move high-byte to MIOS_PARAMETER1
	movwf	MIOS_PARAMETER1
	movf	COUNTUP_L, W		; move low-byte to WREG
	IRQ_ENABLE			; enable the IRQs again

	call	MIOS_HLP_Dec2BCD	; convert to BCD code
	;; now:
	;;    MIOS_PARAMETER3 contains the leftmost digits (n5*10^5, b4*10^4)
	;;    MIOS_PARAMETER2 contains the middle digits (n3*10^3, n2*10^2)
	;;    MIOS_PARAMETER1 contains the rightmost digits (n1*10^1, n0*10^0)

	;; move first digit to LEDDIGITS_8
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_8

	;; move second digit to LEDDIGITS_9
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_9

	;; move third digit to LEDDIGITS_10
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_10

	;; move fourth digit to LEDDIGITS_11
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_11

	return

;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; decrement countdown registers if != 0
	movf	COUNTDOWN_L, W	; check if counter is zero
	iorwf	COUNTDOWN_H, W
	bz	USER_Timer_NoCountdown
USER_Timer_Countdown
	;; a 16-bit decrement can be realized like shown here:	
	decf	COUNTDOWN_L, F
	skpc
	decf	COUNTDOWN_H, F

	;; conversion done in mainprogram (timers should be handled so fast as possible)
USER_Timer_NoCountdown

	;; increment counter if != 9999
	movf	COUNTDOWN_L, W
	xorlw	(9999 << 0) & 0xff
	bnz	USER_Timer_Countup
	movf	COUNTDOWN_L, W
	xorlw	(9999 << 8) & 0xff
	bz	USER_Timer_NoCountup
USER_Timer_Countup
	;; a 16-bit increment can be realized like shown here:	
	incf	COUNTUP_L, F
	skpnc
	incf	COUNTUP_H, F

	;; conversion done in mainprogram (timers should be handled so fast as possible)
USER_Timer_NoCountup
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
	call	MIOS_LCD_Clear			; clear LCD

	TABLE_ADDR TEXT_WELCOME_0		; print welcome message
	call	MIOS_LCD_PrintString

	return
				;;<--------------->
TEXT_WELCOME_0	STRING 16, 0x00, "LED Digits Exmp."

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; print the current counter value:
	movlw	0x40			; move cursor to second line, first column
	call	MIOS_LCD_CursorSet

	movf	LEDDIGITS_15, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_14, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_13, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_12, W
	call	MIOS_LCD_PrintHex1

	movf	LEDDIGITS_11, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_10, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_9, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_8, W
	call	MIOS_LCD_PrintBCD1

	movf	LEDDIGITS_7, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_6, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_5, W
	call	MIOS_LCD_PrintHex1
	movf	LEDDIGITS_4, W
	call	MIOS_LCD_PrintHex1

	movf	LEDDIGITS_3, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_2, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_1, W
	call	MIOS_LCD_PrintBCD1
	movf	LEDDIGITS_0, W
	call	MIOS_LCD_PrintBCD1

	return


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
;;  which has been specified in the MIOS_MPROC_EVENT_TABLE table
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
	;; update the LED digits before the DOUT registers will be re-loaded
	call	LEDDIGITS_Update
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

	END
