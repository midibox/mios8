; $Id$
;
; MIDIbox FM
; Performance Measuring
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function should be called from the USER_Tick loop to increment
;;  the load counter
;; --------------------------------------------------------------------------
MBFM_LOAD_IncLoadCtr
	;; don't increment if already 0xffff
	incf	PERFORMANCE_LOAD_CTR_L, W
	bnz	MBFM_LOAD_IncLoadCtr_Cont
	incf	PERFORMANCE_LOAD_CTR_H, W
	bz	MBFM_LOAD_IncLoadCtr_End

MBFM_LOAD_IncLoadCtr_Cont
	;; increment 16-bit counter
	incf	PERFORMANCE_LOAD_CTR_L, F
	skpnz
	incf	PERFORMANCE_LOAD_CTR_H, F

MBFM_LOAD_IncLoadCtr_End
	return


;; --------------------------------------------------------------------------
;;  This function should be called from a timer (usually USER_SR_Service_Finish)
;;  to increment the reference counter. Once it overruns, the actual load 
;;  counter will be copied into the save registers and then zeroed
;; --------------------------------------------------------------------------
MBFM_LOAD_IncRefCtr
	;; increment reference counter until overrun
	incf	PERFORMANCE_REF_CTR, F
	bnz	MBFM_LOAD_IncRefCtr_End

	;; copy load counter into save registers
	movff	PERFORMANCE_LOAD_CTR_L, PERFORMANCE_LOAD_REG_L
	movff	PERFORMANCE_LOAD_CTR_H, PERFORMANCE_LOAD_REG_H

	;; and clear load counters
	clrf	PERFORMANCE_LOAD_CTR_L
	clrf	PERFORMANCE_LOAD_CTR_H

MBFM_LOAD_IncRefCtr_End
	return


;; --------------------------------------------------------------------------
;;  This function prints the current load in decimal form (1 space + 5 digits)
;; --------------------------------------------------------------------------
MBFM_LOAD_Print
	movlw	' '
	call	MIOS_LCD_PrintChar
	movff	PERFORMANCE_LOAD_REG_H, MIOS_PARAMETER1
	movf	PERFORMANCE_LOAD_REG_L, W
	goto	MIOS_LCD_PrintBCD5

