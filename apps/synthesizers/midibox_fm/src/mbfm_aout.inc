; $Id$
;
; Functions to access AOUT assign and offset value
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_Update
;;  DESCRIPTION: Updates AOUT channels and gates
;; --------------------------------------------------------------------------
MBFM_AOUT_Update
	IRQ_DISABLE

	;; call AOUT driver update function
	call	AOUT_Update

	;; do nothing if gate bits have not been changed
	SET_BSR	GATES
	movf	GATES, W, BANKED
	xorwf	GATES_LAST, W, BANKED
	bz	MBFM_AOUT_Update_End

	;; store new state
	movff	GATES, GATES_LAST

	;; forward digital pins to AOUT driver (only relevant for MAX525 based MBHP_AOUT module)
	movf	GATES, W, BANKED
	call	AOUT_DigitalPinsSet

MBFM_AOUT_Update_End
	;; enable interrupts again
	IRQ_ENABLE

	return


;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_ParGet
;;  DESCRIPTION: returns an AOUT parameter
;;  IN: parameter number in WREG
;;  OUT:  parameter value in WREG
;; --------------------------------------------------------------------------
MBFM_AOUT_ParGet
	lfsr	FSR1, MBFM_AOUT_BASE
	movf	PLUSW1, W
	return

;; --------------------------------------------------------------------------
;;  FUNCTION: MBFM_AOUT_ParSet
;;  DESCRIPTION: writes to an AOUT parameter
;;  IN: parameter number in WREG
;;      parameter value in MIOS_PARAMETER1
;;  OUT:  -
;; --------------------------------------------------------------------------
MBFM_AOUT_ParSet
	lfsr	FSR1, MBFM_AOUT_BASE
	movff	MIOS_PARAMETER1, PLUSW1
	return

