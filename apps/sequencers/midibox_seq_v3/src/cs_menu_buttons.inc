; $Id$
;
; Control Surface Button Handler
;
; ==========================================================================
;
;  Copyright 1998-2006 Thorsten Klose (tk@midibox.org)
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

;; --------------------------------------------------------------------------
CS_MENU_BUTTON_Select
	;; branch to "Exec Button" handler
	goto	CS_MENU_Select

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Left
	;; enable auto-repeat for this button
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
CS_MENU_BUTTON_LeftRemote
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; branch to "Left Button" handler
	goto	CS_MENU_Left

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Right
	;; enable auto-repeat for this button
	movf	MIOS_PARAMETER1, W
	call	MIOS_DIN_PinAutoRepeatEnable
CS_MENU_BUTTON_RightRemote
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return
	;; branch to "Right Button" handler
	goto	CS_MENU_Right

	;; ------------------------------------------------------------------
CS_MENU_BUTTON_Exit
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; clear screen(s)
	call	SEQ_LCD_Clear

	;; set cursor to first position
	clrf	CS_MENU_CURSOR_POS

	;; branch to "Exec Button" handler
	goto	CS_MENU_Exec
	