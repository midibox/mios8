; $Id$
;
; MIOS Application
; MIDIbox SID
;
; -> see README.txt for details
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Version:                <-------------------->
#define MBSID_VERSION_STR "MIDIboxSID V1.7303b "
; (fixed string length - 20 characters!)
;

;; ---[ MIOS header file ]---
#include <mios.h>

;; ---[ useful macros ]---
#include <macros.h>

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

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
	;; initialize application specific variables
	clrf	SID_STAT
	clrf	SID_SYSEX_STATE
	clrf	SID_SYSEX_ACTION
	setf	SID_BANKSTICK_CHK_CTR	; (we have to start with BS0 in SID_BANK_CheckStick)

        ;; Initialize Clock Generator (the PWM unit is used to generate a 1MHz clock with 1:1 duty cycle)
        movlw   0x09
        movwf   PR2
	movlw	0x24
	movwf	T2CON
        movlw   0x0f
        movwf   CCP1CON
	movlw	0x04
        movwf   CCPR1L

	;; initialize the timer for the SIDSW handler
	movlw	8190 & 0xff		; every 819 us (in order to be timing accurate with the PIC16F version!)
	movwf	MIOS_PARAMETER1
	movlw	8190 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00			; prescaler 1:1
	call	MIOS_TIMER_Init

	;; initialize the MBHP_SID module
	call	SID_SR_Init

	;; init patch
	clrf	SID_PATCH
	clrf	SID_BANK
	call	SID_PATCH_Init

	;; initialize the shift registers
	movlw	0x09			; use up to 9 shift registers (maximum would be 16!)
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy
	call	MIOS_SRIO_UpdateFrqSet
	movlw	0x00			; disable touch sensor
	call	MIOS_SRIO_TS_SensitivitySet

	;; initialize J5 for driving LEDs if enabled
#if ENABLE_J5
	movlw	0x00
	call	J5_IO_Init
#endif

#if ENABLE_AIN_LFO_WAVEFORM
	movlw	0x06			; enable the first 6 analog inputs
	call	MIOS_AIN_NumberSet
	call	MIOS_AIN_UnMuxed	; not multiplexed
	movlw	0x03			; 8 bit resolution
#endif

	;; initialize the CBM8580 filter switch if enabled
#if DEFAULT_FILTER_TYPE_SWITCH
	bsf	DEFAULT_FILTER_TYPE_SWITCH_TRIS, DEFAULT_FILTER_TYPE_SWITCH_PIN
#endif

	;; initialize the AOUT module
	call	AOUT_Init

	;; initialize the integrated MIDI merger
	movlw	MIOS_MIDI_MERGER_DISABLED	; should be disabled for a synthesizer
	call	MIOS_MIDI_MergerSet

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
	call	SID_BANK_CheckStick

	;; ---[ update AOUTs channels (if enabled) ]---
	IRQ_DISABLE
	call	AOUT_Update
	IRQ_ENABLE

	;; ---[ call SID shift register handler ]---
	call	SID_SR_Handler

	;; ---[ handle with CC dump reuqests ]---
	call	SID_CCOUT_Handler

	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer

	;; ---[ call Software SID Handler ]---
	call	SIDSW_Handler

#if CS_ENABLED

#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	USER_TimerNoCS
#endif

	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	call	CS_MENU_TIMER
USER_TimerNoCS
#endif

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
TEXT_WELCOME_0	STRING 20, 0x00, MBSID_VERSION_STR
TEXT_WELCOME_10	STRING 16, 0x40, "Launching CS    "
TEXT_WELCOME_11	STRING 16, 0x40, "CS not enabled! "

USER_DISPLAY_Init
	TABLE_ADDR TEXT_WELCOME_0		; print welcome message

#if CS_ENABLED
	;; print welcome message only during startup phase
	movf	CS_MENU_REINIT_CFG_CTR, W
	bz	USER_DISPLAY_Init_NoStartup

	call	MIOS_LCD_PrintString		; first line

	TABLE_ADDR TEXT_WELCOME_10
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W
	bz	USER_DISPLAY_Init_NotDis
USER_DISPLAY_Init_Dis
	TABLE_ADDR TEXT_WELCOME_11
