; $Id$
;
;  Assembly based IIC MIDI Tick Handler
;
; ==========================================================================
;
;  Copyright 2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

	radix	dec
#include <mios.h>
#include <mios_vectors.inc>
#include <macros.h>

; ==========================================================================

	;; export lables
	global	IIC_MIDI_Tick

	global	_IIC_MIDI_Tick		; (for C, declaration in iic_midi.h)

	;; import lables
	extern	_sysex_owner
	extern	_ROUTER_Rx_IIC0
	extern	_ROUTER_Rx_IIC1
	extern	_ROUTER_Rx_IIC2
	extern	_ROUTER_Rx_IIC3

	;; from $MIOS_PATH/modules/iic_midi.asm:
	extern	_IIC_MIDI_Receive

	extern	_iic_midi_rx_package
	extern	_iic_midi_tx_package

; ==========================================================================

;; assembly/C compatible defines
#include "router_ports.h"

; ==========================================================================

;; base address and max number of IIC slaves
#define IIC_MIDI_BASE_ADDR	0x10
#define IIC_MIDI_MAX_NUMBER	4

;; use Receive Interrupt lines for faster polling?
#define IIC_MIDI_USE_RI		1

; ==========================================================================
IIC_MIDI_TICK code
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: IIC_MIDI_Tick
;;  DECLARATION: void IIC_MIDI_Tick(void)
;;  DESCRIPTION: should be called from USER_Tick to check for incoming
;;  messages from the IIC MIDI slaves
;;  The global hook "_ROUTER_Rx_IIC[0123]" will be called once a complete
;;  package has been received - this function is defined in router.c
;;  IN:	  -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
_IIC_MIDI_Tick			; (for C, declaration in iic_midi.h)
IIC_MIDI_Tick
	bsf	LATD, 4

	;; exit if the internal USART is the SysEx owner
	SET_BSR	_sysex_owner
	movf	_sysex_owner, W, BANKED
	xorlw	PORT_INT0
	bz	IIC_MIDI_Tick_End

#if IIC_MIDI_MAX_NUMBER >= 1
IIC_MIDI_Tick_Slave0
	movlw	0x00
	call	_IIC_MIDI_Receive
	bz	IIC_MIDI_Tick_Slave0_End

	movff	_iic_midi_rx_package+3, POSTDEC0
	movff	_iic_midi_rx_package+2, POSTDEC0
	movff	_iic_midi_rx_package+1, POSTDEC0
	movf	_iic_midi_rx_package+0, W
	call	_ROUTER_Rx_IIC0
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)

IIC_MIDI_Tick_Slave0_End
	;; loop so long IIC0 is the SysEx owner
	;; TODO: add timeout
	clrwdt			; feed watchdog
	SET_BSR	_sysex_owner
	movf	_sysex_owner, W, BANKED
	xorlw	PORT_IIC0
	bz	IIC_MIDI_Tick_Slave0
#endif

#if IIC_MIDI_MAX_NUMBER >= 2
IIC_MIDI_Tick_Slave1
	movlw	0x01
	call	_IIC_MIDI_Receive
	bz	IIC_MIDI_Tick_Slave1_End

	movff	_iic_midi_rx_package+3, POSTDEC0
	movff	_iic_midi_rx_package+2, POSTDEC0
	movff	_iic_midi_rx_package+1, POSTDEC0
	movf	_iic_midi_rx_package+0, W
	call	_ROUTER_Rx_IIC1
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
IIC_MIDI_Tick_Slave1_End
	;; loop so long IIC1 is the SysEx owner
	;; TODO: add timeout
	clrwdt			; feed watchdog
	SET_BSR	_sysex_owner
	movf	_sysex_owner, W, BANKED
	xorlw	PORT_IIC1
	bz	IIC_MIDI_Tick_Slave1
#endif

#if IIC_MIDI_MAX_NUMBER >= 3
IIC_MIDI_Tick_Slave2
	movlw	0x02
	call	_IIC_MIDI_Receive
	bz	IIC_MIDI_Tick_Slave2_End

	movff	_iic_midi_rx_package+3, POSTDEC0
	movff	_iic_midi_rx_package+2, POSTDEC0
	movff	_iic_midi_rx_package+1, POSTDEC0
	movf	_iic_midi_rx_package+0, W
	call	_ROUTER_Rx_IIC2
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
IIC_MIDI_Tick_Slave2_End
	;; loop so long IIC2 is the SysEx owner
	;; TODO: add timeout
	clrwdt			; feed watchdog
	SET_BSR	_sysex_owner
	movf	_sysex_owner, W, BANKED
	xorlw	PORT_IIC2
	bz	IIC_MIDI_Tick_Slave2
#endif

#if IIC_MIDI_MAX_NUMBER >= 4
IIC_MIDI_Tick_Slave3
	movlw	0x03
	call	_IIC_MIDI_Receive
	bz	IIC_MIDI_Tick_Slave3_End

	movff	_iic_midi_rx_package+3, POSTDEC0
	movff	_iic_midi_rx_package+2, POSTDEC0
	movff	_iic_midi_rx_package+1, POSTDEC0
	movf	_iic_midi_rx_package+0, W
	call	_ROUTER_Rx_IIC3
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
	movf	PREINC0, W	; (not used anymore)
IIC_MIDI_Tick_Slave3_End
	;; loop so long IIC3 is the SysEx owner
	;; TODO: add timeout
	clrwdt			; feed watchdog
	SET_BSR	_sysex_owner
	movf	_sysex_owner, W, BANKED
	xorlw	PORT_IIC3
	bz	IIC_MIDI_Tick_Slave3
#endif

IIC_MIDI_Tick_End
	bcf	LATD, 4

	return

; ==========================================================================
	END
