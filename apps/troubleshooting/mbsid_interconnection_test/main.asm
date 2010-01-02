; $Id$
;
; A CORE->SID Module Interconnection Test
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

;; ---[ Debug Message Module ]---
#include <debug_msg.inc>


;; ==========================================================================
;;  Include the SID SR Code
;; ==========================================================================
#include "sid_sr.inc"


;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	call	SID_SR_Init
	clrf	SID_PIN_NUMBER
	call	SID_SetPin
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
	db	"MBSID Interconnection Test\n", 0
	call	DEBUG_MSG_SendFooter
	
	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"==========================\n", 0
	call	DEBUG_MSG_SendFooter

	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"Please play a note on any MIDI channel.\n", 0
	call	DEBUG_MSG_SendFooter

	bsf	DISPLAY_UPDATE_REQ, 0

	return

TEXT_WELCOME_0	STRING 15, 0x00, "Test SID Module"
TEXT_WELCOME_1	STRING  4, 0x40, "Pin:"

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
	movlw	0x45
	call	MIOS_LCD_CursorSet
	TABLE_ADDR SID_PIN_NAMES_TABLE
	movf	SID_PIN_NUMBER, W
	mullw	SID_PIN_NAMES_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	SID_PIN_NAMES_LEN
	call	MIOS_LCD_PrintPreconfString

	
	;; send message to MIOS Terminal
	call	DEBUG_MSG_SendHeader

	;; special message for CS# pin (since it's inverted)
	movf	SID_PIN_NUMBER, W
	xorlw	14
	bnz	USER_DISPLAY_Tick_Normal
USER_DISPLAY_Tick_CS
	call	DEBUG_MSG_SendASMString
	db	"Pin 'CS#' of SID chip set to 5V, remaining digital pins set to 0V as well.", 0
	call	DEBUG_MSG_SendFooter
	return


USER_DISPLAY_Tick_Normal
	call	DEBUG_MSG_SendASMString
	db	"Pin '", 0

	TABLE_ADDR SID_PIN_NAMES_TABLE
	movf	SID_PIN_NUMBER, W
	mullw	SID_PIN_NAMES_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; always four chars
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar

	call	DEBUG_MSG_SendASMString
	db	"' of SID chip set to 5V, remaining digital pins set to 0V (exception: CS# set to 5V)", 0
	call	DEBUG_MSG_SendFooter

	return
	

SID_PIN_NAMES_LEN	EQU	4
SID_PIN_NAMES_NUM	EQU	15
SID_PIN_NAMES_TABLE
	db	"A0  "
	db	"A1  "
	db	"A2  "
	db	"A3  "
	db	"A4  "
	db	"RES#"
	db	"D0  "
	db	"D1  "
	db	"D2  "
	db	"D3  "
	db	"D4  "
	db	"D5  "
	db	"D6  "
	db	"D7  "
	db	"CS# "


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
	movwf	SID_PIN_NUMBER
	movlw	SID_PIN_NAMES_NUM
	cpfslt	SID_PIN_NUMBER, ACCESS
	clrf SID_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected SID number
	call	SID_SetPin
	rgoto	USER_NotifyReceivedEvent_End


USER_NotifyReceivedEvent_ChkNote
	;; alternative control via MIDI keyboard (Note On Events with velocity > 0)
	movf	MIOS_PARAMETER1, W
	andlw	0xf0
	xorlw	0x90
	bnz	USER_NotifyReceivedEvent_End
	movf	MIOS_PARAMETER3, W
	bz	USER_NotifyReceivedEvent_End
	;; normalize note to 0..23 range
	movf	MIOS_PARAMETER2, W
USER_NotifyReceivedEvent_NoteNor
	addlw	-24
	BRA_IFCLR WREG, 7, ACCESS, USER_NotifyReceivedEvent_NoteNor
	addlw	24		; now in range 0..23
	movwf	SID_PIN_NUMBER
	
	movlw	SID_PIN_NAMES_NUM
	cpfslt	SID_PIN_NUMBER, ACCESS
	clrf SID_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected SID number
	call	SID_SetPin
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
;;  This function sets a SID pin depending on SID_PIN_NUMBER
;; --------------------------------------------------------------------------
SID_SetPin
	bcf	SID_SR_LAT_WR, SID_SR_PIN_WR
	clrf	MIOS_PARAMETER1
	clrf	MIOS_PARAMETER2

	movf	SID_PIN_NUMBER, W
	JUMPTABLE_2BYTES 15	; entries
	rgoto	SID_SetPin0
	rgoto	SID_SetPin1
	rgoto	SID_SetPin2
	rgoto	SID_SetPin3
	rgoto	SID_SetPin4
	rgoto	SID_SetPin5
	rgoto	SID_SetPin6
	rgoto	SID_SetPin7
	rgoto	SID_SetPin8
	rgoto	SID_SetPin9
	rgoto	SID_SetPin10
	rgoto	SID_SetPin11
	rgoto	SID_SetPin12
	rgoto	SID_SetPin13
	rgoto	SID_SetPin14

SID_SetPin0
	movlw	0x01		; A0
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin1
	movlw	0x02		; A1
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin2
	movlw	0x04		; A2
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin3
	movlw	0x08		; A3
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin4
	movlw	0x10		; A4
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin5
	movlw	0x20		; RES#
	movwf	MIOS_PARAMETER1
	goto	SID_SR_Write

SID_SetPin6
	movlw	0x01		; D0
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin7
	movlw	0x02		; D1
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin8
	movlw	0x04		; D2
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin9
	movlw	0x08		; D3
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin10
	movlw	0x10		; D4
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin11
	movlw	0x20		; D5
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin12
	movlw	0x40		; D6
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin13
	movlw	0x80		; D7
	movwf	MIOS_PARAMETER2
	goto	SID_SR_Write

SID_SetPin14
	bcf	SID_SR_LAT_WR, SID_SR_PIN_WR
	return

	END
