; $Id$
;
; Using J5 as DOUT
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
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  Including and configure J5_IO driver from $MIOS_PATH/modules/j5_io
;; ==========================================================================

;; if set, input functions won't be provided by the driver
#define J5_IO_DONT_USE_INPUT_FUNCTIONS  1

;; if set, output functions won't be provided by the driver
#define J5_IO_DONT_USE_OUTPUT_FUNCTIONS 0
#include <j5_io.inc>

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

	;; initialize J5 as digitial output
	call	J5_IO_Init

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
TEXT_WELCOME_0	STRING 16, 0x00, "J5_DOUT Example "
TEXT_WELCOME_1	STRING 16, 0x40, "Status: ........"

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

	;; print current status of the digital output pins at J5 at second line, pos #8
	movlw	0x48
	call	MIOS_LCD_CursorSet

	movlw	'o'
	btfsc	J5_STATUS, 0
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 1
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 2
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 3
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 4
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 5
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 6
	movlw '*'
	call	MIOS_LCD_PrintChar

	movlw	'o'
	btfsc	J5_STATUS, 7
	movlw '*'
	call	MIOS_LCD_PrintChar

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
	;; forward to J5_IO_PinSet function
	;; it expects: number of pin (0-7) in WREG
	;; value in MIOS_PARAMETER1

	;; store pin number
	movwf	TMP1

	;; set the pin value depending on velocity:
	;;    - 0x00: set pin to 0V
	;;    - other values: set pin to 5V
	movf	MIOS_PARAMETER3, W
	skpz
	movlw	0x01
	movwf	TMP2

	;; if the event was a note off event, zero value independent from velocity
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x80
	skpnz
	clrf	TMP2

	;; finally copy TMP2 to MIOS_PARAMETER1, get pin number and call the J5_IO_PinSet function
	movff	TMP2, MIOS_PARAMETER1
	movf	TMP1, W
	call	J5_IO_PinSet

	;; set status
	movf	TMP1, W			; get AND mask depending on pin number
	call	MIOS_HLP_GetBitANDMask
	andwf	J5_STATUS, F		; AND it with J5_STATUS (means: clear bit entry)
	movf	TMP1, W			; get OR mask depending on pin number
	call	MIOS_HLP_GetBitORMask
	btfsc	MIOS_PARAMETER1, 0	; set this bit if MIOS_PARAMETER1[0] not cleared
	iorwf	J5_STATUS, F

	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0

	;; and exit
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

	END
