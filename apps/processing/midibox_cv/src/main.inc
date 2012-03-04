; $Id$
;
; MIOS Application
; MIDIbox CV
;
MBCV_VERSION_STR MACRO
; Version:                 <-------------->
	STRING	16, 0x00, "MIDIbox CV V1.3 "
	ENDM
; (fixed string length - 16 characters!)
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
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
;;  Control surface settings
;; ==========================================================================

;; ---[ variables used by application ]---
#include "app_defines.h"

; ==========================================================================

;; ---[ configuration table for MIDI processor and rotary encoders ]---
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include <app_lcd.inc>

;; ==========================================================================
;;  Standard MIOS hooks
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize application specific variables
	clrf	MB_STAT

	;; restore dump data
	call	CV_DUMP_RestoreAll

	;; init control surface
	call	CS_MENU_Init

	;; ------------------------------------------------------------------

	;; initialize the SRIO driver
	movlw	0x01
	call	MIOS_SRIO_NumberSet
	movlw	0x01				; set update frequency to 1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR	; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; initialize the AOUT module
	call	AOUT_Init

	;; initialize J5 for driving LEDs if enabled
#if ENABLE_J5
	call	J5_DOUT_Init
#endif

	;; initialize device ID
	call	MIOS_MIDI_DeviceIDGet
	movwf	MIDI_DEVICE_ID

	;; init clock output handler
	call	CV_CLK_Init

	;; ------------------------------------------------------------------

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; map the internal CV voices to the AOUT channels
	call	CV_MAP_Update

	;; update external ports (AOUT/gate pins)
	call	CV_EXT_Update

	;; ---[ check BankStick status ]---
 	call	CV_BANK_CheckStick

	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; call clock timer function
	call	CV_CLK_Timer

	;; call interpolation handler
	call	CV_IP_Handler

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
	;; request an initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ
	;; continue at the control surface menu handler
	goto	CS_MENU_Handler

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; continue at the control surface menu handler
	goto	CS_MENU_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; process MIDI event
	call	CV_MIDI_NotifyReceivedEvent

	;; for best latency: branch to USER_Tick so that the new CV values
	;; will be mapped immediately
	rgoto	USER_Tick
	


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
USER_MPROC_TO_STR_0	STRING 16, 0x00, " MIDI Time Out! "
USER_MPROC_TO_STR_1	STRING 16, 0x40, "  Whats up ???  "
USER_MPROC_NotifyTimeout
	;; print timeout message
	TABLE_ADDR USER_MPROC_TO_STR_0
	call	MIOS_LCD_PrintMessage
	call	MIOS_LCD_PrintMessage

	;; request display update
	bsf	CS_STAT, CS_STAT_DISPLAY_UPDATE_REQ
	
	;; -> jump to "ActionInvalid" for a proper reset of the sysex parser
	goto	CV_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> continue at CV sysex parser
	goto	CV_SYSEX_Parser

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
	;; forward event to MIDI Clock parser
	goto	CV_CLK_NotifyRx


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	;; check for control surface buttons - the CS handler will jump
	;; back if the button has not been assigned to a CS function
	goto	CS_MENU_BUTTON_Handler
CS_MENU_BUTTON_Handler_Return
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
#if DEFAULT_ENC_DATAWHEEL >= 0
	;; branch to CS_MENU_Enc if datawheel
	movlw	DEFAULT_ENC_DATAWHEEL
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto USER_ENC_NotifyChange_NoWheel
USER_ENC_NotifyChange_Wheel
	;; expects incrementer in WREG
	movf	MIOS_PARAMETER2, W
	goto	CS_MENU_Enc
USER_ENC_NotifyChange_NoWheel
#endif

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
	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	goto	CS_MENU_TIMER


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

;; ---[ modules from code library ]---
#include <aout.inc>		; located in $MIOS_PATH/modules/aout/

;; ---[ reusable utility functions ]---
#include "midi_evnt.inc"
#include "math_mul16_16.inc"
#include "j5_dout.inc"

;; ---[ Control surface functions ]---
;; ---[ NOTE: these are not the same files like used in MB64, MB64E, MBMF! ]---
#include "cs_menu_buttons.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu.inc"
#include "cs_menu_tables.inc"
#include "cs_special_chars.inc"

;; ---[ Menu pages handlers ]---
#include "cs_m_hlp.inc"
#include "cs_m_display.inc"
#include "cs_m_event.inc"
#include "cs_m_transp.inc"
#include "cs_m_split.inc"
#include "cs_m_pitchr.inc"
#include "cs_m_noteh.inc"
#include "cs_m_gatep.inc"
#include "cs_m_ccnum.inc"
#include "cs_m_curve.inc"
#include "cs_m_ip.inc"
#include "cs_m_clk.inc"
#include "cs_m_midicfg.inc"
#include "cs_m_sysex.inc"
#include "cs_m_calib.inc"

;; ---[ MBCV kernel ]---
#include "cv_bank.inc"
#include "cv_lcd.inc"
#include "cv_msg.inc"
#include "cv_dump.inc"
#include "cv_midi.inc"
#include "cv_sysex.inc"
#include "cv_map.inc"
#include "cv_ext.inc"
#include "cv_ip.inc"
#include "cv_clk.inc"

;; ---[ Output Curve Table (Curve 0 is always linear (1V/Oct) ]---
#include "cv_curve1_table.inc"
#include "cv_curve2_table.inc"
#include "cv_curve3_table.inc"

;; ---[ MBCV EEPROM content ]---
#include "cv_presets.inc"

	END
