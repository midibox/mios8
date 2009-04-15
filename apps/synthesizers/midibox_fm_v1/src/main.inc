; $Id$
;
; MIOS Application
; MIDIbox FM
;
; Version:                <-------------->
#define MBFM_VERSION_STR "MIDIbox FM V1.1d"
; (fixed string length - 16 characters!)
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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

;; ==========================================================================
;;  General Settings
;; ==========================================================================

#ifndef separate_SETUP_FILE
#endif

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
	;; initialize OPL3 module
	call	MBFM_REG_Init

	;; ------------------------------------------------------------------

	;; initialize the shift registers
	movlw	0x04			; use up to 4 shift registers (maximum would be 16!)
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy
	call	MIOS_SRIO_UpdateFrqSet
	movlw	0x00			; disable touch sensor
	call	MIOS_SRIO_TS_SensitivitySet

	;; initialize the AOUT module
	call	AOUT_Init

	;; initialize the integrated MIDI merger
	movlw	MIOS_MIDI_MERGER_DISABLED	; should be disabled for a synthesizer
	call	MIOS_MIDI_MergerSet

	;; ------------------------------------------------------------------

	;; set MIDI channel of drum section (won't be saved in EEPROM/BankStick)
	movlw	0x0f
	movff	WREG, MBFM_DRUM_MIDI_CHANNEL

	;; init ensemble and drumset
	call	MBFM_ENS_Init

	;; init voices
	call	MBFM_VOICE_Init

	;; ------------------------------------------------------------------

#if CS_ENABLED
	;; reset the control surface
	goto	CS_MENU_Reset
#else
	return
#endif


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick

	;; ---[ check BankStick status ]---
	call	MBFM_BANK_CheckStick

	;; ---[ update the FM registers ]---
	call	MBFM_REG_Update

	;; ---[ call the wavetable handler ]---
	call	MBFM_WT_Handler

	;; ---[ update AOUTs channels (if enabled) ]---
	call	MBFM_AOUT_Update

	;; ---[ for performance measuring ]---
	call	MBFM_LOAD_IncLoadCtr

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

TEXT_WELCOME_0	STRING 16, 0x00, MBFM_VERSION_STR
TEXT_WELCOME_10	STRING 16, 0x40, "Launching CS    "
TEXT_WELCOME_11	STRING 16, 0x40, "CS not enabled! "

USER_DISPLAY_Init
	TABLE_ADDR TEXT_WELCOME_0		; print welcome message

#if CS_ENABLED
	;; print welcome message only during startup phase
	BRA_IFSET CS_STAT, CS_STAT_STARTUP_DONE, ACCESS, USER_DISPLAY_Init_NoStartup
USER_DISPLAY_Init_Startup
	bsf	CS_STAT, CS_STAT_STARTUP_DONE
	call	MIOS_LCD_PrintMessage		; first line

	TABLE_ADDR TEXT_WELCOME_10
	call	MIOS_LCD_PrintMessage		; second line
USER_DISPLAY_Init_NoStartup

	;; request an initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; in the next cycle USER_DISPLAY_Tick will be called which
	;; does the job
	return

#else
	call	MIOS_LCD_PrintString		; first line
	TABLE_ADDR TEXT_WELCOME_11
	call	MIOS_LCD_PrintString		; second line
	return
#endif

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
#if CS_ENABLED == 1
	;; continue at the control surface menu handler
	call	CS_MENU_Handler
#endif

#if DISPLAY_PERFORMANCE == 1
	movlw	40-1-6
	call	MIOS_LCD_CursorSet
	call	MBFM_LOAD_Print
#endif

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent	
	;; process MIDI event
	goto	MBFM_MIDI_NotifyReceivedEvent


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
	goto	MBFM_SYSEX_ActionInvalid


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	goto	MBFM_SYSEX_Parser

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
	;; temporary save received byte in IRQ_TMP1
	movwf	IRQ_TMP1

	;; if MIDI clock: notify clock
	lfsr	FSR2, MBFM_MIDI_SYNC
	xorlw	0xf8
	skpnz
	bsf	INDF2, MBFM_MIDI_SYNC_F8

	;; if MIDI start: notify start
	movf	IRQ_TMP1, W
	xorlw	0xfa
	skpnz
	bsf	INDF2, MBFM_MIDI_SYNC_FA

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
#if CS_ENABLED
	;; continue in cs_menu_button.inc
	goto	CS_MENU_BUTTON_Handler
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
#if CS_ENABLED == 0
	return
#else

#if CS_MENU_USE_INCDEC_BUTTONS == 0
	;; if encoder #0 has been moved, jump to Control Surface Menu Encoder Handler
	movlw	0x00		; encoder #0
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto USER_ENC_NotifyChange_NoMenu

	;; get incrementer and jump to control surface menu encoder handler
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_ENC_Handler

USER_ENC_NotifyChange_NoMenu
	decf	MIOS_PARAMETER1, F; decrement encoder number (the CS encoders begin at 0)
#endif
	;; jump to CS handler
	goto	CS_MENU_ENC_CS_Handler
#endif

;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare

	;; the synth engine handler is hooked here
	;; since updates are made each millisecond (which is also the update
	;; cycle of the shift register chains)

	;; ---[ call Synth Engine Handler ]---
	call	MBFM_SE_Handler

#if CS_ENABLED
	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	goto	CS_MENU_TIMER
USER_SR_Service_Prepare_NoCS
#else
	return
#endif


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	;; ---[ for performance measuring ]---
	call	MBFM_LOAD_IncRefCtr

	;; ---[ handler for selection matrix buttons/LEDs ]---
	goto	CS_MENU_MATRIX_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	return

#if CS_ENABLED == 1
;; ---[ Control surface functions ]---
#include "cs_menu_io_tables.inc"
#include "cs_menu_matrix_table.inc"
#include "cs_menu_buttons.inc"
#include "cs_menu_matrix.inc"
#include "cs_menu_leds.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu_exec.inc"
#include "cs_menu.inc"
#include "cs_menu_p2r_r2p.inc"
#include "cs_menu_print.inc"
#include "cs_menu_ms.inc"
#include "cs_menu_cfg.inc"
#include "cs_menu_tables.inc"
#endif

;; ---[ modules from code library ]---
; override default pin definitions of AOUT driver
#define AOUT_LAT_CS	LATC	; The chip select pin CS#
#define AOUT_TRIS_CS	TRISC	; is connected to Port C.3 
#define AOUT_PIN_CS	3	; (CANNOT be shared with other outputs!)
;
#define AOUT_LAT_DIN	LATC	; The data input pin DIN
#define AOUT_TRIS_DIN	TRISC	; is connected to Port C.1
#define AOUT_PIN_DIN	1	; (can be shared with other outputs)
;
#define AOUT_LAT_SCLK	LATC	; The shift clock input pin SCLK
#define AOUT_TRIS_SCLK	TRISC	; is connected to Port C.0
#define AOUT_PIN_SCLK	0	; (can be shared with other outputs)
; include AOUT driver (located in $MIOS_PATH/modules/aout/)
#include <aout.inc>

;; ---[ reusable functions ]---
#include "math_mul16_16.inc"
#include "special_characters.inc"

;; ---[ MIDIbox FM kernel ]---

#include "mbfm_reg.inc"
#include "mbfm_sysex.inc"
#include "mbfm_midi.inc"
#include "mbfm_aout.inc"
#include "mbfm_se.inc"
#include "mbfm_wt.inc"
#include "mbfm_bank.inc"
#include "mbfm_instr.inc"
#include "mbfm_drum.inc"
#include "mbfm_ens.inc"
#include "mbfm_voice.inc"
#include "mbfm_frq.inc"
#include "mbfm_eg_table.inc"
#include "mbfm_lfo_table.inc"
#include "mbfm_sin_table.inc"
#include "mbfm_lcd.inc"
#include "mbfm_load.inc"
#include "mbfm_note_table.inc"
#include "mbfm_vol_table.inc"
#include "mbfm_par_table.inc"
#include "mbfm_parin.inc"
#include "mbfm_parout.inc"
#include "mbfm_curve_table.inc"

;; ---[ MIDIbox FM EEPROM content ]---
#include "mbfm_presets.inc"

	END
