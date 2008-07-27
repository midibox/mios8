; $Id$
;
; LCD Performance Measuring Application
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
#include <app_defines.h>

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
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick

	;; enable these lines for CLCDs with 4-bit interface
#if 0
	;; use a CLCD, E input of first CLCD at D.7, E of second CLCD @C.4 
	;; using the 4-bit interface: 
	;; -> connect MBHP_CORE:J15:D7-D4 of the core module to D7-D4 of the LCD 
	;; -> left MBHP_CORE:J15:D3-D0 of the core module open! 
	;; -> tie D3-D0 of the LCD to ground 
	movlw   0x37 | 0x80	 ; E1: D.7, 4bit interface 
	movwf   MIOS_PARAMETER1 
	movlw   0x24 | 0x80	 ; E2: C.4, 4bit interface 
	movwf   MIOS_PARAMETER2 
	movlw   0x00  ; LCD type 0 
	call	MIOS_LCD_TypeSet  	
#endif

	;; enable these lines for T6963C and IIC display
	;; (you also have to reference the LCD driver as described under $MIOS_PATH/apps/examples/lcd7/*/asm)
#if 0
	;; select LCD type #7 (custom LCD driver, hooks in app_lcd.inc will be used)
	movlw	0x00			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER1
	movlw	0x00			; here you could forward an additional parameter
	movwf	MIOS_PARAMETER2
	movlw	0x07
	call	MIOS_LCD_TypeSet
#endif

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

	return

TEXT_WELCOME_0	STRING 11, 0x45, " *8 *100 nS"

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick

	;; do the performance measuring on every 16384th execution of this routine
	incf	WAIT_TIMER0, F
	skpnz
	incf	WAIT_TIMER1, F

	btfss	WAIT_TIMER1, 6
	return

	clrf	WAIT_TIMER0
	clrf	WAIT_TIMER1
	goto	LCD_Measure


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
;;  This function measures the performance of the connected LCD
;; --------------------------------------------------------------------------
LCD_Measure
	;; using timer3 to measure the number of cycles
	;; this timer is also used by MIOS for the USER_Timer hook, so
	;; we first should disable the interrupt here
	bcf	PIE2, TMR3IE

	;; (Note: timings could also be measured with timer2, which
	;; isn't used by MIOS - but timer2 is 8 bit only (+8 bit period)
	;; so not accurate enough for long delays)

	;; reset the timer, set prescaler to 1:8, so that we can measure
	;; delays of up to 65536 * 8 = 524288 cycles = 52.429 mS
	movlw	0x30
	movwf	T3CON

	;; reset timer registers
	clrf	TMR3L
	clrf	TMR3H

	;; clear interrupt flag - we will use it as overflow indicator
	bcf	PIR2, TMR3IF	
	

	;; set debug pin at CORE:J14 to '1' (for measurements with the scope)
	bsf	LATD, 4
	;; start measuring
	bsf	T3CON, TMR3ON

	;; benchmark definition:
	;; print 4 times 16 characters at the upper line

	clrf	TMP1		; outer loop counter
LCD_Measure_OuterLoop
	movlw	0x00		; set cursor to first line
	call	MIOS_LCD_CursorSet
	clrf	TMP2		; inner loop counter
LCD_Measure_InnerLoop
	movf	TMP2, W		; print character
	addlw	'A'
	call	MIOS_LCD_PrintChar
	incf	TMP2, F		; loop 16 times
	BRA_IFCLR TMP2, 4, ACCESS, LCD_Measure_InnerLoop

	incf	TMP1, F		; loop 4 times
	BRA_IFCLR TMP1, 2, ACCESS, LCD_Measure_OuterLoop

	;; stop measuring
	bcf	T3CON, TMR3ON
	;; set debug pin at CORE:J14 to '0'
	bcf	LATD, 4	

	;; send result via MIDI
	movlw	0xf0
	call	MIOS_MIDI_TxBufferPut
	swapf	TMR3H, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movf	TMR3H, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	swapf	TMR3L, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movf	TMR3L, W
	andlw	0x0f
	call	MIOS_MIDI_TxBufferPut
	movlw	0xf7
	call	MIOS_MIDI_TxBufferPut

	;; print result
	movlw	0x40
	call	MIOS_LCD_CursorSet

	;; branch on overflow
	BRA_IFSET PIR2, TMR3IF, ACCESS, LCD_Measure_Overflow
LCD_Measure_NoOverflow
	movff	TMR3H, MIOS_PARAMETER1
	movf	TMR3L, W
	call	MIOS_LCD_PrintBCD5
	rgoto	LCD_Measure_End

LCD_Measure_Overflow
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	movlw	'o'
	call	MIOS_LCD_PrintChar
	movlw	'o'
	call	MIOS_LCD_PrintChar
	movlw	' '
	call	MIOS_LCD_PrintChar
	;; 	rgoto	LCD_Measure_End

LCD_Measure_End
	;; and exit
	return

	END
