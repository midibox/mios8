; $Id$
;
; MIOS Delay Routine
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_Delay
;;  C_DECLARATION: void MIOS_Delay(unsigned char delay)
;;  DESCRIPTION: waits for an exact time<BR>
;;  note that this function will not work within an interrupt service routine!
;;  IN:	  time in ms (0-255)
;;  C_IN: time in ms (0-255)
;;  OUT:  -
;;  C_OUT:  -
;;  USES: BSR
;;  EXAMPLE:
;; 
;;	;; wait for exact 250 ms
;;	movlw	250
;;	call	MIOS_Delay
;;
;;  C_EXAMPLE:
;; 
;;	// wait for exact 250 ms
;;	MIOS_Delay(250);
;;
;; --------------------------------------------------------------------------
MIOS_Delay
	SET_BSR	MIOS_DELAY_CTR
	movwf	MIOS_DELAY_CTR, BANKED

	andlw	0xff		; exit if value is zero
	skpnz
	return

MIOS_Delay_Loop
	clrwdt
	movf	MIOS_DELAY_CTR, W
	bnz	MIOS_Delay_Loop
	return

;; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: MIOS_GPCounterGet
;;  C_DECLARATION: unsigned char MIOS_GPCounterGet(void)
;;  DESCRIPTION: get value of general purpose counter, which is incremented
;;  every millisecond
;;  IN:	  -
;;  C_IN:	  -
;;  OUT:  value of general purpose counter in WREG and MIOS_PARAMETER1
;;  C_OUT:  value of general purpose counter
;;  USES: BSR
;; --------------------------------------------------------------------------
MIOS_GPCounterGet
	SET_BSR	MIOS_GP_CTR
	movf	MIOS_GP_CTR, W, BANKED
	movwf	MIOS_PARAMETER1
	return