USER_DISPLAY_Init_NotDis
#endif
	call	MIOS_LCD_PrintString		; second line
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
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; exit if device ID != 0x00
	skpz
	return
#endif
	;; continue at the control surface menu handler
	call	CS_MENU_Handler
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
	;; branch to ReceiveEvent function of SID synth
	goto	SID_MIDI_NotifyReceivedEvent


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
	;; -> jump to "ActionInvalid" for a proper reset of the sysex parser
	goto	SID_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> continue at SID_MPROC_SysExCheck
	goto	SID_SYSEX_SysExCheck
	
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
	;; notify Tx event to MIDI_RXTX handler
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
	;; temporary save received byte in IRQ_TMP1
	movwf	IRQ_TMP1

	;; if MIDI clock: notify clock
	lfsr	FSR2, SID_MIDI_SYNC
	xorlw	0xf8
	skpnz
	bsf	INDF2, SID_MIDI_SYNC_F8

	;; if MIDI start: notify start
	movf	IRQ_TMP1, W
	xorlw	0xfa
	skpnz
	bsf	INDF2, SID_MIDI_SYNC_FA

	;; notify Rx event to MIDI_RXTX handler
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
#if CS_ENABLED

#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	USER_DIN_NotifyToggleNoCS
#endif
	;; continue in cs_menu_button.inc
	goto	CS_MENU_BUTTON_Handler
#endif

USER_DIN_NotifyToggleNoCS
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
#if CS_ENABLED == 0
	return
#else

#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W	; exit if device ID != 0x00
	skpz
	return
#endif

#if CS_MENU_USE_INCDEC_BUTTONS == 0
	;; if encoder #0 has been moved, jump to Control Surface Menu Encoder Handler
	movlw	0x00		; encoder #0
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto USER_ENC_Handler_NoMenu

	;; get incrementer and jump to control surface menu encoder handler
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_ENC_Handler

USER_ENC_Handler_NoMenu
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
#if CS_ENABLED == 1
	;; handle with Modulation matrix on every update cycle if enabled
#if AUTO_CS_ENABLED
	movf	SID_MIDI_DEVICE, W		; skip if device ID != 0x00
	bnz	USER_SR_Service_PrepareNoCS
#endif
#if MOD_MATRIX_ENABLE == 1
	call	CS_MENU_MATRIX_Handler
#endif
USER_SR_Service_PrepareNoCS
#endif
	;; call MIDI RxTx handler and exit
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

#if CS_ENABLED == 1
;; ---[ Control surface functions ]---
#include "cs_menu_io_tables.inc"
#include "cs_menu_enc_table.inc"
#include "cs_menu_buttons.inc"
#include "cs_menu_leds.inc"
#include "cs_menu_matrix.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu_exec.inc"
#include "cs_menu.inc"
#include "cs_menu_p2r_r2p.inc"
#include "cs_menu_print.inc"
#include "cs_menu_ms.inc"
#include "cs_menu_bank.inc"
#include "cs_menu_tables.inc"
#include "cs_menu_cc_table.inc"
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

#define J5_IO_DONT_USE_INPUT_FUNCTIONS 1
#include <j5_io.inc>

;; ---[ reusable functions ]---
#include "math_mul16_16.inc"
#include "midi_rxtx.inc"
#include "special_characters.inc"

;; ---[ SID kernel ]---
#include "sid_lcd.inc"
#include "sid_patch.inc"
#include "sid_bank.inc"
#include "sid_tune.inc"
#include "sid_sr.inc"
#include "sid_sw.inc"
#include "sid_wt.inc"
#include "sid_midi.inc"
#include "sid_sysex.inc"
#include "sid_ccin.inc"
#include "sid_ccout.inc"
#include "sid_cc_table.inc"
#include "sid_sysex_table.inc"
#include "sid_env_table.inc"
#include "sid_lfo_table.inc"
#include "sid_note_table.inc"

;; these three tables are now included from the mios_tables.inc file
;; to save code space
	;; #include "sid_frq_table.inc"
	;; #include "sid_sin_table.inc"
	;; #include "sid_depth_table.inc"

;; ---[ SID EEPROM content ]---
#include "sid_presets.inc"

	END
