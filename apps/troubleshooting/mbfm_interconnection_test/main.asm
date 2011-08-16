; $Id$
;
; A CORE->MBHP_OPL3 Module Interconnection Test
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

; define the pins to which the MBHP_OPL3 module is connected
;
MBFM_LAT_D	EQU	LATB		; Port B
MBFM_LAT_D_23_18F4685 EQU LATE		; RB[23] -> RE[12]
MBFM_LAT_A1	EQU	LATD		; Pin D.6
MBFM_PIN_A1	EQU	6
MBFM_LAT_A0	EQU	LATD		; Pin D.5
MBFM_PIN_A0	EQU	5
MBFM_LAT_CS	EQU	LATC		; Pin C.5
MBFM_PIN_CS	EQU	5
MBFM_LAT_IC	EQU	LATC		; Pin C.4
MBFM_PIN_IC	EQU	4

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; for PIC18F4685:
	;; Use RE1 and RE2 instead of RB2 and RB3
	;; in this application we will set RE1/RB2 and RE2/RB3 together so that the same
	;; binary can be used for both chips
	movlw	0xf9		; set only TRISE[2:1] as output
	andwf	TRISE, F

	clrf	OPL3_PIN_NUMBER
	call	OPL3_SetPin
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
	db	"MBFM Interconnection Test\n", 0
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

TEXT_WELCOME_0	STRING 16, 0x00, "Test OPL3 Module"
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
	TABLE_ADDR OPL3_PIN_NAMES_TABLE
	movf	OPL3_PIN_NUMBER, W
	mullw	OPL3_PIN_NAMES_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	OPL3_PIN_NAMES_LEN
	call	MIOS_LCD_PrintPreconfString


	;; send message to MIOS Terminal
	call	DEBUG_MSG_SendHeader
	call	DEBUG_MSG_SendASMString
	db	"Pin '", 0

	TABLE_ADDR OPL3_PIN_NAMES_TABLE
	movf	OPL3_PIN_NUMBER, W
	mullw	OPL3_PIN_NAMES_LEN
	movf	PRODL, W
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F

	;; always two chars
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar
	tblrd*+
	movf	TABLAT, W
	call	DEBUG_MSG_SendChar

	call	DEBUG_MSG_SendASMString
	db	"' of MBHP_OPL3 module set to 5V, all other pins set to 0V", 0
	call	DEBUG_MSG_SendFooter

	;; IMPORTANT!!!
	;; Since a LCD message could overwrite the data/address/control
	;; line value, we update the pins again
	goto	OPL3_SetPin


OPL3_PIN_NAMES_LEN	EQU	4
OPL3_PIN_NAMES_NUM	EQU	12
OPL3_PIN_NAMES_TABLE
	db	"RS  "
	db	"A0  "
	db	"A1  "
	db	"WR  "
	db	"D0  "
	db	"D1  "
	db	"D2  "
	db	"D3  "
	db	"D4  "
	db	"D5  "
	db	"D6  "
	db	"D7  "


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
	movwf	OPL3_PIN_NUMBER
	movlw	OPL3_PIN_NAMES_NUM
	cpfslt	OPL3_PIN_NUMBER, ACCESS
	clrf OPL3_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected OPL3 number
	call	OPL3_SetPin
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
	movwf	OPL3_PIN_NUMBER
	
	movlw	OPL3_PIN_NAMES_NUM
	cpfslt	OPL3_PIN_NUMBER, ACCESS
	clrf OPL3_PIN_NUMBER
	bsf	DISPLAY_UPDATE_REQ, 0

	;; set the pin depending on selected OPL3 number
	call	OPL3_SetPin
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
;;  This function sets a OPL3 pin depending on OPL3_PIN_NUMBER
;; --------------------------------------------------------------------------
OPL3_SetPin
	;; clear all pins
	clrf	MBFM_LAT_D
	clrf	MBFM_LAT_D_23_18F4685
	bcf	MBFM_LAT_A0, MBFM_PIN_A0
	bcf	MBFM_LAT_A1, MBFM_PIN_A1
	bcf	MBFM_LAT_CS, MBFM_PIN_CS
	bcf	MBFM_LAT_IC, MBFM_PIN_IC

	;; set selected pin
	movf	OPL3_PIN_NUMBER, W
	JUMPTABLE_2BYTES 12	; entries
	rgoto	OPL3_SetPin_RS
	rgoto	OPL3_SetPin_A0
	rgoto	OPL3_SetPin_A1
	rgoto	OPL3_SetPin_WR
	rgoto	OPL3_SetPin_D0
	rgoto	OPL3_SetPin_D1
	rgoto	OPL3_SetPin_D2
	rgoto	OPL3_SetPin_D3
	rgoto	OPL3_SetPin_D4
	rgoto	OPL3_SetPin_D5
	rgoto	OPL3_SetPin_D6
	rgoto	OPL3_SetPin_D7

OPL3_SetPin_RS
	bsf	MBFM_LAT_IC, MBFM_PIN_IC
	return

OPL3_SetPin_A0
	bsf	MBFM_LAT_A0, MBFM_PIN_A0
	return

OPL3_SetPin_A1
	bsf	MBFM_LAT_A1, MBFM_PIN_A1
	return

OPL3_SetPin_WR
	bsf	MBFM_LAT_CS, MBFM_PIN_CS
	return

OPL3_SetPin_D0
	bsf	MBFM_LAT_D, 0
	return

OPL3_SetPin_D1
	bsf	MBFM_LAT_D, 1
	return

OPL3_SetPin_D2
	bsf	MBFM_LAT_D, 2
	bsf	MBFM_LAT_D_23_18F4685, 1
	return

OPL3_SetPin_D3
	bsf	MBFM_LAT_D, 3
	bsf	MBFM_LAT_D_23_18F4685, 2
	return

OPL3_SetPin_D4
	bsf	MBFM_LAT_D, 4
	return

OPL3_SetPin_D5
	bsf	MBFM_LAT_D, 5
	return

OPL3_SetPin_D6
	bsf	MBFM_LAT_D, 6
	return

OPL3_SetPin_D7
	bsf	MBFM_LAT_D, 7
	return


	END
