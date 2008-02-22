; $Id$
;
; MIDImon
; Button handler
;
; ==========================================================================
;
;  Copyright 1998-2003 Thorsten Klose (tk@midibox.org)
;  Licensed for personal non-commercial use only.
;  All other rights reserved.
; 
; ==========================================================================

;; --------------------------------------------------------------------------
;;  this function is called by USER_Tick to update the switches
;; --------------------------------------------------------------------------
MON_BUTTON_UpdateSwitches
	;; save current MB_STAT
	movff	MB_STAT, TMP1

	;; set MB_STAT_PRINT_CC_NAMES depending on button/switch state
	rcall	MON_BUTTON_CC_NamesGet
	bcf	MB_STAT, MB_STAT_PRINT_CC_NAMES
	btfsc	WREG, 0
	bsf	MB_STAT, MB_STAT_PRINT_CC_NAMES

	;; set MB_STAT_FILTER_CLOCK depending on button/switch state
	rcall	MON_BUTTON_FilterGet
	bcf	MB_STAT, MB_STAT_FILTER_CLOCK
	btfsc	WREG, 0
	bsf	MB_STAT, MB_STAT_FILTER_CLOCK

	;; now check if MB_STAT has been changed - in this case update display
	movf	MB_STAT, W
	xorwf	TMP1, F
	skpz
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ

	return

;; --------------------------------------------------------------------------
;;  the button handler of MIDImon
;;  Input:
;;     o Button number in MIOS_PARAMETER1 (AND NOT IN WREG!)
;;     o Button value MIOS_PARAMETER2:
;;       - 1 if button has been released (=5V)
;;       - 0 if button has been pressed (=0V)
;; --------------------------------------------------------------------------
MON_BUTTON_Handler
	movlw	DEFAULT_BUTTON_UP
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto MON_BUTTON_Handler_NoUp
MON_BUTTON_Handler_Up
	rgoto	MON_BUTTON_Up
MON_BUTTON_Handler_NoUp

	movlw	DEFAULT_BUTTON_DOWN
	cpfseq	MIOS_PARAMETER1, ACCESS
	rgoto MON_BUTTON_Handler_NoDown
MON_BUTTON_Handler_Down
	rgoto	MON_BUTTON_Down
MON_BUTTON_Handler_NoDown

	;; DEFAULT_BUTTON_FILTER and DEFAULT_BUTTON_CC_NAMES are
	;; handled by MON_BUTTON_UpdateSwitches

	return

	
;; --------------------------------------------------------------------------
;;  The "up" button
;; --------------------------------------------------------------------------
MON_BUTTON_Up
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; else increment offset until last entry reached
	incf	MON_DISPLAY_OFFSET, F
	movlw	DEFAULT_TRACE_MEM_ENTRIES-DEFAULT_NUMBER_OF_LINES
	cpfslt	MON_DISPLAY_OFFSET, ACCESS
	movwf MON_DISPLAY_OFFSET

	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	return

;; --------------------------------------------------------------------------
;;  The "down" button
;; --------------------------------------------------------------------------
MON_BUTTON_Down
	;; do nothing if button has been depressed
	btfsc	MIOS_PARAMETER2, 0
	return

	;; decrement offset until zero
	movf	MON_DISPLAY_OFFSET, W
	skpz
	decf	MON_DISPLAY_OFFSET, F

	;; request display update
	bsf	MB_STAT, MB_STAT_DISPLAY_UPDATE_REQ
	return


;; --------------------------------------------------------------------------
;;  this function returns the status of the "filter" button/switch
;;  OUT: 1 if filter enabled
;;       0 if filter disabled
;; --------------------------------------------------------------------------
MON_BUTTON_FilterGet
	movlw	DEFAULT_BUTTON_FILTER
#if DEFAULT_USE_J5
	goto	J5_DIN_PinGet
#else
	goto	MIOS_DIN_PinGet
#endif

;; --------------------------------------------------------------------------
;;  this function returns the status of the "CC names" button/switch
;;  OUT: 1 if CC names should be print
;;       0 if CC number should be print
;; --------------------------------------------------------------------------
MON_BUTTON_CC_NamesGet
	movlw	DEFAULT_BUTTON_CC_NAMES
#if DEFAULT_USE_J5
	goto	J5_DIN_PinGet
#else
	goto	MIOS_DIN_PinGet
#endif

