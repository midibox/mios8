; $Id$
;
; MIOS Application
; MIDIbox SEQ V3
;
; ==========================================================================
; Detailed infos for customization can be found at
;    -> http://www.ucapps.de/midibox_seq_options.html
;
; Hardware related setup is located in setup_mbseq_*.asm
; Please make your customisations there
;
; ==========================================================================
;
MBSEQ_VERSION_STR MACRO
; Version:                 <-------------->
	STRING	16, 0x00, "MBSEQ V3.4c     "
	ENDM
; (fixed string length - 16 characters!)
;
;
; only for debugging: prints the MBSEQ load at the right upper edge of the right 2x40 LCD
#define DISPLAY_PERFORMANCE 0
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
	;; avoid that program will be started on a PIC which doesn't meet the memory requirements
	;; this is to gracefully stop the application from exeution of non-available code memory
	IRQ_DISABLE			; interrupts must be disabled, as TBLPTRU is changed
	TABLE_ADDR_FULL 0x3ffffe	; PIC part number is located here
	tblrd*+
	movf	TABLAT, W		; read DEVID1
	movwf	TMP1			; store for MIDI message
	andlw	0xe0			; extract DEV[2:0]
	xorlw	0x00			; PIC18F4620 = 0b000
	bnz	USER_Init_PIC_NotOk
	tblrd*+
	movf	TABLAT, W		; read DEVID2
	movwf	TMP2			; store for MIDI message
	xorlw	0x0c			; for PIC18F2525/2620/4525/4620
	bz	USER_Init_PIC_Ok
USER_Init_PIC_NotOk
	clrf	TBLPTRU			; clear TBLPTRU
	IRQ_ENABLE			; temporary enable interrupts again
	movlw	0xee			; send MIDI event to notify that the app won't be started (for the case that no LCD is connected to core)
	call	MIOS_MIDI_TxBufferPut
	swapf	TMP1, W			; send DEV[2:0]
	andlw	0x07
	call	MIOS_MIDI_TxBufferPut	; send DEV[9:3]
	movf	TMP2, W
	andlw	0x7f
	call	MIOS_MIDI_TxBufferPut	; send DEV[9:3]
	call	MIOS_MIDI_TxBufferFlush	; wait until message sent
	IRQ_DISABLE			; disable interrupts
USER_Init_PIC_NotOk_Kill		; and loop endless (WDT will reset the chip)
	rgoto	USER_Init_PIC_NotOk_Kill
USER_Init_PIC_Ok
	clrf	TBLPTRU			; clear TBLPTRU
	IRQ_ENABLE			; enable interrupts again

	;; derive Sequencer Device ID from MIOS Device ID
	call	MIOS_MIDI_DeviceIDGet
	movwf	MIDI_DEVICE_ID

	;; initialize application specific variables
	clrf	MB_STAT

	;; init sequencer core functions
	call	SEQ_CORE_Init

#if DEFAULT_SRM_BUTTONS_ENABLED
	;; init button/LED matrix
	call	SEQ_BUTTON_SRM_Init
#endif

	;; init control surface
	call	CS_MENU_Init

	;; init default LCD offsets for a message page
	call	SEQ_LCD_SetMsgOffsets

	;; clear mixer map (will be loaded once bankstick detected)
	call	CS_M_MIXER_ClearMap

	;; ------------------------------------------------------------------

	;; initialize the SRIO driver
	movlw	DEFAULT_NUMBER_SR
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequency to 1 mS
	call	MIOS_SRIO_UpdateFrqSet
	movlw	DEFAULT_SRIO_DEBOUNCE_CTR ; set debounce counter
	call	MIOS_SRIO_DebounceSet

	;; initialize the AOUT module
	call	AOUT_Init

#if DEFAULT_ENABLE_J5_GATES
	movlw	0x00
	call	J5_IO_Init
#endif

	;; initialize the IIC MIDI modules
	call	IIC_MIDI_Init

	;; initialize RI_N pin (switch to input)
	;; Note: RC3 is input by default, and it has installed a pull-up on the core module
	;; thats perfect!
#if DEFAULT_IIC1_RI_N_PORT && DEFAULT_IIC1_RI_N_TRIS
	bsf	DEFAULT_IIC1_RI_N_TRIS, DEFAULT_IIC1_RI_N_PIN
#endif

	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; ---[ check BankStick status ]---
	;; check every 256 times
	incf	MB_BS_CHK_CTR, F		; counter is used to improve the performance - check only every 256 loop

	;; check BankStick each 256 cycles
	movf	MB_BS_CHK_CTR, W
	skpnz
	call	SEQ_BANK_CheckStick

	;; ---[ check for incoming MIDI event from IIC1 MIDI In port ]---
	call	SEQ_ROUTER_CheckIIC1

	;; ---[ call prefetching routine ]---
	call	SEQ_PRFTCH_Tick

	;; ---[ call sequencer handler ]---
	call	SEQ_CORE_Tick

	;; ---[ update AOUTs channels and gates ]---
	call	SEQ_EXT_Update

	;; ---[ for performance measuring ]---
	call	SEQ_LOAD_IncLoadCtr

#if DEFAULT_SRM_BUTTONS_ENABLED
	;; ---[ button matrix ]---
	call	SEQ_BUTTON_SRM_Handler
#endif

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
	call	CS_MENU_Handler

