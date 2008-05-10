; $Id$
;
; MIOS Application AIN Driver
;
; ==========================================================================
;
;  Copyright 2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  This function is called by MIOS when a pot has been moved
;;  Input:
;;     o Pot number in WREG and MIOS_PARAMETER1
;;     o LSB value in MIOS_PARAMETER2
;;     o MSB value in MIOS_PARAMETER3
;; --------------------------------------------------------------------------
USER_AIN_NotifyChange
	;; if pot number == 0, set the BPM
	movf	MIOS_PARAMETER1, W
	bz	MMD_AIN_BPMSet

	;; else route the value to the CC handler
MMD_AIN_RouteCC
	;; since MIOS_AIN_Pin7bitGet changes MIOS_PARAMETER[12], copy
	;; CC number to TMP1 first
	movff	MIOS_PARAMETER1, TMP1
	;; get 7-bit result of pot and store value in MIOS_PARAMETER3
	movf	MIOS_PARAMETER1, W
	call	MIOS_AIN_Pin7bitGet
	movwf	MIOS_PARAMETER3
	;; copy CC number to MIOS_PARAMETER2
	movff	TMP1, MIOS_PARAMETER2
	;; branch to CC handler
	goto	MMD_MPROC_CC_Cfg


MMD_AIN_BPMSet
	;; we have a 10-bit result in MIOS_PARAMETER[23], but need a 8-bit value
	;; shift value two times to the right (-> value / 4)
	clrc
	rrf	MIOS_PARAMETER3, F	; 1
	rrf	MIOS_PARAMETER2, F
	clrc
	rrf	MIOS_PARAMETER3, F	; 2
	rrf	MIOS_PARAMETER2, W	; -> WREG
	
	;; 8-bit result is now in WREG, store value into BPM register and set timer
	call	MMD_Timer_BPMSet		; with this function

	;; and request a message
	movlw	MMD_DISPLAY_REQUEST_CFG		; request a message
	movwf	MMD_DISPLAY_REQUEST		; (will be handled by mmd_display.inc)

	return
