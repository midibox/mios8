; $Id: sid_gates.inc 44 2008-01-30 21:39:30Z tk $
;
; Gate Pin update handler
; 
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  Called from USER_Tick to update the gates
;; --------------------------------------------------------------------------
SID_GATES_Update
	;; do nothing if gate bits have not been changed
	SET_BSR	GATES
	movf	GATES, W, BANKED
	xorwf	GATES_LAST, W, BANKED
	bz	SID_GATES_Update_End

	;; disable interrupts to ensure data consistency
	IRQ_DISABLE

	;; store new state
	movff	GATES, GATES_LAST

	;; forward digital pins to AOUT driver (only relevant for MAX525 based MBHP_AOUT module)
	movf	GATES, W, BANKED
	call	AOUT_DigitalPinsSet

	;; optionally forward to DOUT
#if DEFAULT_EXT_SWITCH_DOUT
	movff	GATES, MIOS_PARAMETER1
	movlw	(DEFAULT_EXT_SWITCH_DOUT-1) & 0x0f
	call	MIOS_DOUT_SRSet
#endif

	;; optionally forward to J5
#if DEFAULT_J5_FUNCTION == 3
	SET_BSR	GATES
	movf	GATES, W, BANKED
	call	J5_DOUT_Set
#endif

	;; enable interrupts again
	IRQ_ENABLE

SID_GATES_Update_End
	return
