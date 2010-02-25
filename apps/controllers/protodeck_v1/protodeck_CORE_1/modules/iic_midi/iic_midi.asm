; $Id: iic_midi.asm 146 2008-03-01 00:04:43Z tk $
;
;  Wrapper for iic_midi.inc
;  To be used for relocatable programs
;

; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	IIC_MIDI_Init
	global	IIC_MIDI_Receive
	global	IIC_MIDI_Transmit
	global	IIC_MIDI_TransmitStart
	global	IIC_MIDI_Transmit
	global	IIC_MIDI_TransmitStop

	global	_IIC_MIDI_Init		; (for C, declaration in iic_midi.h)
	global	_IIC_MIDI_Receive
	global	_IIC_MIDI_Transmit
	global	_IIC_MIDI_TransmitStart
	global	_IIC_MIDI_TransmitByte
	global	_IIC_MIDI_TransmitStop

	global	_iic_midi_available_slaves
	global	_iic_midi_slave

	global	_iic_midi_rx_package
	global	_iic_midi_tx_package

; ==========================================================================
;  Declare variables 
; ==========================================================================

accessram		udata_acs
_iic_midi_available_slaves res	1
_iic_midi_slave		res	1

IIC_MIDI_TMP		res	1
IIC_MIDI_TX_RETRY_CTR	res	1
IIC_MIDI_BYTES_TO_SEND	res	1

_iic_midi_rx_package	res	4
_iic_midi_tx_package	res	4

; ==========================================================================
IIC_MIDI code
#include <iic_midi.inc>

; ==========================================================================
	;; C function wrappers

;_IIC_MIDI_Init		; (no parameters)

_IIC_MIDI_Receive
	movwf	_iic_midi_slave
	rgoto	IIC_MIDI_Receive

_IIC_MIDI_Transmit
	movwf	_iic_midi_slave

	movff	FSR0L, FSR2L		; get package content from stack
	movff	PREINC2, _iic_midi_tx_package+0
	movff	PREINC2, _iic_midi_tx_package+1
	movff	PREINC2, _iic_midi_tx_package+2
	movff	PREINC2, _iic_midi_tx_package+3
	rgoto	IIC_MIDI_Transmit

_IIC_MIDI_TransmitStart
	movwf	_iic_midi_slave
	rgoto	IIC_MIDI_TransmitStart

;_IIC_MIDI_TransmitByte		; (direct parameter passing)

;_IIC_MIDI_TransmitStop		; (no parameters)

; ==========================================================================
	END
