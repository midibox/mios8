; $Id$
;
; MIDImon
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
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
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the shift registers
	movlw	0x10			; use all shift registers
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy to 1/1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR ; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; ------------------------------------------------------------------

	;; initialize MIDICLK receiver
	call	MIDICLK_Init

	;; ------------------------------------------------------------------

	;; initialize BPM handler
	call	BPM_Init

	;; call USER_Timer every mS
	movlw	10000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	10000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_TIMER_Init

	;; ------------------------------------------------------------------

#if DEFAULT_USE_J5
	;; initialize J5 driver
	call	J5_DIN_Init
#endif

	;; ------------------------------------------------------------------

	;; initialize MTC receiver
	call	MTC_Init

	;; ------------------------------------------------------------------

	;; initialize LEDdigits driver
#if DEFAULT_LEDDIGITS_ENABLED
	call	LEDDIGITS_Init
#endif

	;; ------------------------------------------------------------------

	;; initialize application specific variables
	clrf	MB_STAT

	;; set MB_STAT_DISPLAY_GLCD depending on LCD mode
	bcf	MB_STAT, MB_STAT_DISPLAY_GLCD
	call	MIOS_LCD_TypeGet
	skpz
	bsf	MB_STAT, MB_STAT_DISPLAY_GLCD

	;; init default LCD offsets
	call	MON_LCD_SetDefaultOffsets

	;; init horizontal bars
	call	MON_LCD_InitHBars

	;; enable auto-repeat for up/down buttons
	movlw	DEFAULT_BUTTON_UP
	call	MIOS_DIN_PinAutoRepeatEnable
	movlw	DEFAULT_BUTTON_DOWN
	call	MIOS_DIN_PinAutoRepeatEnable

	;; ------------------------------------------------------------------
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
#if DEFAULT_USE_J5
	;; call J5 button handler
	call	J5_DIN_Handler
#endif

	;; call the switches handler
	call	MON_BUTTON_UpdateSwitches

	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
#if DEFAULT_USE_J5
	;; update J5 button status
	call	J5_DIN_Update
#endif

	;; call cursor handler
	call	MON_DISPLAY_Timer

	;; branch to BPM handler
	goto	BPM_Handler


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
	;; clear screen
	call	MIOS_LCD_Clear
	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; call the MIDImon display handler
	goto	MON_DISPLAY_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; continue at MIDImon MIDI handler
	goto	MON_MIDI_NotifyReceivedEvent


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
	;; call MIDIclk handler
	call	MON_CLK_NotifyReceivedByte
	;; call MTC handler
	call	MON_MTC_NotifyReceivedByte
	;; continue at MIDImon MIDI handler
	movf	MIOS_PARAMETER1, W
	goto	MON_MIDI_NotifyReceivedByte

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
	;; branch to Rx/Tx handler
	goto	MIDI_RXTX_NotifyTx

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
	;; if F8 event has been received, call BPM_Tick
	movwf	IRQ_TMP1
	xorlw	0xf8
	skpnz
	call	BPM_Tick

	;; if FA event has been received, call BPM_Init
	movf	IRQ_TMP1, W
	xorlw	0xfa
	skpnz
	call	BPM_Init
	
	;; branch to Rx/Tx handler
	goto	MIDI_RXTX_NotifyRx

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
#if DEFAULT_USE_J5 == 0
	;; branch to MIDImon button handler
	goto	MON_BUTTON_Handler
#else
	return
#endif

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
#if DEFAULT_LEDDIGITS_ENABLED
	;; update leddigits
	call	LEDDIGITS_Update
#endif
	;; branch to the Rx/Tx LED handler
	goto	MIDI_RXTX_Handler


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


;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
	
;; ---[ reusable utility functions ]---
#include "j5_din.inc"
#include "leddigits.inc"
#include "midi_evnt.inc"
#include "midi_rxtx.inc"
#include "midiclk.inc"
#include "bpm.inc"
#include "mtc.inc"
#include "convert_bin2bcd.inc"

;; ---[ MIDImon specific modules ]---
#include "mon_midi.inc"
#include "mon_mtc.inc"
#include "mon_clk.inc"
#include "mon_fsr.inc"
#include "mon_buttons.inc"
#include "mon_display.inc"
#include "mon_lcd.inc"
#include "mon_cc_names_table.inc"

;; --------------------------------------------------------------------------
;;  Including fonts from $MIOS_PATH/modules/glcd_font/
;; --------------------------------------------------------------------------
#include <glcd_font_meter_icons_h.inc>


	END