#if DISPLAY_PERFORMANCE == 1
	movlw	0x80 + 34
	call	MIOS_LCD_CursorSet
	call	SEQ_LOAD_Print
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
	;; forward to remote handler (ZERO flag cleared if in remote mode)
	call	SEQ_REMOTE_Handler
	skpz
	return

	;; forward received event to router
	movlw	0		; Source: Int.
	call	SEQ_ROUTER_In

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
	decf	MIOS_PARAMETER1, F
	movf	MIOS_PARAMETER1, W
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
#if DEFAULT_SRM_BUTTONS_ENABLED
	;; ---[ button matrix ]---
	call	SEQ_BUTTON_GetRowIRQ
#endif

#if DEFAULT_DOUT_1MS_TRIGGER
	;; ---[ 1mS triggers ]---
	clrf	MIOS_PARAMETER1
#if DEFAULT_ENABLE_DOUT_GATE_09_16
	movlw	DEFAULT_ENABLE_DOUT_GATE_09_16 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_ENABLE_DOUT_GATE_17_24
	movlw	DEFAULT_ENABLE_DOUT_GATE_17_24 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_ENABLE_DOUT_GATE_25_31
	movlw	DEFAULT_ENABLE_DOUT_GATE_25_31 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_ENABLE_DOUT_GATE_32_39
	movlw	DEFAULT_ENABLE_DOUT_GATE_32_39 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_ENABLE_DOUT_GATE_40_47
	movlw	DEFAULT_ENABLE_DOUT_GATE_40_47 - 1
	call	MIOS_DOUT_SRSet
#endif
#if DEFAULT_ENABLE_DOUT_GATE_48_55
	movlw	DEFAULT_ENABLE_DOUT_GATE_48_55 - 1
	call	MIOS_DOUT_SRSet
#endif
#endif

	;; ---[ for performance measuring ]---
	call	SEQ_LOAD_IncRefCtr

	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	call	CS_MENU_TIMER

	;; workaround for MIOS V1.9 bug (will be fixed in MIOS V1.9e)
	SET_BSR	0x500
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	;; pots not supported by MBSEQ V3
	return

	
;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================

;; ---[ modules from code library ]---
#define IIC_MIDI_USE_RI			0
#define IIC_MIDI_DONT_USE_J5_INPUTS	1
#define IIC_MIDI_PORT_RI_N_SLAVE1       DEFAULT_IIC1_RI_N_PORT
#define IIC_MIDI_TRIS_RI_N_SLAVE1       DEFAULT_IIC1_RI_N_TRIS
#define IIC_MIDI_PIN_RI_N_SLAVE1        DEFAULT_IIC1_RI_N_PIN
#include <iic_midi.inc>		; located in $MIOS_PATH/modules/iic_midi/

; override default pin definitions of AOUT driver
#define AOUT_LAT_CS	LATC	; The chip select pin CS#
#define AOUT_TRIS_CS	TRISC	; is connected to Port C.5
#define AOUT_PIN_CS	5	; (CANNOT be shared with other outputs!)
;
#define AOUT_LAT_DIN	LATD	; The data input pin DIN
#define AOUT_TRIS_DIN	TRISD	; is connected to Port D.4
#define AOUT_PIN_DIN	4	; (can be shared with other outputs)
;
#define AOUT_LAT_SCLK	LATD	; The shift clock input pin SCLK
#define AOUT_TRIS_SCLK	TRISD	; is connected to Port D.5
#define AOUT_PIN_SCLK	5	; (can be shared with other outputs)
; include AOUT driver (located in $MIOS_PATH/modules/aout/)
#include <aout.inc>

#if DEFAULT_ENABLE_J5_GATES
#define J5_IO_DONT_USE_INPUT_FUNCTIONS 1
#include <j5_io.inc>
#endif


;; ---[ reusable utility functions ]---
#include "midi_evnt.inc"
#include "midi_rxtx.inc"
#include "math_div24_16.inc"

;; ---[ Control surface functions ]---
;; ---[ NOTE: these are not the same files like used in MB64, MB64E, MBMF! ]---
#include "cs_menu_buttons.inc"
#include "cs_menu_timer.inc"
#include "cs_menu.inc"
#include "cs_menu_tables.inc"
#include "cs_special_chars.inc"

;; ---[ SEQ BankStick access routines have to be included before CS routines to avoid error warnings w/ gpasm  ]---
#include "seq_bank.inc"
#include "seq_sbank.inc"
#include "seq_mbank.inc"

;; ---[ Menu pages handlers ]---
#include "cs_m_hlp.inc"
#include "cs_m_edit.inc"
#include "cs_m_mixer.inc"
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
#include "cs_m_trkasg.inc"
#include "cs_m_trkrnd.inc"
#include "cs_m_trg.inc"
#include "cs_m_record.inc"
#include "cs_m_morph.inc"
#include "cs_m_bpm.inc"
#include "cs_m_save.inc"
#include "cs_m_opt.inc"
#include "cs_m_metron.inc"
#include "cs_m_midicfg.inc"
#include "cs_m_sysex.inc"
#include "cs_m_format.inc"
#include "cs_m_shortcut.inc"
#include "cs_m_utils.inc"
#include "cs_m_pmute.inc"

;; ---[ MBSEQ kernel ]---
#include "seq_enc.inc"
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
#include "seq_layer.inc"
#include "seq_prftch.inc"
#include "seq_trg.inc"
#include "seq_chord.inc"
#include "seq_scale.inc"
#include "seq_groove.inc"
#include "seq_core.inc"
#include "seq_morph.inc"
#include "seq_humanize.inc"
#include "seq_song.inc"
#include "seq_midi.inc"
#include "seq_router.inc"
#include "seq_record.inc"
#include "seq_sysex.inc"
#include "seq_bpm.inc"
#include "seq_ext.inc"
#include "seq_load.inc"

;; ---[ MBSEQ EEPROM content ]---
#include "seq_presets.inc"

	END
