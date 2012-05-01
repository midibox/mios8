; $Id$
;
; A CORE->DIN/DOUT Module Interconnection Test
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

;; ---[ Debug Message Module ]---
#include <debug_msg.inc>


;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	clrf	SRIO_PIN_NUMBER
	call	SRIO_SetPin
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

	call	DEBUG_MSG_SendHeader
	movlw	'\n'
	call	DEBUG_MSG_SendChar
	call	DEBUG_MSG_SendFooter
	
	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"SRIO Interconnection Test\n", 0
	call	DEBUG_MSG_SendFooter
	
	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"=========================\n", 0
	call	DEBUG_MSG_SendFooter

	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"Please play a note on any MIDI channel.\n", 0
	call	DEBUG_MSG_SendFooter

	bsf	DISPLAY_UPDATE_REQ, 0

	return

TEXT_WELCOME_0	STRING 13, 0x00, "Test DIN&DOUT"
TEXT_WELCOME_1	STRING 13, 0x40, "Module, Pin: "

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; display update requested from USER_SR_Service_Prepare on every LED change
	btfss	DISPLAY_UPDATE_REQ, 0
	return
	bcf	DISPLAY_UPDATE_REQ, 0

	;; print pin name
	movlw	0x4d
	call	MIOS_LCD_CursorSet
	TABLE_ADDR SRIO_PIN_NAMES_TABLE
	movf	SRIO_PIN_NUMBER, W
	mullw	SRIO_PIN_NAMES_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	SRIO_PIN_NAMES_LEN
	call	MIOS_LCD_PrintPreconfString


	;; send message to MIOS Terminal
	call	DEBUG_MSG_SendHeader

	movf	SRIO_PIN_NUMBER, W
	bz	USER_DISPLAY_Tick_DO
	addlw	-1
	bz	USER_DISPLAY_Tick_SC
	addlw	-1
	bz	USER_DISPLAY_Tick_RC
	rgoto	USER_DISPLAY_Tick_Fail

USER_DISPLAY_Tick_DO
	call	DEBUG_MSG_SendASMString
	db	"Pin J8:SO set to 5V, J8:SC/J9:SC and J8:RC/J9:RC set to 0V. ", 0
	rgoto	USER_DISPLAY_Tick_Cont

USER_DISPLAY_Tick_SC
	call	DEBUG_MSG_SendASMString
	db	"Pin J8:SC/J9:SC set to 5V, J8:SO and J8:RC/J9:RC set to 0V. ", 0
	rgoto	USER_DISPLAY_Tick_Cont

USER_DISPLAY_Tick_RC
	call	DEBUG_MSG_SendASMString
	db	"Pin J8:RC/J9:RC set to 5V, J8:SO and J8:SC/J9:SC set to 0V. ", 0
	rgoto	USER_DISPLAY_Tick_Cont

USER_DISPLAY_Tick_Fail
	call	DEBUG_MSG_SendASMString
	db	"Something unexpected happened! Please report to tk@midibox.org! ", 0
	;; 	rgoto	USER_DISPLAY_Tick_Cont

USER_DISPLAY_Tick_Cont
	BRA_IFSET MIOS_SRIO_PORT_DIN, MIOS_SRIO_PIN_DIN, ACCESS, USER_DISPLAY_Tick_DI1
USER_DISPLAY_Tick_DI0
	call	DEBUG_MSG_SendASMString
	db	"Pin J9:SI reads Logic-0 (ca. 0V).\n", 0
	goto	DEBUG_MSG_SendFooter

USER_DISPLAY_Tick_DI1	
	call	DEBUG_MSG_SendASMString
	db	"Pin J9:SI reads Logic-1 (ca. 5V).\n", 0
	goto	DEBUG_MSG_SendFooter

	

SRIO_PIN_NAMES_LEN	EQU	2
SRIO_PIN_NAMES_NUM	EQU	3
SRIO_PIN_NAMES_TABLE
	db	"DO"
	db	"SC"
	db	"RC"


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; if modulation wheel received: set new pin
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0xb0
	bnz	USER_NotifyReceivedEvent_ChkNote
	movf	MIOS_PARAMETER2, W
	xorlw	0x01
	bnz	USER_NotifyReceivedEvent_ChkNote

	movf	MIOS_PARAMETER3, W
	movwf	SRIO_PIN_NUMBER
	movlw	SRIO_PIN_NAMES_NUM
	cpfslt	SRIO_PIN_NUMBER, ACCESS
	clrf SRIO_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected SRIO number
	call	SRIO_SetPin
	rgoto	USER_NotifyReceivedEvent_End


USER_NotifyReceivedEvent_ChkNote
	;; alternative control via MIDI keyboard (Note On Events with velocity > 0)
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x90
	bnz	USER_NotifyReceivedEvent_End
	movf	MIOS_PARAMETER3, W
	bz	USER_NotifyReceivedEvent_End
	;; normalize note to 0..11 range
	movf	MIOS_PARAMETER2, W
USER_NotifyReceivedEvent_NoteNor
	addlw	-12
	BRA_IFCLR WREG, 7, ACCESS, USER_NotifyReceivedEvent_NoteNor
	addlw	12		; now in range 0..11
	movwf	SRIO_PIN_NUMBER
	
	movlw	SRIO_PIN_NAMES_NUM
	cpfslt	SRIO_PIN_NUMBER, ACCESS
	clrf SRIO_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected SRIO number
	call	SRIO_SetPin
	;; 	rgoto	USER_NotifyReceivedEvent_End

USER_NotifyReceivedEvent_End
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
;;  This function sets a DIN/DOUT pin depending on SRIO_PIN_NUMBER
;; --------------------------------------------------------------------------
MIOS_SRIO_LAT_SCLK	EQU	LATD	; Pin D.3
MIOS_SRIO_PIN_SCLK	EQU	3
MIOS_SRIO_LAT_RCLK	EQU	LATD	; Pin D.2
MIOS_SRIO_PIN_RCLK	EQU	2
MIOS_SRIO_PORT_DIN	EQU	PORTD	; Pin D.1
MIOS_SRIO_PIN_DIN	EQU	1
MIOS_SRIO_LAT_DOUT	EQU	LATD	; Pin D.0
MIOS_SRIO_PIN_DOUT	EQU	0

SRIO_SetPin
	;; clear the pins
	bcf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	bcf	MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	bcf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK

	;; set the selected pin
	movf	SRIO_PIN_NUMBER, W
	JUMPTABLE_2BYTES 3	; entries
	rgoto	SRIO_SetPin0
	rgoto	SRIO_SetPin1
	rgoto   SRIO_SetPin2

SRIO_SetPin0
	bsf	MIOS_SRIO_LAT_DOUT, MIOS_SRIO_PIN_DOUT
	return

SRIO_SetPin1
	bsf	MIOS_SRIO_LAT_SCLK, MIOS_SRIO_PIN_SCLK
	return

SRIO_SetPin2
	bsf	MIOS_SRIO_LAT_RCLK, MIOS_SRIO_PIN_RCLK
	return

	END
