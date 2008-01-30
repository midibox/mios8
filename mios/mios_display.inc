; $Id$
;
; MIOS Display Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_DISPLAY_Init
;;  C_DECLARATION: void DISPLAY_Init(void)
;;  DESCRIPTION: This function is called by MIOS when the display content
;;  should be initialized. Thats the case during startup and after a
;;  temporary message has been printed on the screen
;;  IN:  -
;;  C_IN:  -
;;  OUT: -
;;  C_OUT: -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  FUNCTION: USER_DISPLAY_Tick
;;  C_DECLARATION: void DISPLAY_Tick(void)
;;  DESCRIPTION: This function is called by MIOS in the mainloop when no
;;  temporary message is shown on the screen.
;;  IN:  -
;;  C_IN:  -
;;  OUT: -
;;  C_OUT: -
;;  ISR: no
;; --------------------------------------------------------------------------

;; --------------------------------------------------------------------------
;;  Display Handler: Called from MainLoop
;; --------------------------------------------------------------------------
MIOS_DISPLAY_Handler
	;; leave routine if user tasks have been suspended
	btfsc	MIOS_BOX_STAT, MIOS_BOX_STAT_SUSPEND_USER
	return

	;; continue if display message counter == 0
	SET_BSR	MIOS_MESSAGE_CTR
	decf	MIOS_MESSAGE_CTR, W, BANKED
	bz	MIOS_DISPLAY_DoInit
	movf	MIOS_MESSAGE_CTR, W, BANKED
	skpz
	return			; leave routine when display counter > 1

	;; call common display routine
	goto	USER_DISPLAY_Tick

MIOS_DISPLAY_DoInit
	;; clear semaphore
	clrf	MIOS_MESSAGE_CTR, BANKED
	;; a message was displayed - reinitialize screen
	goto	USER_DISPLAY_Init
