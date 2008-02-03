; $Id$
;
; MIOS Application
; MIDIbox64
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
#include <mios_mt_table.inc>	; use dummy table located in $MIOS_PATH/include/asm
#include <mios_enc_table.inc>	; use dummy table located in $MIOS_PATH/include/asm

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
	movlw	DEFAULT_NUMBER_POTS
	call	MIOS_AIN_NumberSet
#if DEFAULT_MUX_ENABLED == 1
	call	MIOS_AIN_Muxed		; service multiplexer interface
#else
	call	MIOS_AIN_UnMuxed	; don't use multiplexer interface
#endif
	movlw	0x07			; set deadband to 7
	call	MIOS_AIN_DeadbandSet

	;; initialize the shift registers
	movlw	0x10			; use all shift registers
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy to 1/1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR ; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; the touch sensor sensitivity is defined in the SysEx dump (see mb64_presets.inc)
	;; and will be set in mb64_dump.inc
#if DEFAULT_ENABLE_AMUX
	movlw	0x00
#else
	movlw	DEFAULT_TOUCH_SENSOR_SENSITIVITY
#endif
	call	MIOS_SRIO_TS_SensitivitySet

	;; initialite the AOUT module if enabled
#if DEFAULT_ENABLE_AOUT
	call	AOUT_Init
#endif

#if DEFAULT_ENABLE_AMUX
	call	AMUX_Init

	;; we want to setup the timer with a frequency of 1kHz = 1 mS
	;; prescaler 1:1 should be used
	;; calculate the required number of clocks for this period:
	;; clocks = period / 100 nS = 1 mS / 100 nS = 10000
	movlw	10000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	10000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_TIMER_Init
	;; now the USER_Timer function is called every 2 mS!
#endif

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
	movff	WREG, MB64_DEVICE_ID

	;; initialize application specific variables
	clrf	MB_STAT

	;; load values from internal dump
	call	MB64_DUMP_RestoreAll

	;; init control surface
	call	CS_MENU_Init

	;; init default LCD offsets
	call	MB64_LCD_SetDefaultOffsets
	;; if "snapshot at poweron" flag is set, request it here
	SET_BSR	MB64_BASE
	bcf	MB_STAT, MB_STAT_SNAPSHOT_REQ
	btfsc	MB64_CFG1, MB64_CFG1_SNAPSHOT_AT_POWERON, BANKED
	bsf	MB_STAT, MB_STAT_SNAPSHOT_REQ

	;; ------------------------------------------------------------------

#if DEFAULT_ENABLE_DRUMS
	;; initialize the drum module
	call	DRUMS_Init
#endif


	;; ------------------------------------------------------------------

	;; initialize MTC receiver
	call	MTC_Init

	;; ------------------------------------------------------------------

	;; initialize MIDICLK receiver
	call	MIDICLK_Init

	;; ------------------------------------------------------------------

	;; initialize BPM handler
	call	BPM_Init

#if DEFAULT_ENABLE_AMUX == 0
	;; call USER_Timer every mS
	movlw	10000 & 0xff
	movwf	MIOS_PARAMETER1
	movlw	10000 >> 8
	movwf	MIOS_PARAMETER2
	movlw	0x00
	call	MIOS_TIMER_Init
#endif

	;; ------------------------------------------------------------------

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; ---[ check BankStick status ]---
	call	MB64_BANK_CheckStick

	;; ---[ if snapshot has been requested, send it here ]---
	CALL_IFSET MB_STAT, MB_STAT_SNAPSHOT_REQ, ACCESS, MB64_PATCH_Send
	bcf	MB_STAT, MB_STAT_SNAPSHOT_REQ

#if DEFAULT_ENABLE_DRUMS
	;; ---[ call drum handler ]---
	call	DRUMS_Tick
#endif
	
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
#if DEFAULT_ENABLE_AMUX
	;; branch to AMUX_Handler if AOUT has been enabled
	call	AMUX_Handler
