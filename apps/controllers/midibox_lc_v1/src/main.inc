; $Id$
;
; Source Code of MIDIbox LC
; A MIOS Application
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
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

;; ---[ configuration table for MIDI processor rotary encoders ]---
#include "mios_tables.inc"

;; ---[ Custom LCD driver ]---
#include "app_lcd.inc"


;; ==========================================================================
;;  Standard MIOS hooks
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize application specific variables
	movlw	LC_EMULATION_ID		; Logic Control ID
	movwf	LC_ID
	clrf	LC_SYSEX_STATE
	clrf	LC_CURSOR
	clrf	LC_MTC_CURSOR

	;; init the MTC digits
	SET_BSR	LC_MTC_DIGIT_0
	movlw	'0'
	movwf	LC_MTC_DIGIT_0, BANKED
	movwf	LC_MTC_DIGIT_1, BANKED
	movwf	LC_MTC_DIGIT_2, BANKED
	movwf	LC_MTC_DIGIT_3, BANKED
	movwf	LC_MTC_DIGIT_4, BANKED
	movwf	LC_MTC_DIGIT_5, BANKED
	movwf	LC_MTC_DIGIT_6, BANKED
	movwf	LC_MTC_DIGIT_7, BANKED
	movwf	LC_MTC_DIGIT_8, BANKED
	movwf	LC_MTC_DIGIT_9, BANKED
	movlw	':'
	movwf	LC_MTC_DIGIT_C0, BANKED
	movwf	LC_MTC_DIGIT_C1, BANKED
	movwf	LC_MTC_DIGIT_C2, BANKED
	movlw	' '
	movwf	LC_MTC_DIGIT_SPACE, BANKED

	movlw	'R' - 0x40
	movwf	LC_STATUS_DIGIT_A, BANKED
	movlw	'T' - 0x40
	movwf	LC_STATUS_DIGIT_B, BANKED

#if LCD_USE_SECOND
	;; initialize the LCD: we allow graphical LCD and up to 2 character LCDs
	;; for the 2-character LCD option, we have to set the offset of 
	;; line 3 and line 4 to 0x80/0xc0
	movlw	0x00
	movwf	MIOS_PARAMETER1
	movlw	0x40
	movwf	MIOS_PARAMETER2
	movlw	0x80
	movwf	MIOS_PARAMETER3
	movlw	0xc0
	call	MIOS_LCD_YAddressSet
#endif

	;; use LC_STAT.LC_STAT_GLCD as indicator if graphical LCD
	;; or character LCD is connected
	bcf	LC_STAT, LC_STAT_GLCD
	btfsc	MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD
	bsf	LC_STAT, LC_STAT_GLCD

	;; initialize the AIN driver
	movlw	NUMBER_MOTORFADERS
	call	MIOS_AIN_NumberSet
	call	MIOS_AIN_UnMuxed

	movlw	AIN_DEADBAND
	call	MIOS_AIN_DeadbandSet

#if ENABLE_MOTORDRIVER == 1
	;; initialize the MF driver
	call	MIOS_MF_Enable
	movlw	MOTORFADERS_DEADBAND
	call	MIOS_MF_DeadbandSet

	movlw	MOTORFADERS_PWM_DUTY_UP
	call	MIOS_MF_PWM_DutyCycleUpSet
	movlw	MOTORFADERS_PWM_DUTY_DOWN
	call	MIOS_MF_PWM_DutyCycleDownSet
	movlw	MOTORFADERS_PWM_PERIOD
	call	MIOS_MF_PWM_PeriodSet
#endif

	;; initialize the shift registers
	movlw	NUMBER_SHIFTREGISTERS
	call	MIOS_SRIO_NumberSet
	movlw	SRIO_UPDATE_FREQUENCY
	call	MIOS_SRIO_UpdateFrqSet
	movlw	TOUCHSENSOR_SENSITIVITY
	call	MIOS_SRIO_TS_SensitivitySet

	;; initialize encoder speed modes
	clrf	TMP1		; loop 8 times
USER_Init_EncSpeedLoop
	movlw	ENC_VPOT_SPEED_DIVIDER
	movwf	MIOS_PARAMETER2
	movlw	ENC_VPOT_SPEED_MODE
	movwf	MIOS_PARAMETER1
	movf	TMP1, W
	call	MIOS_ENC_SpeedSet
	
	incf	TMP1, F
	movlw	8-1
	cpfsgt	TMP1, ACCESS
	rgoto USER_Init_EncSpeedLoop

	movlw	ENC_JOGWHEEL_SPEED_DIVIDER
	movwf	MIOS_PARAMETER2
	movlw	ENC_JOGWHEEL_SPEED_MODE
	movwf	MIOS_PARAMETER1
	movlw	8
	call	MIOS_ENC_SpeedSet

	;; initialize the integrated MIDI merger
	movlw	MIOS_MIDI_MERGER_DISABLED
	call	MIOS_MIDI_MergerSet

	;; initialize the user timer (used for the meter handling)
	;; (see lc_led.inc)
	;; timer should be called every 20 ms == 200000 ticks
	;; since this value is greater than 16 bit (== 65535 max) we just
	;; use a 1:4 prescaler so the init value must be: 200000 / 4 == 50000
	;; (see also the MIOS_TIMER_Init documentation)
	movlw	50000 & 0xff		; low-byte
	movwf	MIOS_PARAMETER1
	movlw	50000 >> 8		; high-byte
	movlw	0x02			; use 1:4 prescaler
	call	MIOS_TIMER_Init

	;; init display page
	movlw	INITIAL_DISPLAY_PAGE
	movwf	LC_DISPLAY_PAGE

	;; update LEDs which have been assigned to MBLC specific functions
	call	LC_SFB_LED_Update

	;; request update of meters and ledrings
	movlw	0xff
	movwf	LC_METER_UPDATE_REQ	
	movwf	LC_LEDRING_UPDATE_REQ

	;; fill host and GPC message screen with spaces
	movlw	0x80
	movwf	TMP1
	lfsr	FSR0, MSG_HOST_BEGIN
	lfsr	FSR1, MSG_GPC_BEGIN
	movlw	' '
