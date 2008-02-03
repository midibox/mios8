; $Id$
;
;
; MIOS Application
; MIDIbox MF
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
	;; initialize the AIN driver
	movlw	DEFAULT_NUMBER_FADERS
	call	MIOS_AIN_NumberSet
#if DEFAULT_ENABLE_AIN_MUX
	call	MIOS_AIN_Muxed		; service multiplexer interface
#else
	call	MIOS_AIN_UnMuxed	; don't use multiplexer interface
#endif
	movlw	0x03			; set deadband to 3
	call	MIOS_AIN_DeadbandSet

#if DEFAULT_ENABLE_MOTORDRIVER
	;; initialize the MF driver
	call	MIOS_MF_Enable
	movlw	0x03			; set MF deadband to 3
	call	MIOS_MF_DeadbandSet

	movlw	DEFAULT_MF_PWM_DUTY_UP		; will be overwritten in mbmf_dump.inc
	call	MIOS_MF_PWM_DutyCycleUpSet
	movlw	DEFAULT_MF_PWM_DUTY_DOWN	; will be overwritten in mbmf_dump.inc
	call	MIOS_MF_PWM_DutyCycleDownSet
	movlw	DEFAULT_MF_PWM_PERIOD		; will be overwritten in mbmf_dump.inc
	call	MIOS_MF_PWM_PeriodSet
#endif

	;; initialize the shift registers
	movlw	0x10			; use all shift registers
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy to 1/1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR ; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; the touch sensor sensitivity is also defined in the SysEx dump (see mbmf_presets.inc)
	;; and will be set in mbmf_dump.inc
	movlw	DEFAULT_TOUCH_SENSOR_SENSITIVITY
	call	MIOS_SRIO_TS_SensitivitySet

	;; ------------------------------------------------------------------

	;; get device ID
#if DEFAULT_AUTO_DEVICE_ID == 0
	;; from EEPROM
	movlw	(EEPROM_READONLY & 0xff) + 1
	movwf	EEADR
	call	MIOS_EEPROM_Read
#else
	;; from MIOS
	call	MIOS_MIDI_DeviceIDGet
#endif
	andlw	0x07
	movff	WREG, MBMF_DEVICE_ID

	;; initialize application specific variables
	clrf	MB_STAT

	;; load values from internal dump
	call	MBMF_DUMP_RestoreAll

	;; init control surface
	call	CS_MENU_Init

	;; init default LCD offsets
	call	MBMF_LCD_SetDefaultOffsets
	;; if "snapshot at poweron" flag is set, request it here
	SET_BSR	MBMF_BASE
	bcf	MB_STAT, MB_STAT_SNAPSHOT_REQ
	btfsc	MBMF_CFG1, MBMF_CFG1_SNAPSHOT_AT_POWERON, BANKED
	bsf	MB_STAT, MB_STAT_SNAPSHOT_REQ

	;; ------------------------------------------------------------------
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; ---[ check BankStick status ]---
	call	MBMF_BANK_CheckStick

	;; ---[ if snapshot has been requested, send it here ]---
	CALL_IFSET MB_STAT, MB_STAT_SNAPSHOT_REQ, ACCESS, MBMF_PATCH_Send
	bcf	MB_STAT, MB_STAT_SNAPSHOT_REQ

	;; ---[ if fader positions should be updated, do it here ]---
	call	MBMF_FADER_CheckUpdate
	
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
	;; continue at MBMF MIDI handler
	goto	MBMF_MIDI_NotifyReceivedEvent


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
	goto	MBMF_SYSEX_ActionInvalid


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> continue at MBMF sysex parser
	goto	MBMF_SYSEX_Parser

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
#if DEFAULT_TOUCH_SENSOR_SR > 0
	;; call TS update function if touch sensor has been pressed/depressed
	rrf	MIOS_PARAMETER1, W
	rrf	WREG, W
	rrf	WREG, W
	andlw	0x0f
	xorlw	DEFAULT_TOUCH_SENSOR_SR - 1
	skpnz
	call	MBMF_TS_Handler
#endif

	;; else branch to the MBMF button handler
	;; (NOTE: this function changes MIOS_PARAMETER[12] -- if additional
	;; handlers should be called from this hook, place them before
	;; this goto instruction)
	goto	MBMF_BUTTON_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
#if DEFAULT_ENC_DATAWHEEL
	;; branch to CS_MENU_Enc if datawheel
	movlw	DEFAULT_ENC_DATAWHEEL - 1
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
	;; branch to the Rx/Tx LED handler
	goto	MIDI_RXTX_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	goto	CS_MENU_TIMER

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a Pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	;; convert 10-bit value to 7-bit value
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	bcf	MIOS_PARAMETER2, 7	; clear 8th bit

	;; now: pot number in WREG and MIOS_PARAMETER1
	;; 7-bit value in MIOS_PARAMETER2

	;; branch to fader handler
	goto	MBMF_FADER_Handler

	
;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
	
;; ---[ reusable utility functions ]---
#include "midi_evnt.inc"
#include "midi_rxtx.inc"

;; ---[ Control surface functions ]---
#include "cs_menu_buttons.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu.inc"
#include "cs_menu_tables.inc"

;; ---[ Menu pages handlers ]---
#include "cs_m_display.inc"
#include "cs_m_bank.inc"
#include "cs_m_patch.inc"
#include "cs_m_channel.inc"
#include "cs_m_learn.inc"
#include "cs_m_morph.inc"
#include "cs_m_ts.inc"
#include "cs_m_tsmode.inc"
#include "cs_m_cali.inc"
#include "cs_m_bname.inc"
#include "cs_m_event.inc"
#include "cs_m_label.inc"
#include "cs_m_copy.inc"
#include "cs_m_midicfg.inc"
#include "cs_m_sysex.inc"

;; ---[ MBMF kernel ]---
#include "mbmf_bank.inc"
#include "mbmf_sysex.inc"
#include "mbmf_fader.inc"
#include "mbmf_ts.inc"
#include "mbmf_buttons.inc"
#include "mbmf_leds.inc"
#include "mbmf_lcd.inc"
#include "mbmf_msg.inc"
#include "mbmf_patch.inc"
#include "mbmf_fsr.inc"
#include "mbmf_addr.inc"
#include "mbmf_mem.inc"
#include "mbmf_dump.inc"
#include "mbmf_midi.inc"
#include "mbmf_meta.inc"
#include "mbmf_sfb.inc"

;; ---[ MBMF presets stored in Flash/EEPROM ]---
#include "mbmf_presets.inc"

	END
