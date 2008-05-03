; $Id$
;
; Jitter monitor
;
; set this #define to 1 if multiplexers are connected
#define AIN_MUXED 0
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
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

#if AIN_MUXED == 1
	;; enable all 64 inputs
	movlw	64
	call	MIOS_AIN_NumberSet

	;; use the AIN 4051 multiplexers
	call	MIOS_AIN_Muxed
#else
	;; enable all 8 inputs
	movlw	8
	call	MIOS_AIN_NumberSet

	;; don't use the AIN 4051 multiplexers
	call	MIOS_AIN_UnMuxed
#endif

	;; set deadband to zero so that we regognize the jitter
	movlw	0
	call	MIOS_AIN_DeadbandSet

	;; set invalid flag (will be cleared by AIN handler after a valid value has been notified)
	bsf	AIN_VALUE_H, 7
	bsf	JITTER_VALUE_H, 7

	;; timer should be called every 20 ms == 200000 ticks
	;; since this value is greater than 16 bit (== 65535 max) we just
	;; use a 1:4 prescaler so the init value must be: 200000 / 4 == 50000
	;; (see also the MIOS_TIMER_Init documentation)
	movlw	50000 & 0xff		; low-byte
	movwf	MIOS_PARAMETER1
	movlw	50000 >> 8		; high-byte
	movlw	0x02			; use 1:4 prescaler
	call	MIOS_TIMER_Init

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
	;; don't continue if jitter value is not valid (AIN input has not been sampled yet)
	btfsc	JITTER_VALUE_H, 7
	return

	;; this routine is called every 20 ms (see USER_Init)
	;; we want to reset the maximum jitter value every 2 seconds,
	;; so we have have to use a post scaler which counts from 0 to 99
	incf	TIMER_POSTSCALER, F
	movlw	99
	cpfsgt	TIMER_POSTSCALER, ACCESS
	rgoto USER_Timer_NoPSOverrun

	;; else reset internal jitter value
	clrf	INT_JITTER_VALUE_L
	clrf	INT_JITTER_VALUE_H
USER_Timer_NoPSOverrun

	;; don't continue if reset counter already zero
	movf	TIMER_RSTCTR, W
	bz	USER_Timer_End

	;; else decrement counter
	decf	TIMER_RSTCTR, F

	;; if counter == 0, reset the displayed jitter value and request display update
	bnz	USER_Timer_End
	clrf	JITTER_VALUE_L
	clrf	JITTER_VALUE_H
	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0
USER_Timer_End

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
	call	MIOS_LCD_PrintString		; first line
	call	MIOS_LCD_PrintString		; second line

	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0

	return

			       ;; <--------------> 
TEXT_WELCOME_0	STRING 16, 0x00, "Pin Value Jitter"
TEXT_WELCOME_1	STRING 16, 0x40, " xx   xxx    xxx"

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; leave routine if update has not been requested
	btfss	DISPLAY_UPDATE_REQ, 0
	return

	;; clear the request flag so that this routine will not be processed
	;; until the flag has been set again
	bcf	DISPLAY_UPDATE_REQ, 0

	;; print pin number
	movlw	0x40			; move cursor to second line, pos #0
	call	MIOS_LCD_CursorSet

	movf	AIN_PIN, W		; print pin number +1
	addlw	1
	call	MIOS_LCD_PrintBCD3

	;; print position value
	movlw	0x46			; move cursor to second line, pos #6
	call	MIOS_LCD_CursorSet

	lfsr	FSR0, AIN_VALUE_L	; print value
	rcall	DISPLAY_PrintHex3

	;; print jitter value
	movlw	0x4d			; move cursor to second line, pos #13
	call	MIOS_LCD_CursorSet

	lfsr	FSR0, JITTER_VALUE_L	; print value
	rcall	DISPLAY_PrintHex3

	return

;; ---
DISPLAY_PrintHex3
	;; utility routine: prints "---" when the invalid flag (*_VALUE_H[7] is set, else a 14-bit value)
	incf	FSR0L, F		; switch to high byte
	BRA_IFCLR INDF0, 7, ACCESS, DISPLAY_PrintHex3_Valid
DISPLAY_PrintHex3_Invalid
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	movlw	'-'
	call	MIOS_LCD_PrintChar
	return

DISPLAY_PrintHex3_Valid
	movf	POSTDEC0, W		; print high byte (decrement pointer)
	call	MIOS_LCD_PrintHex1
	movf	INDF0, W		; print low byte
	call	MIOS_LCD_PrintHex2

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
	;; branch depending on button
	movf	MIOS_PARAMETER1, W
	andlw	0x01		; a nice trick: this "and" function mirrors all DIN pairs, so
				; that DEC buttons are on even pins and INC buttons are on odd pins :)
	JUMPTABLE_2BYTES 2	; 2 entries
	rgoto	DIN_Dec_Button
	rgoto	DIN_Inc_Button

