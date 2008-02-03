; $Id$
;
; This file includes helpful functions to monitor the MIDI Rx/Tx activity
;
;    o MIDI_RXTX_NotifyRx:    to be called when a MIDI byte has been received
;    o MIDI_RXTX_NotifyTx:    to be called when a MIDI byte has been sent
;    o MIDI_RXTX_Handler:     to be called from USER_SR_Service_Prepare
;
; See the function headers for further details
;
; Settings which can be modified:
;
; Number of update cycles (* 1mS) the LEDs will stay active on a Rx/Tx event
#define MIDI_RXTX_LED_DELAY 15
; DOUT pin number of Rx LED
#define MIDI_RXTX_RX_LED    DEFAULT_MIDI_RX_LED
; DOUT pin number of Tx LED
#define MIDI_RXTX_TX_LED    DEFAULT_MIDI_TX_LED
;
; Following registers have to be located to free addresses in app_defines.h:
;
;MIDI_RXTX_RX_CTR	EQU	0x019
;MIDI_RXTX_TX_CTR	EQU	0x01a
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_NotifyRx
;;  DESCRIPTION: this function has to be called from the USER_MIDI_NotifyRx
;;  hook when a MIDI event has been received to reload the RX counter
;; --------------------------------------------------------------------------
MIDI_RXTX_NotifyRx
	movlw	MIDI_RXTX_LED_DELAY
	SET_BSR	MIDI_RXTX_RX_CTR
	movwf	MIDI_RXTX_RX_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_NotifyTx
;;  DESCRIPTION: this function has to be called from the USER_MIDI_NotifyTx
;;  hook when a MIDI event will be transmitted to reload the TX counter
;; --------------------------------------------------------------------------
MIDI_RXTX_NotifyTx
	movlw	MIDI_RXTX_LED_DELAY
	SET_BSR	MIDI_RXTX_TX_CTR
	movwf	MIDI_RXTX_TX_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_Handler
;;  DESCRIPTION: this function has to be called from the USER_SR_ServicePrpeare
;;  hook, it decrements the Rx/Tx counters and sets the LEDs depending on the
;;  counter values
;; --------------------------------------------------------------------------
MIDI_RXTX_Handler
#if DEFAULT_MIDI_MONITOR_ENABLED
	;; Decrement Rx counter if != 0
	SET_BSR	MIDI_RXTX_RX_CTR
	movf	MIDI_RXTX_RX_CTR, W, BANKED
	skpz
	decf	MIDI_RXTX_RX_CTR, F, BANKED

	;; Decrement Tx counter if != 0
	SET_BSR	MIDI_RXTX_TX_CTR
	movf	MIDI_RXTX_TX_CTR, W, BANKED
	skpz
	decf	MIDI_RXTX_TX_CTR, F, BANKED


	;; 
	;; remove the code below if you don't want to use LEDs to
	;; indicate the counter state
	;; 

	;; set the Rx LED depending on counter state
	SET_BSR	MIDI_RXTX_RX_CTR
	movf	MIDI_RXTX_RX_CTR, W, BANKED
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movlw	MIDI_RXTX_RX_LED
	call	MIOS_DOUT_PinSet

	;; set the Tx LED depending on counter state
	SET_BSR	MIDI_RXTX_TX_CTR
	movf	MIDI_RXTX_TX_CTR, W, BANKED
	skpz
	movlw	0x01
	movwf	MIOS_PARAMETER1
	movlw	MIDI_RXTX_TX_LED
	call	MIOS_DOUT_PinSet
#endif
	return
