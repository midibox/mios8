; $Id: mios_wrapper.asm 63 2008-01-31 20:44:17Z tk $
;
; This is the MIOS wrapper for SDCC (qualified for v2.5.0 and v2.6.0)
;
; the upper boundary of the stacks are defined here 
; customize the values for your needs
#ifndef STACK_HEAD
#define STACK_HEAD 0x37f
#endif

#ifndef STACK_IRQ_HEAD
#define STACK_IRQ_HEAD 0x33f
#endif
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

;; ---[ vectors to MIOS functions (never change!) ]---
#include <mios_vectors.inc>

;; ---[ user hooks (never change!) ]---
#include <user_vectors.inc>

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[ configuration table for MIDI processor ]---
#ifndef DONT_INCLUDE_MIOS_MT_TABLE
;; dummy table located in $MIOS_PATH/include/asm
;; If the application should use MT entries,
;; just add "-DDONT_INCLUDE_MIOS_MT_TABLE" to MIOS_WRAPPER_DEFINES
;; and build the table within the C program with:
;; 
;; MIOS_MT_TABLE {
;;   MIOS_MT_ENTRY(0xb0, 0x10), // CC#16
;;   MIOS_MT_ENTRY(0xb0, 0x11), // CC#17
;;   MIOS_MT_ENTRY(0xb0, 0x12), // CC#18
;;   MIOS_MT_ENTRY(0xb0, 0x13), // CC#19
;;   MIOS_MT_EOT
;; };
;; 
;; The MIOS_MT_* macros are defined in $MIOS_PATH/include/c/cmios.h
;; 
;; Note that due to the inflexibility of the MT Table concept, it isn't
;; recommented for new designs
#include <mios_mt_table.inc>
#endif

;; ---[ configuration table for rotary encoders ]---
#ifndef DONT_INCLUDE_MIOS_ENC_TABLE
;; dummy table located in $MIOS_PATH/include/asm
;; If the application should use ENC entries,
;; just add "-DDONT_INCLUDE_MIOS_ENC_TABLE" to MIOS_WRAPPER_DEFINES
;; and build the table within the C program with:
;; 
;; MIOS_ENC_TABLE {
;;              // sr pin mode
;;   MIOS_ENC_ENTRY(1, 0, MIOS_ENC_MODE_DETENTED), // VPot #1
;;   MIOS_ENC_ENTRY(1, 2, MIOS_ENC_MODE_DETENTED), // VPot #2
;;   MIOS_ENC_ENTRY(1, 4, MIOS_ENC_MODE_DETENTED), // VPot #3
;;   MIOS_ENC_EOT
;; };
;; 
;; The MIOS_MT_* macros are defined in $MIOS_PATH/include/c/cmios.h
#include <mios_enc_table.inc>
#endif

	org	0x3300		; never change the origin!

; Don't enable this --- its only used by a special testcase which checks 
; parameter passing on functions which are difficult to verify with
; directed tests
#ifndef PARAMETER_PASSING_TEST
#define PARAMETER_PASSING_TEST 0
#endif

;; ==========================================================================
;;  Export MIOS variables
;; ==========================================================================
	global	_MIOS_BOX_CFG0
	global	_MIOS_BOX_CFG1
	global	_MIOS_BOX_STAT
	global	_MIOS_PARAMETER1
	global	_MIOS_PARAMETER2
	global	_MIOS_PARAMETER3

MIOS_VARIABLES		udata	0x0000
_MIOS_BOX_CFG0		res	1
_MIOS_BOX_CFG1		res	1
_MIOS_BOX_STAT		res	1
_MIOS_PARAMETER1	res	1
_MIOS_PARAMETER2	res	1
_MIOS_PARAMETER3	res	1
_TMP1			res	1
_TMP2			res	1
_TMP3			res	1
_TMP4			res	1
_TMP5			res	1
_IRQ_TMP1		res	1
_IRQ_TMP2		res	1
_IRQ_TMP3		res	1
_IRQ_TMP4		res	1
_IRQ_TMP5		res	1


