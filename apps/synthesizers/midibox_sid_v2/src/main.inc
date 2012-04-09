; $Id$
;
; MIOS Application
; MIDIbox SID
;
; -> see doc/README.txt for details
;
;
; ==========================================================================
;
; Hardware related setup is located in setup_*.asm
; Please make your customisations there
;
; ==========================================================================
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================
;
; Version:                <-------------------->
#define MBSID_VERSION_STR "MIDIboxSID V2.040   "
; (fixed string length - 20 characters!)

;; MBNET specific definitions (ping reply information - don't touch this!)
#define MBNET_CONST_PROTOCOL_V		1
#define MBNET_CONST_TYPE_C0		'S'
#define MBNET_CONST_TYPE_C1		'I'
#define MBNET_CONST_TYPE_C2		'D'
#define MBNET_CONST_TYPE_C3		' '
#define MBNET_CONST_TYPE_VERSION	2
#define MBNET_CONST_TYPE_SUBVERSION	0
;
;
;; temporary flag which allows automatic patch modifications on structural changes
;; BankStick content will be modified!
#define ENABLE_PATCH_FIXING 0
;
;
;
        ERRORLEVEL      -311    ; suppress "Operand of HIGH operator was larger than H'FFFF'"
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

;; ---[ MBNet Clone Code -- must be located at the begin of the user flash range ]---
;; ---[ reserved region: 0x3300-0x3500 ]---
#include "mbnet_clone.inc"

;; ---[ Custom LCD driver ]---
#include "app_lcd.inc"
#include "special_characters.inc"

;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; avoid that program will be started on a PIC which doesn't meet the memory requirements
	;; this is to gracefully stop the application from exeution of non-available code memory

	;; in MBSID, we only consider the usage of PIC18F4685, PIC18F4682 and partly PIC18F4620 (master only)
	;; execution on PIC18F452 is prevented
	;; due to these variations, the DEVID1 won't be checked, only DEVID2
	clrf	SID_STAT		; clear whole SID status register
	IRQ_DISABLE			; interrupts must be disabled, as TBLPTRU is changed
	TABLE_ADDR_FULL 0x3ffffe
	tblrd*+
	movf	TABLAT, W		; read DEVID1
	movwf	TMP1			; store for MIDI message
	tblrd*+
	movf	TABLAT, W		; read DEVID2
	movwf	TMP2			; store for MIDI message
	xorlw	0x27			; for PIC18F2682/2685/4682/4685
	bz	USER_Init_PIC_Ok
#if 0
	;; PIC18F4620 not supported anymore - 64k border reached
	xorlw	0x0c ^ 0x27		; for PIC18F2525/2620/4525/4620
	bz	USER_Init_PIC_Ok
#endif
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


	;; initialize application specific variables
	SET_BSR	SID_BASE
	clrf	SID_SYSEX_STATE, BANKED

	movlw	0x01
	movff	WREG, SYX_SID_SELECTION

	;; we have to start with BS0 in SID_BANK_CheckStick)
	setf	SID_BANKSTICK_CHK_CTR
	
        ;; Initialize Clock Generator (the PWM unit is used to generate a 1MHz clock with 1:1 duty cycle)
        movlw   0x09
        movwf   PR2
	movlw	0x24
	movwf	T2CON
        movlw   0x0f
        movwf   CCP1CON
	movlw	0x04
        movwf   CCPR1L

	;; initialize the MBHP_SID module
	call	SID_SR_Init

	;; get and store the device ID
	call	MIOS_MIDI_DeviceIDGet
	movwf	SID_MIDI_DEVICE

	;; initial random seed
	SET_BSR	SID_BASE
	;; restart pseudo random generator
	movlw	0x42
	movwf	SID_RANDOM_SEED_L, BANKED
	movwf	SID_RANDOM_SEED_H, BANKED

	;; ensure that volume is already maximum when starting MBSID
	;; this might look incorrect, but the reason is, that there is some increased
	;; background noise when VOL registers are zero!
	movlw	0x0f
	movff	WREG, SIDL_BASE + SIDx_MODE_VOL
	movff	WREG, SIDR_BASE + SIDx_MODE_VOL

	;; load patch buffer and init patch
	clrf	SID_PATCH
	clrf	SID_BANK
	call	SID_PATCH_LoadPatchBuffer

	;; init ensemble
	;; (will be done again from the CS once CAN slaves are available)
	call	SID_ENS_LoadDefaultNum
	call	SID_ENS_Init

	;; initialize the shift registers (master only)
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	USER_Init_NoCS
USER_Init_CS
	movlw	DEFAULT_SRIO_NUMBER	; number of used shift registers in a chain
	call	MIOS_SRIO_NumberSet
	movlw	0x01			; set update frequncy
	call	MIOS_SRIO_UpdateFrqSet
	movlw	0x00			; disable touch sensor
	call	MIOS_SRIO_TS_SensitivitySet
