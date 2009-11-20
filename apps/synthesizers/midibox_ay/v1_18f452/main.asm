;; =========================================================================
;; main.asm
;;
;;                              MidiBox AY 3 8912 
;;                                  V1.00 RC1
; ==========================================================================

;; -------------------------------------------------------------------------
;; APPLICATION: MIDIBOX AY 3 8912
;; V1.00.pre01 (asm) / [13.11.2009]
;; CORE8, PIC 18F452 
;; For more Information see the MIDIbox AY 3 8912 Project Page:
;; http://www.midibox.org/dokuwiki/midibox_ay_3_8912
;; -------------------------------------------------------------------------

;; MidiBox AY 8912
;; By lemonhorse (2009)
;; MIOS & MBHP by TK
;; All rights by TK


;; ---[MIOS header file ]---
#include "mios.h"

;; ---[useful macros ]---
#include "macros.h"

;; ---[vectors to MIOS functions (never change!) ]---
#include "mios_vectors.inc"

;; ---[user hooks (never change!) ]---
#include "user_vectors.inc"

;; ==========================================================================
;;  General Application Settings
;; ==========================================================================

;; ---[include applicaion files]----------
#include "app_include.inc"


;; ==========================================================================
;;  All MIOS hooks in one file
;; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS after startup to initialize the 
;;  application
;; --------------------------------------------------------------------------
USER_Init
	;; initialize the scan matrix driver
	call	SM_Init

	;; initialize the shift registers
	movlw	16			; use up to 16 shift registers
	call	MIOS_SRIO_NumberSet
	
	;; SET C64 Keyboard I/O polling Frequency (DIN/DOUT) HERE
	movlw	0x01			; set update frequncy
	call	MIOS_SRIO_UpdateFrqSet
	
	movlw	0x00			; disable touch sensor
	call	MIOS_SRIO_TS_SensitivitySet

	;; --- INIT APLICATION REGS---------
	;;  SET AY 3 8912 (SHADOW) REGISTERS 
	;;  WITH DEFAULT VALUES
	;;  and ADJUST VARIABLES (initial values)
	;;  @ variables_ini.inc
	call  	INIT_VARIABLES

	;; MIDI MERGER Conf.
	call	INIT_MIDI_CONF

	;; set up timer0 FOR fx TIMER
	call INIT_TIMER0

	;; initialize the USER_Timer (for Fx Timmer polling)
	call INIT_USER_Timer
	

;; --------------------------------------------------------------------------
;;  This function is called by MIOS in the mainloop when nothing else is to do
;; --------------------------------------------------------------------------
USER_Tick
	;; SM DRIVER:
	;; call the scan matrix button handler
	call	SM_ButtonHandler

	;; call Fx Routines
	call 	FX_PIPELINE

	;; C64 Key Handling [For [+] / [-]]
	call 	CHECK_CONSTANT_KEY_PRESSING
		
			return


;; --------------------------------------------------------------------------
;;  This function is periodically called by MIOS. The frequency has to be
;;  initialized with MIOS_Timer_Set
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible!
;; --------------------------------------------------------------------------
USER_Timer
		
	;; Fx Timer call (polling)
	;; Function: INC FX TIMER +1 
	;; @ fx_timer.inc
	call 	FX_TIMER_POLLING
		
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

	call AY_START_SCREEN

		;; -------[appl. main cycle start]---
		;; JUMP TO MAIN APPLICATION CYCLE HERE
		;; @ main_menue_functions.inc
		goto LABEL_MAIN_MENUE_SWITCH

;; --------------------------------------------------------------------------
;;  This function is called in the mainloop when no temporary message is shown
;;  on screen. Print the realtime messages here
;; --------------------------------------------------------------------------
USER_DISPLAY_Tick
	;; REDRAW MAIN MENU IF A Control Change (CC, etc.)
	;; OR MOD WHEEL / PITCH BEND
	;; MATCH THE CURRENT MAIN MENU
	call	CHECK_EVENT_MENU_REDRAW

		return
	


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a complete MIDI event has been received
;;  Input:
;;     o first  MIDI event byte in MIOS_PARAMETER1
;;     o second MIDI event byte in MIOS_PARAMETER2
;;     o third  MIDI event byte in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedEvent
	
	; => MIDI_FUNCTIONS.INC
	goto LABEL_MIDI_HANDLING_START

	      return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI event has been received
;;  which has been specified in the CONFIG_MIDI_IN table (mios_tables.inc)
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
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a MIDI byte has been received
;;  Input:
;;     o received MIDI byte in WREG and MIOS_PARAMETER1
;; --------------------------------------------------------------------------
USER_MPROC_NotifyReceivedByte
	return

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
;; DONT USE USER_DIN_NotifyToggle BECAUSE OF THE
;; SM DRIVER!
	return


SM_C64_KEY_NotifyToggle
   ;;  ----------------------------------------------------------------
   ;;  JUNMPED IN HERE FROM THE 8x8 SCAN MATRIX DRIVER
   ;;  [THE C64 KEYBOARD DRIVER]
   ;;  A KEY ON THE C64 KEYBOARD HAS BEEN PRESSED
   ;;  JUMPED IN HERE FROM FUNCTION: SM_NotifyToggle
   ;;  DESCRIPTION: This function is NOT called by MIOS, but by the scan matrix
   ;;  handler when a pin of the scan matrix has been toggled
   ;;  Button number in SM_DRIVER_KEYBOARD_KEY
   ;;  Button value in AY_KEY_STATE:
   ;;  => 1 if button has been released (=5V)
   ;;  => 0 if button has been pressed  (=0V)
   		
   		;; Start POINT of MAIN MENU ROUTINES
   		;; and SUB MENU ROUTINES
       		;; => get_key.inc
       		goto  GET_KEY
      


;; --------------------------------------------------------------------------
;;  This function is called by MIOS when an encoder has been moved
;;  Input:
;;     o Encoder number in WREG and MIOS_PARAMETER1
;;     o signed incrementer value in MIOS_PARAMETER2:
;;       - is positive when encoder has been turned clockwise
;;       - is negative when encoder has been turned counter clockwise
;; --------------------------------------------------------------------------
USER_ENC_NotifyChange
	return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS before the shift register are loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Prepare
	;; call the scan matrix driver
	call	SM_PrepareCol
		return


;; --------------------------------------------------------------------------
;;  This function is called by MIOS after the shift register have been loaded
;;  Note that this is an interrupt service routine! Use FSR2 instead of FSR0
;;  and IRQ_TMPx instead of TMPx -- and make the routine as fast as possible
;; --------------------------------------------------------------------------
USER_SR_Service_Finish
	;; call the scan matrix driver
	call	SM_GetRow
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

	

	END



	