#else
	;; branch to BPM handler
	call	BPM_Handler
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
	;; continue at MB64 MIDI handler
	goto	MB64_MIDI_NotifyReceivedEvent


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
	goto	MB64_SYSEX_ActionInvalid


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; call MIDIclk handler
	call	MB64_CLK_NotifyReceivedByte
	;; call MTC handler
	call	MB64_MTC_NotifyReceivedByte
	;; -> continue at MB64 sysex parser
	movf	MIOS_PARAMETER1, W
	goto	MB64_SYSEX_Parser

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
	;; branch to the MB64 button handler
	;; (NOTE: this function changes MIOS_PARAMETER[12] -- if additional
	;; handlers should be called from this hook, place them before
	;; this goto instruction)
	goto	MB64_BUTTON_Handler


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
	;; branch to the Rx/Tx LED handler
	goto	MIDI_RXTX_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
#if DEFAULT_ENABLE_DRUMS
	;; ---[ handle the counters for the drum module ]---
	call	DRUMS_Timer
#endif

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
	;; convert 10-bit value to 7-bit value
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	rrf	MIOS_PARAMETER3, F	; value / 2
	rrf	MIOS_PARAMETER2, F
	bcf	MIOS_PARAMETER2, 7	; clear 8th bit

	;; now: pot number in MIOS_PARAMETER1
	;; 7-bit value in MIOS_PARAMETER2

#if DEFAULT_ENABLE_DRUMS
	;; if drum trigger (MIOS_PARAMETER1 < 8):
	;; branch to the DRUMS module
	movlw	8-1
	cpfsgt	MIOS_PARAMETER1, ACCESS
	goto DRUMS_AIN_NotifyChange
#endif

	;; else continue with MB64 pot handler
	;; (expects number of pot in WREG)
	movf	MIOS_PARAMETER1, W
	goto	MB64_POT_Handler

	
;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
	
;; ---[ reusable utility functions ]---
#include "midi_evnt.inc"
#include "midi_rxtx.inc"
#include "math_div16_8.inc"
#include "aout.inc"
#include "amux.inc"
#include "drums.inc"
#include "midiclk.inc"
#include "bpm.inc"
#include "mtc.inc"
#include "convert_bin2bcd.inc"

;; ---[ Control surface functions ]---
#include "cs_menu_buttons.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu.inc"
#include "cs_menu_tables.inc"

;; ---[ Menu pages handlers ]---
#if DEFAULT_LCD_SIZE == 0
  #include "cs_m_display_2x16.inc"
#endif
#if DEFAULT_LCD_SIZE == 1
  #include "cs_m_display_2x20.inc"
#endif
#if DEFAULT_LCD_SIZE == 2
  #include "cs_m_display_4x16.inc"
#endif
#if DEFAULT_LCD_SIZE == 3
  #include "cs_m_display_4x20.inc"
#endif

#include "cs_m_bank.inc"
#include "cs_m_patch.inc"
#include "cs_m_channel.inc"
#include "cs_m_learn.inc"
#include "cs_m_morph.inc"
#include "cs_m_potmode.inc"
#include "cs_m_bname.inc"
#include "cs_m_event.inc"
#include "cs_m_label.inc"
#include "cs_m_copy.inc"
#include "cs_m_midicfg.inc"
#include "cs_m_sysex.inc"

;; ---[ MB64 kernel ]---
#include "mb64_bank.inc"
#include "mb64_sysex.inc"
#include "mb64_pots.inc"
#include "mb64_buttons.inc"
#include "mb64_leds.inc"
#include "mb64_lcd.inc"
#include "mb64_msg.inc"
#include "mb64_patch.inc"
#include "mb64_fsr.inc"
#include "mb64_addr.inc"
#include "mb64_mem.inc"
#include "mb64_dump.inc"
#include "mb64_midi.inc"
#include "mb64_mtc.inc"
#include "mb64_clk.inc"
#include "mb64_meta.inc"
#include "mb64_sfb.inc"

;; ---[ MB64 presets stored in Flash/EEPROM ]---
#include "mb64_presets.inc"

	END
