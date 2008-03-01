; $Id$
;
; Using J5 as DIN
; 
; This include file provides following functions:
;    o J5_DIN_Init:    initializes the J5 port
;    o J5_DIN_Get:     user function to request the J5 status (all 8 pins)
;    o J5_DIN_PinGet:  user function to request the pin status (one pin only)
;    o J5_DIN_Update:  see function header
;    o J5_DIN_Handler: see function header
;
; NOTE: two registers have to be inserted in app_defines.h to get this
;       driver working:
;
;J5_DIN_STATUS		EQU	0x020	; used by J5_DIN_Update to store the current status
;J5_DIN_STATUS_CHANGED	EQU	0x021	; used by J5_DIN_Update to notify that an input has been toggled
;
;       they have to be located to free register addresses!
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DIN_Init
;;  DESCRIPTION: This function initializes J5 as digital input
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_DIN_Init
	;; initialize pins for input
	movlw	0xff
	call	J5_IO_Init

	;; init status register with 0xff (assuming low-active logic)
	setf	J5_DIN_STATUS

	;; no changes (yet)
	clrf	J5_DIN_STATUS_CHANGED

	return

;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DIN_Get
;;  DESCRIPTION: This function returns the status of all pins of J5
;;  IN:   -
;;  OUT:  pin status of all 8 pins in WREG and MIOS_PARAMETER1
;;  USES: -
;; --------------------------------------------------------------------------
J5_DIN_Get
	;; just return J5_DIN_STATUS
	movf	J5_DIN_STATUS, W		; in WREG
	movwf	MIOS_PARAMETER1			; and MIOS_PARAMETER1
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DIN_PinGet
;;  DESCRIPTION: This function returns the saved logic status of a pin at J5
;;  of the core
;;  IN:   pin number in WREG
;;  OUT:  pin status (0 or 1) in WREG and MIOS_PARAMETER1
;;  USES: -
;; --------------------------------------------------------------------------
J5_DIN_PinGet
	;; get mask depending on selected pin
	call	MIOS_HLP_GetBitORMask
	;; AND operation with the saved pin status
	andwf	J5_DIN_STATUS, W
	;; now: WREG == 0 if selected pin == 0
	;;      WREG != 0 if selected pin == 1
	
	;; we want to return a proper 0x01 if pin == 1:
	skpz			; write 0x01 to WREG when zero flag not set
	movlw	0x01

	movwf	MIOS_PARAMETER1	; write result to MIOS_PARAMETER1
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DIN_Update
;;  DESCRIPTION: This function should be called periodically either by 
;;  USER_Timer (if the desired scan frequency is unequal to the SRIO update 
;;  frequency) or (-> prefered method <-) by USER_SR_Service_Finish which 
;;  is dedicated for button/LED handlers
;; 
;;  This function is writtin in a way which allows the use either in an ISR
;;  or from the main program. However, the ISR (USER_SR_Service_Finish) 
;;  for periodical calls should be prefered.
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_DIN_Update
	;; build the new status and temporary store it in MIOS_PARAMETER1
	;; (btw.: by using MIOS_PARAMETER[123] we are allowed to call this
	;; function either from an ISR or from the mainprogram, since the
	;; registers are stored/restored by the interrupt handler in order
	;; to avoid conflicts)

	call	J5_IO_Get
	movwf	MIOS_PARAMETER1

	;; now compare the new values with the values from the last call
	movf	MIOS_PARAMETER1, W
	xorwf	J5_DIN_STATUS, W
	;; "or" changes into J5_DIN_STATUS_CHANGED so that no event gets lost
	iorwf	J5_DIN_STATUS_CHANGED, F

	;; copy MIOS_PARAMETER1 to J5_DIN_STATUS for the next call
	movff	MIOS_PARAMETER1, J5_DIN_STATUS

	;; the status will be checked by J5_DIN_Handler from the mainprogram
	return


;; --------------------------------------------------------------------------
;;  FUNCTION: J5_DIN_Handler
;;  DESCRIPTION: This function should be called from USER_Tick (in the
;;  mainprogram) to ask for button changes. Events (change from 0->1 or
;;  from 1->0) will be notified via the USER_J5_DIN_NotifyToggle hook
;;  which behaves similar to the USER_DIN_NotifyToggle hook provided by
;;  MIOS itself
;; 
;;  IN:   -
;;  OUT:  -
;;  USES: -
;; --------------------------------------------------------------------------
J5_DIN_Handler
	;; the fastest solution...
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 0, ACCESS, J5_DIN_Handler_Pin0
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 1, ACCESS, J5_DIN_Handler_Pin1
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 2, ACCESS, J5_DIN_Handler_Pin2
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 3, ACCESS, J5_DIN_Handler_Pin3
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 4, ACCESS, J5_DIN_Handler_Pin4
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 5, ACCESS, J5_DIN_Handler_Pin5
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 6, ACCESS, J5_DIN_Handler_Pin6
	RCALL_IFSET J5_DIN_STATUS_CHANGED, 7, ACCESS, J5_DIN_Handler_Pin7
	return

	;; ---

J5_DIN_Handler_Pin0
	bcf	J5_DIN_STATUS_CHANGED, 0		; clear "changed" flag
	movlw	0x00
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin1
	bcf	J5_DIN_STATUS_CHANGED, 1		; clear "changed" flag
	movlw	0x01
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin2
	bcf	J5_DIN_STATUS_CHANGED, 2		; clear "changed" flag
	movlw	0x02
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin3
	bcf	J5_DIN_STATUS_CHANGED, 3		; clear "changed" flag
	movlw	0x03
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin4
	bcf	J5_DIN_STATUS_CHANGED, 4		; clear "changed" flag
	movlw	0x04
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin5
	bcf	J5_DIN_STATUS_CHANGED, 5		; clear "changed" flag
	movlw	0x05
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin6
	bcf	J5_DIN_STATUS_CHANGED, 6		; clear "changed" flag
	movlw	0x06
	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin7
	bcf	J5_DIN_STATUS_CHANGED, 7		; clear "changed" flag
	movlw	0x07
	;; 	rgoto	J5_DIN_Handler_Pin_Cont
J5_DIN_Handler_Pin_Cont
	;; temp. store pin number in MIOS_PARAMETER3
	movwf	MIOS_PARAMETER3

	;; get pin status
	rcall	J5_DIN_PinGet
	;; save it in MIOS_PARAMETER2
	movwf	MIOS_PARAMETER2
	;; store pin number in MIOS_PARAMETER1
	movf	MIOS_PARAMETER3, W
	movwf	MIOS_PARAMETER1
	;; and call the hook
	goto	USER_J5_DIN_NotifyToggle


;; --------------------------------------------------------------------------
;;  FUNCTION: USER_J5_DIN_NotifyToggle
;;  DESCRIPTION: This function is NOT called by MIOS, but by the J5_DIN 
;;  handler when a pin at J5 has been toggled
;;  IN: Button number in WREG and MIOS_PARAMETER1
;;      Button value MIOS_PARAMETER2:
;;        - 1 if button has been released (=5V)
;;        - 0 if button has been pressed (=0V)
;;  OUT:  -
;;  ISR: no
;; --------------------------------------------------------------------------
	;; disabled here, should be defined somwhere else in the application!
USER_J5_DIN_NotifyToggle
#if DEFAULT_USE_J5
	;; branch to MIDImon button handler
	goto	MON_BUTTON_Handler
#else
	return
#endif