;; ---
DIN_Dec_Button
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0xff		; decrement value
	rgoto	DIN_IncDec

;; ---
DIN_Inc_Button
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	movlw	0x01		; increment value
	rgoto	DIN_IncDec

;; ---
DIN_IncDec
	;; store incrementer in TMP1
	movwf	TMP1

	;; now we have to add (or subtract) the incrementer from the absolute value,
	;; which has been stored in INC_VALUE_16BIT_L

	;; the simplest solution is the use of MIOS_HLP_16bitAddSaturate, which
	;; can handle with values up to 16 bit!!! :-)
	;; please refer to the function documentation for the input/output parameters!

	;; set pointer to INC_VALUE_16BIT_L
	;; (it is located to an even address, thats important when this function is used!)
	;; (and since we are modifying a 16-bit value, the next address to the low-byte
	;; is used as high-byte -> see app_defines.h)
	lfsr	FSR1, INC_VALUE_16BIT_L
	;; set max value
	call	MIOS_AIN_NumberGet
	addlw	-1
	movwf	MIOS_PARAMETER1
	movlw	0x00	; high-byte of max value
	movwf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in TMP1)
	movf	TMP1, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = ENC_VALUE_16BIT_L contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; now copy INC_VALUE_16BIT_L to AIN_PIN
	movff	INC_VALUE_16BIT_L, AIN_PIN

	;; clear current AIN and JITTER value
	clrf	AIN_VALUE_L
	clrf	AIN_VALUE_H
	clrf	JITTER_VALUE_L
	clrf	JITTER_VALUE_H
	clrf	INT_JITTER_VALUE_L
	clrf	INT_JITTER_VALUE_H
	;; set invalid flag (will be cleared by AIN handler after a valid value has been notified)
	bsf	AIN_VALUE_H, 7
	bsf	JITTER_VALUE_H, 7

	;; request a display update by setting DISPLAY_UPDATE_REQ,0
	;; USER_DISPLAY_Tick polls this flag!
	bsf	DISPLAY_UPDATE_REQ, 0

	;; thats all
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
	;; don't continue if pot number != selected AIN pin number
	cpfseq	AIN_PIN, ACCESS
	return

	;; if invalid bit of AIN value set, copy current result to AIN_VALUE_[LH]
	BRA_IFCLR AIN_VALUE_H, 7, ACCESS, USER_AIN_LastResultValid
	movff	MIOS_PARAMETER2, AIN_VALUE_L
	movff	MIOS_PARAMETER3, AIN_VALUE_H
	;; (note: this also clears the invalid flag H[7])
USER_AIN_LastResultValid

	;; clear invalid bit of jitter value
	bcf	JITTER_VALUE_H, 7

	;; calculate the delta value between old and new jitter result
	;; calc TMP[12] = MIOS_PARAMETER[23] - AIN_VALUE_[LH]
	movf	AIN_VALUE_L, W
	subwf	MIOS_PARAMETER2, W
	movwf	TMP1
	movf	AIN_VALUE_H, W
	subwfb	MIOS_PARAMETER3, W
	movwf	TMP2
	;; if result negative, invert values
	btfsc	TMP2, 7
	comf	TMP1, F
	btfsc	TMP2, 7
	comf	TMP2, F

	;; if new delta > jitter value, copy result to jitter value
	;; calc TMP[12] - INT_JITTER_VALUE_[LH]
	movf	INT_JITTER_VALUE_L, W
	subwf	TMP1, W
	movf	INT_JITTER_VALUE_H, W
	subwfb	TMP2, W

	BRA_IFSET WREG, 7, ACCESS, USER_AIN_NoGreaterJitter	
USER_AIN_GreaterJitter
	movff	TMP1, INT_JITTER_VALUE_L
	movff	TMP2, INT_JITTER_VALUE_H
	;; new peak: reset prescaler so that the peak is visible for about 2 seconds
	clrf	TIMER_POSTSCALER
USER_AIN_NoGreaterJitter

	;; store internal jitter value also in displayed jitter value
	movff	INT_JITTER_VALUE_L, JITTER_VALUE_L
	movff	INT_JITTER_VALUE_H, JITTER_VALUE_H

	;; reload reset counter for displayed jitter value
	movlw	100
	movwf	TIMER_RSTCTR

	;; store LSB and MSB in AIN_VALUE_[LH]
	movff	MIOS_PARAMETER2, AIN_VALUE_L
	movff	MIOS_PARAMETER3, AIN_VALUE_H

	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0

	return

	END
