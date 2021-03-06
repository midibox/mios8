; $Id$
;
; DOUT Test
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

	;; clear DOUT pin variable
	clrf	DOUT_PIN

	;; set DOUT pin
	movf	DOUT_PIN, W
	call	MIOS_DOUT_PinSet1

	;; enable this section with "#if 1" it if you want
#if 0
	;; enable auto repeat for DIN #0 and #1
	movlw	0x00
	call	MIOS_DIN_PinAutoRepeatEnable
	movlw	0x01
	call	MIOS_DIN_PinAutoRepeatEnable
#endif

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
	call	MIOS_LCD_Clear			; clear LCD

	TABLE_ADDR TEXT_WELCOME_0		; print welcome message
	call	MIOS_LCD_PrintString		; first line
	call	MIOS_LCD_PrintString		; second line

	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0

	return

			       ;; <--------------> 
TEXT_WELCOME_0	STRING 16, 0x00, "Digital Out Test"
TEXT_WELCOME_1	STRING 16, 0x40, "SR#.. Pin#. 0x.."

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

	;; print SR number
	movlw	0x43			; move cursor to second line, pos #3
	call	MIOS_LCD_CursorSet

	rrf	DOUT_PIN, W		; print SR number (=(DOUT/3)+1)
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	addlw	1
	call	MIOS_LCD_PrintBCD2

	;; print pin number
	movlw	0x4a			; move cursor to second line, pos #10
	call	MIOS_LCD_CursorSet

	movf	DOUT_PIN, W		; print pin number (=(DOUT&7))
	andlw	0x07
	call	MIOS_LCD_PrintBCD1

	;; print pin ID
	movlw	0x4e			; move cursor to second line, pos #14
	call	MIOS_LCD_CursorSet

	movf	DOUT_PIN, W		; print pin ID in hexadecimal format
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
	movlw	0x7f	; low-byte of max value
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

	;; clear old DOUT pin
	movf	DOUT_PIN, W
	call	MIOS_DOUT_PinSet0

	;; now copy INC_VALUE_16BIT_L to DOUT_PIN
	movff	INC_VALUE_16BIT_L, DOUT_PIN

	;; set new DOUT pin
	movf	DOUT_PIN, W
	call	MIOS_DOUT_PinSet1

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
	return

	END