USER_Init_NoCS

#if DEFAULT_J5_FUNCTION == 1
	;; initialize J5 for analog inputs if enabled
 	call	AIN_Init
#endif
#if (DEFAULT_J5_FUNCTION == 2) || (DEFAULT_J5_FUNCTION == 3)
	;; initialize J5 for driving LEDs if enabled
	movlw	0x00
	call	J5_IO_Init
#endif

	;; initialize the AOUT module
	call	AOUT_Init

	;; initialize the integrated MIDI merger
	movlw	MIOS_MIDI_MERGER_DISABLED	; should be disabled for a synthesizer
	call	MIOS_MIDI_MergerSet

	;; initialize the ECAN interface
	call	MBNET_Init

	;; initialize SID player
	call	SIDPLAY_Init

	;; disable engine so long no note is played
	;; this is to overcome a WDT issue when no display is connected (LCD timeout loop takes too long on high IRQ load)
	bsf	SID_STAT, SID_STAT_ENGINE_DISABLE_LEVEL0

	;; initialize the timer for the SIDSW handler
	movlw	LOW(10000)		; every 1 mS (update cycle splitted into 2 halfs!)
	movwf	MIOS_PARAMETER1
	movlw	HIGH(10000)
	movwf	MIOS_PARAMETER2
	movlw	0x00			; prescaler 1:1
	call	MIOS_TIMER_Init

	;; set cursor offsets
	movlw	DEFAULT_LCD_LINE_Y0
	movwf	MIOS_PARAMETER1
	movlw	DEFAULT_LCD_LINE_Y1
	movwf	MIOS_PARAMETER2
	movlw	DEFAULT_LCD_LINE_Y2
	movwf	MIOS_PARAMETER3
	movlw	DEFAULT_LCD_LINE_Y3
	call	MIOS_LCD_YAddressSet

	;; reset the control surface
	call	CS_MENU_Reset

	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; enable engine (only required after startup or after a SysEx/MBNet patch upload)
	bcf	SID_STAT, SID_STAT_ENGINE_DISABLE_LEVEL0

	;; ---[ update AOUTs channels and gates ]---
	call	SID_EXT_Update

	;; if not in SIDPLAY mode:
	BRA_IFSET SID_STAT, SID_STAT_SIDPLAYER, ACCESS, USER_Tick_SIDPlayEnabled
USER_Tick_SIDPlayDisabled
	;; ---[ check BankStick status ]---
 	call	SID_BANK_CheckStick

	;; ---[ call SID shift register handler ]---
	call	SID_SR_Handler
	rgoto	USER_Tick_SIDPlayDisabled_Cont

