; $Id$
;
; LED Digits MIDI Clock Example
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
;;  The MIDI Clock handler
;; ==========================================================================
#include "midiclk.inc"

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

	;; initialize the MIDI Clock handler
	call	MIDICLK_Init
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
	call	MIOS_LCD_PrintString

	;; request a display update
	bsf	DISPLAY_UPDATE_REQ, 0

	return
				;;<--------------->
TEXT_WELCOME_0	STRING 16, 0x00, "LED Digits Clock"

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

	;; print the current counter value:
	movlw	0x40			; move cursor to second line, first column
	call	MIOS_LCD_CursorSet

	call	PRINT_MIDIClk

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

	;; call the MIDI clock handler which parses the incoming stream
	movf	MIOS_PARAMETER1, W
	call	MIDICLK_Handler
	;; returns 0x01 when the display should be updated

	;; if return status != 0, convert the MIDI clock to digits
	bz	USER_MPROC_NotifyReceived_End
	;; convert MIDICLK_* to LEDDIGITS_*
	call	CONVERT_MIDIClk
	;; request display update
	bsf	DISPLAY_UPDATE_REQ, 0
USER_MPROC_NotifyReceived_End
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



;; --------------------------------------------------------------------------
;; this function formats the LEDDIGITS_* and prints them on LCD
;; --------------------------------------------------------------------------
PRINT_MIDIClk
	movlw	' '
	call	MIOS_LCD_PrintChar

	movf	LEDDIGITS_7, W
	call	PRINT_MIDIClk_Hlp
	movf	LEDDIGITS_6, W
	call	PRINT_MIDIClk_Hlp
	movf	LEDDIGITS_5, W
	call	PRINT_MIDIClk_Hlp
	movf	LEDDIGITS_4, W
	call	MIOS_LCD_PrintBCD1
	movlw	'.'
	call	MIOS_LCD_PrintChar
	movf	LEDDIGITS_3, W
	call	PRINT_MIDIClk_Hlp
	movf	LEDDIGITS_2, W
	call	MIOS_LCD_PrintBCD1
	movlw	'.'
	call	MIOS_LCD_PrintChar
	movf	LEDDIGITS_1, W
	call	PRINT_MIDIClk_Hlp
	movf	LEDDIGITS_0, W
	goto	MIOS_LCD_PrintBCD1


;; this help function prints a space if bit 4 set, else the BCD
PRINT_MIDIClk_Hlp
	BRA_IFSET WREG, 4, ACCESS, PRINT_MIDIClk_Hlp_Spc
PRINT_MIDIClk_Hlp_BCD
	goto	MIOS_LCD_PrintBCD1
PRINT_MIDIClk_Hlp_Spc
	movlw	' '
	goto	MIOS_LCD_PrintChar


;; --------------------------------------------------------------------------
;; this function converts MIDICLK_* to LEDDIGITS_*
;; --------------------------------------------------------------------------
CONVERT_MIDIClk	
	;; convert beats (16-bit value, so we have to use MIOS_HLP_Dec2BCD)
	SET_BSR	MIDICLK_BEAT_MSB
	movff	MIDICLK_BEAT_MSB, MIOS_PARAMETER1
	incf	MIDICLK_BEAT_LSB, W, BANKED	; (beat + 1)
	skpnz
	incf	MIOS_PARAMETER1, F	
	call	MIOS_HLP_Dec2BCD
	;; copy result to LEDDIGITS_[7654]

	;; 8th digit
	swapf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_7

	;; 7th digit
	movf	MIOS_PARAMETER2, W
	andlw	0x0f
	movwf	LEDDIGITS_6

	;; 6th digit
	swapf	MIOS_PARAMETER1, W
	andlw	0x0f
	movwf	LEDDIGITS_5

	;; 5th digit
	movf	MIOS_PARAMETER1, W
	andlw	0x0f
	iorlw	0x80		; set dot
	movwf	LEDDIGITS_4


	;; blank unused beat digits
	movf	LEDDIGITS_7, W
	bnz	CONVERT_MIDIClk_Blank_End
	movlw	0x10
	movwf	LEDDIGITS_7
	
	movf	LEDDIGITS_6, W
	bnz	CONVERT_MIDIClk_Blank_End
	movlw	0x10
	movwf	LEDDIGITS_6
	
	movf	LEDDIGITS_5, W
	bnz	CONVERT_MIDIClk_Blank_End
	movlw	0x10
	movwf	LEDDIGITS_5
CONVERT_MIDIClk_Blank_End

	;; blank 4th digit
	movlw	0x10
	movwf	LEDDIGITS_3

	;; copy quarter note counter + 1 to 3rd digit
	SET_BSR	MIDICLK_QN
	incf	MIDICLK_QN, W, BANKED
	andlw	0x0f
	iorlw	0x80		; set dot
	movwf	LEDDIGITS_2

	;; the last digits: use fast BCD routine
	incf	MIDICLK_CTR, W, BANKED
	rcall	CONVERT_Bin2BCD	; result in MSD and LSD
	movf	MSD, W
	;; blank 2nd digit if 0
	skpnz
	movlw	0x10
	movwf	LEDDIGITS_1
	movff	LSD, LEDDIGITS_0

	;; thats all
	return

;
;********************************************************************
; From the Microchip AN526 Application Note
;
; Binary To BCD Conversion Routine
;
; This routine converts the 8 bit binary number in the W Register
; to a 2 digit BCD number.
; The least significant digit is returned in location LSD and
; the most significant digit is returned in location MSD.
;
; Performance :
; Program Memory : 10
; Clock Cycles : 81 (worst case when W = 63 Hex )
; ( i.e max Decimal number 99 )
;
; Program: BIN8BCD.ASM
; Revision Date:
; 1-13-97 Compatibility with MPASMWIN 1.40
;
;*******************************************************************
;
LSD	EQU	TMP1
MSD	EQU	TMP2
CONVERT_Bin2BCD 
	clrf	MSD
	movwf	LSD
CONVERT_Bin2BCD_Loop
	movlw	10
	subwf	LSD, W
	bnc	CONVERT_Bin2BCD_End
	movwf	LSD
	incf	MSD, F
	rgoto	CONVERT_Bin2BCD_Loop
CONVERT_Bin2BCD_End
	return

	END
