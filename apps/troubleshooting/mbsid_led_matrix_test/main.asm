; $Id$
;
; A LED Matrix Test for the MIDIbox SID Control Surface
;
;; define the shift registers to which the LED matrix is connected
;; (note: HERE the shift register begins with 0: 1st SR is 0, 2nd is 1, 3rd is 2, ...)
#define MOD_MATRIX_ANODES	6	; shift register with anodes (HERE: 7th shift register in the chain)
#define MOD_MATRIX_CATHODES	7	; shift register with cathodes (HERE: 8th shift register in the chain)
;; 
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
;;  Inlclude the LED Matrix Code
;; ==========================================================================
#include "cs_menu_matrix.inc"

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the shift registers
	movlw	0x10			; use all 16 shift registers
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy
	call	MIOS_SRIO_UpdateFrqSet
	movlw	0x00			; disable touch sensor
	call	MIOS_SRIO_TS_SensitivitySet

	;; LED matrix: vertical mode
	bsf	CS_MENU_MATRIX_MODE, CS_MENU_MATRIX_MODE_VERTICAL
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

	TABLE_ADDR TEXT_WELCOME_0
	call	MIOS_LCD_PrintString
	call	MIOS_LCD_PrintString

	return

TEXT_WELCOME_0	STRING 15, 0x00, "LED Matrix Test"
TEXT_WELCOME_1	STRING  7, 0x40, "X=   Y="

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; display update requested from USER_SR_Service_Prepare on every LED change
	btfss	DISPLAY_UPDATE_REQ, 0
	return
	bcf	DISPLAY_UPDATE_REQ, 0

	;; print LED X/Y coordinate

	movlw	0x42
	call	MIOS_LCD_CursorSet
	movf	LED_SELECT_CTR, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD2

	movlw	0x47
	call	MIOS_LCD_CursorSet
	rrf	LED_SELECT_CTR, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	addlw	1
	call	MIOS_LCD_PrintBCD2
	
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
	;; decrement delay counter, don't increment LED number when counter != 0
	;; (means: LED will be incremented every 256 mS)
	decf	LED_DELAY_CTR, F
	bnz	USER_SR_Service_Prepare_End

	;; increment counter, wrap at 56
	incf	LED_SELECT_CTR, F
	movlw	56
	cpfslt	LED_SELECT_CTR, ACCESS
	clrf LED_SELECT_CTR

	;; request display update
	bsf	DISPLAY_UPDATE_REQ, 0

	;; create new LED pattern
	clrf	CS_MENU_MATRIX_BEGIN+0
	clrf	CS_MENU_MATRIX_BEGIN+1
	clrf	CS_MENU_MATRIX_BEGIN+2
	clrf	CS_MENU_MATRIX_BEGIN+3
	clrf	CS_MENU_MATRIX_BEGIN+4
	clrf	CS_MENU_MATRIX_BEGIN+5
	clrf	CS_MENU_MATRIX_BEGIN+6
	clrf	CS_MENU_MATRIX_BEGIN+7

	lfsr	FSR2, CS_MENU_MATRIX_BEGIN
	movf	LED_SELECT_CTR, W
	andlw	0x07
	addwf	FSR2L, F

	rrf	LED_SELECT_CTR, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x07
	call	MIOS_HLP_GetBitORMask
	movwf	INDF2
	
	
	
USER_SR_Service_Prepare_End
	;; branch to LED Matrix code
	goto	CS_MENU_MATRIX_Handler


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
