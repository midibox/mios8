; $Id$
;
; Using J5 as DIN
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
;;  Including and configure J5_IO driver from $MIOS_PATH/modules/j5_io
;; ==========================================================================

;; if set, input functions won't be provided by the driver
#define J5_IO_DONT_USE_INPUT_FUNCTIONS  0

;; if set, output functions won't be provided by the driver
#define J5_IO_DONT_USE_OUTPUT_FUNCTIONS 1
#include <j5_io.inc>


;; and now the local j5_din handler
#include "j5_din.inc"

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

	;; initialize J5 as digitial input
	call	J5_DIN_Init

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; call the J5_IO_DIN_Handler function which reacts on a changed pin
	;; status of J5
	call	J5_DIN_Handler
	
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
TEXT_WELCOME_0	STRING 16, 0x00, "J5_DIN Example  "
TEXT_WELCOME_1	STRING 16, 0x40, "Last Pin: - -> -"

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
#if 0
        ;; for debugging
        movlw   0x00
        call    MIOS_LCD_CursorSet
        movf    J5_DIN_STATUS, W
        call    MIOS_LCD_PrintHex2
        movf    J5_DIN_STATUS_CHANGED, W
        call    MIOS_LCD_PrintHex2
#endif

	;; leave routine if update has not been requested
	btfss	DISPLAY_UPDATE_REQ, 0
	return

	;; clear the request flag so that this routine will not be processed
	;; until the flag has been set again
	bcf	DISPLAY_UPDATE_REQ, 0

	;; print last pin number at second line, pos #10
	movlw	0x4a
	call	MIOS_LCD_CursorSet
	movf	LAST_PIN, W
	call	MIOS_LCD_PrintBCD1

	;; print pin value at second line, pos #15
	movlw	0x4f
	call	MIOS_LCD_CursorSet
	movf	LAST_PIN_VALUE, W
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
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish

	;; call the J5_DIN_Update function to copy the new pin status of J5
	;; into J5_DIN_STATUS
	call	J5_DIN_Update

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
;;  This function is NOT called by MIOS, but by the J5_DIN handler when a
;;  pin at J5 has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_J5_DIN_NotifyToggle
	;; store last pin number
	movwf	LAST_PIN
	;; store last pin value
	movff	MIOS_PARAMETER2, LAST_PIN_VALUE
	;; request a display update (handled by USER_LCD_Tick)
	bsf	DISPLAY_UPDATE_REQ, 0

	;; select MIDI event table depending on pin status
	BRA_IFSET LAST_PIN_VALUE, 0, ACCESS, USER_J5_DIN_NotifyToggle_Off
USER_J5_DIN_NotifyToggle_On
	TABLE_ADDR MIDI_EVENT_TABLE_ON_EVENT
	rgoto	USER_J5_DIN_NotifyToggle_Cont
USER_J5_DIN_NotifyToggle_Off
	TABLE_ADDR MIDI_EVENT_TABLE_OFF_EVENT
	
USER_J5_DIN_NotifyToggle_Cont
	;; select table entry depending on pin number
	movf	LAST_PIN, W
	mullw	MIDI_EVENT_TABLE_ENTRY_LEN; (multiply with table len)
	movf	PRODL, W	; add result to table address
	TABLE_ADD_W

	;; read first byte from table entry
	tblrd*+

	;; don't send when first byte is zero
	movf	TABLAT, W
	bz	USER_J5_DIN_NotifyToggle_SkpAll

	;; begin stream
	call	MIOS_MIDI_BeginStream

	;; send byte
	movf	TABLAT, W
	movwf	TMP1		; save the first byte for later
	call	MIOS_MIDI_TxBufferPut

	;; read second byte from table entry
	tblrd*+
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut

	;; read third byte from table entry
	tblrd*+

	;; special case for third byte: send only if no program change and no aftertouch event!
	movf	TMP1, W
	andlw	0xf0		; (filter out channel)
	xorlw	0xc0		; (compare with program change status byte)
	bz	USER_J5_DIN_NotifyToggle_Skp3
	movf	TMP1, W
	andlw	0xf0		; (filter out channel)
	xorlw	0xd0		; (compare with aftertouch status byte)
	bz	USER_J5_DIN_NotifyToggle_Skp3
	;; ok, send third byte
	movf	TABLAT, W
	call	MIOS_MIDI_TxBufferPut
USER_J5_DIN_NotifyToggle_Skp3
	call	MIOS_MIDI_EndStream
USER_J5_DIN_NotifyToggle_SkpAll

	;; thats all
	return


	;; NOTE: due to a bug in the microchip assembler, every entry
	;; which consists of 3 bytes will be filled to 4 bytes!
MIDI_EVENT_TABLE_ENTRY_LEN EQU 4

	;; event triggered by a button at J5 when it is pressed
MIDI_EVENT_TABLE_ON_EVENT
	db	0x90, 0x36, 0x7f	; J5.0: C-4, Channel #1, max velocity
	db	0x90, 0x37, 0x7f	; J5.1: C#4, Channel #1, max velocity
	db	0x90, 0x38, 0x7f	; J5.2: D-4, Channel #1, max velocity
	db	0x90, 0x39, 0x7f	; J5.3: D#4, Channel #1, max velocity
	db	0x90, 0x3a, 0x7f	; J5.4: E-4, Channel #1, max velocity
	db	0x90, 0x3b, 0x7f	; J5.5: F-4, Channel #1, max velocity
	db	0xb0, 0x40, 0x7f	; J5.6: Sustain Pedat Controller, Channel #1
	db	0xc0, 0x00, 0x00	; J5.7: Program Change to 00, Channel #1

	;; event triggered by a button at J5 when it is depressed
MIDI_EVENT_TABLE_OFF_EVENT
	db	0x90, 0x36, 0x00	; J5.0: C-4, Channel #1, no velcity (Note Off)
	db	0x90, 0x37, 0x00	; J5.1: C#4, Channel #1, no velcity (Note Off)
	db	0x90, 0x38, 0x00	; J5.2: D-4, Channel #1, no velcity (Note Off)
	db	0x90, 0x39, 0x00	; J5.3: D#4, Channel #1, no velcity (Note Off)
	db	0x90, 0x3a, 0x00	; J5.4: E-4, Channel #1, no velcity (Note Off)
	db	0x90, 0x3b, 0x00	; J5.5: F-4, Channel #1, no velcity (Note Off)
	db	0xb0, 0x40, 0x00	; J5.6: Sustain Pedat Controller, Channel #1
	db	0x00, 0x00, 0x00	; J5.7: no event

	END
