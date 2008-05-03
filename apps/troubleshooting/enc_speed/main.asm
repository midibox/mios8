; $Id$
;
; ENC Speed Test
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; ---[ MIOS header file ]---
#include "mios.h"

;; ---[ useful macros ]---
#include "macros.h"

;; ---[ vectors to MIOS functions (never change!) ]---
#include "mios_vectors.inc"

;; ---[ user hooks (never change!) ]---
#include "user_vectors.inc"

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include "app_lcd.inc"

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

	;; enable auto repeat for Inc/Dec button
	movlw	PIN_NUMBER_DEC_BUTTON
	call	MIOS_DIN_PinAutoRepeatEnable
	movlw	PIN_NUMBER_INC_BUTTON
	call	MIOS_DIN_PinAutoRepeatEnable

	;; clear speed setting variable and init encoder mode
	clrf	SPEED_SETTING
	call	ENC_SPEED_TABLE_InitEnc

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
TEXT_WELCOME_0	STRING 16, 0x00, "Mode: xxxxxxxxxx"
TEXT_WELCOME_1	STRING 16, 0x40, "Inc:xx Val:xxxxx"

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

	;; print encoder mode
	movlw	0x06
	call	MIOS_LCD_CursorSet
	call	ENC_SPEED_TABLE_PrintMode

	;; print INC value
	movlw	0x44
	call	MIOS_LCD_CursorSet
	movf	LAST_INC, W
	call	MIOS_LCD_PrintHex2

	;; print 16-bit value as BCD
	movlw	0x4b
	call	MIOS_LCD_CursorSet
	movff	ENC_VALUE_16BIT_H, MIOS_PARAMETER1
	movf	ENC_VALUE_16BIT_L, W
	call	MIOS_LCD_PrintBCD5

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
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; branch depending on Inc/Dec button
	movf	MIOS_PARAMETER1, W
	xorlw	PIN_NUMBER_DEC_BUTTON
	bz	DIN_Dec_Button

	movf	MIOS_PARAMETER1, W
	xorlw	PIN_NUMBER_INC_BUTTON
	bz	DIN_Inc_Button

	;; else do nothing
	return

