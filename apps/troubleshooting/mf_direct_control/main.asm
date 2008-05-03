; $Id$
;
; MF Direct Control Test
;
; ==========================================================================
;
;; Pins of MF Shift Registers (don't change this if you are unsure about the purpose)
#define MIOS_MF_LAT_RCLK	LATC	; Pin C.0
#define MIOS_MF_PIN_RCLK	0
#define MIOS_MF_LAT_SCLK	LATC	; Pin C.1
#define MIOS_MF_PIN_SCLK	1
#define MIOS_MF_LAT_DOUT	LATC	; Pin C.2
#define MIOS_MF_PIN_DOUT	2
;
; ==========================================================================
;
;  Copyright 2005 Thorsten Klose (tk@midibox.org)
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
	;; define number of shift registers: for 128 buttons and LEDs, we need
	;; 16 registers. Btw.: thats the maximum number of supported DIN/DOUTs
	movlw	16
	call	MIOS_SRIO_NumberSet

	;; define the update frequency (latency) of DIN/DOUTs in mS
	movlw	1 ; ms
	call	MIOS_SRIO_UpdateFrqSet

	;; reset MF chain
	clrf	MF_CHAIN0
	clrf	MF_CHAIN1
	call	MF_UpdateSR
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
	call	MIOS_LCD_Clear

	TABLE_ADDR TEXT_LINE_0
	call	MIOS_LCD_PrintString

	;; request screen update
	bsf	DISPLAY_UPDATE_REQ, 0

	return

TEXT_LINE_0	STRING 16, 0x00, "M1M2M3M4M5M6M7M8"

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; update screen if requested
	btfss	DISPLAY_UPDATE_REQ, 0
	return
	bcf	DISPLAY_UPDATE_REQ, 0		; clear request

	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; print 16 bits
	movff	MF_CHAIN0, MIOS_PARAMETER1
	movff	MF_CHAIN1, MIOS_PARAMETER2

	movlw	16
	movwf	TMP1
USER_DISPLAY_TickLoop
	movlw	'0'
	btfsc	MIOS_PARAMETER1, 7
	movlw '1'
	call	MIOS_LCD_PrintChar

	rlf	MIOS_PARAMETER2, F
	rlf	MIOS_PARAMETER1, F

	decfsz	TMP1, F
	rgoto	USER_DISPLAY_TickLoop

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
	;; exit if button number >= 0x10
	movlw	0x10
	cpfslt	MIOS_PARAMETER1, ACCESS
	return

	;; just copy the inverted state of DIN SR 0 and 1 to MF_CHAIN[01]
	movlw	0x00
	call	MIOS_DIN_SRGet
	xorlw	0xff
	movwf	MF_CHAIN0

	movlw	0x01
	call	MIOS_DIN_SRGet
	xorlw	0xff
	movwf	MF_CHAIN1

	;; update MF chain
	call	MF_UpdateSR

	;; request screen update
	bsf	DISPLAY_UPDATE_REQ, 0

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
;;  This function updates the shift register chain of the MF module
;;  Input:
;;     o shift register state in MF_CHAIN[01]
;; --------------------------------------------------------------------------
MF_UpdateSR

	;; roll out loop to make it faster!
MF_UPDATESR_HLP MACRO SRMx, SRMx_bit
	btfsc	SRMx, SRMx_bit
	bsf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT
	btfss	SRMx, SRMx_bit
	bcf	MIOS_MF_LAT_DOUT, MIOS_MF_PIN_DOUT

        bsf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK	; toggle clock
        bcf     MIOS_MF_LAT_SCLK, MIOS_MF_PIN_SCLK
	ENDM

	MF_UPDATESR_HLP MF_CHAIN1, 7
	MF_UPDATESR_HLP MF_CHAIN1, 6
	MF_UPDATESR_HLP MF_CHAIN1, 5
	MF_UPDATESR_HLP MF_CHAIN1, 4
	MF_UPDATESR_HLP MF_CHAIN1, 3
	MF_UPDATESR_HLP MF_CHAIN1, 2
	MF_UPDATESR_HLP MF_CHAIN1, 1
	MF_UPDATESR_HLP MF_CHAIN1, 0

	MF_UPDATESR_HLP MF_CHAIN0, 7
	MF_UPDATESR_HLP MF_CHAIN0, 6
	MF_UPDATESR_HLP MF_CHAIN0, 5
	MF_UPDATESR_HLP MF_CHAIN0, 4
	MF_UPDATESR_HLP MF_CHAIN0, 3
	MF_UPDATESR_HLP MF_CHAIN0, 2
	MF_UPDATESR_HLP MF_CHAIN0, 1
	MF_UPDATESR_HLP MF_CHAIN0, 0

        bsf     MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK	; trigger latch
        bcf     MIOS_MF_LAT_RCLK, MIOS_MF_PIN_RCLK

	return

	END