;; ==========================================================================
;;  USER_LCD_* vectors defined externally in $MIOS_PATH/modules/app_lcd/*/*.asm
;; ==========================================================================
	extern	USER_LCD_Init
	extern	USER_LCD_Data
	extern	USER_LCD_Cmd
	extern	USER_LCD_Clear
	extern	USER_LCD_CursorSet
	extern	USER_LCD_PrintChar
	extern	USER_LCD_SpecialCharInit


;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

MIOS_WRAPPER_CODE	code

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	extern	_Init

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD
	goto	_Init	


;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	extern	_Tick

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD
	goto	_Tick


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
	extern	_Timer

	lfsr	FSR2, STACK_IRQ_HEAD	; initialize IRQ stack
	movff	FSR0L, POSTDEC2		; save FSR0[LH] on IRQ stack
	movff	FSR0H, POSTDEC2
	lfsr	FSR0, STACK_IRQ_HEAD-2	; write stack pointer also to FSR0
	call	_Timer			; call C hook
	movff	STACK_IRQ_HEAD-0, FSR0L	; restore FSR0[LH] directly from memory (more fail save)
	movff	STACK_IRQ_HEAD-1, FSR0H	; restore FSR0[LH] directly from memory (more fail save)
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
	;; not really required for C programs
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when the display content should be 
;;  initialized. Thats the case during startup and after a temporary message
;;  has been printed on the screen
;; --------------------------------------------------------------------------
USER_DISPLAY_Init
	extern	_DISPLAY_Init

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD
	goto	_DISPLAY_Init

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	extern	_DISPLAY_Tick

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD
	goto	_DISPLAY_Tick


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	extern	_MPROC_NotifyReceivedEvnt

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	movff	MIOS_PARAMETER3, POSTDEC0
	movff	MIOS_PARAMETER2, POSTDEC0
	movf	MIOS_PARAMETER1, W
	goto	_MPROC_NotifyReceivedEvnt


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
	extern	_MPROC_NotifyFoundEvent

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	movff	MIOS_PARAMETER3, POSTDEC0
	movff	MIOS_PARAMETER2, POSTDEC0
	movff	MIOS_PARAMETER1, POSTDEC0
	movwf	POSTDEC0		; TK: required for SDCC 2.5.0, don't know why...
	;; entry number still in wreg
	goto	_MPROC_NotifyFoundEvent


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has not been completly
;;  received within 2 seconds
;; --------------------------------------------------------------------------
USER_MPROC_NotifyTimeout
	extern	_MPROC_NotifyTimeout

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD
	goto	_MPROC_NotifyTimeout


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	extern	_MPROC_NotifyReceivedByte

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	;; MIDI byte still in WREG
	goto	_MPROC_NotifyReceivedByte

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
	;; too time consuming for C programs --- should be handled so fast as
	;; possible and therefore serviced in assembler

	;; disabled by default
	;; enable it if external function is available
#ifdef ENABLE_MIDI_NOTIFY_TX
	extern	_MIDI_NotifyTx
	goto	_MIDI_NotifyTx
#else
	return
#endif

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
	;; too time consuming for C programs --- should be handled so fast as
	;; possible and therefore serviced in assembler

	;; disabled by default
	;; enable it if external function is available
#ifdef ENABLE_MIDI_NOTIFY_RX
	extern	_MIDI_NotifyRx
	goto	_MIDI_NotifyRx
#else
	return
#endif

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an button has been toggled
;;  Input:
;;     o Button number in WREG and MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
USER_DIN_NotifyToggle
	extern	_DIN_NotifyToggle

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	movff	MIOS_PARAMETER2, POSTDEC0
	;; button number still in WREG
	goto	_DIN_NotifyToggle


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	extern	_ENC_NotifyChange

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	movff	MIOS_PARAMETER2, POSTDEC0
	;; encoder number still in WREG
	goto	_ENC_NotifyChange


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	extern	_SR_Service_Prepare

	lfsr	FSR2, STACK_IRQ_HEAD	; initialize IRQ stack
	movff	FSR0L, POSTDEC2		; save FSR0[LH] on IRQ stack
	movff	FSR0H, POSTDEC2
	lfsr	FSR0, STACK_IRQ_HEAD-2	; write stack pointer also to FSR0
	call	_SR_Service_Prepare	; call C hook
	movff	STACK_IRQ_HEAD-0, FSR0L	; restore FSR0[LH] directly from memory (more fail save)
	movff	STACK_IRQ_HEAD-1, FSR0H	; restore FSR0[LH] directly from memory (more fail save)
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	extern	_SR_Service_Finish

	lfsr	FSR2, STACK_IRQ_HEAD	; initialize IRQ stack
	movff	FSR0L, POSTDEC2		; save FSR0[LH] on IRQ stack
	movff	FSR0H, POSTDEC2
	lfsr	FSR0, STACK_IRQ_HEAD-2	; write stack pointer also to FSR0
	call	_SR_Service_Finish	; call C hook
	movff	STACK_IRQ_HEAD-0, FSR0L	; restore FSR0[LH] directly from memory (more fail save)
	movff	STACK_IRQ_HEAD-1, FSR0H	; restore FSR0[LH] directly from memory (more fail save)
	return

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	extern	_AIN_NotifyChange

	lfsr	FSR0, STACK_HEAD	; initialize stack
	lfsr	FSR2, STACK_HEAD

	movff	MIOS_PARAMETER3, POSTDEC0
	movff	MIOS_PARAMETER2, POSTDEC0
	;; pot number still in WREG
	goto	_AIN_NotifyChange



;; --------------------------------------------------------------------------
;; ==========================================================================
;;  LINKS TO THE MIOS FUNCTIONS
;; ==========================================================================
;; --------------------------------------------------------------------------

.MIOS_MIDI_BeginStream code MIOS_MIDI_BeginStream
_MIOS_MIDI_BeginStream
	global	_MIOS_MIDI_BeginStream

;; --------------------------------------------------------------------------
.MIOS_MIDI_DeviceIDAutoSet code MIOS_MIDI_DeviceIDAutoSet
_MIOS_MIDI_DeviceIDAutoSet
	global	_MIOS_MIDI_DeviceIDAutoSet

;; --------------------------------------------------------------------------
.MIOS_MIDI_DeviceIDGet code MIOS_MIDI_DeviceIDGet
_MIOS_MIDI_DeviceIDGet
	global	_MIOS_MIDI_DeviceIDGet

;; --------------------------------------------------------------------------
.MIOS_MIDI_DeviceIDSet code MIOS_MIDI_DeviceIDSet
_MIOS_MIDI_DeviceIDSet
	global	_MIOS_MIDI_DeviceIDSet

;; --------------------------------------------------------------------------
.MIOS_MIDI_EndStream code MIOS_MIDI_EndStream
_MIOS_MIDI_EndStream
	global	_MIOS_MIDI_EndStream

;; --------------------------------------------------------------------------
.MIOS_MIDI_Init code MIOS_MIDI_Init
_MIOS_MIDI_Init
	global	_MIOS_MIDI_Init

;; --------------------------------------------------------------------------
.MIOS_MIDI_InterfaceAutoSet code MIOS_MIDI_InterfaceAutoSet
_MIOS_MIDI_InterfaceAutoSet
	global	_MIOS_MIDI_InterfaceAutoSet

;; --------------------------------------------------------------------------
.MIOS_MIDI_InterfaceGet code MIOS_MIDI_InterfaceGet
_MIOS_MIDI_InterfaceGet
	global	_MIOS_MIDI_InterfaceGet

;; --------------------------------------------------------------------------
.MIOS_MIDI_InterfaceSet code MIOS_MIDI_InterfaceSet
_MIOS_MIDI_InterfaceSet
	global	_MIOS_MIDI_InterfaceSet

;; --------------------------------------------------------------------------
.MIOS_MIDI_MergerGet code MIOS_MIDI_MergerGet
_MIOS_MIDI_MergerGet
	global	_MIOS_MIDI_MergerGet

;; --------------------------------------------------------------------------
.MIOS_MIDI_MergerSet code MIOS_MIDI_MergerSet
_MIOS_MIDI_MergerSet
	global	_MIOS_MIDI_MergerSet

;; --------------------------------------------------------------------------
.MIOS_MIDI_RxBufferFree code MIOS_MIDI_RxBufferFree
_MIOS_MIDI_RxBufferFree
	global	_MIOS_MIDI_RxBufferFree

;; --------------------------------------------------------------------------
.MIOS_MIDI_RxBufferGet code MIOS_MIDI_RxBufferGet
_MIOS_MIDI_RxBufferGet
	global	_MIOS_MIDI_RxBufferGet

;; --------------------------------------------------------------------------
.MIOS_MIDI_RxBufferPut code MIOS_MIDI_RxBufferPut
_MIOS_MIDI_RxBufferPut
	global	_MIOS_MIDI_RxBufferPut

;; --------------------------------------------------------------------------
.MIOS_MIDI_RxBufferUsed code MIOS_MIDI_RxBufferUsed
_MIOS_MIDI_RxBufferUsed
	global	_MIOS_MIDI_RxBufferUsed

;; --------------------------------------------------------------------------
.MIOS_MIDI_TxBufferFlush code MIOS_MIDI_TxBufferFlush
_MIOS_MIDI_TxBufferFlush
	global	_MIOS_MIDI_TxBufferFlush

;; --------------------------------------------------------------------------
.MIOS_MIDI_TxBufferFree code MIOS_MIDI_TxBufferFree
_MIOS_MIDI_TxBufferFree
	global	_MIOS_MIDI_TxBufferFree

;; --------------------------------------------------------------------------
.MIOS_MIDI_TxBufferGet code MIOS_MIDI_TxBufferGet
_MIOS_MIDI_TxBufferGet
	global	_MIOS_MIDI_TxBufferGet

;; --------------------------------------------------------------------------
.MIOS_MIDI_TxBufferPut code MIOS_MIDI_TxBufferPut
_MIOS_MIDI_TxBufferPut
	global	_MIOS_MIDI_TxBufferPut

;; --------------------------------------------------------------------------
.MIOS_MIDI_TxBufferUsed code MIOS_MIDI_TxBufferUsed
_MIOS_MIDI_TxBufferUsed
	global	_MIOS_MIDI_TxBufferUsed

;; --------------------------------------------------------------------------
.MIOS_MPROC_MergerDisable code MIOS_MPROC_MergerDisable
_MIOS_MPROC_MergerDisable
	global	_MIOS_MPROC_MergerDisable

;; --------------------------------------------------------------------------
.MIOS_MPROC_MergerEnable code MIOS_MPROC_MergerEnable
_MIOS_MPROC_MergerEnable
	global	_MIOS_MPROC_MergerEnable

;; --------------------------------------------------------------------------
.MIOS_MPROC_MergerGet code MIOS_MPROC_MergerGet
_MIOS_MPROC_MergerGet
	global	_MIOS_MPROC_MergerGet

;; --------------------------------------------------------------------------
.MIOS_AIN_DeadbandGet code MIOS_AIN_DeadbandGet
_MIOS_AIN_DeadbandGet
	global	_MIOS_AIN_DeadbandGet

;; --------------------------------------------------------------------------
.MIOS_AIN_DeadbandSet code MIOS_AIN_DeadbandSet
_MIOS_AIN_DeadbandSet
	global	_MIOS_AIN_DeadbandSet

;; --------------------------------------------------------------------------
.MIOS_AIN_Muxed code MIOS_AIN_Muxed
_MIOS_AIN_Muxed
	global	_MIOS_AIN_Muxed

;; --------------------------------------------------------------------------
.MIOS_AIN_NumberGet code MIOS_AIN_NumberGet
_MIOS_AIN_NumberGet
	global	_MIOS_AIN_NumberGet

;; --------------------------------------------------------------------------
.MIOS_AIN_NumberSet code MIOS_AIN_NumberSet
_MIOS_AIN_NumberSet
	global	_MIOS_AIN_NumberSet

;; --------------------------------------------------------------------------
.MIOS_AIN_Pin7bitGet code MIOS_AIN_Pin7bitGet
_MIOS_AIN_Pin7bitGet
	global	_MIOS_AIN_Pin7bitGet

;; --------------------------------------------------------------------------
.MIOS_AIN_PinGet code
_MIOS_AIN_PinGet
	global	_MIOS_AIN_PinGet

	call	MIOS_AIN_PinGet
	movff	MIOS_PARAMETER2, PRODL	; high byte -> PRODL
	return				; low byte already in WREG

;; --------------------------------------------------------------------------
.MIOS_AIN_PinLSBGet code MIOS_AIN_PinLSBGet
_MIOS_AIN_PinLSBGet
	global	_MIOS_AIN_PinLSBGet

;; --------------------------------------------------------------------------
.MIOS_AIN_PinMSBGet code MIOS_AIN_PinMSBGet
_MIOS_AIN_PinMSBGet
	global	_MIOS_AIN_PinMSBGet

;; --------------------------------------------------------------------------
.MIOS_AIN_UnMuxed code MIOS_AIN_UnMuxed
_MIOS_AIN_UnMuxed
	global	_MIOS_AIN_UnMuxed

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_DutyCycleDownGet code MIOS_MF_PWM_DutyCycleDownGet
_MIOS_MF_PWM_DutyCycleDownGet
	global	_MIOS_MF_PWM_DutyCycleDownGet

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_DutyCycleDownSet code MIOS_MF_PWM_DutyCycleDownSet
_MIOS_MF_PWM_DutyCycleDownSet
	global	_MIOS_MF_PWM_DutyCycleDownSet

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_DutyCycleUpGet code MIOS_MF_PWM_DutyCycleUpGet
_MIOS_MF_PWM_DutyCycleUpGet
	global	_MIOS_MF_PWM_DutyCycleUpGet

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_DutyCycleUpSet code MIOS_MF_PWM_DutyCycleUpSet
_MIOS_MF_PWM_DutyCycleUpSet
	global	_MIOS_MF_PWM_DutyCycleUpSet

;; --------------------------------------------------------------------------
.MIOS_MF_DeadbandGet code MIOS_MF_DeadbandGet
_MIOS_MF_DeadbandGet
	global	_MIOS_MF_DeadbandGet

;; --------------------------------------------------------------------------
.MIOS_MF_DeadbandSet code MIOS_MF_DeadbandSet
_MIOS_MF_DeadbandSet
	global	_MIOS_MF_DeadbandSet

;; --------------------------------------------------------------------------
.MIOS_MF_Disable code MIOS_MF_Disable
_MIOS_MF_Disable
	global	_MIOS_MF_Disable

;; --------------------------------------------------------------------------
.MIOS_MF_Enable code MIOS_MF_Enable
_MIOS_MF_Enable
	global	_MIOS_MF_Enable

;; --------------------------------------------------------------------------
.MIOS_MF_FaderMove code
_MIOS_MF_FaderMove
	global	_MIOS_MF_FaderMove

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
 	movff	PREINC2, MIOS_PARAMETER2
	goto	MIOS_MF_FaderMove

;; --------------------------------------------------------------------------
.MIOS_MF_SuspendDisable code MIOS_MF_SuspendDisable
_MIOS_MF_SuspendDisable
	global	_MIOS_MF_SuspendDisable

;; --------------------------------------------------------------------------
.MIOS_MF_SuspendEnable code MIOS_MF_SuspendEnable
_MIOS_MF_SuspendEnable
	global	_MIOS_MF_SuspendEnable

;; --------------------------------------------------------------------------
.MIOS_MF_SuspendGet code MIOS_MF_SuspendGet
_MIOS_MF_SuspendGet
	global	_MIOS_MF_SuspendGet

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_PeriodGet code MIOS_MF_PWM_PeriodGet
_MIOS_MF_PWM_PeriodGet
	global	_MIOS_MF_PWM_PeriodGet

;; --------------------------------------------------------------------------
.MIOS_MF_PWM_PeriodSet code MIOS_MF_PWM_PeriodSet
_MIOS_MF_PWM_PeriodSet
	global	_MIOS_MF_PWM_PeriodSet

;; --------------------------------------------------------------------------
.MIOS_MF_TouchDetectionReset code MIOS_MF_TouchDetectionReset
_MIOS_MF_TouchDetectionReset
	global	_MIOS_MF_TouchDetectionReset

;; --------------------------------------------------------------------------
.MIOS_DIN_PinGet code MIOS_DIN_PinGet
_MIOS_DIN_PinGet
	global	_MIOS_DIN_PinGet

;; --------------------------------------------------------------------------
.MIOS_DIN_SRGet code MIOS_DIN_SRGet
_MIOS_DIN_SRGet
	global	_MIOS_DIN_SRGet

;; --------------------------------------------------------------------------
.MIOS_DOUT_PinGet code MIOS_DOUT_PinGet
_MIOS_DOUT_PinGet
	global	_MIOS_DOUT_PinGet

;; --------------------------------------------------------------------------
.MIOS_DOUT_PinSet code
_MIOS_DOUT_PinSet
	global	_MIOS_DOUT_PinSet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_DOUT_PinSet


;; --------------------------------------------------------------------------
.MIOS_DOUT_PinSet0 code MIOS_DOUT_PinSet0
_MIOS_DOUT_PinSet0
	global	_MIOS_DOUT_PinSet0

;; --------------------------------------------------------------------------
.MIOS_DOUT_PinSet1 code MIOS_DOUT_PinSet1
_MIOS_DOUT_PinSet1
	global	_MIOS_DOUT_PinSet1

;; --------------------------------------------------------------------------
.MIOS_DOUT_SRGet code MIOS_DOUT_SRGet
_MIOS_DOUT_SRGet
	global	_MIOS_DOUT_SRGet

;; --------------------------------------------------------------------------
.MIOS_DOUT_SRSet code
_MIOS_DOUT_SRSet
	global	_MIOS_DOUT_SRSet
	
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_DOUT_SRSet

;; --------------------------------------------------------------------------
.MIOS_ENC_Abs7bitAdd code
_MIOS_ENC_Abs7bitAdd
	global	_MIOS_ENC_Abs7bitAdd
	
	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_ENC_Abs7bitAdd
	
;; --------------------------------------------------------------------------
.MIOS_ENC_Abs7bitGet code MIOS_ENC_Abs7bitGet
_MIOS_ENC_Abs7bitGet
	global	_MIOS_ENC_Abs7bitGet

;; --------------------------------------------------------------------------
.MIOS_ENC_Abs7bitSet code
_MIOS_ENC_Abs7bitSet
	global	_MIOS_ENC_Abs7bitSet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_ENC_Abs7bitSet

;; --------------------------------------------------------------------------
.MIOS_ENC_NumberGet code MIOS_ENC_NumberGet
_MIOS_ENC_NumberGet
	global	_MIOS_ENC_NumberGet

;; --------------------------------------------------------------------------
.MIOS_ENC_SpeedGet code
_MIOS_ENC_SpeedGet
	global	_MIOS_ENC_SpeedGet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_ENC_SpeedGet

;; --------------------------------------------------------------------------
.MIOS_ENC_SpeedSet code
_MIOS_ENC_SpeedSet
	global	_MIOS_ENC_SpeedSet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
 	movff	PREINC2, MIOS_PARAMETER2
	goto	MIOS_ENC_SpeedSet

;; --------------------------------------------------------------------------
.MIOS_SRIO_NumberGet code MIOS_SRIO_NumberGet
_MIOS_SRIO_NumberGet
	global	_MIOS_SRIO_NumberGet

;; --------------------------------------------------------------------------
.MIOS_SRIO_NumberSet code MIOS_SRIO_NumberSet
_MIOS_SRIO_NumberSet
	global	_MIOS_SRIO_NumberSet

;; --------------------------------------------------------------------------
.MIOS_SRIO_TS_SensitivityGet code MIOS_SRIO_TS_SensitivityGet
_MIOS_SRIO_TS_SensitivityGet
	global	_MIOS_SRIO_TS_SensitivityGet

;; --------------------------------------------------------------------------
.MIOS_SRIO_TS_SensitivitySet code MIOS_SRIO_TS_SensitivitySet
_MIOS_SRIO_TS_SensitivitySet
	global	_MIOS_SRIO_TS_SensitivitySet

;; --------------------------------------------------------------------------
.MIOS_SRIO_UpdateFrqGet code MIOS_SRIO_UpdateFrqGet
_MIOS_SRIO_UpdateFrqGet
	global	_MIOS_SRIO_UpdateFrqGet

;; --------------------------------------------------------------------------
.MIOS_SRIO_UpdateFrqSet code MIOS_SRIO_UpdateFrqSet
_MIOS_SRIO_UpdateFrqSet
	global	_MIOS_SRIO_UpdateFrqSet

;; --------------------------------------------------------------------------
.MIOS_SRIO_DebounceGet code MIOS_SRIO_DebounceGet
_MIOS_SRIO_DebounceGet
	global	_MIOS_SRIO_DebounceGet

;; --------------------------------------------------------------------------
.MIOS_SRIO_DebounceSet code MIOS_SRIO_DebounceSet
_MIOS_SRIO_DebounceSet
	global	_MIOS_SRIO_DebounceSet

;; --------------------------------------------------------------------------
.MIOS_LCD_Clear code MIOS_LCD_Clear
_MIOS_LCD_Clear
	global	_MIOS_LCD_Clear

;; --------------------------------------------------------------------------
.MIOS_LCD_Cmd code MIOS_LCD_Cmd
_MIOS_LCD_Cmd
	global	_MIOS_LCD_Cmd

;; --------------------------------------------------------------------------
.MIOS_LCD_CursorGet code MIOS_LCD_CursorGet
_MIOS_LCD_CursorGet
	global	_MIOS_LCD_CursorGet

;; --------------------------------------------------------------------------
.MIOS_LCD_CursorSet code MIOS_LCD_CursorSet
_MIOS_LCD_CursorSet
	global	_MIOS_LCD_CursorSet

;; --------------------------------------------------------------------------
.MIOS_LCD_Data code MIOS_LCD_Data
_MIOS_LCD_Data
	global	_MIOS_LCD_Data

;; --------------------------------------------------------------------------
.MIOS_LCD_Init code MIOS_LCD_Init
_MIOS_LCD_Init
	global	_MIOS_LCD_Init

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintBCD1 code MIOS_LCD_PrintBCD1
_MIOS_LCD_PrintBCD1
	global	_MIOS_LCD_PrintBCD1

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintBCD2 code MIOS_LCD_PrintBCD2
_MIOS_LCD_PrintBCD2
	global	_MIOS_LCD_PrintBCD2

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintBCD3 code MIOS_LCD_PrintBCD3
_MIOS_LCD_PrintBCD3
	global	_MIOS_LCD_PrintBCD3

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintBCD4 code
_MIOS_LCD_PrintBCD4
	global	_MIOS_LCD_PrintBCD4

	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_LCD_PrintBCD4

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintBCD5 code
_MIOS_LCD_PrintBCD5
	global	_MIOS_LCD_PrintBCD5

	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_LCD_PrintBCD5

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintChar code MIOS_LCD_PrintChar
_MIOS_LCD_PrintChar
	global	_MIOS_LCD_PrintChar

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintHex1 code MIOS_LCD_PrintHex1
_MIOS_LCD_PrintHex1
	global	_MIOS_LCD_PrintHex1

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintHex2 code MIOS_LCD_PrintHex2
_MIOS_LCD_PrintHex2
	global	_MIOS_LCD_PrintHex2

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintMessage code
_MIOS_LCD_PrintMessage
	global	_MIOS_LCD_PrintMessage

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	goto	MIOS_LCD_PrintMessage

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintPreconfString code
_MIOS_LCD_PrintPreconfString
	global	_MIOS_LCD_PrintPreconfString

	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRU
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRL
	goto	MIOS_LCD_PrintPreconfString

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintString code
_MIOS_LCD_PrintString
	global	_MIOS_LCD_PrintString

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	goto	MIOS_LCD_PrintString

;; --------------------------------------------------------------------------
.MIOS_LCD_TypeAutoSet code MIOS_LCD_TypeAutoSet
_MIOS_LCD_TypeAutoSet
	global	_MIOS_LCD_TypeAutoSet

;; --------------------------------------------------------------------------
.MIOS_LCD_TypeGet code MIOS_LCD_TypeGet
_MIOS_LCD_TypeGet
	global	_MIOS_LCD_TypeGet

;; --------------------------------------------------------------------------
.MIOS_LCD_TypeSet code
_MIOS_LCD_TypeSet
	global	_MIOS_LCD_TypeSet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
 	movff	PREINC2, MIOS_PARAMETER2
	goto	MIOS_LCD_TypeSet

;; --------------------------------------------------------------------------
.MIOS_LCD_YAddressGet code
_MIOS_LCD_YAddressGet
	global	_MIOS_LCD_YAddressGet
	goto	MIOS_LCD_YAddressGet

;; --------------------------------------------------------------------------
.MIOS_LCD_YAddressSet code
_MIOS_LCD_YAddressSet
	global	_MIOS_LCD_YAddressSet

	movff	FSR0L, FSR2L
	movwf	MIOS_PARAMETER1
 	movff	PREINC2, MIOS_PARAMETER2
 	movff	PREINC2, MIOS_PARAMETER3
	movf	PREINC2, W
	goto	MIOS_LCD_YAddressSet

;; --------------------------------------------------------------------------
.MIOS_LCD_MessageStart code MIOS_LCD_MessageStart
_MIOS_LCD_MessageStart
	global	_MIOS_LCD_MessageStart

;; --------------------------------------------------------------------------
.MIOS_LCD_MessageStop code MIOS_LCD_MessageStop
_MIOS_LCD_MessageStop
	global	_MIOS_LCD_MessageStop

;; --------------------------------------------------------------------------
.MIOS_CLCD_SpecialCharInit code
_MIOS_CLCD_SpecialCharInit
	global	_MIOS_CLCD_SpecialCharInit

	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRL
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	goto	MIOS_CLCD_SpecialCharInit

;; --------------------------------------------------------------------------
.MIOS_CLCD_SpecialCharsInit code
_MIOS_CLCD_SpecialCharsInit
	global	_MIOS_CLCD_SpecialCharsInit

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	goto	MIOS_CLCD_SpecialCharsInit

;; --------------------------------------------------------------------------
.MIOS_GLCD_FontInit code
_MIOS_GLCD_FontInit
	global	_MIOS_GLCD_FontInit

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	goto	MIOS_GLCD_FontInit

;; --------------------------------------------------------------------------
.MIOS_GLCD_GCursorGet code MIOS_GLCD_GCursorGet
_MIOS_GLCD_GCursorGet
	global	_MIOS_GLCD_GCursorGet

;; --------------------------------------------------------------------------
.MIOS_GLCD_GCursorSet code
_MIOS_GLCD_GCursorSet
	global	_MIOS_GLCD_GCursorSet

	movff	FSR0L, FSR2L
 	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_GLCD_GCursorSet

;; --------------------------------------------------------------------------
.MIOS_EEPROM_Read code
_MIOS_EEPROM_Read
	global	_MIOS_EEPROM_Read

	movwf	EEADR
	goto	MIOS_EEPROM_Read

;; --------------------------------------------------------------------------
.MIOS_EEPROM_Write code
_MIOS_EEPROM_Write
	global	_MIOS_EEPROM_Write

	movwf	EEADR
	movff	FSR0L, FSR2L
	movf	PREINC2, W
	goto	MIOS_EEPROM_Write

;; --------------------------------------------------------------------------
.MIOS_EEPROM_ReadPage code
_MIOS_EEPROM_ReadPage
	global	_MIOS_EEPROM_ReadPage

	movwf	EEADR
	movff	FSR0L, FSR2L
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	EEADR, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_EEPROM_ReadPage

;; --------------------------------------------------------------------------
.MIOS_EEPROM_WritePage code
_MIOS_EEPROM_WritePage
	global	_MIOS_EEPROM_WritePage

	movwf	EEADR
	movff	FSR0L, FSR2L
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	EEADR, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_EEPROM_WritePage

;; --------------------------------------------------------------------------
.MIOS_FLASH_Read code
_MIOS_FLASH_Read
	global	_MIOS_FLASH_Read

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	TBLPTRL, ACCESS
	return

	movlw	0x78
	cpfseq	TBLPTRH, ACCESS
	return

	movlw	0x00
	cpfseq	TBLPTRU, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_FLASH_Read

;; --------------------------------------------------------------------------
.MIOS_FLASH_Write code
_MIOS_FLASH_Write
	global	_MIOS_FLASH_Write

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movff	PREINC2, TBLPTRU
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	TBLPTRL, ACCESS
	return

	movlw	0x78
	cpfseq	TBLPTRH, ACCESS
	return

	movlw	0x00
	cpfseq	TBLPTRU, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_FLASH_Write

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_Read code
_MIOS_BANKSTICK_Read
	global	_MIOS_BANKSTICK_Read

	movwf	MIOS_PARAMETER1
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER2

#if PARAMETER_PASSING_TEST
	movlw	0x12
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x34
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif
	goto	MIOS_BANKSTICK_Read

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_Write code
_MIOS_BANKSTICK_Write
	global	_MIOS_BANKSTICK_Write

	movwf	MIOS_PARAMETER1
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER2
	movf	PREINC2, W

#if PARAMETER_PASSING_TEST
	movwf	TMP1
	movlw	0x56
	cpfseq	TMP1, ACCESS
	return

	movlw	0x12
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x34
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_BANKSTICK_Write

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_CtrlSet code MIOS_BANKSTICK_CtrlSet
_MIOS_BANKSTICK_CtrlSet
	global	_MIOS_BANKSTICK_CtrlSet

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_CtrlGet code MIOS_BANKSTICK_CtrlGet
_MIOS_BANKSTICK_CtrlGet
	global	_MIOS_BANKSTICK_CtrlGet

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_WritePage code
_MIOS_BANKSTICK_WritePage
	global	_MIOS_BANKSTICK_WritePage

	movwf	MIOS_PARAMETER1
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER2
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x78
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_BANKSTICK_WritePage

;; --------------------------------------------------------------------------
.MIOS_BANKSTICK_ReadPage code
_MIOS_BANKSTICK_ReadPage
	global	_MIOS_BANKSTICK_ReadPage
	
	movwf	MIOS_PARAMETER1
	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER2
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H

#if PARAMETER_PASSING_TEST
	movlw	0x56
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x78
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x12
	cpfseq	FSR1L, ACCESS
	return

	movlw	0x03
	cpfseq	FSR1H, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_BANKSTICK_ReadPage

;; --------------------------------------------------------------------------
.MIOS_TIMER_Init code
_MIOS_TIMER_Init
	global	_MIOS_TIMER_Init

	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	movff	PREINC2, MIOS_PARAMETER2

#if PARAMETER_PASSING_TEST
	movwf	TMP1
	movlw	0x56
	cpfseq	TMP1, ACCESS
	return

	movlw	0x12
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x34
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_TIMER_Init

;; --------------------------------------------------------------------------
.MIOS_TIMER_Start code MIOS_TIMER_Start
_MIOS_TIMER_Start
	global	_MIOS_TIMER_Start

;; --------------------------------------------------------------------------
.MIOS_TIMER_Stop code MIOS_TIMER_Stop
_MIOS_TIMER_Stop
	global	_MIOS_TIMER_Stop

;; --------------------------------------------------------------------------
.MIOS_TIMER_ReInit code
_MIOS_TIMER_ReInit
	global	_MIOS_TIMER_ReInit

	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	movff	PREINC2, MIOS_PARAMETER2

#if PARAMETER_PASSING_TEST
	movwf	TMP1
	movlw	0x56
	cpfseq	TMP1, ACCESS
	return

	movlw	0x12
	cpfseq	MIOS_PARAMETER1, ACCESS
	return

	movlw	0x34
	cpfseq	MIOS_PARAMETER2, ACCESS
	return

	movlw	0x1a
	movwf	MIOS_PARAMETER1
	return
#endif

	goto	MIOS_TIMER_ReInit

;; --------------------------------------------------------------------------
.MIOS_HLP_GetBitANDMask code MIOS_HLP_GetBitANDMask
_MIOS_HLP_GetBitANDMask
	global	_MIOS_HLP_GetBitANDMask

;; --------------------------------------------------------------------------
.MIOS_HLP_GetBitORMask code MIOS_HLP_GetBitORMask
_MIOS_HLP_GetBitORMask
	global	_MIOS_HLP_GetBitORMask

;; --------------------------------------------------------------------------
.MIOS_HLP_16bitAddSaturate code
_MIOS_HLP_16bitAddSaturate
	global	_MIOS_HLP_16bitAddSaturate

	movff	FSR0L, FSR2L
	movwf	TMP5

	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H
	movf	PREINC2, F
	movff	PREINC2, MIOS_PARAMETER1
	movff	PREINC2, MIOS_PARAMETER2

	movff	POSTINC1, TMP1	; due to requirement that FSR1 is aligned to 16bit address
	movff	POSTINC1, TMP2
	lfsr	FSR1, TMP1

	movf	TMP5, W
	call	MIOS_HLP_16bitAddSaturate
	movff	FSR0L, FSR2L
	movff	PREINC2, FSR1L
	movff	PREINC2, FSR1H
	movff	TMP1, POSTINC1
	movff	TMP2, POSTINC1
	return

;; --------------------------------------------------------------------------
.MIOS_HLP_Dec2BCD code
_MIOS_HLP_Dec2BCD
	global	_MIOS_HLP_Dec2BCD

	movff	FSR0L, FSR2L
	movff	PREINC2, MIOS_PARAMETER1
	goto	MIOS_HLP_Dec2BCD

;; --------------------------------------------------------------------------
.MIOS_Delay code MIOS_Delay
_MIOS_Delay
	global	_MIOS_Delay

;; --------------------------------------------------------------------------
.MIOS_GPCounterGet code MIOS_GPCounterGet
_MIOS_GPCounterGet
	global	_MIOS_GPCounterGet

;; --------------------------------------------------------------------------
.MIOS_Reset code MIOS_Reset
_MIOS_Reset
	global	_MIOS_Reset

;; --------------------------------------------------------------------------
.MIOS_SystemResume code MIOS_SystemResume
_MIOS_SystemResume
	global	_MIOS_SystemResume

;; --------------------------------------------------------------------------
.MIOS_SystemSuspend code MIOS_SystemSuspend
_MIOS_SystemSuspend
	global	_MIOS_SystemSuspend

;; --------------------------------------------------------------------------
.MIOS_UserResume code MIOS_UserResume
_MIOS_UserResume
	global	_MIOS_UserResume

;; --------------------------------------------------------------------------
.MIOS_UserSuspend code MIOS_UserSuspend
_MIOS_UserSuspend
	global	_MIOS_UserSuspend

;; --------------------------------------------------------------------------
.MIOS_DIN_PinAutoRepeatEnable code MIOS_DIN_PinAutoRepeatEnable
_MIOS_DIN_PinAutoRepeatEnable
	global	_MIOS_DIN_PinAutoRepeatEnable

;; --------------------------------------------------------------------------
.MIOS_DIN_PinAutoRepeatDisable code MIOS_DIN_PinAutoRepeatDisable
_MIOS_DIN_PinAutoRepeatDisable
	global	_MIOS_DIN_PinAutoRepeatDisable

;; --------------------------------------------------------------------------
.MIOS_DIN_PinAutoRepeatGet code MIOS_DIN_PinAutoRepeatGet
_MIOS_DIN_PinAutoRepeatGet
	global	_MIOS_DIN_PinAutoRepeatGet

;; --------------------------------------------------------------------------
.MIOS_AIN_DynamicPrioSet code MIOS_AIN_DynamicPrioSet
_MIOS_AIN_DynamicPrioSet
	global	_MIOS_AIN_DynamicPrioSet

;; --------------------------------------------------------------------------
.MIOS_AIN_DynamicPrioGet code MIOS_AIN_DynamicPrioGet
_MIOS_AIN_DynamicPrioGet
	global	_MIOS_AIN_DynamicPrioGet

;; --------------------------------------------------------------------------
.MIOS_AIN_LastPinsGet code MIOS_AIN_LastPinsGet
_MIOS_AIN_LastPinsGet
	global	_MIOS_AIN_LastPinsGet

;; --------------------------------------------------------------------------
.MIOS_IIC_Start code MIOS_IIC_Start
_MIOS_IIC_Start
	global	_MIOS_IIC_Start

;; --------------------------------------------------------------------------
.MIOS_IIC_Stop code MIOS_IIC_Stop
_MIOS_IIC_Stop
	global	_MIOS_IIC_Stop

;; --------------------------------------------------------------------------
.MIOS_IIC_ByteSend code MIOS_IIC_ByteSend
_MIOS_IIC_ByteSend
	global	_MIOS_IIC_ByteSend

;; --------------------------------------------------------------------------
.MIOS_IIC_ByteReceive code MIOS_IIC_ByteReceive
_MIOS_IIC_ByteReceive
	global	_MIOS_IIC_ByteReceive

;; --------------------------------------------------------------------------
.MIOS_IIC_AckSend code MIOS_IIC_AckSend
_MIOS_IIC_AckSend
	global	_MIOS_IIC_AckSend

;; --------------------------------------------------------------------------
.MIOS_IIC_NakSend code MIOS_IIC_NakSend
_MIOS_IIC_NakSend
	global	_MIOS_IIC_NakSend

;; --------------------------------------------------------------------------
.MIOS_IIC_CtrlSet code MIOS_IIC_CtrlSet
_MIOS_IIC_CtrlSet
	global	_MIOS_IIC_CtrlSet

;; --------------------------------------------------------------------------
.MIOS_IIC_CtrlGet code MIOS_IIC_CtrlGet
_MIOS_IIC_CtrlGet
	global	_MIOS_IIC_CtrlGet


;; --------------------------------------------------------------------------
;; ==========================================================================
;;  Special functions supported by the wrapper
;; ==========================================================================
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
.MIOS_LCD_PrintCString code
_MIOS_LCD_PrintCString
	global	_MIOS_LCD_PrintCString

	movwf	TBLPTRL
	movff	FSR0L, FSR2L
	movff	PREINC2, TBLPTRH
	movf	PREINC2, W

	;; SDCC passes a 24bit pointer, the MSB (now in WREG) bit 6 and 7 tells us, where the data is located:
	;; 00 -> code
	;; 01 -> EEPROM (not supported by MIOS_LCD_PrintCString yet)
	;; 10 -> data
	;; 11 -> unimplemented

	btfsc	WREG, 7
	bra	_MIOS_LCD_PrintCString_RAM
_MIOS_LCD_PrintCString_Code
	movwf	TBLPTRU
_MIOS_LCD_PrintCStringLoop_Code
	tblrd*+
	movf	TABLAT, W
	bz	_MIOS_LCD_PrintCString_Code_End
	call	MIOS_LCD_PrintChar
	bra	_MIOS_LCD_PrintCStringLoop_Code
_MIOS_LCD_PrintCString_Code_End
	return

_MIOS_LCD_PrintCString_RAM
	;; using FSR2 as temporary pointer - store current FSR2 into TMP[12]
	movff	FSR2L, TMP1
	movff	FSR2H, TMP2
	movff	TBLPTRL, FSR2L
	movff	TBLPTRH, FSR2H
_MIOS_LCD_PrintCStringLoop_RAM
	movf	POSTINC2, W
	bz	_MIOS_LCD_PrintCString_RAM_End
	call	MIOS_LCD_PrintChar
	bra	_MIOS_LCD_PrintCStringLoop_RAM
_MIOS_LCD_PrintCString_RAM_End
	;; restore FSR2
	movff	TMP1, FSR2L
	movff	TMP2, FSR2H
	return

	END
