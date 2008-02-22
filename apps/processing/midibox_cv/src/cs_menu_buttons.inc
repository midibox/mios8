; $Id$
;
; Control Surface Button Handler
;
; ==========================================================================
;
;  Copyright 1998-2007 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================


;; --------------------------------------------------------------------------
;;  This function has to be called by USER_DIN_NotifyToggle to handle with
;;  the menu navigation
;;  Input:
;;     o Button number in MIOS_PARAMETER1
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;;
;;  The handler will directly executed the CS function if it matches with
;;  the predefined button numbers, else it will return to CS_MENU_BUTTON_Handler_Return
;;
;;  NEVER call this function, use a "goto" instead - use it only once
;;  in your application - example:
;;	goto	CS_MENU_BUTTON_Handler
;;CS_MENU_BUTTON_Handler_Return
;; --------------------------------------------------------------------------
	;; check for control surface buttons - the CS handler will jump
	;; back if the button has not been assigned to a CS function
CS_MENU_BUTTON_Handler
	movf	MIOS_PARAMETER1, W
	xorlw	DEFAULT_DIN_MENU_EXEC
	bz	CS_MENU_BUTTON_Exec

	movf	MIOS_PARAMETER1, W
	xorlw	DEFAULT_DIN_MENU_RIGHT
	bz	CS_MENU_BUTTON_Right

	movf	MIOS_PARAMETER1, W
	xorlw	DEFAULT_DIN_MENU_LEFT
	bz	CS_MENU_BUTTON_Left

	movf	MIOS_PARAMETER1, W
	xorlw	DEFAULT_DIN_MENU_SELECT
	bz	CS_MENU_BUTTON_Select

	;; check for GP buttons if hook has been installed
	call	CS_MENU_CheckMenuGPCallbackHook
	bz	CS_MENU_BUTTON_Handler_NoGP
CS_MENU_BUTTON_Handler_ChkGP1
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	DEFAULT_GP_DIN_SR0 << 3
	bnz	CS_MENU_BUTTON_Handler_ChkGP2
	;; call the GP hook. 
	;; Parameter:
	;;    GP button number in MIOS_PARAMETER1 (changed): 0x00-0x07
	;;    button value in MIOS_PARAMETER2 (forwarded)
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	goto	CS_MENU_ExecMenuGPCallbackHook

CS_MENU_BUTTON_Handler_ChkGP2
	movf	MIOS_PARAMETER1, W
	andlw	0xf8
	xorlw	DEFAULT_GP_DIN_SR1 << 3
	bnz	CS_MENU_BUTTON_Handler_NoGP
	;; call the GP hook. 
	;; Parameter:
	;;    GP button number in MIOS_PARAMETER1 (changed): 0x08-0x0f
	;;    button value in MIOS_PARAMETER2 (forwarded)
	movlw	0x07
	andwf	MIOS_PARAMETER1, F
	bsf	MIOS_PARAMETER1, 3
	goto	CS_MENU_ExecMenuGPCallbackHook

CS_MENU_BUTTON_Handler_NoGP
	goto	CS_MENU_BUTTON_Handler_Return


;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Select
	;; branch to "Exec Button" handler
	goto	CS_MENU_Select

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Left
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; branch to "Left Button" handler
	goto	CS_MENU_Left

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Right
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; branch to "Right Button" handler
	goto	CS_MENU_Right

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Exec
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; branch to "Exec Button" handler
	goto	CS_MENU_Exec
