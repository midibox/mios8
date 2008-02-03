; $Id$
;
; MIOS Application
; MIDIbox SEQ
;
; Detailed infos for customization can be found at http://www.ucapps.de/midibox_seq_options.html
;
; Version:                 <-------------->
#define MBSEQ_VERSION_STR "MIDIboxSEQ V2.4d"
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

	call	SEQ_CORE_Init

	;; init control surface
	call	CS_MENU_Init

	;; init default LCD offsets for a message page
	call	SEQ_LCD_SetMsgOffsets

	;; ------------------------------------------------------------------

	;; initialize the AIN driver
	movlw	DEFAULT_NUMBER_POTS
	call	MIOS_AIN_NumberSet
#if DEFAULT_MUX_ENABLED == 1
	call	MIOS_AIN_Muxed		; service multiplexer interface
#else
	call	MIOS_AIN_UnMuxed	; don't use multiplexer interface
#endif
	movlw	0x07			; set deadband to 7
	call	MIOS_AIN_DeadbandSet

	;; initialize the SRIO driver
	movlw	DEFAULT_NUMBER_SR
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequency to 1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR ; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; initialize the AOUT module
	call	AOUT_Init

	;; initialize clock divider
	SET_BSR	SEQ_BASE
	movlw	DEFAULT_EXT_CLK_DIVIDER
	movwf	SEQ_CORE_EXT_CLK_DIVIDER_PRELOAD, BANKED

	;; ------------------------------------------------------------------

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; ---[ check BankStick status ]---
	;; check every 256 times
	incf	MB_BS_CHK_CTR, F		; counter is used to improve the performance - check only every 256 loop

	;; check pattern BankStick at 0x00
	movf	MB_BS_CHK_CTR, W
	skpnz
	call	SEQ_BANK_CheckStick

	;; check pattern BankStick at 0x80
	movf	MB_BS_CHK_CTR, W
	xorlw	0x80
	skpnz
	call	SEQ_SBANK_CheckStick

	;; ---[ call sequencer handler ]---
	call	SEQ_CORE_Tick

	;; ---[ send out CFG CC's if requested ]---
	call	SEQ_CCOUT_Handler

	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; ---[ call BPM Handler ]---
	call	SEQ_BPM_Tick

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
	;; clear screen(s)
	call	SEQ_LCD_Clear
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
	;; continue at sequencer MIDI handler
	goto	SEQ_MIDI_NotifyReceivedEvent


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
	goto	SEQ_SYSEX_ActionInvalid

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> check midi clock position
	call	SEQ_MIDI_ClkPosParser
	;; -> continue at SEQ sysex parser
	goto	SEQ_SYSEX_Parser

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
	;; call realtime event handler of sequencer
	call	SEQ_MIDI_NotifyRx
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
	;; branch to SEQ button handler
	goto	SEQ_BUTTON_Handler

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
	;; branch to SEQ_ENC_Datawheel if datawheel
	movf	MIOS_PARAMETER1, W
	xorlw	DEFAULT_ENC_DATAWHEEL
	skpnz
	goto	SEQ_ENC_Datawheel
#endif

	;; else branch to encoder handler
	;;  expects encoder number (assigned to step 0-15) in WREG
#if DEFAULT_ENC_DATAWHEEL == 0
	decf	MIOS_PARAMETER1, W
#else
	movf	MIOS_PARAMETER1, W
#endif
	goto	SEQ_ENC_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	;; call the SEQ LED IRQ update handler
	call	SEQ_LED_UpdateIRQ
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
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	;; store pot number in TMP1
	movwf	TMP1

	;; copy 7bit value into MIOS_PARAMETER2 (pot number already in WREG)
	call	MIOS_AIN_Pin7bitGet
	movwf	MIOS_PARAMETER2

	;; restore pot number
	movf	TMP1, W
	movwf	MIOS_PARAMETER1

	;; now: pot number in WREG and MIOS_PARAMETER1
	;; 7-bit value in MIOS_PARAMETER2

	;; branch to sequencer pot handler
	goto	SEQ_POT_Handler

	
;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================

;; ---[ reusable utility functions ]---
#include "midi_evnt.inc"
#include "midi_rxtx.inc"
#include "aout.inc"
#include "math_div24_16.inc"

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
#include "cs_m_pattern.inc"
#include "cs_m_mute.inc"
#include "cs_m_song.inc"
#include "cs_m_trkevnt.inc"
#include "cs_m_trkmode.inc"
#include "cs_m_trkdir.inc"
#include "cs_m_trkdiv.inc"
#include "cs_m_trklen.inc"
#include "cs_m_trktran.inc"
#include "cs_m_trkgrv.inc"
#include "cs_m_trkshft.inc"
#include "cs_m_trkasg.inc"
#include "cs_m_trg.inc"
#include "cs_m_record.inc"
#include "cs_m_morph.inc"
#include "cs_m_bpm.inc"
#include "cs_m_save.inc"
#include "cs_m_metron.inc"
#include "cs_m_opt.inc"
#include "cs_m_midicfg.inc"
#include "cs_m_sysex.inc"

;; ---[ MBSEQ kernel ]---
#include "seq_bank.inc"
#include "seq_sbank.inc"
#include "seq_pots.inc"
#include "seq_enc.inc"
#include "seq_potenc.inc"
#include "seq_remote.inc"
#include "seq_buttons.inc"
#include "seq_leds.inc"
#include "seq_gp.inc"
#include "seq_lcd.inc"
#include "seq_msg.inc"
#include "seq_fsr.inc"
#include "seq_dump.inc"
#include "seq_ccin.inc"
#include "seq_ccout.inc"
#include "seq_cc_table.inc"
#include "seq_ptype.inc"
#include "seq_evnt.inc"
#include "seq_core.inc"
#include "seq_morph.inc"
#include "seq_humanize.inc"
#include "seq_song.inc"
#include "seq_midi.inc"
#include "seq_sysex.inc"
#include "seq_bpm.inc"

;; ---[ BPM table ]---
;; #include "bpm_table.inc"


;; ---[ MBSEQ EEPROM content ]---
#include "seq_presets.inc"

	END