USER_Tick_SIDPlayEnabled
	;; disable CS if SID player enabled and SID#1 selected
	;; (tmp. disable interrupts, so that LED matrix doesn't start to decrement bar counters)
	IRQ_DISABLE
	bcf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	btfsc	CS_MENU_SELECTED_SID_FLAGS, 0
	bsf	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	IRQ_ENABLE
USER_Tick_SIDPlayDisabled_Cont

	;; ---[ call the MBNET handler ]---
	call	MBNET_Handler

#if DEFAULT_SRM_BUTTONS_DIN
	;; ---[ button matrix ]---
	call	CS_MENU_MATRIX_BHandler
#endif

#if DEFAULT_J5_FUNCTION == 1
	;; ---[ AD handler ]---
	call	AIN_Handler
#endif
	return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer

#if DEFAULT_J5_FUNCTION == 1
	;; ---[ AD conversion ]---
	call	AIN_Tick
#endif
	
	;; ---[ call Software SID Handler ]---
	;; (if not in SIDPLAY mode)
	CALL_IFCLR SID_STAT, SID_STAT_SIDPLAYER, ACCESS, SIDSE_Handler

	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	USER_TimerNoCS

	;; ---[ handle with control surface variables (flashing cursor, etc) ]---
	call	CS_MENU_TIMER
USER_TimerNoCS

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
	;; clear screen
	call	MIOS_LCD_Clear

	TABLE_ADDR TEXT_WELCOME_0		; print welcome message

	;; print welcome message only during startup phase
	movf	CS_MENU_REINIT_CFG_CTR, W
	bz	USER_DISPLAY_Init_NoStartup

	call	MIOS_LCD_PrintString		; first line

	TABLE_ADDR TEXT_WELCOME_10
	movf	SID_MIDI_DEVICE, W
	bz	USER_DISPLAY_Init_NotDis
USER_DISPLAY_Init_Dis
	TABLE_ADDR TEXT_WELCOME_11
USER_DISPLAY_Init_NotDis
	call	MIOS_LCD_PrintString		; second line

	;; only if character LCD connected: init special characters
	BRA_IFSET MIOS_BOX_CFG0, MIOS_BOX_CFG0_USE_GLCD, ACCESS, USER_DISPLAY_Init_NoSC
	TABLE_ADDR CS_MENU_SPECIAL_CHARS	; (defined in special_characters.inc)
	call	MIOS_CLCD_SpecialCharsInit
USER_DISPLAY_Init_NoSC

USER_DISPLAY_Init_NoStartup

	;; request an initialization
	bsf	CS_STAT, CS_STAT_DISPLAY_INIT_REQ

	;; exception: if in SID player mode, and first SID selected: call special LCD init routine
	BRA_IFCLR SID_STAT, SID_STAT_SIDPLAYER, ACCESS, USER_DISPLAY_Init_NoPlayer
	BRA_IFCLR CS_MENU_SELECTED_SID_FLAGS, 0, ACCESS, USER_DISPLAY_Init_NoPlayer
USER_DISPLAY_Init_Player
	;; init LCD
	call	SIDPLAY_InitLCD
USER_DISPLAY_Init_NoPlayer

	;; in the next cycle USER_DISPLAY_Tick will be called which
	;; does the job
	return


;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	movf	SID_MIDI_DEVICE, W	; exit if device ID != 0x00
	skpz
	return

	;; exception: if in SID player mode, and first SID selected: don't do anything
	btfsc	SID_STAT, SID_STAT_SIDPLAYER_CS_DISABLE
	return

	;; continue at the control surface menu handler
	call	CS_MENU_Handler

#if 0
	movlw	0x00
	call	MIOS_LCD_CursorSet
	movff	0x3dc, WREG
	call	MIOS_LCD_PrintHex2
	movff	0x3dd, WREG
	call	MIOS_LCD_PrintHex2
	movff	0x3de, WREG
	call	MIOS_LCD_PrintHex2
	movff	0x3df, WREG
	call	MIOS_LCD_PrintHex2
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
	;; -> check for SIDplay stream
	;; (only done for master)
	movf	SID_MIDI_DEVICE, F ; don't modify WREG
	bnz	USER_MPROC_NotifyReceivedByteNSP
USER_MPROC_NotifyReceivedByteSP
	movff	WREG, SID_MIDI_PARAMETER1	; store byte in WREG
	call	SIDPLAY_Handler
	btfsc	SID_STAT, SID_STAT_SIDPLAYER; exit here if SID player enabled
	return	
	movff	SID_MIDI_PARAMETER1, WREG	; restore byte
USER_MPROC_NotifyReceivedByteNSP

	;; -> continue at SID_MPROC_Parser
	goto	SID_SYSEX_Parser
	
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

	SET_BSR	SID_BASE

	;; notify clock event if received
	movlw	0xf8
	cpfseq	IRQ_TMP1, ACCESS
	rgoto USER_MIDI_NotifyRx_NoClk
USER_MIDI_NotifyRx_Clk
	;; we've measured a new delay between two F8 events
	movf	SID_INCOMING_CLK_CTR, W, BANKED
	movwf	SID_INCOMING_CLK_DELAY, BANKED
	clrf	SID_INCOMING_CLK_CTR, BANKED

	;; increment clock counter by 4
	movf	SID_SENT_CLK_CTR, W, BANKED
	sublw	4
	addwf	SID_CLK_REQ_CTR, F, BANKED

	;; clear interpolation clock counter and get new SENT_CLK delay
	clrf	SID_SENT_CLK_CTR, BANKED
	rrf	SID_INCOMING_CLK_DELAY, W, BANKED
	rrf	WREG, W
	andlw	0x3f
	movwf	SID_SENT_CLK_DELAY, BANKED
USER_MIDI_NotifyRx_NoClk

	;; if MIDI start: notify start
	movf	IRQ_TMP1, W
	xorlw	0xfa
	bz	USER_MIDI_NotifyRx_Start
	xorlw	0xf2 ^ 0xfa		; quick&dirty: reset divider on song position change as well
	bz	USER_MIDI_NotifyRx_Start ; normaly we should parse the new position as well, but for
					; most common cases (interaction with DAW) this is sufficient
	rgoto USER_MIDI_NotifyRx_NoStart
USER_MIDI_NotifyRx_Start
	bsf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FA_REQ, BANKED

	;; Auto Mode: immediately switch to slave mode
	movff	SID_LOCAL_ENS + SID_ENSx_CTRL1, WREG
	btfsc	WREG, SID_ENS_CTRL1_CLK_AUTO
	bsf	SID_STAT, SID_STAT_CLK_SLAVE

	;; ensure that incoming clock counter != 0xff (means: no clock received for long time)
	incf	SID_INCOMING_CLK_CTR, W, BANKED
	movlw	0x80		; should be enough until first MIDI clock
	skpnz
	clrf	SID_INCOMING_CLK_CTR, BANKED

	;; cancel all requested clocks
	clrf	SID_CLK_REQ_CTR, BANKED
	movlw	3
	movwf	SID_SENT_CLK_CTR, BANKED
USER_MIDI_NotifyRx_NoStart

	;; if MIDI continue: notify continue
	movlw	0xfb
	cpfseq	IRQ_TMP1, ACCESS
	rgoto USER_MIDI_NotifyRx_NoCont
USER_MIDI_NotifyRx_Cont
	bsf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FB_REQ, BANKED
USER_MIDI_NotifyRx_NoCont

	;; if MIDI stop: notify stop
	movlw	0xfc
	cpfseq	IRQ_TMP1, ACCESS
	rgoto USER_MIDI_NotifyRx_NoStop
USER_MIDI_NotifyRx_Stop
	bsf	SID_SE_STATE, SID_SE_STATE_MIDI_CLK_FC_REQ, BANKED
USER_MIDI_NotifyRx_NoStop

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
	movf	SID_MIDI_DEVICE, W	; skip if device ID != 0x00
	bnz	USER_DIN_NotifyToggleNoCS

	;; continue in cs_menu_button.inc
	goto	CS_MENU_BUTTON_Handler

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
	movf	SID_MIDI_DEVICE, W	; exit if device ID != 0x00
	skpz
	return

	;; store encoder number in CS_MENU_USED_ENCODER - will be used by CS_MENU_ENC_CSInc later to set a new speed!
	movff	MIOS_PARAMETER1, CS_MENU_USED_ENCODER

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


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	;; handle with Modulation matrix on every update cycle if enabled
	movf	SID_MIDI_DEVICE, W		; skip if device ID != 0x00
	bnz	USER_SR_Service_PrepareNoCS
 	call	CS_MENU_MATRIX_Handler
USER_SR_Service_PrepareNoCS
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
#if DEFAULT_SRM_BUTTONS_DIN
	;; ---[ button matrix ]---
	call	CS_MENU_MATRIX_GetRowIRQ
#endif
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange

	;; NOTE: WE HAVE OUR OWN AIN HANDLER IN ain.inc
	;; THE MIOS DRIVER IS NOT USED, THEREFORE THIS FUNCTION WILL NEVER BE CALLED !!!

	return


;; --------------------------------------------------------------------------
;;  This function is called by the ain.inc driver when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o 8bit value in MIOS_PARAMETER2
;; --------------------------------------------------------------------------
AIN_NotifyChange
	;; exit if pin number >= 5 (only forward Knob #1..4)
	movlw	5
	cpfslt	MIOS_PARAMETER1, ACCESS
	return

	;; branch depending on master/slave
	movf	SID_MIDI_DEVICE, W
	bz	AIN_NotifyChange_Master
AIN_NotifyChange_Slave
	;; Slave: only forward to knob handler
	;; forward to knob handler
	movf	MIOS_PARAMETER1, W
	movff	MIOS_PARAMETER2, MIOS_PARAMETER1
	goto	SID_KNOB_SetValue

	
AIN_NotifyChange_Master	
	;; Master: forward via CS for all selected SIDs
	clrf	CS_MENU_SID
AIN_NotifyChange_Loop
	movf	CS_MENU_SID, W
	call	MIOS_HLP_GetBitORMask
	andwf	CS_MENU_SELECTED_SID_FLAGS, W
	bz	AIN_NotifyChange_Loop_Next

	movf	MIOS_PARAMETER1, W
	call	SID_KNOB_GetPtr
	movf	FSR1L, W
	addlw	SID_Ix_Px_VALUE
	movwf	FSR0L
	movf	FSR1H, W
	addlw	HIGH(SID_EDIT_BUFFER)-HIGH(SID_PATCH_BUFFER)
	movwf	FSR0H
	movff	MIOS_PARAMETER2, INDF0

	;; special sending routine knob values
	movff	MIOS_PARAMETER1, CS_MENU_PARAMETER_IX
	call	CS_MENU_MS_SendKnob

AIN_NotifyChange_Loop_Next
	incf	CS_MENU_SID, F
	BRA_IFCLR CS_MENU_SID, 2, ACCESS, AIN_NotifyChange_Loop

	return


;; ==========================================================================
;;  Application code (see comments in files)
;; ==========================================================================

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
#include "math_mul16_8.inc"
#include "math_div24_16.inc"
#include "ain.inc"
#include "mbnet.inc"

;; ---[ Control surface functions ]---
#include "cs_menu_enc_table.inc"
#include "cs_menu_buttons.inc"
#include "cs_menu_leds.inc"
#include "cs_menu_matrix.inc"
#include "cs_menu_enc.inc"
#include "cs_menu_timer.inc"
#include "cs_menu_exec.inc"
#include "cs_menu.inc"
#include "cs_menu_fsr0.inc"
#include "cs_menu_p2r_r2p.inc"
#include "cs_menu_print.inc"
#include "cs_menu_ms.inc"
#include "cs_menu_shift.inc"
#include "cs_menu_bank.inc"
#include "cs_menu_mbnet.inc"
#include "cs_menu_labels.inc"
#include "cs_menu_tables.inc"

;; ---[ SID kernel ]---
#include "sid_lcd.inc"
#include "sid_ens.inc"
#include "sid_patch.inc"
#include "sid_voice.inc"
#include "sid_rnd.inc"
#include "sid_bank.inc"
#include "sid_pbank.inc"
#include "sid_ebank.inc"
#include "sid_tune.inc"
#include "sid_sr.inc"
#include "sid_ext.inc"
#include "sid_se.inc"
#include "sid_se_l.inc"
#include "sid_se_b.inc"
#include "sid_se_d.inc"
#include "sid_se_m.inc"
#include "sid_midi.inc"
#include "sid_midi_l.inc"
#include "sid_midi_b.inc"
#include "sid_midi_d.inc"
#include "sid_midi_m.inc"
#include "sid_sysex.inc"
#include "sid_mbnet.inc"
#include "sid_knob.inc"
#include "sid_parin.inc"
#include "sid_parout.inc"
#include "sid_par_table.inc"
#include "sid_cc_table.inc"
#include "sid_env_table.inc"
#include "sid_lfo_table.inc"
#include "sid_note_table.inc"
#include "sid_frq_table.inc"
#include "sid_sin_table.inc"
#include "sid_depth_table.inc"
#include "sid_dmodel.inc"	; (must be located in upper 64k area)

;; ---[ SIDplay module ]---
#include "sidplay.inc"

;; ---[ Filter calibration table (switches to static address, accordingly should be included after code, but before presets ]---
#include "sid_filter_table.inc"

;; ---[ SID EEPROM content ]---
#include "sid_presets.inc"

	END