USER_Init_Msg_Loop1
	movwf	POSTINC0
	movwf	POSTINC1
	decfsz	TMP1, F
	rgoto	USER_Init_Msg_Loop1

	;; copy welcome message to msg buffer
	TABLE_ADDR USER_INIT_MSG_STR
	lfsr	FSR0, MSG_HOST_BEGIN
	tblrd*+
	movff	TABLAT, TMP1
USER_Init_Msg_Loop2
	tblrd*+
	movff	TABLAT, POSTINC0
	decfsz	TMP1, F
	rgoto	USER_Init_Msg_Loop2

	;; init rec/solo/mute status
	lfsr	FSR0, RSM_MSG_BEGIN
	movlw	8
	movwf	TMP1
	movlw	'-'
USER_Init_Msg_Loop3
	movwf	POSTINC0
	decfsz	TMP1, F
	rgoto	USER_Init_Msg_Loop3

	;; init sel status
	lfsr	FSR0, SEL_MSG_BEGIN
	movlw	8
	movwf	TMP1
	movlw	'.'
USER_Init_Msg_Loop4
	movwf	POSTINC0
	decfsz	TMP1, F
	rgoto	USER_Init_Msg_Loop4

	;; clear position offset of general purpose controller mode
	clrf	GPC_OFFSET

	return

	;;               <------------------------------------------------------>
USER_INIT_MSG_STR db 54, "xxxx---===< Logic Control Emulation ready. >===---xxxx"



;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; check meter update request flags
	call	LC_METERS_Handler

	;; check ledring update request flags
	call	LC_LEDRING_Handler

	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	;; branch to Meter Timer
	goto	LC_LED_MeterTimer


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

	;; branch to LC_[GC]LCD_Init
	GOTO_IFCLR LC_STAT, LC_STAT_GLCD, ACCESS, LC_CLCD_Init
	goto	LC_GLCD_Init

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; check if host/GPC message update has been requested
	btfss	LC_STAT, LC_STAT_MSG_UPDATE_REQ
	return
	;; yes:	clear request flag, print message
	bcf	LC_STAT, LC_STAT_MSG_UPDATE_REQ
	goto	LC_DISPLAY_Print_Msg


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	;; continue at LC MPROC handler
	goto	LC_MPROC_NotifyReceivedEvent


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
	;; update GPC value
	goto	LC_GPC_AbsValue_Set


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	;; -> jump to "ActionInvalid" for a proper reset of the sysex parser
	goto	LC_SYSEX_ActionInvalid


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	;; -> continue at LC sysex parser
	goto	LC_SYSEX_Parser

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
	;; branch directly to "midi_rxtx.inc"
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
	;; branch directly to "midi_rxtx.inc"
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
	goto	LC_BUTTON_Handler

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	goto	LC_ENC_Handler


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	;; ledrings/meters handler
	call	LC_LED_DOUT_Handler

#if LEDDIGITS_ENABLED
	;; leddigits handler
	call	LC_LEDDIGITS_Update
#endif

#if MIDI_MONITOR_ENABLED != 0
	;; MIDI Rx/Tx Monitor handler
	call	MIDI_RXTX_Handler
#endif

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
	goto	LC_MF_FaderEvent


;; ==========================================================================
;;  standard (reusable) routines
;; ==========================================================================
#include "midi_rxtx.inc"

;; ==========================================================================
;;  Configuration
;; ==========================================================================

;; ---[ LC button/LED functions ]---
#include "lc_io_table.inc"

;; ---[ GPC Labels ]---
#include "lc_gpc_lables.inc"

;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================
#include "lc_glcd.inc"
#include "lc_display.inc"
#include "lc_clcd.inc"
#include "lc_sysex.inc"
#include "lc_mproc.inc"
#include "lc_buttons.inc"
#include "lc_sfb.inc"
#include "lc_enc.inc"
#include "lc_leds.inc"
#include "lc_leddigits.inc"
#include "lc_ledrings.inc"
#include "lc_meters.inc"
#include "lc_mf.inc"
#include "lc_hlp.inc"

#include "lc_cali.inc"
#include "lc_gpc.inc"

;; --------------------------------------------------------------------------
;;  Including fonts from $MIOS_PATH/modules/glcd_font/
;; --------------------------------------------------------------------------
#include <glcd_font_big.inc>
#include <glcd_font_small.inc>
#include <glcd_font_knob_icons.inc>
#include <glcd_font_meter_icons_v.inc>
#include <glcd_font_meter_icons_h.inc>

	END
