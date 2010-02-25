; $Id: midi_rxtx_leds.asm 50 2008-01-30 21:47:50Z tk $
;
; This file includes helpful functions to monitor the MIDI Rx/Tx activity
; --> IO PIN VERSION (no DOUT required) <--
;
;    o _MIDI_RXTX_LEDS_NotifyRx:    to be called when a MIDI byte has been received
;    o _MIDI_RXTX_LEDS_NotifyTx:    to be called when a MIDI byte has been sent
;    o _MIDI_RXTX_LEDS_Handler:     to be called from a timer, e.g. USER_SR_Service_Prepare
;
; In addition, _MIDI_NotifyRx and _MIDI_NotifyTx is defined here
; These hooks are called by the MIOS wrapper on incoming/outgoing MIDI bytes
; They have to be enabled in the Makefile by adding:
;   -DENABLE_MIDI_NOTIFY_RX -DENABLE_MIDI_NOTIFY_TX
; to IIC_MIDI_DEFINES
;
; See the function headers for additional details
;
; ==========================================================================
; Compile time options (can be added via -D<name>=<value>
;   MIDI_RXTX_LEDS_DELAY: defines how long a LED is active after byte has been received/transmitted
;   MIDI_RXTX_LEDS_RX_LAT: port name of Rx LED (LATA, LATB, LATC, LATD, ...)
;   MIDI_RXTX_LEDS_RX_PIN: port name of Rx LED (0, 1, ... 7)
;   MIDI_RXTX_LEDS_TX_LAT: port name of Tx LED (LATA, LATB, LATC, LATD, ...)
;   MIDI_RXTX_LEDS_TX_PIN: port name of Tx LED (0, 1, ... 7)
;   MIDI_RXTX_LEDS_NO_HOOKS: disable the _MIDI_NotifyRx and _MIDI_NotifyTx hooks
;                           (-> MIDI_RXTX_LEDS_NotifyRx/Tx need to be called from external!)
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================
; Settings which can be overruled from external via "-D<name>=<value>"
;
; Number of update cycles (* 1mS) the LEDs will stay active on a Rx/Tx event
#ifndef MIDI_RXTX_LEDS_DELAY
#define MIDI_RXTX_LEDS_DELAY 15
#endif

; pin number of Rx LED
#ifndef MIDI_RXTX_LEDS_RX_LAT
#define MIDI_RXTX_LEDS_RX_LAT LATC	; port RC0
#endif

#ifndef MIDI_RXTX_LEDS_RX_PIN
#define MIDI_RXTX_LEDS_RX_PIN 0
#endif

; pin number of Tx LED
#ifndef MIDI_RXTX_LEDS_TX_LAT
#define MIDI_RXTX_LEDS_TX_LAT LATC	; port RC1
#endif

#ifndef MIDI_RXTX_LEDS_TX_PIN
#define MIDI_RXTX_LEDS_TX_PIN 1
#endif
;
; ==========================================================================

	;; export lables
	global	_MIDI_RXTX_LEDS_NotifyRx
	global	_MIDI_RXTX_LEDS_NotifyTx
	global	_MIDI_RXTX_LEDS_Handler

#ifndef MIDI_RXTX_LEDS_NO_HOOKS
	global	_MIDI_NotifyRx		; for MIOS wrapper, has been enabled with -DENABLE_MIDI_NOTIFY_RX in Makefile
	global	_MIDI_NotifyTx		; for MIOS wrapper, has been enabled with -DENABLE_MIDI_NOTIFY_TX in Makefile
#endif

; ==========================================================================

midi_rxtx_data		udata
MIDI_RXTX_LEDS_RX_CTR	res	1
MIDI_RXTX_LEDS_TX_CTR	res	1

; ==========================================================================
MIDI_RXTX_LEDS code
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_LEDS_NotifyRx
;;  DESCRIPTION: this function has to be called from the USER_MIDI_NotifyRx
;;  hook when a MIDI event has been received to reload the RX counter
;; --------------------------------------------------------------------------
_MIDI_RXTX_LEDS_NotifyRx
#ifndef MIDI_RXTX_LEDS_NO_HOOKS
_MIDI_NotifyRx			; for MIOS wrapper, hook has been enabled with -DENABLE_MIDI_NOTIFY_RX in Makefile
#endif
	;; called from mios_wrapper
	movlw	MIDI_RXTX_LEDS_DELAY
	SET_BSR	MIDI_RXTX_LEDS_RX_CTR
	movwf	MIDI_RXTX_LEDS_RX_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_LEDS_NotifyTx
;;  DESCRIPTION: this function has to be called from the USER_MIDI_NotifyTx
;;  hook when a MIDI event will be transmitted to reload the TX counter
;; --------------------------------------------------------------------------
_MIDI_RXTX_LEDS_NotifyTx
#ifndef MIDI_RXTX_LEDS_NO_HOOKS
_MIDI_NotifyTx			; for MIOS wrapper, has been enabled with -DENABLE_MIDI_NOTIFY_RX in Makefile
#endif
	movlw	MIDI_RXTX_LEDS_DELAY
	SET_BSR	MIDI_RXTX_LEDS_TX_CTR
	movwf	MIDI_RXTX_LEDS_TX_CTR, BANKED
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MIDI_RXTX_LEDS_Handler
;;  DESCRIPTION: this function has to be called from a timer, e.g. 
;;  the USER_SR_ServicePrpeare hook
;;  It decrements the Rx/Tx counters and sets the LEDs depending on the
;;  counter values
;; --------------------------------------------------------------------------
_MIDI_RXTX_LEDS_Handler
	;; Decrement Rx counter if != 0
	SET_BSR	MIDI_RXTX_LEDS_RX_CTR
	movf	MIDI_RXTX_LEDS_RX_CTR, W, BANKED
	skpz
	decf	MIDI_RXTX_LEDS_RX_CTR, F, BANKED

	;; Decrement Tx counter if != 0
	SET_BSR	MIDI_RXTX_LEDS_TX_CTR
	movf	MIDI_RXTX_LEDS_TX_CTR, W, BANKED
	skpz
	decf	MIDI_RXTX_LEDS_TX_CTR, F, BANKED


	;; 
	;; remove the code below if you don't want to use LEDs to
	;; indicate the counter state
	;; 

	;; set the Rx LED depending on counter state
	SET_BSR	MIDI_RXTX_LEDS_RX_CTR
	movf	MIDI_RXTX_LEDS_RX_CTR, W, BANKED
	bz	_MIDI_RXTX_LEDS_Handler_RxOff
_MIDI_RXTX_LEDS_Handler_RxOn
	bcf	MIDI_RXTX_LEDS_RX_LAT, MIDI_RXTX_LEDS_RX_PIN	; low-active
	rgoto	_MIDI_RXTX_LEDS_Handler_Rx_Cont
_MIDI_RXTX_LEDS_Handler_RxOff
	bsf	MIDI_RXTX_LEDS_RX_LAT, MIDI_RXTX_LEDS_RX_PIN	; low-active
_MIDI_RXTX_LEDS_Handler_Rx_Cont

	;; set the Tx LED depending on counter state
	SET_BSR	MIDI_RXTX_LEDS_TX_CTR
	movf	MIDI_RXTX_LEDS_TX_CTR, W, BANKED
	bz	_MIDI_RXTX_LEDS_Handler_TxOff
_MIDI_RXTX_LEDS_Handler_TxOn
	bcf	MIDI_RXTX_LEDS_TX_LAT, MIDI_RXTX_LEDS_TX_PIN	; low-active
	rgoto	_MIDI_RXTX_LEDS_Handler_Tx_Cont
_MIDI_RXTX_LEDS_Handler_TxOff
	bsf	MIDI_RXTX_LEDS_TX_LAT, MIDI_RXTX_LEDS_TX_PIN	; low-active
_MIDI_RXTX_LEDS_Handler_Tx_Cont
	return

END