;; ---
DIN_Dec_Button

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
	movwf	TMP3

	;; now we have to add (or subtract) the incrementer from the absolute value,
	;; which has been stored in SPEED_SETTING

	;; copy speed setting value to TMP1 and clear high-byte
	movff	SPEED_SETTING, TMP1
	clrf	TMP2

	;; the simplest solution is the use of MIOS_HLP_16bitAddSaturate, which
	;; can handle with values up to 16 bit!!! :-)
	;; please refer to the function documentation for the input/output parameters!

	;; set pointer to TMP1
	;; (it is located to an even address, thats important when this function is used!)
	;; (and since we are modifying a 16-bit value, the next address to the low-byte
	;; is used as high-byte -> see app_defines.h)
	lfsr	FSR1, TMP1
	;; set max value
	movlw	ENC_SPEED_TABLE_ENTRIES-1 ; low-byte of max value
	movwf	MIOS_PARAMETER1
	movlw	0x00	; high-byte of max value
	movwf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in TMP3)
	movf	TMP3, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = ENC_VALUE_16BIT_L contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; now copy TMP1 to SPEED_SETTING
	movff	TMP1, SPEED_SETTING

	;; init new speed mode
	call	ENC_SPEED_TABLE_InitEnc

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
	;; the encoder has been moved.
	;; increment/decrement the ENC_VALUE_16BIT_L

	;; don't continue if encoder number != 0x00
	xorlw	0
	skpz
	return

	;; save incrementer in LAST_INC
	movff	MIOS_PARAMETER2, LAST_INC

	;; now we have to add (or subtract) the incrementer from the absolute value,
	;; which has been stored in ENC_VALUE_16BIT_L

	;; the simplest solution is the use of MIOS_HLP_16bitAddSaturate, which
	;; can handle with values up to 16 bit!!! :-)
	;; please refer to the function documentation for the input/output parameters!

	;; set pointer to ENC_VALUE_16BIT_L
	;; (it is located to an even address, thats important when this function is used!)
	;; (and since we are modifying a 16-bit value, the next address to the low-byte
	;; is used as high-byte -> see app_defines.h)
	lfsr	FSR1, ENC_VALUE_16BIT_L
	;; set max value
	movlw	0xff	; low-byte of max value
	movwf	MIOS_PARAMETER1
	movlw	0x7f	; high-byte of max value
	movwf	MIOS_PARAMETER2
	;; get incrementer (which has been stored in ENC_INC)
	movf	LAST_INC, W
	;; call routine
	call	MIOS_HLP_16bitAddSaturate
	;; now [FSR1] = INDF1 = ENC_VALUE_16BIT_L contains the result
	;; MIOS_PARAMETER1[0] is set when value has been changed

	;; exit routine if value has not been changed
	btfss	MIOS_PARAMETER1, 0
	return

	;; request a display update by setting DISPLAY_UPDATE_REQ,0
	;; USER_DISPLAY_Tick polls this flag!
	bsf	DISPLAY_UPDATE_REQ, 0

	;; thats all
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
;;  This table contains all selectable encoder speed settings
;;     -> USER_DISPLAY_Tick uses it to print the appr. string on LCD
;;     -> DIN_IncDec uses it to configure the speed setting
;; --------------------------------------------------------------------------
ENC_SPEED_TABLE_ENTRY_LEN	EQU	12	; 12 bytes per entry
ENC_SPEED_TABLE_ENTRIES		EQU	17	; 17 entries
ENC_SPEED_TABLE
	;;      speed mode           parameter   <--------> string
	db	MIOS_ENC_SPEED_SLOW,         0, "Slow (0)  "
	db	MIOS_ENC_SPEED_SLOW,         1, "Slow (1)  "
	db	MIOS_ENC_SPEED_SLOW,         2, "Slow (2)  "
	db	MIOS_ENC_SPEED_SLOW,         3, "Slow (3)  "
	db	MIOS_ENC_SPEED_SLOW,         4, "Slow (4)  "
	db	MIOS_ENC_SPEED_SLOW,         5, "Slow (5)  "
	db	MIOS_ENC_SPEED_SLOW,         6, "Slow (6)  "
	db	MIOS_ENC_SPEED_SLOW,         7, "Slow (7)  "
	db	MIOS_ENC_SPEED_NORMAL,       0, "Normal    "
	db	MIOS_ENC_SPEED_FAST,         0, "Fast (0)  "
	db	MIOS_ENC_SPEED_FAST,         1, "Fast (1)  "
	db	MIOS_ENC_SPEED_FAST,         2, "Fast (2)  "
	db	MIOS_ENC_SPEED_FAST,         3, "Fast (3)  "
	db	MIOS_ENC_SPEED_FAST,         4, "Fast (4)  "
	db	MIOS_ENC_SPEED_FAST,         5, "Fast (5)  "
	db	MIOS_ENC_SPEED_FAST,         6, "Fast (6)  "
	db	MIOS_ENC_SPEED_FAST,         7, "Fast (7)  "

	;; init encoder mode
ENC_SPEED_TABLE_InitEnc
	TABLE_ADDR ENC_SPEED_TABLE		; base pointer to table
	movf	SPEED_SETTING, W		; get speed index
	mullw	ENC_SPEED_TABLE_ENTRY_LEN	; multiply with entry len
	movf	PRODL, W			; add mutliplication result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	tblrd*+					; read first entry
	movff	TABLAT, MIOS_PARAMETER1		; copy it to MIOS_PARAMETER1
	movf	TABLAT, W			
	tblrd*+					; read next entry
	movff	TABLAT, MIOS_PARAMETER2		; copy it to MIOS_PARAMETER2
	movlw	0x00				; modify encoder #0
	goto	MIOS_ENC_SpeedSet		; and exit...

	;; print encoder mode
ENC_SPEED_TABLE_PrintMode
	TABLE_ADDR ENC_SPEED_TABLE+2		; base pointer to table plus 2
	movf	SPEED_SETTING, W		; get speed index
	mullw	ENC_SPEED_TABLE_ENTRY_LEN	; multiply with entry len
	movf	PRODL, W			; add mutliplication result to table pointer
	addwf	TBLPTRL, F
	movf	PRODH, W
	addwfc	TBLPTRH, F
	movlw	10				; print string (10 characters) and exit
	goto	MIOS_LCD_PrintPreconfString

	END
